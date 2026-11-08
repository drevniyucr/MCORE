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

// SYN|ACK now carries a 4-byte MSS option
constexpr size_t SYNACK_FRAME_LEN = TCP_TEMPLATE_FRAME_LEN + 4;

// Fresh stack listening on SPORT
void setup() {
	test_reset();
	NET_TCP_Init();
	NET_TCP_Listen(SPORT);
}

// SYN -> (SYN|ACK) -> ACK. Returns the established connection.
tcp_conn_t* do_handshake(uint16_t cport = CPORT) {
	inject_tcp(cport, SPORT, CSEQ, 0, TCP_SYN);
	inject_tcp(cport, SPORT, CSEQ + 1, TEST_ISN + 1, TCP_ACK);
	return find_conn(cport);
}

} // namespace

TEST_CASE("TCP handshake: SYN gets SYN|ACK and creates SYN_RCVD connection") {
	setup();

	inject_tcp(CPORT, SPORT, CSEQ, 0, TCP_SYN);

	tcp_conn_t *conn = find_conn(CPORT);
	REQUIRE(conn != nullptr);
	CHECK(conn->state == tcp_state_t::TCP_SYN_RCVD);
	CHECK(conn->rcv_next == CSEQ + 1);
	CHECK(conn->tcp_my_seq == TEST_ISN + 1);        // SYN consumed one seq
	CHECK(memcmp(conn->client_mac, CLIENT_MAC, 6) == 0);

	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	REQUIRE(f.data.size() == SYNACK_FRAME_LEN);
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
	setup();

	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	CHECK(conn->state == tcp_state_t::TCP_ESTABLISHED);
	CHECK(conn->snd_unack == TEST_ISN + 1);
	// Only the SYN|ACK was transmitted; the bare ACK produces nothing
	CHECK(g_tx_frames.size() == 1);
}

TEST_CASE("TCP: in-order data lands in the socket buffer and is ACKed") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	const uint8_t payload[] = { 'h', 'e', 'l', 'l', 'o' };
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1, TCP_ACK | TCP_PSH,
			payload, sizeof(payload));

	CHECK(NET_TCP_Available(conn) == sizeof(payload));
	uint8_t out[16] = {};
	CHECK(NET_TCP_Read(conn, out, sizeof(out)) == sizeof(payload));
	CHECK(memcmp(out, payload, sizeof(payload)) == 0);
	CHECK(NET_TCP_Available(conn) == 0);
	CHECK(conn->rcv_next == CSEQ + 1 + sizeof(payload));
	CHECK(conn->window_size == SOCKET_RX_BUFF_LEN);  // restored by the read

	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *tcp = captured_tcp(g_tx_frames[0]);
	CHECK(tcp->flags == TCP_ACK);
	CHECK(bswap32(tcp->ack_num) == CSEQ + 1 + sizeof(payload));
	CHECK(bswap32(tcp->seq_num) == TEST_ISN + 1);
}

TEST_CASE("TCP: out-of-order segment is not buffered, duplicate ACK is sent") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	const uint8_t payload[] = { 'x' };
	inject_tcp(CPORT, SPORT, CSEQ + 50 /*gap*/, TEST_ISN + 1, TCP_ACK,
			payload, sizeof(payload));

	CHECK(NET_TCP_Available(conn) == 0);
	CHECK(conn->rcv_next == CSEQ + 1);              // unchanged
	REQUIRE(g_tx_frames.size() == 1);               // duplicate ACK
	const tcp_hdr_t *tcp = captured_tcp(g_tx_frames[0]);
	CHECK(tcp->flags == TCP_ACK);
	CHECK(bswap32(tcp->ack_num) == CSEQ + 1);
}

TEST_CASE("TCP passive close: FIN -> ACK+FIN|ACK -> LAST_ACK -> closed") {
	setup();
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

TEST_CASE("TCP: in-sequence RST tears the connection down") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);

	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1, TCP_RST);
	CHECK(find_conn(CPORT) == nullptr);
}

