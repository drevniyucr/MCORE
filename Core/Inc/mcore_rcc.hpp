#pragma once

#include "mcore_def.hpp"
#include "mcore_system.hpp"
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
	RCC::_AHB1ENR::setMask
	   <RCC::_AHB1ENR::GPIOAEN::BitMsk |
		RCC::_AHB1ENR::GPIOBEN::BitMsk |
		RCC::_AHB1ENR::GPIOCEN::BitMsk |
		RCC::_AHB1ENR::GPIODEN::BitMsk |
		RCC::_AHB1ENR::GPIOEEN::BitMsk |
		RCC::_AHB1ENR::GPIOFEN::BitMsk |
		RCC::_AHB1ENR::GPIOGEN::BitMsk >();
}

enum class PLL_P : uint8_t {
	Div2 = 0b00, 
    Div4 = 0b01, 
    Div6 = 0b10, 
    Div8 = 0b11
};

enum class AHBPrescaler : uint8_t {
	Div1   = 0b0000,
	Div2   = 0b1000,
	Div4   = 0b1001,
	Div8   = 0b1010,
	Div16  = 0b1011,
	Div64  = 0b1100,
	Div128 = 0b1101,
	Div256 = 0b1110,
	Div512 = 0b1111
};

enum class APBPrescaler : uint8_t {
	Div1  = 0b000,
	Div2  = 0b100,
	Div4  = 0b101,
	Div8  = 0b110,
	Div16 = 0b111
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
	 Scale1 = 0b11,
	 Scale2 = 0b10, 
	 Scale3 = 0b00
};

enum class SysClkStatus : uint32_t {
	 HSISrc = 0b00,
	 HSESrc = 0b01, 
	 PLLSrc = 0b10
};

