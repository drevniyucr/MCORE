/*
 * fault.cpp — fault handlers, see core/fault.hpp
 */
#include "core/fault.hpp"
#include "core/regs.hpp"
#include "core/system.hpp"

#include <cstddef>
#include <cstring>

namespace {
constexpr uint32_t FAULT_MAGIC = 0xFA017EC0;
constexpr uint32_t CHECKSUM_SEED = 0x5EED5EED;
constexpr uint32_t AIRCR_VECTKEY = 0x5FA;
constexpr uint32_t EXC_RETURN_PSP = 1U << 2;   // frame was stacked on PSP
constexpr size_t   FAULT_STACK_WORDS = 512;    // 2 KB: the handler + print() at the next boot never run here
constexpr size_t   FRAME_WORDS = 8;            // r0-r3, r12, lr, pc, xPSR

// Valid stacked frame: word-aligned and entirely inside SRAM.
constexpr uint32_t RAM_START = 0x20000000;
constexpr uint32_t RAM_END   = 0x20080000;

// Survives software / watchdog resets; garbage after power-on, hence magic + checksum.
[[gnu::section(".noinit")]] volatile FaultRecord fault_record;

uint32_t checksum(const FaultRecord& r) {
    uint32_t words[sizeof(FaultRecord) / 4];
    std::memcpy(words, &r, sizeof(words));
    uint32_t sum = CHECKSUM_SEED;
    for (size_t i = 0; i + 1 < sizeof(words) / 4; ++i) {  // everything but the checksum itself
        sum = (sum << 5 | sum >> 27) ^ words[i];
    }
    return sum;
}

struct CfsrBit { uint32_t mask; const char* text; };

// Most specific first: a stacking error explains the rest, an imprecise
// bus error says nothing about pc.
constexpr CfsrBit CFSR_REASONS[] = {
    { SCB::_CFSR::STKERR::BitMsk,      "bus fault while stacking (stack overflow?)" },
    { SCB::_CFSR::MSTKERR::BitMsk,     "MPU fault while stacking (stack overflow?)" },
    { SCB::_CFSR::UNSTKERR::BitMsk,    "bus fault while unstacking" },
    { SCB::_CFSR::MUNSTKERR::BitMsk,   "MPU fault while unstacking" },
    { SCB::_CFSR::LSPERR::BitMsk,      "bus fault during FPU lazy stacking" },
    { SCB::_CFSR::MLSPERR::BitMsk,     "MPU fault during FPU lazy stacking" },
    { SCB::_CFSR::DIVBYZERO::BitMsk,   "integer divide by zero" },
    { SCB::_CFSR::UNALIGNED::BitMsk,   "unaligned access" },
    { SCB::_CFSR::UNDEFINSTR::BitMsk,  "undefined instruction" },
    { SCB::_CFSR::INVSTATE::BitMsk,    "invalid state (bad function pointer / Thumb bit)" },
    { SCB::_CFSR::INVPC::BitMsk,       "invalid EXC_RETURN" },
    { SCB::_CFSR::NOCP::BitMsk,        "coprocessor access (FPU disabled?)" },
    { SCB::_CFSR::PRECISERR::BitMsk,   "precise data bus error (see BFAR)" },
    { SCB::_CFSR::IMPRECISERR::BitMsk, "imprecise data bus error (buffered write, pc is late)" },
    { SCB::_CFSR::IBUSERR::BitMsk,     "instruction fetch bus error" },
    { SCB::_CFSR::DACCVIOL::BitMsk,    "MPU data access violation (see MMFAR)" },
    { SCB::_CFSR::IACCVIOL::BitMsk,    "MPU / XN instruction fetch violation" },
};

[[noreturn]] void system_reset() {
    __DSB();
    SCB::_AIRCR::overwrite((AIRCR_VECTKEY << SCB::_AIRCR::VECTKEY::pos)
                           | (SCB::_AIRCR::read() & SCB::_AIRCR::PRIGROUP::BitMsk)
                           | SCB::_AIRCR::SYSRESETREQ::BitMsk);
    __DSB();
    while (true) {}
}
}  // namespace

// The handler runs here, not on the stack that faulted (a stack overflow
// shows up as a fault while stacking: STKERR / MSTKERR). C linkage: the
// naked entry below addresses it by name.
extern "C" {
[[gnu::aligned(8), gnu::used]] uint32_t fault_stack[FAULT_STACK_WORDS];
}
static_assert(sizeof(fault_stack) == 2048, "keep in sync with the asm in HardFault_Handler");


