# MCORE

Bare-metal C++20 framework for **STM32F767** and the dual-core **STM32H755** (Cortex-M7 + Cortex-M4) written from scratch — **no HAL, no LL, no CMSIS device headers**. Every peripheral is driven through a type-safe, zero-cost register abstraction generated directly from the vendor SVD file.

> Educational pet project: the goal is to understand the MCU from the register level up, not to compete with production frameworks.

## Key ideas

### 1. Type-safe register layer (`mcore/core/utils.hpp`)

All hardware access goes through `Register<Address, Access, RegT>` / `Field<Reg, Pos, Width>` templates:

- **Access control at compile time** — `ReadOnly` / `WriteOnly` / `ReadWrite` tags enforced by C++20 concepts: writing to a read-only register simply does not compile.
- **Atomic read-modify-write** via `LDREX`/`STREX` (optional per call: `atomic_mode` / `non_atomic_mode`).
- **Zero cost** — everything folds into the same `ldr`/`str` instructions you would write by hand.

```cpp
// Field-level access, self-documenting:
SCB::_DEMCR::TRCENA::set();                  // enable trace block
uint32_t src = RCC::_CFGR::SWS::read();      // current sysclock source
SysTick::_STCSR::bitSet<SysTick::_STCSR::CLKSOURCE,
                        SysTick::_STCSR::TICKINT,
                        SysTick::_STCSR::ENABLE>();
```

### 2. Code generation from SVD (`codegen/`)

The register layer is **not written by hand**. The Cortex-M7 core blocks (SCB, NVIC, MPU, cache, FPU, DWT/ITM, SysTick) live in `mcore/core/cortex_m7_regs.hpp`, shared by both chips. The vendor peripherals are generated per chip into `mcore/chip/<chip>/regs.hpp` from the SVD file; peripherals that are copies of another (`GPIOB` from `GPIOA`, `USART2` from `USART1`, ...) become one template plus aliases (`using GPIOB = GPIO<0x58020400>;`). `codegen/nvic/nvic_gen.py` generates a typed IRQ map (`IRQn_Type::USART2_IRQn::Priority / Enable / ...`) from the CMSIS device header.

Vendor SVD files contain errors, so every register header is cross-checked
against the vendor CMSIS device header, an independent source (`check_regs.py`,
CTest `regs_stm32h755` / `regs_stm32f767`): base addresses, register offsets and
the position/width of every bit field whose name exists on both sides (about
6 800 fields on the H755). The errors found are corrected while generating
(`codegen/registers/fixups/<chip>.json`, see its README); differences that are
CMSIS mistakes are listed with the reason. Examples of SVD errors fixed:
ADC `DR.RDATA` 16 instead of 32 bits, BDMA channels numbered 1-7 instead of
0-7, `ETH_MACARPAR` at 0xAE0 instead of 0x210, ADC `SMPR2` fields bound to
`SMPR1` and shifted (F767), DFSDM filter 3 registers at wrong offsets (F767).

### 3. Type-safe pins (`mcore/drivers/gpio.hpp`)

```cpp
using LD2 = Pin<GPIOB, 7>;
LD2::set();

// Groups expand at compile time via fold expressions:
using group = PinGroup<IN1, IN2, IN3 /* ... */>;
group::read_all(states);
```

Board pin maps (`inputs`, `outputs`, `eth`, `rs485`, `i2c1`, ...) are plain declarative structs in `app/board.hpp`.

### 4. Drivers (header-only templates)

| Module | Status |
|---|---|
| RCC (PLL, 216 MHz F7 / 400 MHz H7, bus prescalers) | working |
| GPIO / NVIC / MPU / SysTick | working |
| DMA (`DmaHandle<DmaStream::...>`) | working |
| USART + DMA RX/TX | working |
| I2C (tested with 47L16 EEPROM) | working |
| ADC | working |
| TIM (PWM, input capture, DMA requests) | working |
| WS2812 LED strip over TIM+DMA | working |
| Ethernet MAC + PHY (RMII) | working |
| CAN (bxCAN, F7 only; H7 has FDCAN) | in progress |
| SPI | planned |

