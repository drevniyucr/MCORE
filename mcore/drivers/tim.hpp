#pragma once
#include <cstdint>
#include "core/regs.hpp"
#include "core/rcc.hpp"
#include "drivers/common.hpp"
#include "drivers/dma.hpp"

// ------------------- ENUMS -------------------
enum class TimCounterMode : uint32_t {
    Up = 0,
    Down = TIM1::_CR1::DIR::BitMsk,
    Center1 = 1u << TIM1::_CR1::CMS::pos,
    Center2 = 2u << TIM1::_CR1::CMS::pos,
    Center3 = 3u << TIM1::_CR1::CMS::pos
};

enum class TimCh : uint8_t {
    None, CH1, CH2, CH3, CH4, CH5, CH6
};

enum class TimOCPol : uint8_t {
    HIGH, LOW, NONE
};

enum class TimIdleState : uint8_t {
    RESET, SET, NONE
};

enum class TimOCMode : uint32_t {
    TIMING = 0b0000,
    ACTIVE = 0b0001,
    INACTIVE = 0b0010,
    TOGGLE = 0b0011,
    FORCED_INACTIVE = 0b0100,
    FORCED_ACTIVE = 0b0101,
    PWM1 = 0b0110,
    PWM2 = 0b0111,
    RETRIGERRABLE_OPM1 = 0b1000,
    RETRIGERRABLE_OPM2 = 0b1001,
    COMBINED_PWM1 = 0b1100,
    COMBINED_PWM2 = 0b1101,
    ASYMMETRIC_PWM1 = 0b1110,
    ASYMMETRIC_PWM2 = 0b1111
};

enum class TimClkDiv : uint8_t {
    DIV1 = 0b00, DIV2 = 0b01, DIV4 = 0b10
};

// ------------------- CONFIGS -------------------
struct TimBaseConfig {
    uint32_t Prescaler = 0;   // PSC
    uint32_t Period = 0;      // ARR
    TimCounterMode CounterMode = TimCounterMode::Up;
    TimClkDiv ClockDiv = TimClkDiv::DIV1;
    uint32_t RepetitionCounter = 0;
    bool EnablePreload = false;
};

struct TimOCConfig {
    TimOCMode OCMode = TimOCMode::TIMING;
    TimOCPol OCPolarity = TimOCPol::NONE;
    TimOCPol OCNPolarity = TimOCPol::NONE;
    TimIdleState OCIdleState = TimIdleState::NONE;
    TimIdleState OCNIdleState = TimIdleState::NONE;
    uint32_t Pulse = 0;
    bool OC_PreloadEnable = false;
    bool FastEnable = false;
};

// Clock enable: unified RccEnable<TIMx> map in core/rcc.hpp.

// ------------------- TIMER PROPERTIES (битовая маска) -------------------
struct TIM_properties {
    bool isAdvanced = false, hasOCM3 = false, hasRepetition = false, hasClkDiv =
        false;
    TimCh chnl = TimCh::None;
};

template<typename TIMx> struct TimFlags;
/*  ADV    OCM3   Repet  ClkDiv CHANNEL  */
template<> struct TimFlags<TIM1> {
    static constexpr TIM_properties f{ true, true, true, true, TimCh::CH6 };
};
template<> struct TimFlags<TIM2> {
    static constexpr TIM_properties f{ false, true, false, true, TimCh::CH4 };
};
template<> struct TimFlags<TIM3> {
    static constexpr TIM_properties f{ false, true, false, true, TimCh::CH4 };
};
template<> struct TimFlags<TIM4> {
    static constexpr TIM_properties f{ false, true, false, true, TimCh::CH4 };
};
template<> struct TimFlags<TIM5> {
    static constexpr TIM_properties f{ false, true, false, true, TimCh::CH4 };
};
template<> struct TimFlags<TIM6> {
    static constexpr TIM_properties f{ false, false, false, false, TimCh::None };
};
template<> struct TimFlags<TIM7> {
    static constexpr TIM_properties f{ false, false, false, false, TimCh::None };
};
template<> struct TimFlags<TIM8> {
    static constexpr TIM_properties f{ true, true, true, true, TimCh::CH6 };
};
template<> struct TimFlags<TIM9> {
    static constexpr TIM_properties f{ false, true, false, true, TimCh::CH2 };
};
template<> struct TimFlags<TIM10> {
    static constexpr TIM_properties f{ false, false, false, true, TimCh::CH1 };
};
template<> struct TimFlags<TIM11> {
    static constexpr TIM_properties f{ false, false, false, true, TimCh::CH1 };
};
template<> struct TimFlags<TIM12> {
    static constexpr TIM_properties f{ false, true, false, true, TimCh::CH2 };
};
template<> struct TimFlags<TIM13> {
    static constexpr TIM_properties f{ false, false, false, true, TimCh::CH1 };
};
template<> struct TimFlags<TIM14> {
    static constexpr TIM_properties f{ false, false, false, true, TimCh::CH1 };
};

