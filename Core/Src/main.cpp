#include "mcore.hpp"
#include "mcore_def.hpp"
#include "main.hpp"
#include <cstdio>

namespace {
	// Local constants
	constexpr uint32_t TCP_SEND_INTERVAL_MS = 5000;
	constexpr uint32_t TCP_TIMER_INTERVAL_MS = 100;
	constexpr uint8_t TEST_CLIENT_INDEX = 9;  // Specific client for testing
	
	constexpr uint8_t test_data[] = "LMAO LMAO LMAO LMAO LMAO LMAO LMAO";
	
	// Forward declarations
	void GPIO_Init();
	void RCC_Init();
}

int main()
{
	SystemInit();
	const uint32_t start = DWT::_CYCCNT::read();
	
	RCC_Init();
	MPU_Config();
	SCB_EnableICache();
	SCB_EnableDCache();
	GPIO_Init();
	
	if (ETH_Init() != ETH_Status::OK) {
		Error_Handler();
	}
	
	NET_TCP_Init();
	NVIC_API::EnableIRQGlobal();
	
	const uint32_t elapsed = DWT::_CYCCNT::read() - start;
	print("Spend %lu cycles for init\n", elapsed);
	
	// Main loop
	uint32_t tcp_send_tick = get_tick();
	uint32_t tcp_timer_tick = get_tick();
	
	while (true) {
		ETH_RxWorker();
		
		// Send test data periodically
		if ((get_tick() - tcp_send_tick) >= TCP_SEND_INTERVAL_MS) {
			tcp_send_tick = get_tick();
			if (tcp_clients[TEST_CLIENT_INDEX].state == tcp_state_t::TCP_ESTABLISHED) {
				NET_TCP_SendUser(&tcp_clients[TEST_CLIENT_INDEX], test_data, sizeof(test_data));
			}
		}
		
		// Run TCP timers
		if ((get_tick() - tcp_timer_tick) >= TCP_TIMER_INTERVAL_MS) {
			NET_TCP_Timers();
			tcp_timer_tick = get_tick();
		}
	}
}

namespace {
	void RCC_Init() {
		constexpr const ClockConfig cfg = {
			.PLLM = 4,              // MCO(8Mhz)/8=2
			.PLLN = 216,            // 2*216 = 432
			.PLLP = PLL_P::Div2,    // 432/2=216MHz SYSCKL MAX
			.AHBDiv = AHBPrescaler::Div1,    // 216MHz HCKL MAX
			.APB1Div = APBPrescaler::Div4,    //216/4=54 MHz APB1 MAX
			.APB2Div = APBPrescaler::Div2,    //216/2=108 MHz APB2 MAX
			.FLASHLatency = FLASH_Latency::WS7,
			.useHSE = true,
			.useHSEBypass = true,
			.useSysTick = true
		};

		enablePowerInterface();
		SetVoltageScale<VoltageScale::Scale1>();

		if (OSCInit<cfg>() != RCCStatus::OK ||
			OverDriveInit() != RCCStatus::OK ||
			RCC_ClockInit<cfg>() != RCCStatus::OK)
		{
			Error_Handler();
		}
	}

	void GPIO_Init() {
		RCC_GPIO_ALLEN();

		GPIO_Config eth_config = {
			.mode = Mode::Alt,
			.otype = OType::PP,
			.speed = Speed::VeryHigh,
			.pull = Pull::None,
			.af = AF::AF11,
		};

		// ETH GPIO Configuration
		// PA1, PA2, PA7
		constexpr uint32_t eth_pa_mask = (1U << 1) | (1U << 2) | (1U << 7);
		GPIO_ConfigGroupPin<GPIOA>(eth_config, eth_pa_mask);

		// PB13
		GPIO_ConfigPin<GPIOB>(eth_config, 13);

		// PC1, PC4, PC5
		constexpr uint32_t eth_pc_mask = (1U << 1) | (1U << 4) | (1U << 5);
		GPIO_ConfigGroupPin<GPIOC>(eth_config, eth_pc_mask);

		// PG11, PG13
		constexpr uint32_t eth_pg_mask = (1U << 11) | (1U << 13);
		GPIO_ConfigGroupPin<GPIOG>(eth_config, eth_pg_mask);

		// TIM GPIO Configuration
		GPIO_Config tim_config = {
			.mode = Mode::Alt,
			.otype = OType::PP,
			.speed = Speed::VeryHigh,
			.pull = Pull::PullUp,
			.af = AF::AF1,  // TIM2
		};

		// TIM2 pins
		GPIO_ConfigPin<GPIOB>(tim_config, 10);  // PB10 -> TIM2_CH3
		GPIO_ConfigPin<GPIOA>(tim_config, 3);   // PA3 -> TIM2_CH4

		// TIM3 pins
		tim_config.af = AF::AF2;  // TIM3
		GPIO_ConfigPin<GPIOA>(tim_config, 6);   // PA6 -> TIM3_CH1
		GPIO_ConfigPin<GPIOC>(tim_config, 7);   // PC7 -> TIM3_CH2

		// PB0, PB1 -> TIM3_CH3, TIM3_CH4
		constexpr uint32_t tim_pb_mask = (1U << 0) | (1U << 1);
		GPIO_ConfigGroupPin<GPIOB>(tim_config, tim_pb_mask);
	}
}

void Error_Handler() {
	// Error occurred - enter infinite loop
	// TODO: Add error logging/indication (LED, UART, etc.)
	while (true) {
		__NOP();  // Prevent compiler optimization
	}
}
