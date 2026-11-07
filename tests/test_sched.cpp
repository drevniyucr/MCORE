// Scheduler semantics on the host NVIC model (mcore/sched/port_host.cpp)
#include <string>
#include <vector>

#include "doctest.h"
#include "sched/port.hpp"
#include "sched/sched.hpp"

namespace {

std::vector<std::string> trace;

// Each test task records "<name>:<sig>" and runs the action of the event
struct Ctx {
	const char *name;
	void (*action)(const sched::Event &) = nullptr;
};

void handler(sched::Task &self, const sched::Event &e) {
	auto *c = static_cast<Ctx *>(self.ctx());
	trace.push_back(std::string(c->name) + ":" + std::to_string(e.sig));
	if (c->action != nullptr) {
		c->action(e);
	}
}

struct Fixture {
	Fixture() {
		trace.clear();
		sched::host::reset();
	}
	~Fixture() { sched::host::reset(); }
};

} // namespace

TEST_CASE_FIXTURE(Fixture, "post runs the task; a higher priority preempts, a lower one waits") {
	static sched::Task *hi_p, *lo_p;
	Ctx lo{ "lo" }, hi{ "hi" };
	sched::TaskQ<4> low("lo", 1, 10, handler, &lo);
	sched::TaskQ<4> high("hi", 3, 11, handler, &hi);
	hi_p = &high;
	lo_p = &low;
	REQUIRE(sched::start());

	// lo posts to hi: hi runs inside lo's handler (preemption)
	lo.action = [](const sched::Event &e) {
		if (e.sig == 1) {
			hi_p->post(7);
			trace.push_back("lo:after-post");
		}
	};
	// hi posts to lo: lo runs after hi has finished
	hi.action = [](const sched::Event &e) {
		if (e.sig == 7) {
			lo_p->post(2);
			trace.push_back("hi:after-post");
		}
	};
	low.post(1);
	CHECK(trace == std::vector<std::string>{ "lo:1", "hi:7", "hi:after-post", "lo:after-post", "lo:2" });
	CHECK(sched::host::active_level() == 16);
}

TEST_CASE_FIXTURE(Fixture, "FIFO order, overflow is dropped and counted") {
	Ctx c{ "t" };
	sched::TaskQ<2> t("t", 2, 12, handler, &c);
	REQUIRE(sched::start());
	{
		sched::Lock lock(2);            // hold the task off while queueing
		CHECK(t.post(1));
		CHECK(t.post(2));
		CHECK_FALSE(t.post(3));
		CHECK(trace.empty());
	}
	CHECK(trace == std::vector<std::string>{ "t:1", "t:2" });
	CHECK(t.stats().dropped == 1);
	CHECK(t.stats().max_queue == 2);
}

TEST_CASE_FIXTURE(Fixture, "signals coalesce and are delivered before queued events") {
	Ctx c{ "t" };
	sched::TaskQ<4> t("t", 2, 13, handler, &c);
	REQUIRE(sched::start());
	{
		sched::Lock lock(sched::MAX_PRIO);
		t.post(40);
		t.raise(3);
		t.raise(3);
		t.raise(5);
	}
	CHECK(trace == std::vector<std::string>{ "t:3", "t:5", "t:40" });
}

TEST_CASE_FIXTURE(Fixture, "Lock holds off tasks up to the ceiling only") {
	Ctx a{ "a" }, b{ "b" };
	sched::TaskQ<4> mid("a", 2, 14, handler, &a);
	sched::TaskQ<4> top("b", 4, 15, handler, &b);
	REQUIRE(sched::start());
	{
		sched::Lock lock(2);
		mid.post(1);
		top.post(1);                    // above the ceiling: runs at once
		CHECK(trace == std::vector<std::string>{ "b:1" });
	}
	CHECK(trace == std::vector<std::string>{ "b:1", "a:1" });   // released by the unlock
}

