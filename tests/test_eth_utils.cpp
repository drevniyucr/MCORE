// Unit tests for the pure helpers in mcore/net/eth_utils.
// Reference values were computed independently (Python, RFC 1071 method).
#include "doctest.h"

#include <cstring>

#include "net/eth_utils.hpp"

TEST_CASE("checksum: known vectors") {
	// RFC 1071 sample data
	uint8_t v1[] = { 0x00, 0x01, 0xf2, 0x03, 0xf4, 0xf5, 0xf6, 0xf7 };
	CHECK(checksum(v1, sizeof(v1)) == 0x220d);

	// Classic IPv4 header example (checksum field zeroed) -> 0xb861
	uint8_t ip_hdr[] = { 0x45, 0x00, 0x00, 0x73, 0x00, 0x00, 0x40, 0x00,
	                     0x40, 0x11, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01,
	                     0xc0, 0xa8, 0x00, 0xc7 };
	CHECK(checksum(ip_hdr, sizeof(ip_hdr)) == 0xb861);

	// Verification property: checksum over data INCLUDING a correct
	// checksum folds to zero.
	ip_hdr[10] = 0xb8;
	ip_hdr[11] = 0x61;
	CHECK(checksum(ip_hdr, sizeof(ip_hdr)) == 0x0000);
}

TEST_CASE("checksum: odd length") {
	uint8_t v[] = { 0x01, 0x02, 0x03 };
	CHECK(checksum(v, sizeof(v)) == 0xfbfd);
}

TEST_CASE("udp_checksum: pseudo-header vector") {
	uint8_t src[] = { 192, 168, 0, 10 };
	uint8_t dst[] = { 192, 168, 0, 123 };
	// sport 5000, dport 5001, udp_len 12, checksum 0
	uint8_t udp_hdr[] = { 0x13, 0x88, 0x13, 0x89, 0x00, 0x0C, 0x00, 0x00 };
	uint8_t payload[] = { 't', 'e', 's', 't' };
	CHECK(udp_checksum(src, dst, udp_hdr, payload, sizeof(payload)) == 0x6f15);
}

TEST_CASE("tcp_checksum: pseudo-header vector") {
	uint8_t src[] = { 192, 168, 0, 10 };
	uint8_t dst[] = { 192, 168, 0, 123 };
	// sport 5000, dport 80, seq 0x1000, ack 0x2000, offset 5, ACK, win 1024
	uint8_t tcp_hdr[] = { 0x13, 0x88, 0x00, 0x50, 0x00, 0x00, 0x10, 0x00,
	                      0x00, 0x00, 0x20, 0x00, 0x50, 0x10, 0x04, 0x00,
	                      0x00, 0x00, 0x00, 0x00 };
	CHECK(tcp_checksum(src, dst, tcp_hdr, sizeof(tcp_hdr)) == 0xe626);
}

TEST_CASE("byte order helpers") {
	CHECK(bswap16(0x1234) == 0x3412);
	CHECK(bswap32(0x12345678) == 0x78563412);

	uint8_t be[] = { 0x11, 0x22, 0x33, 0x44 };
	CHECK(BuffU8ToU32(be) == 0x11223344);
	CHECK(BuffU8ToU32rev(be) == 0x44332211);

	uint8_t out[4] = {};
	U32toBuffU8(out, 0xAABBCCDD);
	CHECK(memcmp(out, "\xAA\xBB\xCC\xDD", 4) == 0);
}