// ------------------- CHANNEL TRAITS -------------------
template<typename TIMx, TimCh ch>
struct TimChannelTraits {
    static_assert(TimCh::None != TimFlags<TIMx>::f.chnl, "This timer does not support channels ");
    static_assert(ch <= TimFlags<TIMx>::f.chnl, "This timer does not have the requested channel");
    static_assert(ch != TimCh::None, "Channel must be specified");
    // Выбор типа CCMR. Возврат ОБЪЕКТА из if constexpr здесь намеренный:
    // std::conditional_t не подходит — он требует, чтобы _CCMR3_Output
    // существовал у ВСЕХ таймеров (имена всех веток инстанцируются),
    // а у не-advanced таймеров его нет. Отброшенная ветка if constexpr
    // имён не инстанцирует.
    template<TimCh Ch>
    static auto getCCMRType() {
        if constexpr (Ch <= TimCh::CH2)
            return typename TIMx::_CCMR1_Output{ };
        else if constexpr (Ch <= TimCh::CH4)
            return typename TIMx::_CCMR2_Output{ };
        else
            return typename TIMx::_CCMR3_Output{ };
    }

    // Все канальные константы (CCER-полярность/enable, DIER-DMA, адрес CCR)
    // одной ленивой цепочкой if constexpr — вместо четырёх почти одинаковых
    // helper-структур. Ленивость важна: инстанцируется только взятая ветка,
    // поэтому таймеры без CH5/CH6 или без CCxDE-битов продолжают собираться.
    struct ChConsts { uint32_t pol; uint32_t en; uint32_t dma; uint32_t ccr; };

    static consteval ChConsts ch_consts() {
        if constexpr (ch == TimCh::CH1)
            return { TIMx::_CCER::CC1P::BitMsk, TIMx::_CCER::CC1E::BitMsk,
                     TIMx::_DIER::CC1DE::BitMsk, TIMx::_CCR1::address };
        else if constexpr (ch == TimCh::CH2)
            return { TIMx::_CCER::CC2P::BitMsk, TIMx::_CCER::CC2E::BitMsk,
                     TIMx::_DIER::CC2DE::BitMsk, TIMx::_CCR2::address };
        else if constexpr (ch == TimCh::CH3)
            return { TIMx::_CCER::CC3P::BitMsk, TIMx::_CCER::CC3E::BitMsk,
                     TIMx::_DIER::CC3DE::BitMsk, TIMx::_CCR3::address };
        else if constexpr (ch == TimCh::CH4)
            return { TIMx::_CCER::CC4P::BitMsk, TIMx::_CCER::CC4E::BitMsk,
                     TIMx::_DIER::CC4DE::BitMsk, TIMx::_CCR4::address };
        else if constexpr (ch == TimCh::CH5)
            return { TIMx::_CCER::CC5P::BitMsk, TIMx::_CCER::CC5E::BitMsk,
                     0, TIMx::_CCR5::address };  // CH5/CH6: нет DMA-запроса
        else
            return { TIMx::_CCER::CC6P::BitMsk, TIMx::_CCER::CC6E::BitMsk,
                     0, TIMx::_CCR6::address };
    }
    static constexpr ChConsts chc = ch_consts();

    using CCMR = decltype(getCCMRType<ch>());

    // Маски и сдвиги для режима
    static constexpr uint32_t oc_mode_shift =
    (ch == TimCh::CH1 || ch == TimCh::CH3 || ch == TimCh::CH5) ? 4 : 12;

    static constexpr uint32_t oc_mode_mask = 0b111 << oc_mode_shift;

    static constexpr uint32_t preload_en_mask =
    (ch == TimCh::CH1 || ch == TimCh::CH3 || ch == TimCh::CH5) ? ( 1 << 3) : (1 << 11);

    static constexpr uint32_t fast_en_mask =
    (ch == TimCh::CH1 || ch == TimCh::CH3 || ch == TimCh::CH5) ? ( 1 << 2) : (1 << 10);

    // Маски включения и полярности (см. ChConsts выше)
    static constexpr uint32_t enable_mask     = chc.en;
    static constexpr uint32_t polarity_mask   = chc.pol;
    static constexpr uint32_t dma_enable_mask = chc.dma;

    static constexpr uint32_t ccr_address     = chc.ccr;

