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
#include "core/profile.hpp"
#include "core/scheduler.hpp"
#include "core/ring_buffer.hpp"
#include "core/mcore_config.hpp"
#include "core/mpu.hpp"
#include "core/nvic.hpp"
#include "net/net.hpp"
#include "net/eth_utils.hpp"
#include "net/tcp.hpp"
#include "net/dhcp.hpp"
#include "drivers/common.hpp"
#include "drivers/usart.hpp"
#include "drivers/i2c.hpp"
#include "drivers/can.hpp"
#include "drivers/tim.hpp"
#include "drivers/adc.hpp"
#include "drivers/dma.hpp"
#include "drivers/ws2812.hpp"
#include "drivers/devices/eeprom_47l16.hpp"
