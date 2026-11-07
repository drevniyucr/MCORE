// Host tests for mcore/net/tls (TLS 1.2 client on Mbed TLS 3.6 with the
// MCORE configuration): our tls::Client against an Mbed TLS server over an
// in-memory pipe. Certificates: tests/certs (unit-test only).
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <chrono>
#include <cstring>
#include <ctime>
#include <deque>
#include <fstream>
#include <random>
#include <sstream>
#include <string>

#include "net/tls/tls_client.hpp"
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_ticket.h"
#include "mbedtls/x509.h"

// ---------------------------------------------------------------------------
// Platform stand-ins (the firmware has these in system.cpp and rng.cpp)
// ---------------------------------------------------------------------------
uint32_t get_tick() {
	using namespace std::chrono;
	return static_cast<uint32_t>(
		duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

extern "C" int mbedtls_hardware_poll(void *, unsigned char *out, size_t len, size_t *olen) {
	static std::random_device rd;
	for (size_t i = 0; i < len; ++i) {
		out[i] = static_cast<unsigned char>(rd());
	}
	*olen = len;
	return 0;
}

namespace {

uint64_t host_unix_ms() {
	return static_cast<uint64_t>(std::time(nullptr)) * 1000U;
}
uint64_t no_clock() {
	return 0;
}

std::string load(const char *name) {
	std::ifstream f(std::string(MCORE_TEST_CERTS_DIR) + "/" + name, std::ios::binary);
	REQUIRE_MESSAGE(f.good(), "missing tests/certs/", name, " (run tests/certs/gen.sh)");
	std::stringstream ss;
	ss << f.rdbuf();
	return ss.str();
}

// One TLS arena for the whole test binary (client + server live in it)
alignas(8) uint8_t g_arena[512 * 1024];
struct ArenaInit {
	ArenaInit() { tls::platform_init(g_arena, sizeof(g_arena)); }
} g_arena_init;

// --- in-memory pipe ---------------------------------------------------------
struct Pipe {
	std::deque<uint8_t> data;
	size_t cap = SIZE_MAX;
};
struct End {
	Pipe *out;
	Pipe *in;
};
int pipe_send(void *ctx, const unsigned char *b, size_t n) {
	Pipe *p = static_cast<End *>(ctx)->out;
	if (p->data.size() >= p->cap) {
		return MBEDTLS_ERR_SSL_WANT_WRITE;
	}
	const size_t k = std::min(n, p->cap - p->data.size());
	p->data.insert(p->data.end(), b, b + k);
	return static_cast<int>(k);
}
int pipe_recv(void *ctx, unsigned char *b, size_t n) {
	Pipe *p = static_cast<End *>(ctx)->in;
	if (p->data.empty()) {
		return MBEDTLS_ERR_SSL_WANT_READ;
	}
	const size_t k = std::min(n, p->data.size());
	for (size_t i = 0; i < k; ++i) {
		b[i] = p->data.front();
		p->data.pop_front();
	}
	return static_cast<int>(k);
}

// --- Mbed TLS test server ---------------------------------------------------
struct Server {
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt cert, ca;
	mbedtls_pk_context key;
	mbedtls_entropy_context ent;
	mbedtls_ctr_drbg_context drbg;
	mbedtls_ssl_ticket_context tickets;   // session tickets, own random key
	std::string cert_pem, key_pem, ca_pem;

	Server(const char *cert_file, const char *key_file, bool require_client_cert = false) {
		mbedtls_ssl_init(&ssl);
		mbedtls_ssl_config_init(&conf);
		mbedtls_x509_crt_init(&cert);
		mbedtls_x509_crt_init(&ca);
		mbedtls_pk_init(&key);
		mbedtls_entropy_init(&ent);
		mbedtls_ctr_drbg_init(&drbg);
		mbedtls_ssl_ticket_init(&tickets);
		REQUIRE(mbedtls_ctr_drbg_seed(&drbg, mbedtls_entropy_func, &ent, nullptr, 0) == 0);
		cert_pem = load(cert_file);
		key_pem = load(key_file);
		ca_pem = load("ca.crt");
		REQUIRE(mbedtls_x509_crt_parse(&cert, reinterpret_cast<const unsigned char *>(cert_pem.c_str()), cert_pem.size() + 1) == 0);
		REQUIRE(mbedtls_pk_parse_key(&key, reinterpret_cast<const unsigned char *>(key_pem.c_str()), key_pem.size() + 1,
				nullptr, 0, mbedtls_ctr_drbg_random, &drbg) == 0);
		REQUIRE(mbedtls_x509_crt_parse(&ca, reinterpret_cast<const unsigned char *>(ca_pem.c_str()), ca_pem.size() + 1) == 0);
		REQUIRE(mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER,
				MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT) == 0);
		mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &drbg);
		REQUIRE(mbedtls_ssl_conf_own_cert(&conf, &cert, &key) == 0);
		mbedtls_ssl_conf_ca_chain(&conf, &ca, nullptr);
		mbedtls_ssl_conf_authmode(&conf, require_client_cert ? MBEDTLS_SSL_VERIFY_REQUIRED
		                                                     : MBEDTLS_SSL_VERIFY_NONE);
		REQUIRE(mbedtls_ssl_ticket_setup(&tickets, mbedtls_ctr_drbg_random, &drbg, MBEDTLS_CIPHER_AES_256_GCM,
		                                 86400) == 0);
		mbedtls_ssl_conf_session_tickets_cb(&conf, mbedtls_ssl_ticket_write, mbedtls_ssl_ticket_parse, &tickets);
		REQUIRE(mbedtls_ssl_setup(&ssl, &conf) == 0);
	}
	~Server() {
		mbedtls_ssl_free(&ssl);
		mbedtls_ssl_ticket_free(&tickets);
		mbedtls_ssl_config_free(&conf);
		mbedtls_x509_crt_free(&cert);
		mbedtls_x509_crt_free(&ca);
		mbedtls_pk_free(&key);
		mbedtls_ctr_drbg_free(&drbg);
		mbedtls_entropy_free(&ent);
	}
};

struct Session {
	Pipe c2s, s2c;
	End client_end{ &c2s, &s2c };
	End server_end{ &s2c, &c2s };
	tls::Client client;
	std::string ca_pem, client_cert, client_key;

	int setup(const char *server_name, bool with_client_cert = false, const char *ca = "ca.crt") {
		ca_pem = load(ca);
		tls::Credentials cred{ ca_pem.c_str(), nullptr, nullptr };
		if (with_client_cert) {
			client_cert = load("client.crt");
			client_key = load("client.key");
			cred.client_cert_pem = client_cert.c_str();
			cred.client_key_pem = client_key.c_str();
		}
		const int ret = client.setup(cred, server_name);
		if (ret == 0) {
			REQUIRE(client.start(&client_end, pipe_send, pipe_recv) == 0);
		}
		return ret;
	}
	~Session() { client.teardown(); }

	// Same client, new TCP connection (to the same or another server)
	void reconnect(Server &srv) {
		c2s.data.clear();
		s2c.data.clear();
		REQUIRE(mbedtls_ssl_session_reset(&srv.ssl) == 0);
		REQUIRE(client.start(&client_end, pipe_send, pipe_recv) == 0);
	}
	int crypto_steps = 0;   // handshake() calls that returned with ECC work pending

	// Returns the client's final handshake result (0 = success)
	int handshake(Server &srv) {
		mbedtls_ssl_set_bio(&srv.ssl, &server_end, pipe_send, pipe_recv, nullptr);
		int c = 1, s = MBEDTLS_ERR_SSL_WANT_READ;
		crypto_steps = 0;
		for (int i = 0; i < 100000; ++i) {
			if (c == 1) {
				c = client.handshake();
				crypto_steps += client.crypto_busy() ? 1 : 0;
			}
			if (c < 0) {
				return c;                            // client gave up (alert or own check)
			}
			if (s == MBEDTLS_ERR_SSL_WANT_READ || s == MBEDTLS_ERR_SSL_WANT_WRITE) {
				s = mbedtls_ssl_handshake(&srv.ssl);
			}
			if (c == 0 && s == 0) {
				return 0;
			}
			if (c == 0 && s != MBEDTLS_ERR_SSL_WANT_READ && s != MBEDTLS_ERR_SSL_WANT_WRITE) {
				return -2;                           // client done, server refused
			}
		}
		return -1;
	}
};

} // namespace

