/*
 * ntp.hpp — SNTPv4 client (RFC 4330 / RFC 5905 client mode)
 *
 * Same non-blocking shape as DNS/ARP: NET_NTP_Poll() from a periodic task
 * does everything — resolve the server name, send, retry, resync hourly.
 * The time is handed to a callback instead of being written anywhere, so the
 * net layer does not depend on the RTC driver (and runs in host tests):
 *
 *   NET_NTP_Start("pool.ntp.org", NTP_PORT, [](int64_t unix_s, uint16_t ms) {
 *       RTC_SetUnixTime(unix_s + (ms >= 500));
 *   });
 *
 * Accuracy is that of SNTP: server transmit time + half the round trip,
 * a few milliseconds on a LAN. Replies are accepted only from the server's
 * address and port 123, in server mode, synchronised (LI != 3, stratum 1..15)
 * and echoing the random value we sent as transmit timestamp (anti-spoofing,
 * RFC 5905 "origin timestamp" check).
 */
#pragma once

#include "core/def.hpp"
#include "net/net.hpp"
#include "net/arp.hpp"

constexpr uint16_t NTP_PORT            = 123;
constexpr uint16_t NTP_PACKET_LEN      = 48;
constexpr uint32_t NTP_RETRY_MS        = 2000;
constexpr uint8_t  NTP_MAX_TRIES       = 3;        // requests per sync attempt
constexpr uint32_t NTP_FAIL_BACKOFF_MS = 15000;    // after a failed attempt, doubling...
constexpr uint32_t NTP_FAIL_BACKOFF_MAX_MS = 900000; // ...up to 15 min (RFC 4330: back off)
constexpr uint32_t NTP_RESYNC_MS       = 3600000;  // after a successful one

// Seconds from the NTP epoch (1900) to the Unix epoch (1970)
constexpr int64_t NTP_UNIX_OFFSET = 2208988800LL;

using ntp_time_fn = void (*)(int64_t unix_s, uint16_t ms);

enum class ntp_state_t : uint8_t {
	IDLE = 0,     // not started
	RESOLVING,    // DNS
	WAITING,      // request out (or waiting for ARP), reply pending
	SYNCED,       // last attempt succeeded; next one at next_at
	FAILED,       // last attempt failed; next one at next_at
};

struct ntp_client_t {
	ntp_state_t state;
	uint8_t tries;           // requests sent in this attempt
	uint8_t fail_streak;     // failed attempts in a row (backoff exponent)
	uint8_t stratum;         // of the last good reply
	bool sent;               // false: waiting for the server's MAC
	const char *host;
	uint16_t port;
	uint16_t local_port;
	uint8_t ip[IP_ADDR_LEN];
	uint32_t nonce[2];       // our transmit timestamp = expected origin timestamp
	uint32_t sent_at;        // tick of the last request
	uint32_t next_at;        // tick of the next attempt (SYNCED/FAILED)
	uint32_t rtt_ms;         // round trip of the last good exchange
	uint32_t syncs;
	uint32_t failures;
	int64_t last_unix;       // time delivered by the last sync
	ntp_time_fn on_time;
};

extern ntp_client_t ntp_client;

// Sync now, then every NTP_RESYNC_MS; after failures NTP_FAIL_BACKOFF_MS,
// doubled per failure in a row, capped at NTP_FAIL_BACKOFF_MAX_MS.
void NET_NTP_Start(const char *host, uint16_t port, ntp_time_fn on_time);
void NET_NTP_Poll();

// UDP datagrams to NET_NTP_LocalPort() (wired up in NET_ProcessUDP).
void NET_NTP_ProcessRx(ipv4_frame *frame);
uint16_t NET_NTP_LocalPort();

// NTP seconds -> Unix seconds, with the RFC 4330 era rule: values with the
// top bit clear are after 2036-02-07 (era 1).
constexpr int64_t ntp_to_unix(uint32_t ntp_seconds) {
	const int64_t era = (ntp_seconds & 0x80000000U) ? 0 : 1;
	return static_cast<int64_t>(ntp_seconds) + era * 0x100000000LL - NTP_UNIX_OFFSET;
}
static_assert(ntp_to_unix(0xEA9F3B80U) == 1727315200);   // 2024-09-26 01:46:40
static_assert(ntp_to_unix(0x00000000U) == 2085978496);   // 2036-02-07 06:28:16, era 1
