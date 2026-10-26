/*
 * mqtt_demo.cpp — MQTT 3.1.1 (net/mqtt.c) over NetClient, see mqtt_demo.hpp
 */
#include "mqtt_demo.hpp"

#include <cstdio>
#include <cstring>

#include "core/system.hpp"
#include "net/mqtt.h"
#include "net/tls.hpp"

#if __has_include("tls_bench_creds.hpp")
#include "tls_bench_creds.hpp"   // generated, git-ignored: holds a private key
#define MQTT_DEMO_TLS 1
#else
#define MQTT_DEMO_TLS 0
#endif

MqttDemoStatus mqtt_demo = {};

namespace {

constexpr char CLIENT_ID[] = "mcore-01";
constexpr char TOPIC_STATUS[] = "mcore/mcore-01/status";
constexpr char TOPIC_TELEMETRY[] = "mcore/mcore-01/telemetry";
constexpr char TOPIC_CMD[] = "mcore/mcore-01/cmd";
constexpr char TOPIC_REPLY[] = "mcore/mcore-01/reply";
constexpr uint16_t KEEP_ALIVE_S = 30;
constexpr uint32_t RETRY_MS = 5000;
constexpr uint32_t TELEMETRY_MS = 1000;

#if MQTT_DEMO_TLS
const TlsCredentials creds = {
	BENCH_CA_DER, sizeof(BENCH_CA_DER),
	BENCH_CLIENT_CERT_DER, sizeof(BENCH_CLIENT_CERT_DER),
	BENCH_CLIENT_KEY_DER, sizeof(BENCH_CLIENT_KEY_DER),
	BENCH_BROKER_HOST,
};
const NetClientConfig link_cfg = { BENCH_BROKER_HOST, 8883, &creds };
#else
const NetClientConfig link_cfg = { "192.168.99.10", 1883, nullptr };
#endif

NetClientBuf<> link;
sh_mqtt mqtt;
uint8_t mqtt_rx[1024];
uint32_t retry_at = 0;
uint32_t last_telemetry = 0;
bool announced = false;

bool send(void *, const uint8_t *data, size_t n) {
	return link.write(data, n);
}

bool publish(const char *topic, const char *text, bool retain = false) {
	const bool ok = sh_mqtt_publish(&mqtt, topic, text, strlen(text), 0, retain, get_tick());
	if (ok) mqtt_demo.published++;
	return ok;
}

void on_message(void *, const char *topic, const char *payload, size_t len) {
	mqtt_demo.received++;
	if (strcmp(topic, TOPIC_CMD) != 0) return;
	snprintf(mqtt_demo.last_cmd, sizeof(mqtt_demo.last_cmd), "%.*s", static_cast<int>(len), payload);
	if (len == 4 && memcmp(payload, "ping", 4) == 0) {
		publish(TOPIC_REPLY, "pong");
	}
}

void drop(const char *why) {
	snprintf(mqtt_demo.error, sizeof(mqtt_demo.error), "%s", why);
	link.close();
	sh_mqtt_reset(&mqtt);
	announced = false;
	retry_at = get_tick() + RETRY_MS;
}

} // namespace

void mqtt_demo_poll(const MqttTelemetry &t) {
	const uint32_t now = get_tick();
	static bool init = false;
	if (!init) {
		init = true;
		sh_mqtt_init(&mqtt, mqtt_rx, sizeof(mqtt_rx), send, nullptr, on_message, nullptr);
		mqtt_demo.tls = MQTT_DEMO_TLS;
	}

	link.poll();
	mqtt_demo.link = link.state();
	mqtt_demo.tls_heap_peak = static_cast<uint32_t>(TLS_HeapPeak());

	if (link.failed()) {
		drop(link.error());
	}
	if ((link.state() == netclient_state_t::IDLE) && static_cast<int32_t>(now - retry_at) >= 0) {
		link.open(link_cfg);
		return;
	}
	if (!link.up()) return;

	if (mqtt.state == SH_MQTT_DOWN) {
		mqtt_demo.handshake_ms = link.handshake_ms();
		const sh_mqtt_connect_opts o = {
			CLIENT_ID, nullptr, nullptr, KEEP_ALIVE_S,
			TOPIC_STATUS, reinterpret_cast<const uint8_t*>("offline"), 7, 1, true,
		};
		if (!sh_mqtt_start(&mqtt, &o, now)) { drop("MQTT CONNECT"); return; }
	}

	uint8_t buf[256];
	size_t n;
	while ((n = link.read(buf, sizeof(buf))) > 0) {
		if (!sh_mqtt_feed(&mqtt, buf, n, now)) {
			char why[48];
			snprintf(why, sizeof(why), "MQTT refused or protocol error (CONNACK %u)", mqtt.connack_code);
			drop(why);
			return;
		}
	}
	if (link.failed()) { drop(link.error()); return; }
	if (!sh_mqtt_poll(&mqtt, now)) { drop("MQTT keep-alive / CONNACK timeout"); return; }
	mqtt_demo.mqtt_up = sh_mqtt_up(&mqtt);
	if (!mqtt_demo.mqtt_up) return;

	if (!announced) {
		announced = publish(TOPIC_STATUS, "online", true) && sh_mqtt_subscribe(&mqtt, TOPIC_CMD, 1, now);
		if (announced) {
			mqtt_demo.connects++;
			mqtt_demo.error[0] = '\0';
		}
	}
	if (now - last_telemetry >= TELEMETRY_MS) {
		last_telemetry = now;
		char json[192];
		snprintf(json, sizeof(json),
		         "{\"uptime_ms\":%lu,\"unix_time\":%lu,\"adc\":[%u,%u],\"can_rx\":%lu,\"ntp_syncs\":%lu,\"tls_heap_peak\":%lu}",
		         // newlib-nano printf has no %lld; unsigned 32-bit seconds last until 2106
		         static_cast<unsigned long>(t.uptime_ms), static_cast<unsigned long>(t.unix_time),
		         t.adc[0], t.adc[1], static_cast<unsigned long>(t.can_rx),
		         static_cast<unsigned long>(t.ntp_syncs),
		         static_cast<unsigned long>(mqtt_demo.tls_heap_peak));
		publish(TOPIC_TELEMETRY, json);
	}
}
