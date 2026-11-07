#pragma once
/*
 * STM32H755 (Cortex-M7 core) reset & clock control.
 *
 * Clock tree used by MCORE (RM0399):
 *   HSE/HSI -> /DIVM1 -> PLL1 (xDIVN1) -> /DIVP1 = sys_ck
 *   sys_ck -> /D1CPRE = CPU clock (SystemCoreClock, SysTick)
 *          -> /HPRE   = AXI/AHB clock (AHBClock): flash, DMA, ETH
 *          -> /D2PPRE1, /D2PPRE2, /D1PPRE, /D3PPRE = APB1..APB4
 *
 * Only VOS1 is supported here (no overdrive/VOS0): CPU <= 400 MHz,
 * AHB <= 200 MHz, APB <= 100 MHz. VOS0 (480 MHz) is not available when the
 * chip runs from the direct SMPS supply anyway.
 */
#include "core/def.hpp"
#include "core/system.hpp"
#include "config.hpp"

// ---------------------------------------------------------------------------
// Peripheral clock helpers (GPIO lives on AHB4 in the D3 domain)
// ---------------------------------------------------------------------------
static inline void RCC_GPIOAEN() { RCC::_AHB4ENR::GPIOAEN::set(); }
static inline void RCC_GPIOBEN() { RCC::_AHB4ENR::GPIOBEN::set(); }
static inline void RCC_GPIOCEN() { RCC::_AHB4ENR::GPIOCEN::set(); }
static inline void RCC_GPIODEN() { RCC::_AHB4ENR::GPIODEN::set(); }
static inline void RCC_GPIOEEN() { RCC::_AHB4ENR::GPIOEEN::set(); }
static inline void RCC_GPIOFEN() { RCC::_AHB4ENR::GPIOFEN::set(); }
static inline void RCC_GPIOGEN() { RCC::_AHB4ENR::GPIOGEN::set(); }

static inline void RCC_TIM2EN()  { RCC::_APB1LENR::TIM2EN::set(); }
static inline void RCC_TIM3EN()  { RCC::_APB1LENR::TIM3EN::set(); }

static inline void RCC_GPIO_ALLEN() {
	RCC::_AHB4ENR::setMask
	   <RCC::_AHB4ENR::GPIOAEN::BitMsk |
		RCC::_AHB4ENR::GPIOBEN::BitMsk |
		RCC::_AHB4ENR::GPIOCEN::BitMsk |
		RCC::_AHB4ENR::GPIODEN::BitMsk |
		RCC::_AHB4ENR::GPIOEEN::BitMsk |
		RCC::_AHB4ENR::GPIOFEN::BitMsk |
		RCC::_AHB4ENR::GPIOGEN::BitMsk |
		RCC::_AHB4ENR::GPIOHEN::BitMsk >();
	(void) RCC::_AHB4ENR::read();
}

// ---------------------------------------------------------------------------
// Enums
// ---------------------------------------------------------------------------
// D1CPRE / HPRE encoding (4 bits)
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

// D1PPRE / D2PPRE1 / D2PPRE2 / D3PPRE encoding (3 bits)
enum class APBPrescaler : uint8_t {
	Div1  = 0b000,
	Div2  = 0b100,
	Div4  = 0b101,
	Div8  = 0b110,
	Div16 = 0b111
};

// PWR_D3CR.VOS. Scale1 is the fastest one supported by this driver.
enum class VoltageScale : uint32_t {
	Scale1 = 0b11,
	Scale2 = 0b10,
	Scale3 = 0b01
};

// RCC_CFGR.SW / SWS
enum class SysClkStatus : uint32_t {
	HSISrc  = 0b000,
	CSISrc  = 0b001,
	HSESrc  = 0b010,
	PLLSrc  = 0b011
};

enum class RCCStatus {
	OK,
	HSE_FAILED,
	PLL_FAILED,
	SW_FAILED,
	VOS_FAILED,
	PLL_CONFIG_INVALID
};

