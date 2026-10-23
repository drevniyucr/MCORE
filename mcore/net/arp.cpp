/*
 * arp.cpp — ARP cache, next-hop routing, ARP request/reply
 */
#include <cstring>

#include "net/arp.hpp"
#include "net/net.hpp"
#include "drivers/eth.hpp"
#include "core/system.hpp"

arp_entry_t arp_cache[ARP_CACHE_SIZE] = { };

namespace {

constexpr uint16_t ARP_OP_REQUEST = 1;
constexpr uint16_t ARP_OP_REPLY = 2;

// Offsets inside the ARP payload (after the 14-byte Ethernet header)
constexpr uint16_t ARP_OPCODE_POS = ETH_HDR_LEN + 6;
constexpr uint16_t ARP_SHA_POS = ETH_HDR_LEN + 8;    // sender MAC
constexpr uint16_t ARP_SPA_POS = ETH_HDR_LEN + 14;   // sender IP
constexpr uint16_t ARP_THA_POS = ETH_HDR_LEN + 18;   // target MAC
constexpr uint16_t ARP_TPA_POS = ETH_HDR_LEN + 24;   // target IP

const uint8_t BCAST_MAC[MAC_ADDR_LEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

bool is_zero(const uint8_t *ip) {
	return ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0;
}

bool is_broadcast(const uint8_t *ip) {
	bool limited = true;
	bool directed = true;   // host part all ones on our subnet
	for (uint8_t i = 0; i < IP_ADDR_LEN; i++) {
		limited &= ip[i] == 0xFF;
		directed &= (ip[i] & NET_MASK[i]) == (IP_ADDR[i] & NET_MASK[i])
		         && (ip[i] | NET_MASK[i]) == 0xFF;
	}
	return limited || (directed && !is_zero(NET_MASK));
}

arp_entry_t* find(const uint8_t *ip) {
	for (auto &e : arp_cache) {
		if (e.state != arp_state_t::FREE && memcmp(e.ip, ip, IP_ADDR_LEN) == 0) {
			return &e;
		}
	}
	return nullptr;
}

// A free slot, else the oldest resolved/failed one. Pending entries are
// never evicted: someone is waiting on them.
arp_entry_t* alloc(const uint8_t *ip) {
	arp_entry_t *victim = nullptr;
	for (auto &e : arp_cache) {
		if (e.state == arp_state_t::FREE) {
			victim = &e;
			break;
		}
		if (e.state != arp_state_t::PENDING &&
		    (victim == nullptr || get_tick() - e.stamp > get_tick() - victim->stamp)) {
			victim = &e;
		}
	}
	if (victim != nullptr) {
		memset(victim, 0, sizeof(*victim));
		memcpy(victim->ip, ip, IP_ADDR_LEN);
	}
	return victim;
}

void build_arp(uint8_t *buf, uint16_t op, const uint8_t *dst_mac,
		const uint8_t *target_mac, const uint8_t *target_ip) {
	memset(buf, 0, ARP_FRAME_LEN);
	memcpy(buf, dst_mac, MAC_ADDR_LEN);
	memcpy(&buf[6], MAC_ADDR, MAC_ADDR_LEN);
	buf[12] = static_cast<uint8_t>(ETH_TYPE_ARP >> 8);
	buf[13] = static_cast<uint8_t>(ETH_TYPE_ARP & 0xFF);

	uint8_t *a = &buf[ETH_HDR_LEN];
	a[1] = 1;                              // htype: Ethernet
	a[2] = 0x08;                           // ptype: IPv4
	a[4] = MAC_ADDR_LEN;
	a[5] = IP_ADDR_LEN;
	a[7] = static_cast<uint8_t>(op);
	memcpy(&buf[ARP_SHA_POS], MAC_ADDR, MAC_ADDR_LEN);
	memcpy(&buf[ARP_SPA_POS], IP_ADDR, IP_ADDR_LEN);
	memcpy(&buf[ARP_THA_POS], target_mac, MAC_ADDR_LEN);
	memcpy(&buf[ARP_TPA_POS], target_ip, IP_ADDR_LEN);
}

void send_request(arp_entry_t &e) {
	static const uint8_t ZERO_MAC[MAC_ADDR_LEN] = { };
	build_arp(TxDescList.pBuff, ARP_OP_REQUEST, BCAST_MAC, ZERO_MAC, e.ip);
	ETH_SendFrame(ARP_FRAME_LEN);  // the MAC pads it to 60 (DP not set)
	e.stamp = get_tick();
}

} // namespace


bool NET_NextHop(const uint8_t *dst, uint8_t *hop) {
	bool local = true;
	for (uint8_t i = 0; i < IP_ADDR_LEN; i++) {
		local &= (dst[i] & NET_MASK[i]) == (IP_ADDR[i] & NET_MASK[i]);
	}
	if (local) {
		memcpy(hop, dst, IP_ADDR_LEN);
		return true;
	}
	if (is_zero(NET_GATEWAY)) {
		return false;
	}
	memcpy(hop, NET_GATEWAY, IP_ADDR_LEN);
	return true;
}

net_result_t NET_ARP_Resolve(const uint8_t *dst, uint8_t *mac_out) {
	if (is_broadcast(dst)) {
		memcpy(mac_out, BCAST_MAC, MAC_ADDR_LEN);
		return net_result_t::OK;
	}
	uint8_t hop[IP_ADDR_LEN];
	if (is_zero(IP_ADDR) || !NET_NextHop(dst, hop)) {
		return net_result_t::UNREACHABLE;
	}

	arp_entry_t *e = find(hop);
	if (e != nullptr) {
		switch (e->state) {
		case arp_state_t::RESOLVED:
			if (get_tick() - e->stamp < ARP_ENTRY_TTL_MS) {
				memcpy(mac_out, e->mac, MAC_ADDR_LEN);
				return net_result_t::OK;
			}
			// stale: ask again, the old MAC may belong to a replaced device
			e->state = arp_state_t::PENDING;
			e->retries = 0;
			send_request(*e);
			return net_result_t::PENDING;
		case arp_state_t::FAILED:
			return net_result_t::UNREACHABLE;
		default:
			return net_result_t::PENDING;
		}
	}

	e = alloc(hop);
	if (e == nullptr) {
		return net_result_t::PENDING;  // every slot is mid-resolution: try later
	}
	e->state = arp_state_t::PENDING;
	send_request(*e);
	return net_result_t::PENDING;
}

void NET_ProcessARP(ipv4_frame *frame) {
	const uint8_t *rx = frame->rx_buff;
	if (frame->frame_len < ARP_FRAME_LEN) {
		return;
	}
	// Ethernet / IPv4 / 6-byte MAC / 4-byte IP only
	if (rx[ETH_HDR_LEN] != 0 || rx[ETH_HDR_LEN + 1] != 1 || rx[ETH_HDR_LEN + 2] != 0x08 ||
	    rx[ETH_HDR_LEN + 3] != 0x00 || rx[ETH_HDR_LEN + 4] != MAC_ADDR_LEN ||
	    rx[ETH_HDR_LEN + 5] != IP_ADDR_LEN) {
		return;
	}
	const uint16_t op = static_cast<uint16_t>((rx[ARP_OPCODE_POS] << 8) | rx[ARP_OPCODE_POS + 1]);
	const uint8_t *sha = &rx[ARP_SHA_POS];
	const uint8_t *spa = &rx[ARP_SPA_POS];
	const uint8_t *tpa = &rx[ARP_TPA_POS];
	const bool for_us = !is_zero(IP_ADDR) && memcmp(tpa, IP_ADDR, IP_ADDR_LEN) == 0;

	// RFC 826 merge: refresh a known sender from ANY ARP packet; add a new
	// one only when it talks to us (request for our IP, or reply to ours).
	// 0.0.0.0 senders are address probes (RFC 5227), not hosts.
	if (!is_zero(spa)) {
		arp_entry_t *e = find(spa);
		if (e == nullptr && for_us) {
			e = alloc(spa);
		}
		if (e != nullptr) {
			memcpy(e->mac, sha, MAC_ADDR_LEN);
			e->state = arp_state_t::RESOLVED;
			e->retries = 0;
			e->stamp = get_tick();
		}
	}

	if (op == ARP_OP_REQUEST && for_us) {
		build_arp(*frame->tx_buff, ARP_OP_REPLY, sha, sha, spa);
		ETH_SendFrame(ARP_FRAME_LEN);  // the MAC pads it to 60 (DP not set)
	}
}

void NET_ARP_Poll() {
	const uint32_t now = get_tick();
	for (auto &e : arp_cache) {
		switch (e.state) {
		case arp_state_t::PENDING:
			if (now - e.stamp >= ARP_RETRY_MS) {
				if (++e.retries >= ARP_MAX_RETRIES) {
					e.state = arp_state_t::FAILED;
					e.stamp = now;
				} else {
					send_request(e);
				}
			}
			break;
		case arp_state_t::FAILED:
			if (now - e.stamp >= ARP_FAIL_HOLD_MS) {
				e.state = arp_state_t::FREE;
			}
			break;
		case arp_state_t::RESOLVED:
			if (now - e.stamp >= ARP_ENTRY_TTL_MS) {
				e.state = arp_state_t::FREE;
			}
			break;
		default:
			break;
		}
	}
}

void NET_ARP_Flush() {
	memset(arp_cache, 0, sizeof(arp_cache));
}
