/*
 * tcp_buf.cpp — block pool (free list) and chained byte FIFOs
 */
#include "net/tcp_buf.hpp"

#include <cstring>

namespace {

uint8_t blocks[TCP_POOL_BLOCKS][TCP_BLOCK_SIZE];
uint16_t next_of[TCP_POOL_BLOCKS];      // chain links; free list uses them too
uint16_t free_head = 0;
uint16_t free_count = 0;
constexpr uint16_t NONE = 0xFFFF;

uint16_t alloc() {
	const uint16_t b = free_head;
	free_head = next_of[b];
	free_count--;
	next_of[b] = NONE;
	return b;
}

void release(uint16_t b) {
	next_of[b] = free_head;
	free_head = b;
	free_count++;
}

// Readable bytes in the head block
uint16_t head_avail(const tcp_fifo_t &f) {
	const uint16_t end = (f.blocks == 1) ? f.tail_len : TCP_BLOCK_SIZE;
	return static_cast<uint16_t>(end - f.head_off);
}

uint16_t new_blocks_allowed(const tcp_fifo_t &f, uint16_t max_blocks, uint16_t keep_free) {
	const uint16_t by_fifo = (f.blocks < max_blocks) ? static_cast<uint16_t>(max_blocks - f.blocks) : 0;
	const uint16_t by_pool = (free_count > keep_free) ? static_cast<uint16_t>(free_count - keep_free) : 0;
	return by_fifo < by_pool ? by_fifo : by_pool;
}

} // namespace

namespace tcpbuf {

void init() {
	for (uint16_t i = 0; i < TCP_POOL_BLOCKS; i++) {
		next_of[i] = static_cast<uint16_t>(i + 1 < TCP_POOL_BLOCKS ? i + 1 : NONE);
	}
	free_head = 0;
	free_count = TCP_POOL_BLOCKS;
}

uint16_t free_blocks() {
	return free_count;
}

uint32_t room(const tcp_fifo_t &f, uint16_t max_blocks, uint16_t keep_free) {
	const uint32_t in_tail = (f.blocks > 0) ? TCP_BLOCK_SIZE - f.tail_len : 0;
	return in_tail + uint32_t{new_blocks_allowed(f, max_blocks, keep_free)} * TCP_BLOCK_SIZE;
}

uint32_t push(tcp_fifo_t &f, const uint8_t *data, uint32_t n, uint16_t max_blocks, uint16_t keep_free) {
	uint32_t done = 0;
	while (done < n) {
		if (f.blocks == 0 || f.tail_len == TCP_BLOCK_SIZE) {
			if (new_blocks_allowed(f, max_blocks, keep_free) == 0) break;
			const uint16_t b = alloc();
			if (f.blocks == 0) {
				f.head = b;
				f.head_off = 0;
			} else {
				next_of[f.tail] = b;
			}
			f.tail = b;
			f.tail_len = 0;
			f.blocks++;
		}
		uint32_t chunk = TCP_BLOCK_SIZE - f.tail_len;
		if (chunk > n - done) chunk = n - done;
		memcpy(&blocks[f.tail][f.tail_len], data + done, chunk);
		f.tail_len = static_cast<uint16_t>(f.tail_len + chunk);
		done += chunk;
	}
	f.count += done;
	return done;
}

uint32_t peek(const tcp_fifo_t &f, uint32_t offset, uint8_t *dst, uint32_t n) {
	if (offset >= f.count) return 0;
	if (n > f.count - offset) n = f.count - offset;

	uint16_t b = f.head;
	uint32_t pos = f.head_off + offset;        // position counted from the head block start
	while (pos >= TCP_BLOCK_SIZE) {
		pos -= TCP_BLOCK_SIZE;
		b = next_of[b];
	}
	uint32_t done = 0;
	while (done < n) {
		uint32_t chunk = TCP_BLOCK_SIZE - pos;
		if (chunk > n - done) chunk = n - done;
		memcpy(dst + done, &blocks[b][pos], chunk);
		done += chunk;
		pos = 0;
		b = next_of[b];
	}
	return done;
}

uint32_t pop(tcp_fifo_t &f, uint8_t *dst, uint32_t n) {
	if (n > f.count) n = f.count;
	uint32_t done = 0;
	while (done < n) {
		uint32_t chunk = head_avail(f);
		if (chunk > n - done) chunk = n - done;
		if (dst != nullptr) memcpy(dst + done, &blocks[f.head][f.head_off], chunk);
		f.head_off = static_cast<uint16_t>(f.head_off + chunk);
		done += chunk;
		if (f.head_off == TCP_BLOCK_SIZE) {    // head block fully read
			const uint16_t old = f.head;
			f.head = next_of[old];
			f.head_off = 0;
			f.blocks--;
			release(old);
		}
	}
	f.count -= done;
	if (f.count == 0 && f.blocks > 0) {        // keep idle FIFOs at zero blocks
		release(f.head);
		f.blocks = 0;
		f.head_off = 0;
		f.tail_len = 0;
	}
	return done;
}

void clear(tcp_fifo_t &f) {
	while (f.blocks > 0) {
		const uint16_t old = f.head;
		f.head = next_of[old];
		release(old);
		f.blocks--;
	}
	f = tcp_fifo_t{};
}

} // namespace tcpbuf
