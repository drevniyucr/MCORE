#include "core/system.hpp"
#include "core/nvic.hpp"
#include "config.hpp"
#include <cstdarg>
#include <cstdio>

volatile uint32_t myTick = 0;   // счетчик миллисекунд
// SystemCoreClock / AHBClock / APBxClock / US_Prescaler and
// SystemCoreClockUpdate() are chip-specific: see mcore/chip/<chip>/clock.cpp

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


#if defined(CORE_CM4)
extern "C" uint32_t g_pfnVectors[];   // startup file
#endif

void SystemInit() {
#if defined(CORE_CM4)
	// The STM32H755 Cortex-M4 boots from flash bank 2; point VTOR at its table.
	SCB::_VTOR::overwrite(reinterpret_cast<uint32_t>(g_pfnVectors));
#endif
	/* FPU settings ------------------------------------------------------------*/
	SCB::_CPACR::setMask(SCB::_CPACR::CP10::BitMsk | SCB::_CPACR::CP11::BitMsk); /* set CP10 and CP11 Full Access */
	NVIC_API::SetPriorityGrouping<NVIC_PriorityGroup::Group4>();
#if defined(MCORE_STM32H755)
	// Keep the D1/D3 debug clocks and the trace clock running so ITM/SWO
	// and the debugger keep working (the H7 gates them by default).
	DBGMCU::_CR::setMask<DBGMCU::_CR::D1DBGCKEN::BitMsk |
	                     DBGMCU::_CR::D3DBGCKEN::BitMsk |
	                     DBGMCU::_CR::TRACECLKEN::BitMsk>();
#endif
	DWT_Init();

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

	while (true) {
	}
}

extern "C" void HardFault_Handler(void) {

	while (true) {
	}
}

extern "C" void MemManage_Handler(void) {

	while (true) {
	}
}

extern "C" void BusFault_Handler(void) {
	while (true) {
	}
}

extern "C" void UsageFault_Handler(void) {

	while (true) {
	}
}

extern "C" void SVC_Handler(void) {

}

extern "C" void DebugMon_Handler(void) {

}

extern "C" void PendSV_Handler(void) {

}

// Time base of the scheduler (mcore/sched) and other users; empty by default
extern "C" __attribute__((weak)) void SysTick_Hook(void) {
}

extern "C" void SysTick_Handler(void) {
	myTick += 1;   // каждый тик увеличиваем счетчик
	SysTick_Hook();
}


