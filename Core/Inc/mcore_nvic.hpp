#pragma once

#include <mcore_utils.hpp>

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
    using MemoryManagement_IRQn = SCB::_SHPR1::PRI_4; // < Memory Management Interrupt
    using BusFault_IRQn = SCB::_SHPR1::PRI_5; // < Bus Fault Interrupt
    using UsageFault_IRQn = SCB::_SHPR1::PRI_6; // < Usage Fault Interrupt
    using SVCall_IRQn = SCB::_SHPR2::PRI_11; // < SV Call Interrupt
    using DebugMonitor_IRQn = SCB::_SHPR3::PRI_12; // < Debug Monitor Interrupt
    using PendSV_IRQn = SCB::_SHPR3::PRI_14; // < Pend SV Interrupt
    using SysTick_IRQn = SCB::_SHPR3::PRI_15; // < SysTick Interrupt
//   NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt*/
};


// Auto-generated NVIC IRQ mapping
struct IRQn_Type {
    using WWDG_IRQn = NVIC::_NVIC_IPR0::PRI_N0; // < Window WatchDog Interrupt
    using PVD_IRQn = NVIC::_NVIC_IPR0::PRI_N1; // < PVD through EXTI Line detection Interrupt
    using TAMP_STAMP_IRQn = NVIC::_NVIC_IPR0::PRI_N2; // < Tamper and TimeStamp interrupts through the EXTI line
    using RTC_WKUP_IRQn = NVIC::_NVIC_IPR0::PRI_N3; // < RTC Wakeup interrupt through the EXTI line
    using FLASH_IRQn = NVIC::_NVIC_IPR1::PRI_N0; // < FLASH global Interrupt
    using RCC_IRQn = NVIC::_NVIC_IPR1::PRI_N1; // < RCC global Interrupt
    using EXTI0_IRQn = NVIC::_NVIC_IPR1::PRI_N2; // < EXTI Line0 Interrupt
    using EXTI1_IRQn = NVIC::_NVIC_IPR1::PRI_N3; // < EXTI Line1 Interrupt
    using EXTI2_IRQn = NVIC::_NVIC_IPR2::PRI_N0; // < EXTI Line2 Interrupt
    using EXTI3_IRQn = NVIC::_NVIC_IPR2::PRI_N1; // < EXTI Line3 Interrupt
    using EXTI4_IRQn = NVIC::_NVIC_IPR2::PRI_N2; // < EXTI Line4 Interrupt
    using DMA1_Stream0_IRQn = NVIC::_NVIC_IPR2::PRI_N3; // < DMA1 Stream 0 global Interrupt
    using DMA1_Stream1_IRQn = NVIC::_NVIC_IPR3::PRI_N0; // < DMA1 Stream 1 global Interrupt
    using DMA1_Stream2_IRQn = NVIC::_NVIC_IPR3::PRI_N1; // < DMA1 Stream 2 global Interrupt
    using DMA1_Stream3_IRQn = NVIC::_NVIC_IPR3::PRI_N2; // < DMA1 Stream 3 global Interrupt
    using DMA1_Stream4_IRQn = NVIC::_NVIC_IPR3::PRI_N3; // < DMA1 Stream 4 global Interrupt
    using DMA1_Stream5_IRQn = NVIC::_NVIC_IPR4::PRI_N0; // < DMA1 Stream 5 global Interrupt
    using DMA1_Stream6_IRQn = NVIC::_NVIC_IPR4::PRI_N1; // < DMA1 Stream 6 global Interrupt
    using ADC_IRQn = NVIC::_NVIC_IPR4::PRI_N2; // < ADC1, ADC2 and ADC3 global Interrupts
    using CAN1_TX_IRQn = NVIC::_NVIC_IPR4::PRI_N3; // < CAN1 TX Interrupt
    using CAN1_RX0_IRQn = NVIC::_NVIC_IPR5::PRI_N0; // < CAN1 RX0 Interrupt
    using CAN1_RX1_IRQn = NVIC::_NVIC_IPR5::PRI_N1; // < CAN1 RX1 Interrupt
    using CAN1_SCE_IRQn = NVIC::_NVIC_IPR5::PRI_N2; // < CAN1 SCE Interrupt
    using EXTI9_5_IRQn = NVIC::_NVIC_IPR5::PRI_N3; // < External Line[9:5] Interrupts
    using TIM1_BRK_TIM9_IRQn = NVIC::_NVIC_IPR6::PRI_N0; // < TIM1 Break interrupt and TIM9 global interrupt
    using TIM1_UP_TIM10_IRQn = NVIC::_NVIC_IPR6::PRI_N1; // < TIM1 Update Interrupt and TIM10 global interrupt
    using TIM1_TRG_COM_TIM11_IRQn = NVIC::_NVIC_IPR6::PRI_N2; // < TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt
    using TIM1_CC_IRQn = NVIC::_NVIC_IPR6::PRI_N3; // < TIM1 Capture Compare Interrupt
    using TIM2_IRQn = NVIC::_NVIC_IPR7::PRI_N0; // < TIM2 global Interrupt
    using TIM3_IRQn = NVIC::_NVIC_IPR7::PRI_N1; // < TIM3 global Interrupt
    using TIM4_IRQn = NVIC::_NVIC_IPR7::PRI_N2; // < TIM4 global Interrupt
    using I2C1_EV_IRQn = NVIC::_NVIC_IPR7::PRI_N3; // < I2C1 Event Interrupt
    using I2C1_ER_IRQn = NVIC::_NVIC_IPR8::PRI_N0; // < I2C1 Error Interrupt
    using I2C2_EV_IRQn = NVIC::_NVIC_IPR8::PRI_N1; // < I2C2 Event Interrupt
    using I2C2_ER_IRQn = NVIC::_NVIC_IPR8::PRI_N2; // < I2C2 Error Interrupt
    using SPI1_IRQn = NVIC::_NVIC_IPR8::PRI_N3; // < SPI1 global Interrupt
    using SPI2_IRQn = NVIC::_NVIC_IPR9::PRI_N0; // < SPI2 global Interrupt
    using USART1_IRQn = NVIC::_NVIC_IPR9::PRI_N1; // < USART1 global Interrupt
    using USART2_IRQn = NVIC::_NVIC_IPR9::PRI_N2; // < USART2 global Interrupt
    using USART3_IRQn = NVIC::_NVIC_IPR9::PRI_N3; // < USART3 global Interrupt
    using EXTI15_10_IRQn = NVIC::_NVIC_IPR10::PRI_N0; // < External Line[15:10] Interrupts
    using RTC_Alarm_IRQn = NVIC::_NVIC_IPR10::PRI_N1; // < RTC Alarm (A and B) through EXTI Line Interrupt
    using OTG_FS_WKUP_IRQn = NVIC::_NVIC_IPR10::PRI_N2; // < USB OTG FS Wakeup through EXTI line interrupt
    using TIM8_BRK_TIM12_IRQn = NVIC::_NVIC_IPR10::PRI_N3; // < TIM8 Break Interrupt and TIM12 global interrupt
    using TIM8_UP_TIM13_IRQn = NVIC::_NVIC_IPR11::PRI_N0; // < TIM8 Update Interrupt and TIM13 global interrupt
    using TIM8_TRG_COM_TIM14_IRQn = NVIC::_NVIC_IPR11::PRI_N1; // < TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt
    using TIM8_CC_IRQn = NVIC::_NVIC_IPR11::PRI_N2; // < TIM8 Capture Compare Interrupt
    using DMA1_Stream7_IRQn = NVIC::_NVIC_IPR11::PRI_N3; // < DMA1 Stream7 Interrupt
    using FMC_IRQn = NVIC::_NVIC_IPR12::PRI_N0; // < FMC global Interrupt
    using SDMMC1_IRQn = NVIC::_NVIC_IPR12::PRI_N1; // < SDMMC1 global Interrupt
    using TIM5_IRQn = NVIC::_NVIC_IPR12::PRI_N2; // < TIM5 global Interrupt
    using SPI3_IRQn = NVIC::_NVIC_IPR12::PRI_N3; // < SPI3 global Interrupt
    using UART4_IRQn = NVIC::_NVIC_IPR13::PRI_N0; // < UART4 global Interrupt
    using UART5_IRQn = NVIC::_NVIC_IPR13::PRI_N1; // < UART5 global Interrupt
    using TIM6_DAC_IRQn = NVIC::_NVIC_IPR13::PRI_N2; // < TIM6 global and DAC1&2 underrun error  interrupts
    using TIM7_IRQn = NVIC::_NVIC_IPR13::PRI_N3; // < TIM7 global interrupt
    using DMA2_Stream0_IRQn = NVIC::_NVIC_IPR14::PRI_N0; // < DMA2 Stream 0 global Interrupt
    using DMA2_Stream1_IRQn = NVIC::_NVIC_IPR14::PRI_N1; // < DMA2 Stream 1 global Interrupt
    using DMA2_Stream2_IRQn = NVIC::_NVIC_IPR14::PRI_N2; // < DMA2 Stream 2 global Interrupt
    using DMA2_Stream3_IRQn = NVIC::_NVIC_IPR14::PRI_N3; // < DMA2 Stream 3 global Interrupt
    using DMA2_Stream4_IRQn = NVIC::_NVIC_IPR15::PRI_N0; // < DMA2 Stream 4 global Interrupt
    using ETH_IRQn = NVIC::_NVIC_IPR15::PRI_N1; // < Ethernet global Interrupt
    using ETH_WKUP_IRQn = NVIC::_NVIC_IPR15::PRI_N2; // < Ethernet Wakeup through EXTI line Interrupt
    using CAN2_TX_IRQn = NVIC::_NVIC_IPR15::PRI_N3; // < CAN2 TX Interrupt
    using CAN2_RX0_IRQn = NVIC::_NVIC_IPR16::PRI_N0; // < CAN2 RX0 Interrupt
    using CAN2_RX1_IRQn = NVIC::_NVIC_IPR16::PRI_N1; // < CAN2 RX1 Interrupt
    using CAN2_SCE_IRQn = NVIC::_NVIC_IPR16::PRI_N2; // < CAN2 SCE Interrupt
    using OTG_FS_IRQn = NVIC::_NVIC_IPR16::PRI_N3; // < USB OTG FS global Interrupt
    using DMA2_Stream5_IRQn = NVIC::_NVIC_IPR17::PRI_N0; // < DMA2 Stream 5 global interrupt
    using DMA2_Stream6_IRQn = NVIC::_NVIC_IPR17::PRI_N1; // < DMA2 Stream 6 global interrupt
    using DMA2_Stream7_IRQn = NVIC::_NVIC_IPR17::PRI_N2; // < DMA2 Stream 7 global interrupt
    using USART6_IRQn = NVIC::_NVIC_IPR17::PRI_N3; // < USART6 global interrupt
    using I2C3_EV_IRQn = NVIC::_NVIC_IPR18::PRI_N0; // < I2C3 event interrupt
    using I2C3_ER_IRQn = NVIC::_NVIC_IPR18::PRI_N1; // < I2C3 error interrupt
    using OTG_HS_EP1_OUT_IRQn = NVIC::_NVIC_IPR18::PRI_N2; // < USB OTG HS End Point 1 Out global interrupt
    using OTG_HS_EP1_IN_IRQn = NVIC::_NVIC_IPR18::PRI_N3; // < USB OTG HS End Point 1 In global interrupt
    using OTG_HS_WKUP_IRQn = NVIC::_NVIC_IPR19::PRI_N0; // < USB OTG HS Wakeup through EXTI interrupt
    using OTG_HS_IRQn = NVIC::_NVIC_IPR19::PRI_N1; // < USB OTG HS global interrupt
    using DCMI_IRQn = NVIC::_NVIC_IPR19::PRI_N2; // < DCMI global interrupt
    using RNG_IRQn = NVIC::_NVIC_IPR20::PRI_N0; // < RNG global interrupt
    using FPU_IRQn = NVIC::_NVIC_IPR20::PRI_N1; // < FPU global interrupt
    using UART7_IRQn = NVIC::_NVIC_IPR20::PRI_N2; // < UART7 global interrupt
    using UART8_IRQn = NVIC::_NVIC_IPR20::PRI_N3; // < UART8 global interrupt
    using SPI4_IRQn = NVIC::_NVIC_IPR21::PRI_N0; // < SPI4 global Interrupt
    using SPI5_IRQn = NVIC::_NVIC_IPR21::PRI_N1; // < SPI5 global Interrupt
    using SPI6_IRQn = NVIC::_NVIC_IPR21::PRI_N2; // < SPI6 global Interrupt
    using SAI1_IRQn = NVIC::_NVIC_IPR21::PRI_N3; // < SAI1 global Interrupt
    using LTDC_IRQn = NVIC::_NVIC_IPR22::PRI_N0; // < LTDC global Interrupt
    using LTDC_ER_IRQn = NVIC::_NVIC_IPR22::PRI_N1; // < LTDC Error global Interrupt
    using DMA2D_IRQn = NVIC::_NVIC_IPR22::PRI_N2; // < DMA2D global Interrupt
    using SAI2_IRQn = NVIC::_NVIC_IPR22::PRI_N3; // < SAI2 global Interrupt
    using QUADSPI_IRQn = NVIC::_NVIC_IPR23::PRI_N0; // < Quad SPI global interrupt
    using LPTIM1_IRQn = NVIC::_NVIC_IPR23::PRI_N1; // < LP TIM1 interrupt
    using CEC_IRQn = NVIC::_NVIC_IPR23::PRI_N2; // < HDMI-CEC global Interrupt
    using I2C4_EV_IRQn = NVIC::_NVIC_IPR23::PRI_N3; // < I2C4 Event Interrupt
    using I2C4_ER_IRQn = NVIC::_NVIC_IPR24::PRI_N0; // < I2C4 Error Interrupt
    using SPDIF_RX_IRQn = NVIC::_NVIC_IPR24::PRI_N1; // < SPDIF-RX global Interrupt
    using DFSDM1_FLT0_IRQn = NVIC::_NVIC_IPR24::PRI_N3; // < DFSDM1 Filter 0 global Interrupt
    using DFSDM1_FLT1_IRQn = NVIC::_NVIC_IPR25::PRI_N0; // < DFSDM1 Filter 1 global Interrupt
    using DFSDM1_FLT2_IRQn = NVIC::_NVIC_IPR25::PRI_N1; // < DFSDM1 Filter 2 global Interrupt
    using DFSDM1_FLT3_IRQn = NVIC::_NVIC_IPR25::PRI_N2; // < DFSDM1 Filter 3 global Interrupt
    using SDMMC2_IRQn = NVIC::_NVIC_IPR25::PRI_N3; // < SDMMC2 global Interrupt
    using CAN3_TX_IRQn = NVIC::_NVIC_IPR26::PRI_N0; // < CAN3 TX Interrupt
    using CAN3_RX0_IRQn = NVIC::_NVIC_IPR26::PRI_N1; // < CAN3 RX0 Interrupt
    using CAN3_RX1_IRQn = NVIC::_NVIC_IPR26::PRI_N2; // < CAN3 RX1 Interrupt
    using CAN3_SCE_IRQn = NVIC::_NVIC_IPR26::PRI_N3; // < CAN3 SCE Interrupt
    using JPEG_IRQn = NVIC::_NVIC_IPR27::PRI_N0; // < JPEG global Interrupt
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
    static inline void SetPriority()
    requires(priority < 16)
    {
        IRQ::write(priority << 4U);
    }

    template<typename IRQ>
    [[gnu::always_inline]]
	 static inline void __NVIC_EnableIRQ(IRQn_Type IRQn)
    {
      if ((int32_t)(IRQn) >= 0)
      {
        NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
      }
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
