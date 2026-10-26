/*
 * tls.cpp — mbedTLS platform glue: entropy, DRBG, arena, clock
 */
#include "net/tls.hpp"

#include "core/regs.hpp"
#include "core/system.hpp"
#include "drivers/rng.hpp"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls/platform_time.h"

namespace {

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
	size_t n = 0;
	while (n < len) {
		uint32_t v;
		if (RNG_Read(v) != DrvStatus::Ok) {
			*olen = n;                                 // report what we have
			return n ? 0 : MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
		}
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
#if defined(MBEDTLS_ECP_RESTARTABLE)
	mbedtls_ecp_set_max_ops(TLS_ECP_MAX_OPS);     // without it the handshake runs in one piece
#endif
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
