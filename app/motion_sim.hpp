#pragma once
/*
 * motion_sim — kinematic stand-in for the vehicle on the real-time core.
 *
 * The board has no drives yet, so the Cortex-M7 runs this model in its 1 ms
 * control loop: a differential-drive robot that follows the waypoint stream
 * of the network core. Plain waypoints are passed within their tolerance;
 * the robot brakes to a stop on the last queued waypoint and on waypoints
 * that carry an orientation (where it turns on the spot). pick/drop run as
 * timers. Replace step() with real motor control later; the command/event
 * interface to the M4 stays the same.
 */
#include <cstdint>

struct MotionTarget {
	uint32_t path_index;
	uint32_t node_seq;
	float x, y, theta;
	bool is_node;        // report NodeReached when passed
	bool has_theta;      // stop here and turn to theta
	float max_speed;     // 0 = default
	float tolerance;     // pass-through radius [m]
};

class MotionSim {
public:
	using ReachedFn = void (*)(uint32_t node_seq, void *ctx);
	using ActionDoneFn = void (*)(uint8_t uid, bool ok, void *ctx);

	static constexpr uint8_t QUEUE = 32;
	static constexpr uint8_t ACTIONS = 4;
	static constexpr float DEFAULT_SPEED = 0.5F;      // m/s
	static constexpr float MAX_OMEGA = 1.0F;          // rad/s
	static constexpr float ACCEL = 0.5F;              // m/s^2
	static constexpr float STOP_DIST = 0.005F;        // stop position accuracy [m]
	static constexpr float HEADING_TOL = 0.02F;       // rad

	void init(ReachedFn reached, ActionDoneFn done, void *ctx);
	void set_pose(float x, float y, float theta);
	bool push(const MotionTarget &t);
	void stop(uint32_t ack_seq);          // clear the queue, brake, acknowledge
	void set_paused(bool p) { paused_ = p; }
	void set_hold(bool h) { held_ = h; }
	void set_speed_limit(float v) { speed_cap_ = v; }

	bool start_action(uint8_t uid, uint8_t code, uint32_t duration_ms);
	void cancel_action(uint8_t uid);

	// Advance the model by dt seconds.
	void step(float dt);

	float x() const { return x_; }
	float y() const { return y_; }
	float theta() const { return th_; }
	float speed() const { return v_; }
	float omega() const { return w_; }
	bool driving() const { return v_ != 0.0F || w_ != 0.0F; }
	bool paused() const { return paused_; }
	bool held() const { return held_; }
	bool loaded() const { return loaded_; }
	uint8_t queued() const { return count_; }
	bool any_popped() const { return any_popped_; }
	uint32_t last_popped() const { return last_popped_; }
	uint32_t stop_ack() const { return stop_ack_; }
	float speed_limit() const { return speed_cap_; }

private:
	struct Act {
		uint8_t uid;
		uint8_t code;
		uint32_t remaining_ms;
		bool used;
	};

	void pop();
	void step_actions(float dt);

	ReachedFn reached_ = nullptr;
	ActionDoneFn done_ = nullptr;
	void *ctx_ = nullptr;
	float x_ = 0, y_ = 0, th_ = 0, v_ = 0, w_ = 0;
	bool paused_ = false;
	bool held_ = false;
	bool loaded_ = false;
	float speed_cap_ = 0;
	MotionTarget q_[QUEUE] = {};
	uint8_t head_ = 0, count_ = 0;
	bool final_turn_ = false;
	bool any_popped_ = false;
	uint32_t last_popped_ = 0;
	uint32_t stop_ack_ = 0;
	Act acts_[ACTIONS] = {};
	float act_ms_acc_ = 0;
};
