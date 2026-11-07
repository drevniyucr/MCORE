// Tests for the VDA 5050 3.0.0 layer (mcore/vda5050): JSON reader/writer,
// order acceptance and rejection (6.1), progress and actions (6.2, 6.6),
// instant actions. A fake RobotPort records what the robot was told.
// Generated messages are also written to MCORE_VDA_OUT_DIR, where
// tests/vda/validate.py checks them against the official JSON schemas.
#include "doctest.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "vda5050/json.hpp"
#include "vda5050/vda5050.hpp"

#include "vda_rig.hpp"

using namespace vda_test;

// ---------------------------------------------------------------------------
// JSON
// ---------------------------------------------------------------------------
TEST_CASE("JSON reader: nested values, escapes, numbers, skipping") {
	const char *t = "{\"a\":\"x\\\"y\\u00e9\\n\",\"n\":-12.5e1,\"b\":true,\"z\":null,"
	                "\"arr\":[1,{\"k\":[2,3]},\"s\"],\"i\":42}";
	json::JsonReader r(t, strlen(t));
	REQUIRE(r.enter_object());
	json::Span k;
	char s[32];
	double d = 0;
	bool b = false;
	int64_t i = 0;
	REQUIRE(r.next_key(k)); CHECK(k == "a"); REQUIRE(r.read_string(s, sizeof(s)));
	CHECK(std::string(s) == "x\"y\xc3\xa9\n");
	REQUIRE(r.next_key(k)); CHECK(k == "n"); REQUIRE(r.read_number(d)); CHECK(d == -125.0);
	REQUIRE(r.next_key(k)); CHECK(k == "b"); REQUIRE(r.read_bool(b)); CHECK(b);
	REQUIRE(r.next_key(k)); CHECK(k == "z"); CHECK(r.peek_null());
	REQUIRE(r.next_key(k)); CHECK(k == "arr"); CHECK(r.skip());
	REQUIRE(r.next_key(k)); CHECK(k == "i"); REQUIRE(r.read_int(i)); CHECK(i == 42);
	CHECK_FALSE(r.next_key(k));
	CHECK(r.ok());
}

TEST_CASE("JSON reader: errors latch, strings longer than the buffer fail") {
	const char *bad = "{\"a\":tru}";
	json::JsonReader r(bad, strlen(bad));
	json::Span k;
	bool b;
	REQUIRE(r.enter_object());
	REQUIRE(r.next_key(k));
	CHECK_FALSE(r.read_bool(b));
	CHECK_FALSE(r.ok());

	const char *lng = "\"abcdefgh\"";
	json::JsonReader r2(lng, strlen(lng));
	char small[4];
	CHECK_FALSE(r2.read_string(small, sizeof(small)));
	const char *frac = "1.5";
	json::JsonReader r3(frac, 3);
	int64_t i;
	CHECK_FALSE(r3.read_int(i));
}

TEST_CASE("JSON writer: commas, escapes, numbers without printf") {
	char buf[256];
	json::JsonWriter w(buf, sizeof(buf));
	w.begin_object().kv("s", "a\"b\\").kv("x", 1.25, 3).kv("neg", -0.5, 2).kv("i", 1.0)
	 .kv_int("n", -7).key("arr").begin_array().integer(1).str("t").boolean(false).null().end_array()
	 .key("o").begin_object().end_object().end_object();
	REQUIRE(w.ok());
	CHECK(std::string(buf) ==
	      "{\"s\":\"a\\\"b\\\\\",\"x\":1.25,\"neg\":-0.5,\"i\":1,\"n\":-7,\"arr\":[1,\"t\",false,null],\"o\":{}}");

	char tiny[8];
	json::JsonWriter t(tiny, sizeof(tiny));
	t.begin_object().kv("long", "string").end_object();
	CHECK_FALSE(t.ok());
}

TEST_CASE("ISO 8601 timestamps (UTC, milliseconds)") {
	char ts[25];
	json::format_iso8601(0, ts);
	CHECK(std::string(ts) == "1970-01-01T00:00:00.000Z");
	json::format_iso8601(1767225600123ULL, ts);
	CHECK(std::string(ts) == "2026-01-01T00:00:00.123Z");
	json::format_iso8601(1709164800000ULL + 3723004, ts);   // leap day + 01:02:03.004
	CHECK(std::string(ts) == "2024-02-29T01:02:03.004Z");
}