struct ClockConfig {
	uint8_t  PLLM;               // DIVM1: PLL input divider [1..63]
	uint16_t PLLN;               // DIVN1: VCO multiplier [4..512]
	uint8_t  PLLP;               // DIVP1: sys_ck divider, even [2..128]
	uint8_t  PLLQ = 8;           // DIVQ1 [1..128] (FDCAN, SPI, SAI kernel clocks)
	uint8_t  PLLR = 2;           // DIVR1 [1..128]
	AHBPrescaler CPUDiv  = AHBPrescaler::Div1;  // D1CPRE: sys_ck -> CPU
	AHBPrescaler AHBDiv  = AHBPrescaler::Div2;  // HPRE:   CPU -> AXI/AHB
	APBPrescaler APB3Div = APBPrescaler::Div2;  // D1PPRE
	APBPrescaler APB1Div = APBPrescaler::Div2;  // D2PPRE1
	APBPrescaler APB2Div = APBPrescaler::Div2;  // D2PPRE2
	APBPrescaler APB4Div = APBPrescaler::Div2;  // D3PPRE
	bool useHSE = true;
	bool useHSEBypass = false;
	bool useSysTick = true;
};

// ---------------------------------------------------------------------------
// Compile-time clock math
// ---------------------------------------------------------------------------
namespace rcc_detail {
constexpr uint32_t ahb_div(AHBPrescaler p) {
	const uint32_t v = static_cast<uint32_t>(p);
	return (v < 8) ? 1U : (v < 12 ? (2U << (v - 8)) : (64U << (v - 12)));
}
constexpr uint32_t apb_div(APBPrescaler p) {
	const uint32_t v = static_cast<uint32_t>(p);
	return (v < 4) ? 1U : (2U << (v - 4));
}
constexpr uint32_t pll_in(const ClockConfig &c)  { return (c.useHSE ? HSE_VALUE : HSI_VALUE) / c.PLLM; }
constexpr uint32_t pll_vco(const ClockConfig &c) { return pll_in(c) * c.PLLN; }
constexpr uint32_t sysclk(const ClockConfig &c)  { return pll_vco(c) / c.PLLP; }
constexpr uint32_t cpuclk(const ClockConfig &c)  { return sysclk(c) / ahb_div(c.CPUDiv); }
constexpr uint32_t ahbclk(const ClockConfig &c)  { return cpuclk(c) / ahb_div(c.AHBDiv); }

// PLLxRGE: reference clock range
constexpr uint32_t pll_range(uint32_t ref) {
	return ref < 2000000U ? 0U : ref < 4000000U ? 1U : ref < 8000000U ? 2U : 3U;
}

// Flash wait states and programming delay for VOS1 (RM0399, "FLASH recommended
// number of wait states and programming delay").
constexpr uint32_t flash_latency(uint32_t aclk) {
	return aclk <= 70000000U ? 0U : aclk <= 140000000U ? 1U : aclk <= 210000000U ? 2U : 3U;
}
constexpr uint32_t flash_wrhighfreq(uint32_t aclk) {
	return aclk <= 70000000U ? 0U : aclk <= 185000000U ? 1U : 2U;
}
} // namespace rcc_detail

// Wait with timeout. Returns true on timeout.
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

constexpr uint32_t VOS_TIMEOUT       = 0x00FFFFFF;
constexpr uint32_t CLOCK_SW_TIMEOUT  = 0x00FFFFFF;
constexpr uint32_t HSE_TIMEOUT       = 0x00FFFFFF;
constexpr uint32_t PLL_TIMEOUT       = 0x00FFFFFF;
constexpr uint32_t MS_DIV = 1000U;

// ---------------------------------------------------------------------------
// Power: voltage scaling. The supply source (LDO / SMPS) is configured much
// earlier, in ExitRun0Mode() called from the startup code (see clock.cpp).
// ---------------------------------------------------------------------------
template <VoltageScale scale>
static inline RCCStatus SetVoltageScale() {
	PWR::_D3CR::VOS::write(static_cast<uint32_t>(scale));
	(void) PWR::_D3CR::read();
	if (waitUntil([] { return PWR::_D3CR::VOSRDY::read() != 0U; }, VOS_TIMEOUT))
		return RCCStatus::VOS_FAILED;
	return RCCStatus::OK;
}

