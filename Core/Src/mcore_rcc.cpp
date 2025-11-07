/*
 * Copyright (c) 2025 YourName
 * Licensed under MIT License
 */

#include "mcore_rcc.hpp"
#include "mcore_system.hpp"

constexpr uint32_t OVERDRIVE_TIMEOUT = 0x5000;
constexpr uint32_t CLOCK_SW_TIMEOUT = 0x5000;
constexpr uint32_t HSE_TIMEOUT = 0x1000;
constexpr uint32_t PLL_TIMEOUT = 0x1000;
constexpr uint32_t MS_DIV = 1000U;


RCCStatus OSCInit(const ClockConfig* cfg) {	

	// 1. Проверка PLL параметров (валидные диапазоны по Reference Manual)
	if (cfg->PLLM < 2 || cfg->PLLM > 63 || cfg->PLLN < 50 || cfg->PLLN > 432) {
		return RCCStatus::PLL_CONFIG_INVALID;
	}

	// 2. Включение HSE (если нужно)
	if (cfg->useHSE) {
		if (cfg->useHSEBypass) {
			RCC::_CR::HSEBYP::set();
		}
		RCC::_CR::HSEON::set();

		if(waitUntil(RCC::_CR::HSERDY::read(),HSE_TIMEOUT)){
			RCC::_CR::HSEON::clear();
			return RCCStatus::HSE_FAILED;
		}
	}

	// 3. Настройка PLL
    RCC::_CR::PLLON::clear();
	if(waitUntil(!(RCC::_CR::PLLRDY::read()), PLL_TIMEOUT)){
		RCC::_CR::bitReset<RCC::_CR::HSEON, RCC::_CR::PLLON>();
		return RCCStatus::PLL_FAILED;
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
		| (static_cast<uint32_t>(cfg->PLLP) << RCC::_PLLCFGR::PLLP::pos)
		| (cfg->useHSE ? RCC::_PLLCFGR::PLLSRC::BitMsk : 0U)
		);

	// 4. Включение PLL и ожидание готовности
	RCC::_CR::PLLON::set();
	if(waitUntil(!(RCC::_CR::PLLRDY::read()), PLL_TIMEOUT)){
		RCC::_CR::bitReset<RCC::_CR::HSEON, RCC::_CR::PLLON>();
		return RCCStatus::PLL_FAILED;
	}
	
	uint32_t tempreg =  RCC::_PLLCFGR::read();
	uint32_t pll_p = static_cast<uint32_t>(cfg->PLLP);

	if ( 
		(((tempreg & RCC::_PLLCFGR::PLLM::BitMsk)
		>> RCC::_PLLCFGR::PLLM::pos)   != cfg->PLLM) ||
		(((tempreg & RCC::_PLLCFGR::PLLN::BitMsk)
		>> RCC::_PLLCFGR::PLLN::pos)  != cfg->PLLN)  ||
		(((tempreg & RCC::_PLLCFGR::PLLP::BitMsk)
		>> RCC::_PLLCFGR::PLLP::pos)  != pll_p)       ||
		(((tempreg & RCC::_PLLCFGR::PLLSRC::BitMsk)
		>> RCC::_PLLCFGR::PLLSRC::pos) != cfg->useHSE))
	{
		return RCCStatus::PLL_FAILED;
	}
	else
	{
		return RCCStatus::OK;
	}
}

RCCStatus RCC_ClockInit(const ClockConfig* cfg){
	// 5. Настройка Flash latency
	Flash::_ACR::overwrite(cfg->FLASHLatency);
	// 6. Настройка делителей шин (обновляем только нужные биты)
	RCC::_CFGR::clearMask<(RCC::_CFGR::HPRE::BitMsk)>();

	RCC::_CFGR::setMask
        (static_cast<uint32_t>(cfg->AHBDiv) << RCC::_CFGR::HPRE::pos);
	// 7. Переключение SYSCLK на PLL
	RCC::_CFGR::SW::write<SysClkStatus::PLLSrc>();
	while (RCC::_CFGR::SWS::read() != static_cast<uint32_t>(SysClkStatus::PLLSrc)) {
        __NOP();
    };

	RCC::_CFGR::clearMask<(
		RCC::_CFGR::PPRE1::BitMsk | 
        RCC::_CFGR::PPRE2::BitMsk
	)>();

	RCC::_CFGR::setMask(
		(static_cast<uint32_t>(cfg->APB1Div) << RCC::_CFGR::PPRE1::pos) | 
        (static_cast<uint32_t>(cfg->APB2Div) << RCC::_CFGR::PPRE2::pos));

	// 8. Обновление SystemCoreClock
	SystemCoreClockUpdate();

	// 9. Настройка SysTick
	if (cfg->useSysTick) {
		SysTick_Config(SystemCoreClock / MS_DIV);
	}
	return RCCStatus::OK;
}

RCCStatus OverDriveInit()
{
	enablePowerInterface();

	PWR::_CR1::ODEN::set();
	if (!waitUntil(PWR::_CSR1::ODRDY::read(), OVERDRIVE_TIMEOUT))
		return RCCStatus::OVERDRIVE_FAILED;
	

	PWR::_CR1::ODSWEN::set();
	if (!waitUntil(PWR::_CSR1::ODSWRDY::read(), OVERDRIVE_TIMEOUT))
		return RCCStatus::OVERDRIVE_FAILED;
	
	return RCCStatus::OK;
}

void enablePowerInterface(void) {
	uint32_t tmpreg;
	RCC::_APB1ENR::PWREN::set();
	tmpreg = RCC::_AHB1ENR::read();
	(void) tmpreg;
}

void SetVoltageScale(VoltageScale scale) {
	uint32_t tmpreg;
	PWR::_CR1::VOS::write(static_cast<uint32_t>(scale));
	tmpreg = PWR::_CR1::read();
	(void) tmpreg;
}

void enableEthInterface(void) {
	uint32_t tmpreg;
	RCC::_AHB1ENR::setMask
		  < RCC::_AHB1ENR::ETHMACEN::BitMsk
		  | RCC::_AHB1ENR::ETHMACTXEN::BitMsk
		  | RCC::_AHB1ENR::ETHMACRXEN::BitMsk>();
	tmpreg = RCC::_AHB1ENR::read();
	(void) tmpreg;
}
