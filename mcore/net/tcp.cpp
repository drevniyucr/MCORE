/*
 * tcp.cpp — TCP: connection table, FSM (passive and active open), send and
 *           receive streams on the shared block pool, retransmission,
 *           zero-window probing, admission control
 *
 *  Created on: Sep 28, 2025
 *      Author: AkimovMA
 */
#include <cstddef>
#include <cstring>
#include "net/tcp.hpp"
#include "net/tcp_buf.hpp"
#include "net/net.hpp"
#include "core/system.hpp"
#include "net/eth_utils.hpp"
#include "drivers/eth.hpp"

// TCP frame template (declared extern in tcp.hpp). Lives here — a non-inline
// definition in a header (the old temp_frame.hpp) is an ODR trap. NET_SetIPAddr
// patches the baked-in source IP when DHCP changes the address.
uint8_t TCPsendFrameTemplate[TCP_TEMPLATE_FRAME_LEN] = {
		// === Ethernet Header (14B) ===
		0x00, 0x00, 0x00, 0x00, 0x00,
		0x00,        // Dst MAC (будет подставлен)
		MAC_ADDR[0], MAC_ADDR[1], MAC_ADDR[2], MAC_ADDR[3], MAC_ADDR[4],
		MAC_ADDR[5], // Src MAC = мой MAC
		0x08, 0x00,  // Ethertype = IPv4

		// === IPv4 Header (20B) ===
		0x45,// Version=4, IHL=5 (20 байт)
		0x00,        // TOS
		0x00, 0x00, // Total Length (20 IP + 20 TCP + payload, будет обновляться)
		0x00, 0x00,  // Identification
		0x00, 0x00,  // Flags + Fragment offset
		0x40,        // TTL
		0x06,        // Protocol = TCP (0x06)
		0x00, 0x00,  // Header checksum (обнулять перед расчетом)
		IP_ADDR[0], IP_ADDR[1], IP_ADDR[2], IP_ADDR[3],  // Src IP
		0x00, 0x00, 0x00, 0x00,  // Dst IP (будет подставлен)

		// === TCP Header (20B, без опций) ===
		0x13, 0x88,  // Src Port = 5000 (0x1388) (можно менять)
		0x00, 0x00,  // Dst Port (будет подставлен)
		0x00, 0x00, 0x00, 0x00,  // Seq Num (будет подставлен)
		0x00, 0x00, 0x00, 0x00,  // Ack Num (будет подставлен)
		0x50,        // Data Offset=5 (20 байт), Reserved=0
		0x02,        // Flags = SYN (0x02), можно менять на ACK/PSH/FIN
		0x04, 0x00,  // Window size = 1024 (0x0400)
		0x00, 0x00,  // TCP checksum (обнулять перед расчетом)
		0x00, 0x00   // Urgent Pointer = 0
		// Payload идёт сразу после TCP заголовка
		};

namespace {
	// Constants for TCP processing
	constexpr uint16_t TCP_MIN_HDR_LEN = 20;  // Minimum TCP header length
	constexpr uint32_t TCP_MAX_WINDOW = 65535U;  // Maximum TCP window size
	constexpr uint16_t TCP_MAX_SEG_DATA = ETH_MAX_PACKET_SIZE - ETH_HDR_LEN - IP_HDR_LEN - TCP_HDR_LEN;

	// Serial (mod 2^32) sequence-number comparison, robust across wrap
	inline bool seq_gt(uint32_t a, uint32_t b) {
		return static_cast<int32_t>(a - b) > 0;
	}
	inline bool seq_lte(uint32_t a, uint32_t b) {
		return static_cast<int32_t>(a - b) <= 0;
	}

	// Only the next expected segment is taken (no out-of-order reassembly yet)
	inline bool is_seq_acceptable(uint32_t seq, uint32_t rcv_next) {
		return (seq == rcv_next);
	}

	// ACK must cover something unacknowledged and nothing we never sent
	inline bool is_ack_acceptable(uint32_t ack, uint32_t snd_unack, uint32_t tcp_my_seq) {
		return seq_gt(ack, snd_unack) && seq_lte(ack, tcp_my_seq);
	}

