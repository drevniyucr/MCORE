// Unit tests for the inter-core primitives in mcore/ipc.
// The threaded cases run the producer and the consumer on two host threads,
// standing in for the two Cortex cores.
#include "doctest.h"

#include <cstdint>
#include <thread>

#include "ipc/seqlock.hpp"
#include "ipc/spsc_ring.hpp"

namespace {
struct Msg {
	uint32_t seq;
	uint32_t payload[3];
};

struct Snap {
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t sum;   // a + b + c, to detect torn reads
};
} // namespace

TEST_CASE("SpscRing: empty, fill, overflow, drain in order") {
	ipc::SpscRing<Msg, 4> r;
	r.reset();
	Msg m{};
	CHECK(r.size() == 0);
	CHECK_FALSE(r.pop(m));

	for (uint32_t i = 0; i < 4; ++i) {
		CHECK(r.push(Msg{i, {i, i, i}}));
	}
	CHECK(r.size() == 4);
	CHECK_FALSE(r.push(Msg{99, {}}));   // full: rejected, nothing overwritten

	for (uint32_t i = 0; i < 4; ++i) {
		REQUIRE(r.pop(m));
		CHECK(m.seq == i);
	}
	CHECK_FALSE(r.pop(m));
}

TEST_CASE("SpscRing: counters wrap past 2^32") {
	ipc::SpscRing<Msg, 8> r;
	r.reset();
	r.head.store(0xFFFFFFFEU);
	r.tail.store(0xFFFFFFFEU);
	Msg m{};
	for (uint32_t i = 0; i < 8; ++i) {
		REQUIRE(r.push(Msg{i, {}}));
	}
	CHECK_FALSE(r.push(Msg{}));
	for (uint32_t i = 0; i < 8; ++i) {
		REQUIRE(r.pop(m));
		CHECK(m.seq == i);
	}
	CHECK(r.size() == 0);
}

TEST_CASE("SpscRing: two threads, no loss, no reordering") {
	static ipc::SpscRing<Msg, 16> r;
	r.reset();
	constexpr uint32_t COUNT = 200000;

	std::thread producer([] {
		for (uint32_t i = 0; i < COUNT;) {
			if (r.push(Msg{i, {i * 3U, ~i, i ^ 0x5A5A5A5AU}})) {
				++i;
			}
		}
	});

	uint32_t expected = 0;
	bool ok = true;
	Msg m{};
	while (expected < COUNT) {
		if (r.pop(m)) {
			ok = ok && m.seq == expected && m.payload[0] == expected * 3U
			        && m.payload[1] == ~expected && m.payload[2] == (expected ^ 0x5A5A5A5AU);
			++expected;
		}
	}
	producer.join();
	CHECK(ok);
	CHECK(r.size() == 0);
}

TEST_CASE("Seqlock: nothing published yet, then latest value") {
	ipc::Seqlock<Snap> s;
	s.reset();
	Snap out{};
	CHECK_FALSE(s.read(out));   // never written

	s.write(Snap{1, 2, 3, 6});
	s.write(Snap{4, 5, 6, 15});
	REQUIRE(s.read(out));
	CHECK(out.a == 4);
	CHECK(out.sum == 15);
	CHECK(s.version() == 2);
}

TEST_CASE("Seqlock: concurrent writer never yields a torn copy") {
	static ipc::Seqlock<Snap> s;
	s.reset();
	constexpr uint32_t WRITES = 300000;

	std::thread writer([] {
		for (uint32_t i = 1; i <= WRITES; ++i) {
			s.write(Snap{i, i * 7U, i * 13U, i + i * 7U + i * 13U});
		}
	});

	bool consistent = true;
	uint32_t good_reads = 0;
	uint32_t last_a = 0;
	bool monotonic = true;
	Snap out{};
	while (last_a < WRITES) {
		if (s.read(out, 1000U)) {
			consistent = consistent && (out.a + out.b + out.c == out.sum);
			monotonic = monotonic && out.a >= last_a;
			last_a = out.a;
			++good_reads;
		}
	}
	writer.join();
	CHECK(consistent);
	CHECK(monotonic);
	CHECK(good_reads > 0);
}
