#pragma once
// The STM32H755 runs one image per core; code shared by both asks which core
// it is running on (SCB CPUID part number), instead of a build define.
#include "core/def.hpp"

namespace chip {

constexpr uint32_t CPUID_PART_CM4 = 0xC24U;

// No global state: usable before .data/.bss are initialised (ExitRun0Mode)
inline bool on_cm4() {
	return ID::_CPUID::Partno::read() == CPUID_PART_CM4;
}

} // namespace chip