	/**
	 * @brief Extract the MSS option (kind 2) from a SYN segment
	 *
	 * @return Peer's MSS, or TCP_DEFAULT_PEER_MSS if absent/malformed
	 */
	uint16_t parse_mss_option(const tcp_hdr_t *tcp) {
		const uint8_t hdr_len = (tcp->offset_reserved >> 4) * 4;
		if (hdr_len <= TCP_MIN_HDR_LEN) {
			return TCP_DEFAULT_PEER_MSS;
		}
		const uint8_t *opt = reinterpret_cast<const uint8_t*>(tcp) + TCP_MIN_HDR_LEN;
		const uint8_t *const end = reinterpret_cast<const uint8_t*>(tcp) + hdr_len;
		while (opt < end) {
			const uint8_t kind = opt[0];
			if (kind == 0) {
				break;  // end of option list
			}
			if (kind == 1) {
				opt++;  // NOP padding
				continue;
			}
			if (opt + 1 >= end || opt[1] < 2 || opt + opt[1] > end) {
				break;  // malformed option
			}
			if (kind == 2 && opt[1] == 4) {
				return static_cast<uint16_t>((opt[2] << 8) | opt[3]);
			}
			opt += opt[1];
		}
		return TCP_DEFAULT_PEER_MSS;
	}

	const uint8_t MSS_OPTION[4] = { 2, 4,
			static_cast<uint8_t>(TCP_OUR_MSS >> 8),
			static_cast<uint8_t>(TCP_OUR_MSS & 0xFF) };
}

tcp_conn_t tcp_clients[TCP_MAX_CONNECTIONS] = { };

namespace {
// Connection-table bookkeeping — internal to this TU (was external linkage)
uint8_t active_list[TCP_MAX_CONNECTIONS] = { };
uint8_t active_conn = 0;

uint8_t free_list[TCP_MAX_CONNECTIONS];
uint8_t free_top = 0;

uint8_t **ppTxBuff = nullptr;

uint16_t listen_ports[TCP_MAX_LISTEN_PORTS] = { };

uint16_t next_ephemeral = 0;   // 0 = not seeded yet

// Pool blocks a connection must leave free: incoming ones keep the reserve
// for the connections we open ourselves
uint16_t keep_free(const tcp_conn_t *c) {
	return c->active_open ? 0 : TCP_POOL_RESERVE_BLOCKS;
}

// What we can promise to receive right now (the advertised window)
uint16_t rx_window(const tcp_conn_t *c) {
	const uint32_t room = tcpbuf::room(c->rx, TCP_CONN_RX_BLOCKS, keep_free(c));
	return static_cast<uint16_t>(room > TCP_MAX_WINDOW ? TCP_MAX_WINDOW : room);
}

// Largest payload per segment towards this peer
uint16_t send_mss(const tcp_conn_t *c) {
	uint16_t mss = (c->peer_mss != 0) ? c->peer_mss : TCP_DEFAULT_PEER_MSS;
	if (mss > TCP_OUR_MSS) mss = TCP_OUR_MSS;
	return mss;
}
} // namespace

void NET_TCP_Init() {
	memset(tcp_clients, 0, sizeof(tcp_clients));
	memset(listen_ports, 0, sizeof(listen_ports));
	tcpbuf::init();
	free_top = 0;
	active_conn = 0;
	ppTxBuff = &TxDescList.pBuff;
	for (uint8_t i = 0; i < TCP_MAX_CONNECTIONS; i++) {
		free_list[free_top++] = i;
	}
}

bool NET_TCP_Listen(uint16_t port) {
	if (port == 0) {
		return false;
	}
	for (uint16_t p : listen_ports) {
		if (p == port) {
			return true;  // already listening
		}
	}
	for (uint16_t &p : listen_ports) {
		if (p == 0) {
			p = port;
			return true;
		}
	}
	return false;  // no free listen slots
}

void NET_TCP_StopListen(uint16_t port) {
	for (uint16_t &p : listen_ports) {
		if (p == port) {
			p = 0;
		}
	}
}

static bool NET_TCP_IsListening(uint16_t port) {
	for (uint16_t p : listen_ports) {
		if (p != 0 && p == port) {
			return true;
		}
	}
	return false;
}

// A free control block. Incoming connections (active == false) cannot take
// the last TCP_TCB_RESERVE: those are kept for the ones we open ourselves.
static int tcb_alloc(bool active) {
	if (free_top == 0 || (!active && free_top <= TCP_TCB_RESERVE)) {
		return -1;
	}
	const uint8_t i = free_list[--free_top];
	active_list[active_conn++] = i;
	memset(&tcp_clients[i], 0, sizeof(tcp_conn_t));
	tcp_clients[i].socket_tag = i;
	tcp_clients[i].active_open = active;
	return i;
}

int NET_TCP_ClientAdd() {
	return tcb_alloc(false);
}

void NET_TCP_ClientRemove(uint8_t idx) {
	for (int j = 0; j < active_conn; j++) {
		if (active_list[j] == idx) {
			if (j != active_conn - 1) {
				active_list[j] = active_list[--active_conn];
			} else {
				active_conn--;
			}
			tcpbuf::clear(tcp_clients[idx].rx);    // blocks back to the pool
			tcpbuf::clear(tcp_clients[idx].tx);
			memset(&tcp_clients[idx], 0, sizeof(tcp_conn_t));
			free_list[free_top++] = idx;
			break;
		}
	}
}

