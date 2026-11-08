#pragma once
// I2C kernel clock selection (STM32H755).
#include <cstdint>
#include "core/regs.hpp"

// RCC_D2CCIP2R.I2C123SEL / RCC_D3CCIPR.I2C4SEL
enum class I2CClockSrc : uint8_t {
    PCLK1 = 0, PLL3R = 1, HSI = 2, CSI = 3
};

template<typename I2Cx>
static inline void I2C_ClkSel(I2CClockSrc src);
// I2C1..3 share one kernel-clock mux (D2 domain), I2C4 lives in D3.
template <>
inline void I2C_ClkSel<I2C1>(I2CClockSrc src){
		RCC::_D2CCIP2R::I2C123SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1LENR::I2C1EN::set();
}
template <>
inline void I2C_ClkSel<I2C2>(I2CClockSrc src){
		RCC::_D2CCIP2R::I2C123SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1LENR::I2C2EN::set();
}
template <>
inline void I2C_ClkSel<I2C3>(I2CClockSrc src){
		RCC::_D2CCIP2R::I2C123SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1LENR::I2C3EN::set();
}
template <>
inline void I2C_ClkSel<I2C4>(I2CClockSrc src){
		RCC::_D3CCIPR::I2C4SEL::write(static_cast<uint32_t>(src));
		RCC::_APB4ENR::I2C4EN::set();
}
