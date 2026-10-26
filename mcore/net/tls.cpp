/*
 * tls.cpp — mbedTLS platform glue: entropy, DRBG, arena, clock
 */
#include "net/tls.hpp"

#include "core/regs.hpp"
#include "core/system.hpp"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls/platform_time.h"

namespace {

constexpr uint32_t RNG_TIMEOUT_POLLS = 100000;

alignas(8) uint8_t arena[TLS_ARENA_SIZE];
bool initialised = false;

mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context drbg;
enum class DrbgState : uint8_t { Unseeded, Ready, Failed } drbg_state = DrbgState::Unseeded;

} // namespace

// ---- RNG peripheral as the entropy source (MBEDTLS_ENTROPY_HARDWARE_ALT) ----
// The prototype lives in a private mbedTLS header.
extern "C" int mbedtls_hardware_poll(void *data, unsigned char *out, size_t len, size_t *olen);

extern "C" int mbedtls_hardware_poll(void *, unsigned char *out, size_t len, size_t *olen) {
	if (!RCC::_AHB2ENR::RNGEN::is_set()) {
		RCC::_AHB2ENR::RNGEN::set();                   // clocked from CK48 = VCO / PLLQ = 48 MHz
		(void)RCC::_AHB2ENR::read();
		RNG::_CR::RNGEN::set();
	}
	size_t n = 0;
	while (n < len) {
		uint32_t polls = 0;
		while (!RNG::_SR::DRDY::is_set()) {
			const uint32_t sr = RNG::_SR::read();
			if ((sr & (RNG::_SR::SECS::BitMsk | RNG::_SR::CECS::BitMsk)) || ++polls > RNG_TIMEOUT_POLLS) {
				// seed or clock error: restart the generator, report what we have (RM0410 22.3.5)
				RNG::_CR::RNGEN::clear();
				RNG::_SR::overwrite(sr & ~(RNG::_SR::SEIS::BitMsk | RNG::_SR::CEIS::BitMsk));
				RNG::_CR::RNGEN::set();
				*olen = n;
				return n ? 0 : MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
			}
		}
		uint32_t v = RNG::_DR::read();
		for (int i = 0; i < 4 && n < len; ++i, v >>= 8) out[n++] = static_cast<unsigned char>(v);
	}
	*olen = n;
	return 0;
}

// ---- milliseconds for mbedTLS (MBEDTLS_PLATFORM_MS_TIME_ALT); seconds come
// from libc time(), which app/syscalls.c serves from the RTC ----
extern "C" mbedtls_ms_time_t mbedtls_ms_time(void) {
	return static_cast<mbedtls_ms_time_t>(get_tick());
}

void TLS_Init() {
	if (initialised) return;
	initialised = true;
	mbedtls_memory_buffer_alloc_init(arena, sizeof(arena));
	mbedtls_ecp_set_max_ops(TLS_ECP_MAX_OPS);
}

mbedtls_ctr_drbg_context* TLS_Drbg() {
	TLS_Init();
	if (drbg_state == DrbgState::Unseeded) {
		mbedtls_entropy_init(&entropy);
		mbedtls_ctr_drbg_init(&drbg);
		static const char pers[] = "mcore-tls";
		const int r = mbedtls_ctr_drbg_seed(&drbg, mbedtls_entropy_func, &entropy,
		                                    reinterpret_cast<const unsigned char*>(pers), sizeof(pers) - 1);
		drbg_state = (r == 0) ? DrbgState::Ready : DrbgState::Failed;
	}
	return drbg_state == DrbgState::Ready ? &drbg : nullptr;
}

size_t TLS_HeapUsed() {
	size_t used = 0, blocks = 0;
	mbedtls_memory_buffer_alloc_cur_get(&used, &blocks);
	return used;
}

size_t TLS_HeapPeak() {
	size_t peak = 0, blocks = 0;
	mbedtls_memory_buffer_alloc_max_get(&peak, &blocks);
	return peak;
}
