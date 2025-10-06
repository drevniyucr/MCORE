<<<<<<< HEAD
/*
 * Copyright (c) 2025 YourName
 * Licensed under MIT License
 */

#include "mcore_rcc.hpp"


#define HSE_TIMEOUT 0x1000
#define PLL_TIMEOUT 0x1000
#define MS_DIV 1000U

RCCStatus RCCInit(const ClockConfig &cfg) {

	const uint8_t pll_p = static_cast<uint8_t>(cfg.PLLP);
	const uint16_t ahb_div = static_cast<uint16_t>(cfg.AHBDiv);
	const uint16_t apb1_div = static_cast<uint16_t>(cfg.APB1Div);
	const uint16_t apb2_div = static_cast<uint16_t>(cfg.APB2Div);
	const uint8_t flash_ws = static_cast<uint8_t>(cfg.FLASHLatency);
	uint32_t tmpreg;

	// 1. Проверка PLL параметров (валидные диапазоны по Reference Manual)
	if ((cfg.PLLM < 2 || cfg.PLLM > 63) && (cfg.PLLN < 50 || cfg.PLLN > 432)) {
		return RCCStatus::PLL_CONFIG_INVALID;
	}
	// Enable voltage scale 1
	tmpreg = (PWR->CR1 & (~PWR_CR1_VOS | PWR_CR1_VOS));
	PWR->CR1 = tmpreg;
	tmpreg = PWR->CR1;

	if (cfg.useHSE) {
		// 2. Включение HSE
		if (cfg.useHSEBypass) {
			RCC->CR |= RCC_CR_HSEBYP;
		}
		RCC->CR |= RCC_CR_HSEON;

		for (uint32_t i = 0; !(RCC->CR & RCC_CR_HSERDY); ++i) {
			if (i > HSE_TIMEOUT) {
				RCC->CR &= ~RCC_CR_HSEON;
				return RCCStatus::HSE_FAILED;
			}
		}
	}
	// 3. Настройка PLL
	RCC->PLLCFGR = (cfg.PLLM << RCC_PLLCFGR_PLLM_Pos)
			| (cfg.PLLN << RCC_PLLCFGR_PLLN_Pos)
			| (pll_p << RCC_PLLCFGR_PLLP_Pos)
			| (cfg.useHSE ? RCC_PLLCFGR_PLLSRC_HSE : RCC_PLLCFGR_PLLSRC_HSI);
	// 4. Включение PLL
	RCC->CR |= RCC_CR_PLLON;
	for (uint32_t i = 0; !(RCC->CR & RCC_CR_PLLRDY); ++i) {
		if (i > PLL_TIMEOUT) {
			RCC->CR &= ~RCC_CR_PLLON;
			if (cfg.useHSE)
				RCC->CR &= ~RCC_CR_HSEON;
			return RCCStatus::PLL_FAILED;
		}
	}
	//ENABLE OVER-DRIVE
	PWR->CR1 |= PWR_CR1_ODEN;
	for (uint32_t i = 0; !(PWR->CSR1 & PWR_CSR1_ODRDY); ++i) {
		if (i > PLL_TIMEOUT) {
			return RCCStatus::PLL_FAILED;
		}
	}
	PWR->CR1 |= PWR_CR1_ODSWEN;
	for (uint32_t i = 0; !(PWR->CSR1 & PWR_CSR1_ODSWRDY); ++i) {
		if (i > 200000) {
			return RCCStatus::PLL_FAILED;
		}
	}
	// 5. Настройка Flash latency
	FLASH->ACR = flash_ws;
	// 6. Настройка делителей шин
	RCC->CFGR |= ahb_div | apb1_div | apb2_div;
	// 7. Переключение SYSCLK на PLL
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL)
		;
	// 8. Обновление SystemCoreClock
	SystemCoreClockUpdate();
	// 9. Настройка SysTick
	if (cfg.useSysTick) {
		SysTick_Config(SystemCoreClock / MS_DIV);
	}
	// 10. Выключение HSI (если не нужен)
	if (cfg.useHSE)
		RCC->CR &= ~RCC_CR_HSION;
	// 11. Расчет итоговой частоты (если запрошено)
	return RCCStatus::OK;
}

uint32_t CalculatePLLCLK(uint32_t inputFreq, uint8_t pllm, uint16_t plln,
		uint8_t pllp) {
	if (pllm == 0 || pllp == 0)
		return 0;  // Защита от деления на 0

	const uint32_t vco = (inputFreq / pllm) * plln;
	return vco / pllp;
}

void enablePowerInterface(void) {
	uint32_t tmpreg;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	tmpreg = RCC->APB1ENR;
	(void) tmpreg;
}

void enableEthInterface(void) {
	uint32_t tmpreg;
	RCC->AHB1ENR |=
			RCC_AHB1ENR_ETHMACEN
			| RCC_AHB1ENR_ETHMACTXEN
			| RCC_AHB1ENR_ETHMACRXEN;
	tmpreg = RCC->APB1ENR;
	(void) tmpreg;
}
=======
/*
 * Copyright (c) 2025 YourName
 * Licensed under MIT License
 */

