# MCORE

Bare-metal C++20 framework for **STM32F767** (Cortex-M7) written from scratch — **no HAL, no LL, no CMSIS device headers**. Every peripheral is driven through a type-safe, zero-cost register abstraction generated directly from the vendor SVD file.

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

`mcore/core/regs.hpp` (~19k lines) is **generated** by `codegen/registers/regsgen.py` from `STM32F767.svd`, ARM XML descriptions of the Cortex-M7 core blocks (SCB, NVIC, MPU, cache, FPU) and a small hand-written `patches.toml` for what those sources miss or get wrong; CI fails if the header drifts from them (see "Regenerating" below). `codegen/nvic/nvic_gen.py` generates a typed IRQ map (`IRQn_Type::USART2_IRQn::Priority / Enable / ...`).

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
| RCC (PLL, 216 MHz, bus prescalers) | working |
| GPIO / NVIC / MPU / SysTick | working |
| DMA (`DmaHandle<DmaStream::...>`) | working |
| USART + DMA RX/TX | working |
| I2C (tested with 47L16 EEPROM) | working |
| ADC | working |
| TIM (PWM, input capture, DMA requests) | working |
| WS2812 LED strip over TIM+DMA | working |
| Ethernet MAC + PHY (RMII) | working |
| CAN (bxCAN: compile-time bit timing, filters, 3 TX mailboxes, IRQ callbacks; tested in loopback) | working |
| SPI (master, 4..16-bit frames, polling + full-duplex DMA, RAII chip select) | working |
| IWDG (compile-time timeout, window mode, debug freeze) + reset-cause query | working |
| RTC (LSE/LSI, calendar kept across resets, 1 Hz wakeup IRQ, backup registers) | working |

### 5. Custom network stack (`mcore/net/`)

Hand-written ARP / ICMP / UDP / **TCP** on top of the own ETH DMA driver. No lwIP.

The TCP server supports: listening ports (`NET_TCP_Listen`, multiple clients
per port), MSS negotiation, a socket-style read API (`NET_TCP_Read` /
`NET_TCP_Available`, window reopens as data is consumed), stream transmit
(`NET_TCP_SendUser` queues bytes, segments go out as the peer's window allows,
retransmission from the oldest unacked byte, zero-window probing with backoff),
keepalive probing, in-sequence RST validation, active/passive close with
TIME_WAIT (our FIN follows the queued data).

**Memory, not a connection count, is the limit** (`net/tcp_buf`). A connection
is a ~96-byte control block (64 of them); its send and receive data live in
byte FIFOs built from one shared pool of 192 × 512-byte blocks (96 KB). An idle
connection holds no block; when the pool runs low the receive window closes and
unsent data waits — TCP flow control instead of errors. On top of that:

| Rule | Value | Why |
|---|---|---|
| Per-connection share | 16 KB RX + 16 KB TX | one busy peer cannot take the whole pool |
| Half-open (`SYN_RCVD`) | ≤ 8, the oldest is evicted | a SYN flood cannot fill the table |
| Reserve for our own connections | 4 control blocks + 8 KB of pool | incoming traffic cannot lock out MQTT/TLS |

Checked on the board: 70 incoming connections pushing data the application never
reads — 59 accepted (64 − 4 reserved − 1 MQTT), the rest refused with RST, the
pool filled to exactly 96 − 8 KB, while MQTT over TLS kept publishing every
second and answering commands.

**DHCP client** (RFC 2131): DISCOVER/OFFER/REQUEST/ACK with exponential
backoff, lease renewal at T1 (unicast) and rebinding at T2 (broadcast),
restart on expiry or NAK. If no server answers, the device falls back to
its static IP and keeps retrying in the background. On address change the
frame templates are re-patched and live TCP connections are reset. The lease's
mask, gateway and DNS server become the routing config (`NET_SetRoute`).

**ARP cache + routing** (`net/arp`): 8 entries with TTL, retries and negative
caching; destinations off our subnet go through the gateway. **DNS resolver**
(`net/dns`): A records over UDP, CNAME-aware, 4-name cache honouring the TTL.
Both are non-blocking with one shape — call from a periodic task until it
returns `OK` (`PENDING` = request on the wire, `UNREACHABLE` = no route /
no answer) — and need no packet queue:

```cpp
uint8_t ip[4];
if (NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::OK) { /* ... */ }
```

**UDP sockets** (`net/udp`): bind a port to a handler (called from the RX
path with source address and payload, UDP length checked), send to any
address with the same `OK / PENDING / UNREACHABLE` shape. Unbound ports get
ICMP port unreachable; IP fragments are dropped (no reassembly).

```cpp
NET_UDP_Bind(4001, [](void *, const uint8_t *ip, uint16_t port, const uint8_t *d, uint16_t n) {
    NET_UDP_SendTo(4001, ip, port, d, n);    // echo
});
```

