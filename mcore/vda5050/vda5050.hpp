#pragma once
/*
 * vda5050.hpp — VDA 5050 3.0.0 mobile-robot side: orders, actions, zones,
 * requests/responses, maps, hibernation and the state / connection /
 * factsheet / visualization messages.
 *
 * Transport-free and allocation-free: JSON comes in as text (MQTT payloads),
 * JSON goes out into caller buffers. Motion and actuators are reached through
 * RobotPort (on the STM32H755: numeric commands to the Cortex-M7), so the
 * protocol logic runs and is tested on any host.
 *
 * Implemented (VDA 5050 3.0.0 section numbers):
 *   6.1  orders and order updates, base/horizon, stitching, rejections
 *        (6.1.4), cancellation (6.1.3), corridors (6.1.5, 6.6.10)
 *   6.2  node/edge/zone actions with blocking types NONE/SOFT/SINGLE/HARD,
 *        instant actions incl. hibernation, shutdown, maps, zone sets
 *   6.3  maps (download simulated: the link is not fetched), enable, delete
 *   6.4  zone sets via the zoneSet topic; BLOCKED, RELEASE (ACCESS requests,
 *        lease expiry, releaseLossBehavior), SPEED_LIMIT, ACTION; LINE_GUIDED,
 *        PRIORITY, PENALTY are satisfied trivially (the robot follows the
 *        edges, it never plans its own route)
 *   6.5  connection ONLINE / OFFLINE / HIBERNATING / CONNECTION_BROKEN
 *   6.6  state, 6.7 visualization (intermediatePath), 6.9 responses
 *   7.3  edge trajectories (NURBS) and corridors
 * Not supported: COORDINATED_REPLANNING / DIRECTED / BIDIRECTED zones
 * (accepted, ignored), downloadZoneSet, updateCertificate, logReport.
 *
 * Motion model: the order geometry is turned into waypoints ~PATH_SPACING
 * apart (straight edges or sampled NURBS). Each segment is checked against
 * zones and obstacles BEFORE it is streamed to the robot, so the robot never
 * receives a waypoint inside a zone it may not enter.
 */
#include <cstddef>
#include <cstdint>

#include "vda5050/geometry.hpp"

