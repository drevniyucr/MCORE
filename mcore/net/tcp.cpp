/*
* mcore_tcp.cpp
*
*  Created on: Sep 28, 2025
*      Author: AkimovMA
*/
#include <cstddef>
#include <cstring>
#include "net/tcp.hpp"
#include "net/net.hpp"
#include "core/system.hpp"
#include "net/eth_utils.hpp"
#include "drivers/eth.hpp"
#include "net/temp_frame.hpp"

namespace {
	// Constants for TCP processing
	constexpr uint16_t TCP_MIN_HDR_LEN = 20;  // Minimum TCP header length
	constexpr uint16_t TCP_MAX_HDR_LEN = 60;  // Maximum TCP header length
	constexpr uint32_t TCP_MAX_WINDOW = 65535U;  // Maximum TCP window size

	// Serial (mod 2^32) sequence-number comparison, robust across wrap
	inline bool seq_gt(uint32_t a, uint32_t b) {
		return static_cast<int32_t>(a - b) > 0;
	}
	inline bool seq_lte(uint32_t a, uint32_t b) {
		return static_cast<int32_t>(a - b) <= 0;
	}

	/**
	 * @brief Check if sequence number is within acceptable receive window
	 * 
	 * @param seq Sequence number to check
	 * @param rcv_next Expected next sequence number
	 * @param window_size Receive window size
	 * @return true if sequence is acceptable, false otherwise
	 */
	inline bool is_seq_acceptable(uint32_t seq, uint32_t rcv_next) {
	//inline bool is_seq_acceptable(uint32_t seq, uint32_t rcv_next, uint16_t window_size) {
		// Accept sequence if it's the next expected or within window
		// For simplicity, only accept in-order packets (rcv_next)
		// TODO: Implement proper sliding window for out-of-order packets
		return (seq == rcv_next);
	}
	
