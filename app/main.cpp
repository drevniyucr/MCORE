#include "mcore.hpp"
#include "core/def.hpp"
#include "main.hpp"
#include "board.hpp"
#include <cstdio>
#if defined(MCORE_DUAL_CORE)
// STM32H755: this image runs on the Cortex-M7 (real time); the network
// stack runs on the Cortex-M4 (app_cm4/). They talk through ipc_shared.hpp.
#include "chip/stm32h755/hsem.hpp"
#include "ipc_shared.hpp"
#include "motion_sim.hpp"
#include "sched/sched.hpp"
#endif

// ---------------------------------------------------------------------------
// Chip-specific board constants (everything else in this file is shared)
// ---------------------------------------------------------------------------
namespace chip {
#if defined(MCORE_STM32H755)
// CPU 400 MHz, AXI/AHB 200 MHz, APB1/APB2 100 MHz, timer clocks 200 MHz
constexpr uint16_t WS2812_PERIOD = 250;      // 200 MHz / 800 kHz
// 400 kHz @ I2C kernel clock = PCLK1 100 MHz: PRESC=11 (120 ns), SCLDEL=3,
// SDADEL=2, SCLH=5, SCLL=12 -> tLOW ~1.6 us, tHIGH ~0.8 us (RM0399 limits met
// for tr/tf up to 300 ns)
constexpr uint32_t I2C1_TIMING = 0xB032050C;
constexpr AF USART_6_AF = AF::AF7;           // PG14/PC7: USART6 is AF7 on the H7
constexpr UartClockSrc USART_6_CLK = UartClockSrc::PCLK;
constexpr AdcChnl ADC_PF8 = AdcChnl::ADC_CHANNEL_7;   // PF8 = ADC3_INP7
constexpr AdcChnl ADC_PF9 = AdcChnl::ADC_CHANNEL_2;   // PF9 = ADC3_INP2
constexpr AdcSampleTime ADC_SMP = AdcSampleTime::_387CYCLES5;
using ADC3_IRQ = IRQn_Type::ADC3_IRQn;
#define ADC3_IRQ_HANDLER ADC3_IRQHandler
#else
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
#endif
} // namespace chip

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

// Completed USART6 DMA-TX transfers, incremented from the DMA ISR via the
// on_complete callback (observable via debugger). Demonstrates event-driven DMA.
std::atomic<uint32_t> usart6_tx_count{ 0 };

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
constexpr uint32_t TCP_SEND_INTERVAL_MS = 500;
constexpr uint32_t TCP_TIMER_INTERVAL_MS = 100;
constexpr uint32_t SNAKE_TIMER_INTERVAL_MS = 400;
constexpr uint8_t TEST_CLIENT_INDEX = 9;  // Specific client for testing
constexpr uint16_t TCP_LISTEN_PORT = 5001;  // server port for incoming connections

#if !defined(MCORE_DUAL_CORE)
uint8_t test_data[] = "LMAO LMAO LMAO LMAO LMAO LMAO LMAO";
#endif

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

