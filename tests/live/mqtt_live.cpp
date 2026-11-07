// Live check of the MCORE MQTT + TLS client against a real broker
// (tests/broker/run_broker.sh), over host sockets instead of the
// MCORE TCP stack:
//   1. TLS 1.2 handshake with certificate + host-name verification
//   2. MQTT CONNECT with credentials and a will
//   3. SUBSCRIBE, PUBLISH QoS 1, receive our own message back, PUBACK
//
// Usage: mqtt_live [host] [port] [ca.crt] [user] [password]
// Defaults: 127.0.0.1 8883 tests/broker/pki/ca.crt agv001 mcore-test
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <random>
#include <sstream>
#include <string>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
using sock_t = SOCKET;
static bool would_block() { return WSAGetLastError() == WSAEWOULDBLOCK; }
static void set_nonblocking(sock_t s) { u_long on = 1; ioctlsocket(s, FIONBIO, &on); }
#else
#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using sock_t = int;
static bool would_block() { return errno == EWOULDBLOCK || errno == EAGAIN; }
static void set_nonblocking(sock_t s) { fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK); }
#endif

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "net/mqtt.hpp"
#include "net/tls/tls_client.hpp"

uint32_t get_tick() {
	using namespace std::chrono;
	return static_cast<uint32_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

extern "C" int mbedtls_hardware_poll(void *, unsigned char *out, size_t len, size_t *olen) {
	static std::random_device rd;
	for (size_t i = 0; i < len; ++i) out[i] = static_cast<unsigned char>(rd());
	*olen = len;
	return 0;
}

namespace {
uint64_t host_unix_ms() { return static_cast<uint64_t>(std::time(nullptr)) * 1000U; }

int bio_send(void *ctx, const unsigned char *b, size_t n) {
	const int r = send(*static_cast<sock_t *>(ctx), reinterpret_cast<const char *>(b), static_cast<int>(n), 0);
	if (r < 0) return would_block() ? MBEDTLS_ERR_SSL_WANT_WRITE : MBEDTLS_ERR_NET_SEND_FAILED;
	return r;
}
int bio_recv(void *ctx, unsigned char *b, size_t n) {
	const int r = recv(*static_cast<sock_t *>(ctx), reinterpret_cast<char *>(b), static_cast<int>(n), 0);
	if (r < 0) return would_block() ? MBEDTLS_ERR_SSL_WANT_READ : MBEDTLS_ERR_NET_RECV_FAILED;
	if (r == 0) return MBEDTLS_ERR_NET_CONN_RESET;
	return r;
}

tls::Client g_tls;
int t_write(void *, const uint8_t *d, size_t n) { return g_tls.write(d, n); }
int t_read(void *, uint8_t *b, size_t n) { return g_tls.read(b, n); }

struct Inbox { int count = 0; std::string topic, payload; uint8_t qos = 0; };
void on_msg(void *ctx, const char *topic, uint16_t tlen, const uint8_t *p, size_t len, uint8_t qos, bool) {
	auto *in = static_cast<Inbox *>(ctx);
	in->count += 1;
	in->topic.assign(topic, tlen);
	in->payload.assign(reinterpret_cast<const char *>(p), len);
	in->qos = qos;
}

alignas(8) uint8_t g_arena[96 * 1024];
uint8_t g_tx[2048];
uint8_t g_rx[16384];

int fail(const char *what, int err = 0) {
	char msg[128] = "";
	if (err != 0) tls::error_string(err, msg, sizeof(msg));
	std::printf("FAIL: %s %s (%d)\n", what, msg, err);
	return 1;
}
} // namespace

int main(int argc, char **argv) {
	const char *host = argc > 1 ? argv[1] : "127.0.0.1";
	const int port = argc > 2 ? std::atoi(argv[2]) : 8883;
	const char *ca_file = argc > 3 ? argv[3] : MCORE_LIVE_CA_FILE;
	const char *user = argc > 4 ? argv[4] : "agv001";
	const char *pass = argc > 5 ? argv[5] : "mcore-test";

	std::ifstream f(ca_file, std::ios::binary);
	if (!f) return fail("cannot read CA file");
	std::stringstream ss;
	ss << f.rdbuf();
	const std::string ca = ss.str();

#if defined(_WIN32)
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
	sock_t s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in a = {};
	a.sin_family = AF_INET;
	a.sin_port = htons(static_cast<uint16_t>(port));
	inet_pton(AF_INET, host, &a.sin_addr);
	if (connect(s, reinterpret_cast<sockaddr *>(&a), sizeof(a)) != 0) return fail("TCP connect");
	set_nonblocking(s);

	tls::platform_init(g_arena, sizeof(g_arena));
	tls::set_time_source(host_unix_ms);
	int ret = g_tls.setup({ ca.c_str(), nullptr, nullptr }, host);
	if (ret != 0) return fail("tls setup", ret);
	g_tls.start(&s, bio_send, bio_recv);

	const uint32_t t0 = get_tick();
	while ((ret = g_tls.handshake()) == 1) {
		if (get_tick() - t0 > 10000) return fail("handshake timeout");
	}
	if (ret != 0) return fail("handshake", ret);
	std::printf("TLS ok: %s, %u ms, verify flags 0x%x\n", g_tls.ciphersuite(),
	            static_cast<unsigned>(get_tick() - t0), static_cast<unsigned>(g_tls.verify_flags()));

	Inbox inbox;
	mqtt::Client m;
	m.init({ nullptr, t_write, t_read }, g_tx, sizeof(g_tx), g_rx, sizeof(g_rx), on_msg, &inbox);
	mqtt::ConnectOptions o;
	o.client_id = "mcore-live-test";
	o.username = user;
	o.password = pass;
	o.keepalive_s = 30;
	o.will_topic = "mcore/live/connection";
	static const uint8_t will[] = "CONNECTIONBROKEN";
	o.will_payload = will;
	o.will_len = sizeof(will) - 1;
	o.will_qos = 1;
	o.will_retain = true;
	if (!m.connect(o, get_tick())) return fail("mqtt connect queue");

	auto pump = [&](auto done, uint32_t timeout_ms) {
		const uint32_t start = get_tick();
		while (!done()) {
			m.poll(get_tick());
			if (m.state() == mqtt::State::Failed || get_tick() - start > timeout_ms) return false;
		}
		return true;
	};
	if (!pump([&] { return m.state() == mqtt::State::Connected; }, 5000)) {
		std::printf("FAIL: CONNACK, state %d error %d code %d\n", static_cast<int>(m.state()),
		            static_cast<int>(m.error()), m.connack_code());
		return 1;
	}
	std::printf("MQTT connected\n");

	if (!m.subscribe("mcore/live/#", 1)) return fail("subscribe");
	const uint8_t msg[] = "hello from MCORE";
	// Give the SUBACK a moment so the broker routes our publish back to us
	pump([&] { return false; }, 300);
	if (!m.publish("mcore/live/echo", msg, sizeof(msg) - 1, 1, false)) return fail("publish");
	if (!pump([&] { return inbox.count > 0 && m.inflight() == 0; }, 5000)) {
		std::printf("FAIL: echo/puback, received %d inflight %u\n", inbox.count, m.inflight());
		return 1;
	}
	std::printf("echo ok: topic %s payload '%s' qos %u, puback %u\n", inbox.topic.c_str(),
	            inbox.payload.c_str(), inbox.qos, m.stats().puback);

	m.disconnect();
	pump([&] { return m.tx_pending() == 0; }, 1000);
	g_tls.close_notify();
	std::printf("TLS arena peak: %u bytes\n", static_cast<unsigned>(tls::heap_peak()));
	std::printf("PASS\n");
	return 0;
}
