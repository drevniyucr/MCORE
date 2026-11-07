#pragma once
/*
 * sched — preemptive, priority-based, run-to-completion task scheduler.
 *
 * A task is a handler function with an event queue. It runs when an event is
 * posted to it, handles everything queued and returns; it never blocks. A
 * task with a higher priority preempts a lower one at once, nested on the one
 * stack (in the spirit of Super Simple Tasker / QK).
 *
 * On Cortex-M the NVIC is the scheduler: every task owns an otherwise unused
 * interrupt vector whose NVIC priority is the task priority. Posting an event
 * pends that vector, so preemption, tail-chaining and the return to the
 * preempted task are done by hardware; the kernel only keeps queues.
 *
 *   priority 1 (lowest) .. MAX_PRIO  ->  NVIC level 15 .. 8
 *   SysTick (time events)            ->  NVIC level 7, above every task
 *   NVIC levels 0..6                 ->  free for peripheral interrupts
 *
 * Interrupt handlers of any priority may post and raise. Tasks of different
 * priorities that share data use a priority-ceiling Lock (BASEPRI), which only
 * holds off the tasks up to the ceiling; interrupts above keep running.
 *
 * Two ways to wake a task:
 *   post(event)   FIFO with a payload; full queue -> the event is dropped and
 *                 counted (sizes are a design decision, watch stats().dropped)
 *   raise(sig)    signal 0..31, coalescing: raising it again before the task
 *                 ran delivers it once, never lost (interrupts, time events)
 *
 * Usage:
 *   void control(sched::Task &, const sched::Event &e);
 *   sched::TaskQ<8> control_task("control", 3, IRQn_Type::SPDIF_RX_IRQn::irq_number, control);
 *   MCORE_SCHED_VECTOR(SPDIF_RX_IRQHandler, control_task)
 *   sched::TimeEvent control_tick(control_task, SIG_TICK);
 *   ...
 *   control_tick.arm(1, 1);   // every SysTick
 *   sched::start();
 *   for (;;) sched::idle();
 */
#include <cstddef>
#include <cstdint>

namespace sched {

constexpr uint8_t NVIC_PRIO_BITS = 4;                 // STM32F7 / STM32H7
constexpr uint8_t MAX_PRIO = 8;

// NVIC priority level of task priority p (smaller level = more urgent)
constexpr uint8_t nvic_level(uint8_t prio) {
	return static_cast<uint8_t>((1U << NVIC_PRIO_BITS) - prio);
}
constexpr uint8_t TICK_NVIC_LEVEL = nvic_level(MAX_PRIO) - 1U;

struct Event {
	uint16_t sig;
	uint16_t arg16;
	uint32_t arg;
};

class Task;
using Handler = void (*)(Task &self, const Event &e);

struct TaskStats {
	uint32_t runs;          // activations (dispatch calls that found work)
	uint32_t events;        // events and signals handled
	uint32_t dropped;       // post() on a full queue
	uint8_t max_queue;      // high-water mark of the queue
	uint32_t max_cycles;    // longest single event handling, incl. preemption by higher tasks
};

class Task {
public:
	// prio 1..MAX_PRIO; irq: the NVIC vector that runs this task
	Task(const char *name, uint8_t prio, uint16_t irq, Handler handler, Event *queue, uint8_t queue_len,
	     void *ctx = nullptr);
	~Task();
	Task(const Task &) = delete;
	Task &operator=(const Task &) = delete;

	bool post(const Event &e);
	bool post(uint16_t sig, uint32_t arg = 0, uint16_t arg16 = 0) { return post(Event{ sig, arg16, arg }); }
	void raise(uint8_t sig);

	// Runs the queued work; called from the task's vector (MCORE_SCHED_VECTOR)
	void dispatch();

	const char *name() const { return name_; }
	uint8_t prio() const { return prio_; }
	uint16_t irq() const { return irq_; }
	void *ctx() const { return ctx_; }
	const TaskStats &stats() const { return stats_; }

private:
	friend Task *first_task();
	friend Task *next_task(Task *);
	void run_handler(const Event &e);

	const char *name_;
	Handler handler_;
	void *ctx_;
	Event *queue_;
	uint8_t queue_len_;
	uint8_t head_ = 0;
	uint8_t count_ = 0;
	uint8_t prio_;
	uint16_t irq_;
	volatile uint32_t signals_ = 0;
	TaskStats stats_ = {};
	Task *next_ = nullptr;
};

// A task with its queue storage
template <uint8_t QUEUE_LEN>
class TaskQ : public Task {
public:
	TaskQ(const char *name, uint8_t prio, uint16_t irq, Handler handler, void *ctx = nullptr)
	    : Task(name, prio, irq, handler, queue_, QUEUE_LEN, ctx) {}

private:
	Event queue_[QUEUE_LEN] = {};
};

// Raises a signal on a task after a number of SysTicks, once or periodically
class TimeEvent {
public:
	TimeEvent(Task &task, uint8_t sig);
	~TimeEvent();
	TimeEvent(const TimeEvent &) = delete;
	TimeEvent &operator=(const TimeEvent &) = delete;

	void arm(uint32_t ticks, uint32_t period = 0);   // ticks >= 1; period 0 = one-shot
	void disarm();
	bool armed() const { return ctr_ != 0U; }

private:
	friend void tick();
	Task &task_;
	uint8_t sig_;
	volatile uint32_t ctr_ = 0;
	uint32_t period_ = 0;
	TimeEvent *next_ = nullptr;
};

// Priority-ceiling lock as plain functions (for callback-style users):
// key = lock(ceiling); ... unlock(key);
uint32_t lock(uint8_t ceiling);
void unlock(uint32_t key);

// Priority-ceiling lock: while it exists, tasks up to `ceiling` cannot run.
// Use it in a task for data shared with tasks of a priority up to `ceiling`.
class Lock {
public:
	explicit Lock(uint8_t ceiling);
	~Lock();
	Lock(const Lock &) = delete;
	Lock &operator=(const Lock &) = delete;

private:
	uint32_t prev_;
};

struct Stats {
	uint32_t ticks;
	uint64_t busy_cycles;   // CPU cycles spent in tasks (outermost activations)
};

// Programs the NVIC for every constructed task (priority, enable) and moves
// SysTick above the tasks. False on a bad configuration (priority out of
// range, two tasks on one vector).
bool start();

// Time base: called from SysTick_Handler (SysTick_Hook) on target
void tick();

// Background loop body of main(): nothing to do on target (tasks are run by
// the NVIC); a place for WFI later
void idle();

const Stats &stats();

Task *first_task();
Task *next_task(Task *t);
Task *find_task_by_irq(uint16_t irq);

} // namespace sched

// Binds a task to its vector: MCORE_SCHED_VECTOR(SPDIF_RX_IRQHandler, control_task)
#define MCORE_SCHED_VECTOR(handler, task) \
	extern "C" void handler() { (task).dispatch(); }
