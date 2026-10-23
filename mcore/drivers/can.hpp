/*
 * can.hpp — bxCAN driver (init, acceptance filters, TX/RX, interrupts)
 *
 *  Created on: 26 февр. 2026 г.
 *      Author: AkimovMA
 *
 * Layout follows the other drivers:
 *   - CAN_Init<CANx, cfg>()             one-shot init, config as NTTP
 *   - CAN_FilterConfig<CANx, filter>()  one acceptance-filter bank, NTTP
 *   - CanHandle<CANx>                   data path (send / recv / status / callbacks)
 *   - CAN_*IRQHandler<CANx>()           ISR bodies, called from the vector stubs
 *
 * bxCAN recap (RM0410 §40):
 *   - 3 TX mailboxes; hardware picks the next free one (TSR.CODE) and
 *     arbitrates between pending ones by ID (or FIFO order with TXFP).
 *   - 2 RX FIFOs, 3 frames deep each. A frame lands in a FIFO only if some
 *     *active* filter bank assigned to that FIFO accepts it — with no filter
 *     configured nothing is received at all.
 *   - CAN1 (master) and CAN2 (slave) share 28 filter banks living in CAN1's
 *     register block; CAN2 owns banks [CAN2SB..27]. CAN3 has its own 14 banks.
 */
#pragma once

#include <atomic>
#include <cstdint>
#include <cstring>
#include "core/regs.hpp"
#include "core/rcc.hpp"
#include "core/system.hpp"
#include "core/mcore_config.hpp"
#include "drivers/common.hpp"

constexpr uint8_t CAN_TIMEOUT_MS = 10;     // init / mode-change handshake timeout
constexpr uint8_t CAN_TX_TIMEOUT_MS = 100; // default wait for a free TX mailbox

constexpr uint32_t CAN_STD_ID_MAX = 0x7FFU;
constexpr uint32_t CAN_EXT_ID_MAX = 0x1FFFFFFFU;
constexpr uint8_t  CAN_MAX_DLC = 8;


// ===========================================================================
// Bit timing
// ===========================================================================
// Bit time = (1 + TimeSeg1 + TimeSeg2) time quanta, tq = Prescaler / PCLK1.
// Sample point sits at the end of TimeSeg1.
struct CanBitTiming {
    uint16_t Prescaler = 6;      // 1..1024
    uint8_t  TimeSeg1 = 15;      // 1..16 tq
    uint8_t  TimeSeg2 = 2;       // 1..8  tq
    uint8_t  SyncJumpWidth = 1;  // 1..4  tq, <= TimeSeg2
};                               // defaults: 500 kbit/s @ PCLK1 = 54 MHz, SP 88.9 %

// Deliberately not constexpr: reaching it during constant evaluation turns a
// bad argument into a compile error that names the problem.
void can_bitrate_not_achievable();

// Compile-time bit-timing solver: picks the largest tq count (8..25) that
// divides PCLK1 exactly and lands closest to the requested sample point.
//   constexpr auto t = can_bit_timing(54'000'000, 500'000);  // {6, 15, 2, 1}
consteval CanBitTiming can_bit_timing(uint32_t pclk_hz, uint32_t bitrate,
                                      uint32_t sample_point_permille = 875) {
    CanBitTiming best{};
    uint32_t best_err = UINT32_MAX;

    for (uint32_t tq = 25; tq >= 8; --tq) {
        const uint64_t div = uint64_t{bitrate} * tq;
        if (pclk_hz % div != 0) continue;
        const uint64_t brp = pclk_hz / div;
        if (brp < 1 || brp > 1024) continue;

        // tq index of the sample point (SYNC + TS1), rounded
        uint32_t sp = (tq * sample_point_permille + 500) / 1000;
        uint32_t ts1 = sp - 1;
        if (ts1 > 16) ts1 = 16;
        if (tq - 1 - ts1 > 8) ts1 = tq - 1 - 8;
        if (ts1 < 1) continue;
        const uint32_t ts2 = tq - 1 - ts1;
        if (ts2 < 1) continue;

        const uint32_t actual = (1 + ts1) * 1000 / tq;
        const uint32_t err = actual > sample_point_permille ? actual - sample_point_permille
                                                            : sample_point_permille - actual;
        if (err < best_err) {
            best_err = err;
            best = { static_cast<uint16_t>(brp), static_cast<uint8_t>(ts1),
                     static_cast<uint8_t>(ts2), 1 };
        }
    }
    if (best_err == UINT32_MAX) can_bitrate_not_achievable();
    return best;
}


