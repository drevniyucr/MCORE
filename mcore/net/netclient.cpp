/*
 * netclient.cpp — DNS -> ARP -> TCP -> TLS state machine, TX queue
 */
#include "net/netclient.hpp"

#include <cstdio>
#include <cstring>

#include "core/system.hpp"
#include "net/dns.hpp"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/pk.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"

namespace {

// One TLS session for the whole firmware: ~1 KB of contexts plus the arena.
struct TlsSession {
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt ca;
	mbedtls_x509_crt own;
	mbedtls_pk_context key;
	NetClient *owner = nullptr;
} tls;

int bio_send_cb(void *ctx, const unsigned char *buf, size_t len) {
	return static_cast<NetClient*>(ctx)->bio_send(buf, len);
}
int bio_recv_cb(void *ctx, unsigned char *buf, size_t len) {
	return static_cast<NetClient*>(ctx)->bio_recv(buf, len);
}

bool in_progress(int r) {
	return r == MBEDTLS_ERR_SSL_WANT_READ || r == MBEDTLS_ERR_SSL_WANT_WRITE ||
	       r == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS;
}

} // namespace

void NetClient::open(const NetClientConfig &cfg) {
	release();
	cfg_ = cfg;
	tls_ = cfg.tls != nullptr;
	tx_head_ = tx_tail_ = tx_count_ = tls_pending_ = 0;
	error_[0] = '\0';
	handshake_ms_ = 0;
	opened_at_ = get_tick();
	if (cfg.host == nullptr || cfg.port == 0) {
		fail("no host/port");
		return;
	}
	if (tls_ && tls.owner != nullptr && tls.owner != this) {
		fail("TLS session in use by another NetClient");
		return;
	}
	state_ = netclient_state_t::RESOLVING;
}

void NetClient::fail(const char *what, int err) {
	if (err != 0) {
		char why[64];
		mbedtls_strerror(err, why, sizeof(why));
		snprintf(error_, sizeof(error_), "%s: %s (-0x%04X)", what, why, static_cast<unsigned>(-err));
	} else {
		snprintf(error_, sizeof(error_), "%s", what);
	}
	release();
	state_ = netclient_state_t::FAILED;
}

void NetClient::release() {
	if (tls.owner == this) {
		mbedtls_ssl_free(&tls.ssl);
		mbedtls_ssl_config_free(&tls.conf);
		mbedtls_x509_crt_free(&tls.ca);
		mbedtls_x509_crt_free(&tls.own);
		mbedtls_pk_free(&tls.key);
		tls.owner = nullptr;
	}
	if (tcp_alive()) {
		NET_TCP_Abort(conn_);
	}
	conn_ = nullptr;
	local_port_ = 0;
}

void NetClient::close() {
	if (state_ == netclient_state_t::UP) {
		if (tls_) {
			(void)mbedtls_ssl_close_notify(&tls.ssl);  // best effort, one try
		}
		if (tcp_alive()) {
			NET_TCP_Close(conn_);                       // FIN; the slot finishes on its own
		}
		conn_ = nullptr;
	}
	release();
	state_ = netclient_state_t::IDLE;
}

