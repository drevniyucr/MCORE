// Tests for the TCP state machine (mcore/net/tcp.cpp), driven through
// NET_ProcessTCP with fabricated segments. Time is controlled via the
// get_tick() stub (g_test_now), the ISN via the NET_Entropy() stub.
#include "doctest.h"

#include <cstring>

#include "net/tcp.hpp"
#include "net/net.hpp"
#include "net/eth_utils.hpp"
#include "helpers.hpp"
#include "test_support.hpp"

namespace {

constexpr uint16_t CPORT = 40000;   // client (remote) port
constexpr uint16_t SPORT = 5001;    // server (our) port
constexpr uint32_t CSEQ = 100;      // client initial sequence number

// SYN -> (SYN|ACK) -> ACK. Returns the established connection.
tcp_conn_t* do_handshake(uint16_t cport = CPORT) {
	inject_tcp(cport, SPORT, CSEQ, 0, TCP_SYN);
	inject_tcp(cport, SPORT, CSEQ + 1, TEST_ISN + 1, TCP_ACK);
	return find_conn(cport);
}

} // namespace

TEST_CASE("TCP handshake: SYN gets SYN|ACK and creates SYN_RCVD connection") {
	test_reset();
	NET_TCP_Init();

	inject_tcp(CPORT, SPORT, CSEQ, 0, TCP_SYN);

	tcp_conn_t *conn = find_conn(CPORT);
	REQUIRE(conn != nullptr);
	CHECK(conn->state == tcp_state_t::TCP_SYN_RCVD);
	CHECK(conn->rcv_next == CSEQ + 1);
	CHECK(conn->tcp_my_seq == TEST_ISN + 1);        // SYN consumed one seq
	CHECK(memcmp(conn->client_mac, CLIENT_MAC, 6) == 0);

	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	REQUIRE(f.data.size() == TCP_TEMPLATE_FRAME_LEN);
	CHECK(memcmp(&f.data[0], CLIENT_MAC, 6) == 0);
	const tcp_hdr_t *tcp = captured_tcp(f);
	CHECK(tcp->flags == (TCP_SYN | TCP_ACK));
	CHECK(bswap32(tcp->seq_num) == TEST_ISN);
	CHECK(bswap32(tcp->ack_num) == CSEQ + 1);
	CHECK(bswap16(tcp->src_port) == SPORT);
	CHECK(bswap16(tcp->dst_port) == CPORT);
	CHECK(bswap16(tcp->window) == SOCKET_RX_BUFF_LEN);
}

TEST_CASE("TCP handshake: final ACK establishes the connection") {
	test_reset();
	NET_TCP_Init();

	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	CHECK(conn->state == tcp_state_t::TCP_ESTABLISHED);
	CHECK(conn->snd_unack == TEST_ISN + 1);
	// Only the SYN|ACK was transmitted; the bare ACK produces nothing
	CHECK(g_tx_frames.size() == 1);
}

TEST_CASE("TCP: in-order data lands in the socket buffer and is ACKed") {
	test_reset();
	NET_TCP_Init();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	const uint8_t payload[] = { 'h', 'e', 'l', 'l', 'o' };
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1, TCP_ACK | TCP_PSH,
			payload, sizeof(payload));

	CHECK(conn->soc_rx_buff_pos == sizeof(payload));
	CHECK(memcmp(conn->socket_rx_buff, payload, sizeof(payload)) == 0);
	CHECK(conn->rcv_next == CSEQ + 1 + sizeof(payload));
	CHECK(conn->window_size == SOCKET_RX_BUFF_LEN - sizeof(payload));

	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *tcp = captured_tcp(g_tx_frames[0]);
	CHECK(tcp->flags == TCP_ACK);
	CHECK(bswap32(tcp->ack_num) == CSEQ + 1 + sizeof(payload));
	CHECK(bswap32(tcp->seq_num) == TEST_ISN + 1);
}

TEST_CASE("TCP: out-of-order segment is not buffered, duplicate ACK is sent") {
	test_reset();
	NET_TCP_Init();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	const uint8_t payload[] = { 'x' };
	inject_tcp(CPORT, SPORT, CSEQ + 50 /*gap*/, TEST_ISN + 1, TCP_ACK,
			payload, sizeof(payload));

	CHECK(conn->soc_rx_buff_pos == 0);
	CHECK(conn->rcv_next == CSEQ + 1);              // unchanged
	REQUIRE(g_tx_frames.size() == 1);               // duplicate ACK
	const tcp_hdr_t *tcp = captured_tcp(g_tx_frames[0]);
	CHECK(tcp->flags == TCP_ACK);
	CHECK(bswap32(tcp->ack_num) == CSEQ + 1);
}

TEST_CASE("TCP passive close: FIN -> ACK+FIN|ACK -> LAST_ACK -> closed") {
	test_reset();
	NET_TCP_Init();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1, TCP_FIN | TCP_ACK);

	CHECK(conn->state == tcp_state_t::TCP_LAST_ACK);
	CHECK(conn->rcv_next == CSEQ + 2);              // FIN consumed one seq
	REQUIRE(g_tx_frames.size() == 2);
	CHECK(captured_tcp(g_tx_frames[0])->flags == TCP_ACK);
	CHECK(captured_tcp(g_tx_frames[1])->flags == (TCP_FIN | TCP_ACK));
	CHECK(bswap32(captured_tcp(g_tx_frames[1])->ack_num) == CSEQ + 2);

	// Final ACK of our FIN releases the slot
	inject_tcp(CPORT, SPORT, CSEQ + 2, TEST_ISN + 2, TCP_ACK);
	CHECK(find_conn(CPORT) == nullptr);
}

