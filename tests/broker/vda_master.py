"""Bench "fleet control" for the MCORE VDA 5050 robot (end-to-end test).

Talks to the board through the local test broker (plain listener 1883 on
localhost, see run_broker.sh) using mosquitto_pub / mosquitto_sub, and checks
every message the robot publishes against the official VDA 5050 3.0.0 JSON
schemas (third_party/vda5050/schemas).

Scenario:
  1. robot is ONLINE (retained connection) and has published a factsheet
  2. initializePosition at node "home" (0, 0)
  3. order 0: home -> A (pick, HARD) -> B; horizon B -> C
  4. order update 1 at decision point B: B -> C (drop, HARD)
  5. pause / resume while driving, then a second order cancelled half-way
Usage: python vda_master.py   (exit code 0 = pass)
"""
import json
import os
import queue
import subprocess
import sys
import threading
import time

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "vda"))
from validate import load_schema  # noqa: E402
from jsonschema import Draft202012Validator  # noqa: E402

BASE = "vda5050/v3/MCORE/agv001"
HOST = ["-h", "127.0.0.1", "-p", "1883"]
MAP = "local"

validators = {t: Draft202012Validator(load_schema(t))
              for t in ("state", "connection", "factsheet", "order", "instantActions")}
msgs = queue.Queue()
counts = {"state": 0, "connection": 0, "factsheet": 0}
invalid = []
latest = {}
header_ids = {}
t0 = time.time()


def log(text):
    print(f"[{time.time() - t0:6.1f}s] {text}", flush=True)


def reader():
    p = subprocess.Popen(["mosquitto_sub", *HOST, "-v", "-t", BASE + "/#"],
                         stdout=subprocess.PIPE, text=True, encoding="utf-8")
    for line in p.stdout:
        topic, _, payload = line.rstrip("\n").partition(" ")
        leaf = topic.rsplit("/", 1)[-1]
        if leaf not in counts:
            continue  # our own order / instantActions
        doc = json.loads(payload)
        errors = list(validators[leaf].iter_errors(doc))
        if errors:
            invalid.append((leaf, [e.message for e in errors]))
        prev = header_ids.get(leaf)
        if prev is not None and leaf == "state" and doc["headerId"] != prev + 1:
            invalid.append((leaf, [f"headerId jumped {prev} -> {doc['headerId']}"]))
        header_ids[leaf] = doc["headerId"]
        counts[leaf] += 1
        latest[leaf] = doc
        msgs.put((leaf, doc))


def publish(leaf, doc):
    errors = list(validators[leaf].iter_errors(doc))
    assert not errors, f"test bug: our {leaf} is invalid: {errors[0].message}"
    subprocess.run(["mosquitto_pub", *HOST, "-t", f"{BASE}/{leaf}", "-m", json.dumps(doc)], check=True)


hid = {"order": 0, "instantActions": 0}


def header(leaf):
    hid[leaf] += 1
    return {"headerId": hid[leaf], "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S.000Z", time.gmtime()),
            "version": "3.0.0", "manufacturer": "MCORE", "serialNumber": "agv001"}


def instant(*actions):
    publish("instantActions", {**header("instantActions"), "actions": list(actions)})


def act(kind, aid, blocking="NONE", params=None):
    a = {"actionType": kind, "actionId": aid, "blockingType": blocking}
    if params:
        a["actionParameters"] = [{"key": k, "value": v} for k, v in params.items()]
    return a


def node(nid, seq, x, y, released=True, actions=()):
    return {"nodeId": nid, "sequenceId": seq, "released": released,
            "nodePosition": {"x": x, "y": y, "mapId": MAP}, "actions": list(actions)}


def edge(eid, seq, released=True, actions=()):
    return {"edgeId": eid, "sequenceId": seq, "released": released, "actions": list(actions)}


def order(oid, upd, nodes, edges):
    publish("order", {**header("order"), "orderId": oid, "orderUpdateId": upd, "nodes": nodes, "edges": edges})


def wait_state(pred, timeout, what):
    deadline = time.time() + timeout
    while time.time() < deadline:
        s = latest.get("state")
        if s is not None and pred(s):
            return s
        try:
            msgs.get(timeout=0.2)
        except queue.Empty:
            pass
    raise AssertionError(f"timeout waiting for: {what}; last state: {json.dumps(latest.get('state'))[:600]}")


def action_status(s, aid, key="actionStates"):
    for a in s[key]:
        if a["actionId"] == aid:
            return a["actionStatus"]
    return None


def pos(s):
    p = s["mobileRobotPosition"]
    return f"({p['x']:.2f}, {p['y']:.2f}, th {p['theta']:.2f})"


