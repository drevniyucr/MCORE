#pragma once
#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>

// ---------------------------------------------------------------------------
// spsc::RingBuffer — lock-free single-producer / single-consumer queue.
//
// Purpose: hand data across the ISR <-> main-loop boundary without disabling
// interrupts and without the "ISR scribbles a shared global, the loop reads it
// mid-update" race. Exactly ONE context may call push() (the producer, e.g. an
// ISR) and exactly ONE context may call pop() (the consumer, e.g. the super-
// loop). With that discipline no locks are needed.
//
// How it stays correct with no lock:
//   - Two indices. head_ is written only by the producer, tail_ only by the
//     consumer; each side only *reads* the other's index. So there is never a
//     write-write race on an index.
//   - The data slot is published with a release store of head_ and observed
//     with an acquire load of head_ (and symmetrically for tail_). That
//     release/acquire pair orders the slot write before the index update, so
//     the consumer never reads a slot the producer hasn't finished writing.
//
// On a single-core Cortex-M7 the 32-bit index loads/stores are naturally
// atomic; the acquire/release here are what stop the *compiler* from
// reordering the slot access across the index update. The same code is also
// correct on a real multi-core host, which is how the unit tests exercise it.
//
// Capacity must be a power of two (mask instead of modulo). One slot is left
// empty to tell "full" from "empty", so it holds Capacity-1 elements.
// ---------------------------------------------------------------------------

namespace spsc {

template<typename T, size_t Capacity>
class RingBuffer {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
    static_assert(Capacity >= 2, "Capacity must be >= 2");

public:
    // Producer side. Copies v into the queue. Returns false if full (v dropped).
    bool push(const T& v) noexcept {
        const uint32_t head = head_.load(std::memory_order_relaxed);   // producer owns head_
        const uint32_t next = (head + 1) & kMask;
        if (next == tail_.load(std::memory_order_acquire)) {
            return false;                                              // full
        }
        buf_[head] = v;
        head_.store(next, std::memory_order_release);                 // publish slot + advance
        return true;
    }

    // Consumer side. Moves the front element into out. Returns false if empty.
    bool pop(T& out) noexcept {
        const uint32_t tail = tail_.load(std::memory_order_relaxed);   // consumer owns tail_
        if (tail == head_.load(std::memory_order_acquire)) {
            return false;                                             // empty
        }
        out = buf_[tail];
        tail_.store((tail + 1) & kMask, std::memory_order_release);   // free slot + advance
        return true;
    }

     bool empty() const noexcept {
        return head_.load(std::memory_order_acquire) ==
               tail_.load(std::memory_order_acquire);
    }

    // Number of queued elements (a snapshot; safe to call from either side).
    uint32_t size() const noexcept {
        const uint32_t head = head_.load(std::memory_order_acquire);
        const uint32_t tail = tail_.load(std::memory_order_acquire);
        return (head - tail) & kMask;
    }

    static constexpr uint32_t capacity() noexcept { return Capacity - 1; }

private:
    static constexpr uint32_t kMask = Capacity - 1;

    std::array<T, Capacity> buf_{};
    std::atomic<uint32_t> head_{0};  // next free slot   (producer advances)
    std::atomic<uint32_t> tail_{0};  // next slot to read (consumer advances)
};

} // namespace spsc