### 5. Custom network stack (`mcore/net/`)

Hand-written ARP / ICMP / UDP / **TCP** on top of the own ETH DMA driver. No lwIP.

The TCP server supports: listening ports (`NET_TCP_Listen`, multiple clients
per port), MSS negotiation, a receive ring buffer with a socket-style read API
(`NET_TCP_Read` / `NET_TCP_Available`, window reopens as data is consumed),
sliding-window transmit (up to 3 segments in flight, MSS-sized segmentation,
per-segment retransmission), keepalive probing, in-sequence RST validation,
active/passive close with TIME_WAIT, up to 10 simultaneous connections.

**DHCP client** (RFC 2131): DISCOVER/OFFER/REQUEST/ACK with exponential
backoff, lease renewal at T1 (unicast) and rebinding at T2 (broadcast),
restart on expiry or NAK. If no server answers, the device falls back to
its static IP and keeps retrying in the background. On address change the
frame templates are re-patched and live TCP connections are reset.

**Client side** (used by the STM32H755 network core):

- ARP cache with rate-limited requests, subnet mask and default gateway
  (`NET_ARP_Resolve`, `NET_SetNetConfig`); DHCP applies mask/router.
- UDP sockets: `NET_UDP_Bind` (handler per port), `NET_UDP_SendTo`.
- TCP active open: `NET_TCP_Connect` (ephemeral ports, SYN retransmission),
  `NET_TCP_Abort`, `NET_TCP_IsOpen`.
- **SNTP** client (`net/sntp.*`): originate-timestamp check, half-RTT
  correction and frequency discipline. The tick's rate error is measured
  between syncs and corrected (the NUCLEO-H755ZI-Q runs from the ST-LINK MCO,
  measured 0.63 % fast); the poll interval grows from 64 s to 1 h.
- **MQTT 3.1.1** client (`net/mqtt.*`): non-blocking and allocation-free,
  QoS 0/1 both ways, will, credentials, keep-alive; any byte transport.
- **TLS 1.2** client (`net/tls/`) on Mbed TLS 3.6.7 LTS
  (`third_party/mbedtls`, configuration `mbedtls_config_mcore.h`): ECDHE-ECDSA
  / ECDHE-RSA with AES-GCM, certificate and host-name/IP verification,
  optional client certificate, static memory arena, entropy from the STM32H7
  RNG, wall clock from SNTP.

### 6. VDA 5050 3.0.0 mobile robot (`mcore/vda5050/`)

The STM32H755 network core is a VDA 5050 3.0.0 mobile-robot client, checked
against the official JSON schemas (`third_party/vda5050/schemas`):

- Topics `vda5050/v3/<manufacturer>/<serial>/<topic>`: `order`,
  `instantActions`, `zoneSet` and `responses` in; `state`, `connection`
  (QoS 1, retained, will `CONNECTION_BROKEN`), `factsheet` and
  `visualization` (4 Hz while driving, every 5 s otherwise) out.
- Orders (6.1): base/horizon, order updates stitched at the decision point,
  every rejection case of 6.1.4 (VALIDATION_FAILURE, OUTDATED_ORDER_UPDATE,
  SAME_ORDER_UPDATE_ID, OTHER_ORDER_ACTIVE, START_NODE_OUT_OF_RANGE,
  UNKNOWN_MAP_ID, INVALID_ORDER_ACTION, UNSUPPORTED_PARAMETER,
  ORDER_UPDATE_FOLLOWING_CANCEL, ...).
