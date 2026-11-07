/*
 * On-target TLS self-test (build with -DMCORE_TLS_SELFTEST=ON).
 *
 * Runs one complete TLS 1.2 handshake between tls::Client and an Mbed TLS
 * server, both on this Cortex-M4, over in-memory pipes, then exchanges a
 * record each way. Measures the client and server CPU time separately (DWT
 * cycles) and the arena high-water mark. Uses the unit-test certificates
 * (tests/certs) and the build time as "now". Results: symbol tls_selftest.
 */
#include "tls_selftest.hpp"

#if defined(MCORE_TLS_SELFTEST)
#include <cstring>

#include "core/system.hpp"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "mbedtls/ssl.h"
#include "net/sntp.hpp"
#include "net/tls/tls_client.hpp"
#include "selftest_SELFTEST_CA_PEM.h"
#include "selftest_SELFTEST_SERVER_CRT.h"
#include "selftest_SELFTEST_SERVER_KEY.h"

TlsSelftestResult tls_selftest;

namespace {

struct Fifo {
	uint8_t buf[6144];
	size_t head, tail, count;
};
Fifo c2s, s2c;
struct End {
	Fifo *out;
	Fifo *in;
};
End client_end{ &c2s, &s2c };
End server_end{ &s2c, &c2s };

int fifo_send(void *ctx, const unsigned char *b, size_t n) {
	Fifo *f = static_cast<End *>(ctx)->out;
	size_t k = 0;
	while (k < n && f->count < sizeof(f->buf)) {
		f->buf[f->head] = b[k++];
		f->head = (f->head + 1U) % sizeof(f->buf);
		f->count += 1;
	}
	return k == 0 ? MBEDTLS_ERR_SSL_WANT_WRITE : static_cast<int>(k);
}
int fifo_recv(void *ctx, unsigned char *b, size_t n) {
	Fifo *f = static_cast<End *>(ctx)->in;
	size_t k = 0;
	while (k < n && f->count > 0U) {
		b[k++] = f->buf[f->tail];
		f->tail = (f->tail + 1U) % sizeof(f->buf);
		f->count -= 1;
	}
	return k == 0 ? MBEDTLS_ERR_SSL_WANT_READ : static_cast<int>(k);
}

uint64_t build_time_ms() {
	return MCORE_BUILD_UNIX_TIME * 1000ULL;
}

} // namespace

void run_tls_selftest() {
	TlsSelftestResult &r = tls_selftest;
	r = {};
	c2s = {};
	s2c = {};
	tls::set_time_source(build_time_ms);
	mbedtls_memory_buffer_alloc_max_reset();
	const uint32_t t0 = get_tick();

	// --- server -------------------------------------------------------------
	static mbedtls_ssl_context sssl;
	static mbedtls_ssl_config sconf;
	static mbedtls_x509_crt scert;
	static mbedtls_pk_context skey;
	static mbedtls_entropy_context sent;
	static mbedtls_ctr_drbg_context sdrbg;
	mbedtls_ssl_init(&sssl);
	mbedtls_ssl_config_init(&sconf);
	mbedtls_x509_crt_init(&scert);
	mbedtls_pk_init(&skey);
	mbedtls_entropy_init(&sent);
	mbedtls_ctr_drbg_init(&sdrbg);
	int ret = mbedtls_ctr_drbg_seed(&sdrbg, mbedtls_entropy_func, &sent, nullptr, 0);
	if (ret == 0) ret = mbedtls_x509_crt_parse(&scert, reinterpret_cast<const unsigned char *>(SELFTEST_SERVER_CRT), sizeof(SELFTEST_SERVER_CRT));
	if (ret == 0) ret = mbedtls_pk_parse_key(&skey, reinterpret_cast<const unsigned char *>(SELFTEST_SERVER_KEY), sizeof(SELFTEST_SERVER_KEY), nullptr, 0, mbedtls_ctr_drbg_random, &sdrbg);
	if (ret == 0) ret = mbedtls_ssl_config_defaults(&sconf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
	if (ret == 0) {
		mbedtls_ssl_conf_rng(&sconf, mbedtls_ctr_drbg_random, &sdrbg);
		ret = mbedtls_ssl_conf_own_cert(&sconf, &scert, &skey);
	}
	if (ret == 0) ret = mbedtls_ssl_setup(&sssl, &sconf);
	mbedtls_ssl_set_bio(&sssl, &server_end, fifo_send, fifo_recv, nullptr);

	// --- client (the code under test) ---------------------------------------
	static tls::Client client;
	if (ret == 0) ret = client.setup({ SELFTEST_CA_PEM, nullptr, nullptr }, "broker.test");
	if (ret == 0) ret = client.start(&client_end, fifo_send, fifo_recv);

	// --- handshake, CPU time per side -----------------------------------------
	int c = 1, s = MBEDTLS_ERR_SSL_WANT_READ;
	for (int i = 0; ret == 0 && i < 200 && !(c == 0 && s == 0); ++i) {
		if (c == 1) {
			const uint32_t a = DWT::_CYCCNT::read();
			c = client.handshake();
			r.client_cycles += DWT::_CYCCNT::read() - a;
		}
		if (c < 0) {
			ret = c;
			break;
		}
		if (s == MBEDTLS_ERR_SSL_WANT_READ || s == MBEDTLS_ERR_SSL_WANT_WRITE) {
			const uint32_t a = DWT::_CYCCNT::read();
			s = mbedtls_ssl_handshake(&sssl);
			r.server_cycles += DWT::_CYCCNT::read() - a;
			if (s != 0 && s != MBEDTLS_ERR_SSL_WANT_READ && s != MBEDTLS_ERR_SSL_WANT_WRITE) {
				ret = s;
			}
		}
	}
	if (ret == 0 && !(c == 0 && s == 0)) {
		ret = -1;
	}

	// --- one record each way ----------------------------------------------------
	if (ret == 0) {
		static const uint8_t msg[] = "MQTT CONNECT";
		uint8_t buf[32] = {};
		if (client.write(msg, sizeof(msg)) != static_cast<int>(sizeof(msg)) ||
		    mbedtls_ssl_read(&sssl, buf, sizeof(buf)) != static_cast<int>(sizeof(msg)) ||
		    memcmp(buf, msg, sizeof(msg)) != 0) {
			ret = -2;
		}
		if (ret == 0 && (mbedtls_ssl_write(&sssl, msg, sizeof(msg)) != static_cast<int>(sizeof(msg)) ||
		                 client.read(buf, sizeof(buf)) != static_cast<int>(sizeof(msg)))) {
			ret = -3;
		}
	}

	r.result = ret;
	r.verify_flags = client.verify_flags();
	r.total_ms = get_tick() - t0;
	r.heap_peak = static_cast<uint32_t>(tls::heap_peak());
	strncpy(r.ciphersuite, client.ciphersuite(), sizeof(r.ciphersuite) - 1U);

	client.teardown();
	mbedtls_ssl_free(&sssl);
	mbedtls_ssl_config_free(&sconf);
	mbedtls_x509_crt_free(&scert);
	mbedtls_pk_free(&skey);
	mbedtls_ctr_drbg_free(&sdrbg);
	mbedtls_entropy_free(&sent);
	mbedtls_memory_buffer_alloc_max_reset();
	tls::set_time_source(NET_SNTP_UnixMs);
}

#else
void run_tls_selftest() {}
#endif
