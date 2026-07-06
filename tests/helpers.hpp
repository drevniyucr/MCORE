// Frame factories for injecting fabricated packets into the stack.
#pragma once

#include <cstring>

#include "drivers/eth.hpp"
#include "net/net.hpp"
#include "net/tcp.hpp"
#include "net/eth_utils.hpp"
#include "test_support.hpp"

// The simulated remote peer ("client") talking to the stack.
inline uint8_t CLIENT_MAC[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
inline uint8_t CLIENT_IP[4] = { 192, 168, 0, 10 };

// Builds Ethernet + IPv4 headers into buf (client -> us).
// Returns a pointer to the L4 area (buf + 34).
inline uint8_t* build_ipv4(uint8_t *buf, uint8_t proto, uint16_t l4_len,
		const uint8_t *dst_ip = IP_ADDR) {
	memset(buf, 0, ETH_HDR_LEN + IP_HDR_LEN + l4_len);
	memcpy(buf, MAC_ADDR, MAC_ADDR_LEN);            // dst = our MAC
	memcpy(&buf[6], CLIENT_MAC, MAC_ADDR_LEN);      // src = client MAC
	buf[12] = 0x08;
	buf[13] = 0x00;                                 // ethertype IPv4

	uint8_t *ip = &buf[IP_HDR_POS];
	const uint16_t tot_len = IP_HDR_LEN + l4_len;
	ip[0] = 0x45;                                   // ver 4, IHL 5
	ip[2] = static_cast<uint8_t>(tot_len >> 8);
	ip[3] = static_cast<uint8_t>(tot_len & 0xFF);
	ip[8] = 64;                                     // TTL
	ip[9] = proto;
	memcpy(&ip[12], CLIENT_IP, IP_ADDR_LEN);        // src IP
	memcpy(&ip[16], dst_ip, IP_ADDR_LEN);           // dst IP
	return &buf[ETH_HDR_LEN + IP_HDR_LEN];
}

// Wraps a raw frame into the ipv4_frame handle the protocol handlers expect.
inline ipv4_frame make_ipv4_frame(uint8_t *buf, uint16_t l4_len) {
	ipv4_frame f = {};
	f.rx_buff = buf;
	f.tx_buff = &TxDescList.pBuff;
	f.ip_hdr = &buf[IP_HDR_POS];
	f.ip_hdr_len = IP_HDR_LEN;
	f.ip_len = static_cast<uint16_t>(IP_HDR_LEN + l4_len);
	f.scr_addr = &buf[SCR_ADDR_POS];
	f.scr_mac = &buf[6];
	f.frame_len = static_cast<uint16_t>(ETH_HDR_LEN + IP_HDR_LEN + l4_len);
	return f;
}

// Feeds a raw frame through NET_ProcessRx with a fabricated DMA descriptor.
// desc4 carries the hardware protocol-detection bits (ETH_DMAPTPRXDESC_*).
inline void process_rx(uint8_t *frame, uint16_t len, uint32_t desc4) {
	static ETH_DMADescStruct desc;
	memset(const_cast<uint32_t*>(&desc.DESC0), 0, sizeof(desc));
	desc.DESC4 = desc4;

	ETH_RxDescListStruct rx = {};
	rx.CurrRxDesc = &desc;
	rx.pBuff = frame;
	rx.BuffLen = len;
	NET_ProcessRx(&rx, &TxDescList);
}

// Builds a TCP segment (client -> us) and injects it into NET_ProcessTCP.
// opts, if given, must be a multiple of 4 bytes (TCP option words).
inline void inject_tcp(uint16_t sport, uint16_t dport, uint32_t seq, uint32_t ack,
		uint8_t flags, const uint8_t *payload = nullptr, uint16_t plen = 0,
		uint16_t window = 1024,
		const uint8_t *opts = nullptr, uint8_t opts_len = 0) {
	static uint8_t buf[ETH_MAX_PACKET_SIZE];
	const uint16_t l4_len = TCP_HDR_LEN + opts_len + plen;
	uint8_t *l4 = build_ipv4(buf, IP_PROTO_TCP, l4_len);

	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>(l4);
	tcp->src_port = bswap16(sport);
	tcp->dst_port = bswap16(dport);
	tcp->seq_num = bswap32(seq);
	tcp->ack_num = bswap32(ack);
	tcp->offset_reserved = static_cast<uint8_t>((5 + opts_len / 4) << 4);
	tcp->flags = flags;
	tcp->window = bswap16(window);
	if (opts_len > 0 && opts != nullptr) {
		memcpy(l4 + TCP_HDR_LEN, opts, opts_len);
	}
	if (plen > 0 && payload != nullptr) {
		memcpy(l4 + TCP_HDR_LEN + opts_len, payload, plen);
	}

	ipv4_frame f = make_ipv4_frame(buf, l4_len);
	NET_ProcessTCP(&f);
}

// --- Captured-frame accessors ---

inline const tcp_hdr_t* captured_tcp(const CapturedFrame &f) {
	return reinterpret_cast<const tcp_hdr_t*>(f.data.data() + ETH_HDR_LEN + IP_HDR_LEN);
}

inline const uint8_t* captured_tcp_payload(const CapturedFrame &f) {
	return f.data.data() + ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN;
}

// Finds the live connection for a client port, nullptr if none.
inline tcp_conn_t* find_conn(uint16_t client_port) {
	for (auto &c : tcp_clients) {
		if (c.state != tcp_state_t::TCP_CLOSED && c.client_port == client_port) {
			return &c;
		}
	}
	return nullptr;
}
