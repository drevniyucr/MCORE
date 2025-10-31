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
#include "mcore_utils.hpp"

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


#endif
