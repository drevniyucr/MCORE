#pragma once
// Timer clock enables and chip-only timers; included by drivers/tim.hpp inside the TimFlags list (STM32F767).
#include "core/regs.hpp"
#include "drivers/tim.hpp"

// ------------------- TIMER CLOCK -------------------
template<typename TIMx> inline void TIM_ClkEnable();
template<> inline void TIM_ClkEnable<TIM1>() {
    RCC::_APB2ENR::TIM1EN::set();
}
template<> inline void TIM_ClkEnable<TIM2>() {
    RCC::_APB1ENR::TIM2EN::set();
}
template<> inline void TIM_ClkEnable<TIM3>() {
    RCC::_APB1ENR::TIM3EN::set();
}
template<> inline void TIM_ClkEnable<TIM4>() {
    RCC::_APB1ENR::TIM4EN::set();
}
template<> inline void TIM_ClkEnable<TIM5>() {
    RCC::_APB1ENR::TIM5EN::set();
}
template<> inline void TIM_ClkEnable<TIM6>() {
    RCC::_APB1ENR::TIM6EN::set();
}
template<> inline void TIM_ClkEnable<TIM7>() {
    RCC::_APB1ENR::TIM7EN::set();
}
template<> inline void TIM_ClkEnable<TIM8>() {
    RCC::_APB2ENR::TIM8EN::set();
}
template<> inline void TIM_ClkEnable<TIM9>() {
    RCC::_APB2ENR::TIM9EN::set();
}
template<> inline void TIM_ClkEnable<TIM10>() {
    RCC::_APB2ENR::TIM10EN::set();
}
template<> inline void TIM_ClkEnable<TIM11>() {
    RCC::_APB2ENR::TIM11EN::set();
}
template<> inline void TIM_ClkEnable<TIM12>() {
    RCC::_APB1ENR::TIM12EN::set();
}
template<> inline void TIM_ClkEnable<TIM13>() {
    RCC::_APB1ENR::TIM13EN::set();
}
template<> inline void TIM_ClkEnable<TIM14>() {
    RCC::_APB1ENR::TIM14EN::set();
}


// Timers that exist only on this chip
template<> struct TimFlags<TIM9> {
    static constexpr TIM_properties f{ false, true, false, true, TimCh::CH2 };
};
template<> struct TimFlags<TIM10> {
    static constexpr TIM_properties f{ false, false, false, true, TimCh::CH1 };
};
template<> struct TimFlags<TIM11> {
    static constexpr TIM_properties f{ false, false, false, true, TimCh::CH1 };
};
