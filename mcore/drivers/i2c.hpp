/*
 * i2c.hpp — I2C master driver (init, polling transfers, DMA transfers)
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
#include "drivers/dma.hpp"

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


/*
 * DMA master transfers, non-blocking: start one, then call poll() until it
 * is no longer Busy (or use the blocking write / write_read below).
 *
 *   using bus = I2CDma<I2C2, i2c2_tx, i2c2_rx>;
 *   bus::start_write(0x56, buf, n);                    // START, n bytes, STOP
 *   bus::start_write_read(0x56, reg, 2, data, 6);      // write, repeated START, read, STOP
 *   if (bus::poll() != DrvStatus::Busy) ...            // Ok / Nack / Error / Timeout
 *
 * The streams are configured by the application with DMA_Config: htx
 * MEM_TO_PERIPH and hrx PERIPH_TO_MEM, bytes, memory increment, NORMAL mode,
 * direct mode (no FIFO); their IRQ handlers call DMA_IRQHandler. The buffers
 * must stay valid until poll() returns something other than Busy; a receive
 * buffer should be alignas(32) (D-cache, see core/cache.hpp). 1..255 bytes
 * per phase (NBYTES, no RELOAD). One transfer at a time per bus; poll()
 * belongs to one context (it is not reentrant).
 *
 * The STOP is generated by the hardware (AUTOEND). The write phase of
 * write_read ends with TC instead, which poll() turns into the read phase,
 * so the restart waits for the next poll() (the bus holds SCL low meanwhile).
 * An error or timeout resets the peripheral (PE off/on) and stops both streams.
 */
template<typename I2Cx, auto& htx, auto& hrx>
struct I2CDma {
	static DrvStatus start_write(uint16_t addr, const uint8_t* buf, size_t len,
	                             uint32_t timeout_ms = I2C_TIMEOUT_MS) {
		if (buf == nullptr || len == 0 || len > 255) return DrvStatus::InvalidArg;
		if (phase != Phase::Idle) return DrvStatus::Busy;
		if (DrvStatus st = begin(timeout_ms); st != DrvStatus::Ok) return st;

		I2Cx::_CR1::TXDMAEN::set();
		if (DrvStatus st = DMA_StartIT<htx>(reinterpret_cast<uint32_t>(buf), I2Cx::_TXDR::address,
		                                    static_cast<uint16_t>(len)); st != DrvStatus::Ok) {
			I2Cx::_CR1::TXDMAEN::clear();
			return st;
		}
		phase = Phase::Write;
		I2Cx::_CR2::overwrite(encodeAddress(addr) | (len << I2Cx::_CR2::NBYTES::pos) |
		                      I2Cx::_CR2::AUTOEND::BitMsk | I2Cx::_CR2::START::BitMsk);
		return DrvStatus::Ok;
	}

	static DrvStatus start_write_read(uint16_t addr, const uint8_t* wbuf, size_t wlen,
	                                  uint8_t* rbuf, size_t rlen, uint32_t timeout_ms = I2C_TIMEOUT_MS) {
		if (wbuf == nullptr || rbuf == nullptr || wlen == 0 || wlen > 255 || rlen == 0 || rlen > 255)
			return DrvStatus::InvalidArg;
		if (phase != Phase::Idle) return DrvStatus::Busy;
		if (DrvStatus st = begin(timeout_ms); st != DrvStatus::Ok) return st;

		I2Cx::_CR1::TXDMAEN::set();
		if (DrvStatus st = DMA_StartIT<htx>(reinterpret_cast<uint32_t>(wbuf), I2Cx::_TXDR::address,
		                                    static_cast<uint16_t>(wlen)); st != DrvStatus::Ok) {
			I2Cx::_CR1::TXDMAEN::clear();
			return st;
		}
		r_addr = addr;
		r_buf = rbuf;
		r_len = static_cast<uint16_t>(rlen);
		phase = Phase::WriteThenRead;
		I2Cx::_CR2::overwrite(encodeAddress(addr) | (wlen << I2Cx::_CR2::NBYTES::pos) |
		                      I2Cx::_CR2::START::BitMsk);              // no AUTOEND: TC after the write
		return DrvStatus::Ok;
	}

