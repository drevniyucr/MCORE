/*
 * mcore_usart.hpp
 *
 *  Created on: 26 февр. 2026 г.
 *      Author: AkimovMA
 */
#pragma once

#include <stdint.h>
#include "mcore_regs.hpp"
#include "mcore_system.hpp"
#include "mcore_conf.hpp"

constexpr uint32_t I2C_TIMING_MASK = 0xF0FFFFFF;

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

enum class I2CStatus : uint32_t {
	Success = 0, BaudOutOfRange = 1, Error = 2, Timeout = 3
};


template<typename I2Cx>
static inline void I2C_ClkSel(I2CClockSrc src);
template <>
 inline void I2C_ClkSel<I2C1>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C1SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1ENR::I2C1EN::set();
}
template <>
inline void I2C_ClkSel<I2C2>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C2SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1ENR::I2C2EN::set();
}
template <>
inline void I2C_ClkSel<I2C3>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C3SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1ENR::I2C3EN::set();
}
template <>
inline void I2C_ClkSel<I2C4>(I2CClockSrc src){
		RCC::_DKCFGR2::I2C4SEL::write(static_cast<uint32_t>(src));
		RCC::_APB1ENR::I2C4EN::set();
}


template<typename I2Cx, I2CHwConfig cfg>
static inline I2CStatus I2C_Init() {

    static_assert(cfg.DigitalFilterCoeff <= 15);
    static_assert(cfg.OwnAddr2Mask <= 7);
	/* 0. Clock enable */
	I2C_ClkSel<I2Cx>(cfg.ClockSource);

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
    
    return I2CStatus::Success;
}

enum class I2CPollStatus : uint32_t {
	Success = 0,
	Timeout,
	Nack,
	Error
};

template<typename I2Cx>
struct I2CHandle {

	/* Write buffer to slave (polling) */
	static inline I2CPollStatus write(
		uint16_t addr,
		const uint8_t* buf,
		size_t len,
		bool addr10 = false,
		uint32_t timeout = 100000
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
			if (!waitTXIS(timeout))
				return checkErrorStatus();

			I2Cx::_TXDR::overwrite(buf[i]);
		}

		if (!waitTC(timeout))
			return checkErrorStatus();

		I2Cx::_CR2::STOP::set();
		return I2CPollStatus::Success;
	}

	/* Read buffer from slave (polling) */
	static inline I2CPollStatus read(
		uint16_t addr,
		uint8_t* buf,
		size_t len,
		bool is_addr10 = false,
		uint32_t timeout = 100000
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
			if (!waitRXNE(timeout))
				return checkErrorStatus();

			buf[i] = static_cast<uint8_t>(I2Cx::_RXDR::read());
		}

		if (!waitTC(timeout))
			return checkErrorStatus();

		I2Cx::_CR2::STOP::set();
		return I2CPollStatus::Success;
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

	static inline bool waitTXIS(uint32_t timeout) {
		while (!I2Cx::_ISR::TXIS::is_set()) {
			if (I2Cx::_ISR::NACKF::is_set()) return false;
			if (--timeout == 0) return false;
		}
		return true;
	}

	static inline bool waitRXNE(uint32_t timeout) {
		while (!I2Cx::_ISR::RXNE::is_set()) {
			if (I2Cx::_ISR::NACKF::is_set()) return false;
			if (--timeout == 0) return false;
		}
		return true;
	}

	static inline bool waitTC(uint32_t timeout) {
		while (!I2Cx::_ISR::TC::is_set()) {
			if (I2Cx::_ISR::NACKF::is_set()) return false;
			if (--timeout == 0) return false;
		}
		return true;
	}

	static inline void clearErrors() {
		I2Cx::_ICR::NACKCF::set();
		I2Cx::_ICR::BERRCF::set();
		I2Cx::_ICR::ARLOCF::set();
	}

	static inline I2CPollStatus checkErrorStatus() {
		if (I2Cx::_ISR::NACKF::is_set()) {
			I2Cx::_ICR::NACKCF::set();
			return I2CPollStatus::Nack;
		}
		if (I2Cx::_ISR::BERR::is_set() || I2Cx::_ISR::ARLO::is_set()) {
			clearErrors();
			return I2CPollStatus::Error;
		}
		return I2CPollStatus::Timeout;
	}
};


template<typename I2Cx>
struct Eeprom47L16 {

	static constexpr uint8_t I2C_ADDR = 0x56;
	static constexpr uint32_t WRITE_DELAY = 1000; // polling delay

	/* Write bytes to EEPROM */
	static bool write(uint16_t memAddr, const uint8_t* data, size_t len) {

		uint8_t buf[2 + len];

		buf[0] = static_cast<uint8_t>(memAddr >> 8);
		buf[1] = static_cast<uint8_t>(memAddr & 0xFF);

		for (size_t i = 0; i < len; ++i)
			buf[2 + i] = data[i];

		auto st = I2CHandle<I2Cx>::write(
			I2C_ADDR,
			buf,
			2 + len,
			false,
			700
		);

		if (st != I2CPollStatus::Success)
			return false;

		/* EEPROM write cycle time (ACK polling could be added later) */
		delay_ms(WRITE_DELAY);
		return true;
	}

	/* Read bytes from EEPROM */
	static bool read(uint16_t memAddr, uint8_t* data, size_t len) {
		uint8_t addr[2] = {
			static_cast<uint8_t>(memAddr >> 8),
			static_cast<uint8_t>(memAddr & 0xFF)
		};

		/* Set memory address */
		auto st = I2CHandle<I2Cx>::write(
			I2C_ADDR,
			addr,
			2
		);
		if (st != I2CPollStatus::Success)
			return false;

		/* Read data */
		st = I2CHandle<I2Cx>::read(
			I2C_ADDR,
			data,
			len,
			false,
			1000
		);

		return (st == I2CPollStatus::Success);
	}
};
