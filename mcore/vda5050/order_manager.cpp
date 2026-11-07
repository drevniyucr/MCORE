/*
 * order_manager.cpp — VDA 5050 3.0.0 orders, actions, instant actions,
 * maps and lifecycle (hibernation, shutdown). See vda5050.hpp.
 * Motion streaming: motion.cpp. Zones and requests: zones.cpp.
 */
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "vda5050/internal.hpp"

namespace vda {

using namespace detail;

// =============================================================================
// Shared helpers (internal.hpp)
// =============================================================================
namespace detail {

bool parse_blocking(const char *s, Blocking &b) {
	if (strcmp(s, "NONE") == 0) { b = Blocking::None; return true; }
	if (strcmp(s, "SOFT") == 0) { b = Blocking::Soft; return true; }
	if (strcmp(s, "SINGLE") == 0) { b = Blocking::Single; return true; }
	if (strcmp(s, "HARD") == 0) { b = Blocking::Hard; return true; }
	return false;
}

const char *param(const ActionParams &a, const char *key) {
	for (uint8_t i = 0; i < a.n; ++i) {
		if (strcmp(a.p[i].key, key) == 0) return a.p[i].value;
	}
	return nullptr;
}

bool param_float(const ActionParams &a, const char *key, float &out) {
	const char *v = param(a, key);
	if (v == nullptr || v[0] == '\0') return false;
	char *end = nullptr;
	out = strtof(v, &end);
	return end != v;
}

bool allowed_on_node(ActionKind k) {
	switch (k) {
	case ActionKind::Pick: case ActionKind::Drop: case ActionKind::WaitForTrigger:
	case ActionKind::FinePositioning: case ActionKind::StartCharging: case ActionKind::StopCharging:
		return true;
	default:
		return false;
	}
}
bool allowed_on_edge(ActionKind k) {
	return k == ActionKind::Pick || k == ActionKind::Drop || k == ActionKind::FinePositioning;
}
bool allowed_in_zone(ActionKind k) {
	return k == ActionKind::FinePositioning || k == ActionKind::WaitForTrigger ||
	       k == ActionKind::StartCharging || k == ActionKind::StopCharging;
}
bool is_instant_kind(ActionKind k) {
	return k >= ActionKind::CancelOrder;
}

void u32_to_str(uint32_t v, char *out) {
	char tmp[12];
	int n = 0;
	do { tmp[n++] = static_cast<char>('0' + v % 10); v /= 10; } while (v != 0U);
	int j = 0;
	while (n > 0) out[j++] = tmp[--n];
	out[j] = '\0';
}

uint64_t parse_iso8601(const char *s) {
	// YYYY-MM-DDTHH:mm:ss[.f+]Z
	auto num = [&](int pos, int len) -> int {
		int v = 0;
		for (int i = 0; i < len; ++i) {
			const char c = s[pos + i];
			if (c < '0' || c > '9') return -1;
			v = v * 10 + (c - '0');
		}
		return v;
	};
	if (s == nullptr || strlen(s) < 20 || s[4] != '-' || s[7] != '-' || s[10] != 'T' || s[13] != ':' || s[16] != ':') {
		return 0;
	}
	const int y = num(0, 4), mo = num(5, 2), d = num(8, 2), h = num(11, 2), mi = num(14, 2), se = num(17, 2);
	if (y < 1970 || mo < 1 || mo > 12 || d < 1 || d > 31 || h < 0 || h > 23 || mi < 0 || mi > 59 || se < 0 || se > 60) {
		return 0;
	}
	int ms = 0;
	int pos = 19;
	if (s[pos] == '.') {
		int scale = 100;
		++pos;
		while (s[pos] >= '0' && s[pos] <= '9') {
			ms += (s[pos] - '0') * scale;
			scale /= 10;
			++pos;
		}
	}
	if (s[pos] != 'Z') return 0;
	// days_from_civil (H. Hinnant)
	const int yy = y - (mo <= 2 ? 1 : 0);
	const int era = yy / 400;
	const unsigned yoe = static_cast<unsigned>(yy - era * 400);
	const unsigned doy = static_cast<unsigned>((153 * (mo + (mo > 2 ? -3 : 9)) + 2) / 5 + d - 1);
	const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
	const int64_t days = static_cast<int64_t>(era) * 146097 + static_cast<int64_t>(doe) - 719468;
	return static_cast<uint64_t>(((days * 24 + h) * 60 + mi) * 60 + se) * 1000U + static_cast<uint64_t>(ms);
}

namespace {
void read_param_value(json::JsonReader &r, char *out, size_t cap) {
	if (r.peek() == '"') {
		char tmp[128];
		if (r.read_string(tmp, sizeof(tmp))) {
			strncpy(out, tmp, cap - 1);
			out[cap - 1] = '\0';
		}
		return;
	}
	(void) r.peek();
	const char *start = r.pos();
	while (*start == ' ' || *start == '\n' || *start == '\r' || *start == '\t') ++start;
	r.skip();
	const char *end = r.pos();
	while (end > start && (end[-1] == ',' || end[-1] == ' ' || end[-1] == '\n' || end[-1] == '\r' || end[-1] == '\t')) {
		--end;
	}
	size_t n = static_cast<size_t>(end - start);
	if (n >= cap) n = cap - 1;
	memcpy(out, start, n);
	out[n] = '\0';
}
} // namespace

bool parse_action(json::JsonReader &r, Action &a, int32_t owner_seq, bool require_id, ParseErr &err,
                  ActionParams *params) {
	memset(&a, 0, sizeof(a));
	if (params != nullptr) params->n = 0;
	a.owner_seq = owner_seq;
	a.status = ActionStatus::Waiting;
	bool have_type = false, have_id = false, have_block = false;
	if (!r.enter_object()) return false;
	json::Span k;
	while (r.next_key(k)) {
		if (k == "actionType") {
			have_type = r.read_string(a.type, sizeof(a.type));
			if (!have_type) { err.too_long = true; copy_str(err.detail, "actionType"); }
		} else if (k == "actionId") {
			have_id = r.read_string(a.id, sizeof(a.id));
			if (!have_id) { err.too_long = true; copy_str(err.detail, "actionId"); }
		} else if (k == "blockingType") {
			char b[12];
			have_block = r.read_string(b, sizeof(b)) && parse_blocking(b, a.blocking);
		} else if (k == "actionParameters") {
			if (params == nullptr) {
				r.skip();
				if (!r.ok()) return false;
				continue;
			}
			if (!r.enter_array()) return false;
			while (r.next_element()) {
				Param p{};
				bool have_key = false;
				if (!r.enter_object()) return false;
				json::Span pk;
				while (r.next_key(pk)) {
					if (pk == "key") have_key = r.read_string(p.key, sizeof(p.key));
					else if (pk == "value") read_param_value(r, p.value, sizeof(p.value));
					else r.skip();
				}
				if (have_key && params->n < MAX_PARAMS) params->p[params->n++] = p;
			}
		} else {
			r.skip();   // actionDescriptor, retriable
		}
		if (!r.ok()) return false;
	}
	if (!(have_type && have_block && (have_id || !require_id))) {
		r.fail();
		return false;
	}
	a.kind = action_kind(a.type);
	a.robot_side = (a.kind == ActionKind::Pick || a.kind == ActionKind::Drop);
	return r.ok();
}

} // namespace detail

// =============================================================================
// Order parsing
// =============================================================================
namespace {

enum class ParseResult : uint8_t { Ok, Malformed, TooBig, InvalidAction, UnknownMap };

struct ParseCtx {
	ParseResult result = ParseResult::Ok;
	char detail[ID_LEN] = "";
	const MapEntry *maps = nullptr;

