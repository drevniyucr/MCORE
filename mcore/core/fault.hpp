/*
 * fault.hpp — fault capture across resets
 *
 * HardFault / MemManage / BusFault / UsageFault all land in one handler
 * (core/fault.cpp) that:
 *   1. switches to a private stack (the faulting one may be what broke),
 *   2. snapshots the stacked registers and the SCB fault status into a
 *      .noinit RAM record that survives the reset,
 *   3. with a debugger attached: halts on BKPT so the state can be inspected
 *      ("set $pc += 2" + continue in GDB resumes into the reset);
 *      without one: resets the MCU immediately (SYSRESETREQ).
 * On the next boot Fault_TakeLast() hands the record over once.
 *
 *   Fault_Init();                       // separate fault vectors + div-by-0 trap
 *   FaultRecord f;
 *   if (Fault_TakeLast(f)) Fault_Print(f);
 */
#pragma once

#include <cstdint>

struct FaultRecord {
    uint32_t magic;
    // Exception frame stacked by the core (the first 8 words of a basic or FP frame)
    uint32_t r0, r1, r2, r3, r12;
    uint32_t lr;          // return address of the function that faulted
    uint32_t pc;          // faulting instruction (precise faults)
    uint32_t xpsr;
    uint32_t sp;          // address of the stacked frame (registers above are 0 if it was unreadable)
    uint32_t exc_return;  // LR on handler entry: which stack, FP frame or not
    uint32_t vector;      // IPSR: 3 HardFault, 4 MemManage, 5 BusFault, 6 UsageFault
    uint32_t cfsr;        // UFSR:BFSR:MMFSR
    uint32_t hfsr;
    uint32_t mmfar;       // valid if CFSR.MMARVALID
    uint32_t bfar;        // valid if CFSR.BFARVALID
    uint32_t uptime_ms;   // get_tick() at the fault
    uint32_t checksum;
};

/** Route MemManage/BusFault/UsageFault to their own vectors (instead of
 *  escalating everything to HardFault) and trap integer division by zero. */
void Fault_Init(bool trap_div_by_zero = true);

/** The fault that caused the last reset, if any. Consumes it: a second call
 *  (or the next clean reset) returns false. */
bool Fault_TakeLast(FaultRecord& out);

/** Human-readable main cause: the first set CFSR/HFSR bit, most specific first. */
const char* Fault_Reason(const FaultRecord& f);

/** One-line report over ITM/SWO via print(). */
void Fault_Print(const FaultRecord& f);
