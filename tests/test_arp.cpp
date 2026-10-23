// Tests for the ARP cache and next-hop routing (mcore/net/arp.cpp).
#include "doctest.h"

#include <cstring>

#include "net/arp.hpp"
#include "net/net.hpp"
#include "helpers.hpp"
#include "test_support.hpp"

namespace {

uint8_t MY_IP[4] = { 192, 168, 0, 123 };
uint8_t MASK24[4] = { 255, 255, 255, 0 };
uint8_t GW_IP[4] = { 192, 168, 0, 1 };
uint8_t GW_MAC[6] = { 0x10, 0x20, 0x30, 0x40, 0x50, 0x60 };
uint8_t NO_GW[4] = { 0, 0, 0, 0 };
uint8_t NO_DNS[4] = { 0, 0, 0, 0 };
uint8_t REMOTE_IP[4] = { 8, 8, 8, 8 };
const uint8_t BCAST[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

void arp_setup(const uint8_t *gw = GW_IP) {
	test_reset();
	NET_SetIPAddr(MY_IP);
	NET_SetRoute(MASK24, gw, NO_DNS);
	NET_ARP_Flush();
}

// The captured frame is a well-formed "who has <ip>?" from us
void check_request(const CapturedFrame &f, const uint8_t *ip) {
	REQUIRE(f.data.size() == ARP_FRAME_LEN);
	CHECK(memcmp(&f.data[0], BCAST, 6) == 0);
	CHECK(memcmp(&f.data[6], MAC_ADDR, 6) == 0);
	CHECK(f.data[12] == 0x08);
	CHECK(f.data[13] == 0x06);
	CHECK(f.data[21] == 1);                        // opcode request
	CHECK(memcmp(&f.data[22], MAC_ADDR, 6) == 0);  // sender MAC
	CHECK(memcmp(&f.data[28], MY_IP, 4) == 0);     // sender IP
	CHECK(memcmp(&f.data[38], ip, 4) == 0);        // target IP
}

} // namespace

TEST_CASE("ARP: unknown neighbour -> one request, PENDING until the reply") {
	arp_setup();
	uint8_t mac[6];

	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::PENDING);
	REQUIRE(g_tx_frames.size() == 1);
	check_request(g_tx_frames[0], CLIENT_IP);

	// Asking again does not flood the wire
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::PENDING);
	CHECK(g_tx_frames.size() == 1);

	inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::OK);
	CHECK(memcmp(mac, CLIENT_MAC, 6) == 0);
}

TEST_CASE("ARP: off-subnet destination resolves through the gateway") {
	arp_setup();
	uint8_t mac[6];

	CHECK(NET_ARP_Resolve(REMOTE_IP, mac) == net_result_t::PENDING);
	REQUIRE(g_tx_frames.size() == 1);
	check_request(g_tx_frames[0], GW_IP);          // asks for the gateway, not 8.8.8.8

	inject_arp(2, GW_MAC, GW_IP, MY_IP);
	CHECK(NET_ARP_Resolve(REMOTE_IP, mac) == net_result_t::OK);
	CHECK(memcmp(mac, GW_MAC, 6) == 0);
}

TEST_CASE("ARP: off-subnet without a gateway is UNREACHABLE, nothing sent") {
	arp_setup(NO_GW);
	uint8_t mac[6];
	CHECK(NET_ARP_Resolve(REMOTE_IP, mac) == net_result_t::UNREACHABLE);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("ARP: limited and subnet broadcast map to ff:ff:ff:ff:ff:ff") {
	arp_setup();
	uint8_t mac[6] = {};
	const uint8_t limited[4] = { 255, 255, 255, 255 };
	const uint8_t directed[4] = { 192, 168, 0, 255 };

	CHECK(NET_ARP_Resolve(limited, mac) == net_result_t::OK);
	CHECK(memcmp(mac, BCAST, 6) == 0);
	memset(mac, 0, 6);
	CHECK(NET_ARP_Resolve(directed, mac) == net_result_t::OK);
	CHECK(memcmp(mac, BCAST, 6) == 0);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("ARP: no address yet (DHCP in progress) is UNREACHABLE") {
	arp_setup();
	const uint8_t zero[4] = {};
	NET_SetIPAddr(zero);
	uint8_t mac[6];
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::UNREACHABLE);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("ARP: retries, then negative cache, then a fresh attempt") {
	arp_setup();
	uint8_t mac[6];
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::PENDING);

	// Retransmitted every ARP_RETRY_MS, ARP_MAX_RETRIES requests in total
	for (uint8_t i = 1; i < ARP_MAX_RETRIES; i++) {
		g_test_now += ARP_RETRY_MS;
		NET_ARP_Poll();
		CHECK(g_tx_frames.size() == i + 1u);
	}
	g_test_now += ARP_RETRY_MS;
	NET_ARP_Poll();
	CHECK(g_tx_frames.size() == ARP_MAX_RETRIES);  // gave up, no extra request
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::UNREACHABLE);

	// Hold-down over: the next call starts again
	g_test_now += ARP_FAIL_HOLD_MS;
	NET_ARP_Poll();
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::PENDING);
	CHECK(g_tx_frames.size() == ARP_MAX_RETRIES + 1u);
}

