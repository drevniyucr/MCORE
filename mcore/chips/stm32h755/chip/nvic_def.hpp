// Auto-generated NVIC IRQ mapping
#pragma once
#include "core/regs.hpp"
struct IRQn_Type {
    //SYSTEM EXCEPTIONS
    struct NonMaskableInt_IRQn{
        static constexpr uint32_t irq_number = -14;
    };
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
    struct WWDG_IRQn { // < Window WatchDog Interrupt ( wwdg1_it, wwdg2_it)
        static constexpr uint32_t irq_number = 0;
        using Priority = NVIC::_NVIC_IPR0::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct PVD_AVD_IRQn { // < PVD/AVD through EXTI Line detection Interrupt
        static constexpr uint32_t irq_number = 1;
        using Priority = NVIC::_NVIC_IPR0::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct TAMP_STAMP_IRQn { // < Tamper and TimeStamp interrupts through the EXTI line
        static constexpr uint32_t irq_number = 2;
        using Priority = NVIC::_NVIC_IPR0::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct RTC_WKUP_IRQn { // < RTC Wakeup interrupt through the EXTI line
        static constexpr uint32_t irq_number = 3;
        using Priority = NVIC::_NVIC_IPR0::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct FLASH_IRQn { // < FLASH global Interrupt
        static constexpr uint32_t irq_number = 4;
        using Priority = NVIC::_NVIC_IPR1::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct RCC_IRQn { // < RCC global Interrupt
        static constexpr uint32_t irq_number = 5;
        using Priority = NVIC::_NVIC_IPR1::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct EXTI0_IRQn { // < EXTI Line0 Interrupt
        static constexpr uint32_t irq_number = 6;
        using Priority = NVIC::_NVIC_IPR1::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct EXTI1_IRQn { // < EXTI Line1 Interrupt
        static constexpr uint32_t irq_number = 7;
        using Priority = NVIC::_NVIC_IPR1::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct EXTI2_IRQn { // < EXTI Line2 Interrupt
        static constexpr uint32_t irq_number = 8;
        using Priority = NVIC::_NVIC_IPR2::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct EXTI3_IRQn { // < EXTI Line3 Interrupt
        static constexpr uint32_t irq_number = 9;
        using Priority = NVIC::_NVIC_IPR2::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct EXTI4_IRQn { // < EXTI Line4 Interrupt
        static constexpr uint32_t irq_number = 10;
        using Priority = NVIC::_NVIC_IPR2::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct DMA1_Stream0_IRQn { // < DMA1 Stream 0 global Interrupt
        static constexpr uint32_t irq_number = 11;
        using Priority = NVIC::_NVIC_IPR2::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct DMA1_Stream1_IRQn { // < DMA1 Stream 1 global Interrupt
        static constexpr uint32_t irq_number = 12;
        using Priority = NVIC::_NVIC_IPR3::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct DMA1_Stream2_IRQn { // < DMA1 Stream 2 global Interrupt
        static constexpr uint32_t irq_number = 13;
        using Priority = NVIC::_NVIC_IPR3::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct DMA1_Stream3_IRQn { // < DMA1 Stream 3 global Interrupt
        static constexpr uint32_t irq_number = 14;
        using Priority = NVIC::_NVIC_IPR3::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct DMA1_Stream4_IRQn { // < DMA1 Stream 4 global Interrupt
        static constexpr uint32_t irq_number = 15;
        using Priority = NVIC::_NVIC_IPR3::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct DMA1_Stream5_IRQn { // < DMA1 Stream 5 global Interrupt
        static constexpr uint32_t irq_number = 16;
        using Priority = NVIC::_NVIC_IPR4::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct DMA1_Stream6_IRQn { // < DMA1 Stream 6 global Interrupt
        static constexpr uint32_t irq_number = 17;
        using Priority = NVIC::_NVIC_IPR4::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct ADC_IRQn { // < ADC1 and  ADC2 global Interrupts
        static constexpr uint32_t irq_number = 18;
        using Priority = NVIC::_NVIC_IPR4::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct FDCAN1_IT0_IRQn { // < FDCAN1 Interrupt line 0
        static constexpr uint32_t irq_number = 19;
        using Priority = NVIC::_NVIC_IPR4::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct FDCAN2_IT0_IRQn { // < FDCAN2 Interrupt line 0
        static constexpr uint32_t irq_number = 20;
        using Priority = NVIC::_NVIC_IPR5::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct FDCAN1_IT1_IRQn { // < FDCAN1 Interrupt line 1
        static constexpr uint32_t irq_number = 21;
        using Priority = NVIC::_NVIC_IPR5::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct FDCAN2_IT1_IRQn { // < FDCAN2 Interrupt line 1
        static constexpr uint32_t irq_number = 22;
        using Priority = NVIC::_NVIC_IPR5::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct EXTI9_5_IRQn { // < External Line[9:5] Interrupts
        static constexpr uint32_t irq_number = 23;
        using Priority = NVIC::_NVIC_IPR5::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct TIM1_BRK_IRQn { // < TIM1 Break Interrupt
        static constexpr uint32_t irq_number = 24;
        using Priority = NVIC::_NVIC_IPR6::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct TIM1_UP_IRQn { // < TIM1 Update Interrupt
        static constexpr uint32_t irq_number = 25;
        using Priority = NVIC::_NVIC_IPR6::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct TIM1_TRG_COM_IRQn { // < TIM1 Trigger and Commutation Interrupt
        static constexpr uint32_t irq_number = 26;
        using Priority = NVIC::_NVIC_IPR6::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct TIM1_CC_IRQn { // < TIM1 Capture Compare Interrupt
        static constexpr uint32_t irq_number = 27;
        using Priority = NVIC::_NVIC_IPR6::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct TIM2_IRQn { // < TIM2 global Interrupt
        static constexpr uint32_t irq_number = 28;
        using Priority = NVIC::_NVIC_IPR7::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct TIM3_IRQn { // < TIM3 global Interrupt
        static constexpr uint32_t irq_number = 29;
        using Priority = NVIC::_NVIC_IPR7::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct TIM4_IRQn { // < TIM4 global Interrupt
        static constexpr uint32_t irq_number = 30;
        using Priority = NVIC::_NVIC_IPR7::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct I2C1_EV_IRQn { // < I2C1 Event Interrupt
        static constexpr uint32_t irq_number = 31;
        using Priority = NVIC::_NVIC_IPR7::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER0;
        using Disable  = NVIC::_NVIC_ICER0;
        using SetPend  = NVIC::_NVIC_ISPR0;
        using ClrPend  = NVIC::_NVIC_ICPR0;
    };
    struct I2C1_ER_IRQn { // < I2C1 Error Interrupt
        static constexpr uint32_t irq_number = 32;
        using Priority = NVIC::_NVIC_IPR8::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct I2C2_EV_IRQn { // < I2C2 Event Interrupt
        static constexpr uint32_t irq_number = 33;
        using Priority = NVIC::_NVIC_IPR8::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct I2C2_ER_IRQn { // < I2C2 Error Interrupt
        static constexpr uint32_t irq_number = 34;
        using Priority = NVIC::_NVIC_IPR8::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct SPI1_IRQn { // < SPI1 global Interrupt
        static constexpr uint32_t irq_number = 35;
        using Priority = NVIC::_NVIC_IPR8::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct SPI2_IRQn { // < SPI2 global Interrupt
        static constexpr uint32_t irq_number = 36;
        using Priority = NVIC::_NVIC_IPR9::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct USART1_IRQn { // < USART1 global Interrupt
        static constexpr uint32_t irq_number = 37;
        using Priority = NVIC::_NVIC_IPR9::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct USART2_IRQn { // < USART2 global Interrupt
        static constexpr uint32_t irq_number = 38;
        using Priority = NVIC::_NVIC_IPR9::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct USART3_IRQn { // < USART3 global Interrupt
        static constexpr uint32_t irq_number = 39;
        using Priority = NVIC::_NVIC_IPR9::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct EXTI15_10_IRQn { // < External Line[15:10] Interrupts
        static constexpr uint32_t irq_number = 40;
        using Priority = NVIC::_NVIC_IPR10::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct RTC_Alarm_IRQn { // < RTC Alarm (A and B) through EXTI Line Interrupt
        static constexpr uint32_t irq_number = 41;
        using Priority = NVIC::_NVIC_IPR10::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct TIM8_BRK_TIM12_IRQn { // < TIM8 Break Interrupt and TIM12 global interrupt
        static constexpr uint32_t irq_number = 43;
        using Priority = NVIC::_NVIC_IPR10::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct TIM8_UP_TIM13_IRQn { // < TIM8 Update Interrupt and TIM13 global interrupt
        static constexpr uint32_t irq_number = 44;
        using Priority = NVIC::_NVIC_IPR11::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct TIM8_TRG_COM_TIM14_IRQn { // < TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt
        static constexpr uint32_t irq_number = 45;
        using Priority = NVIC::_NVIC_IPR11::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct TIM8_CC_IRQn { // < TIM8 Capture Compare Interrupt
        static constexpr uint32_t irq_number = 46;
        using Priority = NVIC::_NVIC_IPR11::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct DMA1_Stream7_IRQn { // < DMA1 Stream7 Interrupt
        static constexpr uint32_t irq_number = 47;
        using Priority = NVIC::_NVIC_IPR11::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct FMC_IRQn { // < FMC global Interrupt
        static constexpr uint32_t irq_number = 48;
        using Priority = NVIC::_NVIC_IPR12::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct SDMMC1_IRQn { // < SDMMC1 global Interrupt
        static constexpr uint32_t irq_number = 49;
        using Priority = NVIC::_NVIC_IPR12::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct TIM5_IRQn { // < TIM5 global Interrupt
        static constexpr uint32_t irq_number = 50;
        using Priority = NVIC::_NVIC_IPR12::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct SPI3_IRQn { // < SPI3 global Interrupt
        static constexpr uint32_t irq_number = 51;
        using Priority = NVIC::_NVIC_IPR12::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct UART4_IRQn { // < UART4 global Interrupt
        static constexpr uint32_t irq_number = 52;
        using Priority = NVIC::_NVIC_IPR13::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct UART5_IRQn { // < UART5 global Interrupt
        static constexpr uint32_t irq_number = 53;
        using Priority = NVIC::_NVIC_IPR13::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct TIM6_DAC_IRQn { // < TIM6 global and DAC1&2 underrun error  interrupts
        static constexpr uint32_t irq_number = 54;
        using Priority = NVIC::_NVIC_IPR13::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct TIM7_IRQn { // < TIM7 global interrupt
        static constexpr uint32_t irq_number = 55;
        using Priority = NVIC::_NVIC_IPR13::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct DMA2_Stream0_IRQn { // < DMA2 Stream 0 global Interrupt
        static constexpr uint32_t irq_number = 56;
        using Priority = NVIC::_NVIC_IPR14::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct DMA2_Stream1_IRQn { // < DMA2 Stream 1 global Interrupt
        static constexpr uint32_t irq_number = 57;
        using Priority = NVIC::_NVIC_IPR14::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct DMA2_Stream2_IRQn { // < DMA2 Stream 2 global Interrupt
        static constexpr uint32_t irq_number = 58;
        using Priority = NVIC::_NVIC_IPR14::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct DMA2_Stream3_IRQn { // < DMA2 Stream 3 global Interrupt
        static constexpr uint32_t irq_number = 59;
        using Priority = NVIC::_NVIC_IPR14::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct DMA2_Stream4_IRQn { // < DMA2 Stream 4 global Interrupt
        static constexpr uint32_t irq_number = 60;
        using Priority = NVIC::_NVIC_IPR15::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct ETH_IRQn { // < Ethernet global Interrupt
        static constexpr uint32_t irq_number = 61;
        using Priority = NVIC::_NVIC_IPR15::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct ETH_WKUP_IRQn { // < Ethernet Wakeup through EXTI line Interrupt
        static constexpr uint32_t irq_number = 62;
        using Priority = NVIC::_NVIC_IPR15::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct FDCAN_CAL_IRQn { // < FDCAN Calibration unit Interrupt
        static constexpr uint32_t irq_number = 63;
        using Priority = NVIC::_NVIC_IPR15::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER1;
        using Disable  = NVIC::_NVIC_ICER1;
        using SetPend  = NVIC::_NVIC_ISPR1;
        using ClrPend  = NVIC::_NVIC_ICPR1;
    };
    struct CM7_SEV_IRQn { // < CM7 Send event interrupt for CM4
        static constexpr uint32_t irq_number = 64;
        using Priority = NVIC::_NVIC_IPR16::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct CM4_SEV_IRQn { // < CM4 Send event interrupt for CM7
        static constexpr uint32_t irq_number = 65;
        using Priority = NVIC::_NVIC_IPR16::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct DMA2_Stream5_IRQn { // < DMA2 Stream 5 global interrupt
        static constexpr uint32_t irq_number = 68;
        using Priority = NVIC::_NVIC_IPR17::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct DMA2_Stream6_IRQn { // < DMA2 Stream 6 global interrupt
        static constexpr uint32_t irq_number = 69;
        using Priority = NVIC::_NVIC_IPR17::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct DMA2_Stream7_IRQn { // < DMA2 Stream 7 global interrupt
        static constexpr uint32_t irq_number = 70;
        using Priority = NVIC::_NVIC_IPR17::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct USART6_IRQn { // < USART6 global interrupt
        static constexpr uint32_t irq_number = 71;
        using Priority = NVIC::_NVIC_IPR17::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct I2C3_EV_IRQn { // < I2C3 event interrupt
        static constexpr uint32_t irq_number = 72;
        using Priority = NVIC::_NVIC_IPR18::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct I2C3_ER_IRQn { // < I2C3 error interrupt
        static constexpr uint32_t irq_number = 73;
        using Priority = NVIC::_NVIC_IPR18::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct OTG_HS_EP1_OUT_IRQn { // < USB OTG HS End Point 1 Out global interrupt
        static constexpr uint32_t irq_number = 74;
        using Priority = NVIC::_NVIC_IPR18::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct OTG_HS_EP1_IN_IRQn { // < USB OTG HS End Point 1 In global interrupt
        static constexpr uint32_t irq_number = 75;
        using Priority = NVIC::_NVIC_IPR18::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct OTG_HS_WKUP_IRQn { // < USB OTG HS Wakeup through EXTI interrupt
        static constexpr uint32_t irq_number = 76;
        using Priority = NVIC::_NVIC_IPR19::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct OTG_HS_IRQn { // < USB OTG HS global interrupt
        static constexpr uint32_t irq_number = 77;
        using Priority = NVIC::_NVIC_IPR19::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct DCMI_IRQn { // < DCMI global interrupt
        static constexpr uint32_t irq_number = 78;
        using Priority = NVIC::_NVIC_IPR19::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct CRYP_IRQn { // < CRYP crypto global interrupt
        static constexpr uint32_t irq_number = 79;
        using Priority = NVIC::_NVIC_IPR19::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct HASH_RNG_IRQn { // < HASH and RNG global interrupt
        static constexpr uint32_t irq_number = 80;
        using Priority = NVIC::_NVIC_IPR20::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct FPU_IRQn { // < FPU global interrupt
        static constexpr uint32_t irq_number = 81;
        using Priority = NVIC::_NVIC_IPR20::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct UART7_IRQn { // < UART7 global interrupt
        static constexpr uint32_t irq_number = 82;
        using Priority = NVIC::_NVIC_IPR20::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct UART8_IRQn { // < UART8 global interrupt
        static constexpr uint32_t irq_number = 83;
        using Priority = NVIC::_NVIC_IPR20::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct SPI4_IRQn { // < SPI4 global Interrupt
        static constexpr uint32_t irq_number = 84;
        using Priority = NVIC::_NVIC_IPR21::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct SPI5_IRQn { // < SPI5 global Interrupt
        static constexpr uint32_t irq_number = 85;
        using Priority = NVIC::_NVIC_IPR21::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct SPI6_IRQn { // < SPI6 global Interrupt
        static constexpr uint32_t irq_number = 86;
        using Priority = NVIC::_NVIC_IPR21::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct SAI1_IRQn { // < SAI1 global Interrupt
        static constexpr uint32_t irq_number = 87;
        using Priority = NVIC::_NVIC_IPR21::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct LTDC_IRQn { // < LTDC global Interrupt
        static constexpr uint32_t irq_number = 88;
        using Priority = NVIC::_NVIC_IPR22::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct LTDC_ER_IRQn { // < LTDC Error global Interrupt
        static constexpr uint32_t irq_number = 89;
        using Priority = NVIC::_NVIC_IPR22::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct DMA2D_IRQn { // < DMA2D global Interrupt
        static constexpr uint32_t irq_number = 90;
        using Priority = NVIC::_NVIC_IPR22::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct SAI2_IRQn { // < SAI2 global Interrupt
        static constexpr uint32_t irq_number = 91;
        using Priority = NVIC::_NVIC_IPR22::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct QUADSPI_IRQn { // < Quad SPI global interrupt
        static constexpr uint32_t irq_number = 92;
        using Priority = NVIC::_NVIC_IPR23::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct LPTIM1_IRQn { // < LP TIM1 interrupt
        static constexpr uint32_t irq_number = 93;
        using Priority = NVIC::_NVIC_IPR23::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct CEC_IRQn { // < HDMI-CEC global Interrupt
        static constexpr uint32_t irq_number = 94;
        using Priority = NVIC::_NVIC_IPR23::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct I2C4_EV_IRQn { // < I2C4 Event Interrupt
        static constexpr uint32_t irq_number = 95;
        using Priority = NVIC::_NVIC_IPR23::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER2;
        using Disable  = NVIC::_NVIC_ICER2;
        using SetPend  = NVIC::_NVIC_ISPR2;
        using ClrPend  = NVIC::_NVIC_ICPR2;
    };
    struct I2C4_ER_IRQn { // < I2C4 Error Interrupt
        static constexpr uint32_t irq_number = 96;
        using Priority = NVIC::_NVIC_IPR24::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct SPDIF_RX_IRQn { // < SPDIF-RX global Interrupt
        static constexpr uint32_t irq_number = 97;
        using Priority = NVIC::_NVIC_IPR24::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct OTG_FS_EP1_OUT_IRQn { // < USB OTG HS2 global interrupt
        static constexpr uint32_t irq_number = 98;
        using Priority = NVIC::_NVIC_IPR24::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct OTG_FS_EP1_IN_IRQn { // < USB OTG HS2 End Point 1 Out global interrupt
        static constexpr uint32_t irq_number = 99;
        using Priority = NVIC::_NVIC_IPR24::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct OTG_FS_WKUP_IRQn { // < USB OTG HS2 End Point 1 In global interrupt
        static constexpr uint32_t irq_number = 100;
        using Priority = NVIC::_NVIC_IPR25::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct OTG_FS_IRQn { // < USB OTG HS2 Wakeup through EXTI interrupt
        static constexpr uint32_t irq_number = 101;
        using Priority = NVIC::_NVIC_IPR25::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct DMAMUX1_OVR_IRQn { // < DMAMUX1 Overrun interrupt
        static constexpr uint32_t irq_number = 102;
        using Priority = NVIC::_NVIC_IPR25::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct HRTIM1_Master_IRQn { // < HRTIM Master Timer global Interrupts
        static constexpr uint32_t irq_number = 103;
        using Priority = NVIC::_NVIC_IPR25::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct HRTIM1_TIMA_IRQn { // < HRTIM Timer A global Interrupt
        static constexpr uint32_t irq_number = 104;
        using Priority = NVIC::_NVIC_IPR26::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct HRTIM1_TIMB_IRQn { // < HRTIM Timer B global Interrupt
        static constexpr uint32_t irq_number = 105;
        using Priority = NVIC::_NVIC_IPR26::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct HRTIM1_TIMC_IRQn { // < HRTIM Timer C global Interrupt
        static constexpr uint32_t irq_number = 106;
        using Priority = NVIC::_NVIC_IPR26::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct HRTIM1_TIMD_IRQn { // < HRTIM Timer D global Interrupt
        static constexpr uint32_t irq_number = 107;
        using Priority = NVIC::_NVIC_IPR26::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct HRTIM1_TIME_IRQn { // < HRTIM Timer E global Interrupt
        static constexpr uint32_t irq_number = 108;
        using Priority = NVIC::_NVIC_IPR27::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct HRTIM1_FLT_IRQn { // < HRTIM Fault global Interrupt
        static constexpr uint32_t irq_number = 109;
        using Priority = NVIC::_NVIC_IPR27::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct DFSDM1_FLT0_IRQn { // < DFSDM Filter1 Interrupt
        static constexpr uint32_t irq_number = 110;
        using Priority = NVIC::_NVIC_IPR27::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct DFSDM1_FLT1_IRQn { // < DFSDM Filter2 Interrupt
        static constexpr uint32_t irq_number = 111;
        using Priority = NVIC::_NVIC_IPR27::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct DFSDM1_FLT2_IRQn { // < DFSDM Filter3 Interrupt
        static constexpr uint32_t irq_number = 112;
        using Priority = NVIC::_NVIC_IPR28::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct DFSDM1_FLT3_IRQn { // < DFSDM Filter4 Interrupt
        static constexpr uint32_t irq_number = 113;
        using Priority = NVIC::_NVIC_IPR28::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct SAI3_IRQn { // < SAI3 global Interrupt
        static constexpr uint32_t irq_number = 114;
        using Priority = NVIC::_NVIC_IPR28::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct SWPMI1_IRQn { // < Serial Wire Interface 1 global interrupt
        static constexpr uint32_t irq_number = 115;
        using Priority = NVIC::_NVIC_IPR28::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct TIM15_IRQn { // < TIM15 global Interrupt
        static constexpr uint32_t irq_number = 116;
        using Priority = NVIC::_NVIC_IPR29::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct TIM16_IRQn { // < TIM16 global Interrupt
        static constexpr uint32_t irq_number = 117;
        using Priority = NVIC::_NVIC_IPR29::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct TIM17_IRQn { // < TIM17 global Interrupt
        static constexpr uint32_t irq_number = 118;
        using Priority = NVIC::_NVIC_IPR29::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct MDIOS_WKUP_IRQn { // < MDIOS Wakeup  Interrupt
        static constexpr uint32_t irq_number = 119;
        using Priority = NVIC::_NVIC_IPR29::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct MDIOS_IRQn { // < MDIOS global Interrupt
        static constexpr uint32_t irq_number = 120;
        using Priority = NVIC::_NVIC_IPR30::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct JPEG_IRQn { // < JPEG global Interrupt
        static constexpr uint32_t irq_number = 121;
        using Priority = NVIC::_NVIC_IPR30::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct MDMA_IRQn { // < MDMA global Interrupt
        static constexpr uint32_t irq_number = 122;
        using Priority = NVIC::_NVIC_IPR30::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct SDMMC2_IRQn { // < SDMMC2 global Interrupt
        static constexpr uint32_t irq_number = 124;
        using Priority = NVIC::_NVIC_IPR31::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct HSEM1_IRQn { // < HSEM1 global Interrupt
        static constexpr uint32_t irq_number = 125;
        using Priority = NVIC::_NVIC_IPR31::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct HSEM2_IRQn { // < HSEM2 global Interrupt
        static constexpr uint32_t irq_number = 126;
        using Priority = NVIC::_NVIC_IPR31::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct ADC3_IRQn { // < ADC3 global Interrupt
        static constexpr uint32_t irq_number = 127;
        using Priority = NVIC::_NVIC_IPR31::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER3;
        using Disable  = NVIC::_NVIC_ICER3;
        using SetPend  = NVIC::_NVIC_ISPR3;
        using ClrPend  = NVIC::_NVIC_ICPR3;
    };
    struct DMAMUX2_OVR_IRQn { // < DMAMUX2 Overrun interrupt
        static constexpr uint32_t irq_number = 128;
        using Priority = NVIC::_NVIC_IPR32::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct BDMA_Channel0_IRQn { // < BDMA Channel 0 global Interrupt
        static constexpr uint32_t irq_number = 129;
        using Priority = NVIC::_NVIC_IPR32::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct BDMA_Channel1_IRQn { // < BDMA Channel 1 global Interrupt
        static constexpr uint32_t irq_number = 130;
        using Priority = NVIC::_NVIC_IPR32::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct BDMA_Channel2_IRQn { // < BDMA Channel 2 global Interrupt
        static constexpr uint32_t irq_number = 131;
        using Priority = NVIC::_NVIC_IPR32::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct BDMA_Channel3_IRQn { // < BDMA Channel 3 global Interrupt
        static constexpr uint32_t irq_number = 132;
        using Priority = NVIC::_NVIC_IPR33::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct BDMA_Channel4_IRQn { // < BDMA Channel 4 global Interrupt
        static constexpr uint32_t irq_number = 133;
        using Priority = NVIC::_NVIC_IPR33::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct BDMA_Channel5_IRQn { // < BDMA Channel 5 global Interrupt
        static constexpr uint32_t irq_number = 134;
        using Priority = NVIC::_NVIC_IPR33::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct BDMA_Channel6_IRQn { // < BDMA Channel 6 global Interrupt
        static constexpr uint32_t irq_number = 135;
        using Priority = NVIC::_NVIC_IPR33::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct BDMA_Channel7_IRQn { // < BDMA Channel 7 global Interrupt
        static constexpr uint32_t irq_number = 136;
        using Priority = NVIC::_NVIC_IPR34::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct LPTIM2_IRQn { // < LP TIM2 global interrupt
        static constexpr uint32_t irq_number = 138;
        using Priority = NVIC::_NVIC_IPR34::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct LPTIM3_IRQn { // < LP TIM3 global interrupt
        static constexpr uint32_t irq_number = 139;
        using Priority = NVIC::_NVIC_IPR34::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct LPTIM4_IRQn { // < LP TIM4 global interrupt
        static constexpr uint32_t irq_number = 140;
        using Priority = NVIC::_NVIC_IPR35::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct LPTIM5_IRQn { // < LP TIM5 global interrupt
        static constexpr uint32_t irq_number = 141;
        using Priority = NVIC::_NVIC_IPR35::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct LPUART1_IRQn { // < LP UART1 interrupt
        static constexpr uint32_t irq_number = 142;
        using Priority = NVIC::_NVIC_IPR35::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct WWDG_RST_IRQn { // < Window Watchdog reset interrupt (exti_d2_wwdg_it, exti_d1_wwdg_it)
        static constexpr uint32_t irq_number = 143;
        using Priority = NVIC::_NVIC_IPR35::PRI_N3;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct CRS_IRQn { // < Clock Recovery Global Interrupt
        static constexpr uint32_t irq_number = 144;
        using Priority = NVIC::_NVIC_IPR36::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct ECC_IRQn { // < ECC diagnostic Global Interrupt
        static constexpr uint32_t irq_number = 145;
        using Priority = NVIC::_NVIC_IPR36::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct SAI4_IRQn { // < SAI4 global interrupt
        static constexpr uint32_t irq_number = 146;
        using Priority = NVIC::_NVIC_IPR36::PRI_N2;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct HOLD_CORE_IRQn { // < Hold core interrupt
        static constexpr uint32_t irq_number = 148;
        using Priority = NVIC::_NVIC_IPR37::PRI_N0;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
    struct WAKEUP_PIN_IRQn { // < Interrupt for all 6 wake-up pins
        static constexpr uint32_t irq_number = 149;
        using Priority = NVIC::_NVIC_IPR37::PRI_N1;
        using Enable   = NVIC::_NVIC_ISER4;
        using Disable  = NVIC::_NVIC_ICER4;
        using SetPend  = NVIC::_NVIC_ISPR4;
        using ClrPend  = NVIC::_NVIC_ICPR4;
    };
};