	void fail(ParseResult r, const char *d) {
		if (result == ParseResult::Ok) {
			result = r;
			copy_str(detail, d);
		}
	}
	bool map_known(const char *id) const {
		for (size_t i = 0; i < MAX_MAPS; ++i) {
			if (maps[i].used && maps[i].enabled && strcmp(maps[i].id, id) == 0) return true;
		}
		return false;
	}
};

bool read_u32(json::JsonReader &r, uint32_t &out) {
	int64_t v = 0;
	if (!r.read_int(v) || v < 0 || v > 0xFFFFFFFFLL) {
		r.fail();
		return false;
	}
	out = static_cast<uint32_t>(v);
	return true;
}

bool read_float(json::JsonReader &r, float &out) {
	double d = 0;
	if (!r.read_number(d)) return false;
	out = static_cast<float>(d);
	return true;
}

bool parse_actions_into(json::JsonReader &r, Order &o, int32_t marker, ParseCtx &ctx) {
	if (!r.enter_array()) return false;
	while (r.next_element()) {
		if (o.nactions >= MAX_ACTIONS) {
			ctx.fail(ParseResult::TooBig, "actions");
			r.skip();
			continue;
		}
		ParseErr err;
		if (!parse_action(r, o.actions[o.nactions], marker, true, err)) return false;
		if (err.too_long) ctx.fail(ParseResult::TooBig, err.detail);
		++o.nactions;
	}
	return r.ok();
}

bool parse_node(json::JsonReader &r, Order &o, ParseCtx &ctx) {
	if (o.nnodes >= MAX_NODES) {
		ctx.fail(ParseResult::TooBig, "nodes");
		r.skip();
		return r.ok();
	}
	Node &n = o.nodes[o.nnodes];
	memset(&n, 0, sizeof(n));
	bool have_id = false, have_seq = false, have_rel = false, have_pos = false, have_actions = false;
	if (!r.enter_object()) return false;
	json::Span k;
	while (r.next_key(k)) {
		if (k == "nodeId") {
			have_id = r.read_string(n.id, sizeof(n.id));
			if (!have_id) ctx.fail(ParseResult::TooBig, "nodeId");
		} else if (k == "sequenceId") {
			have_seq = read_u32(r, n.seq);
		} else if (k == "released") {
			have_rel = r.read_bool(n.released);
		} else if (k == "nodePosition") {
			bool hx = false, hy = false, hm = false;
			if (!r.enter_object()) return false;
			json::Span pk;
			while (r.next_key(pk)) {
				if (pk == "x") hx = read_float(r, n.x);
				else if (pk == "y") hy = read_float(r, n.y);
				else if (pk == "theta") n.has_theta = read_float(r, n.theta);
				else if (pk == "mapId") hm = r.read_string(n.map, sizeof(n.map));
				else if (pk == "allowedDeviationXY") {
					if (!r.enter_object()) return false;
					json::Span dk;
					while (r.next_key(dk)) {
						float v = 0;
						if ((dk == "a" || dk == "b") && read_float(r, v)) {
							if (v > n.dev_xy) n.dev_xy = v;
						} else if (!(dk == "a" || dk == "b")) {
							r.skip();
						}
					}
				} else {
					r.skip();   // allowedDeviationTheta
				}
			}
			have_pos = hx && hy && hm;
			if (!have_pos) r.fail();
			if (hm && !ctx.map_known(n.map)) ctx.fail(ParseResult::UnknownMap, n.map);
		} else if (k == "actions") {
			have_actions = true;
			if (!parse_actions_into(r, o, -1000, ctx)) return false;
		} else {
			r.skip();   // nodeDescriptor
		}
		if (!r.ok()) return false;
	}
	if (!(have_id && have_seq && have_rel && have_actions)) {
		r.fail();
		return false;
	}
	if (!have_pos) ctx.fail(ParseResult::Malformed, "nodePosition");
	for (uint8_t i = 0; i < o.nactions; ++i) {
		if (o.actions[i].owner_seq == -1000) {
			o.actions[i].owner_seq = static_cast<int32_t>(n.seq);
			if (!allowed_on_node(o.actions[i].kind)) ctx.fail(ParseResult::InvalidAction, o.actions[i].id);
		}
	}
	++o.nnodes;
	return r.ok();
}

bool parse_trajectory(json::JsonReader &r, geo::Nurbs &t) {
	memset(&t, 0, sizeof(t));
	t.degree = 1;
	bool have_cps = false;
	if (!r.enter_object()) return false;
	json::Span k;
	while (r.next_key(k)) {
		if (k == "degree") {
			int64_t d = 0;
			if (!r.read_int(d) || d < 1 || d > 7) { r.fail(); return false; }
			t.degree = static_cast<uint8_t>(d);
		} else if (k == "knotVector") {
			if (!r.enter_array()) return false;
			while (r.next_element()) {
				float v = 0;
				if (!read_float(r, v)) return false;
				if (t.nknots >= geo::MAX_KNOTS) { r.fail(); return false; }
				t.knots[t.nknots++] = v;
			}
		} else if (k == "controlPoints") {
			have_cps = true;
			if (!r.enter_array()) return false;
			while (r.next_element()) {
				if (t.ncp >= geo::MAX_CONTROL_POINTS) { r.fail(); return false; }
				geo::Vec2 &p = t.cp[t.ncp];
				float w = 1.0F;
				bool hx = false, hy = false;
				if (!r.enter_object()) return false;
				json::Span ck;
				while (r.next_key(ck)) {
					if (ck == "x") hx = read_float(r, p.x);
					else if (ck == "y") hy = read_float(r, p.y);
					else if (ck == "weight") read_float(r, w);
					else r.skip();
				}
				if (!(hx && hy)) { r.fail(); return false; }
				t.w[t.ncp++] = w;
			}
		} else {
			r.skip();
		}
		if (!r.ok()) return false;
	}
	return have_cps && r.ok();
}

bool parse_corridor(json::JsonReader &r, Corridor &c) {
	memset(&c, 0, sizeof(c));
	bool hl = false, hr = false;
	if (!r.enter_object()) return false;
	json::Span k;
	while (r.next_key(k)) {
		char s[24];
		if (k == "leftWidth") hl = read_float(r, c.left);
		else if (k == "rightWidth") hr = read_float(r, c.right);
		else if (k == "releaseRequired") r.read_bool(c.release_required);
		else if (k == "releaseLossBehavior") {
			if (r.read_string(s, sizeof(s))) c.return_on_loss = strcmp(s, "RETURN") == 0;
		} else if (k == "corridorReferencePoint") {
			if (r.read_string(s, sizeof(s))) c.contour_ref = strcmp(s, "CONTOUR") == 0;
		} else {
			r.skip();
		}
		if (!r.ok()) return false;
	}
	c.present = hl && hr && c.left >= 0.0F && c.right >= 0.0F;
	if (!c.present) r.fail();
	return r.ok();
}

bool parse_edge(json::JsonReader &r, Order &o, ParseCtx &ctx) {
	if (o.nedges >= MAX_EDGES) {
		ctx.fail(ParseResult::TooBig, "edges");
		r.skip();
		return r.ok();
	}
	Edge &e = o.edges[o.nedges];
	memset(&e, 0, sizeof(e));
	e.traj = -1;
	bool have_id = false, have_seq = false, have_rel = false, have_actions = false;
	if (!r.enter_object()) return false;
	json::Span k;
	while (r.next_key(k)) {
		if (k == "edgeId") {
			have_id = r.read_string(e.id, sizeof(e.id));
			if (!have_id) ctx.fail(ParseResult::TooBig, "edgeId");
		} else if (k == "sequenceId") {
			have_seq = read_u32(r, e.seq);
		} else if (k == "released") {
			have_rel = r.read_bool(e.released);
		} else if (k == "maximumSpeed") {
			read_float(r, e.max_speed);
		} else if (k == "trajectory") {
			if (o.ntraj >= MAX_TRAJ) {
				ctx.fail(ParseResult::TooBig, "trajectory");
				r.skip();
			} else {
				if (!parse_trajectory(r, o.traj[o.ntraj])) return false;
				if (!geo::nurbs_prepare(o.traj[o.ntraj])) {
					ctx.fail(ParseResult::Malformed, "trajectory");
				} else {
					e.traj = static_cast<int8_t>(o.ntraj++);
				}
			}
		} else if (k == "corridor") {
			if (!parse_corridor(r, e.corridor)) return false;
		} else if (k == "actions") {
			have_actions = true;
			if (!parse_actions_into(r, o, -1001, ctx)) return false;
		} else {
			r.skip();   // descriptor, heights, orientation, direction, length, ...
		}
		if (!r.ok()) return false;
	}
	if (!(have_id && have_seq && have_rel && have_actions)) {
		r.fail();
		return false;
	}
	for (uint8_t i = 0; i < o.nactions; ++i) {
		if (o.actions[i].owner_seq == -1001) {
			o.actions[i].owner_seq = static_cast<int32_t>(e.seq);
			if (!allowed_on_edge(o.actions[i].kind)) ctx.fail(ParseResult::InvalidAction, o.actions[i].id);
		}
	}
	++o.nedges;
	return r.ok();
}

void sort_actions_by_owner(Order &o) {
	for (uint8_t i = 1; i < o.nactions; ++i) {
		Action tmp = o.actions[i];
		int j = i - 1;
		while (j >= 0 && o.actions[j].owner_seq > tmp.owner_seq) {
			o.actions[j + 1] = o.actions[j];
			--j;
		}
		o.actions[j + 1] = tmp;
	}
}

bool structure_ok(const Order &o) {
	if (o.nnodes == 0 || o.nedges + 1 != o.nnodes) return false;
	const uint32_t s0 = o.nodes[0].seq;
	for (uint8_t i = 0; i < o.nnodes; ++i) {
		if (o.nodes[i].seq != s0 + 2U * i) return false;
	}
	for (uint8_t i = 0; i < o.nedges; ++i) {
		if (o.edges[i].seq != s0 + 2U * i + 1U) return false;
	}
	if (!o.nodes[0].released) return false;
	bool horizon = false;
	for (uint8_t i = 0; i < o.nnodes; ++i) {
		if (i > 0) {
			const Edge &e = o.edges[i - 1];
			if (e.released && (horizon || !o.nodes[i].released)) return false;
			if (!e.released) horizon = true;
		}
		if (o.nodes[i].released && horizon) return false;
		if (!o.nodes[i].released) horizon = true;
	}
	for (uint8_t i = 0; i < o.nactions; ++i) {
		for (uint8_t j = i + 1; j < o.nactions; ++j) {
			if (strcmp(o.actions[i].id, o.actions[j].id) == 0) return false;
		}
	}
	return true;
}

ParseResult parse_order_text(const char *json_text, size_t len, Order &o, ParseCtx &ctx) {
	o.nnodes = o.nedges = o.nactions = o.ntraj = 0;
	o.order_id[0] = '\0';
	o.update_id = 0;
	o.content_hash = 2166136261U;
	bool have_id = false, have_upd = false, have_nodes = false, have_edges = false;

	json::JsonReader r(json_text, len);
	if (!r.enter_object()) return ParseResult::Malformed;
	json::Span k;
	while (r.next_key(k)) {
		if (k == "orderId") {
			have_id = r.read_string(o.order_id, sizeof(o.order_id));
			if (!have_id) ctx.fail(ParseResult::TooBig, "orderId");
		} else if (k == "orderUpdateId") {
			have_upd = read_u32(r, o.update_id);
		} else if (k == "nodes" || k == "edges") {
			const bool nodes = (k == "nodes");
			(nodes ? have_nodes : have_edges) = true;
			const char *start = r.pos();
			if (!r.enter_array()) break;
			while (r.next_element()) {
				if (!(nodes ? parse_node(r, o, ctx) : parse_edge(r, o, ctx))) break;
			}
			o.content_hash = fnv1a(start, static_cast<size_t>(r.pos() - start), o.content_hash);
		} else {
			r.skip();
		}
		if (!r.ok()) break;
	}
	if (!r.ok() || !(have_id && have_upd && have_nodes && have_edges)) return ParseResult::Malformed;
	if (ctx.result != ParseResult::Ok) return ctx.result;
	if (!structure_ok(o)) {
		ctx.fail(ParseResult::Malformed, "sequence/release structure");
		return ParseResult::Malformed;
	}
	sort_actions_by_owner(o);
	return ParseResult::Ok;
}

} // namespace

// =============================================================================
// Free helpers
// =============================================================================
uint32_t fnv1a(const char *p, size_t n, uint32_t h) {
	for (size_t i = 0; i < n; ++i) {
		h ^= static_cast<uint8_t>(p[i]);
		h *= 16777619U;
	}
	return h;
}

ActionKind action_kind(const char *t) {
	struct Entry { const char *name; ActionKind kind; };
	static const Entry table[] = {
		{ "pick", ActionKind::Pick }, { "drop", ActionKind::Drop },
		{ "waitForTrigger", ActionKind::WaitForTrigger }, { "finePositioning", ActionKind::FinePositioning },
		{ "startCharging", ActionKind::StartCharging }, { "stopCharging", ActionKind::StopCharging },
		{ "cancelOrder", ActionKind::CancelOrder }, { "startPause", ActionKind::StartPause },
		{ "stopPause", ActionKind::StopPause }, { "stateRequest", ActionKind::StateRequest },
		{ "factsheetRequest", ActionKind::FactsheetRequest }, { "initializePosition", ActionKind::InitializePosition },
		{ "clearInstantActions", ActionKind::ClearInstantActions }, { "trigger", ActionKind::Trigger },
		{ "startHibernation", ActionKind::StartHibernation }, { "stopHibernation", ActionKind::StopHibernation },
		{ "shutdown", ActionKind::Shutdown },
		{ "downloadMap", ActionKind::DownloadMap }, { "enableMap", ActionKind::EnableMap },
		{ "deleteMap", ActionKind::DeleteMap },
		{ "enableZoneSet", ActionKind::EnableZoneSet }, { "deleteZoneSet", ActionKind::DeleteZoneSet },
		{ "clearZoneActions", ActionKind::ClearZoneActions },
		{ "mcoreSimObstacle", ActionKind::SimObstacle }, { "mcoreSimClearObstacles", ActionKind::SimClearObstacles },
	};
	for (const Entry &e : table) {
		if (strcmp(t, e.name) == 0) return e.kind;
	}
	return ActionKind::Unknown;
}

const char *to_string(ActionStatus s) {
	switch (s) {
	case ActionStatus::Waiting: return "WAITING";
	case ActionStatus::Initializing: return "INITIALIZING";
	case ActionStatus::Running: return "RUNNING";
	case ActionStatus::Paused: return "PAUSED";
	case ActionStatus::Retriable: return "RETRIABLE";
	case ActionStatus::Finished: return "FINISHED";
	case ActionStatus::Failed: return "FAILED";
	}
	return "FAILED";
}

const char *to_string(ErrorLevel l) {
	switch (l) {
	case ErrorLevel::Warning: return "WARNING";
	case ErrorLevel::Urgent: return "URGENT";
	case ErrorLevel::Critical: return "CRITICAL";
	case ErrorLevel::Fatal: return "FATAL";
	}
	return "WARNING";
}

const char *to_string(ReqStatus s) {
	switch (s) {
	case ReqStatus::Requested: return "REQUESTED";
	case ReqStatus::Granted: return "GRANTED";
	case ReqStatus::Revoked: return "REVOKED";
	case ReqStatus::Expired: return "EXPIRED";
	}
	return "REQUESTED";
}

// =============================================================================
// OrderManager — setup and inspection
// =============================================================================
void OrderManager::init(const Config &cfg, RobotPort *robot) {
	cfg_ = cfg;
	robot_port_ = robot;
	robot_ = {};
	have_order_ = false;
	cancelled_ = false;
	cur_.nnodes = cur_.nedges = cur_.nactions = cur_.ntraj = 0;
	cur_.order_id[0] = '\0';
	cur_.update_id = 0;
	first_pending_node_ = first_pending_edge_ = gen_nodes_ = 0;
	last_node_id_[0] = '\0';
	last_node_seq_ = 0;
	ninstant_ = 0;
	pending_cancel_uid_ = 0;
	next_uid_ = 1;
	nerrors_ = 0;
	path_base_ = path_gen_ = path_sent_ = path_done_ = 0;
	stop_seq_ = 0;
	replanning_ = false;
	hold_mask_ = 0;
	speed_limit_ = 0.0F;
	blocked_reported_ = false;
	waiting_obstacle_ = false;
	memset(zone_sets_, 0, sizeof(zone_sets_));
	nzone_actions_ = 0;
	memset(requests_, 0, sizeof(requests_));
	memset(maps_, 0, sizeof(maps_));
	maps_[0].used = true;
	maps_[0].enabled = true;
	copy_str(maps_[0].id, cfg.map_id);
	copy_str(maps_[0].version, "1");
	copy_str(current_map_, cfg.map_id);
	download_uid_ = 0;
	memset(obstacles_, 0, sizeof(obstacles_));
	hibernating_ = hibernate_pending_ = shutdown_ = shutdown_pending_ = false;
	wake_unix_ms_ = 0;
	dirty_ = true;
	factsheet_req_ = false;
}

bool OrderManager::idle() const {
	if (pending_cancel_uid_ != 0) return false;
	if (first_pending_node_ < cur_.nnodes || first_pending_edge_ < cur_.nedges) return false;
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		if (!is_final(cur_.actions[i].status)) return false;
	}
	return true;
}