namespace vda {

// --- capacities (reported in the factsheet's protocolLimits) ---------------
constexpr size_t ID_LEN = 48;          // nodeId, edgeId, actionId, orderId (+NUL)
constexpr size_t TYPE_LEN = 32;        // actionType, mapId
constexpr size_t MAX_NODES = 32;       // nodes held (base + horizon)
constexpr size_t MAX_EDGES = MAX_NODES - 1;
constexpr size_t MAX_ACTIONS = 32;     // node + edge actions held
constexpr size_t MAX_TRAJ = 8;         // edges with a NURBS trajectory per order
constexpr size_t MAX_INSTANT = 16;     // instantActionStates kept
constexpr size_t MAX_PARAMS = 5;       // actionParameters kept per action
constexpr size_t MAX_ERRORS = 8;
constexpr size_t MAX_REFS = 3;
constexpr size_t MAX_ZONE_SETS = 2;
constexpr size_t MAX_ZONES = 16;       // per zone set
constexpr size_t MAX_VERTICES = 12;    // per zone
constexpr size_t MAX_ZONE_ACTION_DEFS = 12;   // per zone set
constexpr size_t MAX_ZONE_ACTIONS = 12;       // zoneActionStates kept
constexpr size_t MAX_REQUESTS = 8;
constexpr size_t MAX_MAPS = 4;
constexpr size_t MAX_OBSTACLES = 4;
constexpr size_t PATH_CAP = 128;       // waypoints generated ahead (12.8 m)
constexpr float PATH_SPACING = 0.1F;   // [m]
constexpr uint32_t STREAM_AHEAD = 16;  // waypoints queued on the robot
constexpr float ROBOT_RADIUS = 0.5F;   // contour as a disc (0.8 x 0.6 m)

enum class Blocking : uint8_t { None, Soft, Single, Hard };
enum class ActionStatus : uint8_t { Waiting, Initializing, Running, Paused, Retriable, Finished, Failed };

enum class ActionKind : uint8_t {
	Unknown,
	// order / zone actions
	Pick, Drop, WaitForTrigger, FinePositioning, StartCharging, StopCharging,
	// instant actions
	CancelOrder, StartPause, StopPause, StateRequest, FactsheetRequest,
	InitializePosition, ClearInstantActions, Trigger,
	StartHibernation, StopHibernation, Shutdown,
	DownloadMap, EnableMap, DeleteMap,
	EnableZoneSet, DeleteZoneSet, ClearZoneActions,
	SimObstacle, SimClearObstacles     // vendor actions for the vehicle model
};

enum class ErrorLevel : uint8_t { Warning, Urgent, Critical, Fatal };

struct Param {
	char key[20];
	char value[32];    // JSON strings unescaped, numbers/bools as written
};

// actionParameters. Only instant actions read them, synchronously while the
// message is processed, so they live on the stack and not in every Action.
struct ActionParams {
	uint8_t n;
	Param p[MAX_PARAMS];
};

struct Action {
	char id[ID_LEN];
	char type[TYPE_LEN];
	ActionKind kind;
	Blocking blocking;
	ActionStatus status;
	uint8_t uid;          // stable handle for the robot (slots are compacted)
	int32_t owner_seq;    // sequenceId of its node/edge, -1 instant, -2 zone
	bool robot_side;      // executed by RobotPort (pick/drop)
};

struct Node {
	char id[ID_LEN];
	char map[TYPE_LEN];
	uint32_t seq;
	bool released;
	bool has_theta;
	float x, y, theta;
	float dev_xy;         // allowedDeviationXY: larger semi-axis (ellipse -> circle)
};

struct Corridor {
	bool present;
	bool release_required;
	bool return_on_loss;  // releaseLossBehavior RETURN (default STOP)
	bool contour_ref;     // corridorReferencePoint CONTOUR
	float left, right;    // widths [m]
};

struct Edge {
	char id[ID_LEN];
	uint32_t seq;
	bool released;
	int8_t traj;          // index into Order::traj, -1 = straight line
	float max_speed;      // 0 = not set
	Corridor corridor;
};

// What the order manager streams to the vehicle
struct DriveTarget {
	uint32_t path_index;  // global waypoint counter, reported back as popped
	uint32_t node_seq;    // valid if is_node
	float x, y, theta;
	bool is_node;
	bool has_theta;
	float max_speed;      // 0 = robot default
	float tolerance;      // pass-through radius [m]
};

class RobotPort {
public:
	virtual bool drive_to(const DriveTarget &t) = 0;      // append to the motion queue
	virtual void stop_motion(uint32_t ack_seq) = 0;       // clear the queue, brake; acked in status
	virtual void set_paused(bool paused) = 0;             // VDA pause (startPause/stopPause)
	virtual void set_hold(bool hold) = 0;                 // stop for zones/actions, keep the queue
	virtual void set_speed_limit(float mps) = 0;          // 0 = none
	virtual bool start_action(uint8_t uid, const Action &a) = 0;
	virtual void cancel_action(uint8_t uid) = 0;
	virtual void set_pose(float x, float y, float theta) = 0;
protected:
	~RobotPort() = default;
};

struct RobotStatus {
	bool valid;           // false until the real-time core reports
	float x, y, theta;
	float vx, vy, omega;
	bool driving;
	bool paused;
	bool loaded;
	float battery_soc;    // percent
	bool any_popped;      // a waypoint was reached since boot
	uint32_t last_popped; // path_index of the last waypoint reached
	uint32_t stop_ack;    // ack_seq of the last stop_motion processed
	uint8_t queued;       // waypoints still queued on the robot
};

struct Config {
	const char *manufacturer;
	const char *serial;
	const char *map_id;           // map enabled at boot (version "1")
	const char *series_name;
	float default_dev_xy;         // node reached / start node range if the order has none [m]
	float max_speed;              // [m/s] (factsheet)
	uint32_t state_interval_ms;   // periodic state while idle
	uint32_t state_interval_driving_ms;
	uint32_t min_state_interval_ms;
	uint32_t map_download_ms;     // simulated download time
};

struct ErrorRef {
	char key[16];
	char value[ID_LEN];
};

struct Error {
	char type[32];
	char description[64];
	ErrorLevel level;
	uint8_t nrefs;
	ErrorRef refs[MAX_REFS];
	uint8_t clear_on;     // OrderManager::ClearOn
	uint32_t expires_ms;  // for ClearOn::AfterTimeout
};

struct Order {
	char order_id[ID_LEN];
	uint32_t update_id;
	uint32_t content_hash;    // nodes+edges text, for SAME_ORDER_UPDATE_ID
	uint8_t nnodes, nedges, nactions, ntraj;
	Node nodes[MAX_NODES];
	Edge edges[MAX_EDGES];
	Action actions[MAX_ACTIONS];
	geo::Nurbs traj[MAX_TRAJ];
};

// --- zones ---------------------------------------------------------------------
enum class ZoneType : uint8_t {
	Blocked, LineGuided, Release, CoordinatedReplanning, SpeedLimit,
	Action, Priority, Penalty, Directed, Bidirected
};
enum class LossBehavior : uint8_t { Stop, Continue, Evacuate };

struct ZoneActionDef {
	char type[TYPE_LEN];
	ActionKind kind;
	Blocking blocking;
	uint8_t phase;        // 0 entry, 1 during, 2 exit
	uint8_t zone;         // index in ZoneSet::zones
};

struct Zone {
	char id[32];
	ZoneType type;
	LossBehavior loss;
	uint8_t nverts;
	bool inside;          // robot contour touches the zone
	float max_speed;
	geo::Vec2 v[MAX_VERTICES];
};

struct ZoneSet {
	bool used;
	bool enabled;
	char id[ID_LEN];
	char map[TYPE_LEN];
	uint8_t nzones, nacts;
	Zone zones[MAX_ZONES];
	ZoneActionDef acts[MAX_ZONE_ACTION_DEFS];
};

enum class ReqType : uint8_t { Access, Corridor };
enum class ReqStatus : uint8_t { Requested, Granted, Revoked, Expired };

struct Request {
	bool used;
	bool queued;          // fleet control answered QUEUED
	bool in_use;          // inside the zone / deviating in the corridor
	ReqType type;
	ReqStatus status;
	char id[16];
	uint8_t zone_set, zone;
	uint8_t edge;         // edge index (corridor)
	uint32_t edge_seq;
	uint64_t lease_ms;    // 0 = no lease
	bool rejected;        // fleet control answered REJECTED (not reported)
	uint32_t rejected_ms; // tick of that answer: re-request after a while
	uint32_t until_index; // corridor: last waypoint of the deviation
};

struct MapEntry {
	bool used;
	bool enabled;
	char id[TYPE_LEN];
	char version[16];
};

struct Obstacle {
	bool used;
	float x, y, r;
};

struct PathPoint {
	float x, y, theta;
	float vmax;
	float tol;
	uint32_t node_seq;
	uint8_t edge;         // edge index in the current order
	uint8_t flags;        // PP_NODE, PP_THETA, PP_OFFSET
	float ox, oy;         // original (unshifted) position
};
constexpr uint8_t PP_NODE = 1, PP_THETA = 2, PP_OFFSET = 4;

class OrderManager {
public:
	void init(const Config &cfg, RobotPort *robot);

