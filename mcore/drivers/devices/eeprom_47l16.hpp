/*
 * eeprom_47l16.hpp — 47L16 I2C EERAM device driver.
 *
 * Device drivers (chips hanging off a bus) live under drivers/devices/,
 * separate from the peripheral drivers they use.
 */
#pragma once

#include <cstdint>
#include <cstddef>
#include "core/system.hpp"
#include "drivers/common.hpp"
#include "drivers/i2c.hpp"

template<typename I2Cx>
struct Eeprom47L16 {

	static constexpr uint8_t I2C_ADDR = 0x56;
	// TODO: hardware-verify a shorter value / ACK polling. 47L16 is an EERAM
	// (SRAM-backed), so the classic EEPROM write-cycle wait is likely
	// unnecessary — kept as-is until measured on the board.
	static constexpr uint32_t WRITE_DELAY_MS = 1000;
	// Largest single write (address prologue + data) without heap/VLA.
	static constexpr size_t MAX_WRITE = 64;

	/* Write bytes to EERAM. len is limited to MAX_WRITE per call. */
	static bool write(uint16_t memAddr, const uint8_t* data, size_t len) {
		if (len > MAX_WRITE)
			return false;

		uint8_t buf[2 + MAX_WRITE];   // fixed size: no VLA (non-standard C++)

		buf[0] = static_cast<uint8_t>(memAddr >> 8);
		buf[1] = static_cast<uint8_t>(memAddr & 0xFF);

		for (size_t i = 0; i < len; ++i)
			buf[2 + i] = data[i];

		auto st = I2CHandle<I2Cx>::write(I2C_ADDR, buf, 2 + len);
		if (st != DrvStatus::Ok)
			return false;

		delay_ms(WRITE_DELAY_MS);
		return true;
	}

	/* Read bytes from EERAM. */
	static bool read(uint16_t memAddr, uint8_t* data, size_t len) {
		uint8_t addr[2] = {
			static_cast<uint8_t>(memAddr >> 8),
			static_cast<uint8_t>(memAddr & 0xFF)
		};

		/* Set memory address */
		auto st = I2CHandle<I2Cx>::write(I2C_ADDR, addr, 2);
		if (st != DrvStatus::Ok)
			return false;

		/* Read data */
		st = I2CHandle<I2Cx>::read(I2C_ADDR, data, len);

		return (st == DrvStatus::Ok);
	}
};
