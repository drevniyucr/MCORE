/*
 * i2c.hpp — I2C master driver (init + polling transfers)
 *
 *  Created on: 26 февр. 2026 г.
 *      Author: AkimovMA
 */
#pragma once

#include <cstdint>
#include <cstddef>
#include "core/regs.hpp"
#include "core/rcc.hpp"
#include "core/system.hpp"
#include "core/mcore_config.hpp"
#include "drivers/common.hpp"

constexpr uint32_t I2C_TIMING_MASK = 0xF0FFFFFF;
constexpr uint32_t I2C_TIMEOUT_MS = 100; // default per-wait timeout for polling transfers

enum class I2CClockSrc : uint8_t {
    PCLK1 = 0, SYSCLK = 1, HSI = 2
};

enum class I2CAddrMode : uint8_t {
	Addr7bit  = 0, Addr10bit = 1
};

enum class I2CGenCallMode : uint8_t {
    Disable = 0, Enable = 1
};

enum class I2CStretchMode : uint8_t {
    Enable = 0, Disable = 1
};

enum class I2CDualAddrMode : uint8_t {
    Disable = 0, Enable = 1
};
struct I2CHwConfig {
	uint32_t Timing = 0;                 // Hardware timing register  value (calculated based on desired I2C speed and clock source)
    uint16_t OwnAddres1 = 0;             // Own address1  (if in slave mode)
    uint16_t OwnAddres2 = 0;             // Own address2  (if in slave mode)
    I2CDualAddrMode DualAddrMode = I2CDualAddrMode::Disable;  // Dual addressing mode (if in slave mode)
    I2CGenCallMode GenCallMode =   I2CGenCallMode::Disable;   // General call mode (if in slave mode)
    I2CStretchMode StretchMode =   I2CStretchMode::Enable;    // Clock stretching mode (if in slave mode)
    I2CClockSrc ClockSource =      I2CClockSrc::PCLK1;        // I2C clock source
    I2CAddrMode AddrMode =         I2CAddrMode::Addr7bit;     // 7-bit or 10-bit addressing mode
    uint8_t DigitalFilterCoeff = 0;      // Digital noise filter coefficient (0-15)
    uint8_t OwnAddr2Mask = 0;            // Own address 2 mask (if in slave mode)
    bool EnAnalogFilter = false;          // Enable or disable the analog noise filter
    bool EnInterrupts = false;           // Enable or disable I2C interrupts
	bool UseDMARx = false;              
	bool UseDMATx = false;
};

// Kernel clock SOURCE selection (DKCFGR2). Bus clock enable is the unified
// RccEnable<I2Cx> map in core/rcc.hpp.
template<typename I2Cx>
static inline void I2C_ClkSrcSel(I2CClockSrc src);
template <>
inline void I2C_ClkSrcSel<I2C1>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C1SEL::write(static_cast<uint32_t>(src));
}
template <>
inline void I2C_ClkSrcSel<I2C2>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C2SEL::write(static_cast<uint32_t>(src));
}
template <>
inline void I2C_ClkSrcSel<I2C3>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C3SEL::write(static_cast<uint32_t>(src));
}
template <>
inline void I2C_ClkSrcSel<I2C4>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C4SEL::write(static_cast<uint32_t>(src));
}


template<typename I2Cx, I2CHwConfig cfg>
static inline DrvStatus I2C_Init() {

    static_assert(cfg.DigitalFilterCoeff <= 15);
    static_assert(cfg.OwnAddr2Mask <= 7);
	/* 0. Clock enable + kernel clock source */
	RccEnable<I2Cx>::enable();
	I2C_ClkSrcSel<I2Cx>(cfg.ClockSource);

	/* 1. Disable I2C */
	I2Cx::_CR1::PE::clear();

	/* 2. Timing */
	I2Cx::_TIMINGR::overwrite(static_cast<uint32_t>(cfg.Timing) & I2C_TIMING_MASK);

    /* 3. Addressing mode */
    I2Cx::_OAR1::OA1EN::clear();

    if constexpr (cfg.AddrMode == I2CAddrMode::Addr10bit) {
        I2Cx::_OAR1::overwrite(
            I2Cx::_OAR1::OA1::Val2Mask(static_cast<uint32_t>(cfg.OwnAddres1)) 
            | I2Cx::_OAR1::MODE::Val2Mask(static_cast<uint32_t>(cfg.AddrMode))  
            | I2Cx::_OAR1::OA1EN::BitMsk
        );
    } else {// 7-bit mode
        I2Cx::_OAR1::overwrite(
            ((static_cast<uint32_t>(cfg.OwnAddres1)  & 0x7F) << 1) 
            | I2Cx::_OAR1::OA1EN::BitMsk
        );
    }
    /* 5. OAR */
    I2Cx::_OAR2::OA2EN::clear();

    I2Cx::_OAR2::overwrite(
        (cfg.DualAddrMode == I2CDualAddrMode::Enable ? I2Cx::_OAR2::OA2EN::BitMsk : 0) 
        | I2Cx::_OAR2::OA2MSK::Val2Mask(static_cast<uint32_t>(cfg.OwnAddr2Mask))         
        | I2Cx::_OAR2::OA2::Val2Mask(static_cast<uint32_t>(cfg.OwnAddres2))      
    );
       
     /* 6. CR1 */
    I2Cx::_CR1::overwrite(
        I2Cx::_CR1::ANFOFF::Val2Mask(static_cast<uint32_t>(!cfg.EnAnalogFilter)) 
        | I2Cx::_CR1::DNF::Val2Mask(static_cast<uint32_t>(cfg.DigitalFilterCoeff)) 
        | I2Cx::_CR1::NOSTRETCH::Val2Mask(static_cast<uint32_t>(cfg.StretchMode))    
        | I2Cx::_CR1::GCEN::Val2Mask(static_cast<uint32_t>(cfg.GenCallMode))       
    );

    I2Cx::_CR1::PE::set();

    return DrvStatus::Ok;
}

