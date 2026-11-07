/*
 * messages.cpp — VDA 5050 3.0.0 outgoing messages (state, connection,
 * factsheet), see vda5050.hpp.
 */
#include <cstring>

#include "vda5050/json.hpp"
#include "vda5050/vda5050.hpp"

namespace vda {

namespace {
constexpr const char *PROTOCOL_VERSION = "3.0.0";

void header(json::JsonWriter &w, uint32_t header_id, uint64_t unix_ms, const Config &cfg) {
	char ts[25];
	json::format_iso8601(unix_ms, ts);
	w.kv_int("headerId", header_id);
	w.kv("timestamp", ts);
	w.kv("version", PROTOCOL_VERSION);
	w.kv("manufacturer", cfg.manufacturer);
	w.kv("serialNumber", cfg.serial);
}

void action_state(json::JsonWriter &w, const Action &a) {
	w.begin_object();
	w.kv("actionId", a.id);
	w.kv("actionType", a.type);
	w.kv("actionStatus", to_string(a.status));
	w.end_object();
}
} // namespace

size_t OrderManager::build_state(char *buf, size_t cap, uint32_t header_id, uint64_t unix_ms) {
	json::JsonWriter w(buf, cap);
	w.begin_object();
	header(w, header_id, unix_ms, cfg_);

	w.kv("orderId", cur_.order_id);
	w.kv_int("orderUpdateId", cur_.update_id);
	w.kv("lastNodeId", last_node_id_);
	w.kv_int("lastNodeSequenceId", last_node_seq_);

	w.key("nodeStates").begin_array();
	for (uint8_t i = first_pending_node_; i < cur_.nnodes; ++i) {
		const Node &n = cur_.nodes[i];
		w.begin_object();
		w.kv("nodeId", n.id);
		w.kv_int("sequenceId", n.seq);
		w.kv_bool("released", n.released);
		w.key("nodePosition").begin_object();
		w.kv("x", n.x, 4);
		w.kv("y", n.y, 4);
		if (n.has_theta) w.kv("theta", n.theta, 4);
		w.kv("mapId", n.map);
		w.end_object();
		w.end_object();
	}
	w.end_array();

	w.key("edgeStates").begin_array();
	for (uint8_t i = first_pending_edge_; i < cur_.nedges; ++i) {
		const Edge &e = cur_.edges[i];
		w.begin_object();
		w.kv("edgeId", e.id);
		w.kv_int("sequenceId", e.seq);
		w.kv_bool("released", e.released);
		w.end_object();
	}
	w.end_array();

	w.key("maps").begin_array();
	for (const MapEntry &m : maps_) {
		if (!m.used) continue;
		w.begin_object().kv("mapId", m.id).kv("mapVersion", m.version)
		 .kv("mapStatus", m.enabled ? "ENABLED" : "DISABLED").end_object();
	}
	w.end_array();

	w.key("zoneSets").begin_array();
	for (const ZoneSet &zs : zone_sets_) {
		if (!zs.used) continue;
		w.begin_object().kv("zoneSetId", zs.id).kv("mapId", zs.map)
		 .kv("zoneSetStatus", zs.enabled ? "ENABLED" : "DISABLED").end_object();
	}
	w.end_array();

	w.key("mobileRobotPosition").begin_object();
	w.kv("x", robot_.x, 4);
	w.kv("y", robot_.y, 4);
	w.kv("theta", robot_.theta, 4);
	w.kv("mapId", current_map_);
	w.kv_bool("localized", robot_.valid);
	w.end_object();

	w.key("velocity").begin_object();
	w.kv("vx", robot_.vx, 3);
	w.kv("vy", robot_.vy, 3);
	w.kv("omega", robot_.omega, 3);
	w.end_object();

	w.key("loads").begin_array();
	if (robot_.loaded) {
		w.begin_object().kv("loadPosition", "LHD1").end_object();
	}
	w.end_array();

	w.kv_bool("driving", robot_.driving);
	w.kv_bool("paused", robot_.paused);
	w.kv_bool("newBaseRequest", false);

	// 6.9 requests (REJECTED ones are not reported; QUEUED stays REQUESTED)
	w.key("zoneRequests").begin_array();
	for (const Request &r : requests_) {
		if (!r.used || r.rejected || r.type != ReqType::Access) continue;
		const ZoneSet &zs = zone_sets_[r.zone_set];
		w.begin_object().kv("requestId", r.id).kv("requestType", "ACCESS")
		 .kv("zoneId", zs.zones[r.zone].id).kv("zoneSetId", zs.id)
		 .kv("requestStatus", to_string(r.status)).end_object();
	}
	w.end_array();
	w.key("edgeRequests").begin_array();
	for (const Request &r : requests_) {
		if (!r.used || r.rejected || r.type != ReqType::Corridor) continue;
		const char *edge_id = "";
		for (uint8_t i = 0; i < cur_.nedges; ++i) {
			if (cur_.edges[i].seq == r.edge_seq) edge_id = cur_.edges[i].id;
		}
		w.begin_object().kv("requestId", r.id).kv("requestType", "CORRIDOR")
		 .kv("edgeId", edge_id).kv_int("sequenceId", r.edge_seq)
		 .kv("requestStatus", to_string(r.status)).end_object();
	}
	w.end_array();

	w.key("actionStates").begin_array();
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		action_state(w, cur_.actions[i]);
	}
	w.end_array();

