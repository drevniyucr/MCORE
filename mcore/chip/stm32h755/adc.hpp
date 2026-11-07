#pragma once
/*
 * STM32H755 ADC driver (16-bit SAR ADC, RM0399 "Analog-to-digital converters").
 *
 * Same API shape as the STM32F7 driver (ADC_Config / ADC_ChannelConfig /
 * ADC_Start_IT / ADC_Read ...), but the H7 ADC is a different IP:
 *   - leaves deep power-down, starts its LDO and self-calibrates before use;
 *   - every used channel must be pre-selected in PCSEL;
 *   - sample times are 1.5 ... 810.5 cycles; resolution codes are rev.V ones.
 *
 * Kernel clock: per_ck (= HSI 64 MHz after reset) through ADCx_CCR.PRESC,
 * asynchronous mode. Rev.V silicon divides it by 2 once more internally.
 */
#include <stdint.h>
#include "core/regs.hpp"
#include "core/system.hpp"
#include "core/nvic_def.hpp"
#include "config.hpp"

// ADCx_CCR.PRESC (asynchronous clock prescaler)
enum class AdcClkPresc : uint8_t {
    DIV1 = 0b0000, DIV2 = 0b0001, DIV4 = 0b0010, DIV6 = 0b0011,
    DIV8 = 0b0100, DIV10 = 0b0101, DIV12 = 0b0110, DIV16 = 0b0111,
    DIV32 = 0b1000, DIV64 = 0b1001, DIV128 = 0b1010, DIV256 = 0b1011
};

// ADC_CFGR.RES, revision V encoding (NUCLEO-H755ZI-Q ships rev.V silicon).
// RES_16B is 0b000 on every revision.
enum class AdcResolution : uint8_t {
    RES_16B = 0b000,
    RES_14B = 0b101,
    RES_12B = 0b110,
    RES_10B = 0b011,
    RES_8B  = 0b111
};

enum class AdcEOCSel : uint8_t {
    SEQ_CONV    = 0b0,   // interrupt at end of sequence (EOS)
    SINGLE_CONV = 0b1,   // interrupt at end of each conversion (EOC)
};

enum class AdcChnl : uint8_t {
    ADC_CHANNEL_0  = 0,  ADC_CHANNEL_1  = 1,  ADC_CHANNEL_2  = 2,  ADC_CHANNEL_3  = 3,
    ADC_CHANNEL_4  = 4,  ADC_CHANNEL_5  = 5,  ADC_CHANNEL_6  = 6,  ADC_CHANNEL_7  = 7,
    ADC_CHANNEL_8  = 8,  ADC_CHANNEL_9  = 9,  ADC_CHANNEL_10 = 10, ADC_CHANNEL_11 = 11,
    ADC_CHANNEL_12 = 12, ADC_CHANNEL_13 = 13, ADC_CHANNEL_14 = 14, ADC_CHANNEL_15 = 15,
    ADC_CHANNEL_16 = 16, ADC_CHANNEL_17 = 17, ADC_CHANNEL_18 = 18, ADC_CHANNEL_19 = 19
};

enum class AdcChnlRank : uint8_t {
    NONE = 0,
    ADC_RANK_1 = 1,   ADC_RANK_2 = 2,   ADC_RANK_3 = 3,   ADC_RANK_4 = 4,
    ADC_RANK_5 = 5,   ADC_RANK_6 = 6,   ADC_RANK_7 = 7,   ADC_RANK_8 = 8,
    ADC_RANK_9 = 9,   ADC_RANK_10 = 10, ADC_RANK_11 = 11, ADC_RANK_12 = 12,
    ADC_RANK_13 = 13, ADC_RANK_14 = 14, ADC_RANK_15 = 15, ADC_RANK_16 = 16
};

// ADC_SMPRx.SMPy
enum class AdcSampleTime : uint32_t {
    _1CYCLE5    = 0b000,
    _2CYCLES5   = 0b001,
    _8CYCLES5   = 0b010,
    _16CYCLES5  = 0b011,
    _32CYCLES5  = 0b100,
    _64CYCLES5  = 0b101,
    _387CYCLES5 = 0b110,
    _810CYCLES5 = 0b111
};

struct AdcHwConfig {
    AdcClkPresc ClockPrescaler = AdcClkPresc::DIV8;
    AdcResolution Resolution = AdcResolution::RES_16B;
    AdcEOCSel EOCSelection = AdcEOCSel::SINGLE_CONV;
    uint8_t NbrOfConversion = 1;
    bool ScanConvModeEnable = false;        // kept for F7 API parity: H7 always scans SQRx
    bool ContinuousConvModeEnable = false;
    bool OverrunOverwrite = true;           // OVRMOD: keep the newest sample on overrun
};

struct AdcChannel {
    AdcSampleTime SamplingTime = AdcSampleTime::_387CYCLES5;
    AdcChnlRank Rank = AdcChnlRank::NONE;
};

enum class AdcStatus : uint8_t { OK, CAL_TIMEOUT, READY_TIMEOUT };

