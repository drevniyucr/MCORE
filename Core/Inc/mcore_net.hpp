/*
 * mcore_net.hpp
 *
 *  Created on: Sep 24, 2025
 *      Author: AkimovMA
 */

#pragma once

#include "mcore_def.hpp"
#include "mcore_eth.hpp"

#define ETH_TYPE_IP  0x0800
#define IP_PROTO_TCP 6

#define MAC_ADDR_LEN 6
#define IP_ADDR_LEN 4

#define UDP_TEMPLATE_FRAME_LEN 42
#define ICMP_UNRCHBLE_FRAME_LEN 50
#define UDP_DST_PORT 5000

#define DST_ADDR_POS 30U
#define SCR_ADDR_POS 26U

#define ETH_HDR_POS 0U
#define ETH_HDR_LEN 14U

#define IP_HDR_POS 14U
#define IP_HDR_LEN 20U

#define UDP_HDR_POS 34U
#define UDP_HDR_LEN 8U

//#define TCP_HDR_POS 0U
#define TCP_HDR_LEN 20U

#define IP_VERSION_IHL     0x45
#define IP_TOS             0x00
#define IP_TTL             64
#define IP_PROTO_UDP       17


struct UDP_SendFrameStruct{
	uint8_t *DstMAC;
	uint8_t *DstIP;
	uint16_t DstPort;
	uint16_t SrcPort;
	uint8_t *pDataBuff;
	uint16_t DataBuffLen;
};

struct ipv4_frame{
	uint16_t ip_len;
	uint16_t frame_len;
	uint8_t  ip_hdr_len;
	uint8_t* scr_mac;
	uint8_t* scr_addr;
	uint8_t* ip_hdr;
	uint8_t** tx_buff;
	uint8_t* rx_buff;
};


extern uint8_t MAC_ADDR[6];
extern uint8_t IP_ADDR[4];

void NET_ProcessRx(ETH_RxDescListStruct* RxDesc, ETH_TxDescListStruct* TxDesc);

void NET_ProcessICMP(ipv4_frame* frame);
void NET_ProcessARP(ipv4_frame*  frame);
void NET_ProcessUDP(ipv4_frame*  frame);


void NET_SendUDP(UDP_SendFrameStruct SendFrame);
void NET_SendICMP_Unreachable(ipv4_frame* frame);

