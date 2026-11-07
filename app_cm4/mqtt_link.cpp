/*
 * mqtt_link.cpp — see mqtt_link.hpp.
 */
#include <cstring>

#include "mqtt_link.hpp"
#include "net_config.hpp"

#include "broker_ca.h"   // generated from app_cm4/certs/broker_ca.pem
#include "chip/stm32h755/rng.hpp"
#include "core/system.hpp"
#include "net/net.hpp"
#include "net/sntp.hpp"
#include "net/tcp.hpp"
#include "net/tls/tls_client.hpp"
#include "net/tls/tls_tcp.hpp"
#include "prof.hpp"

namespace mqtt_link {

namespace {

constexpr uint32_t TCP_CONNECT_TIMEOUT_MS = 10000;
constexpr uint32_t TLS_HANDSHAKE_TIMEOUT_MS = 20000;
constexpr uint32_t ARP_RETRY_MS = 250;
constexpr uint32_t BACKOFF_MIN_MS = 1000;
constexpr uint32_t BACKOFF_MAX_MS = 30000;
// ECC work per handshake step (Mbed TLS basic operations). Tuned on the
// 200 MHz Cortex-M4 for slices of a few milliseconds (handshake_slice_max).
constexpr unsigned ECC_SLICE_OPS = 200;   // P-256 minimum with window 4: 160 (ecp.h); a P-256 multiplication is ~3300

// Mbed TLS arena: the client peaks at ~29 KB on the host and 26.4 KB on the
// board (tls_heap_peak, ECDSA P-256 broker with a one-level chain) with a 16 KB
// receive record buffer. Check tls_heap_peak again for a broker with a longer
// or RSA certificate chain.
#if defined(MCORE_TLS_SELFTEST)
alignas(8) uint8_t tls_arena[80 * 1024];   // self-test runs client AND server
#else
alignas(8) uint8_t tls_arena[36 * 1024];
#endif
uint8_t mqtt_tx[12 * 1024];  // a VDA 5050 state with a long order is several KB
uint8_t mqtt_rx[16 * 1024];  // largest incoming message (VDA 5050 orders)

tls::Client tls_client;
tls::TcpBio bio;
mqtt::Client client;
Status st = {};
uint32_t phase_since = 0;
uint32_t next_try = 0;
Hooks hooks = {};
volatile bool reconnect_requested = false;

int mqtt_write(void *, const uint8_t *d, size_t n) { return tls_client.write(d, n); }
int mqtt_read(void *, uint8_t *b, size_t n) { return tls_client.read(b, n); }

void enter(Phase p) {
	st.phase = p;
	phase_since = get_tick();
}

void drop_connection(Phase failed_in) {
	st.last_fail_phase = static_cast<uint8_t>(failed_in);
	if (bio.open()) {
		NET_TCP_Abort(bio.conn);
	}
	bio = {};
	client.reset();
	st.backoff_ms = (st.backoff_ms == 0U) ? BACKOFF_MIN_MS
	              : (st.backoff_ms * 2U > BACKOFF_MAX_MS ? BACKOFF_MAX_MS : st.backoff_ms * 2U);
	next_try = get_tick() + st.backoff_ms;
	enter(Phase::Backoff);
}

bool timed_out(uint32_t limit_ms) {
	return (get_tick() - phase_since) > limit_ms;
}

void start_mqtt_session() {
	mqtt::ConnectOptions o;
	o.client_id = netcfg::CLIENT_ID;
	o.username = netcfg::USERNAME;
	o.password = netcfg::PASSWORD;
	o.keepalive_s = netcfg::KEEPALIVE_S;
	o.clean_session = true;
	if (hooks.prepare_will != nullptr) {
		hooks.prepare_will(hooks.ctx, &o.will_topic, &o.will_payload, &o.will_len);
		o.will_qos = 1;
		o.will_retain = true;
	}
	if (client.connect(o, get_tick())) {
		enter(Phase::MqttConnect);
	} else {
		drop_connection(Phase::MqttConnect);
	}
}

} // namespace

void init(const Hooks &h) {
	hooks = h;
	st = {};
	(void) rng::init();   // HSI48 is started by the Cortex-M7
	tls::platform_init(tls_arena, sizeof(tls_arena));
	tls::set_time_source(NET_SNTP_UnixMs);
	st.tls_setup_error = tls_client.setup({ BROKER_CA_PEM, nullptr, nullptr }, netcfg::BROKER_NAME);
	tls::set_ecc_slice(ECC_SLICE_OPS);
	client.init({ nullptr, mqtt_write, mqtt_read }, mqtt_tx, sizeof(mqtt_tx),
	            mqtt_rx, sizeof(mqtt_rx), hooks.on_message, hooks.ctx);
	enter(st.tls_setup_error == 0 ? Phase::WaitNetwork : Phase::Disabled);
}

void poll(bool net_up) {
	const uint32_t now = get_tick();
	if (st.phase == Phase::Disabled) {
		return;
	}
	if (reconnect_requested) {
		reconnect_requested = false;
		if (st.phase == Phase::Online) {
			drop_connection(Phase::Online);
			return;
		}
	}
	if (!net_up && st.phase != Phase::WaitNetwork) {
		drop_connection(st.phase);
		enter(Phase::WaitNetwork);
		return;
	}

	switch (st.phase) {
	case Phase::WaitNetwork:
		if (net_up && (IP_ADDR[0] | IP_ADDR[1] | IP_ADDR[2] | IP_ADDR[3]) != 0U) {
			enter(Phase::WaitTime);
		}
		break;

	case Phase::WaitTime:
		if (NET_SNTP_IsSynced()) {
			next_try = now;
			enter(Phase::TcpConnect);
		}
		break;

	case Phase::TcpConnect:
		if (bio.conn == nullptr) {
			if (static_cast<int32_t>(now - next_try) < 0) {
				break;
			}
			tcp_conn_t *c = NET_TCP_Connect(netcfg::BROKER_IP, netcfg::BROKER_PORT);
			if (c == nullptr) {
				next_try = now + ARP_RETRY_MS;   // ARP pending / no slot
				if (timed_out(TCP_CONNECT_TIMEOUT_MS)) {
					drop_connection(Phase::TcpConnect);
				}
				break;
			}
			st.attempts += 1;
			bio.conn = c;
			bio.local_port = c->server_port;
			phase_since = now;
		} else if (!bio.open() || timed_out(TCP_CONNECT_TIMEOUT_MS)) {
			drop_connection(Phase::TcpConnect);   // refused, SYN timeout, ...
		} else if (bio.established()) {
			st.last_tls_error = tls_client.start(&bio, tls::tcp_bio_send, tls::tcp_bio_recv);
			if (st.last_tls_error != 0) {
				drop_connection(Phase::TcpConnect);
			} else {
				enter(Phase::TlsHandshake);
			}
		}
		break;

	case Phase::TlsHandshake: {
		int r;
		{
			const int state = tls_client.handshake_state();
			const uint32_t worst = prof::stats.max[prof::Handshake];
			{
				prof::Scope p(prof::Handshake);
				r = tls_client.handshake();
			}
			if (prof::stats.max[prof::Handshake] != worst) {
				prof::handshake_worst_state = static_cast<uint32_t>(state);
			}
		}
		if (r == 0) {
			st.handshake_ms = now - phase_since;
			st.resumed += tls_client.resumed() ? 1U : 0U;
			st.last_verify_flags = tls_client.verify_flags();
			st.tls_heap_peak = static_cast<uint32_t>(tls::heap_peak());
			start_mqtt_session();
		} else if (r < 0 || timed_out(TLS_HANDSHAKE_TIMEOUT_MS) || !bio.open()) {
			st.last_tls_error = (r < 0) ? r : tls_client.last_error();
			st.last_verify_flags = tls_client.verify_flags();
			drop_connection(Phase::TlsHandshake);
		}
		break;
	}

	case Phase::MqttConnect:
	case Phase::Online:
		client.poll(now);
		if (client.state() == mqtt::State::Failed || !bio.open()) {
			st.last_mqtt_error = static_cast<uint8_t>(client.error());
			st.last_connack = client.connack_code();
			st.last_tls_error = tls_client.last_error();
			drop_connection(st.phase);
			break;
		}
		if (st.phase == Phase::MqttConnect && client.state() == mqtt::State::Connected) {
			st.sessions += 1;
			st.backoff_ms = 0;
			enter(Phase::Online);
			if (hooks.on_online != nullptr) {
				hooks.on_online(hooks.ctx);
			}
		}
		break;

	case Phase::Backoff:
		if (static_cast<int32_t>(now - next_try) >= 0) {
			enter(NET_SNTP_IsSynced() ? Phase::TcpConnect : Phase::WaitTime);
		}
		break;

	case Phase::Disabled:
		break;
	}
}

void request_reconnect() {
	reconnect_requested = true;
}

bool cpu_work_pending() {
	return st.phase == Phase::TlsHandshake && tls_client.crypto_busy();
}

bool online() {
	return st.phase == Phase::Online && client.state() == mqtt::State::Connected;
}

bool publish(const char *topic, const uint8_t *payload, size_t len, uint8_t qos, bool retain) {
	return online() && client.publish(topic, payload, len, qos, retain);
}

bool subscribe(const char *topic, uint8_t qos) {
	return online() && client.subscribe(topic, qos);
}

void shutdown() {
	if (online()) {
		client.disconnect();
		// give the DISCONNECT a moment to leave, then close everything
		const uint32_t t0 = get_tick();
		while (client.tx_pending() != 0U && (get_tick() - t0) < 200U) {
			client.poll(get_tick());
		}
		tls_client.close_notify();
	}
	if (bio.open()) {
		NET_TCP_Close(bio.conn);
	}
	bio = {};
	client.reset();
	enter(Phase::Disabled);
}

size_t tx_free() {
	return sizeof(mqtt_tx) - client.tx_pending();
}

const Status &status() {
	return st;
}

const mqtt::Stats &mqtt_stats() {
	return client.stats();
}

} // namespace mqtt_link
