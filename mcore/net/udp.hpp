/*
 * udp.hpp — UDP sockets for the application: bind a port to a handler,
 *           send a datagram to any address
 *
 * The stack's own UDP users (DHCP, DNS, NTP) keep their fixed hooks in
 * NET_ProcessUDP; application ports go through this table. A datagram to a
 * port nobody bound gets ICMP port unreachable, as before.
 *
 *   void on_cmd(void *, const uint8_t *ip, uint16_t port, const uint8_t *d, uint16_t n) { ... }
 *   NET_UDP_Bind(4001, on_cmd);
 *   NET_UDP_SendTo(4001, ip, port, reply, len);   // from the handler or later
 */
#pragma once

#include <cstdint>
#include "net/net.hpp"
#include "net/arp.hpp"

constexpr uint8_t NET_UDP_MAX_BINDS = 4;

// Largest payload of one datagram (no IP fragmentation)
constexpr uint16_t NET_UDP_MAX_PAYLOAD = ETH_MAX_PACKET_SIZE - UDP_TEMPLATE_FRAME_LEN;

// Called from the RX path (ETH_RxWorker) with the payload of a datagram to
// a bound port. The pointers are valid only during the call.
using udp_rx_fn = void (*)(void *user, const uint8_t *src_ip, uint16_t src_port,
                           const uint8_t *data, uint16_t len);

// False if the port is 0, already bound, or all NET_UDP_MAX_BINDS slots are used.
bool NET_UDP_Bind(uint16_t port, udp_rx_fn fn, void *user = nullptr);
void NET_UDP_Unbind(uint16_t port);

// One datagram from local port src_port. Non-blocking like the rest of the
// stack: PENDING while the next hop's MAC is being resolved or the MAC's TX
// buffer is busy (the datagram is NOT queued: send it again), UNREACHABLE
// without a route or for len > NET_UDP_MAX_PAYLOAD.
net_result_t NET_UDP_SendTo(uint16_t src_port, const uint8_t *dst_ip, uint16_t dst_port,
                            const uint8_t *data, uint16_t len);

// RX path (NET_ProcessUDP): hands a datagram to its bound handler. False if
// the port is not bound; malformed datagrams are dropped and count as handled.
bool NET_UDP_Deliver(const ipv4_frame *frame, uint16_t dst_port);
