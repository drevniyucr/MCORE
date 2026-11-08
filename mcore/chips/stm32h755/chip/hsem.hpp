#pragma once
/*
 * STM32H755 hardware semaphores (HSEM, RM0399 ch. 11) and dual-core boot.
 *
 * MCORE uses HSEM only as a doorbell between the cores: a core "rings"
 * semaphore n by taking and immediately releasing it; the release sets
 * bit n in the other core's HSEM_CxISR and, if enabled there, raises its
 * HSEM interrupt (HSEM1_IRQn on the Cortex-M7, HSEM2_IRQn on the Cortex-M4).
 * Data itself goes through shared memory (mcore/ipc).
 *
 * The register file is indexed (R0..R31, RLR0..RLR31), so this driver
 * addresses it by offset from the generated HSEM base address.
 */
#include "core/def.hpp"
#include "chip/core.hpp"

namespace hsem {

constexpr uint32_t CORE_ID_CM7 = 0x3U;   // HSEM_CPU1_COREID
constexpr uint32_t CORE_ID_CM4 = 0x1U;   // HSEM_CPU2_COREID
// HSEM core id of the core running this code
inline uint32_t core_id() {
	return chip::on_cm4() ? CORE_ID_CM4 : CORE_ID_CM7;
}

constexpr uint32_t COUNT = 32U;

namespace detail {
constexpr uint32_t BASE = HSEM::HSEM_BASE;
constexpr uint32_t R    = 0x000U;   // R0..R31: 2-step lock / release
constexpr uint32_t RLR  = 0x080U;   // RLR0..RLR31: 1-step (read) lock
// Interrupt registers of the core running this code: C1 (Cortex-M7) at
// 0x100, C2 (Cortex-M4) at 0x110
inline uint32_t irq_base() { return chip::on_cm4() ? 0x110U : 0x100U; }
inline uint32_t IER()  { return irq_base() + 0x0U; }
inline uint32_t ICR()  { return irq_base() + 0x4U; }
inline uint32_t ISR()  { return irq_base() + 0x8U; }
inline uint32_t MISR() { return irq_base() + 0xCU; }
constexpr uint32_t LOCK = 1UL << 31;
constexpr uint32_t COREID_POS = 8U;

inline volatile uint32_t &reg(uint32_t offset) {
	return *reinterpret_cast<volatile uint32_t *>(BASE + offset);
}
} // namespace detail

// HSEM clock for this core (the RCC enable registers are per-core aliases).
inline void clock_enable() {
	RCC::_AHB4ENR::HSEMEN::set();
	(void) RCC::_AHB4ENR::read();
}

// 1-step lock with process ID 0. True if this core now owns the semaphore.
inline bool try_take(uint32_t id) {
	using namespace detail;
	return reg(RLR + 4U * id) == (LOCK | (core_id() << COREID_POS));
}

// Release (process ID 0). Sets bit id in every core's HSEM_CxISR.
inline void release(uint32_t id) {
	using namespace detail;
	reg(R + 4U * id) = core_id() << COREID_POS;
}

// Doorbell: take + release. Returns false if the other core holds the
// semaphore (only possible if both cores use the same id as a mutex).
inline bool notify(uint32_t id) {
	if (!try_take(id)) {
		return false;
	}
	__DMB();   // shared-memory writes before the other core sees the event
	release(id);
	return true;
}

// Interrupt flags of THIS core
inline void irq_enable(uint32_t mask)  { detail::reg(detail::IER()) |= mask; }
inline void irq_disable(uint32_t mask) { detail::reg(detail::IER()) &= ~mask; }
inline uint32_t pending()        { return detail::reg(detail::ISR()); }    // raw, before masking
inline uint32_t pending_masked() { return detail::reg(detail::MISR()); }
inline void clear(uint32_t mask) { detail::reg(detail::ICR()) = mask; }

constexpr uint32_t mask(uint32_t id) { return 1UL << id; }

// ---------------------------------------------------------------------------
// Dual-core boot handshake (semaphore BOOT_SEM).
//
// Both cores can start at reset (option byte BCM4 = 1, factory default) or
// the M4 can be gated (BCM4 = 0). Either way the M4 must not run its
// application before the M7 has set up power, clocks, GPIO and the shared
// memory. The HSEM status flags are cleared by every system reset, so a
// stale "go" from a previous run is impossible.
// ---------------------------------------------------------------------------
constexpr uint32_t BOOT_SEM = 0U;

// Cortex-M7 side
// RCC_GCR.BOOT_C2 (bit 3, not in the CM7 SVD): starts the M4 when BCM4 = 0;
// harmless when it is already running.
inline void release_cm4() {
	clock_enable();
	(void) notify(BOOT_SEM);
	RCC::_GCR::setMask(1UL << 3);
}

// Cortex-M4 side
// Spins until the M7 has released the boot semaphore, then clears the flag.
inline void wait_for_cm7() {
	clock_enable();
	while ((pending() & mask(BOOT_SEM)) == 0U) {
	}
	clear(mask(BOOT_SEM));
}

} // namespace hsem
