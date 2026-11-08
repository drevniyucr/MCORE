#pragma once
// Timer clock enables and chip-only timers; included by drivers/tim.hpp inside the TimFlags list (STM32H755).
#include "core/regs.hpp"
#include "drivers/tim.hpp"

// ------------------- TIMER CLOCK -------------------
// STM32H7: APB1 timers are enabled in APB1LENR; TIM9..11 do not exist.
template<typename TIMx> inline void TIM_ClkEnable();
template<> inline void TIM_ClkEnable<TIM1>()  { RCC::_APB2ENR::TIM1EN::set(); }
template<> inline void TIM_ClkEnable<TIM2>()  { RCC::_APB1LENR::TIM2EN::set(); }
template<> inline void TIM_ClkEnable<TIM3>()  { RCC::_APB1LENR::TIM3EN::set(); }
template<> inline void TIM_ClkEnable<TIM4>()  { RCC::_APB1LENR::TIM4EN::set(); }
template<> inline void TIM_ClkEnable<TIM5>()  { RCC::_APB1LENR::TIM5EN::set(); }
template<> inline void TIM_ClkEnable<TIM6>()  { RCC::_APB1LENR::TIM6EN::set(); }
template<> inline void TIM_ClkEnable<TIM7>()  { RCC::_APB1LENR::TIM7EN::set(); }
template<> inline void TIM_ClkEnable<TIM8>()  { RCC::_APB2ENR::TIM8EN::set(); }
template<> inline void TIM_ClkEnable<TIM12>() { RCC::_APB1LENR::TIM12EN::set(); }
template<> inline void TIM_ClkEnable<TIM13>() { RCC::_APB1LENR::TIM13EN::set(); }
template<> inline void TIM_ClkEnable<TIM14>() { RCC::_APB1LENR::TIM14EN::set(); }
