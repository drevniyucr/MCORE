// VDA 5050 3.0.0 extensions: NURBS trajectories, zones (BLOCKED, RELEASE,
// SPEED_LIMIT, ACTION), request/response, corridors and obstacles, maps,
// hibernation, shutdown, visualization.
#include "vda_rig.hpp"

using namespace vda_test;

namespace {

std::string iso(uint64_t ms) {
	char ts[25];
	json::format_iso8601(ms, ts);
	return ts;
}

std::string square(float x0, float y0, float x1, float y1) {
	char b[220];
	snprintf(b, sizeof(b), "[{\"x\":%g,\"y\":%g},{\"x\":%g,\"y\":%g},{\"x\":%g,\"y\":%g},{\"x\":%g,\"y\":%g}]",
	         x0, y0, x1, y0, x1, y1, x0, y1);
	return b;
}

std::string zone(const char *id, const char *type, const std::string &verts, const std::string &extra = "") {
	return "{\"zoneId\":\"" + std::string(id) + "\",\"zoneType\":\"" + type + "\",\"vertices\":" + verts + extra + "}";
}

std::string zone_set(const char *id, const std::string &zones, const char *map = "local") {
	return "{\"headerId\":1,\"timestamp\":\"2026-11-07T10:00:00.000Z\",\"version\":\"3.0.0\","
	       "\"manufacturer\":\"MCORE\",\"serialNumber\":\"agv001\",\"zoneSet\":{\"mapId\":\"" + std::string(map) +
	       "\",\"zoneSetId\":\"" + id + "\",\"zones\":[" + zones + "]}}";
}

std::string responses(const std::string &id, const char *grant, const std::string &lease = "") {
	std::string r = "{\"requestId\":\"" + id + "\",\"grantType\":\"" + grant + "\"";
	if (!lease.empty()) r += ",\"leaseExpiry\":\"" + lease + "\"";
	r += "}";
	return "{\"headerId\":1,\"timestamp\":\"2026-11-07T10:00:00.000Z\",\"version\":\"3.0.0\","
	       "\"manufacturer\":\"MCORE\",\"serialNumber\":\"agv001\",\"responses\":[" + r + "]}";
}

void load_zones(Rig &g, const std::string &zs_json, const char *id) {
	g.m.on_zone_set(zs_json.data(), zs_json.size());
	g.inst(action("enableZoneSet", "ez", "NONE", ("{\"key\":\"zoneSetId\",\"value\":\"" + std::string(id) + "\"}").c_str()));
	REQUIRE(g.m.zone_set(0) != nullptr);
	REQUIRE(g.m.zone_set(0)->enabled);
}

void respond(Rig &g, const std::string &id, const char *grant, const std::string &lease = "") {
	const std::string s = responses(id, grant, lease);
	g.m.on_responses(s.data(), s.size());
	g.m.poll(g.now, g.unix_ms());
}

// The robot follows the stream until the node is reached or the stream stops
void drive_to_node(Rig &g, uint32_t seq) {
	for (int guard = 0; guard < 200; ++guard) {
		if (g.robot.node_target(seq) != nullptr) {
			g.reach(seq);
			return;
		}
		const size_t before = g.robot.targets.size();
		if (before == 0) return;
		g.pop_to(g.robot.targets.back().path_index);
		if (g.robot.targets.size() == before) return;
	}
}

float max_x(const FakeRobot &r) {
	float m = -1e9F;
	for (const auto &t : r.targets) m = t.x > m ? t.x : m;
	return m;
}

const std::vector<NodeSpec> LINE_0_3 = { { "n0", 0, 0, true }, { "n1", 3, 0, true } };
const std::vector<NodeSpec> LINE_0_4 = { { "n0", 0, 0, true }, { "n1", 4, 0, true } };

} // namespace

