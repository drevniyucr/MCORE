// Unit tests for the MQTT 3.1.1 client (mcore/net/mqtt.cpp) over a scripted
// in-memory transport. Byte sequences follow the OASIS MQTT 3.1.1 standard.
#include "doctest.h"

#include <cstring>
#include <deque>
#include <string>
#include <vector>

#include "net/mqtt.hpp"

namespace {

struct FakeTransport {
	std::vector<uint8_t> written;       // everything the client sent
	std::deque<uint8_t> to_client;      // bytes the "broker" will deliver
	size_t write_limit = 1u << 20;      // max bytes accepted per write call
	size_t read_chunk = 1u << 20;       // max bytes returned per read call
	bool blocked = false;               // write returns 0 (would block)
	bool broken = false;                // both directions fail

	static int write(void *ctx, const uint8_t *d, size_t n) {
		auto *t = static_cast<FakeTransport *>(ctx);
		if (t->broken) return -1;
		if (t->blocked) return 0;
		const size_t k = n < t->write_limit ? n : t->write_limit;
		t->written.insert(t->written.end(), d, d + k);
		return static_cast<int>(k);
	}
	static int read(void *ctx, uint8_t *b, size_t n) {
		auto *t = static_cast<FakeTransport *>(ctx);
		if (t->broken) return -1;
		size_t k = 0;
		while (k < n && k < t->read_chunk && !t->to_client.empty()) {
			b[k++] = t->to_client.front();
			t->to_client.pop_front();
		}
		return static_cast<int>(k);
	}
	void feed(std::initializer_list<uint8_t> bytes) { to_client.insert(to_client.end(), bytes); }
	void feed(const std::vector<uint8_t> &bytes) { to_client.insert(to_client.end(), bytes.begin(), bytes.end()); }
	mqtt::Transport transport() { return { this, &FakeTransport::write, &FakeTransport::read }; }
};

struct Received {
	std::string topic;
	std::vector<uint8_t> payload;
	uint8_t qos = 0;
	bool retain = false;
};

struct Fixture {
	FakeTransport t;
	uint8_t tx[512];
	uint8_t rx[128];
	std::vector<Received> got;
	mqtt::Client c;

	static void on_msg(void *ctx, const char *topic, uint16_t tlen, const uint8_t *p,
			size_t len, uint8_t qos, bool retain) {
		auto *f = static_cast<Fixture *>(ctx);
		f->got.push_back({ std::string(topic, tlen), std::vector<uint8_t>(p, p + len), qos, retain });
	}

	Fixture() { c.init(t.transport(), tx, sizeof(tx), rx, sizeof(rx), on_msg, this); }

	void connect_ok(uint16_t keepalive = 60) {
		mqtt::ConnectOptions o;
		o.client_id = "agv";
		o.keepalive_s = keepalive;
		REQUIRE(c.connect(o, 0));
		t.feed({ 0x20, 0x02, 0x00, 0x00 });        // CONNACK accepted
		c.poll(1);
		REQUIRE(c.state() == mqtt::State::Connected);
		t.written.clear();
	}
};

std::vector<uint8_t> publish_packet(const std::string &topic, const std::string &payload,
		uint8_t qos, uint16_t id = 0, bool retain = false) {
	std::vector<uint8_t> body;
	body.push_back(static_cast<uint8_t>(topic.size() >> 8));
	body.push_back(static_cast<uint8_t>(topic.size()));
	body.insert(body.end(), topic.begin(), topic.end());
	if (qos > 0) {
		body.push_back(static_cast<uint8_t>(id >> 8));
		body.push_back(static_cast<uint8_t>(id));
	}
	body.insert(body.end(), payload.begin(), payload.end());
	std::vector<uint8_t> pkt;
	pkt.push_back(static_cast<uint8_t>(0x30 | (qos << 1) | (retain ? 1 : 0)));
	uint8_t len[4];
	const size_t n = mqtt::encode_remaining_length(len, body.size());
	pkt.insert(pkt.end(), len, len + n);
	pkt.insert(pkt.end(), body.begin(), body.end());
	return pkt;
}

} // namespace

