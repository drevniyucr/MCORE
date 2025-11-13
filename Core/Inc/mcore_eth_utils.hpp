/*
 * mcore_eth_utils.hpp
 *
 *  Created on: Sep 25, 2025
 *      Author: AkimovMA
 */
#pragma once

#include "mcore_def.hpp"

uint16_t checksum(const uint8_t *data, uint16_t len);

uint16_t udp_checksum(uint8_t *src_ip, uint8_t *dst_ip, uint8_t *udp_hdr,
		uint8_t *data, uint16_t len);

uint16_t tcp_checksum(uint8_t *src_ip, uint8_t *dst_ip, uint8_t *tcp_hdr,
		uint16_t tcp_len);

static inline uint16_t bswap16(uint16_t x) {
	return (x << 8) | (x >> 8);
}
static inline uint32_t bswap32(uint32_t x) {
	return ((x & 0x000000FFUL) << 24) | ((x & 0x0000FF00UL) << 8)
			| ((x & 0x00FF0000UL) >> 8) | ((x & 0xFF000000UL) >> 24);
}
static inline uint32_t BuffU8ToU32rev(uint8_t *x) {
	return ((x[3] << 24) | (x[2] << 16) | (x[1] << 8) | x[0]);
}
static inline uint32_t BuffU8ToU32(uint8_t *x) {
	return ((x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3]);
}

static inline void U32toBuffU8(uint8_t *buff, uint32_t num) {
	buff[0] = (uint8_t)((num >> 24) & 0xFF);
	buff[1] = (uint8_t)((num >> 16) & 0xFF);
	buff[2] = (uint8_t)((num>> 8)   & 0xFF);
	buff[3] = (uint8_t)((num)       & 0xFF);
}
