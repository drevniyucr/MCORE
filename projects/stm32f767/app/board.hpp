/*
 * board.hpp — board pin maps (moved from mcore_gpio.hpp)
 */
#pragma once

#include "drivers/gpio.hpp"

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
	IN13,IN15, IN16,
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
	//using RS485_RE_DE = Pin<GPIOD,7>;

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
	using USART2_RE_DE = Pin<GPIOD,4>;
	using group = PinGroup<USART2_RX, USART2_TX,USART2_RE_DE>;
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

