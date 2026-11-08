/*
 * net.hpp — IPv4/ARP/ICMP/UDP layer: RX dispatch and frame builders
 *
 *  Created on: Sep 24, 2025
 *      Author: AkimovMA
 */

#pragma once

#include "core/def.hpp"
#include "drivers/eth.hpp"

// Optional serialisation of the stack. The task that runs ETH_RxWorker and
// NET_TCP_Timers owns the stack; when other (lower priority) tasks call the
// public TCP/SNTP functions, the application installs a lock that holds the
// owner off meanwhile (e.g. a scheduler priority ceiling). Default: none.
using NetLockFn = uint32_t (*)();
using NetUnlockFn = void (*)(uint32_t key);
void NET_SetLock(NetLockFn lock, NetUnlockFn unlock);

class NetGuard {
public:
	NetGuard();
	~NetGuard();
	NetGuard(const NetGuard &) = delete;
	NetGuard &operator=(const NetGuard &) = delete;

private:
	uint32_t key_;
};

#define ETH_TYPE_IP  0x0800
#define IP_PROTO_TCP 6

constexpr uint8_t MAC_ADDR_LEN = 6;
constexpr uint8_t IP_ADDR_LEN  = 4;

constexpr uint16_t UDP_TEMPLATE_FRAME_LEN  = 42;
constexpr uint16_t ICMP_UNRCHBLE_FRAME_LEN = 50;
constexpr uint16_t UDP_DST_PORT = 5000;

constexpr uint16_t DST_ADDR_POS = 30;
constexpr uint16_t SCR_ADDR_POS = 26;

constexpr uint16_t ETH_HDR_POS = 0;
constexpr uint16_t ETH_HDR_LEN = 14;

constexpr uint16_t IP_HDR_POS = 14;
constexpr uint16_t IP_HDR_LEN = 20;

constexpr uint16_t UDP_HDR_POS = 34;
constexpr uint16_t UDP_HDR_LEN = 8;

constexpr uint16_t TCP_HDR_LEN = 20;

constexpr uint8_t IP_VERSION_IHL = 0x45;
constexpr uint8_t IP_TOS         = 0x00;
constexpr uint8_t IP_TTL         = 64;


struct UDP_SendFrameStruct{
	uint8_t *DstMAC;
	uint8_t *DstIP;
	uint16_t DstPort;
	uint16_t SrcPort;
	uint8_t *pDataBuff;
	uint16_t DataBuffLen;
};

// RX-frame view handed to the protocol handlers. The RX buffer is read-only
// (handlers build replies in *tx_buff), so the pointers are const — no
// const_cast needed anywhere down the chain.
struct ipv4_frame{
	uint16_t ip_len;
	uint16_t frame_len;
	uint8_t  ip_hdr_len;
	uint8_t  is_broadcast;  // dst IP was 255.255.255.255
	const uint8_t* scr_mac;
	const uint8_t* scr_addr;
	const uint8_t* ip_hdr;
	uint8_t** tx_buff;
	const uint8_t* rx_buff;
};


extern uint8_t MAC_ADDR[6];
extern uint8_t IP_ADDR[4];
extern uint8_t NET_NETMASK[4];   // on-link subnet (default 255.255.255.0)
extern uint8_t NET_GATEWAY[4];   // default router, 0.0.0.0 = none

// Frame templates (defined in net.cpp); NET_SetIPAddr patches their
// baked-in source IP when the address changes
extern uint8_t UDPsendFrameTemplate[UDP_TEMPLATE_FRAME_LEN];
extern uint8_t icmp_port_unrchble_template[ICMP_UNRCHBLE_FRAME_LEN];

// Entropy source (TCP ISN, DHCP xid). Firmware implementation reads
// SysTick (mcore/net/port.cpp); host tests provide a deterministic stub.
uint32_t NET_Entropy(void);

// Change our IP at runtime (DHCP): updates IP_ADDR and the source-IP
// bytes baked into the UDP/ICMP/TCP frame templates.
void NET_SetIPAddr(const uint8_t *ip);

void NET_ProcessRx(ETH_RxDescListStruct* RxDesc, ETH_TxDescListStruct* TxDesc);

void NET_ProcessICMP(ipv4_frame* frame);
void NET_ProcessARP(ipv4_frame*  frame);
void NET_ProcessUDP(ipv4_frame*  frame);


void NET_SendUDP(const UDP_SendFrameStruct& SendFrame);
void NET_SendICMP_Unreachable(ipv4_frame* frame);

// Static configuration: address, subnet mask and default gateway in one go
// (also the fallback that DHCP restores when no server answers).
void NET_SetNetConfig(const uint8_t *ip, const uint8_t *mask, const uint8_t *gw);
void NET_SetNetmask(const uint8_t *mask);
void NET_SetGateway(const uint8_t *gw);

// ---------------------------------------------------------------------------
// UDP sockets (connectionless). A bound port receives every datagram sent to
// it; the handler runs inside NET_ProcessRx, so it must be short.
// ---------------------------------------------------------------------------
#define NET_UDP_MAX_BINDINGS 4

using NET_UDP_Handler = void (*)(const uint8_t *src_ip, uint16_t src_port,
		const uint8_t *data, uint16_t len, void *ctx);

bool NET_UDP_Bind(uint16_t port, NET_UDP_Handler handler, void *ctx);
void NET_UDP_Unbind(uint16_t port);

// Unicast datagram. The next hop's MAC must be known (see NET_ARP_Resolve):
// returns false without sending if it is not, or if the TX queue is full.
bool NET_UDP_SendTo(const uint8_t *dst_ip, uint16_t dst_port, uint16_t src_port,
		const uint8_t *data, uint16_t len);

// ---------------------------------------------------------------------------
// ARP cache (net/arp.cpp)
// ---------------------------------------------------------------------------
#define NET_ARP_CACHE_SIZE     8
#define NET_ARP_ENTRY_TTL_MS   300000   // re-resolve after 5 min
#define NET_ARP_RETRY_MS       1000     // min gap between requests for one IP

// IP whose MAC an outgoing packet to dst needs: dst itself when on-link,
// otherwise the gateway. Returns false if dst is off-link and there is no
// gateway.
bool NET_ARP_NextHop(const uint8_t *dst_ip, uint8_t *next_hop);

// Copies the MAC for dst_ip (routed via NET_ARP_NextHop) into mac and
// returns true if it is cached. Otherwise sends an ARP request (rate
// limited) and returns false: call again later.
bool NET_ARP_Resolve(const uint8_t *dst_ip, uint8_t *mac);

// Learns/refreshes a mapping (called for every ARP packet addressed to us).
void NET_ARP_Learn(const uint8_t *ip, const uint8_t *mac);
void NET_ARP_Flush();

