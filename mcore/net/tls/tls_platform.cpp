/*
 * tls_platform.cpp — Mbed TLS platform hooks for MCORE (see tls_client.hpp).
 *
 * Entropy (mbedtls_hardware_poll) is chip-specific and lives elsewhere:
 * mcore/chips/stm32h755/chip/rng.cpp on the target, a stub in host tests.
 */
#include "net/tls/tls_client.hpp"
#include "core/system.hpp"

#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_time.h"

namespace {
tls::UnixMsFn g_time_source = nullptr;

mbedtls_time_t wall_clock(mbedtls_time_t *out) {
	const uint64_t ms = (g_time_source != nullptr) ? g_time_source() : 0U;
	const mbedtls_time_t t = static_cast<mbedtls_time_t>(ms / 1000U);
	if (out != nullptr) {
		*out = t;
	}
	return t;
}
} // namespace

// Monotonic milliseconds (MBEDTLS_PLATFORM_MS_TIME_ALT)
extern "C" mbedtls_ms_time_t mbedtls_ms_time(void) {
	return static_cast<mbedtls_ms_time_t>(get_tick());
}

namespace tls {

void platform_init(uint8_t *arena, size_t size) {
	mbedtls_memory_buffer_alloc_init(arena, size);
	mbedtls_platform_set_time(wall_clock);
}

void set_time_source(UnixMsFn fn) {
	g_time_source = fn;
}

size_t heap_used() {
#if defined(MBEDTLS_MEMORY_DEBUG)
	size_t cur_used = 0, cur_blocks = 0;
	mbedtls_memory_buffer_alloc_cur_get(&cur_used, &cur_blocks);
	return cur_used;
#else
	return 0;
#endif
}

size_t heap_peak() {
#if defined(MBEDTLS_MEMORY_DEBUG)
	size_t max_used = 0, max_blocks = 0;
	mbedtls_memory_buffer_alloc_max_get(&max_used, &max_blocks);
	return max_used;
#else
	return 0;
#endif
}

} // namespace tls