TEST_CASE("TCP: out-of-sequence RST is ignored (anti-spoofing)") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);

	inject_tcp(CPORT, SPORT, CSEQ + 999, TEST_ISN + 1, TCP_RST);
	REQUIRE(find_conn(CPORT) != nullptr);
	CHECK(conn->state == tcp_state_t::TCP_ESTABLISHED);
}

TEST_CASE("TCP: forged ACK beyond snd_next does not move snd_unack") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	const uint32_t unack_before = conn->snd_unack;

	// ACK for data we never sent
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 5000, TCP_ACK);
	CHECK(conn->snd_unack == unack_before);
}

TEST_CASE("TCP MSS: option parsed from SYN, default without it") {
	setup();
	// SYN carrying MSS = 1400
	const uint8_t mss_opt[4] = { 2, 4, 0x05, 0x78 };
	inject_tcp(CPORT, SPORT, CSEQ, 0, TCP_SYN, nullptr, 0, 1024,
			mss_opt, sizeof(mss_opt));
	tcp_conn_t *conn = find_conn(CPORT);
	REQUIRE(conn != nullptr);
	CHECK(conn->peer_mss == 1400);

	// SYN without options -> RFC default
	inject_tcp(CPORT + 1, SPORT, CSEQ, 0, TCP_SYN);
	REQUIRE(find_conn(CPORT + 1) != nullptr);
	CHECK(find_conn(CPORT + 1)->peer_mss == TCP_DEFAULT_PEER_MSS);
}

TEST_CASE("TCP MSS: SYN|ACK advertises our MSS option") {
	setup();
	inject_tcp(CPORT, SPORT, CSEQ, 0, TCP_SYN);

	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	const tcp_hdr_t *tcp = captured_tcp(f);
	CHECK((tcp->offset_reserved >> 4) == 6);        // 20B header + 4B options
	const uint8_t *opt = f.data.data() + ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN;
	CHECK(opt[0] == 2);
	CHECK(opt[1] == 4);
	CHECK(((opt[2] << 8) | opt[3]) == TCP_OUR_MSS);
}

TEST_CASE("TCP RX ring: data survives wrap-around of the ring buffer") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);

	// Sequential byte pattern so any reordering/corruption is visible
	static uint8_t chunk[1000];
	uint8_t out[1000];
	uint32_t seq = CSEQ + 1;
	uint32_t pattern = 0;
	uint32_t verify = 0;

	// Fill 5 KB, read 4 KB, fill 3 KB more (write crosses the wrap point)
	for (int i = 0; i < 5; i++) {
		for (auto &b : chunk) b = static_cast<uint8_t>(pattern++);
		inject_tcp(CPORT, SPORT, seq, TEST_ISN + 1, TCP_ACK, chunk, sizeof(chunk));
		seq += sizeof(chunk);
	}
	REQUIRE(NET_TCP_Available(conn) == 5000);
	for (int i = 0; i < 4; i++) {
		REQUIRE(NET_TCP_Read(conn, out, sizeof(out)) == 1000);
		for (auto b : out) REQUIRE(b == static_cast<uint8_t>(verify++));
	}
	for (int i = 0; i < 3; i++) {
		for (auto &b : chunk) b = static_cast<uint8_t>(pattern++);
		inject_tcp(CPORT, SPORT, seq, TEST_ISN + 1, TCP_ACK, chunk, sizeof(chunk));
		seq += sizeof(chunk);
	}
	REQUIRE(NET_TCP_Available(conn) == 4000);
	for (int i = 0; i < 4; i++) {
		REQUIRE(NET_TCP_Read(conn, out, sizeof(out)) == 1000);
		for (auto b : out) REQUIRE(b == static_cast<uint8_t>(verify++));
	}
	CHECK(NET_TCP_Available(conn) == 0);
}

