/*
 * sntp.cpp — SNTPv4 client, see sntp.hpp.
 */
#include <cstring>

#include "net/sntp.hpp"
#include "net/net.hpp"
#include "net/eth_utils.hpp"
#include "core/system.hpp"

namespace {

constexpr uint16_t SNTP_PACKET_LEN = 48;
constexpr uint8_t LI_VN_MODE_CLIENT = (0U << 6) | (4U << 3) | 3U;   // no leap, v4, client
constexpr uint8_t MODE_SERVER = 4;
constexpr uint8_t MODE_BROADCAST = 5;
constexpr uint32_t SEND_RETRY_MS = 200;   // ARP not resolved / TX busy

// Seconds between 1900-01-01 (NTP era 0) and 1970-01-01 (Unix epoch)
constexpr uint64_t NTP_UNIX_OFFSET_S = 2208988800ULL;

struct sntp_client_t {
	bool active;
	bool synced;
	bool awaiting;             // request out, no answer yet
	uint8_t server[IP_ADDR_LEN];
	uint32_t nonce_hi;         // our transmit timestamp, echoed as "originate"
	uint32_t nonce_lo;
	uint32_t sent_tick;        // tick of the last successful send
	uint32_t next_tick;        // when to (re)send
	uint64_t base_unix_ms;     // wall clock at base_tick
	uint32_t base_tick;
	int32_t drift_ppm;         // local tick rate error (+ = local clock fast)
	uint32_t interval_ms;      // current resync interval
	uint32_t drift_weight_ms;  // observation time behind drift_ppm
};


sntp_client_t c = {};
sntp_stats_t stats = {};

// Wall clock = base + local elapsed time corrected by the measured drift
uint64_t corrected_elapsed(uint32_t local_ms) {
	const int64_t e = static_cast<int64_t>(local_ms);
	return static_cast<uint64_t>(e - (e * c.drift_ppm) / 1000000);
}

inline uint32_t rd32(const uint8_t *p) {
	return BuffU8ToU32(const_cast<uint8_t *>(p));
}

bool time_reached(uint32_t now, uint32_t when) {
	return static_cast<int32_t>(now - when) >= 0;
}

void on_datagram(const uint8_t *src_ip, uint16_t src_port, const uint8_t *data,
		uint16_t len, void *) {
	if (!c.awaiting || src_port != SNTP_SERVER_PORT ||
	    memcmp(src_ip, c.server, IP_ADDR_LEN) != 0 || len < SNTP_PACKET_LEN) {
		return;
	}
	const uint8_t mode = data[0] & 0x07U;
	const uint8_t stratum = data[1];
	const uint8_t leap = data[0] >> 6;
	// Must answer OUR request: originate timestamp == our transmit timestamp
	const bool ours = rd32(&data[24]) == c.nonce_hi && rd32(&data[28]) == c.nonce_lo;
	const uint32_t tx_s = rd32(&data[40]);
	const uint32_t tx_f = rd32(&data[44]);
	if ((mode != MODE_SERVER && mode != MODE_BROADCAST) || !ours ||
	    stratum == 0 /* kiss-o'-death */ || stratum > 15 ||
	    leap == 3 /* server not synchronised */ || (tx_s == 0 && tx_f == 0)) {
		stats.rejected += 1;
		return;
	}

	const uint32_t now = get_tick();
	const uint32_t rtt = now - c.sent_tick;
	const uint64_t server_ms = NET_SNTP_NtpToUnixMs(tx_s, tx_f) + rtt / 2U;

	// Frequency discipline: compare how far our tick and the server moved
	// since the previous sync. The tick comes from the board oscillator
	// (NUCLEO-H755ZI-Q: the ST-LINK MCO, measured 0.63 % fast), so without
	// this the clock would drift by tens of seconds per hour.
	const uint32_t local_elapsed = now - c.base_tick;
	// How wrong our clock was just before this reply (with the old estimate)
	stats.last_offset_ms = c.synced
		? static_cast<int32_t>(static_cast<int64_t>(c.base_unix_ms + corrected_elapsed(local_elapsed)) -
		                       static_cast<int64_t>(server_ms))
		: 0;
	if (c.synced && local_elapsed >= SNTP_MIN_DRIFT_WINDOW_MS && server_ms > c.base_unix_ms) {
		const int64_t server_elapsed = static_cast<int64_t>(server_ms - c.base_unix_ms);
		int64_t ppm = (static_cast<int64_t>(local_elapsed) - server_elapsed) * 1000000 / server_elapsed;
		if (ppm > SNTP_MAX_DRIFT_PPM) ppm = SNTP_MAX_DRIFT_PPM;
		if (ppm < -SNTP_MAX_DRIFT_PPM) ppm = -SNTP_MAX_DRIFT_PPM;
		// Average weighted by observation time: a 1 h window outweighs the
		// first 64 s one, whose few ms of timestamp jitter cost ~100 ppm
		const int64_t w_old = c.drift_weight_ms;
		const int64_t w_new = local_elapsed;
		c.drift_ppm = static_cast<int32_t>((c.drift_ppm * w_old + ppm * w_new) / (w_old + w_new));
		c.drift_weight_ms = static_cast<uint32_t>(
			(w_old + w_new) > SNTP_DRIFT_MEMORY_MS ? SNTP_DRIFT_MEMORY_MS : (w_old + w_new));
	}

	c.base_unix_ms = server_ms;
	c.base_tick = now;
	c.synced = true;
	c.awaiting = false;
	// Poll often while the drift estimate settles, then back off (like NTP)
	c.interval_ms = (c.interval_ms == 0U) ? SNTP_MIN_INTERVAL_MS
	              : (c.interval_ms * 2U > SNTP_RESYNC_MS ? SNTP_RESYNC_MS : c.interval_ms * 2U);
	c.next_tick = now + c.interval_ms;
	stats.drift_ppm = c.drift_ppm;
	stats.interval_ms = c.interval_ms;
	stats.replies += 1;
	stats.last_rtt_ms = rtt;
	stats.last_sync_tick = now;
}

bool send_request() {
	uint8_t pkt[SNTP_PACKET_LEN] = {};
	pkt[0] = LI_VN_MODE_CLIENT;
	// Random transmit timestamp: the server copies it into "originate",
	// which lets us drop stale or spoofed replies (RFC 4330 section 5)
	c.nonce_hi = NET_Entropy() ^ 0x5A5A5A5AU;
	c.nonce_lo = NET_Entropy() * 2654435761U + get_tick();
	U32toBuffU8(&pkt[40], c.nonce_hi);
	U32toBuffU8(&pkt[44], c.nonce_lo);
	return NET_UDP_SendTo(c.server, SNTP_SERVER_PORT, SNTP_LOCAL_PORT, pkt, sizeof(pkt));
}

} // namespace

