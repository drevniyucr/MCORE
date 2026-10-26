/*
 * mqtt_demo.hpp — MQTT telemetry over NetClient (TLS when bench credentials
 * exist, see bench/tls/make_bench_certs.py; plain TCP otherwise)
 *
 * Topics (client id mcore-01):
 *   mcore/mcore-01/status     "online" (retained) / "offline" (last will)
 *   mcore/mcore-01/telemetry  JSON once a second
 *   mcore/mcore-01/cmd        subscribed; "ping" -> "pong" on .../reply
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "net/netclient.hpp"

struct MqttTelemetry {
	uint32_t uptime_ms;
	int64_t unix_time;
	uint16_t adc[2];
	uint32_t can_rx;
};

// Debugger view of the demo
struct MqttDemoStatus {
	netclient_state_t link;
	bool mqtt_up;
	bool tls;
	uint32_t connects;        // MQTT sessions established
	uint32_t published;
	uint32_t received;
	uint32_t handshake_ms;    // last TLS handshake
	uint32_t tls_heap_peak;
	char last_cmd[32];
	char error[96];
};

extern MqttDemoStatus mqtt_demo;

void mqtt_demo_poll(const MqttTelemetry &t);
