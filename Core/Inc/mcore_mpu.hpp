/*
 * mcore_mpu.hpp
 *
 *  Created on: Sep 23, 2025
 *      Author: AkimovMA
 */
#pragma once
#include "mcore_def.hpp"


#define  MPU_HFNMI_PRIVDEF_NONE      ((uint32_t)0x00000000U)
#define  MPU_HARDFAULT_NMI           ((uint32_t)0x00000002U)
#define  MPU_PRIVILEGED_DEFAULT      ((uint32_t)0x00000004U)
#define  MPU_HFNMI_PRIVDEF           ((uint32_t)0x00000006U)

void MPU_Config(void);
void MPU_Disable(void);
void MPU_Enable(void);