/**
 * @brief Find TCP connection by IP and port
 */
static tcp_conn_t* NET_TCP_FindConnection(uint32_t client_ip, uint16_t client_port, uint16_t server_port) {
	for (int i = 0; i < active_conn; i++) {
		tcp_conn_t* conn = &tcp_clients[active_list[i]];
		if (conn->client_ip == client_ip &&
		    conn->client_port == client_port &&
		    conn->server_port == server_port) {
			return conn;
		}
	}
	return nullptr;
}

// SYN flood protection: with TCP_MAX_HALF_OPEN connections in SYN_RCVD,
// the oldest one makes room (silently: it is likely a spoofed source)
static void evict_half_open_if_full() {
	uint8_t half_open = 0;
	tcp_conn_t *oldest = nullptr;
	for (int i = 0; i < active_conn; i++) {
		tcp_conn_t *c = &tcp_clients[active_list[i]];
		if (c->state == tcp_state_t::TCP_SYN_RCVD) {
			half_open++;
			if (oldest == nullptr || seq_gt(oldest->last_activity, c->last_activity)) {
				oldest = c;
			}
		}
	}
	if (half_open >= TCP_MAX_HALF_OPEN && oldest != nullptr) {
		NET_TCP_ClientRemove(oldest->socket_tag);
	}
}

/**
 * @brief Send TCP RST frame without creating connection
 */
void NET_SendTCP_RST(ipv4_frame *frame, uint16_t src_port, uint16_t dst_port,
		uint32_t seq, uint32_t ack, uint8_t flags) {
	if (!ETH_IsTxBufferAvailable()) {
		return;  // TX queue full, drop RST
	}

	uint8_t* tx_buf = *ppTxBuff;
	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(tx_buf);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>(tx_buf + ETH_HDR_LEN);
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>(tx_buf + ETH_HDR_LEN + IP_HDR_LEN);

	memcpy(eth, TCPsendFrameTemplate, TCP_TEMPLATE_FRAME_LEN);
	memcpy(eth->dst, frame->scr_mac, MAC_ADDR_LEN);
	ip->tot_len = bswap16(IP_HDR_LEN + TCP_HDR_LEN);
	ip->dst_ip = bswap32(BuffU8ToU32(frame->scr_addr));
	tcp->src_port = bswap16(src_port);
	tcp->dst_port = bswap16(dst_port);
	tcp->seq_num = bswap32(seq);
	tcp->ack_num = bswap32(ack);
	tcp->flags = flags;
	tcp->offset_reserved = 0x50;  // Data offset = 5 (20 bytes), reserved = 0
	tcp->window = 0;
	tcp->urgent_ptr = 0;
	// Checksum is inserted by the MAC

	ETH_SendFrame(ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN);
}

// ==========================
// Frame building
// ==========================

// Headers for a segment of conn into the driver's TX buffer; returns where
// the payload goes. Advertises the current receive window.
static uint8_t* tcp_frame(tcp_conn_t *conn, uint32_t seq, uint32_t ack, uint8_t flags,
		const uint8_t *opts, uint8_t opts_len, uint16_t data_len) {
	uint8_t* tx_buf = *ppTxBuff;
	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(tx_buf);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>(tx_buf + ETH_HDR_LEN);
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>(tx_buf + ETH_HDR_LEN + IP_HDR_LEN);
	const uint16_t tcp_hdr_total = TCP_HDR_LEN + opts_len;

	conn->window_size = rx_window(conn);

	memcpy(eth, TCPsendFrameTemplate, TCP_TEMPLATE_FRAME_LEN);
	memcpy(eth->dst, conn->client_mac, MAC_ADDR_LEN);
	ip->tot_len = bswap16(IP_HDR_LEN + tcp_hdr_total + data_len);
	ip->dst_ip = bswap32(conn->client_ip);
	tcp->src_port = bswap16(conn->server_port);
	tcp->dst_port = bswap16(conn->client_port);
	tcp->seq_num = bswap32(seq);
	tcp->ack_num = bswap32(ack);
	tcp->window = bswap16(conn->window_size);
	tcp->flags = flags;
	tcp->offset_reserved = static_cast<uint8_t>((5 + opts_len / 4) << 4);
	tcp->urgent_ptr = 0;
	if (opts_len > 0 && opts != nullptr) {
		memcpy(reinterpret_cast<uint8_t*>(tcp) + TCP_HDR_LEN, opts, opts_len);
	}
	return reinterpret_cast<uint8_t*>(tcp) + tcp_hdr_total;
}

/**
 * @brief Send one TCP segment with an explicit payload
 */
