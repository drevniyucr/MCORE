/*
 * arp.hpp — ARP cache and next-hop resolution (RFC 826)
 *
 * Lets the stack START a conversation: before, it could only answer, sending
 * replies to the MAC of the incoming frame. Now any IPv4 destination can be
 * turned into a MAC:
 *
 *   uint8_t mac[6];
 *   switch (NET_ARP_Resolve(dst_ip, mac)) {
 *   case net_result_t::OK:          send to mac
 *   case net_result_t::PENDING:     ARP request is out, try again later
 *   case net_result_t::UNREACHABLE: off-subnet without a gateway, or no reply
 *   }
 *
 * No packet queue: a caller that gets PENDING simply retries from its own
 * poll loop (DNS, NTP and TCP retransmission all do anyway), so an
 * unresolved destination costs no buffer memory.
 */
#pragma once

#include "core/def.hpp"
#include "net/net.hpp"

constexpr uint8_t  ARP_CACHE_SIZE     = 8;
constexpr uint32_t ARP_ENTRY_TTL_MS   = 300000;  // re-resolve every 5 min
constexpr uint32_t ARP_RETRY_MS       = 1000;
constexpr uint8_t  ARP_MAX_RETRIES    = 3;       // requests before giving up
constexpr uint32_t ARP_FAIL_HOLD_MS   = 5000;    // UNREACHABLE answered from cache this long

constexpr uint16_t ETH_TYPE_ARP       = 0x0806;
constexpr uint16_t ARP_FRAME_LEN      = 42;      // 14 ETH + 28 ARP

enum class net_result_t : uint8_t {
	OK,           // done (MAC / address available)
	PENDING,      // request sent, call again later
	UNREACHABLE,  // no route, no reply, or no server configured
};

enum class arp_state_t : uint8_t {
	FREE = 0,
	PENDING,      // request sent, waiting for the reply
	RESOLVED,
	FAILED,       // no reply after ARP_MAX_RETRIES: negative-cached
};

struct arp_entry_t {
	uint8_t ip[IP_ADDR_LEN];
	uint8_t mac[MAC_ADDR_LEN];
	arp_state_t state;
	uint8_t retries;
	uint32_t stamp;   // RESOLVED: learned at; PENDING: last request; FAILED: gave up at
};

extern arp_entry_t arp_cache[ARP_CACHE_SIZE];

// Next hop for dst: dst itself on our subnet, the gateway otherwise.
// false: off-subnet and no gateway configured.
bool NET_NextHop(const uint8_t *dst, uint8_t *hop);

// MAC for an IPv4 destination (routed through the gateway when needed).
// 255.255.255.255 and our subnet broadcast resolve to ff:ff:ff:ff:ff:ff.
net_result_t NET_ARP_Resolve(const uint8_t *dst, uint8_t *mac_out);

// Learn from a received ARP packet (request or reply) and answer requests
// for our address. Wired up in NET_ProcessRx.
void NET_ProcessARP(ipv4_frame *frame);

// Retries pending requests and ages entries; call periodically (~100 ms).
void NET_ARP_Poll();

// Drop every entry (our address or subnet changed).
void NET_ARP_Flush();
