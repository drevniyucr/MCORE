/*
 * rtc.hpp — real-time clock: calendar, periodic wakeup, backup registers
 *
 *   RTC_Init<cfg>()                start the clock; a clock that is already
 *                                  running from the same source is left alone,
 *                                  so the time survives every reset that keeps
 *                                  the backup domain powered (VDD or VBAT)
 *   RTC_SetDateTime / RTC_GetDateTime
 *   RTC_IsTimeSet()                false until the calendar was set once
 *   RTC_WakeupStart / RTC_WakeupStop + RTC_WakeupIRQHandler
 *   RtcBackup<N>                   32-bit backup registers 0..30 (31 is the driver's)
 *
 * The RTC lives in the backup domain: RCC_BDCR and every RTC register need
 * PWR_CR1.DBP = 1 to be written, and the calendar registers additionally sit
 * behind the RTC_WPR key. 24-hour format, years 2000..2099.
 * Alarms, tamper, timestamp and smooth calibration are not covered.
 */
#pragma once

#include <cstdint>
#include "core/regs.hpp"
#include "core/system.hpp"
#include "core/mcore_config.hpp"
#include "drivers/common.hpp"

constexpr uint16_t RTC_LSE_TIMEOUT_MS = 5000;  // a 32.768 kHz crystal may take ~2 s to start
constexpr uint8_t  RTC_TIMEOUT_MS = 100;       // INIT / RSF / LSI / WUTWF handshakes
constexpr uint8_t  RTC_PREDIV_A = 127;         // async /128: keeps ck_apre fast, saves power
constexpr uint8_t  RTC_WPR_KEY1 = 0xCA;
constexpr uint8_t  RTC_WPR_KEY2 = 0x53;
constexpr uint8_t  RTC_WPR_LOCK = 0xFF;
constexpr uint32_t RTC_ISR_WRITABLE = 0x0001FFFF;  // ISR bits 0..16: rc_w0 flags + INIT
constexpr uint32_t RTC_TIME_SET_MAGIC = 0x32F2;
constexpr uint8_t  RTC_BKP_DRIVER = 31;            // backup register used for the magic
constexpr uint32_t RTC_EXTI_WAKEUP_LINE = 1U << 22;
constexpr uint32_t RTC_WUCKSEL_1HZ = 0b100;        // wakeup timer clocked by ck_spre (1 Hz)

enum class RtcClock : uint8_t { Lse, Lsi };

struct RtcConfig {
    RtcClock Source = RtcClock::Lse;  // LSE: 32.768 kHz crystal (on NUCLEO-144: X2)
    bool LseBypass = false;           // LSE driven by an external clock instead of a crystal
};

struct RtcDateTime {
    uint16_t year = 2000;   // 2000..2099
    uint8_t  month = 1;     // 1..12
    uint8_t  day = 1;       // 1..31
    uint8_t  hour = 0;      // 0..23
    uint8_t  minute = 0;
    uint8_t  second = 0;
    uint8_t  weekday = 6;   // 1 = Monday .. 7 = Sunday; computed by RTC_SetDateTime
    uint16_t millis = 0;    // read only: sub-second part
};

