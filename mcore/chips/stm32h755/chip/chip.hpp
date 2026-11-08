#pragma once
// Chip hooks of the core startup code (mcore/core/system.cpp).
namespace chip {

// Chip-specific part of SystemInit() (after the FPU, before DWT)
void system_init();

} // namespace chip
