// Tests for the client-side additions of the stack: ARP cache and routing
// (net/arp.cpp), UDP sockets (net.cpp), TCP active open (tcp.cpp) and the
// SNTP client (net/sntp.cpp).
#include "doctest.h"

#include <cstring>

#include "net/net.hpp"
#include "net/sntp.hpp"
#include "net/tcp.hpp"
#include "net/eth_utils.hpp"
#include "drivers/eth.hpp"
#include "helpers.hpp"
#include "test_support.hpp"

namespace {

constexpr uint32_t DESC4_UDP = ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_UDP;
const uint8_t MASK24[4] = { 255, 255, 255, 0 };
const uint8_t NO_GW[4] = { 0, 0, 0, 0 };
const uint8_t OUR_IP[4] = { 192, 168, 0, 123 };
uint8_t GW_IP[4] = { 192, 168, 0, 1 };
uint8_t GW_MAC[6] = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x01 };

void fresh(uint32_t now = 1000) {
	test_reset(now);
	NET_SetNetConfig(OUR_IP, MASK24, NO_GW);
	NET_TCP_Init();
	NET_SNTP_Stop();
}

bool is_arp_request_for(const CapturedFrame &f, const uint8_t *ip) {
	const auto &d = f.data;
	return d.size() == 42 && d[12] == 0x08 && d[13] == 0x06 && d[21] == 1 &&
	       memcmp(&d[0], "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0 &&
	       memcmp(&d[22], MAC_ADDR, 6) == 0 && memcmp(&d[28], IP_ADDR, 4) == 0 &&
	       memcmp(&d[38], ip, 4) == 0;
}

// ARP reply "ip is at mac", addressed to us
void inject_arp_reply(const uint8_t *ip, const uint8_t *mac) {
	uint8_t buf[42] = {};
	memcpy(&buf[0], MAC_ADDR, 6);
	memcpy(&buf[6], mac, 6);
	buf[12] = 0x08; buf[13] = 0x06;
	buf[14] = 0x00; buf[15] = 0x01;
	buf[16] = 0x08; buf[17] = 0x00;
	buf[18] = 6; buf[19] = 4;
	buf[20] = 0x00; buf[21] = 0x02;
	memcpy(&buf[22], mac, 6);
	memcpy(&buf[28], ip, 4);
	memcpy(&buf[32], MAC_ADDR, 6);
	memcpy(&buf[38], IP_ADDR, 4);
	process_rx(buf, sizeof(buf), 0);
}

// UDP datagram CLIENT_IP:sport -> us:dport
void inject_udp(uint16_t sport, uint16_t dport, const uint8_t *data, uint16_t len) {
	static uint8_t frame[ETH_MAX_PACKET_SIZE];
	const uint16_t l4_len = static_cast<uint16_t>(UDP_HDR_LEN + len);
	uint8_t *udp = build_ipv4(frame, 17, l4_len);
	udp[0] = static_cast<uint8_t>(sport >> 8); udp[1] = static_cast<uint8_t>(sport);
	udp[2] = static_cast<uint8_t>(dport >> 8); udp[3] = static_cast<uint8_t>(dport);
	udp[4] = static_cast<uint8_t>(l4_len >> 8); udp[5] = static_cast<uint8_t>(l4_len);
	memcpy(&udp[UDP_HDR_LEN], data, len);
	process_rx(frame, static_cast<uint16_t>(ETH_HDR_LEN + IP_HDR_LEN + l4_len), DESC4_UDP);
}

uint16_t rd16(const uint8_t *p) { return static_cast<uint16_t>((p[0] << 8) | p[1]); }

} // namespace

// ---------------------------------------------------------------------------
// ARP
// ---------------------------------------------------------------------------
TEST_CASE("ARP routing: on-link goes direct, off-link via gateway, none without one") {
	fresh();
	uint8_t hop[4];
	const uint8_t local[4] = { 192, 168, 0, 77 };
	const uint8_t remote[4] = { 8, 8, 8, 8 };

	REQUIRE(NET_ARP_NextHop(local, hop));
	CHECK(memcmp(hop, local, 4) == 0);
	CHECK_FALSE(NET_ARP_NextHop(remote, hop));

	NET_SetGateway(GW_IP);
	REQUIRE(NET_ARP_NextHop(remote, hop));
	CHECK(memcmp(hop, GW_IP, 4) == 0);
}

