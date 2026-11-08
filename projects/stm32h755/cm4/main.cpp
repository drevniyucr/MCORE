/*
 * STM32H755 Cortex-M4 application: network core.
 *
 * Owns Ethernet and everything above it:
 *   - IP stack: DHCP (link-local fallback), TCP server on port 5001
 *   - SNTP wall clock
 *   - MQTT 3.1.1 over TLS 1.2 to the broker (mqtt_link)
 *   - VDA 5050 3.0.0 mobile-robot client (vda_agent)
 * Talks to the Cortex-M7 real-time application (cm7/main.cpp) through
 * shared/ipc_shared.hpp:
 *   - motion/action commands from VDA 5050 orders, node/action events back,
 *   - reads the M7 state snapshot (VDA 5050 state, diagnostics on TCP 5001),
 *   - sends a Ping command every second and measures the round trip.
 *
 * The M7 owns power, clocks and GPIO configuration (Ethernet pins included).
 * This core waits for its go-ahead before touching anything clock-dependent.
 */
#include <cstdio>
#include <cstring>

#include "core/nvic.hpp"
#include "core/system.hpp"
#include "drivers/eth.hpp"
#include "net/dhcp.hpp"
#include "net/net.hpp"
#include "net/sntp.hpp"
#include "net/tcp.hpp"
#include "net/tls/gcm_hw.hpp"
#include "chip/hsem.hpp"
#include "ipc_shared.hpp"
#include "mqtt_link.hpp"
#include "net_config.hpp"
#include "prof.hpp"
#include "sched/sched.hpp"
#include "tls_selftest.hpp"
#include "vda_agent.hpp"

void Error_Handler();

namespace {
constexpr uint16_t TCP_LISTEN_PORT = 5001;
constexpr uint32_t TCP_TIMER_INTERVAL_MS = 100;
constexpr uint32_t REPORT_INTERVAL_MS = 500;
// Not a multiple of the M7 loop's timers, so pings sample every phase of
// the M7 loop: min RTT = IPC cost, max RTT = longest M7 loop stall.
constexpr uint32_t PING_INTERVAL_MS = 997;
constexpr uint32_t ETH_RETRY_INTERVAL_MS = 1000;
constexpr uint32_t STACK_PAINT = 0xA5A5A5A5U;

// Diagnostics, read with the debugger (symbol cm4_stats).
struct Stats {
	uint32_t eth_init_tries;
	uint32_t eth_last_status;     // ETH_Status of the last failed attempt
	uint32_t pings_sent;
	uint32_t pings_dropped;       // command ring full
	uint32_t pongs;
	uint32_t last_rtt_cycles;     // M4 DWT cycles, Ping push -> Pong pop
	uint32_t min_rtt_cycles;
	uint32_t max_rtt_cycles;
	uint32_t doorbells;           // HSEM interrupts from the M7
	uint32_t state_reads_failed;
	uint32_t tcp_bytes_rx;
	uint32_t stack_free_min;      // bytes never touched in the 32 KB below the stack top
};
} // namespace

// Not static so that they survive -O and are easy to find in a debugger.
Stats cm4_stats;
ipc_app::RtState cm4_rt_state;

extern "C" uint32_t _ebss[];    // linker: end of .bss (heap/stack area follows)
extern "C" uint32_t _estack[];   // linker: initial stack pointer (top of RAM_D2)

namespace {
bool net_up = false;
uint32_t ping_seq = 0;

// --- stack high-water mark ----------------------------------------------------
// Scanned window: the 8 KB reserve and a generous margin below the top of
// the stack; scanning all free RAM took ~3 ms every 100 ms
constexpr uintptr_t STACK_WINDOW = 32U * 1024U;
uint32_t *stack_floor() {
	const uintptr_t lo = reinterpret_cast<uintptr_t>(_ebss) + 1024U;   // skip the heap reserve
	const uintptr_t top = reinterpret_cast<uintptr_t>(_estack);
	return reinterpret_cast<uint32_t *>(top - lo > STACK_WINDOW ? top - STACK_WINDOW : lo);
}
void stack_paint() {
	uint32_t *sp;
	__asm volatile("mov %0, sp" : "=r"(sp));
	for (uint32_t *p = stack_floor(); p < sp - 32; ++p) {
		*p = STACK_PAINT;
	}
}
uint32_t stack_free() {
	const uint32_t *p = stack_floor();
	while (*p == STACK_PAINT) {
		++p;
	}
	return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p) - reinterpret_cast<uintptr_t>(stack_floor()));
}

