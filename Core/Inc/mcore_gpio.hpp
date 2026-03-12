/*
 * mcore_gpio.hpp
 *
 *  Created on: Sep 15, 2025
 *      Author: AkimovMA
 */
#pragma once

#include "mcore_def.hpp"
#include "mcore_system.hpp"
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

struct not_allowed {
	using SWDIO = Pin<GPIOA,13>;
	using SWCLK = Pin<GPIOA,14>;
	using SWO = Pin<GPIOB,3>;
	using OSC32_IN = Pin<GPIOC,14>;
	using OSC32_OUT = Pin<GPIOC,15>;
	using OSC_IN = Pin<GPIOH,0>;
	using OSC_OUT = Pin<GPIOH,1>;
	using LD2_BLUE = Pin<GPIOB,7>;
	using BTN1 = Pin<GPIOC,13>;
};
struct inputs {
	using IN1 = Pin<GPIOC,10>;
	using IN2 = Pin<GPIOC,12>;
	using IN3 = Pin<GPIOC,11>;
	using IN4 = Pin<GPIOD,2>;
	using IN5 = Pin<GPIOF,6>;
	using IN6 = Pin<GPIOF,7>;
	using IN7 = Pin<GPIOF,2>; //pa13BANNED USED BY SWDIO
	using IN8 = Pin<GPIOA,4>;
	using IN9 = Pin<GPIOB,0>;
	using IN10 = Pin<GPIOC,2>;
	using IN11 = Pin<GPIOC,3>;
	using IN12 = Pin<GPIOC,0>;
	using IN13 = Pin<GPIOD,3>;
	using IN14 = Pin<GPIOD,4>;
	using IN15 = Pin<GPIOG,2>;
	using IN16 = Pin<GPIOG,3>;
	using IN17 = Pin<GPIOE,2>;
	using IN18 = Pin<GPIOE,3>;
	using IN19 = Pin<GPIOE,4>;
	using IN20 = Pin<GPIOE,5>;
	using IN21 = Pin<GPIOB,11>;
	using IN22 = Pin<GPIOB,6>;
	using IN23 = Pin<GPIOB,12>;
	using IN24 = Pin<GPIOA,6>;

	using group = PinGroup<
	IN1, IN2, IN3, IN4,
	IN5, IN6, IN7, IN8,
	IN9, IN10, IN11, IN12,
	IN13, IN14, IN15, IN16,
	IN17, IN18, IN19, IN20,
	IN21, IN22, IN23, IN24
	>;
};
struct outputs {
	using OUT1_H = Pin<GPIOD,11>;
	using OUT2_H = Pin<GPIOF,10>;
	using OUT3_H = Pin<GPIOD,13>;
	using OUT4_H = Pin<GPIOE,8>;
	using OUT5_H = Pin<GPIOA,3>;
	using OUT6_H = Pin<GPIOF,4>;
	using OUT7_H = Pin<GPIOF,5>;
	using OUT8_H = Pin<GPIOB,5>;
	using OUT9_H = Pin<GPIOB,4>;
	using OUT10_H = Pin<GPIOB,10>;
	using OUT11_H = Pin<GPIOB,15>;
	using OUT12_H = Pin<GPIOB,1>;

	using OUT13_L = Pin<GPIOE,10>;
	using OUT14_L = Pin<GPIOE,12>;
	using OUT15_L = Pin<GPIOF,14>;
	using OUT16_L = Pin<GPIOE,14>;

	using RLY1 = Pin<GPIOC,6>;
	using RLY2 = Pin<GPIOA,5>;
	using RLY3 = Pin<GPIOC,8>;

	using MEM_WP = Pin<GPIOC,9>;

	using group = PinGroup<
	OUT1_H, OUT2_H, OUT3_H, OUT4_H,
	OUT5_H, OUT6_H, OUT7_H, OUT8_H,
	OUT9_H, OUT10_H, OUT11_H, OUT12_H,
	OUT13_L, OUT14_L, OUT15_L, OUT16_L,
	RLY1, RLY2, RLY3
	>;
};

struct eth {
	using ETH_CRS_DV = Pin<GPIOA,7>;
	using ETH_MDC = Pin<GPIOC,1>;
	using ETH_MDIO = Pin<GPIOA,2>;
	using ETH_REF_CLK = Pin<GPIOA,1>;
	using ETH_RXD0 = Pin<GPIOC,4>;
	using ETH_RXD1 = Pin<GPIOC,5>;
	using ETH_TXD0 = Pin<GPIOG,13>;
	using ETH_TXD1 = Pin<GPIOB,13>;
	using ETH_TX_EN = Pin<GPIOG,11>;

	using group = PinGroup<
	ETH_CRS_DV, ETH_MDC,
	ETH_MDIO, ETH_REF_CLK,
	ETH_RXD0, ETH_RXD1,
	ETH_TXD0, ETH_TXD1,
	ETH_TX_EN
	>;
};

struct i2c1 {
	using I2C1_SCL = Pin<GPIOB,8>;
	using I2C1_SDA = Pin<GPIOB,9>;
	using group = PinGroup<I2C1_SCL, I2C1_SDA>;
};

struct rs485 {
	using USART2_RX = Pin<GPIOD,6>;
	using USART2_TX = Pin<GPIOD,5>;
	using RS485_RE_DE = Pin<GPIOD,7>;
	using group = PinGroup<USART2_RX, USART2_TX>;
};

struct can1 {
	using CAN1_RX = Pin<GPIOD,0>;
	using CAN1_TX = Pin<GPIOD,1>;
	using group = PinGroup<CAN1_RX, CAN1_TX>;
};

struct usart6 {
	using USART6_TX = Pin<GPIOG,14>;
	using USART6_RX = Pin<GPIOC,7>;
	using group = PinGroup<USART6_TX, USART6_RX>;
};

struct tim1 {
	using TIM1_CH1 = Pin<GPIOE,9>;
	using TIM1_CH2 = Pin<GPIOE,11>;
	using group = PinGroup<TIM1_CH1, TIM1_CH2>;
};

struct tim4 {
	using TIM4_CH1 = Pin<GPIOD,12>;
	using TIM4_CH4 = Pin<GPIOD,15>;
	using group = PinGroup<TIM4_CH1, TIM4_CH4>;
};

struct adc3 {
	using ADC3_IN6 = Pin<GPIOF,8>;
	using ADC3_IN7 = Pin<GPIOF,9>;
	using group = PinGroup<ADC3_IN6, ADC3_IN7>;
};

