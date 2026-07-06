/*
 * mcore_tcp.hpp
 *
 *  Created on: Sep 29, 2025
 *      Author: AkimovMA
 */
#pragma once
#include "core/def.hpp"
#include "net/net.hpp"

#define TCP_MAX_CONNECTIONS 10
#define TCP_MAX_LISTEN_PORTS 4

#define TCP_KEEPALIVE_TIMEOUT 20000
#define TCP_KEEPALIVE_MAX_COUNT 3
#define TCP_KEEPALIVE_INTERVAL 1000

#define TCP_TIME_WAIT_TIMEOUT 10000 // ms (scaled-down 2*MSL)

#define TCP_DEFAULT_PEER_MSS 536  // RFC 1122 default when no MSS option
#define TCP_OUR_MSS 1460          // advertised in SYN|ACK

#define TCP_RETRANSMISSION_MAX_COUNT 3
#define TCP_RETRANSMIT_TIMEOUT 2500 // ms
#define TCP_TEMPLATE_FRAME_LEN 54  // 14 (ETH) + 20 (IP) + 20 (TCP)

#define TCP_LAST_ACK_WAIT_TO_CLOSE 5000

// TX sliding window: up to TCP_TX_QUEUE_SLOTS segments in flight,
// each carrying at most TCP_TX_SLOT_LEN payload bytes
#define TCP_TX_QUEUE_SLOTS 3
#define TCP_TX_SLOT_LEN 1200

#define SOCKET_RX_BUFF_LEN 5120
#define SOCKET_TX_BUFF_LEN (TCP_TX_QUEUE_SLOTS * TCP_TX_SLOT_LEN)

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
 * @brief One in-flight TX segment (payload lives in socket_tx_buff slot)
 */
struct tcp_tx_seg_t {
	uint32_t seq;        // first sequence number of the segment
	uint32_t sent_at;    // tick of the last (re)transmission
	uint16_t len;        // payload length (0 for control segments)
	uint8_t retries;     // retransmission attempts so far
	uint8_t flags;       // TCP flags to rebuild the header
	uint8_t opts[4];     // TCP options (SYN|ACK carries MSS)
	uint8_t opts_len;
	bool used;
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
	uint32_t last_keepalive;      // Last keepalive sent timestamp
	
	// === 16-bit fields (ports, windows, buffer positions) ===
	uint16_t client_port;         // Client port number
	uint16_t server_port;         // Server port number
	uint16_t window_size;         // Our receive window size
	uint16_t client_window;       // Client's advertised window size
	uint16_t peer_mss;            // Peer's MSS from SYN option (or default)
	uint16_t rx_head;             // RX ring: write position
	uint16_t rx_tail;             // RX ring: read position
	uint16_t rx_count;            // RX ring: bytes buffered

	// === TX sliding window ===
	tcp_tx_seg_t tx_queue[TCP_TX_QUEUE_SLOTS];  // in-flight segment metadata

	// === 8-bit fields and small arrays ===
	uint8_t socket_tag;           // Connection identifier/index
	uint8_t keep_alive_count;     // Keepalive probe count
	uint8_t client_mac[MAC_ADDR_LEN];  // Client MAC address
	tcp_state_t state;            // Current TCP state
	
	// === Large buffers (at end to minimize struct size impact) ===
	uint8_t socket_rx_buff[SOCKET_RX_BUFF_LEN];  // Receive ring buffer
	uint8_t socket_tx_buff[SOCKET_TX_BUFF_LEN];  // TX slot arena (payloads)
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

void NET_SendTCP(tcp_conn_t *conn, uint32_t seq, uint32_t ack, uint8_t flags,
		const uint8_t *data, uint16_t data_len,
		const uint8_t *opts = nullptr, uint8_t opts_len = 0);

void NET_SendTCP_RST(ipv4_frame *frame, uint16_t src_port, uint16_t dst_port,
		uint32_t seq, uint32_t ack, uint8_t flags);

void NET_ProcessTCP(ipv4_frame *frame);
void NET_TCP_Timers(void);

void NET_TCP_Init(void);
int  NET_TCP_ClientAdd(void);
void NET_TCP_ClientRemove(uint8_t idx);

// Server ports: only SYNs to listening ports create connections
bool NET_TCP_Listen(uint16_t port);
void NET_TCP_StopListen(uint16_t port);

// Active close: FIN|ACK from ESTABLISHED, completes via FIN_WAIT/TIME_WAIT
void NET_TCP_Close(tcp_conn_t *conn);

int NET_TCP_SendUser(tcp_conn_t *conn, const uint8_t *data, uint16_t len);

// Received-data access: bytes buffered / consume up to maxlen bytes.
// Reading reopens the receive window; a window-update ACK is sent when
// the window grows from below the peer's MSS back to a usable size.
uint16_t NET_TCP_Available(tcp_conn_t *conn);
int NET_TCP_Read(tcp_conn_t *conn, uint8_t *dst, uint16_t maxlen);

// Defined in temp_frame.hpp (one TU: tcp.cpp); NET_SetIPAddr patches
// its baked-in source IP when DHCP changes the address
extern uint8_t TCPsendFrameTemplate[TCP_TEMPLATE_FRAME_LEN];