TEST_CASE("TCP RX window: closes when full, reading sends a window update") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);

	// Fill the whole 5120-byte buffer
	static uint8_t chunk[1280];
	uint32_t seq = CSEQ + 1;
	for (int i = 0; i < 4; i++) {
		inject_tcp(CPORT, SPORT, seq, TEST_ISN + 1, TCP_ACK, chunk, sizeof(chunk));
		seq += sizeof(chunk);
	}
	CHECK(conn->window_size == 0);
	// The last in-band ACK must already advertise the zero window
	REQUIRE(!g_tx_frames.empty());
	CHECK(bswap16(captured_tcp(g_tx_frames.back())->window) == 0);

	// Data beyond the closed window is dropped and NOT acked forward
	g_tx_frames.clear();
	inject_tcp(CPORT, SPORT, seq, TEST_ISN + 1, TCP_ACK, chunk, sizeof(chunk));
	CHECK(conn->rcv_next == seq);                   // nothing consumed
	CHECK(NET_TCP_Available(conn) == SOCKET_RX_BUFF_LEN);

	// Reading reopens the window and emits exactly one window-update ACK
	g_tx_frames.clear();
	static uint8_t sink[SOCKET_RX_BUFF_LEN];
	CHECK(NET_TCP_Read(conn, sink, sizeof(sink)) == SOCKET_RX_BUFF_LEN);
	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *upd = captured_tcp(g_tx_frames[0]);
	CHECK(upd->flags == TCP_ACK);
	CHECK(bswap16(upd->window) == SOCKET_RX_BUFF_LEN);
}

TEST_CASE("TCP active close: FIN|ACK -> FIN_WAIT_2 -> TIME_WAIT -> released") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	NET_TCP_Close(conn);
	CHECK(conn->state == tcp_state_t::TCP_FIN_WAIT_1);
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(captured_tcp(g_tx_frames[0])->flags == (TCP_FIN | TCP_ACK));
	CHECK(bswap32(captured_tcp(g_tx_frames[0])->seq_num) == TEST_ISN + 1);

	// Peer ACKs our FIN (ack = FIN seq + 1)
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 2, TCP_ACK);
	CHECK(conn->state == tcp_state_t::TCP_FIN_WAIT_2);

	// Peer sends its FIN -> we ACK and enter TIME_WAIT
	g_tx_frames.clear();
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 2, TCP_FIN | TCP_ACK);
	CHECK(conn->state == tcp_state_t::TCP_TIME_WAIT);
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(captured_tcp(g_tx_frames[0])->flags == TCP_ACK);
	CHECK(bswap32(captured_tcp(g_tx_frames[0])->ack_num) == CSEQ + 2);

	// Retransmitted FIN in TIME_WAIT is re-ACKed
	g_tx_frames.clear();
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 2, TCP_FIN | TCP_ACK);
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(captured_tcp(g_tx_frames[0])->flags == TCP_ACK);

	// Slot survives TIME_WAIT and is released by the timer
	NET_TCP_Timers();
	CHECK(find_conn(CPORT) != nullptr);
	g_test_now += TCP_TIME_WAIT_TIMEOUT + 1;
	NET_TCP_Timers();
	CHECK(find_conn(CPORT) == nullptr);
}

TEST_CASE("TCP listen: second client on the same port also connects") {
	setup();
	REQUIRE(do_handshake(CPORT) != nullptr);

	tcp_conn_t *second = do_handshake(CPORT + 1);
	REQUIRE(second != nullptr);
	CHECK(second->state == tcp_state_t::TCP_ESTABLISHED);
	CHECK(find_conn(CPORT)->state == tcp_state_t::TCP_ESTABLISHED);
}

TEST_CASE("TCP listen: SYN to a non-listening port gets RST|ACK") {
	setup();
	g_tx_frames.clear();

	inject_tcp(CPORT, SPORT + 1, 777, 0, TCP_SYN);  // nobody listens there

	CHECK(find_conn(CPORT) == nullptr);
	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *tcp = captured_tcp(g_tx_frames[0]);
	CHECK(tcp->flags == (TCP_RST | TCP_ACK));
	CHECK(bswap32(tcp->ack_num) == 778);
	CHECK(bswap16(tcp->src_port) == SPORT + 1);
	CHECK(bswap16(tcp->dst_port) == CPORT);
}