- Actions (6.2): node and edge actions with blocking types
  NONE/SOFT/SINGLE/HARD (Figure 11); order actions `pick`, `drop`,
  `waitForTrigger`, `finePositioning`, `startCharging`, `stopCharging`;
  instant actions `cancelOrder`, `startPause`, `stopPause`, `stateRequest`,
  `factsheetRequest`, `initializePosition`, `clearInstantActions`, `trigger`,
  `startHibernation`/`stopHibernation` (optional `wakeUpTime`), `shutdown`,
  `downloadMap`/`enableMap`/`deleteMap`, `enableZoneSet`/`deleteZoneSet`,
  `clearZoneActions`.
- Geometry: edges are straight lines or NURBS `trajectory` (any degree,
  weights, clamped knot vector), sampled into waypoints every 0.1 m that the
  M4 streams to the M7 16 ahead of the robot. Every streamed segment is
  checked against zones and obstacles with the robot contour (0.5 m disc).
- Zones (6.4): BLOCKED (stop in front, NODE_UNREACHABLE; BLOCKED_ZONE_VIOLATION
  when inside), RELEASE (ACCESS request, QUEUED/GRANTED/REVOKED/REJECTED,
  `leaseExpiry`, `releaseLossBehavior`), SPEED_LIMIT (applied before the
  contour enters), ACTION (entry/during/exit actions in `zoneActionStates`).
  LINE_GUIDED/PRIORITY/PENALTY are met trivially; COORDINATED_REPLANNING and
  DIRECTED/BIDIRECTED are accepted and ignored.
- Corridors (6.5): `leftWidth`/`rightWidth`, `corridorReferencePoint`,
  `releaseRequired` (CORRIDOR edge request before the first deviation),
  `releaseLossBehavior` STOP/RETURN. The robot has no sensors, so obstacles
  are simulated with the vendor instant actions `mcoreSimObstacle`
  (x, y, radius) and `mcoreSimClearObstacles`; the path bends around them
  inside the corridor or the robot waits.
- Maps (6.3): `maps` in the state, downloads are simulated (3 s, the link is
  not fetched), DUPLICATE_MAP, orders on disabled maps are rejected.
- Hibernation and shutdown (6.10): the final state goes out first, then
  `connection` HIBERNATING (no state while asleep) or OFFLINE followed by a
  graceful MQTT disconnect.
- State (6.6): nodeStates/edgeStates/actionStates, errors with references,
  published on every relevant event (100 ms minimum spacing), every second
  while driving and every 10 s otherwise.
- Allocation-free: a pull JSON parser reads orders straight from the MQTT
  buffer (up to 32 nodes, 32 actions, 16 KB); the writer needs no printf.
- Simplifications: the 3.0.0 `requestStatus` enum has no QUEUED/REJECTED, so
  QUEUED is reported as REQUESTED and a REJECTED request disappears (a new one
  follows after 10 s); map files are never downloaded.

The protocol logic talks to the vehicle through `vda::RobotPort`. On the
STM32H755 that is the Cortex-M7: the M4 sends numeric commands (drive to a
node, stop, pause, set pose, start/cancel an action) and gets "node reached"
/ "action done" events. Until real drives exist, the M7 runs a kinematic
vehicle model (`app/motion_sim.*`: differential drive, 0.5 m/s, pick/drop as
3 s timers) in its 1 ms loop; only that file has to change for real motor
control. IPC v3 adds waypoint streaming (`path_index`, `is_node`), a
hold flag, a speed limit and acknowledged stops (`StopMotion` with a sequence
number echoed in `stop_ack`) for replanning.

### 7. Preemptive task scheduler (`mcore/sched/`)

A priority-based, run-to-completion scheduler on one stack (in the spirit of
Super Simple Tasker): a task is a handler with an event queue, posting an
event makes it run, a higher priority preempts a lower one at once. The NVIC
does the scheduling: each task owns an otherwise unused interrupt vector whose
priority is the task priority, so preemption, tail-chaining and the return to
the preempted task cost what an interrupt costs.

