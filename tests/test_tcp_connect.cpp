// Tests for the TCP active open (NET_TCP_Connect, SYN_SENT) in
// mcore/net/tcp.cpp. The simulated peer CLIENT_IP/CLIENT_MAC plays the
// remote server, so inject_tcp() fabricates its segments.
#include "doctest.h"

#include <cstring>

#include "net/tcp.hpp"
#include "net/arp.hpp"
#include "net/net.hpp"
#include "net/eth_utils.hpp"
#include "helpers.hpp"
#include "test_support.hpp"

namespace {

uint8_t MY_IP[4] = { 192, 168, 0, 123 };
uint8_t MASK24[4] = { 255, 255, 255, 0 };
uint8_t NO_IP[4] = { 0, 0, 0, 0 };
constexpr uint16_t SERVER_PORT = 1883;
constexpr uint32_t SERVER_ISN = 0x50000000;

void connect_setup(bool server_mac_known = true) {
	test_reset();
	NET_TCP_Init();
	NET_SetIPAddr(MY_IP);
	NET_SetRoute(MASK24, NO_IP, NO_IP);
	NET_ARP_Flush();
	if (server_mac_known) {
		inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);
	}
	g_tx_frames.clear();
}

// Connect and return the connection in SYN_SENT
tcp_conn_t* start() {
	tcp_conn_t *c = nullptr;
	REQUIRE(NET_TCP_Connect(CLIENT_IP, SERVER_PORT, &c) == net_result_t::OK);
	REQUIRE(c != nullptr);
	return c;
}

const uint8_t MSS_1000[4] = { 2, 4, 0x03, 0xE8 };

// Server answers our SYN; returns the established connection
tcp_conn_t* establish() {
	tcp_conn_t *c = start();
	inject_tcp(SERVER_PORT, c->server_port, SERVER_ISN, TEST_ISN + 1, TCP_SYN | TCP_ACK,
	           nullptr, 0, 4096, MSS_1000, sizeof(MSS_1000));
	REQUIRE(c->state == tcp_state_t::TCP_ESTABLISHED);
	return c;
}

} // namespace

TEST_CASE("TCP connect: SYN with MSS to the server from an ephemeral port") {
	connect_setup();
	tcp_conn_t *c = start();
	CHECK(c->state == tcp_state_t::TCP_SYN_SENT);
	CHECK(c->server_port >= TCP_EPHEMERAL_BASE);
	CHECK(NET_TCP_IsOpen(c, c->server_port));

	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	CHECK(memcmp(&f.data[0], CLIENT_MAC, 6) == 0);
	CHECK(memcmp(&f.data[DST_ADDR_POS], CLIENT_IP, 4) == 0);
	const tcp_hdr_t *t = captured_tcp(f);
	CHECK(t->flags == TCP_SYN);
	CHECK(bswap16(t->src_port) == c->server_port);
	CHECK(bswap16(t->dst_port) == SERVER_PORT);
	CHECK(bswap32(t->seq_num) == TEST_ISN);
	CHECK((t->offset_reserved >> 4) == 6);         // 20-byte header + MSS option
	const uint8_t *opt = reinterpret_cast<const uint8_t*>(t) + TCP_HDR_LEN;
	CHECK(opt[0] == 2);
	CHECK(((opt[2] << 8) | opt[3]) == TCP_OUR_MSS);
}

TEST_CASE("TCP connect: PENDING while ARP resolves the server, then OK") {
	connect_setup(/*server_mac_known=*/false);
	tcp_conn_t *c = nullptr;
	CHECK(NET_TCP_Connect(CLIENT_IP, SERVER_PORT, &c) == net_result_t::PENDING);
	CHECK(c == nullptr);
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(g_tx_frames[0].data[13] == 0x06);        // an ARP request

	inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);
	CHECK(NET_TCP_Connect(CLIENT_IP, SERVER_PORT, &c) == net_result_t::OK);
	CHECK(c != nullptr);
}

