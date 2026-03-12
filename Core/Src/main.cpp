#include "mcore.hpp"
#include "mcore_def.hpp"
#include "main.hpp"
#include <cstdio>
bool states[inputs::group::count];

uint32_t adc3_ch[2];
uint8_t adc_idx = 0;

DmaHandle<DmaStream::DMA2_Stream_7> uart_tx;


namespace {
	// Local constants
	constexpr uint32_t TCP_SEND_INTERVAL_MS = 5000;
	constexpr uint32_t TCP_TIMER_INTERVAL_MS = 100;
	constexpr uint32_t SNAKE_TIMER_INTERVAL_MS = 400;
	constexpr uint8_t TEST_CLIENT_INDEX = 9;  // Specific client for testing

	uint8_t test_data[] = "LMAO LMAO LMAO LMAO LMAO LMAO LMAO";
	uint8_t txData1[] = "Hello, USART! Hello, USART! Hello, USART! "
			"Hello, USART! Hello, USART! Hello, USART! Hello, USART!"
			" Hello, USART! Hello, USART! Hello, USART!";
	uint8_t txData2[] = "Hello, RS485!";


	CanFrame tx_can{
		.id = 0x123,
		.dlc = 2,
		.data = {0xDE, 0xAD}
	};

	uint8_t tx[4] = { 0xDE, 0xAD, 0xBE, 0xDD };
	uint8_t rx[4] = { 0 };
	bool i2cstat = true;
	// Forward declarations
	void GPIO_Init();
	void RCC_Init();
	void uart_init();
	void i2c_init();
	void can_init();
	void tim_init();
	void adc_init();
	void dma_init();
	void inputs_update();
}

int main() {

	SystemInit();
	const uint32_t start = DWT::_CYCCNT::read();

	RCC_Init();
	MPU_Config();
	//SCB_EnableICache();
	//SCB_EnableDCache();
	GPIO_Init();
	uart_init();
	i2c_init();
	can_init();
	tim_init();
	NVIC_API::EnableIRQGlobal();
	adc_init();
	dma_init();
	if (ETH_Init() != ETH_Status::OK) {
		Error_Handler();
	}

	NET_TCP_Init();

	const uint32_t elapsed = DWT::_CYCCNT::read() - start;
	print("Spend %lu cycles for init\n", elapsed);

	i2cstat = Eeprom47L16<I2C1>::write(0x0010, tx, sizeof(tx));

	/* Read back */
	i2cstat &= Eeprom47L16<I2C1>::read(0x0010, rx, sizeof(rx));

	/* Verify */
	if (i2cstat &&
		rx[0] == 0xDE &&
		rx[1] == 0xAD &&
		rx[2] == 0xBE &&
		rx[3] == 0xEF) {
		// EEPROM OK
	}
	else {
		// error
	//	Error_Handler();
	}

	// Main loop
	uint32_t tcp_send_tick = get_tick();
	uint32_t tcp_timer_tick = get_tick();
	uint32_t snake_timer_tick = get_tick();
	size_t i_snake = 0;
	bool snake_flag = true;


	while (true) {
		ETH_RxWorker();
		inputs_update();
		// Send test data periodically
		if ((get_tick() - tcp_send_tick) >= TCP_SEND_INTERVAL_MS) {
			tcp_send_tick = get_tick();
			UartHandle<USART2>::sendBuffer(txData2, sizeof(txData2));
			if (tcp_clients[TEST_CLIENT_INDEX].state
				== tcp_state_t::TCP_ESTABLISHED) {
				NET_TCP_SendUser(&tcp_clients[TEST_CLIENT_INDEX], test_data,
					sizeof(test_data));
			}

		}

		// Run TCP timers
		if ((get_tick() - tcp_timer_tick) >= TCP_TIMER_INTERVAL_MS) {
			i2cstat &= Eeprom47L16<I2C1>::read(0x0010, rx, sizeof(rx));
			CanHandle<CAN1>::CAN_Send(tx_can);
			NET_TCP_Timers();
			tcp_timer_tick = get_tick();
		}
		if ((get_tick() - snake_timer_tick) >= SNAKE_TIMER_INTERVAL_MS) {
			snake_timer_tick = get_tick();

			//UartHandle<USART6>::sendBuffer(txData1, sizeof(txData1));

			if (uart_tx.state == DmaState::READY) {
				UartHandle<USART6>::UartTxDMA<uart_tx>(txData1, sizeof(txData1));
			}

			if (snake_flag) // движение вперёд
			{
				if (i_snake < outputs::group::count) {
					outputs::group::set_table[i_snake++]();
				}
				else {
					snake_flag = false;
				}
			}
			else // движение назад
			{
				if (i_snake > 0) {
					outputs::group::reset_table[--i_snake]();
				}
				else if (i_snake == 0) {
					outputs::group::reset_table[i_snake]();
					snake_flag = true;
				}
			}
		}
	}
}

