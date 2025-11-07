/*
* mcore_system.cpp
*
*  Created on: Jul 13, 2025
*      Author: doomt
*/

#include "mcore_system.hpp"
#include "mcore_nvic.hpp"


volatile uint32_t myTick = 0;   // счетчик миллисекунд

// Обработчик прерывания SysTick
extern "C" void SysTick_Handler(void) {
   myTick += 1;   // каждый тик увеличиваем счетчик
}


void delay_ms(uint32_t ms) {
   uint32_t start = myTick;
   while ((myTick - start) < ms) {
       // ждем
   }
}

uint32_t get_tick(void){
	return myTick;
}


#if !defined  (HSE_VALUE)
  #define HSE_VALUE    ((uint32_t)8000000) /*!< Default value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */


#if defined(USER_VECT_TAB_ADDRESS)
/*!< Uncomment the following line if you need to relocate your vector Table
     in Sram else user remap will be done in Flash. */
/* #define VECT_TAB_SRAM */
#if defined(VECT_TAB_SRAM)
#define VECT_TAB_BASE_ADDRESS   RAMDTCM_BASE    /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x200. */
#else
#define VECT_TAB_BASE_ADDRESS   FLASH_BASE      /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x200. */
#endif /* VECT_TAB_SRAM */
#if !defined(VECT_TAB_OFFSET)
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table offset field.
                                                     This value must be a multiple of 0x200. */
#endif /* VECT_TAB_OFFSET */
#endif /* USER_VECT_TAB_ADDRESS */
/******************************************************************************/

  uint32_t SystemCoreClock = 16000000;
  const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
  const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

/**
  * @brief  Setup the microcontroller system
  *         Initialize the Embedded Flash Interface, the PLL and update the
  *         SystemFrequency variable.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  /* FPU settings ------------------------------------------------------------*/
  SCB::_CPACR::setMask(
    SCB::_CPACR::CP10::BitMsk|SCB::_CPACR::CP11::BitMsk); /* set CP10 and CP11 Full Access */
}

void SystemCoreClockUpdate(void)
{
  uint32_t tmp, pllvco, pllp, pllsource, pllm;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC::_CFGR::SWS::read();
  switch (tmp)
  {
    case 0b00:  /* HSI used as system clock source */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0b01:  /* HSE used as system clock source */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0b10:  /* PLL used as system clock source */

      /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
         SYSCLK = PLL_VCO / PLL_P
         */

      pllsource = RCC::_PLLCFGR::PLLSRC::read();
      pllm = RCC::_PLLCFGR::PLLM::read(); 
      // pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
      // pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;

      if (pllsource != 0)
      {
        /* HSE used as PLL clock source */
        pllvco = (HSE_VALUE/pllm) * ((RCC::_PLLCFGR::PLLN::read()));
      }
      else
      {
        /* HSI used as PLL clock source */
        pllvco = (HSI_VALUE/pllm) * ((RCC::_PLLCFGR::PLLN::read()));
      }
      pllp = ((RCC::_PLLCFGR::PLLP::read()) + 1 ) * 2;
      SystemCoreClock = pllvco/pllp;
      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }
  /* Compute HCLK frequency --------------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[RCC::_CFGR::HPRE::read()];
  /* HCLK frequency */
  SystemCoreClock >>= tmp;
}


uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > 0xFFFFFFUL)
  {
    return (1UL); /* Reload value impossible */
  }
  SysTick::_STRVR::overwrite(ticks - 1U); /* set reload register */

  NVIC_API::SetPriority<SYS_IRQn::SysTick_IRQn, 15>(); /* set Priority for Systick Interrupt */
  
  SysTick::_STCVR::overwrite(0U); /* Load the SysTick Counter Value */
 
  SysTick::_STCSR::bitSet<
  SysTick::_STCSR::CLKSOURCE,
  SysTick::_STCSR::TICKINT, /* Enable SysTick IRQ and SysTick Timer */
  SysTick::_STCSR::ENABLE
  >();

  return (0UL); /* Function successful */
}