enum class RCCStatus {
	OK, 
	HSE_FAILED, 
	PLL_FAILED,
	SW_FAILED,
	OVERDRIVE_FAILED, 
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

// Wait with timeout
template <typename Predicate>
bool waitUntil(Predicate condition, uint32_t timeout) {
    volatile uint32_t i = 0;
    while (!condition()) {
    	i += 1;
        if (i > timeout)
        	return true;
    }
    return false;
}

constexpr uint32_t OVERDRIVE_TIMEOUT = 0xFFFFAAAA;
constexpr uint32_t CLOCK_SW_TIMEOUT = 0xFFFFAAAA;
constexpr uint32_t HSE_TIMEOUT = 0xFFFFAAAA;
constexpr uint32_t PLL_TIMEOUT = 0xFFFFAAAA;
constexpr uint32_t MS_DIV = 1000U;

static inline void enablePowerInterface(void) {
	uint32_t tmpreg;
	RCC::_APB1ENR::PWREN::set();
	tmpreg = RCC::_AHB1ENR::read();
	(void) tmpreg;
}

static inline RCCStatus OverDriveInit()
{
	enablePowerInterface();

	PWR::_CR1::ODEN::set();
	if (waitUntil([]{return PWR::_CSR1::ODRDY::read();}, OVERDRIVE_TIMEOUT))
		return RCCStatus::OVERDRIVE_FAILED;
	

	PWR::_CR1::ODSWEN::set();
	if (waitUntil([]{return PWR::_CSR1::ODSWRDY::read();}, OVERDRIVE_TIMEOUT))
		return RCCStatus::OVERDRIVE_FAILED;
	
	return RCCStatus::OK;
}
	

template<VoltageScale scale>	
static inline void SetVoltageScale() {
	uint32_t tmpreg;
	PWR::_CR1::VOS::write(static_cast<uint32_t>(scale));
	tmpreg = PWR::_CR1::read();
	(void) tmpreg;
}
	
static inline void enableEthInterface(void) {
	uint32_t tmpreg;
	RCC::_AHB1ENR::setMask
		  < RCC::_AHB1ENR::ETHMACEN::BitMsk
		  | RCC::_AHB1ENR::ETHMACTXEN::BitMsk
		  | RCC::_AHB1ENR::ETHMACRXEN::BitMsk>();
	tmpreg = RCC::_AHB1ENR::read();
	(void) tmpreg;
}

template<ClockConfig cfg>
static inline RCCStatus OSCInit() {	

	// 1. Проверка PLL параметров (валидные диапазоны по Reference Manual)
	static_assert(cfg.PLLM >= 2 && cfg.PLLM <= 63 && cfg.PLLN >= 50 && cfg.PLLN <= 432, "PLL configuration is out of valid range");

	// 2. Включение HSE (если нужно)
	if constexpr (cfg.useHSE) {
		if constexpr (cfg.useHSEBypass) {
			RCC::_CR::HSEBYP::set();
		}
		RCC::_CR::HSEON::set();

		if(waitUntil([]{return RCC::_CR::HSERDY::read();},HSE_TIMEOUT)){
			RCC::_CR::HSEON::clear();
			return RCCStatus::HSE_FAILED;
		}
	}

	// 3. Настройка PLL
    RCC::_CR::PLLON::clear();
	if(waitUntil(([]{return ! RCC::_CR::PLLRDY::read();}), PLL_TIMEOUT)){
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
		  (static_cast<uint32_t>(cfg.PLLM) << RCC::_PLLCFGR::PLLM::pos)
		| (static_cast<uint32_t>(cfg.PLLN) << RCC::_PLLCFGR::PLLN::pos)
		| (static_cast<uint32_t>(cfg.PLLP) << RCC::_PLLCFGR::PLLP::pos)
		| (cfg.useHSE ? RCC::_PLLCFGR::PLLSRC::BitMsk : 0U)
		);

	// 4. Включение PLL и ожидание готовности
	RCC::_CR::PLLON::set();
	if(waitUntil([]{return RCC::_CR::PLLRDY::read();}, PLL_TIMEOUT)){
		RCC::_CR::bitReset<RCC::_CR::HSEON, RCC::_CR::PLLON>();
		return RCCStatus::PLL_FAILED;
	}
	
	uint32_t tempreg =  RCC::_PLLCFGR::read();
	constexpr uint32_t pll_p = static_cast<uint32_t>(cfg.PLLP);

	if ( 
		(((tempreg & RCC::_PLLCFGR::PLLM::BitMsk)
		>> RCC::_PLLCFGR::PLLM::pos)   != cfg.PLLM) ||
		(((tempreg & RCC::_PLLCFGR::PLLN::BitMsk)
		>> RCC::_PLLCFGR::PLLN::pos)  != cfg.PLLN)  ||
		(((tempreg & RCC::_PLLCFGR::PLLP::BitMsk)
		>> RCC::_PLLCFGR::PLLP::pos)  != pll_p)       ||
		(((tempreg & RCC::_PLLCFGR::PLLSRC::BitMsk)
		>> RCC::_PLLCFGR::PLLSRC::pos) != cfg.useHSE))
	{
		return RCCStatus::PLL_FAILED;
	}
	else
	{
		return RCCStatus::OK;
	}
}

template<ClockConfig cfg>
static inline RCCStatus RCC_ClockInit(){
	// 5. Настройка Flash latency
	Flash::_ACR::overwrite(cfg.FLASHLatency);
	// 6. Настройка делителей шин (обновляем только нужные биты)
	RCC::_CFGR::clearMask<(RCC::_CFGR::HPRE::BitMsk)>();

	RCC::_CFGR::setMask
        (static_cast<uint32_t>(cfg.AHBDiv) << RCC::_CFGR::HPRE::pos);
	// 7. Переключение SYSCLK на PLL
	RCC::_CFGR::SW::write<SysClkStatus::PLLSrc>();
	if (waitUntil([]{return RCC::_CFGR::SWS::read();}, OVERDRIVE_TIMEOUT))
			return RCCStatus::OVERDRIVE_FAILED;

	RCC::_CFGR::clearMask<(
		RCC::_CFGR::PPRE1::BitMsk | 
        RCC::_CFGR::PPRE2::BitMsk
	)>();

	RCC::_CFGR::setMask(
		(static_cast<uint32_t>(cfg.APB1Div) << RCC::_CFGR::PPRE1::pos) | 
        (static_cast<uint32_t>(cfg.APB2Div) << RCC::_CFGR::PPRE2::pos));

	// 8. Обновление SystemCoreClock
	SystemCoreClockUpdate();

	// 9. Настройка SysTick
	if constexpr (cfg.useSysTick) {
		SysTick_Config(SystemCoreClock / MS_DIV);
	}
	return RCCStatus::OK;
}