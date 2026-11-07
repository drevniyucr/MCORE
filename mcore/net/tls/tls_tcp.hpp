/*
 * tls_tcp.hpp — Mbed TLS BIO callbacks on an MCORE TCP connection.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "net/tcp.hpp"

namespace tls {

struct TcpBio {
	tcp_conn_t *conn = nullptr;
	uint16_t local_port = 0;   // identifies the connection (slots are reused)

	bool open() const { return NET_TCP_IsOpen(conn, local_port); }
	bool established() const {
		return open() && conn->state == tcp_state_t::TCP_ESTABLISHED;
	}
};

// Mbed TLS f_send / f_recv with a TcpBio* context
int tcp_bio_send(void *ctx, const unsigned char *buf, size_t len);
int tcp_bio_recv(void *ctx, unsigned char *buf, size_t len);

} // namespace tls