TEST_CASE("NURBS: line, default knots, rational quarter circle, validation") {
	using namespace vda::geo;
	Nurbs n{};
	n.degree = 1; n.ncp = 2; n.cp[0] = { 0, 0 }; n.cp[1] = { 2, 0 }; n.w[0] = n.w[1] = 1;
	REQUIRE(nurbs_prepare(n));
	CHECK(nurbs_eval(n, 0.5F).x == doctest::Approx(1.0));

	Nurbs c{};
	c.degree = 2; c.ncp = 3;
	c.cp[0] = { 1, 0 }; c.cp[1] = { 1, 1 }; c.cp[2] = { 0, 1 };
	c.w[0] = 1; c.w[1] = 0.70710678F; c.w[2] = 1;
	c.nknots = 6;
	const float k[6] = { 0, 0, 0, 1, 1, 1 };
	for (int i = 0; i < 6; ++i) c.knots[i] = k[i];
	REQUIRE(nurbs_prepare(c));
	for (float u = 0; u <= 1.0F; u += 0.125F) {
		const Vec2 p = nurbs_eval(c, u);
		CHECK(std::sqrt(p.x * p.x + p.y * p.y) == doctest::Approx(1.0).epsilon(1e-4));
	}
	Vec2 pts[64];
	const size_t m = nurbs_sample(c, 0.1F, pts, 64);
	CHECK(m >= 16);
	CHECK(pts[m - 1].y == doctest::Approx(1.0));
	for (size_t i = 1; i < m; ++i) CHECK(dist(pts[i - 1], pts[i]) < 0.12F);

	Nurbs bad = c;
	bad.ncp = 2;                       // degree 2 needs 3 control points
	CHECK_FALSE(nurbs_prepare(bad));
	bad = c;
	bad.knots[1] = 0.5F;               // not clamped
	CHECK_FALSE(nurbs_prepare(bad));
}

TEST_CASE("Geometry: polygons and swept-disc checks") {
	using namespace vda::geo;
	const Vec2 sq[4] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
	CHECK(point_in_polygon({ 0.5F, 0.5F }, sq, 4));
	CHECK_FALSE(point_in_polygon({ 1.5F, 0.5F }, sq, 4));
	CHECK(disc_touches_polygon({ 1.3F, 0.5F }, 0.4F, sq, 4));
	CHECK_FALSE(disc_touches_polygon({ 1.5F, 0.5F }, 0.4F, sq, 4));
	CHECK(capsule_touches_polygon({ -1, 1.3F }, { 2, 1.3F }, 0.4F, sq, 4));
	CHECK_FALSE(capsule_touches_polygon({ -1, 1.5F }, { 2, 1.5F }, 0.4F, sq, 4));
	CHECK(capsule_touches_polygon({ 0.5F, -2 }, { 0.5F, 3 }, 0.0F, sq, 4));   // crosses it
}

TEST_CASE("Trajectory edge: waypoints follow the NURBS arc") {
	Rig g;
	g.at(1, 0);
	const std::string traj = ",\"trajectory\":{\"degree\":2,\"knotVector\":[0,0,0,1,1,1],\"controlPoints\":"
	                         "[{\"x\":1,\"y\":0},{\"x\":1,\"y\":1,\"weight\":0.70710678},{\"x\":0,\"y\":1}]}";
	g.order(order_json("arc", 0, 0, { { "a", 1, 0, true }, { "b", 0, 1, true } }, {}, "local", traj.c_str()));
	REQUIRE(g.m.error_count() == 0);
	drive_to_node(g, 2);
	CHECK(std::string(g.m.last_node_id()) == "b");
	REQUIRE(g.robot.targets.size() >= 15);
	for (const auto &t : g.robot.targets) {
		CHECK(std::sqrt(t.x * t.x + t.y * t.y) == doctest::Approx(1.0).epsilon(0.01));
	}
	CHECK(g.robot.targets.back().is_node);
	CHECK(g.robot.targets.back().y == doctest::Approx(1.0));
}

TEST_CASE("Zone sets: stored DISABLED, enabled by instant action, duplicates and invalid sets rejected") {
	Rig g;
	const std::string zs = zone_set("zs1", zone("z1", "BLOCKED", square(1, 1, 2, 2)));
	g.m.on_zone_set(zs.data(), zs.size());
	REQUIRE(g.m.zone_set(0) != nullptr);
	CHECK_FALSE(g.m.zone_set(0)->enabled);
	dump("zoneSet_sample.json", zs);
	g.m.on_zone_set(zs.data(), zs.size());
	CHECK(g.m.has_error("DUPLICATE_ZONE_SET"));
	const std::string bad = zone_set("zs2", zone("z", "BLOCKED", "[{\"x\":0,\"y\":0},{\"x\":1,\"y\":0}]"));
	g.m.on_zone_set(bad.data(), bad.size());
	CHECK(g.m.has_error("VALIDATION_FAILURE"));
	CHECK(g.m.zone_set(1) == nullptr);
	g.inst(action("enableZoneSet", "e1", "NONE", "{\"key\":\"zoneSetId\",\"value\":\"zs1\"}"));
	CHECK(g.m.zone_set(0)->enabled);
	g.tick(31000);
	CHECK_FALSE(g.m.has_error("DUPLICATE_ZONE_SET"));   // reported for a limited time
	g.inst(action("deleteZoneSet", "d1", "NONE", "{\"key\":\"zoneSetId\",\"value\":\"zs1\"}"));
	CHECK(g.m.zone_set(0) == nullptr);
}

