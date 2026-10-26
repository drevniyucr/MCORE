/*
 * rng.hpp — hardware random number generator (analog noise, 32-bit words)
 *
 *   uint32_t r;
 *   if (RNG_Read(r) == DrvStatus::Ok) { ... }
 *
 * Clocked from CK48 (VCO / PLLQ), which must be 48 MHz (rcc.hpp checks it).
 * The single source of randomness in MCORE: mbedTLS entropy (net/tls.cpp)
 * and NET_Entropy() — TCP ISN, ephemeral ports, DNS ids, NTP nonces, DHCP xid.
 */
#pragma once

#include <cstdint>
#include "core/regs.hpp"
#include "drivers/common.hpp"

constexpr uint32_t RNG_TIMEOUT_POLLS = 100000;   // a word takes ~40 RNG clocks; this is ~2 ms

// Clock and enable; idempotent (RNG_Read calls it).
inline void RNG_Init() noexcept {
	if (!RCC::_AHB2ENR::RNGEN::is_set()) {
		RCC::_AHB2ENR::RNGEN::set();
		(void)RCC::_AHB2ENR::read();               // let the clock settle before touching RNG
	}
	if (!RNG::_CR::RNGEN::is_set()) {
		RNG::_CR::RNGEN::set();
	}
}

// One 32-bit random word. Error on a seed or clock fault (the generator is
// restarted, RM0410 22.3.5 — the next call may succeed), Timeout if no data.
inline DrvStatus RNG_Read(uint32_t &out) noexcept {
	RNG_Init();
	uint32_t polls = 0;
	while (!RNG::_SR::DRDY::is_set()) {
		const uint32_t sr = RNG::_SR::read();
		if (sr & (RNG::_SR::SECS::BitMsk | RNG::_SR::CECS::BitMsk)) {
			RNG::_CR::RNGEN::clear();
			RNG::_SR::overwrite(sr & ~(RNG::_SR::SEIS::BitMsk | RNG::_SR::CEIS::BitMsk));  // rc_w0
			RNG::_CR::RNGEN::set();
			return DrvStatus::Error;
		}
		if (++polls > RNG_TIMEOUT_POLLS) {
			return DrvStatus::Timeout;
		}
	}
	out = RNG::_DR::read();
	return DrvStatus::Ok;
}