void NET_SendTCP(tcp_conn_t *conn, uint32_t seq, uint32_t ack, uint8_t flags,
		const uint8_t *data, uint16_t data_len,
		const uint8_t *opts, uint8_t opts_len) {
	// Options must keep the header 32-bit aligned
	if ((opts_len & 0x3) != 0 || opts_len > 40 ||
	    data_len > TCP_MAX_SEG_DATA - opts_len) {
		return;
	}
	uint8_t *payload = tcp_frame(conn, seq, ack, flags, opts, opts_len, data_len);
	if (data_len > 0 && data != nullptr) {
		memcpy(payload, data, data_len);
	}
	ETH_SendFrame(ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN + opts_len + data_len);
}

// A data segment straight from the TX FIFO: bytes at `offset` past snd_unack
static void send_from_tx(tcp_conn_t *conn, uint32_t seq, uint32_t offset, uint16_t len, uint8_t flags) {
	uint8_t *payload = tcp_frame(conn, seq, conn->rcv_next, flags, nullptr, 0, len);
	tcpbuf::peek(conn->tx, offset, payload, len);
	ETH_SendFrame(ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN + len);
}

// SYN (active open) or SYN|ACK (passive) with our MSS, at the ISN
static void send_syn(tcp_conn_t *conn) {
	const bool passive = conn->state == tcp_state_t::TCP_SYN_RCVD;
	NET_SendTCP(conn, conn->snd_unack, passive ? conn->rcv_next : 0,
	            passive ? (TCP_SYN | TCP_ACK) : TCP_SYN,
	            nullptr, 0, MSS_OPTION, sizeof(MSS_OPTION));
}

// ==========================
// Send side
// ==========================

// Send what the peer's window allows, then our FIN once every byte is out.
static void tcp_output(tcp_conn_t *conn) {
	const bool draining = (conn->state == tcp_state_t::TCP_FIN_WAIT_1 ||
	                       conn->state == tcp_state_t::TCP_LAST_ACK) && !conn->fin_sent;
	if (conn->state != tcp_state_t::TCP_ESTABLISHED && !draining) {
		return;
	}
	const uint16_t mss = send_mss(conn);
	while (true) {
		const uint32_t in_flight = conn->tcp_my_seq - conn->snd_unack;
		if (in_flight >= conn->tx.count || in_flight >= conn->client_window) {
			break;                                  // all sent, or window full
		}
		uint32_t len = conn->tx.count - in_flight;
		if (len > mss) len = mss;
		if (len > conn->client_window - in_flight) len = conn->client_window - in_flight;
		if (!ETH_IsTxBufferAvailable()) {
			break;                                  // driver busy: NET_TCP_Timers retries
		}
		if (in_flight == 0) {
			conn->rtx_at = get_tick();             // first byte in flight starts the clock
			conn->rtx_retries = 0;
		}
		send_from_tx(conn, conn->tcp_my_seq, in_flight, static_cast<uint16_t>(len), TCP_ACK | TCP_PSH);
		conn->tcp_my_seq += len;
	}
	if (conn->fin_pending && !conn->fin_sent &&
	    conn->tcp_my_seq - conn->snd_unack == conn->tx.count && ETH_IsTxBufferAvailable()) {
		if (conn->tcp_my_seq == conn->snd_unack) {
			conn->rtx_at = get_tick();
			conn->rtx_retries = 0;
		}
		NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_FIN | TCP_ACK, nullptr, 0);
		conn->tcp_my_seq += 1;                     // FIN consumes one number
		conn->fin_sent = true;
	}
}

// Cumulative ACK: drop acknowledged bytes, restart the retransmission clock
static void tcp_ack_received(tcp_conn_t *conn, uint32_t ack) {
	if (!is_ack_acceptable(ack, conn->snd_unack, conn->tcp_my_seq)) {
		return;                                     // old duplicate or beyond snd_nxt
	}
	const uint32_t acked = ack - conn->snd_unack;
	// SYN and FIN take a sequence number but no FIFO byte
	tcpbuf::pop(conn->tx, nullptr, acked < conn->tx.count ? acked : conn->tx.count);
	conn->snd_unack = ack;
	conn->rtx_at = get_tick();
	conn->rtx_retries = 0;
}

// Resend the oldest unacknowledged thing: SYN, SYN|ACK, data or FIN
static void retransmit_oldest(tcp_conn_t *conn) {
	if (!ETH_IsTxBufferAvailable()) {
		return;                                     // the timer will retry
	}
	if (conn->state == tcp_state_t::TCP_SYN_SENT || conn->state == tcp_state_t::TCP_SYN_RCVD) {
		send_syn(conn);
		return;
	}
	uint32_t data = conn->tcp_my_seq - conn->snd_unack;
	if (data > conn->tx.count) data = conn->tx.count;   // minus a FIN in flight
	if (data > 0) {
		const uint16_t mss = send_mss(conn);
		send_from_tx(conn, conn->snd_unack, 0, static_cast<uint16_t>(data > mss ? mss : data),
		             TCP_ACK | TCP_PSH);
	} else if (conn->fin_sent) {
		NET_SendTCP(conn, conn->tcp_my_seq - 1, conn->rcv_next, TCP_FIN | TCP_ACK, nullptr, 0);
	}
}