TEST_CASE("MQTT remaining length: boundary values from the standard (2.2.3)") {
	struct { size_t v; std::vector<uint8_t> enc; } cases[] = {
		{ 0, { 0x00 } }, { 127, { 0x7F } },
		{ 128, { 0x80, 0x01 } }, { 16383, { 0xFF, 0x7F } },
		{ 16384, { 0x80, 0x80, 0x01 } }, { 2097151, { 0xFF, 0xFF, 0x7F } },
		{ 2097152, { 0x80, 0x80, 0x80, 0x01 } }, { 268435455, { 0xFF, 0xFF, 0xFF, 0x7F } },
	};
	for (const auto &tc : cases) {
		uint8_t buf[4];
		REQUIRE(mqtt::encode_remaining_length(buf, tc.v) == tc.enc.size());
		CHECK(std::vector<uint8_t>(buf, buf + tc.enc.size()) == tc.enc);
		size_t v = 0, used = 0;
		REQUIRE(mqtt::decode_remaining_length(buf, tc.enc.size(), &v, &used));
		CHECK(v == tc.v);
		CHECK(used == tc.enc.size());
	}
	uint8_t buf[5];
	CHECK(mqtt::encode_remaining_length(buf, 268435456) == 0);
	const uint8_t bad[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0x01 };
	size_t v = 0, used = 0;
	CHECK_FALSE(mqtt::decode_remaining_length(bad, 5, &v, &used));
}

TEST_CASE("MQTT CONNECT: exact bytes with will, user and password") {
	Fixture f;
	mqtt::ConnectOptions o;
	o.client_id = "id";
	o.username = "u";
	o.password = "pw";
	o.keepalive_s = 30;
	o.will_topic = "w";
	const uint8_t will[] = { 'x' };
	o.will_payload = will;
	o.will_len = 1;
	o.will_qos = 1;
	o.will_retain = true;
	REQUIRE(f.c.connect(o, 0));
	const std::vector<uint8_t> expected = {
		0x10, 27,
		0x00, 0x04, 'M', 'Q', 'T', 'T', 0x04,
		0xEE,                      // user|pass|will retain|will QoS 1|will|clean
		0x00, 30,
		0x00, 0x02, 'i', 'd',
		0x00, 0x01, 'w', 0x00, 0x01, 'x',
		0x00, 0x01, 'u',
		0x00, 0x02, 'p', 'w',
	};
	CHECK(f.t.written == expected);
	CHECK(f.c.state() == mqtt::State::Connecting);
}

TEST_CASE("MQTT CONNACK: refusal code is reported, timeout fails the session") {
	{
		Fixture f;
		mqtt::ConnectOptions o;
		o.client_id = "a";
		REQUIRE(f.c.connect(o, 0));
		f.t.feed({ 0x20, 0x02, 0x00, 0x05 });    // not authorised
		f.c.poll(1);
		CHECK(f.c.state() == mqtt::State::Failed);
		CHECK(f.c.error() == mqtt::Error::Refused);
		CHECK(f.c.connack_code() == 5);
	}
	{
		Fixture f;
		mqtt::ConnectOptions o;
		o.client_id = "a";
		o.connack_timeout_ms = 1000;
		REQUIRE(f.c.connect(o, 0));
		f.c.poll(1000);
		CHECK(f.c.state() == mqtt::State::Connecting);
		f.c.poll(1001);
		CHECK(f.c.error() == mqtt::Error::ConnackTimeout);
	}
}

TEST_CASE("MQTT PUBLISH out: QoS 0 and QoS 1 bytes, PUBACK frees the slot") {
	Fixture f;
	f.connect_ok();
	const uint8_t data[] = { 'h', 'i' };
	REQUIRE(f.c.publish("a/b", data, 2, 0, true));
	CHECK(f.t.written == std::vector<uint8_t>{ 0x31, 7, 0, 3, 'a', '/', 'b', 'h', 'i' });

	f.t.written.clear();
	REQUIRE(f.c.publish("a/b", data, 2, 1, false));
	REQUIRE(f.t.written.size() == 11);
	CHECK(f.t.written[0] == 0x32);
	CHECK(f.t.written[1] == 9);
	const uint16_t id = static_cast<uint16_t>((f.t.written[7] << 8) | f.t.written[8]);
	CHECK(id != 0);
	CHECK(f.c.inflight() == 1);

	f.t.feed({ 0x40, 0x02, static_cast<uint8_t>(id >> 8), static_cast<uint8_t>(id) });
	f.c.poll(2);
	CHECK(f.c.inflight() == 0);
	CHECK(f.c.stats().puback == 1);
}

TEST_CASE("MQTT PUBLISH out: refused when not connected, QoS 2 or too many in flight") {
	Fixture f;
	const uint8_t d[] = { 1 };
	CHECK_FALSE(f.c.publish("t", d, 1, 0, false));   // not connected
	f.connect_ok();
	CHECK_FALSE(f.c.publish("t", d, 1, 2, false));   // QoS 2
	for (int i = 0; i < mqtt::Client::MAX_INFLIGHT; ++i) {
		REQUIRE(f.c.publish("t", d, 1, 1, false));
	}
	CHECK_FALSE(f.c.publish("t", d, 1, 1, false));
	CHECK(f.c.publish("t", d, 1, 0, false));         // QoS 0 still goes
}

