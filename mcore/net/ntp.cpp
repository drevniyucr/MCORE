/*
 * ntp.cpp — SNTPv4 client: request, reply validation, resync schedule
 */
#include <cstring>

#include "net/ntp.hpp"
#include "net/net.hpp"
#include "net/arp.hpp"
#include "net/dns.hpp"
#include "net/eth_utils.hpp"
#include "core/system.hpp"

ntp_client_t ntp_client = { };

namespace {

constexpr uint8_t NTP_LI_VN_CLIENT = (0 << 6) | (4 << 3) | 3;   // no warning, v4, client
constexpr uint8_t NTP_MODE_SERVER  = 4;
constexpr uint8_t NTP_LI_ALARM     = 3;                         // server not synchronised
constexpr uint8_t NTP_STRATUM_MAX  = 15;                        // 0 = kiss-of-death
constexpr uint16_t NTP_ORIGIN_POS   = 24;
constexpr uint16_t NTP_TRANSMIT_POS = 40;
constexpr uint16_t EPHEMERAL_PORT_BASE = 49152;
constexpr uint16_t EPHEMERAL_PORT_MASK = 0x3FFF;

uint16_t rd16(const uint8_t *p) { return static_cast<uint16_t>((p[0] << 8) | p[1]); }

void schedule(ntp_state_t state, uint32_t delay_ms) {
	ntp_client.state = state;
	ntp_client.next_at = get_tick() + delay_ms;
}

void fail() {
	ntp_client_t &c = ntp_client;
	c.failures++;
	uint32_t backoff = NTP_FAIL_BACKOFF_MS;
	for (uint8_t i = 0; i < c.fail_streak && backoff < NTP_FAIL_BACKOFF_MAX_MS; i++) backoff *= 2;
	if (backoff > NTP_FAIL_BACKOFF_MAX_MS) backoff = NTP_FAIL_BACKOFF_MAX_MS;
	if (c.fail_streak < UINT8_MAX) c.fail_streak++;
	schedule(ntp_state_t::FAILED, backoff);
}

// Send a request; stays unsent (retried from poll) while ARP resolves.
void send_request() {
	ntp_client_t &c = ntp_client;
	uint8_t mac[MAC_ADDR_LEN];
	const net_result_t arp = NET_ARP_Resolve(c.ip, mac);
	if (arp == net_result_t::UNREACHABLE) { fail(); return; }
	c.sent_at = get_tick();
	if (arp == net_result_t::PENDING) { c.sent = false; return; }

	if (c.local_port == 0) {
		c.local_port = static_cast<uint16_t>(EPHEMERAL_PORT_BASE + (NET_Entropy() & EPHEMERAL_PORT_MASK));
		if (c.local_port == NET_DNS_LocalPort()) c.local_port++;   // both are random: keep them apart
	}
	// A random transmit timestamp instead of our (unknown) clock: the server
	// echoes it as origin timestamp, which ties its reply to this request
	c.nonce[0] = NET_Entropy();
	c.nonce[1] = NET_Entropy() ^ get_tick();

	uint8_t pkt[NTP_PACKET_LEN] = { };
	pkt[0] = NTP_LI_VN_CLIENT;
	U32toBuffU8(&pkt[NTP_TRANSMIT_POS], c.nonce[0]);
	U32toBuffU8(&pkt[NTP_TRANSMIT_POS + 4], c.nonce[1]);

	UDP_SendFrameStruct f = { mac, c.ip, c.port, c.local_port, pkt, NTP_PACKET_LEN };
	NET_SendUDP(f);
	c.sent = true;
	c.tries++;
}

} // namespace

void NET_NTP_Start(const char *host, uint16_t port, ntp_time_fn on_time) {
	const uint16_t local = ntp_client.local_port;   // keep: late replies to the old port are harmless
	memset(&ntp_client, 0, sizeof(ntp_client));
	ntp_client.local_port = local;
	ntp_client.host = host;
	ntp_client.port = port;
	ntp_client.on_time = on_time;
	schedule(ntp_state_t::FAILED, 0);               // "next attempt: now"
}

void NET_NTP_Poll() {
	ntp_client_t &c = ntp_client;
	const uint32_t now = get_tick();
	switch (c.state) {
	case ntp_state_t::SYNCED:
	case ntp_state_t::FAILED:
		if (static_cast<int32_t>(now - c.next_at) < 0) return;
		c.tries = 0;
		c.state = ntp_state_t::RESOLVING;
		[[fallthrough]];

	case ntp_state_t::RESOLVING:
		switch (NET_DNS_Resolve(c.host, c.ip)) {
		case net_result_t::OK:          break;
		case net_result_t::UNREACHABLE: fail(); return;
		default:                        return;
		}
		c.state = ntp_state_t::WAITING;
		send_request();
		return;

	case ntp_state_t::WAITING:
		if (!c.sent) {
			send_request();                         // the server's MAC may be known now
		} else if (now - c.sent_at >= NTP_RETRY_MS) {
			if (c.tries >= NTP_MAX_TRIES) fail();
			else send_request();
		}
		return;

	default:
		return;
	}
}

void NET_NTP_ProcessRx(ipv4_frame *frame) {
	ntp_client_t &c = ntp_client;
	const uint8_t *udp = &frame->ip_hdr[frame->ip_hdr_len];
	if (c.state != ntp_state_t::WAITING || !c.sent || c.local_port == 0 ||
	    rd16(udp) != c.port || memcmp(frame->scr_addr, c.ip, IP_ADDR_LEN) != 0) {
		return;                                     // not the reply we wait for
	}
	const uint16_t udp_len = rd16(&udp[4]);
	if (udp_len < UDP_HDR_LEN + NTP_PACKET_LEN || udp_len > frame->ip_len - frame->ip_hdr_len) {
		return;
	}
	const uint8_t *p = udp + UDP_HDR_LEN;
	const uint8_t li = p[0] >> 6;
	const uint8_t vn = (p[0] >> 3) & 0x7;
	const uint8_t mode = p[0] & 0x7;
	if (mode != NTP_MODE_SERVER || vn < 3 || vn > 4 ||
	    BuffU8ToU32(&p[NTP_ORIGIN_POS]) != c.nonce[0] || BuffU8ToU32(&p[NTP_ORIGIN_POS + 4]) != c.nonce[1]) {
		return;                                     // not an answer to our request
	}
	const uint8_t stratum = p[1];
	const uint32_t tx_sec = BuffU8ToU32(&p[NTP_TRANSMIT_POS]);
	const uint32_t tx_frac = BuffU8ToU32(&p[NTP_TRANSMIT_POS + 4]);
	if (li == NTP_LI_ALARM || stratum == 0 || stratum > NTP_STRATUM_MAX || tx_sec == 0) {
		fail();                                     // unsynchronised server or kiss-of-death
		return;
	}

	// Server transmit time + half the round trip
	c.rtt_ms = get_tick() - c.sent_at;
	uint32_t ms = static_cast<uint32_t>((static_cast<uint64_t>(tx_frac) * 1000U) >> 32) + c.rtt_ms / 2;
	int64_t unix_s = ntp_to_unix(tx_sec) + ms / 1000U;
	ms %= 1000U;

	c.stratum = stratum;
	c.last_unix = unix_s;
	c.syncs++;
	c.fail_streak = 0;
	schedule(ntp_state_t::SYNCED, NTP_RESYNC_MS);
	if (c.on_time != nullptr) {
		c.on_time(unix_s, static_cast<uint16_t>(ms));
	}
}

uint16_t NET_NTP_LocalPort() {
	return ntp_client.local_port;
}
