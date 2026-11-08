#pragma once
/*
 * vda_agent — VDA 5050 3.0.0 mobile-robot client on the Cortex-M4.
 *
 * Glue between the protocol logic (mcore/vda5050), the broker session
 * (mqtt_link) and the real-time core (ipc_shared.hpp):
 *   topics  vda5050/v3/<manufacturer>/<serial>/{order,instantActions,state,
 *           connection,factsheet}  (cm4/net_config.hpp)
 *   MQTT    order/instantActions subscribed with QoS 0, state/factsheet
 *           published with QoS 0, connection with QoS 1 + retain, will =
 *           CONNECTION_BROKEN (VDA 5050 4.1, 6.5)
 *   M7      DriveTo / StopMotion / SetPaused / SetPose / ActionStart /
 *           ActionCancel commands; NodeReached / ActionDone events; motion
 *           state from the snapshot
 */
#include <cstdint>

#include "ipc_shared.hpp"

namespace vda_agent {

void init();
void poll(uint32_t now_ms);                // main loop
void on_event(const ipc_app::Event &ev);   // NodeReached / ActionDone from the M7
void update_robot(const ipc_app::RtState &s);

struct Stats {
	uint32_t orders_rx;
	uint32_t instant_rx;
	uint32_t states_tx;
	uint32_t state_build_fail;     // state JSON did not fit the buffer
	uint32_t publish_busy;         // TX buffer full, retried later
	uint32_t cmd_ring_full;        // command to the M7 dropped
	uint32_t factsheets_tx;
};
const Stats &stats();

} // namespace vda_agent
