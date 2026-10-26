/*
 * tcp.hpp — TCP: connection table, FSM (passive and active open), send and
 *           receive streams on the shared block pool (net/tcp_buf)
 *
 * A connection is a small control block; its data lives in two byte FIFOs
 * whose blocks come from one pool. So the limit is memory, not a connection
 * count: idle connections cost ~96 bytes, busy ones borrow blocks, and when
 * the pool runs low the receive window closes instead of anything failing.
 * Fairness and safety rules on top:
 *   - a connection holds at most TCP_CONN_RX_BLOCKS / TCP_CONN_TX_BLOCKS;
 *   - at most TCP_MAX_HALF_OPEN connections in SYN_RCVD: a SYN flood evicts
 *     the oldest half-open one instead of filling the table;
 *   - TCP_TCB_RESERVE control blocks and TCP_POOL_RESERVE_BLOCKS pool blocks
 *     are only for connections we open ourselves (MQTT etc.), so incoming
 *     traffic cannot lock our own client out.
 *
 *  Created on: Sep 29, 2025
 *      Author: AkimovMA
 */
#pragma once
#include "core/def.hpp"
#include "net/net.hpp"
#include "net/arp.hpp"
#include "net/tcp_buf.hpp"

// Control blocks: cheap (~96 B each), memory is governed by the pool
constexpr uint8_t  TCP_MAX_CONNECTIONS  = 64;
constexpr uint8_t  TCP_TCB_RESERVE      = 4;      // only for active opens
constexpr uint8_t  TCP_MAX_HALF_OPEN    = 8;      // SYN_RCVD at once (SYN flood)
constexpr uint8_t  TCP_MAX_LISTEN_PORTS = 4;

// Per-connection share of the pool (16 KB each way) and the active-open reserve
constexpr uint16_t TCP_CONN_RX_BLOCKS      = 32;
constexpr uint16_t TCP_CONN_TX_BLOCKS      = 32;
constexpr uint16_t TCP_POOL_RESERVE_BLOCKS = 16;  // 8 KB
static_assert(TCP_POOL_BLOCKS > 2 * TCP_POOL_RESERVE_BLOCKS,
              "MCORE_TCP_POOL_BLOCKS too small for the active-open reserve");
constexpr uint32_t TCP_RX_BUFF_MAX = uint32_t{TCP_CONN_RX_BLOCKS} * TCP_BLOCK_SIZE;
constexpr uint32_t TCP_TX_BUFF_MAX = uint32_t{TCP_CONN_TX_BLOCKS} * TCP_BLOCK_SIZE;

constexpr uint32_t TCP_KEEPALIVE_TIMEOUT   = 20000;  // ms
constexpr uint8_t  TCP_KEEPALIVE_MAX_COUNT = 3;
constexpr uint32_t TCP_KEEPALIVE_INTERVAL  = 1000;   // ms

constexpr uint32_t TCP_TIME_WAIT_TIMEOUT = 10000;    // ms (scaled-down 2*MSL)

constexpr uint16_t TCP_DEFAULT_PEER_MSS = 536;   // RFC 1122 default when no MSS option
constexpr uint16_t TCP_OUR_MSS          = 1460;  // advertised in SYN and SYN|ACK

constexpr uint8_t  TCP_RETRANSMISSION_MAX_COUNT = 3;
constexpr uint32_t TCP_RETRANSMIT_TIMEOUT = 2500;    // ms
constexpr uint32_t TCP_PERSIST_MIN_MS     = 1000;    // zero-window probe, doubling...
constexpr uint32_t TCP_PERSIST_MAX_MS     = 30000;   // ...up to this
constexpr uint16_t TCP_TEMPLATE_FRAME_LEN = 54;      // 14 (ETH) + 20 (IP) + 20 (TCP)

constexpr uint16_t TCP_EPHEMERAL_BASE = 49152;  // RFC 6335 dynamic range for our client ports

constexpr uint8_t TCP_FIN = 0x01;
constexpr uint8_t TCP_SYN = 0x02;
constexpr uint8_t TCP_RST = 0x04;
constexpr uint8_t TCP_PSH = 0x08;
constexpr uint8_t TCP_ACK = 0x10;
constexpr uint8_t TCP_URG = 0x20;

