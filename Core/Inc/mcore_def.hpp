/*
 * mcore_def.hpp
 *
 *  Created on: Sep 25, 2025
 *      Author: AkimovMA
 */

#pragma once

#ifdef USE_CMSIS
#include <stm32f7xx.h>
#else
#include "mcore_regs.hpp"
#endif

inline static void __DSB(void) {
    __asm volatile ("dsb" ::: "memory");
}

inline static void __ISB(void) {
    __asm volatile ("isb" ::: "memory");
}

inline static void __DMB(void) {
    __asm volatile ("dmb" ::: "memory");
}

inline static void __NOP(void) {
    __asm volatile ("nop" ::: "memory");
}
inline static void SCB_EnableICache (void)
{
    __DSB();
    __ISB();
    Cache::_ICIALLU::clear();                /* invalidate I-Cache */
    // SCB->ICIALLU = 0UL;                     /* invalidate I-Cache */
    __DSB();
    __ISB();
    Control::_CCR::IC::set();
    // SCB->CCR |=  (uint32_t)SCB_CCR_IC_Msk;  /* enable I-Cache */
    __DSB();
    __ISB();
}

