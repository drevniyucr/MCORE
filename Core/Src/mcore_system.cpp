#include "mcore_system.hpp"
#include "mcore_nvic.hpp"
#include "mcore_conf.hpp"
#include <cstdarg>
#include <cstdio>

volatile uint32_t myTick = 0;   // счетчик миллисекунд
uint32_t SystemCoreClock = 16000000;
uint32_t US_Prescaler = 1;
uint32_t APB1Clock = 0;
uint32_t APB2Clock = 0;
const uint8_t AHBPrescTable[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9 };
const uint8_t APBPrescTable[8] = { 0, 0, 0, 0, 1, 2, 3, 4 };

// Обработчик прерывания SysTick

void delay_ms(uint32_t ms) {
	uint32_t start = myTick;
	while ((myTick - start) < ms) {
		// ждем
	}
}

bool delay_us(uint32_t us) {
	if (US_Prescaler != 1){
		uint32_t start = us * US_Prescaler;
		while (start != 0) {
			start--;
		}
		return true;
	}
	return false;
}

uint32_t get_tick() {
	return myTick;
}

void print(const char *fmt, ...)
{
    char buf[256];  // буфер для форматированной строки
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    const char *str = buf;

    while (*str) {
        if ((ITM::_TCR::ITMENA::read() != 0U) && (ITM::_TER::STIMENA0::read() != 0U)) {
            while (ITM::_PORT0::u32::read() == 0U) {
                __NOP(); // ждём готовности
            }
            ITM::_PORT0::u8::write(*str);
        }
        str++;
    }
}

inline static void DWT_Init()
{
	SCB::_DEMCR::TRCENA::set();
	DWT::_LAR::overwrite(0xC5ACCE55);// unlock access DWT
	DWT::_CYCCNT::VALUE::write(0U);//clear cycle counter
	DWT::_CTRL::CYCCNTENA::set(); // Enable the cycle counter
}


void SystemInit() {
	/* FPU settings ------------------------------------------------------------*/
	SCB::_CPACR::setMask(SCB::_CPACR::CP10::BitMsk | SCB::_CPACR::CP11::BitMsk); /* set CP10 and CP11 Full Access */
	NVIC_API::SetPriorityGrouping<NVIC_PriorityGroup::Group4>();
	DWT_Init();

}

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
	tmp = APBPrescTable[RCC::_CFGR::PPRE1::read()];
	APB1Clock = SystemCoreClock >> tmp;// APB1 frequency
	tmp = APBPrescTable[RCC::_CFGR::PPRE2::read()];
	APB2Clock = SystemCoreClock >> tmp;// APB2 frequency
	US_Prescaler = SystemCoreClock / 1000000;
}

uint32_t SysTick_Config(uint32_t ticks) {
	if ((ticks - 1UL) > 0xFFFFFFUL) {
		return (1UL); /* Reload value impossible */
	}
	SysTick::_STRVR::overwrite(ticks - 1U); /* set reload register */

	NVIC_API::SetPriority<IRQn_Type::SysTick_IRQn, 15>(); /* set Priority for Systick Interrupt */

	SysTick::_STCVR::overwrite(0U); /* Load the SysTick Counter Value */

	SysTick::_STCSR::bitSet<SysTick::_STCSR::CLKSOURCE,
			SysTick::_STCSR::TICKINT, /* Enable SysTick IRQ and SysTick Timer */
			SysTick::_STCSR::ENABLE>();

	return (0UL); /* Function successful */
}

extern "C" void NMI_Handler(void) {

	while (1) {
	}
}

extern "C" void HardFault_Handler(void) {

	while (1) {
	}
}

extern "C" void MemManage_Handler(void) {

	while (1) {
	}
}

extern "C" void BusFault_Handler(void) {
	while (1) {
	}
}

extern "C" void UsageFault_Handler(void) {

	while (1) {
	}
}

extern "C" void SVC_Handler(void) {

}

extern "C" void DebugMon_Handler(void) {

}

extern "C" void PendSV_Handler(void) {

}

extern "C" void SysTick_Handler(void) {
	myTick += 1;   // каждый тик увеличиваем счетчик
}