```cpp
enum : uint8_t { SIG_TICK, SIG_CMD };
void control(sched::Task &, const sched::Event &e) { /* never blocks */ }
sched::TaskQ<4> control_task("control", 3, IRQn_Type::SPDIF_RX_IRQn::irq_number, control);
MCORE_SCHED_VECTOR(SPDIF_RX_IRQHandler, control_task)
sched::TimeEvent tick(control_task, SIG_TICK);

tick.arm(1, 1);            // every millisecond
sched::start();            // NVIC set up, SysTick above all tasks
for (;;) sched::idle();
// from anywhere, interrupts included: control_task.post(sig, arg) / .raise(SIG_CMD)
```

- Priorities 1..8 map to NVIC levels 15..8; SysTick runs at level 7 so time
  keeps going under long tasks; levels 0..6 stay free for peripheral interrupts.
- `post()` queues events with a payload (overflow is counted), `raise()` sets
  one of 32 coalescing signals that is never lost (interrupts, time events).
- `sched::Lock(ceiling)` is a priority-ceiling mutex (BASEPRI): tasks up to the
  ceiling are held off, everything above keeps running.
- Per task statistics: runs, events, dropped, queue high-water mark, longest
  activation in cycles; global CPU load (`sched::stats().busy_cycles`).
- Host port with an NVIC model for unit tests (`tests/test_sched.cpp`).

On the STM32H755 the Cortex-M7 runs `control` (prio 3, 1 kHz: inputs, M4
commands, vehicle model, state snapshot; woken at once by the M4 doorbell) and
`leds` (prio 1: LED strip, running light). The Cortex-M4 runs:

| Task | Prio | Work | Worst case (measured) |
|---|---|---|---|
| `eth` | 3 | owns the IP stack: frames in (ARP, ICMP, DHCP, SNTP, TCP input/ACKs), TCP/DHCP/SNTP timers; woken by the ETH interrupt and 1 ms | 0.3 ms, also during a TLS handshake |
| `app` | 2 | MQTT/TLS, VDA 5050, M7 events/snapshot; woken by `eth` and the M7 doorbell | 2.6 ms |
| `diag` | 1 | ping to the M7, TCP 5001 report | 0.05 ms |

`app` calls into the stack through the TCP/SNTP API, which takes the optional
net lock (`NET_SetLock`, here a `sched::lock(eth)` ceiling): the stack is
never entered twice and keeps answering while `app` encrypts or parses.
Phase timings of the M4 (DWT) are in `prof::stats` (`app_cm4/prof.hpp`).
M7: control worst case 11 us at 0.7 % CPU; M4 at 4 % CPU; ping round trip
M4 -> M7 -> M4 8-115 us.

Network core performance notes (measured, see `prof::stats`):

- **ART accelerator**: the Cortex-M4 has no cache and runs from flash bank 2
  across the D2-D1 interconnect. Enabling ART for its 1 MB page made the
  whole core 2.5-3x faster (TLS handshake 1.7 s -> 0.65 s).
- **Restartable ECC** (`MBEDTLS_ECP_RESTARTABLE`, 200 operations per step):
  the handshake runs in ~80 steps instead of one 1.8 s call. Only P-256/P-384
  are offered: brokers pick X25519 when offered and it cannot be split.
- **AES tables in RAM** instead of flash; JSON writer without per-character
  calls and 64-bit divisions (state JSON 3.9 ms -> 0.8 ms).
- **AES-GCM on the CRYP** (`mcore/net/tls/gcm_hw.cpp`, `MBEDTLS_GCM_ALT`):
  TLS records are encrypted/decrypted by the H755 crypto accelerator
  (10 KB state 4.2 ms -> 0.65 ms). Mbed TLS's software GCM is compiled next to
  it (`gcm_soft.c`): a boot self-test compares both on 144 vectors (key sizes,
  partial blocks, forged tags) and the hardware is used only if all match;
  `gcm_hw::stats` counts records per path.
- **TLS session resumption** (session tickets): a reconnect to a running
  broker takes 1-2 ms instead of 0.65 s. `reconnect` on TCP 5001 drops the
  link for bench tests; the 5001 report shows `hs_ms` and `resumed=n/sessions`.