	w.key("instantActionStates").begin_array();
	for (uint8_t i = 0; i < ninstant_; ++i) {
		action_state(w, instant_[i]);
	}
	w.end_array();

	w.key("zoneActionStates").begin_array();
	for (uint8_t i = 0; i < nzone_actions_; ++i) {
		action_state(w, zone_actions_[i]);
	}
	w.end_array();

	w.key("powerSupply").begin_object();
	w.kv("stateOfCharge", robot_.battery_soc, 1);
	w.kv_bool("charging", charging_);
	w.end_object();

	w.kv("operatingMode", robot_.valid ? "AUTOMATIC" : "STARTUP");

	w.key("errors").begin_array();
	for (uint8_t i = 0; i < nerrors_; ++i) {
		const Error &e = errors_[i];
		w.begin_object();
		w.kv("errorType", e.type);
		w.kv("errorLevel", to_string(e.level));
		if (e.description[0] != '\0') w.kv("errorDescription", e.description);
		w.key("errorReferences").begin_array();
		for (uint8_t r = 0; r < e.nrefs; ++r) {
			w.begin_object();
			w.kv("referenceKey", e.refs[r].key);
			w.kv("referenceValue", e.refs[r].value);
			w.end_object();
		}
		w.end_array();
		w.end_object();
	}
	w.end_array();

	w.key("information").begin_array();
	if (waiting_obstacle_) {
		w.begin_object().kv("infoType", "OBSTACLE_AHEAD").kv("infoLevel", "INFO")
		 .kv("infoDescriptor", "waiting in front of an obstacle").end_object();
	}
	if (hold_mask_ != 0) {
		w.begin_object().kv("infoType", "HOLD").kv("infoLevel", "DEBUG").key("infoReferences").begin_array()
		 .begin_object().kv("referenceKey", "reasons").key("referenceValue");
		char m[4];
		m[0] = static_cast<char>('0' + (hold_mask_ / 100) % 10);
		m[1] = static_cast<char>('0' + (hold_mask_ / 10) % 10);
		m[2] = static_cast<char>('0' + hold_mask_ % 10);
		m[3] = '\0';
		w.str(m).end_object().end_array().end_object();
	}
	w.end_array();

	w.key("safetyState").begin_object();
	w.kv("activeEmergencyStop", "NONE");
	w.kv_bool("fieldViolation", false);
	w.end_object();

	w.end_object();
	return w.ok() ? w.size() : 0;
}

size_t OrderManager::build_connection(char *buf, size_t cap, uint32_t header_id, uint64_t unix_ms,
		const char *connection_state) const {
	json::JsonWriter w(buf, cap);
	w.begin_object();
	header(w, header_id, unix_ms, cfg_);
	w.kv("connectionState", connection_state);
	w.end_object();
	return w.ok() ? w.size() : 0;
}

