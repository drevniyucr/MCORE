// Tests for the application UDP sockets (mcore/net/udp.cpp) and the drop of
// IP fragments in NET_ProcessRx.
#include "doctest.h"

#include <cstring>
#include <vector>

#include "net/udp.hpp"
#include "net/arp.hpp"
#include "net/net.hpp"
#include "helpers.hpp"
#include "test_support.hpp"

namespace {

constexpr uint32_t DESC4_UDP = ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_UDP;
constexpr uint16_t PORT = 4001;
constexpr uint16_t PEER_PORT = 50000;

struct Rx {
	int calls = 0;
	uint8_t ip[4] = {};
	uint16_t port = 0;
	std::vector<uint8_t> data;
};

void on_rx(void *user, const uint8_t *ip, uint16_t port, const uint8_t *d, uint16_t n) {
	auto *rx = static_cast<Rx*>(user);
	rx->calls++;
	memcpy(rx->ip, ip, 4);
	rx->port = port;
	rx->data.assign(d, d + n);
}

// Client -> us datagram; udp_len_field overrides the UDP length header field
void inject_udp(uint16_t dport, const uint8_t *payload, uint16_t n,
                int udp_len_field = -1, uint16_t frag = 0) {
	static uint8_t frame[ETH_MAX_PACKET_SIZE];
	const uint16_t l4_len = static_cast<uint16_t>(UDP_HDR_LEN + n);
	uint8_t *udp = build_ipv4(frame, 17, l4_len);
	frame[IP_HDR_POS + 6] = static_cast<uint8_t>(frag >> 8);
	frame[IP_HDR_POS + 7] = static_cast<uint8_t>(frag & 0xFF);
	const uint16_t len = static_cast<uint16_t>(udp_len_field < 0 ? l4_len : udp_len_field);
	udp[0] = PEER_PORT >> 8; udp[1] = PEER_PORT & 0xFF;
	udp[2] = static_cast<uint8_t>(dport >> 8); udp[3] = static_cast<uint8_t>(dport & 0xFF);
	udp[4] = static_cast<uint8_t>(len >> 8); udp[5] = static_cast<uint8_t>(len & 0xFF);
	if (n) memcpy(&udp[UDP_HDR_LEN], payload, n);
	process_rx(frame, static_cast<uint16_t>(ETH_HDR_LEN + IP_HDR_LEN + l4_len), DESC4_UDP);
}

uint8_t MY_IP[4] = { 192, 168, 0, 123 };

void setup() {
	test_reset();
	NET_SetIPAddr(MY_IP);
	NET_ARP_Flush();
	NET_UDP_Unbind(PORT);
	NET_UDP_Unbind(PORT + 1);
	NET_UDP_Unbind(PORT + 2);
	NET_UDP_Unbind(PORT + 3);
	NET_UDP_Unbind(PORT + 4);
}

} // namespace

TEST_CASE("UDP bind: the handler gets source and payload of a datagram") {
	setup();
	Rx rx;
	REQUIRE(NET_UDP_Bind(PORT, on_rx, &rx));
	const uint8_t msg[] = { 'G', 'E', 'T', 'V', 'E', 'R' };
	inject_udp(PORT, msg, sizeof(msg));

	CHECK(rx.calls == 1);
	CHECK(memcmp(rx.ip, CLIENT_IP, 4) == 0);
	CHECK(rx.port == PEER_PORT);
	CHECK(rx.data == std::vector<uint8_t>(msg, msg + sizeof(msg)));
	CHECK(g_tx_frames.empty());                     // no ICMP for a bound port
}

TEST_CASE("UDP bind: the UDP length field limits the payload, bad lengths are dropped") {
	setup();
	Rx rx;
	REQUIRE(NET_UDP_Bind(PORT, on_rx, &rx));
	const uint8_t msg[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

	inject_udp(PORT, msg, sizeof(msg), UDP_HDR_LEN + 3);      // Ethernet padding after 3 bytes
	CHECK(rx.calls == 1);
	CHECK(rx.data.size() == 3);

	inject_udp(PORT, msg, sizeof(msg), UDP_HDR_LEN + 100);    // claims more than the packet holds
	inject_udp(PORT, msg, sizeof(msg), 4);                    // shorter than its own header
	CHECK(rx.calls == 1);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("UDP bind: unbound ports still get ICMP unreachable, slots are limited") {
	setup();
	Rx rx;
	CHECK_FALSE(NET_UDP_Bind(0, on_rx, &rx));
	for (uint16_t i = 0; i < NET_UDP_MAX_BINDS; i++) {
		REQUIRE(NET_UDP_Bind(static_cast<uint16_t>(PORT + i), on_rx, &rx));
	}
	CHECK_FALSE(NET_UDP_Bind(PORT, on_rx, &rx));              // already bound
	CHECK_FALSE(NET_UDP_Bind(PORT + NET_UDP_MAX_BINDS, on_rx, &rx));   // table full

	NET_UDP_Unbind(PORT);
	inject_udp(PORT, nullptr, 0);
	CHECK(rx.calls == 0);
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(g_tx_frames[0].data[34] == 3);                       // ICMP destination unreachable
}

TEST_CASE("UDP send: to a resolved neighbour, PENDING while ARP resolves") {
	setup();
	const uint8_t msg[] = { 'o', 'k' };
	CHECK(NET_UDP_SendTo(PORT, CLIENT_IP, PEER_PORT, msg, sizeof(msg)) == net_result_t::PENDING);
	REQUIRE(g_tx_frames.size() == 1);                          // the ARP request
	CHECK(g_tx_frames[0].data[12] == 0x08);
	CHECK(g_tx_frames[0].data[13] == 0x06);

	inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);
	g_tx_frames.clear();
	CHECK(NET_UDP_SendTo(PORT, CLIENT_IP, PEER_PORT, msg, sizeof(msg)) == net_result_t::OK);
	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0].data;
	REQUIRE(f.size() == UDP_TEMPLATE_FRAME_LEN + sizeof(msg));
	CHECK(memcmp(&f[0], CLIENT_MAC, 6) == 0);
	CHECK(memcmp(&f[DST_ADDR_POS], CLIENT_IP, 4) == 0);
	CHECK(((f[UDP_HDR_POS] << 8) | f[UDP_HDR_POS + 1]) == PORT);
	CHECK(((f[UDP_HDR_POS + 2] << 8) | f[UDP_HDR_POS + 3]) == PEER_PORT);
	CHECK(memcmp(&f[UDP_TEMPLATE_FRAME_LEN], msg, sizeof(msg)) == 0);

	static uint8_t big[NET_UDP_MAX_PAYLOAD + 1];
	CHECK(NET_UDP_SendTo(PORT, CLIENT_IP, PEER_PORT, big, sizeof(big)) == net_result_t::UNREACHABLE);
}

TEST_CASE("RX: IP fragments are dropped, not parsed as UDP") {
	setup();
	Rx rx;
	REQUIRE(NET_UDP_Bind(PORT, on_rx, &rx));
	const uint8_t msg[] = { 'x' };
	inject_udp(PORT, msg, sizeof(msg), -1, 0x2000);           // first fragment (MF)
	inject_udp(PORT, msg, sizeof(msg), -1, 0x00B9);           // a later fragment (offset)
	CHECK(rx.calls == 0);
	CHECK(g_tx_frames.empty());
	inject_udp(PORT, msg, sizeof(msg), -1, 0x4000);           // DF only: a whole datagram
	CHECK(rx.calls == 1);
}
