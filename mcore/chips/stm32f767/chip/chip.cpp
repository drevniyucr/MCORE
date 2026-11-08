/*
 * chip.cpp — STM32F767 hooks of the core startup code.
 */
#include "chip/chip.hpp"

namespace chip {

void system_init() {
	// nothing chip-specific: the F7 keeps its debug clocks running
}

} // namespace chip
