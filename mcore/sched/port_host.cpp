/*
 * Host port of the scheduler: a small NVIC model for unit tests.
 *
 * Exceptions are modelled as nested calls. A pended task runs as soon as its
 * level is more urgent (numerically lower) than both the active level and
 * BASEPRI and interrupts are not masked, exactly when the NVIC would take it;
 * among several, the most urgent goes first (tail-chaining on return).
 */
#if defined(MCORE_SCHED_HOST)

#include <set>

#include "port.hpp"

namespace sched {
namespace {
constexpr uint8_t THREAD = 16;

std::set<uint16_t> g_pending;
uint8_t g_active = THREAD;
uint32_t g_primask = 0;
uint32_t g_basepri = 0;
uint8_t g_levels[256] = {};
bool g_enabled[256] = {};
uint32_t g_cycles = 0;

uint8_t threshold() {
	uint8_t t = g_active;
	if (g_basepri != 0U && g_basepri < t) {
		t = static_cast<uint8_t>(g_basepri);
	}
	return t;
}

void reschedule() {
	for (;;) {
		if (g_primask != 0U) {
			return;
		}
		uint16_t best = 0;
		uint8_t best_level = threshold();
		bool found = false;
		for (uint16_t irq : g_pending) {
			if (g_enabled[irq] && g_levels[irq] < best_level) {
				best = irq;
				best_level = g_levels[irq];
				found = true;
			}
		}
		if (!found) {
			return;
		}
		g_pending.erase(best);
		Task *t = find_task_by_irq(best);
		const uint8_t prev = g_active;
		g_active = best_level;
		if (t != nullptr) {
			t->dispatch();
		}
		g_active = prev;
	}
}
} // namespace

namespace port {
uint32_t crit_enter() {
	const uint32_t k = g_primask;
	g_primask = 1;
	return k;
}
void crit_exit(uint32_t key) {
	g_primask = key;
	reschedule();
}
void pend(uint16_t irq) {
	g_pending.insert(irq);
	reschedule();
}
uint32_t basepri() {
	return g_basepri;
}
void set_basepri(uint32_t level) {
	g_basepri = level;
	reschedule();
}
void irq_setup(uint16_t irq, uint8_t level) {
	g_levels[irq] = level;
	g_enabled[irq] = true;
	reschedule();
}
void systick_level(uint8_t) {
}
uint32_t cycles() {
	return g_cycles;
}
} // namespace port

namespace host {
void reset() {
	g_pending.clear();
	g_active = THREAD;
	g_primask = 0;
	g_basepri = 0;
	for (bool &e : g_enabled) e = false;
}
void run_isr(uint8_t level, void (*fn)(void *), void *ctx) {
	const uint8_t prev = g_active;
	g_active = level;
	fn(ctx);
	g_active = prev;
	reschedule();
}
uint8_t active_level() {
	return g_active;
}
void set_cycles(uint32_t c) {
	g_cycles = c;
}
} // namespace host
} // namespace sched

#endif