namespace {
	void dma_init() {
		constexpr const	DmaHwConfig cfg_s0 = {
			.Channel = DmaChannel::CHNL_5,
			.Direction = DmaDir::MEM_TO_PERIPH,
			.PeriphIncEnable = false,
			.MemIncEnable = true,
			.PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE,
			.Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::VERY_HIGH,
			.FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL
		};

		DMA_Config<uart_tx, cfg_s0>();
		NVIC_API::SetPriority<IRQn_Type::DMA2_Stream7_IRQn, 4>();
		NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream7_IRQn>();

	}

	void adc_init()
	{
		constexpr const AdcHwConfig cfg = {
			.ClockPrescaler = AdcClkPresc::DIV4,
			.EOCSelection = AdcEOCSel::SINGLE_CONV,
			.NbrOfConversion = 2,
			.ScanConvModeEnable = true,
			.ContinuousConvModeEnable = false

		};

		constexpr const AdcChannel ch1 = {
			.SamplingTime = AdcSampleTime::_480CYCLES,
			.Rank = AdcChnlRank::ADC_RANK_1,
		};
		constexpr const AdcChannel ch2 = {
		.SamplingTime = AdcSampleTime::_480CYCLES,
		.Rank = AdcChnlRank::ADC_RANK_2,
		};
		//constexpr const AdcWDGConfig cfg2;

		ADC_Config<ADC3, cfg>();
		ADC_ChannelConfig<ADC3, ch1, AdcChnl::ADC_CHANNEL_6>();
		ADC_ChannelConfig<ADC3, ch2, AdcChnl::ADC_CHANNEL_7>();
		//ADC_AWDConfig<ADC1, cfg2>();
		NVIC_API::SetPriority<IRQn_Type::ADC_IRQn, 0>();
		NVIC_API::EnableIRQ<IRQn_Type::ADC_IRQn>();
		ADC_Start<ADC3>();
		//0,01460837035305120411503390226794 - modifuer
		//ratio in/out 0,05516431924882629107981220657277
	}
	void tim_init()
	{
		constexpr const TimBaseConfig cfg = {
			.Prescaler = 216 - 1,   // PSC
			.Period = 5 - 1,      // ARR
			.CounterMode = TimCounterMode::Up,
			.ClockDiv = TimClkDiv::DIV1,
			.RepetitionCounter = 1,
			.EnablePreload = true
		};

		constexpr const TimBaseConfig cfg2 = {
			.Prescaler = 108 - 1,   // PSC
			.Period = 5 - 1,     // ARR
			.CounterMode = TimCounterMode::Up,
			.ClockDiv = TimClkDiv::DIV1,
			.RepetitionCounter = 1,
			.EnablePreload = true
		};

		constexpr const TimOCConfig cfgOC = {
			.OCMode = TimOCMode::PWM1,
			.OCPolarity = TimOCPol::NONE,
			.OCNPolarity = TimOCPol::NONE,
			.OCIdleState = TimIdleState::NONE,
			.OCNIdleState = TimIdleState::NONE,
			.Pulse = 2
		};

		TIM_BaseConfig<TIM1, cfg>();
		TIM_BaseConfig<TIM4, cfg2>();

		TIM_PWM_ConfigChannel<TIM1, cfgOC, TimCh::CH1>();
		TIM_PWM_ConfigChannel<TIM1, cfgOC, TimCh::CH2>();
		TIM_PWM_ConfigChannel<TIM4, cfgOC, TimCh::CH1>();
		TIM_PWM_ConfigChannel<TIM4, cfgOC, TimCh::CH4>();


		TIM_PWM_ChannelEnable<TIM1, TimCh::CH1>();
		TIM_PWM_ChannelEnable<TIM1, TimCh::CH2>();
		TIM_PWM_ChannelEnable<TIM4, TimCh::CH1>();
		TIM_PWM_ChannelEnable<TIM4, TimCh::CH4>();
	}

