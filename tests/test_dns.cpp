// Tests for the DNS stub resolver (mcore/net/dns.cpp). The simulated peer
// CLIENT_IP/CLIENT_MAC plays the DNS server, so build_ipv4() fabricates
// its replies.
#include "doctest.h"

#include <cstring>
#include <string>
#include <vector>

#include "net/dns.hpp"
#include "net/arp.hpp"
#include "net/net.hpp"
#include "net/eth_utils.hpp"
#include "helpers.hpp"
#include "test_support.hpp"

namespace {

uint8_t MY_IP[4] = { 192, 168, 0, 123 };
uint8_t MASK24[4] = { 255, 255, 255, 0 };
uint8_t GW_IP[4] = { 192, 168, 0, 1 };
const uint8_t NTP_IP[4] = { 162, 159, 200, 1 };
constexpr uint32_t DESC4_UDP = ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_UDP;
constexpr uint16_t DNS_POS = ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN;

void dns_setup(bool server_mac_known = true) {
	test_reset();
	NET_SetIPAddr(MY_IP);
	NET_SetRoute(MASK24, GW_IP, CLIENT_IP);        // the DNS server is CLIENT_IP
	NET_ARP_Flush();
	memset(dns_cache, 0, sizeof(dns_cache));
	if (server_mac_known) {
		inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);
	}
	g_tx_frames.clear();
}

// The DNS payload of a captured query frame
const uint8_t* query_of(const CapturedFrame &f) { return f.data.data() + DNS_POS; }
uint16_t rd16(const uint8_t *p) { return static_cast<uint16_t>((p[0] << 8) | p[1]); }

// Server reply: echo the question of `query`, then the given answer records
struct Rr { bool cname; std::vector<uint8_t> rdata; uint32_t ttl; };

void reply(const CapturedFrame &query, uint8_t rcode, const std::vector<Rr> &answers,
		uint16_t src_port = DNS_SERVER_PORT, int id_delta = 0) {
	const uint8_t *q = query_of(query);
	const uint16_t qlen = static_cast<uint16_t>(query.data.size() - DNS_POS);

	std::vector<uint8_t> m(q, q + qlen);           // header + question
	m[0] = static_cast<uint8_t>(m[0] + id_delta);
	m[2] = 0x81;                                   // QR | RD
	m[3] = static_cast<uint8_t>(0x80 | rcode);     // RA | RCODE
	m[7] = static_cast<uint8_t>(answers.size());   // ANCOUNT
	for (const auto &a : answers) {
		m.push_back(0xC0); m.push_back(12);        // name: pointer to the question
		m.push_back(0); m.push_back(a.cname ? 5 : 1);
		m.push_back(0); m.push_back(1);            // class IN
		for (int s = 24; s >= 0; s -= 8) m.push_back(static_cast<uint8_t>(a.ttl >> s));
		m.push_back(0); m.push_back(static_cast<uint8_t>(a.rdata.size()));
		m.insert(m.end(), a.rdata.begin(), a.rdata.end());
	}

	static uint8_t buf[600];
	const uint16_t l4 = static_cast<uint16_t>(UDP_HDR_LEN + m.size());
	uint8_t *udp = build_ipv4(buf, IP_PROTO_UDP, l4);
	udp[0] = static_cast<uint8_t>(src_port >> 8);
	udp[1] = static_cast<uint8_t>(src_port & 0xFF);
	udp[2] = static_cast<uint8_t>(NET_DNS_LocalPort() >> 8);
	udp[3] = static_cast<uint8_t>(NET_DNS_LocalPort() & 0xFF);
	udp[4] = static_cast<uint8_t>(l4 >> 8);
	udp[5] = static_cast<uint8_t>(l4 & 0xFF);
	memcpy(udp + UDP_HDR_LEN, m.data(), m.size());
	process_rx(buf, static_cast<uint16_t>(ETH_HDR_LEN + IP_HDR_LEN + l4), DESC4_UDP);
}

