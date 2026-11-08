/*
 * mcore.hpp
 *
 *  Created on: Sep 24, 2025
 *      Author: AkimovMA
 */

#pragma once
#include "drivers/eth.hpp"
#include "drivers/gpio.hpp"
#include "core/rcc.hpp"
#include "core/system.hpp"
#include "config.hpp"
#include "core/mpu.hpp"
#include "core/nvic.hpp"
#include "net/net.hpp"
#include "net/eth_utils.hpp"
#include "net/tcp.hpp"
#include "net/dhcp.hpp"
#include "drivers/usart.hpp"
#include "drivers/i2c.hpp"
#if !defined(MCORE_STM32H755)
#include "drivers/can.hpp"   // bxCAN: STM32F7 only (H7 has FDCAN)
#endif
#include "drivers/tim.hpp"
#include "drivers/adc.hpp"
#include "drivers/dma.hpp"
#include "drivers/ws2812.hpp"
