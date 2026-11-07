#pragma once
// Chip register layer dispatcher. The target chip is selected by CMake
// (-DMCORE_CHIP=STM32F767 | STM32H755), which defines MCORE_<CHIP>.
// Host builds (tests) define nothing and get the STM32F767 layout.
#if defined(MCORE_STM32H755)
#include "chip/stm32h755/regs.hpp"
#else
#include "chip/stm32f767/regs.hpp"
#endif
