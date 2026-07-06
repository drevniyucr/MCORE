/*
 * dhcp.cpp — DHCP client state machine
 */
#include <cstring>

#include "net/dhcp.hpp"
#include "net/net.hpp"
#include "net/tcp.hpp"
#include "net/eth_utils.hpp"
#include "drivers/eth.hpp"
#include "core/system.hpp"

dhcp_client_t dhcp_client = { };

namespace {

// Message types (option 53)
constexpr uint8_t DHCP_MSG_DISCOVER = 1;
constexpr uint8_t DHCP_MSG_OFFER = 2;
constexpr uint8_t DHCP_MSG_REQUEST = 3;
constexpr uint8_t DHCP_MSG_ACK = 5;
constexpr uint8_t DHCP_MSG_NAK = 6;

constexpr uint16_t BOOTP_POS = ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN;
constexpr uint16_t BOOTP_HDR_LEN = 236;    // fixed part before magic cookie
constexpr uint16_t BOOTP_MIN_LEN = 300;    // classic BOOTP minimum payload
constexpr uint32_t DHCP_MAGIC_COOKIE = 0x63825363;

constexpr uint32_t DHCP_DEFAULT_LEASE_S = 86400;

const uint8_t BCAST_MAC[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
const uint8_t BCAST_IP[4] = { 255, 255, 255, 255 };
const uint8_t ZERO_IP[4] = { 0, 0, 0, 0 };

// Seconds elapsed in the current lease
inline uint32_t lease_age_s() {
	return (get_tick() - dhcp_client.lease_start) / 1000U;
}

// Find a DHCP option; returns pointer to its value or nullptr
const uint8_t* find_option(const uint8_t *opts, uint16_t len, uint8_t code,
		uint8_t *out_len) {
	uint16_t i = 0;
	while (i < len) {
		const uint8_t kind = opts[i];
		if (kind == 255) {
			break;  // end
		}
		if (kind == 0) {
			i++;    // pad
			continue;
		}
		if (i + 1 >= len || i + 2 + opts[i + 1] > len) {
			break;  // malformed
		}
		if (kind == code) {
			*out_len = opts[i + 1];
			return &opts[i + 2];
		}
		i += 2 + opts[i + 1];
	}
	return nullptr;
}

// Live TCP connections become unreachable when our address changes
void reset_tcp_connections() {
	for (auto &conn : tcp_clients) {
		if (conn.state != tcp_state_t::TCP_CLOSED) {
			NET_TCP_ClientRemove(conn.socket_tag);
		}
	}
}

/**
 * @brief Build and send a DHCP message for the current state
 *
 * SELECTING/REQUESTING: no address yet -> src 0.0.0.0, IP+MAC broadcast,
 * BOOTP BROADCAST flag set so replies come back as broadcast too.
 * RENEWING: unicast to the leasing server, ciaddr = our address.
 * REBINDING: broadcast, but ciaddr is still valid.
 */
void dhcp_send(uint8_t msg_type) {
	dhcp_client_t &c = dhcp_client;
	const bool have_ip = (c.state == dhcp_state_t::DHCP_RENEWING ||
	                      c.state == dhcp_state_t::DHCP_REBINDING);
	const bool unicast = (c.state == dhcp_state_t::DHCP_RENEWING);

	uint8_t *buf = TxDescList.pBuff;
	const uint16_t frame_len = BOOTP_POS + BOOTP_MIN_LEN;
	memset(buf, 0, frame_len);

	// Ethernet
	memcpy(buf, unicast ? c.server_mac : BCAST_MAC, MAC_ADDR_LEN);
	memcpy(&buf[6], MAC_ADDR, MAC_ADDR_LEN);
	buf[12] = 0x08;
	buf[13] = 0x00;

	// IPv4
	uint8_t *ip = &buf[IP_HDR_POS];
	const uint16_t ip_total = IP_HDR_LEN + UDP_HDR_LEN + BOOTP_MIN_LEN;
	ip[0] = 0x45;
	ip[2] = static_cast<uint8_t>(ip_total >> 8);
	ip[3] = static_cast<uint8_t>(ip_total & 0xFF);
	ip[8] = 64;                                    // TTL
	ip[9] = 17;                                    // UDP
	memcpy(&ip[12], have_ip ? IP_ADDR : ZERO_IP, IP_ADDR_LEN);
	memcpy(&ip[16], unicast ? c.server_id : BCAST_IP, IP_ADDR_LEN);
	const uint16_t ip_csum = checksum(ip, IP_HDR_LEN);
	ip[10] = static_cast<uint8_t>(ip_csum >> 8);
	ip[11] = static_cast<uint8_t>(ip_csum & 0xFF);

	// UDP (checksum 0 = disabled, legal for IPv4)
	uint8_t *udp = &buf[UDP_HDR_POS];
	udp[0] = 0;
	udp[1] = DHCP_CLIENT_PORT;
	udp[2] = 0;
	udp[3] = DHCP_SERVER_PORT;
	const uint16_t udp_len = UDP_HDR_LEN + BOOTP_MIN_LEN;
	udp[4] = static_cast<uint8_t>(udp_len >> 8);
	udp[5] = static_cast<uint8_t>(udp_len & 0xFF);

	// BOOTP
	uint8_t *b = &buf[BOOTP_POS];
	b[0] = 1;                                      // BOOTREQUEST
	b[1] = 1;                                      // htype ethernet
	b[2] = 6;                                      // hlen
	U32toBuffU8(&b[4], c.xid);
	uint32_t secs = (get_tick() - c.start_tick) / 1000U;
	if (secs > 0xFFFF) {
		secs = 0xFFFF;
	}
	b[8] = static_cast<uint8_t>(secs >> 8);
	b[9] = static_cast<uint8_t>(secs & 0xFF);
	if (!have_ip) {
		b[10] = 0x80;                              // BROADCAST flag
	} else {
		memcpy(&b[12], IP_ADDR, IP_ADDR_LEN);      // ciaddr
	}
	memcpy(&b[28], MAC_ADDR, MAC_ADDR_LEN);        // chaddr
	U32toBuffU8(&b[BOOTP_HDR_LEN], DHCP_MAGIC_COOKIE);

	// Options
	uint8_t *o = &b[BOOTP_HDR_LEN + 4];
	*o++ = 53; *o++ = 1; *o++ = msg_type;
	if (c.state == dhcp_state_t::DHCP_REQUESTING) {
		// Only the SELECTING->REQUESTING request names the offer explicitly
		*o++ = 50; *o++ = 4;                       // requested IP
		memcpy(o, c.offered_ip, 4);
		o += 4;
		*o++ = 54; *o++ = 4;                       // server identifier
		memcpy(o, c.server_id, 4);
		o += 4;
	}
	*o++ = 55; *o++ = 3;                           // parameter request list
	*o++ = 1;                                      // subnet mask
	*o++ = 3;                                      // router
	*o++ = 6;                                      // DNS
	*o = 255;                                      // end (rest is zero padding)

	ETH_SendFrame(frame_len);
	c.last_tx = get_tick();
}

// Begin a fresh acquisition cycle (new xid, DISCOVER broadcast)
void dhcp_restart_discovery() {
	dhcp_client_t &c = dhcp_client;
	c.xid = NET_Entropy();
	c.retries = 0;
	c.start_tick = get_tick();
	c.state = dhcp_state_t::DHCP_SELECTING;
	dhcp_send(DHCP_MSG_DISCOVER);
}

// Apply a lease from an ACK
void dhcp_apply_ack(const uint8_t *bootp, const uint8_t *opts, uint16_t opts_len) {
	dhcp_client_t &c = dhcp_client;
	uint8_t olen = 0;
	const uint8_t *v;

	c.lease_time = DHCP_DEFAULT_LEASE_S;
	if ((v = find_option(opts, opts_len, 51, &olen)) != nullptr && olen == 4) {
		c.lease_time = BuffU8ToU32(const_cast<uint8_t*>(v));
	}
	c.t1 = c.lease_time / 2;
	if ((v = find_option(opts, opts_len, 58, &olen)) != nullptr && olen == 4) {
		c.t1 = BuffU8ToU32(const_cast<uint8_t*>(v));
	}
	c.t2 = c.lease_time - c.lease_time / 8;        // 87.5%
	if ((v = find_option(opts, opts_len, 59, &olen)) != nullptr && olen == 4) {
		c.t2 = BuffU8ToU32(const_cast<uint8_t*>(v));
	}
	if ((v = find_option(opts, opts_len, 1, &olen)) != nullptr && olen == 4) {
		memcpy(c.mask, v, 4);
	}
	if ((v = find_option(opts, opts_len, 3, &olen)) != nullptr && olen >= 4) {
		memcpy(c.gw, v, 4);
	}
	if ((v = find_option(opts, opts_len, 6, &olen)) != nullptr && olen >= 4) {
		memcpy(c.dns, v, 4);
	}
	if ((v = find_option(opts, opts_len, 54, &olen)) != nullptr && olen == 4) {
		memcpy(c.server_id, v, 4);
	}

	const uint8_t *yiaddr = &bootp[16];
	if (memcmp(yiaddr, IP_ADDR, IP_ADDR_LEN) != 0) {
		reset_tcp_connections();
		NET_SetIPAddr(yiaddr);
	}
	c.lease_start = get_tick();
	c.retries = 0;
	c.state = dhcp_state_t::DHCP_BOUND;
}

} // namespace

void NET_DHCP_Start() {
	memset(&dhcp_client, 0, sizeof(dhcp_client));
	memcpy(dhcp_client.static_ip, IP_ADDR, IP_ADDR_LEN);
	NET_SetIPAddr(ZERO_IP);                        // no address while acquiring
	dhcp_restart_discovery();
}

void NET_DHCP_ProcessRx(ipv4_frame *frame) {
	dhcp_client_t &c = dhcp_client;
	const uint8_t *udp = &frame->ip_hdr[frame->ip_hdr_len];
	const uint8_t *bootp = udp + UDP_HDR_LEN;
	const uint16_t bootp_len = static_cast<uint16_t>(
			frame->ip_len - frame->ip_hdr_len - UDP_HDR_LEN);

	// Sanity: BOOTREPLY for us, matching transaction, valid cookie
	if (bootp_len < BOOTP_HDR_LEN + 4 + 3) {
		return;
	}
	if (bootp[0] != 2) {
		return;
	}
	if (BuffU8ToU32(const_cast<uint8_t*>(&bootp[4])) != c.xid) {
		return;
	}
	if (memcmp(&bootp[28], MAC_ADDR, MAC_ADDR_LEN) != 0) {
		return;
	}
	if (BuffU8ToU32(const_cast<uint8_t*>(&bootp[BOOTP_HDR_LEN])) != DHCP_MAGIC_COOKIE) {
		return;
	}

	const uint8_t *opts = &bootp[BOOTP_HDR_LEN + 4];
	const uint16_t opts_len = static_cast<uint16_t>(bootp_len - BOOTP_HDR_LEN - 4);
	uint8_t olen = 0;
	const uint8_t *type = find_option(opts, opts_len, 53, &olen);
	if (type == nullptr || olen != 1) {
		return;
	}

	switch (c.state) {
	case dhcp_state_t::DHCP_SELECTING:
		if (*type == DHCP_MSG_OFFER) {
			const uint8_t *sid = find_option(opts, opts_len, 54, &olen);
			if (sid == nullptr || olen != 4) {
				return;  // OFFER without server id - ignore
			}
			memcpy(c.offered_ip, &bootp[16], IP_ADDR_LEN);   // yiaddr
			memcpy(c.server_id, sid, IP_ADDR_LEN);
			memcpy(c.server_mac, frame->scr_mac, MAC_ADDR_LEN);
			c.retries = 0;
			c.state = dhcp_state_t::DHCP_REQUESTING;
			dhcp_send(DHCP_MSG_REQUEST);
		}
		break;

	case dhcp_state_t::DHCP_REQUESTING:
	case dhcp_state_t::DHCP_RENEWING:
	case dhcp_state_t::DHCP_REBINDING:
		if (*type == DHCP_MSG_ACK) {
			dhcp_apply_ack(bootp, opts, opts_len);
		} else if (*type == DHCP_MSG_NAK) {
			// Server refused: drop the address and start over
			reset_tcp_connections();
			NET_SetIPAddr(ZERO_IP);
			dhcp_restart_discovery();
		}
		break;

	default:
		break;
	}
}

void NET_DHCP_Poll() {
	dhcp_client_t &c = dhcp_client;
	const uint32_t now = get_tick();

	switch (c.state) {
	case dhcp_state_t::DHCP_SELECTING:
	case dhcp_state_t::DHCP_REQUESTING: {
		const uint32_t timeout = DHCP_RETRY_BASE_MS << c.retries;
		if (now - c.last_tx >= timeout) {
			if (c.retries + 1 >= DHCP_MAX_RETRIES) {
				// No server: fall back to the saved static address,
				// keep retrying slowly in the background
				NET_SetIPAddr(c.static_ip);
				c.state = dhcp_state_t::DHCP_FALLBACK;
				c.last_tx = now;
			} else {
				c.retries++;
				dhcp_send(c.state == dhcp_state_t::DHCP_SELECTING
						? DHCP_MSG_DISCOVER : DHCP_MSG_REQUEST);
			}
		}
		break;
	}

	case dhcp_state_t::DHCP_BOUND:
		if (lease_age_s() >= c.t1) {
			c.retries = 0;
			c.start_tick = now;
			c.state = dhcp_state_t::DHCP_RENEWING;
			dhcp_send(DHCP_MSG_REQUEST);
		}
		break;

	case dhcp_state_t::DHCP_RENEWING:
		if (lease_age_s() >= c.t2) {
			c.retries = 0;
			c.state = dhcp_state_t::DHCP_REBINDING;
			dhcp_send(DHCP_MSG_REQUEST);
		} else if (now - c.last_tx >= DHCP_RENEW_RETRY_MS) {
			dhcp_send(DHCP_MSG_REQUEST);
		}
		break;

	case dhcp_state_t::DHCP_REBINDING:
		if (lease_age_s() >= c.lease_time) {
			// Lease expired: we no longer own the address
			reset_tcp_connections();
			NET_SetIPAddr(ZERO_IP);
			dhcp_restart_discovery();
		} else if (now - c.last_tx >= DHCP_RENEW_RETRY_MS) {
			dhcp_send(DHCP_MSG_REQUEST);
		}
		break;

	case dhcp_state_t::DHCP_FALLBACK:
		if (now - c.last_tx >= DHCP_FALLBACK_RETRY_MS) {
			// Background retry; static IP stays active until an ACK arrives
			dhcp_restart_discovery();
		}
		break;

	default:
		break;
	}
}