template<typename I2Cx>
struct I2CHandle {

	/* Write buffer to slave (polling, millisecond timeout per wait) */
	static inline DrvStatus write(
		uint16_t addr,
		const uint8_t* buf,
		size_t len,
		bool addr10 = false,
		uint32_t timeout_ms = I2C_TIMEOUT_MS
	) {
		/* Clear error flags */
		clearErrors();

		/* Configure transfer */
		I2Cx::_CR2::overwrite(
			  (addr10 ? I2Cx::_CR2::ADD10::BitMsk : 0)
			| encodeAddress(addr, addr10)
			| (len << I2Cx::_CR2::NBYTES::pos)
			| I2Cx::_CR2::START::BitMsk
		);

		for (size_t i = 0; i < len; ++i) {
			if (!waitTXIS(timeout_ms))
				return checkErrorStatus();

			I2Cx::_TXDR::overwrite(buf[i]);
		}

		if (!waitTC(timeout_ms))
			return checkErrorStatus();

		I2Cx::_CR2::STOP::set();
		return DrvStatus::Ok;
	}

	/* Read buffer from slave (polling, millisecond timeout per wait) */
	static inline DrvStatus read(
		uint16_t addr,
		uint8_t* buf,
		size_t len,
		bool is_addr10 = false,
		uint32_t timeout_ms = I2C_TIMEOUT_MS
	) {
		/* Clear error flags */
		clearErrors();

		I2Cx::_CR2::overwrite(
			  (is_addr10 ? I2Cx::_CR2::ADD10::BitMsk : 0)
			| encodeAddress(addr, is_addr10)
			| (len << I2Cx::_CR2::NBYTES::pos)
			| I2Cx::_CR2::RD_WRN::BitMsk
			| I2Cx::_CR2::START::BitMsk
		);

		for (size_t i = 0; i < len; ++i) {
			if (!waitRXNE(timeout_ms))
				return checkErrorStatus();

			buf[i] = static_cast<uint8_t>(I2Cx::_RXDR::read());
		}

		if (!waitTC(timeout_ms))
			return checkErrorStatus();

		I2Cx::_CR2::STOP::set();
		return DrvStatus::Ok;
	}

private:
	/* Address encoding for CR2.SADD */
	static constexpr uint32_t encodeAddress(uint16_t addr, bool is_addr10) {
		if (is_addr10) {
			/* 10-bit: bits[9:0] go into SADD[9:0] */
			return I2Cx::_CR2::SADD::Val2Mask(addr);
		} else {
			/* 7-bit: addr[6:0] -> SADD[7:1] */
			return (addr & 0x7F) << 1;
		}
	}

	// Millisecond-based waits (get_tick), consistent with the other drivers.
	static inline bool waitTXIS(uint32_t timeout_ms) {
		uint32_t tickstart = get_tick();
		while (!I2Cx::_ISR::TXIS::is_set()) {
			if (I2Cx::_ISR::NACKF::is_set()) return false;
			if ((get_tick() - tickstart) > timeout_ms) return false;
		}
		return true;
	}

	static inline bool waitRXNE(uint32_t timeout_ms) {
		uint32_t tickstart = get_tick();
		while (!I2Cx::_ISR::RXNE::is_set()) {
			if (I2Cx::_ISR::NACKF::is_set()) return false;
			if ((get_tick() - tickstart) > timeout_ms) return false;
		}
		return true;
	}

	static inline bool waitTC(uint32_t timeout_ms) {
		uint32_t tickstart = get_tick();
		while (!I2Cx::_ISR::TC::is_set()) {
			if (I2Cx::_ISR::NACKF::is_set()) return false;
			if ((get_tick() - tickstart) > timeout_ms) return false;
		}
		return true;
	}

	static inline void clearErrors() {
		I2Cx::_ICR::NACKCF::set();
		I2Cx::_ICR::BERRCF::set();
		I2Cx::_ICR::ARLOCF::set();
	}

	static inline DrvStatus checkErrorStatus() {
		if (I2Cx::_ISR::NACKF::is_set()) {
			I2Cx::_ICR::NACKCF::set();
			return DrvStatus::Nack;
		}
		if (I2Cx::_ISR::BERR::is_set() || I2Cx::_ISR::ARLO::is_set()) {
			clearErrors();
			return DrvStatus::Error;
		}
		return DrvStatus::Timeout;
	}
};


// Eeprom47L16 (an I2C *device*, not a peripheral) moved to
// drivers/devices/eeprom_47l16.hpp.
