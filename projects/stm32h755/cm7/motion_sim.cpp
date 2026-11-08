/*
 * motion_sim.cpp — see motion_sim.hpp.
 */
#include <cmath>

#include "motion_sim.hpp"

namespace {
constexpr float PI = 3.14159265F;

float wrap(float a) {
	while (a > PI) a -= 2.0F * PI;
	while (a < -PI) a += 2.0F * PI;
	return a;
}

float clampf(float v, float lo, float hi) {
	return v < lo ? lo : (v > hi ? hi : v);
}
} // namespace

void MotionSim::init(ReachedFn reached, ActionDoneFn done, void *ctx) {
	reached_ = reached;
	done_ = done;
	ctx_ = ctx;
}

void MotionSim::set_pose(float x, float y, float theta) {
	x_ = x;
	y_ = y;
	th_ = wrap(theta);
}

bool MotionSim::push(const MotionTarget &t) {
	if (count_ >= QUEUE) {
		return false;
	}
	q_[(head_ + count_) % QUEUE] = t;
	++count_;
	return true;
}

void MotionSim::pop() {
	const MotionTarget &t = q_[head_];
	last_popped_ = t.path_index;
	any_popped_ = true;
	const bool node = t.is_node;
	const uint32_t seq = t.node_seq;
	head_ = static_cast<uint8_t>((head_ + 1U) % QUEUE);
	--count_;
	final_turn_ = false;
	if (node && reached_) reached_(seq, ctx_);
}

void MotionSim::stop(uint32_t ack_seq) {
	count_ = 0;
	final_turn_ = false;
	v_ = 0;
	w_ = 0;
	stop_ack_ = ack_seq;
}

bool MotionSim::start_action(uint8_t uid, uint8_t code, uint32_t duration_ms) {
	for (Act &a : acts_) {
		if (!a.used) {
			a = { uid, code, duration_ms, true };
			return true;
		}
	}
	return false;
}

void MotionSim::cancel_action(uint8_t uid) {
	for (Act &a : acts_) {
		if (a.used && a.uid == uid) {
			a.used = false;
		}
	}
}

void MotionSim::step_actions(float dt) {
	if (paused_) {
		return;   // pausable actions hold their progress
	}
	act_ms_acc_ += dt * 1000.0F;
	const uint32_t ms = static_cast<uint32_t>(act_ms_acc_);
	act_ms_acc_ -= static_cast<float>(ms);
	for (Act &a : acts_) {
		if (!a.used) continue;
		a.remaining_ms = a.remaining_ms > ms ? a.remaining_ms - ms : 0;
		if (a.remaining_ms == 0) {
			a.used = false;
			loaded_ = (a.code == 1);   // pick loads, drop unloads
			if (done_) done_(a.uid, true, ctx_);
		}
	}
}

void MotionSim::step(float dt) {
	step_actions(dt);
	if (paused_ || held_ || count_ == 0) {
		v_ = 0;
		w_ = 0;
		return;
	}
	const MotionTarget &t = q_[head_];
	const bool stop_here = (count_ == 1) || t.has_theta;
	const float dx = t.x - x_;
	const float dy = t.y - y_;
	const float dist = std::sqrt(dx * dx + dy * dy);
	const float reach = stop_here ? STOP_DIST : (t.tolerance > STOP_DIST ? t.tolerance : STOP_DIST);

	if (final_turn_ || dist <= reach) {
		if (stop_here && t.has_theta) {
			const float err = wrap(t.theta - th_);
			if (std::fabs(err) > HEADING_TOL) {
				final_turn_ = true;
				v_ = 0;
				w_ = clampf(err * 4.0F, -MAX_OMEGA, MAX_OMEGA);
				th_ = wrap(th_ + w_ * dt);
				return;
			}
		}
		pop();
		if (count_ == 0) {
			v_ = 0;
			w_ = 0;
		}
		return;
	}

	// Turn towards the waypoint, drive when roughly aligned
	const float heading = std::atan2(dy, dx);
	const float err = wrap(heading - th_);
	w_ = clampf(err * 4.0F, -MAX_OMEGA, MAX_OMEGA);
	float vmax = DEFAULT_SPEED;
	if (t.max_speed > 0.0F && t.max_speed < vmax) vmax = t.max_speed;
	if (speed_cap_ > 0.0F && speed_cap_ < vmax) vmax = speed_cap_;
	float v_target = std::fabs(err) < 0.3F ? vmax : 0.0F;
	if (stop_here) {
		const float vbrake = std::sqrt(2.0F * ACCEL * dist);   // stop on the waypoint
		if (v_target > vbrake) v_target = vbrake;
	}
	if (v_target > v_) v_ = (v_ + ACCEL * dt < v_target) ? v_ + ACCEL * dt : v_target;
	else v_ = v_target;
	if (v_ * dt > dist) {
		v_ = dist / dt;   // do not overshoot
	}
	th_ = wrap(th_ + w_ * dt);
	x_ += v_ * std::cos(th_) * dt;
	y_ += v_ * std::sin(th_) * dt;
}
