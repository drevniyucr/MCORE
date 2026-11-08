/*
 * mcore_gpio.hpp
 *
 *  Created on: Sep 15, 2025
 *      Author: AkimovMA
 */
#pragma once

#include "core/def.hpp"
#include "core/system.hpp"
#include <stddef.h>

// Режим работы пина (MODER)
enum class Mode : uint8_t {
	In = 0b00,  // Вход
	Out = 0b01,  // Выход
	Alt = 0b10,  // Альтернативная функция (AF)
	Ang = 0b11   // Аналоговый режим
};

// Тип выхода (OTYPER)
enum class OType : uint8_t {
	PP = 0,  // Тотемный выход (обычный)
	OD = 1   // Открытый сток
};

// Скорость выхода (OSPEEDR)
enum class Speed : uint8_t {
	Low = 0b00,  // Низкая
	Medium = 0b01,  // Средняя
	High = 0b10,  // Высокая
	VeryHigh = 0b11   // Очень высокая
};

// Подтяжка (PUPDR)
enum class Pull : uint8_t {
	None = 0b00,  // Без подтяжки
	PullUp = 0b01,  // Подтяжка к VCC
	PullDown = 0b10  // Подтяжка к GND
};

// Альтернативные функции (AFR)
enum class AF : uint8_t {
	AF0 = 0,  // Системные функции
	AF1 = 1,
	AF2 = 2,
	AF3 = 3,
	AF4 = 4,
	AF5 = 5,
	AF6 = 6,
	AF7 = 7,  // USART1/2
	AF8 = 8,  // USART6
	AF9 = 9,  // CAN, TIM
	AF10 = 10, // OTG_FS
	AF11 = 11, // Ethernet
	AF12 = 12, // FMC/SDIO
	AF13 = 13, // DCMI
	AF14 = 14, // LTDC
	AF15 = 15  // Eventout
};

struct GPIO_Config {
	Mode mode;        // Режим работы
	OType otype;      // Тип выхода
	Speed speed;      // Скорость
	Pull pull;        // Подтяжка
	AF af;            // Альтернативная функция
};

template<typename Pin>
void GPIO_ConfigPin(GPIO_Config &cfg) {

	using Port = typename Pin::port_type;
	uint32_t pos = Pin::num * 2;
	uint32_t temp = 0x00;

	// --- MODER ---
	temp = Port::_MODER::read();
	temp &= ~(3U << pos);
	temp |= (static_cast<uint32_t>(cfg.mode) << pos);
	Port::_MODER::overwrite(temp);

	// --- OTYPER ---
	temp = Port::_OTYPER::read();
	temp &= ~(1U << Pin::num);
	temp |= (static_cast<uint32_t>(cfg.otype) << Pin::num);
	Port::_OTYPER::overwrite(temp);

	// --- OSPEEDR ---
	temp = Port::_OSPEEDR::read();
	temp &= ~(3U << pos);
	temp |= (static_cast<uint32_t>(cfg.speed) << pos);
	Port::_OSPEEDR::overwrite(temp);

	// --- PUPDR ---
	temp = Port::_PUPDR::read();
	temp &= ~(3U << pos);
	temp |= (static_cast<uint32_t>(cfg.pull) << pos);
	Port::_PUPDR::overwrite(temp);

	// --- AFR (если выбран Alternate Function) ---
	if (cfg.mode == Mode::Alt) {
		if (Pin::num < 8) {
			temp = Port::_AFRL::read();
			temp &= ~(0xFU << (Pin::num * 4));
			temp |= ((static_cast<uint32_t>(cfg.af) & 0xFU)
					<< (static_cast<uint32_t>(Pin::num) * 4));
			Port::_AFRL::overwrite(temp);
		} else {
			temp = Port::_AFRH::read();
			temp &= ~(0xFU << ((Pin::num - 8) * 4));
			temp |= ((static_cast<uint32_t>(cfg.af) & 0xFU)
					<< ((static_cast<uint32_t>(Pin::num) - 8) * 4));
			Port::_AFRH::overwrite(temp);
		}
	}
}
template<typename ... Pins>
struct PinGroup {

	using Action = void(*)();
	static constexpr size_t count = sizeof...(Pins);

	// Массив функций set
	static constexpr Action set_table[count] = { &Pins::set... };

	// Массив функций reset
	static constexpr Action reset_table[count] = { &Pins::reset... };

	template<size_t ... Is>
//    static void read_all_impl(bool* states, std::index_sequence<Is...>) {
//        ((states[Is] = Pins::read()), ...);
//    }
	static void read_all(bool *states) {
		size_t i = 0;
		((states[i++] = Pins::read()), ...);
	}
	inline static void GPIO_ConfigGroupPin(GPIO_Config &cfg) {
		(GPIO_ConfigPin<Pins>(cfg),...);
	}
};

template<typename Port, uint8_t Num>
struct Pin {
	static constexpr uint32_t mask = 1u << Num;
	static constexpr uint8_t num = Num;
	using port_type = Port;

	static bool read() {
		return (Port::_IDR::read() & mask) != 0;
	}
	static void set() {
		Port::_BSRR::overwrite(mask);
	}
	static void reset() {
		Port::_BSRR::overwrite(mask << 16);
	}
};

