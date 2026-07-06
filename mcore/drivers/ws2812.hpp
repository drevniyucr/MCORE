#pragma once
#include <cstdint>
#include <cstddef>
#include "drivers/tim.hpp"
#include "drivers/dma.hpp"

constexpr uint32_t LED_BITS = 24;

// Тайминги (под ARR ≈ 270)
constexpr uint16_t DUTY_1 = 173; // ~2/3 172.8
constexpr uint16_t DUTY_0 = 86;  // ~1/3 86.4

// reset > 50us → ~40 бит минимум
constexpr uint32_t RESET_SLOTS = 64;

template<typename Timer, TimCh ch, auto &dma, size_t LED_COUNT>
class WS2812 {
public:

	struct RGB {
		uint8_t g;
		uint8_t r;
		uint8_t b;
	};

	static inline RGB leds[LED_COUNT];

	// =========================
	// API
	// =========================

	static bool ready() {
		return dma.state == DmaState::READY;
	}

	// Animations (rainbow etc.) are application code — see app/main.cpp.
	// The driver only exposes the pixel buffer (leds) and show()/ready().

	static void show() {
		if (!ready())
			return;

		encode_all();

		TIM_PWM_ChannelDMAEnable<Timer, ch, dma>(
				reinterpret_cast<uint32_t>(dma_buffer), TOTAL_SLOTS);
	}

private:

	// =========================
	// Buffer
	// =========================

	static constexpr uint32_t DATA_SLOTS = LED_COUNT * LED_BITS;
	static constexpr uint32_t TOTAL_SLOTS = DATA_SLOTS + RESET_SLOTS;

	// NOT volatile: the CPU only writes this buffer before the transfer starts,
	// and DMA_StartIT() issues a __DSB() before enabling the stream — that is
	// what publishes the data to the DMA master. volatile here would only
	// forbid the compiler from optimising the encode loop (one halfword store
	// per slot instead of coalesced writes).
	static inline uint16_t dma_buffer[TOTAL_SLOTS];

	// =========================
	// Encoding
	// =========================

	static void encode_all() {
		uint32_t idx = 0;

		// --- DATA ---
		for (uint32_t led = 0; led < LED_COUNT; led++) {
			encode_byte(leds[led].g, idx);
			encode_byte(leds[led].r, idx);
			encode_byte(leds[led].b, idx);
		}

		// --- RESET ---
		for (; idx < TOTAL_SLOTS; idx++) {
			dma_buffer[idx] = 0;
		}
	}

	static void encode_byte(uint8_t byte, uint32_t &idx) {
		for (int i = 7; i >= 0; i--) {
			dma_buffer[idx++] = (byte & (1 << i)) ? DUTY_1 : DUTY_0;
		}
	}
};
