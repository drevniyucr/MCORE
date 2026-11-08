#pragma once
// USART kernel clock selection (STM32H755).
// RCC_D2CCIP2R.USART16SEL / USART234578SEL
enum class UartClockSrc : uint32_t {
	PCLK = 0b000, PLL2Q = 0b001, PLL3Q = 0b010, HSI = 0b011, CSI = 0b100, LSE = 0b101
};

template<typename USARTx>
static inline void USART_ClkSel(UartClockSrc src);
// USART1/6 share one kernel-clock mux, USART2/3, UART4/5/7/8 share another.
template <>
inline void USART_ClkSel<USART1>(UartClockSrc src) {
	RCC::_D2CCIP2R::USART16SEL::write(static_cast<uint32_t>(src));
	RCC::_APB2ENR::USART1EN::set();
}
template <>
inline void USART_ClkSel<USART6>(UartClockSrc src) {
	RCC::_D2CCIP2R::USART16SEL::write(static_cast<uint32_t>(src));
	RCC::_APB2ENR::USART6EN::set();
}
template <>
inline void USART_ClkSel<USART2>(UartClockSrc src) {
	RCC::_D2CCIP2R::USART234578SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1LENR::USART2EN::set();
}
template <>
inline void USART_ClkSel<USART3>(UartClockSrc src) {
	RCC::_D2CCIP2R::USART234578SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1LENR::USART3EN::set();
}
template <>
inline void USART_ClkSel<UART4>(UartClockSrc src) {
	RCC::_D2CCIP2R::USART234578SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1LENR::UART4EN::set();
}
template <>
inline void USART_ClkSel<UART5>(UartClockSrc src) {
	RCC::_D2CCIP2R::USART234578SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1LENR::UART5EN::set();
}
template <>
inline void USART_ClkSel<UART7>(UartClockSrc src) {
	RCC::_D2CCIP2R::USART234578SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1LENR::USART7EN::set();
}
template <>
inline void USART_ClkSel<UART8>(UartClockSrc src) {
	RCC::_D2CCIP2R::USART234578SEL::write(static_cast<uint32_t>(src));
	RCC::_APB1LENR::USART8EN::set();
}

// Kernel clock in Hz of a USART for a clock source (0 = not supported)
inline uint32_t usart_kernel_clock(UartClockSrc src, uint32_t pclk) {
	switch (src) {
	case UartClockSrc::PCLK: return pclk;
	case UartClockSrc::CSI:  return CSI_VALUE;
	case UartClockSrc::HSI:  return HSI_VALUE;
	case UartClockSrc::LSE:  return LSE_VALUE;
	default:                 return 0;   // PLL2Q / PLL3Q are not configured by MCORE
	}
}