	// --- input from MQTT (payload text, need not be NUL-terminated) -------
	void on_order(const char *json, size_t len);
	void on_instant_actions(const char *json, size_t len);
	void on_zone_set(const char *json, size_t len);
	void on_responses(const char *json, size_t len);

	// --- input from the robot ----------------------------------------------
	void update_robot(const RobotStatus &s);
	void on_node_reached(uint32_t seq);
	void on_action_done(uint8_t uid, bool ok);

	// Advances actions, zones and motion; call every main-loop pass.
	// unix_ms = wall clock (lease expiry, ETAs, wake-up time), 0 if unknown.
	void poll(uint32_t now_ms, uint64_t unix_ms = 0);

	// --- output --------------------------------------------------------------
	bool state_due(uint32_t now_ms) const;
	bool factsheet_requested() const { return factsheet_req_; }
	size_t build_state(char *buf, size_t cap, uint32_t header_id, uint64_t unix_ms);
	size_t build_factsheet(char *buf, size_t cap, uint32_t header_id, uint64_t unix_ms);
	size_t build_connection(char *buf, size_t cap, uint32_t header_id, uint64_t unix_ms,
	                        const char *connection_state) const;
	// state_header_id: headerId of the last state message (referenceStateHeaderId)
	size_t build_visualization(char *buf, size_t cap, uint32_t header_id, uint64_t unix_ms,
	                           uint32_t state_header_id);
	void state_sent(uint32_t now_ms);
	void factsheet_sent() { factsheet_req_ = false; }

