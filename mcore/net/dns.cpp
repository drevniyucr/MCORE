/*
 * dns.cpp — DNS stub resolver: query builder, response parser, cache
 */
#include <cstring>

#include "net/dns.hpp"
#include "net/net.hpp"
#include "net/arp.hpp"
#include "net/eth_utils.hpp"
#include "core/system.hpp"

dns_entry_t dns_cache[DNS_CACHE_SIZE] = { };

namespace {

constexpr uint16_t DNS_HDR_LEN = 12;
constexpr uint16_t DNS_QUERY_MAX = DNS_HDR_LEN + DNS_NAME_MAX + 1 + 4;  // header + qname + type/class
constexpr uint8_t  DNS_LABEL_MAX = 63;
constexpr uint16_t DNS_TYPE_A = 1;
constexpr uint16_t DNS_CLASS_IN = 1;
constexpr uint8_t  DNS_FLAG_QR = 0x80;      // byte 2: response
constexpr uint8_t  DNS_FLAG_TC = 0x02;      // byte 2: truncated
constexpr uint8_t  DNS_FLAG_RD = 0x01;      // byte 2: recursion desired
constexpr uint8_t  DNS_RCODE_MASK = 0x0F;   // byte 3
constexpr uint16_t EPHEMERAL_PORT_BASE = 49152;  // RFC 6335 dynamic range
constexpr uint16_t EPHEMERAL_PORT_MASK = 0x3FFF;

uint16_t local_port = 0;

uint16_t rd16(const uint8_t *p) { return static_cast<uint16_t>((p[0] << 8) | p[1]); }

char lower(char c) { return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c; }

// DNS names compare case-insensitively (RFC 4343)
bool same_name(const char *a, const char *b) {
	while (*a != '\0' && lower(*a) == lower(*b)) { a++; b++; }
	return *a == *b;
}

// "a.b.c.d" with each part 0..255
bool parse_ipv4(const char *s, uint8_t *ip) {
	for (uint8_t i = 0; i < IP_ADDR_LEN; i++) {
		uint16_t v = 0;
		uint8_t digits = 0;
		while (*s >= '0' && *s <= '9') {
			v = static_cast<uint16_t>(v * 10 + (*s++ - '0'));
			if (++digits > 3 || v > 255) return false;
		}
		if (digits == 0) return false;
		ip[i] = static_cast<uint8_t>(v);
		if (i < IP_ADDR_LEN - 1 && *s++ != '.') return false;
	}
	return *s == '\0';
}

// Encodes "pool.ntp.org" as 4pool3ntp3org0; returns the length, 0 if invalid.
uint16_t encode_name(const char *name, uint8_t *out) {
	uint16_t pos = 0;
	while (true) {
		const char *dot = name;
		while (*dot != '\0' && *dot != '.') dot++;
		const auto len = static_cast<uint16_t>(dot - name);
		if (len == 0 || len > DNS_LABEL_MAX) return 0;   // empty label: "a..b", ".a", "a."
		out[pos++] = static_cast<uint8_t>(len);
		memcpy(&out[pos], name, len);
		pos += len;
		if (*dot == '\0') break;
		name = dot + 1;
	}
	out[pos++] = 0;
	return pos;
}

// Skips a (possibly compressed) name; nullptr if it runs past end.
const uint8_t* skip_name(const uint8_t *p, const uint8_t *end) {
	while (p < end) {
		const uint8_t len = *p;
		if ((len & 0xC0) == 0xC0) return (p + 2 <= end) ? p + 2 : nullptr;  // pointer ends the name
		if (len == 0) return p + 1;
		p += len + 1;
	}
	return nullptr;
}

dns_entry_t* find(const char *name) {
	for (auto &e : dns_cache) {
		if (e.state != dns_state_t::FREE && same_name(e.name, name)) return &e;
	}
	return nullptr;
}

// A free slot, else the oldest non-pending one.
dns_entry_t* alloc(const char *name) {
	dns_entry_t *victim = nullptr;
	for (auto &e : dns_cache) {
		if (e.state == dns_state_t::FREE) { victim = &e; break; }
		if (e.state != dns_state_t::PENDING &&
		    (victim == nullptr || get_tick() - e.stamp > get_tick() - victim->stamp)) {
			victim = &e;
		}
	}
	if (victim != nullptr) {
		memset(victim, 0, sizeof(*victim));
		strncpy(victim->name, name, DNS_NAME_MAX - 1);
	}
	return victim;
}

void fail(dns_entry_t &e) {
	e.state = dns_state_t::FAILED;
	e.stamp = get_tick();
}

// Send (or re-send) the query; waits in PENDING while the server's MAC resolves.
void send_query(dns_entry_t &e) {
	uint8_t mac[MAC_ADDR_LEN];
	const net_result_t arp = NET_ARP_Resolve(NET_DNS_SERVER, mac);
	if (arp == net_result_t::UNREACHABLE) {
		fail(e);
		return;
	}
	e.stamp = get_tick();
	if (arp == net_result_t::PENDING) {
		e.sent = false;
		return;
	}

	if (local_port == 0) {
		local_port = static_cast<uint16_t>(EPHEMERAL_PORT_BASE + (NET_Entropy() & EPHEMERAL_PORT_MASK));
	}
	if (e.retries == 0) {
		e.id = static_cast<uint16_t>(NET_Entropy());
	}

	uint8_t q[DNS_QUERY_MAX] = { };
	q[0] = static_cast<uint8_t>(e.id >> 8);
	q[1] = static_cast<uint8_t>(e.id & 0xFF);
	q[2] = DNS_FLAG_RD;
	q[5] = 1;                                      // QDCOUNT = 1
	uint16_t len = DNS_HDR_LEN;
	len += encode_name(e.name, &q[len]);           // validated in NET_DNS_Resolve
	q[len + 1] = DNS_TYPE_A;
	q[len + 3] = DNS_CLASS_IN;
	len += 4;

	UDP_SendFrameStruct f = { mac, NET_DNS_SERVER, DNS_SERVER_PORT, local_port, q, len };
	NET_SendUDP(f);
	e.sent = true;
	e.retries++;
}

} // namespace


