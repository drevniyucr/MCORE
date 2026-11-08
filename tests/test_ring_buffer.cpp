// Unit tests for the lock-free SPSC ring buffer (sequential logic).
// Memory-ordering correctness is a property of std::atomic; here we verify the
// index/FIFO/full/empty/wrap behaviour deterministically.
#include "doctest.h"

#include "core/ring_buffer.hpp"

TEST_CASE("ring: empty / full boundaries (capacity == N-1)") {
	spsc::RingBuffer<int, 4> rb;   // holds 3
	CHECK(rb.capacity() == 3);
	CHECK(rb.empty());
	CHECK(rb.size() == 0);

	CHECK(rb.push(10));
	CHECK(rb.push(20));
	CHECK(rb.push(30));
	CHECK(rb.size() == 3);
	CHECK(rb.push(40) == false);   // full: last slot kept empty
	CHECK(rb.size() == 3);
	CHECK(!rb.empty());
}

TEST_CASE("ring: FIFO order") {
	spsc::RingBuffer<int, 8> rb;
	for (int i = 0; i < 7; ++i) CHECK(rb.push(i));
	int out = -1;
	for (int i = 0; i < 7; ++i) {
		CHECK(rb.pop(out));
		CHECK(out == i);
	}
	CHECK(rb.empty());
	CHECK(rb.pop(out) == false);   // empty
}

TEST_CASE("ring: index wrap-around holds FIFO over many cycles") {
	spsc::RingBuffer<int, 4> rb;   // small -> indices wrap the 2-bit mask fast
	int expected = 0;
	int out = -1;
	for (int round = 0; round < 1000; ++round) {
		// push a burst of 3 (fills the buffer), then drain it
		for (int k = 0; k < 3; ++k) CHECK(rb.push(expected + k));
		CHECK(rb.push(999) == false);          // full
		for (int k = 0; k < 3; ++k) {
			CHECK(rb.pop(out));
			CHECK(out == expected + k);         // still FIFO after wrap
		}
		CHECK(rb.pop(out) == false);           // drained
		expected += 3;
	}
}

TEST_CASE("ring: interleaved producer/consumer keeps order past capacity") {
	spsc::RingBuffer<int, 8> rb;
	int next_push = 0, next_pop = 0, out = -1;
	// Producer runs 2 ahead, consumer 1 behind, for far more than capacity.
	for (int step = 0; step < 500; ++step) {
		if (rb.push(next_push)) ++next_push;
		if (rb.push(next_push)) ++next_push;
		if (rb.pop(out)) { CHECK(out == next_pop); ++next_pop; }
	}
	while (rb.pop(out)) { CHECK(out == next_pop); ++next_pop; }
	CHECK(next_pop == next_push);   // nothing lost or duplicated
}

TEST_CASE("ring: works with a non-trivial-ish struct payload") {
	struct Sample { uint8_t ch; uint16_t val; };
	spsc::RingBuffer<Sample, 4> rb;
	CHECK(rb.push({1, 0xABCD}));
	CHECK(rb.push({0, 0x0042}));
	Sample s{};
	CHECK(rb.pop(s)); CHECK(s.ch == 1); CHECK(s.val == 0xABCD);
	CHECK(rb.pop(s)); CHECK(s.ch == 0); CHECK(s.val == 0x0042);
	CHECK(rb.empty());
}
