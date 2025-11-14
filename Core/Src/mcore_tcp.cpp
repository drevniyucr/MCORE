/*
* mcore_tcp.cpp
*
*  Created on: Sep 28, 2025
*      Author: AkimovMA
*/
#include <cstddef>
#include <cstring>
#include "mcore_tcp.hpp"
#include "mcore_net.hpp"
#include "mcore_system.hpp"
#include "mcore_eth_utils.hpp"
#include "mcore_eth.hpp"
#include "mcore_temp_frame.hpp"

namespace {
	// Constants for TCP processing
	constexpr uint16_t TCP_MIN_HDR_LEN = 20;  // Minimum TCP header length
	constexpr uint16_t TCP_MAX_HDR_LEN = 60;  // Maximum TCP header length
	constexpr uint32_t TCP_INVALID_SEQ = 0xFFFFFFFFU;
	constexpr uint32_t TCP_MAX_WINDOW = 65535U;  // Maximum TCP window size
	
	/**
	 * @brief Check if sequence number is within acceptable receive window
	 * 
	 * @param seq Sequence number to check
	 * @param rcv_next Expected next sequence number
	 * @param window_size Receive window size
	 * @return true if sequence is acceptable, false otherwise
	 */
	inline bool is_seq_acceptable(uint32_t seq, uint32_t rcv_next, uint16_t window_size) {
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
		return (ack > snd_unack && ack <= tcp_my_seq);
	}
}

tcp_conn_t tcp_clients[TCP_MAX_CONNECTIONS] = { };
uint8_t active_list[TCP_MAX_CONNECTIONS] = { };
uint8_t active_conn = 0;

uint8_t free_list[TCP_MAX_CONNECTIONS];
uint8_t free_top = 0;

uint8_t **ppTxBuff = NULL;