const Action *OrderManager::find_action(const char *id) const {
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		if (strcmp(cur_.actions[i].id, id) == 0) return &cur_.actions[i];
	}
	return nullptr;
}

const Action *OrderManager::find_instant(const char *id) const {
	for (uint8_t i = 0; i < ninstant_; ++i) {
		if (strcmp(instant_[i].id, id) == 0) return &instant_[i];
	}
	return nullptr;
}

Action *OrderManager::find_instant_uid(uint8_t uid) {
	for (uint8_t i = 0; i < ninstant_; ++i) {
		if (instant_[i].uid == uid) return &instant_[i];
	}
	return nullptr;
}

Action *OrderManager::find_by_uid(uint8_t uid) {
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		if (cur_.actions[i].uid == uid) return &cur_.actions[i];
	}
	for (uint8_t i = 0; i < nzone_actions_; ++i) {
		if (zone_actions_[i].uid == uid) return &zone_actions_[i];
	}
	return nullptr;
}

bool OrderManager::has_error(const char *type) const {
	for (uint8_t i = 0; i < nerrors_; ++i) {
		if (strcmp(errors_[i].type, type) == 0) return true;
	}
	return false;
}

bool OrderManager::map_enabled(const char *id) const {
	for (const MapEntry &m : maps_) {
		if (m.used && m.enabled && strcmp(m.id, id) == 0) return true;
	}
	return false;
}

