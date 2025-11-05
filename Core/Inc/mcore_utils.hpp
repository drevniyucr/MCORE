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

//concepts
template<typename Reg, typename Bit, typename Access>
concept BitValid = std::is_same_v<typename Bit::RegType, Reg> && 
(Bit::width == 1) && (!std::is_same_v<Access, ReadOnly>) && (sizeof(Bit) > 0);

template<auto Value, uint32_t Mask>
concept ValueValid = ((static_cast<uint32_t>(Value) & ~Mask) == 0);

template<typename Access, typename T>
concept WriteTypeReq = (!std::is_same_v<Access, ReadOnly> &&
        ((std::is_integral_v<T> && std::is_unsigned_v<T>) ||
        (std::is_enum_v<T> && std::is_unsigned_v<std::underlying_type_t<T>>)));

template<typename Access, auto Val>
concept WriteValReq = (!std::is_same_v<Access, ReadOnly> &&
        ((std::is_integral_v<decltype(Val)> && std::is_unsigned_v<decltype(Val)>) ||
        (std::is_enum_v<decltype(Val)> && std::is_unsigned_v<std::underlying_type_t<decltype(Val)>>)));


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
    [[gnu::always_inline]]
    inline static void write_atomic_impl(const uint32_t clr_mask, const uint32_t set_mask) noexcept
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
    
    static_assert((Address % 4) == 0, "Register address must be 4-byte aligned");
    static_assert((Address >= 0x40000000), "Not a register address");

    using access                               = Access;
    using Register_type                        = RegT;
    static constexpr uintptr_t address         = Address;   
   
    [[gnu::always_inline]]
    inline static volatile uint32_t* ptr() noexcept
    {
        return reinterpret_cast<volatile uint32_t*>(address);
    }

    [[gnu::always_inline]]
    inline static volatile uint32_t& reg() noexcept
    {
        return *ptr();
    }

    [[gnu::always_inline]]
    inline static uint32_t read() noexcept
    {   
        static_assert(!std::is_same_v<Access, WriteOnly>, "Access denied");
        return reg();    // volatile read
    }

    //------------------- compile-time operations -------------------
    template<auto Val>
    [[gnu::always_inline]]
    inline static void overwrite() noexcept
    requires(WriteValReq<Access,Val>)
    {
        reg() = static_cast<uint32_t>(Val);   
    }

    template<auto Val>
    [[gnu::always_inline]]
    inline static void setMask() noexcept
    requires(WriteValReq<Access,Val>)
    {
        reg() |= static_cast<uint32_t>(Val);
    }

    template<auto Val>
    [[gnu::always_inline]]
    inline static void clearMask() noexcept
    requires(WriteValReq<Access,Val>)
    {
        reg() &= ~static_cast<uint32_t>(Val);
    }

    [[gnu::always_inline]]
    inline static void clear() noexcept
    {
        overwrite<0U>();   
    }
//------------------- run-time operations -------------------
  
    template<typename T>
    [[gnu::always_inline]]
    inline static void overwrite(T Val) noexcept
    requires(WriteTypeReq<Access,T>)
    {
        reg() = static_cast<uint32_t>(Val);   
    }

    template<typename T>
    [[gnu::always_inline]]
    inline static void setMask(T Val) noexcept
    requires(WriteTypeReq<Access,T>)
    {
        reg() |= static_cast<uint32_t>(Val);
    }

    template<typename T>
    [[gnu::always_inline]]
    inline static void clearMask(T Val) noexcept
    requires(WriteTypeReq<Access,T>)
    {
        reg() &= ~static_cast<uint32_t>(Val);
    }

// ------------------- Bit operations ------------------- 
    template<typename... Bits>
    [[gnu::always_inline]]
    inline static void bitSet() noexcept
    requires (BitValid<Register_type, Bits, Access> && ...) 
    {   
        constexpr uint32_t mask = (Bits::BitMsk| ...);
        setMask<mask>();
    }
    
    template<typename... Bits>
    [[gnu::always_inline]]
    inline static void bitReset() noexcept
    requires (BitValid<Register_type, Bits, Access> && ...) 
    { 
        constexpr uint32_t mask = (Bits::BitMsk | ...);
        clearMask<mask>();
    }
};
    
