/*
 * iwdg.hpp — independent watchdog + reset-cause query
 *
 *   IWDG_Start<cfg>()   start the watchdog (cannot be stopped until reset)
 *   IWDG_Refresh()      kick it
 *   RCC_ResetCause()    why the MCU came out of reset (call once at boot)
 *
 * The IWDG runs from LSI, which on F7 is only specified to 17..47 kHz
 * (nominal 32 kHz, LSI_VALUE). Timeouts are computed from the nominal value,
 * so the real one may be ~0.7x..1.5x: leave generous margin on both sides.
 */
#pragma once

#include <cstdint>
#include "core/regs.hpp"
#include "core/system.hpp"
#include "core/mcore_config.hpp"
#include "drivers/common.hpp"

constexpr uint8_t  IWDG_TIMEOUT_MS = 10;        // register-update handshake (~5 LSI cycles)
constexpr uint16_t IWDG_KEY_REFRESH = 0xAAAA;
constexpr uint16_t IWDG_KEY_UNLOCK  = 0x5555;   // unlock PR / RLR / WINR
constexpr uint16_t IWDG_KEY_START   = 0xCCCC;   // also starts LSI
constexpr uint16_t IWDG_RELOAD_MAX  = 0xFFF;
constexpr uint8_t  IWDG_PRESCALER_STEPS = 7;    // PR = 0..6 -> LSI / 4..256


struct IwdgTiming {
    uint8_t  Prescaler;  // PR code: divider = 4 << Prescaler
    uint16_t Reload;     // RLR: counter start value
};

void iwdg_timeout_out_of_range();  // not constexpr: a call is a compile error

// Smallest divider that fits -> finest refresh granularity.
// Range at 32 kHz: 0.125 ms .. 32.768 s.
consteval IwdgTiming iwdg_timing(uint32_t timeout_ms, uint32_t lsi_hz = LSI_VALUE) {
    for (uint8_t pr = 0; pr < IWDG_PRESCALER_STEPS; ++pr) {
        const uint64_t ticks = uint64_t{timeout_ms} * lsi_hz / (uint64_t{4U << pr} * 1000U);
        if (ticks >= 1 && ticks <= IWDG_RELOAD_MAX + 1U)
            return { pr, static_cast<uint16_t>(ticks - 1) };
    }
    iwdg_timeout_out_of_range();
    return {};
}

struct IwdgConfig {
    uint32_t TimeoutMs = 1000;    // no refresh within this -> reset
    uint32_t MinRefreshMs = 0;    // window mode: a refresh EARLIER than this also resets (0 = off)
    bool FreezeOnDebug = true;    // stop the counter while the core is halted by a debugger
};


template<IwdgConfig cfg>
static inline DrvStatus IWDG_Start() {
    constexpr IwdgTiming t = iwdg_timing(cfg.TimeoutMs);
    static_assert(cfg.MinRefreshMs < cfg.TimeoutMs, "IWDG: MinRefreshMs must be below TimeoutMs");

    // Window: refreshing is allowed only once the down-counter is <= WIN,
    // i.e. at least (Reload - WIN) ticks after the previous refresh.
    constexpr uint32_t min_ticks = static_cast<uint32_t>(
        uint64_t{cfg.MinRefreshMs} * LSI_VALUE / (uint64_t{4U << t.Prescaler} * 1000U));
    constexpr uint32_t win = cfg.MinRefreshMs ? t.Reload - min_ticks : IWDG_RELOAD_MAX;

    if constexpr (cfg.FreezeOnDebug) {
        DBGMCU::_APB1_FZ::DBG_IWDG_STOP::set();
    }

    IWDG::_KR::template overwrite<uint32_t{IWDG_KEY_START}>();
    IWDG::_KR::template overwrite<uint32_t{IWDG_KEY_UNLOCK}>();
    IWDG::_PR::template overwrite<uint32_t{t.Prescaler}>();
    IWDG::_RLR::template overwrite<uint32_t{t.Reload}>();

    using SR = IWDG::_SR;
    uint32_t tickstart = get_tick();
    while (SR::read() & (SR::PVU::BitMsk | SR::RVU::BitMsk)) {
        if ((get_tick() - tickstart) > IWDG_TIMEOUT_MS) return DrvStatus::Timeout;
    }

    if constexpr (cfg.MinRefreshMs != 0) {
        // Writing WINR also reloads the counter (RM0410 §42.3.5)
        IWDG::_WINR::template overwrite<win>();
        tickstart = get_tick();
        while (SR::WVU::is_set()) {
            if ((get_tick() - tickstart) > IWDG_TIMEOUT_MS) return DrvStatus::Timeout;
        }
    } else {
        IWDG::_KR::template overwrite<uint32_t{IWDG_KEY_REFRESH}>();
    }
    return DrvStatus::Ok;
}

[[gnu::always_inline]] inline void IWDG_Refresh() noexcept {
    IWDG::_KR::template overwrite<uint32_t{IWDG_KEY_REFRESH}>();
}


// ---------------------------------------------------------------------------
// Reset cause
// ---------------------------------------------------------------------------
enum class ResetCause : uint8_t {
    Unknown,
    PowerOn,              // POR/PDR
    Brownout,             // BOR
    Pin,                  // NRST pin (debugger / reset button)
    Software,             // SYSRESETREQ (NVIC_SystemReset)
    IndependentWatchdog,
    WindowWatchdog,
    LowPower,             // illegal Stop/Standby entry
};

// Decodes RCC_CSR and clears the flags (RMVF), so the answer refers to the
// last reset only. Every internal reset also pulses NRST, so the pin flag is
// checked last; a power-on sets POR, BOR and PIN together.
inline ResetCause RCC_ResetCause() noexcept {
    using CSR = RCC::_CSR;
    const uint32_t csr = CSR::read();
    CSR::RMVF::set();

    if (csr & CSR::LPWRRSTF::BitMsk) return ResetCause::LowPower;
    if (csr & CSR::WWDGRSTF::BitMsk) return ResetCause::WindowWatchdog;
    if (csr & CSR::WDGRSTF::BitMsk)  return ResetCause::IndependentWatchdog;
    if (csr & CSR::SFTRSTF::BitMsk)  return ResetCause::Software;
    if (csr & CSR::PORRSTF::BitMsk)  return ResetCause::PowerOn;
    if (csr & CSR::BORRSTF::BitMsk)  return ResetCause::Brownout;
    if (csr & CSR::PADRSTF::BitMsk)  return ResetCause::Pin;
    return ResetCause::Unknown;
}
