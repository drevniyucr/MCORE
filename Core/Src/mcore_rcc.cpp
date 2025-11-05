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
	PWR::_CR1::VOS::write<VoltageScale::Scale1>();

	// 2. Включение HSE (если нужно)
	if (cfg->useHSE) {
		if (cfg->useHSEBypass) {
			RCC::_CR::HSEBYP::set();
		}
		RCC::_CR::HSEON::set();
		uint32_t i = 0;
		while (!(RCC::_CR::HSERDY::read())) {
			if (++i > HSE_TIMEOUT) {
				RCC::_CR::HSEON::clear();
				return RCCStatus::HSE_FAILED;
			}
		}
	}

	// 3. Настройка PLL

    RCC::_CR::PLLON::clear();
    uint32_t i = 0;
	while ((RCC::_CR::PLLRDY::read())) {
		if (++i > PLL_TIMEOUT) {
            RCC::_CR::bitReset<RCC::_CR::HSEON, RCC::_CR::PLLON>();
            return RCCStatus::PLL_FAILED;
		}
	}
    RCC::_PLLCFGR::clearMask(
		  (RCC::_PLLCFGR::PLLM::BitMsk)
		| (RCC::_PLLCFGR::PLLN::BitMsk)
		| (RCC::_PLLCFGR::PLLP::BitMsk)
		| (RCC::_PLLCFGR::PLLSRC::BitMsk)
		);

	RCC::_PLLCFGR::setMask(
		  (static_cast<uint32_t>(cfg->PLLM) << RCC::_PLLCFGR::PLLM::pos)
		| (static_cast<uint32_t>(cfg->PLLN) << RCC::_PLLCFGR::PLLN::pos)
		| (static_cast<uint32_t>(pll_p) << RCC::_PLLCFGR::PLLP::pos)
		| (cfg->useHSE ? RCC::_PLLCFGR::PLLSRC::BitMsk : 0U)
		);

	// 4. Включение PLL и ожидание готовности
	RCC::_CR::PLLON::set();
	 i = 0;
	while (!(RCC::_CR::PLLRDY::read())) {
		if (++i > PLL_TIMEOUT) {
            RCC::_CR::bitReset<RCC::_CR::HSEON, RCC::_CR::PLLON>();
            return RCCStatus::PLL_FAILED;
		}
	}

	// ENABLE OVER-DRIVE
	PWR::_CR1::ODEN::set();
	i = 0;
	while (!(PWR::_CSR1::ODRDY::read())) {
		if (++i > PLL_TIMEOUT) return RCCStatus::PLL_FAILED;
	}
	PWR::_CR1::ODSWEN::set();
	i = 0;
	while (!(PWR::_CSR1::ODSWRDY::read())) {
		if (++i > 200000U) return RCCStatus::PLL_FAILED;
	}

	// 5. Настройка Flash latency
	 Flash::_ACR::overwrite(flash_ws);
	// 6. Настройка делителей шин (обновляем только нужные биты)
	RCC::_CFGR::clearMask<(
        RCC::_CFGR::HPRE::BitMsk | 
        RCC::_CFGR::PPRE1::BitMsk | 
        RCC::_CFGR::PPRE2::BitMsk
    )>();
	RCC::_CFGR::setMask(
        (ahb_div << RCC::_CFGR::HPRE::pos) | 
        (apb1_div << RCC::_CFGR::PPRE1::pos) | 
        (apb2_div << RCC::_CFGR::PPRE2::pos));
	// 7. Переключение SYSCLK на PLL
	RCC::_CFGR::SW::write<2U>();
	while (RCC::_CFGR::SWS::read() != 2U) {
        __NOP();
    };

	// 8. Обновление SystemCoreClock
	//SystemCoreClockUpdate();

	// 9. Настройка SysTick
	if (cfg->useSysTick) {
		//SysTick_Config(SystemCoreClock / MS_DIV);
	}

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
	tmpreg = RCC::_AHB1ENR::read();
	(void) tmpreg;
}

void enableEthInterface(void) {
	uint32_t tmpreg;
	RCC::_AHB1ENR::setMask
		  < RCC::_AHB1ENR::ETHMACEN::BitMsk
		  | RCC::_AHB1ENR::ETHMACTXEN::BitMsk
		  | RCC::_AHB1ENR::ETHMACRXEN::BitMsk>();
	// RCC->AHB1ENR |=
	// 		RCC_AHB1ENR_ETHMACEN
	// 		| RCC_AHB1ENR_ETHMACTXEN
	// 		| RCC_AHB1ENR_ETHMACRXEN;
	tmpreg = RCC::_AHB1ENR::read();
	(void) tmpreg;
}
