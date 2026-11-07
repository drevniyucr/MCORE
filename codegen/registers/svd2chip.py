#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SVD -> MCORE register header generator (peripherals only).

The Cortex-M7 core blocks (SCB, NVIC, MPU, cache, DWT, ITM, SysTick, ...) are
shared by every Cortex-M7 chip and live in mcore/core/cortex_m7_regs.hpp; this
script emits only the vendor peripherals and includes that header.

Output style matches the hand-tuned STM32F767 header:
  * a peripheral nobody derives from     -> struct NAME { ... };
  * a peripheral other blocks derive from -> template <uint32_t BASE_ADDR> struct T { ... };
                                             using NAME = T<base>;  (one alias per instance)

Usage:
  python svd2chip.py --svd xml-svd/STM32H755_CM7.svd --chip STM32H755 \
                     --out ../../mcore/chip/stm32h755/regs.hpp
"""

import argparse
import json
import keyword
import re
import xml.etree.ElementTree as ET
from collections import OrderedDict
from pathlib import Path

CPP_KEYWORDS = {
    "and", "bitand", "bitor", "compl", "not", "or", "xor", "delete", "new",
    "default", "register", "volatile", "const", "struct", "class", "union",
    "template", "typename", "operator", "private", "public", "protected",
    "static", "switch", "case", "this", "true", "false", "int", "char", "bool",
}


def sanitize(name: str) -> str:
    name = re.sub(r"[^A-Za-z0-9_]", "_", name.strip())
    if name and name[0].isdigit():
        name = "_" + name
    if name.lower() in CPP_KEYWORDS or keyword.iskeyword(name):
        name += "_"
    return name


def clean(text):
    return re.sub(r"\s+", " ", text or "").strip()


def parse_int(text: str) -> int:
    text = text.strip().lower()
    if text.startswith("#"):
        return int(text[1:].replace("x", "0"), 2)
    return int(text, 0)


def field_range(f):
    """Return (offset, width) for any of the three SVD bit-range notations."""
    off, wid = f.findtext("bitOffset"), f.findtext("bitWidth")
    if off is not None and wid is not None:
        return parse_int(off), parse_int(wid)
    lsb, msb = f.findtext("lsb"), f.findtext("msb")
    if lsb is not None and msb is not None:
        lo, hi = parse_int(lsb), parse_int(msb)
        return lo, hi - lo + 1
    rng = f.findtext("bitRange")
    if rng:
        hi, lo = (parse_int(x) for x in rng.strip("[]").split(":"))
        return lo, hi - lo + 1
    return None


def access_of(text, default="read-write"):
    text = (text or default).lower()
    if text == "read-only":
        return "ReadOnly"
    if text == "write-only":
        return "WriteOnly"
    return "ReadWrite"


def merged_fields(fields):
    """NAME0..NAMEn single-bit fields on consecutive bits -> extra NAME field.

    Some SVDs (e.g. STM32H7) describe multi-bit fields bit by bit (SADD0..9,
    DEAT0..4); MCORE drivers address them as one field, like the F7 SVD does.
    """
    names = {f[0] for f in fields}
    groups = {}
    for name, off, wid in fields:
        m = re.fullmatch(r"(.*[A-Za-z_])(\d+)", name)
        if m and wid == 1:
            groups.setdefault(m.group(1), []).append((int(m.group(2)), off))
    extra = []
    for prefix, items in groups.items():
        items.sort()
        if prefix in names or len(items) < 2 or items[0][0] != 0:
            continue
        lo = items[0][1]
        if all(idx == i and off == lo + i for i, (idx, off) in enumerate(items)):
            extra.append((prefix, lo, len(items)))
    return extra


def apply_field_fixups(emitted, fix):
    """emitted: [(name, off, wid, comment)] -> corrected list (see fixups/README.md)."""
    if not fix.get("field_array") and not fix.get("fields"):
        return emitted          # keep the SVD order untouched
    arr = fix.get("field_array")
    if arr:
        emitted = []
        for ch in range(arr["count"]):
            for i, prefix in enumerate(arr["names"]):
                idx = arr.get("first", 0) + ch
                emitted.append((f"{prefix}{idx}", ch * len(arr["names"]) + i, 1,
                                f" // channel {idx} (fixup: SVD numbering is wrong)"))
    for name, rng in fix.get("fields", {}).items():
        emitted = [e for e in emitted if e[0] != name]
        if rng is not None:
            emitted.append((name, rng[0], rng[1], " // fixup: corrected against the reference manual / CMSIS"))
    return sorted(emitted, key=lambda e: e[1])


def gen_registers(periph, base_sym, periph_access, fixups=None):
    out = []
    regs = periph.find("registers")
    if regs is None:
        return out
    seen_regs = set()
    for reg in regs.findall("register"):
        rname = sanitize(reg.findtext("name"))
        fix = (fixups or {}).get(rname, {})
        rfix = fix.get("registers", {})
        rname = rfix.get("rename", rname)
        if rname in seen_regs:
            continue
        seen_regs.add(rname)
        offset = parse_int(rfix.get("offset", reg.findtext("addressOffset")))
        rw = access_of(reg.findtext("access"), periph_access)
        desc = clean(reg.findtext("description"))
        if rfix:
            desc = (desc + " " if desc else "") + "(fixup: " + ", ".join(
                f"{k} {v}" for k, v in rfix.items() if k != "_why") + f" - {rfix.get('_why', 'SVD error')})"
        if desc:
            out.append(f"    // {desc}")
        out.append(f"    struct _{rname} : Register<{base_sym} + 0x{offset:X}, {rw}, _{rname}> {{")
        fields = reg.find("fields")
        seen = set()
        emitted = []
        if fields is not None:
            for f in fields.findall("field"):
                fname = sanitize(f.findtext("name"))
                rng = field_range(f)
                if not fname or rng is None or fname in seen:
                    continue
                seen.add(fname)
                off, wid = rng
                if off + wid > 32 or wid == 0:
                    continue
                fdesc = clean(f.findtext("description"))
                emitted.append((fname, off, wid, f" // {fdesc}" if fdesc else ""))
        emitted = apply_field_fixups(emitted, fix)
        seen = {e[0] for e in emitted}
        for fname, off, wid, comment in emitted:
            out.append(f"        using {fname} = Field<_{rname}, {off}, {wid}>;{comment}")
        for name, off, wid in merged_fields([e[:3] for e in emitted]):
            seen.add(name)
            out.append(f"        using {name} = Field<_{rname}, {off}, {wid}>; // merged {name}0..{name}{wid - 1}")
        for alias, target in fix.get("aliases", {}).items():
            if alias not in seen and target in seen:
                out.append(f"        using {alias} = {target}; // MCORE alias (see fixups)")
        out.append("    };")
        out.append("")
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--svd", required=True)
    ap.add_argument("--chip", required=True, help="Chip name used in banners, e.g. STM32H755")
    ap.add_argument("--out", required=True)
    ap.add_argument("--fixups", help="JSON corrections of SVD errors and field aliases, see fixups/README.md")
    args = ap.parse_args()

    fixups = {}
    if args.fixups:
        for kind, entries in json.loads(Path(args.fixups).read_text(encoding="utf-8")).items():
            if kind.startswith("_"):
                continue
            if kind not in ("aliases", "fields", "field_array", "registers"):
                raise SystemExit(f"unknown fixup section {kind}")
            for key, table in entries.items():
                if key.startswith("_"):
                    continue
                periph_name, reg_name = key.split("/")
                fixups.setdefault(periph_name, {}).setdefault(reg_name, {})[kind] = table

    root = ET.parse(args.svd).getroot()
    default_access = root.findtext("access") or "read-write"
    periphs = OrderedDict()
    for p in root.findall("peripherals/peripheral"):
        periphs[sanitize(p.findtext("name"))] = p

    # base peripheral -> list of derived peripheral names
    derived = OrderedDict()
    for name, p in periphs.items():
        src = p.get("derivedFrom")
        if src:
            derived.setdefault(sanitize(src), []).append(name)

    # template names: prefer the SVD groupName when it is unique and free
    group_bases = {}
    for base in derived:
        grp = sanitize(periphs[base].findtext("groupName") or "")
        group_bases.setdefault(grp, []).append(base)
    template_name = {}
    for base in derived:
        grp = sanitize(periphs[base].findtext("groupName") or "")
        if grp and len(group_bases[grp]) == 1 and grp not in periphs:
            template_name[base] = grp
        else:
            template_name[base] = f"{base}_T"

    lines = [
        "#pragma once",
        f"// {args.chip} peripheral registers.",
        f"// GENERATED by codegen/registers/svd2chip.py from {Path(args.svd).name} - do not edit by hand.",
        "",
        '#include "core/cortex_m7_regs.hpp"',
        "",
        "/*-----------------------------------------------------------------------------",
        f"------------------------ {args.chip} Register Description ------------------------",
        "-----------------------------------------------------------------------------*/",
        "",
    ]

    for name, p in periphs.items():
        if p.get("derivedFrom"):
            continue
        base_addr = parse_int(p.findtext("baseAddress"))
        desc = clean(p.findtext("description"))
        p_access = access_of(p.findtext("access"), default_access)
        lines.append("// --------------------------------------------")
        lines.append(f"// {name}: {desc}")
        lines.append(f"// Base address: 0x{base_addr:08X}")
        lines.append("// --------------------------------------------")
        if name in derived:
            tname = template_name[name]
            lines.append("template <uint32_t BASE_ADDR>")
            lines.append(f"struct {tname} {{")
            lines.append(f"    static constexpr uint32_t {tname}_BASE = BASE_ADDR;")
            lines.append("")
            lines += gen_registers(p, f"{tname}_BASE", p_access, fixups.get(name))
            lines.append(f"}}; // struct {tname}")
            lines.append("")
            for inst in [name] + derived[name]:
                ib = parse_int(periphs[inst].findtext("baseAddress"))
                lines.append(f"using {inst} = {tname}<0x{ib:08X}>;")
            lines.append("")
        else:
            lines.append(f"struct {name} {{")
            lines.append(f"    static constexpr uint32_t {name}_BASE = 0x{base_addr:08X};")
            lines.append("")
            lines += gen_registers(p, f"{name}_BASE", p_access, fixups.get(name))
            lines.append(f"}}; // struct {name}")
            lines.append("")

    Path(args.out).parent.mkdir(parents=True, exist_ok=True)
    Path(args.out).write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
    print(f"Generated {args.out}: {sum(1 for p in periphs.values() if not p.get('derivedFrom'))} base "
          f"peripherals, {sum(len(v) for v in derived.values())} derived instances")


if __name__ == "__main__":
    main()