#include "mcore_rcc.hpp"


#define HSE_TIMEOUT 0x1000
#define PLL_TIMEOUT 0x1000
#define MS_DIV 1000U

RCCStatus RCCInit(const ClockConfig &cfg) {

	const uint8_t pll_p = static_cast<uint8_t>(cfg.PLLP);
	const uint16_t ahb_div = static_cast<uint16_t>(cfg.AHBDiv);
	const uint16_t apb1_div = static_cast<uint16_t>(cfg.APB1Div);
	const uint16_t apb2_div = static_cast<uint16_t>(cfg.APB2Div);
	const uint8_t flash_ws = static_cast<uint8_t>(cfg.FLASHLatency);
	uint32_t tmpreg;

	// 1. Проверка PLL параметров (валидные диапазоны по Reference Manual)
	if ((cfg.PLLM < 2 || cfg.PLLM > 63) && (cfg.PLLN < 50 || cfg.PLLN > 432)) {
		return RCCStatus::PLL_CONFIG_INVALID;
	}
	// Enable voltage scale 1
	tmpreg = (PWR->CR1 & (~PWR_CR1_VOS | PWR_CR1_VOS));
	PWR->CR1 = tmpreg;
	tmpreg = PWR->CR1;

	if (cfg.useHSE) {
		// 2. Включение HSE
		if (cfg.useHSEBypass) {
			RCC->CR |= RCC_CR_HSEBYP;
		}
		RCC->CR |= RCC_CR_HSEON;

		for (uint32_t i = 0; !(RCC->CR & RCC_CR_HSERDY); ++i) {
			if (i > HSE_TIMEOUT) {
				RCC->CR &= ~RCC_CR_HSEON;
				return RCCStatus::HSE_FAILED;
			}
		}
	}
	// 3. Настройка PLL
	RCC->PLLCFGR = (cfg.PLLM << RCC_PLLCFGR_PLLM_Pos)
			| (cfg.PLLN << RCC_PLLCFGR_PLLN_Pos)
			| (pll_p << RCC_PLLCFGR_PLLP_Pos)
			| (cfg.useHSE ? RCC_PLLCFGR_PLLSRC_HSE : RCC_PLLCFGR_PLLSRC_HSI);
	// 4. Включение PLL
	RCC->CR |= RCC_CR_PLLON;
	for (uint32_t i = 0; !(RCC->CR & RCC_CR_PLLRDY); ++i) {
		if (i > PLL_TIMEOUT) {
			RCC->CR &= ~RCC_CR_PLLON;
			if (cfg.useHSE)
				RCC->CR &= ~RCC_CR_HSEON;
			return RCCStatus::PLL_FAILED;
		}
	}
	//ENABLE OVER-DRIVE
	PWR->CR1 |= PWR_CR1_ODEN;
	for (uint32_t i = 0; !(PWR->CSR1 & PWR_CSR1_ODRDY); ++i) {
		if (i > PLL_TIMEOUT) {
			return RCCStatus::PLL_FAILED;
		}
	}
	PWR->CR1 |= PWR_CR1_ODSWEN;
	for (uint32_t i = 0; !(PWR->CSR1 & PWR_CSR1_ODSWRDY); ++i) {
		if (i > 200000) {
			return RCCStatus::PLL_FAILED;
		}
	}
	// 5. Настройка Flash latency
	FLASH->ACR = flash_ws;
	// 6. Настройка делителей шин
	RCC->CFGR |= ahb_div | apb1_div | apb2_div;
	// 7. Переключение SYSCLK на PLL
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL)
		;
	// 8. Обновление SystemCoreClock
	SystemCoreClockUpdate();
	// 9. Настройка SysTick
	if (cfg.useSysTick) {
		SysTick_Config(SystemCoreClock / MS_DIV);
	}
	// 10. Выключение HSI (если не нужен)
	if (cfg.useHSE)
		RCC->CR &= ~RCC_CR_HSION;
	// 11. Расчет итоговой частоты (если запрошено)
	return RCCStatus::OK;
}

uint32_t CalculatePLLCLK(uint32_t inputFreq, uint8_t pllm, uint16_t plln,
		uint8_t pllp) {
	if (pllm == 0 || pllp == 0)
		return 0;  // Защита от деления на 0

	const uint32_t vco = (inputFreq / pllm) * plln;
	return vco / pllp;
}

void enablePowerInterface(void) {
	uint32_t tmpreg;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	tmpreg = RCC->APB1ENR;
	(void) tmpreg;
}

void enableEthInterface(void) {
	uint32_t tmpreg;
	RCC->AHB1ENR |=
			RCC_AHB1ENR_ETHMACEN
			| RCC_AHB1ENR_ETHMACTXEN
			| RCC_AHB1ENR_ETHMACRXEN;
	tmpreg = RCC->APB1ENR;
	(void) tmpreg;
}
>>>>>>> 7688bfa (update)
