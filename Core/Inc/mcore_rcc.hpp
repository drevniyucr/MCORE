/*
 * Copyright (c) 2025 YourName
 * Licensed under MIT License
 */
#pragma once

#include "mcore_def.hpp"
// Replace old macros with inline accessor functions to enable type safety and easier debugging
static inline void RCC_GPIOAEN() { RCC::_AHB1ENR::GPIOAEN::set(); }
static inline void RCC_GPIOBEN() { RCC::_AHB1ENR::GPIOBEN::set(); }
static inline void RCC_GPIOCEN() { RCC::_AHB1ENR::GPIOCEN::set(); }
static inline void RCC_GPIODEN() { RCC::_AHB1ENR::GPIODEN::set(); }
static inline void RCC_GPIOEEN() { RCC::_AHB1ENR::GPIOEEN::set(); }
static inline void RCC_GPIOFEN() { RCC::_AHB1ENR::GPIOFEN::set(); }
static inline void RCC_GPIOGEN() { RCC::_AHB1ENR::GPIOGEN::set(); }

static inline void RCC_TIM2EN()  { RCC::_APB1ENR::TIM2EN::set(); }
static inline void RCC_TIM3EN()  { RCC::_APB1ENR::TIM3EN::set(); }

static inline void RCC_GPIO_ALLEN() { 
	RCC::_AHB1ENR::setMask(
		RCC::_AHB1ENR::GPIOAEN::bitmsk |
		RCC::_AHB1ENR::GPIOBEN::bitmsk |
		RCC::_AHB1ENR::GPIOCEN::bitmsk |
		RCC::_AHB1ENR::GPIODEN::bitmsk |
		RCC::_AHB1ENR::GPIOEEN::bitmsk |
		RCC::_AHB1ENR::GPIOFEN::bitmsk |
		RCC::_AHB1ENR::GPIOGEN::bitmsk 
	);
}

enum class PLL_P : uint8_t {
	Div2 = 0, Div4 = 1, Div6 = 2, Div8 = 3
};


enum class AHBPrescaler : uint16_t {
	Div1   = 0x00,
	Div2   = 0x80,
	Div4   = 0x90,
	Div8   = 0xA0,
	Div16  = 0xB0,
	Div64  = 0xC0,
	Div128 = 0xD0,
	Div256 = 0xE0,
	Div512 = 0xF0
};

enum class APBPrescaler : uint16_t {
	Div1  = 0x0000,
	Div2  = 0x1000,
	Div4  = 0x1400,
	Div8  = 0x1800,
	Div16 = 0x1C00
};
/*
 +----------------------+--------------------+--------------------+--------------------+--------------------+
 | Wait states (WS)     |   2.7V – 3.6V      |   2.4V – 2.7V      |   2.1V – 2.4V      |   1.8V – 2.1V      |
 | (LATENCY)            |   HCLK (MHz)       |   HCLK (MHz)       |   HCLK (MHz)       |   HCLK (MHz)       |
 +----------------------+--------------------+--------------------+--------------------+--------------------+
 | 0 WS (1 CPU cycle)   |   0 < HCLK ≤ 30    |   0 < HCLK ≤ 24    |   0 < HCLK ≤ 22    |   0 < HCLK ≤ 20    |
 | 1 WS (2 CPU cycles)  |  30 < HCLK ≤ 60    |  24 < HCLK ≤ 48    |  22 < HCLK ≤ 44    |  20 < HCLK ≤ 40    |
 | 2 WS (3 CPU cycles)  |  60 < HCLK ≤ 90    |  48 < HCLK ≤ 72    |  44 < HCLK ≤ 66    |  40 < HCLK ≤ 60    |
 | 3 WS (4 CPU cycles)  |  90 < HCLK ≤ 120   |  72 < HCLK ≤ 96    |  66 < HCLK ≤ 88    |  60 < HCLK ≤ 80    |
 | 4 WS (5 CPU cycles)  | 120 < HCLK ≤ 150   |  96 < HCLK ≤ 120   |  88 < HCLK ≤ 110   |  80 < HCLK ≤ 100   |
 | 5 WS (6 CPU cycles)  | 150 < HCLK ≤ 180   | 120 < HCLK ≤ 144   | 110 < HCLK ≤ 132   | 100 < HCLK ≤ 120   |
 | 6 WS (7 CPU cycles)  | 180 < HCLK ≤ 210   | 144 < HCLK ≤ 168   | 132 < HCLK ≤ 154   | 120 < HCLK ≤ 140   |
 | 7 WS (8 CPU cycles)  | 210 < HCLK ≤ 216   | 168 < HCLK ≤ 192   | 154 < HCLK ≤ 176   | 140 < HCLK ≤ 160   |
 | 8 WS (9 CPU cycles)  |        -           | 192 < HCLK ≤ 216   | 176 < HCLK ≤ 198   | 160 < HCLK ≤ 180   |
 | 9 WS (10 CPU cycles) |        -           |        -           | 198 < HCLK ≤ 216   |        -           |
 +----------------------+--------------------+--------------------+--------------------+--------------------+
 */

enum class FLASH_Latency : uint8_t {
	WS0 = 0x00,
	WS1 = 0x01,
	WS2 = 0x02,
	WS3 = 0x03,
	WS4 = 0x04,
	WS5 = 0x05,
	WS6 = 0x06,
	WS7 = 0x07
};

enum class VoltageScale : uint32_t {
	 Scale1 = 0x03,
	 Scale2 = 0x02, 
	 Scale3 = 0x01
};

enum class RCCStatus {
	OK, 
	HSE_FAILED, 
	PLL_FAILED, 
	PLL_CONFIG_INVALID
};

struct ClockConfig {
	uint8_t PLLM;     // делитель HSE → PLL вход [2..63]
	uint16_t PLLN;    // множитель PLL [50..432]
	PLL_P PLLP;       // делитель PLL → SYSCLK
	AHBPrescaler AHBDiv;    // AHB делитель
	APBPrescaler APB1Div;   // APB1 делитель
	APBPrescaler APB2Div;   // APB2 делитель
	FLASH_Latency FLASHLatency;
	bool useHSE;
	bool useHSEBypass;
	bool useSysTick = true;
};
// Основная инициализация тактирования
RCCStatus RCCInit(const ClockConfig* cfg);
// Расчёт итоговой частоты PLL
uint32_t CalculatePLLCLK(uint32_t inputFreq, uint8_t pllm, uint16_t plln,
		uint8_t pllp);
void enablePowerInterface(void);
void enableEthInterface(void);

