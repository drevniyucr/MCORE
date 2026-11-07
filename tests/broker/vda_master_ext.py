"""Bench "fleet control", part 2: the extended VDA 5050 3.0.0 features.

Same setup as vda_master.py (local broker on 127.0.0.1:1883, board online,
SNTP running). Every message of the robot is checked against the official
schemas, including visualization.

Scenario:
  1. visualization arrives with referenceStateHeaderId
  2. zoneSet zs1 (SPEED_LIMIT, RELEASE, ACTION, BLOCKED) loaded and enabled
  3. order home -> n1 through the zones: slows down in SPEED_LIMIT, stops in
     front of RELEASE and asks, drives on after GRANTED, entry/exit zone actions
  4. order towards the BLOCKED zone: stops in front, NODE_UNREACHABLE, cancel
  5. NURBS trajectory edge (quarter circle)
  6. corridor + simulated obstacle + releaseRequired: edgeRequest, GRANTED,
     detour inside the corridor
  7. maps: downloadMap (simulated), enableMap, deleteMap
  8. hibernation: woken by wakeUpTime, then by stopHibernation
  9. (--shutdown) shutdown: OFFLINE, graceful disconnect (no will)
Usage: python vda_master_ext.py [--shutdown]   (exit code 0 = pass)
"""
import json
import math
import subprocess
import sys
import time

import vda_master as vm
from validate import load_schema
from jsonschema import Draft202012Validator

for leaf in ("visualization", "zoneSet", "responses"):
    vm.validators[leaf] = Draft202012Validator(load_schema(leaf))
vm.counts["visualization"] = 0
vm.hid.update({"zoneSet": 0, "responses": 0})

log, node, edge, instant = vm.log, vm.node, vm.edge, vm.instant
wait_state, pos = vm.wait_state, vm.pos
RUN = f"-{int(time.time()) % 100000}"   # action ids unique per run (the robot keeps old states)


def act(kind, aid, blocking="NONE", params=None):
    return vm.act(kind, aid + RUN, blocking, params)


def action_status(s, aid, key="actionStates"):
    return vm.action_status(s, aid + RUN, key)


samples = []        # (time, leaf, doc) for state / visualization
conn_log = []       # (time, connectionState)


def reader():
    p = subprocess.Popen(["mosquitto_sub", *vm.HOST, "-v", "-t", vm.BASE + "/#"],
                         stdout=subprocess.PIPE, text=True, encoding="utf-8")
    for line in p.stdout:
        topic, _, payload = line.rstrip("\n").partition(" ")
        leaf = topic.rsplit("/", 1)[-1]
        if leaf not in vm.counts:
            continue
        doc = json.loads(payload)
        errors = list(vm.validators[leaf].iter_errors(doc))
        if errors:
            vm.invalid.append((leaf, [e.message for e in errors]))
        prev = vm.header_ids.get(leaf)
        if prev is not None and leaf in ("state", "visualization") and doc["headerId"] != prev + 1:
            vm.invalid.append((leaf, [f"headerId jumped {prev} -> {doc['headerId']}"]))
        vm.header_ids[leaf] = doc["headerId"]
        vm.counts[leaf] += 1
        vm.latest[leaf] = doc
        now = time.time()
        if leaf in ("state", "visualization"):
            samples.append((now, leaf, doc))
        if leaf == "connection":
            conn_log.append((now, doc["connectionState"]))
        vm.msgs.put((leaf, doc))


def iso(t):
    return time.strftime("%Y-%m-%dT%H:%M:%S", time.gmtime(t)) + f".{int((t % 1) * 1000):03d}Z"


def pub(leaf, body):
    vm.publish(leaf, {**vm.header(leaf), **body})


def respond(req_id, grant, lease_s=None):
    r = {"requestId": req_id, "grantType": grant}
    if lease_s is not None:
        r["leaseExpiry"] = iso(time.time() + lease_s)
    pub("responses", {"responses": [r]})


def order(oid, upd, nodes, edges):
    pub("order", {"orderId": oid, "orderUpdateId": upd, "nodes": nodes, "edges": edges})


def square(x0, y0, x1, y1):
    return [{"x": x0, "y": y0}, {"x": x1, "y": y0}, {"x": x1, "y": y1}, {"x": x0, "y": y1}]


def wait_for(pred, timeout, what):
    deadline = time.time() + timeout
    while time.time() < deadline:
        if pred():
            return
        time.sleep(0.05)
    raise AssertionError(f"timeout waiting for: {what}")


