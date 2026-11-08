#pragma once
/*
 * Protocol between the two STM32H755 cores (MCORE_DUAL_CORE builds).
 *
 *   Cortex-M7  real time: I/O, ADC, timers, UARTs, motion (later)
 *   Cortex-M4  network:   Ethernet, TCP/IP, DHCP, MQTT/VDA 5050 (later)
 *
 * Channels, all in SRAM4 (0x38000000, non-cacheable on the M7):
 *   state     M7 -> M4  latest RtState, seqlock, written every ms
 *   events    M7 -> M4  must-not-lose events, ring + HSEM doorbell
 *   commands  M4 -> M7  ring + HSEM doorbell (also polled every 1 ms)
 *
 * Rule: the real-time core never waits for the network core. If a ring is
 * full the M7 drops the message and counts it.
 *
 * Both firmwares are built from this header, so the layout matches as long
 * as both images come from the same build. The M4 still checks magic,
 * version and size before touching anything (see shared_ok()).
 */
#include <atomic>
#include <cstdint>

#include "ipc/seqlock.hpp"
#include "ipc/spsc_ring.hpp"

namespace ipc_app {

constexpr uint32_t MAGIC   = 0x4D434F52U;  // "MCOR"
constexpr uint32_t VERSION = 3U;            // bump on any layout change

// HSEM ids (0 is the boot handshake, see chip/hsem.hpp)
constexpr uint32_t EVT_SEM = 1U;   // M7 -> M4: events were pushed
constexpr uint32_t CMD_SEM = 2U;   // M4 -> M7: commands were pushed

// ---------------------------------------------------------------------------
// M7 -> M4 state snapshot
// ---------------------------------------------------------------------------
struct RtState {
	uint32_t tick;             // M7 millisecond tick
	uint32_t inputs;           // bit i = digital input i
	uint16_t adc[2];           // ADC3 PF8, PF9 raw counts
	uint32_t cmds_handled;     // commands taken from the ring
	uint32_t events_dropped;   // events lost because the ring was full
	// Motion (vehicle model on the M7; project coordinates, metres/radians)
	float x, y, theta;
	float vx, omega;           // vehicle frame
	uint8_t driving;
	uint8_t paused;
	uint8_t loaded;
	uint8_t queued_targets;
	float battery_soc;         // percent
	uint32_t last_popped;      // path_index of the last waypoint reached
	uint32_t stop_ack;         // ack_seq of the last StopMotion processed
	uint8_t any_popped;
	uint8_t held;
	uint8_t pad[2];
	float speed_limit;         // active cap [m/s], 0 = none
};

// ---------------------------------------------------------------------------
// M4 -> M7 commands
// ---------------------------------------------------------------------------
enum class CmdType : uint16_t {
	None = 0,
	Ping = 1,         // arg[0] = sender timestamp, echoed back in Event::Pong
	DriveTo = 2,      // drive: append a waypoint to the motion queue
	StopMotion = 3,   // flag = ack_seq: stop as soon as possible, clear the queue
	SetPaused = 4,    // flag: 1 = pause motion and actions, 0 = resume (VDA pause)
	SetPose = 5,      // pose: (re)initialise the position
	ActionStart = 6,  // action
	ActionCancel = 7, // action.uid
	SetHold = 8,      // flag: 1 = stop and keep the queue (zones, blocking actions)
	SetSpeedLimit = 9,// speed: cap in m/s, 0 = none
};

struct DrivePayload {
	uint32_t path_index;   // reported back as RtState::last_popped
	uint32_t node_seq;     // reported back in Event::NodeReached (if is_node)
	float x, y, theta;
	float max_speed;       // 0 = vehicle default
	float tolerance;       // pass-through radius [m]
	uint8_t is_node;
	uint8_t has_theta;     // stop here and turn to theta
	uint8_t pad[2];
};

struct PosePayload {
	float x, y, theta;
};

enum class ActionCode : uint8_t { Pick = 1, Drop = 2 };

struct ActionPayload {
	uint8_t uid;           // reported back in Event::ActionDone
	ActionCode code;
	uint8_t pad[2];
	uint32_t duration_ms;
};

struct Command {
	CmdType  type;
	uint16_t reserved;
	uint32_t seq;
	union {
		uint32_t arg[8];
		DrivePayload drive;
		PosePayload pose;
		ActionPayload action;
		uint32_t flag;
		float speed;
	};
};

// ---------------------------------------------------------------------------
// M7 -> M4 events
// ---------------------------------------------------------------------------
enum class EventType : uint16_t {
	None = 0,
	Pong = 1,         // seq/arg[0] copied from the Ping, arg[1] = M7 tick
	NodeReached = 2,  // arg[0] = node_seq of the DriveTo target
	ActionDone = 3,   // arg[0] = uid, arg[1] = 1 success / 0 failure
};

struct Event {
	EventType type;
	uint16_t  reserved;
	uint32_t  seq;
	uint32_t  arg[6];
};

// ---------------------------------------------------------------------------
// The shared area
// ---------------------------------------------------------------------------
struct SharedArea {
	std::atomic<uint32_t> magic;   // written last by the M7
	uint32_t version;
	uint32_t size;
	ipc::Seqlock<RtState> state;
	ipc::SpscRing<Event, 64> events;
	ipc::SpscRing<Command, 64> commands;
};
static_assert(sizeof(SharedArea) <= 64U * 1024U, "SharedArea must fit in SRAM4");
static_assert(sizeof(DrivePayload) <= sizeof(Command::arg), "payload too large");

// Raw storage at the start of SRAM4 in both images (section .ipc_shared,
// NOLOAD, asserted at 0x38000000 by both linker scripts). Deliberately NOT a
// SharedArea object: that would get a C++ start-up constructor, and the M4,
// which may boot after the M7, would wipe the area the M7 just set up.
alignas(SharedArea) inline uint8_t g_shared_storage[sizeof(SharedArea)]
	__attribute__((section(".ipc_shared")));

inline SharedArea &shared() {
	return *reinterpret_cast<SharedArea *>(g_shared_storage);
}

// M7: call before releasing the M4.
inline void init_shared() {
	SharedArea &s = shared();
	s.magic.store(0U, std::memory_order_relaxed);
	s.version = VERSION;
	s.size = sizeof(SharedArea);
	s.state.reset();
	s.events.reset();
	s.commands.reset();
	s.magic.store(MAGIC, std::memory_order_release);
}

// M4: true if the M7 initialised a compatible layout.
inline bool shared_ok() {
	const SharedArea &s = shared();
	return s.magic.load(std::memory_order_acquire) == MAGIC
	    && s.version == VERSION
	    && s.size == sizeof(SharedArea);
}

} // namespace ipc_app