TEST_CASE("BLOCKED zone: the robot stops in front of it and reports NODE_UNREACHABLE") {
	Rig g;
	load_zones(g, zone_set("zs", zone("wall", "BLOCKED", square(1.4F, -1, 1.6F, 1))), "zs");
	g.order(order_json("o", 0, 0, LINE_0_3));
	drive_to_node(g, 2);
	CHECK(g.robot.node_target(2) == nullptr);
	CHECK(max_x(g.robot) <= 0.9F + 1e-3F);                      // the contour never touches it
	CHECK(g.m.has_error("NODE_UNREACHABLE"));
}

TEST_CASE("BLOCKED zone under the robot: stop and BLOCKED_ZONE_VIOLATION") {
	Rig g;
	load_zones(g, zone_set("zs", zone("pit", "BLOCKED", square(-1, -1, 1, 1))), "zs");
	CHECK(g.m.has_error("BLOCKED_ZONE_VIOLATION"));
	CHECK(g.robot.hold);
	g.at(3, 0);
	CHECK_FALSE(g.m.has_error("BLOCKED_ZONE_VIOLATION"));
	CHECK_FALSE(g.robot.hold);
}

TEST_CASE("RELEASE zone: request, QUEUED, GRANTED, drive through") {
	Rig g;
	load_zones(g, zone_set("zs", zone("gate", "RELEASE", square(1.4F, -1, 1.6F, 1), ",\"releaseLossBehavior\":\"STOP\"")), "zs");
	g.order(order_json("o", 0, 0, LINE_0_3));
	drive_to_node(g, 2);
	CHECK(max_x(g.robot) <= 0.9F + 1e-3F);
	const vda::Request *r = g.m.request(0);
	REQUIRE(r != nullptr);
	CHECK(r->type == vda::ReqType::Access);
	CHECK(r->status == vda::ReqStatus::Requested);
	dump("state_zone_request.json", g.state());

	respond(g, r->id, "QUEUED");
	drive_to_node(g, 2);
	CHECK(max_x(g.robot) <= 0.9F + 1e-3F);                      // queued: still waiting
	respond(g, r->id, "GRANTED", iso(g.unix_ms() + 600000));
	CHECK(r->status == vda::ReqStatus::Granted);
	drive_to_node(g, 2);
	CHECK(std::string(g.m.last_node_id()) == "n1");
}

TEST_CASE("RELEASE zone: lease expires inside (STOP), request removed after leaving") {
	Rig h;
	load_zones(h, zone_set("zs", zone("gate", "RELEASE", square(1.4F, -1, 1.6F, 1))), "zs");
	h.order(order_json("o", 0, 0, LINE_0_3));
	drive_to_node(h, 2);
	const vda::Request *q = h.m.request(0);
	REQUIRE(q != nullptr);
	respond(h, q->id, "GRANTED", iso(h.unix_ms() + 50));
	h.at(1.5F, 0);                                              // inside
	CHECK(q->in_use);
	h.tick(100);
	CHECK(q->status == vda::ReqStatus::Expired);
	CHECK(h.robot.hold);
	CHECK(h.m.has_error("RELEASE_LOST"));
	h.at(2.5F, 0);                                              // contour has left the zone
	CHECK(h.m.request(0) == nullptr);
	CHECK_FALSE(h.robot.hold);
	CHECK_FALSE(h.m.has_error("RELEASE_LOST"));
}

TEST_CASE("RELEASE zone: REJECTED keeps the robot out, a new request follows later") {
	Rig g;
	load_zones(g, zone_set("zs", zone("gate", "RELEASE", square(1.4F, -1, 1.6F, 1))), "zs");
	g.order(order_json("o", 0, 0, LINE_0_3));
	drive_to_node(g, 2);
	const std::string first = g.m.request(0)->id;
	respond(g, first, "REJECTED");
	CHECK(g.state().find(first) == std::string::npos);          // not reported any more
	g.tick(11000);
	drive_to_node(g, 2);
	REQUIRE(g.m.request(0) != nullptr);
	CHECK(std::string(g.m.request(0)->id) != first);
	CHECK(max_x(g.robot) <= 0.9F + 1e-3F);
}