def init_pose(aid, x, y, theta, node_id):
    instant(act("initializePosition", aid, params={"x": x, "y": y, "theta": theta, "mapId": vm.MAP,
                                                    "lastNodeId": node_id}))
    wait_state(lambda s: action_status(s, aid, "instantActionStates") == "FINISHED"
               and abs(s["mobileRobotPosition"]["x"] - x) < 0.01
               and abs(s["mobileRobotPosition"]["y"] - y) < 0.01, 5, aid)


def poses_since(t_start, leaf="visualization"):
    return [(d["mobileRobotPosition"], d["velocity"]) for t, lf, d in samples if t >= t_start and lf == leaf]


def has_error(s, kind):
    return any(e["errorType"] == kind for e in s["errors"])


def main():
    import threading
    threading.Thread(target=reader, daemon=True).start()
    wait_state(lambda s: True, 15, "first state")
    wait_for(lambda: vm.latest.get("connection", {}).get("connectionState") == "ONLINE", 5, "ONLINE")
    log("robot ONLINE")

    # 1. visualization ------------------------------------------------------------------
    wait_for(lambda: "visualization" in vm.latest, 8, "visualization")
    v = vm.latest["visualization"]
    assert "referenceStateHeaderId" in v
    log(f"visualization ok (headerId {v['headerId']}, refState {v['referenceStateHeaderId']})")

    init_pose("x-init-1", 0.0, 0.0, 0.0, "home")

    # 2. zone set -------------------------------------------------------------------------
    pub("zoneSet", {"zoneSet": {"mapId": vm.MAP, "zoneSetId": "zs1", "zones": [
        {"zoneId": "slow", "zoneType": "SPEED_LIMIT", "vertices": square(1.5, -1, 2.5, 1), "maximumSpeed": 0.2},
        {"zoneId": "gate", "zoneType": "RELEASE", "vertices": square(3.4, -1, 3.6, 1),
         "releaseLossBehavior": "STOP"},
        {"zoneId": "dock", "zoneType": "ACTION", "vertices": square(4.4, -1, 4.8, 1),
         "entryActions": [{"actionType": "startCharging", "blockingType": "NONE"}],
         "duringActions": [],
         "exitActions": [{"actionType": "stopCharging", "blockingType": "NONE"}]},
        {"zoneId": "wall", "zoneType": "BLOCKED", "vertices": square(4.0, 1.4, 8.0, 1.6)},
    ]}})
    s = wait_state(lambda s: any(z["zoneSetId"] == "zs1" for z in s["zoneSets"]), 5, "zone set stored")
    assert [z["zoneSetStatus"] for z in s["zoneSets"] if z["zoneSetId"] == "zs1"] == ["DISABLED"]
    instant(act("enableZoneSet", "x-ez-1", params={"zoneSetId": "zs1"}))
    wait_state(lambda s: [z["zoneSetStatus"] for z in s["zoneSets"] if z["zoneSetId"] == "zs1"] == ["ENABLED"],
               5, "zone set enabled")
    log("zoneSet zs1 stored DISABLED, enabled")

    # 3. through the zones ----------------------------------------------------------------
    t_start = time.time()
    order("zones-1", 0, [node("home", 0, 0, 0), node("n1", 2, 6.0, 0)], [edge("z1", 1)])
    s = wait_state(lambda s: s["zoneRequests"], 30, "access request for the gate")
    req = s["zoneRequests"][0]
    assert req["zoneId"] == "gate" and req["requestType"] == "ACCESS" and req["requestStatus"] == "REQUESTED"
    s = wait_state(lambda s: not s["driving"], 10, "stop in front of the gate")
    x_wait = s["mobileRobotPosition"]["x"]
    assert x_wait <= 2.9 + 0.02, f"robot too close to the gate: {x_wait}"
    slow = [vel["vx"] for p, vel in poses_since(t_start) if 1.6 <= p["x"] <= 2.4]
    assert slow and max(slow) <= 0.21, f"speed in SPEED_LIMIT zone: {slow}"
    log(f"SPEED_LIMIT: max vx {max(slow):.3f} m/s in zone ({len(slow)} samples); "
        f"waiting at x={x_wait:.2f} for {req['requestId']}")

    respond(req["requestId"], "QUEUED")
    time.sleep(1.5)
    s = vm.latest["state"]
    assert not s["driving"] and s["zoneRequests"][0]["requestStatus"] == "REQUESTED"
    respond(req["requestId"], "GRANTED", lease_s=120)
    s = wait_state(lambda s: s["zoneRequests"] and s["zoneRequests"][0]["requestStatus"] == "GRANTED", 5, "GRANTED")
    log("QUEUED: still waiting; GRANTED: driving on")
    s = wait_state(lambda s: any(a["actionType"] == "startCharging" for a in s["zoneActionStates"]), 20,
                   "dock entry action")
    s = wait_state(lambda s: s["lastNodeId"] == "n1" and not s["driving"] and not s["nodeStates"], 30, "n1")
    types = [a["actionType"] for a in s["zoneActionStates"]]
    assert types == ["startCharging", "stopCharging"], types
    assert not s["zoneRequests"], "access request not removed after leaving the gate"
    log(f"at n1 {pos(s)}; zone actions {types}; request removed")

    # 4. BLOCKED --------------------------------------------------------------------------
    order("zones-2", 0, [node("n1", 0, 6.0, 0), node("n2", 2, 6.0, 3.0)], [edge("z2", 1)])
    s = wait_state(lambda s: has_error(s, "NODE_UNREACHABLE"), 20, "NODE_UNREACHABLE")
    wait_state(lambda s: s["mobileRobotPosition"]["y"] > 0.5, 15, "driving towards the wall")
    s = wait_state(lambda s: not s["driving"], 10, "stop in front of the wall")
    assert s["mobileRobotPosition"]["y"] <= 0.9 + 0.02, pos(s)
    log(f"BLOCKED: stopped at {pos(s)} with NODE_UNREACHABLE")
    instant(act("cancelOrder", "x-cancel-1"))
    wait_state(lambda s: action_status(s, "x-cancel-1", "instantActionStates") == "FINISHED", 10, "cancel")
    instant(act("deleteZoneSet", "x-dz-1", params={"zoneSetId": "zs1"}),
            act("clearZoneActions", "x-cz-1"))
    wait_state(lambda s: not s["zoneSets"] and not s["zoneActionStates"], 5, "zone set deleted")
    log("order cancelled, zone set deleted")

    # 5. NURBS trajectory -----------------------------------------------------------------
    init_pose("x-init-2", 0.0, 0.0, 0.0, "home")
    t_start = time.time()
    e = edge("arc", 1)
    e["trajectory"] = {"degree": 2, "knotVector": [0, 0, 0, 1, 1, 1],
                       "controlPoints": [{"x": 0, "y": 0}, {"x": 1, "y": 0, "weight": math.sqrt(0.5)},
                                         {"x": 1, "y": 1}]}
    order("arc-1", 0, [node("home", 0, 0, 0), node("q", 2, 1.0, 1.0)], [e])
    s = wait_state(lambda s: s["lastNodeId"] == "q" and not s["driving"] and not s["nodeStates"], 20, "arc end")
    assert not has_error(s, "NODE_UNREACHABLE"), "NODE_UNREACHABLE must go with a new order"
    dev = [abs(math.hypot(p["x"], p["y"] - 1.0) - 1.0) for p, _ in poses_since(t_start)]
    assert len(dev) > 5 and max(dev) < 0.05, f"off the arc: {max(dev):.3f}"
    log(f"trajectory: {len(dev)} samples, max radial error {max(dev) * 100:.1f} cm, end {pos(s)}")

    # 6. corridor + obstacle + releaseRequired --------------------------------------------
    instant(act("mcoreSimObstacle", "x-ob-1", params={"x": 1.0, "y": 3.0, "radius": 0.2}))
    t_start = time.time()
    e = edge("c1", 1)
    e["corridor"] = {"leftWidth": 1.0, "rightWidth": 1.0, "releaseRequired": True}
    order("corr-1", 0, [node("q", 0, 1.0, 1.0), node("r", 2, 1.0, 5.0)], [e])
    s = wait_state(lambda s: s["edgeRequests"], 20, "edge request")
    req = s["edgeRequests"][0]
    assert req["requestType"] == "CORRIDOR" and req["edgeId"] == "c1"
    s = wait_state(lambda s: not s["driving"], 10, "waiting for the corridor")
    log(f"obstacle ahead: waiting at {pos(s)} for {req['requestId']}")
    respond(req["requestId"], "GRANTED")
    s = wait_state(lambda s: s["lastNodeId"] == "r" and not s["driving"] and not s["nodeStates"], 30, "r")
    lat = [abs(p["x"] - 1.0) for p, _ in poses_since(t_start)]
    clear = [math.hypot(p["x"] - 1.0, p["y"] - 3.0) for p, _ in poses_since(t_start)]
    assert max(lat) > 0.3 and max(lat) <= 1.0, f"lateral offset {max(lat):.2f}"
    assert min(clear) >= 0.65, f"too close to the obstacle: {min(clear):.2f}"
    assert not s["edgeRequests"]
    log(f"detour: max lateral {max(lat):.2f} m (corridor 1.0), min distance to obstacle centre "
        f"{min(clear):.2f} m; at r {pos(s)}")
    instant(act("mcoreSimClearObstacles", "x-ob-2"))

    # 7. maps -----------------------------------------------------------------------------
    if any(m["mapId"] == "hall2" for m in vm.latest["state"]["maps"]):   # left over by an aborted run
        instant(act("deleteMap", "x-dm-0", params={"mapId": "hall2", "mapVersion": "7"}))
        wait_state(lambda s: all(m["mapId"] != "hall2" for m in s["maps"]), 5, "old map deleted")
    instant(act("downloadMap", "x-dl-1", params={"mapId": "hall2", "mapVersion": "7",
                                               "mapDownloadLink": "https://example.invalid/hall2"}))
    wait_state(lambda s: action_status(s, "x-dl-1", "instantActionStates") == "RUNNING", 5, "download running")
    s = wait_state(lambda s: action_status(s, "x-dl-1", "instantActionStates") == "FINISHED", 10, "downloaded")
    assert {"mapId": "hall2", "mapVersion": "7", "mapStatus": "DISABLED"} in s["maps"], s["maps"]
    instant(act("enableMap", "x-em-1", params={"mapId": "hall2", "mapVersion": "7"}))
    s = wait_state(lambda s: {"mapId": "hall2", "mapVersion": "7", "mapStatus": "ENABLED"} in s["maps"], 5,
                   "map enabled")
    instant(act("deleteMap", "x-dm-1", params={"mapId": "hall2", "mapVersion": "7"}))
    s = wait_state(lambda s: all(m["mapId"] != "hall2" for m in s["maps"]), 5, "map deleted")
    log(f"maps: download -> DISABLED, enable -> ENABLED, delete; now {s['maps']}")

    # 8. hibernation ----------------------------------------------------------------------
    instant(act("startHibernation", "x-hb-1", params={"wakeUpTime": iso(time.time() + 6)}))
    wait_for(lambda: vm.latest["connection"]["connectionState"] == "HIBERNATING", 5, "HIBERNATING")
    wait_for(lambda: vm.latest["connection"]["connectionState"] == "ONLINE", 10, "woken by wakeUpTime")
    t_h = max(t for t, c in conn_log if c == "HIBERNATING")
    t_w = max(t for t, c in conn_log if c == "ONLINE")
    slept = t_w - t_h
    quiet = sum(1 for t, lf, _ in samples if lf == "state" and t_h < t < t_w)
    assert quiet == 0, f"{quiet} state messages while hibernating"
    log(f"hibernation: HIBERNATING for {slept:.1f} s without state messages, woke by wakeUpTime")
    instant(act("startHibernation", "x-hb-2"))
    wait_for(lambda: vm.latest["connection"]["connectionState"] == "HIBERNATING", 5, "HIBERNATING 2")
    time.sleep(1)
    instant(act("stopHibernation", "x-hb-3"))
    wait_for(lambda: vm.latest["connection"]["connectionState"] == "ONLINE", 5, "woken by stopHibernation")
    wait_state(lambda s: action_status(s, "x-hb-3", "instantActionStates") == "FINISHED", 5, "stopHibernation state")
    log("stopHibernation: ONLINE again, states resume")

    # 9. shutdown -------------------------------------------------------------------------
    if "--shutdown" in sys.argv:
        instant(act("shutdown", "x-sd-1"))
        wait_for(lambda: vm.latest["connection"]["connectionState"] == "OFFLINE", 5, "OFFLINE")
        t_off = time.time()
        time.sleep(4)
        late = [c for t, c in conn_log if t > t_off]
        assert not late, f"after OFFLINE: {late} (the will must not fire on a graceful disconnect)"
        log("shutdown: OFFLINE published, graceful disconnect (no CONNECTION_BROKEN)")

    time.sleep(1)
    log(f"messages: {vm.counts}; schema violations: {len(vm.invalid)}")
    for leaf, errs in vm.invalid[:10]:
        log(f"  INVALID {leaf}: {errs[:3]}")
    print("PASS" if not vm.invalid else "FAIL")
    return 0 if not vm.invalid else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except AssertionError as e:
        log(f"FAIL: {e}")
        sys.exit(1)
