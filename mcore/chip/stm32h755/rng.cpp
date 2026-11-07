/*
 * STM32H755 RNG driver, see rng.hpp.
 */
#include <cstring>

#include "chip/stm32h755/rng.hpp"
#include "core/def.hpp"

namespace {
rng::Stats g_stats = {};
constexpr uint32_t DRDY_SPINS = 100000;   // a word takes ~ 40 RNG clocks

void restart() {
	// RM0399 "Error management": clear the flag, disable/enable the RNG
	RNG::_SR::SEIS::clear();
	RNG::_CR::RNGEN::clear();
	RNG::_CR::RNGEN::set();
}
} // namespace

namespace rng {

bool init() {
	if (RCC::_CR::RC48RDY::read() == 0U) {
		return false;
	}
	RCC::_D2CCIP2R::RNGSEL::write(0U);   // HSI48
	RCC::_AHB2ENR::RNGEN::set();
	(void) RCC::_AHB2ENR::read();
	RNG::_CR::CED::clear();              // clock error detection on
	RNG::_CR::RNGEN::set();
	return true;
}

bool read(uint32_t &out) {
	for (uint32_t i = 0; i < DRDY_SPINS; ++i) {
		const uint32_t sr = RNG::_SR::read();
		if ((sr & RNG::_SR::SECS::BitMsk) != 0U) {
			g_stats.seed_errors += 1;
			restart();
			return false;
		}
		if ((sr & RNG::_SR::CECS::BitMsk) != 0U) {
			g_stats.clock_errors += 1;
			RNG::_SR::CEIS::clear();
			return false;
		}
		if ((sr & RNG::_SR::DRDY::BitMsk) != 0U) {
			out = RNG::_DR::read();
			// RM0399: a zero word after a seed error must be discarded
			if (out == 0U) {
				continue;
			}
			g_stats.words += 1;
			return true;
		}
	}
	g_stats.timeouts += 1;
	return false;
}

const Stats &stats() {
	return g_stats;
}

} // namespace rng

// Mbed TLS entropy source (MBEDTLS_ENTROPY_HARDWARE_ALT)
extern "C" int mbedtls_hardware_poll(void *, unsigned char *output, size_t len, size_t *olen) {
	size_t done = 0;
	uint32_t failures = 0;
	while (done < len) {
		uint32_t w = 0;
		if (!rng::read(w)) {
			if (++failures > 8U) {
				break;   // report what we have; Mbed TLS asks again if short
			}
			continue;
		}
		const size_t n = (len - done) < sizeof(w) ? (len - done) : sizeof(w);
		memcpy(&output[done], &w, n);
		done += n;
	}
	*olen = done;
	return 0;
}