void NET_TCP_Init() {
	memset(tcp_clients, 0, sizeof(tcp_clients));
	free_top = 0;
	active_conn = 0;
	ppTxBuff = &TxDescList.pBuff;
	for (uint8_t i = 0; i < TCP_MAX_CONNECTIONS; i++) {
		free_list[free_top++] = i;
	}
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
 * @brief Check if server port is already in use
 * 
 * @param server_port Server port to check
 * @return true if port is in use, false otherwise
 */
static bool NET_TCP_IsPortInUse(uint16_t server_port) {
	for (int i = 0; i < active_conn; i++) {
		uint8_t idx = active_list[i];
		if (tcp_clients[idx].server_port == server_port) {
			return true;
		}
	}
	return false;
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

/**
 * @brief Retransmit saved TCP frame
 * 
 * Uses saved frame from connection buffer
 */
void NET_SendRetransmitTCP(tcp_conn_t *conn) {
	// Check TX buffer availability
	if (!ETH_IsTxBufferAvailable()) {
		return;  // TX queue full
	}

	uint8_t* tx_buf = *ppTxBuff;
	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(tx_buf);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>(tx_buf + ETH_HDR_LEN);
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>(tx_buf + ETH_HDR_LEN + IP_HDR_LEN);

	// Copy Ethernet + IP headers from template
	memcpy(eth, TCPsendFrameTemplate, ETH_HDR_LEN + IP_HDR_LEN);
	
	// Update Ethernet destination MAC
	memcpy(eth->dst, conn->client_mac, MAC_ADDR_LEN);
	
	// Update IP header
	ip->tot_len = bswap16(IP_HDR_LEN + conn->soc_tx_buff_pos);
	ip->dst_ip = bswap32(conn->client_ip);
	
	// Copy saved TCP header + payload
	memcpy(tcp, conn->socket_tx_buff, conn->soc_tx_buff_pos);

	// Send frame
	if (!ETH_SendFrame(ETH_HDR_LEN + IP_HDR_LEN + conn->soc_tx_buff_pos)) {
		// TX failed - retransmission will be retried on next timer
	}
}

/**
 * @brief Send TCP frame on established socket
 * 
 * Optimized frame construction with proper error handling
 */
void NET_SendTCP(tcp_conn_t *conn, uint32_t seq, uint32_t ack, uint8_t flags,
		uint8_t retransmit, const uint8_t *data, uint16_t data_len) {
	
	// Validate data length
	if (data_len > (ETH_MAX_PACKET_SIZE - ETH_HDR_LEN - IP_HDR_LEN - TCP_HDR_LEN)) {
		return;  // Frame too large
	}


	uint8_t* tx_buf = *ppTxBuff;
	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(tx_buf);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>(tx_buf + ETH_HDR_LEN);
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>(tx_buf + ETH_HDR_LEN + IP_HDR_LEN);

	// Copy template
	memcpy(eth, TCPsendFrameTemplate, TCP_TEMPLATE_FRAME_LEN);
	
	// Update Ethernet destination MAC
	memcpy(eth->dst, conn->client_mac, MAC_ADDR_LEN);
	
	// Update IP header
	ip->tot_len = bswap16(IP_HDR_LEN + TCP_HDR_LEN + data_len);
	ip->dst_ip = bswap32(conn->client_ip);
	
	// Update TCP header
	tcp->src_port = bswap16(conn->server_port);
	tcp->dst_port = bswap16(conn->client_port);
	tcp->seq_num = bswap32(seq);
	tcp->ack_num = bswap32(ack);
	tcp->window = bswap16(conn->window_size);
	tcp->flags = flags;
	tcp->offset_reserved = 0x50;  // Data offset = 5 (20 bytes)
	tcp->urgent_ptr = 0;

	// Copy payload if present
	if (data_len > 0 && data != nullptr) {
		memcpy(reinterpret_cast<uint8_t*>(tcp) + TCP_HDR_LEN, data, data_len);
	}

	// Save for retransmission if requested
	if (retransmit == SAVE_FOR_RETRANSMIT) {
		const uint16_t tcp_total_len = TCP_HDR_LEN + data_len;
		if (tcp_total_len <= SOCKET_TX_BUFF_LEN) {
			memcpy(conn->socket_tx_buff, tcp, tcp_total_len);
			conn->soc_tx_buff_pos = tcp_total_len;
			conn->retransmit_timer = get_tick();
			conn->retransmit_count = 0;
		}
	}

	// Send frame
	if (!ETH_SendFrame(ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN + data_len)) {
		// TX failed - frame dropped
		// If retransmit was requested, it won't be saved
		if (retransmit == SAVE_FOR_RETRANSMIT) {
			conn->soc_tx_buff_pos = 0;
		}
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

		// Check retransmission timeout
		if (conn->retransmit_timer &&
		    (get_tick() - conn->retransmit_timer > TCP_RETRANSMIT_TIMEOUT)) {

			if (conn->retransmit_count < TCP_RETRANSMISSION_MAX_COUNT) {
				NET_SendRetransmitTCP(conn);
				conn->retransmit_timer = get_tick();
				conn->retransmit_count++;
			} else {
				// Max retransmissions reached - send RST and close
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
							TCP_RST, NOT_SAVE_FOR_RETRANSMIT, nullptr, 0);
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
								TCP_RST, NOT_SAVE_FOR_RETRANSMIT, nullptr, 0);
					NET_TCP_ClientRemove(idx);
					continue;
				}

				// Send keepalive (ACK with seq-1)
				NET_SendTCP(conn, conn->tcp_my_seq - 1, conn->tcp_client_seq,
							TCP_ACK, NOT_SAVE_FOR_RETRANSMIT, nullptr, 0);

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

	// Handle RST flag
	if (conn && (client_flags & TCP_RST)) {
		NET_TCP_ClientRemove(conn->socket_tag);
		return;
	}

	// Check if port is in use by another connection
	if (!conn && NET_TCP_IsPortInUse(server_port)) {
		NET_SendTCP_RST(frame, server_port, client_port, 0, client_seq + 1, TCP_RST);
		return;
	}

	// Handle new connection (SYN)
	if (!conn && (client_flags & TCP_SYN)) {
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
		conn->tcp_my_seq = SysTick::_STCVR::read();
		conn->snd_unack = conn->tcp_my_seq;
		conn->state = tcp_state_t::TCP_SYN_RCVD;
		
		// Send SYN+ACK
		NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
					TCP_SYN | TCP_ACK, SAVE_FOR_RETRANSMIT, nullptr, 0);
		conn->tcp_my_seq += 1;  // SYN consumes 1 sequence number
		return;
	}

	// No connection found for this packet
	if (!conn) {
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
				conn->retransmit_timer = 0;
				conn->retransmit_count = 0;
				conn->soc_tx_buff_pos = 0;
				conn->state = tcp_state_t::TCP_ESTABLISHED;
			}
		}

		// Retransmit SYN+ACK if SYN received again
		if ((client_flags & TCP_SYN) && (client_seq + 1 == conn->rcv_next)) {
			NET_SendRetransmitTCP(conn);
		}
		break;

	case tcp_state_t::TCP_ESTABLISHED:
		conn->last_activity = get_tick();
		conn->keep_alive_count = 0;

		// Validate sequence number is acceptable (in-order packets only for now)
		if (is_seq_acceptable(client_seq, conn->rcv_next, conn->window_size)) {
			
			// Validate ACK number if ACK flag is set
			if ((client_flags & TCP_ACK) && !is_ack_acceptable(client_ack, conn->snd_unack, conn->tcp_my_seq)) {
				// Invalid ACK - ignore packet (could be old duplicate)
				break;
			}

			if (client_flags & TCP_FIN) {
				// FIN received - start graceful close
				conn->rcv_next += 1;  // FIN consumes 1 sequence number
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
							TCP_ACK, NOT_SAVE_FOR_RETRANSMIT, nullptr, 0);

				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
							TCP_FIN | TCP_ACK, SAVE_FOR_RETRANSMIT, nullptr, 0);
				conn->tcp_my_seq += 1;  // FIN consumes 1 sequence number
				conn->state = tcp_state_t::TCP_LAST_ACK;
			} else if (client_flags & TCP_ACK) {
				// Update acknowledged sequence (validated above)
				bool data_acked = (client_ack > conn->snd_unack);
				if (data_acked) {
					conn->snd_unack = client_ack;
					// Clear retransmission state since data was acknowledged
					conn->soc_tx_buff_pos = 0;
					conn->retransmit_timer = 0;
					conn->retransmit_count = 0;
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
				if (data_len > (frame->ip_len - frame->ip_hdr_len)) {
					break;  // Invalid data length
				}

				// Process payload if present
				if (data_len > 0) {
					const uint8_t *payload = reinterpret_cast<const uint8_t*>(tcp) + tcp_hdr_len;
					uint16_t free_space = SOCKET_RX_BUFF_LEN - conn->soc_rx_buff_pos;

					// Copy as much data as fits in buffer
					uint16_t copy_len = (data_len <= free_space) ? data_len : free_space;
					if (copy_len > 0) {
						memcpy(&conn->socket_rx_buff[conn->soc_rx_buff_pos], payload, copy_len);
						conn->soc_rx_buff_pos += copy_len;
						conn->rcv_next += copy_len;
					}
					// Note: If copy_len < data_len, data is dropped (buffer full)
				}

				// Always update receive window (even if no data)
				conn->window_size = SOCKET_RX_BUFF_LEN - conn->soc_rx_buff_pos;

				// Send ACK
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
							TCP_ACK, NOT_SAVE_FOR_RETRANSMIT, nullptr, 0);
			}
		} else {
			// Out-of-order or duplicate packet - send ACK with current rcv_next
			// This helps the sender recover from packet loss
			if (client_flags & TCP_ACK) {
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
							TCP_ACK, NOT_SAVE_FOR_RETRANSMIT, nullptr, 0);
			}
		}
		break;

	case tcp_state_t::TCP_FIN_WAIT_1:
		if ((client_flags & TCP_ACK) && (client_ack == conn->tcp_my_seq)) {
			conn->last_activity = get_tick();
			conn->keep_alive_count = 0;
			conn->soc_tx_buff_pos = 0;
			conn->retransmit_timer = 0;
			conn->retransmit_count = 0;

			if (client_flags & TCP_FIN) {
				// Simultaneous close - send ACK and close
				conn->rcv_next++;
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
							TCP_ACK, NOT_SAVE_FOR_RETRANSMIT, nullptr, 0);
				NET_TCP_ClientRemove(conn->socket_tag);
				return;
			}
			conn->state = tcp_state_t::TCP_FIN_WAIT_2;
		}
		break;

	case tcp_state_t::TCP_FIN_WAIT_2:
		if ((client_flags & TCP_FIN) && (client_seq == conn->rcv_next)) {
			conn->rcv_next += 1;
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK,
						NOT_SAVE_FOR_RETRANSMIT, nullptr, 0);
			NET_TCP_ClientRemove(conn->socket_tag);
			return;
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