// ---------------------------------------------------------------------------
// Oscillators + PLL1
// ---------------------------------------------------------------------------
template <ClockConfig cfg>
static inline RCCStatus OSCInit() {
	using namespace rcc_detail;
	static_assert(cfg.PLLM >= 1 && cfg.PLLM <= 63, "DIVM1 must be 1..63");
	static_assert(cfg.PLLN >= 4 && cfg.PLLN <= 512, "DIVN1 must be 4..512");
	static_assert(cfg.PLLP >= 2 && cfg.PLLP <= 128 && (cfg.PLLP % 2) == 0, "DIVP1 must be even, 2..128");
	static_assert(cfg.PLLQ >= 1 && cfg.PLLQ <= 128 && cfg.PLLR >= 1 && cfg.PLLR <= 128, "DIVQ1/DIVR1 must be 1..128");
	static_assert(pll_in(cfg) >= 2000000U && pll_in(cfg) <= 16000000U,
	              "PLL1 reference must be 2..16 MHz for the wide-range VCO");
	static_assert(pll_vco(cfg) >= 192000000U && pll_vco(cfg) <= 960000000U,
	              "PLL1 VCO (wide range) must be 192..960 MHz");
	static_assert(cpuclk(cfg) <= 400000000U, "CPU clock above 400 MHz needs VOS0 (not supported)");
	static_assert(ahbclk(cfg) <= 200000000U, "AXI/AHB clock must be <= 200 MHz in VOS1");
	static_assert(ahbclk(cfg) / apb_div(cfg.APB1Div) <= 100000000U &&
	              ahbclk(cfg) / apb_div(cfg.APB2Div) <= 100000000U &&
	              ahbclk(cfg) / apb_div(cfg.APB3Div) <= 100000000U &&
	              ahbclk(cfg) / apb_div(cfg.APB4Div) <= 100000000U, "APB clocks must be <= 100 MHz");

	// 1. HSE
	if constexpr (cfg.useHSE) {
		if constexpr (cfg.useHSEBypass) {
			RCC::_CR::HSEBYP::set();
		}
		RCC::_CR::HSEON::set();
		if (waitUntil([] { return RCC::_CR::HSERDY::read() != 0U; }, HSE_TIMEOUT)) {
			RCC::_CR::HSEON::clear();
			return RCCStatus::HSE_FAILED;
		}
	}

	// 2. PLL1 must be off while it is reconfigured
	RCC::_CR::PLL1ON::clear();
	if (waitUntil([] { return RCC::_CR::PLL1RDY::read() == 0U; }, PLL_TIMEOUT))
		return RCCStatus::PLL_FAILED;

	// 3. Source + DIVM1 (PLLSRC: 0 = HSI, 1 = CSI, 2 = HSE)
	RCC::_PLLCKSELR::PLLSRC::write(cfg.useHSE ? 2U : 0U);
	RCC::_PLLCKSELR::DIVM1::write(static_cast<uint32_t>(cfg.PLLM));

	// 4. Dividers (registers hold value - 1)
	RCC::_PLL1DIVR::overwrite(
		  ((static_cast<uint32_t>(cfg.PLLN) - 1U) << RCC::_PLL1DIVR::DIVN1::pos)
		| ((static_cast<uint32_t>(cfg.PLLP) - 1U) << RCC::_PLL1DIVR::DIVP1::pos)
		| ((static_cast<uint32_t>(cfg.PLLQ) - 1U) << RCC::_PLL1DIVR::DIVQ1::pos)
		| ((static_cast<uint32_t>(cfg.PLLR) - 1U) << RCC::_PLL1DIVR::DIVR1::pos));

	// 5. Input range, wide VCO, integer mode, enable P/Q/R outputs
	uint32_t pllcfgr = RCC::_PLLCFGR::read();
	pllcfgr &= ~(RCC::_PLLCFGR::PLL1RGE::BitMsk | RCC::_PLLCFGR::PLL1VCOSEL::BitMsk |
	             RCC::_PLLCFGR::PLL1FRACEN::BitMsk);
	pllcfgr |= (pll_range(pll_in(cfg)) << RCC::_PLLCFGR::PLL1RGE::pos)
	         | RCC::_PLLCFGR::DIVP1EN::BitMsk
	         | RCC::_PLLCFGR::DIVQ1EN::BitMsk
	         | RCC::_PLLCFGR::DIVR1EN::BitMsk;
	RCC::_PLLCFGR::overwrite(pllcfgr);

	// 6. Enable and wait for lock
	RCC::_CR::PLL1ON::set();
	if (waitUntil([] { return RCC::_CR::PLL1RDY::read() != 0U; }, PLL_TIMEOUT))
		return RCCStatus::PLL_FAILED;

	return RCCStatus::OK;
}