// ==========================
// Active open
// ==========================

static bool local_port_in_use(uint16_t port) {
	for (int i = 0; i < active_conn; i++) {
		if (tcp_clients[active_list[i]].server_port == port) {
			return true;
		}
	}
	return NET_TCP_IsListening(port);
}

// Next free port of the ephemeral range, starting at a random offset so a
// reboot does not reuse the previous run's ports (stale peer state).
static uint16_t alloc_ephemeral_port() {
	constexpr uint16_t RANGE = 0xFFFF - TCP_EPHEMERAL_BASE + 1;
	if (next_ephemeral == 0) {
		next_ephemeral = static_cast<uint16_t>(TCP_EPHEMERAL_BASE + NET_Entropy() % RANGE);
	}
	for (uint16_t tries = 0; tries < RANGE; tries++) {
		const uint16_t port = next_ephemeral;
		next_ephemeral = (next_ephemeral == 0xFFFF) ? TCP_EPHEMERAL_BASE
		                                            : static_cast<uint16_t>(next_ephemeral + 1);
		if (!local_port_in_use(port)) {
			return port;
		}
	}
	return 0;
}

net_result_t NET_TCP_Connect(const uint8_t *ip, uint16_t port, tcp_conn_t **out) {
	*out = nullptr;
	if (port == 0) {
		return net_result_t::UNREACHABLE;
	}
	uint8_t mac[MAC_ADDR_LEN];
	const net_result_t arp = NET_ARP_Resolve(ip, mac);
	if (arp != net_result_t::OK) {
		return arp;                                // PENDING: ask again later
	}
	if (!ETH_IsTxBufferAvailable()) {
		return net_result_t::PENDING;              // TX path busy right now
	}
	const uint16_t local_port = alloc_ephemeral_port();
	const int num = (local_port != 0) ? tcb_alloc(true) : -1;
	if (num < 0) {
		return net_result_t::UNREACHABLE;          // no free connection slot
	}

	tcp_conn_t *conn = &tcp_clients[num];
	conn->client_ip = BuffU8ToU32(ip);             // same byte order as the RX path
	memcpy(conn->client_mac, mac, MAC_ADDR_LEN);
	conn->client_port = port;
	conn->server_port = local_port;
	conn->peer_mss = TCP_DEFAULT_PEER_MSS;         // until the SYN|ACK says otherwise
	conn->snd_unack = NET_Entropy();               // ISN
	conn->last_activity = get_tick();
	conn->state = tcp_state_t::TCP_SYN_SENT;

	// SYN with our MSS; the timers retransmit it and give up (RST + close)
	// after TCP_RETRANSMISSION_MAX_COUNT attempts
	send_syn(conn);
	conn->tcp_my_seq = conn->snd_unack + 1;
	conn->rtx_at = get_tick();
	*out = conn;
	return net_result_t::OK;
}

bool NET_TCP_IsOpen(const tcp_conn_t *conn, uint16_t local_port) {
	return conn != nullptr && conn->state != tcp_state_t::TCP_CLOSED &&
	       conn->server_port == local_port;
}

// Active open, RFC 793 "SYN-SENT STATE" (simultaneous open not supported)
static void tcp_syn_sent(tcp_conn_t *conn, ipv4_frame *frame, const tcp_hdr_t *tcp) {
	const uint8_t flags = tcp->flags;
	const uint32_t seq = bswap32(tcp->seq_num);
	const uint32_t ack = bswap32(tcp->ack_num);
	const bool ack_ok = (flags & TCP_ACK) && ack == conn->tcp_my_seq;

	if (flags & TCP_RST) {
		if (ack_ok) {
			NET_TCP_ClientRemove(conn->socket_tag);    // connection refused
		}
		return;
	}
	if ((flags & TCP_ACK) && !ack_ok) {
		// ACK of something we never sent (old duplicate): reset it
		NET_SendTCP_RST(frame, conn->server_port, conn->client_port, ack, 0, TCP_RST);
		return;
	}
	if ((flags & TCP_SYN) && ack_ok) {
		conn->tcp_client_seq = seq;
		conn->rcv_next = seq + 1;                  // their SYN consumes one number
		conn->client_window = bswap16(tcp->window);
		conn->peer_mss = parse_mss_option(tcp);
		tcp_ack_received(conn, ack);
		conn->last_activity = get_tick();
		conn->keep_alive_count = 0;
		conn->state = tcp_state_t::TCP_ESTABLISHED;
		NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK, nullptr, 0);
	}
}