// --- ART accelerator ----------------------------------------------------------
// The Cortex-M4 has no cache of its own and fetches code from flash bank 2
// across the D2-D1 interconnect with wait states. ART caches one 1 MB flash
// page for it: the page of this image (RM0399, ART accelerator).
void art_enable() {
	RCC::_AHB1ENR::ARTEN::set();
	(void) RCC::_AHB1ENR::read();
	const uint32_t page = 0x08100000UL >> 20;   // 1 MB page index of bank 2
	ART::_CTR::overwrite((page << ART::_CTR::PCACHEADDR::pos) | ART::_CTR::EN::BitMsk);
}

// --- network bring-up ------------------------------------------------------
void net_try_start() {
	cm4_stats.eth_init_tries += 1;
	const ETH_Status st = ETH_Init();
	if (st != ETH_Status::OK) {
		cm4_stats.eth_last_status = static_cast<uint32_t>(st);
		return;   // e.g. no cable yet: retried by the main loop
	}
	NET_TCP_Init();
	NET_TCP_Listen(TCP_LISTEN_PORT);
	// The static configuration is what DHCP falls back to
	NET_SetNetConfig(netcfg::STATIC_IP, netcfg::STATIC_MASK, netcfg::STATIC_GW);
	NET_DHCP_Start();
	NET_SNTP_Start(netcfg::SNTP_SERVER);
	net_up = true;
}

// --- M7 link ----------------------------------------------------------------
void send_ping() {
	ipc_app::Command cmd{};
	cmd.type = ipc_app::CmdType::Ping;
	cmd.seq = ++ping_seq;
	cmd.arg[0] = DWT::_CYCCNT::read();
	if (ipc_app::shared().commands.push(cmd)) {
		(void) hsem::notify(ipc_app::CMD_SEM);
		cm4_stats.pings_sent += 1;
	} else {
		cm4_stats.pings_dropped += 1;
	}
}

void drain_events() {
	ipc_app::Event ev;
	while (ipc_app::shared().events.pop(ev)) {
		switch (ev.type) {
		case ipc_app::EventType::Pong: {
			const uint32_t rtt = DWT::_CYCCNT::read() - ev.arg[0];
			cm4_stats.pongs += 1;
			cm4_stats.last_rtt_cycles = rtt;
			if (cm4_stats.min_rtt_cycles == 0U || rtt < cm4_stats.min_rtt_cycles) {
				cm4_stats.min_rtt_cycles = rtt;
			}
			if (rtt > cm4_stats.max_rtt_cycles) {
				cm4_stats.max_rtt_cycles = rtt;
			}
			break;
		}
		case ipc_app::EventType::NodeReached:
		case ipc_app::EventType::ActionDone:
			vda_agent::on_event(ev);
			break;
		default:
			break;
		}
	}
}

void read_snapshot() {
	if (ipc_app::shared().state.read(cm4_rt_state)) {
		vda_agent::update_robot(cm4_rt_state);
	} else {
		cm4_stats.state_reads_failed += 1;
	}
}

// --- reporting --------------------------------------------------------------
void send_to_tcp_clients(const char *line, int n) {
	if (n <= 0) {
		return;
	}
	for (tcp_conn_t &c : tcp_clients) {
		// Only our server port: the MQTT connection is a client socket too
		if (c.state == tcp_state_t::TCP_ESTABLISHED && c.server_port == TCP_LISTEN_PORT) {
			NET_TCP_SendUser(&c, reinterpret_cast<const uint8_t *>(line), static_cast<uint16_t>(n));
		}
	}
}

