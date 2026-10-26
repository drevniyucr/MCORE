// Regression tests for malformed lengths (found in the security audit):
// header lengths that do not fit the packet must be dropped, never turned
// into a negative payload length that reads past the frame.
#include "doctest.h"

#include <cstring>

#include "net/tcp.hpp"
#include "net/net.hpp"
#include "net/dhcp.hpp"
#include "net/eth_utils.hpp"
#include "helpers.hpp"
#include "test_support.hpp"

namespace {

constexpr uint16_t PORT = 5001;
constexpr uint32_t DESC4_ICMP = ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_ICMP;
constexpr uint32_t DESC4_UDP = ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_UDP;
constexpr uint32_t DESC4_TCP = ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_TCP;
uint8_t MY_IP[4] = { 192, 168, 0, 123 };

tcp_conn_t* established() {
	test_reset();
	NET_SetIPAddr(MY_IP);
	NET_TCP_Init();
	NET_TCP_Listen(PORT);
	inject_tcp(40000, PORT, 1000, 0, TCP_SYN);
	tcp_conn_t *c = find_conn(40000);
	REQUIRE(c != nullptr);
	inject_tcp(40000, PORT, 1001, c->tcp_my_seq, TCP_ACK);
	REQUIRE(c->state == tcp_state_t::TCP_ESTABLISHED);
	g_tx_frames.clear();
	return c;
}

// TCP segment whose data offset claims `hdr_len` bytes while the IP header
// says the whole segment is `ip_l4_len` bytes long
ipv4_frame bogus_tcp(uint8_t *buf, uint32_t seq, uint32_t ack, uint8_t flags,
		uint8_t hdr_len, uint16_t ip_l4_len) {
	uint8_t *l4 = build_ipv4(buf, IP_PROTO_TCP, hdr_len);
	tcp_hdr_t *t = reinterpret_cast<tcp_hdr_t*>(l4);
	t->src_port = bswap16(40000);
	t->dst_port = bswap16(PORT);
	t->seq_num = bswap32(seq);
	t->ack_num = bswap32(ack);
	t->offset_reserved = static_cast<uint8_t>((hdr_len / 4) << 4);
	t->flags = flags;
	t->window = bswap16(1024);
	ipv4_frame f = make_ipv4_frame(buf, hdr_len);
	f.ip_len = static_cast<uint16_t>(IP_HDR_LEN + ip_l4_len);
	return f;
}

} // namespace

TEST_CASE("RX lengths: TCP header longer than the IP payload is dropped") {
	tcp_conn_t *c = established();
	static uint8_t buf[ETH_MAX_PACKET_SIZE];
	// 60-byte TCP header claimed, 20 bytes of TCP in the IP packet: the old
	// code computed 20 - 60 = 65496 payload bytes and copied 5120 of memory
	ipv4_frame f = bogus_tcp(buf, 1001, c->tcp_my_seq, TCP_ACK | TCP_PSH, 60, 20);
	NET_ProcessTCP(&f);
	CHECK(NET_TCP_Available(c) == 0);
	CHECK(c->rcv_next == 1001);
	CHECK(g_tx_frames.empty());                    // no ACK, no RST to a forged segment
}

TEST_CASE("RX lengths: a SYN whose options run past the packet opens nothing") {
	test_reset();
	NET_SetIPAddr(MY_IP);
	NET_TCP_Init();
	NET_TCP_Listen(PORT);
	static uint8_t buf[ETH_MAX_PACKET_SIZE];
	ipv4_frame f = bogus_tcp(buf, 1000, 0, TCP_SYN, 60, 24);
	NET_ProcessTCP(&f);
	CHECK(find_conn(40000) == nullptr);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("RX lengths: TCP shorter than its minimal header is dropped") {
	tcp_conn_t *c = established();
	static uint8_t buf[ETH_MAX_PACKET_SIZE];
	ipv4_frame f = bogus_tcp(buf, 1001, c->tcp_my_seq, TCP_ACK | TCP_PSH, 20, 12);
	NET_ProcessTCP(&f);
	CHECK(NET_TCP_Available(c) == 0);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("RX lengths: IP total length below the IP header is dropped") {
	test_reset();
	NET_SetIPAddr(MY_IP);
	static uint8_t buf[128];
	uint8_t *icmp = build_ipv4(buf, 1, 8);
	icmp[0] = 8;                                   // echo request
	buf[IP_HDR_POS + 2] = 0;
	buf[IP_HDR_POS + 3] = 10;                      // total length 10 < IHL 20
	process_rx(buf, ETH_HDR_LEN + IP_HDR_LEN + 8, DESC4_ICMP);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("RX lengths: UDP/TCP without room for their header are dropped by NET_ProcessRx") {
	test_reset();
	NET_SetIPAddr(MY_IP);
	NET_TCP_Init();
	NET_TCP_Listen(PORT);
	static uint8_t buf[128];

	// 4 bytes of "UDP" to a closed port: would answer port unreachable
	uint8_t *udp = build_ipv4(buf, IP_PROTO_UDP, 4);
	udp[3] = 99;
	process_rx(buf, ETH_HDR_LEN + IP_HDR_LEN + 4, DESC4_UDP);
	CHECK(g_tx_frames.empty());

	// 12 bytes of "TCP" SYN to a listening port
	uint8_t *tcp = build_ipv4(buf, IP_PROTO_TCP, 12);
	tcp[1] = 0x40; tcp[3] = static_cast<uint8_t>(PORT & 0xFF); tcp[2] = static_cast<uint8_t>(PORT >> 8);
	process_rx(buf, ETH_HDR_LEN + IP_HDR_LEN + 12, DESC4_TCP);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("RX lengths: DHCP ignores a datagram shorter than its UDP header") {
	test_reset();
	NET_SetIPAddr(MY_IP);
	NET_DHCP_Start();
	const dhcp_state_t before = dhcp_client.state;
	g_tx_frames.clear();
	static uint8_t buf[600];
	uint8_t *udp = build_ipv4(buf, IP_PROTO_UDP, 400);
	udp[1] = 67; udp[3] = 68;
	ipv4_frame f = make_ipv4_frame(buf, 400);
	f.ip_len = IP_HDR_LEN + 4;                     // old code: bootp_len = 4 - 8 = 65532
	NET_DHCP_ProcessRx(&f);
	CHECK(dhcp_client.state == before);
	CHECK(g_tx_frames.empty());
}