size_t OrderManager::build_factsheet(char *buf, size_t cap, uint32_t header_id, uint64_t unix_ms) {
	json::JsonWriter w(buf, cap);
	w.begin_object();
	header(w, header_id, unix_ms, cfg_);

	w.key("typeSpecification").begin_object();
	w.kv("seriesName", cfg_.series_name);
	w.kv("mobileRobotKinematics", "DIFFERENTIAL");
	// factsheet.schema 3.0.0 requires the misspelt "mobileRobotKinematic"
	// (fixed upstream for 3.0.1); send both so strict 3.0.0 validators pass.
	w.kv("mobileRobotKinematic", "DIFFERENTIAL");
	w.kv("mobileRobotClass", "CARRIER");
	w.kv("maximumLoadMass", 0.0);
	w.key("localizationTypes").begin_array().str("NATURAL").end_array();
	w.key("navigationTypes").begin_array().str("VIRTUAL_LINE_GUIDED").end_array();
	w.key("supportedZones").begin_array().str("BLOCKED").str("LINE_GUIDED").str("RELEASE")
	 .str("SPEED_LIMIT").str("ACTION").str("PRIORITY").str("PENALTY").end_array();
	w.end_object();

	w.key("physicalParameters").begin_object();
	w.kv("minimumSpeed", 0.0);
	w.kv("maximumSpeed", cfg_.max_speed);
	w.kv("maximumAcceleration", 0.5);
	w.kv("maximumDeceleration", 0.5);
	w.kv("minimumHeight", 0.3);
	w.kv("maximumHeight", 0.3);
	w.kv("width", 0.6);
	w.kv("length", 0.8);
	w.end_object();

	w.key("protocolLimits").begin_object();
	w.key("maximumStringLengths").begin_object();
	w.kv_int("maximumMessageLength", 16384);
	w.kv_int("maximumIdLength", static_cast<int64_t>(ID_LEN - 1));
	w.kv_bool("idNumericalOnly", false);
	w.end_object();
	w.key("maximumArrayLengths").begin_object();
	w.kv_int("order.nodes", MAX_NODES);
	w.kv_int("order.edges", MAX_EDGES);
	w.kv_int("node.actions", MAX_ACTIONS);
	w.kv_int("edge.actions", MAX_ACTIONS);
	w.kv_int("actions.actionsParameters", MAX_PARAMS);
	w.kv_int("instantActions", MAX_INSTANT);
	w.end_object();
	w.key("timing").begin_object();
	w.kv("minimumOrderInterval", 0.5);
	w.kv("minimumStateInterval", static_cast<double>(cfg_.min_state_interval_ms) / 1000.0);
	w.kv("defaultStateInterval", static_cast<double>(cfg_.state_interval_ms) / 1000.0);
	w.end_object();
	w.end_object();

	w.key("protocolFeatures").begin_object();
	w.key("optionalParameters").begin_array();
	static const char *const supported[] = {
		"order.nodes.nodePosition.theta", "order.nodes.nodePosition.allowedDeviationXY",
		"order.edges.maximumSpeed", "order.edges.trajectory", "order.edges.corridor",
	};
	for (const char *p : supported) {
		w.begin_object().kv("parameter", p).kv("support", "SUPPORTED").end_object();
	}
	w.begin_object().kv("parameter", "order.nodes.nodePosition").kv("support", "REQUIRED").end_object();
	w.end_array();

	struct Feature { const char *type; const char *scopes[3]; uint8_t nscopes; const char *blocking[4]; uint8_t nblock; };
	static const Feature features[] = {
		{ "pick", { "NODE", "EDGE" }, 2, { "NONE", "SOFT", "SINGLE", "HARD" }, 4 },
		{ "drop", { "NODE", "EDGE" }, 2, { "NONE", "SOFT", "SINGLE", "HARD" }, 4 },
		{ "waitForTrigger", { "NODE", "ZONE" }, 2, { "NONE", "SOFT", "SINGLE", "HARD" }, 4 },
		{ "finePositioning", { "NODE", "EDGE" }, 2, { "NONE", "SOFT", "SINGLE", "HARD" }, 4 },
		{ "startCharging", { "NODE", "ZONE" }, 2, { "NONE", "SOFT", "SINGLE", "HARD" }, 4 },
		{ "stopCharging", { "NODE", "ZONE" }, 2, { "NONE", "SOFT", "SINGLE", "HARD" }, 4 },
		{ "cancelOrder", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "startPause", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "stopPause", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "stateRequest", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "factsheetRequest", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "initializePosition", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "clearInstantActions", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "trigger", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "startHibernation", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "stopHibernation", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "shutdown", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "downloadMap", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "enableMap", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "deleteMap", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "enableZoneSet", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "deleteZoneSet", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "clearZoneActions", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "mcoreSimObstacle", { "INSTANT" }, 1, { "NONE" }, 1 },
		{ "mcoreSimClearObstacles", { "INSTANT" }, 1, { "NONE" }, 1 },
	};
	w.key("mobileRobotActions").begin_array();
	for (const Feature &f : features) {
		w.begin_object();
		w.kv("actionType", f.type);
		w.key("actionScopes").begin_array();
		for (uint8_t i = 0; i < f.nscopes; ++i) w.str(f.scopes[i]);
		w.end_array();
		w.key("blockingTypes").begin_array();
		for (uint8_t i = 0; i < f.nblock; ++i) w.str(f.blocking[i]);
		w.end_array();
		const bool robot = strcmp(f.type, "pick") == 0 || strcmp(f.type, "drop") == 0;
		w.kv("pauseAllowed", robot ? "true" : "false");
		w.kv("cancelAllowed", "true");
		w.end_object();
	}
	w.end_array();
	w.end_object();

	w.key("mobileRobotGeometry").begin_object().end_object();
	w.key("loadSpecification").begin_object();
	w.key("loadPositions").begin_array().str("LHD1").end_array();
	w.end_object();

	w.end_object();
	return w.ok() ? w.size() : 0;
}