**SNTP client** (`net/ntp`): sets the RTC to UTC from an NTP server and
resyncs hourly — server time + half the round trip, replies tied to the
request by a random origin timestamp, kiss-of-death and unsynchronised servers
rejected, exponential backoff (15 s .. 15 min), the 2036 era rule. The time
goes to a callback, so the net layer stays free of the RTC driver. Until the
first sync the RTC is at least the firmware's configure time (UTC).

**TCP client** (`NET_TCP_Connect`): active open from an ephemeral port,
SYN_SENT with MSS, SYN retransmission, refusal by RST, same API shape.

**NetClient** (`net/netclient`): one non-blocking state machine for
"host:port, optionally over TLS" — DNS → ARP → TCP connect → TLS handshake →
byte stream, with a TX queue that takes messages whole. **TLS 1.2** is
mbedTLS 3.6 (fetched at configure time, SHA-256 checked), configured as a
small client: ECDHE-ECDSA-AES128-GCM-SHA256 / P-256 only, broker certificate
always verified (dates against the RTC), optional client certificate,
entropy from the RNG peripheral. Restartable ECC slices the handshake math so
no superloop pass blocks (handshake ≈ 0.5 s in total, the 1 s watchdog stays
on). mbedTLS gets a 32 KB static arena — the only heap in MCORE (≈ 26 KB peak,
reconnects included); AES tables are `const` in flash rather than built in RAM.

**MQTT 3.1.1** (`net/mqtt.c`): the transport-agnostic, heap-free client from
the goz-shuttle firmware (C11, its own tests in `tests/c`), on top of NetClient.
`app/mqtt_demo.cpp` publishes telemetry once a second, keeps a retained
`online` status with an `offline` last will, answers `ping` on its command
topic and reconnects by itself.

### 6. Debug & profiling

- `print()` over **ITM/SWO** (`mcore/core/system.cpp`)
- Cycle-exact profiling via **DWT** counter
- **Fault capture** (`mcore/core/fault.cpp`): HardFault/MemManage/BusFault/UsageFault
  switch to a private stack, save the stacked registers + CFSR/HFSR/MMFAR/BFAR to
  a `.noinit` record, then halt on `BKPT` if a debugger is attached or reset.
  After the reboot `Fault_TakeLast()` + `Fault_Print()` report it
  (e.g. `pc=0800060c ... integer divide by zero`)
- **Stack guard**: the 9 KB MSP stack is the first thing in RAM (DTCM, linker
  script), its lowest 1 KB an MPU no-access region — an overflow is a precise
  MemManage fault reported as "stack overflow", never silent data corruption.
  Measured peak with TLS/MQTT running: ~2.4 KB. The Ethernet DMA area is kept
  out of the linker's RAM region and the newlib heap is bounded below it.
- **Randomness**: TCP ISNs, ephemeral ports, DNS ids, NTP nonces and DHCP xids
  come from the hardware RNG (`drivers/rng.hpp`), the same source as TLS.
- OpenOCD configs (`debug/`) + VS Code launch/tasks

## Project layout

```
mcore/               framework (include root: #include "drivers/gpio.hpp")
├── mcore.hpp        umbrella header
├── core/            regs.hpp (generated), utils.hpp, def.hpp,
│                    device.hpp (MCU seam), mcore_config.hpp (config contract),
│                    system, rcc, nvic, nvic_def, mpu, fault
├── drivers/         gpio, dma, tim, usart, i2c, spi, adc, can, iwdg, rtc, ws2812, eth
└── net/             net, arp, dns, ntp, dhcp, tcp, netclient, tls, mqtt, eth_utils
app/                 application: main.cpp, board.hpp (pin maps),
                     config.hpp (HSE/MAC/PHY), syscalls.c
platform/            startup_stm32f767zitx.s, linker scripts (.ld)
codegen/
├── registers/       regsgen.py: SVD + core XML + patches.toml → mcore/core/regs.hpp
└── nvic/            IRQ map generator (Python)
debug/               openocd/ configs, MCORE_Debug.cfg, STM32F767.svd
tests/               host-side unit tests for the network stack (doctest)
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
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target STM32F767
```

Toolchain discovery: pass `-DSTM32_TOOLCHAIN_PATH=...` or set the `STM32_TOOLCHAIN_PATH` environment variable. Output: `build/STM32F767` (ELF) + `.bin`, size report is printed post-build.

Flash/debug: OpenOCD (`debug/MCORE_Debug.cfg`) or the provided VS Code tasks.

The first configure downloads mbedTLS; offline, pass `-DMBEDTLS_DIR=<unpacked mbedtls-3.6.7>`.

### Using MCORE from another project

`mcore.cmake` turns the framework into a target for any arm-none-eabi CMake
project (an OBJECT library, so its interrupt handlers always override the weak
ones of the startup file):