Rr a_record(const uint8_t *ip, uint32_t ttl = 300) { return { false, { ip[0], ip[1], ip[2], ip[3] }, ttl }; }
Rr cname_record() { return { true, { 3, 'f', 'o', 'o', 0xC0, 12 }, 300 }; }

} // namespace

TEST_CASE("DNS: IPv4 literal resolves at once, nothing sent") {
	dns_setup();
	uint8_t ip[4];
	CHECK(NET_DNS_Resolve("10.1.2.3", ip) == net_result_t::OK);
	CHECK(ip[0] == 10);
	CHECK(ip[3] == 3);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("DNS: invalid names and a missing server are UNREACHABLE") {
	dns_setup();
	uint8_t ip[4];
	CHECK(NET_DNS_Resolve("", ip) == net_result_t::UNREACHABLE);
	CHECK(NET_DNS_Resolve("a..b", ip) == net_result_t::UNREACHABLE);
	CHECK(NET_DNS_Resolve(".a", ip) == net_result_t::UNREACHABLE);
	CHECK(NET_DNS_Resolve(std::string(64, 'x').c_str(), ip) == net_result_t::UNREACHABLE);
	CHECK(g_tx_frames.empty());

	const uint8_t none[4] = {};
	NET_SetRoute(MASK24, GW_IP, none);
	CHECK(NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::UNREACHABLE);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("DNS: query is a well-formed RD A/IN question to the server") {
	dns_setup();
	uint8_t ip[4];
	CHECK(NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::PENDING);
	REQUIRE(g_tx_frames.size() == 1);
	const auto &f = g_tx_frames[0];

	CHECK(memcmp(&f.data[0], CLIENT_MAC, 6) == 0);             // to the server's MAC
	CHECK(memcmp(&f.data[DST_ADDR_POS], CLIENT_IP, 4) == 0);   // and IP
	const uint8_t *udp = &f.data[UDP_HDR_POS];
	CHECK(rd16(udp) == NET_DNS_LocalPort());
	CHECK(NET_DNS_LocalPort() >= 49152);
	CHECK(rd16(udp + 2) == DNS_SERVER_PORT);

	const uint8_t *q = query_of(f);
	CHECK(q[2] == 0x01);                                       // RD only
	CHECK(rd16(q + 4) == 1);                                   // QDCOUNT
	const uint8_t qname[] = { 4, 'p', 'o', 'o', 'l', 3, 'n', 't', 'p', 3, 'o', 'r', 'g', 0 };
	CHECK(memcmp(q + 12, qname, sizeof(qname)) == 0);
	CHECK(rd16(q + 12 + sizeof(qname)) == 1);                  // type A
	CHECK(rd16(q + 14 + sizeof(qname)) == 1);                  // class IN
	CHECK(f.data.size() == DNS_POS + 12 + sizeof(qname) + 4);

	// A second call while pending sends nothing new
	CHECK(NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::PENDING);
	CHECK(g_tx_frames.size() == 1);
}

TEST_CASE("DNS: CNAME then A -> the A record, cached for its TTL") {
	dns_setup();
	uint8_t ip[4] = {};
	NET_DNS_Resolve("pool.ntp.org", ip);
	REQUIRE(g_tx_frames.size() == 1);
	reply(g_tx_frames[0], 0, { cname_record(), a_record(NTP_IP, 120) });

	CHECK(NET_DNS_Resolve("POOL.ntp.ORG", ip) == net_result_t::OK);  // case-insensitive
	CHECK(memcmp(ip, NTP_IP, 4) == 0);
	CHECK(g_tx_frames.size() == 1);                            // from the cache

	g_test_now += 120 * 1000;                                  // TTL over: asks again
	CHECK(NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::PENDING);
	CHECK(g_tx_frames.size() == 2);
}

TEST_CASE("DNS: TTL is clamped to [DNS_MIN_TTL_S, DNS_MAX_TTL_S]") {
	dns_setup();
	uint8_t ip[4];
	NET_DNS_Resolve("a.example", ip);
	reply(g_tx_frames.back(), 0, { a_record(NTP_IP, 0) });
	NET_DNS_Resolve("b.example", ip);
	reply(g_tx_frames.back(), 0, { a_record(NTP_IP, 86400) });

	g_test_now += DNS_MIN_TTL_S * 1000 - 1;
	CHECK(NET_DNS_Resolve("a.example", ip) == net_result_t::OK);
	g_test_now += DNS_MAX_TTL_S * 1000;
	CHECK(NET_DNS_Resolve("b.example", ip) == net_result_t::PENDING);
}

TEST_CASE("DNS: NXDOMAIN is negative-cached, then retried") {
	dns_setup();
	uint8_t ip[4];
	NET_DNS_Resolve("nope.invalid", ip);
	reply(g_tx_frames[0], 3 /*NXDOMAIN*/, {});
	CHECK(NET_DNS_Resolve("nope.invalid", ip) == net_result_t::UNREACHABLE);
	CHECK(g_tx_frames.size() == 1);

	g_test_now += DNS_FAIL_HOLD_MS;
	NET_DNS_Poll();
	CHECK(NET_DNS_Resolve("nope.invalid", ip) == net_result_t::PENDING);
	CHECK(g_tx_frames.size() == 2);
}

TEST_CASE("DNS: replies with a wrong id or source port are ignored") {
	dns_setup();
	uint8_t ip[4];
	NET_DNS_Resolve("pool.ntp.org", ip);
	reply(g_tx_frames[0], 0, { a_record(NTP_IP) }, DNS_SERVER_PORT, /*id_delta=*/1);
	reply(g_tx_frames[0], 0, { a_record(NTP_IP) }, /*src_port=*/5353);
	CHECK(NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::PENDING);

	reply(g_tx_frames[0], 0, { a_record(NTP_IP) });            // the real one
	CHECK(NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::OK);
}

TEST_CASE("DNS: an answer without an A record fails the lookup") {
	dns_setup();
	uint8_t ip[4];
	NET_DNS_Resolve("alias.example", ip);
	reply(g_tx_frames[0], 0, { cname_record() });
	CHECK(NET_DNS_Resolve("alias.example", ip) == net_result_t::UNREACHABLE);
}

TEST_CASE("DNS: retransmits with the same id, gives up after DNS_MAX_RETRIES") {
	dns_setup();
	uint8_t ip[4];
	NET_DNS_Resolve("pool.ntp.org", ip);
	const uint16_t id = rd16(query_of(g_tx_frames[0]));

	for (uint8_t i = 1; i < DNS_MAX_RETRIES; i++) {
		g_test_now += DNS_RETRY_MS;
		NET_DNS_Poll();
		REQUIRE(g_tx_frames.size() == i + 1u);
		CHECK(rd16(query_of(g_tx_frames.back())) == id);
	}
	g_test_now += DNS_RETRY_MS;
	NET_DNS_Poll();
	CHECK(g_tx_frames.size() == DNS_MAX_RETRIES);
	CHECK(NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::UNREACHABLE);
}

TEST_CASE("DNS: waits for the server's MAC, then sends the query") {
	dns_setup(/*server_mac_known=*/false);
	uint8_t ip[4];
	CHECK(NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::PENDING);
	REQUIRE(g_tx_frames.size() == 1);
	CHECK(g_tx_frames[0].data[13] == 0x06);                    // an ARP request, not DNS

	inject_arp(2, CLIENT_MAC, CLIENT_IP, MY_IP);
	NET_DNS_Poll();
	REQUIRE(g_tx_frames.size() == 2);
	CHECK(rd16(&g_tx_frames[1].data[UDP_HDR_POS + 2]) == DNS_SERVER_PORT);

	reply(g_tx_frames[1], 0, { a_record(NTP_IP) });
	CHECK(NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::OK);
	CHECK(memcmp(ip, NTP_IP, 4) == 0);
}
