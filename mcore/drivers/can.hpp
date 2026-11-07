#pragma once
// Chip dispatcher (see core/regs.hpp).
#if defined(MCORE_STM32H755)
#error "drivers/can.hpp is the STM32F7 bxCAN driver; the STM32H7 has FDCAN (no MCORE driver yet)"
#else
#include "chip/stm32f767/can.hpp"
#endif
