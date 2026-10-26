/*
 * tcp_buf.hpp — shared block pool and byte FIFOs for TCP send/receive data
 *
 * Every TCP connection used to own a fixed 8.8 KB of buffers, idle or not,
 * which is what capped the stack at 10 connections. Now a connection owns a
 * ~96-byte control block, and its data lives in byte FIFOs built from one
 * shared pool of fixed-size blocks:
 *
 *   - no fragmentation: all blocks have the same size, alloc/free is O(1);
 *   - an idle connection holds no block at all (an emptied FIFO frees its last);
 *   - running out of blocks is not an error: the receive window closes and
 *     unsent data waits, TCP's own flow control takes it from there.
 *
 * A FIFO that is all zero is empty, so a memset control block is valid.
 */
#pragma once

#include <cstddef>
#include <cstdint>

constexpr uint16_t TCP_BLOCK_SIZE  = 512;
constexpr uint16_t TCP_POOL_BLOCKS = 192;                        // 96 KB shared by all connections
constexpr uint32_t TCP_POOL_BYTES  = uint32_t{TCP_BLOCK_SIZE} * TCP_POOL_BLOCKS;

struct tcp_fifo_t {
	uint32_t count;      // bytes buffered
	uint16_t head;       // first block (valid when blocks > 0)
	uint16_t tail;       // last block
	uint16_t head_off;   // read position inside head
	uint16_t tail_len;   // bytes written into tail
	uint16_t blocks;     // blocks owned
};

namespace tcpbuf {

// Every block back to the pool. Any FIFO still holding blocks becomes invalid:
// call only together with resetting all FIFOs (NET_TCP_Init).
void init();

uint16_t free_blocks();

// Bytes push() could take right now: free space in the tail block plus new
// blocks, while the FIFO owns at most max_blocks and the pool keeps at least
// keep_free blocks for others.
uint32_t room(const tcp_fifo_t &f, uint16_t max_blocks, uint16_t keep_free);

// Append up to n bytes (same limits as room()); returns how many were taken.
uint32_t push(tcp_fifo_t &f, const uint8_t *data, uint32_t n, uint16_t max_blocks, uint16_t keep_free);

// Copy up to n bytes starting `offset` bytes into the FIFO, without consuming.
uint32_t peek(const tcp_fifo_t &f, uint32_t offset, uint8_t *dst, uint32_t n);

// Consume up to n bytes from the front (into dst unless it is nullptr).
uint32_t pop(tcp_fifo_t &f, uint8_t *dst, uint32_t n);

// Drop everything, blocks back to the pool.
void clear(tcp_fifo_t &f);

} // namespace tcpbuf