// ---------------------------------------------------------------------------
template<typename ADCx> struct AdcTraits;
template<> struct AdcTraits<ADC1> {
    using Common = ADC12_Common;
    using IRQn = IRQn_Type::ADC_IRQn;
    static void clk_enable() { RCC::_AHB1ENR::ADC12EN::set(); (void) RCC::_AHB1ENR::read(); }
};
template<> struct AdcTraits<ADC2> {
    using Common = ADC12_Common;
    using IRQn = IRQn_Type::ADC_IRQn;
    static void clk_enable() { RCC::_AHB1ENR::ADC12EN::set(); (void) RCC::_AHB1ENR::read(); }
};
template<> struct AdcTraits<ADC3> {
    using Common = ADC3_Common;
    using IRQn = IRQn_Type::ADC3_IRQn;
    static void clk_enable() { RCC::_AHB4ENR::ADC3EN::set(); (void) RCC::_AHB4ENR::read(); }
};

namespace adc_detail {
constexpr uint32_t ISR_LDORDY = 1U << 12;   // rev.V: ADC LDO ready (missing from the SVD)
constexpr uint32_t ADC_TIMEOUT_MS = 10;
// Offset + linearity calibration takes tens of ms at the default kernel clock
// (measured on rev.V: just over 10 ms with per_ck = HSI 64 MHz, PRESC = /8).
constexpr uint32_t ADC_CAL_TIMEOUT_MS = 100;
constexpr uint32_t D3CCIPR_ADCSEL_PER_CK = 0b10;

constexpr uint32_t presc_div(AdcClkPresc p) {
    constexpr uint16_t table[12] = {1, 2, 4, 6, 8, 10, 12, 16, 32, 64, 128, 256};
    return table[static_cast<uint32_t>(p)];
}
// BOOST range (rev.V) from the prescaled kernel clock, ignoring the extra
// internal /2: picking a range that is too high is safe, too low is not.
constexpr uint32_t boost_for(uint32_t f) {
    return f <= 6250000U ? 0b00U : f <= 12500000U ? 0b01U : f <= 25000000U ? 0b10U : 0b11U;
}

template<typename Pred>
bool wait(Pred p, uint32_t timeout_ms = ADC_TIMEOUT_MS) {
    const uint32_t start = get_tick();
    while (!p()) {
        if ((get_tick() - start) > timeout_ms) return false;
    }
    return true;
}
} // namespace adc_detail

// ---------------------------------------------------------------------------
// Power-up, calibration, enable, global configuration.
// ---------------------------------------------------------------------------
template <typename ADCx, auto cfg>
AdcStatus ADC_Config() {
    using namespace adc_detail;
    using Common = typename AdcTraits<ADCx>::Common;
    static_assert(cfg.NbrOfConversion >= 1 && cfg.NbrOfConversion <= 16, "NbrOfConversion must be 1..16");

    // 1. Bus clock + kernel clock (per_ck, HSI after reset) + prescaler
    AdcTraits<ADCx>::clk_enable();
    RCC::_D3CCIPR::ADCSEL::write(D3CCIPR_ADCSEL_PER_CK);
    Common::_CCR::CKMODE::write(0U);   // asynchronous kernel clock
    Common::_CCR::PRESC::write(static_cast<uint32_t>(cfg.ClockPrescaler));

    // 2. Leave deep power-down and start the internal regulator
    ADCx::_CR::DEEPPWD::clear();
    ADCx::_CR::ADVREGEN::set();
    delay_us(20);   // TADCVREG_STUP = 10 us
    // Rev.V silicon also reports LDORDY; rev.Y has no such flag, so a missing
    // flag after the start-up delay is not treated as an error.
    (void) wait([] { return (ADCx::_ISR::read() & ISR_LDORDY) != 0U; });
    ADCx::_CR::BOOST::write(boost_for(HSI_VALUE / presc_div(cfg.ClockPrescaler)));

    // 3. Single-ended offset + linearity calibration (ADEN must be 0)
    ADCx::_CR::ADCALDIF::clear();
    ADCx::_CR::ADCALLIN::set();
    ADCx::_CR::ADCAL::set();
    if (!wait([] { return ADCx::_CR::ADCAL::read() == 0U; }, ADC_CAL_TIMEOUT_MS)) {
        return AdcStatus::CAL_TIMEOUT;
    }

    // 4. Enable and wait for ADRDY (flags are write-1-to-clear)
    ADCx::_ISR::overwrite(ADCx::_ISR::ADRDY::BitMsk);
    ADCx::_CR::ADEN::set();
    if (!wait([] { return ADCx::_ISR::ADRDY::read() != 0U; })) {
        return AdcStatus::READY_TIMEOUT;
    }
    ADCx::_ISR::overwrite(ADCx::_ISR::ADRDY::BitMsk);

    // 5. Regular group: software trigger, data kept in DR (DMNGT = 0)
    ADCx::_CFGR::overwrite(
          (static_cast<uint32_t>(cfg.Resolution) << ADCx::_CFGR::RES::pos)
        | (cfg.ContinuousConvModeEnable ? ADCx::_CFGR::CONT::BitMsk : 0U)
        | (cfg.OverrunOverwrite ? ADCx::_CFGR::OVRMOD::BitMsk : 0U));

    // 6. Sequence length: SQR1.L (bits 3:0) = conversions - 1
    ADCx::_SQR1::clearMask(0xFU);
    ADCx::_SQR1::setMask(static_cast<uint32_t>(cfg.NbrOfConversion - 1U));

    // Interrupt source used by ADC_Start_IT
    if constexpr (cfg.EOCSelection == AdcEOCSel::SEQ_CONV) {
        ADCx::_IER::overwrite(ADCx::_IER::EOSIE::BitMsk | ADCx::_IER::OVRIE::BitMsk);
    } else {
        ADCx::_IER::overwrite(ADCx::_IER::EOCIE::BitMsk | ADCx::_IER::OVRIE::BitMsk);
    }
    return AdcStatus::OK;
}