bool NetClient::tls_setup() {
	TLS_Init();
	mbedtls_ctr_drbg_context *drbg = TLS_Drbg();
	if (drbg == nullptr) {
		fail("RNG");
		return false;
	}
	tls.owner = this;
	mbedtls_ssl_init(&tls.ssl);
	mbedtls_ssl_config_init(&tls.conf);
	mbedtls_x509_crt_init(&tls.ca);
	mbedtls_x509_crt_init(&tls.own);
	mbedtls_pk_init(&tls.key);

	const TlsCredentials &c = *cfg_.tls;
	int r = mbedtls_x509_crt_parse_der(&tls.ca, c.ca, c.ca_len);
	if (r != 0) { fail("CA certificate", r); return false; }
	if (c.cert != nullptr) {
		if ((r = mbedtls_x509_crt_parse_der(&tls.own, c.cert, c.cert_len)) != 0) {
			fail("client certificate", r);
			return false;
		}
		if ((r = mbedtls_pk_parse_key(&tls.key, c.key, c.key_len, nullptr, 0,
		                              mbedtls_ctr_drbg_random, drbg)) != 0) {
			fail("client key", r);
			return false;
		}
	}
	if ((r = mbedtls_ssl_config_defaults(&tls.conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
	                                     MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
		fail("TLS config", r);
		return false;
	}
	mbedtls_ssl_conf_authmode(&tls.conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_ca_chain(&tls.conf, &tls.ca, nullptr);
	if (c.cert != nullptr && (r = mbedtls_ssl_conf_own_cert(&tls.conf, &tls.own, &tls.key)) != 0) {
		fail("certificate and key do not match", r);
		return false;
	}
	mbedtls_ssl_conf_rng(&tls.conf, mbedtls_ctr_drbg_random, drbg);
	// Ask for records of at most 4 KB; servers may ignore it (OpenSSL does),
	// so MBEDTLS_SSL_IN_CONTENT_LEN still holds a full 16 KB record
	mbedtls_ssl_conf_max_frag_len(&tls.conf, MBEDTLS_SSL_MAX_FRAG_LEN_4096);
	if ((r = mbedtls_ssl_setup(&tls.ssl, &tls.conf)) != 0) { fail("TLS setup", r); return false; }
	if ((r = mbedtls_ssl_set_hostname(&tls.ssl, c.server_name ? c.server_name : cfg_.host)) != 0) {
		fail("TLS server name", r);
		return false;
	}
	mbedtls_ssl_set_bio(&tls.ssl, this, bio_send_cb, bio_recv_cb, nullptr);
	return true;
}

void NetClient::poll() {
	const bool opening = state_ == netclient_state_t::RESOLVING ||
	                     state_ == netclient_state_t::CONNECTING ||
	                     state_ == netclient_state_t::HANDSHAKE;
	if (opening && get_tick() - opened_at_ > NETCLIENT_OPEN_TIMEOUT_MS) {
		fail(state_ == netclient_state_t::HANDSHAKE ? "TLS handshake timeout" : "connect timeout");
		return;
	}

	switch (state_) {
	case netclient_state_t::RESOLVING:
		switch (NET_DNS_Resolve(cfg_.host, ip_)) {
		case net_result_t::OK:          state_ = netclient_state_t::CONNECTING; break;
		case net_result_t::UNREACHABLE: fail("DNS: cannot resolve host"); return;
		default:                        return;
		}
		[[fallthrough]];

	case netclient_state_t::CONNECTING:
		if (conn_ == nullptr) {
			switch (NET_TCP_Connect(ip_, cfg_.port, &conn_)) {
			case net_result_t::OK:          local_port_ = conn_->server_port; break;
			case net_result_t::UNREACHABLE: fail("TCP: no route or no free connection"); return;
			default:                        return;   // ARP in progress
			}
		}
		if (!tcp_alive()) { fail("TCP: refused or no answer"); return; }
		if (conn_->state != tcp_state_t::TCP_ESTABLISHED) return;
		if (!tls_) { state_ = netclient_state_t::UP; return; }
		if (!tls_setup()) return;
		handshake_started_ = get_tick();
		state_ = netclient_state_t::HANDSHAKE;
		[[fallthrough]];

	case netclient_state_t::HANDSHAKE: {
		const int r = mbedtls_ssl_handshake(&tls.ssl);
		if (r == 0) {
			handshake_ms_ = get_tick() - handshake_started_;
			state_ = netclient_state_t::UP;
		} else if (!in_progress(r)) {
			const uint32_t flags = mbedtls_ssl_get_verify_result(&tls.ssl);
			if (r == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED && flags != 0) {
				char why[64];
				mbedtls_x509_crt_verify_info(why, sizeof(why), "", flags);
				for (char *p = why; *p; ++p) if (*p == '\n') *p = ' ';
				snprintf(error_, sizeof(error_), "broker certificate: %s", why);
				release();
				state_ = netclient_state_t::FAILED;
			} else {
				fail("TLS handshake", r);
			}
		}
		return;
	}

	case netclient_state_t::UP:
		if (!tcp_alive()) { fail("connection closed by peer"); return; }
		drain_tx();
		return;

	default:
		return;
	}
}

bool NetClient::write(const uint8_t *data, size_t n) {
	if (!up() || n > NETCLIENT_TX_LEN - tx_count_) {
		return false;
	}
	for (size_t i = 0; i < n; i++) {
		tx_[tx_head_] = data[i];
		tx_head_ = (tx_head_ + 1) % NETCLIENT_TX_LEN;
	}
	tx_count_ += n;
	drain_tx();                       // start right away, the rest goes in poll()
	return true;
}

void NetClient::drain_tx() {
	while (tx_count_ > 0 && up()) {
		// contiguous run from the tail (the ring may wrap)
		size_t run = NETCLIENT_TX_LEN - tx_tail_;
		if (run > tx_count_) run = tx_count_;
		int done;
		if (tls_) {
			// mbedTLS wants the very same buffer again after WANT_WRITE
			if (tls_pending_ == 0) {
				tls_pending_ = (run > MBEDTLS_SSL_OUT_CONTENT_LEN) ? MBEDTLS_SSL_OUT_CONTENT_LEN : run;
			}
			done = mbedtls_ssl_write(&tls.ssl, &tx_[tx_tail_], tls_pending_);
			if (in_progress(done)) return;
			if (done < 0) { fail("TLS write", done); return; }
			tls_pending_ = 0;
		} else {
			done = NET_TCP_SendUser(conn_, &tx_[tx_tail_], static_cast<uint16_t>(run > 0xFFFF ? 0xFFFF : run));
			if (done < 0) { fail("TCP write"); return; }
			if (done == 0) return;    // window or TX slots full: next poll
		}
		tx_tail_ = (tx_tail_ + static_cast<size_t>(done)) % NETCLIENT_TX_LEN;
		tx_count_ -= static_cast<size_t>(done);
	}
}

size_t NetClient::read(uint8_t *buf, size_t cap) {
	if (!up() || cap == 0) {
		return 0;
	}
	if (!tls_) {
		const int n = NET_TCP_Read(conn_, buf, static_cast<uint16_t>(cap > 0xFFFF ? 0xFFFF : cap));
		return n > 0 ? static_cast<size_t>(n) : 0;
	}
	const int n = mbedtls_ssl_read(&tls.ssl, buf, cap);
	if (n > 0) return static_cast<size_t>(n);
	if (n == 0 || n == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
		fail("closed by peer (TLS close_notify)");
	} else if (!in_progress(n)) {
		fail("TLS read", n);
	}
	return 0;
}

int NetClient::bio_send(const unsigned char *buf, size_t len) {
	if (!tcp_alive()) return MBEDTLS_ERR_NET_CONN_RESET;
	const int n = NET_TCP_SendUser(conn_, buf, static_cast<uint16_t>(len > 0xFFFF ? 0xFFFF : len));
	if (n < 0) return MBEDTLS_ERR_NET_SEND_FAILED;
	return n > 0 ? n : MBEDTLS_ERR_SSL_WANT_WRITE;
}

int NetClient::bio_recv(unsigned char *buf, size_t len) {
	if (!tcp_alive()) return MBEDTLS_ERR_NET_CONN_RESET;
	const int n = NET_TCP_Read(conn_, buf, static_cast<uint16_t>(len > 0xFFFF ? 0xFFFF : len));
	return n > 0 ? n : MBEDTLS_ERR_SSL_WANT_READ;
}
