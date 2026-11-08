/*
 * STM32H755 clock bookkeeping and early power-supply setup.
 */
#include "core/system.hpp"
#include "chip/core.hpp"
#include "config.hpp"

uint32_t SystemCoreClock = HSI_VALUE;  // CPU clock (d1_ck / D1CPRE)
uint32_t AHBClock = HSI_VALUE;         // AXI/AHB clock (hclk)
uint32_t US_Prescaler = 1;
uint32_t APB1Clock = 0;
uint32_t APB2Clock = 0;
// D1CPRE/HPRE and DxPPRE use the same encodings as on the F7.
const uint8_t AHBPrescTable[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9 };
const uint8_t APBPrescTable[8] = { 0, 0, 0, 0, 1, 2, 3, 4 };

namespace {
uint32_t hsi_clock() {
	return HSI_VALUE >> RCC::_CR::HSIDIV::read();
}

uint32_t pll1_p_clock() {
	const uint32_t divm = RCC::_PLLCKSELR::DIVM1::read();
	if (divm == 0U) {
		return 0U;  // PLL1 prescaler disabled
	}
	uint32_t src;
	switch (RCC::_PLLCKSELR::PLLSRC::read()) {
	case 0U: src = hsi_clock(); break;
	case 1U: src = CSI_VALUE;   break;
	case 2U: src = HSE_VALUE;   break;
	default: return 0U;         // no clock
	}
	// Integer mode only (PLL1FRACEN is never set by MCORE)
	const uint32_t n = RCC::_PLL1DIVR::DIVN1::read() + 1U;
	const uint32_t p = RCC::_PLL1DIVR::DIVP1::read() + 1U;
	return static_cast<uint32_t>((static_cast<uint64_t>(src) / divm) * n / p);
}
} // namespace

void SystemCoreClockUpdate() {
	uint32_t sysclk;
	switch (RCC::_CFGR::SWS::read()) {
	case 0b000: sysclk = hsi_clock();    break;
	case 0b001: sysclk = CSI_VALUE;      break;
	case 0b010: sysclk = HSE_VALUE;      break;
	case 0b011: sysclk = pll1_p_clock(); break;
	default:    sysclk = hsi_clock();    break;
	}

	const uint32_t cm7_clock = sysclk >> AHBPrescTable[RCC::_D1CFGR::D1CPRE::read()];
	AHBClock = cm7_clock >> AHBPrescTable[RCC::_D1CFGR::HPRE::read()];
	// the Cortex-M4 runs from the AHB clock (rcc_c2_ck)
	SystemCoreClock = chip::on_cm4() ? AHBClock : cm7_clock;
	APB1Clock = AHBClock >> APBPrescTable[RCC::_D2CFGR::D2PPRE1::read()];
	APB2Clock = AHBClock >> APBPrescTable[RCC::_D2CFGR::D2PPRE2::read()];
	US_Prescaler = SystemCoreClock / 1000000U;
}

/*
 * Called by the startup code before .data/.bss are initialised, so it must
 * not touch any global object.
 *
 * The H7 supply configuration (PWR_CR3) is WRITE-ONCE after power-on and
 * MUST match the board hardware. A mismatch leaves the chip unable to reach
 * the requested voltage; recovery then needs BOOT0=1 + full erase.
 * Select the board variant with MCORE_PWR_SUPPLY in the project's config.hpp.
 */
extern "C" void ExitRun0Mode() {
	// Power is owned by the Cortex-M7; the M4 waits for it in main()
	// (hsem::wait_for_cm7) before using any clock-dependent peripheral.
	if (chip::on_cm4()) {
		return;
	}
	uint32_t cr3 = PWR::_CR3::read();
	cr3 &= ~(PWR::_CR3::BYPASS::BitMsk | PWR::_CR3::LDOEN::BitMsk | PWR::_CR3::SDEN::BitMsk);
#if MCORE_PWR_SUPPLY == MCORE_PWR_DIRECT_SMPS
	cr3 |= PWR::_CR3::SDEN::BitMsk;
#elif MCORE_PWR_SUPPLY == MCORE_PWR_LDO
	cr3 |= PWR::_CR3::LDOEN::BitMsk;
#else
#error "Unsupported MCORE_PWR_SUPPLY value"
#endif
	PWR::_CR3::overwrite(cr3);

	// Wait until the voltage level matches the requested scale
	while (PWR::_CSR1::ACTVOSRDY::read() == 0U) {
	}
}
