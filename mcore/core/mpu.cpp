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

	// MPU Region 1: all 512 KB of SRAM (DTCM, SRAM1, SRAM2) execute-never.
	// Nothing runs from RAM here (.RamFunc is empty), so an instruction fetch
	// from it - a corrupted return address or function pointer - is a precise
	// MemManage fault (IACCVIOL) instead of executing data. Normal memory,
	// write-back read/write-allocate, non-shareable: the same as the default
	// map, so caching does not change. Higher regions override it.
	constexpr _MPU::RegionConfig region1_sram_xn = {
		.regionNum = _MPU::regionNumber::REGION_1,
		.baseAddress = 0x20000000,
		.instructionAccessDisable = true,
		.accessPermission = _MPU::regionPermission::REGION_FULL_ACCESS,
		.texLevel = _MPU::regionTEX::REGION_TEX_LEVEL1,
		.shareable = false,
		.cacheable = true,
		.bufferable = true,
		.subregionDisable = 0,
		.size = _MPU::regionSize::REGION_SIZE_512KB,
		.enable = true
	};

	// MPU Region 2: Ethernet DMA area (32 KB at 0x20078000: descriptors and
	// RX/TX buffers). Non-cacheable, shareable memory, so the Ethernet DMA and
	// the CPU see the same bytes without cache maintenance
	constexpr _MPU::RegionConfig region2_eth_pool = {
		.regionNum = _MPU::regionNumber::REGION_2,
		.baseAddress = 0x20078000,  // ETH RX buffer base address
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

	// MPU Region 3: stack guard. The MSP stack is the first thing in RAM
	// (linker script, .stack at 0x20000000) and grows down; its lowest 1 KB is
	// no-access, so running past the usable 8 KB is a precise MemManage fault
	// (MMFAR = the address) that core/fault.cpp records. Without it an overflow
	// walks on through 0x1FFF0020..0x1FFFFFFF, which silently swallows writes
	// on this part, and faults only ~64 KB further down. 1 KB, not 32 B: a
	// function frame larger than the guard steps over it (push above, locals
	// below) - frames here stay well under 1 KB.
	constexpr _MPU::RegionConfig region3_stack_guard = {
		.regionNum = _MPU::regionNumber::REGION_3,
		.baseAddress = 0x20000000,  // = _sstack, checked in MPU_Config
		.instructionAccessDisable = true,
		.accessPermission = _MPU::regionPermission::REGION_NO_ACCESS,
		.texLevel = _MPU::regionTEX::REGION_TEX_LEVEL0,
		.shareable = false,
		.cacheable = false,
		.bufferable = false,
		.subregionDisable = 0,
		.size = _MPU::regionSize::REGION_SIZE_1KB,
		.enable = true
	};
}

extern "C" uint8_t _sstack[];   // linker script

void MPU_Config() {
	// Disable MPU before configuration (required by ARM architecture)
	_MPU::disable();
	
	// Configure regions in order
	_MPU::configure<region0>();        // Default deny region
	_MPU::configure<region1_sram_xn>();  // SRAM execute-never
	_MPU::configure<region2_eth_pool>(); // Ethernet DMA area, non-cacheable
	if (reinterpret_cast<uintptr_t>(_sstack) == region3_stack_guard.baseAddress) {
		_MPU::configure<region3_stack_guard>();  // only where the stack really starts
	}
	
	// Enable MPU with privileged default memory map
	_MPU::enable();
}
