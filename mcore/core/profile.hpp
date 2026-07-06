#pragma once
#include <cstdint>
#include "core/regs.hpp"

// ---------------------------------------------------------------------------
// Cycle-accurate profiling via the Cortex-M7 DWT cycle counter.
//
// Requires the DWT cycle counter to be running — DWT_Init() (core/system.cpp)
// enables it (TRCENA + CYCCNTENA). At 216 MHz one tick == 1/216 µs.
//
// Two ways to use it:
//
//   uint32_t c = prof::measure([]{ do_work(); });      // functional
//
//   uint32_t c;
//   { prof::ScopeCycles _t{&c}; do_work(); }           // RAII scope timer
//   print("work took %lu cycles\n", c);
//
// The counter is 32-bit and wraps every ~19.9 s at 216 MHz; unsigned
// subtraction (end - start) stays correct across a single wrap.
// ---------------------------------------------------------------------------

namespace prof {

// Raw DWT cycle counter snapshot.
[[gnu::always_inline]] inline uint32_t cycles() noexcept {
    return DWT::_CYCCNT::read();
}

// Cycle cost of invoking f(). Returns elapsed CYCCNT ticks.
template<typename F>
[[gnu::always_inline]] inline uint32_t measure(F&& f) noexcept {
    const uint32_t start = DWT::_CYCCNT::read();
    f();
    return DWT::_CYCCNT::read() - start;
}

// RAII scope timer: samples CYCCNT at construction and, on scope exit, writes
// the elapsed cycle count into *out. `out` must outlive the timer.
class ScopeCycles {
public:
    [[gnu::always_inline]] explicit ScopeCycles(uint32_t* out) noexcept
        : out_(out), start_(DWT::_CYCCNT::read()) {}

    [[gnu::always_inline]] ~ScopeCycles() noexcept {
        *out_ = DWT::_CYCCNT::read() - start_;
    }

    ScopeCycles(const ScopeCycles&) = delete;
    ScopeCycles& operator=(const ScopeCycles&) = delete;

private:
    uint32_t* out_;
    uint32_t  start_;
};

} // namespace prof
