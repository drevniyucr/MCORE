/*
 * Copyright (c) 2025 YourName
 * Licensed under MIT License
 */

#include "mcore_rcc.hpp"



constexpr uint32_t HSE_TIMEOUT = 0x1000U;
constexpr uint32_t PLL_TIMEOUT = 0x1000U;
constexpr uint32_t MS_DIV = 1000U;


RCCStatus RCCInit(const ClockConfig* cfg) {

	const uint8_t pll_p = static_cast<uint8_t>(cfg->PLLP);
	const uint32_t ahb_div = static_cast<uint32_t>(cfg->AHBDiv);
	const uint32_t apb1_div = static_cast<uint32_t>(cfg->APB1Div);
	const uint32_t apb2_div = static_cast<uint32_t>(cfg->APB2Div);
	const uint8_t flash_ws = static_cast<uint8_t>(cfg->FLASHLatency);

	// 1. Проверка PLL параметров (валидные диапазоны по Reference Manual)
	if (cfg->PLLM < 2 || cfg->PLLM > 63 || cfg->PLLN < 50 || cfg->PLLN > 432) {
		return RCCStatus::PLL_CONFIG_INVALID;
	}

	// Enable voltage scale 1 (set VOS bits to required value)
	PWR::_CR1::VOS::modify(static_cast<uint32_t>(VoltageScale::Scale1));
//	PWR->CR1 = (PWR->CR1 & ~PWR_CR1_VOS) | static_cast<uint32_t>(VoltageScale::Scale1);

	// 2. Включение HSE (если нужно)
	if (cfg->useHSE) {
		if (cfg->useHSEBypass) {
			RCC::_CR::HSEBYP::set();
			//RCC->CR |= RCC_CR_HSEBYP;
		}
		RCC::_CR::HSEON::set();
		//RCC->CR |= RCC_CR_HSEON;
		uint32_t i = 0;
		while (!(RCC::_CR::HSERDY::read())) {
	//	while (!(RCC->CR & RCC_CR_HSERDY)) {
			if (++i > HSE_TIMEOUT) {
				RCC::_CR::HSEON::clear();
				//RCC->CR &= ~RCC_CR_HSEON;
				return RCCStatus::HSE_FAILED;
			}
		}
	}

	// 3. Настройка PLL
	RCC::_PLLCFGR::overwrite(
		  (static_cast<uint32_t>(cfg->PLLM) << RCC::_PLLCFGR::PLLM0::pos)
		| (static_cast<uint32_t>(cfg->PLLN) << RCC::_PLLCFGR::PLLN0::pos)
		| (static_cast<uint32_t>(pll_p) << RCC::_PLLCFGR::PLLP0::pos)
		| (cfg->useHSE ? RCC::_PLLCFGR::PLLSRC::bitmsk : 0U)
		);

	// RCC->PLLCFGR = (static_cast<uint32_t>(cfg.PLLM) << RCC_PLLCFGR_PLLM_Pos)
	// 		| (static_cast<uint32_t>(cfg.PLLN) << RCC_PLLCFGR_PLLN_Pos)
	// 		| (static_cast<uint32_t>(pll_p) << RCC_PLLCFGR_PLLP_Pos)
	// 		| (cfg.useHSE ? RCC_PLLCFGR_PLLSRC_HSE : RCC_PLLCFGR_PLLSRC_HSI);

	// 4. Включение PLL и ожидание готовности
	RCC::_CR::PLLON::set();
	//RCC->CR |= RCC_CR_PLLON;
	uint32_t i = 0;
	while (!(RCC::_CR::PLLRDY::read())) {
	//while (!(RCC->CR & RCC_CR_PLLRDY)) {
		if (++i > PLL_TIMEOUT) {
			RCC::_CR::PLLON::clear();
			//RCC->CR &= ~RCC_CR_PLLON;
			if (cfg->useHSE) RCC::_CR::HSEON::clear();
			//if (cfg->useHSE) RCC->CR &= ~RCC_CR_HSEON;
			return RCCStatus::PLL_FAILED;
		}
	}

	// ENABLE OVER-DRIVE
	PWR::_CR1::ODEN::set();
	// PWR->CR1 |= PWR_CR1_ODEN;
	i = 0;
	while (!(PWR::_CSR1::ODRDY::read())) {
	//while (!(PWR->CSR1 & PWR_CSR1_ODRDY)) {
		if (++i > PLL_TIMEOUT) return RCCStatus::PLL_FAILED;
	}
	PWR::_CR1::ODSWEN::set();
	// PWR->CR1 |= PWR_CR1_ODSWEN;
	i = 0;
	while (!(PWR::_CSR1::ODSWRDY::read())) {
	// while (!(PWR->CSR1 & PWR_CSR1_ODSWRDY)) {
		if (++i > 200000U) return RCCStatus::PLL_FAILED;
	}

	// 5. Настройка Flash latency
	Flash::_ACR::overwrite(flash_ws);
	//FLASH->ACR = flash_ws;
	// 6. Настройка делителей шин (обновляем только нужные биты)
	RCC::_CFGR::clearMask(RCC::_CFGR::HPRE::bitmsk | RCC::_CFGR::PPRE1::bitmsk | RCC::_CFGR::PPRE2::bitmsk);
	RCC::_CFGR::setMask(ahb_div | apb1_div | apb2_div);
	// RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2))
	// 			| ahb_div | apb1_div | apb2_div;
	// 7. Переключение SYSCLK на PLL
	///!!!!!!RCC::_CFGR::setMask(RCC::_CFGR::SW::bitmsk);
	RCC::_CFGR::SW1::set();
	///!!!!!!while (RCC::_CFGR::SWS::read() != 0x02);
	// RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
	// while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);

	// 8. Обновление SystemCoreClock
	//SystemCoreClockUpdate();

	// 9. Настройка SysTick
	if (cfg->useSysTick) {
		//SysTick_Config(SystemCoreClock / MS_DIV);
	}

	// 10. Выключение HSI (если не нужен)
	if (cfg->useHSE) RCC::_CR::HSION::read();
//	RCC->CR &= ~RCC_CR_HSION;

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
	RCC::_APB1ENR::PWREN::set();
	//RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	tmpreg = RCC::_AHB1ENR::read();
	(void) tmpreg;
}

void enableEthInterface(void) {
	uint32_t tmpreg;
	RCC::_AHB1ENR::setMask(
			RCC::_AHB1ENR::ETHMACEN::bitmsk
		  | RCC::_AHB1ENR::ETHMACTXEN::bitmsk
		  | RCC::_AHB1ENR::ETHMACRXEN::bitmsk);
	// RCC->AHB1ENR |=
	// 		RCC_AHB1ENR_ETHMACEN
	// 		| RCC_AHB1ENR_ETHMACTXEN
	// 		| RCC_AHB1ENR_ETHMACRXEN;
	tmpreg = RCC::_AHB1ENR::read();
	(void) tmpreg;
}
