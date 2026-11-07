"""Validate VDA 5050 messages against the official 3.0.0 JSON schemas.

Usage:
  python validate.py <dir>            every *.json in dir; the schema is picked
                                      from the file name prefix (state_*, order_*, ...)
  python validate.py --topic <topic>  read one message per line from stdin
                                      (e.g. piped from mosquitto_sub)
Exit code 0 only if every message is valid.
"""
import json
import os
import re
import sys

from jsonschema import Draft202012Validator

HERE = os.path.dirname(os.path.abspath(__file__))
SCHEMAS = os.path.join(HERE, "..", "..", "third_party", "vda5050", "schemas")
TOPICS = ["state", "connection", "factsheet", "order", "instantActions",
          "visualization", "responses", "zoneSet"]


def load_schema(topic):
    with open(os.path.join(SCHEMAS, topic + ".schema"), encoding="utf-8") as f:
        text = f.read()
    # order.schema 3.0.0 has a trailing comma (invalid JSON); tolerate it
    return json.loads(re.sub(r",(\s*[}\]])", r"\1", text))


def check(validator, doc, label):
    errors = sorted(validator.iter_errors(doc), key=lambda e: list(e.absolute_path))
    for e in errors:
        path = "/".join(str(p) for p in e.absolute_path)
        print(f"  {label}: {path}: {e.message}")
    return not errors


def topic_of(name):
    base = os.path.basename(name)
    for t in TOPICS:
        if base.startswith(t):
            return t
    return None


def main():
    if len(sys.argv) >= 3 and sys.argv[1] == "--topic":
        topic = sys.argv[2]
        v = Draft202012Validator(load_schema(topic))
        ok = True
        for n, line in enumerate(sys.stdin, 1):
            line = line.strip()
            if not line:
                continue
            payload = line.split(" ", 1)[1] if line.startswith(("vda5050/", "uagv/")) else line
            good = check(v, json.loads(payload), f"{topic} #{n}")
            print(f"{topic} #{n}: {'valid' if good else 'INVALID'}", flush=True)
            ok &= good
        return 0 if ok else 1

    folder = sys.argv[1]
    files = sorted(f for f in os.listdir(folder) if f.endswith(".json"))
    if not files:
        print("no messages found in", folder)
        return 1
    ok = True
    validators = {}
    for f in files:
        t = topic_of(f)
        if t is None:
            print(f"skip {f}: unknown topic")
            continue
        validators.setdefault(t, Draft202012Validator(load_schema(t)))
        with open(os.path.join(folder, f), encoding="utf-8") as fh:
            doc = json.load(fh)
        good = check(validators[t], doc, f)
        print(f"{f}: {'valid' if good else 'INVALID'} ({t}.schema)")
        ok &= good
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
