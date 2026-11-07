#pragma once
/*
 * "Latest value" snapshot with a sequence lock.
 *
 * One writer publishes a whole struct at its own pace and never waits.
 * Readers copy the struct out and retry if the writer was in the middle of
 * an update, so they always get a consistent copy of one publication.
 * Intended for the real-time core publishing its state to the network core.
 *
 * Like SpscRing it has no constructor (NOLOAD shared memory) and must live
 * in memory both sides see coherently. The writer calls reset() first.
 */
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace ipc {

template <typename T>
struct Seqlock {
	static_assert(std::is_trivially_copyable_v<T>, "snapshots are copied word-wise between cores");
	static_assert(sizeof(T) % 4U == 0U && alignof(T) <= 4U, "pad the snapshot to whole 32-bit words");

	std::atomic<uint32_t> seq;   // odd while a write is in progress
	uint32_t words[sizeof(T) / 4U];

	void reset() {
		for (uint32_t &w : words) {
			w = 0U;
		}
		seq.store(0U, std::memory_order_release);
	}

	// Writer side (single writer). Never blocks.
	void write(const T &value) {
		const uint32_t s = seq.load(std::memory_order_relaxed);
		seq.store(s + 1U, std::memory_order_relaxed);
		std::atomic_thread_fence(std::memory_order_release);   // odd seq visible before the data
		copy_words(reinterpret_cast<volatile uint32_t *>(words),
		           reinterpret_cast<const uint32_t *>(&value));
		seq.store(s + 2U, std::memory_order_release);           // data visible before the even seq
	}

	// Reader side. Returns false only if no consistent copy was obtained in
	// max_tries attempts (the writer kept updating), or nothing was ever
	// published (seq == 0).
	bool read(T &out, uint32_t max_tries = 16U) const {
		for (uint32_t i = 0; i < max_tries; ++i) {
			const uint32_t s1 = seq.load(std::memory_order_acquire);
			if (s1 == 0U) {
				return false;
			}
			if (s1 & 1U) {
				continue;   // write in progress
			}
			copy_words(reinterpret_cast<uint32_t *>(&out),
			           reinterpret_cast<const volatile uint32_t *>(words));
			std::atomic_thread_fence(std::memory_order_acquire);  // data read before the re-check
			if (seq.load(std::memory_order_relaxed) == s1) {
				return true;
			}
		}
		return false;
	}

	// Number of completed publications (wraps).
	uint32_t version() const { return seq.load(std::memory_order_acquire) / 2U; }

private:
	template <typename D, typename S>
	static void copy_words(D *dst, S *src) {
		for (std::size_t i = 0; i < sizeof(T) / 4U; ++i) {
			dst[i] = src[i];
		}
	}
};

} // namespace ipc