TEST_CASE("TCP: non-SYN segment to unknown connection gets RST") {
	setup();
	g_tx_frames.clear();

	inject_tcp(CPORT, SPORT, 500, 12345, TCP_ACK);  // no such connection

	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *tcp = captured_tcp(g_tx_frames[0]);
	CHECK(tcp->flags == TCP_RST);
	CHECK(bswap32(tcp->seq_num) == 12345);  // seq = incoming ack

	// ...but an RST to an unknown connection must NOT be answered
	g_tx_frames.clear();
	inject_tcp(CPORT, SPORT, 500, 12345, TCP_RST);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("TCP send: SendUser emits PSH|ACK segments through the TX queue") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	const uint8_t data[] = { 'p', 'i', 'n', 'g' };
	CHECK(NET_TCP_SendUser(conn, data, sizeof(data)) == sizeof(data));

	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	const tcp_hdr_t *tcp = captured_tcp(f);
	CHECK(tcp->flags == (TCP_ACK | TCP_PSH));
	CHECK(bswap32(tcp->seq_num) == TEST_ISN + 1);
	CHECK(memcmp(captured_tcp_payload(f), data, sizeof(data)) == 0);
	CHECK(conn->tcp_my_seq == TEST_ISN + 1 + sizeof(data));

	// Invalid arguments are rejected
	CHECK(NET_TCP_SendUser(conn, nullptr, 1) == -1);
	CHECK(NET_TCP_SendUser(conn, data, 0) == -1);
}

TEST_CASE("TCP send: payload larger than MSS is split into segments") {
	setup();
	// Handshake advertising a peer MSS of 1000 and a large window
	const uint8_t mss_opt[4] = { 2, 4, 0x03, 0xE8 };  // MSS 1000
	inject_tcp(CPORT, SPORT, CSEQ, 0, TCP_SYN, nullptr, 0, 65535,
			mss_opt, sizeof(mss_opt));
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1, TCP_ACK, nullptr, 0, 65535);
	tcp_conn_t *conn = find_conn(CPORT);
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	static uint8_t big[2500];
	for (size_t i = 0; i < sizeof(big); i++) big[i] = static_cast<uint8_t>(i);
	CHECK(NET_TCP_SendUser(conn, big, sizeof(big)) == sizeof(big));

	REQUIRE(g_tx_frames.size() == 3);               // 1000 + 1000 + 500
	CHECK(bswap32(captured_tcp(g_tx_frames[0])->seq_num) == TEST_ISN + 1);
	CHECK(bswap32(captured_tcp(g_tx_frames[1])->seq_num) == TEST_ISN + 1001);
	CHECK(bswap32(captured_tcp(g_tx_frames[2])->seq_num) == TEST_ISN + 2001);
	// Byte-exact continuation across segment boundaries
	CHECK(captured_tcp_payload(g_tx_frames[1])[0] == big[1000]);
	CHECK(captured_tcp_payload(g_tx_frames[2])[0] == big[2000]);
	CHECK(conn->tcp_my_seq == TEST_ISN + 1 + sizeof(big));
}

TEST_CASE("TCP send: peer window limits how much goes in flight") {
	setup();
	tcp_conn_t *conn = do_handshake();  // client window 1024, peer MSS 536
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	// 2000 bytes: 536 fits, second chunk would exceed the 1024 window
	static uint8_t big[2000] = {};
	CHECK(NET_TCP_SendUser(conn, big, sizeof(big)) == 536);
	CHECK(g_tx_frames.size() == 1);

	// ACK opens the window again -> next chunk goes out
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1 + 536, TCP_ACK);
	CHECK(NET_TCP_SendUser(conn, big, 536) == 536);
}

