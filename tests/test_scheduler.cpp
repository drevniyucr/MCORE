// Unit tests for the cooperative software-timer scheduler.
// The scheduler takes `now` as a parameter (no hardware clock), so it is
// fully exercisable on the host with a controlled tick value.
#include "doctest.h"

#include "core/scheduler.hpp"

namespace {
// Free-function task callbacks writing into file-local counters (the scheduler
// takes plain void(*)() — captureless, no std::function / heap).
int c0, c1, c2;
void hit0() { ++c0; }
void hit1() { ++c1; }
void hit2() { ++c2; }
void reset() { c0 = c1 = c2 = 0; }
}

TEST_CASE("scheduler: period-0 task runs on every poll") {
	reset();
	sched::Scheduler<2> s;
	s.every(0, hit0);
	s.start(1000);
	for (int i = 0; i < 5; ++i) s.poll(1000);  // same tick, still every pass
	CHECK(c0 == 5);
}

TEST_CASE("scheduler: periodic task fires only after its interval") {
	reset();
	sched::Scheduler<2> s;
	s.every(100, hit1);
	s.start(1000);

	s.poll(1050);  CHECK(c1 == 0);   // 50 < 100 -> not yet
	s.poll(1099);  CHECK(c1 == 0);   // 99 < 100
	s.poll(1100);  CHECK(c1 == 1);   // exactly 100 -> fires, last := 1100
	s.poll(1150);  CHECK(c1 == 1);   // 50 since last
	s.poll(1200);  CHECK(c1 == 2);   // 100 since last -> fires
}

TEST_CASE("scheduler: independent tasks keep independent deadlines") {
	reset();
	sched::Scheduler<3> s;
	s.every(0,   hit0);   // every poll
	s.every(100, hit1);
	s.every(400, hit2);
	s.start(0);

	// Poll every 100 ms for 800 ms of virtual time.
	for (uint32_t t = 100; t <= 800; t += 100) s.poll(t);

	CHECK(c0 == 8);   // 8 polls
	CHECK(c1 == 8);   // 100 ms task on every 100 ms poll
	CHECK(c2 == 2);   // 400 ms task at t=400 and t=800
}

TEST_CASE("scheduler: unsigned tick wrap is handled") {
	reset();
	sched::Scheduler<1> s;
	s.every(100, hit0);
	s.start(0xFFFFFFC0u);            // 64 ticks before wrap
	s.poll(0xFFFFFFF0u);  CHECK(c0 == 0);   // elapsed 48 < 100
	s.poll(0x00000024u);  CHECK(c0 == 1);   // wrapped: elapsed = 0x64 = 100 -> fires
}

TEST_CASE("scheduler: capacity is enforced") {
	sched::Scheduler<2> s;
	CHECK(s.every(10, hit0) == true);
	CHECK(s.every(10, hit1) == true);
	CHECK(s.every(10, hit2) == false);   // full
	CHECK(s.size() == 2);
}

TEST_CASE("scheduler: null callback is rejected") {
	sched::Scheduler<2> s;
	CHECK(s.every(10, nullptr) == false);  // not full, but fn == null
	CHECK(s.size() == 0);
}
