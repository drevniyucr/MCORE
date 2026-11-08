#pragma once
// MPU region 1 of the chip's memory map (used by MPU_Config) (STM32F767).
#include "core/mpu.hpp"

namespace chip {
	// MPU Region 1: Ethernet descriptors + buffers (32KB) at the top of SRAM1.
	// Non-cacheable, shareable memory for DMA-accessible Ethernet buffers.
	// Must match the linker script.
	constexpr uint32_t ETH_POOL_BASE = 0x20078000;
	constexpr _MPU::RegionConfig region1_eth_pool = {
		.regionNum = _MPU::regionNumber::REGION_1,
		.baseAddress = ETH_POOL_BASE,
		.instructionAccessDisable = true,
		.accessPermission = _MPU::regionPermission::REGION_FULL_ACCESS,
		.texLevel = _MPU::regionTEX::REGION_TEX_LEVEL1,
		.shareable = true,
		.cacheable = false,
		.bufferable = false,
		.subregionDisable = 0,
		.size = _MPU::regionSize::REGION_SIZE_32KB,
		.enable = true
	};
} // namespace chip
