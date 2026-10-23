#!/usr/bin/env python3
"""
regsmodel.py — read a generated (or hand-edited) regs.hpp back into a model
and compare two such models.

The model is what the C++ code can observe, independent of formatting:

    { instance: { register: {"addr": int, "access": str,
                             "fields": {field: (pos, width, owner)}} } }

`owner` is the register named in Field<_Owner, ...>; it must equal the
enclosing register, anything else is a copy-paste bug.

Usage:
    python regsmodel.py OLD.hpp NEW.hpp     # print the semantic difference
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

RE_BLOCK_COMMENT = re.compile(r"/\*.*?\*/", re.S)
RE_TEMPLATE = re.compile(r"^template\s*<\s*uint32_t\s+BASE_ADDR\s*>")
RE_STRUCT = re.compile(r"^struct\s+(\w+)\s*(?:\{|$)")
RE_BASE = re.compile(r"(?:static\s+constexpr|constexpr\s+static)\s+uint32_t\s+(\w+)\s*=\s*([\w]+)\s*;")
RE_REG = re.compile(r"struct\s+(_\w+)\s*:\s*Register\s*<\s*(.+?)\s*,\s*(ReadWrite|ReadOnly|WriteOnly)\s*,\s*_\w+\s*>")
RE_FIELD = re.compile(r"using\s+(\w+)\s*=\s*Field\s*<\s*(_\w+)\s*,\s*(\d+)\s*,\s*(\d+)\s*>")
RE_ALIAS = re.compile(r"^using\s+(\w+)\s*=\s*(\w+)\s*<\s*(0[xX][0-9A-Fa-f]+)\s*>\s*;")

TEMPLATE_BASE = 0  # template registers are stored relative to their instance base


def _num(text: str) -> int:
    return int(text.rstrip("uUlL"), 0)


def parse(path: Path) -> dict:
    src = RE_BLOCK_COMMENT.sub("", path.read_text(encoding="utf-8"))
    templates: dict[str, dict] = {}
    instances: dict[str, dict] = {}

    depth = 0
    pending_template = False
    top: str | None = None          # current top-level struct
    top_is_template = False
    consts: dict[str, int] = {}
    regs: dict[str, dict] = {}
    reg: str | None = None
    reg_depth = 0

    for raw in src.splitlines():
        line = raw.split("//", 1)[0].strip()
        if not line:
            continue

        if depth == 0:
            if RE_TEMPLATE.match(line):
                pending_template = True
            m = RE_STRUCT.match(line)
            if m:
                top, top_is_template = m.group(1), pending_template
                pending_template = False
                consts, regs, reg = {}, {}, None
            m = RE_ALIAS.match(line)
            if m:
                name, tmpl, base = m.group(1), m.group(2), _num(m.group(3))
                body = templates[tmpl]
                instances[name] = {
                    r: {**v, "addr": v["addr"] + base} for r, v in body.items()
                }

        if top is not None and depth >= 1:
            m = RE_BASE.search(line)
            if m:
                consts[m.group(1)] = TEMPLATE_BASE if m.group(2) == "BASE_ADDR" else _num(m.group(2))
            m = RE_REG.search(line)
            if m:
                expr = m.group(2)
                for k, v in consts.items():
                    expr = re.sub(rf"\b{k}\b", str(v), expr)
                expr = re.sub(r"(0[xX][0-9A-Fa-f]+|\d+)[uUlL]+", r"\1", expr)
                reg = m.group(1)[1:]
                regs[reg] = {"addr": eval(expr, {}), "access": m.group(3), "fields": {}}
                reg_depth = depth + 1
            m = RE_FIELD.search(line)
            if m and reg is not None:
                regs[reg]["fields"][m.group(1)] = (int(m.group(3)), int(m.group(4)), m.group(2)[1:])

        depth += line.count("{") - line.count("}")
        # the register body may open on the next line ("struct _X : Register<...>\n{")
        if reg is not None and depth < reg_depth and "}" in line:
            reg = None
        if top is not None and depth == 0 and "}" in line:
            (templates if top_is_template else instances)[top] = regs
            top = None

    return instances


def diff(old: dict, new: dict) -> list[str]:
    out: list[str] = []
    for inst in sorted(old.keys() - new.keys()):
        out.append(f"- {inst}")
    for inst in sorted(new.keys() - old.keys()):
        out.append(f"+ {inst} ({len(new[inst])} regs)")
    for inst in sorted(old.keys() & new.keys()):
        a, b = old[inst], new[inst]
        for r in sorted(a.keys() - b.keys()):
            out.append(f"- {inst}::{r}")
        for r in sorted(b.keys() - a.keys()):
            out.append(f"+ {inst}::{r} @{b[r]['addr']:#010x}")
        for r in sorted(a.keys() & b.keys()):
            ra, rb = a[r], b[r]
            if ra["addr"] != rb["addr"]:
                out.append(f"~ {inst}::{r} addr {ra['addr']:#010x} -> {rb['addr']:#010x}")
            if ra["access"] != rb["access"]:
                out.append(f"~ {inst}::{r} access {ra['access']} -> {rb['access']}")
            fa, fb = ra["fields"], rb["fields"]
            for f in sorted(fa.keys() - fb.keys()):
                out.append(f"- {inst}::{r}::{f} {fa[f][:2]}")
            for f in sorted(fb.keys() - fa.keys()):
                out.append(f"+ {inst}::{r}::{f} {fb[f][:2]}")
            for f in sorted(fa.keys() & fb.keys()):
                if fa[f] != fb[f]:
                    out.append(f"~ {inst}::{r}::{f} {fa[f]} -> {fb[f]}")
    return out


def owner_mismatches(model: dict) -> list[str]:
    return [f"{inst}::{r}::{f} declared as Field<_{own}>"
            for inst, regs in model.items()
            for r, v in regs.items()
            for f, (_, _, own) in v["fields"].items() if own != r]


if __name__ == "__main__":
    if len(sys.argv) == 2:
        m = parse(Path(sys.argv[1]))
        print(f"{len(m)} instances, {sum(len(v) for v in m.values())} registers")
        for line in owner_mismatches(m):
            print("owner mismatch:", line)
    else:
        for line in diff(parse(Path(sys.argv[1])), parse(Path(sys.argv[2]))):
            print(line)