def main():
    threading.Thread(target=reader, daemon=True).start()

    wait_state(lambda s: True, 15, "first state")
    deadline = time.time() + 5
    while time.time() < deadline and "connection" not in latest:
        time.sleep(0.1)
    assert latest.get("connection", {}).get("connectionState") == "ONLINE", "connection not ONLINE"
    log("robot ONLINE, state received")
    if "factsheet" not in latest:
        instant(act("factsheetRequest", "fs-1"))
        deadline = time.time() + 5
        while time.time() < deadline and "factsheet" not in latest:
            time.sleep(0.1)
    assert "factsheet" in latest, "no factsheet"
    log(f"factsheet: {len(latest['factsheet']['protocolFeatures']['mobileRobotActions'])} actions supported")

    # 1. position
    instant(act("initializePosition", "init-1", params={"x": 0.0, "y": 0.0, "theta": 0.0,
                                                         "mapId": MAP, "lastNodeId": "home"}))
    wait_state(lambda s: action_status(s, "init-1", "instantActionStates") == "FINISHED"
               and abs(s["mobileRobotPosition"]["x"]) < 0.01, 5, "initializePosition")
    log("position initialised at home")

    # 2. order 0: base home-A-B, horizon B-C
    order("order-1", 0,
          [node("home", 0, 0, 0), node("A", 2, 1.0, 0, actions=[act("pick", "pick-A", "HARD", {"loadId": "L1"})]),
           node("B", 4, 1.0, 1.0), node("C", 6, 0, 1.0, released=False,
                                       actions=[act("drop", "drop-C", "HARD")])],
          [edge("e1", 1), edge("e2", 3), edge("e3", 5, released=False)])
    s = wait_state(lambda s: s["orderId"] == "order-1", 5, "order accepted")
    assert not s["errors"], f"order rejected: {s['errors']}"
    log(f"order-1/0 accepted: nodeStates {[n['nodeId'] for n in s['nodeStates']]}")

    s = wait_state(lambda s: action_status(s, "pick-A") == "RUNNING", 15, "pick at A")
    log(f"at A {pos(s)}, lastNodeId={s['lastNodeId']}, pick RUNNING, driving={s['driving']}")
    assert not s["driving"], "must stand still during a HARD action"
    s = wait_state(lambda s: action_status(s, "pick-A") == "FINISHED", 10, "pick finished")
    log(f"pick FINISHED, loads={s['loads']}")

    # 3. pause / resume on the way to B
    wait_state(lambda s: s["driving"], 5, "driving to B")
    instant(act("startPause", "pause-1"))
    s = wait_state(lambda s: s["paused"] and not s["driving"], 5, "paused")
    p1 = s["mobileRobotPosition"]["y"]
    time.sleep(1.5)
    assert abs(latest["state"]["mobileRobotPosition"]["y"] - p1) < 0.01, "moved while paused"
    log(f"paused at {pos(s)}, stays still")
    instant(act("stopPause", "pause-2"))
    wait_state(lambda s: not s["paused"], 5, "resumed")
    log("resumed")

    s = wait_state(lambda s: s["lastNodeId"] == "B" and not s["driving"], 20, "decision point B")
    log(f"stopped at decision point B {pos(s)}; horizon: {[n['nodeId'] for n in s['nodeStates']]}")
    assert [n["nodeId"] for n in s["nodeStates"]] == ["C"]

    # 4. update at decision point: release B-C
    order("order-1", 1, [node("B", 4, 1.0, 1.0), node("C", 6, 0, 1.0, actions=[act("drop", "drop-C", "HARD")])],
          [edge("e3", 5)])
    s = wait_state(lambda s: s["orderUpdateId"] == 1, 5, "update accepted")
    assert not s["errors"], f"update rejected: {s['errors']}"
    s = wait_state(lambda s: action_status(s, "drop-C") == "FINISHED" and not s["nodeStates"], 30, "order done")
    log(f"order-1 done at {pos(s)}, lastNodeId={s['lastNodeId']}, loads={s['loads']}")

    # 5. a new order, cancelled while driving
    order("order-2", 0, [node("C", 0, 0, 1.0), node("D", 2, 0, 3.0)], [edge("e10", 1)])
    wait_state(lambda s: s["orderId"] == "order-2" and s["driving"], 10, "driving to D")
    instant(act("cancelOrder", "cancel-1"))
    s = wait_state(lambda s: action_status(s, "cancel-1", "instantActionStates") == "FINISHED", 10, "cancelled")
    log(f"cancelOrder FINISHED at {pos(s)}, nodeStates={s['nodeStates']}, driving={s['driving']}")
    assert not s["nodeStates"] and not s["edgeStates"] and not s["driving"]

    # an update to the cancelled order must be rejected
    order("order-2", 1, [node("C", 0, 0, 1.0), node("D", 2, 0, 3.0)], [edge("e10", 1)])
    s = wait_state(lambda s: any(e["errorType"] == "ORDER_UPDATE_FOLLOWING_CANCEL" for e in s["errors"]), 5,
                   "ORDER_UPDATE_FOLLOWING_CANCEL")
    log("update after cancel rejected with ORDER_UPDATE_FOLLOWING_CANCEL")

    time.sleep(1)
    log(f"messages: {counts}; schema violations: {len(invalid)}")
    for leaf, errs in invalid[:10]:
        log(f"  INVALID {leaf}: {errs[:3]}")
    print("PASS" if not invalid else "FAIL")
    return 0 if not invalid else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except AssertionError as e:
        log(f"FAIL: {e}")
        sys.exit(1)