TEST_CASE("ARP resolve: request, rate limit, reply fills the cache") {
	fresh();
	uint8_t mac[6];
	CHECK_FALSE(NET_ARP_Resolve(CLIENT_IP, mac));
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(is_arp_request_for(g_tx_frames[0], CLIENT_IP));

	g_test_now += NET_ARP_RETRY_MS / 2;
	CHECK_FALSE(NET_ARP_Resolve(CLIENT_IP, mac));
	CHECK(g_tx_frames.size() == 1);                 // rate limited

	g_test_now += NET_ARP_RETRY_MS;
	CHECK_FALSE(NET_ARP_Resolve(CLIENT_IP, mac));
	CHECK(g_tx_frames.size() == 2);                 // retried

	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	g_tx_frames.clear();
	REQUIRE(NET_ARP_Resolve(CLIENT_IP, mac));
	CHECK(memcmp(mac, CLIENT_MAC, 6) == 0);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("ARP resolve: off-link destination resolves the gateway's MAC") {
	fresh();
	NET_SetGateway(GW_IP);
	const uint8_t remote[4] = { 10, 1, 2, 3 };
	uint8_t mac[6];
	CHECK_FALSE(NET_ARP_Resolve(remote, mac));
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(is_arp_request_for(g_tx_frames[0], GW_IP));

	inject_arp_reply(GW_IP, GW_MAC);
	REQUIRE(NET_ARP_Resolve(remote, mac));
	CHECK(memcmp(mac, GW_MAC, 6) == 0);
}

TEST_CASE("ARP: requests for our address teach us the sender") {
	fresh();
	uint8_t buf[42] = {};
	memset(buf, 0xFF, 6);
	memcpy(&buf[6], CLIENT_MAC, 6);
	buf[12] = 0x08; buf[13] = 0x06; buf[15] = 0x01; buf[16] = 0x08;
	buf[18] = 6; buf[19] = 4; buf[21] = 1;
	memcpy(&buf[22], CLIENT_MAC, 6);
	memcpy(&buf[28], CLIENT_IP, 4);
	memcpy(&buf[38], IP_ADDR, 4);
	process_rx(buf, sizeof(buf), 0);
	REQUIRE(g_tx_frames.size() == 1);               // our reply
	g_tx_frames.clear();

	uint8_t mac[6];
	REQUIRE(NET_ARP_Resolve(CLIENT_IP, mac));
	CHECK(memcmp(mac, CLIENT_MAC, 6) == 0);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("ARP: entries expire and are refreshed with a new request") {
	fresh();
	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	uint8_t mac[6];
	REQUIRE(NET_ARP_Resolve(CLIENT_IP, mac));
	CHECK(g_tx_frames.empty());

	g_test_now += NET_ARP_ENTRY_TTL_MS + 1;
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac));         // stale entry still usable...
	REQUIRE(g_tx_frames.size() == 1);               // ...while being refreshed
	CHECK(is_arp_request_for(g_tx_frames[0], CLIENT_IP));
}

// ---------------------------------------------------------------------------
// UDP sockets
// ---------------------------------------------------------------------------
namespace {
struct UdpCapture {
	int calls = 0;
	uint16_t src_port = 0;
	uint8_t data[64] = {};
	uint16_t len = 0;
};
void udp_capture(const uint8_t *, uint16_t src_port, const uint8_t *data, uint16_t len, void *ctx) {
	auto *c = static_cast<UdpCapture *>(ctx);
	c->calls += 1;
	c->src_port = src_port;
	c->len = len;
	memcpy(c->data, data, len < sizeof(c->data) ? len : sizeof(c->data));
}
} // namespace

TEST_CASE("UDP: bound port delivers payload, unbinding closes it") {
	fresh();
	UdpCapture cap;
	REQUIRE(NET_UDP_Bind(40000, udp_capture, &cap));
	const uint8_t msg[] = { 'p', 'i', 'n', 'g' };
	inject_udp(1234, 40000, msg, sizeof(msg));
	CHECK(cap.calls == 1);
	CHECK(cap.src_port == 1234);
	CHECK(cap.len == 4);
	CHECK(memcmp(cap.data, msg, 4) == 0);
	CHECK(g_tx_frames.empty());

	NET_UDP_Unbind(40000);
	inject_udp(1234, 40000, msg, sizeof(msg));
	CHECK(cap.calls == 1);
	REQUIRE(g_tx_frames.size() == 1);               // ICMP port unreachable now
	CHECK(g_tx_frames[0].data[IP_HDR_POS + 9] == 1);
}