int main() {
	//SystemInit(); startup_stm32f767zitx.s calls SystemInit() before main() is entered, so no need to call it here.

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

	const uint32_t elapsed = DWT::_CYCCNT::read() - start;
	print("Spend %lu cycles for init\n", elapsed);

	i2cstat = Eeprom47L16<I2C1>::write(0x0010, tx, sizeof(tx));

	/* Read back */
	i2cstat &= Eeprom47L16<I2C1>::read(0x0010, rx, sizeof(rx));

	/* Verify */
	if (memcmp(tx, rx, sizeof(tx)) == 0 && i2cstat) {
		// Data matches
	} else {
		// Data does not match
		Error_Handler();
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
		if ((get_tick() - tcp_send_tick) >= 500) {
			tcp_send_tick = get_tick();
			if(1){
				UART_sTATUS = UartHandle<USART2>::sendBuffer(txData2,sizeof(txData2));
				UartHandle<USART6>::sendBuffer(txData1,sizeof(txData1));
			}
			if (tcp_clients[TEST_CLIENT_INDEX].state == tcp_state_t::TCP_ESTABLISHED) {
				NET_TCP_SendUser(&tcp_clients[TEST_CLIENT_INDEX], test_data,sizeof(test_data));
			}

		}

		// Run TCP/DHCP timers
		if ((get_tick() - tcp_timer_tick) >= TCP_TIMER_INTERVAL_MS) {
			NET_TCP_Timers();
			NET_DHCP_Poll();
			tcp_timer_tick = get_tick();
			hue += 12;
			LedStrip1::fill_rgb_gradient(hue);
			LedStrip2::fill_rgb_gradient(hue);
			LedStrip1::show();
			LedStrip2::show();

		}
		if ((get_tick() - snake_timer_tick) >= SNAKE_TIMER_INTERVAL_MS) {
			snake_timer_tick = get_tick();

			if (usart6_tx.state == DmaState::READY) {
				UartHandle<USART6>::UartTxDMA<usart6_tx>(txData1,
						sizeof(txData1));
			}

			if (snake_flag) // движение вперёд
			{
				if (i_snake < outputs::group::count) {
					outputs::group::set_table[i_snake++]();
				} else {
					snake_flag = false;
				}
			} else // движение назад
			{
				if (i_snake > 0) {
					outputs::group::reset_table[--i_snake]();
				} else if (i_snake == 0) {
					outputs::group::reset_table[i_snake]();
					snake_flag = true;
				}
			}
		}
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

	constexpr const DmaHwConfig cfg_dma1_s6 = { MCORE_DMA_ROUTE(CHNL_4, USART2_TX),
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

	DMA_Config<usart2_rx, cfg_dma1_s5>();
	NVIC_API::SetPriority<IRQn_Type::DMA1_Stream5_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA1_Stream5_IRQn>();

	DMA_Config<usart2_tx, cfg_dma1_s6>();
	NVIC_API::SetPriority<IRQn_Type::DMA1_Stream6_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA1_Stream6_IRQn>();

	// BUGFIX: was DMA_Config<i2c1_rx, ...> — the TX config (MEM_TO_PERIPH,
	// stream 7) was applied to the RX handle a second time, and i2c1_tx was
	// never configured (the second call also failed: state != RESET).
	DMA_Config<i2c1_tx, cfg_dma1_s7>();
	NVIC_API::SetPriority<IRQn_Type::DMA1_Stream7_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA1_Stream7_IRQn>();

	DMA_Config<usart6_rx, cfg_dma2_s1>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream1_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream1_IRQn>();

	DMA_Config<tim1_ch1, cfg_dma2_s2>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream3_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream3_IRQn>();

	DMA_Config<tim1_ch2, cfg_dma2_s2>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream2_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream2_IRQn>();

	DMA_Config<usart6_tx, cfg_dma2_s7>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream7_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream7_IRQn>();
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

// 	DrvStatus status = CAN_Init<CAN1, cfg>();
// 	if (status != DrvStatus::Ok) {
// 		Error_Handler(); // Handle initialization error
// 	}
// }

void i2c_init() {
	constexpr const I2CHwConfig cfg = { .Timing = 0x20404768, // Timing value for 400kHz I2C speed with 54MHz PCLK1
			.OwnAddres1 = 0x52,   // Example own address
			.OwnAddres2 = 0x4,   // Example own address
			.EnAnalogFilter = true };

	DrvStatus status = I2C_Init<I2C1, cfg>();
	if (status != DrvStatus::Ok) {
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
	DrvStatus status = USART_Init<USART2, cfg>();
	if (status != DrvStatus::Ok) Error_Handler();

	constexpr const UartHwConfig cfg2 = {
			.Baud = 115200,
			.ClockSource = chip::USART_6_CLK,
			.UseDMARx = true,
			.UseDMATx = true };
	status = USART_Init<USART6, cfg2>();
	if (status != DrvStatus::Ok) Error_Handler();
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

#if defined(MCORE_DUAL_CORE)
// --- vehicle model + link to the network core ---------------------------------
MotionSim motion;
uint32_t ipc_events_dropped = 0;

// Events raised by the model are held back until the snapshot of the same
// millisecond is published: when the M4 sees "node reached" or "action done",
// the state it reads already shows the robot standing / the new load.
ipc_app::Event held_events[8];
uint8_t held_count = 0;

void push_event(const ipc_app::Event &ev) {
	if (ipc_app::shared().events.push(ev)) {
		(void) hsem::notify(ipc_app::EVT_SEM);
	} else {
		ipc_events_dropped += 1;
	}
}

void hold_event(const ipc_app::Event &ev) {
	if (held_count < sizeof(held_events) / sizeof(held_events[0])) {
		held_events[held_count++] = ev;
	} else {
		push_event(ev);
	}
}

void release_events() {
	for (uint8_t i = 0; i < held_count; ++i) {
		push_event(held_events[i]);
	}
	held_count = 0;
}

void on_node_reached(uint32_t seq, void *) {
	ipc_app::Event ev{};
	ev.type = ipc_app::EventType::NodeReached;
	ev.arg[0] = seq;
	hold_event(ev);
}

void on_action_done(uint8_t uid, bool ok, void *) {
	ipc_app::Event ev{};
	ev.type = ipc_app::EventType::ActionDone;
	ev.arg[0] = uid;
	ev.arg[1] = ok ? 1U : 0U;
	hold_event(ev);
}

void handle_command(const ipc_app::Command &cmd) {
	switch (cmd.type) {
	case ipc_app::CmdType::Ping: {
		ipc_app::Event ev{};
		ev.type = ipc_app::EventType::Pong;
		ev.seq = cmd.seq;
		ev.arg[0] = cmd.arg[0];
		ev.arg[1] = get_tick();
		push_event(ev);
		break;
	}
	case ipc_app::CmdType::DriveTo: {
		const ipc_app::DrivePayload &d = cmd.drive;
		MotionTarget t{ d.path_index, d.node_seq, d.x, d.y, d.theta, d.is_node != 0, d.has_theta != 0,
		                d.max_speed, d.tolerance };
		(void) motion.push(t);   // the M4 keeps at most 16 waypoints outstanding
		break;
	}
	case ipc_app::CmdType::StopMotion:
		motion.stop(cmd.flag);
		break;
	case ipc_app::CmdType::SetHold:
		motion.set_hold(cmd.flag != 0U);
		break;
	case ipc_app::CmdType::SetSpeedLimit:
		motion.set_speed_limit(cmd.speed);
		break;
	case ipc_app::CmdType::SetPaused:
		motion.set_paused(cmd.flag != 0U);
		break;
	case ipc_app::CmdType::SetPose:
		motion.set_pose(cmd.pose.x, cmd.pose.y, cmd.pose.theta);
		break;
	case ipc_app::CmdType::ActionStart:
		if (!motion.start_action(cmd.action.uid, static_cast<uint8_t>(cmd.action.code),
		                         cmd.action.duration_ms)) {
			on_action_done(cmd.action.uid, false, nullptr);
		}
		break;
	case ipc_app::CmdType::ActionCancel:
		motion.cancel_action(cmd.action.uid);
		break;
	default:
		break;
	}
}

uint32_t cmds_handled = 0;

// Commands from the network core: on its doorbell and every millisecond
void ipc_commands() {
	ipc_app::Command cmd;
	while (ipc_app::shared().commands.pop(cmd)) {
		cmds_handled += 1;
		handle_command(cmd);
	}
}

// One control period: the vehicle model advances and the state snapshot is
// republished. Never blocks.
void ipc_step() {
	static uint32_t last_tick = get_tick();
	ipc_app::SharedArea &sh = ipc_app::shared();
	ipc_commands();

	const uint32_t now = get_tick();
	if (now != last_tick) {
		motion.step(static_cast<float>(now - last_tick) * 0.001F);
		last_tick = now;

		static_assert(inputs::group::count <= 32, "inputs bitmask is 32 bits wide");
		ipc_app::RtState st{};
		st.tick = now;
		for (uint32_t i = 0; i < inputs::group::count; ++i) {
			st.inputs |= states[i] ? (1UL << i) : 0UL;
		}
		st.adc[0] = static_cast<uint16_t>(adc3_ch[0]);
		st.adc[1] = static_cast<uint16_t>(adc3_ch[1]);
		st.cmds_handled = cmds_handled;
		st.events_dropped = ipc_events_dropped;
		st.x = motion.x();
		st.y = motion.y();
		st.theta = motion.theta();
		st.vx = motion.speed();
		st.omega = motion.omega();
		st.driving = motion.driving() ? 1U : 0U;
		st.paused = motion.paused() ? 1U : 0U;
		st.loaded = motion.loaded() ? 1U : 0U;
		st.queued_targets = motion.queued();
		st.battery_soc = 100.0F;   // no battery measurement yet
		st.last_popped = motion.last_popped();
		st.stop_ack = motion.stop_ack();
		st.any_popped = motion.any_popped() ? 1U : 0U;
		st.held = motion.held() ? 1U : 0U;
		st.speed_limit = motion.speed_limit();
		sh.state.write(st);
		release_events();
	}
	if (held_count != 0) {
		release_events();          // e.g. a failed ActionStart outside a step
	}
}

// --- tasks ------------------------------------------------------------------------
//   control  prio 3  1 kHz: inputs, M4 commands, vehicle model, state snapshot;
//                    woken early by the M4 doorbell (HSEM1) for commands
//   leds     prio 1  LED strip animation and the output running light: slow
//                    work that must never delay the control period
enum : uint8_t { SIG_TICK = 0, SIG_CMD = 1, SIG_LED = 2, SIG_SNAKE = 3 };
constexpr uint8_t PRIO_CONTROL = 3;
constexpr uint8_t PRIO_LEDS = 1;

void control_handler(sched::Task &, const sched::Event &e) {
	if (e.sig == SIG_TICK) {
		inputs_update();
		ipc_step();
	} else if (e.sig == SIG_CMD) {
		ipc_commands();
	}
}

void leds_handler(sched::Task &, const sched::Event &e) {
	if (e.sig == SIG_LED) {
		led_step();
	} else if (e.sig == SIG_SNAKE) {
		snake_step();
	}
}
#endif

void RCC_Init() {
#if defined(MCORE_STM32H755)
	// HSE 8 MHz (ST-LINK MCO, bypass) /4 = 2 MHz x400 = 800 MHz VCO
	//   /2 -> sys_ck 400 MHz -> CPU 400 MHz, AXI/AHB 200 MHz, APBx 100 MHz
	constexpr const ClockConfig cfg = {
			.PLLM = 4, .PLLN = 400, .PLLP = 2, .PLLQ = 8, .PLLR = 2,
			.CPUDiv = AHBPrescaler::Div1,
			.AHBDiv = AHBPrescaler::Div2,
			.APB3Div = APBPrescaler::Div2,
			.APB1Div = APBPrescaler::Div2,
			.APB2Div = APBPrescaler::Div2,
			.APB4Div = APBPrescaler::Div2,
			.useHSE = true, .useHSEBypass = true, .useSysTick = true };

	// The supply (SMPS/LDO) was already set by ExitRun0Mode() at reset.
	if (SetVoltageScale<VoltageScale::Scale1>() != RCCStatus::OK
			|| OSCInit<cfg>() != RCCStatus::OK
			|| RCC_ClockInit<cfg>() != RCCStatus::OK
			|| enableHSI48() != RCCStatus::OK) {   // RNG clock for the M4 (TLS)
		Error_Handler();
	}
#else
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
#endif
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
