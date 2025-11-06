
// #include "mcore_gpio.hpp"


// namespace GPIO {

// void GPIO_ConfigPin(GPIO_Config cfg, uint8_t pin_number) {

// 	uint32_t pos  = pin_number * 2;
// 	uint32_t temp = 0x00;

// 	// --- MODER ---
// 	temp = cfg.port->MODER;
// 	temp &= ~(3U << pos);
// 	temp |= (static_cast<uint32_t>(cfg.mode) << pos);
// 	cfg.port->MODER = temp;

// 	// --- OTYPER ---
// 	temp = cfg.port->OTYPER;
// 	temp &= ~(1U << pin_number);
// 	temp |= (static_cast<uint32_t>(cfg.otype) << pin_number);
// 	cfg.port->OTYPER = temp;

// 	// --- OSPEEDR ---
// 	temp = cfg.port->OSPEEDR;
// 	temp &= ~(3U << pos);
// 	temp |= (static_cast<uint32_t>(cfg.speed) << pos);
// 	cfg.port->OSPEEDR = temp;

// 	// --- PUPDR ---
// 	temp = cfg.port->PUPDR;
// 	temp &= ~(3U << pos);
// 	temp |= (static_cast<uint32_t>(cfg.pull) << pos);
// 	cfg.port->PUPDR = temp;

// 	// --- AFR (если выбран Alternate Function) ---
// 	if (cfg.mode == Mode::Alt) {
// 		if (pin_number < 8) {
// 			temp = cfg.port->AFR[0];
// 			temp &= ~(0xFU << (pin_number * 4));
// 			temp |= ((static_cast<uint32_t>(cfg.af) & 0xFU)
// 					<< (static_cast<uint32_t>(pin_number) * 4));
// 			cfg.port->AFR[0] = temp;
// 		} else {
// 			temp = cfg.port->AFR[1];
// 			temp &= ~(0xFU << ((pin_number - 8) * 4));
// 			temp |= ((static_cast<uint32_t>(cfg.af) & 0xFU)
// 					<< ((static_cast<uint32_t>(pin_number) - 8) * 4));
// 			cfg.port->AFR[1] = temp;
// 		}
// 	}
// }

// void GPIO_ConfigGroupPin(GPIO_Config cfg, uint32_t pin_mask) {

// 	uint32_t pos = 0;
// 	uint32_t temp = 0x00;

// 	for (int i = 0; i < 16; i++) {

// 		if (pin_mask & (1U << i)) {

// 			pos = i * 2;
// 			//--- MODER ---
// 			temp = cfg.port->MODER;
// 			temp &= ~(3U << pos);
// 			temp |= (static_cast<uint32_t>(cfg.mode) << pos);
// 			cfg.port->MODER = temp;

// 			// --- OTYPER ---
// 			temp = cfg.port->OTYPER;
// 			temp &= ~(1U << i);
// 			temp |= (static_cast<uint32_t>(cfg.otype) << i);
// 			cfg.port->OTYPER = temp;

// 			// --- OSPEEDR ---
// 			temp = cfg.port->OSPEEDR;
// 			temp &= ~(3U << pos);
// 			temp |= (static_cast<uint32_t>(cfg.speed) << pos);
// 			cfg.port->OSPEEDR = temp;

// 			// --- PUPDR ---
// 			temp = cfg.port->PUPDR;
// 			temp &= ~(3U << pos);
// 			temp |= (static_cast<uint32_t>(cfg.pull) << pos);
// 			cfg.port->PUPDR = temp;

// 			// --- AFR (если выбран Alternate Function) ---
// 			if (cfg.mode == Mode::Alt) {
// 				if (i < 8) {
// 					temp = cfg.port->AFR[0];
// 					temp &= ~(0xFU << (i * 4));
// 					temp |= ((static_cast<uint32_t>(cfg.af) & 0xFU) << (i * 4));
// 					cfg.port->AFR[0] = temp;
// 				} else {
// 					temp = cfg.port->AFR[1];
// 					temp &= ~(0xFU << ((i - 8) * 4));
// 					temp |= ((static_cast<uint32_t>(cfg.af) & 0xFU)
// 							<< ((i - 8) * 4));
// 					cfg.port->AFR[1] = temp;
// 				}
// 			}
// 		}
// 	}

// }
// }