TEST_CASE("TLS: ECDSA broker certificate, then data both ways") {
	tls::set_time_source(host_unix_ms);
	Server srv("server.crt", "server.key");
	Session s;
	REQUIRE(s.setup("broker.test") == 0);
	REQUIRE(s.handshake(srv) == 0);
	CHECK(std::string(s.client.ciphersuite()).find("ECDHE-ECDSA") != std::string::npos);
	CHECK(std::string(s.client.ciphersuite()).find("GCM") != std::string::npos);
	CHECK(s.client.verify_flags() == 0);

	const char msg[] = "MQTT bytes";
	CHECK(s.client.write(reinterpret_cast<const uint8_t *>(msg), sizeof(msg)) == static_cast<int>(sizeof(msg)));
	unsigned char got[64] = {};
	REQUIRE(mbedtls_ssl_read(&srv.ssl, got, sizeof(got)) == static_cast<int>(sizeof(msg)));
	CHECK(memcmp(got, msg, sizeof(msg)) == 0);

	const char reply[] = "CONNACK";
	REQUIRE(mbedtls_ssl_write(&srv.ssl, reinterpret_cast<const unsigned char *>(reply), sizeof(reply)) > 0);
	uint8_t back[64] = {};
	CHECK(s.client.read(back, sizeof(back)) == static_cast<int>(sizeof(reply)));
	CHECK(memcmp(back, reply, sizeof(reply)) == 0);
	CHECK(s.client.read(back, sizeof(back)) == 0);   // nothing more: would block
}

