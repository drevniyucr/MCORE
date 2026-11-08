#pragma once
// ---------------------------------------------------------------------------
// Device selection seam.
//
// This is the ONE place where the MCU-independent framework binds to a concrete
// MCU register map. Everything else in mcore/ is written against the generic
// Register/Field templates and the peripheral type names those maps provide.
//
// Select a target with a build macro; the default is STM32F767 (the in-tree
// generated map mcore/core/regs.hpp).
//
// To port to another MCU:
//   1. Generate/obtain its register-map header in the same Register/Field style.
//   2. Add an #elif branch below that includes it.
//   3. Build with -DMCORE_DEVICE_<part>. No other framework file changes.
// ---------------------------------------------------------------------------

#if defined(USE_CMSIS)
  // Opt-in: use the vendor CMSIS device headers instead of the generated map.
  #include <stm32f7xx.h>
#elif defined(MCORE_DEVICE_STM32F767)
  #include "core/regs.hpp"
#elif !defined(MCORE_DEVICE)
  // No device specified -> default to the in-tree STM32F767 register map.
  #include "core/regs.hpp"
#else
  #error "MCORE: unknown MCORE_DEVICE. Add its register map to core/device.hpp."
#endif