TEST_CASE("SPEED_LIMIT zone applies before the contour enters") {
	Rig g;
	load_zones(g, zone_set("zs", zone("slow", "SPEED_LIMIT", square(2, -1, 3, 1), ",\"maximumSpeed\":0.2")), "zs");
	CHECK(g.robot.speed_limit == 0.0F);
	g.at(1.5F, 0);           // contour 0.5 m + margin reaches x = 2.1
	CHECK(g.robot.speed_limit == doctest::Approx(0.2));
	g.at(4.0F, 0);
	CHECK(g.robot.speed_limit == 0.0F);
}

TEST_CASE("ACTION zone: entry and exit actions run, reported in zoneActionStates") {
	Rig g;
	load_zones(g, zone_set("zs", zone("dock", "ACTION", square(2, -1, 3, 1),
	           ",\"entryActions\":[{\"actionType\":\"startCharging\",\"blockingType\":\"NONE\"}],"
	           "\"duringActions\":[],\"exitActions\":[{\"actionType\":\"stopCharging\",\"blockingType\":\"NONE\"}]")), "zs");
	g.at(2.5F, 0);
	REQUIRE(g.m.zone_action_count() == 1);
	CHECK(g.m.zone_action(0)->status == vda::ActionStatus::Finished);
	CHECK(g.state().find("\"charging\":true") != std::string::npos);
	g.at(5.0F, 0);
	REQUIRE(g.m.zone_action_count() == 2);
	CHECK(std::string(g.m.zone_action(1)->type) == "stopCharging");
	CHECK(g.state().find("\"charging\":false") != std::string::npos);
	dump("state_zone_actions.json", g.state());
	g.inst(action("clearZoneActions", "cz", "NONE"));
	CHECK(g.m.zone_action_count() == 0);
}

TEST_CASE("ACTION zone with a HARD waitForTrigger holds the robot until the trigger") {
	Rig g;
	load_zones(g, zone_set("zs", zone("check", "ACTION", square(2, -1, 3, 1),
	           ",\"entryActions\":[{\"actionType\":\"waitForTrigger\",\"blockingType\":\"HARD\"}],"
	           "\"duringActions\":[],\"exitActions\":[]")), "zs");
	g.at(2.5F, 0);
	CHECK(g.robot.hold);
	g.inst(action("trigger", "t", "NONE"));
	CHECK_FALSE(g.robot.hold);
}

TEST_CASE("Corridor: obstacle avoided inside the corridor") {
	Rig g;
	g.inst(action("mcoreSimObstacle", "ob", "NONE",
	       "{\"key\":\"x\",\"value\":2},{\"key\":\"y\",\"value\":0.1},{\"key\":\"radius\",\"value\":0.2}"));
	g.order(order_json("o", 0, 0, LINE_0_4, {}, "local", ",\"corridor\":{\"leftWidth\":1.0,\"rightWidth\":1.0}"));
	drive_to_node(g, 2);
	CHECK(std::string(g.m.last_node_id()) == "n1");
	float min_clear = 1e9F, max_dev = 0;
	for (const auto &t : g.robot.targets) {
		const float d = std::sqrt((t.x - 2) * (t.x - 2) + (t.y - 0.1F) * (t.y - 0.1F));
		min_clear = d < min_clear ? d : min_clear;
		max_dev = std::fabs(t.y) > max_dev ? std::fabs(t.y) : max_dev;
	}
	CHECK(min_clear >= 0.7F);          // obstacle radius + robot radius
	CHECK(max_dev <= 1.0F);            // inside the corridor
	CHECK(max_dev > 0.5F);             // it did deviate
	CHECK(g.robot.targets.back().y == doctest::Approx(0.0));   // back on the node
}

