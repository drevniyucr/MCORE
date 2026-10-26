// Tests for the SNTP client (mcore/net/ntp.cpp). The simulated peer
// CLIENT_IP/CLIENT_MAC plays the NTP server.
#include "doctest.h"

#include <cstring>

#include "net/ntp.hpp"
#include "net/arp.hpp"
#include "net/dns.hpp"
#include "net/net.hpp"
#include "net/eth_utils.hpp"
#include "helpers.hpp"
#include "test_support.hpp"

namespace {

uint8_t MY_IP[4] = { 192, 168, 0, 123 };
uint8_t MASK24[4] = { 255, 255, 255, 0 };
uint8_t NO_IP[4] = { 0, 0, 0, 0 };
char SERVER[] = "192.168.0.10";                 // = CLIENT_IP
constexpr uint32_t DESC4_UDP = ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_UDP;
constexpr uint16_t NTP_POS = ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN;
constexpr uint32_t SERVER_SECS = 0xEA9F3B80;    // 2024-09-26 01:46:40 UTC
constexpr int64_t SERVER_UNIX = 1727315200;

int calls = 0;
int64_t got_unix = 0;
uint16_t got_ms = 0;
void on_time(int64_t unix_s, uint16_t ms) { calls++; got_unix = unix_s; got_ms = ms; }

void ntp_setup(bool server_mac_known = true) {
	test_reset();
	NET_SetIPAddr(MY_IP);
	NET_SetRoute(MASK24, NO_IP, NO_IP);
	NET_ARP_Flush();
	if (server_mac_known) inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);
	g_tx_frames.clear();
	calls = 0;
	NET_NTP_Start(SERVER, NTP_PORT, on_time);
}

const uint8_t* req(const CapturedFrame &f) { return f.data.data() + NTP_POS; }

struct Reply {
	uint8_t li_vn_mode = 0x24;                 // LI 0, v4, server
	uint8_t stratum = 2;
	uint32_t frac = 0x80000000;                // .500 s
	uint16_t src_port = NTP_PORT;
	bool echo = true;                          // put our nonce into the origin field
};

void reply(const CapturedFrame &request, const Reply &r = {}) {
	static uint8_t buf[200];
	const uint16_t l4 = UDP_HDR_LEN + NTP_PACKET_LEN;
	uint8_t *udp = build_ipv4(buf, IP_PROTO_UDP, l4);
	udp[0] = static_cast<uint8_t>(r.src_port >> 8);
	udp[1] = static_cast<uint8_t>(r.src_port & 0xFF);
	udp[2] = static_cast<uint8_t>(NET_NTP_LocalPort() >> 8);
	udp[3] = static_cast<uint8_t>(NET_NTP_LocalPort() & 0xFF);
	udp[5] = l4;
	uint8_t *p = udp + UDP_HDR_LEN;
	p[0] = r.li_vn_mode;
	p[1] = r.stratum;
	if (r.echo) memcpy(&p[24], req(request) + 40, 8);   // origin = our transmit
	U32toBuffU8(&p[32], SERVER_SECS);                     // receive
	U32toBuffU8(&p[40], SERVER_SECS);                     // transmit
	U32toBuffU8(&p[44], r.frac);
	process_rx(buf, static_cast<uint16_t>(ETH_HDR_LEN + IP_HDR_LEN + l4), DESC4_UDP);
}

uint16_t rd16(const uint8_t *p) { return static_cast<uint16_t>((p[0] << 8) | p[1]); }

// Long backoffs outlive the ARP entry (5 min): refresh it so the next poll
// sends NTP, not an ARP request
void poll_fresh() {
	inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);
	NET_NTP_Poll();
}

} // namespace

TEST_CASE("NTP: client-mode v4 request with a random transmit timestamp") {
	ntp_setup();
	NET_NTP_Poll();
	CHECK(ntp_client.state == ntp_state_t::WAITING);
	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];
	CHECK(f.data.size() == NTP_POS + NTP_PACKET_LEN);
	CHECK(memcmp(&f.data[0], CLIENT_MAC, 6) == 0);
	CHECK(memcmp(&f.data[DST_ADDR_POS], CLIENT_IP, 4) == 0);
	CHECK(rd16(&f.data[UDP_HDR_POS + 2]) == NTP_PORT);
	CHECK(rd16(&f.data[UDP_HDR_POS]) == NET_NTP_LocalPort());
	CHECK(NET_NTP_LocalPort() >= 49152);
	CHECK(req(f)[0] == 0x23);                      // LI 0, VN 4, mode 3
	CHECK(BuffU8ToU32(req(f) + 40) == ntp_client.nonce[0]);
	CHECK(BuffU8ToU32(req(f) + 44) == ntp_client.nonce[1]);
}

TEST_CASE("NTP: a valid reply delivers server time + half the round trip") {
	ntp_setup();
	NET_NTP_Poll();
	g_test_now += 40;                              // 40 ms round trip
	reply(g_tx_frames[0]);
	REQUIRE(calls == 1);
	CHECK(got_unix == SERVER_UNIX);
	CHECK(got_ms == 500 + 20);
	CHECK(ntp_client.state == ntp_state_t::SYNCED);
	CHECK(ntp_client.rtt_ms == 40);
	CHECK(ntp_client.stratum == 2);
	CHECK(ntp_client.syncs == 1);
}