TEST_CASE("ARP: a request for our IP teaches us the requester (and is answered)") {
	arp_setup();
	inject_arp(1, CLIENT_MAC, CLIENT_IP, MY_IP);

	REQUIRE(g_tx_frames.size() == 1);
	const auto &tx = g_tx_frames[0].data;
	CHECK(tx[21] == 2);                            // reply
	CHECK(memcmp(&tx[0], CLIENT_MAC, 6) == 0);
	CHECK(memcmp(&tx[32], CLIENT_MAC, 6) == 0);
	CHECK(memcmp(&tx[38], CLIENT_IP, 4) == 0);

	uint8_t mac[6];
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::OK);  // no request needed
	CHECK(memcmp(mac, CLIENT_MAC, 6) == 0);
	CHECK(g_tx_frames.size() == 1);
}

TEST_CASE("ARP: third-party traffic refreshes known hosts but adds none") {
	arp_setup();
	const uint8_t other[4] = { 192, 168, 0, 77 };
	uint8_t mac[6];

	// Unknown sender asking for someone else: not cached
	inject_arp(1, CLIENT_MAC, CLIENT_IP, other);
	CHECK(g_tx_frames.empty());
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::PENDING);

	// Known (pending) sender: its MAC is taken even from a request not for us
	inject_arp(1, CLIENT_MAC, CLIENT_IP, other);
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::OK);
	CHECK(memcmp(mac, CLIENT_MAC, 6) == 0);
}

TEST_CASE("ARP: address probes from 0.0.0.0 are not cached") {
	arp_setup();
	const uint8_t zero[4] = {};
	inject_arp(1, CLIENT_MAC, zero, MY_IP);
	for (const auto &e : arp_cache) CHECK(e.state == arp_state_t::FREE);
}

TEST_CASE("ARP: entries expire after ARP_ENTRY_TTL_MS and are re-requested") {
	arp_setup();
	inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);   // unsolicited reply to us: cached
	uint8_t mac[6];
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::OK);

	g_test_now += ARP_ENTRY_TTL_MS;
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::PENDING);
	REQUIRE(g_tx_frames.size() == 1);
	check_request(g_tx_frames[0], CLIENT_IP);
}

TEST_CASE("ARP: a full cache evicts the oldest resolved entry") {
	arp_setup();
	uint8_t ip[4] = { 192, 168, 0, 0 };
	uint8_t mac6[6] = { 0x02, 0, 0, 0, 0, 0 };
	for (uint8_t i = 0; i < ARP_CACHE_SIZE; i++) {
		ip[3] = static_cast<uint8_t>(20 + i);
		mac6[5] = i;
		inject_arp(2, mac6, ip, MY_IP);
		g_test_now += 10;
	}
	uint8_t mac[6];
	CHECK(NET_ARP_Resolve(CLIENT_IP, mac) == net_result_t::PENDING);  // took a slot

	// Look, don't Resolve: a lookup of .20 would itself evict the next one
	auto state_of = [](uint8_t last) {
		for (const auto &e : arp_cache) {
			if (e.state != arp_state_t::FREE && e.ip[3] == last) return e.state;
		}
		return arp_state_t::FREE;
	};
	CHECK(state_of(20) == arp_state_t::FREE);      // the oldest one is gone
	CHECK(state_of(21) == arp_state_t::RESOLVED);  // the rest survived
	CHECK(state_of(CLIENT_IP[3]) == arp_state_t::PENDING);
}

TEST_CASE("ARP: changing our address flushes the cache") {
	arp_setup();
	inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);
	const uint8_t new_ip[4] = { 192, 168, 0, 124 };
	NET_SetIPAddr(new_ip);
	for (const auto &e : arp_cache) CHECK(e.state == arp_state_t::FREE);
	NET_SetIPAddr(MY_IP);
}