TEST_CASE("Corridor too narrow / no corridor: the robot waits, continues once cleared") {
	Rig g;
	g.inst(action("mcoreSimObstacle", "ob", "NONE", "{\"key\":\"x\",\"value\":2},{\"key\":\"y\",\"value\":0}"));
	g.order(order_json("o", 0, 0, LINE_0_4, {}, "local", ",\"corridor\":{\"leftWidth\":0.3,\"rightWidth\":0.3}"));
	drive_to_node(g, 2);
	CHECK(g.robot.node_target(2) == nullptr);
	CHECK(max_x(g.robot) <= 2.0F - 0.7F + 1e-3F);
	CHECK(g.state().find("OBSTACLE_AHEAD") != std::string::npos);
	g.inst(action("mcoreSimClearObstacles", "cl", "NONE"));
	drive_to_node(g, 2);
	CHECK(std::string(g.m.last_node_id()) == "n1");
}

TEST_CASE("Corridor with releaseRequired: edgeRequest first, deviation after GRANTED") {
	Rig g;
	g.inst(action("mcoreSimObstacle", "ob", "NONE", "{\"key\":\"x\",\"value\":2},{\"key\":\"y\",\"value\":0}"));
	g.order(order_json("o", 0, 0, LINE_0_4, {}, "local",
	        ",\"corridor\":{\"leftWidth\":1.0,\"rightWidth\":1.0,\"releaseRequired\":true}"));
	drive_to_node(g, 2);
	const vda::Request *r = g.m.request(0);
	REQUIRE(r != nullptr);
	CHECK(r->type == vda::ReqType::Corridor);
	CHECK(r->status == vda::ReqStatus::Requested);
	CHECK(max_x(g.robot) <= 1.3F + 1e-3F);
	dump("state_edge_request.json", g.state());
	respond(g, r->id, "GRANTED");
	drive_to_node(g, 2);
	CHECK(std::string(g.m.last_node_id()) == "n1");
	CHECK(g.m.request(0) == nullptr);   // deviation over: request removed
}

TEST_CASE("Obstacle in the already streamed part: stop, re-stream with checks") {
	Rig g;
	g.order(order_json("o", 0, 0, LINE_0_4));
	REQUIRE(g.robot.targets.size() == vda::STREAM_AHEAD);   // up to x = 1.6
	const int stops = g.robot.stops;
	g.inst(action("mcoreSimObstacle", "ob", "NONE", "{\"key\":\"x\",\"value\":1.5},{\"key\":\"y\",\"value\":0}"));
	CHECK(g.robot.stops == stops + 1);
	CHECK(g.robot.hold);
	const size_t before = g.robot.targets.size();
	g.ack_stops();                     // the robot confirms it dropped its queue
	CHECK_FALSE(g.robot.hold);
	REQUIRE(g.robot.targets.size() > before);
	for (size_t i = before; i < g.robot.targets.size(); ++i) CHECK(g.robot.targets[i].x <= 0.8F + 1e-3F);
}

TEST_CASE("Maps: download (simulated), duplicate, enable, use, delete") {
	Rig g;
	g.inst(action("downloadMap", "d1", "NONE", "{\"key\":\"mapId\",\"value\":\"hall2\"},"
	       "{\"key\":\"mapVersion\",\"value\":\"7\"},{\"key\":\"mapDownloadLink\",\"value\":\"https://maps/hall2\"}"));
	CHECK(g.m.find_instant("d1")->status == vda::ActionStatus::Running);
	g.tick(2001);
	CHECK(g.m.find_instant("d1")->status == vda::ActionStatus::Finished);
	REQUIRE(g.m.map(1) != nullptr);
	CHECK_FALSE(g.m.map(1)->enabled);
	g.inst(action("downloadMap", "d2", "NONE", "{\"key\":\"mapId\",\"value\":\"hall2\"},"
	       "{\"key\":\"mapVersion\",\"value\":\"7\"},{\"key\":\"mapDownloadLink\",\"value\":\"x\"}"));
	CHECK(g.m.has_error("DUPLICATE_MAP"));
	g.order(order_json("o", 0, 0, LINE3, {}, "hall2"));
	CHECK(g.m.has_error("UNKNOWN_MAP_ID"));                    // not enabled yet
	g.inst(action("enableMap", "e", "NONE", "{\"key\":\"mapId\",\"value\":\"hall2\"},{\"key\":\"mapVersion\",\"value\":\"7\"}"));
	CHECK(g.m.map(1)->enabled);
	g.inst(action("initializePosition", "ip", "NONE", "{\"key\":\"x\",\"value\":0},{\"key\":\"y\",\"value\":0},"
	       "{\"key\":\"theta\",\"value\":0},{\"key\":\"mapId\",\"value\":\"hall2\"}"));
	CHECK(std::string(g.m.current_map()) == "hall2");
	g.order(order_json("o2", 0, 0, LINE3, {}, "hall2"));
	CHECK(std::string(g.m.order_id()) == "o2");
	g.inst(action("deleteMap", "del", "NONE", "{\"key\":\"mapId\",\"value\":\"local\"},{\"key\":\"mapVersion\",\"value\":\"1\"}"));
	CHECK(g.m.find_instant("del")->status == vda::ActionStatus::Finished);
	CHECK(g.m.map(0) == nullptr);
	dump("state_maps.json", g.state());
}

