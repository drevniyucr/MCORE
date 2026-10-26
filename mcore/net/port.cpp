/*
 * port.cpp — hardware-backed implementations of net-stack seams.
 * Host tests link their own stubs instead of this file.
 */
#include "net/net.hpp"
#include "drivers/rng.hpp"

// Times the RNG failed and NET_Entropy fell back (observable via debugger)
uint32_t net_entropy_fallbacks = 0;

// TCP ISNs, ephemeral ports, DNS ids, NTP nonces and DHCP xids must not be
// guessable by an off-path attacker: they come from the hardware RNG. If it
// ever fails, the stack keeps working on a SysTick-seeded xorshift (weak, but
// counted) instead of stalling.
uint32_t NET_Entropy() {
	uint32_t r;
	if (RNG_Read(r) == DrvStatus::Ok) {
		return r;
	}
	net_entropy_fallbacks++;
	static uint32_t x = 0x9E3779B9;
	x ^= SysTick::_STCVR::read();
	x ^= x << 13; x ^= x >> 17; x ^= x << 5;
	return x;
}
