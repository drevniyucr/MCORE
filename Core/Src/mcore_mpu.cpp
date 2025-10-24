/*
 * mcore_mpu.cpp
 *
 *  Created on: Sep 23, 2025
 *      Author: AkimovMA
 */
#include "mcore_mpu.hpp"

void MPU_Config(void) {
	/// mpu disable
	MPU_Disable();

	///region 0
	MPU->RNR = MPU_REGION_NUMBER0;
	/* Disable the Region */
	MPU->RASR &= ~MPU_RASR_ENABLE_Msk;
	/* Apply configuration */
	MPU->RBAR = 0x00; //base address
	MPU->RASR =
			((uint32_t) MPU_INSTRUCTION_ACCESS_DISABLE << MPU_RASR_XN_Pos) | // instruction access disable
			((uint32_t) MPU_REGION_NO_ACCESS << MPU_RASR_AP_Pos) | // region no access
			((uint32_t) MPU_TEX_LEVEL0 << MPU_RASR_TEX_Pos) | //level 0
			((uint32_t) MPU_ACCESS_SHAREABLE << MPU_RASR_S_Pos) | //shareable
			((uint32_t) MPU_ACCESS_NOT_CACHEABLE << MPU_RASR_C_Pos) | // not cacheble
			((uint32_t) MPU_ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos) | // not bufferable
			((uint32_t) 0x87 << MPU_RASR_SRD_Pos) | // sub region disable 1000 0111
			((uint32_t) MPU_REGION_SIZE_4GB << MPU_RASR_SIZE_Pos) | //size 4GB
			((uint32_t) MPU_REGION_ENABLE << MPU_RASR_ENABLE_Pos); // mpu enable

	///region 1 rx buff eth
	MPU->RNR = 0x01;
	/* Disable the Region */
	MPU->RASR &= ~MPU_RASR_ENABLE_Msk;
	/* Apply configuration */
	MPU->RBAR = 0x20078000; //base address
	MPU->RASR = ((uint32_t) 0x01 << MPU_RASR_XN_Pos) | // istruction access disable
			((uint32_t) 0x03 << MPU_RASR_AP_Pos) | // full access
			((uint32_t) 0x01 << MPU_RASR_TEX_Pos) | //level 1
			((uint32_t) 0x00 << MPU_RASR_S_Pos) | // not shareble
			((uint32_t) 0x00 << MPU_RASR_C_Pos) | // not cacheble
			((uint32_t) 0x00 << MPU_RASR_B_Pos) | // not bufferable
			((uint32_t) 0x00 << MPU_RASR_SRD_Pos) | // sub region 0x87
			((uint32_t) 0x0D << MPU_RASR_SIZE_Pos) | //size 16kB
			((uint32_t) 0x01 << MPU_RASR_ENABLE_Pos); // mpu enable

	///region 2 rx/tx desc
	MPU->RNR = 0x02;
	/* Disable the Region */
	MPU->RASR &= ~MPU_RASR_ENABLE_Msk;
	/* Apply configuration */
	MPU->RBAR = 0x2007c000; //base address
	MPU->RASR = ((uint32_t) 0x01 << MPU_RASR_XN_Pos) | // istruction access disable
			((uint32_t) 0x03 << MPU_RASR_AP_Pos) | // full access
			((uint32_t) 0x00 << MPU_RASR_TEX_Pos) | //level 1
			((uint32_t) 0x01 << MPU_RASR_S_Pos) | // shareble
			((uint32_t) 0x00 << MPU_RASR_C_Pos) | // not cacheble
			((uint32_t) 0x01 << MPU_RASR_B_Pos) | //  bufferable
			((uint32_t) 0x00 << MPU_RASR_SRD_Pos) | // sub region 0x87
			((uint32_t) 0x09 << MPU_RASR_SIZE_Pos) | //size 1kB
			((uint32_t) 0x01 << MPU_RASR_ENABLE_Pos); // mpu enable

	MPU_Enable();
}

void MPU_Disable(void) {
	__DMB();
	/* Disable fault exceptions */
	SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
	/* Disable the MPU and clear the control register*/
	MPU->CTRL = 0;

}
void MPU_Enable(void) {
	/* Enable the MPU */
	MPU->CTRL = MPU_PRIVILEGED_DEFAULT | MPU_CTRL_ENABLE_Msk;
	/* Enable fault exceptions */
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
	/* Ensure MPU setting take effects */
	__DSB();
	__ISB();
}
