<<<<<<< HEAD
/*
 * Copyright (c) 2025 YourName
 * Licensed under MIT License
 */
#pragma once

#include "mcore_def.hpp"

#define RCC_GPIOAEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;} while(0)
#define RCC_GPIOBEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;} while(0)
#define RCC_GPIOCEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;} while(0)
#define RCC_GPIODEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;} while(0)
#define RCC_GPIOEEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;} while(0)
#define RCC_GPIOFEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;} while(0)
#define RCC_GPIOGEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;} while(0)

#define RCC_TIM2EN()     do{RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;} while(0)
#define RCC_TIM3EN()     do{RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;} while(0)

#define RCC_GPIO_ALLEN() do{RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOAEN\
									      |RCC_AHB1ENR_GPIOBEN\
										  |RCC_AHB1ENR_GPIOCEN\
										  |RCC_AHB1ENR_GPIODEN\
										  |RCC_AHB1ENR_GPIOEEN\
										  |RCC_AHB1ENR_GPIOFEN\
										  |RCC_AHB1ENR_GPIOGEN;\
					  } while(0)

enum class PLL_P : uint8_t {
	Div2 = 0, Div4 = 1, Div6 = 2, Div8 = 3
};

enum class AHBPrescaler : uint16_t {
	Div1 = RCC_CFGR_HPRE_DIV1,
	Div2 = RCC_CFGR_HPRE_DIV2,
	Div4 = RCC_CFGR_HPRE_DIV4,
	Div8 = RCC_CFGR_HPRE_DIV8,
	Div16 = RCC_CFGR_HPRE_DIV16,
	Div64 = RCC_CFGR_HPRE_DIV64,
	Div128 = RCC_CFGR_HPRE_DIV128,
	Div256 = RCC_CFGR_HPRE_DIV256,
	Div512 = RCC_CFGR_HPRE_DIV512
};

enum class APBPrescaler : uint16_t {
	Div1 = RCC_CFGR_PPRE1_DIV1,
	Div2 = RCC_CFGR_PPRE1_DIV2,
	Div4 = RCC_CFGR_PPRE1_DIV4,
	Div8 = RCC_CFGR_PPRE1_DIV8,
	Div16 = RCC_CFGR_PPRE1_DIV16
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
	WS0 = FLASH_ACR_LATENCY_0WS,
	WS1 = FLASH_ACR_LATENCY_1WS,
	WS2 = FLASH_ACR_LATENCY_2WS,
	WS3 = FLASH_ACR_LATENCY_3WS,
	WS4 = FLASH_ACR_LATENCY_4WS,
	WS5 = FLASH_ACR_LATENCY_5WS,
	WS6 = FLASH_ACR_LATENCY_6WS,
	WS7 = FLASH_ACR_LATENCY_7WS
};

enum class VoltageScale : uint32_t {
	Scale1 = PWR_CR1_VOS, Scale2 = PWR_CR1_VOS_1, Scale3 = PWR_CR1_VOS_0
};

enum class RCCStatus {
	OK, HSE_FAILED, PLL_FAILED, PLL_CONFIG_INVALID
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
RCCStatus RCCInit(const ClockConfig &cfg);
// Расчёт итоговой частоты PLL
uint32_t CalculatePLLCLK(uint32_t inputFreq, uint8_t pllm, uint16_t plln,
		uint8_t pllp);
void enablePowerInterface(void);
void enableEthInterface(void);

=======
/*
 * Copyright (c) 2025 YourName
 * Licensed under MIT License
 */
#pragma once

#include "mcore_def.hpp"

#define RCC_GPIOAEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;} while(0)
#define RCC_GPIOBEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;} while(0)
#define RCC_GPIOCEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;} while(0)
#define RCC_GPIODEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;} while(0)
#define RCC_GPIOEEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;} while(0)
#define RCC_GPIOFEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;} while(0)
#define RCC_GPIOGEN()    do{RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;} while(0)

#define RCC_TIM2EN()     do{RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;} while(0)
#define RCC_TIM3EN()     do{RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;} while(0)

#define RCC_GPIO_ALLEN() do{RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOAEN\
									      |RCC_AHB1ENR_GPIOBEN\
										  |RCC_AHB1ENR_GPIOCEN\
										  |RCC_AHB1ENR_GPIODEN\
										  |RCC_AHB1ENR_GPIOEEN\
										  |RCC_AHB1ENR_GPIOFEN\
										  |RCC_AHB1ENR_GPIOGEN;\
					  } while(0)

enum class PLL_P : uint8_t {
	Div2 = 0, Div4 = 1, Div6 = 2, Div8 = 3
};

enum class AHBPrescaler : uint16_t {
	Div1 = RCC_CFGR_HPRE_DIV1,
	Div2 = RCC_CFGR_HPRE_DIV2,
	Div4 = RCC_CFGR_HPRE_DIV4,
	Div8 = RCC_CFGR_HPRE_DIV8,
	Div16 = RCC_CFGR_HPRE_DIV16,
	Div64 = RCC_CFGR_HPRE_DIV64,
	Div128 = RCC_CFGR_HPRE_DIV128,
	Div256 = RCC_CFGR_HPRE_DIV256,
	Div512 = RCC_CFGR_HPRE_DIV512
};

enum class APBPrescaler : uint16_t {
	Div1 = RCC_CFGR_PPRE1_DIV1,
	Div2 = RCC_CFGR_PPRE1_DIV2,
	Div4 = RCC_CFGR_PPRE1_DIV4,
	Div8 = RCC_CFGR_PPRE1_DIV8,
	Div16 = RCC_CFGR_PPRE1_DIV16
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
	WS0 = FLASH_ACR_LATENCY_0WS,
	WS1 = FLASH_ACR_LATENCY_1WS,
	WS2 = FLASH_ACR_LATENCY_2WS,
	WS3 = FLASH_ACR_LATENCY_3WS,
	WS4 = FLASH_ACR_LATENCY_4WS,
	WS5 = FLASH_ACR_LATENCY_5WS,
	WS6 = FLASH_ACR_LATENCY_6WS,
	WS7 = FLASH_ACR_LATENCY_7WS
};

enum class VoltageScale : uint32_t {
	Scale1 = PWR_CR1_VOS, Scale2 = PWR_CR1_VOS_1, Scale3 = PWR_CR1_VOS_0
};

enum class RCCStatus {
	OK, HSE_FAILED, PLL_FAILED, PLL_CONFIG_INVALID
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
RCCStatus RCCInit(const ClockConfig &cfg);
// Расчёт итоговой частоты PLL
uint32_t CalculatePLLCLK(uint32_t inputFreq, uint8_t pllm, uint16_t plln,
		uint8_t pllp);
void enablePowerInterface(void);
void enableEthInterface(void);

>>>>>>> 7688bfa (update)
