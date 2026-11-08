#include "mcore.hpp"
#include "core/def.hpp"
#include "main.hpp"
#include "board.hpp"
#include <cstdio>

// ---------------------------------------------------------------------------
// Chip-specific board constants (everything else in this file is shared)
// ---------------------------------------------------------------------------
namespace chip {
// CPU/AHB 216 MHz, APB1 54 MHz, APB2 108 MHz, TIM1 216 MHz
constexpr uint16_t WS2812_PERIOD = 270;      // 216 MHz / 800 kHz
constexpr uint32_t I2C1_TIMING = 0x20404768; // 54 MHz PCLK1
constexpr AF USART_6_AF = AF::AF8;
constexpr UartClockSrc USART_6_CLK = UartClockSrc::SYSCLK;
constexpr AdcChnl ADC_PF8 = AdcChnl::ADC_CHANNEL_6;   // PF8 = ADC3_IN6
constexpr AdcChnl ADC_PF9 = AdcChnl::ADC_CHANNEL_7;   // PF9 = ADC3_IN7
constexpr AdcSampleTime ADC_SMP = AdcSampleTime::_480CYCLES;
using ADC3_IRQ = IRQn_Type::ADC_IRQn;
#define ADC3_IRQ_HANDLER ADC_IRQHandler
} // namespace chip

bool states[inputs::group::count];

uint32_t adc3_ch[2];
uint8_t adc_idx = 0;
uint8_t hue = 0;
uint8_t rx_usart6_buf[100];
uint8_t rx_usart2_buf[100];

DmaHandle<DmaStream::DMA2_Stream_1> usart6_rx;
DmaHandle<DmaStream::DMA2_Stream_7> usart6_tx;

DmaHandle<DmaStream::DMA1_Stream_5> usart2_rx;
DmaHandle<DmaStream::DMA1_Stream_6> usart2_tx;

DmaHandle<DmaStream::DMA2_Stream_3> tim1_ch1;
DmaHandle<DmaStream::DMA2_Stream_2> tim1_ch2;

DmaHandle<DmaStream::DMA1_Stream_0> i2c1_rx;
DmaHandle<DmaStream::DMA1_Stream_7> i2c1_tx;

using LedStrip1 = WS2812<TIM1, TimCh::CH1, tim1_ch1, 13, chip::WS2812_PERIOD>;
using LedStrip2 = WS2812<TIM1, TimCh::CH2, tim1_ch2, 13, chip::WS2812_PERIOD>;