TEST_CASE("TLS: restartable ECC splits the ECDHE-ECDSA handshake into slices") {
	tls::set_time_source(host_unix_ms);
	Server srv("server.crt", "server.key");
	Session s;
	REQUIRE(s.setup("broker.test") == 0);
	tls::set_ecc_slice(200);
	const int r = s.handshake(srv);
	tls::set_ecc_slice(0);
	REQUIRE(r == 0);
	CHECK(std::string(s.client.ciphersuite()).find("ECDHE-ECDSA") != std::string::npos);
	CHECK(s.crypto_steps > 10);          // certificate, ServerKeyExchange, ECDHE in pieces
	CHECK_FALSE(s.client.crypto_busy());
}

TEST_CASE("TLS: a reconnect resumes the session with a ticket, without ECC") {
	tls::set_time_source(host_unix_ms);
	Server srv("server.crt", "server.key");
	Session s;
	REQUIRE(s.setup("broker.test") == 0);
	REQUIRE(s.handshake(srv) == 0);
	CHECK_FALSE(s.client.resumed());

	s.reconnect(srv);
	tls::set_ecc_slice(200);
	const int r = s.handshake(srv);
	tls::set_ecc_slice(0);
	REQUIRE(r == 0);
	CHECK(s.client.resumed());
	CHECK(s.crypto_steps == 0);                // no certificate check, no ECDHE

	const char msg[] = "after resume";
	CHECK(s.client.write(reinterpret_cast<const uint8_t *>(msg), sizeof(msg)) == static_cast<int>(sizeof(msg)));
	unsigned char got[32] = {};
	REQUIRE(mbedtls_ssl_read(&srv.ssl, got, sizeof(got)) == static_cast<int>(sizeof(msg)));
	CHECK(memcmp(got, msg, sizeof(msg)) == 0);

	// A server that cannot read the ticket (restarted broker): full handshake
	Server other("server.crt", "server.key");
	s.reconnect(other);
	REQUIRE(s.handshake(other) == 0);
	CHECK_FALSE(s.client.resumed());
}

