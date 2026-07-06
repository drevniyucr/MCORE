/*
 * net.hpp — IPv4/ARP/ICMP/UDP layer: RX dispatch and frame builders
 *
 *  Created on: Sep 24, 2025
 *      Author: AkimovMA
 */

#pragma once

#include "core/def.hpp"
#include "drivers/eth.hpp"

constexpr uint16_t ETH_TYPE_IP  = 0x0800;
constexpr uint8_t  IP_PROTO_TCP = 6;
constexpr uint8_t  IP_PROTO_UDP = 17;

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

