/*
 * mcore_system.hpp
 *
 *  Created on: Jul 13, 2025
 *      Author: doomt
 */

#pragma once

#include "mcore_def.hpp"

extern uint32_t SystemCoreClock;
extern const uint8_t AHBPrescTable[16];
extern const uint8_t APBPrescTable[8];

void SystemCoreClockUpdate(void);
void delay_ms(uint32_t ms);
uint32_t get_tick(void);
void SystemInit(void);
uint32_t SysTick_Config(uint32_t ticks);