void report_state() {
	char line[200];
	const uint32_t mhz = SystemCoreClock / 1000000U;
	const int n = snprintf(line, sizeof(line),
		"m7_tick=%lu in=0x%08lx adc=%u,%u cmds=%lu ev_drop=%lu rtt_us=%lu mqtt=%u hs_ms=%lu resumed=%lu/%lu\r\n",
		static_cast<unsigned long>(cm4_rt_state.tick),
		static_cast<unsigned long>(cm4_rt_state.inputs),
		cm4_rt_state.adc[0], cm4_rt_state.adc[1],
		static_cast<unsigned long>(cm4_rt_state.cmds_handled),
		static_cast<unsigned long>(cm4_rt_state.events_dropped),
		static_cast<unsigned long>(mhz ? cm4_stats.last_rtt_cycles / mhz : 0U),
		static_cast<unsigned>(mqtt_link::status().phase),
		static_cast<unsigned long>(mqtt_link::status().handshake_ms),
		static_cast<unsigned long>(mqtt_link::status().resumed),
		static_cast<unsigned long>(mqtt_link::status().sessions));
	send_to_tcp_clients(line, n < static_cast<int>(sizeof(line)) ? n : static_cast<int>(sizeof(line)) - 1);
}

// Incoming TCP data on port 5001 is drained so the window stays open; the
// one command is "reconnect" (drops the MQTT link, a bench test of TLS session
// resumption). The MQTT socket is read by mqtt_link, not here.
void drain_tcp_rx() {
	uint8_t buf[64];
	for (tcp_conn_t &c : tcp_clients) {
		while (c.state == tcp_state_t::TCP_ESTABLISHED && c.server_port == TCP_LISTEN_PORT &&
		       NET_TCP_Available(&c) > 0U) {
			const int n = NET_TCP_Read(&c, buf, sizeof(buf));
			if (n <= 0) {
				break;
			}
			cm4_stats.tcp_bytes_rx += static_cast<uint32_t>(n);
			for (int i = 0; i + 9 <= n; ++i) {
				if (memcmp(&buf[i], "reconnect", 9) == 0) {
					mqtt_link::request_reconnect();
					break;
				}
			}
		}
	}
}
} // namespace

// Time events of the tasks (defined with the tasks below main)
extern sched::TimeEvent eth_tick, ping_tick, report_tick;
void install_net_lock();

int main() {
	stack_paint();
	SystemInit();               // VTOR, FPU, DWT
	hsem::wait_for_cm7();       // power, clocks, GPIO and shared memory are ready
	art_enable();               // instruction cache for this core's flash bank
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000U);

	if (!ipc_app::shared_ok()) {
		Error_Handler();        // images from different builds
	}

	// Doorbell from the M7 (HSEM2 is this core's HSEM interrupt)
	hsem::clear(hsem::mask(ipc_app::EVT_SEM));
	hsem::irq_enable(hsem::mask(ipc_app::EVT_SEM));
	NVIC_API::SetPriority<IRQn_Type::HSEM2_IRQn, 5>();
	NVIC_API::EnableIRQ<IRQn_Type::HSEM2_IRQn>();
	NVIC_API::EnableIRQGlobal();

	(void) gcm_hw::selftest();  // CRYP AES-GCM for TLS records if it matches software
	vda_agent::init();          // also starts mqtt_link
	run_tls_selftest();         // no-op unless built with MCORE_TLS_SELFTEST
	net_try_start();

	install_net_lock();
	eth_tick.arm(1, 1);         // eth wakes app after every pass
	ping_tick.arm(PING_INTERVAL_MS, PING_INTERVAL_MS);
	report_tick.arm(REPORT_INTERVAL_MS, REPORT_INTERVAL_MS);
	if (!sched::start()) {
		Error_Handler();
	}
	for (;;) {
		sched::idle();
	}
}

