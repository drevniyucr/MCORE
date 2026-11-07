/*
 * mqtt.cpp — MQTT 3.1.1 client, see mqtt.hpp.
 */
#include <cstring>

#include "net/mqtt.hpp"

namespace mqtt {

namespace {
// Control packet types (MQTT 3.1.1, 2.2.1)
constexpr uint8_t CONNECT = 1, CONNACK = 2, PUBLISH = 3, PUBACK = 4, PUBREC = 5,
                  PUBREL = 6, PUBCOMP = 7, SUBSCRIBE = 8, SUBACK = 9, UNSUBACK = 11,
                  PINGREQ = 12, PINGRESP = 13, DISCONNECT = 14;
constexpr size_t MAX_REMAINING = 268435455U;
constexpr int RX_STEPS_PER_POLL = 256;   // bounds the time spent in one poll()
} // namespace

size_t encode_remaining_length(uint8_t *buf, size_t value) {
	if (value > MAX_REMAINING) {
		return 0;
	}
	size_t n = 0;
	do {
		uint8_t b = static_cast<uint8_t>(value % 128U);
		value /= 128U;
		if (value != 0U) {
			b |= 0x80U;
		}
		buf[n++] = b;
	} while (value != 0U);
	return n;
}

bool decode_remaining_length(const uint8_t *buf, size_t avail, size_t *value, size_t *used) {
	size_t v = 0;
	size_t mult = 1;
	for (size_t i = 0; i < 4U; ++i) {
		if (i >= avail) {
			return false;
		}
		v += static_cast<size_t>(buf[i] & 0x7FU) * mult;
		if ((buf[i] & 0x80U) == 0U) {
			*value = v;
			*used = i + 1U;
			return true;
		}
		mult *= 128U;
	}
	return false;   // more than 4 length bytes: malformed
}

size_t Client::varint_size(size_t v) {
	return v < 128U ? 1U : v < 16384U ? 2U : v < 2097152U ? 3U : 4U;
}

void Client::init(const Transport &t, uint8_t *tx_buf, size_t tx_cap,
		uint8_t *rx_buf, size_t rx_cap, MessageHandler on_message, void *ctx) {
	t_ = t;
	tx_ = tx_buf;
	tx_cap_ = tx_cap;
	rx_ = rx_buf;
	rx_cap_ = rx_cap;
	on_message_ = on_message;
	ctx_ = ctx;
	stats_ = {};
	reset();
}

void Client::reset() {
	state_ = State::Idle;
	error_ = Error::None;
	connack_code_ = 0;
	tx_len_ = 0;
	phase_ = RxPhase::Header;
	rx_remaining_ = 0;
	rx_got_ = 0;
	ping_outstanding_ = false;
	memset(inflight_, 0, sizeof(inflight_));
}

void Client::fail(Error e) {
	state_ = State::Failed;
	error_ = e;
}

void Client::put_bytes(const void *p, size_t n) {
	if (n != 0U) {
		memcpy(&tx_[tx_len_], p, n);
		tx_len_ += n;
	}
}

// Writes the fixed header only if the WHOLE packet fits, so a refused
// packet never leaves a partial header behind.
bool Client::put_fixed_header(uint8_t type_flags, size_t remaining) {
	if (remaining > MAX_REMAINING || !tx_room(1U + varint_size(remaining) + remaining)) {
		return false;
	}
	put8(type_flags);
	tx_len_ += encode_remaining_length(&tx_[tx_len_], remaining);
	return true;
}

uint16_t Client::next_packet_id() {
	packet_id_ = static_cast<uint16_t>(packet_id_ + 1U);
	if (packet_id_ == 0U) {
		packet_id_ = 1U;
	}
	return packet_id_;
}

uint8_t Client::inflight() const {
	uint8_t n = 0;
	for (uint16_t id : inflight_) {
		n = static_cast<uint8_t>(n + (id != 0U ? 1U : 0U));
	}
	return n;
}

bool Client::connect(const ConnectOptions &opt, uint32_t now_ms) {
	if (state_ != State::Idle || opt.client_id == nullptr) {
		return false;
	}
	now_ms_ = now_ms;
	const size_t id_len = strlen(opt.client_id);
	const bool will = opt.will_topic != nullptr;
	const size_t wt_len = will ? strlen(opt.will_topic) : 0U;
	const bool user = opt.username != nullptr;
	const bool pass = user && opt.password != nullptr;   // 3.1.2.9: no password without user
	const size_t u_len = user ? strlen(opt.username) : 0U;
	const size_t p_len = pass ? strlen(opt.password) : 0U;
	if (id_len > 65535U || wt_len > 65535U || u_len > 65535U || p_len > 65535U ||
	    opt.will_qos > 1U) {
		return false;
	}

	const size_t remaining = 10U + 2U + id_len
	                       + (will ? 2U + wt_len + 2U + opt.will_len : 0U)
	                       + (user ? 2U + u_len : 0U)
	                       + (pass ? 2U + p_len : 0U);
	if (!put_fixed_header(CONNECT << 4, remaining)) {
		fail(Error::TxOverflow);
		return false;
	}
	static const uint8_t proto[] = { 0x00, 0x04, 'M', 'Q', 'T', 'T', 0x04 };   // level 4 = 3.1.1
	put_bytes(proto, sizeof(proto));
	uint8_t flags = 0;
	flags |= opt.clean_session ? 0x02U : 0x00U;
	if (will) {
		flags |= 0x04U;
		flags |= static_cast<uint8_t>(opt.will_qos << 3);
		flags |= opt.will_retain ? 0x20U : 0x00U;
	}
	flags |= pass ? 0x40U : 0x00U;
	flags |= user ? 0x80U : 0x00U;
	put8(flags);
	put16(opt.keepalive_s);
	put_str(opt.client_id, id_len);
	if (will) {
		put_str(opt.will_topic, wt_len);
		put16(opt.will_len);
		put_bytes(opt.will_payload, opt.will_len);
	}
	if (user) {
		put_str(opt.username, u_len);
	}
	if (pass) {
		put_str(opt.password, p_len);
	}

	keepalive_s_ = opt.keepalive_s;
	connack_timeout_ms_ = opt.connack_timeout_ms;
	connect_ms_ = now_ms;
	last_tx_ms_ = now_ms;
	state_ = State::Connecting;
	flush();
	return true;
}

bool Client::publish(const char *topic, const uint8_t *payload, size_t len,
		uint8_t qos, bool retain) {
	if (state_ != State::Connected || topic == nullptr || qos > 1U) {
		return false;
	}
	const size_t t_len = strlen(topic);
	if (t_len == 0U || t_len > 65535U) {
		return false;
	}
	uint16_t *slot = nullptr;
	if (qos == 1U) {
		for (uint16_t &id : inflight_) {
			if (id == 0U) {
				slot = &id;
				break;
			}
		}
		if (slot == nullptr) {
			return false;   // too many unacknowledged messages
		}
	}
	const size_t remaining = 2U + t_len + (qos != 0U ? 2U : 0U) + len;
	const uint8_t hdr = static_cast<uint8_t>((PUBLISH << 4) | (qos << 1) | (retain ? 1U : 0U));
	if (!put_fixed_header(hdr, remaining)) {
		return false;   // no room now: retry after the TX buffer drains
	}
	put_str(topic, t_len);
	if (qos != 0U) {
		*slot = next_packet_id();
		put16(*slot);
	}
	put_bytes(payload, len);
	stats_.published += 1;
	flush();
	return true;
}

bool Client::subscribe(const char *topic, uint8_t qos) {
	if (state_ != State::Connected || topic == nullptr) {
		return false;
	}
	const size_t t_len = strlen(topic);
	if (t_len == 0U || t_len > 65535U) {
		return false;
	}
	if (!put_fixed_header((SUBSCRIBE << 4) | 0x02U, 2U + 2U + t_len + 1U)) {
		return false;
	}
	put16(next_packet_id());
	put_str(topic, t_len);
	put8(qos > 1U ? 1U : qos);   // QoS 2 is not supported
	flush();
	return true;
}

void Client::disconnect() {
	if ((state_ == State::Connected || state_ == State::Connecting) && tx_room(2U)) {
		put8(DISCONNECT << 4);
		put8(0);
		flush();
	}
	if (state_ != State::Failed) {
		state_ = State::Idle;
	}
}

void Client::flush() {
	while (tx_len_ > 0U && state_ != State::Failed) {
		const int n = t_.write(t_.ctx, tx_, tx_len_);
		if (n < 0) {
			fail(Error::Transport);
			return;
		}
		if (n == 0) {
			return;   // would block
		}
		const size_t sent = static_cast<size_t>(n) < tx_len_ ? static_cast<size_t>(n) : tx_len_;
		memmove(tx_, tx_ + sent, tx_len_ - sent);
		tx_len_ -= sent;
		last_tx_ms_ = now_ms_;
	}
}

void Client::poll(uint32_t now_ms) {
	now_ms_ = now_ms;
	if (state_ == State::Failed) {
		return;
	}
	flush();
	if (state_ == State::Connecting || state_ == State::Connected) {
		receive();
	}
	if (state_ == State::Connecting && (now_ms - connect_ms_) > connack_timeout_ms_) {
		fail(Error::ConnackTimeout);
		return;
	}
	if (state_ != State::Connected || keepalive_s_ == 0U) {
		return;
	}
	const uint32_t ka_ms = static_cast<uint32_t>(keepalive_s_) * 1000U;
	if (ping_outstanding_) {
		if ((now_ms - ping_sent_ms_) > ka_ms) {
			fail(Error::PingTimeout);
		}
	} else if ((now_ms - last_tx_ms_) >= (ka_ms / 4U) * 3U && tx_room(2U)) {
		// Idle for 3/4 of the keep-alive: prove we are alive
		put8(PINGREQ << 4);
		put8(0);
		ping_outstanding_ = true;
		ping_sent_ms_ = now_ms;
		stats_.pings += 1;
		flush();
	}
}

void Client::receive() {
	uint8_t b = 0;
	for (int step = 0; step < RX_STEPS_PER_POLL; ++step) {
		if (state_ != State::Connecting && state_ != State::Connected) {
			return;
		}
		int n = 0;
		switch (phase_) {
		case RxPhase::Header:
			n = t_.read(t_.ctx, &b, 1);
			if (n <= 0) {
				break;
			}
			rx_type_ = b;
			rx_remaining_ = 0;
			len_bytes_ = 0;
			len_mult_ = 1;
			phase_ = RxPhase::Length;
			break;

		case RxPhase::Length:
			n = t_.read(t_.ctx, &b, 1);
			if (n <= 0) {
				break;
			}
			rx_remaining_ += static_cast<size_t>(b & 0x7FU) * len_mult_;
			len_mult_ *= 128U;
			len_bytes_ = static_cast<uint8_t>(len_bytes_ + 1U);
			if ((b & 0x80U) != 0U) {
				if (len_bytes_ >= 4U) {
					fail(Error::Protocol);
				}
				break;
			}
			rx_got_ = 0;
			if (rx_remaining_ > rx_cap_) {
				stats_.rx_oversize += 1;
				phase_ = RxPhase::Skip;
			} else if (rx_remaining_ == 0U) {
				phase_ = RxPhase::Header;
				handle_packet();
			} else {
				phase_ = RxPhase::Body;
			}
			break;

		case RxPhase::Body:
			n = t_.read(t_.ctx, rx_ + rx_got_, rx_remaining_ - rx_got_);
			if (n <= 0) {
				break;
			}
			rx_got_ += static_cast<size_t>(n);
			if (rx_got_ >= rx_remaining_) {
				phase_ = RxPhase::Header;
				handle_packet();
			}
			break;

		case RxPhase::Skip: {
			uint8_t sink[64];
			const size_t want = rx_remaining_ - rx_got_;
			n = t_.read(t_.ctx, sink, want < sizeof(sink) ? want : sizeof(sink));
			if (n <= 0) {
				break;
			}
			rx_got_ += static_cast<size_t>(n);
			if (rx_got_ >= rx_remaining_) {
				phase_ = RxPhase::Header;
			}
			break;
		}
		}
		if (n < 0) {
			fail(Error::Transport);
			return;
		}
		if (n == 0) {
			return;   // nothing more to read now
		}
	}
}

void Client::handle_packet() {
	const uint8_t type = static_cast<uint8_t>(rx_type_ >> 4);
	const uint8_t flags = static_cast<uint8_t>(rx_type_ & 0x0FU);
	const size_t len = rx_remaining_;

	switch (type) {
	case CONNACK:
		if (state_ != State::Connecting || len != 2U) {
			fail(Error::Protocol);
			return;
		}
		connack_code_ = rx_[1];
		if (connack_code_ != 0U) {
			fail(Error::Refused);
			return;
		}
		state_ = State::Connected;
		ping_outstanding_ = false;
		break;

	case PUBLISH: {
		const uint8_t qos = static_cast<uint8_t>((flags >> 1) & 0x03U);
		const bool retain = (flags & 0x01U) != 0U;
		if (state_ != State::Connected || qos > 1U || len < 2U) {
			fail(Error::Protocol);
			return;
		}
		const uint16_t t_len = static_cast<uint16_t>((rx_[0] << 8) | rx_[1]);
		size_t pos = 2U + t_len;
		uint16_t id = 0;
		if (qos == 1U) {
			if (pos + 2U > len) {
				fail(Error::Protocol);
				return;
			}
			id = static_cast<uint16_t>((rx_[pos] << 8) | rx_[pos + 1U]);
			pos += 2U;
		}
		if (pos > len) {
			fail(Error::Protocol);
			return;
		}
		stats_.received += 1;
		if (on_message_ != nullptr) {
			on_message_(ctx_, reinterpret_cast<const char *>(&rx_[2]), t_len,
			            &rx_[pos], len - pos, qos, retain);
		}
		if (qos == 1U && tx_room(4U)) {
			put8(PUBACK << 4);
			put8(2);
			put16(id);
		}
		break;
	}

	case PUBACK: {
		if (len != 2U) {
			fail(Error::Protocol);
			return;
		}
		const uint16_t id = static_cast<uint16_t>((rx_[0] << 8) | rx_[1]);
		for (uint16_t &slot : inflight_) {
			if (slot == id && id != 0U) {
				slot = 0;
				stats_.puback += 1;
				break;
			}
		}
		break;
	}

	case SUBACK:
		if (len < 3U) {
			fail(Error::Protocol);
			return;
		}
		for (size_t i = 2; i < len; ++i) {
			if (rx_[i] == 0x80U) {
				stats_.suback_fail += 1;
			}
		}
		break;

	case PINGRESP:
		ping_outstanding_ = false;
		break;

	case UNSUBACK:
		break;

	case PUBREC:
	case PUBREL:
	case PUBCOMP:
	default:
		fail(Error::Protocol);   // QoS 2 flow or a server-only packet
		break;
	}
}

} // namespace mqtt
