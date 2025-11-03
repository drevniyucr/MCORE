#pragma once

#include <cassert>
#include <cstdint>
#include <type_traits>
#include <utility>


// Если вы хотите использовать CMSIS-интринсики вместо inline asm,
// определите этот макрос (например, в CMake): -DUSE_CMSIS_ATOMICS=1
// Тогда должны быть доступны __LDREXW / __STREXW или аналоги.
#if defined(USE_CMSIS_ATOMICS)
#include "cmsis_gcc.h"    // убедитесь, что у вас есть заголовок с LDREX/STREX-интринсиками
#endif

// ---------------------------
// Access tags
// ---------------------------
struct ReadWrite{};
struct ReadOnly{};
struct WriteOnly{};
// ---------------------------
// Atomic control tags
// ---------------------------
template<bool IsAtomic>
struct access_mode : std::bool_constant<IsAtomic> {};

inline constexpr access_mode<true>  atomic_mode{};
inline constexpr access_mode<false> non_atomic_mode{};


template<typename Reg, typename Bit>
concept BitValid = std::is_same_v<typename Bit::RegType, Reg> && (Bit::width == 1);

// ---------------------------
// Register template
// ---------------------------
// Address: uintptr_t (compile-time), Access: ReadWrite/ReadOnly/WriteOnly
// SupportsAtomic: true если регистр можно изменять через LDREX/STREX (по умолчанию true)
template<uintptr_t Address, typename Access, typename RegT>
struct Register
{
    private:
    // ------------------- atomic impl -------------------
     #if defined(USE_CMSIS_ATOMICS)
    // Если у вас есть CMSIS-интринсики (проверьте имена в вашей среде)
    [[gnu::always_inline]] static void write_atomic_impl(uint32_t value) noexcept
    {
        volatile uint32_t* addr = Reg::ptr();
        uint32_t           expected, result;
        const uint32_t     set_bits   = (value & raw_mask_no_shift) << Offset;
        const uint32_t     clear_mask = mask;

        do
        {
            expected     = __LDREXW(reinterpret_cast<volatile uint32_t*>(addr));
            uint32_t tmp = (expected & ~clear_mask) | set_bits;
            result       = __STREXW(tmp, reinterpret_cast<volatile uint32_t*>(addr));
        } while (result != 0);
        __DMB();    // memory barrier if needed
    }

    template<typename F>
    [[gnu::always_inline]] static void modify_atomic_impl(F&& f) noexcept
    {
        volatile uint32_t* addr = Reg::ptr();
        uint32_t           res;
        do
        {
            uint32_t cur   = __LDREXW(reinterpret_cast<volatile uint32_t*>(addr));
            uint32_t field = (cur >> Offset) & raw_mask_no_shift;
            uint32_t next  = static_cast<uint32_t>(f(field)) & raw_mask_no_shift;
            uint32_t tmp   = (cur & ~mask) | (next << Offset);
            res            = __STREXW(tmp, reinterpret_cast<volatile uint32_t*>(addr));
        } while (res != 0);
        __DMB();
    }
#else
    // Inline ASM fallback (ARMv7-M style LDREX/STREX)
    [[gnu::always_inline]] static void write_atomic_impl(const uint32_t clr_mask, const uint32_t set_mask) noexcept
    {
        volatile uint32_t* addr = ptr();
        uint32_t           tmp, res;

        do{
            asm volatile("ldrex %0, [%3]\n"
                         "bic   %0, %0, %4\n"
                         "orr   %0, %0, %5\n"
                         "strex %1, %0, [%3]\n"
                         : "=&r"(tmp), "=&r"(res)
                         : "0"(tmp), "r"(addr), "r"(clr_mask), "r"(set_mask)
                         : "memory");
        } while (res != 0);
        asm volatile("" ::: "memory");
    }

    // template<typename F>
    // [[gnu::always_inline]] static void modify_atomic_impl(F&& f) noexcept
    // {
    //     volatile uint32_t* addr = Reg::ptr();
    //     uint32_t           cur, tmp, res;

    //     do
    //     {
    //         asm volatile("ldrex %0, [%3]\n" : "=&r"(cur) : "r"(addr) : "memory");

    //         uint32_t field = (cur >> Offset) & raw_mask_no_shift;
    //         uint32_t next  = static_cast<uint32_t>(f(field)) & raw_mask_no_shift;
    //         tmp            = (cur & ~mask) | (next << Offset);

    //         asm volatile("strex %0, %1, [%2]\n"
    //                      : "=&r"(res)
    //                      : "r"(tmp), "r"(addr)
    //                      : "memory");
    //     } while (res != 0);
    //     asm volatile("" ::: "memory");
    // }
    #endif
    public:

    using access                               = Access;
    using Register_type                        = RegT;
    static constexpr uintptr_t address         = Address;   
   
    static_assert((Address % 4) == 0, "Register address must be 4-byte aligned");

    [[gnu::always_inline]]
    static volatile uint32_t* ptr() noexcept
    {
        return reinterpret_cast<volatile uint32_t*>(address);
    }