TEST_CASE("TCP send: TX queue slots cap the number of in-flight segments") {
	setup();
	// Small MSS so slots run out before the window does
	const uint8_t mss_opt[4] = { 2, 4, 0x00, 0x64 };  // MSS 100
	inject_tcp(CPORT, SPORT, CSEQ, 0, TCP_SYN, nullptr, 0, 65535,
			mss_opt, sizeof(mss_opt));
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1, TCP_ACK, nullptr, 0, 65535);
	tcp_conn_t *conn = find_conn(CPORT);
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	static uint8_t data[400] = {};
	// Only TCP_TX_QUEUE_SLOTS segments fit in flight
	CHECK(NET_TCP_SendUser(conn, data, sizeof(data)) == TCP_TX_QUEUE_SLOTS * 100);
	CHECK(g_tx_frames.size() == TCP_TX_QUEUE_SLOTS);
	CHECK(NET_TCP_SendUser(conn, data, 100) == 0);   // no free slot

	// ACK of the first segment frees a slot
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1 + 100, TCP_ACK, nullptr, 0, 65535);
	CHECK(NET_TCP_SendUser(conn, data, 100) == 100);
}

TEST_CASE("TCP retransmit: oldest segment resent, then RST after max retries") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	const uint8_t data[] = { 'd', 'a', 't', 'a' };
	REQUIRE(NET_TCP_SendUser(conn, data, sizeof(data)) == sizeof(data));
	g_tx_frames.clear();

	// Not yet expired
	g_test_now += TCP_RETRANSMIT_TIMEOUT - 100;
	NET_TCP_Timers();
	CHECK(g_tx_frames.empty());

	// Expired -> retransmission rebuilt from the slot
	g_test_now += 200;
	NET_TCP_Timers();
	REQUIRE(g_tx_frames.size() == 1);
	const auto &rt = g_tx_frames[0];
	CHECK(captured_tcp(rt)->flags == (TCP_ACK | TCP_PSH));
	CHECK(bswap32(captured_tcp(rt)->seq_num) == TEST_ISN + 1);
	CHECK(memcmp(captured_tcp_payload(rt), data, sizeof(data)) == 0);

	// Exhaust remaining attempts, then expect RST + connection teardown
	for (int i = 0; i < TCP_RETRANSMISSION_MAX_COUNT; i++) {
		g_test_now += TCP_RETRANSMIT_TIMEOUT + 1;
		NET_TCP_Timers();
	}
	CHECK(find_conn(CPORT) == nullptr);
	REQUIRE(!g_tx_frames.empty());
	CHECK(captured_tcp(g_tx_frames.back())->flags == TCP_RST);
}

TEST_CASE("TCP retransmit: duplicate SYN triggers SYN|ACK retransmission") {
	setup();
	inject_tcp(CPORT, SPORT, CSEQ, 0, TCP_SYN);
	REQUIRE(find_conn(CPORT) != nullptr);
	g_tx_frames.clear();

	// Our SYN|ACK was lost - the client repeats its SYN
	inject_tcp(CPORT, SPORT, CSEQ, 0, TCP_SYN);
	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *tcp = captured_tcp(g_tx_frames[0]);
	CHECK(tcp->flags == (TCP_SYN | TCP_ACK));
	CHECK(bswap32(tcp->seq_num) == TEST_ISN);       // same ISN as before
	// MSS option is preserved on retransmission
	CHECK((tcp->offset_reserved >> 4) == 6);
}

TEST_CASE("TCP keepalive: idle established connection gets probed") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	g_test_now += TCP_KEEPALIVE_TIMEOUT + 1;
	NET_TCP_Timers();

	REQUIRE(g_tx_frames.size() == 1);
	const tcp_hdr_t *tcp = captured_tcp(g_tx_frames[0]);
	CHECK(tcp->flags == TCP_ACK);
	// Keepalive probe uses seq = snd_next - 1 and acks what we expect next
	CHECK(bswap32(tcp->seq_num) == conn->tcp_my_seq - 1);
	CHECK(bswap32(tcp->ack_num) == conn->rcv_next);
	CHECK(conn->keep_alive_count == 1);
}