```cmake
include(path/to/MCORE/mcore.cmake)
mcore_add_library(mcore)
add_executable(app.elf main.cpp)
target_link_libraries(app.elf PRIVATE mcore)
target_link_options(app.elf PRIVATE ${MCORE_LINK_OPTIONS} -specs=nano.specs -specs=nosys.specs)
```

The mbedTLS glue and the C MQTT client are left out (they need mbedTLS and
its config). A C11 application with its own platform layer has been brought
up this way: a superloop on `sched::Scheduler`, I/O through `PinGroup`
masks, step pulses on TIM2/3/5, RS-485 by interrupts, UDP sockets.

### MQTT over TLS bench

Board on the PC's Ethernet (PC = 192.168.99.10/24, board = 192.168.99.123,
its static fallback when no DHCP server answers):

```sh
python bench/tls/make_bench_certs.py     # once: test CA + broker + client certs (git-ignored)
cmake --build build --target STM32F767   # app/tls_bench_creds.hpp present -> TLS demo on 8883
mosquitto -c bench/mosquitto_tls.conf -v
mosquitto_sub -h 127.0.0.1 -t "mcore/#" -v
mosquitto_pub -h 127.0.0.1 -t mcore/mcore-01/cmd -m ping    # -> mcore/mcore-01/reply pong
```

Without the generated credentials the demo builds for plain MQTT on 1883.

NTP on the bench: `python bench/ntp_server.py` (UDP 12300 — on Windows port
123 belongs to the time service). On a network with Internet access point
`NET_NTP_Start` at `pool.ntp.org` / `NTP_PORT` instead.

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

Two executables: `net_tests` (doctest, C++) and `mqtt_tests` (the goz-shuttle
minitest suite of the shared MQTT client, C11).

Covered scenarios include the TCP three-way handshake, in-order/out-of-order
data, passive close, RST handling, retransmission with controlled time,
keepalive probing, the block pool (window closing, per-connection shares,
reserves for active opens), SYN-flood eviction, zero-window probing, the TCP active open
(SYN_SENT, refusal, SYN retransmission), ARP, DNS and SNTP. Tests also run in CI on
every push.

## Regenerating the register layer

`mcore/core/regs.hpp` is **generated — do not edit it by hand**. It is built by
`codegen/registers/regsgen.py` (Python ≥ 3.11, no packages needed) from:

1. `xml-svd/STM32F767.svd` — the vendor peripherals;
2. `xml-svd/*_registers_*.xml` — ARM `core_reg` descriptions of the Cortex-M7
   blocks (SCB, NVIC, MPU, cache...), which the SVD format leaves out on purpose;
3. `patches.toml` — hand-written corrections, each with its reason: blocks
   neither source has (DWT, ITM, DHCSR, DBGMCU freeze registers), fields the SVD
   splits into single bits (`RCC_PLLCFGR.PLLM0..5` → `PLLM`), typos (`O24CE`),
   wrong access types.

Peripherals with the same register layout (SVD `derivedFrom`, or simply
identical like GPIOA/B/D) become one `template <uint32_t BASE_ADDR>` plus a
`using` per instance; the rest are plain structs. The generator rejects
duplicate names and fields past bit 31.

```sh
python codegen/registers/regsgen.py           # regenerate mcore/core/regs.hpp
python codegen/registers/regsgen.py --diff    # semantic diff (registers/addresses/fields) vs the current file
python codegen/registers/regsgen.py --check   # CI: fail if regs.hpp is stale or hand-edited
python codegen/registers/regsgen.py --svd OTHER.svd --patches other.toml -o other_regs.hpp

cd codegen/nvic
python nvic_gen.py         # stm32f767xx.h → irqn_type_autogen.hpp
```

To fix a register: add an entry to `patches.toml`, run `regsgen.py --diff` to
see exactly what changes, then regenerate.

## Roadmap

- [x] CI (GitHub Actions: firmware build + host tests)
- [x] Host-side unit tests for the network stack (header parsing, checksums, TCP FSM)
- [x] CAN driver
- [x] SPI driver
- [x] IWDG, RTC
- [x] DHCP client
- [x] ARP cache, gateway routing, DNS resolver
- [x] TCP client (active open), NetClient, TLS 1.2 (mbedTLS), MQTT 3.1.1
- [x] SNTP client (RTC in UTC from the network)
- [x] TCP buffers from a shared block pool (no fixed connection limit), zero-window probing
- [ ] TCP: out-of-order reassembly, delayed ACK
- [x] HardFault handler with register dump (kept across the reset)
- [x] Split framework / board / application layers
- [x] Register header generated from SVD + core XML + patches, checked in CI
      (`--svd/--patches` for another STM32; core XML is Cortex-M7 only for now)
