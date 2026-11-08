#pragma once
// MPU region 1 of the chip's memory map (used by MPU_Config) (STM32H755).
#include "core/mpu.hpp"

namespace chip {
	// MPU Region 1 (STM32H755 Cortex-M7): SRAM4 (0x38000000, 64KB), the memory
	// shared with the Cortex-M4 (projects/stm32h755/shared/ipc_shared.hpp). Non-cacheable so both
	// cores see each other's writes without cache maintenance. The Ethernet
	// pool belongs to the Cortex-M4, which has no data cache.
	constexpr _MPU::RegionConfig region1_eth_pool = {
		.regionNum = _MPU::regionNumber::REGION_1,
		.baseAddress = 0x38000000,
		.instructionAccessDisable = true,
		.accessPermission = _MPU::regionPermission::REGION_FULL_ACCESS,
		.texLevel = _MPU::regionTEX::REGION_TEX_LEVEL1,
		.shareable = true,
		.cacheable = false,
		.bufferable = false,
		.subregionDisable = 0,
		.size = _MPU::regionSize::REGION_SIZE_64KB,
		.enable = true
	};
} // namespace chip