TEST_CASE("NTP: fraction + half round trip carries into the seconds") {
	ntp_setup();
	NET_NTP_Poll();
	g_test_now += 200;
	Reply r;
	r.frac = 0xF0000000;                           // .9375 s -> 937 ms + 100 ms
	reply(g_tx_frames[0], r);
	CHECK(got_unix == SERVER_UNIX + 1);
	CHECK(got_ms == 37);
}

TEST_CASE("NTP: replies not echoing our nonce, from another port, or in client mode are ignored") {
	ntp_setup();
	NET_NTP_Poll();
	Reply wrong_origin; wrong_origin.echo = false;
	Reply wrong_port; wrong_port.src_port = 5000;
	Reply wrong_mode; wrong_mode.li_vn_mode = 0x23;
	reply(g_tx_frames[0], wrong_origin);
	reply(g_tx_frames[0], wrong_port);
	reply(g_tx_frames[0], wrong_mode);
	CHECK(calls == 0);
	CHECK(ntp_client.state == ntp_state_t::WAITING);
	reply(g_tx_frames[0]);
	CHECK(calls == 1);
}

TEST_CASE("NTP: kiss-of-death and unsynchronised servers fail the attempt, retried later") {
	ntp_setup();
	NET_NTP_Poll();
	Reply kod; kod.stratum = 0;
	reply(g_tx_frames[0], kod);
	CHECK(calls == 0);
	CHECK(ntp_client.state == ntp_state_t::FAILED);
	CHECK(ntp_client.failures == 1);

	g_test_now += NTP_FAIL_BACKOFF_MS;
	NET_NTP_Poll();
	REQUIRE(g_tx_frames.size() == 2);              // new attempt after the backoff
	Reply alarm; alarm.li_vn_mode = 0xE4;          // LI 3 = not synchronised
	reply(g_tx_frames[1], alarm);
	CHECK(ntp_client.state == ntp_state_t::FAILED);
	CHECK(calls == 0);
}

TEST_CASE("NTP: retries with a fresh nonce, then backs off") {
	ntp_setup();
	NET_NTP_Poll();
	const uint32_t first = ntp_client.nonce[1];
	for (uint8_t i = 1; i < NTP_MAX_TRIES; i++) {
		g_test_now += NTP_RETRY_MS;
		NET_NTP_Poll();
		REQUIRE(g_tx_frames.size() == i + 1u);
	}
	CHECK(ntp_client.nonce[1] != first);           // a late reply to try 1 no longer matches
	g_test_now += NTP_RETRY_MS;
	NET_NTP_Poll();
	CHECK(ntp_client.state == ntp_state_t::FAILED);
	CHECK(g_tx_frames.size() == NTP_MAX_TRIES);
}

TEST_CASE("NTP: the backoff doubles per failure in a row, resets after a success") {
	ntp_setup();
	Reply kod; kod.stratum = 0;
	uint32_t expect = NTP_FAIL_BACKOFF_MS;
	for (int i = 0; i < 3; i++) {
		poll_fresh();
		reply(g_tx_frames.back(), kod);
		CHECK(ntp_client.next_at - g_test_now == expect);
		g_test_now = ntp_client.next_at;
		expect *= 2;
	}
	poll_fresh();
	reply(g_tx_frames.back());                     // success
	CHECK(ntp_client.fail_streak == 0);
	g_test_now = ntp_client.next_at;
	poll_fresh();
	reply(g_tx_frames.back(), kod);
	CHECK(ntp_client.next_at - g_test_now == NTP_FAIL_BACKOFF_MS);
}

TEST_CASE("NTP: the backoff is capped") {
	ntp_setup();
	Reply kod; kod.stratum = 0;
	for (int i = 0; i < 12; i++) {
		poll_fresh();
		reply(g_tx_frames.back(), kod);
		CHECK(ntp_client.next_at - g_test_now <= NTP_FAIL_BACKOFF_MAX_MS);
		g_test_now = ntp_client.next_at;
	}
	CHECK(ntp_client.next_at - (g_test_now - NTP_FAIL_BACKOFF_MAX_MS) == NTP_FAIL_BACKOFF_MAX_MS);
}

TEST_CASE("NTP: resyncs after NTP_RESYNC_MS") {
	ntp_setup();
	NET_NTP_Poll();
	reply(g_tx_frames[0]);
	g_test_now += NTP_RESYNC_MS - 1;
	NET_NTP_Poll();
	CHECK(g_tx_frames.size() == 1);
	g_test_now += 1;
	NET_NTP_Poll();
	CHECK(g_tx_frames.size() == 2);
	CHECK(ntp_client.state == ntp_state_t::WAITING);
}

TEST_CASE("NTP: waits for the server's MAC before sending") {
	ntp_setup(/*server_mac_known=*/false);
	NET_NTP_Poll();
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(g_tx_frames[0].data[13] == 0x06);        // ARP request first
	inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);
	NET_NTP_Poll();
	REQUIRE(g_tx_frames.size() == 2);
	CHECK(rd16(&g_tx_frames[1].data[UDP_HDR_POS + 2]) == NTP_PORT);
}

TEST_CASE("NTP: era rule around 2036") {
	CHECK(ntp_to_unix(0xFFFFFFFFU) == 2085978495);  // last second of era 0
	CHECK(ntp_to_unix(0x80000000U) == 2085978496 - 0x80000000LL);  // 1968 still era 0
	CHECK(ntp_to_unix(0x7FFFFFFFU) == 2085978496 + 0x7FFFFFFFLL);  // era 1, 2104
}
