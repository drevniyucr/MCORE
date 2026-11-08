#pragma once
// USART kernel clock selection (STM32F767).
#include <cstdint>
#include "core/regs.hpp"
#include "core/system.hpp"   // SystemCoreClock
#include "config.hpp"        // HSI_VALUE, CSI_VALUE, LSE_VALUE

enum class UartClockSrc : uint32_t {
	PCLK = 0b00, SYSCLK = 0b01, HSI = 0b10, LSE = 0b11
};

template<typename USARTx>
static inline void USART_ClkSel(UartClockSrc src);
template <>
inline void USART_ClkSel<USART1>(UartClockSrc src) {
	RCC::_DKCFGR2::USART1SEL::write(static_cast<uint32_t>(src));
	RCC::_APB2ENR::USART1EN::set();
}
template <>
inline void USART_ClkSel<USART2>(UartClockSrc src) {
	RCC::_DKCFGR2::USART2SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1ENR::USART2EN::set();
}
template <>
inline void USART_ClkSel<USART3>(UartClockSrc src) {
	RCC::_DKCFGR2::USART3SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1ENR::USART3EN::set();
}
template <>
inline void USART_ClkSel<USART6>(UartClockSrc src) {
	RCC::_DKCFGR2::USART6SEL::write(static_cast<uint32_t>(src));
	RCC::_APB2ENR::USART6EN::set();
}
template <>
inline void USART_ClkSel<UART4>(UartClockSrc src) {
	RCC::_DKCFGR2::UART4SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1ENR::UART4EN::set();
}
template <>
inline void USART_ClkSel<UART5>(UartClockSrc src) {
	RCC::_DKCFGR2::UART5SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1ENR::UART5EN::set();
}
template <>
inline void USART_ClkSel<UART7>(UartClockSrc src) {
	RCC::_DKCFGR2::UART7SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1ENR::UART7ENR::set();
}
template <>
inline void USART_ClkSel<UART8>(UartClockSrc src) {
	RCC::_DKCFGR2::UART8SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1ENR::UART8ENR::set();
}

// Kernel clock in Hz of a USART for a clock source (0 = not supported)
inline uint32_t usart_kernel_clock(UartClockSrc src, uint32_t pclk) {
	switch (src) {
	case UartClockSrc::PCLK:   return pclk;
	case UartClockSrc::SYSCLK: return SystemCoreClock;
	case UartClockSrc::HSI:    return HSI_VALUE;
	case UartClockSrc::LSE:    return LSE_VALUE;
	default:                   return 0;
	}
}
