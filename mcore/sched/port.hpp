#pragma once
/*
 * sched port: interrupt masking, pending a task's vector, NVIC setup, cycles.
 *
 * Cortex-M (default): inline register access, no CMSIS needed.
 * Host (MCORE_SCHED_HOST): port_host.cpp emulates the NVIC so that the
 * scheduler semantics (preemption, tail-chaining, BASEPRI ceilings) can be
 * unit-tested on the PC.
 */
#include <cstdint>

#include "sched.hpp"

namespace sched {
namespace port {

#if defined(MCORE_SCHED_HOST)

uint32_t crit_enter();
void crit_exit(uint32_t key);
void pend(uint16_t irq);
uint32_t basepri();
void set_basepri(uint32_t level);     // 0 = no masking, else NVIC level
void irq_setup(uint16_t irq, uint8_t level);
void systick_level(uint8_t level);
uint32_t cycles();

#else

inline uint32_t crit_enter() {
	uint32_t primask;
	__asm volatile("mrs %0, primask\n\tcpsid i" : "=r"(primask)::"memory");
	return primask;
}

inline void crit_exit(uint32_t key) {
	__asm volatile("msr primask, %0" ::"r"(key) : "memory");
}

// Software trigger of an interrupt (NVIC STIR); the barriers make a more
// urgent task run before the next instruction of the caller
inline void pend(uint16_t irq) {
	*reinterpret_cast<volatile uint32_t *>(0xE000EF00UL) = irq;
	__asm volatile("dsb\n\tisb" ::: "memory");
}

inline uint32_t basepri() {
	uint32_t v;
	__asm volatile("mrs %0, basepri" : "=r"(v));
	return v >> (8U - NVIC_PRIO_BITS);
}

// Cortex-M7 r0p1 erratum 837070: BASEPRI writes need interrupts disabled
inline void set_basepri(uint32_t level) {
	const uint32_t v = level << (8U - NVIC_PRIO_BITS);
	const uint32_t key = crit_enter();
	__asm volatile("msr basepri, %0" ::"r"(v) : "memory");
	crit_exit(key);
	__asm volatile("isb" ::: "memory");
}

inline void irq_setup(uint16_t irq, uint8_t level) {
	reinterpret_cast<volatile uint8_t *>(0xE000E400UL)[irq] = static_cast<uint8_t>(level << (8U - NVIC_PRIO_BITS));
	reinterpret_cast<volatile uint32_t *>(0xE000E100UL)[irq >> 5] = 1UL << (irq & 31U);
}

inline void systick_level(uint8_t level) {
	*reinterpret_cast<volatile uint8_t *>(0xE000ED23UL) = static_cast<uint8_t>(level << (8U - NVIC_PRIO_BITS));
}

inline uint32_t cycles() {
	return *reinterpret_cast<volatile uint32_t *>(0xE0001004UL);   // DWT_CYCCNT (enabled by SystemInit)
}

#endif

} // namespace port

#if defined(MCORE_SCHED_HOST)
// Test helpers of the host port
namespace host {
void reset();                                              // forget pending state, priority 0
void run_isr(uint8_t level, void (*fn)(void *), void *ctx);  // fn runs as an ISR at that NVIC level
uint8_t active_level();                                    // 16 = thread mode
void set_cycles(uint32_t c);
} // namespace host
#endif

} // namespace sched