const char *OrderManager::connection_state() const {
	if (shutdown_) return "OFFLINE";
	if (hibernating_) return "HIBERNATING";
	return "ONLINE";
}

// --- errors -------------------------------------------------------------------
void OrderManager::remove_error(const char *type) {
	for (uint8_t i = 0; i < nerrors_;) {
		if (strcmp(errors_[i].type, type) == 0) {
			errors_[i] = errors_[--nerrors_];
			mark_dirty();
		} else {
			++i;
		}
	}
}

void OrderManager::add_error(const char *type, ErrorLevel level, uint8_t clear_on, const char *descr,
		const char *k1, const char *v1, const char *k2, const char *v2, const char *k3, const char *v3) {
	remove_error(type);
	if (nerrors_ >= MAX_ERRORS) {
		for (uint8_t i = 1; i < nerrors_; ++i) errors_[i - 1] = errors_[i];
		--nerrors_;
	}
	Error &e = errors_[nerrors_++];
	memset(&e, 0, sizeof(e));
	copy_str(e.type, type);
	copy_str(e.description, descr);
	e.level = level;
	e.clear_on = clear_on;
	e.expires_ms = now_ms_ + 30000U;   // AfterTimeout: "reasonable amount of time"
	const char *ks[3] = { k1, k2, k3 };
	const char *vs[3] = { v1, v2, v3 };
	for (int i = 0; i < 3; ++i) {
		if (ks[i] != nullptr && vs[i] != nullptr) {
			copy_str(e.refs[e.nrefs].key, ks[i]);
			copy_str(e.refs[e.nrefs].value, vs[i]);
			++e.nrefs;
		}
	}
	mark_dirty();
}

void OrderManager::clear_errors(uint8_t clear_on) {
	for (uint8_t i = 0; i < nerrors_;) {
		if (errors_[i].clear_on == clear_on) {
			errors_[i] = errors_[--nerrors_];
			mark_dirty();
		} else {
			++i;
		}
	}
}

void OrderManager::reject(const char *type, ErrorLevel level, const Order &in, const char *descr) {
	char upd[12];
	u32_to_str(in.update_id, upd);
	add_error(type, level, UntilNewOrder, descr, "topic", "order",
	          in.order_id[0] != '\0' ? "orderId" : nullptr, in.order_id, "orderUpdateId", upd);
}

