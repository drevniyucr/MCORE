/*
 * netclient.hpp — outgoing byte stream: DNS -> ARP -> TCP connect -> TLS
 *
 * One non-blocking state machine for "connect to host:port, optionally over
 * TLS, and give me a reliable byte stream". poll() advances it from the
 * superloop; nothing ever waits:
 *
 *   NetClientBuf<> link;                             // TX queue of NETCLIENT_TX_LEN bytes
 *   link.open({ "broker.local", 8883, &creds });     // creds == nullptr: plain TCP
 *   ... every loop pass:
 *   link.poll();
 *   if (link.up()) { link.write(...); link.read(...); }
 *   if (link.failed()) { log link.error(); retry open() later }
 *
 * write() queues a message whole or not at all (the MQTT client relies on
 * that); poll() drains the queue into TCP/TLS as the window allows. The
 * queue belongs to the instance: NetClientBuf<N> holds N bytes, the largest
 * message write() has to take in one piece (an MQTT link with big messages
 * needs kilobytes, an HTTP GET or a Modbus poll a few hundred bytes).
 * Only one NetClient may use TLS at a time (one static mbedTLS session).
 *
 * Build options (define them PUBLIC on the target, the class layout depends
 * on them):
 *   MCORE_NET_TLS           1 (default): TLS through mbedTLS (net/tls.cpp);
 *                           0: plain TCP only, no mbedTLS needed
 *   MCORE_NETCLIENT_TX_LEN  default N of NetClientBuf<>, 2048
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "net/net.hpp"
#include "net/tcp.hpp"
#include "net/tls.hpp"

#ifndef MCORE_NET_TLS
#define MCORE_NET_TLS 1
#endif
#ifndef MCORE_NETCLIENT_TX_LEN
#define MCORE_NETCLIENT_TX_LEN 2048
#endif

constexpr size_t   NETCLIENT_TX_LEN        = MCORE_NETCLIENT_TX_LEN;
constexpr uint32_t NETCLIENT_OPEN_TIMEOUT_MS = 20000;   // DNS + connect + handshake

struct NetClientConfig {
	const char *host = nullptr;            // name or dotted IPv4
	uint16_t port = 0;
	const TlsCredentials *tls = nullptr;   // nullptr: plain TCP
};

enum class netclient_state_t : uint8_t {
	IDLE = 0,
	RESOLVING,    // DNS
	CONNECTING,   // ARP + TCP three-way handshake
	HANDSHAKE,    // TLS
	UP,
	FAILED,       // see error(); open() again to retry
};

class NetClient {
public:
	// The TX queue is the caller's (NetClientBuf below owns one)
	NetClient(uint8_t *tx, size_t tx_cap) : tx_(tx), tx_cap_(tx_cap) {}
	NetClient(const NetClient &) = delete;
	NetClient &operator=(const NetClient &) = delete;

	void open(const NetClientConfig &cfg);
	void close();                     // graceful: TLS close_notify, TCP FIN
	void poll();

	netclient_state_t state() const { return state_; }
	bool up() const { return state_ == netclient_state_t::UP; }
	bool failed() const { return state_ == netclient_state_t::FAILED; }
	const char* error() const { return error_; }

	// Queue n bytes (all or nothing); false if not up or the queue lacks room.
	bool write(const uint8_t *data, size_t n);
	// Received (decrypted) bytes available right now; 0 if none.
	size_t read(uint8_t *buf, size_t cap);

	// Negotiated TLS cipher suite while up over TLS, "" otherwise
	const char* cipher() const;

	size_t tx_queued() const { return tx_count_; }
	size_t tx_free() const { return tx_cap_ - tx_count_; }
	size_t tx_capacity() const { return tx_cap_; }
	uint32_t handshake_ms() const { return handshake_ms_; }

	// mbedTLS BIO callbacks (public only for the C trampolines)
	int bio_send(const unsigned char *buf, size_t len);
	int bio_recv(unsigned char *buf, size_t len);

private:
	void fail(const char *what, int mbedtls_err = 0);
	void release();                   // drop TCP and TLS state
	bool tls_setup();
	void drain_tx();
	bool tcp_alive() const { return NET_TCP_IsOpen(conn_, local_port_); }

	NetClientConfig cfg_{};
	netclient_state_t state_ = netclient_state_t::IDLE;
	uint8_t ip_[IP_ADDR_LEN] = {};
	tcp_conn_t *conn_ = nullptr;
	uint16_t local_port_ = 0;
	uint32_t opened_at_ = 0;
	uint32_t handshake_started_ = 0;
	uint32_t handshake_ms_ = 0;
	bool tls_ = false;
	char error_[96] = {};

	uint8_t *const tx_;
	const size_t tx_cap_;
	size_t tx_head_ = 0, tx_tail_ = 0, tx_count_ = 0;
	size_t tls_pending_ = 0;          // length of an ssl_write that returned WANT_WRITE
};

// A NetClient with its own TX queue of N bytes
template<size_t N = NETCLIENT_TX_LEN>
class NetClientBuf : public NetClient {
	static_assert(N > 0);
public:
	NetClientBuf() : NetClient(buf_, N) {}   // buf_ is storage here, filled later

private:
	uint8_t buf_[N] = {};
};