### 8. Debug & profiling

- `print()` over **ITM/SWO** (`mcore/core/system.cpp`)
- Cycle-exact profiling via **DWT** counter
- OpenOCD configs (`debug/`) + VS Code launch/tasks

## Supported chips

The chip is picked at configure time with `-DMCORE_CHIP=...`; the firmware
target is named after it. Everything above the register layer (drivers'
public API, network stack, application) is shared.

| | STM32F767ZI (default) | STM32H755ZI (dual core) |
|---|---|---|
| Board | NUCLEO-F767ZI | NUCLEO-H755ZI-Q |
| Firmware images | `STM32F767`: everything | `STM32H755`: Cortex-M7, real time (`app/`); `STM32H755_CM4`: Cortex-M4, network (`app_cm4/`) |
| Clocks | 216 MHz CPU/AHB, APB1 54, APB2 108 | M7 400 MHz, M4 and AXI/AHB 200 MHz, APBx 100 |
| RAM used | SRAM1 (`0x20000000`) | M7: AXI SRAM (`0x24000000`); M4: SRAM1/2 (`0x30000000`), ETH pool in SRAM3 (`0x30040000`); shared: SRAM4 (`0x38000000`) |
| DMA routing | stream + channel (CHSEL) | stream + DMAMUX1 request |
| Ethernet | F7 MAC, enhanced descriptors | QoS MAC, RDES1 status translated to the F7 layout |
| ADC | 12-bit ADC3 (PF8 = IN6, PF9 = IN7) | 16-bit ADC3 (PF8 = INP7, PF9 = INP2) |
| CAN | bxCAN (in progress) | FDCAN — no driver yet |

STM32H755 notes:

- **Power supply.** `MCORE_PWR_SUPPLY` in `app/config.hpp` must match the
  board (NUCLEO-H755ZI-Q default: direct SMPS). `PWR_CR3` is write-once after
  power-on; a wrong value leaves the chip stuck and needs BOOT0=1 + mass erase.
- **Two images, two flash banks.** The M7 image goes to bank 1
  (`0x08000000`), the M4 image to bank 2 (`0x08100000`). Always flash both
  from the same build: the M4 checks the shared-memory layout at boot and
  stops in `Error_Handler` if it does not match.
- **Boot order.** The M7 sets up power, clocks and all GPIO (Ethernet pins
  included), initialises the shared area, then releases the M4 through HSEM
  semaphore 0 and `RCC_GCR.BOOT_C2`. This works with the option byte BCM4
  set (factory default: the M4 starts at reset and waits) or cleared.
- **Inter-core channels** (`app/ipc_shared.hpp`, primitives in `mcore/ipc/`):
  a seqlock state snapshot M7 -> M4, an event ring M7 -> M4 with an HSEM
  doorbell, and a command ring M4 -> M7 that the M7 polls. The M7 never
  waits on the M4. Measured on hardware: about 5 us for a command plus its
  reply event when the M7 loop is not blocked.
- **Network core (M4).** `app_cm4/` brings up Ethernet (retrying until the
  link is up), DHCP with a static fallback, SNTP, and keeps an MQTT-over-TLS
  session to the broker (`mqtt_link`: back-off 1-30 s). On top of it runs the
  VDA 5050 client (`vda_agent`, see above). Broker, credentials, addresses
  and the VDA 5050 identity: `app_cm4/net_config.hpp`;
  trusted CA: `app_cm4/certs/broker_ca.pem` (the test CA by default; CMake
  warns until it is replaced).
- **DMA-able memory.** DMA1/DMA2 and the Ethernet DMA cannot access DTCM, so
  `.data/.bss` are linked into AXI SRAM.
- **USART6** on PG14/PC7 is AF7 on the H7 (AF8 on the F7).

## Project layout