	// Advance the transfer: Busy while it runs, then its result once (Idle again)
	static DrvStatus poll() {
		if (phase == Phase::Idle) return DrvStatus::Ok;
		const uint32_t isr = I2Cx::_ISR::read();

		if (isr & I2Cx::_ISR::NACKF::BitMsk) {
			// address or data not acknowledged: the hardware ends with STOP
			// in AUTOEND mode, the write phase of write_read needs it by hand
			if (phase == Phase::WriteThenRead) I2Cx::_CR2::STOP::set();
			return finish(DrvStatus::Nack);
		}
		if (isr & (I2Cx::_ISR::BERR::BitMsk | I2Cx::_ISR::ARLO::BitMsk)) return finish(DrvStatus::Error);
		if (get_tick() - t0 > timeout) return finish(DrvStatus::Timeout);

		if (phase == Phase::WriteThenRead) {
			if (!(isr & I2Cx::_ISR::TC::BitMsk)) return DrvStatus::Busy;
			I2Cx::_CR1::TXDMAEN::clear();
			I2Cx::_CR1::RXDMAEN::set();
			if (DMA_StartIT<hrx>(I2Cx::_RXDR::address, reinterpret_cast<uint32_t>(r_buf), r_len) != DrvStatus::Ok) {
				I2Cx::_CR2::STOP::set();
				return finish(DrvStatus::Error);
			}
			phase = Phase::Read;
			I2Cx::_CR2::overwrite(encodeAddress(r_addr) | (uint32_t{r_len} << I2Cx::_CR2::NBYTES::pos) |
			                      I2Cx::_CR2::RD_WRN::BitMsk | I2Cx::_CR2::AUTOEND::BitMsk |
			                      I2Cx::_CR2::START::BitMsk);
			return DrvStatus::Busy;
		}

		// Write / Read: done at STOP, once the stream has finished too (its
		// interrupt has made received data visible to the CPU)
		if (!(isr & I2Cx::_ISR::STOPF::BitMsk)) return DrvStatus::Busy;
		if (phase == Phase::Read ? hrx.busy() : htx.busy()) return DrvStatus::Busy;
		I2Cx::_ICR::overwrite(I2Cx::_ICR::STOPCF::BitMsk);
		I2Cx::_CR1::template clearMask<I2Cx::_CR1::TXDMAEN::BitMsk | I2Cx::_CR1::RXDMAEN::BitMsk>();
		phase = Phase::Idle;
		return DrvStatus::Ok;
	}

	static bool idle() noexcept { return phase == Phase::Idle; }

	// Blocking forms: start, then poll until done
	static DrvStatus write(uint16_t addr, const uint8_t* buf, size_t len, uint32_t timeout_ms = I2C_TIMEOUT_MS) {
		DrvStatus st = start_write(addr, buf, len, timeout_ms);
		if (st != DrvStatus::Ok) return st;
		while ((st = poll()) == DrvStatus::Busy) {}
		return st;
	}
	static DrvStatus write_read(uint16_t addr, const uint8_t* wbuf, size_t wlen, uint8_t* rbuf, size_t rlen,
	                            uint32_t timeout_ms = I2C_TIMEOUT_MS) {
		DrvStatus st = start_write_read(addr, wbuf, wlen, rbuf, rlen, timeout_ms);
		if (st != DrvStatus::Ok) return st;
		while ((st = poll()) == DrvStatus::Busy) {}
		return st;
	}

private:
	enum class Phase : uint8_t { Idle, Write, WriteThenRead, Read };
	static inline Phase phase = Phase::Idle;
	static inline uint32_t t0 = 0;
	static inline uint32_t timeout = 0;
	static inline uint16_t r_addr = 0;
	static inline uint8_t* r_buf = nullptr;
	static inline uint16_t r_len = 0;

	static constexpr uint32_t encodeAddress(uint16_t addr) { return (addr & 0x7Fu) << 1; }   // 7-bit

	static DrvStatus begin(uint32_t timeout_ms) {
		if (I2Cx::_ISR::BUSY::is_set()) return DrvStatus::Busy;     // someone else on the bus
		I2Cx::_ICR::overwrite(I2Cx::_ICR::STOPCF::BitMsk | I2Cx::_ICR::NACKCF::BitMsk |
		                      I2Cx::_ICR::BERRCF::BitMsk | I2Cx::_ICR::ARLOCF::BitMsk);
		t0 = get_tick();
		timeout = timeout_ms;
		return DrvStatus::Ok;
	}

	// End a transfer that went wrong: streams stopped, peripheral reset
	// (PE off clears the state machine and the flags), then Idle
	static DrvStatus finish(DrvStatus st) {
		(void)DMA_Stop<htx>();
		(void)DMA_Stop<hrx>();
		I2Cx::_CR1::template clearMask<I2Cx::_CR1::TXDMAEN::BitMsk | I2Cx::_CR1::RXDMAEN::BitMsk>();
		if (st != DrvStatus::Nack) {
			I2Cx::_CR1::PE::clear();
			while (I2Cx::_CR1::PE::is_set()) {}
			I2Cx::_CR1::PE::set();
		} else {
			// wait (bounded) for the STOP that follows the NACK, then clear
			const uint32_t ts = get_tick();
			while (!I2Cx::_ISR::STOPF::is_set() && get_tick() - ts < 2) {}
			I2Cx::_ICR::overwrite(I2Cx::_ICR::STOPCF::BitMsk | I2Cx::_ICR::NACKCF::BitMsk);
		}
		phase = Phase::Idle;
		return st;
	}
};

// Eeprom47L16 (an I2C *device*, not a peripheral) moved to
// drivers/devices/eeprom_47l16.hpp.