// =============================================================================
// Orders
// =============================================================================
void OrderManager::on_order(const char *json_text, size_t len) {
	if (hibernating_ || shutdown_) return;   // 6.2.3: no reaction while hibernating
	mark_dirty();
	ParseCtx ctx;
	ctx.maps = maps_;
	const ParseResult pr = parse_order_text(json_text, len, in_, ctx);

	switch (pr) {
	case ParseResult::Ok:
		break;
	case ParseResult::Malformed:
		reject("VALIDATION_FAILURE", ErrorLevel::Warning, in_, ctx.detail[0] ? ctx.detail : "malformed order");
		return;
	case ParseResult::TooBig:
		reject("INSUFFICIENT_MEMORY", ErrorLevel::Urgent, in_, ctx.detail);
		return;
	case ParseResult::InvalidAction:
		add_error("INVALID_ORDER_ACTION", ErrorLevel::Warning, UntilNewOrder, "action not supported here",
		          "actionId", ctx.detail, "orderId", in_.order_id);
		return;
	case ParseResult::UnknownMap:
		reject("UNKNOWN_MAP_ID", ErrorLevel::Warning, in_, ctx.detail);
		return;
	}

	if (!robot_.valid) {
		reject("MOBILE_ROBOT_NOT_AVAILABLE", ErrorLevel::Warning, in_, "operating mode STARTUP");
		return;
	}

	if (!have_order_ || strcmp(in_.order_id, cur_.order_id) != 0) {
		if (!idle()) {
			reject("OTHER_ORDER_ACTIVE", ErrorLevel::Warning, in_, "robot is not idle");
			return;
		}
		const Node &n0 = in_.nodes[0];
		const float range = n0.dev_xy > 0.0F ? n0.dev_xy : cfg_.default_dev_xy;
		const float dx = n0.x - robot_.x;
		const float dy = n0.y - robot_.y;
		const bool on_node = strcmp(n0.id, last_node_id_) == 0;
		if (!on_node && (dx * dx + dy * dy) > range * range) {
			reject("START_NODE_OUT_OF_RANGE", ErrorLevel::Warning, in_, n0.id);
			return;
		}
		accept_new(in_);
		return;
	}

	if (cancelled_) {
		reject("ORDER_UPDATE_FOLLOWING_CANCEL", ErrorLevel::Warning, in_, "order was cancelled");
		return;
	}
	if (in_.update_id < cur_.update_id) {
		reject("OUTDATED_ORDER_UPDATE", ErrorLevel::Warning, in_, "orderUpdateId is older");
		return;
	}
	if (in_.update_id == cur_.update_id) {
		if (in_.content_hash != cur_.content_hash) {
			reject("SAME_ORDER_UPDATE_ID", ErrorLevel::Warning, in_, "same orderUpdateId, different content");
		}
		return;
	}
	(void) accept_update(in_);
}

void OrderManager::accept_new(const Order &in) {
	cur_ = in;
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		cur_.actions[i].uid = next_uid_++;
		if (next_uid_ == 0) next_uid_ = 1;
	}
	have_order_ = true;
	cancelled_ = false;
	first_pending_node_ = 0;
	first_pending_edge_ = 0;
	gen_nodes_ = 1;                // the first node is where the robot already is
	path_base_ = path_gen_ = path_sent_;
	if (path_done_ < path_sent_) path_done_ = path_sent_;
	blocked_reported_ = false;
	waiting_obstacle_ = false;
	clear_errors(UntilNewOrder);
	traverse_first_node();
	mark_dirty();
}

bool OrderManager::accept_update(const Order &in) {
	int dp = -1;
	for (int i = 0; i < cur_.nnodes; ++i) {
		if (cur_.nodes[i].released) dp = i;
	}
	const Node &first = in.nodes[0];
	if (dp < 0 || strcmp(first.id, cur_.nodes[dp].id) != 0 || first.seq != cur_.nodes[dp].seq) {
		reject("VALIDATION_FAILURE", ErrorLevel::Warning, in, "order update does not start at the decision point");
		return false;
	}
	const uint32_t dp_seq = cur_.nodes[dp].seq;

	int keep_actions = 0;
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		if (cur_.actions[i].owner_seq <= static_cast<int32_t>(dp_seq)) ++keep_actions;
	}
	int new_actions = 0;
	for (uint8_t i = 0; i < in.nactions; ++i) {
		if (in.actions[i].owner_seq > static_cast<int32_t>(dp_seq)) ++new_actions;
	}
	// Traversed nodes can be dropped to make room (keep the last traversed one,
	// and every node whose approach may still be streamed)
	int drop = first_pending_node_ > 0 ? first_pending_node_ - 1 : 0;
	if (drop > dp) drop = dp;
	// trajectories referenced by kept edges
	if ((dp + 1 - drop) + (in.nnodes - 1) > static_cast<int>(MAX_NODES) ||
	    keep_actions + new_actions > static_cast<int>(MAX_ACTIONS)) {
		reject("INSUFFICIENT_MEMORY", ErrorLevel::Urgent, in, "order too long");
		return false;
	}

	// 1. drop the horizon and its actions
	cur_.nnodes = static_cast<uint8_t>(dp + 1);
	cur_.nedges = static_cast<uint8_t>(dp);
	uint8_t w = 0;
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		if (cur_.actions[i].owner_seq <= static_cast<int32_t>(dp_seq)) cur_.actions[w++] = cur_.actions[i];
	}
	cur_.nactions = w;

	// 2. drop traversed nodes/edges from the front
	if (drop > 0) {
		memmove(&cur_.nodes[0], &cur_.nodes[drop], sizeof(Node) * static_cast<size_t>(cur_.nnodes - drop));
		cur_.nnodes = static_cast<uint8_t>(cur_.nnodes - drop);
		memmove(&cur_.edges[0], &cur_.edges[drop], sizeof(Edge) * static_cast<size_t>(cur_.nedges - drop));
		cur_.nedges = static_cast<uint8_t>(cur_.nedges - drop);
		first_pending_node_ = static_cast<uint8_t>(first_pending_node_ - drop);
		first_pending_edge_ = static_cast<uint8_t>(first_pending_edge_ > drop ? first_pending_edge_ - drop : 0);
		gen_nodes_ = static_cast<uint8_t>(gen_nodes_ > drop ? gen_nodes_ - drop : 1);
		for (uint32_t i = path_done_; i < path_gen_; ++i) {
			PathPoint &p = pp(i);
			p.edge = static_cast<uint8_t>(p.edge >= drop ? p.edge - drop : 0);
		}
		for (Request &r : requests_) {
			if (r.used && r.type == ReqType::Corridor) r.edge = static_cast<uint8_t>(r.edge >= drop ? r.edge - drop : 0);
		}
	}

	// 3. compact the trajectory pool to the kept edges, then append the new part
	geo::Nurbs kept[MAX_TRAJ];
	uint8_t nk = 0;
	for (uint8_t i = 0; i < cur_.nedges; ++i) {
		if (cur_.edges[i].traj >= 0) {
			kept[nk] = cur_.traj[cur_.edges[i].traj];
			cur_.edges[i].traj = static_cast<int8_t>(nk++);
		}
	}
	if (nk + in.ntraj > MAX_TRAJ) {
		reject("INSUFFICIENT_MEMORY", ErrorLevel::Urgent, in, "too many trajectories");
		return false;   // (state already trimmed of the horizon, which is allowed)
	}
	for (uint8_t i = 0; i < nk; ++i) cur_.traj[i] = kept[i];
	cur_.ntraj = nk;
	for (uint8_t i = 1; i < in.nnodes; ++i) cur_.nodes[cur_.nnodes++] = in.nodes[i];
	for (uint8_t i = 0; i < in.nedges; ++i) {
		Edge e = in.edges[i];
		if (e.traj >= 0) {
			cur_.traj[cur_.ntraj] = in.traj[e.traj];
			e.traj = static_cast<int8_t>(cur_.ntraj++);
		}
		cur_.edges[cur_.nedges++] = e;
	}
	for (uint8_t i = 0; i < in.nactions; ++i) {
		if (in.actions[i].owner_seq > static_cast<int32_t>(dp_seq)) {
			Action a = in.actions[i];
			a.uid = next_uid_++;
			if (next_uid_ == 0) next_uid_ = 1;
			cur_.actions[cur_.nactions++] = a;
		}
	}
	cur_.update_id = in.update_id;
	cur_.content_hash = in.content_hash;
	if (first_pending_node_ > 0 && last_node_seq_ == dp_seq) {
		first_pending_edge_ = static_cast<uint8_t>(first_pending_node_ - 1);
	}
	blocked_reported_ = false;
	clear_errors(UntilNewOrder);
	mark_dirty();
	return true;
}