```
mcore/               framework (include root: #include "drivers/gpio.hpp")
├── mcore.hpp        umbrella header
├── core/            cortex_m7_regs.hpp, utils.hpp, def.hpp, system, nvic,
│                    mpu; regs/rcc/nvic_def.hpp dispatch to the chip
├── chip/
│   ├── stm32f767/   regs, nvic_def, rcc, clock, adc, can, eth
│   └── stm32h755/   regs, nvic_def, rcc, clock, adc, eth, dma_request,
│                    hsem (semaphores + dual-core boot)
├── drivers/         gpio, dma, tim, usart, i2c, ws2812, eth.hpp
│                    (+ adc/can dispatchers)
├── ipc/             spsc_ring, seqlock: lock-free inter-core primitives
├── sched/           preemptive run-to-completion scheduler (NVIC-based)
├── net/             net, arp, tcp, dhcp, sntp, mqtt, eth_utils, temp_frame
│   └── tls/         TLS client on Mbed TLS: config, platform hooks, TCP BIO
└── vda5050/         VDA 5050 3.0.0: json, order_manager, messages
app/                 application (F767; Cortex-M7 on the H755): main.cpp,
                     board.hpp (pin maps), config.hpp (HSE/MAC/PHY),
                     ipc_shared.hpp (M7 <-> M4 protocol), motion_sim
                     (vehicle model on the M7), syscalls.c
app_cm4/             STM32H755 Cortex-M4 application: network, MQTT/TLS link,
                     vda_agent, net_config.hpp, certs/broker_ca.pem
third_party/mbedtls/ Mbed TLS 3.6.7 (include/, library/ only, unmodified)
third_party/vda5050/ VDA 5050 3.0.0 JSON schemas (tests only, MIT)
platform/            stm32f767/, stm32h755/: startup + linker scripts
codegen/
├── registers/       SVD → mcore/chip/<chip>/regs.hpp generator (Python)
└── nvic/            IRQ map generator (Python)
debug/               openocd/ configs, MCORE_Debug_F767.cfg,
                     MCORE_Debug_H755.cfg, SVD files for the debugger
tests/               host-side unit tests: network stack, ipc, MQTT, TLS
                     (doctest); live/ broker check; broker/ test broker
CMakeLists.txt       cross-build (arm-none-eabi)
```

## Portability layers

The framework separates generic code from device- and board-specific code:

- **Generic (MCU-agnostic):** `core/utils.hpp` (`Register`/`Field`),
  `atomic.hpp`, `scheduler.hpp`, `ring_buffer.hpp`, `profile.hpp`, the driver
  *templates*, and the network stack. These depend only on register/peripheral
  symbols, never on a specific part number.
- **Device seam (`core/device.hpp`):** the single place that binds the framework
  to a concrete register map. Defaults to STM32F767 (`core/regs.hpp`). Porting to
  another MCU = add an `#elif` branch for its generated map and build with
  `-DMCORE_DEVICE_<part>`; no other framework file changes.
- **Config contract (`core/mcore_config.hpp`):** declares the oscillator/clock
  values the framework consumes (`HSE_VALUE`, `HSI_VALUE`, `LSE_VALUE`,
  `LSI_VALUE`) with defaults. It pulls the application's `config.hpp` if present
  (board MAC/PHY, custom crystal, overrides) and fills in the rest — so the
  framework also compiles standalone (e.g. host unit tests, where `app/` is not
  on the include path).
- **Board/app:** `app/board.hpp` (pin maps) and `app/config.hpp` (HSE/MAC/PHY).

## Build

Requirements: `arm-none-eabi-gcc` (C++20), CMake ≥ 3.21, Ninja.

```sh
# STM32F767 (default)
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target STM32F767

# STM32H755: builds STM32H755 (Cortex-M7) and STM32H755_CM4 (Cortex-M4)
cmake -G Ninja -B build-h7 -DCMAKE_BUILD_TYPE=Debug -DMCORE_CHIP=STM32H755
cmake --build build-h7
```

Toolchain discovery: pass `-DSTM32_TOOLCHAIN_PATH=...` or set the `STM32_TOOLCHAIN_PATH` environment variable. Output: `<build dir>/<chip>` (ELF) + `.bin`, size report is printed post-build.

