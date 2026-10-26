/*
 * cache.hpp — D-cache maintenance by address range (Cortex-M7, 32-byte lines)
 *
 *   cache::clean(buf, n);             // CPU wrote it, a DMA master reads it next
 *   cache::invalidate(buf, n);        // a DMA master wrote it, the CPU reads it next
 *
 * All functions do nothing while the D-cache is off and for ranges in DTCM
 * (0x20000000..0x2001FFFF), which is never cached.
 *
 * Invalidation works on whole lines: a line that is shared with other data
 * (the buffer does not start / end on a 32-byte boundary) is cleaned and
 * invalidated instead, so CPU writes to the neighbours are kept. A DMA
 * receive buffer must still not share a line with data the CPU writes WHILE
 * the transfer runs (the write-back would overwrite received bytes): give it
 * alignas(32) and a size that is a multiple of 32, or put it in DTCM.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "core/def.hpp"
#include "core/regs.hpp"

namespace cache {

constexpr uint32_t LINE = 32;
constexpr uint32_t DTCM_BASE = 0x20000000;
constexpr uint32_t DTCM_END = 0x20020000;

inline bool dcache_on() {
	return SCB::_CCR::DC::is_set();
}

// True when [addr, addr + n) may have lines in the D-cache
inline bool cached(uint32_t addr, uint32_t n) {
	if (n == 0 || !dcache_on()) return false;
	return !(addr >= DTCM_BASE && addr + n <= DTCM_END);
}

// Write dirty lines back to memory (they stay valid)
inline void clean(uint32_t addr, uint32_t n) {
	if (!cached(addr, n)) return;
	__DSB();
	for (uint32_t a = addr & ~(LINE - 1); a < addr + n; a += LINE) Cache::_DCCMVAC::overwrite(a);
	__DSB();
}

// Write dirty lines back, then drop them
inline void clean_invalidate(uint32_t addr, uint32_t n) {
	if (!cached(addr, n)) return;
	__DSB();
	for (uint32_t a = addr & ~(LINE - 1); a < addr + n; a += LINE) Cache::_DCCIMVAC::overwrite(a);
	__DSB();
	__ISB();
}

// Drop lines so the next read comes from memory; partial lines at either end
// are cleaned first (see the note at the top)
inline void invalidate(uint32_t addr, uint32_t n) {
	if (!cached(addr, n)) return;
	const uint32_t end = addr + n;
	__DSB();
	for (uint32_t a = addr & ~(LINE - 1); a < end; a += LINE) {
		if (a < addr || a + LINE > end) {
			Cache::_DCCIMVAC::overwrite(a);
		} else {
			Cache::_DCIMVAC::overwrite(a);
		}
	}
	__DSB();
	__ISB();
}

inline void clean(const void *p, size_t n) { clean(reinterpret_cast<uint32_t>(p), n); }
inline void clean_invalidate(const void *p, size_t n) { clean_invalidate(reinterpret_cast<uint32_t>(p), n); }
inline void invalidate(const void *p, size_t n) { invalidate(reinterpret_cast<uint32_t>(p), n); }

} // namespace cache
