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
    SCB::ICIALLU_Reg::write(0U);                /* invalidate I-Cache */
    // SCB->ICIALLU = 0UL;                     /* invalidate I-Cache */
    __DSB();
    __ISB();
    SCB::_CCR::IC::set();
    // SCB->CCR |=  (uint32_t)SCB_CCR_IC_Msk;  /* enable I-Cache */
    __DSB();
    __ISB();
}
inline static void NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)(0x07UL));             /* only values 0..7 are used          */
  
  reg_value = SCB::AIRCR_Reg::read();
  reg_value = (reg_value & ~(SCB::_AIRCR::VECTKEY::bitmsk | SCB::_AIRCR::PRIGROUP::bitmsk));
  reg_value |= (
    reinterpret_cast<uint32_t>(0x5FAUL << SCB::_AIRCR::VECTKEY::pos) |
    (PriorityGroupTmp << SCB::_AIRCR::PRIGROUP::pos));  
  SCB::AIRCR_Reg::write(reg_value);      
//   reg_value  =  SCB->AIRCR;                                                   /* read old register configuration    */
//   reg_value &= ~((uint32_t)(SCB_AIRCR_VECTKEY_Msk | SCB_AIRCR_PRIGROUP_Msk)); /* clear bits to change               */
//   reg_value  =  (reg_value                                   |
//                 ((uint32_t)0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |
//                 (PriorityGroupTmp << SCB_AIRCR_PRIGROUP_Pos)  );              /* Insert write key and priority group */
//   SCB->AIRCR =  reg_value;
}


