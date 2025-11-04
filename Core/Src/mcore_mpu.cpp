/*
 * mcore_mpu.cpp
 *
 *  Created on: Sep 23, 2025
 *      Author: AkimovMA
 */
#include "mcore_mpu.hpp"
#include "mcore_utils.hpp"

constexpr  _MPU::RegionConfig region0 = {
	.regionNum = _MPU::regionNumber::REGION_0,
	.baseAddress = 0x00000000,
    .instructionAccessDisable = true,
    .accessPermission = _MPU::regionPermission::REGION_NO_ACCESS,
    .texLevel = _MPU::regionTEX::REGION_TEX_LEVEL0,
    .shareable = true,
    .cacheable = false,
    .bufferable = false,
    .subregionDisable = 0x87,//1000 0111
    .size = _MPU::regionSize::REGION_SIZE_4GB,
    .enable = true
};

constexpr  _MPU::RegionConfig region1_eth_rx = {
    .regionNum = _MPU::regionNumber::REGION_1,
    .baseAddress = 0x20078000,
    .instructionAccessDisable = true,
    .accessPermission = _MPU::regionPermission::REGION_FULL_ACCESS,
    .texLevel =  _MPU::regionTEX::REGION_TEX_LEVEL1,
    .shareable = true,
    .cacheable = false,
    .bufferable = false,
    .subregionDisable = 0,
    .size = _MPU::regionSize::REGION_SIZE_32KB,
    .enable = true
};

constexpr  _MPU::RegionConfig region2_eth_desc = {
	.regionNum = _MPU::regionNumber::REGION_2,
	.baseAddress = 0x20077FC0,
	.instructionAccessDisable = true,
	.accessPermission = _MPU::regionPermission::REGION_FULL_ACCESS,
	.texLevel = _MPU::regionTEX::REGION_TEX_LEVEL0,
	.shareable = true,
	.cacheable = false,
	.bufferable = true,
	.subregionDisable = 0,
	.size = _MPU::regionSize::REGION_SIZE_1KB,
	.enable = true
};


void MPU_Config(){

	_MPU::disable();
	_MPU::configure<region0>();
	_MPU::configure<region1_eth_rx>();
	_MPU::configure<region2_eth_desc>();
	_MPU::enable();
}