// --- traversal --------------------------------------------------------------------
void OrderManager::traverse_first_node() {
	const Node &n = cur_.nodes[0];
	copy_str(last_node_id_, n.id);
	last_node_seq_ = n.seq;
	first_pending_node_ = 1;
	first_pending_edge_ = 0;
	mark_dirty();
}

void OrderManager::leave_edge(uint32_t edge_seq) {
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		Action &a = cur_.actions[i];
		if (a.owner_seq == static_cast<int32_t>(edge_seq) && !is_final(a.status)) {
			if (a.robot_side && a.status != ActionStatus::Waiting) robot_port_->cancel_action(a.uid);
			a.status = ActionStatus::Finished;   // 6.6.2: edge actions end with the edge
		}
	}
	// a granted corridor ends with its edge
	for (Request &r : requests_) {
		if (r.used && r.type == ReqType::Corridor && r.edge_seq == edge_seq) r.used = false;
	}
	mark_dirty();
}

void OrderManager::on_node_reached(uint32_t seq) {
	for (uint8_t i = first_pending_node_; i < gen_nodes_ && i < cur_.nnodes; ++i) {
		if (cur_.nodes[i].seq != seq) continue;
		for (uint8_t j = first_pending_node_; j <= i; ++j) {
			const Node &n = cur_.nodes[j];
			leave_edge(n.seq - 1U);
			copy_str(last_node_id_, n.id);
			last_node_seq_ = n.seq;
			first_pending_node_ = static_cast<uint8_t>(j + 1);
			first_pending_edge_ = j;
		}
		mark_dirty();
		return;
	}
}

void OrderManager::on_action_done(uint8_t uid, bool ok) {
	Action *a = find_by_uid(uid);
	if (a != nullptr && !is_final(a->status)) {
		a->status = ok ? ActionStatus::Finished : ActionStatus::Failed;
		mark_dirty();
	}
	if (download_uid_ == uid) {
		// not used: downloads are timed in poll()
	}
}

void OrderManager::update_robot(const RobotStatus &s) {
	if (s.driving != robot_.driving || s.paused != robot_.paused || s.loaded != robot_.loaded ||
	    s.valid != robot_.valid) {
		mark_dirty();
	}
	if (pose_pending_ && std::fabs(s.x - pose_x_) < 0.01F && std::fabs(s.y - pose_y_) < 0.01F) {
		pose_pending_ = false;
		mark_dirty();                  // report the new pose right away
	}
	robot_ = s;
	if (robot_.any_popped && robot_.last_popped + 1U > path_done_ && robot_.last_popped + 1U <= path_sent_) {
		path_done_ = robot_.last_popped + 1U;
	}
}

// --- actions ------------------------------------------------------------------------
void OrderManager::start_action_common(Action &a) {
	switch (a.kind) {
	case ActionKind::Pick:
	case ActionKind::Drop:
		a.status = robot_port_->start_action(a.uid, a) ? ActionStatus::Running : ActionStatus::Failed;
		break;
	case ActionKind::WaitForTrigger:
		a.status = ActionStatus::Running;   // until instant action 'trigger'
		break;
	case ActionKind::FinePositioning:
		a.status = ActionStatus::Finished;  // targets are approached exactly already
		break;
	case ActionKind::StartCharging:
		charging_ = true;
		a.status = ActionStatus::Finished;
		break;
	case ActionKind::StopCharging:
		charging_ = false;
		a.status = ActionStatus::Finished;
		break;
	default:
		a.status = ActionStatus::Failed;
		break;
	}
	mark_dirty();
}

// Figure 11: NONE/SOFT run in parallel, SINGLE/HARD run alone.
// Eligible = node traversed (owner <= lastNodeSeq) or edge entered (owner == lastNodeSeq + 1).
void OrderManager::process_actions() {
	if (pending_cancel_uid_ != 0) return;
	bool any_running = false;
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		Action &a = cur_.actions[i];
		if (is_final(a.status)) continue;
		if (a.owner_seq > static_cast<int32_t>(last_node_seq_) + 1) break;
		const bool on_edge = (a.owner_seq % 2) != static_cast<int32_t>(cur_.nodes[0].seq % 2);
		if (on_edge) {
			const uint32_t idx = (static_cast<uint32_t>(a.owner_seq) - cur_.nodes[0].seq) / 2U;
			if (idx >= cur_.nedges || !cur_.edges[idx].released) break;
		}
		if (a.robot_side && a.status == ActionStatus::Running && robot_.paused) {
			a.status = ActionStatus::Paused;
			mark_dirty();
		} else if (a.robot_side && a.status == ActionStatus::Paused && !robot_.paused) {
			a.status = ActionStatus::Running;
			mark_dirty();
		}
		if (a.status == ActionStatus::Waiting) {
			if (robot_.paused) break;
			if (is_exclusive(a.blocking)) {
				if (any_running) break;
				start_action_common(a);
				if (!is_final(a.status)) break;
			} else {
				start_action_common(a);
				if (!is_final(a.status)) any_running = true;
			}
		} else {
			if (is_exclusive(a.blocking)) break;
			any_running = true;
		}
	}
}

bool OrderManager::blocking_pending() const {
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		const Action &a = cur_.actions[i];
		if (a.owner_seq > static_cast<int32_t>(last_node_seq_) + 1) break;
		if (!is_final(a.status) && is_blocking_drive(a.blocking)) return true;
	}
	return false;
}

