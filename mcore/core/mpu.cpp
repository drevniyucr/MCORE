/*
 * mcore_mpu.cpp
 *
 *  Created on: Sep 23, 2025
 *      Author: AkimovMA
 */

#include "core/mpu.hpp"
#include "core/utils.hpp"

namespace {
	// MPU Region 0: Default region - deny all access to entire 4GB address space
	// This creates a "default deny" policy where only explicitly configured regions are accessible
	constexpr _MPU::RegionConfig region0 = {
		.regionNum = _MPU::regionNumber::REGION_0,
		.baseAddress = 0x00000000,
		.instructionAccessDisable = true,
		.accessPermission = _MPU::regionPermission::REGION_NO_ACCESS,
		.texLevel = _MPU::regionTEX::REGION_TEX_LEVEL0,
		.shareable = true,
		.cacheable = false,
		.bufferable = false,
		.subregionDisable = 0x87,  // Binary: 1000 0111 - disable subregions 0,1,2,7
		.size = _MPU::regionSize::REGION_SIZE_4GB,
		.enable = true
	};

#if defined(MCORE_STM32H755)
	// MPU Region 1 (STM32H755 Cortex-M7): SRAM4 (0x38000000, 64KB), the memory
	// shared with the Cortex-M4 (app/ipc_shared.hpp). Non-cacheable so both
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
#else
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
#endif
}

void MPU_Config() {
	// Disable MPU before configuration (required by ARM architecture)
	_MPU::disable();
	
	// Configure regions in order
	_MPU::configure<region0>();        // Default deny region
	_MPU::configure<region1_eth_pool>(); // Ethernet RX buffer
	
	// Enable MPU with privileged default memory map
	_MPU::enable();
}
