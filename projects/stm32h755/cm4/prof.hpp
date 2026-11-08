#pragma once
/*
 * prof — worst-case duration of the network core's work phases (DWT cycles,
 * 200 MHz). Read with the debugger (symbol prof::stats) or over TCP 5001.
 * A phase's time includes interrupts and more urgent tasks that ran inside it.
 */
#include <cstdint>

#include "core/def.hpp"

namespace prof {

enum Phase : uint8_t {
	EthRx,          // ETH_RxWorker + TCP input + port 5001 drain
	NetTimers,      // TCP retransmit/keepalive, DHCP, SNTP
	Mqtt,           // mqtt_link::poll (TLS records, MQTT, incoming messages)
	Handshake,      // one TLS handshake step (an ECC slice)
	OnMessage,      // VDA 5050 message handling (order parse, ...)
	Ipc,            // M7 events + snapshot
	VdaPoll,        // vda_agent::poll total
	VdaLogic,       // OrderManager::poll (path, zones, requests)
	StateBuild,     // state JSON
	StatePublish,   // state into MQTT/TLS/TCP
	Visualization,  // visualization JSON + publish
	COUNT
};

struct Stats {
	uint32_t max[COUNT];
	uint32_t calls[COUNT];
};
extern Stats stats;
extern uint32_t handshake_worst_state;   // TLS state that started the longest step

inline uint32_t now() {
	return DWT::_CYCCNT::read();
}

class Scope {
public:
	explicit Scope(Phase p) : p_(p), t0_(now()) {}
	~Scope() {
		const uint32_t d = now() - t0_;
		stats.calls[p_] += 1;
		if (d > stats.max[p_]) {
			stats.max[p_] = d;
		}
	}
	Scope(const Scope &) = delete;
	Scope &operator=(const Scope &) = delete;

private:
	Phase p_;
	uint32_t t0_;
};

} // namespace prof
