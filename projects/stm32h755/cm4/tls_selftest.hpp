#pragma once
#include <cstdint>

// Filled by run_tls_selftest() when built with -DMCORE_TLS_SELFTEST=ON.
struct TlsSelftestResult {
	int32_t result;            // 0 = handshake + data exchange OK
	uint32_t verify_flags;
	uint32_t client_cycles;    // DWT cycles spent in the client
	uint32_t server_cycles;    // DWT cycles spent in the (test) server
	uint32_t total_ms;
	uint32_t heap_peak;        // arena high-water mark, client + server
	char ciphersuite[48];
};

#if defined(MCORE_TLS_SELFTEST)
extern TlsSelftestResult tls_selftest;
#endif

// No-op unless MCORE_TLS_SELFTEST. Call after mqtt_link::init().
void run_tls_selftest();
