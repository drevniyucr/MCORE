/*
 * mqtt.hpp — MQTT 3.1.1 client (OASIS standard), transport-agnostic.
 *
 * Non-blocking and allocation-free: the caller owns the TX/RX buffers and a
 * byte-stream Transport (TLS over TCP on the target, sockets in host tests)
 * and calls poll() from its main loop.
 *
 * Supported: CONNECT (credentials, will, clean session, keep-alive),
 * PUBLISH QoS 0/1 both ways, SUBSCRIBE (QoS 0/1), PINGREQ/PINGRESP,
 * DISCONNECT. QoS 2 is not supported: subscriptions request at most QoS 1,
 * so a compliant broker never sends QoS 2 to us.
 *
 * Outgoing QoS 1 messages are tracked until PUBACK. They are not resent
 * after a reconnect (clean sessions), which is what VDA 5050 expects:
 * state is republished in full anyway.
 */
#pragma once

#include <cstddef>
#include <cstdint>

namespace mqtt {

// Byte stream. Return >0 bytes moved, 0 = would block (try later),
// <0 = connection lost / fatal error.
struct Transport {
	void *ctx;
	int (*write)(void *ctx, const uint8_t *data, size_t len);
	int (*read)(void *ctx, uint8_t *buf, size_t len);
};

struct ConnectOptions {
	const char *client_id = "";
	const char *username = nullptr;
	const char *password = nullptr;
	uint16_t keepalive_s = 30;       // 0 disables keep-alive
	bool clean_session = true;
	const char *will_topic = nullptr;    // nullptr: no will
	const uint8_t *will_payload = nullptr;
	uint16_t will_len = 0;
	uint8_t will_qos = 0;
	bool will_retain = false;
	uint32_t connack_timeout_ms = 10000;
};

enum class State : uint8_t {
	Idle,         // not connected (initial, or after reset())
	Connecting,   // CONNECT queued/sent, waiting for CONNACK
	Connected,
	Failed        // see error(); call reset() and reconnect the transport
};

enum class Error : uint8_t {
	None,
	Transport,       // transport read/write returned < 0
	Protocol,        // malformed or unexpected packet
	Refused,         // CONNACK return code != 0, see connack_code()
	ConnackTimeout,
	PingTimeout,     // no PINGRESP within one keep-alive period
	TxOverflow       // CONNECT did not fit the TX buffer
};

// Called for every PUBLISH received. topic is NOT NUL-terminated.
using MessageHandler = void (*)(void *ctx, const char *topic, uint16_t topic_len,
		const uint8_t *payload, size_t len, uint8_t qos, bool retain);

struct Stats {
	uint32_t published;      // PUBLISH packets queued by publish()
	uint32_t puback;         // PUBACKs received for our QoS 1 messages
	uint32_t received;       // PUBLISH packets delivered to the handler
	uint32_t rx_oversize;    // packets dropped: larger than the RX buffer
	uint32_t pings;          // PINGREQs sent
	uint32_t suback_fail;    // SUBACK return code 0x80
};

class Client {
public:
	static constexpr uint8_t MAX_INFLIGHT = 8;   // QoS 1 publishes awaiting PUBACK

	void init(const Transport &t, uint8_t *tx_buf, size_t tx_cap,
	          uint8_t *rx_buf, size_t rx_cap, MessageHandler on_message, void *ctx);

	// Queues CONNECT. The transport must already be connected.
	bool connect(const ConnectOptions &opt, uint32_t now_ms);

	// Moves bytes both ways, dispatches packets, keep-alive and timeouts.
	void poll(uint32_t now_ms);

	// Queue a message. False if not connected, qos > 1, the TX buffer has
	// no room (retry later) or MAX_INFLIGHT QoS 1 messages are unacked.
	bool publish(const char *topic, const uint8_t *payload, size_t len,
	             uint8_t qos, bool retain);
	bool subscribe(const char *topic, uint8_t qos);

	// Queues DISCONNECT; the caller closes the transport once tx_pending()
	// is 0 (or after a short grace period) and calls reset().
	void disconnect();
	void reset();

	State state() const { return state_; }
	Error error() const { return error_; }
	uint8_t connack_code() const { return connack_code_; }
	size_t tx_pending() const { return tx_len_; }
	uint8_t inflight() const;
	const Stats &stats() const { return stats_; }

private:
	enum class RxPhase : uint8_t { Header, Length, Body, Skip };

	bool tx_room(size_t n) const { return tx_cap_ - tx_len_ >= n; }
	void put8(uint8_t v) { tx_[tx_len_++] = v; }
	void put16(uint16_t v) { put8(static_cast<uint8_t>(v >> 8)); put8(static_cast<uint8_t>(v)); }
	void put_bytes(const void *p, size_t n);
	void put_str(const char *s, size_t n) { put16(static_cast<uint16_t>(n)); put_bytes(s, n); }
	bool put_fixed_header(uint8_t type_flags, size_t remaining);
	static size_t varint_size(size_t v);

	uint16_t next_packet_id();
	void flush();
	void receive();
	void handle_packet();
	void fail(Error e);

	Transport t_ = {};
	uint8_t *tx_ = nullptr;
	size_t tx_cap_ = 0;
	size_t tx_len_ = 0;
	uint8_t *rx_ = nullptr;
	size_t rx_cap_ = 0;
	MessageHandler on_message_ = nullptr;
	void *ctx_ = nullptr;

	State state_ = State::Idle;
	Error error_ = Error::None;
	uint8_t connack_code_ = 0;

	RxPhase phase_ = RxPhase::Header;
	uint8_t rx_type_ = 0;
	size_t rx_remaining_ = 0;    // packet body length being decoded/received
	size_t rx_got_ = 0;
	uint8_t len_bytes_ = 0;
	uint32_t len_mult_ = 1;

	uint16_t keepalive_s_ = 0;
	uint32_t connack_timeout_ms_ = 0;
	uint32_t connect_ms_ = 0;
	uint32_t last_tx_ms_ = 0;
	uint32_t ping_sent_ms_ = 0;
	bool ping_outstanding_ = false;
	uint32_t now_ms_ = 0;

	uint16_t packet_id_ = 0;
	uint16_t inflight_[MAX_INFLIGHT] = {};
	Stats stats_ = {};
};

// Packet encoders, exposed for unit tests. Write into buf, return the
// packet length or 0 if it does not fit.
size_t encode_remaining_length(uint8_t *buf, size_t value);
bool decode_remaining_length(const uint8_t *buf, size_t avail, size_t *value, size_t *used);

} // namespace mqtt