namespace rtc_detail {
constexpr uint8_t to_bcd(uint32_t v) { return static_cast<uint8_t>(((v / 10) << 4) | (v % 10)); }
constexpr uint8_t from_bcd(uint32_t v) { return static_cast<uint8_t>((v >> 4) * 10 + (v & 0xF)); }

constexpr uint8_t days_in_month(uint16_t year, uint8_t month) {
    constexpr uint8_t days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    const bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    return month == 2 && leap ? 29 : days[month - 1];
}

// Sakamoto: 0 = Sunday .. 6 = Saturday, remapped to the RTC's 1 = Monday .. 7 = Sunday.
constexpr uint8_t weekday(uint16_t year, uint8_t month, uint8_t day) {
    constexpr uint8_t t[12] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    const uint32_t y = month < 3 ? year - 1U : year;
    const uint32_t w = (y + y / 4 - y / 100 + y / 400 + t[month - 1] + day) % 7;
    return static_cast<uint8_t>(w == 0 ? 7 : w);
}

// Howard Hinnant's days_from_civil: days since 1970-01-01
constexpr int64_t days_from_civil(int64_t y, unsigned m, unsigned d) {
	y -= m <= 2;
	const int64_t era = (y >= 0 ? y : y - 399) / 400;
	const auto yoe = static_cast<unsigned>(y - era * 400);
	const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
	const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
	return era * 146097 + static_cast<int64_t>(doe) - 719468;
}
static_assert(days_from_civil(1970, 1, 1) == 0);
static_assert(days_from_civil(2000, 3, 1) == 11017);

// ...and back: civil date of a day count since 1970-01-01
struct civil_t { int64_t y; unsigned m, d; };
constexpr civil_t civil_from_days(int64_t z) {
	z += 719468;
	const int64_t era = (z >= 0 ? z : z - 146096) / 146097;
	const auto doe = static_cast<unsigned>(z - era * 146097);
	const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
	const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
	const unsigned mp = (5 * doy + 2) / 153;
	const unsigned d = doy - (153 * mp + 2) / 5 + 1;
	const unsigned m = mp < 10 ? mp + 3 : mp - 9;
	return { static_cast<int64_t>(yoe) + era * 400 + (m <= 2), m, d };
}
static_assert(civil_from_days(0).y == 1970 && civil_from_days(0).m == 1 && civil_from_days(0).d == 1);
static_assert(civil_from_days(11017).y == 2000 && civil_from_days(11017).m == 3 && civil_from_days(11017).d == 1);

constexpr bool valid(const RtcDateTime& t) {
    return t.year >= 2000 && t.year <= 2099 && t.month >= 1 && t.month <= 12
        && t.day >= 1 && t.day <= days_in_month(t.year, t.month)
        && t.hour < 24 && t.minute < 60 && t.second < 60;
}

constexpr uint8_t parse2(const char* s) {
    return static_cast<uint8_t>((s[0] == ' ' ? 0 : s[0] - '0') * 10 + (s[1] - '0'));
}

// ISR mixes rc_w0 flags with the INIT control bit: write 1 to every flag that
// must survive and keep INIT as it is — never a read-modify-write, which
// would clear a flag raised between the read and the write.
inline void isr_clear(uint32_t flags) noexcept {
    const uint32_t init = RTC::_ISR::read() & RTC::_ISR::INIT::BitMsk;
    RTC::_ISR::overwrite((RTC_ISR_WRITABLE & ~(flags | RTC::_ISR::INIT::BitMsk)) | init);
}

inline void unlock() noexcept {
    RTC::_WPR::overwrite(uint32_t{RTC_WPR_KEY1});
    RTC::_WPR::overwrite(uint32_t{RTC_WPR_KEY2});
}
inline void lock() noexcept { RTC::_WPR::overwrite(uint32_t{RTC_WPR_LOCK}); }

inline DrvStatus wait(uint32_t mask, bool set, uint32_t timeout_ms) noexcept {
    const uint32_t tickstart = get_tick();
    while (((RTC::_ISR::read() & mask) != 0) != set) {
        if ((get_tick() - tickstart) > timeout_ms) return DrvStatus::Timeout;
    }
    return DrvStatus::Ok;
}

inline DrvStatus enter_init() noexcept {
    RTC::_ISR::overwrite(RTC_ISR_WRITABLE);  // INIT = 1, every flag written 1 = untouched
    return wait(RTC::_ISR::INITF::BitMsk, true, RTC_TIMEOUT_MS);
}

// Leave init mode and wait for the shadow registers to resynchronise, so the
// next RTC_GetDateTime sees the new values.
inline DrvStatus exit_init() noexcept {
    RTC::_ISR::overwrite(RTC_ISR_WRITABLE & ~(RTC::_ISR::INIT::BitMsk | RTC::_ISR::RSF::BitMsk));
    return wait(RTC::_ISR::RSF::BitMsk, true, RTC_TIMEOUT_MS);
}
}  // namespace rtc_detail


// Backup register N (0..31) — kept across resets with the backup domain.
template<uint8_t N>
struct RtcBackup {
    static_assert(N < 32, "RTC: only 32 backup registers");
    struct Reg : Register<RTC::_BKP0R::address + 4U * N, ReadWrite, Reg> {};
    static uint32_t read() noexcept { return Reg::read(); }
    static void write(uint32_t v) noexcept { Reg::overwrite(v); }
};