uint64_t NET_SNTP_NtpToUnixMs(uint32_t seconds, uint32_t fraction) {
	// Era 0 ends in 2036; timestamps with the top bit clear belong to era 1
	// (valid until 2104), anything else is era 0 (1968..2036).
	uint64_t s = seconds;
	if ((seconds & 0x80000000U) == 0U) {
		s += 0x100000000ULL;
	}
	const uint64_t ms_frac = (static_cast<uint64_t>(fraction) * 1000U) >> 32;
	return (s - NTP_UNIX_OFFSET_S) * 1000U + ms_frac;
}

void NET_SNTP_Start(const uint8_t *server_ip) {
	memcpy(c.server, server_ip, IP_ADDR_LEN);
	c.active = NET_UDP_Bind(SNTP_LOCAL_PORT, on_datagram, nullptr);
	c.awaiting = false;
	c.next_tick = get_tick();
}

void NET_SNTP_Stop() {
	NET_UDP_Unbind(SNTP_LOCAL_PORT);
	c = {};   // forget the server and the clock
}

void NET_SNTP_Poll() {
	if (!c.active) {
		return;
	}
	const uint32_t now = get_tick();
	if (!time_reached(now, c.next_tick)) {
		return;
	}
	if (send_request()) {
		stats.requests += 1;
		c.awaiting = true;
		c.sent_tick = now;
		c.next_tick = now + SNTP_RETRY_MS;
	} else {
		c.next_tick = now + SEND_RETRY_MS;   // ARP pending or TX queue full
	}
}

bool NET_SNTP_IsSynced() {
	return c.synced;
}

uint64_t NET_SNTP_UnixMs() {
	NetGuard guard;   // 64-bit state updated by the stack's task
	if (!c.synced) {
		return 0;
	}
	return c.base_unix_ms + corrected_elapsed(get_tick() - c.base_tick);
}

const sntp_stats_t &NET_SNTP_Stats() {
	return stats;
}