size_t OrderManager::build_visualization(char *buf, size_t cap, uint32_t header_id, uint64_t unix_ms,
		uint32_t state_header_id) {
	json::JsonWriter w(buf, cap);
	w.begin_object();
	header(w, header_id, unix_ms, cfg_);
	w.kv_int("referenceStateHeaderId", state_header_id);
	w.key("mobileRobotPosition").begin_object();
	w.kv("x", robot_.x, 4);
	w.kv("y", robot_.y, 4);
	w.kv("theta", robot_.theta, 4);
	w.kv("mapId", current_map_);
	w.kv_bool("localized", robot_.valid);
	w.end_object();
	w.key("velocity").begin_object();
	w.kv("vx", robot_.vx, 3);
	w.kv("vy", robot_.vy, 3);
	w.kv("omega", robot_.omega, 3);
	w.end_object();
	// 6.8: polyline from the current position over the next waypoints, with ETAs
	if (path_done_ < path_sent_) {
		const float v = cfg_.max_speed > 0.0F ? cfg_.max_speed : 0.5F;
		char ts[25];
		w.key("intermediatePath").begin_object().key("polyline").begin_array();
		json::format_iso8601(unix_ms, ts);
		w.begin_object().kv("x", robot_.x, 3).kv("y", robot_.y, 3).kv("eta", ts).end_object();
		float px = robot_.x, py = robot_.y, dist = 0.0F;
		const uint32_t end = path_sent_ - path_done_ > 10U ? path_done_ + 10U : path_sent_;
		for (uint32_t i = path_done_; i < end; ++i) {
			const PathPoint &q = path_[i % PATH_CAP];
			const float dx = q.x - px, dy = q.y - py;
			dist += geo::dist(geo::Vec2{ px, py }, geo::Vec2{ q.x, q.y });
			(void) dx; (void) dy;
			px = q.x;
			py = q.y;
			json::format_iso8601(unix_ms + static_cast<uint64_t>(dist / v * 1000.0F), ts);
			w.begin_object().kv("x", q.x, 3).kv("y", q.y, 3).kv("eta", ts).end_object();
		}
		w.end_array().end_object();
	}
	w.end_object();
	return w.ok() ? w.size() : 0;
}

} // namespace vda
