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

[[gnu::always_inline]] 
inline static void __DSB(void) {
    __asm volatile ("dsb 0xF" ::: "memory");
}

[[gnu::always_inline]] 
inline static void __ISB(void) {
    __asm volatile ("isb 0xF" ::: "memory");
}

[[gnu::always_inline]] 
inline static void __DMB(void) {
    __asm volatile ("dmb 0xF" ::: "memory");
}

[[gnu::always_inline]] 
inline static void __NOP(void) {
    __asm volatile ("nop" ::: "memory");
}
[[gnu::always_inline]] 
inline static void SCB_EnableICache (void)                             
{
    __DSB();
    __ISB();
    Cache::_ICIALLU::clear();                /* invalidate I-Cache */
    __DSB();
    __ISB();
    SCB::_CCR::IC::set();
    __DSB();
    __ISB();
}

[[gnu::always_inline]] 
inline static void SCB_EnableDCache (void)
{
    uint32_t sets,ways;
 
    Cache::_CSSELR::clear();
    __DSB();
    sets = Cache::_CCSIDR::NS::read();
    do {
        ways = Cache::_CCSIDR::A::read();
        do{
            Cache::_DCISW::overwrite(
                ((sets << Cache::_DCISW::SET::pos) & Cache::_DCISW::SET::BitMsk)|
                ((ways << Cache::_DCISW::WAY::pos) & Cache::_DCISW::WAY::BitMsk)
            );
    }while (ways-- != 0U);
}while (sets-- != 0U);
   __DSB();
   SCB::_CCR::DC::set();
   __DSB();
   __ISB();
}

