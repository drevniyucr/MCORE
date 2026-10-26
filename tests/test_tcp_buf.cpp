// Tests for the shared block pool and chained byte FIFOs (mcore/net/tcp_buf.cpp).
#include "doctest.h"

#include <cstring>
#include <vector>

#include "net/tcp_buf.hpp"

namespace {

std::vector<uint8_t> pattern(size_t n, uint8_t seed) {
	std::vector<uint8_t> v(n);
	for (size_t i = 0; i < n; i++) v[i] = static_cast<uint8_t>(seed + i * 7);
	return v;
}

constexpr uint16_t ANY = TCP_POOL_BLOCKS;   // no per-FIFO cap

} // namespace

TEST_CASE("tcp_buf: bytes come out as they went in, across block borders") {
	tcpbuf::init();
	tcp_fifo_t f{};
	const auto in = pattern(3 * TCP_BLOCK_SIZE + 77, 1);
	CHECK(tcpbuf::push(f, in.data(), 100, ANY, 0) == 100);           // uneven pieces
	CHECK(tcpbuf::push(f, in.data() + 100, in.size() - 100, ANY, 0) == in.size() - 100);
	CHECK(f.count == in.size());
	CHECK(f.blocks == 4);

	std::vector<uint8_t> out(in.size());
	CHECK(tcpbuf::pop(f, out.data(), 333) == 333);
	CHECK(tcpbuf::pop(f, out.data() + 333, out.size()) == in.size() - 333);
	CHECK(out == in);
	CHECK(f.count == 0);
	CHECK(f.blocks == 0);                                             // nothing kept when empty
	CHECK(tcpbuf::free_blocks() == TCP_POOL_BLOCKS);
}

TEST_CASE("tcp_buf: peek at any offset without consuming") {
	tcpbuf::init();
	tcp_fifo_t f{};
	const auto in = pattern(2000, 9);
	tcpbuf::push(f, in.data(), in.size(), ANY, 0);
	tcpbuf::pop(f, nullptr, 300);                                     // head starts mid-block

	for (uint32_t off : { 0u, 1u, 211u, 212u, 700u, 1699u }) {
		uint8_t buf[64];
		const uint32_t n = tcpbuf::peek(f, off, buf, sizeof(buf));
		CHECK(n == (off + 64 <= 1700 ? 64u : 1700 - off));
		CHECK(memcmp(buf, in.data() + 300 + off, n) == 0);
	}
	uint8_t one;
	CHECK(tcpbuf::peek(f, 1700, &one, 1) == 0);                       // past the end
	CHECK(f.count == 1700);                                           // untouched
}

TEST_CASE("tcp_buf: per-FIFO cap and pool reserve limit push, room() predicts it") {
	tcpbuf::init();
	tcp_fifo_t f{};
	const auto in = pattern(10 * TCP_BLOCK_SIZE, 3);
	CHECK(tcpbuf::room(f, 2, 0) == 2u * TCP_BLOCK_SIZE);
	CHECK(tcpbuf::push(f, in.data(), in.size(), 2, 0) == 2u * TCP_BLOCK_SIZE);
	CHECK(tcpbuf::room(f, 2, 0) == 0);

	tcp_fifo_t g{};
	const uint16_t keep = static_cast<uint16_t>(TCP_POOL_BLOCKS - 2 - 3);   // leaves 3 usable
	CHECK(tcpbuf::room(g, ANY, keep) == 3u * TCP_BLOCK_SIZE);
	CHECK(tcpbuf::push(g, in.data(), in.size(), ANY, keep) == 3u * TCP_BLOCK_SIZE);
	CHECK(tcpbuf::free_blocks() == keep);

	tcpbuf::clear(f);
	tcpbuf::clear(g);
	CHECK(tcpbuf::free_blocks() == TCP_POOL_BLOCKS);
	CHECK(f.blocks == 0);
	CHECK(f.count == 0);
}

TEST_CASE("tcp_buf: a partially filled tail is reused before a new block") {
	tcpbuf::init();
	tcp_fifo_t f{};
	const auto in = pattern(TCP_BLOCK_SIZE, 5);
	tcpbuf::push(f, in.data(), 10, 1, 0);
	CHECK(tcpbuf::room(f, 1, 0) == TCP_BLOCK_SIZE - 10u);
	CHECK(tcpbuf::push(f, in.data(), TCP_BLOCK_SIZE, 1, 0) == TCP_BLOCK_SIZE - 10u);
	CHECK(f.blocks == 1);
	tcpbuf::clear(f);
}

TEST_CASE("tcp_buf: the pool runs dry across FIFOs and recovers") {
	tcpbuf::init();
	static tcp_fifo_t fifos[TCP_POOL_BLOCKS + 1];
	for (auto &f : fifos) f = tcp_fifo_t{};
	const uint8_t b = 0x5A;
	uint32_t taken = 0;
	for (auto &f : fifos) taken += tcpbuf::push(f, &b, 1, ANY, 0);
	CHECK(taken == TCP_POOL_BLOCKS);                                  // one block each, last gets none
	CHECK(tcpbuf::free_blocks() == 0);
	for (auto &f : fifos) tcpbuf::pop(f, nullptr, 1);
	CHECK(tcpbuf::free_blocks() == TCP_POOL_BLOCKS);
}