	/**
	 * @brief Check if ACK number is within acceptable range
	 * 
	 * @param ack ACK number to check
	 * @param snd_unack Oldest unacknowledged sequence
	 * @param tcp_my_seq Next sequence to send
	 * @return true if ACK is acceptable, false otherwise
	 */
	inline bool is_ack_acceptable(uint32_t ack, uint32_t snd_unack, uint32_t tcp_my_seq) {
		// ACK must be greater than oldest unacknowledged and <= next to send
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
}

tcp_conn_t tcp_clients[TCP_MAX_CONNECTIONS] = { };
uint8_t active_list[TCP_MAX_CONNECTIONS] = { };
uint8_t active_conn = 0;

uint8_t free_list[TCP_MAX_CONNECTIONS];
uint8_t free_top = 0;

uint8_t **ppTxBuff = NULL;

uint16_t listen_ports[TCP_MAX_LISTEN_PORTS] = { };

void NET_TCP_Init() {
	memset(tcp_clients, 0, sizeof(tcp_clients));
	memset(listen_ports, 0, sizeof(listen_ports));
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

int NET_TCP_ClientAdd() {
	if (free_top != 0 && active_conn < TCP_MAX_CONNECTIONS) {
		uint8_t i = free_list[--free_top];
		active_list[active_conn++] = i; // добавляем в список активных
		print("Added TCP client %d\n", i);
		return i;
	}
	return -1;
}

void NET_TCP_ClientRemove(uint8_t idx) {
	// удаляем из active_list
	for (int j = 0; j < active_conn; j++) {
		if (active_list[j] == idx) {
			if (j != active_conn - 1) {
				active_list[j] = active_list[--active_conn];
			} else {
				active_conn--;
			}
			memset(&tcp_clients[idx], 0, sizeof(tcp_conn_t));
			free_list[free_top++] = idx;
			print("Removed TCP client %d\n", idx);
			break;
		}
	}
}

/**
 * @brief Find TCP connection by IP and port (optimized lookup)
 * 
 * @param client_ip Client IP address
 * @param client_port Client port
 * @param server_port Server port
 * @return Pointer to connection or NULL if not found
 */
static tcp_conn_t* NET_TCP_FindConnection(uint32_t client_ip, uint16_t client_port, uint16_t server_port) {
	// Linear search through active connections
	// TODO: Could be optimized with hash table for many connections
	for (int i = 0; i < active_conn; i++) {
		uint8_t idx = active_list[i];
		tcp_conn_t* conn = &tcp_clients[idx];
		
		if (conn->client_ip == client_ip &&
		    conn->client_port == client_port &&
		    conn->server_port == server_port) {
			return conn;
		}
	}
	return nullptr;
}

/**
 * @brief Send TCP RST frame without creating connection
 * 
 * Optimized frame construction with error handling
 */
void NET_SendTCP_RST(ipv4_frame *frame, uint16_t src_port, uint16_t dst_port,
		uint32_t seq, uint32_t ack, uint8_t flags) {
	
	// Check TX buffer availability before constructing frame
	if (!ETH_IsTxBufferAvailable()) {
		return;  // TX queue full, drop RST
	}

	uint8_t* tx_buf = *ppTxBuff;
	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(tx_buf);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>(tx_buf + ETH_HDR_LEN);
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>(tx_buf + ETH_HDR_LEN + IP_HDR_LEN);

	// Copy template (only Ethernet + IP headers, TCP will be filled)
	memcpy(eth, TCPsendFrameTemplate, TCP_TEMPLATE_FRAME_LEN);
	
	// Update Ethernet destination MAC
	memcpy(eth->dst, frame->scr_mac, MAC_ADDR_LEN);
	
	// Update IP header
	ip->tot_len = bswap16(IP_HDR_LEN + TCP_HDR_LEN);
	ip->dst_ip = bswap32(BuffU8ToU32(frame->scr_addr));
	
	// Update TCP header
	tcp->src_port = bswap16(src_port);
	tcp->dst_port = bswap16(dst_port);
	tcp->seq_num = bswap32(seq);
	tcp->ack_num = bswap32(ack);
	tcp->flags = flags;
	tcp->offset_reserved = 0x50;  // Data offset = 5 (20 bytes), reserved = 0
	tcp->window = 0;
	tcp->urgent_ptr = 0;
	// Checksum will be calculated by hardware if enabled

	// Send frame - check return value
	if (!ETH_SendFrame(ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN)) {
		// TX failed - frame dropped
	}
}

// ==========================
// TX sliding window helpers
// ==========================

// Payload storage of a slot inside the connection's TX arena
static uint8_t* tx_slot_payload(tcp_conn_t *conn, const tcp_tx_seg_t *seg) {
	const ptrdiff_t idx = seg - &conn->tx_queue[0];
	return &conn->socket_tx_buff[idx * TCP_TX_SLOT_LEN];
}

static tcp_tx_seg_t* tx_slot_alloc(tcp_conn_t *conn) {
	for (auto &seg : conn->tx_queue) {
		if (!seg.used) {
			return &seg;
		}
	}
	return nullptr;
}

// Oldest (lowest sequence) in-flight segment, nullptr if queue is empty
static tcp_tx_seg_t* tx_oldest_unacked(tcp_conn_t *conn) {
	tcp_tx_seg_t *oldest = nullptr;
	for (auto &seg : conn->tx_queue) {
		if (seg.used && (oldest == nullptr || seq_gt(oldest->seq, seg.seq))) {
			oldest = &seg;
		}
	}
	return oldest;
}

// End sequence of a segment (SYN/FIN consume one sequence number)
static uint32_t tx_seg_end(const tcp_tx_seg_t *seg) {
	return seg->seq + seg->len + (((seg->flags & (TCP_SYN | TCP_FIN)) != 0) ? 1 : 0);
}

// Release every slot fully covered by the cumulative ACK
static void tx_release_acked(tcp_conn_t *conn, uint32_t ack) {
	for (auto &seg : conn->tx_queue) {
		if (seg.used && seq_lte(tx_seg_end(&seg), ack)) {
			seg.used = false;
		}
	}
}

// Rebuild and resend one saved segment (header is reconstructed,
// payload comes from the slot arena)
static void tx_retransmit_slot(tcp_conn_t *conn, tcp_tx_seg_t *seg) {
	if (!ETH_IsTxBufferAvailable()) {
		return;  // TX queue full; the timer will retry
	}
	NET_SendTCP(conn, seg->seq, conn->rcv_next, seg->flags,
				(seg->len > 0) ? tx_slot_payload(conn, seg) : nullptr, seg->len,
				(seg->opts_len > 0) ? seg->opts : nullptr, seg->opts_len);
}

/**
 * @brief Send a segment and keep it in the TX queue until ACKed
 *
 * Advances tcp_my_seq by the consumed sequence space.
 * @return true if the segment was queued and handed to the driver
 */
static bool NET_TCP_EnqueueSegment(tcp_conn_t *conn, uint8_t flags,
		const uint8_t *data, uint16_t data_len,
		const uint8_t *opts = nullptr, uint8_t opts_len = 0) {
	if (data_len > TCP_TX_SLOT_LEN || opts_len > sizeof(tcp_tx_seg_t::opts)) {
		return false;
	}
	tcp_tx_seg_t *seg = tx_slot_alloc(conn);
	if (seg == nullptr || !ETH_IsTxBufferAvailable()) {
		return false;
	}

	seg->seq = conn->tcp_my_seq;
	seg->len = data_len;
	seg->flags = flags;
	seg->retries = 0;
	seg->sent_at = get_tick();
	seg->opts_len = opts_len;
	if (opts_len > 0) {
		memcpy(seg->opts, opts, opts_len);
	}
	if (data_len > 0) {
		memcpy(tx_slot_payload(conn, seg), data, data_len);
	}
	seg->used = true;

	NET_SendTCP(conn, seg->seq, conn->rcv_next, flags, data, data_len,
				opts, opts_len);
	conn->tcp_my_seq = tx_seg_end(seg);
	return true;
}

/**
 * @brief Send TCP frame on established socket
 * 
 * Optimized frame construction with proper error handling
 */
void NET_SendTCP(tcp_conn_t *conn, uint32_t seq, uint32_t ack, uint8_t flags,
		const uint8_t *data, uint16_t data_len,
		const uint8_t *opts, uint8_t opts_len) {

	// Options must keep the header 32-bit aligned
	if ((opts_len & 0x3) != 0 || opts_len > 40) {
		return;
	}

	// Validate data length
	if (data_len > (ETH_MAX_PACKET_SIZE - ETH_HDR_LEN - IP_HDR_LEN - TCP_HDR_LEN - opts_len)) {
		return;  // Frame too large
	}

	const uint16_t tcp_hdr_total = TCP_HDR_LEN + opts_len;

	uint8_t* tx_buf = *ppTxBuff;
	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(tx_buf);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>(tx_buf + ETH_HDR_LEN);
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>(tx_buf + ETH_HDR_LEN + IP_HDR_LEN);

	// Copy template
	memcpy(eth, TCPsendFrameTemplate, TCP_TEMPLATE_FRAME_LEN);

	// Update Ethernet destination MAC
	memcpy(eth->dst, conn->client_mac, MAC_ADDR_LEN);

	// Update IP header
	ip->tot_len = bswap16(IP_HDR_LEN + tcp_hdr_total + data_len);
	ip->dst_ip = bswap32(conn->client_ip);

	// Update TCP header
	tcp->src_port = bswap16(conn->server_port);
	tcp->dst_port = bswap16(conn->client_port);
	tcp->seq_num = bswap32(seq);
	tcp->ack_num = bswap32(ack);
	tcp->window = bswap16(conn->window_size);
	tcp->flags = flags;
	tcp->offset_reserved = static_cast<uint8_t>((5 + opts_len / 4) << 4);
	tcp->urgent_ptr = 0;

	// Copy options if present
	if (opts_len > 0 && opts != nullptr) {
		memcpy(reinterpret_cast<uint8_t*>(tcp) + TCP_HDR_LEN, opts, opts_len);
	}

	// Copy payload if present
	if (data_len > 0 && data != nullptr) {
		memcpy(reinterpret_cast<uint8_t*>(tcp) + tcp_hdr_total, data, data_len);
	}

	// Send frame (retransmission bookkeeping is the TX queue's job)
	if (!ETH_SendFrame(ETH_HDR_LEN + IP_HDR_LEN + tcp_hdr_total + data_len)) {
		// TX failed - frame dropped; queued segments will be retransmitted
	}
}

// ==========================
// Обработка TCP пакета
// ==========================

void NET_TCP_Timers() {
	uint8_t idx;
	tcp_conn_t *conn;
	
	// Process from end to beginning to allow safe removal
	for (int i = active_conn - 1; i >= 0; i--) {
		idx = active_list[i];
		conn = &tcp_clients[idx];

		// TIME_WAIT: release the slot after 2*MSL, nothing else to do
		if (conn->state == tcp_state_t::TCP_TIME_WAIT) {
			if ((get_tick() - conn->last_activity) > TCP_TIME_WAIT_TIMEOUT) {
				NET_TCP_ClientRemove(idx);
			}
			continue;
		}

		// Retransmission: resend the oldest unacknowledged segment
		tcp_tx_seg_t *seg = tx_oldest_unacked(conn);
		if (seg != nullptr &&
		    (get_tick() - seg->sent_at > TCP_RETRANSMIT_TIMEOUT)) {

			if (seg->retries < TCP_RETRANSMISSION_MAX_COUNT) {
				tx_retransmit_slot(conn, seg);
				seg->sent_at = get_tick();
				seg->retries++;
			} else {
				// Max retransmissions reached - send RST and close
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
							TCP_RST, nullptr, 0);
				NET_TCP_ClientRemove(idx);
				continue;
			}
		}

		// Check keepalive timeout
		if (((get_tick() - conn->last_activity) > TCP_KEEPALIVE_TIMEOUT) &&
		    (conn->state == tcp_state_t::TCP_ESTABLISHED)) {

			if ((get_tick() - conn->last_keepalive) >= TCP_KEEPALIVE_INTERVAL) {

				if (conn->keep_alive_count >= TCP_KEEPALIVE_MAX_COUNT) {
					// Max keepalive attempts - send RST and close
					NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
								TCP_RST, nullptr, 0);
					NET_TCP_ClientRemove(idx);
					continue;
				}

				// Send keepalive (ACK with seq-1)
				NET_SendTCP(conn, conn->tcp_my_seq - 1, conn->rcv_next,
							TCP_ACK, nullptr, 0);

				conn->keep_alive_count++;
				conn->last_keepalive = get_tick();
			}
		}
	}
}

