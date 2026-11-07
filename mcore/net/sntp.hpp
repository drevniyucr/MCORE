/*
 * sntp.hpp — SNTPv4 client (RFC 4330 subset), unicast to one server.
 *
 * Gives the network core wall-clock time for TLS certificate validity checks
 * and VDA 5050 timestamps. Each reply is corrected by half the measured round
 * trip; between replies the local ms tick carries the clock, corrected by
 * the tick's rate error measured from successive syncs (frequency
 * discipline). The poll interval starts at SNTP_MIN_INTERVAL_MS and doubles
 * up to SNTP_RESYNC_MS.
 */
#pragma once

#include "core/def.hpp"

#define SNTP_SERVER_PORT    123
#define SNTP_LOCAL_PORT     50123
#define SNTP_RETRY_MS            5000      // no reply: ask again
#define SNTP_MIN_INTERVAL_MS     64000     // first resync after 64 s ...
#define SNTP_RESYNC_MS           3600000   // ... doubling up to hourly
#define SNTP_MIN_DRIFT_WINDOW_MS 16000     // shortest span used to measure drift
#define SNTP_MAX_DRIFT_PPM       20000     // plausibility limit (2 %)
#define SNTP_DRIFT_MEMORY_MS     14400000  // drift average forgets beyond ~4 h

struct sntp_stats_t {
	uint32_t requests;
	uint32_t replies;          // accepted replies
	uint32_t rejected;         // wrong mode/originate, kiss-o'-death, ...
	uint32_t last_rtt_ms;
	uint32_t last_sync_tick;
	int32_t last_offset_ms;    // our clock minus server, just before the last sync
	int32_t drift_ppm;         // measured local tick error (+ = fast)
	uint32_t interval_ms;      // current resync interval
};

// Binds the local port and asks the server as soon as it is reachable
// (ARP permitting). Calling it again switches servers.
void NET_SNTP_Start(const uint8_t *server_ip);
// Unbinds and forgets the clock (NET_SNTP_IsSynced() becomes false).
void NET_SNTP_Stop();

// Retries / periodic resync; call from the main loop.
void NET_SNTP_Poll();

bool NET_SNTP_IsSynced();

// Milliseconds since 1970-01-01 UTC, or 0 before the first sync.
uint64_t NET_SNTP_UnixMs();

const sntp_stats_t &NET_SNTP_Stats();

// Exposed for unit tests: converts an NTP timestamp (seconds since 1900,
// 32-bit fraction) to Unix milliseconds, handling the 2036 era rollover.
uint64_t NET_SNTP_NtpToUnixMs(uint32_t seconds, uint32_t fraction);
