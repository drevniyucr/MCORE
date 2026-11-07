/*
 * sched.cpp — see sched.hpp.
 */
#include "sched.hpp"

#include "port.hpp"

namespace sched {

namespace {
Task *g_tasks = nullptr;
TimeEvent *g_timers = nullptr;
Stats g_stats = {};
uint32_t g_depth = 0;          // nested task activations
uint32_t g_busy_start = 0;
} // namespace

// =============================================================================
// Task
// =============================================================================
Task::Task(const char *name, uint8_t prio, uint16_t irq, Handler handler, Event *queue, uint8_t queue_len,
           void *ctx)
    : name_(name), handler_(handler), ctx_(ctx), queue_(queue), queue_len_(queue_len), prio_(prio), irq_(irq) {
	const uint32_t key = port::crit_enter();
	next_ = g_tasks;
	g_tasks = this;
	port::crit_exit(key);
}

Task::~Task() {
	const uint32_t key = port::crit_enter();
	for (Task **p = &g_tasks; *p != nullptr; p = &(*p)->next_) {
		if (*p == this) {
			*p = next_;
			break;
		}
	}
	port::crit_exit(key);
}

bool Task::post(const Event &e) {
	const uint32_t key = port::crit_enter();
	if (count_ >= queue_len_) {
		stats_.dropped += 1;
		port::crit_exit(key);
		return false;
	}
	queue_[(head_ + count_) % queue_len_] = e;
	count_ += 1;
	if (count_ > stats_.max_queue) {
		stats_.max_queue = count_;
	}
	port::crit_exit(key);
	port::pend(irq_);
	return true;
}

void Task::raise(uint8_t sig) {
	const uint32_t key = port::crit_enter();
	signals_ |= 1UL << (sig & 31U);
	port::crit_exit(key);
	port::pend(irq_);
}

void Task::run_handler(const Event &e) {
	const uint32_t t0 = port::cycles();
	handler_(*this, e);
	const uint32_t d = port::cycles() - t0;
	stats_.events += 1;
	if (d > stats_.max_cycles) {
		stats_.max_cycles = d;
	}
}

void Task::dispatch() {
	const uint32_t t0 = port::cycles();
	uint32_t key = port::crit_enter();
	if (g_depth++ == 0U) {
		g_busy_start = t0;
	}
	port::crit_exit(key);

	bool worked = false;
	for (;;) {
		key = port::crit_enter();
		const uint32_t sigs = signals_;
		signals_ = 0;
		Event e{};
		bool have = false;
		if (sigs == 0U && count_ > 0U) {
			e = queue_[head_];
			head_ = static_cast<uint8_t>((head_ + 1U) % queue_len_);
			count_ -= 1;
			have = true;
		}
		port::crit_exit(key);

		if (sigs != 0U) {
			for (uint8_t s = 0; s < 32U; ++s) {
				if ((sigs & (1UL << s)) != 0U) {
					run_handler(Event{ s, 0, 0 });
				}
			}
			worked = true;
			continue;
		}
		if (!have) {
			break;
		}
		run_handler(e);
		worked = true;
	}

	const uint32_t t1 = port::cycles();
	if (worked) {
		stats_.runs += 1;
	}
	key = port::crit_enter();
	if (--g_depth == 0U) {
		g_stats.busy_cycles += t1 - g_busy_start;
	}
	port::crit_exit(key);
}

// =============================================================================
// Time events
// =============================================================================
TimeEvent::TimeEvent(Task &task, uint8_t sig) : task_(task), sig_(sig) {
	const uint32_t key = port::crit_enter();
	next_ = g_timers;
	g_timers = this;
	port::crit_exit(key);
}

TimeEvent::~TimeEvent() {
	const uint32_t key = port::crit_enter();
	for (TimeEvent **p = &g_timers; *p != nullptr; p = &(*p)->next_) {
		if (*p == this) {
			*p = next_;
			break;
		}
	}
	port::crit_exit(key);
}

void TimeEvent::arm(uint32_t ticks, uint32_t period) {
	const uint32_t key = port::crit_enter();
	ctr_ = ticks == 0U ? 1U : ticks;
	period_ = period;
	port::crit_exit(key);
}

void TimeEvent::disarm() {
	const uint32_t key = port::crit_enter();
	ctr_ = 0;
	port::crit_exit(key);
}

void tick() {
	const uint32_t key = port::crit_enter();
	g_stats.ticks += 1;
	for (TimeEvent *t = g_timers; t != nullptr; t = t->next_) {
		const uint32_t c = t->ctr_;
		if (c == 0U) {
			continue;
		}
		t->ctr_ = c - 1U;
		if (c != 1U) {
			continue;
		}
		t->ctr_ = t->period_;
		t->task_.raise(t->sig_);
	}
	port::crit_exit(key);
}

// =============================================================================
// Lock, start, misc
// =============================================================================
uint32_t lock(uint8_t ceiling) {
	const uint32_t prev = port::basepri();
	const uint32_t level = nvic_level(ceiling);
	if (prev == 0U || level < prev) {
		port::set_basepri(level);
	}
	return prev;
}

void unlock(uint32_t key) {
	port::set_basepri(key);
}

Lock::Lock(uint8_t ceiling) : prev_(lock(ceiling)) {
}

Lock::~Lock() {
	unlock(prev_);
}

bool start() {
	for (Task *t = g_tasks; t != nullptr; t = next_task(t)) {
		if (t->prio() == 0U || t->prio() > MAX_PRIO) {
			return false;
		}
		for (Task *u = next_task(t); u != nullptr; u = next_task(u)) {
			if (u->irq() == t->irq()) {
				return false;
			}
		}
	}
	port::systick_level(TICK_NVIC_LEVEL);
	for (Task *t = g_tasks; t != nullptr; t = next_task(t)) {
		port::irq_setup(t->irq(), nvic_level(t->prio()));
	}
	return true;
}

void idle() {
}

const Stats &stats() {
	return g_stats;
}

Task *first_task() {
	return g_tasks;
}

Task *next_task(Task *t) {
	return t->next_;
}

Task *find_task_by_irq(uint16_t irq) {
	for (Task *t = g_tasks; t != nullptr; t = next_task(t)) {
		if (t->irq() == irq) {
			return t;
		}
	}
	return nullptr;
}

} // namespace sched

#if !defined(MCORE_SCHED_HOST)
// Called by SysTick_Handler (mcore/core/system.cpp) every millisecond
extern "C" void SysTick_Hook() {
	sched::tick();
}
#endif
