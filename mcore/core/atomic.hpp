#pragma once
#include <cstdint>

// ---------------------------------------------------------------------------
// RegAtomic — atomic read-modify-write of a masked region inside a 32-bit MMIO
// register, using the Cortex-M LDREX/STREX exclusive monitor.
//
// This used to live inside the Field<> template. It is opt-in (only reachable
// via the atomic_mode overloads) and pulls in ~90 lines of inline asm, so it
// is kept out of the hot register-access core and included here as a separate,
// self-contained component.
//
// On non-ARM (host unit-test) builds there is no exclusive monitor, so we fall
// back to a plain RMW. Host tests do not exercise real hardware concurrency,
// so this is sufficient — and it keeps regs.hpp/utils.hpp compilable natively.
// ---------------------------------------------------------------------------

#if defined(USE_CMSIS_ATOMICS)
#include "cmsis_gcc.h"    // __LDREXW / __STREXW intrinsics
#endif

struct RegAtomic {
#if defined(__ARM_ARCH)

    // *addr = (*addr & ~clear_mask) | set_bits, atomically.
    // set_bits must already be masked/shifted into place by the caller.
    [[gnu::always_inline]] static inline
    void write_masked(volatile uint32_t* addr, uint32_t clear_mask, uint32_t set_bits) noexcept
    {
    #if defined(USE_CMSIS_ATOMICS)
        uint32_t expected, result;
        do {
            expected = __LDREXW(addr);
            uint32_t tmp = (expected & ~clear_mask) | set_bits;
            result = __STREXW(tmp, addr);
        } while (result != 0);
        asm volatile("dmb 0xF" ::: "memory");
    #else
        uint32_t tmp, res;
        do {
            asm volatile(
                "ldrex %0, [%3]\n"
                "bic   %0, %0, %4\n"
                "orr   %0, %0, %5\n"
                "strex %1, %0, [%3]\n"
                : "=&r"(tmp), "=&r"(res)
                : "0"(tmp), "r"(addr), "r"(clear_mask), "r"(set_bits)
                : "memory");
        } while (res != 0);
        asm volatile("dmb" ::: "memory");
    #endif
    }

    // Atomically apply f to the field value located at [offset, offset+width).
    // field_mask is the width mask (unshifted); clear_mask is (field_mask << offset).
    template<typename F>
    [[gnu::always_inline]] static inline
    void modify_masked(volatile uint32_t* addr, uint32_t clear_mask,
                       uint32_t offset, uint32_t field_mask, F&& f) noexcept
    {
    #if defined(USE_CMSIS_ATOMICS)
        uint32_t expected, result;
        do {
            expected = __LDREXW(addr);
            uint32_t field = (expected >> offset) & field_mask;
            uint32_t next  = static_cast<uint32_t>(f(field)) & field_mask;
            uint32_t tmp   = (expected & ~clear_mask) | ((next << offset) & clear_mask);
            result = __STREXW(tmp, addr);
        } while (result != 0);
        asm volatile("dmb 0xF" ::: "memory");
    #else
        uint32_t expected, tmp, res;
        do {
            asm volatile("ldrex %0, [%1]\n" : "=&r"(expected) : "r"(addr) : "memory");
            uint32_t field = (expected >> offset) & field_mask;
            uint32_t next  = static_cast<uint32_t>(f(field)) & field_mask;
            uint32_t set_bits = (next << offset) & clear_mask;
            // NOTE: bic from `expected` (not the output reg). The original in-Field
            // version read an uninitialized register here — a latent bug on the
            // (previously unused) atomic modify path, fixed during extraction.
            asm volatile(
                "bic   %0, %2, %3\n"
                "orr   %0, %0, %4\n"
                "strex %1, %0, [%5]\n"
                : "=&r"(tmp), "=&r"(res)
                : "r"(expected), "r"(clear_mask), "r"(set_bits), "r"(addr)
                : "memory");
        } while (res != 0);
        asm volatile("dmb" ::: "memory");
    #endif
    }

#else  // ---- Host fallback: plain RMW (NOT atomic; host tests only) ----

    [[gnu::always_inline]] static inline
    void write_masked(volatile uint32_t* addr, uint32_t clear_mask, uint32_t set_bits) noexcept
    {
        *addr = (*addr & ~clear_mask) | set_bits;
    }

    template<typename F>
    [[gnu::always_inline]] static inline
    void modify_masked(volatile uint32_t* addr, uint32_t clear_mask,
                       uint32_t offset, uint32_t field_mask, F&& f) noexcept
    {
        uint32_t cur   = *addr;
        uint32_t field = (cur >> offset) & field_mask;
        uint32_t next  = static_cast<uint32_t>(f(field)) & field_mask;
        *addr = (cur & ~clear_mask) | ((next << offset) & clear_mask);
    }

#endif
};
