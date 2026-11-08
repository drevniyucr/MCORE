/*
 * chip.cpp — STM32H755 hooks of the core startup code.
 */
#include "chip/chip.hpp"
#include "core/def.hpp"

namespace chip {

void system_init() {
	// Keep the D1/D3 debug clocks and the trace clock running so ITM/SWO
	// and the debugger keep working (the H7 gates them by default).
	DBGMCU::_CR::setMask<DBGMCU::_CR::D1DBGCKEN::BitMsk |
	                     DBGMCU::_CR::D3DBGCKEN::BitMsk |
	                     DBGMCU::_CR::TRACECLKEN::BitMsk>();
}

} // namespace chip