// ---------------------------------------------------------------------------
// Orders
// ---------------------------------------------------------------------------
TEST_CASE("Order: accepted, first node traversed, released nodes sent to the robot") {
	Rig g;
	g.order(order_json("o1", 0, 0, LINE3));
	CHECK(g.m.error_count() == 0);
	CHECK(std::string(g.m.order_id()) == "o1");
	CHECK(std::string(g.m.last_node_id()) == "n0");
	CHECK(g.m.node_states() == 2);
	CHECK(g.m.edge_states() == 2);
	CHECK_FALSE(g.m.idle());
	// waypoints every 0.1 m; the first node is the 10th one
	REQUIRE(g.robot.targets.size() == vda::STREAM_AHEAD);
	CHECK(g.robot.targets[0].x == doctest::Approx(0.1));
	CHECK_FALSE(g.robot.targets[0].is_node);
	REQUIRE(g.robot.nodes().size() == 1);
	CHECK(g.robot.node_target(2)->x == doctest::Approx(1.0));
	CHECK(g.robot.node_target(2)->path_index == g.robot.targets[9].path_index);

	g.reach(2);
	CHECK(std::string(g.m.last_node_id()) == "n1");
	CHECK(g.m.last_node_seq() == 2);
	CHECK(g.m.node_states() == 1);
	CHECK(g.m.edge_states() == 1);
	REQUIRE(g.robot.node_target(4) != nullptr);   // streamed as the robot advanced
	g.reach(4);
	CHECK(g.m.node_states() == 0);
	CHECK(g.m.edge_states() == 0);
	CHECK(g.m.idle());
	dump("state_after_order.json", g.state());
}

TEST_CASE("Order rejections (6.1.4)") {
	SUBCASE("malformed: edge count does not match") {
		Rig g;
		std::string s = order_json("o1", 0, 0, LINE3);
		const size_t pos = s.find(",{\"edgeId\":\"e1\"");
		s.erase(pos, s.find("]}", pos) - pos);   // drop the second edge
		g.order(s);
		CHECK(g.m.has_error("VALIDATION_FAILURE"));
		CHECK(g.robot.targets.empty());
		dump("state_with_error.json", g.state());
	}
	SUBCASE("start node out of range") {
		Rig g;
		g.order(order_json("o1", 0, 0, { { "far", 5, 5, true } }));
		CHECK(g.m.has_error("START_NODE_OUT_OF_RANGE"));
	}
	SUBCASE("unknown map") {
		Rig g;
		g.order(order_json("o1", 0, 0, LINE3, {}, "other"));
		CHECK(g.m.has_error("UNKNOWN_MAP_ID"));
	}
	SUBCASE("unsupported action type") {
		Rig g;
		auto nodes = LINE3;
		nodes[1].actions = action("dance", "a1", "NONE");
		g.order(order_json("o1", 0, 0, nodes));
		CHECK(g.m.has_error("INVALID_ORDER_ACTION"));
	}
	SUBCASE("invalid trajectory (degree 2 needs 3 control points)") {
		Rig g;
		g.order(order_json("o1", 0, 0, LINE3, {}, "local",
		        ",\"trajectory\":{\"degree\":2,\"controlPoints\":[{\"x\":0,\"y\":0},{\"x\":1,\"y\":0}]}"));
		CHECK(g.m.has_error("VALIDATION_FAILURE"));
	}
	SUBCASE("robot not ready") {
		Rig g;
		g.st.valid = false;
		g.m.update_robot(g.st);
		g.order(order_json("o1", 0, 0, LINE3));
		CHECK(g.m.has_error("MOBILE_ROBOT_NOT_AVAILABLE"));
	}
	SUBCASE("other order active, outdated and duplicate updates") {
		Rig g;
		g.order(order_json("o1", 3, 0, LINE3));
		g.order(order_json("o2", 0, 0, LINE3));
		CHECK(g.m.has_error("OTHER_ORDER_ACTIVE"));
		g.order(order_json("o1", 2, 0, LINE3));
		CHECK(g.m.has_error("OUTDATED_ORDER_UPDATE"));
		const uint8_t before = g.m.error_count();
		g.order(order_json("o1", 3, 0, LINE3));                 // identical resend
		CHECK(g.m.error_count() == before);
		CHECK_FALSE(g.m.has_error("SAME_ORDER_UPDATE_ID"));
		auto changed = LINE3;
		changed[2].x = 3;
		g.order(order_json("o1", 3, 0, changed));
		CHECK(g.m.has_error("SAME_ORDER_UPDATE_ID"));
		CHECK(std::string(g.m.order_id()) == "o1");             // kept
	}
	SUBCASE("a new accepted order clears order errors") {
		Rig g;
		g.order(order_json("o1", 0, 0, { { "far", 5, 5, true } }));
		REQUIRE(g.m.has_error("START_NODE_OUT_OF_RANGE"));
		g.order(order_json("o2", 0, 0, LINE3));
		CHECK(g.m.error_count() == 0);
	}
}