TEST_CASE("UDP send: needs ARP first, then builds a correct datagram") {
	fresh();
	const uint8_t msg[] = { 1, 2, 3 };
	CHECK_FALSE(NET_UDP_SendTo(CLIENT_IP, 9999, 40001, msg, sizeof(msg)));
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(is_arp_request_for(g_tx_frames[0], CLIENT_IP));

	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	g_tx_frames.clear();
	REQUIRE(NET_UDP_SendTo(CLIENT_IP, 9999, 40001, msg, sizeof(msg)));
	REQUIRE(g_tx_frames.size() == 1);
	const auto &d = g_tx_frames[0].data;
	CHECK(memcmp(&d[0], CLIENT_MAC, 6) == 0);
	CHECK(memcmp(&d[DST_ADDR_POS], CLIENT_IP, 4) == 0);
	CHECK(rd16(&d[UDP_HDR_POS]) == 40001);
	CHECK(rd16(&d[UDP_HDR_POS + 2]) == 9999);
	CHECK(rd16(&d[UDP_HDR_POS + 4]) == UDP_HDR_LEN + 3);
	CHECK(d[UDP_HDR_POS + 8] == 1);
	CHECK(checksum(&d[IP_HDR_POS], IP_HDR_LEN) == 0);   // valid IP header checksum
}

// ---------------------------------------------------------------------------
// TCP active open
// ---------------------------------------------------------------------------
TEST_CASE("TCP connect: waits for ARP, then sends SYN with MSS") {
	fresh();
	CHECK(NET_TCP_Connect(CLIENT_IP, 8883) == nullptr);   // ARP request instead
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(is_arp_request_for(g_tx_frames[0], CLIENT_IP));

	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	g_tx_frames.clear();
	tcp_conn_t *c = NET_TCP_Connect(CLIENT_IP, 8883);
	REQUIRE(c != nullptr);
	CHECK(c->state == tcp_state_t::TCP_SYN_SENT);
	CHECK(c->server_port >= 49152);
	CHECK(NET_TCP_IsOpen(c, c->server_port));

	REQUIRE(g_tx_frames.size() == 1);
	const CapturedFrame &syn = g_tx_frames[0];
	const tcp_hdr_t *t = captured_tcp(syn);
	CHECK(memcmp(syn.data.data(), CLIENT_MAC, 6) == 0);
	CHECK(t->flags == TCP_SYN);
	CHECK(bswap16(t->dst_port) == 8883);
	CHECK(bswap32(t->seq_num) == TEST_ISN);
	CHECK((t->offset_reserved >> 4) == 6);           // MSS option present
	const uint8_t *opt = reinterpret_cast<const uint8_t *>(t) + TCP_HDR_LEN;
	CHECK(opt[0] == 2);
	CHECK(rd16(&opt[2]) == TCP_OUR_MSS);
}

TEST_CASE("TCP connect: SYN|ACK establishes, data then flows both ways") {
	fresh();
	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	tcp_conn_t *c = NET_TCP_Connect(CLIENT_IP, 8883);
	REQUIRE(c != nullptr);
	const uint16_t lport = c->server_port;
	g_tx_frames.clear();

	const uint32_t peer_isn = 5000;
	const uint8_t mss[4] = { 2, 4, 0x02, 0x00 };     // MSS 512
	inject_tcp(8883, lport, peer_isn, TEST_ISN + 1, TCP_SYN | TCP_ACK, nullptr, 0,
			8192, mss, sizeof(mss));
	CHECK(c->state == tcp_state_t::TCP_ESTABLISHED);
	CHECK(c->peer_mss == 512);
	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *ack = captured_tcp(g_tx_frames[0]);
	CHECK(ack->flags == TCP_ACK);
	CHECK(bswap32(ack->ack_num) == peer_isn + 1);
	CHECK(bswap32(ack->seq_num) == TEST_ISN + 1);

	// We send, they send
	g_tx_frames.clear();
	const uint8_t hello[] = "hello";
	CHECK(NET_TCP_SendUser(c, hello, 5) == 5);
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(memcmp(captured_tcp_payload(g_tx_frames[0]), hello, 5) == 0);

	const uint8_t world[] = "world";
	inject_tcp(8883, lport, peer_isn + 1, TEST_ISN + 6, TCP_ACK | TCP_PSH, world, 5, 8192);
	uint8_t buf[8] = {};
	CHECK(NET_TCP_Read(c, buf, sizeof(buf)) == 5);
	CHECK(memcmp(buf, world, 5) == 0);
}

