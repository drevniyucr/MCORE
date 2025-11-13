/*
 * mcore_eth_utils.cpp
 *
 *  Created on: Sep 24, 2025
 *      Author: AkimovMA
 */
#include "mcore_eth_utils.hpp"

namespace {
	// Optimized checksum calculation using 32-bit accumulator
	// Processes 2 bytes at a time for better performance
	inline uint32_t checksum_accumulate(const uint8_t *data, uint16_t len) {
		uint32_t sum = 0;
		const uint16_t word_count = len / 2;
		
		// Process 16-bit words
		for (uint16_t i = 0; i < word_count; i++) {
			sum += static_cast<uint32_t>((data[i * 2] << 8) | data[i * 2 + 1]);
		}
		
		// Handle odd byte if present
		if (len & 1) {
			sum += static_cast<uint32_t>(data[len - 1] << 8);
		}
		
		return sum;
	}
	
	// Fold 32-bit sum to 16-bit
	inline uint16_t checksum_fold(uint32_t sum) {
		while (sum >> 16) {
			sum = (sum & 0xFFFF) + (sum >> 16);
		}
		return static_cast<uint16_t>(~sum);
	}
}

/**
 * @brief Calculate IP header checksum
 * 
 * Optimized version using 32-bit accumulator and word-aligned processing
 * 
 * @param data Pointer to IP header
 * @param len Length of IP header (must be multiple of 2)
 * @return 16-bit checksum in network byte order
 */
uint16_t checksum(const uint8_t *data, uint16_t len) {
	uint32_t sum = checksum_accumulate(data, len);
	return checksum_fold(sum);
}

/**
 * @brief Calculate UDP checksum
 * 
 * Includes pseudo-IP header, UDP header, and payload
 * 
 * @param src_ip Source IP address (4 bytes)
 * @param dst_ip Destination IP address (4 bytes)
 * @param udp_hdr Pointer to UDP header
 * @param data Pointer to UDP payload
 * @param len Length of UDP payload
 * @return 16-bit checksum in network byte order
 */
uint16_t udp_checksum(uint8_t *src_ip, uint8_t *dst_ip, uint8_t *udp_hdr,
		uint8_t *data, uint16_t len) {

	uint32_t sum = 0;
	const uint16_t udp_len = UDP_HDR_LEN + len;

	// Pseudo-IP header (12 bytes)
	sum += static_cast<uint32_t>((src_ip[0] << 8) | src_ip[1]);
	sum += static_cast<uint32_t>((src_ip[2] << 8) | src_ip[3]);
	sum += static_cast<uint32_t>((dst_ip[0] << 8) | dst_ip[1]);
	sum += static_cast<uint32_t>((dst_ip[2] << 8) | dst_ip[3]);
	sum += 0x0011;  // Protocol = UDP (17)
	sum += udp_len;

	// UDP header (8 bytes) - process as words
	sum += static_cast<uint32_t>((udp_hdr[0] << 8) | udp_hdr[1]);
	sum += static_cast<uint32_t>((udp_hdr[2] << 8) | udp_hdr[3]);
	sum += static_cast<uint32_t>((udp_hdr[4] << 8) | udp_hdr[5]);
	sum += static_cast<uint32_t>((udp_hdr[6] << 8) | udp_hdr[7]);

	// UDP payload
	if (len > 0 && data != nullptr) {
		sum += checksum_accumulate(data, len);
	}

	return checksum_fold(sum);
}

/**
 * @brief Calculate TCP checksum
 * 
 * Includes pseudo-IP header, TCP header, and payload
 * 
 * @param src_ip Source IP address (4 bytes)
 * @param dst_ip Destination IP address (4 bytes)
 * @param tcp_hdr Pointer to TCP header
 * @param tcp_len Total TCP length (header + payload)
 * @return 16-bit checksum in network byte order
 */
uint16_t tcp_checksum(uint8_t *src_ip, uint8_t *dst_ip, uint8_t *tcp_hdr,
		uint16_t tcp_len) {

	uint32_t sum = 0;

	// Pseudo-IP header (12 bytes)
	sum += static_cast<uint32_t>((src_ip[0] << 8) | src_ip[1]);
	sum += static_cast<uint32_t>((src_ip[2] << 8) | src_ip[3]);
	sum += static_cast<uint32_t>((dst_ip[0] << 8) | dst_ip[1]);
	sum += static_cast<uint32_t>((dst_ip[2] << 8) | dst_ip[3]);
	sum += 0x0006;  // Protocol = TCP (6)
	sum += tcp_len;

	// TCP header + payload
	if (tcp_len > 0 && tcp_hdr != nullptr) {
		sum += checksum_accumulate(tcp_hdr, tcp_len);
	}

	return checksum_fold(sum);
}
