#pragma once

#include <cassert>
#include <cstdint>
#include <type_traits>
#include <utility>

// Atomic (LDREX/STREX) RMW helper, opt-in via the atomic_mode overloads.
// Define -DUSE_CMSIS_ATOMICS=1 to use __LDREXW/__STREXW intrinsics instead of
// inline asm. See core/atomic.hpp.
#include "core/atomic.hpp"

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

// ---------------------------
// Concepts
// ---------------------------
template<typename Reg, typename Bit, typename Access>
concept BitValid = std::is_same_v<typename Bit::RegType, Reg> && 
                   (Bit::width == 1) && 
                   (!std::is_same_v<Access, ReadOnly>) && 
                   (sizeof(Bit) > 0);

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
// Supports atomic operations via LDREX/STREX (default true)
template<uintptr_t Address, typename Access, typename RegT>
struct Register
{
public:
    static_assert((Address % 4) == 0, "Register address must be 4-byte aligned");
    static_assert((Address >= 0x40000000), "Address not in valid peripheral range");

    using access = Access;
    using Register_type = RegT;
    static constexpr uintptr_t address = Address;   
   
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
        static_assert(!std::is_same_v<Access, WriteOnly>, "Access denied: WriteOnly register");
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
        constexpr uint32_t mask = (Bits::BitMsk | ...);
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

    // Raw mask for width without shift (safe for Width==32)
    constexpr static uint32_t raw_mask_no_shift =
        (Width >= 32) ? 0xFFFFFFFFU : ((1U << Width) - 1U);
    // Shifted mask (safe)
    constexpr static uint32_t mask =
        (Width >= 32) ? 0xFFFFFFFFU : (raw_mask_no_shift << Offset);

    // Atomic RMW lives in core/atomic.hpp (RegAtomic); Field just delegates.

public:
    // ------------------- public meta-info -------------------
    using RegType = Reg;
    static constexpr uint32_t pos = Offset;
    static constexpr uint32_t width = Width;
    static constexpr uint32_t BitMsk = mask;
    static constexpr uint32_t BitMskNoShft = raw_mask_no_shift;

    // ---------- READ ----------
    [[gnu::always_inline]]
    inline static uint32_t read() noexcept
    requires(!std::is_same_v<access, WriteOnly>)
    {
        return (Reg::read() >> pos) & BitMskNoShft;
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
            RegAtomic::write_masked(Reg::ptr(), mask, (value << pos) & mask);
        }
        else
        {
            uint32_t regv = Reg::read();
            regv = (regv & ~mask) | ((value << pos) & mask);
            Reg::overwrite(regv);
        }
    }

    template<typename T, typename Mode = access_mode<false>>
    [[gnu::always_inline]]
    inline static void write(T Val) noexcept
    requires(!std::is_same_v<access, ReadOnly>)
    {
        uint32_t value = static_cast<uint32_t>(Val);
        // Validate value fits in field (only in debug builds)
        #ifndef NDEBUG
        assert((value & ~BitMskNoShft) == 0U && "Value exceeds field width");
        #endif
        if constexpr (Mode::value)
        {
            RegAtomic::write_masked(Reg::ptr(), mask, (value & BitMskNoShft) << pos);
        }
        else
        {
            uint32_t regv = Reg::read();
            regv = (regv & ~mask) | ((value & BitMskNoShft) << pos);
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
            RegAtomic::modify_masked(Reg::ptr(), mask, pos, BitMskNoShft, std::forward<T>(f));
        }
        else
        {
            uint32_t cur = Reg::read();
            uint32_t field = (cur >> pos) & BitMskNoShft;
            uint32_t next = static_cast<uint32_t>(f(field)) & BitMskNoShft;
            uint32_t new_val = (cur & ~mask) | (next << pos);
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
            RegAtomic::write_masked(Reg::ptr(), mask, mask);
        }
        else
        {
            Reg::template setMask<mask>();
        }
    }

    template<typename Mode = access_mode<false>>
    [[gnu::always_inline]]
    inline static void clear() noexcept
    requires(Width == 1 && !std::is_same_v<access, ReadOnly>)
    {
        if constexpr (Mode::value)
        {
            RegAtomic::write_masked(Reg::ptr(), mask, 0U);
        }
        else
        {
            Reg::template clearMask<mask>();
        }
    }

    // ---------- IS_SET ----------
    [[gnu::always_inline]]
    inline static bool is_set() noexcept
    requires(Width == 1 && !std::is_same_v<access, WriteOnly>)
    {
        return (Reg::read() & mask) != 0U;
    }
    [[gnu::always_inline]]
    inline static uint32_t Val2Mask(uint32_t Val) noexcept
    requires(!std::is_same_v<access, WriteOnly> && !std::is_same_v<access, ReadOnly>)
    {
        return (Val & BitMskNoShft) << pos;
    }

};

