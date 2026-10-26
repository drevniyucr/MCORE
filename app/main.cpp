#include "mcore.hpp"
#include "core/def.hpp"
#include "main.hpp"
#include "board.hpp"
#include "mqtt_demo.hpp"
#include <cstdio>
#include <atomic>
#include <cstring>

bool states[inputs::group::count];

// ADC ISR (producer) -> main loop (consumer) over a lock-free SPSC queue.
struct AdcSample { uint8_t channel; uint16_t value; };
spsc::RingBuffer<AdcSample, 16> adc_queue;
uint16_t adc_latest[2] = { 0, 0 };  // last value per rank, drained in the loop
uint8_t adc_idx = 0;                 // ISR-owned scan position (rank 0 / 1)
uint8_t hue = 0;
bool eeprom_ok = true;               // EEPROM self-test result (watch in the debugger)
alignas(32) uint8_t rx_usart6_buf[128];   // DMA receive: whole cache lines
alignas(32) uint8_t rx_usart2_buf[128];

DmaHandle<DmaStream::DMA2_Stream_1> usart6_rx;
DmaHandle<DmaStream::DMA2_Stream_7> usart6_tx;

// Completed USART6 DMA-TX transfers, incremented from the DMA ISR via the
// on_complete callback (observable via debugger). Demonstrates event-driven DMA.
std::atomic<uint32_t> usart6_tx_count{ 0 };

// CAN1 loopback frames received / sent (observable via debugger).
std::atomic<uint32_t> can_rx_count{ 0 };
uint32_t can_tx_count = 0;
CanFrame can_last_rx;

DmaHandle<DmaStream::DMA1_Stream_5> usart2_rx;
DmaHandle<DmaStream::DMA1_Stream_6> usart2_tx;

DmaHandle<DmaStream::DMA2_Stream_3> tim1_ch1;
DmaHandle<DmaStream::DMA2_Stream_2> tim1_ch2;

DmaHandle<DmaStream::DMA1_Stream_0> i2c1_rx;
DmaHandle<DmaStream::DMA1_Stream_7> i2c1_tx;

DmaHandle<DmaStream::DMA2_Stream_0> spi4_rx;  // channel 4
DmaHandle<DmaStream::DMA2_Stream_4> spi4_tx;  // channel 5

// SPI4 with nothing attached: MISO only sees its pull resistor, so every
// received byte is 0xFF (pull-up, as configured) or 0x00 (pull-down).
// Observable via debugger.
uint8_t spi_rx_poll[4];
alignas(32) uint8_t spi_dma_buf[32];
DrvStatus spi_status;
std::atomic<uint32_t> spi_dma_count{ 0 };

// Why the MCU last reset, RTC clock state and a 1 Hz wakeup counter
// (observable via debugger). Setting wdg_starve = true from the debugger
// stops the watchdog kicks: the board must reset with IndependentWatchdog.
ResetCause reset_cause;
RtcClock rtc_source;
RtcDateTime rtc_now;
std::atomic<uint32_t> rtc_wakeups{ 0 };
volatile bool wdg_starve = false;

// Fault that caused the last reset (if have_last_fault). Setting fault_test
// from the debugger provokes one: 1 bus error, 2 divide by zero,
// 3 call through a pointer without the Thumb bit, 4 stack overflow.
FaultRecord last_fault;
bool have_last_fault = false;
volatile uint8_t fault_test = 0;
volatile int fault_sink;

// DNS demo (see task_dns_demo)
constexpr const char* DNS_DEMO_HOST = "pool.ntp.org";
uint8_t dns_demo_ip[4];
net_result_t dns_demo_result = net_result_t::PENDING;

using LedStrip1 = WS2812<TIM1, TimCh::CH1, tim1_ch1, 13>;
using LedStrip2 = WS2812<TIM1, TimCh::CH2, tim1_ch2, 13>;