	// Connection state the robot should advertise: ONLINE, HIBERNATING or
	// OFFLINE (after shutdown). Changes are to be published (retained).
	const char *connection_state() const;
	bool shutdown_requested() const { return shutdown_; }
	bool hibernating() const { return hibernating_; }

	// --- inspection (tests, diagnostics) -------------------------------------
	bool idle() const;
	const char *order_id() const { return cur_.order_id; }
	uint32_t order_update_id() const { return cur_.update_id; }
	const char *last_node_id() const { return last_node_id_; }
	uint32_t last_node_seq() const { return last_node_seq_; }
	uint8_t node_states() const { return static_cast<uint8_t>(cur_.nnodes - first_pending_node_); }
	uint8_t edge_states() const { return static_cast<uint8_t>(cur_.nedges - first_pending_edge_); }
	const Action *find_action(const char *id) const;
	const Action *find_instant(const char *id) const;
	const Action *zone_action(uint8_t i) const { return i < nzone_actions_ ? &zone_actions_[i] : nullptr; }
	uint8_t zone_action_count() const { return nzone_actions_; }
	bool has_error(const char *type) const;
	uint8_t error_count() const { return nerrors_; }
	bool paused() const { return robot_.paused; }
	const Request *request(uint8_t i) const { return i < MAX_REQUESTS && requests_[i].used ? &requests_[i] : nullptr; }
	const ZoneSet *zone_set(uint8_t i) const { return i < MAX_ZONE_SETS && zone_sets_[i].used ? &zone_sets_[i] : nullptr; }
	const MapEntry *map(uint8_t i) const { return i < MAX_MAPS && maps_[i].used ? &maps_[i] : nullptr; }
	const char *current_map() const { return current_map_; }
	float speed_limit() const { return speed_limit_; }
	bool held() const { return hold_mask_ != 0; }
	uint32_t streamed() const { return path_sent_; }
	const PathPoint *path_point(uint32_t index) const;

	enum ClearOn : uint8_t { UntilNewOrder, UntilNewInstantAction, UntilResolved, AfterTimeout };
	enum HoldReason : uint8_t {
		HoldZoneViolation = 1, HoldReleaseLost = 2, HoldZoneAction = 4, HoldReplan = 8, HoldCorridorLost = 16
	};

private:
	// acceptance
	void accept_new(const Order &in);
	bool accept_update(const Order &in);
	void reject(const char *type, ErrorLevel level, const Order &in, const char *descr);

	// errors
	void add_error(const char *type, ErrorLevel level, uint8_t clear_on, const char *descr,
	               const char *k1 = nullptr, const char *v1 = nullptr,
	               const char *k2 = nullptr, const char *v2 = nullptr,
	               const char *k3 = nullptr, const char *v3 = nullptr);
	void clear_errors(uint8_t clear_on);
	void remove_error(const char *type);

	// order progress / actions
	void traverse_first_node();
	void leave_edge(uint32_t edge_seq);
	void process_actions();
	bool blocking_pending() const;
	void clear_order(bool cancelled);
	void cancel_order(Action &cancel_action, const ActionParams &params);
	void start_action_common(Action &a);
	Action *find_by_uid(uint8_t uid);
	Action *find_instant_uid(uint8_t uid);
	Action *add_instant(const Action &a);
	bool map_enabled(const char *id) const;
	void process_instant(Action &ia, const ActionParams &pa);
	void mark_dirty() { dirty_ = true; }