// ---------------------------------------------------------------------------
// Channel: preselection, sample time, rank in the regular sequence.
// Must be called while no conversion is running (ADSTART = 0).
// ---------------------------------------------------------------------------
template <typename ADCx, auto cfg, AdcChnl ch>
void ADC_ChannelConfig() {
    constexpr uint32_t n = static_cast<uint32_t>(ch);
    static_assert(n <= 19, "H7 ADC channels are 0..19");

    ADCx::_PCSEL::setMask(1U << n);

    if constexpr (n < 10) {
        constexpr uint32_t shft = n * 3U;
        ADCx::_SMPR1::clearMask(0b111U << shft);
        ADCx::_SMPR1::setMask(static_cast<uint32_t>(cfg.SamplingTime) << shft);
    } else {
        constexpr uint32_t shft = (n - 10U) * 3U;
        ADCx::_SMPR2::clearMask(0b111U << shft);
        ADCx::_SMPR2::setMask(static_cast<uint32_t>(cfg.SamplingTime) << shft);
    }

    if constexpr (cfg.Rank != AdcChnlRank::NONE) {
        constexpr uint32_t r = static_cast<uint32_t>(cfg.Rank);
        if constexpr (r <= 4) {
            constexpr uint32_t shft = r * 6U;          // SQ1 @6, SQ2 @12, SQ3 @18, SQ4 @24
            ADCx::_SQR1::clearMask(0x1FU << shft);
            ADCx::_SQR1::setMask(n << shft);
        } else if constexpr (r <= 9) {
            constexpr uint32_t shft = (r - 5U) * 6U;
            ADCx::_SQR2::clearMask(0x1FU << shft);
            ADCx::_SQR2::setMask(n << shft);
        } else if constexpr (r <= 14) {
            constexpr uint32_t shft = (r - 10U) * 6U;
            ADCx::_SQR3::clearMask(0x1FU << shft);
            ADCx::_SQR3::setMask(n << shft);
        } else {
            constexpr uint32_t shft = (r - 15U) * 6U;
            ADCx::_SQR4::clearMask(0x1FU << shft);
            ADCx::_SQR4::setMask(n << shft);
        }
    }
}

// ---------------------------------------------------------------------------
// Runtime
// ---------------------------------------------------------------------------
template<typename ADCx>
static inline void ADC_Start_IT() {
    ADCx::_ISR::overwrite(ADCx::_ISR::EOC::BitMsk | ADCx::_ISR::EOS::BitMsk | ADCx::_ISR::OVR::BitMsk);
    ADCx::_CR::ADSTART::set();
}

// Polling start: same as ADC_Start_IT with the ADC interrupts masked.
template<typename ADCx>
static inline void ADC_Start() {
    ADCx::_IER::clearMask(ADCx::_IER::EOCIE::BitMsk | ADCx::_IER::EOSIE::BitMsk | ADCx::_IER::OVRIE::BitMsk);
    ADC_Start_IT<ADCx>();
}

template<typename ADCx>
static inline bool ADC_Ready() { return ADCx::_ISR::EOC::read() != 0U; }

// Reading DR clears EOC.
template<typename ADCx>
static inline uint32_t ADC_Read() { return ADCx::_DR::RDATA::read(); }

template<typename ADCx>
static inline bool ADC_IsOVR() { return ADCx::_ISR::OVR::read() != 0U; }

template<typename ADCx>
static inline void ADC_ClearOVR() { ADCx::_ISR::overwrite(ADCx::_ISR::OVR::BitMsk); }

template<typename ADCx>
inline void ADC_Stop() {
    if (ADCx::_CR::ADSTART::read() != 0U) {
        ADCx::_CR::ADSTP::set();
        while (ADCx::_CR::ADSTP::read() != 0U) {
        }
    }
}

template<typename ADCx>
inline bool ADC_IsEnabled() { return ADCx::_CR::ADEN::read() != 0U; }
