/*
 * dns.hpp — DNS stub resolver (RFC 1035, A records over UDP)
 *
 * Same non-blocking shape as ARP: call from a periodic task until it is OK.
 *
 *   uint8_t ip[4];
 *   if (NET_DNS_Resolve("pool.ntp.org", ip) == net_result_t::OK) { ... }
 *
 * The server is NET_DNS_SERVER (DHCP option 6, or the static default).
 * Answers are cached for their TTL (clamped), failures for DNS_FAIL_HOLD_MS.
 * Dotted IPv4 literals ("192.168.0.1") resolve immediately.
 */
#pragma once

#include "core/def.hpp"
#include "net/net.hpp"
#include "net/arp.hpp"

constexpr uint16_t DNS_SERVER_PORT  = 53;
constexpr uint8_t  DNS_CACHE_SIZE   = 4;
constexpr uint8_t  DNS_NAME_MAX     = 64;      // including the terminating '\0'
constexpr uint32_t DNS_RETRY_MS     = 2000;
constexpr uint8_t  DNS_MAX_RETRIES  = 3;       // queries sent before giving up
constexpr uint32_t DNS_FAIL_HOLD_MS = 10000;   // negative-cache time
constexpr uint32_t DNS_MIN_TTL_S    = 30;
constexpr uint32_t DNS_MAX_TTL_S    = 3600;

enum class dns_state_t : uint8_t {
	FREE = 0,
	PENDING,    // query out (or waiting for the server's MAC)
	RESOLVED,
	FAILED,     // NXDOMAIN, server error, no answer, timeout
};

struct dns_entry_t {
	char name[DNS_NAME_MAX];
	uint8_t ip[IP_ADDR_LEN];
	dns_state_t state;
	uint8_t retries;     // queries sent so far
	bool sent;           // false: still waiting for ARP of the server
	uint16_t id;         // transaction id of the outstanding query
	uint32_t stamp;      // PENDING: last send; RESOLVED: answer time; FAILED: gave up at
	uint32_t ttl_ms;
};

extern dns_entry_t dns_cache[DNS_CACHE_SIZE];

net_result_t NET_DNS_Resolve(const char *host, uint8_t *ip_out);

// Retransmissions, ARP waits and cache expiry; call periodically (~100 ms).
void NET_DNS_Poll();

// UDP datagrams to NET_DNS_LocalPort() (wired up in NET_ProcessUDP).
void NET_DNS_ProcessRx(ipv4_frame *frame);

// Our ephemeral source port (random per boot; 0 until the first query).
uint16_t NET_DNS_LocalPort();