extern "C" [[noreturn, gnu::used]] void fault_capture(const uint32_t* frame, uint32_t exc_return) {
    FaultRecord r{};
    r.magic = FAULT_MAGIC;

    const auto addr = reinterpret_cast<uint32_t>(frame);
    if ((addr & 3U) == 0 && addr >= RAM_START && addr + FRAME_WORDS * 4 <= RAM_END) {
        r.r0 = frame[0]; r.r1 = frame[1]; r.r2 = frame[2]; r.r3 = frame[3];
        r.r12 = frame[4]; r.lr = frame[5]; r.pc = frame[6]; r.xpsr = frame[7];
        r.sp = addr;
    }

    uint32_t ipsr;
    asm volatile("mrs %0, ipsr" : "=r"(ipsr));
    r.exc_return = exc_return;
    r.vector = ipsr & 0x1FFU;
    r.cfsr = SCB::_CFSR::read();
    r.hfsr = SCB::_HFSR::read();
    r.mmfar = SCB::_MMFAR::read();
    r.bfar = SCB::_BFAR::read();
    r.uptime_ms = get_tick();
    r.checksum = checksum(r);

    std::memcpy(const_cast<FaultRecord*>(&fault_record), &r, sizeof(r));

    if (SCB::_DHCSR::C_DEBUGEN::is_set()) {
        asm volatile("bkpt #0");  // inspect fault_record / registers, then "set $pc += 2"
    }
    system_reset();
}

// Shared entry for all four fault vectors. EXC_RETURN bit 2 tells which
// stack holds the frame; then MSP moves to the private fault stack.
// Basic asm only: a naked function has no prologue to back operands.
extern "C" [[gnu::naked]] void HardFault_Handler() noexcept {
    asm volatile(
        "tst   lr, #4                                  \n"  // EXC_RETURN_PSP
        "ite   eq                                      \n"
        "mrseq r0, msp                                 \n"
        "mrsne r0, psp                                 \n"
        "mov   r1, lr                                  \n"
        "movw  r2, #:lower16:(fault_stack + 2048)      \n"
        "movt  r2, #:upper16:(fault_stack + 2048)      \n"
        "msr   msp, r2                                 \n"
        "b     fault_capture                           \n");
}
static_assert(EXC_RETURN_PSP == 4, "keep in sync with the asm in HardFault_Handler");

extern "C" void MemManage_Handler() noexcept  __attribute__((naked, alias("HardFault_Handler")));
extern "C" void BusFault_Handler() noexcept   __attribute__((naked, alias("HardFault_Handler")));
extern "C" void UsageFault_Handler() noexcept __attribute__((naked, alias("HardFault_Handler")));


void Fault_Init(bool trap_div_by_zero) {
    SCB::_SHCSR::bitSet<SCB::_SHCSR::USGFAULTENA, SCB::_SHCSR::BUSFAULTENA,
                        SCB::_SHCSR::MEMFAULTENA>();
    if (trap_div_by_zero) SCB::_CCR::DIV_0_TRP::set();
    __DSB();
    __ISB();
}

bool Fault_TakeLast(FaultRecord& out) {
    std::memcpy(&out, const_cast<const FaultRecord*>(&fault_record), sizeof(out));
    fault_record.magic = 0;
    return out.magic == FAULT_MAGIC && out.checksum == checksum(out);
}

const char* Fault_Reason(const FaultRecord& f) {
    for (const auto& b : CFSR_REASONS) {
        if (f.cfsr & b.mask) return b.text;
    }
    if (f.hfsr & SCB::_HFSR::VECTTBL::BitMsk) return "vector table read error";
    if (f.hfsr & SCB::_HFSR::DEBUGEVT::BitMsk) return "debug event (BKPT without debugger?)";
    return "unknown";
}

void Fault_Print(const FaultRecord& f) {
    print("FAULT vec=%lu at %lu ms: %s\n", f.vector, f.uptime_ms, Fault_Reason(f));
    print("  pc=%08lx lr=%08lx sp=%08lx xpsr=%08lx\n", f.pc, f.lr, f.sp, f.xpsr);
    print("  cfsr=%08lx hfsr=%08lx mmfar=%08lx bfar=%08lx\n", f.cfsr, f.hfsr, f.mmfar, f.bfar);
}