// ---------------------------
// Field template
// ---------------------------
template<typename Reg, uint32_t Offset, uint32_t Width>
struct Field
{
private:
    using access = typename Reg::access;
    static_assert(Width > 0 && Offset + Width <= 32, "Field out of range");
    static_assert(!(Width == 32 && Offset != 0), "32-bit field must have Offset == 0");

    // raw mask for width without shift (safe for Width==32)
    constexpr static uint32_t raw_mask_no_shift =
        (Width == 32) ? 0xFFFFFFFFU : ((1U << Width) - 1U);
    // shifted mask (safe)
    constexpr static uint32_t mask =
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
    [[gnu::always_inline]] 
    inline static void write_atomic_impl(uint32_t value) noexcept
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
    [[gnu::always_inline]] 
    inline static void modify_atomic_impl(F&& f) noexcept
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
    static constexpr uint32_t pos    = Offset;
    static constexpr uint32_t width  = Width;
    static constexpr uint32_t BitMsk = mask;
    static constexpr uint32_t BitMskNoShft = raw_mask_no_shift;


    // ---------- READ ----------
    [[gnu::always_inline]]
    inline static uint32_t read() noexcept
    requires(!std::is_same_v<access, WriteOnly>)
    {
        return (Reg::read() >> Offset) & raw_mask_no_shift;
    }

    // ---------- WRITE ----------
template<auto Val, typename Mode = access_mode<false>>
[[gnu::always_inline]]
inline static void write() noexcept
requires(ValueValid<Val, BitMskNoShft> && WriteValReq<access,Val>)
{
    constexpr uint32_t value = static_cast<uint32_t>(Val);
    if constexpr (Mode::value)
    {
      //  write_atomic_impl(value);
    }
    else
    {
        uint32_t regv = Reg::read();
        regv = (regv & ~mask) | ((value << pos) & mask);
        Reg::overwrite(regv);
    }
}

    // ---------- MODIFY ----------
    template<typename T, typename Mode = access_mode<false>>
    [[gnu::always_inline]]
    inline static void modify(T&& f) noexcept
        requires(!std::is_same_v<access, ReadOnly>)
    {
        if constexpr (Mode::value)
        {
          //  modify_atomic_impl(std::forward<T>(f));
        } else
        {
            uint32_t cur     = Reg::read();
            uint32_t field   = (cur >> Offset) & raw_mask_no_shift;
            uint32_t next    = static_cast<uint32_t>(f(field)) & raw_mask_no_shift;
            uint32_t new_val = (cur & ~mask) | (next << Offset);
            Reg::overwrite(new_val);
        }
    }

    // ---------- SET/CLEAR (only for single-bit fields) ----------
    template<typename Mode = access_mode<false>>
    [[gnu::always_inline]]
    inline static void set() noexcept
        requires(Width == 1 && !std::is_same_v<access, ReadOnly>)
    {
        if constexpr (Mode::value)
        {
           // write_atomic_impl(1u);
        } else
        {
            Reg::template setMask<mask>();
        }
    }

    template<typename Mode = access_mode<false>>
    [[gnu::always_inline]]
    inline  static void clear() noexcept
        requires(Width == 1 && !std::is_same_v<access, ReadOnly>)
    {
        if constexpr (Mode::value)
        {
           // write_atomic_impl(0u);
        } else
        {
            Reg::template clearMask<mask>();
        }
    }

    // ---------- IS_SET ----------
    [[gnu::always_inline]]
    inline static bool is_set() noexcept
        requires(Width == 1 && !std::is_same_v<access, WriteOnly>)
    {
        return (Reg::read() & mask) != 0u;
    }
};