    // Запись в CCR
    static inline void write_ccr(uint32_t v) {
        if constexpr (ch == TimCh::CH1)
            TIMx::_CCR1::overwrite(v);
        else if constexpr (ch == TimCh::CH2)
            TIMx::_CCR2::overwrite(v);
        else if constexpr (ch == TimCh::CH3)
            TIMx::_CCR3::overwrite(v);
        else if constexpr (ch == TimCh::CH4)
            TIMx::_CCR4::overwrite(v);
        else if constexpr (ch == TimCh::CH5)
            TIMx::_CCR5::overwrite(v);
        else if constexpr (ch == TimCh::CH6)
            TIMx::_CCR6::overwrite(v);
    }

    // Настройка комплементарного выхода
    static inline void set_complementary(uint32_t& ccer, TimOCPol pol) {
        static_assert(ch <= TimCh::CH3, "Complementary output only supported for channels 1-3");
        static_assert(TimFlags<TIMx>::f.isAdvanced, "Complementary output only for advanced timers");

        if (pol != TimOCPol::NONE) {
            if constexpr (ch == TimCh::CH1) {
                ccer &= ~TIMx::_CCER::CC1NP::BitMsk;
                ccer |= (static_cast<uint32_t>(pol) << TIMx::_CCER::CC1NP::pos)
                    | TIMx::_CCER::CC1NE::BitMsk;
            }
            else if constexpr (ch == TimCh::CH2) {
                ccer &= ~TIMx::_CCER::CC2NP::BitMsk;
                ccer |= (static_cast<uint32_t>(pol) << TIMx::_CCER::CC2NP::pos)
                    | TIMx::_CCER::CC2NE::BitMsk;
            }
            else if constexpr (ch == TimCh::CH3) {
                ccer &= ~TIMx::_CCER::CC3NP::BitMsk;
                ccer |= (static_cast<uint32_t>(pol) << TIMx::_CCER::CC3NP::pos)
                    | TIMx::_CCER::CC3NE::BitMsk;
            }
        }
    }

    // Настройка idle state. Канал — параметр шаблона, поэтому выбор
    // OISx-поля делается через if constexpr (одна ветка на инстанс),
    // runtime остаётся только проверка "задан ли idle в конфиге".
    static inline void set_idle(uint32_t& cr2, TimIdleState s,
        TimIdleState sn) {
        if constexpr (TimFlags<TIMx>::f.isAdvanced) {
            const uint32_t val  = static_cast<uint32_t>(s);
            const uint32_t valn = static_cast<uint32_t>(sn);

            if constexpr (ch == TimCh::CH1) {
                if (s != TimIdleState::NONE)
                    cr2 = (cr2 & ~TIMx::_CR2::OIS1::BitMsk)  | (val  << TIMx::_CR2::OIS1::pos);
                if (sn != TimIdleState::NONE)
                    cr2 = (cr2 & ~TIMx::_CR2::OIS1N::BitMsk) | (valn << TIMx::_CR2::OIS1N::pos);
            }
            else if constexpr (ch == TimCh::CH2) {
                if (s != TimIdleState::NONE)
                    cr2 = (cr2 & ~TIMx::_CR2::OIS2::BitMsk)  | (val  << TIMx::_CR2::OIS2::pos);
                if (sn != TimIdleState::NONE)
                    cr2 = (cr2 & ~TIMx::_CR2::OIS2N::BitMsk) | (valn << TIMx::_CR2::OIS2N::pos);
            }
            else if constexpr (ch == TimCh::CH3) {
                if (s != TimIdleState::NONE)
                    cr2 = (cr2 & ~TIMx::_CR2::OIS3::BitMsk)  | (val  << TIMx::_CR2::OIS3::pos);
                if (sn != TimIdleState::NONE)
                    cr2 = (cr2 & ~TIMx::_CR2::OIS3N::BitMsk) | (valn << TIMx::_CR2::OIS3N::pos);
            }
            else if constexpr (ch == TimCh::CH4) {
                if (s != TimIdleState::NONE)
                    cr2 = (cr2 & ~TIMx::_CR2::OIS4::BitMsk)  | (val  << TIMx::_CR2::OIS4::pos);
            }
            else if constexpr (ch == TimCh::CH5) {
                if (s != TimIdleState::NONE)
                    cr2 = (cr2 & ~TIMx::_CR2::OIS5::BitMsk)  | (val  << TIMx::_CR2::OIS5::pos);
            }
            else if constexpr (ch == TimCh::CH6) {
                if (s != TimIdleState::NONE)
                    cr2 = (cr2 & ~TIMx::_CR2::OIS6::BitMsk)  | (val  << TIMx::_CR2::OIS6::pos);
            }
        }
    }
};

