#pragma once
// Chip dispatcher (see core/regs.hpp).
#if defined(MCORE_STM32H755)
#include "chip/stm32h755/nvic_def.hpp"
#else
#include "chip/stm32f767/nvic_def.hpp"
#endif