void NET_ProcessTCP(ipv4_frame *frame) {
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>(&frame->ip_hdr[frame->ip_hdr_len]);

	// Extract TCP header fields (network byte order)
	uint8_t client_flags = tcp->flags;
	uint16_t client_window = bswap16(tcp->window);
	uint16_t client_port = bswap16(tcp->src_port);
	uint16_t server_port = bswap16(tcp->dst_port);
	uint32_t client_ip = BuffU8ToU32(frame->scr_addr);
	uint32_t client_seq = bswap32(tcp->seq_num);
	uint32_t client_ack = bswap32(tcp->ack_num);

	// Find existing connection
	tcp_conn_t *conn = NET_TCP_FindConnection(client_ip, client_port, server_port);

	// Handle RST flag: only accept an in-sequence RST (anti-spoofing, RFC 793)
	if (conn && (client_flags & TCP_RST)) {
		if (client_seq == conn->rcv_next) {
			NET_TCP_ClientRemove(conn->socket_tag);
		}
		return;
	}

	// Handle new connection (SYN)
	if (!conn && (client_flags & TCP_SYN)) {
		// Only listening ports accept connections
		if (!NET_TCP_IsListening(server_port)) {
			NET_SendTCP_RST(frame, server_port, client_port, 0, client_seq + 1,
							TCP_RST | TCP_ACK);
			return;
		}

		int num = NET_TCP_ClientAdd();
		if (num == -1) {
			// No free connections - send RST
			NET_SendTCP_RST(frame, server_port, client_port, 0, client_seq + 1,
							TCP_RST | TCP_ACK);
			return;
		}
		
		// Initialize new connection
		conn = &tcp_clients[num];
		conn->socket_tag = static_cast<uint8_t>(num);
		conn->client_ip = client_ip;
		conn->last_activity = get_tick();
		memcpy(conn->client_mac, frame->scr_mac, MAC_ADDR_LEN);
		conn->client_port = client_port;
		conn->server_port = server_port;
		conn->tcp_client_seq = client_seq;
		conn->rcv_next = client_seq + 1;  // SYN consumes 1 sequence number
		conn->client_window = client_window;
		conn->window_size = SOCKET_RX_BUFF_LEN;
		conn->peer_mss = parse_mss_option(tcp);
		conn->tcp_my_seq = NET_Entropy();
		conn->snd_unack = conn->tcp_my_seq;
		conn->state = tcp_state_t::TCP_SYN_RCVD;

		// Send SYN+ACK advertising our MSS (queued for retransmission;
		// consumes one sequence number)
		const uint8_t mss_opt[4] = { 2, 4,
				static_cast<uint8_t>(TCP_OUR_MSS >> 8),
				static_cast<uint8_t>(TCP_OUR_MSS & 0xFF) };
		NET_TCP_EnqueueSegment(conn, TCP_SYN | TCP_ACK, nullptr, 0,
				mss_opt, sizeof(mss_opt));
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

	// Update connection state
	conn->client_window = client_window;

	// Process based on connection state
	switch (conn->state) {
	case tcp_state_t::TCP_SYN_RCVD:
		if (client_flags & TCP_ACK) {
			if ((client_ack == conn->tcp_my_seq) && (client_seq == conn->rcv_next)) {
				// ACK for SYN+ACK received - connection established
				conn->last_activity = get_tick();
				conn->snd_unack = client_ack;
				conn->keep_alive_count = 0;
				tx_release_acked(conn, client_ack);
				conn->state = tcp_state_t::TCP_ESTABLISHED;
			}
		}

		// Retransmit SYN+ACK if SYN received again
		if ((client_flags & TCP_SYN) && (client_seq + 1 == conn->rcv_next)) {
			tcp_tx_seg_t *synack = tx_oldest_unacked(conn);
			if (synack != nullptr) {
				tx_retransmit_slot(conn, synack);
			}
		}
		break;

	case tcp_state_t::TCP_ESTABLISHED:
		conn->last_activity = get_tick();
		conn->keep_alive_count = 0;

		// Validate sequence number is acceptable (in-order packets only for now)
		if (is_seq_acceptable(client_seq, conn->rcv_next)) {

			if (client_flags & TCP_FIN) {
				// FIN received - start graceful close
				conn->rcv_next += 1;  // FIN consumes 1 sequence number
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
							TCP_ACK, nullptr, 0);

				NET_TCP_EnqueueSegment(conn, TCP_FIN | TCP_ACK, nullptr, 0);
				conn->state = tcp_state_t::TCP_LAST_ACK;
			} else if (client_flags & TCP_ACK) {
				// Update acknowledged sequence; ACKs beyond what we sent
				// (or old duplicates) must not move snd_unack
				bool data_acked = is_ack_acceptable(client_ack,
						conn->snd_unack, conn->tcp_my_seq);
				if (data_acked) {
					conn->snd_unack = client_ack;
					// Release the TX slots covered by this ACK
					tx_release_acked(conn, client_ack);
				}

				// Extract TCP header length and validate
				uint16_t tcp_hdr_len = (tcp->offset_reserved >> 4) * 4;
				if (tcp_hdr_len < TCP_MIN_HDR_LEN || tcp_hdr_len > TCP_MAX_HDR_LEN) {
					break;  // Invalid TCP header length
				}

				// Calculate and validate data length
				uint16_t data_len = static_cast<uint16_t>(
					frame->ip_len - frame->ip_hdr_len - tcp_hdr_len);
				
				// Validate data length is reasonable (not larger than frame)
//				if (data_len > (frame->ip_len - frame->ip_hdr_len)) {
//					break;  // Invalid data length
//				}

				// Process payload if present
				if (data_len > 0) {
					const uint8_t *payload = reinterpret_cast<const uint8_t*>(tcp) + tcp_hdr_len;
					const uint16_t free_space = SOCKET_RX_BUFF_LEN - conn->rx_count;

					// Copy as much data as fits into the RX ring
					uint16_t copy_len = (data_len <= free_space) ? data_len : free_space;
					if (copy_len > 0) {
						uint16_t first = SOCKET_RX_BUFF_LEN - conn->rx_head;
						if (first > copy_len) {
							first = copy_len;
						}
						memcpy(&conn->socket_rx_buff[conn->rx_head], payload, first);
						if (copy_len > first) {
							memcpy(&conn->socket_rx_buff[0], payload + first,
									copy_len - first);
						}
						conn->rx_head = static_cast<uint16_t>(
								(conn->rx_head + copy_len) % SOCKET_RX_BUFF_LEN);
						conn->rx_count += copy_len;
						conn->rcv_next += copy_len;
					}
					// Note: If copy_len < data_len, the tail is dropped (buffer
					// full); the missing ACK makes the peer retransmit it

					// Update the window BEFORE advertising it in the ACK
					conn->window_size = SOCKET_RX_BUFF_LEN - conn->rx_count;

					// Send ACK
					NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
								TCP_ACK, nullptr, 0);
				}

				// Always update receive window (even if no data)
				conn->window_size = SOCKET_RX_BUFF_LEN - conn->rx_count;
			}
		} else {
			// Out-of-order or duplicate packet - send ACK with current rcv_next
			// This helps the sender recover from packet loss
			if (client_flags & TCP_ACK) {
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
							TCP_ACK, nullptr, 0);
			}
		}
		break;

	case tcp_state_t::TCP_FIN_WAIT_1:
		if ((client_flags & TCP_ACK) && (client_ack == conn->tcp_my_seq)) {
			conn->last_activity = get_tick();
			conn->keep_alive_count = 0;
			tx_release_acked(conn, client_ack);

			if (client_flags & TCP_FIN) {
				// Simultaneous close - ACK the FIN and wait out TIME_WAIT
				conn->rcv_next++;
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
							TCP_ACK, nullptr, 0);
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
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK,
						nullptr, 0);
			conn->last_activity = get_tick();
			conn->state = tcp_state_t::TCP_TIME_WAIT;
			return;
		}
		break;

	case tcp_state_t::TCP_TIME_WAIT:
		// Peer retransmitted its FIN (our ACK was lost) - re-ACK it
		if (client_flags & TCP_FIN) {
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK,
						nullptr, 0);
		}
		break;

	case tcp_state_t::TCP_LAST_ACK:
		if ((client_flags & TCP_ACK) && (client_ack == conn->tcp_my_seq) &&
		    (client_seq == conn->rcv_next)) {
			NET_TCP_ClientRemove(conn->socket_tag);
			return;
		}
		break;

	default:
		break;
	}
}