enum class tcp_state_t : uint8_t {
	TCP_CLOSED = 0,
	TCP_SYN_SENT,      // active open: our SYN is out, waiting for SYN|ACK
	TCP_SYN_RCVD,
	TCP_ESTABLISHED,
	TCP_FIN_WAIT_1,
	TCP_FIN_WAIT_2,
	TCP_LAST_ACK,
	TCP_TIME_WAIT
};

/**
 * @brief TCP connection control block
 *
 * Send side as in RFC 793: the TX FIFO holds every byte from snd_unack on
 * (sent-but-unacked, then not-yet-sent); tcp_my_seq is snd_nxt. SYN and FIN
 * occupy sequence numbers but no FIFO bytes.
 *
 * "client" is the remote end and "server_port" our local port, whichever
 * side opened the connection (the names predate active open).
 */
struct tcp_conn_t {
	// === 32-bit fields (sequence numbers, timers, IP) ===
	uint32_t client_ip;           // Client IP address (network byte order)
	uint32_t tcp_my_seq;          // snd_nxt: next sequence number to send
	uint32_t tcp_client_seq;      // Client's initial sequence number
	uint32_t rcv_next;            // Expected next receive sequence number
	uint32_t snd_unack;           // snd_una: oldest unacknowledged sequence number
	uint32_t last_activity;       // Last packet activity timestamp
	uint32_t last_keepalive;      // Last keepalive sent timestamp
	uint32_t rtx_at;              // when the oldest unacked data/SYN/FIN was (re)sent
	uint32_t persist_at;          // next zero-window probe
	uint32_t persist_ms;          // current probe interval (backoff)

	// === data streams on the shared pool ===
	tcp_fifo_t rx;                // received, not yet read by the application
	tcp_fifo_t tx;                // from snd_unack on: in flight, then unsent

	// === 16-bit fields (ports, windows) ===
	uint16_t client_port;         // Client port number
	uint16_t server_port;         // Server port number
	uint16_t window_size;         // Our receive window size (last advertised)
	uint16_t client_window;       // Client's advertised window size
	uint16_t peer_mss;            // Peer's MSS from SYN option (or default)

	// === 8-bit fields and small arrays ===
	uint8_t socket_tag;           // Connection identifier/index
	uint8_t keep_alive_count;     // Keepalive probe count
	uint8_t rtx_retries;          // retransmissions of the oldest unacked
	uint8_t client_mac[MAC_ADDR_LEN];  // Client MAC address
	tcp_state_t state;            // Current TCP state
	bool active_open;             // we connected (may use the reserves)
	bool fin_pending;             // close requested: FIN goes after the data
	bool fin_sent;                // our FIN is out (its number is tcp_my_seq - 1)
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

// Active open to ip:port from a fresh ephemeral local port. Non-blocking,
// like ARP/DNS: PENDING while the next hop's MAC is being resolved (call
// again), OK with *out in TCP_SYN_SENT (watch it reach ESTABLISHED, or drop
// to CLOSED on refusal/timeout), UNREACHABLE without a route or free slot.
net_result_t NET_TCP_Connect(const uint8_t *ip, uint16_t port, tcp_conn_t **out);

// True while `conn` is still the connection that was opened from local_port:
// a closed slot is zeroed and may be reused by another connection.
bool NET_TCP_IsOpen(const tcp_conn_t *conn, uint16_t local_port);

// Active close: FIN|ACK from ESTABLISHED, completes via FIN_WAIT/TIME_WAIT
void NET_TCP_Close(tcp_conn_t *conn);

// Hard close: RST to the peer, slot released at once
void NET_TCP_Abort(tcp_conn_t *conn);

// Queue data for sending: up to the connection's TX share and what the pool
// allows. Returns the bytes taken (0 = buffer full: call again after ACKs),
// -1 if not connected. Segments go out as the peer's window permits.
int NET_TCP_SendUser(tcp_conn_t *conn, const uint8_t *data, uint16_t len);

// Received-data access: bytes buffered / consume up to maxlen bytes.
// Reading reopens the receive window; a window-update ACK is sent when
// the window grows from below the peer's MSS back to a usable size.
uint16_t NET_TCP_Available(tcp_conn_t *conn);
int NET_TCP_Read(tcp_conn_t *conn, uint8_t *dst, uint16_t maxlen);

// Defined in tcp.cpp; NET_SetIPAddr patches its baked-in source IP when
// DHCP changes the address
extern uint8_t TCPsendFrameTemplate[TCP_TEMPLATE_FRAME_LEN];