	void can_init()
	{
		constexpr const CanHwConfig cfg = {
			.Prescaler = 6,
			.SyncJumpWidth = 1,
			.TimeSeg1 = 15,
			.TimeSeg2 = 2,
			.Mode = CanMode::Loopback,
			.TimeTriggerMode = false,
			.AutoBusOff = true,
			.AutoWakeUp = true,
			.AutoRetransmission = true,
			.FifoLock = false,
			.FifoPriority = false
		};

		CANStatus status = CAN_Init<CAN1, cfg>();
		if (status != CANStatus::Success) {
			Error_Handler(); // Handle initialization error
		}
	}

	void i2c_init()
	{
		constexpr const I2CHwConfig cfg = {
			.Timing = 0x20404768, // Timing value for 400kHz I2C speed with 54MHz PCLK1
			.OwnAddres1 = 0x52,   // Example own address
			.OwnAddres2 = 0x4,   // Example own address
			.EnAnalogFilter = true
		};

		I2CStatus status = I2C_Init<I2C1, cfg>();
		if (status != I2CStatus::Success) {
			Error_Handler(); // Handle initialization error
		}
	}

	void uart_init()
	{
		constexpr const UartHwConfig cfg = {
					.Baud = 115200,
					.AssertTime = 4,
					.DeAssertTime = 4,
					.EnableDE = true,
					.DEPolarityHigh = true
		};

		USARTStatus status = USART_Init<USART2, cfg>();
		if (status != USARTStatus::Success) Error_Handler();

		constexpr const UartHwConfig cfg2 = {
						.Baud = 115200,
						.ClockSource = UartClockSrc::PCLK,
						.UseDMARx = true,
						.UseDMATx = true
		};
		status = USART_Init<USART6, cfg2>();
		if (status != USARTStatus::Success) Error_Handler();
	}


	void inputs_update()
	{
		inputs::group::read_all(states);
	}

	void RCC_Init() {
		constexpr const ClockConfig cfg = { .PLLM = 4,              // MCO(8Mhz)/8=2
				.PLLN = 216,            // 2*216 = 432
				.PLLP = PLL_P::Div2,    // 432/2=216MHz SYSCKL MAX
				.AHBDiv = AHBPrescaler::Div1,    // 216MHz HCKL MAX
				.APB1Div = APBPrescaler::Div4,    //216/4=54 MHz APB1 MAX
				.APB2Div = APBPrescaler::Div2,    //216/2=108 MHz APB2 MAX
				.FLASHLatency = FLASH_Latency::WS7, .useHSE = true, .useHSEBypass =
						true, .useSysTick = true };

		enablePowerInterface();
		SetVoltageScale<VoltageScale::Scale1>();

		if (OSCInit<cfg>() != RCCStatus::OK || OverDriveInit() != RCCStatus::OK
			|| RCC_ClockInit<cfg>() != RCCStatus::OK) {
			Error_Handler();
		}
	}