// ==========================
// Timers
// ==========================

void NET_TCP_Timers() {
	const uint32_t now = get_tick();

	// From the end: removal swaps the last entry into the hole
	for (int i = active_conn - 1; i >= 0; i--) {
		const uint8_t idx = active_list[i];
		tcp_conn_t *conn = &tcp_clients[idx];

		// TIME_WAIT: release the slot after 2*MSL, nothing else to do
		if (conn->state == tcp_state_t::TCP_TIME_WAIT) {
			if ((now - conn->last_activity) > TCP_TIME_WAIT_TIMEOUT) {
				NET_TCP_ClientRemove(idx);
			}
			continue;
		}

		// Retransmission of the oldest unacknowledged SYN / data / FIN
		if (conn->snd_unack != conn->tcp_my_seq && (now - conn->rtx_at) > TCP_RETRANSMIT_TIMEOUT) {
			if (conn->rtx_retries >= TCP_RETRANSMISSION_MAX_COUNT) {
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_RST, nullptr, 0);
				NET_TCP_ClientRemove(idx);
				continue;
			}
			retransmit_oldest(conn);
			conn->rtx_at = now;
			conn->rtx_retries++;
		}

		// Data that waited for the driver or the window
		tcp_output(conn);

		// Zero-window probing (RFC 1122 4.2.2.17): the peer closed its window
		// and we have data; without probes a lost window update stalls forever
		const uint32_t in_flight = conn->tcp_my_seq - conn->snd_unack;
		if (conn->state == tcp_state_t::TCP_ESTABLISHED && conn->client_window == 0 &&
		    in_flight == 0 && conn->tx.count > 0) {
			if (conn->persist_ms == 0) {
				conn->persist_ms = TCP_PERSIST_MIN_MS;
				conn->persist_at = now + TCP_PERSIST_MIN_MS;
			} else if (static_cast<int32_t>(now - conn->persist_at) >= 0 && ETH_IsTxBufferAvailable()) {
				send_from_tx(conn, conn->tcp_my_seq, 0, 1, TCP_ACK);   // 1 byte beyond the window
				conn->persist_ms = (conn->persist_ms * 2 > TCP_PERSIST_MAX_MS) ? TCP_PERSIST_MAX_MS
				                                                             : conn->persist_ms * 2;
				conn->persist_at = now + conn->persist_ms;
			}
		} else {
			conn->persist_ms = 0;
		}

		// Keepalive: probe an idle established connection, drop a dead one
		if (((now - conn->last_activity) > TCP_KEEPALIVE_TIMEOUT) &&
		    (conn->state == tcp_state_t::TCP_ESTABLISHED) && in_flight == 0) {
			if ((now - conn->last_keepalive) >= TCP_KEEPALIVE_INTERVAL) {
				if (conn->keep_alive_count >= TCP_KEEPALIVE_MAX_COUNT) {
					NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_RST, nullptr, 0);
					NET_TCP_ClientRemove(idx);
					continue;
				}
				// Send keepalive (ACK with seq-1)
				NET_SendTCP(conn, conn->tcp_my_seq - 1, conn->rcv_next, TCP_ACK, nullptr, 0);
				conn->keep_alive_count++;
				conn->last_keepalive = now;
			}
		}
	}
}

// ==========================
// Receive side
// ==========================

