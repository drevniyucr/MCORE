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
#include <utility>
#include <type_traits>

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

namespace gpio_detail {

// Nth type in a pack, без зависимости от <tuple>.
template<size_t I, typename First, typename... Rest>
struct type_at : type_at<I - 1, Rest...> {};
template<typename First, typename... Rest>
struct type_at<0, First, Rest...> { using type = First; };
template<size_t I, typename... Ts>
using type_at_t = typename type_at<I, Ts...>::type;

// Ширины полей регистров GPIO.
inline constexpr uint32_t AF_MASK   = 0xFu;  // AFRL/AFRH: 4 бита на пин
inline constexpr uint32_t MODE_MASK = 0x3u;  // MODER/OSPEEDR/PUPDR: 2 бита на пин

// Накопленные маски (clear|set) для одного порта: по одному RMW на регистр.
struct PortMasks {
	uint32_t moder_clr = 0,  moder_set = 0;
	uint32_t otyper_clr = 0, otyper_set = 0;
	uint32_t ospeed_clr = 0, ospeed_set = 0;
	uint32_t pupdr_clr = 0,  pupdr_set = 0;
	uint32_t afrl_clr = 0,   afrl_set = 0;
	uint32_t afrh_clr = 0,   afrh_set = 0;
};

// Вклад одного пина (num — номер в пределах порта) в маски. Всё на compile-time.
constexpr void add_pin(PortMasks &m, uint32_t num, const GPIO_Config &cfg) {
	const uint32_t p2 = num * 2;
	m.moder_clr  |= MODE_MASK << p2;  m.moder_set  |= static_cast<uint32_t>(cfg.mode)  << p2;
	m.otyper_clr |= 1u << num;        m.otyper_set |= static_cast<uint32_t>(cfg.otype) << num;
	m.ospeed_clr |= MODE_MASK << p2;  m.ospeed_set |= static_cast<uint32_t>(cfg.speed) << p2;
	m.pupdr_clr  |= MODE_MASK << p2;  m.pupdr_set  |= static_cast<uint32_t>(cfg.pull)  << p2;
	if (cfg.mode == Mode::Alt) {
		const uint32_t af = static_cast<uint32_t>(cfg.af) & AF_MASK;
		if (num < 8) { m.afrl_clr |= AF_MASK << (num * 4);       m.afrl_set |= af << (num * 4); }
		else         { m.afrh_clr |= AF_MASK << ((num - 8) * 4); m.afrh_set |= af << ((num - 8) * 4); }
	}
}

} // namespace gpio_detail

template<typename ... Pins>
struct PinGroup {

	using Action = void(*)();
	static constexpr size_t count = sizeof...(Pins);

	// Таблицы указателей set/reset (используются приложением, поведение прежнее).
	static constexpr Action set_table[count]   = { &Pins::set... };
	static constexpr Action reset_table[count] = { &Pins::reset... };

	static void read_all(bool *states) {
		size_t i = 0;
		((states[i++] = Pins::read()), ...);
	}

	// Compile-time конфигурация группы: одна RMW-запись на порт на регистр.
	// cfg передаётся как non-type template parameter → все маски сворачиваются в константы.
	template<GPIO_Config Cfg>
	static void configure() {
		apply<Cfg>(std::make_index_sequence<count>{});
	}

private:
	template<GPIO_Config Cfg, size_t... Is>
	static void apply(std::index_sequence<Is...>) {
		// Порт конфигурируется ровно один раз — на первом пине этого порта в пакете.
		(write_if_first<Cfg, Is>(), ...);
	}

	template<GPIO_Config Cfg, size_t I>
	static void write_if_first() {
		using Port = typename gpio_detail::type_at_t<I, Pins...>::port_type;
		if constexpr (is_first_of_port<I, Port>())
			write_port<Cfg, Port>();
	}

	// true, если ни один пин с индексом < I не принадлежит порту Port.
	template<size_t I, typename Port>
	static consteval bool is_first_of_port() {
		return is_first_impl<I, Port>(std::make_index_sequence<count>{});
	}
	template<size_t I, typename Port, size_t... Js>
	static consteval bool is_first_impl(std::index_sequence<Js...>) {
		return (... && !((Js < I) &&
			std::is_same_v<typename gpio_detail::type_at_t<Js, Pins...>::port_type, Port>));
	}

	// Накопить маски по всем пинам порта Port.
	template<GPIO_Config Cfg, typename Port, size_t... Is>
	static consteval gpio_detail::PortMasks collect(std::index_sequence<Is...>) {
		gpio_detail::PortMasks m{};
		(add_if_on_port<Cfg, Port, Is>(m), ...);
		return m;
	}
	template<GPIO_Config Cfg, typename Port, size_t I>
	static consteval void add_if_on_port(gpio_detail::PortMasks &m) {
		using PinI = gpio_detail::type_at_t<I, Pins...>;
		if constexpr (std::is_same_v<typename PinI::port_type, Port>)
			gpio_detail::add_pin(m, PinI::num, Cfg);
	}

	template<GPIO_Config Cfg, typename Port>
	static void write_port() {
		constexpr gpio_detail::PortMasks m =
			collect<Cfg, Port>(std::make_index_sequence<count>{});
		Port::_MODER::overwrite((Port::_MODER::read()   & ~m.moder_clr)  | m.moder_set);
		Port::_OTYPER::overwrite((Port::_OTYPER::read()  & ~m.otyper_clr) | m.otyper_set);
		Port::_OSPEEDR::overwrite((Port::_OSPEEDR::read() & ~m.ospeed_clr) | m.ospeed_set);
		Port::_PUPDR::overwrite((Port::_PUPDR::read()   & ~m.pupdr_clr)  | m.pupdr_set);
		if constexpr (m.afrl_clr != 0)
			Port::_AFRL::overwrite((Port::_AFRL::read() & ~m.afrl_clr) | m.afrl_set);
		if constexpr (m.afrh_clr != 0)
			Port::_AFRH::overwrite((Port::_AFRH::read() & ~m.afrh_clr) | m.afrh_set);
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