TEST_CASE("TCP: connection table exhaustion answers SYN with RST|ACK") {
	setup();
	for (uint16_t i = 0; i < TCP_MAX_CONNECTIONS; i++) {
		inject_tcp(static_cast<uint16_t>(CPORT + i), SPORT, CSEQ, 0, TCP_SYN);
	}
	g_tx_frames.clear();

	inject_tcp(CPORT + 100, SPORT, CSEQ, 0, TCP_SYN);

	CHECK(find_conn(CPORT + 100) == nullptr);
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(captured_tcp(g_tx_frames[0])->flags == (TCP_RST | TCP_ACK));
}

TEST_CASE("TCP: FIN carrying data delivers the payload before closing") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);
	g_tx_frames.clear();

	// RFC 793: data on a FIN segment logically precedes the FIN
	const uint8_t payload[] = { 'b', 'y', 'e' };
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1,
			TCP_FIN | TCP_ACK | TCP_PSH, payload, sizeof(payload));

	// The payload must not be lost
	CHECK(NET_TCP_Available(conn) == sizeof(payload));
	uint8_t out[8] = {};
	CHECK(NET_TCP_Read(conn, out, sizeof(out)) == sizeof(payload));
	CHECK(memcmp(out, payload, sizeof(payload)) == 0);

	// FIN consumed one sequence number AFTER the data
	CHECK(conn->rcv_next == CSEQ + 1 + sizeof(payload) + 1);
	CHECK(conn->state == tcp_state_t::TCP_LAST_ACK);

	// ACK covering data+FIN, then our FIN|ACK
	REQUIRE(g_tx_frames.size() == 2);
	const tcp_hdr_t *ack = captured_tcp(g_tx_frames[0]);
	CHECK(ack->flags == TCP_ACK);
	CHECK(bswap32(ack->ack_num) == CSEQ + 1 + sizeof(payload) + 1);
	CHECK(captured_tcp(g_tx_frames[1])->flags == (TCP_FIN | TCP_ACK));
}

TEST_CASE("TCP: the ACK field of a FIN segment is processed (releases TX state)") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);

	// Put data in flight so there is something for the FIN's ACK to cover
	const uint8_t data[] = { 'p', 'i', 'n', 'g' };
	REQUIRE(NET_TCP_SendUser(conn, data, sizeof(data)) == sizeof(data));
	g_tx_frames.clear();

	// Peer acks our data and closes in the same segment
	inject_tcp(CPORT, SPORT, CSEQ + 1, TEST_ISN + 1 + sizeof(data),
			TCP_FIN | TCP_ACK);

	// snd_unack must have advanced over the acked data
	CHECK(conn->snd_unack == TEST_ISN + 1 + sizeof(data));
	CHECK(conn->state == tcp_state_t::TCP_LAST_ACK);
	REQUIRE(g_tx_frames.size() == 2);   // ACK of the FIN + our FIN|ACK
}

TEST_CASE("TCP: FIN whose data does not fit is not acknowledged") {
	setup();
	tcp_conn_t *conn = do_handshake();
	REQUIRE(conn != nullptr);

	// Fill the RX ring completely (4 x 1280 = SOCKET_RX_BUFF_LEN)
	static uint8_t chunk[1280] = {};
	uint32_t seq = CSEQ + 1;
	for (int i = 0; i < 4; i++) {
		inject_tcp(CPORT, SPORT, seq, TEST_ISN + 1, TCP_ACK,
				chunk, sizeof(chunk));
		seq += sizeof(chunk);
	}
	REQUIRE(conn->rx_count == SOCKET_RX_BUFF_LEN);
	g_tx_frames.clear();

	// FIN with one byte of payload: nothing fits, so neither the byte nor
	// the FIN may be consumed - the peer must retransmit both later
	const uint8_t last[] = { 'z' };
	inject_tcp(CPORT, SPORT, seq, TEST_ISN + 1, TCP_FIN | TCP_ACK,
			last, sizeof(last));

	CHECK(conn->state == tcp_state_t::TCP_ESTABLISHED);  // not closed
	CHECK(conn->rcv_next == seq);                        // nothing consumed
	CHECK(conn->rx_count == SOCKET_RX_BUFF_LEN);
}
