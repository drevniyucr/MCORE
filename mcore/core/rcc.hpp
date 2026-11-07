#pragma once
// Chip dispatcher (see core/regs.hpp).
#if defined(MCORE_STM32H755)
#include "chip/stm32h755/rcc.hpp"
#else
#include "chip/stm32f767/rcc.hpp"
#endif