void NET_TCP_Close(tcp_conn_t *conn) {
	if (conn->state == tcp_state_t::TCP_ESTABLISHED) {
		NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_FIN,
					SAVE_FOR_RETRANSMIT, nullptr, 0);
		conn->state = tcp_state_t::TCP_FIN_WAIT_1;
	}
}

int NET_TCP_SendUser(tcp_conn_t *conn, const uint8_t *data, uint16_t len) {
	if (conn->state != tcp_state_t::TCP_ESTABLISHED) {
		return -1;  // Not connected
	}

	// Validate input
	if (data == nullptr || len == 0) {
		return -1;  // Invalid parameters
	}

	// Check send window (client's receive window)
	// Use minimum of our send window and client's advertised window
	uint16_t send_window = (conn->client_window < TCP_MAX_WINDOW) ? 
	                       conn->client_window : TCP_MAX_WINDOW;
	
	if (len > send_window) {
		return -1;  // Data too large for send window
	}

	// Check if we can send (only if all previous data was acknowledged)
	if (conn->tcp_my_seq == conn->snd_unack) {
		NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK | TCP_PSH,
					SAVE_FOR_RETRANSMIT, data, len);
		conn->tcp_my_seq += len;
		return 0;
	} else {
		return -2;  // Previous data not acknowledged yet
	}
}
