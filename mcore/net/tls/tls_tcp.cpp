/*
 * tls_tcp.cpp — see tls_tcp.hpp.
 */
#include "net/tls/tls_tcp.hpp"

#include "mbedtls/net_sockets.h"   // MBEDTLS_ERR_NET_CONN_RESET
#include "mbedtls/ssl.h"

namespace tls {

int tcp_bio_send(void *ctx, const unsigned char *buf, size_t len) {
	TcpBio *bio = static_cast<TcpBio *>(ctx);
	if (!bio->open()) {
		return MBEDTLS_ERR_NET_CONN_RESET;
	}
	if (bio->conn->state != tcp_state_t::TCP_ESTABLISHED) {
		return MBEDTLS_ERR_SSL_WANT_WRITE;   // still connecting, or closing
	}
	const uint16_t chunk = static_cast<uint16_t>(len > 65535U ? 65535U : len);
	const int n = NET_TCP_SendUser(bio->conn, buf, chunk);
	if (n < 0) {
		return MBEDTLS_ERR_NET_CONN_RESET;
	}
	if (n == 0) {
		return MBEDTLS_ERR_SSL_WANT_WRITE;   // no TX slot / peer window full
	}
	return n;
}

int tcp_bio_recv(void *ctx, unsigned char *buf, size_t len) {
	TcpBio *bio = static_cast<TcpBio *>(ctx);
	if (!bio->open()) {
		return MBEDTLS_ERR_NET_CONN_RESET;
	}
	const uint16_t want = static_cast<uint16_t>(len > 65535U ? 65535U : len);
	const int n = NET_TCP_Read(bio->conn, buf, want);
	if (n < 0) {
		return MBEDTLS_ERR_NET_CONN_RESET;
	}
	if (n == 0) {
		// Nothing buffered. Once the peer has closed, no more will come.
		return (bio->conn->state == tcp_state_t::TCP_ESTABLISHED ||
		        bio->conn->state == tcp_state_t::TCP_SYN_SENT)
		     ? MBEDTLS_ERR_SSL_WANT_READ : MBEDTLS_ERR_NET_CONN_RESET;
	}
	return n;
}

} // namespace tls