// ===========================================================================
// Init configuration
// ===========================================================================
enum class CanMode : uint32_t {
    Normal         = 0,
    Loopback       = CAN1::_BTR::LBKM::BitMsk,
    Silent         = CAN1::_BTR::SILM::BitMsk,
    SilentLoopback = CAN1::_BTR::LBKM::BitMsk | CAN1::_BTR::SILM::BitMsk
};

// IER sources. Error sources also carry ERRIE: without it EWG/EPV/BOF/LEC
// never reach the SCE vector.
enum class CanIrq : uint32_t {
    None          = 0,
    TxComplete    = CAN1::_IER::TMEIE::BitMsk,                                 // -> TX
    Rx0Pending    = CAN1::_IER::FMPIE0::BitMsk,                                // -> RX0
    Rx0Overrun    = CAN1::_IER::FOVIE0::BitMsk,                                // -> RX0
    Rx1Pending    = CAN1::_IER::FMPIE1::BitMsk,                                // -> RX1
    Rx1Overrun    = CAN1::_IER::FOVIE1::BitMsk,                                // -> RX1
    ErrorWarning  = CAN1::_IER::EWGIE::BitMsk | CAN1::_IER::ERRIE::BitMsk,     // -> SCE
    ErrorPassive  = CAN1::_IER::EPVIE::BitMsk | CAN1::_IER::ERRIE::BitMsk,     // -> SCE
    BusOff        = CAN1::_IER::BOFIE::BitMsk | CAN1::_IER::ERRIE::BitMsk,     // -> SCE
    LastErrorCode = CAN1::_IER::LECIE::BitMsk | CAN1::_IER::ERRIE::BitMsk,     // -> SCE
};