TEST_CASE("Order update: horizon is replaced, robot continues past the decision point") {
	Rig g;
	// base n0-n1, horizon n2
	g.order(order_json("o1", 0, 0, { { "n0", 0, 0, true }, { "n1", 1, 0, true }, { "n2", 2, 0, false } }));
	CHECK(g.robot.targets.size() == 10);                      // stops at the decision point
	CHECK(g.robot.nodes() == std::vector<uint32_t>{ 2 });
	CHECK(g.m.node_states() == 2);                            // n1 + horizon n2

	// update must start at the decision point n1 (seq 2)
	g.order(order_json("o1", 1, 0, { { "n0", 0, 0, true }, { "nX", 9, 0, true } }));
	CHECK(g.m.has_error("VALIDATION_FAILURE"));
	g.order(order_json("o1", 1, 2, { { "n1", 1, 0, true }, { "n2", 2, 0, true }, { "n3", 3, 0, true } }));
	CHECK_FALSE(g.m.has_error("VALIDATION_FAILURE"));
	CHECK(g.m.order_update_id() == 1);
	CHECK(g.robot.targets.size() == vda::STREAM_AHEAD);       // streaming resumed
	g.reach(2);
	g.reach(4);
	g.reach(6);
	CHECK(std::string(g.m.last_node_id()) == "n3");
	CHECK(g.m.idle());
}

TEST_CASE("Order update after the robot finished its base") {
	Rig g;
	g.order(order_json("o1", 0, 0, { { "n0", 0, 0, true }, { "n1", 1, 0, true } }));
	g.reach(2);
	REQUIRE(g.m.idle());
	g.order(order_json("o1", 1, 2, { { "n1", 1, 0, true }, { "n2", 2, 0, true } }));
	CHECK(g.robot.nodes() == std::vector<uint32_t>({ 2, 4 }));
	CHECK(g.robot.node_target(4)->x == doctest::Approx(2.0));
	CHECK(g.m.edge_states() == 1);
}

// ---------------------------------------------------------------------------
// Actions
// ---------------------------------------------------------------------------
TEST_CASE("HARD node action: robot stops at the node until the action is done") {
	Rig g;
	auto nodes = LINE3;
	nodes[1].actions = action("pick", "pick1", "HARD", "{\"key\":\"loadId\",\"value\":\"L7\"}");
	g.order(order_json("o1", 0, 0, nodes));
	CHECK(g.robot.targets.size() == 10);                      // only up to n1
	CHECK(std::string(vda::to_string(g.m.find_action("pick1")->status)) == "WAITING");

	g.reach(2);
	REQUIRE(g.robot.started.size() == 1);
	CHECK(g.m.find_action("pick1")->status == vda::ActionStatus::Running);
	CHECK(g.robot.targets.size() == 10);                      // still holding

	g.m.on_action_done(g.robot.started[0], true);
	g.tick();
	CHECK(g.m.find_action("pick1")->status == vda::ActionStatus::Finished);
	CHECK(g.robot.node_target(4) != nullptr);                 // driving on
}

