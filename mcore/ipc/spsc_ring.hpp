#pragma once
/*
 * Lock-free single-producer / single-consumer ring of fixed-size messages.
 *
 * Meant for memory shared between two cores (or a core and an ISR): exactly
 * one side calls push(), exactly one side calls pop(). Neither side ever
 * blocks: push() fails when the ring is full, pop() fails when it is empty.
 *
 * The object must live in memory both sides see coherently (on the
 * STM32H7 Cortex-M7: a non-cacheable MPU region). It has no constructor
 * so that it can sit in a NOLOAD section; one side calls reset() before
 * the other side starts using it.
 */
#include <atomic>
#include <cstdint>
#include <type_traits>

namespace ipc {

template <typename T, uint32_t N>
struct SpscRing {
	static_assert(N >= 2 && (N & (N - 1U)) == 0U, "N must be a power of two");
	static_assert(std::is_trivially_copyable_v<T>, "messages are copied byte-wise between cores");
	static_assert(std::atomic<uint32_t>::is_always_lock_free, "needs lock-free 32-bit atomics");

	// Free-running counters; index = counter % N. head is written only by
	// the producer, tail only by the consumer.
	std::atomic<uint32_t> head;
	std::atomic<uint32_t> tail;
	T slots[N];

	void reset() {
		head.store(0U, std::memory_order_relaxed);
		tail.store(0U, std::memory_order_relaxed);
		std::atomic_thread_fence(std::memory_order_release);
	}

	// Producer side
	bool push(const T &msg) {
		const uint32_t h = head.load(std::memory_order_relaxed);
		const uint32_t t = tail.load(std::memory_order_acquire);
		if ((h - t) >= N) {
			return false;   // full: the caller decides (drop, count, retry later)
		}
		slots[h & (N - 1U)] = msg;
		head.store(h + 1U, std::memory_order_release);   // publish after the copy
		return true;
	}

	// Consumer side
	bool pop(T &out) {
		const uint32_t t = tail.load(std::memory_order_relaxed);
		const uint32_t h = head.load(std::memory_order_acquire);
		if (h == t) {
			return false;
		}
		out = slots[t & (N - 1U)];
		tail.store(t + 1U, std::memory_order_release);   // free the slot after the copy
		return true;
	}

	// Either side; a snapshot that may be stale by the time it is used.
	uint32_t size() const {
		return head.load(std::memory_order_acquire) - tail.load(std::memory_order_acquire);
	}
	static constexpr uint32_t capacity() { return N; }
};

} // namespace ipc