bool i2cstat = true;
uint8_t tx[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
uint8_t rx[4] = { 0 };
namespace {
// Local constants
constexpr uint32_t TCP_SEND_INTERVAL_MS = 5000;
constexpr uint32_t TCP_TIMER_INTERVAL_MS = 100;
constexpr uint32_t SNAKE_TIMER_INTERVAL_MS = 400;
constexpr uint8_t TEST_CLIENT_INDEX = 9;  // Specific client for testing
constexpr uint16_t TCP_LISTEN_PORT = 5001;  // server port for incoming connections

uint8_t test_data[] = "LMAO LMAO LMAO LMAO LMAO LMAO LMAO";

// CanFrame tx_can = {
// 	.id = 0x123,
// 	.dlc = 2,
// 	.data = {0xDE, 0xAD}
// };

// Forward declarations
void GPIO_Init();
void RCC_Init();
void uart_init();
void i2c_init();
// void can_init();
void tim_init();
void adc_init();
void dma_init();
void inputs_update();
void led_step();
void snake_step();
}

int main() {
	SystemInit();
	const uint32_t start = DWT::_CYCCNT::read();

	RCC_Init();
	MPU_Config();
	SCB_EnableICache();
	//SCB_EnableDCache();
	GPIO_Init();
	uart_init();
	i2c_init();
	//	can_init();
	tim_init();
	NVIC_API::EnableIRQGlobal();
	adc_init();
	dma_init();
	if (ETH_Init() != ETH_Status::OK) {
		Error_Handler();
	}

	NET_TCP_Init();
	NET_TCP_Listen(TCP_LISTEN_PORT);
	NET_DHCP_Start();  // falls back to the static IP if no server answers
	UartHandle<USART2>::UartRxDMA<usart2_rx>(rx_usart2_buf,sizeof(rx_usart2_buf));
	UartHandle<USART6>::UartRxDMA<usart6_rx>(rx_usart6_buf,sizeof(rx_usart6_buf));

	const uint32_t elapsed = DWT::_CYCCNT::read() - start;
	print("Spend %lu cycles for init\n", elapsed);

	i2cstat = Eeprom47L16<I2C1>::write(0x0010, tx, sizeof(tx));

	/* Read back */
	i2cstat &= Eeprom47L16<I2C1>::read(0x0010, rx, sizeof(rx));

	/* Verify */
	if (i2cstat && rx[0] == 0xDE && rx[1] == 0xAD && rx[2] == 0xBE
			&& rx[3] == 0xEF) {
		// EEPROM OK
	} else {
		// error
		//	Error_Handler();
	}

	// Main loop
	uint32_t tcp_send_tick = get_tick();
	uint32_t tcp_timer_tick = get_tick();
	uint32_t snake_timer_tick = get_tick();

	while (true) {
		ETH_RxWorker();
		inputs_update();
		// Send test data to the TCP test client periodically
		if ((get_tick() - tcp_send_tick) >= 500) {
			tcp_send_tick = get_tick();
			if (tcp_clients[TEST_CLIENT_INDEX].state == tcp_state_t::TCP_ESTABLISHED) {
				NET_TCP_SendUser(&tcp_clients[TEST_CLIENT_INDEX], test_data,sizeof(test_data));
			}
		}

		// Run TCP/DHCP timers (+ LED strip animation)
		if ((get_tick() - tcp_timer_tick) >= TCP_TIMER_INTERVAL_MS) {
			NET_TCP_Timers();
			NET_DHCP_Poll();
			tcp_timer_tick = get_tick();
			led_step();
		}
		if ((get_tick() - snake_timer_tick) >= SNAKE_TIMER_INTERVAL_MS) {
			snake_timer_tick = get_tick();
			snake_step();
		}
	}
}

namespace {
void dma_init() {
	constexpr const DmaHwConfig cfg_dma1_s5 = { .Route = DmaChannel::CHNL_4,
			.Direction = DmaDir::PERIPH_TO_MEM, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::CIRCULAR,
			.Priority = DmaPrior::VERY_HIGH, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	constexpr const DmaHwConfig cfg_dma1_s6 = { .Route = DmaChannel::CHNL_4,
			.Direction = DmaDir::MEM_TO_PERIPH, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::VERY_HIGH, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	DMA_Config<usart2_rx, cfg_dma1_s5>();
	NVIC_API::SetPriority<IRQn_Type::DMA1_Stream5_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA1_Stream5_IRQn>();

	DMA_Config<usart2_tx, cfg_dma1_s6>();
	NVIC_API::SetPriority<IRQn_Type::DMA1_Stream6_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA1_Stream6_IRQn>();

	constexpr const DmaHwConfig cfg_dma2_s1 = { .Route = DmaChannel::CHNL_5,
			.Direction = DmaDir::PERIPH_TO_MEM, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::CIRCULAR,
			.Priority = DmaPrior::MEDIUM, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	constexpr const DmaHwConfig cfg_dma2_s7 = { .Route = DmaChannel::CHNL_5,
			.Direction = DmaDir::MEM_TO_PERIPH, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::CIRCULAR,
			.Priority = DmaPrior::MEDIUM, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	DMA_Config<usart6_rx, cfg_dma2_s1>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream1_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream1_IRQn>();

	DMA_Config<usart6_tx, cfg_dma2_s7>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream7_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream7_IRQn>();

	// F7: DMA2 stream 3 / stream 2, channel 6 = TIM1_CH1 / TIM1_CH2
	constexpr const DmaHwConfig cfg_dma2_s3 = { .Route = DmaChannel::CHNL_6,
			.Direction = DmaDir::MEM_TO_PERIPH, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::HALFWORD,
			.MemDataAlignment = DmaDataAlign::HALFWORD, .Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::VERY_HIGH, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	constexpr const DmaHwConfig cfg_dma2_s2 = { .Route = DmaChannel::CHNL_6,
			.Direction = DmaDir::MEM_TO_PERIPH, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::HALFWORD,
			.MemDataAlignment = DmaDataAlign::HALFWORD, .Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::VERY_HIGH, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	DMA_Config<tim1_ch1, cfg_dma2_s3>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream3_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream3_IRQn>();

	DMA_Config<tim1_ch2, cfg_dma2_s2>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream2_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream2_IRQn>();



	constexpr const DmaHwConfig cfg_dma1_s0 = { .Route = DmaChannel::CHNL_1,
			.Direction = DmaDir::PERIPH_TO_MEM, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::MEDIUM, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	constexpr const DmaHwConfig cfg_dma1_s7 = { .Route = DmaChannel::CHNL_1,
			.Direction = DmaDir::MEM_TO_PERIPH, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::CIRCULAR,
			.Priority = DmaPrior::MEDIUM, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	DMA_Config<i2c1_rx, cfg_dma1_s0>();
	NVIC_API::SetPriority<IRQn_Type::DMA1_Stream0_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA1_Stream0_IRQn>();

	DMA_Config<i2c1_rx, cfg_dma1_s7>();
	NVIC_API::SetPriority<IRQn_Type::DMA1_Stream7_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA1_Stream7_IRQn>();

}

void adc_init() {
	constexpr const AdcHwConfig cfg = { .ClockPrescaler = AdcClkPresc::DIV8,
			.EOCSelection = AdcEOCSel::SINGLE_CONV, .NbrOfConversion = 2,
			.ScanConvModeEnable = true, .ContinuousConvModeEnable = true };
	constexpr const AdcChannel ch1 = {
			.SamplingTime = chip::ADC_SMP, .Rank =
					AdcChnlRank::ADC_RANK_1, };
	constexpr const AdcChannel ch2 = {
			.SamplingTime = chip::ADC_SMP, .Rank =
					AdcChnlRank::ADC_RANK_2, };
	//constexpr const AdcWDGConfig cfg2;

	ADC_Config<ADC3, cfg>();
	ADC_ChannelConfig<ADC3, ch1, chip::ADC_PF8>();
	ADC_ChannelConfig<ADC3, ch2, chip::ADC_PF9>();
	//ADC_AWDConfig<ADC1, cfg2>();
	NVIC_API::SetPriority<chip::ADC3_IRQ, 5>();
	NVIC_API::EnableIRQ<chip::ADC3_IRQ>();
	ADC_Start_IT<ADC3>();
	//0,01460837035305120411503390226794 - modifier
	//ratio in/out 0,05516431924882629107981220657277
}
void tim_init() {
	constexpr const TimBaseConfig cfg = {
			.Prescaler = 0,   // PSC
			.Period = chip::WS2812_PERIOD - 1U,  // ARR: 800 kHz WS2812 bit
			.CounterMode = TimCounterMode::Up, .ClockDiv = TimClkDiv::DIV1,
			.RepetitionCounter = 0, .EnablePreload = true };

	constexpr const TimBaseConfig cfg2 = {
			.Prescaler = 0,   // PSC
			.Period = 500 - 1,     // ARR
			.CounterMode = TimCounterMode::Up, .ClockDiv = TimClkDiv::DIV1,
			.RepetitionCounter = 0, .EnablePreload = true };

	constexpr const TimOCConfig cfgOC = { .OCMode = TimOCMode::PWM1,
			.OCPolarity = TimOCPol::NONE, .OCNPolarity = TimOCPol::NONE,
			.OCIdleState = TimIdleState::NONE, .OCNIdleState =
					TimIdleState::NONE, .Pulse = 80, .OC_PreloadEnable = true };

	TIM_BaseConfig<TIM1, cfg>();
	TIM_BaseConfig<TIM4, cfg2>();

	TIM_PWM_ConfigChannel<TIM1, cfgOC, TimCh::CH1>();
	TIM_PWM_ConfigChannel<TIM1, cfgOC, TimCh::CH2>();
	TIM_PWM_ConfigChannel<TIM4, cfgOC, TimCh::CH1>();
	TIM_PWM_ConfigChannel<TIM4, cfgOC, TimCh::CH4>();

	// TIM_PWM_ChannelEnable<TIM1, TimCh::CH1>();
	// TIM_PWM_ChannelEnable<TIM1, TimCh::CH2>();
	TIM_PWM_ChannelEnable<TIM4, TimCh::CH1>();
	TIM_PWM_ChannelEnable<TIM4, TimCh::CH4>();

	NVIC_API::SetPriority<IRQn_Type::TIM1_CC_IRQn, 1>();
	NVIC_API::EnableIRQ<IRQn_Type::TIM1_CC_IRQn>();
}

// void can_init()
// {
// 	constexpr const CanHwConfig cfg = {
// 		.Prescaler = 6,
// 		.SyncJumpWidth = 1,
// 		.TimeSeg1 = 15,
// 		.TimeSeg2 = 2,
// 		.Mode = CanMode::Loopback,
// 		.TimeTriggerMode = false,
// 		.AutoBusOff = true,
// 		.AutoWakeUp = true,
// 		.AutoRetransmission = true,
// 		.FifoLock = false,
// 		.FifoPriority = false
// 	};

// 	CANStatus status = CAN_Init<CAN1, cfg>();
// 	if (status != CANStatus::Success) {
// 		Error_Handler(); // Handle initialization error
// 	}
// }

void i2c_init() {
	constexpr const I2CHwConfig cfg = { .Timing = chip::I2C1_TIMING, // 400 kHz
			.OwnAddres1 = 0x52,   // Example own address
			.OwnAddres2 = 0x4,   // Example own address
			.EnAnalogFilter = true };

	I2CStatus status = I2C_Init<I2C1, cfg>();
	if (status != I2CStatus::Success) {
		Error_Handler(); // Handle initialization error
	}
}

void uart_init() {
	constexpr const UartHwConfig cfg = {
			.Baud = 115200,
			.AssertTime = 4,
			.DeAssertTime = 4,
			.UseDMARx = true,
			.UseDMATx = true,
			.EnableDE = true,
			.DEPolarityHigh = false};
	USARTStatus status = USART_Init<USART2, cfg>();
	if (status != USARTStatus::Success) Error_Handler();

	constexpr const UartHwConfig cfg2 = {
			.Baud = 115200,
			.ClockSource = chip::USART_6_CLK,
			.UseDMARx = true,
			.UseDMATx = true };
	status = USART_Init<USART6, cfg2>();
	if (status != USARTStatus::Success) Error_Handler();
}

void inputs_update() {
	inputs::group::read_all(states);
}

// LED strip colour wheel
void led_step() {
	hue += 12;
	LedStrip1::fill_rgb_gradient(hue);
	LedStrip2::fill_rgb_gradient(hue);
	LedStrip1::show();
	LedStrip2::show();
}

// Running light on the outputs: forward, then back
void snake_step() {
	static size_t i_snake = 0;
	static bool snake_flag = true;
	if (snake_flag) {
		if (i_snake < outputs::group::count) {
			outputs::group::set_table[i_snake++]();
		} else {
			snake_flag = false;
		}
	} else if (i_snake > 0) {
		outputs::group::reset_table[--i_snake]();
	} else {
		outputs::group::reset_table[i_snake]();
		snake_flag = true;
	}
}


void RCC_Init() {
	constexpr const ClockConfig cfg = {
			.PLLM = 4,              // MCO(8Mhz)/8=2
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
	temp_conf.af = AF::AF9; // CAN1 (F7 bxCAN) / FDCAN1 (H7)
	can1::group::GPIO_ConfigGroupPin(temp_conf);

	//USART6/LIDAR GPIO Configuration
	temp_conf.mode = Mode::Alt;
	temp_conf.otype = OType::PP;
	temp_conf.speed = Speed::VeryHigh;
	temp_conf.pull = Pull::None;
	temp_conf.af = chip::USART_6_AF; // usart6
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


extern "C" void ADC3_IRQ_HANDLER() {
	if (ADC_Ready<ADC3>()) {
		adc3_ch[adc_idx++] = ADC_Read<ADC3>();
		if (adc_idx > 1)
			adc_idx = 0;
	}
	if (ADC_IsOVR<ADC3>()) {
		ADC_ClearOVR<ADC3>();
	}
}

extern "C" void DMA1_Stream0_IRQHandler() {}
extern "C" void DMA1_Stream1_IRQHandler() {}
extern "C" void DMA1_Stream2_IRQHandler() {}
extern "C" void DMA1_Stream3_IRQHandler() {}
extern "C" void DMA1_Stream4_IRQHandler() {}
extern "C" void DMA1_Stream5_IRQHandler() {}
extern "C" void DMA1_Stream6_IRQHandler() {}
extern "C" void DMA1_Stream7_IRQHandler() {}


extern "C" void DMA2_Stream0_IRQHandler() {}
extern "C" void DMA2_Stream2_IRQHandler() {DMA_IRQHandler<tim1_ch2>();}
extern "C" void DMA2_Stream3_IRQHandler() {DMA_IRQHandler<tim1_ch1>();}
extern "C" void DMA2_Stream1_IRQHandler() {}
extern "C" void DMA2_Stream4_IRQHandler() {}
extern "C" void DMA2_Stream5_IRQHandler() {}
extern "C" void DMA2_Stream6_IRQHandler() {}
extern "C" void DMA2_Stream7_IRQHandler() {DMA_IRQHandler<usart6_tx>();}








extern "C" void TIM1_CC_IRQHandler() {

}

void Error_Handler() {
	// Error occurred - enter infinite loop
	// TODO: Add error logging/indication (LED, UART, etc.)
	while (true) {
		__NOP();  // Prevent compiler optimization
	}
}