    [[gnu::always_inline]]
    static volatile uint32_t& reg() noexcept
    {
        return *ptr();
    }

    template<typename A = Access>
    [[gnu::always_inline]]
    static uint32_t read() noexcept
    requires(!std::is_same_v<A, WriteOnly>)
    {
        return reg();    // volatile read
    }

    template<typename A = Access, typename T = uint32_t>
    [[gnu::always_inline]]
    static void overwrite(T v) noexcept
    requires(!std::is_same_v<A, ReadOnly> &&
        ((std::is_integral_v<T> && std::is_unsigned_v<T>) ||
        (std::is_enum_v<T> && std::is_unsigned_v<std::underlying_type_t<T>>)))
    {
        reg() = static_cast<uint32_t>(v);   
    }
   
    [[gnu::always_inline]]
    static void clear() noexcept
    {
        overwrite(0U);   
    }

    template<typename A = Access, typename T = uint32_t>
    [[gnu::always_inline]]
    static void setMask(T mask) noexcept
    requires(!std::is_same_v<A, ReadOnly> &&
        ((std::is_integral_v<T> && std::is_unsigned_v<T>) ||
        (std::is_enum_v<T> && std::is_unsigned_v<std::underlying_type_t<T>>)))
    {
        reg() |= static_cast<uint32_t>(mask);
    }

    template<typename A = Access, typename T = uint32_t>
    [[gnu::always_inline]]
    static void clearMask(T mask) noexcept
    requires(!std::is_same_v<A, ReadOnly> &&
        ((std::is_integral_v<T> && std::is_unsigned_v<T>) ||
        (std::is_enum_v<T> && std::is_unsigned_v<std::underlying_type_t<T>>)))
    {
        reg() &= ~static_cast<uint32_t>(mask);
    }


    template<typename... Bits>
    [[gnu::always_inline]]
    static void bitSet() noexcept
    requires (BitValid<Register_type, Bits> && ...) 
    {   
        static_assert(!std::is_same_v<Access, ReadOnly>, "Acces denied");
        static_assert(sizeof...(Bits) > 0, "Must provide at least one bit");
        // Проверяем, что все Bits принадлежат этому регистру и одно-битные
        // static_assert(((std::is_same<typename Bits::RegType, Register>::value && Bits::width == 1) && ...),
        //               "All bits must belong to this register and be single-bit fields");
        constexpr uint32_t mask = (Bits::bitmsk| ...);
        // if constexpr (std::is_same_v<mode, atomic_mode>)
        //     asm("nop");// write_atomic_impl(mask);
        //  else
        setMask(mask);
    }
    
    template<typename... Bits>
    static inline void bitReset() noexcept {
        static_assert(std::is_same_v<Access, ReadOnly>, "Acces denied");
        static_assert(sizeof...(Bits) > 0, "Must provide at least one bit");
        static_assert(((std::is_same<typename Bits::RegType, Register>::value && Bits::width == 1) && ...),
                      "All bits must belong to this register and be single-bit fields");
        constexpr uint32_t mask = (Bits::mask | ...);
        clearMask(mask);
    }
};
    
// ---------------------------
// Field template
// ---------------------------
template<typename Reg, unsigned Offset, unsigned Width = 1>
struct Field
{
private:
    using access = typename Reg::access;
    static_assert(Width > 0 && Offset + Width <= 32, "Field out of range");
    static_assert(!(Width == 32 && Offset != 0), "32-bit field must have Offset == 0");

    // raw mask for width without shift (safe for Width==32)
    static constexpr uint32_t raw_mask_no_shift =
        (Width == 32) ? 0xFFFFFFFFu : ((1u << Width) - 1u);
    // shifted mask (safe)
    static constexpr uint32_t mask =
        (Width == 32) ? 0xFFFFFFFFu : (raw_mask_no_shift << Offset);

    // ------------------- atomic impl -------------------
#if defined(USE_CMSIS_ATOMICS)
    // Если у вас есть CMSIS-интринсики (проверьте имена в вашей среде)
    [[gnu::always_inline]] static void write_atomic_impl(uint32_t value) noexcept
    {
        volatile uint32_t* addr = Reg::ptr();
        uint32_t           expected, result;
        const uint32_t     set_bits   = (value & raw_mask_no_shift) << Offset;
        const uint32_t     clear_mask = mask;

        do
        {
            expected     = __LDREXW(reinterpret_cast<volatile uint32_t*>(addr));
            uint32_t tmp = (expected & ~clear_mask) | set_bits;
            result       = __STREXW(tmp, reinterpret_cast<volatile uint32_t*>(addr));
        } while (result != 0);
        __DMB();    // memory barrier if needed
    }