// --- tasks --------------------------------------------------------------------------
//   eth   prio 3  owns the IP stack: frames in (ARP, ICMP, DHCP, SNTP, TCP input
//                 and ACKs), TCP/DHCP/SNTP timers, port 5001. Woken by the ETH
//                 interrupt and every 1 ms. Short: never waits for the rest.
//   app   prio 2  MQTT/TLS, VDA 5050, M7 events and snapshot. Woken by eth
//                 after each of its passes (data may have arrived; at least
//                 every 1 ms) and by the M7 doorbell.
//                 Its calls into the TCP/SNTP API take the net lock (ceiling
//                 eth), so the stack is never entered twice.
//   diag  prio 1  ping to the M7 (command ring shared with app) and the TCP
//                 5001 report, under locks.
namespace {
enum : uint8_t { SIG_TICK = 0, SIG_ETH = 1, SIG_DOORBELL = 2, SIG_NET = 3, SIG_PING = 4, SIG_REPORT = 5 };
constexpr uint8_t PRIO_ETH = 3;
constexpr uint8_t PRIO_APP = 2;
constexpr uint8_t PRIO_DIAG = 1;

void raise_app(uint8_t sig);

void eth_handler(sched::Task &, const sched::Event &) {
	static uint32_t eth_retry_tick = get_tick();
	static uint32_t tcp_timer_tick = get_tick();
	const uint32_t now = get_tick();

	if (net_up) {
		{
			prof::Scope p(prof::EthRx);
			ETH_RxWorker();
			drain_tcp_rx();
		}
		if ((now - tcp_timer_tick) >= TCP_TIMER_INTERVAL_MS) {
			prof::Scope p(prof::NetTimers);
			tcp_timer_tick = now;
			NET_TCP_Timers();
			NET_DHCP_Poll();
			NET_SNTP_Poll();
			cm4_stats.stack_free_min = stack_free();
		}
	} else if ((now - eth_retry_tick) >= ETH_RETRY_INTERVAL_MS) {
		eth_retry_tick = now;
		net_try_start();
	}
	raise_app(SIG_NET);
}

void app_handler(sched::Task &self, const sched::Event &) {
	const uint32_t now = get_tick();
	{
		prof::Scope p(prof::Mqtt);
		mqtt_link::poll(net_up);
	}
	// Events first, then the snapshot (the M7 publishes it before the
	// events), then the VDA 5050 logic: the state message is consistent.
	{
		prof::Scope p(prof::Ipc);
		drain_events();
		read_snapshot();
	}
	{
		prof::Scope p(prof::VdaPoll);
		vda_agent::poll(now);
	}
	if (mqtt_link::cpu_work_pending()) {
		self.raise(SIG_TICK);   // next ECC slice right away, not in 1 ms
	}
}

void diag_handler(sched::Task &, const sched::Event &e) {
	if (e.sig == SIG_PING) {
		sched::Lock lock(PRIO_APP);   // command ring: app is the other producer
		send_ping();
	} else if (e.sig == SIG_REPORT && net_up) {
		sched::Lock lock(PRIO_ETH);   // TCP stack and app data
		report_state();
	}
}

uint32_t net_lock() {
	return sched::lock(PRIO_ETH);
}
} // namespace

void install_net_lock() {
	NET_SetLock(net_lock, sched::unlock);
}

// Otherwise unused vectors (different from the M7 ones: peripheral lines
// reach both cores)
sched::TaskQ<1> eth_task("eth", PRIO_ETH, IRQn_Type::DFSDM1_FLT0_IRQn::irq_number, eth_handler);
sched::TaskQ<1> app_task("app", PRIO_APP, IRQn_Type::JPEG_IRQn::irq_number, app_handler);
sched::TaskQ<1> diag_task("diag", PRIO_DIAG, IRQn_Type::MDIOS_IRQn::irq_number, diag_handler);
MCORE_SCHED_VECTOR(DFSDM1_FLT0_IRQHandler, eth_task)
MCORE_SCHED_VECTOR(JPEG_IRQHandler, app_task)
MCORE_SCHED_VECTOR(MDIOS_IRQHandler, diag_task)
sched::TimeEvent eth_tick(eth_task, SIG_TICK);
sched::TimeEvent ping_tick(diag_task, SIG_PING);
sched::TimeEvent report_tick(diag_task, SIG_REPORT);

namespace {
void raise_app(uint8_t sig) {
	app_task.raise(sig);
}
} // namespace

extern "C" void ETH_EventHook() {
	eth_task.raise(SIG_ETH);
}

extern "C" void HSEM2_IRQHandler() {
	const uint32_t m = hsem::pending_masked();
	hsem::clear(m);
	cm4_stats.doorbells += 1;
	app_task.raise(SIG_DOORBELL);   // events from the M7
}

void Error_Handler() {
	while (true) {
		__NOP();
	}
}
