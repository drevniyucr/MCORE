/*
 * STM32F767 clock bookkeeping: SystemCoreClock and bus clocks from RCC state.
 */
#include "core/system.hpp"
#include "config.hpp"

uint32_t SystemCoreClock = 16000000;
uint32_t AHBClock = 16000000;
uint32_t US_Prescaler = 1;
uint32_t APB1Clock = 0;
uint32_t APB2Clock = 0;
const uint8_t AHBPrescTable[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9 };
const uint8_t APBPrescTable[8] = { 0, 0, 0, 0, 1, 2, 3, 4 };

void SystemCoreClockUpdate() {
	uint32_t tmp, pllvco, pllp, pllsource, pllm;

	/* Get SYSCLK source -------------------------------------------------------*/
	tmp = RCC::_CFGR::SWS::read();
	switch (tmp) {
	case 0b00: /* HSI used as system clock source */
		SystemCoreClock = HSI_VALUE;
		break;
	case 0b01: /* HSE used as system clock source */
		SystemCoreClock = HSE_VALUE;
		break;
	case 0b10: /* PLL used as system clock source */

		pllsource = RCC::_PLLCFGR::PLLSRC::read();
		pllm      = RCC::_PLLCFGR::PLLM::read();

		if (pllsource != 0) {
			/* HSE used as PLL clock source */
			pllvco = (HSE_VALUE / pllm) * ((RCC::_PLLCFGR::PLLN::read()));
		} else {
			/* HSI used as PLL clock source */
			pllvco = (HSI_VALUE / pllm) * ((RCC::_PLLCFGR::PLLN::read()));
		}
		pllp = ((RCC::_PLLCFGR::PLLP::read()) + 1) * 2;
		SystemCoreClock = pllvco / pllp;
		break;
	default:
		SystemCoreClock = HSI_VALUE;
		break;
	}

	tmp = AHBPrescTable[RCC::_CFGR::HPRE::read()];
	/* HCLK frequency */
	SystemCoreClock >>= tmp;
	AHBClock = SystemCoreClock; // on F7 the CPU runs at HCLK
	tmp = APBPrescTable[RCC::_CFGR::PPRE1::read()];
	APB1Clock = SystemCoreClock >> tmp;// APB1 frequency
	tmp = APBPrescTable[RCC::_CFGR::PPRE2::read()];
	APB2Clock = SystemCoreClock >> tmp;// APB2 frequency
	US_Prescaler = SystemCoreClock / 1000000;
}

