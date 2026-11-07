#pragma once
/*
 * STM32H755 true random number generator (RM0399 ch. 34).
 *
 * Kernel clock: HSI48 (RCC_D2CCIP2R.RNGSEL = 00, the reset value). HSI48 must
 * already run (enableHSI48() in rcc.hpp, done by the Cortex-M7).
 * Also provides mbedtls_hardware_poll() for Mbed TLS (rng.cpp).
 */
#include <cstddef>
#include <cstdint>

namespace rng {

// Clock + enable. Returns false if HSI48 is not running.
bool init();

// One 32-bit random word. Returns false on a seed/clock error (the
// generator is restarted; call again).
bool read(uint32_t &out);

struct Stats {
	uint32_t words;
	uint32_t seed_errors;
	uint32_t clock_errors;
	uint32_t timeouts;
};
const Stats &stats();

} // namespace rng