TEST_CASE("TCP connect: off-subnet without a gateway is UNREACHABLE") {
	connect_setup();
	const uint8_t remote[4] = { 8, 8, 8, 8 };
	tcp_conn_t *c = nullptr;
	CHECK(NET_TCP_Connect(remote, SERVER_PORT, &c) == net_result_t::UNREACHABLE);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("TCP connect: SYN|ACK establishes, takes the peer MSS, ACKs it") {
	connect_setup();
	tcp_conn_t *c = establish();
	CHECK(c->peer_mss == 1000);
	CHECK(c->rcv_next == SERVER_ISN + 1);
	CHECK(c->client_window == 4096);

	const tcp_hdr_t *t = captured_tcp(g_tx_frames.back());
	CHECK(t->flags == TCP_ACK);
	CHECK(bswap32(t->seq_num) == TEST_ISN + 1);
	CHECK(bswap32(t->ack_num) == SERVER_ISN + 1);

	// The SYN left the retransmission queue: nothing is resent later
	const size_t sent = g_tx_frames.size();
	g_test_now += TCP_RETRANSMIT_TIMEOUT + 1;
	NET_TCP_Timers();
	CHECK(g_tx_frames.size() == sent);
}

TEST_CASE("TCP connect: data both ways once established") {
	connect_setup();
	tcp_conn_t *c = establish();

	const uint8_t hello[] = { 'h', 'i' };
	CHECK(NET_TCP_SendUser(c, hello, sizeof(hello)) == 2);
	const auto &out = g_tx_frames.back();
	CHECK(bswap32(captured_tcp(out)->seq_num) == TEST_ISN + 1);
	CHECK(memcmp(captured_tcp_payload(out), hello, 2) == 0);

	const uint8_t reply[] = { 'o', 'k', '!' };
	inject_tcp(SERVER_PORT, c->server_port, SERVER_ISN + 1, TEST_ISN + 3, TCP_ACK | TCP_PSH,
	           reply, sizeof(reply));
	uint8_t buf[8];
	CHECK(NET_TCP_Read(c, buf, sizeof(buf)) == 3);
	CHECK(memcmp(buf, reply, 3) == 0);
}

TEST_CASE("TCP connect: RST|ACK = refused, the slot is released") {
	connect_setup();
	tcp_conn_t *c = start();
	const uint16_t port = c->server_port;
	inject_tcp(SERVER_PORT, port, 0, TEST_ISN + 1, TCP_RST | TCP_ACK);
	CHECK_FALSE(NET_TCP_IsOpen(c, port));
}

TEST_CASE("TCP connect: an RST that does not ack our SYN is ignored") {
	connect_setup();
	tcp_conn_t *c = start();
	inject_tcp(SERVER_PORT, c->server_port, 0, TEST_ISN + 7, TCP_RST | TCP_ACK);
	CHECK(c->state == tcp_state_t::TCP_SYN_SENT);
}

TEST_CASE("TCP connect: SYN|ACK with a wrong ACK is reset, we keep waiting") {
	connect_setup();
	tcp_conn_t *c = start();
	g_tx_frames.clear();
	inject_tcp(SERVER_PORT, c->server_port, SERVER_ISN, 0xDEAD0000, TCP_SYN | TCP_ACK);

	CHECK(c->state == tcp_state_t::TCP_SYN_SENT);
	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *t = captured_tcp(g_tx_frames[0]);
	CHECK(t->flags == TCP_RST);
	CHECK(bswap32(t->seq_num) == 0xDEAD0000);      // seq = their bogus ACK
}

TEST_CASE("TCP connect: SYN retransmitted, then given up with RST") {
	connect_setup();
	tcp_conn_t *c = start();
	const uint16_t port = c->server_port;

	for (uint8_t i = 0; i < TCP_RETRANSMISSION_MAX_COUNT; i++) {
		g_test_now += TCP_RETRANSMIT_TIMEOUT + 1;
		NET_TCP_Timers();
		REQUIRE(g_tx_frames.size() == i + 2u);
		CHECK(captured_tcp(g_tx_frames.back())->flags == TCP_SYN);
	}
	g_test_now += TCP_RETRANSMIT_TIMEOUT + 1;
	NET_TCP_Timers();
	CHECK((captured_tcp(g_tx_frames.back())->flags & TCP_RST) != 0);
	CHECK_FALSE(NET_TCP_IsOpen(c, port));
}

TEST_CASE("TCP connect: each connection gets its own local port") {
	connect_setup();
	tcp_conn_t *a = start();
	tcp_conn_t *b = start();
	CHECK(a != b);
	CHECK(a->server_port != b->server_port);
}

TEST_CASE("TCP connect: Abort sends RST and frees the slot") {
	connect_setup();
	tcp_conn_t *c = establish();
	const uint16_t port = c->server_port;
	NET_TCP_Abort(c);
	CHECK((captured_tcp(g_tx_frames.back())->flags & TCP_RST) != 0);
	CHECK_FALSE(NET_TCP_IsOpen(c, port));
}

TEST_CASE("TCP connect: Close in SYN_SENT aborts") {
	connect_setup();
	tcp_conn_t *c = start();
	const uint16_t port = c->server_port;
	NET_TCP_Close(c);
	CHECK_FALSE(NET_TCP_IsOpen(c, port));
}

TEST_CASE("TCP connect: the reserves keep our own connections possible") {
	connect_setup();
	constexpr uint16_t LPORT = 5001;
	NET_TCP_Listen(LPORT);
	// Incoming connections take every control block they may...
	uint16_t passive = 0;
	for (uint16_t p = 30000; ; p++) {
		inject_tcp(p, LPORT, 1000, 0, TCP_SYN);
		if (find_conn(p) == nullptr) break;
		inject_tcp(p, LPORT, 1001, TEST_ISN + 1, TCP_ACK);
		passive++;
	}
	CHECK(passive == TCP_MAX_CONNECTIONS - TCP_TCB_RESERVE);
	// ...and all the pool they may: fill each RX share until the window closes
	static uint8_t chunk[1024];
	for (uint16_t p = 30000; p < 30000 + passive; p++) {
		tcp_conn_t *c = find_conn(p);
		for (int i = 0; i < 16 && tcpbuf::room(c->rx, TCP_CONN_RX_BLOCKS, TCP_POOL_RESERVE_BLOCKS) > 0; i++) {
			inject_tcp(p, LPORT, 1001 + NET_TCP_Available(c), TEST_ISN + 1, TCP_ACK, chunk, sizeof(chunk));
		}
	}
	CHECK(tcpbuf::free_blocks() == TCP_POOL_RESERVE_BLOCKS);

	// Our own connection still gets a control block and buffer space
	g_tx_frames.clear();
	tcp_conn_t *c = establish();
	CHECK(c->active_open);
	CHECK(c->window_size == TCP_POOL_RESERVE_BLOCKS * TCP_BLOCK_SIZE);   // the reserve
	const uint8_t msg[] = { 'h', 'i' };
	CHECK(NET_TCP_SendUser(c, msg, sizeof(msg)) == sizeof(msg));
}