uint16_t NET_TCP_Available(tcp_conn_t *conn) {
	return (conn != nullptr) ? conn->rx_count : 0;
}

int NET_TCP_Read(tcp_conn_t *conn, uint8_t *dst, uint16_t maxlen) {
	if (conn == nullptr || dst == nullptr) {
		return -1;
	}

	uint16_t n = (conn->rx_count < maxlen) ? conn->rx_count : maxlen;
	if (n == 0) {
		return 0;
	}

	// Copy out of the ring (up to two chunks across the wrap point)
	uint16_t first = SOCKET_RX_BUFF_LEN - conn->rx_tail;
	if (first > n) {
		first = n;
	}
	memcpy(dst, &conn->socket_rx_buff[conn->rx_tail], first);
	if (n > first) {
		memcpy(dst + first, &conn->socket_rx_buff[0], n - first);
	}
	conn->rx_tail = static_cast<uint16_t>((conn->rx_tail + n) % SOCKET_RX_BUFF_LEN);
	conn->rx_count -= n;

	const uint16_t old_window = conn->window_size;
	conn->window_size = SOCKET_RX_BUFF_LEN - conn->rx_count;

	// The peer saw a (nearly) closed window: announce that it reopened,
	// otherwise it may never learn it can transmit again
	if (conn->state == tcp_state_t::TCP_ESTABLISHED &&
	    old_window < conn->peer_mss && conn->window_size >= conn->peer_mss) {
		NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK,
					nullptr, 0);
	}

	return n;
}