// ---------------------------------------------------------------------------
// Bus prescalers, flash latency, SYSCLK switch
// ---------------------------------------------------------------------------
template <ClockConfig cfg>
static inline RCCStatus RCC_ClockInit() {
	using namespace rcc_detail;
	constexpr uint32_t aclk = ahbclk(cfg);

	// 1. Flash wait states BEFORE raising the clock
	Flash::_ACR::overwrite(
		  (flash_latency(aclk)    << Flash::_ACR::LATENCY::pos)
		| (flash_wrhighfreq(aclk) << Flash::_ACR::WRHIGHFREQ::pos));
	if (Flash::_ACR::LATENCY::read() != flash_latency(aclk))
		return RCCStatus::SW_FAILED;

	// 2. Prescalers BEFORE the switch so no bus ever runs over its limit
	RCC::_D1CFGR::overwrite(
		  (static_cast<uint32_t>(cfg.CPUDiv)  << RCC::_D1CFGR::D1CPRE::pos)
		| (static_cast<uint32_t>(cfg.AHBDiv)  << RCC::_D1CFGR::HPRE::pos)
		| (static_cast<uint32_t>(cfg.APB3Div) << RCC::_D1CFGR::D1PPRE::pos));
	RCC::_D2CFGR::overwrite(
		  (static_cast<uint32_t>(cfg.APB1Div) << RCC::_D2CFGR::D2PPRE1::pos)
		| (static_cast<uint32_t>(cfg.APB2Div) << RCC::_D2CFGR::D2PPRE2::pos));
	RCC::_D3CFGR::overwrite(
		  (static_cast<uint32_t>(cfg.APB4Div) << RCC::_D3CFGR::D3PPRE::pos));

	// 3. Switch SYSCLK to PLL1
	RCC::_CFGR::SW::write(static_cast<uint32_t>(SysClkStatus::PLLSrc));
	if (waitUntil([] { return RCC::_CFGR::SWS::read() == static_cast<uint32_t>(SysClkStatus::PLLSrc); },
	              CLOCK_SW_TIMEOUT))
		return RCCStatus::SW_FAILED;

	// 4. Update the cached clock values
	SystemCoreClockUpdate();

	// 5. 1 ms SysTick from the CPU clock
	if constexpr (cfg.useSysTick) {
		SysTick_Config(SystemCoreClock / MS_DIV);
	}
	return RCCStatus::OK;
}

// HSI48 (RC48): kernel clock of the RNG (and USB). Owned by the Cortex-M7,
// which turns it on before releasing the Cortex-M4.
static inline RCCStatus enableHSI48() {
	RCC::_CR::RC48ON::set();
	if (waitUntil([] { return RCC::_CR::RC48RDY::read() != 0U; }, HSE_TIMEOUT))
		return RCCStatus::HSE_FAILED;
	return RCCStatus::OK;
}

// D2 SRAM1/2/3 clocks (Ethernet buffers live in SRAM3).
static inline void enableD2SRAM() {
	RCC::_AHB2ENR::setMask<RCC::_AHB2ENR::SRAM1EN::BitMsk |
	                       RCC::_AHB2ENR::SRAM2EN::BitMsk |
	                       RCC::_AHB2ENR::SRAM3EN::BitMsk>();
	(void) RCC::_AHB2ENR::read();
}
