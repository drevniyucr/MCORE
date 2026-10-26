#pragma once
// ---------------------------------------------------------------------------
// Preemptive priority levels without an RTOS ("Super Simple Tasker" style).
//
// A level is an interrupt vector the application does not otherwise use,
// given its own NVIC priority and its own cooperative sched::Scheduler.
// SysTick pends every started level once per millisecond; the vector's
// handler polls that level's scheduler. So:
//   - a higher level preempts a lower one and the background superloop
//     (the plain Scheduler polled from main), which a long background task
//     (a TLS handshake step, a big JSON parse) can no longer stall;
//   - tasks still run to completion, all on the one MSP stack: no task
//     stacks, no context switch code, no RTOS;
//   - within a level, tasks are cooperative as before.
//
//   using Fast = sched::Level<IRQn_Type::SPDIF_RX_IRQn, 5, 4>;  // unused vector, NVIC priority 5
//   extern "C" void SPDIF_RX_IRQHandler() { Fast::run(); }
//   Fast::every(1, task_cycle);
//   Fast::start(get_tick());
//
// Data shared with a lower level or the background: take the level's lock
// there, sched::Lock<Fast::priority> lock; — a priority ceiling (BASEPRI):
// that level and everything below it are held off for the scope, higher
// interrupts keep running. Inside the level itself no lock is needed.
//
// Interrupts the level calls into (e.g. a UART whose TX it waits for) must
// have a HIGHER priority (lower number) than the level.
// ---------------------------------------------------------------------------
#include <cstddef>
#include <cstdint>

#include "core/nvic.hpp"
#include "core/scheduler.hpp"
#include "core/system.hpp"

namespace sched {

// Priority ceiling: interrupts with priority value >= Prio (that level and
// all lower ones) wait until the scope ends. BASEPRI_MAX only ever raises,
// so nested locks of different levels compose.
template<uint32_t Prio>
class Lock {
	static_assert(Prio > 0 && Prio < 16, "priority 0 cannot be masked by BASEPRI");
public:
	Lock() noexcept : saved_(read_basepri()) {
		asm volatile("msr basepri_max, %0" : : "r"(Prio << 4) : "memory");
	}
	~Lock() {
		asm volatile("msr basepri, %0" : : "r"(saved_) : "memory");
	}
	Lock(const Lock &) = delete;
	Lock &operator=(const Lock &) = delete;

private:
	static uint32_t read_basepri() noexcept {
		uint32_t v;
		asm volatile("mrs %0, basepri" : "=r"(v));
		return v;
	}
	uint32_t saved_;
};

// True in any interrupt handler, including a level (IPSR != 0)
[[gnu::always_inline]] inline bool in_interrupt() noexcept {
	uint32_t ipsr;
	asm volatile("mrs %0, ipsr" : "=r"(ipsr));
	return ipsr != 0;
}

namespace detail {
constexpr size_t MAX_LEVELS = 4;
inline TickHook level_pends[MAX_LEVELS] = { };
inline volatile size_t level_count = 0;

inline void pend_levels() {
	for (size_t i = 0; i < level_count; ++i) {
		level_pends[i]();
	}
}
} // namespace detail

template<typename IRQ, uint32_t Prio, size_t Capacity>
class Level {
	static_assert(Prio > 0 && Prio < 16);
public:
	static constexpr uint32_t priority = Prio;

	// Register a task (see Scheduler::every); before start()
	static bool every(uint32_t period_ms, TaskFn fn) noexcept {
		return sched_.every(period_ms, fn);
	}

	// Seed the deadlines, enable the vector and join the 1 ms tick
	static bool start(uint32_t now) noexcept {
		if (detail::level_count >= detail::MAX_LEVELS) {
			return false;
		}
		sched_.start(now);
		NVIC_API::SetPriority<IRQ, Prio>();
		NVIC_API::EnableIRQ<IRQ>();
		detail::level_pends[detail::level_count] = &pend;
		detail::level_count = detail::level_count + 1;
		SysTick_SetHook(&detail::pend_levels);
		return true;
	}

	// Request a run now (ISPR is write-1-to-set: a plain store)
	static void pend() noexcept {
		IRQ::SetPend::overwrite(static_cast<uint32_t>(1u << (IRQ::irq_number % 32)));
	}

	// The vector's handler
	static void run() noexcept {
		sched_.poll(get_tick());
	}

private:
	static inline Scheduler<Capacity> sched_{};
};

} // namespace sched