// ------------------- CONFIG FUNCTIONS -------------------
template<typename TIMx, TimBaseConfig cfg>
void TIM_BaseConfig() {

    RccEnable<TIMx>::enable();

    TIMx::_CR1::CEN::clear();

    uint32_t tmp = TIMx::_CR1::read();

    if constexpr (TimFlags<TIMx>::f.hasClkDiv) {
        tmp &= ~TIMx::_CR1::CKD::BitMsk;
        tmp |= static_cast<uint32_t>(cfg.ClockDiv) << TIMx::_CR1::CKD::pos;
    }

    tmp &= ~TIMx::_CR1::ARPE::BitMsk;
    if constexpr (cfg.EnablePreload) {
        tmp |= TIMx::_CR1::ARPE::BitMsk;
    }

    if constexpr (TimFlags<TIMx>::f.hasRepetition) {
        TIMx::_RCR::overwrite(cfg.RepetitionCounter);
    }

    tmp &= ~(TIMx::_CR1::DIR::BitMsk | TIMx::_CR1::CMS::BitMsk);
    tmp |= static_cast<uint32_t>(cfg.CounterMode);

    TIMx::_PSC::overwrite(cfg.Prescaler);
    TIMx::_ARR::overwrite(cfg.Period);
    TIMx::_CR1::overwrite(tmp);
    TIMx::_EGR::UG::set();
    TIMx::_SR::UIF::clear();
}

template<typename TIMx, TimOCConfig cfg, TimCh ch>
static inline void TIM_PWM_ConfigChannel() {
    using Traits = TimChannelTraits<TIMx, ch>;

    uint32_t tmpccer = TIMx::_CCER::read();
    uint32_t tmpccmr = Traits::CCMR::read();
    uint32_t tmpcr2 = TIMx::_CR2::read();

    // 1. Disable channel
    tmpccer &= ~Traits::enable_mask;

    // 2. Output Compare mode
    uint32_t mode = static_cast<uint32_t>(cfg.OCMode);
    tmpccmr &= ~(Traits::oc_mode_mask | Traits::preload_en_mask
        | Traits::fast_en_mask);
    tmpccmr |= ((mode & 0b111) << Traits::oc_mode_shift);
    if constexpr (TimFlags<TIMx>::f.hasOCM3) {
        tmpccmr |= ((mode >> 3) << (Traits::oc_mode_shift + 12));
    }

    // 3. Preload / Fast enable
    tmpccmr |= (cfg.OC_PreloadEnable ? Traits::preload_en_mask : 0)
        | (cfg.FastEnable ? Traits::fast_en_mask : 0);

    // 4. Polarity
    if (cfg.OCPolarity != TimOCPol::NONE) {
        tmpccer &= ~Traits::polarity_mask;
        if (cfg.OCPolarity == TimOCPol::LOW)
            tmpccer |= Traits::polarity_mask;
    }

    // 5. Complementary output (CH1–3, advanced timers)
    if constexpr ((ch == TimCh::CH1 || ch == TimCh::CH2 || ch == TimCh::CH3)
        && TimFlags<TIMx>::f.isAdvanced) {
        Traits::set_complementary(tmpccer, cfg.OCNPolarity);
    }

    // 6. Idle state / Break (только для advanced)
    Traits::set_idle(tmpcr2, cfg.OCIdleState, cfg.OCNIdleState);

    // 7. Write registers
    Traits::CCMR::overwrite(tmpccmr);
    TIMx::_CCER::overwrite(tmpccer);
    TIMx::_CR2::overwrite(tmpcr2);
    Traits::write_ccr(cfg.Pulse);
    TIMx::_EGR::UG::set();
    TIMx::_SR::UIF::clear();
}

template<typename TIMx, TimCh ch>
static inline void TIM_PWM_ChannelEnable() {

    using Traits = TimChannelTraits<TIMx, ch>;

    if constexpr (TimFlags<TIMx>::f.isAdvanced)
        TIMx::_BDTR::MOE::set();

    TIMx::_CCER::setMask(Traits::enable_mask);

    TIMx::_CR1::CEN::set();
}

template<typename TIMx, TimCh ch, auto& hdma>
static inline void TIM_PWM_ChannelDMAEnable(uint32_t pData, uint16_t Size) {
    using Traits = TimChannelTraits<TIMx, ch>;

    auto& h = hdma;
    if (h.state != DmaState::READY)
        return;

    //TIMx::_EGR::UG::set();
    TIMx::_SR::clear();
    TIMx::_DIER::setMask(Traits::dma_enable_mask);
    //TIMx::_DIER::UDE::set();

    if (DMA_StartIT<hdma>(pData, Traits::ccr_address, Size)
        != DrvStatus::Ok) {
        return;
    }


    TIMx::_CCER::setMask(Traits::enable_mask);

    if constexpr (TimFlags<TIMx>::f.isAdvanced)TIMx::_BDTR::MOE::set();

    TIMx::_CR1::CEN::set();

}

