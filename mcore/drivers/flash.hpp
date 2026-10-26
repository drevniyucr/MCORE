/*
 * flash.hpp — internal flash: sector erase and word programming (STM32F76x/77x)
 *
 *   flash::erase_sector(9);                       // 256 KB at 0x08140000
 *   flash::program(0x08140000, data, n);          // n and address multiples of 4
 *
 * Single-bank layout only (option bit nDBANK = 1, the factory default):
 * sectors 0-3 32 KB, 4 128 KB, 5-11 256 KB. In dual-bank mode sector numbers
 * and sizes differ, so the functions refuse to touch anything there.
 *
 * With one bank, every instruction fetch from flash stalls while a sector is
 * erased (~1-2 s for 256 KB): interrupt handlers in flash, SysTick and the
 * watchdog refresh included. The caller decides when that is acceptable and
 * stretches the watchdog (IWDG_SetTimeout) around it. Programming a word
 * stalls only ~16 us.
 *
 * Program size x32 needs VDD 2.7..3.6 V (PSIZE = 0b10).
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "core/regs.hpp"
#include "core/system.hpp"
#include "drivers/common.hpp"

namespace flash {

constexpr uint32_t BASE = 0x08000000;
constexpr uint8_t  SECTORS = 12;
constexpr uint32_t KEY1 = 0x45670123;
constexpr uint32_t KEY2 = 0xCDEF89AB;
constexpr uint32_t PSIZE_X32 = 0b10;
constexpr uint32_t ERASE_TIMEOUT_MS = 4000;     // 256 KB sector: 1..2 s typical (RM0410)
constexpr uint32_t PROGRAM_TIMEOUT_MS = 5;

// OPERR | WRPERR | PGAERR | PGPERR | ERSERR (bit 7, "PGSERR" in the SVD)
constexpr uint32_t SR_ERRORS = Flash::_SR::OPERR::BitMsk | Flash::_SR::WRPERR::BitMsk |
                               Flash::_SR::PGAERR::BitMsk | Flash::_SR::PGPERR::BitMsk |
                               Flash::_SR::PGSERR::BitMsk;

constexpr uint32_t sector_size(uint8_t s) {
	return s < 4 ? 32U * 1024 : s == 4 ? 128U * 1024 : 256U * 1024;
}
constexpr uint32_t sector_addr(uint8_t s) {
	uint32_t a = BASE;
	for (uint8_t i = 0; i < s; ++i) a += sector_size(i);
	return a;
}
static_assert(sector_addr(9) == 0x08140000 && sector_addr(11) + sector_size(11) == 0x08200000);

inline bool single_bank() {
	return Flash::_OPTCR::nDBANK::is_set();
}

namespace detail {

inline void unlock() {
	if (Flash::_CR::LOCK::is_set()) {
		Flash::_KEYR::overwrite(KEY1);
		Flash::_KEYR::overwrite(KEY2);
	}
}

inline void lock() {
	Flash::_CR::LOCK::set();
}

inline DrvStatus wait_idle(uint32_t timeout_ms) {
	const uint32_t t0 = get_tick();
	while (Flash::_SR::BSY::is_set()) {
		if (get_tick() - t0 > timeout_ms) return DrvStatus::Timeout;
	}
	const uint32_t sr = Flash::_SR::read();
	Flash::_SR::overwrite(sr & (SR_ERRORS | Flash::_SR::EOP::BitMsk));   // rc_w1
	return (sr & SR_ERRORS) ? DrvStatus::Error : DrvStatus::Ok;
}

// Flash is read through the D-cache when it is on: drop the stale lines
inline void invalidate_dcache(uint32_t addr, uint32_t n) {
	if (!SCB::_CCR::DC::is_set()) return;
	__DSB();
	for (uint32_t a = addr & ~31U; a < addr + n; a += 32) {
		Cache::_DCIMVAC::overwrite(a);
	}
	__DSB();
	__ISB();
}

} // namespace detail

// Erase one sector (blocks; see the stall note above)
inline DrvStatus erase_sector(uint8_t sector) {
	if (sector >= SECTORS || !single_bank()) return DrvStatus::InvalidArg;
	detail::unlock();
	DrvStatus st = detail::wait_idle(PROGRAM_TIMEOUT_MS);
	if (st == DrvStatus::Ok) {
		Flash::_CR::overwrite(Flash::_CR::SER::BitMsk |
		                      (uint32_t{sector} << Flash::_CR::SNB::pos) |
		                      (PSIZE_X32 << Flash::_CR::PSIZE::pos));
		Flash::_CR::STRT::set();
		__DSB();
		st = detail::wait_idle(ERASE_TIMEOUT_MS);
		Flash::_CR::overwrite(0u);
	}
	detail::lock();
	detail::invalidate_dcache(sector_addr(sector), sector_size(sector));
	return st;
}

// Program n bytes (address and n multiples of 4) into erased flash, word by
// word; words that are already 0xFFFFFFFF are skipped. Verified on return.
inline DrvStatus program(uint32_t addr, const void *data, size_t n) {
	if ((addr % 4) != 0 || (n % 4) != 0 || addr < BASE || addr + n > BASE + 2 * 1024 * 1024 ||
	    !single_bank()) {
		return DrvStatus::InvalidArg;
	}
	const auto *src = static_cast<const uint8_t *>(data);
	detail::unlock();
	DrvStatus st = detail::wait_idle(PROGRAM_TIMEOUT_MS);
	if (st == DrvStatus::Ok) {
		Flash::_CR::overwrite(Flash::_CR::PG::BitMsk | (PSIZE_X32 << Flash::_CR::PSIZE::pos));
		for (size_t i = 0; i < n && st == DrvStatus::Ok; i += 4) {
			uint32_t w;
			memcpy(&w, src + i, 4);
			if (w == 0xFFFFFFFFU) continue;
			*reinterpret_cast<volatile uint32_t *>(addr + i) = w;
			__DSB();
			st = detail::wait_idle(PROGRAM_TIMEOUT_MS);
		}
		Flash::_CR::overwrite(0u);
	}
	detail::lock();
	detail::invalidate_dcache(addr, static_cast<uint32_t>(n));
	if (st == DrvStatus::Ok && memcmp(reinterpret_cast<const void *>(addr), data, n) != 0) {
		st = DrvStatus::Error;
	}
	return st;
}

} // namespace flash
