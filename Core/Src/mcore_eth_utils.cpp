<<<<<<< HEAD
/*
 * mcore_eth_utils.c
 *
 *  Created on: Sep 24, 2025
 *      Author: AkimovMA
 */
#include "mcore_eth_utils.hpp"


// Вычисление IP checksum (над IP header)
uint16_t checksum(const uint8_t *data, uint16_t len) {

	uint32_t sum = 0;
	while (len > 1) {
		sum += ((uint16_t) data[0] << 8) | data[1];
		data += 2;
		len -= 2;
	}
	if (len > 0) { // остался один байт
		sum += ((uint16_t) data[0] << 8);
	}
	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	return (uint16_t) (~sum);
}

uint16_t udp_checksum(uint8_t *src_ip, uint8_t *dst_ip, uint8_t *udp_hdr,
		uint8_t *data, uint16_t len) {

	uint32_t sum = 0;
	uint16_t udp_len = 8 + len; // UDP header + payload
	// --- Pseudo-IP header (12B) ---
	sum += (src_ip[0] << 8) | src_ip[1];
	sum += (src_ip[2] << 8) | src_ip[3];

	sum += (dst_ip[0] << 8) | dst_ip[1];
	sum += (dst_ip[2] << 8) | dst_ip[3];

	sum += 0x0011;          // Protocol UDP (17)
	sum += udp_len;         // UDP length
	// --- UDP header (8B) ---
	for (int i = 0; i < 8; i += 2) {
		sum += (udp_hdr[i] << 8) | udp_hdr[i + 1];
	}
	// --- UDP payload ---
	for (int i = 0; i < len; i += 2) {
		uint16_t word = data[i] << 8;
		if (i + 1 < len)
			word |= data[i + 1];
		sum += word;
	}
	// --- Обработка переноса ---
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t) (~sum);
}

uint16_t tcp_checksum(uint8_t *src_ip, uint8_t *dst_ip, uint8_t *tcp_hdr,
		uint16_t tcp_len) {

	uint32_t sum = 0;
	// --- Pseudo-IP header (12B) ---
	  // --- Псевдо-IP заголовок (12 байт) ---
	    sum += (src_ip[0] << 8) | src_ip[1];
	    sum += (src_ip[2] << 8) | src_ip[3];

	    sum += (dst_ip[0] << 8) | dst_ip[1];
	    sum += (dst_ip[2] << 8) | dst_ip[3];

	    sum += 0x0006;          // Protocol = TCP (6)
	    sum += tcp_len;         // TCP length (заголовок + payload)

	    // --- TCP заголовок + payload ---
	    for (int i = 0; i < tcp_len; i += 2) {
	        uint16_t word = tcp_hdr[i] << 8;
	        if (i + 1 < tcp_len)
	            word |= tcp_hdr[i + 1];
	        sum += word;
	    }

	    // --- Обработка переносов ---
	    while (sum >> 16)
	        sum = (sum & 0xFFFF) + (sum >> 16);

	    return (uint16_t)(~sum);
}
=======
/*
 * mcore_eth_utils.c
 *
 *  Created on: Sep 24, 2025
 *      Author: AkimovMA
 */
#include "mcore_eth_utils.hpp"


// Вычисление IP checksum (над IP header)
uint16_t checksum(const uint8_t *data, uint16_t len) {

	uint32_t sum = 0;
	while (len > 1) {
		sum += ((uint16_t) data[0] << 8) | data[1];
		data += 2;
		len -= 2;
	}
	if (len > 0) { // остался один байт
		sum += ((uint16_t) data[0] << 8);
	}
	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	return (uint16_t) (~sum);
}

uint16_t udp_checksum(uint8_t *src_ip, uint8_t *dst_ip, uint8_t *udp_hdr,
		uint8_t *data, uint16_t len) {

	uint32_t sum = 0;
	uint16_t udp_len = 8 + len; // UDP header + payload
	// --- Pseudo-IP header (12B) ---
	sum += (src_ip[0] << 8) | src_ip[1];
	sum += (src_ip[2] << 8) | src_ip[3];

	sum += (dst_ip[0] << 8) | dst_ip[1];
	sum += (dst_ip[2] << 8) | dst_ip[3];

	sum += 0x0011;          // Protocol UDP (17)
	sum += udp_len;         // UDP length
	// --- UDP header (8B) ---
	for (int i = 0; i < 8; i += 2) {
		sum += (udp_hdr[i] << 8) | udp_hdr[i + 1];
	}
	// --- UDP payload ---
	for (int i = 0; i < len; i += 2) {
		uint16_t word = data[i] << 8;
		if (i + 1 < len)
			word |= data[i + 1];
		sum += word;
	}
	// --- Обработка переноса ---
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t) (~sum);
}

uint16_t tcp_checksum(uint8_t *src_ip, uint8_t *dst_ip, uint8_t *tcp_hdr,
		uint16_t tcp_len) {

	uint32_t sum = 0;
	// --- Pseudo-IP header (12B) ---
	  // --- Псевдо-IP заголовок (12 байт) ---
	    sum += (src_ip[0] << 8) | src_ip[1];
	    sum += (src_ip[2] << 8) | src_ip[3];

	    sum += (dst_ip[0] << 8) | dst_ip[1];
	    sum += (dst_ip[2] << 8) | dst_ip[3];

	    sum += 0x0006;          // Protocol = TCP (6)
	    sum += tcp_len;         // TCP length (заголовок + payload)

	    // --- TCP заголовок + payload ---
	    for (int i = 0; i < tcp_len; i += 2) {
	        uint16_t word = tcp_hdr[i] << 8;
	        if (i + 1 < tcp_len)
	            word |= tcp_hdr[i + 1];
	        sum += word;
	    }

	    // --- Обработка переносов ---
	    while (sum >> 16)
	        sum = (sum & 0xFFFF) + (sum >> 16);

	    return (uint16_t)(~sum);
}
>>>>>>> 7688bfa (update)