TEST_CASE("MQTT PUBLISH in: split across reads, QoS 1 is acknowledged") {
	Fixture f;
	f.connect_ok();
	f.t.read_chunk = 3;                              // trickle in
	f.t.feed(publish_packet("cmd/x", "payload", 1, 0x1234, true));
	for (int i = 0; i < 20; ++i) {
		f.c.poll(10 + i);
	}
	REQUIRE(f.got.size() == 1);
	CHECK(f.got[0].topic == "cmd/x");
	CHECK(std::string(f.got[0].payload.begin(), f.got[0].payload.end()) == "payload");
	CHECK(f.got[0].qos == 1);
	CHECK(f.got[0].retain);
	CHECK(f.t.written == std::vector<uint8_t>{ 0x40, 0x02, 0x12, 0x34 });
}

TEST_CASE("MQTT PUBLISH in: packet larger than the RX buffer is skipped") {
	Fixture f;
	f.connect_ok();
	f.t.feed(publish_packet("big", std::string(500, 'z'), 0));   // rx buffer is 128
	f.t.feed(publish_packet("small", "ok", 0));
	for (int i = 0; i < 10; ++i) {
		f.c.poll(10 + i);
	}
	CHECK(f.c.stats().rx_oversize == 1);
	REQUIRE(f.got.size() == 1);
	CHECK(f.got[0].topic == "small");
	CHECK(f.c.state() == mqtt::State::Connected);
}

TEST_CASE("MQTT SUBSCRIBE: bytes, SUBACK failure code is counted") {
	Fixture f;
	f.connect_ok();
	REQUIRE(f.c.subscribe("uagv/#", 1));
	REQUIRE(f.t.written.size() == 13);
	CHECK(f.t.written[0] == 0x82);
	CHECK(f.t.written[1] == 11);
	CHECK(f.t.written[4] == 0x00);
	CHECK(f.t.written[5] == 6);
	CHECK(f.t.written[12] == 1);
	f.t.feed({ 0x90, 0x03, f.t.written[2], f.t.written[3], 0x80 });
	f.c.poll(5);
	CHECK(f.c.stats().suback_fail == 1);
}

TEST_CASE("MQTT keep-alive: PINGREQ when idle, PINGRESP clears it, silence fails") {
	Fixture f;
	f.connect_ok(/*keepalive=*/4);                   // 4 s: ping after 3 s idle
	f.c.poll(2000);
	CHECK(f.t.written.empty());
	f.c.poll(3001);
	CHECK(f.t.written == std::vector<uint8_t>{ 0xC0, 0x00 });
	f.t.feed({ 0xD0, 0x00 });
	f.c.poll(3100);
	CHECK(f.c.state() == mqtt::State::Connected);

	f.t.written.clear();
	f.c.poll(6200);                                  // next ping
	CHECK(f.t.written == std::vector<uint8_t>{ 0xC0, 0x00 });
	f.c.poll(10201);                                 // no PINGRESP for > 4 s
	CHECK(f.c.state() == mqtt::State::Failed);
	CHECK(f.c.error() == mqtt::Error::PingTimeout);
}

TEST_CASE("MQTT transport: would-block keeps bytes queued, partial writes complete") {
	Fixture f;
	f.connect_ok();
	f.t.blocked = true;
	const uint8_t d[] = { 1, 2, 3 };
	REQUIRE(f.c.publish("t", d, 3, 0, false));
	CHECK(f.t.written.empty());
	CHECK(f.c.tx_pending() == 8);

	f.t.blocked = false;
	f.t.write_limit = 3;
	f.c.poll(10);
	CHECK(f.c.tx_pending() == 0);
	CHECK(f.t.written == std::vector<uint8_t>{ 0x30, 6, 0, 1, 't', 1, 2, 3 });

	f.t.broken = true;
	f.c.publish("t", d, 3, 0, false);
	CHECK(f.c.state() == mqtt::State::Failed);
	CHECK(f.c.error() == mqtt::Error::Transport);
}

TEST_CASE("MQTT: QoS 2 traffic and a server-only packet are protocol errors") {
	Fixture f;
	f.connect_ok();
	f.t.feed(publish_packet("t", "x", 2, 7));
	f.c.poll(10);
	CHECK(f.c.error() == mqtt::Error::Protocol);

	Fixture g;
	g.connect_ok();
	g.t.feed({ 0x10, 0x00 });                        // CONNECT from the server
	g.c.poll(10);
	CHECK(g.c.error() == mqtt::Error::Protocol);
}