constexpr CanIrq operator|(CanIrq a, CanIrq b) {
    return static_cast<CanIrq>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

struct CanHwConfig {
    CanBitTiming Timing{};
    CanMode Mode = CanMode::Normal;
    CanIrq Interrupts = CanIrq::None;  // NVIC lines are enabled by the application
    bool TimeTriggerMode = false;      // TTCM: timestamp frames, TGT support
    bool AutoBusOff = true;            // ABOM: leave bus-off by itself after 128x11 recessive bits
    bool AutoWakeUp = true;            // AWUM: wake from sleep on bus activity
    bool AutoRetransmission = true;    // !NART: retry until success (CAN default)
    bool FifoLock = false;             // RFLM: on overrun drop the new frame instead of the oldest
    bool FifoPriority = false;         // TXFP: send mailboxes in request order, not by ID
    bool DebugFreeze = true;           // DBF: halt the controller while the core is halted
};

// CAN2 is a slave of CAN1: its filters and part of its logic live behind
// CAN1's clock, so CAN1 must be clocked for CAN2 to work.
template<typename CANx>
struct can_master { using type = CANx; };
template<> struct can_master<CAN2> { using type = CAN1; };

template<typename CANx>
using can_master_t = typename can_master<CANx>::type;

template<typename CANx>
inline constexpr uint8_t can_filter_banks = std::is_same_v<CANx, CAN3> ? 14 : 28;


template<typename CANx, CanHwConfig cfg>
static inline DrvStatus CAN_Init() {
    constexpr CanBitTiming t = cfg.Timing;
    static_assert(t.Prescaler >= 1 && t.Prescaler <= 1024, "CAN: Prescaler must be 1..1024");
    static_assert(t.TimeSeg1 >= 1 && t.TimeSeg1 <= 16, "CAN: TimeSeg1 must be 1..16 tq");
    static_assert(t.TimeSeg2 >= 1 && t.TimeSeg2 <= 8, "CAN: TimeSeg2 must be 1..8 tq");
    static_assert(t.SyncJumpWidth >= 1 && t.SyncJumpWidth <= 4, "CAN: SJW must be 1..4 tq");
    static_assert(t.SyncJumpWidth <= t.TimeSeg2, "CAN: SJW must not exceed TimeSeg2");

    uint32_t tickstart;

    /* 0. Clock enable (CAN2 also needs its master) */
    if constexpr (!std::is_same_v<can_master_t<CANx>, CANx>) {
        RccEnable<can_master_t<CANx>>::enable();
    }
    RccEnable<CANx>::enable();

    /* 1. Enter init mode */
    CANx::_MCR::INRQ::set();

    tickstart = get_tick();
    while (!CANx::_MSR::INAK::is_set()) {
        if ((get_tick() - tickstart) > CAN_TIMEOUT_MS) return DrvStatus::Timeout;
    }

    /* 2. Exit sleep mode (the controller comes out of reset asleep) */
    CANx::_MCR::SLEEP::clear();

    tickstart = get_tick();
    while (CANx::_MSR::SLAK::is_set()) {
        if ((get_tick() - tickstart) > CAN_TIMEOUT_MS) return DrvStatus::Timeout;
    }

    /* 3. MCR options, folded into one mask at compile time */
    using MCR = typename CANx::_MCR;
    constexpr uint32_t mcr_mask = MCR::TTCM::BitMsk | MCR::ABOM::BitMsk | MCR::AWUM::BitMsk
                                | MCR::NART::BitMsk | MCR::RFLM::BitMsk | MCR::TXFP::BitMsk
                                | MCR::DBF::BitMsk;
    constexpr uint32_t mcr_bits = (cfg.TimeTriggerMode     ? MCR::TTCM::BitMsk : 0U)
                                | (cfg.AutoBusOff          ? MCR::ABOM::BitMsk : 0U)
                                | (cfg.AutoWakeUp          ? MCR::AWUM::BitMsk : 0U)
                                | (!cfg.AutoRetransmission ? MCR::NART::BitMsk : 0U)
                                | (cfg.FifoLock            ? MCR::RFLM::BitMsk : 0U)
                                | (cfg.FifoPriority        ? MCR::TXFP::BitMsk : 0U)
                                | (cfg.DebugFreeze         ? MCR::DBF::BitMsk  : 0U);
    MCR::overwrite((MCR::read() & ~mcr_mask) | mcr_bits);

    /* 4. Bit timing + test mode (writable only in init mode) */
    using BTR = typename CANx::_BTR;
    constexpr uint32_t btr = ((t.SyncJumpWidth - 1U) << BTR::SJW::pos)
                           | ((t.TimeSeg2 - 1U)      << BTR::TS2::pos)
                           | ((t.TimeSeg1 - 1U)      << BTR::TS1::pos)
                           | ((t.Prescaler - 1U)     << BTR::BRP::pos)
                           | static_cast<uint32_t>(cfg.Mode);
    BTR::template overwrite<btr>();

    /* 5. Interrupt sources */
    CANx::_IER::template overwrite<static_cast<uint32_t>(cfg.Interrupts)>();

    /* 6. Leave init mode: the node joins the bus after 11 recessive bits */
    CANx::_MCR::INRQ::clear();

    tickstart = get_tick();
    while (CANx::_MSR::INAK::is_set()) {
        if ((get_tick() - tickstart) > CAN_TIMEOUT_MS) return DrvStatus::Timeout;
    }

    return DrvStatus::Ok;
}


// ===========================================================================
// Acceptance filters
// ===========================================================================
enum class CanFifo : uint8_t { Fifo0 = 0, Fifo1 = 1 };

enum class CanFilterMode : uint8_t {
    Mask,   // accept when (id & mask) == (filter & mask)
    List    // accept on exact match with any listed id
};

enum class CanFilterScale : uint8_t {
    Dual16,   // two 16-bit filters per bank (standard IDs only here)
    Single32  // one 32-bit filter per bank (standard or extended IDs)
};

void can_filter_bad_argument();  // not constexpr, see can_bitrate_not_achievable

// ID word layouts shared by TIxR / RIxR / 32-bit filter registers:
//   [31:21] STID  [20:3] EXID  [2] IDE  [1] RTR
// so an extended id is simply the 29-bit value << 3.
namespace can_detail {
using IdReg = CAN1::_TI0R;

constexpr uint32_t id_word(uint32_t id, bool extended, bool remote) {
    return (extended ? (id & CAN_EXT_ID_MAX) << IdReg::EXID::pos | IdReg::IDE::BitMsk
                     : (id & CAN_STD_ID_MAX) << IdReg::STID::pos)
         | (remote ? IdReg::RTR::BitMsk : 0U);
}

// 16-bit filter layout: [15:5] STID  [4] RTR  [3] IDE  [2:0] EXID[17:15]
constexpr uint32_t FILT16_STID_POS = 5;
constexpr uint32_t FILT16_IDE = 1U << 3;

constexpr uint32_t std16(uint32_t id) { return (id & CAN_STD_ID_MAX) << FILT16_STID_POS; }
}  // namespace can_detail

// One filter bank, built with the consteval factories below and applied with
// CAN_FilterConfig. Mask filters also match the IDE bit, so a standard-ID
// filter never accepts an extended frame with the same low bits and vice versa.
// Remote-request (RTR) frames pass whenever the ID matches.
struct CanFilter {
    uint8_t bank = 0;
    CanFilterMode mode = CanFilterMode::Mask;
    CanFilterScale scale = CanFilterScale::Single32;
    CanFifo fifo = CanFifo::Fifo0;
    uint32_t r1 = 0;  // FxR1: id (mask mode) / first id (list mode)
    uint32_t r2 = 0;  // FxR2: mask (mask mode) / second id (list mode)

    // Everything goes to `fifo`.
    static consteval CanFilter accept_all(uint8_t bank, CanFifo fifo = CanFifo::Fifo0) {
        return { bank, CanFilterMode::Mask, CanFilterScale::Single32, fifo, 0U, 0U };
    }

    // Standard IDs: accept when (rx_id & mask) == (id & mask).
    static consteval CanFilter std_mask(uint8_t bank, uint32_t id, uint32_t mask = CAN_STD_ID_MAX,
                                        CanFifo fifo = CanFifo::Fifo0) {
        if (id > CAN_STD_ID_MAX || mask > CAN_STD_ID_MAX) can_filter_bad_argument();
        using namespace can_detail;
        return { bank, CanFilterMode::Mask, CanFilterScale::Single32, fifo,
                 id_word(id, false, false),
                 id_word(mask, false, false) | IdReg::IDE::BitMsk };
    }

    // Extended IDs: accept when (rx_id & mask) == (id & mask).
    static consteval CanFilter ext_mask(uint8_t bank, uint32_t id, uint32_t mask = CAN_EXT_ID_MAX,
                                        CanFifo fifo = CanFifo::Fifo0) {
        if (id > CAN_EXT_ID_MAX || mask > CAN_EXT_ID_MAX) can_filter_bad_argument();
        using namespace can_detail;
        return { bank, CanFilterMode::Mask, CanFilterScale::Single32, fifo,
                 id_word(id, true, false),
                 id_word(mask, true, false) | IdReg::IDE::BitMsk };
    }

    // Up to four exact standard IDs in one bank (repeat an id to fill unused slots).
    static consteval CanFilter std_list(uint8_t bank, uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                                        CanFifo fifo = CanFifo::Fifo0) {
        if (a > CAN_STD_ID_MAX || b > CAN_STD_ID_MAX || c > CAN_STD_ID_MAX || d > CAN_STD_ID_MAX)
            can_filter_bad_argument();
        using namespace can_detail;
        return { bank, CanFilterMode::List, CanFilterScale::Dual16, fifo,
                 std16(a) | std16(b) << 16, std16(c) | std16(d) << 16 };
    }

    // Two exact extended IDs in one bank.
    static consteval CanFilter ext_list(uint8_t bank, uint32_t a, uint32_t b,
                                        CanFifo fifo = CanFifo::Fifo0) {
        if (a > CAN_EXT_ID_MAX || b > CAN_EXT_ID_MAX) can_filter_bad_argument();
        using namespace can_detail;
        return { bank, CanFilterMode::List, CanFilterScale::Single32, fifo,
                 id_word(a, true, false), id_word(b, true, false) };
    }
};

// Filter bank n registers: FxR1/FxR2 at +0x240 + 8*n, compile-time address.
template<typename CANx, uint8_t Bank>
struct CanFilterBank {
    static constexpr uintptr_t base = CANx::_F0R1::address + 8U * Bank;
    struct R1 : Register<base,      ReadWrite, R1> {};
    struct R2 : Register<base + 4U, ReadWrite, R2> {};
};

// Program one filter bank. For CAN2 the bank must be >= CAN2SB
// (see CAN_FilterSplit); the registers are CAN1's either way.
template<typename CANx, CanFilter f>
static inline void CAN_FilterConfig() {
    static_assert(f.bank < can_filter_banks<CANx>, "CAN: filter bank out of range");

    using F = can_master_t<CANx>;
    using bank = CanFilterBank<F, f.bank>;
    constexpr uint32_t bit = 1U << f.bank;

    F::_FMR::FINIT::set();                // filter init mode (reception paused)
    F::_FA1R::template clearMask<bit>();  // bank must be inactive while edited

    if constexpr (f.scale == CanFilterScale::Single32) F::_FS1R::template setMask<bit>();
    else                                               F::_FS1R::template clearMask<bit>();

    if constexpr (f.mode == CanFilterMode::List) F::_FM1R::template setMask<bit>();
    else                                         F::_FM1R::template clearMask<bit>();

    if constexpr (f.fifo == CanFifo::Fifo1) F::_FFA1R::template setMask<bit>();
    else                                    F::_FFA1R::template clearMask<bit>();

    bank::R1::template overwrite<f.r1>();
    bank::R2::template overwrite<f.r2>();

    F::_FA1R::template setMask<bit>();
    F::_FMR::FINIT::clear();
}

// Bank split between CAN1 [0..start-1] and CAN2 [start..27]. Reset value: 14.
template<uint8_t can2_start>
static inline void CAN_FilterSplit() {
    static_assert(can2_start >= 1 && can2_start <= 27, "CAN: CAN2SB must be 1..27");
    CAN1::_FMR::FINIT::set();
    CAN1::_FMR::CAN2SB::template write<can2_start>();
    CAN1::_FMR::FINIT::clear();
}


// ===========================================================================
// Data path
// ===========================================================================
struct CanFrame {
    uint32_t id = 0;          // 11-bit standard or 29-bit extended
    uint8_t  dlc = 0;         // 0..8
    bool     extended = false;
    bool     remote = false;  // remote transmission request, no payload
    uint8_t  filter = 0;      // RX only: index of the matching filter (FMI)
    uint16_t timestamp = 0;   // RX only: bit-time counter at SOF (TTCM)
    uint8_t  data[8] = {};
};

enum class CanLastError : uint8_t {
    None = 0, Stuff, Form, Ack, BitRecessive, BitDominant, Crc, SetBySoftware
};

struct CanErrorState {
    uint8_t tec;          // transmit error counter
    uint8_t rec;          // receive error counter
    CanLastError last;
    bool warning;         // TEC or REC >= 96
    bool passive;         // TEC or REC > 127
    bool bus_off;         // TEC > 255: node is off the bus
};

using CanRxCallback  = void (*)(const CanFrame&);
using CanTxCallback  = void (*)(uint8_t mailbox, bool ok);
using CanErrCallback = void (*)(const CanErrorState&);

// TX mailbox N and RX FIFO F: the same register layout repeated at a fixed
// stride, so they are addressed by template index instead of 3x/2x copies.
template<typename CANx, uint8_t N>
struct CanTxMailbox {
    static_assert(N < 3, "CAN: only 3 TX mailboxes");
    static constexpr uintptr_t base = CANx::_TI0R::address + 0x10U * N;
    struct TIR  : Register<base,        ReadWrite, TIR>  {};
    struct TDTR : Register<base + 0x4U, ReadWrite, TDTR> {};
    struct TDLR : Register<base + 0x8U, ReadWrite, TDLR> {};
    struct TDHR : Register<base + 0xCU, ReadWrite, TDHR> {};
};

template<typename CANx, CanFifo F>
struct CanRxFifo {
    static constexpr uint32_t idx = static_cast<uint32_t>(F);
    static constexpr uintptr_t base = CANx::_RI0R::address + 0x10U * idx;
    struct RIR  : Register<base,        ReadOnly, RIR>  {};
    struct RDTR : Register<base + 0x4U, ReadOnly, RDTR> {};
    struct RDLR : Register<base + 0x8U, ReadOnly, RDLR> {};
    struct RDHR : Register<base + 0xCU, ReadOnly, RDHR> {};
    // RFxR: FMP / FULL / FOVR / RFOM sit at the same bits in both FIFOs
    struct RFR  : Register<CANx::_RF0R::address + 4U * idx, ReadWrite, RFR> {};
};


template<typename CANx>
struct CanHandle {
    static inline CanRxCallback  rx_callback[2] = { nullptr, nullptr };
    static inline CanTxCallback  tx_callback = nullptr;
    static inline CanErrCallback err_callback = nullptr;
    static inline std::atomic<uint32_t> rx_overruns{ 0 };  // frames lost to a full FIFO

    // --- Callbacks (run in interrupt context — keep them short) ----------
    // on_rx: called from CAN_RxIRQHandler with each frame already read out.
    template<CanFifo F = CanFifo::Fifo0>
    static void on_rx(CanRxCallback cb) noexcept { rx_callback[static_cast<uint8_t>(F)] = cb; }
    static void on_tx(CanTxCallback cb) noexcept { tx_callback = cb; }
    static void on_error(CanErrCallback cb) noexcept { err_callback = cb; }

    // --- TX ----------------------------------------------------------------
    /** Queue a frame into any free mailbox; Busy if all three are pending. */
    static DrvStatus try_send(const CanFrame& f, uint8_t* mailbox = nullptr) {
        if (f.dlc > CAN_MAX_DLC) return DrvStatus::InvalidArg;
        if (f.id > (f.extended ? CAN_EXT_ID_MAX : CAN_STD_ID_MAX)) return DrvStatus::InvalidArg;

        const uint32_t tsr = CANx::_TSR::read();
        if ((tsr & TME_ALL) == 0) return DrvStatus::Busy;

        // With at least one mailbox empty, CODE is the number of the next one
        const uint8_t n = static_cast<uint8_t>((tsr & TSR::CODE::BitMsk) >> TSR::CODE::pos);
        switch (n) {
        case 0: load_mailbox<0>(f); break;
        case 1: load_mailbox<1>(f); break;
        case 2: load_mailbox<2>(f); break;
        default: return DrvStatus::Error;
        }
        if (mailbox) *mailbox = n;
        return DrvStatus::Ok;
    }

    /** Queue a frame, waiting up to timeout_ms for a free mailbox. */
    static DrvStatus send(const CanFrame& f, uint32_t timeout_ms = CAN_TX_TIMEOUT_MS) {
        const uint32_t tickstart = get_tick();
        while (true) {
            const DrvStatus st = try_send(f);
            if (st != DrvStatus::Busy) return st;
            if (bus_off()) return DrvStatus::Error;
            if ((get_tick() - tickstart) > timeout_ms) return DrvStatus::Timeout;
        }
    }

    /** True while any mailbox still holds an unsent frame. */
    static bool tx_pending() noexcept { return (CANx::_TSR::read() & TME_ALL) != TME_ALL; }

    /** Drop every frame still waiting in a mailbox. */
    static void abort_all() noexcept {
        // ABRQx are write-1-to-set; the w1c status bits ignore the zeros
        CANx::_TSR::template overwrite<TSR::ABRQ0::BitMsk | TSR::ABRQ1::BitMsk
                                     | TSR::ABRQ2::BitMsk>();
    }

    // --- RX ----------------------------------------------------------------
    /** Frames waiting in FIFO F (0..3). */
    template<CanFifo F = CanFifo::Fifo0>
    static uint8_t pending() noexcept {
        return static_cast<uint8_t>(CanRxFifo<CANx, F>::RFR::read() & CANx::_RF0R::FMP0::BitMsk);
    }

    /** Pop one frame from FIFO F if there is one. */
    template<CanFifo F = CanFifo::Fifo0>
    static bool try_recv(CanFrame& f) noexcept {
        if (pending<F>() == 0) return false;
        read_fifo<F>(f);
        return true;
    }

    /** Pop one frame from FIFO F, waiting up to timeout_ms. */
    template<CanFifo F = CanFifo::Fifo0>
    static DrvStatus recv(CanFrame& f, uint32_t timeout_ms = CAN_TX_TIMEOUT_MS) {
        const uint32_t tickstart = get_tick();
        while (!try_recv<F>(f)) {
            if ((get_tick() - tickstart) > timeout_ms) return DrvStatus::Timeout;
        }
        return DrvStatus::Ok;
    }

    // --- Status ------------------------------------------------------------
    static CanErrorState errors() noexcept {
        using ESR = typename CANx::_ESR;
        const uint32_t esr = ESR::read();
        return {
            .tec     = static_cast<uint8_t>((esr & ESR::TEC::BitMsk) >> ESR::TEC::pos),
            .rec     = static_cast<uint8_t>((esr & ESR::REC::BitMsk) >> ESR::REC::pos),
            .last    = static_cast<CanLastError>((esr & ESR::LEC::BitMsk) >> ESR::LEC::pos),
            .warning = (esr & ESR::EWGF::BitMsk) != 0,
            .passive = (esr & ESR::EPVF::BitMsk) != 0,
            .bus_off = (esr & ESR::BOFF::BitMsk) != 0,
        };
    }

    static bool bus_off() noexcept { return CANx::_ESR::BOFF::is_set(); }

private:
    using TSR = typename CANx::_TSR;
    static constexpr uint32_t TME_ALL = TSR::TME0::BitMsk | TSR::TME1::BitMsk | TSR::TME2::BitMsk;

    template<uint8_t N>
    static void load_mailbox(const CanFrame& f) noexcept {
        using MB = CanTxMailbox<CANx, N>;
        uint32_t lo, hi;
        std::memcpy(&lo, &f.data[0], 4);  // DATA0 is the low byte: little-endian copy
        std::memcpy(&hi, &f.data[4], 4);

        MB::TDTR::overwrite(static_cast<uint32_t>(f.dlc));
        MB::TDLR::overwrite(lo);
        MB::TDHR::overwrite(hi);
        // ID last, with TXRQ in the same store: the mailbox is armed only
        // once length and payload are in place
        MB::TIR::overwrite(can_detail::id_word(f.id, f.extended, f.remote)
                           | CANx::_TI0R::TXRQ::BitMsk);
    }

    template<CanFifo F>
    static void read_fifo(CanFrame& f) noexcept {
        using X = CanRxFifo<CANx, F>;
        using RI = typename CANx::_RI0R;
        using RDT = typename CANx::_RDT0R;

        const uint32_t rir = X::RIR::read();
        f.extended = (rir & RI::IDE::BitMsk) != 0;
        f.remote   = (rir & RI::RTR::BitMsk) != 0;
        f.id = f.extended ? (rir >> RI::EXID::pos) & CAN_EXT_ID_MAX
                          : (rir >> RI::STID::pos) & CAN_STD_ID_MAX;

        const uint32_t rdtr = X::RDTR::read();
        const uint8_t dlc = static_cast<uint8_t>(rdtr & RDT::DLC::BitMsk);
        f.dlc = dlc > CAN_MAX_DLC ? CAN_MAX_DLC : dlc;  // 9..15 on the wire still mean 8 bytes
        f.filter = static_cast<uint8_t>((rdtr & RDT::FMI::BitMsk) >> RDT::FMI::pos);
        f.timestamp = static_cast<uint16_t>((rdtr & RDT::TIME::BitMsk) >> RDT::TIME::pos);

        const uint32_t lo = X::RDLR::read();
        const uint32_t hi = X::RDHR::read();
        std::memcpy(&f.data[0], &lo, 4);
        std::memcpy(&f.data[4], &hi, 4);

        // Release the output mailbox. Plain store, not RMW: FULL/FOVR are
        // write-1-to-clear, a read-modify-write would wipe them unseen.
        X::RFR::template overwrite<CANx::_RF0R::RFOM0::BitMsk>();
    }
};


// ===========================================================================
// Interrupt handlers — call from the vector stubs, e.g.
//   extern "C" void CAN1_RX0_IRQHandler() { CAN_RxIRQHandler<CAN1, CanFifo::Fifo0>(); }
// ===========================================================================

// TX vector: a mailbox finished (sent, failed, or aborted).
template<typename CANx>
void CAN_TxIRQHandler() {
    using TSR = typename CANx::_TSR;
    using H = CanHandle<CANx>;
    constexpr uint32_t rqcp_all = TSR::RQCP0::BitMsk | TSR::RQCP1::BitMsk | TSR::RQCP2::BitMsk;

    const uint32_t tsr = TSR::read();
    // RQCPx is w1c and clearing it also clears TXOKx/ALSTx/TERRx
    TSR::overwrite(tsr & rqcp_all);

    if (!H::tx_callback) return;
    if (tsr & TSR::RQCP0::BitMsk) H::tx_callback(0, (tsr & TSR::TXOK0::BitMsk) != 0);
    if (tsr & TSR::RQCP1::BitMsk) H::tx_callback(1, (tsr & TSR::TXOK1::BitMsk) != 0);
    if (tsr & TSR::RQCP2::BitMsk) H::tx_callback(2, (tsr & TSR::TXOK2::BitMsk) != 0);
}

// RX0 / RX1 vector: drain the FIFO into the on_rx callback (frames are
// dropped if none is registered — otherwise FMPIE would refire forever).
template<typename CANx, CanFifo F>
void CAN_RxIRQHandler() {
    using X = CanRxFifo<CANx, F>;
    using RF = typename CANx::_RF0R;
    using H = CanHandle<CANx>;

    const uint32_t rfr = X::RFR::read();
    if (rfr & (RF::FOVR0::BitMsk | RF::FULL0::BitMsk)) {
        if (rfr & RF::FOVR0::BitMsk) H::rx_overruns.fetch_add(1, std::memory_order_relaxed);
        X::RFR::overwrite(rfr & (RF::FOVR0::BitMsk | RF::FULL0::BitMsk));
    }

    const CanRxCallback cb = H::rx_callback[static_cast<uint8_t>(F)];
    CanFrame f;
    while (H::template try_recv<F>(f)) {
        if (cb) cb(f);
    }
}

// SCE vector: error state change / protocol error.
template<typename CANx>
void CAN_SceIRQHandler() {
    using H = CanHandle<CANx>;
    const CanErrorState st = H::errors();

    CANx::_MSR::template overwrite<CANx::_MSR::ERRI::BitMsk>();     // w1c
    CANx::_ESR::template clearMask<CANx::_ESR::LEC::BitMsk>();      // LEC is cleared by software

    if (H::err_callback) H::err_callback(st);
}