template<RtcConfig cfg>
static inline DrvStatus RTC_Init() {
    using BDCR = RCC::_BDCR;
    constexpr uint32_t sel_mask = BDCR::RTCSEL0::BitMsk | BDCR::RTCSEL1::BitMsk;
    constexpr uint32_t sel = cfg.Source == RtcClock::Lse ? BDCR::RTCSEL0::BitMsk   // 0b01
                                                         : BDCR::RTCSEL1::BitMsk;  // 0b10
    uint32_t tickstart;

    /* 0. Backup-domain write access */
    RCC::_APB1ENR::PWREN::set();
    PWR::_CR1::DBP::set();
    tickstart = get_tick();
    while (!PWR::_CR1::DBP::is_set()) {
        if ((get_tick() - tickstart) > RTC_TIMEOUT_MS) return DrvStatus::Timeout;
    }

    /* 1. RTCSEL is write-once: another source needs a backup-domain reset
          (which also wipes the calendar and the backup registers) */
    const uint32_t current = BDCR::read() & sel_mask;
    if (current != 0 && current != sel) {
        BDCR::BDRST::set();
        BDCR::BDRST::clear();
    }

    /* 2. Oscillator. LSE lives in the backup domain and keeps running through
          resets; LSI does not and has to be restarted after every reset. */
    if constexpr (cfg.Source == RtcClock::Lse) {
        if (!BDCR::LSERDY::is_set()) {
            if constexpr (cfg.LseBypass) BDCR::LSEBYP::set();
            BDCR::LSEON::set();
            tickstart = get_tick();
            while (!BDCR::LSERDY::is_set()) {
                if ((get_tick() - tickstart) > RTC_LSE_TIMEOUT_MS) return DrvStatus::Timeout;
            }
        }
    } else {
        RCC::_CSR::LSION::set();
        tickstart = get_tick();
        while (!RCC::_CSR::LSIRDY::is_set()) {
            if ((get_tick() - tickstart) > RTC_TIMEOUT_MS) return DrvStatus::Timeout;
        }
    }

    /* 3. Already running from this source: keep the time */
    if (BDCR::RTCEN::is_set() && (BDCR::read() & sel_mask) == sel) {
        return DrvStatus::Ok;
    }

    /* 4. Fresh start: clock the RTC and set prescalers for a 1 Hz ck_spre */
    BDCR::overwrite((BDCR::read() & ~sel_mask) | sel | BDCR::RTCEN::BitMsk);

    constexpr uint32_t rtcclk = cfg.Source == RtcClock::Lse ? LSE_VALUE : LSI_VALUE;
    constexpr uint32_t prediv_s = rtcclk / (RTC_PREDIV_A + 1U) - 1U;
    static_assert(prediv_s <= 0x7FFF, "RTC: PREDIV_S out of range");

    rtc_detail::unlock();
    DrvStatus st = rtc_detail::enter_init();
    if (st == DrvStatus::Ok) {
        // PRER takes two separate writes: synchronous part first (RM0410 §32.3.7)
        RTC::_PRER::template overwrite<prediv_s>();
        RTC::_PRER::template overwrite<prediv_s | (uint32_t{RTC_PREDIV_A} << RTC::_PRER::PREDIV_A::pos)>();
        RTC::_CR::template overwrite<0U>();   // 24 h, wakeup/alarms off
        st = rtc_detail::exit_init();         // calendar stays at its reset value: 2000-01-01
    }
    rtc_detail::lock();
    return st;
}

/** True once RTC_SetDateTime has run since the backup domain was last reset. */
inline bool RTC_IsTimeSet() noexcept {
    return RtcBackup<RTC_BKP_DRIVER>::read() == RTC_TIME_SET_MAGIC;
}

