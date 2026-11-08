#pragma once
/*
 * mqtt_link — keeps one MQTT-over-TLS session to the broker alive.
 *
 *   WaitNetwork -> WaitTime (SNTP) -> TcpConnect -> TlsHandshake
 *     -> MqttConnect -> Online
 *   any failure -> Backoff (1, 2, 4 ... 30 s) -> TcpConnect
 *
 * Everything is non-blocking except the public-key steps of the TLS
 * handshake (about a second on the Cortex-M4, see tls_client.hpp).
 */
#include <cstddef>
#include <cstdint>

#include "net/mqtt.hpp"

namespace mqtt_link {

enum class Phase : uint8_t {
	WaitNetwork,
	WaitTime,
	TcpConnect,
	TlsHandshake,
	MqttConnect,
	Online,
	Backoff,
	Disabled      // TLS setup failed (bad CA PEM, RNG): see tls_setup_error
};

struct Status {
	Phase phase;
	uint32_t attempts;           // TCP connection attempts
	uint32_t sessions;           // times we reached Online
	int32_t tls_setup_error;     // Mbed TLS error from setup (0 = ok)
	int32_t last_tls_error;      // last handshake / record error
	uint32_t last_verify_flags;  // certificate check result of the last handshake
	uint8_t last_mqtt_error;     // mqtt::Error of the last failed session
	uint8_t last_connack;        // CONNACK return code (5 = not authorised)
	uint8_t last_fail_phase;     // Phase in which the last attempt failed
	uint32_t handshake_ms;       // duration of the last successful handshake
	uint32_t tls_heap_peak;      // Mbed TLS arena high-water mark (bytes)
	uint32_t backoff_ms;
	uint32_t resumed;            // sessions that resumed a TLS session (ticket)
};

// What the application adds to a session
struct Hooks {
	// Called right before CONNECT: the will message (must stay valid until
	// the next call). VDA 5050: connection topic, CONNECTION_BROKEN, retained.
	void (*prepare_will)(void *ctx, const char **topic, const uint8_t **payload, uint16_t *len);
	// Called once per session after CONNACK: subscribe, publish birth messages.
	void (*on_online)(void *ctx);
	mqtt::MessageHandler on_message;
	void *ctx;
};

// Brings up the RNG, the TLS arena and credentials, and the MQTT client.
void init(const Hooks &hooks);

// Call every main-loop pass. net_up: Ethernet and the IP stack are running.
void poll(bool net_up);

bool online();

// Drop the connection and connect again (bench test of session resumption).
// Safe from any task: the link task acts on it in poll().
void request_reconnect();

// The TLS handshake has CPU work pending (an ECC slice): call poll() again
// soon instead of waiting for network data.
bool cpu_work_pending();
bool publish(const char *topic, const uint8_t *payload, size_t len, uint8_t qos, bool retain);
bool subscribe(const char *topic, uint8_t qos);
size_t tx_free();   // bytes the next publish may use

// Graceful end (VDA 5050 shutdown): DISCONNECT, close TLS/TCP, stay offline
// until reset.
void shutdown();

const Status &status();
const mqtt::Stats &mqtt_stats();

} // namespace mqtt_link
