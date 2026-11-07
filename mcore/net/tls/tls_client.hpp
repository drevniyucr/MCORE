/*
 * tls_client.hpp — non-blocking TLS 1.2 client on Mbed TLS 3.6.
 *
 * The byte transport is a pair of BIO callbacks with Mbed TLS conventions
 * (return bytes, or MBEDTLS_ERR_SSL_WANT_READ / _WANT_WRITE, or another
 * negative error). On the target that is a TCP connection of the MCORE
 * stack (tls_tcp.hpp); host tests plug in sockets or memory pipes.
 *
 * Call order: tls::platform_init() once, Client::setup() once,
 * then per connection start() -> handshake() until 0 -> read()/write().
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/pk.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"

namespace tls {

// ---------------------------------------------------------------------------
// Platform (tls_platform.cpp)
// ---------------------------------------------------------------------------
// All Mbed TLS allocations come from this arena. Call once, first.
void platform_init(uint8_t *arena, size_t size);

// Wall clock for certificate validity, in ms since 1970 (0 = unknown).
// Until a source reports a real time, handshakes fail certificate checks
// with MBEDTLS_X509_BADCERT_FUTURE, so wait for SNTP before connecting.
using UnixMsFn = uint64_t (*)();
void set_time_source(UnixMsFn fn);

// Arena statistics (bytes); peak needs MCORE_TLS_MEMORY_DEBUG.
size_t heap_used();
size_t heap_peak();

// ---------------------------------------------------------------------------
// Client
// ---------------------------------------------------------------------------
struct Credentials {
	const char *ca_pem;            // trusted CA(s), NUL-terminated PEM
	const char *client_cert_pem;   // optional: mutual TLS
	const char *client_key_pem;    // optional: key for client_cert_pem
};

using SendFn = int (*)(void *ctx, const unsigned char *buf, size_t len);
using RecvFn = int (*)(void *ctx, unsigned char *buf, size_t len);

class Client {
public:
	// Parses the certificates and seeds the DRBG from the hardware RNG.
	// server_name: host name or IP literal that the broker certificate must
	// match (also sent as SNI when it is a name). Returns 0 or an Mbed TLS
	// error code.
	int setup(const Credentials &cred, const char *server_name);

	// Frees everything setup() allocated (before a new setup()).
	void teardown();

	// New connection on a fresh transport. Offers the session of the last
	// successful handshake for resumption (the server may decline it and run
	// a full handshake instead).
	int start(void *bio_ctx, SendFn send, RecvFn recv);

	// The last completed handshake resumed a session (no certificate check,
	// no ECDHE)
	bool resumed() const { return resumed_; }
	// Do not offer the saved session any more (e.g. after a new CA)
	void forget_session();

	// 1 = in progress (call again), 0 = established, < 0 = failed.
	// With set_ecc_slice() the ECC steps (certificate and key exchange
	// signatures, ECDHE) return after a bounded amount of work; crypto_busy()
	// then says that the next call has CPU work to do (no network wait).
	int handshake();
	bool crypto_busy() const { return crypto_busy_; }
	int handshake_state() const;   // mbedtls_ssl_states, for profiling

	// > 0 bytes, 0 = would block (call again with the SAME data), < 0 error.
	int write(const uint8_t *data, size_t len);
	// > 0 bytes, 0 = nothing yet, < 0 closed or error.
	int read(uint8_t *buf, size_t len);

	void close_notify();

	int last_error() const { return last_error_; }
	uint32_t verify_flags() const;
	const char *ciphersuite() const;
	bool ready() const { return setup_done_; }

private:
	mbedtls_ssl_context ssl_;
	mbedtls_ssl_config conf_;
	mbedtls_x509_crt ca_;
	mbedtls_x509_crt own_cert_;
	mbedtls_pk_context own_key_;
	mbedtls_entropy_context entropy_;
	mbedtls_ctr_drbg_context drbg_;
	size_t pending_write_ = 0;     // length of a record that returned WANT_WRITE
	int last_error_ = 0;
	bool setup_done_ = false;
	bool crypto_busy_ = false;
	mbedtls_ssl_session saved_;      // last established session (ticket)
	bool have_saved_ = false;
	bool saw_certificate_ = false;   // this handshake checked a certificate
	bool resumed_ = false;
};

// Bounded ECC work per handshake() call (Mbed TLS "basic operations", see
// mbedtls_ecp_set_max_ops; 0 = unbounded). Global to all connections.
void set_ecc_slice(unsigned max_ops);

// Human-readable Mbed TLS error (MBEDTLS_ERROR_C), for logs/diagnostics.
void error_string(int err, char *buf, size_t len);

} // namespace tls
