/*
 * motion.cpp — order geometry -> waypoints -> robot, see vda5050.hpp.
 *
 * Waypoints are generated per released edge (straight line or sampled NURBS)
 * up to the decision point or to a node where SOFT/HARD actions must run.
 * They are streamed STREAM_AHEAD ahead of the robot; before each one is sent
 * the segment leading to it is checked (robot contour as a disc of
 * ROBOT_RADIUS) against BLOCKED zones, RELEASE zones without a grant and
 * obstacles. Obstacles inside an edge corridor are avoided by shifting the
 * not-yet-sent waypoints sideways (after an edgeRequest if releaseRequired).
 */
#include <cmath>
#include <cstring>

#include "vda5050/internal.hpp"

namespace vda {

using namespace detail;

namespace {
constexpr float VIA_TOLERANCE = 0.05F;     // pass-through radius of plain waypoints [m]
constexpr float OBSTACLE_MARGIN = 0.1F;    // extra clearance around obstacles [m]
constexpr uint32_t LOOKAHEAD = 15;         // waypoints scanned for obstacles ahead
constexpr uint32_t RAMP_POINTS = 5;        // smooth in/out of a deviation
constexpr size_t MAX_EDGE_POINTS = 200;
} // namespace

const PathPoint *OrderManager::path_point(uint32_t index) const {
	if (index < path_done_ || index >= path_gen_) return nullptr;
	return &path_[index % PATH_CAP];
}

void OrderManager::set_hold_reason(uint8_t reason, bool on) {
	const uint8_t before = hold_mask_;
	hold_mask_ = on ? static_cast<uint8_t>(hold_mask_ | reason) : static_cast<uint8_t>(hold_mask_ & ~reason);
	if ((before != 0) != (hold_mask_ != 0)) {
		robot_port_->set_hold(hold_mask_ != 0);
		mark_dirty();
	}
}

// Something changed (zones enabled, obstacle appeared) that may invalidate
// waypoints already queued on the robot: stop, then re-stream from the first
// unreached waypoint with fresh checks.
void OrderManager::request_replan() {
	if (path_sent_ == path_done_ || replanning_) return;
	set_hold_reason(HoldReplan, true);
	robot_port_->stop_motion(++stop_seq_);
	replanning_ = true;
}

void OrderManager::generate_path() {
	geo::Vec2 pts[MAX_EDGE_POINTS];
	while (gen_nodes_ < cur_.nnodes) {
		const uint8_t i = gen_nodes_;
		const Node &n = cur_.nodes[i];
		const Edge &e = cur_.edges[i - 1];
		if (!n.released || !e.released) break;                 // decision point
		// The robot must stop at node i-1 while it (or edge i-1) has SOFT/HARD actions
		const uint32_t prev_seq = cur_.nodes[i - 1].seq;
		bool stop_before = false;
		for (uint8_t k = 0; k < cur_.nactions; ++k) {
			const Action &a = cur_.actions[k];
			if ((a.owner_seq == static_cast<int32_t>(prev_seq) || a.owner_seq == static_cast<int32_t>(prev_seq + 1U)) &&
			    is_blocking_drive(a.blocking) && !is_final(a.status)) {
				stop_before = true;
				break;
			}
		}
		if (stop_before) break;

		const geo::Vec2 start{ cur_.nodes[i - 1].x, cur_.nodes[i - 1].y };
		const geo::Vec2 end{ n.x, n.y };
		size_t count = 0;
		if (e.traj >= 0) {
			const geo::Nurbs &t = cur_.traj[e.traj];
			// keep the waypoint count bounded on long curves
			const float approx = geo::dist(start, end) * 1.6F + 0.1F;
			float spacing = PATH_SPACING;
			if (approx / spacing > static_cast<float>(MAX_EDGE_POINTS - 2)) spacing = approx / static_cast<float>(MAX_EDGE_POINTS - 2);
			const size_t m = geo::nurbs_sample(t, spacing, pts, MAX_EDGE_POINTS);
			// pts[0] is the curve start (= start node): skip it
			for (size_t j = 1; j < m; ++j) pts[count++] = pts[j];
			if (count == 0) pts[count++] = end;
			pts[count - 1] = end;   // finish exactly on the node
		} else {
			float spacing = PATH_SPACING;
			const float d = geo::dist(start, end);
			if (d / spacing > static_cast<float>(MAX_EDGE_POINTS)) spacing = d / static_cast<float>(MAX_EDGE_POINTS);
			count = geo::line_sample(start, end, spacing, pts, MAX_EDGE_POINTS);
		}
		if ((path_gen_ - path_done_) + count > PATH_CAP) break;     // wait for room

		for (size_t j = 0; j < count; ++j) {
			PathPoint &p = pp(path_gen_);
			memset(&p, 0, sizeof(p));
			p.x = p.ox = pts[j].x;
			p.y = p.oy = pts[j].y;
			p.vmax = e.max_speed;
			p.tol = VIA_TOLERANCE;
			p.edge = static_cast<uint8_t>(i - 1);
			if (j + 1 == count) {
				p.flags = PP_NODE;
				p.node_seq = n.seq;
				const float dev = n.dev_xy > 0.0F ? n.dev_xy : cfg_.default_dev_xy;
				p.tol = dev < PATH_SPACING ? dev : PATH_SPACING;   // keep zone checks valid
				if (n.has_theta) {
					p.flags |= PP_THETA;
					p.theta = n.theta;
				}
			}
			++path_gen_;
		}
		++gen_nodes_;
	}
}

bool OrderManager::segment_clear(uint32_t index) {
	const PathPoint &b = pp(index);
	geo::Vec2 a{ robot_.x, robot_.y };
	if (index > path_base_ && index > path_done_) {
		const PathPoint &prev = pp(index - 1U);
		a = geo::Vec2{ prev.x, prev.y };
	}
	const geo::Vec2 bb{ b.x, b.y };

	// next node of this waypoint (for NODE_UNREACHABLE)
	const char *node_id = "";
	const uint8_t node_idx = static_cast<uint8_t>(b.edge + 1U);
	if (node_idx < cur_.nnodes) node_id = cur_.nodes[node_idx].id;

	for (uint8_t s = 0; s < MAX_ZONE_SETS; ++s) {
		const ZoneSet &zs = zone_sets_[s];
		if (!zs.used || !zs.enabled || strcmp(zs.map, current_map_) != 0) continue;
		for (uint8_t z = 0; z < zs.nzones; ++z) {
			const Zone &zone = zs.zones[z];
			if (zone.type != ZoneType::Blocked && zone.type != ZoneType::Release) continue;
			if (!geo::capsule_touches_polygon(a, bb, ROBOT_RADIUS, zone.v, zone.nverts)) continue;
			if (zone.type == ZoneType::Blocked) {
				if (zone.inside) continue;   // already a violation, handled by process_zones
				if (!blocked_reported_) {
					add_error("NODE_UNREACHABLE", ErrorLevel::Critical, UntilNewOrder,
					          "path crosses a BLOCKED zone", "nodeId", node_id, "zoneId", zone.id);
					blocked_reported_ = true;
				}
				return false;
			}
			// RELEASE: request access, wait for the grant
			if (!zone_granted(s, z)) {
				(void) need_request(ReqType::Access, s, z, 0, 0);
				return false;
			}
		}
	}

	for (const Obstacle &o : obstacles_) {
		if (!o.used) continue;
		if (geo::dist_point_segment(geo::Vec2{ o.x, o.y }, a, bb) < o.r + ROBOT_RADIUS) {
			if (!waiting_obstacle_) {
				waiting_obstacle_ = true;
				mark_dirty();
			}
			return false;
		}
	}
	if (waiting_obstacle_) {
		waiting_obstacle_ = false;
		mark_dirty();
	}
	return true;
}

// Shift waypoints [from, c2 + RAMP] sideways so that the robot passes the
// obstacle inside the corridor. Only unsent waypoints are touched.
bool OrderManager::plan_avoidance(uint32_t from, uint32_t c1, uint32_t c2, const Obstacle &ob,
		const Corridor &c, uint32_t &end_index) {
	const float clearance = ob.r + ROBOT_RADIUS + OBSTACLE_MARGIN;
	// closest original waypoint to the obstacle and the path direction there
	uint32_t best = c1;
	float best_d = 1e30F;
	for (uint32_t j = c1; j <= c2; ++j) {
		const PathPoint &p = pp(j);
		const float d = geo::dist(geo::Vec2{ p.ox, p.oy }, geo::Vec2{ ob.x, ob.y });
		if (d < best_d) { best_d = d; best = j; }
	}
	const PathPoint &pa = pp(best > path_base_ ? best - 1U : best);
	const PathPoint &pb = pp(best + 1U < path_gen_ ? best + 1U : best);
	float tx = pb.ox - pa.ox, ty = pb.oy - pa.oy;
	const float tl = std::sqrt(tx * tx + ty * ty);
	if (tl < 1e-4F) return false;
	tx /= tl;
	ty /= tl;
	// signed lateral distance of the obstacle (left of travel = positive)
	const PathPoint &pc = pp(best);
	const float lat = -ty * (ob.x - pc.ox) + tx * (ob.y - pc.oy);
	const float reserve = c.contour_ref ? ROBOT_RADIUS : 0.0F;
	// Moving right by m puts the obstacle at lat + m (needs >= clearance),
	// moving left by m puts it at lat - m (needs <= -clearance). Take the
	// smaller shift that fits the corridor.
	const float right_need = clearance - lat;
	const float left_need = clearance + lat;
	const bool right_ok = right_need <= c.right - reserve;
	const bool left_ok = left_need <= c.left - reserve;
	float offset = 0.0F;                           // + = left
	if (right_ok && (!left_ok || right_need <= left_need)) offset = -right_need;
	else if (left_ok) offset = left_need;
	else return false;                             // corridor too narrow

	const uint32_t ramp_in = (c1 > from + RAMP_POINTS) ? RAMP_POINTS : (c1 - from);
	const uint32_t start = c1 - ramp_in;
	uint32_t stop = c2 + RAMP_POINTS;
	if (stop >= path_gen_) stop = path_gen_ - 1U;
	for (uint32_t j = start; j <= stop; ++j) {
		PathPoint &p = pp(j);
		if (p.flags & PP_NODE) {
			// never shift a node position: keep the deviation within the edge
			if (j > c2) { stop = j - 1U; break; }
		}
		float w = 1.0F;
		if (j < c1 && ramp_in > 0) w = static_cast<float>(j - start + 1U) / static_cast<float>(ramp_in + 1U);
		if (j > c2) w = 1.0F - static_cast<float>(j - c2) / static_cast<float>(RAMP_POINTS + 1U);
		w = 0.5F - 0.5F * std::cos(w * 3.14159265F);   // smooth
		// local normal of the original path
		const PathPoint &qa = pp(j > path_base_ ? j - 1U : j);
		const PathPoint &qb = pp(j + 1U < path_gen_ ? j + 1U : j);
		float nx = -(qb.oy - qa.oy), ny = qb.ox - qa.ox;
		const float nl = std::sqrt(nx * nx + ny * ny);
		if (nl > 1e-4F) { nx /= nl; ny /= nl; } else { nx = -ty; ny = tx; }
		p.x = p.ox + nx * offset * w;
		p.y = p.oy + ny * offset * w;
		p.flags |= PP_OFFSET;
	}
	end_index = stop;
	return true;
}

// Plans a corridor deviation for an obstacle ahead. While no deviation is
// possible (no corridor, too narrow, release not granted yet) streaming stops
// RAMP_POINTS before the conflict, so that there is room to swerve smoothly
// once it becomes possible.
uint32_t OrderManager::plan_obstacles_ahead() {
	constexpr uint32_t NO_LIMIT = 0xFFFFFFFFU;
	const uint32_t limit = (path_sent_ + LOOKAHEAD < path_gen_) ? path_sent_ + LOOKAHEAD : path_gen_;
	for (uint32_t j = path_sent_; j < limit; ++j) {
		const PathPoint &p = pp(j);
		if (p.flags & PP_OFFSET) continue;
		for (const Obstacle &o : obstacles_) {
			if (!o.used) continue;
			const float clearance = o.r + ROBOT_RADIUS + OBSTACLE_MARGIN;
			if (geo::dist(geo::Vec2{ p.ox, p.oy }, geo::Vec2{ o.x, o.y }) >= clearance) continue;
			// conflict starting at j; find where it ends
			uint32_t c2 = j;
			while (c2 + 1U < path_gen_) {
				const PathPoint &q = pp(c2 + 1U);
				if (geo::dist(geo::Vec2{ q.ox, q.oy }, geo::Vec2{ o.x, o.y }) >= clearance) break;
				++c2;
			}
			const uint32_t wait_at = (j > path_sent_ + RAMP_POINTS) ? j - RAMP_POINTS : path_sent_;
			const Edge &e = cur_.edges[p.edge];
			if (!e.corridor.present) return wait_at;   // line-guided: wait in front of it
			Request *req = nullptr;
			if (e.corridor.release_required) {
				req = need_request(ReqType::Corridor, 0, 0, p.edge, e.seq);
				if (req == nullptr || req->status != ReqStatus::Granted) return wait_at;   // wait for the grant
			}
			uint32_t end_index = 0;
			if (!plan_avoidance(path_sent_, j, c2, o, e.corridor, end_index)) return wait_at;   // too narrow
			if (req != nullptr) {
				req->in_use = true;
				req->until_index = end_index;
			}
			mark_dirty();
			return NO_LIMIT;
		}
	}
	return NO_LIMIT;
}

void OrderManager::stream_path() {
	if (replanning_) return;
	while (path_sent_ < path_gen_ && (path_sent_ - path_done_) < STREAM_AHEAD) {
		if (path_sent_ >= plan_obstacles_ahead()) {
			if (!waiting_obstacle_) {
				waiting_obstacle_ = true;
				mark_dirty();
			}
			break;
		}
		if (!segment_clear(path_sent_)) break;
		const PathPoint &p = pp(path_sent_);
		DriveTarget t{};
		t.path_index = path_sent_;
		t.node_seq = p.node_seq;
		t.x = p.x;
		t.y = p.y;
		t.theta = p.theta;
		t.is_node = (p.flags & PP_NODE) != 0;
		t.has_theta = (p.flags & PP_THETA) != 0;
		t.max_speed = p.vmax;
		t.tolerance = p.tol;
		if (!robot_port_->drive_to(t)) break;
		++path_sent_;
	}
}

void OrderManager::process_motion() {
	if (replanning_ && robot_.stop_ack == stop_seq_) {
		// the robot dropped its queue: continue from the first unreached waypoint
		path_sent_ = path_done_;
		replanning_ = false;
		set_hold_reason(HoldReplan, false);
	}
	if (pending_cancel_uid_ != 0 || !have_order_ || cancelled_) return;
	// corridor deviations end once the robot is past them
	for (Request &r : requests_) {
		if (r.used && r.type == ReqType::Corridor && r.in_use && path_done_ > r.until_index) {
			r.used = false;
			set_hold_reason(HoldCorridorLost, false);
			mark_dirty();
		}
	}
	generate_path();
	stream_path();
}

} // namespace vda
