/*
 * zones.cpp — VDA 5050 3.0.0 zones (6.4), request/response (6.9), zone
 * actions. See vda5050.hpp.
 */
#include <cmath>
#include <cstdio>
#include <cstring>

#include "vda5050/internal.hpp"

namespace vda {

using namespace detail;

namespace {
constexpr uint32_t REREQUEST_MS = 10000;   // after REJECTED
constexpr float BRAKE_DECEL = 0.5F;        // m/s^2, for the speed-limit look-ahead

bool parse_zone_type(const char *s, ZoneType &t) {
	struct E { const char *n; ZoneType t; };
	static const E table[] = {
		{ "BLOCKED", ZoneType::Blocked }, { "LINE_GUIDED", ZoneType::LineGuided },
		{ "RELEASE", ZoneType::Release }, { "COORDINATED_REPLANNING", ZoneType::CoordinatedReplanning },
		{ "SPEED_LIMIT", ZoneType::SpeedLimit }, { "ACTION", ZoneType::Action },
		{ "PRIORITY", ZoneType::Priority }, { "PENALTY", ZoneType::Penalty },
		{ "DIRECTED", ZoneType::Directed }, { "BIDIRECTED", ZoneType::Bidirected },
	};
	for (const E &e : table) {
		if (strcmp(s, e.n) == 0) { t = e.t; return true; }
	}
	return false;
}

// One zone object; actions go into the set's pool. Returns false on a
// structural error (zone set rejected).
bool parse_zone(json::JsonReader &r, ZoneSet &zs, const char *&why) {
	if (zs.nzones >= MAX_ZONES) { why = "too many zones"; r.skip(); return false; }
	Zone &z = zs.zones[zs.nzones];
	memset(&z, 0, sizeof(z));
	bool have_id = false, have_type = false, have_speed = false;
	uint8_t action_lists = 0;
	if (!r.enter_object()) return false;
	json::Span k;
	while (r.next_key(k)) {
		if (k == "zoneId") {
			have_id = r.read_string(z.id, sizeof(z.id));
		} else if (k == "zoneType") {
			char t[32];
			have_type = r.read_string(t, sizeof(t)) && parse_zone_type(t, z.type);
		} else if (k == "vertices") {
			if (!r.enter_array()) return false;
			while (r.next_element()) {
				if (z.nverts >= MAX_VERTICES) { why = "too many vertices"; r.skip(); continue; }
				double x = 0, y = 0;
				bool hx = false, hy = false;
				if (!r.enter_object()) return false;
				json::Span vk;
				while (r.next_key(vk)) {
					if (vk == "x") hx = r.read_number(x);
					else if (vk == "y") hy = r.read_number(y);
					else r.skip();
				}
				if (!(hx && hy)) return false;
				z.v[z.nverts++] = geo::Vec2{ static_cast<float>(x), static_cast<float>(y) };
			}
		} else if (k == "maximumSpeed") {
			double v = 0;
			have_speed = r.read_number(v);
			z.max_speed = static_cast<float>(v);
		} else if (k == "releaseLossBehavior") {
			char s[16];
			if (r.read_string(s, sizeof(s))) {
				z.loss = strcmp(s, "CONTINUE") == 0 ? LossBehavior::Continue
				       : strcmp(s, "EVACUATE") == 0 ? LossBehavior::Evacuate : LossBehavior::Stop;
			}
		} else if (k == "entryActions" || k == "duringActions" || k == "exitActions") {
			const uint8_t phase = k == "entryActions" ? 0 : (k == "duringActions" ? 1 : 2);
			++action_lists;
			if (!r.enter_array()) return false;
			while (r.next_element()) {
				Action a;
				ParseErr err;
				if (!parse_action(r, a, -2, false, err)) return false;
				if (!allowed_in_zone(a.kind)) { why = "zone action not supported"; continue; }
				if (zs.nacts >= MAX_ZONE_ACTION_DEFS) { why = "too many zone actions"; continue; }
				ZoneActionDef &d = zs.acts[zs.nacts++];
				copy_str(d.type, a.type);
				d.kind = a.kind;
				d.blocking = a.blocking;
				d.phase = phase;
				d.zone = zs.nzones;
			}
		} else {
			r.skip();   // descriptor, priority/penalty factors, direction, limitations
		}
		if (!r.ok()) return false;
	}
	if (!have_id || !have_type || z.nverts < 3) { if (!why) why = "zone needs id, type, 3+ vertices"; return false; }
	if (z.type == ZoneType::SpeedLimit && (!have_speed || z.max_speed <= 0.0F)) { why = "SPEED_LIMIT needs maximumSpeed"; return false; }
	if (z.type == ZoneType::Action && action_lists != 3) { why = "ACTION zone needs entry/during/exit actions"; return false; }
	for (uint8_t i = 0; i < zs.nzones; ++i) {
		if (strcmp(zs.zones[i].id, z.id) == 0) { why = "duplicate zoneId"; return false; }
	}
	++zs.nzones;
	return true;
}
} // namespace

// =============================================================================
// zoneSet topic
// =============================================================================
void OrderManager::on_zone_set(const char *json_text, size_t len) {
	if (hibernating_ || shutdown_) return;
	ZoneSet *slot = nullptr;
	for (ZoneSet &zs : zone_sets_) {
		if (!zs.used) { slot = &zs; break; }
	}
	ZoneSet scratch;
	ZoneSet &zs = slot != nullptr ? *slot : scratch;
	memset(&zs, 0, sizeof(zs));
	const char *why = nullptr;
	bool ok = true, have_set = false;

	json::JsonReader r(json_text, len);
	if (!r.enter_object()) return;
	json::Span k;
	while (r.next_key(k)) {
		if (!(k == "zoneSet")) { r.skip(); continue; }
		have_set = true;
		if (!r.enter_object()) break;
		json::Span sk;
		while (r.next_key(sk)) {
			if (sk == "mapId") ok = r.read_string(zs.map, sizeof(zs.map)) && ok;
			else if (sk == "zoneSetId") ok = r.read_string(zs.id, sizeof(zs.id)) && ok;
			else if (sk == "zones") {
				if (!r.enter_array()) break;
				while (r.next_element()) {
					if (!parse_zone(r, zs, why)) ok = false;
					if (!r.ok()) break;
				}
			} else {
				r.skip();
			}
			if (!r.ok()) break;
		}
		if (!r.ok()) break;
	}
	mark_dirty();
	if (!r.ok() || !have_set || !ok || zs.id[0] == '\0' || zs.map[0] == '\0') {
		add_error("VALIDATION_FAILURE", ErrorLevel::Warning, AfterTimeout, why != nullptr ? why : "malformed zone set",
		          "topic", "zoneSet", zs.id[0] != '\0' ? "zoneSetId" : nullptr, zs.id);
		zs.used = false;
		return;
	}
	for (const ZoneSet &o : zone_sets_) {
		if (&o != &zs && o.used && strcmp(o.id, zs.id) == 0) {
			add_error("DUPLICATE_ZONE_SET", ErrorLevel::Warning, AfterTimeout, "zone set already on the robot",
			          "zoneSetId", zs.id);
			zs.used = false;
			return;
		}
	}
	if (slot == nullptr) {
		add_error("INSUFFICIENT_MEMORY", ErrorLevel::Urgent, AfterTimeout, "no room for another zone set",
		          "zoneSetId", zs.id);
		return;
	}
	zs.used = true;
	zs.enabled = false;   // 6.4.2: new zone sets start DISABLED
}

// =============================================================================
// requests / responses
// =============================================================================
Request *OrderManager::find_request(ReqType t, uint8_t zs, uint8_t zone, uint32_t edge_seq) {
	for (Request &r : requests_) {
		if (!r.used || r.type != t) continue;
		if (t == ReqType::Access && r.zone_set == zs && r.zone == zone) return &r;
		if (t == ReqType::Corridor && r.edge_seq == edge_seq) return &r;
	}
	return nullptr;
}

// The request the robot needs now for a zone/edge, creating (or renewing)
// one when there is none usable.
Request *OrderManager::need_request(ReqType t, uint8_t zs, uint8_t zone, uint8_t edge, uint32_t edge_seq) {
	Request *r = find_request(t, zs, zone, edge_seq);
	if (r != nullptr) {
		if (r->rejected && (now_ms_ - r->rejected_ms) < REREQUEST_MS) return r;   // cool down
		const bool stale = r->rejected || ((r->status == ReqStatus::Expired || r->status == ReqStatus::Revoked) && !r->in_use);
		if (!stale) return r;
		r->used = false;   // replace it with a fresh request
	}
	Request *slot = nullptr;
	for (Request &q : requests_) {
		if (!q.used) { slot = &q; break; }
	}
	if (slot == nullptr) return nullptr;
	memset(slot, 0, sizeof(*slot));
	slot->used = true;
	slot->type = t;
	slot->status = ReqStatus::Requested;
	slot->zone_set = zs;
	slot->zone = zone;
	slot->edge = edge;
	slot->edge_seq = edge_seq;
	snprintf(slot->id, sizeof(slot->id), "req-%lu", static_cast<unsigned long>(++request_counter_));
	mark_dirty();
	return slot;
}

bool OrderManager::zone_granted(uint8_t zs, uint8_t z) {
	const Request *r = find_request(ReqType::Access, zs, z, 0);
	return r != nullptr && !r->rejected && r->status == ReqStatus::Granted &&
	       (r->lease_ms == 0 || unix_ms_ == 0 || unix_ms_ < r->lease_ms);
}

void OrderManager::release_lost(Request &r) {
	if (r.type == ReqType::Access) {
		const Zone &z = zone_sets_[r.zone_set].zones[r.zone];
		if (z.loss == LossBehavior::Stop) {
			set_hold_reason(HoldReleaseLost, true);
			add_error("RELEASE_LOST", ErrorLevel::Critical, UntilResolved, "release of the zone lost",
			          "zoneId", z.id);
		}
		// CONTINUE / EVACUATE: keep driving (the order leads out of the zone),
		// keep the request until the zone is left
	} else {
		const Edge *e = nullptr;
		for (uint8_t i = 0; i < cur_.nedges; ++i) {
			if (cur_.edges[i].seq == r.edge_seq) e = &cur_.edges[i];
		}
		if (e != nullptr && e->corridor.return_on_loss) {
			// RETURN: drop the deviation; the robot goes back to the trajectory
			// and waits in front of the obstacle
			for (uint32_t j = path_done_; j < path_gen_; ++j) {
				PathPoint &p = pp(j);
				if (p.flags & PP_OFFSET) {
					p.x = p.ox;
					p.y = p.oy;
					p.flags = static_cast<uint8_t>(p.flags & ~PP_OFFSET);
				}
			}
			r.in_use = false;
			request_replan();
		} else {
			set_hold_reason(HoldCorridorLost, true);   // STOP: await intervention
		}
	}
	mark_dirty();
}

void OrderManager::on_responses(const char *json_text, size_t len) {
	if (hibernating_ || shutdown_) return;
	json::JsonReader r(json_text, len);
	if (!r.enter_object()) return;
	json::Span k;
	while (r.next_key(k)) {
		if (!(k == "responses")) { r.skip(); continue; }
		if (!r.enter_array()) break;
		while (r.next_element()) {
			char id[16] = "", grant[16] = "", lease[32] = "";
			if (!r.enter_object()) return;
			json::Span rk;
			while (r.next_key(rk)) {
				if (rk == "requestId") r.read_string(id, sizeof(id));
				else if (rk == "grantType") r.read_string(grant, sizeof(grant));
				else if (rk == "leaseExpiry") r.read_string(lease, sizeof(lease));
				else r.skip();
			}
			if (!r.ok()) return;
			for (Request &q : requests_) {
				if (!q.used || strcmp(q.id, id) != 0) continue;
				mark_dirty();
				if (strcmp(grant, "GRANTED") == 0) {
					q.status = ReqStatus::Granted;
					q.queued = false;
					q.rejected = false;
					q.lease_ms = lease[0] != '\0' ? parse_iso8601(lease) : 0;
					if (q.type == ReqType::Access) {
						set_hold_reason(HoldReleaseLost, false);
						remove_error("RELEASE_LOST");
					} else {
						set_hold_reason(HoldCorridorLost, false);
					}
				} else if (strcmp(grant, "QUEUED") == 0) {
					q.queued = true;
				} else if (strcmp(grant, "REJECTED") == 0) {
					q.rejected = true;
					q.rejected_ms = now_ms_;
				} else if (strcmp(grant, "REVOKED") == 0) {
					if (q.in_use) {
						q.status = ReqStatus::Revoked;
						release_lost(q);
					} else {
						q.used = false;
					}
				}
			}
		}
	}
}

void OrderManager::process_requests() {
	if (unix_ms_ == 0) return;
	for (Request &q : requests_) {
		if (!q.used || q.status != ReqStatus::Granted || q.lease_ms == 0 || unix_ms_ < q.lease_ms) continue;
		q.status = ReqStatus::Expired;
		mark_dirty();
		if (q.in_use) release_lost(q);
	}
}

// =============================================================================
// zones at the robot's position
// =============================================================================
void OrderManager::zone_entered(uint8_t s, uint8_t zi) {
	ZoneSet &zs = zone_sets_[s];
	const Zone &z = zs.zones[zi];
	if (z.type == ZoneType::Release) {
		Request *r = find_request(ReqType::Access, s, zi, 0);
		if (r != nullptr) r->in_use = true;
	}
	if (z.type != ZoneType::Action) return;
	for (uint8_t i = 0; i < zs.nacts; ++i) {
		const ZoneActionDef &d = zs.acts[i];
		if (d.zone != zi || d.phase == 2) continue;
		if (nzone_actions_ >= MAX_ZONE_ACTIONS) {
			// drop the oldest finished entry
			for (uint8_t j = 0; j < nzone_actions_; ++j) {
				if (is_final(zone_actions_[j].status)) {
					for (uint8_t m = j + 1; m < nzone_actions_; ++m) zone_actions_[m - 1] = zone_actions_[m];
					--nzone_actions_;
					break;
				}
			}
			if (nzone_actions_ >= MAX_ZONE_ACTIONS) return;
		}
		Action &a = zone_actions_[nzone_actions_++];
		memset(&a, 0, sizeof(a));
		char zid[sizeof(z.id)];
		copy_str(zid, z.id);   // a.id and z.id both live in *this
		snprintf(a.id, sizeof(a.id), "%s-%s-%lu", zid, d.phase == 0 ? "entry" : "during",
		         static_cast<unsigned long>(++zone_action_counter_));
		copy_str(a.type, d.type);
		a.kind = d.kind;
		a.blocking = d.blocking;
		a.status = ActionStatus::Waiting;
		a.owner_seq = -2 - static_cast<int32_t>(s * MAX_ZONES + zi) * 4 - d.phase;   // zone + phase tag
		a.uid = next_uid_++;
		if (next_uid_ == 0) next_uid_ = 1;
		a.robot_side = false;
	}
	mark_dirty();
}

void OrderManager::zone_left(uint8_t s, uint8_t zi) {
	ZoneSet &zs = zone_sets_[s];
	const Zone &z = zs.zones[zi];
	if (z.type == ZoneType::Release) {
		Request *r = find_request(ReqType::Access, s, zi, 0);
		if (r != nullptr) r->used = false;          // 6.4.3: remove after leaving
		set_hold_reason(HoldReleaseLost, false);
		remove_error("RELEASE_LOST");
	}
	if (z.type != ZoneType::Action) return;
	const int32_t tag_base = -2 - static_cast<int32_t>(s * MAX_ZONES + zi) * 4;
	for (uint8_t i = 0; i < nzone_actions_; ++i) {
		Action &a = zone_actions_[i];
		if (a.owner_seq == tag_base - 1 && !is_final(a.status)) a.status = ActionStatus::Finished;   // during
	}
	for (uint8_t i = 0; i < zs.nacts; ++i) {
		const ZoneActionDef &d = zs.acts[i];
		if (d.zone != zi || d.phase != 2 || nzone_actions_ >= MAX_ZONE_ACTIONS) continue;
		Action &a = zone_actions_[nzone_actions_++];
		memset(&a, 0, sizeof(a));
		char zid[sizeof(z.id)];
		copy_str(zid, z.id);
		snprintf(a.id, sizeof(a.id), "%s-exit-%lu", zid, static_cast<unsigned long>(++zone_action_counter_));
		copy_str(a.type, d.type);
		a.kind = d.kind;
		a.blocking = d.blocking;
		a.status = ActionStatus::Waiting;
		a.owner_seq = tag_base - 2;
		a.uid = next_uid_++;
		if (next_uid_ == 0) next_uid_ = 1;
	}
	mark_dirty();
}

void OrderManager::process_zones() {
	const geo::Vec2 p{ robot_.x, robot_.y };
	const float speed = std::fabs(robot_.vx);
	const float look = ROBOT_RADIUS + speed * speed / (2.0F * BRAKE_DECEL) + 0.1F;
	bool violation = false;
	const char *violated = "";
	float limit = 0.0F;
	for (uint8_t s = 0; s < MAX_ZONE_SETS; ++s) {
		ZoneSet &zs = zone_sets_[s];
		if (!zs.used || !zs.enabled || strcmp(zs.map, current_map_) != 0) continue;
		for (uint8_t zi = 0; zi < zs.nzones; ++zi) {
			Zone &z = zs.zones[zi];
			const bool touch = robot_.valid && geo::disc_touches_polygon(p, ROBOT_RADIUS, z.v, z.nverts);
			if (touch != z.inside) {
				z.inside = touch;
				if (touch) zone_entered(s, zi); else zone_left(s, zi);
			}
			if (z.type == ZoneType::Blocked && touch) {
				violation = true;
				violated = z.id;
			}
			// SPEED_LIMIT must already hold when the contour enters the zone
			if (z.type == ZoneType::SpeedLimit && geo::disc_touches_polygon(p, look, z.v, z.nverts)) {
				if (limit == 0.0F || z.max_speed < limit) limit = z.max_speed;
			}
		}
	}
	if (violation) {
		if (!has_error("BLOCKED_ZONE_VIOLATION")) {
			add_error("BLOCKED_ZONE_VIOLATION", ErrorLevel::Critical, UntilResolved, "inside a BLOCKED zone",
			          "zoneId", violated);
		}
		set_hold_reason(HoldZoneViolation, true);
	} else if (hold_mask_ & HoldZoneViolation) {
		remove_error("BLOCKED_ZONE_VIOLATION");
		set_hold_reason(HoldZoneViolation, false);
	}
	if (limit != speed_limit_) {
		speed_limit_ = limit;
		robot_port_->set_speed_limit(limit);
		mark_dirty();
	}
}

// Zone actions follow the same rules as order actions (Figure 11); SOFT/HARD
// stop automatic driving until they are done.
void OrderManager::process_zone_actions() {
	bool any_running = false;
	bool blocking = false;
	for (uint8_t i = 0; i < nzone_actions_; ++i) {
		Action &a = zone_actions_[i];
		if (is_final(a.status)) continue;
		if (is_blocking_drive(a.blocking)) blocking = true;
		if (a.status == ActionStatus::Waiting) {
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
	// re-evaluate after starting (instantly finished actions do not block)
	blocking = false;
	for (uint8_t i = 0; i < nzone_actions_; ++i) {
		if (!is_final(zone_actions_[i].status) && is_blocking_drive(zone_actions_[i].blocking)) blocking = true;
	}
	set_hold_reason(HoldZoneAction, blocking);
}

} // namespace vda
