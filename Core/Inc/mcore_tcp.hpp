/*
 * mcore_tcp.hpp
 *
 *  Created on: Sep 29, 2025
 *      Author: AkimovMA
 */
#pragma once
#include "mcore_def.hpp"
#include "mcore_net.hpp"

#define TCP_MAX_CONNECTIONS 10

#define TCP_KEEPALIVE_TIMEOUT 20000
#define TCP_KEEPALIVE_MAX_COUNT 3
#define TCP_KEEPALIVE_INTERVAL 1000

#define TCP_RETRANSMIT_MAX_BUFF_SIZE 512
#define TCP_RETRANSMISSION_MAX_COUNT 3
#define TCP_RETRANSMIT_MAX_QUEUE 3
#define TCP_RETRANSMIT_TIMEOUT 2500 // ms
#define SAVE_FOR_RETRANSMIT 1
#define NOT_SAVE_FOR_RETRANSMIT 0
#define TCP_TEMPLATE_FRAME_LEN 54  // 14 (ETH) + 20 (IP) + 20 (TCP)

#define TCP_LAST_ACK_WAIT_TO_CLOSE 5000

#define SOCKET_RX_BUFF_LEN 5120
#define SOCKET_TX_BUFF_LEN 1200

#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20

enum class tcp_state_t : uint8_t {
	TCP_CLOSED = 0,
	TCP_SYN_RCVD,
	TCP_ESTABLISHED,
	TCP_FIN_WAIT_1,
	TCP_FIN_WAIT_2,
	TCP_LAST_ACK,
	TCP_TIME_WAIT
};

/**
 * @brief TCP connection structure
 * 
 * Optimized memory layout:
 * - 32-bit fields grouped together (minimizes padding)
 * - 16-bit fields grouped together
 * - 8-bit fields grouped together
 * - Large buffers at the end
 * - Frequently accessed fields at the beginning
 */
struct tcp_conn_t {
	// === 32-bit fields (sequence numbers, timers, IP) ===
	uint32_t client_ip;           // Client IP address (network byte order)
	uint32_t tcp_my_seq;          // Our sequence number
	uint32_t tcp_client_seq;      // Client's sequence number
	uint32_t rcv_next;            // Expected next receive sequence number
	uint32_t snd_unack;           // Sent but not acknowledged sequence number
	uint32_t last_activity;       // Last packet activity timestamp
	uint32_t retransmit_timer;    // Retransmission timer timestamp
	uint32_t last_keepalive;      // Last keepalive sent timestamp
	
	// === 16-bit fields (ports, windows, buffer positions) ===
	uint16_t client_port;         // Client port number
	uint16_t server_port;         // Server port number
	uint16_t window_size;         // Our receive window size
	uint16_t client_window;       // Client's advertised window size
	uint16_t soc_rx_buff_pos;     // RX buffer write position
	uint16_t soc_tx_buff_pos;     // TX buffer position (for retransmission)
	
	// === 8-bit fields and small arrays ===
	uint8_t socket_tag;           // Connection identifier/index
	uint8_t retransmit_count;     // Number of retransmission attempts
	uint8_t keep_alive_count;     // Keepalive probe count
	uint8_t client_mac[MAC_ADDR_LEN];  // Client MAC address
	tcp_state_t state;            // Current TCP state
	
	// === Large buffers (at end to minimize struct size impact) ===
	uint8_t socket_rx_buff[SOCKET_RX_BUFF_LEN];  // Receive buffer
	uint8_t socket_tx_buff[SOCKET_TX_BUFF_LEN];  // Retransmission buffer
};

#pragma pack(push,1)

struct eth_hdr_t {
	uint8_t dst[6];
	uint8_t src[6];
	uint16_t type;
};

struct ip_hdr_t {
	uint8_t ver_ihl;
	uint8_t tos;
	uint16_t tot_len;
	uint16_t id;
	uint16_t frag_off;
	uint8_t ttl;
	uint8_t proto;
	uint16_t checksum;
	uint32_t src_ip;
	uint32_t dst_ip;
};

struct tcp_hdr_t {
	uint16_t src_port;
	uint16_t dst_port;
	uint32_t seq_num;
	uint32_t ack_num;
	uint8_t offset_reserved;
	uint8_t flags;
	uint16_t window;
	uint16_t checksum;
	uint16_t urgent_ptr;
};

#pragma pack(pop)

extern tcp_conn_t tcp_clients[TCP_MAX_CONNECTIONS];

void NET_SendTCP(tcp_conn_t *conn, uint32_t seq, uint32_t ack, uint8_t flags, uint8_t retransmit,
		const uint8_t *data, uint16_t data_len);

void NET_SendTCP_RST(ipv4_frame *frame, uint16_t src_port, uint16_t dst_port,
		uint32_t seq, uint32_t ack, uint8_t flags);

void NET_SendRetransmitTCP(tcp_conn_t *conn);

void NET_ProcessTCP(ipv4_frame *frame);
void NET_TCP_Timers(void);
void NET_TCP_RetransmitCheck(void);

void NET_TCP_Init(void);
int  NET_TCP_ClientAdd(void);
void NET_TCP_ClientRemove(uint8_t idx);

int NET_TCP_SendUser(tcp_conn_t *conn, const uint8_t *data, uint16_t len);
