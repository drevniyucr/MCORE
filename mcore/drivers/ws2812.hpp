#pragma once
#include <cstdint>
#include <cstddef>
#include "drivers/tim.hpp"
#include "drivers/dma.hpp"

constexpr uint32_t LED_BITS = 24;

// Bit period is 1.25 us (800 kHz): Period = timer_clock / 800 kHz, ARR = Period - 1.
//   STM32F767: TIM1 @ 216 MHz -> Period 270
//   STM32H755: TIM1 @ 200 MHz -> Period 250
constexpr uint16_t WS2812_DEFAULT_PERIOD = 270;

// reset > 50us → ~40 бит минимум
constexpr uint32_t RESET_SLOTS = 64;

template<typename Timer, TimCh ch, auto &dma, size_t LED_COUNT,
         uint16_t Period = WS2812_DEFAULT_PERIOD>
class WS2812 {
	// "1" = 0.8 us high (64 % of the bit), "0" = 0.4 us high (32 %)
	static constexpr uint16_t DUTY_1 = static_cast<uint16_t>((Period * 16U + 12U) / 25U);
	static constexpr uint16_t DUTY_0 = static_cast<uint16_t>((Period * 8U + 12U) / 25U);

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
