#pragma once

#include "mcore_nvic_def.hpp"
#include "mcore_utils.hpp"

enum class NVIC_PriorityGroup : uint32_t
{
    Group0 = 0b111,    // 0 bits for pre-emption priority, 4 bits for subpriority
    Group1 = 0b110,
    Group2 = 0b101,
    Group3 = 0b100,
    Group4 = 0b011
};


struct SYS_IRQn
{
    struct MemoryManagement_IRQn { // < Window WatchDog Interrupt
        using Priority = SCB::_SHPR1::PRI_4;
    };
    struct BusFault_IRQn {// < Bus Fault Interrupt
        using Priority = SCB::_SHPR1::PRI_5;
    };
    struct UsageFault_IRQn {// < Usage Fault Interrupt
        using Priority = SCB::_SHPR1::PRI_6;
    };
    struct SVCall_IRQn {// < SV Call Interrupt
        using Priority = SCB::_SHPR2::PRI_11;
    };
    struct DebugMonitor_IRQn {// < Debug Monitor Interrupt
        using Priority = SCB::_SHPR3::PRI_12;
    };
    struct PendSV_IRQn {// < Pend SV Interrupt
        using Priority = SCB::_SHPR3::PRI_14;
    };
    struct SysTick_IRQn {// < SysTick Interrupt
        using Priority = SCB::_SHPR3::PRI_15;
    };
//   NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt*/
};


struct NVIC_API
{
    static constexpr uint32_t VECTKEY        = 0x5FAUL;
    static constexpr uint32_t VECTKEY_SHIFT  = SCB::_AIRCR::VECTKEY::pos;
    static constexpr uint32_t PRIGROUP_SHIFT = SCB::_AIRCR::PRIGROUP::pos;

    template<NVIC_PriorityGroup Group>
    [[gnu::always_inline]]
    static constexpr void SetPriorityGrouping() noexcept
    {
        constexpr uint32_t group_value = static_cast<uint32_t>(Group) & 0x07UL;

        // compile-time calculation
        constexpr uint32_t new_value_mask =
            (VECTKEY << VECTKEY_SHIFT) | (group_value << PRIGROUP_SHIFT);

        uint32_t reg = SCB::_AIRCR::read();
        reg &= ~(SCB::_AIRCR::VECTKEY::BitMsk | SCB::_AIRCR::PRIGROUP::BitMsk);
        reg |= new_value_mask;
        SCB::_AIRCR::overwrite(reg);
    }

    template<typename IRQ, uint32_t priority>
    [[gnu::always_inline]] 
    static void SetPriority()
    requires(priority < 16)
    {   
        IRQ::Priority::template write(priority << 4U);
    }


    template<typename IRQ>
    [[gnu::always_inline]] 
    static inline void enable_irq()
    {
        IRQ::Enable::setMask(static_cast<uint32_t>(1<<(IRQ::irq_number % 32)));
    }

    [[gnu::always_inline]] 
    static void enable_irq_global(void)
    {
        asm volatile("cpsie i" : : : "memory");
    }

    [[gnu::always_inline]] 
    static void disable_irq(void)
    {
        asm volatile("cpsid i" : : : "memory");
    }
};