void OrderManager::poll(uint32_t now_ms, uint64_t unix_ms) {
	now_ms_ = now_ms;
	unix_ms_ = unix_ms;

	// timed errors (DUPLICATE_ZONE_SET)
	for (uint8_t i = 0; i < nerrors_;) {
		if (errors_[i].clear_on == AfterTimeout && static_cast<int32_t>(now_ms - errors_[i].expires_ms) >= 0) {
			errors_[i] = errors_[--nerrors_];
			mark_dirty();
		} else {
			++i;
		}
	}

	if (hibernating_) {
		if (wake_unix_ms_ != 0 && unix_ms != 0 && unix_ms >= wake_unix_ms_) {
			hibernating_ = false;     // autonomous wake-up (6.2.3)
			wake_unix_ms_ = 0;
			mark_dirty();
		}
		return;
	}

	if (pending_cancel_uid_ != 0 && !robot_.driving) {
		Action *c = find_instant_uid(pending_cancel_uid_);
		if (c != nullptr) c->status = ActionStatus::Finished;
		pending_cancel_uid_ = 0;
		mark_dirty();
	}
	for (uint8_t i = 0; i < ninstant_; ++i) {
		Action &a = instant_[i];
		if (a.status != ActionStatus::Running) continue;
		if ((a.kind == ActionKind::StartPause && robot_.paused) ||
		    (a.kind == ActionKind::StopPause && !robot_.paused)) {
			a.status = ActionStatus::Finished;
			mark_dirty();
		}
	}
	// simulated map download
	if (download_uid_ != 0 && static_cast<int32_t>(now_ms - download_done_ms_) >= 0) {
		Action *d = find_instant_uid(download_uid_);
		MapEntry *slot = nullptr;
		for (MapEntry &m : maps_) {
			if (!m.used) { slot = &m; break; }
		}
		if (slot != nullptr) {
			*slot = download_;
			slot->used = true;
			slot->enabled = false;   // 6.3.3: downloaded maps start DISABLED
		}
		if (d != nullptr) d->status = slot != nullptr ? ActionStatus::Finished : ActionStatus::Failed;
		if (slot == nullptr) {
			add_error("INSUFFICIENT_MEMORY", ErrorLevel::Urgent, UntilNewInstantAction, "no room for another map");
		}
		download_uid_ = 0;
		mark_dirty();
	}
	if (robot_.driving != last_driving_ || robot_.paused != last_paused_) {
		last_driving_ = robot_.driving;
		last_paused_ = robot_.paused;
		mark_dirty();
	}
	process_actions();
	process_zones();
	process_requests();
	process_zone_actions();
	process_motion();
}

bool OrderManager::state_due(uint32_t now_ms) const {
	if (hibernating_ || shutdown_) return false;
	const uint32_t since = now_ms - last_state_ms_;
	if (dirty_ && since >= cfg_.min_state_interval_ms) return true;
	const uint32_t period = robot_.driving ? cfg_.state_interval_driving_ms : cfg_.state_interval_ms;
	return since >= period;
}

void OrderManager::state_sent(uint32_t now_ms) {
	dirty_ = false;
	last_state_ms_ = now_ms;
	// The FINISHED startHibernation / shutdown has been reported: go quiet
	if (hibernate_pending_) {
		hibernate_pending_ = false;
		hibernating_ = true;
	}
	if (shutdown_pending_) {
		shutdown_pending_ = false;
		shutdown_ = true;
	}
}

// =============================================================================
// Instant actions
// =============================================================================
Action *OrderManager::add_instant(const Action &a) {
	if (ninstant_ >= MAX_INSTANT) {
		for (uint8_t i = 0; i < ninstant_; ++i) {
			if (is_final(instant_[i].status)) {
				for (uint8_t j = i + 1; j < ninstant_; ++j) instant_[j - 1] = instant_[j];
				--ninstant_;
				break;
			}
		}
		if (ninstant_ >= MAX_INSTANT) return nullptr;
	}
	instant_[ninstant_] = a;
	return &instant_[ninstant_++];
}

// Stops everything of the current order (6.6.7); ids and lastNode are kept.
void OrderManager::clear_order(bool cancelled) {
	robot_port_->stop_motion(++stop_seq_);
	for (uint8_t i = 0; i < cur_.nactions; ++i) {
		Action &a = cur_.actions[i];
		if (is_final(a.status)) continue;
		if (a.robot_side && a.status != ActionStatus::Waiting) robot_port_->cancel_action(a.uid);
		a.status = ActionStatus::Failed;
	}
	first_pending_node_ = cur_.nnodes;
	first_pending_edge_ = cur_.nedges;
	gen_nodes_ = cur_.nnodes;
	path_gen_ = path_sent_;
	path_done_ = path_sent_;
	replanning_ = false;
	set_hold_reason(0xFF, false);
	// requests belong to the order's movement
	for (Request &r : requests_) {
		if (r.used && !r.in_use) r.used = false;
	}
	cancelled_ = cancelled;
	mark_dirty();
}

void OrderManager::cancel_order(Action &c, const ActionParams &params) {
	const char *wanted = param(params, "orderId");
	if (idle() || !have_order_ || (wanted != nullptr && wanted[0] != '\0' && strcmp(wanted, cur_.order_id) != 0)) {
		c.status = ActionStatus::Failed;
		add_error("NO_ORDER_TO_CANCEL", ErrorLevel::Warning, UntilNewOrder, "no matching active order",
		          "actionId", c.id);
		return;
	}
	clear_order(true);
	c.status = ActionStatus::Running;      // until the robot stands still
	pending_cancel_uid_ = c.uid;
}

