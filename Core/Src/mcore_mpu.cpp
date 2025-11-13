/*
 * mcore_mpu.cpp
 *
 *  Created on: Sep 23, 2025
 *      Author: AkimovMA
 */

#include "mcore_mpu.hpp"
#include "mcore_utils.hpp"

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

	// MPU Region 1: Ethernet RX Buffer (16KB at 0x20078000)
	// Non-cacheable, shareable memory for DMA-accessible Ethernet receive buffers
	constexpr _MPU::RegionConfig region1_eth_rx = {
		.regionNum = _MPU::regionNumber::REGION_1,
		.baseAddress = 0x20078000,  // ETH RX buffer base address
		.instructionAccessDisable = true,
		.accessPermission = _MPU::regionPermission::REGION_FULL_ACCESS,
		.texLevel = _MPU::regionTEX::REGION_TEX_LEVEL1,
		.shareable = true,
		.cacheable = false,
		.bufferable = false,
		.subregionDisable = 0,
		.size = _MPU::regionSize::REGION_SIZE_16KB,
		.enable = true
	};

	// MPU Region 2: Ethernet Descriptor Buffer (1KB at 0x20077FC0)
	// Bufferable memory for Ethernet DMA descriptors (must be bufferable for DMA writes)
	constexpr _MPU::RegionConfig region2_eth_desc = {
		.regionNum = _MPU::regionNumber::REGION_2,
		.baseAddress = 0x20077FC0,  // ETH descriptor buffer base address
		.instructionAccessDisable = true,
		.accessPermission = _MPU::regionPermission::REGION_FULL_ACCESS,
		.texLevel = _MPU::regionTEX::REGION_TEX_LEVEL0,
		.shareable = true,
		.cacheable = false,
		.bufferable = true,  // Required for DMA descriptor writes
		.subregionDisable = 0,
		.size = _MPU::regionSize::REGION_SIZE_1KB,
		.enable = true
	};
}

void MPU_Config() {
	// Disable MPU before configuration (required by ARM architecture)
	_MPU::disable();
	
	// Configure regions in order
	_MPU::configure<region0>();        // Default deny region
	_MPU::configure<region1_eth_rx>(); // Ethernet RX buffer
	_MPU::configure<region2_eth_desc>(); // Ethernet descriptors
	
	// Enable MPU with privileged default memory map
	_MPU::enable();
}