/** Set the calendar; the weekday is derived from the date. */
inline DrvStatus RTC_SetDateTime(const RtcDateTime& t) {
    using namespace rtc_detail;
    if (!valid(t)) return DrvStatus::InvalidArg;

    using TR = RTC::_TR;
    using DR = RTC::_DR;
    const uint32_t tr = uint32_t{to_bcd(t.hour)} << TR::HU::pos
                      | uint32_t{to_bcd(t.minute)} << TR::MNU::pos
                      | uint32_t{to_bcd(t.second)} << TR::SU::pos;
    const uint32_t dr = uint32_t{to_bcd(t.year - 2000U)} << DR::YU::pos
                      | uint32_t{weekday(t.year, t.month, t.day)} << DR::WDU::pos
                      | uint32_t{to_bcd(t.month)} << DR::MU::pos
                      | uint32_t{to_bcd(t.day)} << DR::DU::pos;

    unlock();
    DrvStatus st = enter_init();
    if (st == DrvStatus::Ok) {
        TR::overwrite(tr);
        DR::overwrite(dr);
        st = exit_init();
    }
    lock();

    if (st == DrvStatus::Ok) RtcBackup<RTC_BKP_DRIVER>::write(RTC_TIME_SET_MAGIC);
    return st;
}

/** Read the calendar. SSR -> TR -> DR order: reading SSR/TR freezes the
 *  higher-order shadow registers until DR is read, so the snapshot is coherent. */
inline void RTC_GetDateTime(RtcDateTime& t) noexcept {
    using namespace rtc_detail;
    using TR = RTC::_TR;
    using DR = RTC::_DR;

    const uint32_t ss = RTC::_SSR::read() & RTC::_SSR::SS::BitMsk;
    const uint32_t tr = TR::read();
    const uint32_t dr = DR::read();
    const uint32_t prediv_s = RTC::_PRER::PREDIV_S::read();

    t.hour   = from_bcd((tr & (TR::HT::BitMsk | TR::HU::BitMsk)) >> TR::HU::pos);
    t.minute = from_bcd((tr & (TR::MNT::BitMsk | TR::MNU::BitMsk)) >> TR::MNU::pos);
    t.second = from_bcd((tr & (TR::ST::BitMsk | TR::SU::BitMsk)) >> TR::SU::pos);
    t.year   = static_cast<uint16_t>(2000U + from_bcd((dr & (DR::YT::BitMsk | DR::YU::BitMsk)) >> DR::YU::pos));
    t.month  = from_bcd((dr & (DR::MT::BitMsk | DR::MU::BitMsk)) >> DR::MU::pos);
    t.day    = from_bcd((dr & (DR::DT::BitMsk | DR::DU::BitMsk)) >> DR::DU::pos);
    t.weekday = static_cast<uint8_t>((dr & DR::WDU::BitMsk) >> DR::WDU::pos);
    // SS counts down from PREDIV_S within each second
    t.millis = static_cast<uint16_t>((prediv_s - ss) * 1000U / (prediv_s + 1U));
}

/** Seconds since 1970-01-01 00:00 of a calendar value (taken as UTC). */
constexpr int64_t rtc_unix(const RtcDateTime &t) {
	return rtc_detail::days_from_civil(t.year, t.month, t.day) * 86400
	     + t.hour * 3600 + t.minute * 60 + t.second;
}

/** Calendar value of Unix time (weekday filled in). */
constexpr RtcDateTime rtc_from_unix(int64_t unix_s) {
	const int64_t days = (unix_s >= 0 ? unix_s : unix_s - 86399) / 86400;
	const auto secs = static_cast<uint32_t>(unix_s - days * 86400);
	const rtc_detail::civil_t c = rtc_detail::civil_from_days(days);
	RtcDateTime t{};
	t.year = static_cast<uint16_t>(c.y);
	t.month = static_cast<uint8_t>(c.m);
	t.day = static_cast<uint8_t>(c.d);
	t.hour = static_cast<uint8_t>(secs / 3600);
	t.minute = static_cast<uint8_t>(secs / 60 % 60);
	t.second = static_cast<uint8_t>(secs % 60);
	t.weekday = rtc_detail::weekday(t.year, t.month, t.day);
	return t;
}
static_assert(rtc_unix(rtc_from_unix(1793006848)) == 1793006848);
static_assert(rtc_from_unix(1793006848).weekday == 1);   // 2026-10-26: Monday

/** The RTC calendar as Unix time. */
inline int64_t RTC_UnixTime() noexcept {
	RtcDateTime t;
	RTC_GetDateTime(t);
	return rtc_unix(t);
}