	// motion (motion.cpp)
	void generate_path();
	void stream_path();
	void request_replan();
	bool segment_clear(uint32_t index);
	uint32_t plan_obstacles_ahead();   // returns the index streaming must stop at
	bool plan_avoidance(uint32_t from, uint32_t c1, uint32_t c2, const Obstacle &ob, const Corridor &c,
	                    uint32_t &end_index);
	void process_motion();
	void set_hold_reason(uint8_t reason, bool on);
	PathPoint &pp(uint32_t index) { return path_[index % PATH_CAP]; }

	// zones (zones.cpp)
	void process_zones();
	void process_requests();
	Request *find_request(ReqType t, uint8_t zs, uint8_t zone, uint32_t edge_seq);
	Request *need_request(ReqType t, uint8_t zs, uint8_t zone, uint8_t edge, uint32_t edge_seq);
	bool zone_granted(uint8_t zs, uint8_t z);
	void zone_entered(uint8_t zs, uint8_t z);
	void zone_left(uint8_t zs, uint8_t z);
	void release_lost(Request &r);
	void process_zone_actions();

	Config cfg_{};
	RobotPort *robot_port_ = nullptr;
	RobotStatus robot_{};

	Order cur_{};
	Order in_{};              // parse buffer
	bool have_order_ = false;
	bool cancelled_ = false;
	uint8_t first_pending_node_ = 0;   // nodes before this index are traversed
	uint8_t first_pending_edge_ = 0;
	uint8_t gen_nodes_ = 0;            // nodes whose approach is in the path buffer
	char last_node_id_[ID_LEN] = "";
	uint32_t last_node_seq_ = 0;

	Action instant_[MAX_INSTANT]{};
	uint8_t ninstant_ = 0;
	uint8_t pending_cancel_uid_ = 0;   // cancelOrder waiting for the robot to stop
	uint8_t next_uid_ = 0;             // set by init(): all-zero members keep the object in .bss
	bool charging_ = false;

	Error errors_[MAX_ERRORS]{};
	uint8_t nerrors_ = 0;

	// path streaming
	PathPoint path_[PATH_CAP]{};
	uint32_t path_base_ = 0;           // first index of the current order's path
	uint32_t path_gen_ = 0;            // next index to generate
	uint32_t path_sent_ = 0;           // next index to stream
	uint32_t path_done_ = 0;           // first index not yet reached by the robot
	uint32_t stop_seq_ = 0;            // last stop_motion ack_seq requested
	bool replanning_ = false;
	uint8_t hold_mask_ = 0;
	float speed_limit_ = 0.0F;
	bool blocked_reported_ = false;    // NODE_UNREACHABLE for the current stop
	bool pose_pending_ = false;        // initializePosition sent, robot not there yet
	float pose_x_ = 0, pose_y_ = 0;
	bool waiting_obstacle_ = false;

	// zones / requests / maps / obstacles
	ZoneSet zone_sets_[MAX_ZONE_SETS]{};
	Action zone_actions_[MAX_ZONE_ACTIONS]{};
	uint8_t nzone_actions_ = 0;
	uint32_t zone_action_counter_ = 0;
	Request requests_[MAX_REQUESTS]{};
	uint32_t request_counter_ = 0;
	MapEntry maps_[MAX_MAPS]{};
	char current_map_[TYPE_LEN] = "";
	uint8_t download_uid_ = 0;         // downloadMap in progress
	uint32_t download_done_ms_ = 0;
	MapEntry download_{};
	Obstacle obstacles_[MAX_OBSTACLES]{};

	// lifecycle
	bool hibernating_ = false;
	bool hibernate_pending_ = false;   // last state before going quiet
	uint64_t wake_unix_ms_ = 0;
	bool shutdown_ = false;
	bool shutdown_pending_ = false;

	bool dirty_ = false;
	bool factsheet_req_ = false;
	uint32_t last_state_ms_ = 0;
	bool last_driving_ = false;
	bool last_paused_ = false;
	uint32_t now_ms_ = 0;
	uint64_t unix_ms_ = 0;
};

const char *to_string(ActionStatus s);
const char *to_string(ErrorLevel l);
const char *to_string(ReqStatus s);
ActionKind action_kind(const char *type);
uint32_t fnv1a(const char *p, size_t n, uint32_t h = 2166136261U);

} // namespace vda
