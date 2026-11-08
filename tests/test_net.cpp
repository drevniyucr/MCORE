// Tests for the RX dispatch and ARP/ICMP/UDP handlers (mcore/net/net.cpp),
// driven through NET_ProcessRx with fabricated DMA descriptors.
#include "doctest.h"

#include <cstring>

#include "net/net.hpp"
#include "net/eth_utils.hpp"
#include "drivers/eth.hpp"
#include "helpers.hpp"
#include "test_support.hpp"

namespace {

constexpr uint32_t DESC4_ICMP = ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_ICMP;
constexpr uint32_t DESC4_UDP = ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_UDP;

// 42-byte ARP request: "who has <target_ip>? tell CLIENT_IP"
void build_arp_request(uint8_t *buf, const uint8_t *target_ip, uint16_t opcode = 1) {
	memset(buf, 0, 42);
	memset(buf, 0xFF, 6);                          // dst = broadcast
	memcpy(&buf[6], CLIENT_MAC, 6);                // src
	buf[12] = 0x08; buf[13] = 0x06;                // ethertype ARP
	buf[14] = 0x00; buf[15] = 0x01;                // htype ethernet
	buf[16] = 0x08; buf[17] = 0x00;                // ptype IPv4
	buf[18] = 6; buf[19] = 4;                      // hlen, plen
	buf[20] = static_cast<uint8_t>(opcode >> 8);
	buf[21] = static_cast<uint8_t>(opcode & 0xFF);
	memcpy(&buf[22], CLIENT_MAC, 6);               // sender MAC
	memcpy(&buf[28], CLIENT_IP, 4);                // sender IP
	memcpy(&buf[38], target_ip, 4);                // target IP
}

} // namespace

TEST_CASE("ARP: request for our IP gets a reply") {
	test_reset();
	uint8_t frame[42];
	build_arp_request(frame, IP_ADDR);

	process_rx(frame, sizeof(frame), 0);

	REQUIRE(g_tx_frames.size() == 1);
	const auto &tx = g_tx_frames[0].data;
	REQUIRE(tx.size() == 42);
	CHECK(memcmp(&tx[0], CLIENT_MAC, 6) == 0);     // dst = requester
	CHECK(memcmp(&tx[6], MAC_ADDR, 6) == 0);       // src = us
	CHECK(tx[20] == 0x00);
	CHECK(tx[21] == 0x02);                         // opcode = reply
	CHECK(memcmp(&tx[22], MAC_ADDR, 6) == 0);      // sender MAC = us
	CHECK(memcmp(&tx[28], IP_ADDR, 4) == 0);       // sender IP = us
	CHECK(memcmp(&tx[32], CLIENT_MAC, 6) == 0);    // target MAC = requester
	CHECK(memcmp(&tx[38], CLIENT_IP, 4) == 0);     // target IP = requester
}