TEST_CASE("NONE node action runs while driving on; SINGLE waits for it") {
	Rig g;
	auto nodes = LINE3;
	nodes[1].actions = action("waitForTrigger", "w1", "NONE") + "," + action("finePositioning", "f1", "SINGLE");
	g.order(order_json("o1", 0, 0, nodes));
	CHECK(g.robot.targets.size() == vda::STREAM_AHEAD);       // nothing blocks driving
	g.reach(2);
	CHECK(g.m.find_action("w1")->status == vda::ActionStatus::Running);
	CHECK(g.m.find_action("f1")->status == vda::ActionStatus::Waiting);   // exclusive: waits

	g.inst(action("trigger", "t1", "NONE"));
	CHECK(g.m.find_action("w1")->status == vda::ActionStatus::Finished);
	CHECK(g.m.find_instant("t1")->status == vda::ActionStatus::Finished);
	g.tick();
	CHECK(g.m.find_action("f1")->status == vda::ActionStatus::Finished);
}

TEST_CASE("SOFT edge action: robot waits at the start node, edge end finishes edge actions") {
	Rig g;
	g.order(order_json("o1", 0, 0, LINE3, { action("drop", "d1", "SOFT") }));
	// drop runs on edge e0 (entered right away): driving blocked until it is done
	REQUIRE(g.robot.started.size() == 1);
	CHECK(g.robot.targets.empty());
	g.m.on_action_done(g.robot.started[0], true);
	g.tick();
	CHECK(g.robot.targets.size() == vda::STREAM_AHEAD);
	dump("state_with_actions.json", g.state());
}

TEST_CASE("Horizon actions are reported as WAITING and replaced by an update") {
	Rig g;
	auto nodes = std::vector<NodeSpec>{ { "n0", 0, 0, true }, { "n1", 1, 0, true }, { "n2", 2, 0, false } };
	nodes[2].actions = action("pick", "hp", "HARD");
	g.order(order_json("o1", 0, 0, nodes));
	REQUIRE(g.m.find_action("hp") != nullptr);
	CHECK(g.m.find_action("hp")->status == vda::ActionStatus::Waiting);
	g.order(order_json("o1", 1, 2, { { "n1", 1, 0, true }, { "n2b", 2, 1, true } }));
	CHECK(g.m.find_action("hp") == nullptr);
}

// ---------------------------------------------------------------------------
// Instant actions
// ---------------------------------------------------------------------------
TEST_CASE("cancelOrder: stops the robot, fails pending actions, finishes when stopped") {
	Rig g;
	auto nodes = LINE3;
	nodes[2].actions = action("pick", "p2", "HARD");
	g.order(order_json("o1", 0, 0, nodes));
	g.robot_state(true, false);
	g.inst(action("cancelOrder", "c1", "NONE"));
	CHECK(g.robot.stops == 1);
	CHECK(g.m.node_states() == 0);
	CHECK(g.m.edge_states() == 0);
	CHECK(g.m.find_action("p2")->status == vda::ActionStatus::Failed);
	CHECK(g.m.find_instant("c1")->status == vda::ActionStatus::Running);
	CHECK(std::string(g.m.order_id()) == "o1");              // ids kept
	g.robot_state(false, false);
	CHECK(g.m.find_instant("c1")->status == vda::ActionStatus::Finished);
	CHECK(g.m.idle());

	g.order(order_json("o1", 1, 0, LINE3));
	CHECK(g.m.has_error("ORDER_UPDATE_FOLLOWING_CANCEL"));
	g.order(order_json("o2", 0, 0, LINE3));                  // a new order is fine
	CHECK(g.m.error_count() == 0);
}

TEST_CASE("cancelOrder while idle fails with NO_ORDER_TO_CANCEL") {
	Rig g;
	g.inst(action("cancelOrder", "c1", "NONE"));
	CHECK(g.m.find_instant("c1")->status == vda::ActionStatus::Failed);
	CHECK(g.m.has_error("NO_ORDER_TO_CANCEL"));
}