TEST_CASE("Hibernation: final state, HIBERNATING, ignores orders, wakes by action or time") {
	Rig g;
	g.inst(action("startHibernation", "h1", "NONE"));
	CHECK(g.m.find_instant("h1")->status == vda::ActionStatus::Finished);
	CHECK(std::string(g.m.connection_state()) == "ONLINE");   // until the state is out
	CHECK(g.m.state_due(g.now + 1000));
	g.m.state_sent(g.now);
	CHECK(std::string(g.m.connection_state()) == "HIBERNATING");
	CHECK_FALSE(g.m.state_due(g.now + 60000));
	g.order(order_json("o", 0, 0, LINE3));
	CHECK(std::string(g.m.order_id()).empty());               // ignored
	g.inst(action("stateRequest", "s", "NONE"));
	CHECK(g.m.find_instant("s") == nullptr);                  // ignored
	g.inst(action("stopHibernation", "h2", "NONE"));
	CHECK(std::string(g.m.connection_state()) == "ONLINE");

	g.inst(action("startHibernation", "h3", "NONE",
	       ("{\"key\":\"wakeUpTime\",\"value\":\"" + iso(g.unix_ms() + 1000) + "\"}").c_str()));
	g.m.state_sent(g.now);
	CHECK(g.m.hibernating());
	g.tick(1100);
	CHECK_FALSE(g.m.hibernating());
}

TEST_CASE("Shutdown: only when idle, then OFFLINE") {
	Rig g;
	g.order(order_json("o", 0, 0, LINE3));
	g.inst(action("shutdown", "s1", "NONE"));
	CHECK(g.m.find_instant("s1")->status == vda::ActionStatus::Failed);
	g.reach(2);
	g.reach(4);
	g.inst(action("shutdown", "s2", "NONE"));
	CHECK(g.m.find_instant("s2")->status == vda::ActionStatus::Finished);
	g.m.state_sent(g.now);
	CHECK(g.m.shutdown_requested());
	CHECK(std::string(g.m.connection_state()) == "OFFLINE");
}

TEST_CASE("Visualization and the extended messages are complete JSON") {
	Rig g;
	g.order(order_json("o", 0, 0, LINE3));
	static char buf[8192];
	const size_t n = g.m.build_visualization(buf, sizeof(buf), 5, g.unix_ms(), 42);
	REQUIRE(n > 0);
	const std::string v(buf, n);
	CHECK(v.find("intermediatePath") != std::string::npos);
	dump("visualization.json", v);
	dump("responses_sample.json", responses("req-1", "GRANTED", iso(g.unix_ms() + 10000)));
	char cbuf[256];
	const size_t c = g.m.build_connection(cbuf, sizeof(cbuf), 1, g.unix_ms(), "HIBERNATING");
	dump("connection_hibernating.json", std::string(cbuf, c));
	char fs[8192];
	const size_t f = g.m.build_factsheet(fs, sizeof(fs), 2, g.unix_ms());
	REQUIRE(f > 0);
	dump("factsheet.json", std::string(fs, f));
}

TEST_CASE("initializePosition: a fresh state once the robot reports the new pose") {
	Rig g;
	g.at(0, 1);
	g.inst(action("initializePosition", "ip", "NONE", "{\"key\":\"x\",\"value\":3},{\"key\":\"y\",\"value\":0},"
	       "{\"key\":\"theta\",\"value\":0}"));
	CHECK(g.m.state_due(g.now + 1000));
	g.m.state_sent(g.now + 1000);
	g.now += 1000;
	g.at(0, 1);                                  // snapshot still old
	CHECK_FALSE(g.m.state_due(g.now + 200));
	g.at(3, 0);                                  // the real-time core applied it
	CHECK(g.m.state_due(g.now + 200));
}
