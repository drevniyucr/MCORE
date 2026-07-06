#pragma once
#include <cstdint>
#include <cstddef>
#include <array>

// ---------------------------------------------------------------------------
// Cooperative software-timer scheduler — a small replacement for hand-written
//   if ((get_tick() - last) >= period) { last = get_tick(); ... }
// blocks in the super-loop. No heap, no RTOS, no preemption: tasks are plain
// callbacks run to completion, in registration order, when their period has
// elapsed.
//
// The scheduler is intentionally hardware-agnostic — it never reads the clock
// itself; the caller passes `now` into poll(). That keeps it unit-testable on
// the host with a fake clock (see tests/test_scheduler.cpp).
//
// Deadlines use unsigned tick subtraction (now - last), so a single 32-bit
// wrap of the millisecond tick is handled correctly.
// ---------------------------------------------------------------------------

namespace sched {

using TaskFn = void (*)();

template<size_t Capacity>
class Scheduler {
public:
    // Register a task run every `period_ms` (period_ms == 0 -> every poll).
    // Returns false if the table is full or fn is null.
    bool every(uint32_t period_ms, TaskFn fn) noexcept {
        if (count_ >= Capacity || fn == nullptr) return false;
        tasks_[count_++] = Task{fn, /*last=*/0, period_ms};
        return true;
    }

    // Seed every task's deadline to `now`; call once before the poll loop so
    // the first firing happens one full period later (not immediately).
    void start(uint32_t now) noexcept {
        for (size_t i = 0; i < count_; ++i) tasks_[i].last = now;
    }

    // Run every task whose period has elapsed. Call as often as possible.
    void poll(uint32_t now) noexcept {
        for (size_t i = 0; i < count_; ++i) {
            Task& t = tasks_[i];
            if (t.period == 0 || (now - t.last) >= t.period) {
                t.last = now;
                t.fn();
            }
        }
    }

    size_t size() const noexcept { return count_; }

private:
    struct Task {
        TaskFn   fn     = nullptr;
        uint32_t last   = 0;
        uint32_t period = 0;
    };
    std::array<Task, Capacity> tasks_{};
    size_t count_ = 0;
};

} // namespace sched