Flash/debug: OpenOCD ≥ 0.12 (`debug/MCORE_Debug_F767.cfg`, `debug/MCORE_Debug_H755.cfg`) or the provided VS Code tasks / launch configurations.

```sh
openocd -s debug/openocd -f debug/MCORE_Debug_H755.cfg \
        -c "program build-h7/STM32H755.bin 0x08000000 verify" \
        -c "program build-h7/STM32H755_CM4.bin 0x08100000 verify reset exit"
```

`MCORE_Debug_H755.cfg` exposes both cores: GDB port 3333 is the Cortex-M7,
3334 the Cortex-M4 (VS Code: "STM32H755 (CM4): attach").

## Testing

The network stack (checksums, RX dispatch, ARP/ICMP/UDP handlers, the full
TCP state machine) is unit-tested **on the host** — no board needed. The real
stack sources are compiled with the native compiler; the ETH driver, tick
counter and ISN entropy are replaced by test stubs (`tests/stubs.cpp`).

```sh
cmake -S tests -B build-tests -G Ninja
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
```

Covered scenarios include the TCP three-way handshake, in-order/out-of-order
data, passive close, RST handling, retransmission with controlled time,
keepalive probing and connection-table exhaustion, the TCP active open, ARP
resolution/routing, UDP sockets, SNTP (including drift correction), the MQTT
codec and session logic, the VDA 5050 layer (JSON, order acceptance and
rejection, base/horizon updates, blocking types, instant actions). The
`vda_schemas` test validates the messages it generates against the official
schemas. `tls_tests` runs our TLS client against an Mbed TLS
server in memory: ECDSA and RSA brokers, IP-literal names, wrong name,
untrusted CA, missing wall clock, mutual TLS, partial writes
(certificates: `tests/certs`, unit-test only). The inter-core ring and
seqlock are tested with a producer and a consumer on two host threads.
Tests also run in CI on every push.

### MQTT broker for bench tests

`tests/broker/` has a test PKI generator, a mosquitto configuration
(TLS on 8883 with user `agv001` / `mcore-test`, plain 1883 on localhost) and
a small SNTP server:

```sh
tests/broker/gen_pki.sh            # test CA + broker cert (SAN 127.0.0.1,
                                   # 192.168.0.10); copies the CA to app_cm4/certs
tests/broker/run_broker.sh         # mosquitto (scoop install mosquitto)
python tests/broker/sntp_server.py 192.168.0.10 123
build-tests/mqtt_live              # our MQTT+TLS client vs the broker (host)
python tests/broker/vda_master.py  # VDA 5050 end-to-end run against the board
python tests/broker/vda_master_ext.py [--shutdown]   # zones, corridors, maps, ...
```

`vda_master.py` acts as a minimal fleet control: it initialises the position,
sends an order with a horizon and a HARD pick, extends it at the decision
point, pauses and resumes the robot, cancels a second order and checks the
rejection of an update after the cancel. Every message the robot publishes is
validated against the official schemas (needs `pip install jsonschema`).
`vda_master_ext.py` covers the rest: a zone set with SPEED_LIMIT, RELEASE
(answered QUEUED, then GRANTED), ACTION and BLOCKED zones, a NURBS edge, a
corridor with a simulated obstacle and `releaseRequired`, map download /
enable / delete, hibernation (woken by `wakeUpTime` and by `stopHibernation`)
and optionally `shutdown` (reset the board afterwards).

The firmware defaults expect the board wired to a PC adapter with
192.168.0.10/24; the board falls back to 192.168.0.123 without DHCP. The PC
firewall must allow inbound TCP 8883 and UDP 123 on that adapter.

On-target TLS self-test: configure with `-DMCORE_TLS_SELFTEST=ON`; the M4
runs one in-memory handshake at boot and stores timing/RAM in `tls_selftest`.

