#pragma once

#include <mcore_utils.hpp>

enum class NVIC_PriorityGroup : uint32_t
{
    Group0 = 0b000,    // 0 bits for pre-emption priority, 4 bits for subpriority
    Group1 = 0b001,
    Group2 = 0b010,
    Group3 = 0b011,
    Group4 = 0b100,
    Group5 = 0b101,
    Group6 = 0b110,
    Group7 = 0b111
};

struct NVIC_API
{
    static constexpr uint32_t VECTKEY        = 0x5FAUL;
    static constexpr uint32_t VECTKEY_SHIFT  = Control::_AIRCR::VECTKEY::pos;
    static constexpr uint32_t PRIGROUP_SHIFT = Control::_AIRCR::PRIGROUP::pos;

    template<NVIC_PriorityGroup Group>
    [[gnu::always_inline]]
    static constexpr void SetPriorityGrouping() noexcept
    {
        constexpr uint32_t group_value = static_cast<uint32_t>(Group) & 0x07UL;

        // compile-time calculation
        constexpr uint32_t new_value_mask =
            (VECTKEY << VECTKEY_SHIFT) | (group_value << PRIGROUP_SHIFT);

        uint32_t reg = Control::_AIRCR::read();
        reg &= ~(Control::_AIRCR::VECTKEY::bitmsk | Control::_AIRCR::PRIGROUP::bitmsk);
        reg |= new_value_mask;

        Control::_AIRCR::overwrite(reg);
    }

    [[gnu::always_inline]] 
    static void enable_irq(void)
    {
        asm volatile("cpsie i" : : : "memory");
    }

    [[gnu::always_inline]] 
    static void disable_irq(void)
    {
        asm volatile("cpsid i" : : : "memory");
    }
};