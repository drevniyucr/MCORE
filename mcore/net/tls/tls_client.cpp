/*
 * tls_client.cpp — see tls_client.hpp.
 */
#include <cstring>

#include "net/tls/tls_client.hpp"
#include "mbedtls/error.h"

namespace tls {

namespace {
const char DRBG_PERSONALISATION[] = "mcore-tls-client";

// Only restartable groups: brokers (OpenSSL) pick X25519 whenever it is
// offered, and an X25519 key exchange runs ~665 ms in one piece on the M4
const uint16_t GROUPS[] = {
	MBEDTLS_SSL_IANA_TLS_GROUP_SECP256R1,
	MBEDTLS_SSL_IANA_TLS_GROUP_SECP384R1,
	MBEDTLS_SSL_IANA_TLS_GROUP_NONE,
};
} // namespace

void set_ecc_slice(unsigned max_ops) {
#if defined(MBEDTLS_ECP_RESTARTABLE)
	mbedtls_ecp_set_max_ops(max_ops);
#else
	(void) max_ops;
#endif
}

int Client::setup(const Credentials &cred, const char *server_name) {
	mbedtls_ssl_init(&ssl_);
	mbedtls_ssl_config_init(&conf_);
	mbedtls_x509_crt_init(&ca_);
	mbedtls_x509_crt_init(&own_cert_);
	mbedtls_pk_init(&own_key_);
	mbedtls_entropy_init(&entropy_);
	mbedtls_ctr_drbg_init(&drbg_);
	mbedtls_ssl_session_init(&saved_);
	have_saved_ = false;
	setup_done_ = false;

	int ret = mbedtls_ctr_drbg_seed(&drbg_, mbedtls_entropy_func, &entropy_,
			reinterpret_cast<const unsigned char *>(DRBG_PERSONALISATION),
			sizeof(DRBG_PERSONALISATION) - 1U);
	if (ret != 0) {
		return last_error_ = ret;
	}
	if (cred.ca_pem == nullptr) {
		return last_error_ = MBEDTLS_ERR_X509_BAD_INPUT_DATA;
	}
	// PEM input length must include the terminating NUL
	ret = mbedtls_x509_crt_parse(&ca_, reinterpret_cast<const unsigned char *>(cred.ca_pem),
			strlen(cred.ca_pem) + 1U);
	if (ret != 0) {
		return last_error_ = ret;
	}

	ret = mbedtls_ssl_config_defaults(&conf_, MBEDTLS_SSL_IS_CLIENT,
			MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
	if (ret != 0) {
		return last_error_ = ret;
	}
	mbedtls_ssl_conf_authmode(&conf_, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_ca_chain(&conf_, &ca_, nullptr);
	mbedtls_ssl_conf_rng(&conf_, mbedtls_ctr_drbg_random, &drbg_);
	mbedtls_ssl_conf_groups(&conf_, GROUPS);

	if (cred.client_cert_pem != nullptr && cred.client_key_pem != nullptr) {
		ret = mbedtls_x509_crt_parse(&own_cert_,
				reinterpret_cast<const unsigned char *>(cred.client_cert_pem),
				strlen(cred.client_cert_pem) + 1U);
		if (ret == 0) {
			ret = mbedtls_pk_parse_key(&own_key_,
					reinterpret_cast<const unsigned char *>(cred.client_key_pem),
					strlen(cred.client_key_pem) + 1U, nullptr, 0,
					mbedtls_ctr_drbg_random, &drbg_);
		}
		if (ret == 0) {
			ret = mbedtls_ssl_conf_own_cert(&conf_, &own_cert_, &own_key_);
		}
		if (ret != 0) {
			return last_error_ = ret;
		}
	}

	ret = mbedtls_ssl_setup(&ssl_, &conf_);
	if (ret != 0) {
		return last_error_ = ret;
	}
	// Name the certificate must carry (SAN DNS name or IP address). Mbed TLS
	// does not send IP literals as SNI.
	ret = mbedtls_ssl_set_hostname(&ssl_, server_name);
	if (ret != 0) {
		return last_error_ = ret;
	}
	setup_done_ = true;
	last_error_ = 0;
	return 0;
}

void Client::teardown() {
	mbedtls_ssl_free(&ssl_);
	mbedtls_ssl_config_free(&conf_);
	mbedtls_x509_crt_free(&ca_);
	mbedtls_x509_crt_free(&own_cert_);
	mbedtls_pk_free(&own_key_);
	mbedtls_ctr_drbg_free(&drbg_);
	mbedtls_entropy_free(&entropy_);
	forget_session();
	setup_done_ = false;
}

void Client::forget_session() {
	mbedtls_ssl_session_free(&saved_);
	mbedtls_ssl_session_init(&saved_);
	have_saved_ = false;
}

int Client::start(void *bio_ctx, SendFn send, RecvFn recv) {
	if (!setup_done_) {
		return last_error_ = MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
	}
	const int ret = mbedtls_ssl_session_reset(&ssl_);
	if (ret != 0) {
		return last_error_ = ret;
	}
	mbedtls_ssl_set_bio(&ssl_, bio_ctx, send, recv, nullptr);
	if (have_saved_ && mbedtls_ssl_set_session(&ssl_, &saved_) != 0) {
		forget_session();          // unusable: full handshake
	}
	pending_write_ = 0;
	last_error_ = 0;
	crypto_busy_ = false;
	saw_certificate_ = false;
	resumed_ = false;
	return 0;
}

int Client::handshake() {
	// Step by step (what mbedtls_ssl_handshake does) to see the states: a
	// resumed handshake goes from ServerHello straight to ChangeCipherSpec
	int ret = 0;
	while (!mbedtls_ssl_is_handshake_over(&ssl_)) {
		if (ssl_.MBEDTLS_PRIVATE(state) == MBEDTLS_SSL_SERVER_CERTIFICATE) {
			saw_certificate_ = true;
		}
		ret = mbedtls_ssl_handshake_step(&ssl_);
		if (ret != 0) {
			break;
		}
	}
	crypto_busy_ = (ret == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS);
	if (ret == 0) {
		resumed_ = have_saved_ && !saw_certificate_;
		// Keep the session for the next connection
		mbedtls_ssl_session fresh;
		mbedtls_ssl_session_init(&fresh);
		if (mbedtls_ssl_get_session(&ssl_, &fresh) == 0) {
			mbedtls_ssl_session_free(&saved_);
			saved_ = fresh;            // takes over the allocations
			have_saved_ = true;
		} else {
			mbedtls_ssl_session_free(&fresh);
		}
		return 0;
	}
	if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
		forget_session();
	}
	if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE || crypto_busy_) {
		return 1;
	}
	last_error_ = ret;
	return ret;
}

int Client::write(const uint8_t *data, size_t len) {
	// After WANT_WRITE Mbed TLS has already encrypted the record and must
	// be called again with the same length, otherwise it would report the
	// new length as written. The caller keeps the data in place.
	const size_t n = (pending_write_ != 0U) ? pending_write_ : len;
	const int ret = mbedtls_ssl_write(&ssl_, data, n);
	if (ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ) {
		pending_write_ = n;
		return 0;
	}
	pending_write_ = 0;
	if (ret < 0) {
		last_error_ = ret;
	}
	return ret;
}

int Client::read(uint8_t *buf, size_t len) {
	const int ret = mbedtls_ssl_read(&ssl_, buf, len);
	if (ret > 0) {
		return ret;
	}
	if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
		return 0;
	}
	// 0 = clean EOF, PEER_CLOSE_NOTIFY, or a real error
	last_error_ = (ret == 0) ? MBEDTLS_ERR_SSL_CONN_EOF : ret;
	return last_error_;
}

void Client::close_notify() {
	(void) mbedtls_ssl_close_notify(&ssl_);
}

int Client::handshake_state() const {
	return ssl_.MBEDTLS_PRIVATE(state);
}

uint32_t Client::verify_flags() const {
	return mbedtls_ssl_get_verify_result(&ssl_);
}

const char *Client::ciphersuite() const {
	const char *s = mbedtls_ssl_get_ciphersuite(&ssl_);
	return s != nullptr ? s : "";
}

void error_string(int err, char *buf, size_t len) {
	mbedtls_strerror(err, buf, len);
}

} // namespace tls