Measured on the NUCLEO-H755ZI-Q (Cortex-M4 at 200 MHz):

| | |
|---|---|
| TLS handshake with mosquitto (ECDHE-ECDSA P-256, AES-256-GCM) | 0.65 s, in ECC slices of at most 32 ms; resumed: 1-2 ms |
| Mbed TLS arena peak, client | 26.4 KB (arena 36 KB) |
| Firmware size, M4 image (with VDA 5050) | 158 KB flash, 142 KB of 256 KB RAM incl. 8 KB stack (4 KB used) |
| Wall clock after drift correction | within ~20 ms of the PC |

## Regenerating the register layer

> **Note:** `mcore/core/regs.hpp` is the maintained source of truth. The register
> generator was used to bootstrap it and has since drifted (its raw output uses
> `namespace`s instead of peripheral structs, lacks the host-build guards, and
> misses hand-applied access-mode fixes). Its output is git-ignored and must not
> be copied over `regs.hpp` as-is — refresh the generator first if you intend to
> regenerate. Edit `mcore/core/regs.hpp` directly for now.

```sh
cd codegen/registers
# STM32H755: SVD → mcore/chip/stm32h755/regs.hpp (fixups: SVD errors, F7-style aliases)
python svd2chip.py --svd xml-svd/STM32H755_CM7.svd --chip STM32H755 \
                   --fixups fixups/STM32H755.json --out ../../mcore/chip/stm32h755/regs.hpp
# cross-check against the CMSIS header (also a CTest)
python check_regs.py --regs ../../mcore/chip/stm32h755/regs.hpp --cmsis ../nvic/stm32h755xx.h
# STM32F767 (legacy flow; the result was hand-tuned afterwards)
python xml-svd2reg.py      # STM32F767.svd + core XMLs → mcore_regs.hpp

cd ../nvic
python nvic_gen.py --header stm32h755xx.h --out ../../mcore/chip/stm32h755/nvic_def.hpp
python nvic_gen.py --header stm32f767xx.h --out ../../mcore/chip/stm32f767/nvic_def.hpp
```

Third-party inputs: `STM32H755_CM7.svd` from [modm-io/cmsis-svd-stm32](https://github.com/modm-io/cmsis-svd-stm32);
`stm32h755xx.h` and the startup file from [STMicroelectronics/cmsis_device_h7](https://github.com/STMicroelectronics/cmsis_device_h7);
the DMAMUX request list from [STMicroelectronics/stm32h7xx_hal_driver](https://github.com/STMicroelectronics/stm32h7xx_hal_driver) (`stm32h7xx_hal_dma.h`).

## Roadmap

- [x] CI (GitHub Actions: firmware build + host tests)
- [x] Host-side unit tests for the network stack (header parsing, checksums, TCP FSM)
- [ ] Finish CAN, add SPI, IWDG, RTC
- [x] DHCP client
- [x] ARP cache, gateway routing
- [ ] DNS
- [ ] TCP: out-of-order reassembly, zero-window probing, delayed ACK
- [ ] HardFault handler with register dump
- [x] Split framework / board / application layers
- [x] Make svd2reg a standalone tool (any STM32 from its SVD) — `svd2chip.py`
- [x] Port to STM32H755 (Cortex-M7 core)
- [x] STM32H755 dual core: network on the Cortex-M4, shared-memory IPC
- [ ] STM32H7: FDCAN driver
- [x] Cortex-M4: SNTP, MQTT 3.1.1 over TLS 1.2 (Mbed TLS), tested against mosquitto
- [x] VDA 5050 3.0.0 on the Cortex-M4: orders, actions, state, connection, factsheet
- [x] VDA 5050: zones, corridors, responses, visualization, maps, hibernation, NURBS
- [ ] Real drive control on the Cortex-M7 instead of `motion_sim`
- [x] Register headers cross-checked against CMSIS, SVD errors fixed
- [x] Preemptive priority scheduler (run-to-completion, NVIC-based) on both cores