void OrderManager::process_instant(Action &ia, const ActionParams &pa) {
	switch (ia.kind) {
	case ActionKind::CancelOrder:
		cancel_order(ia, pa);
		break;
	case ActionKind::StartPause:
		robot_port_->set_paused(true);
		ia.status = robot_.paused ? ActionStatus::Finished : ActionStatus::Running;
		break;
	case ActionKind::StopPause:
		robot_port_->set_paused(false);
		ia.status = !robot_.paused ? ActionStatus::Finished : ActionStatus::Running;
		break;
	case ActionKind::StateRequest:
		ia.status = ActionStatus::Finished;
		break;
	case ActionKind::FactsheetRequest:
		factsheet_req_ = true;
		ia.status = ActionStatus::Finished;
		break;
	case ActionKind::InitializePosition: {
		float x = 0, y = 0, th = 0;
		const char *map = param(pa, "mapId");
		const char *ln = param(pa, "lastNodeId");
		if (!idle() || !param_float(pa, "x", x) || !param_float(pa, "y", y) || !param_float(pa, "theta", th) ||
		    (map != nullptr && !map_enabled(map))) {
			ia.status = ActionStatus::Failed;
			break;
		}
		robot_port_->set_pose(x, y, th);
		pose_pending_ = true;
		pose_x_ = x;
		pose_y_ = y;
		if (map != nullptr) copy_str(current_map_, map);
		copy_str(last_node_id_, ln != nullptr ? ln : "");
		ia.status = ActionStatus::Finished;
		break;
	}
	case ActionKind::ClearInstantActions: {
		ia.status = ActionStatus::Finished;
		uint8_t w = 0;
		for (uint8_t i = 0; i < ninstant_; ++i) {
			if (!is_final(instant_[i].status)) instant_[w++] = instant_[i];
		}
		ninstant_ = w;
		break;
	}
	case ActionKind::ClearZoneActions: {
		ia.status = ActionStatus::Finished;
		uint8_t w = 0;
		for (uint8_t i = 0; i < nzone_actions_; ++i) {
			if (!is_final(zone_actions_[i].status)) zone_actions_[w++] = zone_actions_[i];
		}
		nzone_actions_ = w;
		break;
	}
	case ActionKind::Trigger: {
		bool any = false;
		for (uint8_t i = 0; i < cur_.nactions; ++i) {
			Action &w = cur_.actions[i];
			if (w.kind == ActionKind::WaitForTrigger && w.status == ActionStatus::Running) {
				w.status = ActionStatus::Finished;
				any = true;
			}
		}
		for (uint8_t i = 0; i < nzone_actions_; ++i) {
			Action &w = zone_actions_[i];
			if (w.kind == ActionKind::WaitForTrigger && w.status == ActionStatus::Running) {
				w.status = ActionStatus::Finished;
				any = true;
			}
		}
		ia.status = any ? ActionStatus::Finished : ActionStatus::Failed;
		break;
	}
	case ActionKind::StartHibernation: {
		if (have_order_ && !idle()) clear_order(false);
		robot_port_->stop_motion(++stop_seq_);
		const char *wake = param(pa, "wakeUpTime");
		wake_unix_ms_ = wake != nullptr ? parse_iso8601(wake) : 0;
		ia.status = ActionStatus::Finished;
		hibernate_pending_ = true;        // after this state message goes out
		break;
	}
	case ActionKind::StopHibernation:
		hibernating_ = false;
		hibernate_pending_ = false;
		wake_unix_ms_ = 0;
		ia.status = ActionStatus::Finished;
		break;
	case ActionKind::Shutdown:
		if (!idle()) {
			ia.status = ActionStatus::Failed;   // requires the idle state
		} else {
			ia.status = ActionStatus::Finished;
			shutdown_pending_ = true;
		}
		break;
	case ActionKind::DownloadMap: {
		const char *id = param(pa, "mapId");
		const char *ver = param(pa, "mapVersion");
		const char *link = param(pa, "mapDownloadLink");
		if (id == nullptr || ver == nullptr || link == nullptr || download_uid_ != 0) {
			ia.status = ActionStatus::Failed;
			break;
		}
		for (const MapEntry &m : maps_) {
			if (m.used && strcmp(m.id, id) == 0 && strcmp(m.version, ver) == 0) {
				ia.status = ActionStatus::Failed;
				add_error("DUPLICATE_MAP", ErrorLevel::Warning, UntilNewInstantAction, "map already on the robot",
				          "mapId", id, "mapVersion", ver);
				return;
			}
		}
		memset(&download_, 0, sizeof(download_));
		copy_str(download_.id, id);
		copy_str(download_.version, ver);
		download_uid_ = ia.uid;
		download_done_ms_ = now_ms_ + cfg_.map_download_ms;   // simulated transfer
		ia.status = ActionStatus::Running;
		break;
	}
	case ActionKind::EnableMap: {
		const char *id = param(pa, "mapId");
		const char *ver = param(pa, "mapVersion");
		MapEntry *found = nullptr;
		for (MapEntry &m : maps_) {
			if (m.used && id != nullptr && ver != nullptr && strcmp(m.id, id) == 0 && strcmp(m.version, ver) == 0) found = &m;
		}
		if (found == nullptr) {
			ia.status = ActionStatus::Failed;
			break;
		}
		for (MapEntry &m : maps_) {
			if (m.used && strcmp(m.id, id) == 0) m.enabled = (&m == found);
		}
		ia.status = ActionStatus::Finished;
		break;
	}
	case ActionKind::DeleteMap: {
		const char *id = param(pa, "mapId");
		const char *ver = param(pa, "mapVersion");
		ia.status = ActionStatus::Failed;
		for (MapEntry &m : maps_) {
			if (m.used && id != nullptr && ver != nullptr && strcmp(m.id, id) == 0 && strcmp(m.version, ver) == 0) {
				const bool in_use = m.enabled && strcmp(m.id, current_map_) == 0 && !idle();
				if (!in_use) {
					m.used = false;
					ia.status = ActionStatus::Finished;
				}
			}
		}
		break;
	}
	case ActionKind::EnableZoneSet: {
		const char *id = param(pa, "zoneSetId");
		ZoneSet *found = nullptr;
		for (ZoneSet &zs : zone_sets_) {
			if (zs.used && id != nullptr && strcmp(zs.id, id) == 0) found = &zs;
		}
		if (found == nullptr) {
			ia.status = ActionStatus::Failed;
			break;
		}
		for (ZoneSet &zs : zone_sets_) {
			if (zs.used && strcmp(zs.map, found->map) == 0) zs.enabled = (&zs == found);
		}
		for (uint8_t z = 0; z < found->nzones; ++z) found->zones[z].inside = false;
		request_replan();      // queued waypoints may now cross a zone
		ia.status = ActionStatus::Finished;
		break;
	}
	case ActionKind::DeleteZoneSet: {
		const char *id = param(pa, "zoneSetId");
		ia.status = ActionStatus::Failed;
		for (uint8_t s = 0; s < MAX_ZONE_SETS; ++s) {
			ZoneSet &zs = zone_sets_[s];
			if (!zs.used || id == nullptr || strcmp(zs.id, id) != 0) continue;
			bool inside = false;
			for (uint8_t z = 0; z < zs.nzones; ++z) inside = inside || zs.zones[z].inside;
			if (zs.enabled && inside) break;   // in use
			for (Request &r : requests_) {
				if (r.used && r.type == ReqType::Access && r.zone_set == s) r.used = false;
			}
			zs.used = false;
			ia.status = ActionStatus::Finished;
		}
		break;
	}
	case ActionKind::SimObstacle: {
		float x = 0, y = 0, r = 0.2F;
		(void) param_float(pa, "radius", r);
		if (!param_float(pa, "x", x) || !param_float(pa, "y", y)) {
			ia.status = ActionStatus::Failed;
			break;
		}
		Obstacle *slot = nullptr;
		for (Obstacle &o : obstacles_) {
			if (!o.used) { slot = &o; break; }
		}
		if (slot == nullptr) {
			ia.status = ActionStatus::Failed;
			break;
		}
		*slot = Obstacle{ true, x, y, r };
		request_replan();
		ia.status = ActionStatus::Finished;
		break;
	}
	case ActionKind::SimClearObstacles:
		for (Obstacle &o : obstacles_) o.used = false;
		ia.status = ActionStatus::Finished;
		break;
	default:
		ia.status = ActionStatus::Failed;
		break;
	}
}

void OrderManager::on_instant_actions(const char *json_text, size_t len) {
	if (shutdown_) return;
	json::JsonReader r(json_text, len);
	if (!r.enter_object()) return;
	json::Span k;
	while (r.next_key(k)) {
		if (!(k == "actions")) {
			r.skip();
			continue;
		}
		if (!r.enter_array()) break;
		while (r.next_element()) {
			Action a;
			ActionParams params;
			ParseErr err;
			if (!parse_action(r, a, -1, true, err, &params)) break;
			// While hibernating only stopHibernation is answered (6.2.3)
			if (hibernating_ && a.kind != ActionKind::StopHibernation) continue;
			Action *slot = add_instant(a);
			if (slot == nullptr) continue;
			Action &ia = *slot;
			ia.uid = next_uid_++;
			if (next_uid_ == 0) next_uid_ = 1;
			mark_dirty();
			if (ia.blocking != Blocking::None || !is_instant_kind(ia.kind)) {
				ia.status = ActionStatus::Failed;
				add_error("INVALID_INSTANT_ACTION", ErrorLevel::Warning, UntilNewInstantAction,
				          "instant action not supported", "actionId", ia.id);
				continue;
			}
			clear_errors(UntilNewInstantAction);
			process_instant(ia, params);
		}
	}
}

} // namespace vda