void NET_ProcessTCP(ipv4_frame *frame) {
	const tcp_hdr_t *tcp = reinterpret_cast<const tcp_hdr_t*>(&frame->ip_hdr[frame->ip_hdr_len]);

	// The header (with options) must fit into the IP payload: data length,
	// payload pointer and the MSS option parser all trust the data offset
	if (frame->ip_len < frame->ip_hdr_len + TCP_MIN_HDR_LEN) {
		return;
	}
	const uint16_t seg_len = static_cast<uint16_t>(frame->ip_len - frame->ip_hdr_len);
	const uint16_t tcp_hdr_len = static_cast<uint16_t>((tcp->offset_reserved >> 4) * 4);
	if (tcp_hdr_len < TCP_MIN_HDR_LEN || tcp_hdr_len > seg_len) {
		return;  // malformed: drop silently, no RST to a forged segment
	}

	const uint8_t client_flags = tcp->flags;
	const uint16_t client_window = bswap16(tcp->window);
	const uint16_t client_port = bswap16(tcp->src_port);
	const uint16_t server_port = bswap16(tcp->dst_port);
	const uint32_t client_ip = BuffU8ToU32(frame->scr_addr);
	const uint32_t client_seq = bswap32(tcp->seq_num);
	const uint32_t client_ack = bswap32(tcp->ack_num);

	tcp_conn_t *conn = NET_TCP_FindConnection(client_ip, client_port, server_port);

	// Our active open has its own rules (RST and ACK checks differ)
	if (conn && conn->state == tcp_state_t::TCP_SYN_SENT) {
		tcp_syn_sent(conn, frame, tcp);
		return;
	}

	// Handle RST flag: only accept an in-sequence RST (anti-spoofing, RFC 793)
	if (conn && (client_flags & TCP_RST)) {
		if (client_seq == conn->rcv_next) {
			NET_TCP_ClientRemove(conn->socket_tag);
		}
		return;
	}

	// New connection (SYN)
	if (!conn && (client_flags & TCP_SYN)) {
		// Only listening ports accept connections
		if (!NET_TCP_IsListening(server_port)) {
			NET_SendTCP_RST(frame, server_port, client_port, 0, client_seq + 1,
							TCP_RST | TCP_ACK);
			return;
		}
		evict_half_open_if_full();
		const int num = tcb_alloc(false);
		if (num == -1) {
			// No control block left for incoming connections
			NET_SendTCP_RST(frame, server_port, client_port, 0, client_seq + 1,
							TCP_RST | TCP_ACK);
			return;
		}

		conn = &tcp_clients[num];
		conn->client_ip = client_ip;
		conn->last_activity = get_tick();
		memcpy(conn->client_mac, frame->scr_mac, MAC_ADDR_LEN);
		conn->client_port = client_port;
		conn->server_port = server_port;
		conn->tcp_client_seq = client_seq;
		conn->rcv_next = client_seq + 1;  // SYN consumes 1 sequence number
		conn->client_window = client_window;
		conn->peer_mss = parse_mss_option(tcp);
		conn->snd_unack = NET_Entropy();  // ISN
		conn->state = tcp_state_t::TCP_SYN_RCVD;

		// SYN|ACK with our MSS (retransmitted by the timers; consumes one number)
		send_syn(conn);
		conn->tcp_my_seq = conn->snd_unack + 1;
		conn->rtx_at = get_tick();
		return;
	}

	// Segment for a non-existent connection: reset the sender (RFC 793).
	// Never answer an RST with an RST.
	if (!conn) {
		if (!(client_flags & TCP_RST)) {
			NET_SendTCP_RST(frame, server_port, client_port, client_ack, 0, TCP_RST);
		}
		return;
	}

	conn->client_window = client_window;

	switch (conn->state) {
	case tcp_state_t::TCP_SYN_RCVD:
		if (client_flags & TCP_ACK) {
			if ((client_ack == conn->tcp_my_seq) && (client_seq == conn->rcv_next)) {
				// ACK for SYN+ACK received - connection established
				conn->last_activity = get_tick();
				conn->keep_alive_count = 0;
				tcp_ack_received(conn, client_ack);
				conn->state = tcp_state_t::TCP_ESTABLISHED;
			}
		}
		// Retransmit SYN+ACK if SYN received again
		if ((client_flags & TCP_SYN) && (client_seq + 1 == conn->rcv_next)) {
			retransmit_oldest(conn);
		}
		break;

	case tcp_state_t::TCP_ESTABLISHED: {
		conn->last_activity = get_tick();
		conn->keep_alive_count = 0;

		// In-order segments only for now: anything else gets a duplicate
		// ACK with rcv_next, which helps the sender recover from a loss
		if (!is_seq_acceptable(client_seq, conn->rcv_next)) {
			if (client_flags & TCP_ACK) {
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK, nullptr, 0);
			}
			break;
		}

		// 1) ACK bookkeeping first: a FIN segment normally carries an ACK too
		if (client_flags & TCP_ACK) {
			tcp_ack_received(conn, client_ack);
		}

		// 2) Payload into the RX stream, as far as our share of the pool goes.
		// Data on a FIN segment logically precedes the FIN (RFC 793).
		const uint16_t data_len = static_cast<uint16_t>(seg_len - tcp_hdr_len);
		uint32_t copy_len = 0;
		if (data_len > 0) {
			const uint8_t *payload = reinterpret_cast<const uint8_t*>(tcp) + tcp_hdr_len;
			copy_len = tcpbuf::push(conn->rx, payload, data_len, TCP_CONN_RX_BLOCKS, keep_free(conn));
			conn->rcv_next += copy_len;
			// A tail that did not fit is not acked: the peer retransmits it
		}

		// 3) FIN - only if every byte before it was taken. Our FIN follows
		// once all our queued data is out (tcp_output).
		if ((client_flags & TCP_FIN) && copy_len == data_len) {
			conn->rcv_next += 1;  // FIN consumes 1 sequence number
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK, nullptr, 0);
			conn->fin_pending = true;
			conn->state = tcp_state_t::TCP_LAST_ACK;
		} else if (data_len > 0) {
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK, nullptr, 0);
		}

		// 4) The ACK may have opened the peer's window or freed our FIFO
		tcp_output(conn);
		break;
	}

	case tcp_state_t::TCP_FIN_WAIT_1:
		if (client_flags & TCP_ACK) {
			conn->last_activity = get_tick();
			tcp_ack_received(conn, client_ack);
			tcp_output(conn);                       // still draining before our FIN
		}
		if (conn->fin_sent && conn->snd_unack == conn->tcp_my_seq) {
			// Our FIN is acknowledged
			if ((client_flags & TCP_FIN) && client_seq == conn->rcv_next) {
				// ... and theirs came with it: ACK it and wait out TIME_WAIT
				conn->rcv_next++;
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK, nullptr, 0);
				conn->last_activity = get_tick();
				conn->state = tcp_state_t::TCP_TIME_WAIT;
				return;
			}
			conn->state = tcp_state_t::TCP_FIN_WAIT_2;
		}
		break;

	case tcp_state_t::TCP_FIN_WAIT_2:
		if ((client_flags & TCP_FIN) && (client_seq == conn->rcv_next)) {
			conn->rcv_next += 1;
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK, nullptr, 0);
			conn->last_activity = get_tick();
			conn->state = tcp_state_t::TCP_TIME_WAIT;
			return;
		}
		break;

	case tcp_state_t::TCP_TIME_WAIT:
		// Peer retransmitted its FIN (our ACK was lost) - re-ACK it
		if (client_flags & TCP_FIN) {
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK, nullptr, 0);
		}
		break;

	case tcp_state_t::TCP_LAST_ACK:
		if (client_flags & TCP_ACK) {
			tcp_ack_received(conn, client_ack);
			tcp_output(conn);                       // remaining data, then our FIN
		}
		if (conn->fin_sent && conn->snd_unack == conn->tcp_my_seq && client_seq == conn->rcv_next) {
			NET_TCP_ClientRemove(conn->socket_tag);
			return;
		}
		break;

	default:
		break;
	}
}

