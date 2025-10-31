#include <cstdint>
#include <type_traits>
#include <utility>

#pragma once
// ---------------------------
// Access types
// ---------------------------
struct ReadWrite {};
struct ReadOnly {};
struct WriteOnly {};

// ---------------------------
// Atomic control tags
// ---------------------------
struct atomic_t  : std::true_type  {};
struct non_atomic_t : std::false_type {};

inline constexpr atomic_t atomic_mode{};
inline constexpr non_atomic_t non_atomic_mode{};

// ---------------------------
// Register template
// ---------------------------
template<uint32_t Address, typename Access = ReadWrite>
struct Register {
    using access = Access;
    static constexpr uint32_t address = Address;
    static_assert(Address % 4 == 0, "Register address must be 4-byte aligned");

    [[gnu::always_inline]]
    static volatile uint32_t & reg() noexcept {

        return *reinterpret_cast<volatile uint32_t*> (Address);
    }

    template<typename A = Access>
    [[gnu::always_inline]]
    static uint32_t read() noexcept
    requires (!std::is_same_v<A, WriteOnly>)
    { return reg(); }

    template<typename A = Access>
    [[gnu::always_inline]]
    static void write(uint32_t v) noexcept
    requires (!std::is_same_v<A, ReadOnly>)
    { reg() = v; }

    template<typename A = Access>
    [[gnu::always_inline]]
    static void setMask(uint32_t mask) noexcept
    requires (!std::is_same_v<A, ReadOnly>)
    { reg() |= mask; }

    template<typename A = Access>
    [[gnu::always_inline]]
    static void clearMask(uint32_t mask) noexcept
    requires (!std::is_same_v<A, ReadOnly>)
    { reg() &= ~mask; }
};

// ---------------------------
// Field template
// ---------------------------
template<typename Reg, unsigned Offset, unsigned Width = 1>
struct Field {
private:
    using access = typename Reg::access;
    static_assert(Width > 0 && Offset + Width <= 32, "Field out of range");

    static constexpr uint32_t mask =
        ((Width == 32 ? 0xFFFFFFFFu : ((1u << Width) - 1u)) << Offset);

    // ------------------- atomic impl -------------------
    [[gnu::always_inline]] static void write_atomic_impl(uint32_t value) noexcept {
        volatile uint32_t* addr = &Reg::reg();
        uint32_t tmp, res;

        do {
            asm volatile (
                "ldrex %0, [%2]\n"
                "bic %0, %0, %3\n"
                "orr %0, %0, %4\n"
                "strex %1, %0, [%2]\n"
                : "=&r"(tmp), "=&r"(res)
                : "r"(addr), "r"(mask), "r"(value << Offset)
                : "memory"
            );
        } while(res != 0);
    }

    template<typename F>
    [[gnu::always_inline]] static void modify_atomic_impl(F&& f) noexcept {
        volatile uint32_t* addr = &Reg::reg();
        uint32_t tmp, res;

        do {
            uint32_t cur;
            asm volatile (
                "ldrex %0, [%2]\n"
                : "=&r"(cur)
                : "r"(addr)
                : "memory"
            );
            uint32_t field = (cur >> Offset) & ((1u << Width) - 1u);
            uint32_t next = static_cast<uint32_t>(f(field)) & ((1u << Width) - 1u);
            tmp = (cur & ~mask) | (next << Offset);
            asm volatile (
                "strex %0, %1, [%2]"
                : "=&r"(res)
                : "r"(tmp), "r"(addr)
                : "memory"
            );
        } while(res != 0);
    }

public:
    static constexpr uint32_t max() noexcept { return (Width == 32 ? 0xFFFFFFFFu : ((1u << Width) - 1u)); }

    template<typename A = access>
    [[gnu::always_inline]]
    static uint32_t read() noexcept
    requires (!std::is_same_v<A, WriteOnly>)
    { return (Reg::read() >> Offset) & max(); }

    // ---------- WRITE ----------
    template<typename A = access>
    [[gnu::always_inline]]
    static void write(uint32_t value, atomic_t) noexcept
    requires (!std::is_same_v<A, ReadOnly>)
    { write_atomic_impl(value); }
template<typename A = access>
    [[gnu::always_inline]]
    static void write(uint32_t value, non_atomic_t) noexcept
    requires (!std::is_same_v<A, ReadOnly>)
    {
        uint32_t regv = Reg::read();
        regv = (regv & ~mask) | ((value << Offset) & mask);
        Reg::write(regv);
    }

    // ---------- MODIFY ----------
    template<typename F, typename A = access>
    [[gnu::always_inline]]
    static void modify(F&& f, atomic_t) noexcept
    requires (!std::is_same_v<A, ReadOnly>)
    { modify_atomic_impl(std::forward<F>(f)); }

    template<typename F, typename A = access>
    [[gnu::always_inline]]
    static void modify(F&& f, non_atomic_t) noexcept
    requires (!std::is_same_v<A, ReadOnly>)
    {
        uint32_t cur = Reg::read();
        uint32_t field = (cur >> Offset) & max();
        uint32_t next = static_cast<uint32_t>(f(field)) & max();
        uint32_t new_val = (cur & ~mask) | (next << Offset);
        Reg::write(new_val);
    }

    // ---------- SET/CLEAR ----------
    template<typename A = access>
    [[gnu::always_inline]]
    static void set(atomic_t) noexcept
    requires (Width == 1 && !std::is_same_v<A, ReadOnly>)
    { write_atomic_impl(1); }

    template<typename A = access>
    [[gnu::always_inline]]
    static void set(non_atomic_t) noexcept
    requires (Width == 1 && !std::is_same_v<A, ReadOnly>)
    { Reg::setMask(mask); }

    template<typename A = access>
    [[gnu::always_inline]]
    static void clear(atomic_t) noexcept
    requires (Width == 1 && !std::is_same_v<A, ReadOnly>)
    { write_atomic_impl(0); }

    template<typename A = access>
    [[gnu::always_inline]]
    static void clear(non_atomic_t) noexcept
    requires (Width == 1 && !std::is_same_v<A, ReadOnly>)
    { Reg::clearMask(mask); }

    template<typename A = access>
    [[gnu::always_inline]]
    static bool is_set() noexcept
    requires (Width == 1 && !std::is_same_v<A, WriteOnly>)
    { return (Reg::read() & mask) != 0; }
};

// ---------------------------
// Namespaced RCC
// ---------------------------
static constexpr uint32_t PERIPH_BASE = 0x40000000;

static constexpr uint32_t APB1PERIPH_BASE = PERIPH_BASE;

static constexpr uint32_t AHB1PERIPH_BASE = PERIPH_BASE + 0x00020000;

static constexpr uint32_t RCC_BASE = AHB1PERIPH_BASE + 0x3800;


namespace RCC {
    using AHB1ENR_Reg = Register<RCC_BASE, ReadWrite>;
    struct AHB1ENR {
        using GPIOAEN = Field<AHB1ENR_Reg, 0, 1>;
        using GPIOBEN = Field<AHB1ENR_Reg, 1, 1>;
        using GPIOCEN = Field<AHB1ENR_Reg, 2, 1>;
        // ... добавь остальные биты по аналогии
    };
}