TEST_CASE_FIXTURE(Fixture, "interrupts post; tasks run after the ISR, most urgent first") {
	static sched::Task *p1, *p2;
	Ctx a{ "a" }, b{ "b" };
	sched::TaskQ<4> t1("a", 1, 16, handler, &a);
	sched::TaskQ<4> t2("b", 5, 17, handler, &b);
	p1 = &t1;
	p2 = &t2;
	REQUIRE(sched::start());
	sched::host::run_isr(3, [](void *) {
		p1->raise(0);
		p2->raise(0);
		trace.push_back("isr:end");
	}, nullptr);
	CHECK(trace == std::vector<std::string>{ "isr:end", "b:0", "a:0" });
}

TEST_CASE_FIXTURE(Fixture, "a task posting to itself runs again after it returns, not nested") {
	static sched::Task *self_p;
	static int depth = 0, max_depth = 0, count = 0;
	Ctx c{ "s" };
	c.action = [](const sched::Event &) {
		depth += 1;
		max_depth = depth > max_depth ? depth : max_depth;
		if (++count < 3) self_p->post(1);
		depth -= 1;
	};
	sched::TaskQ<4> s("s", 2, 18, handler, &c);
	self_p = &s;
	REQUIRE(sched::start());
	s.post(1);
	CHECK(count == 3);
	CHECK(max_depth == 1);
}

TEST_CASE_FIXTURE(Fixture, "time events: one-shot, periodic, disarm") {
	Ctx c{ "t" };
	sched::TaskQ<4> t("t", 2, 19, handler, &c);
	sched::TimeEvent once(t, 1), every(t, 2);
	REQUIRE(sched::start());
	once.arm(3);
	every.arm(2, 2);
	auto systick = [](void *) { sched::tick(); };
	for (int i = 0; i < 6; ++i) {
		sched::host::run_isr(sched::TICK_NVIC_LEVEL, systick, nullptr);
		trace.push_back("tick");
	}
	CHECK(trace == std::vector<std::string>{ "tick", "t:2", "tick", "t:1", "tick", "t:2", "tick", "tick", "t:2",
	                                         "tick" });
	CHECK_FALSE(once.armed());
	every.disarm();
	trace.clear();
	sched::host::run_isr(sched::TICK_NVIC_LEVEL, systick, nullptr);
	sched::host::run_isr(sched::TICK_NVIC_LEVEL, systick, nullptr);
	CHECK(trace.empty());
}

TEST_CASE_FIXTURE(Fixture, "start() rejects bad priorities and shared vectors") {
	Ctx c{ "t" };
	{
		sched::TaskQ<1> a("a", 2, 20, handler, &c);
		sched::TaskQ<1> b("b", 3, 20, handler, &c);
		CHECK_FALSE(sched::start());
	}
	{
		sched::TaskQ<1> a("a", sched::MAX_PRIO + 1, 21, handler, &c);
		CHECK_FALSE(sched::start());
	}
	sched::TaskQ<1> ok("ok", 1, 22, handler, &c);
	CHECK(sched::start());
	CHECK(sched::nvic_level(1) == 15);
	CHECK(sched::nvic_level(sched::MAX_PRIO) == 8);
	CHECK(sched::TICK_NVIC_LEVEL == 7);
}

TEST_CASE_FIXTURE(Fixture, "stats: runs, events, cycles of the outermost activation") {
	static sched::Task *hp;
	Ctx lo{ "lo" }, hi{ "hi" };
	lo.action = [](const sched::Event &) {
		sched::host::set_cycles(100);
		hp->post(1);
		sched::host::set_cycles(150);
	};
	hi.action = [](const sched::Event &) { sched::host::set_cycles(130); };
	sched::host::set_cycles(0);
	sched::TaskQ<2> l("lo", 1, 23, handler, &lo);
	sched::TaskQ<2> h("hi", 2, 24, handler, &hi);
	hp = &h;
	REQUIRE(sched::start());
	const uint64_t busy0 = sched::stats().busy_cycles;
	l.post(1);
	CHECK(l.stats().runs == 1);
	CHECK(l.stats().events == 1);
	CHECK(l.stats().max_cycles == 150);          // includes the preemption
	CHECK(h.stats().max_cycles == 30);
	CHECK(sched::stats().busy_cycles - busy0 == 150);   // counted once
}
