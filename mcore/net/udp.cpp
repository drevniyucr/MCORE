/*
 * udp.cpp — UDP sockets for the application (bind table, send with ARP)
 */
#include "net/udp.hpp"

namespace {

struct udp_bind_t {
	uint16_t port;      // 0 = free slot
	udp_rx_fn fn;
	void *user;
};

udp_bind_t binds[NET_UDP_MAX_BINDS] = { };

uint16_t rd16(const uint8_t *p) {
	return static_cast<uint16_t>((p[0] << 8) | p[1]);
}

} // namespace

bool NET_UDP_Bind(uint16_t port, udp_rx_fn fn, void *user) {
	if (port == 0 || fn == nullptr) {
		return false;
	}
	udp_bind_t *free_slot = nullptr;
	for (auto &b : binds) {
		if (b.port == port) {
			return false;
		}
		if (b.port == 0 && free_slot == nullptr) {
			free_slot = &b;
		}
	}
	if (free_slot == nullptr) {
		return false;
	}
	*free_slot = { port, fn, user };
	return true;
}

void NET_UDP_Unbind(uint16_t port) {
	for (auto &b : binds) {
		if (b.port == port) {
			b = { };
		}
	}
}

bool NET_UDP_Deliver(const ipv4_frame *frame, uint16_t dst_port) {
	for (const auto &b : binds) {
		if (b.port != 0 && b.port == dst_port) {
			const uint8_t *udp = &frame->ip_hdr[frame->ip_hdr_len];
			// The UDP length must cover its header and stay inside the IP payload
			const uint16_t udp_len = rd16(&udp[4]);
			if (udp_len < UDP_HDR_LEN || udp_len > frame->ip_len - frame->ip_hdr_len) {
				return true;
			}
			b.fn(b.user, frame->scr_addr, rd16(&udp[0]), udp + UDP_HDR_LEN,
			     static_cast<uint16_t>(udp_len - UDP_HDR_LEN));
			return true;
		}
	}
	return false;
}

net_result_t NET_UDP_SendTo(uint16_t src_port, const uint8_t *dst_ip, uint16_t dst_port,
                            const uint8_t *data, uint16_t len) {
	if (len > NET_UDP_MAX_PAYLOAD) {
		return net_result_t::UNREACHABLE;
	}
	uint8_t mac[MAC_ADDR_LEN];
	const net_result_t arp = NET_ARP_Resolve(dst_ip, mac);
	if (arp != net_result_t::OK) {
		return arp;
	}
	if (!ETH_IsTxBufferAvailable()) {
		return net_result_t::PENDING;
	}
	NET_SendUDP({ mac, dst_ip, dst_port, src_port, data, len });
	return net_result_t::OK;
}