    template<typename F>
    [[gnu::always_inline]] static void modify_atomic_impl(F&& f) noexcept
    {
        volatile uint32_t* addr = Reg::ptr();
        uint32_t           res;
        do
        {
            uint32_t cur   = __LDREXW(reinterpret_cast<volatile uint32_t*>(addr));
            uint32_t field = (cur >> Offset) & raw_mask_no_shift;
            uint32_t next  = static_cast<uint32_t>(f(field)) & raw_mask_no_shift;
            uint32_t tmp   = (cur & ~mask) | (next << Offset);
            res            = __STREXW(tmp, reinterpret_cast<volatile uint32_t*>(addr));
        } while (res != 0);
        __DMB();
    }
#else
    // Inline ASM fallback (ARMv7-M style LDREX/STREX)
    [[gnu::always_inline]] static void write_atomic_impl(uint32_t value) noexcept
    {
        volatile uint32_t* addr = Reg::ptr();
        const uint32_t     clear_mask = mask;
        const uint32_t     set_bits   = (value & raw_mask_no_shift) << Offset;
        uint32_t           tmp, res;

        do
        {
            asm volatile("ldrex %0, [%3]\n"
                         "bic   %0, %0, %4\n"
                         "orr   %0, %0, %5\n"
                         "strex %1, %0, [%3]\n"
                         : "=&r"(tmp), "=&r"(res)
                         : "0"(tmp), "r"(addr), "r"(clear_mask), "r"(set_bits)
                         : "memory");
        } while (res != 0);
        asm volatile("" ::: "memory");
    }

    template<typename F>
    [[gnu::always_inline]] static void modify_atomic_impl(F&& f) noexcept
    {
        volatile uint32_t* addr = Reg::ptr();
        uint32_t           cur, tmp, res;

        do
        {
            asm volatile("ldrex %0, [%3]\n" : "=&r"(cur) : "r"(addr) : "memory");

            uint32_t field = (cur >> Offset) & raw_mask_no_shift;
            uint32_t next  = static_cast<uint32_t>(f(field)) & raw_mask_no_shift;
            tmp            = (cur & ~mask) | (next << Offset);

            asm volatile("strex %0, %1, [%2]\n"
                         : "=&r"(res)
                         : "r"(tmp), "r"(addr)
                         : "memory");
        } while (res != 0);
        asm volatile("" ::: "memory");
    }
#endif

public:
    // ------------------- public meta-info -------------------
    using RegType = Reg;
    static constexpr unsigned pos    = Offset;
    static constexpr unsigned width  = Width;
    static constexpr uint32_t bitmsk = mask;
    static constexpr uint32_t max() noexcept
    {
        return (Width == 32 ? 0xFFFFFFFFu : ((1u << Width) - 1u));
    }

    // ---------- READ ----------
    template<typename A = access>
    [[gnu::always_inline]]
    static uint32_t read() noexcept
        requires(!std::is_same_v<A, WriteOnly>)
    {
        return (Reg::read() >> Offset) & max();
    }

    // ---------- WRITE ----------
   template<typename A = access,typename Mode = access_mode<false>>
    [[gnu::always_inline]]
    static void write(uint32_t value, Mode = {}) noexcept
        requires(!std::is_same_v<A, ReadOnly>)
    {
        if constexpr (Mode::value)
        {
            static_assert((value & ~raw_mask_no_shift) == 0 &&
                          "Field::write: value out of range");
            write_atomic_impl(value);
        } else
        {
            static_assert((value & ~raw_mask_no_shift) == 0 &&
                          "Field::write: value out of range");
            uint32_t regv = Reg::read();
            regv          = (regv & ~mask) | ((value << Offset) & mask);
            Reg::write(regv);
        }
    }

    // ---------- MODIFY ----------
    template<typename F, typename A = access, typename Mode = access_mode<false>>
    [[gnu::always_inline]]
    static void modify(F f, Mode = {}) noexcept
        requires(!std::is_same_v<A, ReadOnly>)
    {
        if constexpr (Mode::value)
        {
            modify_atomic_impl(std::forward<F>(f));
        } else
        {
            uint32_t cur     = Reg::read();
           // uint32_t field   = (cur >> Offset) & max();
            uint32_t next    = static_cast<uint32_t>(f) & max();
            uint32_t new_val = (cur & ~mask) | (next << Offset);
            Reg::overwrite(new_val);
        }
    }

    // ---------- SET/CLEAR (only for single-bit fields) ----------
    template<typename A = access, typename Mode = access_mode<false>>
    [[gnu::always_inline]]
    static void set(Mode = {}) noexcept
        requires(Width == 1 && !std::is_same_v<A, ReadOnly>)
    {
        if constexpr (Mode::value)
        {
            write_atomic_impl(1u);
        } else
        {
            Reg::setMask(mask);
        }
    }

    template<typename A = access, typename Mode = access_mode<false>>
    [[gnu::always_inline]]
    static void clear(Mode = {}) noexcept
        requires(Width == 1 && !std::is_same_v<A, ReadOnly>)
    {
        if constexpr (Mode::value)
        {
            write_atomic_impl(0u);
        } else
        {
            Reg::clearMask(mask);
        }
    }

    // ---------- IS_SET ----------
    template<typename A = access>
    [[gnu::always_inline]]
    static bool is_set() noexcept
        requires(Width == 1 && !std::is_same_v<A, WriteOnly>)
    {
        return (Reg::read() & mask) != 0u;
    }
};