TEST_CASE("TLS: RSA broker certificate uses ECDHE-RSA") {
	tls::set_time_source(host_unix_ms);
	Server srv("server_rsa.crt", "server_rsa.key");
	Session s;
	REQUIRE(s.setup("broker.test") == 0);
	REQUIRE(s.handshake(srv) == 0);
	CHECK(std::string(s.client.ciphersuite()).find("ECDHE-RSA") != std::string::npos);
}

TEST_CASE("TLS: broker addressed by IP literal matches the iPAddress SAN") {
	tls::set_time_source(host_unix_ms);
	Server srv("server.crt", "server.key");
	Session s;
	REQUIRE(s.setup("10.0.0.1") == 0);
	CHECK(s.handshake(srv) == 0);
}

TEST_CASE("TLS: wrong host name is rejected") {
	tls::set_time_source(host_unix_ms);
	Server srv("server.crt", "server.key");
	Session s;
	REQUIRE(s.setup("evil.test") == 0);
	CHECK(s.handshake(srv) == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED);
	CHECK((s.client.verify_flags() & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0);
}

TEST_CASE("TLS: certificate from an untrusted CA is rejected") {
	tls::set_time_source(host_unix_ms);
	Server srv("rogue.crt", "rogue.key");
	Session s;
	REQUIRE(s.setup("broker.test") == 0);
	CHECK(s.handshake(srv) == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED);
	CHECK((s.client.verify_flags() & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0);
}

TEST_CASE("TLS: without wall-clock time the certificate is 'not yet valid'") {
	tls::set_time_source(no_clock);
	Server srv("server.crt", "server.key");
	Session s;
	REQUIRE(s.setup("broker.test") == 0);
	CHECK(s.handshake(srv) == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED);
	CHECK((s.client.verify_flags() & MBEDTLS_X509_BADCERT_FUTURE) != 0);
	tls::set_time_source(host_unix_ms);
}

TEST_CASE("TLS: mutual TLS - broker demands a client certificate") {
	tls::set_time_source(host_unix_ms);
	{
		Server srv("server.crt", "server.key", /*require_client_cert=*/true);
		Session s;
		REQUIRE(s.setup("broker.test", /*with_client_cert=*/true) == 0);
		CHECK(s.handshake(srv) == 0);
		CHECK(mbedtls_ssl_get_verify_result(&srv.ssl) == 0);
	}
	{
		Server srv("server.crt", "server.key", true);
		Session s;
		REQUIRE(s.setup("broker.test", false) == 0);
		CHECK(s.handshake(srv) != 0);
	}
}

TEST_CASE("TLS write: WANT_WRITE is retried with the same data, nothing is lost") {
	tls::set_time_source(host_unix_ms);
	Server srv("server.crt", "server.key");
	Session s;
	REQUIRE(s.setup("broker.test") == 0);
	REQUIRE(s.handshake(srv) == 0);

	std::string big(3000, '\0');
	for (size_t i = 0; i < big.size(); ++i) {
		big[i] = static_cast<char>('a' + i % 26);
	}
	s.c2s.cap = 256;                                  // transport takes 256 bytes at a time
	const auto *p = reinterpret_cast<const uint8_t *>(big.data());
	size_t sent = 0;
	std::string received;
	for (int guard = 0; guard < 200 && (sent < big.size() || received.size() < big.size()); ++guard) {
		if (sent < big.size()) {
			// MQTT-style caller: unsent data stays at the front, more may be appended
			const int n = s.client.write(p + sent, big.size() - sent);
			REQUIRE(n >= 0);
			sent += static_cast<size_t>(n);
		}
		unsigned char buf[1024];
		const int r = mbedtls_ssl_read(&srv.ssl, buf, sizeof(buf));
		if (r > 0) {
			received.append(reinterpret_cast<char *>(buf), static_cast<size_t>(r));
		}
	}
	CHECK(sent == big.size());
	CHECK(received == big);
}