TEST_CASE("ARP: request for another IP is ignored") {
	test_reset();
	uint8_t other_ip[4] = { 192, 168, 0, 99 };
	uint8_t frame[42];
	build_arp_request(frame, other_ip);

	process_rx(frame, sizeof(frame), 0);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("ARP: non-request opcode is ignored") {
	test_reset();
	uint8_t frame[42];
	build_arp_request(frame, IP_ADDR, /*opcode=*/2);

	process_rx(frame, sizeof(frame), 0);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("ICMP: echo request gets an echo reply with payload preserved") {
	test_reset();
	uint8_t frame[ETH_MAX_PACKET_SIZE];
	const uint8_t payload[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
	const uint16_t l4_len = 8 + sizeof(payload);

	uint8_t *icmp = build_ipv4(frame, 1 /*ICMP*/, l4_len);
	icmp[0] = 8;                                   // echo request
	icmp[4] = 0x12; icmp[5] = 0x34;                // id
	icmp[6] = 0x00; icmp[7] = 0x01;                // seq
	memcpy(&icmp[8], payload, sizeof(payload));

	process_rx(frame, ETH_HDR_LEN + IP_HDR_LEN + l4_len, DESC4_ICMP);

	REQUIRE(g_tx_frames.size() == 1);
	const auto &tx = g_tx_frames[0].data;
	REQUIRE(tx.size() == ETH_HDR_LEN + IP_HDR_LEN + l4_len);
	CHECK(memcmp(&tx[0], CLIENT_MAC, 6) == 0);           // eth dst = client
	CHECK(memcmp(&tx[6], MAC_ADDR, 6) == 0);             // eth src = us
	CHECK(memcmp(&tx[SCR_ADDR_POS], IP_ADDR, 4) == 0);   // ip src = us
	CHECK(memcmp(&tx[DST_ADDR_POS], CLIENT_IP, 4) == 0); // ip dst = client
	CHECK(tx[34] == 0);                                  // type = echo reply
	CHECK(tx[38] == 0x12);                               // id preserved
	CHECK(tx[39] == 0x34);
	CHECK(memcmp(&tx[42], payload, sizeof(payload)) == 0);
}

TEST_CASE("ICMP: non-echo-request type is ignored") {
	test_reset();
	uint8_t frame[ETH_MAX_PACKET_SIZE];
	uint8_t *icmp = build_ipv4(frame, 1, 8);
	icmp[0] = 0;                                   // echo reply, not request

	process_rx(frame, ETH_HDR_LEN + IP_HDR_LEN + 8, DESC4_ICMP);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("UDP: datagram to the open port is accepted silently") {
	test_reset();
	uint8_t frame[ETH_MAX_PACKET_SIZE];
	const uint16_t l4_len = UDP_HDR_LEN + 4;
	uint8_t *udp = build_ipv4(frame, 17 /*UDP*/, l4_len);
	udp[2] = static_cast<uint8_t>(UDP_DST_PORT >> 8);
	udp[3] = static_cast<uint8_t>(UDP_DST_PORT & 0xFF);
	udp[4] = 0; udp[5] = l4_len;

	process_rx(frame, ETH_HDR_LEN + IP_HDR_LEN + l4_len, DESC4_UDP);
	CHECK(g_tx_frames.empty());                    // no ICMP unreachable
}

TEST_CASE("UDP: datagram to a closed port triggers ICMP port unreachable") {
	test_reset();
	uint8_t frame[ETH_MAX_PACKET_SIZE];
	const uint16_t l4_len = UDP_HDR_LEN + 4;
	uint8_t *udp = build_ipv4(frame, 17, l4_len);
	udp[2] = 0x17; udp[3] = 0x70;                  // port 6000 (not ours)
	udp[4] = 0; udp[5] = l4_len;

	process_rx(frame, ETH_HDR_LEN + IP_HDR_LEN + l4_len, DESC4_UDP);

	REQUIRE(g_tx_frames.size() == 1);
	const auto &tx = g_tx_frames[0].data;
	// 14 (eth) + 20 (ip) + 8 (icmp) + 28 (orig ip hdr + 8 payload bytes)
	REQUIRE(tx.size() == 70);
	CHECK(tx[34] == 3);                            // type = dest unreachable
	CHECK(tx[35] == 3);                            // code = port unreachable
	CHECK(memcmp(&tx[DST_ADDR_POS], CLIENT_IP, 4) == 0);
	// Both checksums must be valid: folding over data incl. checksum == 0
	CHECK(checksum(&tx[IP_HDR_POS], IP_HDR_LEN) == 0);
	CHECK(checksum(&tx[34], 36) == 0);
	// Quoted original datagram starts with the original IP header
	CHECK(tx[42] == 0x45);
}

TEST_CASE("RX dispatch: frame for another IP is dropped") {
	test_reset();
	uint8_t frame[ETH_MAX_PACKET_SIZE];
	uint8_t other_ip[4] = { 192, 168, 0, 99 };
	uint8_t *icmp = build_ipv4(frame, 1, 8, other_ip);
	icmp[0] = 8;

	process_rx(frame, ETH_HDR_LEN + IP_HDR_LEN + 8, DESC4_ICMP);
	CHECK(g_tx_frames.empty());
}

TEST_CASE("RX dispatch: runt and inconsistent frames are dropped") {
	test_reset();
	uint8_t frame[ETH_MAX_PACKET_SIZE] = {};

	// Shorter than an Ethernet header
	process_rx(frame, 10, DESC4_ICMP);
	CHECK(g_tx_frames.empty());

	// IP total length larger than what actually arrived
	uint8_t *icmp = build_ipv4(frame, 1, 100);
	icmp[0] = 8;
	process_rx(frame, ETH_HDR_LEN + IP_HDR_LEN + 8, DESC4_ICMP);
	CHECK(g_tx_frames.empty());

	// Descriptor says "not IPv4"
	build_ipv4(frame, 1, 8)[0] = 8;
	process_rx(frame, ETH_HDR_LEN + IP_HDR_LEN + 8, /*desc4=*/0);
	CHECK(g_tx_frames.empty());
}