net_result_t NET_DNS_Resolve(const char *host, uint8_t *ip_out) {
	if (parse_ipv4(host, ip_out)) {
		return net_result_t::OK;
	}
	uint8_t scratch[DNS_NAME_MAX + 1];
	if (strlen(host) >= DNS_NAME_MAX || encode_name(host, scratch) == 0) {
		return net_result_t::UNREACHABLE;          // not a valid host name
	}
	if (NET_DNS_SERVER[0] == 0 && NET_DNS_SERVER[1] == 0 &&
	    NET_DNS_SERVER[2] == 0 && NET_DNS_SERVER[3] == 0) {
		return net_result_t::UNREACHABLE;          // no server configured
	}

	dns_entry_t *e = find(host);
	if (e != nullptr) {
		switch (e->state) {
		case dns_state_t::RESOLVED:
			if (get_tick() - e->stamp < e->ttl_ms) {
				memcpy(ip_out, e->ip, IP_ADDR_LEN);
				return net_result_t::OK;
			}
			e->state = dns_state_t::PENDING;       // expired: ask again
			e->retries = 0;
			send_query(*e);
			return e->state == dns_state_t::FAILED ? net_result_t::UNREACHABLE : net_result_t::PENDING;
		case dns_state_t::FAILED:
			return net_result_t::UNREACHABLE;
		default:
			return net_result_t::PENDING;
		}
	}

	e = alloc(host);
	if (e == nullptr) {
		return net_result_t::PENDING;              // every slot busy resolving
	}
	e->state = dns_state_t::PENDING;
	send_query(*e);
	return e->state == dns_state_t::FAILED ? net_result_t::UNREACHABLE : net_result_t::PENDING;
}

void NET_DNS_Poll() {
	const uint32_t now = get_tick();
	for (auto &e : dns_cache) {
		switch (e.state) {
		case dns_state_t::PENDING:
			if (!e.sent) {
				send_query(e);                     // server MAC may be known by now
			} else if (now - e.stamp >= DNS_RETRY_MS) {
				if (e.retries >= DNS_MAX_RETRIES) {
					fail(e);
				} else {
					send_query(e);
				}
			}
			break;
		case dns_state_t::FAILED:
			if (now - e.stamp >= DNS_FAIL_HOLD_MS) e.state = dns_state_t::FREE;
			break;
		case dns_state_t::RESOLVED:
			if (now - e.stamp >= e.ttl_ms) e.state = dns_state_t::FREE;
			break;
		default:
			break;
		}
	}
}

void NET_DNS_ProcessRx(ipv4_frame *frame) {
	const uint8_t *udp = &frame->ip_hdr[frame->ip_hdr_len];
	if (local_port == 0 || rd16(udp) != DNS_SERVER_PORT ||
	    memcmp(frame->scr_addr, NET_DNS_SERVER, IP_ADDR_LEN) != 0) {
		return;                                    // not from our server
	}
	const uint16_t udp_len = rd16(&udp[4]);
	if (udp_len < UDP_HDR_LEN + DNS_HDR_LEN || udp_len > frame->ip_len - frame->ip_hdr_len) {
		return;
	}
	const uint8_t *msg = udp + UDP_HDR_LEN;
	const uint8_t *end = udp + udp_len;

	dns_entry_t *e = nullptr;
	for (auto &x : dns_cache) {
		if (x.state == dns_state_t::PENDING && x.sent && x.id == rd16(msg)) { e = &x; break; }
	}
	if (e == nullptr || !(msg[2] & DNS_FLAG_QR)) {
		return;                                    // stale, spoofed or not a response
	}
	if ((msg[2] & DNS_FLAG_TC) || (msg[3] & DNS_RCODE_MASK) != 0) {
		fail(*e);                                  // NXDOMAIN / SERVFAIL / truncated
		return;
	}

	const uint8_t *p = msg + DNS_HDR_LEN;
	for (uint16_t q = rd16(&msg[4]); q > 0 && p != nullptr; q--) {
		p = skip_name(p, end);
		p = (p != nullptr && p + 4 <= end) ? p + 4 : nullptr;
	}
	// First A/IN record wins; CNAMEs before it are skipped
	for (uint16_t a = rd16(&msg[6]); a > 0 && p != nullptr; a--) {
		p = skip_name(p, end);
		if (p == nullptr || p + 10 > end) break;
		const uint16_t type = rd16(p);
		const uint16_t cls = rd16(p + 2);
		uint32_t ttl = BuffU8ToU32(p + 4);
		const uint16_t rdlen = rd16(p + 8);
		p += 10;
		if (p + rdlen > end) break;
		if (type == DNS_TYPE_A && cls == DNS_CLASS_IN && rdlen == IP_ADDR_LEN) {
			if (ttl < DNS_MIN_TTL_S) ttl = DNS_MIN_TTL_S;
			if (ttl > DNS_MAX_TTL_S) ttl = DNS_MAX_TTL_S;
			memcpy(e->ip, p, IP_ADDR_LEN);
			e->ttl_ms = ttl * 1000U;
			e->state = dns_state_t::RESOLVED;
			e->stamp = get_tick();
			return;
		}
		p += rdlen;
	}
	fail(*e);                                      // no usable A record
}

uint16_t NET_DNS_LocalPort() {
	return local_port;
}