TEST_CASE("startPause / stopPause finish when the robot reports the mode") {
	Rig g;
	g.inst(action("startPause", "p1", "NONE"));
	CHECK(g.robot.paused_req);
	CHECK(g.m.find_instant("p1")->status == vda::ActionStatus::Running);
	g.robot_state(false, true);
	CHECK(g.m.find_instant("p1")->status == vda::ActionStatus::Finished);
	CHECK(g.m.paused());
	g.inst(action("stopPause", "p2", "NONE"));
	g.robot_state(false, false);
	CHECK(g.m.find_instant("p2")->status == vda::ActionStatus::Finished);
}

TEST_CASE("Instant actions: unknown type, wrong blocking, initializePosition, factsheet, clear") {
	Rig g;
	g.inst(action("teleport", "x1", "NONE") + "," + action("stateRequest", "s1", "HARD"));
	CHECK(g.m.find_instant("x1")->status == vda::ActionStatus::Failed);
	CHECK(g.m.find_instant("s1")->status == vda::ActionStatus::Failed);
	CHECK(g.m.has_error("INVALID_INSTANT_ACTION"));

	g.inst(action("initializePosition", "i1", "NONE",
	       "{\"key\":\"x\",\"value\":2.5},{\"key\":\"y\",\"value\":-1},{\"key\":\"theta\",\"value\":0.5},"
	       "{\"key\":\"mapId\",\"value\":\"local\"},{\"key\":\"lastNodeId\",\"value\":\"home\"}"));
	CHECK(g.m.find_instant("i1")->status == vda::ActionStatus::Finished);
	CHECK(g.robot.px == doctest::Approx(2.5));
	CHECK(g.robot.py == doctest::Approx(-1.0));
	CHECK(std::string(g.m.last_node_id()) == "home");
	CHECK_FALSE(g.m.has_error("INVALID_INSTANT_ACTION"));    // cleared by an accepted one

	g.inst(action("factsheetRequest", "f1", "NONE"));
	CHECK(g.m.factsheet_requested());

	g.inst(action("clearInstantActions", "cl", "NONE"));
	CHECK(g.m.find_instant("x1") == nullptr);
	CHECK(g.m.find_instant("i1") == nullptr);
	dump("state_instant.json", g.state());
}

// ---------------------------------------------------------------------------
// Messages
// ---------------------------------------------------------------------------
TEST_CASE("State publication: event-driven with a minimum interval, periodic otherwise") {
	Rig g;
	g.m.state_sent(g.now);
	CHECK_FALSE(g.m.state_due(g.now + 50));
	g.order(order_json("o1", 0, 0, LINE3));
	CHECK_FALSE(g.m.state_due(g.now + 50));                  // min interval 100 ms
	CHECK(g.m.state_due(g.now + 100));
	g.m.state_sent(g.now + 100);
	CHECK_FALSE(g.m.state_due(g.now + 5000));
	CHECK(g.m.state_due(g.now + 10100));                     // periodic
}

TEST_CASE("Messages: state, connection and factsheet are complete JSON") {
	Rig g;
	std::string s = g.state();
	CHECK(s.find("\"timestamp\":\"2026-11-06T") != std::string::npos);
	CHECK(s.find("\"operatingMode\":\"AUTOMATIC\"") != std::string::npos);
	dump("state_idle.json", s);

	char buf[8192];
	size_t n = g.m.build_connection(buf, sizeof(buf), 3, 1794000000000ULL, "ONLINE");
	REQUIRE(n > 0);
	dump("connection.json", std::string(buf, n));
	n = g.m.build_factsheet(buf, sizeof(buf), 1, 1794000000000ULL);
	REQUIRE(n > 0);
	dump("factsheet.json", std::string(buf, n));

	// The inputs used by these tests must be valid VDA 5050 too
	auto nodes = LINE3;
	nodes[1].actions = action("pick", "pick1", "HARD", "{\"key\":\"loadId\",\"value\":\"L7\"}");
	dump("order_sample.json", order_json("o1", 0, 0, nodes, { action("drop", "d1", "SOFT") }));
	dump("instantActions_sample.json", instant(action("cancelOrder", "c1", "NONE") + "," +
	     action("initializePosition", "i1", "NONE", "{\"key\":\"x\",\"value\":2.5}")));
}