void NET_TCP_Close(tcp_conn_t *conn) {
	if (conn->state == tcp_state_t::TCP_ESTABLISHED) {
		NET_TCP_EnqueueSegment(conn, TCP_FIN | TCP_ACK, nullptr, 0);
		conn->state = tcp_state_t::TCP_FIN_WAIT_1;
	}
}

/**
 * @brief Send application data (sliding window)
 *
 * Splits the buffer into MSS-sized segments; each takes a TX queue slot
 * and must fit into the peer's advertised window. Partial sends are
 * normal - call again for the remainder once ACKs free slots/window.
 *
 * @return Number of bytes accepted (0 = no slot/window right now),
 *         -1 on invalid arguments or connection state
 */
int NET_TCP_SendUser(tcp_conn_t *conn, const uint8_t *data, uint16_t len) {
	if (conn == nullptr || conn->state != tcp_state_t::TCP_ESTABLISHED) {
		return -1;  // Not connected
	}
	if (data == nullptr || len == 0) {
		return -1;  // Invalid parameters
	}

	uint16_t mss = (conn->peer_mss < TCP_TX_SLOT_LEN) ? conn->peer_mss
	                                                  : TCP_TX_SLOT_LEN;
	if (mss == 0) {
		mss = TCP_DEFAULT_PEER_MSS;
	}

	const uint16_t send_window = (conn->client_window < TCP_MAX_WINDOW)
			? conn->client_window : TCP_MAX_WINDOW;

	uint16_t sent = 0;
	while (sent < len) {
		uint16_t chunk = len - sent;
		if (chunk > mss) {
			chunk = mss;
		}

		// Respect the peer's receive window (bytes already in flight count)
		const uint32_t in_flight = conn->tcp_my_seq - conn->snd_unack;
		if (in_flight + chunk > send_window) {
			break;  // window full - retry after ACKs arrive
		}

		if (!NET_TCP_EnqueueSegment(conn, TCP_ACK | TCP_PSH,
				data + sent, chunk)) {
			break;  // no free TX slot / driver queue full
		}
		sent += chunk;
	}
	return sent;
}