bool i2cstat = true;
uint8_t tx[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
uint8_t rx[4] = { 0 };
namespace {
// Local constants
constexpr uint32_t TCP_TIMER_INTERVAL_MS = 100;
constexpr uint32_t SNAKE_TIMER_INTERVAL_MS = 400;
constexpr uint16_t TCP_LISTEN_PORT = 5001;  // server port for incoming connections

uint8_t txData1[] = "Hello, USART! Hello, USART! Hello, USART! "
		"Hello, USART! Hello, USART! Hello, USART! Hello, USART!"
		" Hello, USART! Hello, USART! Hello, USART!";

// CAN1 runs in loopback: every frame sent is received back through filter
// bank 0 into FIFO0 and counted by the RX ISR callback.
constexpr uint32_t CAN_SEND_INTERVAL_MS = 500;
CanFrame tx_can = {
	.id = 0x123,
	.dlc = 2,
	.data = {0xDE, 0xAD}
};

constexpr uint32_t SPI_INTERVAL_MS = 500;
constexpr uint32_t RTC_READ_INTERVAL_MS = 1000;
constexpr uint32_t WDG_TIMEOUT_MS = 1000;
constexpr uint32_t WDG_KICK_INTERVAL_MS = 100;  // 10x margin covers the LSI spread

// Forward declarations
void GPIO_Init();
void RCC_Init();
void uart_init();
void i2c_init();
void can_init();
void spi_init();
void rtc_init();
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

// Periodic: send a CAN1 test frame (byte 1 carries a rolling counter).
void task_can_send() {
	tx_can.data[1] = static_cast<uint8_t>(can_tx_count);
	if (CanHandle<CAN1>::try_send(tx_can) == DrvStatus::Ok) {
		++can_tx_count;
	}
}

// Periodic: one polling read and one in-place DMA exchange on SPI4.
void task_spi() {
	spi_status = SpiHandle<SPI4>::read(spi_rx_poll, sizeof(spi_rx_poll));

	if (spi4_rx.ready()) {
		for (size_t i = 0; i < sizeof(spi_dma_buf); ++i) spi_dma_buf[i] = static_cast<uint8_t>(i);
		spi_status = SpiHandle<SPI4>::transfer_dma<spi4_rx, spi4_tx>(
				spi_dma_buf, spi_dma_buf, sizeof(spi_dma_buf));
	}
}

// Periodic: snapshot the calendar.
void task_rtc_read() {
	RTC_GetDateTime(rtc_now);
}

// Periodic: kick the watchdog (unless starved on purpose from the debugger).
void task_watchdog() {
	if (!wdg_starve) IWDG_Refresh();
}

// Periodic: resolve a public host name (DHCP-provided DNS server, through the
// gateway). Result observable via debugger: dns_demo_ip / dns_demo_result.
// Every poll: MQTT telemetry link (DNS -> TCP -> TLS -> MQTT, all non-blocking)
void task_mqtt() {
	const MqttTelemetry t = {
		get_tick(), RTC_UnixTime(), { adc_latest[0], adc_latest[1] },
		can_rx_count.load(std::memory_order_relaxed),
		ntp_client.syncs,
	};
	mqtt_demo_poll(t);
}

void task_dns_demo() {
	dns_demo_result = NET_DNS_Resolve(DNS_DEMO_HOST, dns_demo_ip);
}

// Unbounded recursion for fault_test 4: 256+ bytes per level until the stack
// runs off its bottom (0x20000000) into reserved address space
[[gnu::noinline]] int stack_eater(int depth) {
	volatile uint8_t pad[256];
	pad[0] = static_cast<uint8_t>(depth);
	if (depth > 100000) return 0;              // never reached: 8 KB run out at ~30 levels
	return stack_eater(depth + 1) + pad[0];    // not a tail call: the frame stays
}

// Periodic: provoke the fault requested from the debugger (see fault_test).
void task_fault_test() {
	switch (fault_test) {
	case 1:  // just past the end of SRAM: precise BusFault
		fault_sink = static_cast<int>(*reinterpret_cast<volatile uint32_t*>(0x2008'0000));
		break;
	case 2: {  // UsageFault, trapped by Fault_Init (CCR.DIV_0_TRP)
		volatile int zero = 0;
		fault_sink = 100 / zero;  // not 1/x: GCC turns that into a compare, no sdiv
		break;
	}
	case 3:  // even address = Thumb bit clear: UsageFault INVSTATE
		reinterpret_cast<void (*)()>(0x0800'0000)();
		break;
	case 4:  // stack overflow: must fault at the stack bottom, not corrupt data
		fault_sink = stack_eater(0);
		break;
	default:
		break;
	}
}

// Rainbow animation over a WS2812 strip's pixel buffer. Application code
// (moved out of the driver); integer math instead of the old float version —
// the hue wheel is 256 wide, three sectors of 85 steps (3*85 ≈ 255).
template<typename Strip>
void fill_rainbow(uint8_t offset) {
	if (!Strip::ready())
		return;

	constexpr size_t n = sizeof(Strip::leds) / sizeof(Strip::leds[0]);
	for (size_t i = 0; i < n; i++) {
		uint8_t h = static_cast<uint8_t>((i * 255u) / n + offset);
		uint8_t r, g, b;
		if (h < 85) {
			r = 255 - h * 3; g = h * 3;       b = 0;
		} else if (h < 170) {
			h -= 85;
			r = 0;           g = 255 - h * 3; b = h * 3;
		} else {
			h -= 170;
			r = h * 3;       g = 0;           b = 255 - h * 3;
		}
		Strip::leds[i].r = r;
		Strip::leds[i].g = g;
		Strip::leds[i].b = b;
	}
}

// Periodic: run TCP/DHCP timers and advance the LED gradient animation.
void task_net_timers() {
	NET_TCP_Timers();
	NET_DHCP_Poll();
	NET_ARP_Poll();
	NET_DNS_Poll();
	NET_NTP_Poll();
	hue += 12;
	fill_rainbow<LedStrip1>(hue);
	fill_rainbow<LedStrip2>(hue);
	LedStrip1::show();
	LedStrip2::show();
}

// Periodic: "snake" sweep across the digital outputs + a USART6 DMA burst.
void task_snake() {
	if (usart6_tx.ready()) {
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
	//SystemInit(); startup_stm32f767zitx.s calls SystemInit() before main() is entered, so no need to call it here.

	reset_cause = RCC_ResetCause();  // first thing: the flags accumulate until cleared
	have_last_fault = Fault_TakeLast(last_fault);
	Fault_Init();

	uint32_t init_cycles;
	{
		prof::ScopeCycles _timer{&init_cycles};  // elapsed written on scope exit

		RCC_Init();
		MPU_Config();
		SCB_EnableICache();
		SCB_EnableDCache();   // DMA buffers: the DMA driver does the cache maintenance
		GPIO_Init();
		uart_init();
		i2c_init();
		can_init();
		spi_init();
		rtc_init();
		tim_init();
		NVIC_API::EnableIRQGlobal();
		adc_init();
		dma_init();
		if (ETH_Init() != ETH_Status::OK) {
			Error_Handler();
		}

		NET_TCP_Init();
		NET_TCP_Listen(TCP_LISTEN_PORT);
		// Bench: the board sits on the PC's 192.168.99.0/24 cable (broker at
		// .10); DHCP saves this as the fallback when no server answers
		const uint8_t bench_ip[4] = { 192, 168, 99, 123 };
		NET_SetIPAddr(bench_ip);
		NET_DHCP_Start();  // falls back to the static IP if no server answers
		// RTC from the network (UTC), hourly. Bench: bench/ntp_server.py on the
		// PC (port 123 there belongs to the Windows time service); on a network
		// with Internet access: NET_NTP_Start("pool.ntp.org", NTP_PORT, ...)
		NET_NTP_Start("192.168.99.10", 12300, [](int64_t unix_s, uint16_t ms) {
			RTC_SetUnixTime(unix_s + (ms >= 500 ? 1 : 0));
		});
		UartHandle<USART2>::UartRxDMA<usart2_rx>(rx_usart2_buf,sizeof(rx_usart2_buf));
		UartHandle<USART6>::UartRxDMA<usart6_rx>(rx_usart6_buf,sizeof(rx_usart6_buf));
	}
	print("Spend %lu cycles for init\n", init_cycles);
	if (have_last_fault) {
		Fault_Print(last_fault);
	}

	i2cstat = Eeprom47L16<I2C1>::write(0x0010, tx, sizeof(tx));

	/* Read back */
	i2cstat &= Eeprom47L16<I2C1>::read(0x0010, rx, sizeof(rx));

	/* Verify */
	if (memcmp(tx, rx, sizeof(tx)) == 0 && i2cstat) {
		// Data matches
	} else {
		// Data does not match (or no EEPROM). Release builds stop here; Debug
		// builds go on, so a bench board without the 47L16 still runs the rest
		eeprom_ok = false;
#ifdef NDEBUG
		Error_Handler();
#endif
	}

	// Main loop: a cooperative scheduler replaces the hand-rolled tick diffs.
	// Registration order == run order within a poll. period 0 == every pass.
	// Event-driven DMA: fire a callback from the ISR on each USART6 TX complete
	// instead of only polling usart6_tx.ready() in the snake task.
	usart6_tx.on_complete([] { usart6_tx_count.fetch_add(1, std::memory_order_relaxed); });

	CanHandle<CAN1>::on_rx([](const CanFrame& f) {
		can_last_rx = f;
		can_rx_count.fetch_add(1, std::memory_order_relaxed);
	});

	spi4_rx.on_complete([] { spi_dma_count.fetch_add(1, std::memory_order_relaxed); });

	sched::Scheduler<11> scheduler;
	scheduler.every(0,                       task_poll_io);    // ETH RX + inputs
	scheduler.every(0,                       task_adc_drain);  // drain ADC SPSC queue
	scheduler.every(TCP_TIMER_INTERVAL_MS,   task_net_timers);
	scheduler.every(SNAKE_TIMER_INTERVAL_MS, task_snake);
	scheduler.every(CAN_SEND_INTERVAL_MS,    task_can_send);
	scheduler.every(SPI_INTERVAL_MS,         task_spi);
	scheduler.every(RTC_READ_INTERVAL_MS,    task_rtc_read);
	scheduler.every(WDG_KICK_INTERVAL_MS,    task_watchdog);
	scheduler.every(WDG_KICK_INTERVAL_MS,    task_fault_test);
	scheduler.every(RTC_READ_INTERVAL_MS,    task_dns_demo);
	scheduler.every(0,                       task_mqtt);

	// Last, so the slow init above (LSE start, ETH, EEPROM) cannot trip it.
	// From here on a hang anywhere in the loop resets the MCU within ~1 s.
	if (IWDG_Start<IwdgConfig{ .TimeoutMs = WDG_TIMEOUT_MS }>() != DrvStatus::Ok) {
		Error_Handler();
	}
	scheduler.start(get_tick());

	while (true) {
		scheduler.poll(get_tick());
	}
}

namespace {
void dma_init() {

	constexpr const DmaHwConfig cfg_dma1_s0 = { .Channel = DmaChannel::CHNL_1,
			.Direction = DmaDir::PERIPH_TO_MEM, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::MEDIUM, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

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

	constexpr const DmaHwConfig cfg_dma1_s7 = { .Channel = DmaChannel::CHNL_1,
			.Direction = DmaDir::MEM_TO_PERIPH, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::CIRCULAR,
			.Priority = DmaPrior::MEDIUM, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	constexpr const DmaHwConfig cfg_dma2_s1 = { .Channel = DmaChannel::CHNL_5,
			.Direction = DmaDir::PERIPH_TO_MEM, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::CIRCULAR,
			.Priority = DmaPrior::MEDIUM, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	constexpr const DmaHwConfig cfg_dma2_s2 = { .Channel = DmaChannel::CHNL_6,
			.Direction = DmaDir::MEM_TO_PERIPH, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::HALFWORD,
			.MemDataAlignment = DmaDataAlign::HALFWORD, .Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::VERY_HIGH, .FIFOModeEnable = true,
			.FIFOThreshold = DmaThrhold::FULL };

	// USART6 TX: one burst per task_snake call. CIRCULAR here kept the line
	// 100% busy (the burst repeated back to back) and starved polled writes.
	constexpr const DmaHwConfig cfg_dma2_s7 = { .Channel = DmaChannel::CHNL_5,
			.Direction = DmaDir::MEM_TO_PERIPH, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::NORMAL,
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

	constexpr const DmaHwConfig cfg_spi4_rx = { .Channel = DmaChannel::CHNL_4,
			.Direction = DmaDir::PERIPH_TO_MEM, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::HIGH };

	constexpr const DmaHwConfig cfg_spi4_tx = { .Channel = DmaChannel::CHNL_5,
			.Direction = DmaDir::MEM_TO_PERIPH, .PeriphIncEnable = false,
			.MemIncEnable = true, .PeriphDataAlignment = DmaDataAlign::BYTE,
			.MemDataAlignment = DmaDataAlign::BYTE, .Mode = DmaMode::NORMAL,
			.Priority = DmaPrior::MEDIUM };

	DMA_Config<spi4_rx, cfg_spi4_rx>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream0_IRQn, 1>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream0_IRQn>();

	DMA_Config<spi4_tx, cfg_spi4_tx>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream4_IRQn, 1>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream4_IRQn>();

	DMA_Config<usart6_tx, cfg_dma2_s7>();
	NVIC_API::SetPriority<IRQn_Type::DMA2_Stream7_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::DMA2_Stream7_IRQn>();
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

void can_init()
{
	// 500 kbit/s from PCLK1 = 54 MHz (SYSCLK 216 MHz / 4), solved at compile time
	constexpr const CanHwConfig cfg = {
		.Timing = can_bit_timing(54'000'000, 500'000),
		.Mode = CanMode::Loopback,
		.Interrupts = CanIrq::Rx0Pending | CanIrq::Rx0Overrun,
	};

	if (CAN_Init<CAN1, cfg>() != DrvStatus::Ok) {
		Error_Handler();
	}
	CAN_FilterConfig<CAN1, CanFilter::accept_all(0, CanFifo::Fifo0)>();

	NVIC_API::SetPriority<IRQn_Type::CAN1_RX0_IRQn, 2>();
	NVIC_API::EnableIRQ<IRQn_Type::CAN1_RX0_IRQn>();
}

void spi_init()
{
	constexpr const SpiHwConfig cfg = {
		.MaxClockHz = 1'000'000,  // APB2 108 MHz / 128 = 843.75 kHz
		.Mode = SpiMode::Mode0,
	};

	if (SPI_Init<SPI4, cfg>() != DrvStatus::Ok) {
		Error_Handler();
	}
}

void rtc_init()
{
	// LSE crystal first; fall back to LSI (less accurate) if it does not start
	rtc_source = RtcClock::Lse;
	if (RTC_Init<RtcConfig{ .Source = RtcClock::Lse }>() != DrvStatus::Ok) {
		rtc_source = RtcClock::Lsi;
		if (RTC_Init<RtcConfig{ .Source = RtcClock::Lsi }>() != DrvStatus::Ok) {
			Error_Handler();
		}
	}
	// The RTC keeps UTC (NTP sets it). Until the first sync, and after the
	// backup domain lost power, it must at least not be older than the
	// firmware: TLS rejects certificates "valid from the future" otherwise.
#ifdef MCORE_BUILD_UNIX
	constexpr int64_t build_unix = MCORE_BUILD_UNIX;              // UTC, from CMake
#else
	constexpr int64_t build_unix = rtc_unix(rtc_build_time());    // local time: may be ahead of UTC
#endif
	if (!RTC_IsTimeSet() || RTC_UnixTime() < build_unix) {
		RTC_SetUnixTime(build_unix);
	}

	RTC_WakeupStart(1, [] { rtc_wakeups.fetch_add(1, std::memory_order_relaxed); });
	NVIC_API::SetPriority<IRQn_Type::RTC_WKUP_IRQn, 3>();
	NVIC_API::EnableIRQ<IRQn_Type::RTC_WKUP_IRQn>();
}

void i2c_init() {
	constexpr const I2CHwConfig cfg = { 
			.Timing = 0x20404768, // Timing value for 400kHz I2C speed with 54MHz PCLK1
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
			.ClockSource =UartClockSrc::SYSCLK,
			.UseDMARx = true,
			.UseDMATx = true };
	status = USART_Init<USART6, cfg2>();
	if (status != DrvStatus::Ok) Error_Handler();
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
	constexpr GPIO_Config eth_cfg    { .mode=Mode::Alt, .otype=OType::PP, .speed=Speed::VeryHigh, .pull=Pull::None,   .af=AF::AF11 }; // Ethernet
	constexpr GPIO_Config in_cfg     { .mode=Mode::In,  .otype=OType::PP, .speed=Speed::Medium,   .pull=Pull::None,   .af=AF::AF0  };
	constexpr GPIO_Config out_cfg    { .mode=Mode::Out, .otype=OType::PP, .speed=Speed::VeryHigh, .pull=Pull::None,   .af=AF::AF0  };
	constexpr GPIO_Config rs485_cfg  { .mode=Mode::Alt, .otype=OType::PP, .speed=Speed::VeryHigh, .pull=Pull::PullUp, .af=AF::AF7  }; // USART2
	constexpr GPIO_Config can1_cfg   { .mode=Mode::Alt, .otype=OType::PP, .speed=Speed::VeryHigh, .pull=Pull::PullUp, .af=AF::AF9  };  // RX idles recessive without a transceiver
	constexpr GPIO_Config usart6_cfg { .mode=Mode::Alt, .otype=OType::PP, .speed=Speed::VeryHigh, .pull=Pull::PullUp, .af=AF::AF8  };  // RX idles high when nothing is connected
	constexpr GPIO_Config tim1_cfg   { .mode=Mode::Alt, .otype=OType::PP, .speed=Speed::VeryHigh, .pull=Pull::None,   .af=AF::AF1  };
	constexpr GPIO_Config tim4_cfg   { .mode=Mode::Alt, .otype=OType::PP, .speed=Speed::VeryHigh, .pull=Pull::None,   .af=AF::AF2  };
	constexpr GPIO_Config adc3_cfg   { .mode=Mode::Ang, .otype=OType::PP, .speed=Speed::VeryHigh, .pull=Pull::None,   .af=AF::AF0  };
	constexpr GPIO_Config spi4_cfg   { .mode=Mode::Alt, .otype=OType::PP, .speed=Speed::VeryHigh, .pull=Pull::PullUp, .af=AF::AF5  };  // MISO idles high when nothing drives it
	constexpr GPIO_Config i2c1_cfg  { .mode=Mode::Alt, .otype=OType::OD, .speed=Speed::VeryHigh, .pull=Pull::None,   .af=AF::AF4  };

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
	spi4::group::configure<spi4_cfg>();
}
}

extern "C" void ADC_IRQHandler() {
	if (ADC3::_SR::EOC::is_set()) {  // reading DR below clears EOC
		const AdcSample s{ 
			.channel=adc_idx, 
			.value=static_cast<uint16_t>(ADC3::_DR::DATA::read()) };

		adc_queue.push(s);           // ISR never blocks; drop if the loop fell behind
		adc_idx = (adc_idx + 1) & 1;
	}
	if (ADC3::_SR::OVR::is_set()) {
		ADC3::_SR::OVR::clear();
	}
}

extern "C" void DMA1_Stream0_IRQHandler() {DMA_IRQHandler<i2c1_rx>();}
extern "C" void DMA1_Stream1_IRQHandler() {}
extern "C" void DMA1_Stream2_IRQHandler() {}
extern "C" void DMA1_Stream3_IRQHandler() {}
extern "C" void DMA1_Stream4_IRQHandler() {}
extern "C" void DMA1_Stream5_IRQHandler() {DMA_IRQHandler<usart2_rx>();}
extern "C" void DMA1_Stream6_IRQHandler() {DMA_IRQHandler<usart2_tx>();}
extern "C" void DMA1_Stream7_IRQHandler() {DMA_IRQHandler<i2c1_tx>();}


extern "C" void DMA2_Stream0_IRQHandler() {DMA_IRQHandler<spi4_rx>();}
extern "C" void DMA2_Stream2_IRQHandler() {DMA_IRQHandler<tim1_ch2>();}
extern "C" void DMA2_Stream3_IRQHandler() {DMA_IRQHandler<tim1_ch1>();}
extern "C" void DMA2_Stream1_IRQHandler() {DMA_IRQHandler<usart6_rx>();}
extern "C" void DMA2_Stream4_IRQHandler() {DMA_IRQHandler<spi4_tx>();}
extern "C" void DMA2_Stream5_IRQHandler() {}
extern "C" void DMA2_Stream6_IRQHandler() {}
extern "C" void DMA2_Stream7_IRQHandler() {DMA_IRQHandler<usart6_tx>();}


extern "C" void CAN1_RX0_IRQHandler() { CAN_RxIRQHandler<CAN1, CanFifo::Fifo0>(); }

extern "C" void RTC_WKUP_IRQHandler() { RTC_WakeupIRQHandler(); }

extern "C" void TIM1_CC_IRQHandler() {

}

void Error_Handler() {
	// Error occurred - enter infinite loop
	// TODO: Add error logging/indication (LED, UART, etc.)
	while (true) {
		__NOP();  // Prevent compiler optimization
	}
}
