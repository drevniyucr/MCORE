/*
 * mcore_gpio.hpp
 *
 *  Created on: Sep 15, 2025
 *      Author: AkimovMA
 */
#pragma once

#include"mcore_def.hpp"

namespace GPIO {

// Режим работы пина (MODER)
enum class Mode : uint8_t {
    In      = 0b00,  // Вход
    Out     = 0b01,  // Выход
    Alt  = 0b10,  // Альтернативная функция (AF)
    Ang     = 0b11   // Аналоговый режим
};

// Тип выхода (OTYPER)
enum class OType : uint8_t {
    PP  = 0,  // Тотемный выход (обычный)
    OD = 1   // Открытый сток
};

// Скорость выхода (OSPEEDR)
enum class Speed : uint8_t {
    Low       = 0b00,  // Низкая
    Medium    = 0b01,  // Средняя
    High      = 0b10,  // Высокая
    VeryHigh  = 0b11   // Очень высокая
};

// Подтяжка (PUPDR)
enum class Pull : uint8_t {
    None     = 0b00,  // Без подтяжки
    PullUp   = 0b01,  // Подтяжка к VCC
    PullDown = 0b10  // Подтяжка к GND
};

// Альтернативные функции (AFR)
enum class AF : uint8_t {
    AF0  = 0,  // Системные функции
    AF1  = 1,
    AF2  = 2,
    AF3  = 3,
    AF4  = 4,
    AF5  = 5,
    AF6  = 6,
    AF7  = 7,  // USART1/2
    AF8  = 8,  // USART6
    AF9  = 9,  // CAN, TIM
    AF10 = 10, // OTG_FS
    AF11 = 11, // Ethernet
    AF12 = 12, // FMC/SDIO
    AF13 = 13, // DCMI
    AF14 = 14, // LTDC
    AF15 = 15  // Eventout
};

struct GPIO_Config {
    GPIO_TypeDef* port;   // GPIOA, GPIOB...
    Mode  mode;        // Режим работы
    OType otype;      // Тип выхода
    Speed speed;      // Скорость
    Pull  pull;        // Подтяжка
    AF    af;            // Альтернативная функция
};
void GPIO_ConfigPin(GPIO_Config cfg, uint8_t pin_number);
void GPIO_ConfigGroupPin(GPIO_Config cfg, uint32_t pin_mask);


}