// ==========================
// Application API
// ==========================

uint16_t NET_TCP_Available(tcp_conn_t *conn) {
	return (conn != nullptr) ? static_cast<uint16_t>(conn->rx.count) : 0;
}

int NET_TCP_Read(tcp_conn_t *conn, uint8_t *dst, uint16_t maxlen) {
	if (conn == nullptr || dst == nullptr) {
		return -1;
	}
	const uint32_t n = tcpbuf::pop(conn->rx, dst, maxlen);
	if (n == 0) {
		return 0;
	}

	// The peer saw a (nearly) closed window: announce that it reopened,
	// otherwise it may never learn it can transmit again
	const uint16_t old_window = conn->window_size;
	if (conn->state == tcp_state_t::TCP_ESTABLISHED &&
	    old_window < conn->peer_mss && rx_window(conn) >= conn->peer_mss) {
		NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK, nullptr, 0);
	}
	return static_cast<int>(n);
}

void NET_TCP_Close(tcp_conn_t *conn) {
	if (conn->state == tcp_state_t::TCP_ESTABLISHED) {
		conn->fin_pending = true;                  // FIN after the queued data
		conn->state = tcp_state_t::TCP_FIN_WAIT_1;
		tcp_output(conn);
	} else if (conn->state == tcp_state_t::TCP_SYN_SENT) {
		NET_TCP_Abort(conn);                       // nothing to close gracefully yet
	}
}

void NET_TCP_Abort(tcp_conn_t *conn) {
	if (conn == nullptr || conn->state == tcp_state_t::TCP_CLOSED) {
		return;
	}
	// SYN_SENT: the peer has not synchronised to our numbers, any RST will do
	NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_RST | TCP_ACK, nullptr, 0);
	NET_TCP_ClientRemove(conn->socket_tag);
}

/**
 * @brief Queue application data on the TX stream and send what the window allows
 *
 * @return Bytes taken (0 = TX share or pool full right now), -1 if not connected
 */
int NET_TCP_SendUser(tcp_conn_t *conn, const uint8_t *data, uint16_t len) {
	if (conn == nullptr || conn->state != tcp_state_t::TCP_ESTABLISHED) {
		return -1;  // Not connected
	}
	if (data == nullptr || len == 0) {
		return -1;  // Invalid parameters
	}
	const uint32_t taken = tcpbuf::push(conn->tx, data, len, TCP_CONN_TX_BLOCKS, keep_free(conn));
	tcp_output(conn);
	return static_cast<int>(taken);
}