TEST_CASE("TCP: RST from peer tears the connection down") {
	test_reset();
	NET_TCP_Init();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);

	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1, TCP_RST);
	CHECK(find_conn(CPORT) == nullptr);
}

TEST_CASE("TCP: SYN to a port already in use gets RST") {
	test_reset();
	NET_TCP_Init();
	REQUIRE(do_handshake() != nullptr);
	g_tx_frames.clear();

	inject_tcp(CPORT + 1, SPORT, 777, 0, TCP_SYN);  // second client, same port

	CHECK(find_conn(CPORT + 1) == nullptr);
	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *tcp = captured_tcp(g_tx_frames[0]);
	CHECK(tcp->flags == TCP_RST);
	CHECK(bswap32(tcp->ack_num) == 778);
	CHECK(bswap16(tcp->src_port) == SPORT);
	CHECK(bswap16(tcp->dst_port) == CPORT + 1);
}

TEST_CASE("TCP send: NET_TCP_SendUser emits PSH|ACK and blocks until ACKed") {
	test_reset();
	NET_TCP_Init();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	const uint8_t data[] = { 'p', 'i', 'n', 'g' };
	CHECK(NET_TCP_SendUser(conn, data, sizeof(data)) == 0);

	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	const tcp_hdr_t *tcp = captured_tcp(f);
	CHECK(tcp->flags == (TCP_ACK | TCP_PSH));
	CHECK(bswap32(tcp->seq_num) == TEST_ISN + 1);
	CHECK(memcmp(captured_tcp_payload(f), data, sizeof(data)) == 0);
	CHECK(conn->tcp_my_seq == TEST_ISN + 1 + sizeof(data));

	// Second send must be refused until the first is acknowledged
	CHECK(NET_TCP_SendUser(conn, data, sizeof(data)) == -2);

	// Peer ACKs our data -> sending unblocks
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1 + sizeof(data), TCP_ACK);
	CHECK(conn->snd_unack == TEST_ISN + 1 + sizeof(data));
	CHECK(NET_TCP_SendUser(conn, data, sizeof(data)) == 0);
}

TEST_CASE("TCP send: rejected when not established or oversized for window") {
	test_reset();
	NET_TCP_Init();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);

	const uint8_t data[] = { 'x' };
	CHECK(NET_TCP_SendUser(conn, nullptr, 1) == -1);
	CHECK(NET_TCP_SendUser(conn, data, 0) == -1);

	// client_window is 1024 (set by inject_tcp default)
	uint8_t big[1100] = {};
	CHECK(NET_TCP_SendUser(conn, big, sizeof(big)) == -1);
}

TEST_CASE("TCP retransmit: unACKed data is resent, then RST after max retries") {
	test_reset();
	NET_TCP_Init();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	const uint8_t data[] = { 'd', 'a', 't', 'a' };
	REQUIRE(NET_TCP_SendUser(conn, data, sizeof(data)) == 0);
	g_tx_frames.clear();

	// Not yet expired
	g_test_now += TCP_RETRANSMIT_TIMEOUT - 100;
	NET_TCP_Timers();
	CHECK(g_tx_frames.empty());

	// Expired -> retransmission of the saved segment
	g_test_now += 200;
	NET_TCP_Timers();
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(conn->retransmit_count == 1);
	const auto &rt = g_tx_frames[0];
	CHECK(captured_tcp(rt)->flags == (TCP_ACK | TCP_PSH));
	CHECK(memcmp(captured_tcp_payload(rt), data, sizeof(data)) == 0);

	// Exhaust remaining attempts, then expect RST + connection teardown
	for (int i = 0; i < TCP_RETRANSMISSION_MAX_COUNT - 1; i++) {
		g_test_now += TCP_RETRANSMIT_TIMEOUT + 1;
		NET_TCP_Timers();
	}
	CHECK(conn->retransmit_count == TCP_RETRANSMISSION_MAX_COUNT);

	g_test_now += TCP_RETRANSMIT_TIMEOUT + 1;
	NET_TCP_Timers();
	CHECK(find_conn(CPORT) == nullptr);
	REQUIRE(!g_tx_frames.empty());
	CHECK(captured_tcp(g_tx_frames.back())->flags == TCP_RST);
}

TEST_CASE("TCP keepalive: idle established connection gets probed") {
	test_reset();
	NET_TCP_Init();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	g_test_now += TCP_KEEPALIVE_TIMEOUT + 1;
	NET_TCP_Timers();

	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *tcp = captured_tcp(g_tx_frames[0]);
	CHECK(tcp->flags == TCP_ACK);
	// Keepalive probe uses seq = snd_next - 1
	CHECK(bswap32(tcp->seq_num) == conn->tcp_my_seq - 1);
	CHECK(conn->keep_alive_count == 1);
}

TEST_CASE("TCP: connection table exhaustion answers SYN with RST|ACK") {
	test_reset();
	NET_TCP_Init();
	for (uint16_t i = 0; i < TCP_MAX_CONNECTIONS; i++) {
		// Different server ports so port-in-use check doesn't fire
		inject_tcp(static_cast<uint16_t>(CPORT + i), static_cast<uint16_t>(SPORT + i),
				CSEQ, 0, TCP_SYN);
	}
	g_tx_frames.clear();

	inject_tcp(CPORT + 100, SPORT + 100, CSEQ, 0, TCP_SYN);

	CHECK(find_conn(CPORT + 100) == nullptr);
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(captured_tcp(g_tx_frames[0])->flags == (TCP_RST | TCP_ACK));
}
