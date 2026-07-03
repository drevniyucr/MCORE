#pragma once
#include <stdint.h>
#include "mcore_tim.hpp"
#include "mcore_dma.hpp"

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

	static void fill_rgb_gradient(uint8_t offset) {
		if (!ready())
			return;

		for (size_t i = 0; i < LED_COUNT; i++) {
			// вычисляем позицию в цветовом круге (0.0 - 255.0)
			float hue = (static_cast<float>(i) / LED_COUNT) * 255.0f;
			hue += static_cast<float>(offset);
			if (hue >= 255.0f)
				hue -= 255.0f;

			uint8_t r, g, b;

			if (hue < 85.0f) {
				r = static_cast<uint8_t>(255.0f - hue * 3.0f);
				g = static_cast<uint8_t>(hue * 3.0f);
				b = 0;
			} else if (hue < 170.0f) {
				hue -= 85.0f;
				r = 0;
				g = static_cast<uint8_t>(255.0f - hue * 3.0f);
				b = static_cast<uint8_t>(hue * 3.0f);
			} else {
				hue -= 170.0f;
				r = static_cast<uint8_t>(hue * 3.0f);
				g = 0;
				b = static_cast<uint8_t>(255.0f - hue * 3.0f);
			}

			leds[i].r = r;
			leds[i].g = g;
			leds[i].b = b;
		}
	}

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

	static inline volatile uint16_t dma_buffer[TOTAL_SLOTS];

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