/** Set the calendar from Unix time (InvalidArg outside 2000..2099). */
inline DrvStatus RTC_SetUnixTime(int64_t unix_s) {
	return RTC_SetDateTime(rtc_from_unix(unix_s));
}

void rtc_bad_build_timestamp();  // not constexpr: a call is a compile error

// Compile time of this translation unit, e.g. to seed the clock on first boot:
//   if (!RTC_IsTimeSet()) RTC_SetDateTime(rtc_build_time());
consteval RtcDateTime rtc_build_time(const char* date = __DATE__, const char* time = __TIME__) {
    // __DATE__ = "Mmm dd yyyy" (day space-padded), __TIME__ = "hh:mm:ss"
    constexpr const char* months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    uint8_t month = 0;
    for (uint8_t m = 0; m < 12; ++m) {
        if (date[0] == months[m * 3] && date[1] == months[m * 3 + 1] && date[2] == months[m * 3 + 2])
            month = static_cast<uint8_t>(m + 1);
    }
    if (month == 0) rtc_bad_build_timestamp();

    RtcDateTime t{};
    t.year = static_cast<uint16_t>(rtc_detail::parse2(date + 7) * 100 + rtc_detail::parse2(date + 9));
    t.month = month;
    t.day = rtc_detail::parse2(date + 4);
    t.hour = rtc_detail::parse2(time);
    t.minute = rtc_detail::parse2(time + 3);
    t.second = rtc_detail::parse2(time + 6);
    t.weekday = rtc_detail::weekday(t.year, t.month, t.day);
    return t;
}


// ---------------------------------------------------------------------------
// Periodic wakeup (EXTI line 22 -> RTC_WKUP_IRQn; NVIC is up to the application)
// ---------------------------------------------------------------------------
using RtcCallback = void (*)();

struct RtcWakeup {
    static inline RtcCallback callback = nullptr;  // runs in interrupt context
};

/** Fire RTC_WKUP_IRQn every period_s seconds (1..65535). */
inline DrvStatus RTC_WakeupStart(uint16_t period_s, RtcCallback cb) {
    using namespace rtc_detail;
    using CR = RTC::_CR;
    if (period_s == 0) return DrvStatus::InvalidArg;

    RtcWakeup::callback = cb;

    unlock();
    CR::overwrite(CR::read() & ~(CR::WUTE::BitMsk | CR::WUTIE::BitMsk));
    // WUTR / WUCKSEL are writable only once WUTWF confirms the timer stopped
    DrvStatus st = wait(RTC::_ISR::WUTWF::BitMsk, true, RTC_TIMEOUT_MS);
    if (st == DrvStatus::Ok) {
        RTC::_WUTR::overwrite(uint32_t{period_s} - 1U);
        isr_clear(RTC::_ISR::WUTF::BitMsk);
        CR::overwrite((CR::read() & ~CR::WCKSEL::BitMsk)
                      | (RTC_WUCKSEL_1HZ << CR::WCKSEL::pos)
                      | CR::WUTIE::BitMsk | CR::WUTE::BitMsk);
    }
    lock();

    EXTI::_PR::overwrite(RTC_EXTI_WAKEUP_LINE);  // w1c
    EXTI::_RTSR::TR22::set();
    EXTI::_IMR::MR22::set();
    return st;
}

inline void RTC_WakeupStop() noexcept {
    using CR = RTC::_CR;
    EXTI::_IMR::MR22::clear();
    rtc_detail::unlock();
    CR::overwrite(CR::read() & ~(CR::WUTE::BitMsk | CR::WUTIE::BitMsk));
    rtc_detail::lock();
}

// Call from the vector stub:
//   extern "C" void RTC_WKUP_IRQHandler() { RTC_WakeupIRQHandler(); }
inline void RTC_WakeupIRQHandler() {
    if (RTC::_ISR::WUTF::is_set()) {
        rtc_detail::isr_clear(RTC::_ISR::WUTF::BitMsk);  // RTC flag first, then EXTI
        EXTI::_PR::overwrite(RTC_EXTI_WAKEUP_LINE);
        if (RtcWakeup::callback) RtcWakeup::callback();
    }
}
