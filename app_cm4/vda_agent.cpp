/*
 * vda_agent.cpp — see vda_agent.hpp.
 */
#include <cstdio>
#include <cstring>

#include "vda_agent.hpp"

#include "chip/stm32h755/hsem.hpp"
#include "mqtt_link.hpp"
#include "net/sntp.hpp"
#include "net_config.hpp"
#include "prof.hpp"
#include "vda5050/vda5050.hpp"

namespace vda_agent {

namespace {

constexpr uint32_t PICK_DROP_DURATION_MS = 3000;   // simulated load handling

// --- topics -------------------------------------------------------------------
char topic_order[96], topic_instant[96], topic_state[96], topic_connection[96], topic_factsheet[96],
     topic_zone_set[96], topic_responses[96], topic_visualization[96];

void make_topic(char (&out)[96], const char *leaf) {
	snprintf(out, sizeof(out), "%s/v3/%s/%s/%s", netcfg::VDA_INTERFACE,
	         netcfg::VDA_MANUFACTURER, netcfg::VDA_SERIAL, leaf);
}

// --- per-topic header ids (VDA 5050 7.2) ----------------------------------------
uint32_t hid_state = 0, hid_connection = 0, hid_factsheet = 0, hid_visualization = 0;
constexpr uint32_t VIS_PERIOD_DRIVING_MS = 250;
constexpr uint32_t VIS_PERIOD_IDLE_MS = 5000;
uint32_t last_vis_ms = 0;
const char *published_conn = "ONLINE";   // last connection state published
bool shutdown_done = false;

char json_buf[10 * 1024];      // state / factsheet / visualization (publish copies)
char will_buf[256];            // CONNECTION_BROKEN, built before each CONNECT
char online_buf[256];

vda::OrderManager manager;
Stats st = {};
bool robot_driving = false;
bool cm4_driving() { return robot_driving; }

// --- RobotPort over the inter-core command ring ------------------------------------
bool send(ipc_app::Command &c) {
	if (ipc_app::shared().commands.push(c)) {
		(void) hsem::notify(ipc_app::CMD_SEM);   // wake the M7 control task
		return true;
	}
	st.cmd_ring_full += 1;
	return false;
}

class IpcRobot final : public vda::RobotPort {
public:
	bool drive_to(const vda::DriveTarget &t) override {
		ipc_app::Command c{};
		c.type = ipc_app::CmdType::DriveTo;
		c.drive.path_index = t.path_index;
		c.drive.node_seq = t.node_seq;
		c.drive.x = t.x;
		c.drive.y = t.y;
		c.drive.theta = t.theta;
		c.drive.is_node = t.is_node ? 1U : 0U;
		c.drive.has_theta = t.has_theta ? 1U : 0U;
		c.drive.max_speed = t.max_speed;
		c.drive.tolerance = t.tolerance;
		return send(c);
	}
	void stop_motion(uint32_t ack_seq) override {
		ipc_app::Command c{};
		c.type = ipc_app::CmdType::StopMotion;
		c.flag = ack_seq;
		send(c);
	}
	void set_hold(bool hold) override {
		ipc_app::Command c{};
		c.type = ipc_app::CmdType::SetHold;
		c.flag = hold ? 1U : 0U;
		send(c);
	}
	void set_speed_limit(float mps) override {
		ipc_app::Command c{};
		c.type = ipc_app::CmdType::SetSpeedLimit;
		c.speed = mps;
		send(c);
	}
	void set_paused(bool paused) override {
		ipc_app::Command c{};
		c.type = ipc_app::CmdType::SetPaused;
		c.flag = paused ? 1U : 0U;
		send(c);
	}
	bool start_action(uint8_t uid, const vda::Action &a) override {
		ipc_app::Command c{};
		c.type = ipc_app::CmdType::ActionStart;
		c.action.uid = uid;
		c.action.code = (a.kind == vda::ActionKind::Pick) ? ipc_app::ActionCode::Pick : ipc_app::ActionCode::Drop;
		c.action.duration_ms = PICK_DROP_DURATION_MS;
		return send(c);
	}
	void cancel_action(uint8_t uid) override {
		ipc_app::Command c{};
		c.type = ipc_app::CmdType::ActionCancel;
		c.action.uid = uid;
		send(c);
	}
	void set_pose(float x, float y, float theta) override {
		ipc_app::Command c{};
		c.type = ipc_app::CmdType::SetPose;
		c.pose.x = x;
		c.pose.y = y;
		c.pose.theta = theta;
		send(c);
	}
};
IpcRobot robot;

// --- MQTT session hooks --------------------------------------------------------------
void prepare_will(void *, const char **topic, const uint8_t **payload, uint16_t *len) {
	const size_t n = manager.build_connection(will_buf, sizeof(will_buf), hid_connection++,
	                                          NET_SNTP_UnixMs(), "CONNECTION_BROKEN");
	*topic = topic_connection;
	*payload = reinterpret_cast<const uint8_t *>(will_buf);
	*len = static_cast<uint16_t>(n);
}

bool publish_factsheet() {
	const size_t n = manager.build_factsheet(json_buf, sizeof(json_buf), hid_factsheet, NET_SNTP_UnixMs());
	if (n == 0 || !mqtt_link::publish(topic_factsheet, reinterpret_cast<const uint8_t *>(json_buf), n, 0, false)) {
		return false;
	}
	hid_factsheet += 1;
	st.factsheets_tx += 1;
	return true;
}

void on_online(void *) {
	(void) mqtt_link::subscribe(topic_order, 0);
	(void) mqtt_link::subscribe(topic_instant, 0);
	(void) mqtt_link::subscribe(topic_zone_set, 0);
	(void) mqtt_link::subscribe(topic_responses, 0);
	published_conn = manager.connection_state();
	const size_t n = manager.build_connection(online_buf, sizeof(online_buf), hid_connection++,
	                                          NET_SNTP_UnixMs(), published_conn);
	(void) mqtt_link::publish(topic_connection, reinterpret_cast<const uint8_t *>(online_buf), n, 1, true);
	(void) publish_factsheet();
	// a fresh state right away (event-driven path picks it up)
	manager.state_sent(0);
}

bool topic_is(const char *topic, uint16_t len, const char *want) {
	return strlen(want) == len && memcmp(topic, want, len) == 0;
}

void on_message(void *, const char *topic, uint16_t tlen, const uint8_t *payload, size_t len, uint8_t, bool) {
	prof::Scope prof_scope(prof::OnMessage);
	const char *text = reinterpret_cast<const char *>(payload);
	if (topic_is(topic, tlen, topic_order)) {
		st.orders_rx += 1;
		manager.on_order(text, len);
	} else if (topic_is(topic, tlen, topic_instant)) {
		st.instant_rx += 1;
		manager.on_instant_actions(text, len);
	} else if (topic_is(topic, tlen, topic_zone_set)) {
		manager.on_zone_set(text, len);
	} else if (topic_is(topic, tlen, topic_responses)) {
		manager.on_responses(text, len);
	}
}

} // namespace

void init() {
	make_topic(topic_order, "order");
	make_topic(topic_instant, "instantActions");
	make_topic(topic_state, "state");
	make_topic(topic_connection, "connection");
	make_topic(topic_factsheet, "factsheet");
	make_topic(topic_zone_set, "zoneSet");
	make_topic(topic_responses, "responses");
	make_topic(topic_visualization, "visualization");

	vda::Config cfg{};
	cfg.manufacturer = netcfg::VDA_MANUFACTURER;
	cfg.serial = netcfg::VDA_SERIAL;
	cfg.map_id = netcfg::VDA_MAP_ID;
	cfg.series_name = netcfg::VDA_SERIES;
	cfg.default_dev_xy = 0.5F;
	cfg.max_speed = 0.5F;                 // MotionSim::DEFAULT_SPEED
	cfg.state_interval_ms = 10000;        // VDA 5050: at least every 30 s
	cfg.state_interval_driving_ms = 1000;
	cfg.min_state_interval_ms = 100;
	cfg.map_download_ms = 3000;           // simulated map transfer
	manager.init(cfg, &robot);

	mqtt_link::init({ prepare_will, on_online, on_message, nullptr });
}

void update_robot(const ipc_app::RtState &s) {
	vda::RobotStatus r{};
	r.valid = true;           // the real-time core is alive and reporting
	r.x = s.x;
	r.y = s.y;
	r.theta = s.theta;
	r.vx = s.vx;
	r.vy = 0.0F;
	r.omega = s.omega;
	r.driving = s.driving != 0U;
	robot_driving = r.driving;
	r.paused = s.paused != 0U;
	r.loaded = s.loaded != 0U;
	r.battery_soc = s.battery_soc;
	r.any_popped = s.any_popped != 0U;
	r.last_popped = s.last_popped;
	r.stop_ack = s.stop_ack;
	r.queued = s.queued_targets;
	manager.update_robot(r);
}

void on_event(const ipc_app::Event &ev) {
	switch (ev.type) {
	case ipc_app::EventType::NodeReached:
		manager.on_node_reached(ev.arg[0]);
		break;
	case ipc_app::EventType::ActionDone:
		manager.on_action_done(static_cast<uint8_t>(ev.arg[0]), ev.arg[1] != 0U);
		break;
	default:
		break;
	}
}

void poll(uint32_t now_ms) {
	const uint64_t unix_ms = NET_SNTP_UnixMs();
	{
		prof::Scope p(prof::VdaLogic);
		manager.poll(now_ms, unix_ms);
	}
	if (shutdown_done || !mqtt_link::online()) {
		return;
	}
	if (manager.factsheet_requested() && publish_factsheet()) {
		manager.factsheet_sent();
	}
	// connection state changes (hibernation, shutdown): retained, QoS 1. Before
	// the state, so a wake-up announces ONLINE first; HIBERNATING / OFFLINE follow
	// the final state because state_sent() switches them.
	const char *conn = manager.connection_state();
	if (strcmp(conn, published_conn) != 0) {
		const size_t n = manager.build_connection(online_buf, sizeof(online_buf), hid_connection, unix_ms, conn);
		if (mqtt_link::publish(topic_connection, reinterpret_cast<const uint8_t *>(online_buf), n, 1, true)) {
			hid_connection += 1;
			published_conn = conn;
			if (manager.shutdown_requested()) {
				mqtt_link::shutdown();       // OFFLINE is out: leave the broker
				shutdown_done = true;
				return;
			}
		}
	}
	if (manager.state_due(now_ms)) {
		size_t n;
		{
			prof::Scope p(prof::StateBuild);
			n = manager.build_state(json_buf, sizeof(json_buf), hid_state, unix_ms);
		}
		prof::Scope p(prof::StatePublish);
		if (n == 0) {
			st.state_build_fail += 1;
			manager.state_sent(now_ms);      // do not spin on an oversize state
		} else if (mqtt_link::publish(topic_state, reinterpret_cast<const uint8_t *>(json_buf), n, 0, false)) {
			hid_state += 1;
			st.states_tx += 1;
			manager.state_sent(now_ms);
		} else {
			st.publish_busy += 1;            // TX buffer full: retry next pass
		}
	}
	// visualization (6.7): fast while driving, slow otherwise, none while hibernating
	const uint32_t vis_period = cm4_driving() ? VIS_PERIOD_DRIVING_MS : VIS_PERIOD_IDLE_MS;
	if (!manager.hibernating() && (now_ms - last_vis_ms) >= vis_period) {
		prof::Scope p(prof::Visualization);
		const size_t n = manager.build_visualization(json_buf, sizeof(json_buf), hid_visualization, unix_ms,
		                                             hid_state > 0 ? hid_state - 1U : 0U);
		if (n > 0 && mqtt_link::publish(topic_visualization, reinterpret_cast<const uint8_t *>(json_buf), n, 0, false)) {
			hid_visualization += 1;
			last_vis_ms = now_ms;
		}
	}
}

const Stats &stats() {
	return st;
}

} // namespace vda_agent
