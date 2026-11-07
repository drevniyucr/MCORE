// Shared fixtures for the VDA 5050 tests (test_vda.cpp, test_vda_ext.cpp):
// a fake RobotPort, JSON builders and a Rig that drives an OrderManager.
#pragma once

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "doctest.h"
#include "vda5050/json.hpp"
#include "vda5050/vda5050.hpp"

namespace vda_test {

struct FakeRobot : vda::RobotPort {
	std::vector<vda::DriveTarget> targets;
	std::vector<uint8_t> started, cancelled;
	int stops = 0;
	uint32_t last_stop_seq = 0;
	int pause_calls = 0;
	bool paused_req = false;
	bool hold = false;
	float speed_limit = 0;
	float px = 0, py = 0, pth = 0;

	bool drive_to(const vda::DriveTarget &t) override { targets.push_back(t); return true; }
	void stop_motion(uint32_t seq) override { ++stops; last_stop_seq = seq; }
	void set_paused(bool p) override { ++pause_calls; paused_req = p; }
	void set_hold(bool h) override { hold = h; }
	void set_speed_limit(float v) override { speed_limit = v; }
	bool start_action(uint8_t uid, const vda::Action &) override { started.push_back(uid); return true; }
	void cancel_action(uint8_t uid) override { cancelled.push_back(uid); }
	void set_pose(float x, float y, float th) override { px = x; py = y; pth = th; }

	// node sequence ids handed to the robot so far
	std::vector<uint32_t> nodes() const {
		std::vector<uint32_t> v;
		for (const auto &t : targets) if (t.is_node) v.push_back(t.node_seq);
		return v;
	}
	const vda::DriveTarget *node_target(uint32_t seq) const {
		for (const auto &t : targets) if (t.is_node && t.node_seq == seq) return &t;
		return nullptr;
	}
};

inline vda::Config config() {
	vda::Config c{};
	c.manufacturer = "MCORE";
	c.serial = "agv001";
	c.map_id = "local";
	c.series_name = "MCORE-H755";
	c.default_dev_xy = 0.5F;
	c.max_speed = 1.0F;
	c.state_interval_ms = 10000;
	c.state_interval_driving_ms = 1000;
	c.min_state_interval_ms = 100;
	c.map_download_ms = 2000;
	return c;
}

struct NodeSpec {
	const char *id;
	float x, y;
	bool released;
	std::string actions = "";   // JSON array body
};

inline std::string order_json(const char *order_id, uint32_t update, uint32_t first_seq,
		const std::vector<NodeSpec> &nodes, const std::vector<std::string> &edge_actions = {},
		const char *map = "local", const char *edge_extra = "") {
	std::string s = "{\"headerId\":1,\"timestamp\":\"2026-11-07T10:00:00.000Z\",\"version\":\"3.0.0\","
	                "\"manufacturer\":\"MCORE\",\"serialNumber\":\"agv001\",";
	s += "\"orderId\":\"" + std::string(order_id) + "\",\"orderUpdateId\":" + std::to_string(update) + ",\"nodes\":[";
	for (size_t i = 0; i < nodes.size(); ++i) {
		const NodeSpec &n = nodes[i];
		if (i) s += ",";
		char pos[160];
		snprintf(pos, sizeof(pos), "{\"x\":%.3f,\"y\":%.3f,\"theta\":0.0,\"mapId\":\"%s\"}", n.x, n.y, map);
		s += "{\"nodeId\":\"" + std::string(n.id) + "\",\"sequenceId\":" + std::to_string(first_seq + 2 * i) +
		     ",\"released\":" + (n.released ? "true" : "false") + ",\"nodePosition\":" + pos +
		     ",\"actions\":[" + n.actions + "]}";
	}
	s += "],\"edges\":[";
	for (size_t i = 0; i + 1 < nodes.size(); ++i) {
		if (i) s += ",";
		const bool rel = nodes[i].released && nodes[i + 1].released;
		const std::string acts = i < edge_actions.size() ? edge_actions[i] : "";
		s += "{\"edgeId\":\"e" + std::to_string(i) + "\",\"sequenceId\":" + std::to_string(first_seq + 2 * i + 1) +
		     ",\"released\":" + (rel ? "true" : "false") + edge_extra + ",\"actions\":[" + acts + "]}";
	}
	s += "]}";
	return s;
}

inline std::string action(const char *type, const char *id, const char *blocking, const char *params = "") {
	std::string s = "{\"actionType\":\"" + std::string(type) + "\",\"actionId\":\"" + id +
	                "\",\"blockingType\":\"" + blocking + "\"";
	if (params[0]) s += ",\"actionParameters\":[" + std::string(params) + "]";
	return s + "}";
}

inline std::string instant(const std::string &actions) {
	return "{\"headerId\":7,\"timestamp\":\"2026-11-07T10:00:00.000Z\",\"version\":\"3.0.0\","
	       "\"manufacturer\":\"MCORE\",\"serialNumber\":\"agv001\",\"actions\":[" + actions + "]}";
}

struct Rig {
	FakeRobot robot;
	vda::OrderManager m;
	vda::RobotStatus st{};
	uint32_t now = 1000;

	Rig() {
		m.init(config(), &robot);
		st.valid = true;
		st.battery_soc = 100.0F;
		m.update_robot(st);
	}
	void order(const std::string &s) { m.on_order(s.data(), s.size()); m.poll(now, unix_ms()); }
	void inst(const std::string &actions) {
		const std::string s = instant(actions);
		m.on_instant_actions(s.data(), s.size());
		m.poll(now, unix_ms());
	}
	// The robot arrives at node seq: every waypoint up to it counts as reached.
	void reach(uint32_t seq) {
		const vda::DriveTarget *t = robot.node_target(seq);
		REQUIRE_MESSAGE(t != nullptr, "node was never sent to the robot: ", seq);
		pop_to(t->path_index);
		m.on_node_reached(seq);
		m.poll(now, unix_ms());
	}
	// Waypoints up to path index i reached (robot standing on it)
	void pop_to(uint32_t i) {
		for (const auto &t : robot.targets) {
			if (t.path_index == i) { st.x = t.x; st.y = t.y; }
		}
		st.any_popped = true;
		st.last_popped = i;
		m.update_robot(st);
		m.poll(now, unix_ms());
	}
	void tick(uint32_t ms = 10) { now += ms; m.poll(now, unix_ms()); }
	uint64_t unix_ms() const { return 1794000000000ULL + now; }
	void robot_state(bool driving, bool paused) { st.driving = driving; st.paused = paused; m.update_robot(st); m.poll(now, unix_ms()); }
	void at(float x, float y) { st.x = x; st.y = y; m.update_robot(st); m.poll(now, unix_ms()); }
	void ack_stops() { st.stop_ack = robot.last_stop_seq; m.update_robot(st); m.poll(now, unix_ms()); }
	std::string state() {
		static char buf[16384];
		const size_t n = m.build_state(buf, sizeof(buf), 42, 1794000000123ULL);
		REQUIRE(n > 0);
		return std::string(buf, n);
	}
};

inline const std::vector<NodeSpec> LINE3 = { { "n0", 0, 0, true }, { "n1", 1, 0, true }, { "n2", 2, 0, true } };

inline void dump(const char *name, const std::string &text) {
#if defined(MCORE_VDA_OUT_DIR)
	FILE *f = fopen((std::string(MCORE_VDA_OUT_DIR) + "/" + name).c_str(), "wb");
	if (f) {
		fwrite(text.data(), 1, text.size(), f);
		fclose(f);
	}
#else
	(void) name; (void) text;
#endif
}

} // namespace vda_test