TEST_CASE("TCP connect: RST|ACK (port closed) drops the connection") {
	fresh();
	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	tcp_conn_t *c = NET_TCP_Connect(CLIENT_IP, 8883);
	REQUIRE(c != nullptr);
	const uint16_t lport = c->server_port;

	inject_tcp(8883, lport, 0, TEST_ISN + 7, TCP_RST | TCP_ACK);   // wrong ACK: ignored
	CHECK(NET_TCP_IsOpen(c, lport));
	inject_tcp(8883, lport, 0, TEST_ISN + 1, TCP_RST | TCP_ACK);
	CHECK_FALSE(NET_TCP_IsOpen(c, lport));
}

TEST_CASE("TCP connect: SYN is retransmitted, then the attempt gives up") {
	fresh();
	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	tcp_conn_t *c = NET_TCP_Connect(CLIENT_IP, 8883);
	REQUIRE(c != nullptr);
	const uint16_t lport = c->server_port;
	g_tx_frames.clear();

	for (int i = 0; i < TCP_RETRANSMISSION_MAX_COUNT; ++i) {
		g_test_now += TCP_RETRANSMIT_TIMEOUT + 1;
		NET_TCP_Timers();
		REQUIRE(g_tx_frames.size() == static_cast<size_t>(i + 1));
		CHECK(captured_tcp(g_tx_frames.back())->flags == TCP_SYN);
	}
	g_test_now += TCP_RETRANSMIT_TIMEOUT + 1;
	NET_TCP_Timers();
	CHECK_FALSE(NET_TCP_IsOpen(c, lport));
}

TEST_CASE("TCP connect: each connection gets its own ephemeral port; abort sends RST") {
	fresh();
	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	tcp_conn_t *a = NET_TCP_Connect(CLIENT_IP, 8883);
	tcp_conn_t *b = NET_TCP_Connect(CLIENT_IP, 8883);
	REQUIRE(a != nullptr);
	REQUIRE(b != nullptr);
	CHECK(a->server_port != b->server_port);

	const uint16_t lport = a->server_port;
	g_tx_frames.clear();
	NET_TCP_Abort(a);
	CHECK_FALSE(NET_TCP_IsOpen(a, lport));
	REQUIRE(g_tx_frames.size() == 1);
	CHECK((captured_tcp(g_tx_frames[0])->flags & TCP_RST) != 0);
}

// ---------------------------------------------------------------------------
// SNTP
// ---------------------------------------------------------------------------
namespace {
// Server reply to the last captured request (originate = its transmit ts)
void inject_sntp_reply(const CapturedFrame &request, uint32_t tx_s, uint32_t tx_f,
		uint8_t stratum = 2, bool good_originate = true) {
	const uint8_t *req = request.data.data() + UDP_TEMPLATE_FRAME_LEN;
	uint8_t pkt[48] = {};
	pkt[0] = (0 << 6) | (4 << 3) | 4;               // server
	pkt[1] = stratum;
	memcpy(&pkt[24], &req[40], 8);                  // originate
	if (!good_originate) {
		pkt[31] ^= 0x01;
	}
	U32toBuffU8(&pkt[40], tx_s);
	U32toBuffU8(&pkt[44], tx_f);
	inject_udp(SNTP_SERVER_PORT, SNTP_LOCAL_PORT, pkt, sizeof(pkt));
}
} // namespace

TEST_CASE("SNTP: NTP timestamps convert to Unix time across the 2036 rollover") {
	CHECK(NET_SNTP_NtpToUnixMs(2208988800U, 0) == 0);                      // 1970-01-01
	CHECK(NET_SNTP_NtpToUnixMs(3976214400U, 0x80000000U) == 1767225600500ULL); // 2026-01-01 + 0.5 s
	// 2036-02-07 06:28:16 UTC is NTP era 1, second 0
	CHECK(NET_SNTP_NtpToUnixMs(0, 0) == 2085978496000ULL);
	CHECK(NET_SNTP_NtpToUnixMs(100, 0) == 2085978596000ULL);
}

