#pragma once
// I2C kernel clock selection (STM32F767).
#include <cstdint>
#include "core/regs.hpp"

enum class I2CClockSrc : uint8_t {
    PCLK1 = 0, SYSCLK = 1, HSI = 2
};

template<typename I2Cx>
static inline void I2C_ClkSel(I2CClockSrc src);
template <>
 inline void I2C_ClkSel<I2C1>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C1SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1ENR::I2C1EN::set();
}
template <>
inline void I2C_ClkSel<I2C2>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C2SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1ENR::I2C2EN::set();
}
template <>
inline void I2C_ClkSel<I2C3>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C3SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1ENR::I2C3EN::set();
}
template <>
inline void I2C_ClkSel<I2C4>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C4SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1ENR::I2C4EN::set();
}
