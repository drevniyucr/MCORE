#include "mcore.hpp"
#include "core/def.hpp"
#include "main.hpp"
#include "board.hpp"
#include <cstdio>
bool states[inputs::group::count];

// ADC ISR (producer) -> main loop (consumer) over a lock-free SPSC queue.
struct AdcSample { uint8_t channel; uint16_t value; };
spsc::RingBuffer<AdcSample, 16> adc_queue;
uint16_t adc_latest[2] = { 0, 0 };  // last value per rank, drained in the loop
uint8_t adc_idx = 0;                 // ISR-owned scan position (rank 0 / 1)
uint8_t hue = 0;
UartStatus UART_sTATUS;
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

using LedStrip1 = WS2812<TIM1, TimCh::CH1, tim1_ch1, 13>;
using LedStrip2 = WS2812<TIM1, TimCh::CH2, tim1_ch2, 13>;

bool i2cstat = true;
uint8_t tx[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
uint8_t rx[4] = { 0 };
namespace {
// Local constants
constexpr uint32_t TCP_SEND_INTERVAL_MS = 500;
constexpr uint32_t TCP_TIMER_INTERVAL_MS = 100;
constexpr uint32_t SNAKE_TIMER_INTERVAL_MS = 400;
constexpr uint8_t TEST_CLIENT_INDEX = 9;  // Specific client for testing
constexpr uint16_t TCP_LISTEN_PORT = 5001;  // server port for incoming connections

uint8_t test_data[] = "LMAO LMAO LMAO LMAO LMAO LMAO LMAO";
uint8_t txData1[] = "Hello, USART! Hello, USART! Hello, USART! "
		"Hello, USART! Hello, USART! Hello, USART! Hello, USART!"
		" Hello, USART! Hello, USART! Hello, USART!";
uint8_t txData2[] = "Hello, RS485!";

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
}

namespace {
// --- periodic-task state (file-local; tasks below are stateless callbacks) ---
size_t snake_idx = 0;
bool   snake_forward = true;

// Every poll: service the Ethernet RX ring and sample the digital inputs.
void task_poll_io() {
	ETH_RxWorker();
	inputs_update();
}

// Every poll: drain ADC samples produced by the ISR into per-rank latest values.
void task_adc_drain() {
	AdcSample s;
	while (adc_queue.pop(s)) {
		if (s.channel < 2) adc_latest[s.channel] = s.value;
	}
}

// Periodic: push test traffic over RS-485 / USART6 and the test TCP client.
void task_tcp_send() {
	UART_sTATUS = UartHandle<USART2>::sendBuffer(txData2, sizeof(txData2));
	UartHandle<USART6>::sendBuffer(txData1, sizeof(txData1));
	if (tcp_clients[TEST_CLIENT_INDEX].state == tcp_state_t::TCP_ESTABLISHED) {
		NET_TCP_SendUser(&tcp_clients[TEST_CLIENT_INDEX], test_data, sizeof(test_data));
	}
}

// Periodic: run TCP/DHCP timers and advance the LED gradient animation.
void task_net_timers() {
	NET_TCP_Timers();
	NET_DHCP_Poll();
	hue += 12;
	LedStrip1::fill_rgb_gradient(hue);
	LedStrip2::fill_rgb_gradient(hue);
	LedStrip1::show();
	LedStrip2::show();
}

// Periodic: "snake" sweep across the digital outputs + a USART6 DMA burst.
void task_snake() {
	if (usart6_tx.state == DmaState::READY) {
		UartHandle<USART6>::UartTxDMA<usart6_tx>(txData1, sizeof(txData1));
	}
	if (snake_forward) {
		if (snake_idx < outputs::group::count) {
			outputs::group::set_table[snake_idx++]();
		} else {
			snake_forward = false;
		}
	} else {
		if (snake_idx > 0) {
			outputs::group::reset_table[--snake_idx]();
		} else {
			outputs::group::reset_table[snake_idx]();
			snake_forward = true;
		}
	}
}
}

int main() {
	SystemInit();

	uint32_t init_cycles;
	{
		prof::ScopeCycles _timer{&init_cycles};  // elapsed written on scope exit

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
	}
	print("Spend %lu cycles for init\n", init_cycles);

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

	// Main loop: a cooperative scheduler replaces the hand-rolled tick diffs.
	// Registration order == run order within a poll. period 0 == every pass.
	sched::Scheduler<5> scheduler;
	scheduler.every(0,                       task_poll_io);    // ETH RX + inputs
	scheduler.every(0,                       task_adc_drain);  // drain ADC SPSC queue
	scheduler.every(TCP_SEND_INTERVAL_MS,    task_tcp_send);
	scheduler.every(TCP_TIMER_INTERVAL_MS,   task_net_timers);
	scheduler.every(SNAKE_TIMER_INTERVAL_MS, task_snake);
	scheduler.start(get_tick());

	while (true) {
		scheduler.poll(get_tick());
	}
}

namespace {
void dma_init() {
	constexpr const DmaHwConfig cfg_dma1_s5 = { .Channel = DmaChannel::CHNL_4,
			.Direction = DmaDir::PERIPH_TO_MEM, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::CIRCULAR,
			.Priority = DmaPrior::VERY_HIGH, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	constexpr const DmaHwConfig cfg_dma1_s6 = { .Channel = DmaChannel::CHNL_4,
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

	constexpr const DmaHwConfig cfg_dma2_s1 = { .Channel = DmaChannel::CHNL_5,
			.Direction = DmaDir::PERIPH_TO_MEM, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::CIRCULAR,
			.Priority = DmaPrior::MEDIUM, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	constexpr const DmaHwConfig cfg_dma2_s7 = { .Channel = DmaChannel::CHNL_5,
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

	constexpr const DmaHwConfig cfg_dma2_s2_3 = { .Channel = DmaChannel::CHNL_6,
			.Direction = DmaDir::MEM_TO_PERIPH, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::HALFWORD,
			.MemDataAlignment = DmaDataAlign::HALFWORD, .Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::VERY_HIGH, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	DMA_Config<tim1_ch1, cfg_dma2_s2_3>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream3_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream3_IRQn>();

	DMA_Config<tim1_ch2, cfg_dma2_s2_3>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream2_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream2_IRQn>();



	constexpr const DmaHwConfig cfg_dma1_s0 = { .Channel = DmaChannel::CHNL_1,
			.Direction = DmaDir::PERIPH_TO_MEM, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::MEDIUM, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	constexpr const DmaHwConfig cfg_dma1_s7 = { .Channel = DmaChannel::CHNL_1,
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
			.SamplingTime = AdcSampleTime::_480CYCLES, .Rank =
					AdcChnlRank::ADC_RANK_1, };
	constexpr const AdcChannel ch2 = {
			.SamplingTime = AdcSampleTime::_480CYCLES, .Rank =
					AdcChnlRank::ADC_RANK_2, };
	//constexpr const AdcWDGConfig cfg2;

	ADC_Config<ADC3, cfg>();
	ADC_ChannelConfig<ADC3, ch1, AdcChnl::ADC_CHANNEL_6>();
	ADC_ChannelConfig<ADC3, ch2, AdcChnl::ADC_CHANNEL_7>();
	//ADC_AWDConfig<ADC1, cfg2>();
	NVIC_API::SetPriority<IRQn_Type::ADC_IRQn, 5>();
	NVIC_API::EnableIRQ<IRQn_Type::ADC_IRQn>();
	ADC_Start_IT<ADC3>();
	//0,01460837035305120411503390226794 - modifier
	//ratio in/out 0,05516431924882629107981220657277
}
void tim_init() {
	constexpr const TimBaseConfig cfg = {
			.Prescaler = 0,   // PSC
			.Period = 270 - 1,      // ARR
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
	constexpr const I2CHwConfig cfg = { .Timing = 0x20404768, // Timing value for 400kHz I2C speed with 54MHz PCLK1
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
			.ClockSource =UartClockSrc::SYSCLK,
			.UseDMARx = true,
			.UseDMATx = true };
	status = USART_Init<USART6, cfg2>();
	if (status != USARTStatus::Success) Error_Handler();
}

void inputs_update() {
	inputs::group::read_all(states);
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

	// Конфиги пинов заданы как compile-time константы: configure<Cfg>() сворачивает
	// маски в литералы и делает одну RMW-запись на порт на регистр.
	// Порядок полей GPIO_Config: { mode, otype, speed, pull, af }.
	constexpr GPIO_Config eth_cfg    { Mode::Alt, OType::PP, Speed::VeryHigh, Pull::None,   AF::AF11 }; // Ethernet
	constexpr GPIO_Config in_cfg     { Mode::In,  OType::PP, Speed::Medium,   Pull::None,   AF::AF0  };
	constexpr GPIO_Config out_cfg    { Mode::Out, OType::PP, Speed::VeryHigh, Pull::None,   AF::AF0  };
	constexpr GPIO_Config rs485_cfg  { Mode::Alt, OType::PP, Speed::VeryHigh, Pull::PullUp, AF::AF7  }; // USART2
	constexpr GPIO_Config can1_cfg   { Mode::Alt, OType::PP, Speed::VeryHigh, Pull::None,   AF::AF9  };
	constexpr GPIO_Config usart6_cfg { Mode::Alt, OType::PP, Speed::VeryHigh, Pull::None,   AF::AF8  };
	constexpr GPIO_Config tim1_cfg   { Mode::Alt, OType::PP, Speed::VeryHigh, Pull::None,   AF::AF1  };
	constexpr GPIO_Config tim4_cfg   { Mode::Alt, OType::PP, Speed::VeryHigh, Pull::None,   AF::AF2  };
	constexpr GPIO_Config adc3_cfg   { Mode::Ang, OType::PP, Speed::VeryHigh, Pull::None,   AF::AF0  };
	constexpr GPIO_Config i2c1_cfg   { Mode::Alt, OType::OD, Speed::VeryHigh, Pull::None,   AF::AF4  };

	eth::group::configure<eth_cfg>();
	inputs::group::configure<in_cfg>();
	outputs::group::configure<out_cfg>();
	rs485::group::configure<rs485_cfg>();
	can1::group::configure<can1_cfg>();
	usart6::group::configure<usart6_cfg>();
	tim1::group::configure<tim1_cfg>();
	tim4::group::configure<tim4_cfg>();
	adc3::group::configure<adc3_cfg>();
	i2c1::group::configure<i2c1_cfg>();
}
}

extern "C" void ADC_IRQHandler() {
	if (ADC3::_SR::EOC::is_set()) {  // reading DR below clears EOC
		const AdcSample s{ adc_idx, static_cast<uint16_t>(ADC3::_DR::DATA::read()) };
		adc_queue.push(s);           // ISR never blocks; drop if the loop fell behind
		adc_idx = (adc_idx + 1) & 1;
	}
	if (ADC3::_SR::OVR::is_set()) {
		ADC3::_SR::OVR::clear();
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
