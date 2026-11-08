#pragma once
/*
 * Network / broker settings of the Cortex-M4 network application.
 *
 * The defaults target the bench set-up: the board's Ethernet port wired
 * straight to a PC (adapter address 192.168.0.10/24) that runs
 * tests/broker (mosquitto + sntp_server.py). Without a DHCP server the
 * board falls back to STATIC_IP. With a DHCP server on the network, DHCP wins
 * and BROKER_IP/SNTP_SERVER must be reachable from the leased address.
 *
 * Credentials and addresses here are TEST values.
 */
#include <cstdint>

namespace netcfg {

// Fallback address when DHCP gets no answer
constexpr uint8_t STATIC_IP[4]   = { 192, 168, 0, 123 };
constexpr uint8_t STATIC_MASK[4] = { 255, 255, 255, 0 };
constexpr uint8_t STATIC_GW[4]   = { 0, 0, 0, 0 };

// Time source (SNTP). TLS needs wall-clock time to check certificates.
constexpr uint8_t SNTP_SERVER[4] = { 192, 168, 0, 10 };

// MQTT broker
constexpr uint8_t BROKER_IP[4]   = { 192, 168, 0, 10 };
constexpr uint16_t BROKER_PORT   = 8883;
// Name the broker certificate must contain (SAN DNS name or IP address)
constexpr const char *BROKER_NAME = "192.168.0.10";
constexpr const char *CLIENT_ID   = "mcore-agv-001";
constexpr const char *USERNAME    = "agv001";
constexpr const char *PASSWORD    = "mcore-test";
constexpr uint16_t KEEPALIVE_S    = 30;

// VDA 5050 identity: topics are <interface>/v3/<manufacturer>/<serial>/<topic>
constexpr const char *VDA_INTERFACE    = "vda5050";
constexpr const char *VDA_MANUFACTURER = "MCORE";
constexpr const char *VDA_SERIAL       = "agv001";
constexpr const char *VDA_SERIES       = "MCORE-H755";
constexpr const char *VDA_MAP_ID       = "local";   // the map the robot is localised in

} // namespace netcfg
