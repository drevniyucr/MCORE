// Tests for the DHCP client (mcore/net/dhcp.cpp): full lifecycle driven
// through NET_ProcessRx with fabricated server replies, time controlled
// via the get_tick() stub.
#include "doctest.h"

#include <cstring>

#include "net/dhcp.hpp"
#include "net/net.hpp"
#include "net/eth_utils.hpp"
#include "drivers/eth.hpp"
#include "helpers.hpp"
#include "test_support.hpp"

namespace {

uint8_t STATIC_IP[4] = { 192, 168, 0, 123 };
uint8_t SERVER_MAC[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
uint8_t SERVER_IP[4] = { 192, 168, 0, 1 };
uint8_t YIADDR[4] = { 192, 168, 0, 50 };

constexpr uint32_t DESC4_UDP =
		ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_UDP;
constexpr uint16_t BOOTP_POS = 42;
constexpr uint16_t OPTS_POS = BOOTP_POS + 240;
constexpr uint32_t LEASE_S = 600;   // -> T1 300 s, T2 525 s

// Restore the static address and start a fresh DHCP exchange
void dhcp_setup() {
	test_reset();
	NET_SetIPAddr(STATIC_IP);
	NET_DHCP_Start();
}

// Fabricate a server BOOTREPLY (broadcast) and feed it through the RX path
void send_server_reply(uint8_t msg_type, uint32_t xid, const uint8_t *yiaddr,
		uint32_t lease = LEASE_S) {
	static uint8_t f[600];
	memset(f, 0, sizeof(f));

	const uint16_t bootp_len = 240 + 64;
	const uint16_t ip_total = 20 + 8 + bootp_len;

	memset(f, 0xFF, 6);                            // eth dst = broadcast
	memcpy(&f[6], SERVER_MAC, 6);
	f[12] = 0x08;
	f[13] = 0x00;

	uint8_t *ip = &f[14];
	ip[0] = 0x45;
	ip[2] = static_cast<uint8_t>(ip_total >> 8);
	ip[3] = static_cast<uint8_t>(ip_total & 0xFF);
	ip[8] = 64;
	ip[9] = 17;
	memcpy(&ip[12], SERVER_IP, 4);
	memset(&ip[16], 255, 4);                       // ip dst = broadcast

	uint8_t *udp = &f[34];
	udp[1] = 67;
	udp[3] = 68;
	const uint16_t ulen = 8 + bootp_len;
	udp[4] = static_cast<uint8_t>(ulen >> 8);
	udp[5] = static_cast<uint8_t>(ulen & 0xFF);

	uint8_t *b = &f[BOOTP_POS];
	b[0] = 2;                                      // BOOTREPLY
	b[1] = 1;
	b[2] = 6;
	U32toBuffU8(&b[4], xid);
	memcpy(&b[16], yiaddr, 4);                     // yiaddr
	memcpy(&b[28], MAC_ADDR, 6);                   // chaddr
	U32toBuffU8(&b[236], 0x63825363);

	uint8_t *o = &b[240];
	*o++ = 53; *o++ = 1; *o++ = msg_type;
	*o++ = 54; *o++ = 4; memcpy(o, SERVER_IP, 4); o += 4;
	*o++ = 51; *o++ = 4; U32toBuffU8(o, lease); o += 4;
	*o++ = 1;  *o++ = 4; o[0] = 255; o[1] = 255; o[2] = 255; o[3] = 0; o += 4;
	*o++ = 3;  *o++ = 4; memcpy(o, SERVER_IP, 4); o += 4;
	*o = 255;

	process_rx(f, 14 + ip_total, DESC4_UDP);
}

// Find an option in a captured client frame; nullptr if absent
const uint8_t* client_opt(const CapturedFrame &fr, uint8_t code) {
	const uint8_t *o = fr.data.data() + OPTS_POS;
	const uint8_t *end = fr.data.data() + fr.data.size();
	while (o < end && *o != 255) {
		if (*o == 0) { o++; continue; }
		if (o + 2 + o[1] > end) break;
		if (*o == code) return o + 2;
		o += 2 + o[1];
	}
	return nullptr;
}

// Full DISCOVER/OFFER/REQUEST/ACK exchange -> BOUND on YIADDR
void do_bind() {
	send_server_reply(2 /*OFFER*/, TEST_ISN, YIADDR);
	send_server_reply(5 /*ACK*/, TEST_ISN, YIADDR);
	REQUIRE(dhcp_client.state == dhcp_state_t::DHCP_BOUND);
}

} // namespace

TEST_CASE("DHCP: Start drops the IP and broadcasts a valid DISCOVER") {
	dhcp_setup();

	// Address is gone while acquiring, static saved as fallback
	const uint8_t zero[4] = { 0, 0, 0, 0 };
	CHECK(memcmp(IP_ADDR, zero, 4) == 0);
	CHECK(memcmp(dhcp_client.static_ip, STATIC_IP, 4) == 0);
	CHECK(dhcp_client.state == dhcp_state_t::DHCP_SELECTING);

	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	REQUIRE(f.data.size() == 42 + 300);
	// Ethernet/IP broadcast, src IP 0.0.0.0
	for (int i = 0; i < 6; i++) CHECK(f.data[i] == 0xFF);
	CHECK(memcmp(&f.data[6], MAC_ADDR, 6) == 0);
	CHECK(memcmp(&f.data[26], zero, 4) == 0);
	for (int i = 30; i < 34; i++) CHECK(f.data[i] == 255);
	CHECK(checksum(&f.data[14], 20) == 0);          // valid IP checksum
	// UDP 68 -> 67
	CHECK(f.data[35] == 68);
	CHECK(f.data[37] == 67);
	// BOOTP
	CHECK(f.data[BOOTP_POS] == 1);                  // BOOTREQUEST
	CHECK(BuffU8ToU32(const_cast<uint8_t*>(&f.data[BOOTP_POS + 4])) == TEST_ISN);
	CHECK(f.data[BOOTP_POS + 10] == 0x80);          // BROADCAST flag
	CHECK(memcmp(&f.data[BOOTP_POS + 28], MAC_ADDR, 6) == 0);
	// Options: message type DISCOVER, parameter list, no 50/54
	const uint8_t *t = client_opt(f, 53);
	REQUIRE(t != nullptr);
	CHECK(*t == 1);
	CHECK(client_opt(f, 55) != nullptr);
	CHECK(client_opt(f, 50) == nullptr);
	CHECK(client_opt(f, 54) == nullptr);
}

TEST_CASE("DHCP: OFFER produces a REQUEST naming the offer and server") {
	dhcp_setup();
	g_tx_frames.clear();

	send_server_reply(2 /*OFFER*/, TEST_ISN, YIADDR);

	CHECK(dhcp_client.state == dhcp_state_t::DHCP_REQUESTING);
	CHECK(memcmp(dhcp_client.offered_ip, YIADDR, 4) == 0);
	CHECK(memcmp(dhcp_client.server_mac, SERVER_MAC, 6) == 0);

	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	const uint8_t *t = client_opt(f, 53);
	REQUIRE(t != nullptr);
	CHECK(*t == 3);                                 // REQUEST
	const uint8_t *req_ip = client_opt(f, 50);
	REQUIRE(req_ip != nullptr);
	CHECK(memcmp(req_ip, YIADDR, 4) == 0);
	const uint8_t *sid = client_opt(f, 54);
	REQUIRE(sid != nullptr);
	CHECK(memcmp(sid, SERVER_IP, 4) == 0);
}

TEST_CASE("DHCP: OFFER with a foreign xid is ignored") {
	dhcp_setup();
	g_tx_frames.clear();

	send_server_reply(2, TEST_ISN ^ 0xDEAD, YIADDR);

	CHECK(dhcp_client.state == dhcp_state_t::DHCP_SELECTING);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("DHCP: ACK binds the lease, applies IP and patches templates") {
	dhcp_setup();
	do_bind();

	CHECK(memcmp(IP_ADDR, YIADDR, 4) == 0);
	// Frame templates must carry the new source IP
	CHECK(memcmp(&UDPsendFrameTemplate[26], YIADDR, 4) == 0);
	CHECK(memcmp(&TCPsendFrameTemplate[26], YIADDR, 4) == 0);
	// Options applied
	const uint8_t mask[4] = { 255, 255, 255, 0 };
	CHECK(memcmp(dhcp_client.mask, mask, 4) == 0);
	CHECK(memcmp(dhcp_client.gw, SERVER_IP, 4) == 0);
	CHECK(dhcp_client.lease_time == LEASE_S);
	CHECK(dhcp_client.t1 == LEASE_S / 2);
	CHECK(dhcp_client.t2 == LEASE_S - LEASE_S / 8);
}

TEST_CASE("DHCP: NAK restarts discovery from scratch") {
	dhcp_setup();
	send_server_reply(2, TEST_ISN, YIADDR);         // -> REQUESTING
	g_tx_frames.clear();

	send_server_reply(6 /*NAK*/, TEST_ISN, YIADDR);

	CHECK(dhcp_client.state == dhcp_state_t::DHCP_SELECTING);
	const uint8_t zero[4] = { 0, 0, 0, 0 };
	CHECK(memcmp(IP_ADDR, zero, 4) == 0);
	REQUIRE(g_tx_frames.size() == 1);               // fresh DISCOVER
	const uint8_t *t = client_opt(g_tx_frames[0], 53);
	REQUIRE(t != nullptr);
	CHECK(*t == 1);
}

TEST_CASE("DHCP: exponential backoff, then fallback to the static IP") {
	dhcp_setup();                                   // DISCOVER #1 at t=1000
	g_tx_frames.clear();

	// Retries at +2s, +4s, +8s (4 DISCOVERs total), then fallback at +16s
	for (int i = 0; i < 3; i++) {
		g_test_now += (DHCP_RETRY_BASE_MS << i) - 100;
		NET_DHCP_Poll();
		CHECK(g_tx_frames.size() == static_cast<size_t>(i));  // not yet
		g_test_now += 100;
		NET_DHCP_Poll();
		CHECK(g_tx_frames.size() == static_cast<size_t>(i + 1));
	}
	CHECK(dhcp_client.state == dhcp_state_t::DHCP_SELECTING);

	g_test_now += DHCP_RETRY_BASE_MS << 3;
	NET_DHCP_Poll();
	CHECK(dhcp_client.state == dhcp_state_t::DHCP_FALLBACK);
	CHECK(memcmp(IP_ADDR, STATIC_IP, 4) == 0);      // reachable again

	// Background retry after the fallback period
	g_tx_frames.clear();
	g_test_now += DHCP_FALLBACK_RETRY_MS;
	NET_DHCP_Poll();
	CHECK(dhcp_client.state == dhcp_state_t::DHCP_SELECTING);
	CHECK(g_tx_frames.size() == 1);
	CHECK(memcmp(IP_ADDR, STATIC_IP, 4) == 0);      // static stays while retrying
}

TEST_CASE("DHCP renew: T1 sends unicast REQUEST to the leasing server") {
	dhcp_setup();
	do_bind();
	g_tx_frames.clear();

	g_test_now += dhcp_client.t1 * 1000;
	NET_DHCP_Poll();

	CHECK(dhcp_client.state == dhcp_state_t::DHCP_RENEWING);
	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	CHECK(memcmp(&f.data[0], SERVER_MAC, 6) == 0);  // unicast eth dst
	CHECK(memcmp(&f.data[30], SERVER_IP, 4) == 0);  // unicast ip dst
	CHECK(memcmp(&f.data[26], YIADDR, 4) == 0);     // src = our leased IP
	CHECK(memcmp(&f.data[BOOTP_POS + 12], YIADDR, 4) == 0);  // ciaddr
	CHECK(f.data[BOOTP_POS + 10] == 0);             // no BROADCAST flag
	// RFC 2131: RENEW carries no option 50/54
	CHECK(client_opt(f, 50) == nullptr);
	CHECK(client_opt(f, 54) == nullptr);

	// Server ACKs -> back to BOUND with a fresh lease
	send_server_reply(5, TEST_ISN, YIADDR);
	CHECK(dhcp_client.state == dhcp_state_t::DHCP_BOUND);
	CHECK(memcmp(IP_ADDR, YIADDR, 4) == 0);
}

TEST_CASE("DHCP rebind: T2 falls back to broadcast REQUEST") {
	dhcp_setup();
	do_bind();
	g_tx_frames.clear();

	g_test_now += dhcp_client.t1 * 1000;
	NET_DHCP_Poll();                                // -> RENEWING
	g_tx_frames.clear();

	g_test_now += (dhcp_client.t2 - dhcp_client.t1) * 1000;
	NET_DHCP_Poll();

	CHECK(dhcp_client.state == dhcp_state_t::DHCP_REBINDING);
	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	for (int i = 0; i < 6; i++) CHECK(f.data[i] == 0xFF);  // broadcast again
	CHECK(memcmp(&f.data[BOOTP_POS + 12], YIADDR, 4) == 0);  // ciaddr kept
}

TEST_CASE("DHCP expiry: lease runs out, address dropped, discovery restarts") {
	dhcp_setup();
	do_bind();

	g_test_now += dhcp_client.t1 * 1000;
	NET_DHCP_Poll();                                // RENEWING
	g_test_now += (dhcp_client.t2 - dhcp_client.t1) * 1000;
	NET_DHCP_Poll();                                // REBINDING
	g_tx_frames.clear();

	g_test_now += (dhcp_client.lease_time - dhcp_client.t2) * 1000;
	NET_DHCP_Poll();

	CHECK(dhcp_client.state == dhcp_state_t::DHCP_SELECTING);
	const uint8_t zero[4] = { 0, 0, 0, 0 };
	CHECK(memcmp(IP_ADDR, zero, 4) == 0);
	REQUIRE(g_tx_frames.size() == 1);
	const uint8_t *t = client_opt(g_tx_frames[0], 53);
	REQUIRE(t != nullptr);
	CHECK(*t == 1);                                 // DISCOVER
}

TEST_CASE("DHCP: broadcast UDP to a foreign port draws no ICMP unreachable") {
	dhcp_setup();
	g_tx_frames.clear();

	// Broadcast datagram to a port nobody owns
	static uint8_t f[100];
	memset(f, 0, sizeof(f));
	memset(f, 0xFF, 6);
	memcpy(&f[6], SERVER_MAC, 6);
	f[12] = 0x08; f[13] = 0x00;
	uint8_t *ip = &f[14];
	ip[0] = 0x45;
	ip[2] = 0; ip[3] = 20 + 8 + 4;
	ip[8] = 64; ip[9] = 17;
	memcpy(&ip[12], SERVER_IP, 4);
	memset(&ip[16], 255, 4);
	uint8_t *udp = &f[34];
	udp[3] = 99;                                    // random port
	udp[5] = 8 + 4;
	process_rx(f, 14 + 20 + 8 + 4, DESC4_UDP);

	CHECK(g_tx_frames.empty());
}