TEST_CASE("SNTP: request after ARP, reply sets the clock (half-RTT corrected)") {
	fresh(5000);
	NET_SNTP_Start(CLIENT_IP);
	NET_SNTP_Poll();
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(is_arp_request_for(g_tx_frames[0], CLIENT_IP));
	CHECK_FALSE(NET_SNTP_IsSynced());

	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	g_tx_frames.clear();
	g_test_now += 300;
	NET_SNTP_Poll();
	REQUIRE(g_tx_frames.size() == 1);
	const CapturedFrame req = g_tx_frames[0];
	const uint8_t *ntp = req.data.data() + UDP_TEMPLATE_FRAME_LEN;
	CHECK(rd16(&req.data[UDP_HDR_POS]) == SNTP_LOCAL_PORT);
	CHECK(rd16(&req.data[UDP_HDR_POS + 2]) == SNTP_SERVER_PORT);
	CHECK(ntp[0] == 0x23);                          // v4, client

	g_test_now += 40;                               // 40 ms round trip
	inject_sntp_reply(req, 3976214400U, 0);         // 2026-01-01 00:00:00
	REQUIRE(NET_SNTP_IsSynced());
	CHECK(NET_SNTP_UnixMs() == 1767225600000ULL + 20);
	g_test_now += 1000;
	CHECK(NET_SNTP_UnixMs() == 1767225601000ULL + 20);
	CHECK(NET_SNTP_Stats().last_rtt_ms == 40);
}

TEST_CASE("SNTP: forged, kiss-o'-death and unsolicited replies are rejected") {
	fresh(5000);
	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	NET_SNTP_Start(CLIENT_IP);
	NET_SNTP_Poll();
	REQUIRE(g_tx_frames.size() == 1);
	const CapturedFrame req = g_tx_frames[0];
	const uint32_t rejected = NET_SNTP_Stats().rejected;

	inject_sntp_reply(req, 3976214400U, 0, 2, /*good_originate=*/false);
	inject_sntp_reply(req, 3976214400U, 0, /*stratum=*/0);
	CHECK_FALSE(NET_SNTP_IsSynced());
	CHECK(NET_SNTP_Stats().rejected == rejected + 2);

	// No answer: asks again after the retry interval
	g_tx_frames.clear();
	g_test_now += SNTP_RETRY_MS;
	NET_SNTP_Poll();
	CHECK(g_tx_frames.size() == 1);
}

TEST_CASE("SNTP: drift of a fast local oscillator is measured and corrected") {
	// Local tick 6300 ppm fast (the NUCLEO's ST-LINK MCO clock)
	fresh(5000);
	inject_arp_reply(CLIENT_IP, CLIENT_MAC);
	NET_SNTP_Start(CLIENT_IP);
	auto ntp_at = [](uint64_t unix_ms, uint32_t &s, uint32_t &f) {
		s = static_cast<uint32_t>(unix_ms / 1000U + 2208988800ULL);
		f = static_cast<uint32_t>(((unix_ms % 1000U) << 32) / 1000U);
	};
	const uint64_t T0 = 1767225600000ULL;            // server time at the first reply
	uint32_t s = 0, f = 0;

	NET_SNTP_Poll();
	REQUIRE(g_tx_frames.size() == 1);
	ntp_at(T0, s, f);
	inject_sntp_reply(g_tx_frames.back(), s, f);     // rtt 0
	REQUIRE(NET_SNTP_IsSynced());
	CHECK(NET_SNTP_Stats().interval_ms == SNTP_MIN_INTERVAL_MS);

	// 64 s of LOCAL time pass; the server only advanced 64000 / 1.0063 ms
	const uint32_t local = SNTP_MIN_INTERVAL_MS;
	const uint64_t true_elapsed = static_cast<uint64_t>(local / 1.0063);
	g_test_now += local;
	g_tx_frames.clear();
	NET_SNTP_Poll();
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(NET_SNTP_UnixMs() - (T0 + true_elapsed) > 300);   // uncorrected: ~400 ms ahead
	ntp_at(T0 + true_elapsed, s, f);
	inject_sntp_reply(g_tx_frames.back(), s, f);
	CHECK(NET_SNTP_Stats().drift_ppm > 6200);
	CHECK(NET_SNTP_Stats().drift_ppm < 6400);
	CHECK(NET_SNTP_Stats().last_offset_ms > 300);
	CHECK(NET_SNTP_Stats().interval_ms == 2 * SNTP_MIN_INTERVAL_MS);

	// Another minute of local time: the corrected clock tracks the server
	g_test_now += 60000;
	const int64_t truth = static_cast<int64_t>(T0 + true_elapsed + static_cast<uint64_t>(60000 / 1.0063));
	const int64_t err = static_cast<int64_t>(NET_SNTP_UnixMs()) - truth;
	CHECK(err > -5);
	CHECK(err < 5);
}
