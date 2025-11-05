/*
 * mcore_mpu.hpp
 *
 *  Created on: Sep 23, 2025
 *      Author: AkimovMA
 */
#pragma once
#include "mcore_def.hpp"

/** @defgroup CORTEX_MPU_HFNMI_PRIVDEF_Control MPU HFNMI and PRIVILEGED Access control*/
#define  MPU_HFNMI_PRIVDEF_NONE      ((uint32_t)0x00000000U)
#define  MPU_HARDFAULT_NMI           ((uint32_t)0x00000002U)
#define  MPU_PRIVILEGED_DEFAULT      ((uint32_t)0x00000004U)
#define  MPU_HFNMI_PRIVDEF           ((uint32_t)0x00000006U)
// MPU region constants moved into enums below (we keep only device-specific helpers/macros from CMSIS)

void MPU_Config(void);
namespace _MPU {

enum class regionNumber : uint8_t {
	REGION_0 = 0x00U,
	REGION_1 = 0x01U,
	REGION_2 = 0x02U,
	REGION_3 = 0x03U,
	REGION_4 = 0x04U,
	REGION_5 = 0x05U,
	REGION_6 = 0x06U,
	REGION_7 = 0x07U
};

enum class regionPermission : uint8_t {
	REGION_NO_ACCESS = 0x00U,
	REGION_PRIV_RW = 0x01U,
	REGION_PRIV_RW_URO = 0x02U,
	REGION_FULL_ACCESS = 0x03U,
	REGION_PRIV_RO = 0x05U,
	REGION_PRIV_RO_URO = 0x06U
};

enum class regionSize : uint8_t {
	REGION_SIZE_32B = 0x04U,
	REGION_SIZE_64B = 0x05U,
	REGION_SIZE_128B = 0x06U,
	REGION_SIZE_256B = 0x07U,
	REGION_SIZE_512B = 0x08U,
	REGION_SIZE_1KB = 0x09U,
	REGION_SIZE_2KB = 0x0AU,
	REGION_SIZE_4KB = 0x0BU,
	REGION_SIZE_8KB = 0x0CU,
	REGION_SIZE_16KB = 0x0DU,
	REGION_SIZE_32KB = 0x0EU,
	REGION_SIZE_64KB = 0x0FU,
	REGION_SIZE_128KB = 0x10U,
	REGION_SIZE_256KB = 0x11U,
	REGION_SIZE_512KB = 0x12U,
	REGION_SIZE_1MB = 0x13U,
	REGION_SIZE_2MB = 0x14U,
	REGION_SIZE_4MB = 0x15U,
	REGION_SIZE_8MB = 0x16U,
	REGION_SIZE_16MB = 0x17U,
	REGION_SIZE_32MB = 0x18U,
	REGION_SIZE_64MB = 0x19U,
	REGION_SIZE_128MB = 0x1AU,
	REGION_SIZE_256MB = 0x1BU,
	REGION_SIZE_512MB = 0x1CU,
	REGION_SIZE_1GB = 0x1DU,
	REGION_SIZE_2GB = 0x1EU,
	REGION_SIZE_4GB = 0x1FU
};

enum class regionTEX : uint8_t {
	REGION_TEX_LEVEL0 = 0x00U,
	REGION_TEX_LEVEL1 = 0x01U,
	REGION_TEX_LEVEL2 = 0x02U
};

struct RegionConfig {
	regionNumber regionNum;
	uint32_t baseAddress;
	bool instructionAccessDisable;
	regionPermission accessPermission;
	regionTEX texLevel;
	bool shareable;
	bool cacheable;
	bool bufferable;
	uint8_t subregionDisable;
	regionSize size;
	bool enable;
};

// Build RASR register value from RegionConfig. Can be evaluated at compile-time
constexpr inline uint32_t buildRASR(const RegionConfig &cfg) noexcept {
	return (static_cast<uint32_t>(cfg.instructionAccessDisable) << MPU::_MPU_RASR::XN::pos)
		| (static_cast<uint32_t>(cfg.accessPermission) << MPU::_MPU_RASR::AP::pos)
		| (static_cast<uint32_t>(cfg.texLevel) << MPU::_MPU_RASR::TEX::pos)
		| (static_cast<uint32_t>(cfg.shareable) << MPU::_MPU_RASR::S::pos)
		| (static_cast<uint32_t>(cfg.cacheable) << MPU::_MPU_RASR::C::pos)
		| (static_cast<uint32_t>(cfg.bufferable) << MPU::_MPU_RASR::B::pos)
		| (static_cast<uint32_t>(cfg.subregionDisable) << MPU::_MPU_RASR::SRD::pos)
		| (static_cast<uint32_t>(cfg.size) << MPU::_MPU_RASR::SIZE::pos)
		| (static_cast<uint32_t>(cfg.enable) << MPU::_MPU_RASR::ENABLE::pos);
}

inline static void disable() {
	__DMB();
	SCB::_SHCSR::MEMFAULTENA::clear();
	MPU::_MPU_CTRL::clear();
}

inline static void enable() {
	MPU::_MPU_CTRL::bitSet<MPU::_MPU_CTRL::ENABLE, MPU::_MPU_CTRL::PRIVDEFENA>();
	MPU::_MPU_CTRL::ENABLE::set();
	MPU::_MPU_CTRL::PRIVDEFENA::set();
	SCB::_SHCSR::MEMFAULTENA::set();
	__DSB();
	__ISB();
}
template<RegionConfig cfg>
inline static void configure() {
	
	MPU::_MPU_RNR ::REGION::write<cfg.regionNum>();
	MPU::_MPU_RASR::ENABLE::clear();
	MPU::_MPU_RBAR::overwrite<cfg.baseAddress>();
	MPU::_MPU_RASR::overwrite<buildRASR(cfg)>();
}
};
