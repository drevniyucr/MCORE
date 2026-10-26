/*
 * tls.hpp — platform glue for mbedTLS (see tls/mcore_mbedtls_config.h)
 *
 * Owns what mbedTLS needs from the chip: the RNG peripheral as entropy
 * source, one CTR-DRBG, a static memory arena and SysTick milliseconds.
 * Certificate dates use libc time() = the RTC (app/syscalls.c). The TLS session itself lives
 * in net/netclient.cpp.
 */
#pragma once

#include <cstddef>
#include <cstdint>

struct mbedtls_ctr_drbg_context;

// The only heap in MCORE, and only mbedTLS allocates from it. Measured peak
// on the bench (mTLS, P-256, 16 KB input record buffer): 25.7 KB. An
// application that also parses certificates beside an open session can
// enlarge it with MCORE_TLS_ARENA_SIZE (bytes).
#ifndef MCORE_TLS_ARENA_SIZE
#define MCORE_TLS_ARENA_SIZE (32 * 1024)
#endif
constexpr size_t TLS_ARENA_SIZE = MCORE_TLS_ARENA_SIZE;

// ECC work per handshake step (mbedtls_ecp_set_max_ops). Keeps every call
// into the handshake short enough for the superloop and the 1 s watchdog.
// An application whose time-critical work runs on a priority level
// (core/levels.hpp) can take larger steps, or 0 = no slicing, with
// MCORE_TLS_ECP_MAX_OPS: the handshake then finishes sooner.
#ifndef MCORE_TLS_ECP_MAX_OPS
#define MCORE_TLS_ECP_MAX_OPS 400
#endif
constexpr unsigned TLS_ECP_MAX_OPS = MCORE_TLS_ECP_MAX_OPS;

struct TlsCredentials {
	const uint8_t *ca = nullptr;       // DER of the CA that signed the broker (or several, back to back)
	size_t ca_len = 0;
	const uint8_t *cert = nullptr;     // optional client certificate (DER)
	size_t cert_len = 0;
	const uint8_t *key = nullptr;      // its private key (DER, P-256)
	size_t key_len = 0;
	const char *server_name = nullptr; // checked against the broker certificate (name or IP)
};

// Arena, clock and ECC slicing; idempotent, called by NetClient before TLS use.
void TLS_Init();

// The shared DRBG, seeded from the RNG on first use; nullptr if the RNG failed.
mbedtls_ctr_drbg_context* TLS_Drbg();

// Arena usage in bytes: now and the peak since boot.
size_t TLS_HeapUsed();
size_t TLS_HeapPeak();