	void GPIO_Init() {
		RCC_GPIO_ALLEN();
		//ETH GPIO Configuration
		GPIO_Config temp_conf;
		temp_conf.mode = Mode::Alt;
		temp_conf.otype = OType::PP;
		temp_conf.speed = Speed::VeryHigh;
		temp_conf.pull = Pull::None;
		temp_conf.af = AF::AF11;
		eth::group::GPIO_ConfigGroupPin(temp_conf);

		// INPUTS GPIO Configuration
		temp_conf.mode = Mode::In;
		temp_conf.otype = OType::PP;
		temp_conf.speed = Speed::Medium;
		temp_conf.pull = Pull::None;
		temp_conf.af = AF::AF0;
		inputs::group::GPIO_ConfigGroupPin(temp_conf);

		//OUTPUTS GPIO Configuration
		temp_conf.mode = Mode::Out;
		temp_conf.otype = OType::PP;
		temp_conf.speed = Speed::VeryHigh;
		temp_conf.pull = Pull::None;
		temp_conf.af = AF::AF0;
		outputs::group::GPIO_ConfigGroupPin(temp_conf);

		//USART2/RS-485 GPIO Configuration
		temp_conf.mode = Mode::Alt;
		temp_conf.otype = OType::PP;
		temp_conf.speed = Speed::VeryHigh;
		temp_conf.pull = Pull::PullUp;
		temp_conf.af = AF::AF7; // usart2
		rs485::group::GPIO_ConfigGroupPin(temp_conf);

		//CAN1 GPIO Configuration
		temp_conf.mode = Mode::Alt;
		temp_conf.otype = OType::PP;
		temp_conf.speed = Speed::VeryHigh;
		temp_conf.pull = Pull::None;
		temp_conf.af = AF::AF9; // CAN1
		can1::group::GPIO_ConfigGroupPin(temp_conf);

		//USART6/LIDAR GPIO Configuration
		temp_conf.mode = Mode::Alt;
		temp_conf.otype = OType::PP;
		temp_conf.speed = Speed::VeryHigh;
		temp_conf.pull = Pull::None;
		temp_conf.af = AF::AF8; // usart6
		usart6::group::GPIO_ConfigGroupPin(temp_conf);

		//TIM/LED DATA/PWM GPIO Configuration
		temp_conf.mode = Mode::Alt;
		temp_conf.otype = OType::PP;
		temp_conf.speed = Speed::VeryHigh;
		temp_conf.pull = Pull::None;
		temp_conf.af = AF::AF1; // tim1
		tim1::group::GPIO_ConfigGroupPin(temp_conf);

		temp_conf.af = AF::AF2; //tim4
		tim4::group::GPIO_ConfigGroupPin(temp_conf);

		//ADC3 GPIO Configuration
		temp_conf.mode = Mode::Ang;
		temp_conf.otype = OType::PP;
		temp_conf.speed = Speed::VeryHigh;
		temp_conf.pull = Pull::None;
		temp_conf.af = AF::AF0; // none-analog
		adc3::group::GPIO_ConfigGroupPin(temp_conf);

		//I2C1 GPIO Configuration
		temp_conf.mode = Mode::Alt;
		temp_conf.otype = OType::OD;
		temp_conf.speed = Speed::VeryHigh;
		temp_conf.pull = Pull::None;
		temp_conf.af = AF::AF4;	 // i2c1
		i2c1::group::GPIO_ConfigGroupPin(temp_conf);
	}
}

extern "C" void ADC_IRQHandler() {
	if (ADC3::_SR::EOC::is_set()) {
		adc3_ch[adc_idx++] = ADC3::_DR::DATA::read();
	}
	if (adc_idx > 1)adc_idx = 0;
		__NOP();
}

extern "C" void  DMA2_Stream7_IRQHandler() {
	DMA_IRQHandler<uart_tx>();
}

void Error_Handler() {
	// Error occurred - enter infinite loop
	// TODO: Add error logging/indication (LED, UART, etc.)
	while (true) {
		__NOP();  // Prevent compiler optimization
	}
}
