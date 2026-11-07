/*
 * arp.cpp — ARP cache and resolution for outgoing (client) traffic.
 *
 * Replies to requests for our own address stay in NET_ProcessARP (net.cpp);
 * this file adds what an active opener needs: a small cache filled from
 * every ARP packet addressed to us, rate-limited requests and the on-link /
 * gateway decision.
 */
#include <cstring>

#include "net/net.hpp"
#include "core/system.hpp"
#include "drivers/eth.hpp"

namespace {

struct arp_entry_t {
	uint8_t ip[IP_ADDR_LEN];
	uint8_t mac[MAC_ADDR_LEN];
	uint32_t updated;       // tick of the last reply/learn
	uint32_t requested;     // tick of the last request we sent
	bool resolved;
	bool used;
};

arp_entry_t cache[NET_ARP_CACHE_SIZE] = {};

constexpr uint8_t BCAST_MAC[MAC_ADDR_LEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
constexpr uint16_t ARP_FRAME_LEN = 42;   // 14 Ethernet + 28 ARP

bool ip_is_zero(const uint8_t *ip) {
	return (ip[0] | ip[1] | ip[2] | ip[3]) == 0;
}

arp_entry_t *find(const uint8_t *ip) {
	for (arp_entry_t &e : cache) {
		if (e.used && memcmp(e.ip, ip, IP_ADDR_LEN) == 0) {
			return &e;
		}
	}
	return nullptr;
}

// Free slot, else the least recently updated one
arp_entry_t *alloc(const uint8_t *ip) {
	arp_entry_t *victim = &cache[0];
	for (arp_entry_t &e : cache) {
		if (!e.used) {
			victim = &e;
			break;
		}
		if (static_cast<int32_t>(e.updated - victim->updated) < 0) {
			victim = &e;
		}
	}
	memset(victim, 0, sizeof(*victim));
	memcpy(victim->ip, ip, IP_ADDR_LEN);
	victim->used = true;
	return victim;
}

bool send_request(const uint8_t *target_ip) {
	if (!ETH_IsTxBufferAvailable()) {
		return false;
	}
	uint8_t *f = TxDescList.pBuff;
	memcpy(&f[0], BCAST_MAC, MAC_ADDR_LEN);        // Ethernet dst: broadcast
	memcpy(&f[6], MAC_ADDR, MAC_ADDR_LEN);
	f[12] = 0x08; f[13] = 0x06;                    // ethertype ARP
	uint8_t *a = &f[ETH_HDR_LEN];
	a[0] = 0x00; a[1] = 0x01;                      // HTYPE Ethernet
	a[2] = 0x08; a[3] = 0x00;                      // PTYPE IPv4
	a[4] = MAC_ADDR_LEN; a[5] = IP_ADDR_LEN;
	a[6] = 0x00; a[7] = 0x01;                      // request
	memcpy(&a[8], MAC_ADDR, MAC_ADDR_LEN);         // sender MAC
	memcpy(&a[14], IP_ADDR, IP_ADDR_LEN);          // sender IP
	memset(&a[18], 0, MAC_ADDR_LEN);               // target MAC: unknown
	memcpy(&a[24], target_ip, IP_ADDR_LEN);        // target IP
	return ETH_SendFrame(ARP_FRAME_LEN);
}

} // namespace

bool NET_ARP_NextHop(const uint8_t *dst_ip, uint8_t *next_hop) {
	bool on_link = true;
	for (uint8_t i = 0; i < IP_ADDR_LEN; ++i) {
		if ((dst_ip[i] & NET_NETMASK[i]) != (IP_ADDR[i] & NET_NETMASK[i])) {
			on_link = false;
			break;
		}
	}
	if (on_link) {
		memcpy(next_hop, dst_ip, IP_ADDR_LEN);
		return true;
	}
	if (ip_is_zero(NET_GATEWAY)) {
		return false;   // off-link and no router
	}
	memcpy(next_hop, NET_GATEWAY, IP_ADDR_LEN);
	return true;
}

bool NET_ARP_Resolve(const uint8_t *dst_ip, uint8_t *mac) {
	uint8_t hop[IP_ADDR_LEN];
	if (ip_is_zero(IP_ADDR) || !NET_ARP_NextHop(dst_ip, hop)) {
		return false;   // no address yet, or unreachable
	}
	const uint32_t now = get_tick();
	arp_entry_t *e = find(hop);
	if (e != nullptr && e->resolved && (now - e->updated) < NET_ARP_ENTRY_TTL_MS) {
		memcpy(mac, e->mac, MAC_ADDR_LEN);
		return true;
	}
	if (e == nullptr) {
		e = alloc(hop);
		e->updated = now;
		e->requested = now - NET_ARP_RETRY_MS;   // allow an immediate request
	}
	if ((now - e->requested) >= NET_ARP_RETRY_MS) {
		if (send_request(hop)) {
			e->requested = now;
		}
	}
	// A stale but resolved entry is still usable while it is being refreshed
	if (e->resolved) {
		memcpy(mac, e->mac, MAC_ADDR_LEN);
		return true;
	}
	return false;
}

void NET_ARP_Learn(const uint8_t *ip, const uint8_t *mac) {
	if (ip_is_zero(ip) || (mac[0] & 0x01U) != 0U) {
		return;   // ignore probes from 0.0.0.0 and multicast/broadcast senders
	}
	arp_entry_t *e = find(ip);
	if (e == nullptr) {
		e = alloc(ip);
	}
	memcpy(e->mac, mac, MAC_ADDR_LEN);
	e->updated = get_tick();
	e->resolved = true;
}

void NET_ARP_Flush() {
	memset(cache, 0, sizeof(cache));
}
