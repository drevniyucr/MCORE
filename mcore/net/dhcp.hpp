/*
 * dhcp.hpp — DHCP client (RFC 2131 subset: DISCOVER/REQUEST/RENEW/REBIND)
 */
#pragma once

#include "core/def.hpp"
#include "net/net.hpp"

#define DHCP_CLIENT_PORT 68
#define DHCP_SERVER_PORT 67

#define DHCP_RETRY_BASE_MS     2000   // exponential backoff: 2/4/8/16 s
#define DHCP_MAX_RETRIES       4      // DISCOVER/REQUEST attempts before fallback
#define DHCP_RENEW_RETRY_MS    8000   // resend interval in RENEWING/REBINDING
#define DHCP_FALLBACK_RETRY_MS 60000  // background DISCOVER period on static IP

enum class dhcp_state_t : uint8_t {
	DHCP_IDLE = 0,
	DHCP_SELECTING,    // DISCOVER sent, waiting for OFFER
	DHCP_REQUESTING,   // REQUEST sent, waiting for ACK
	DHCP_BOUND,        // lease active
	DHCP_RENEWING,     // T1 passed: unicast REQUEST to our server
	DHCP_REBINDING,    // T2 passed: broadcast REQUEST
	DHCP_FALLBACK      // gave up, static IP active, slow background retries
};

struct dhcp_client_t {
	dhcp_state_t state;
	uint8_t retries;
	uint32_t xid;
	uint32_t lease_time;    // seconds (option 51)
	uint32_t t1;            // seconds (option 58, default lease/2)
	uint32_t t2;            // seconds (option 59, default lease*7/8)
	uint32_t lease_start;   // tick (ms) when the lease was granted
	uint32_t start_tick;    // tick when the current exchange started (secs field)
	uint32_t last_tx;       // tick of the last transmission
	uint8_t offered_ip[4];
	uint8_t server_id[4];   // option 54
	uint8_t mask[4];        // option 1
	uint8_t gw[4];          // option 3
	uint8_t dns[4];         // option 6
	uint8_t server_mac[6];  // from the OFFER frame (unicast RENEW target)
	uint8_t static_ip[4];   // fallback address saved at Start()
};

extern dhcp_client_t dhcp_client;

// Saves the current (static) IP as fallback, drops to 0.0.0.0 and
// broadcasts the first DISCOVER
void NET_DHCP_Start();

// Drives retransmissions, T1/T2 renewal and lease expiry; call
// periodically from the main loop (~100 ms cadence is fine)
void NET_DHCP_Poll();

// Handler for UDP datagrams to port 68 (wired up in NET_ProcessUDP)
void NET_DHCP_ProcessRx(ipv4_frame *frame);
