#!/usr/bin/env python3
"""
Cross-check a generated MCORE register header against the vendor CMSIS device
header (an independent source written by ST from the reference manual).

Compared:
  * peripheral base addresses       (#define X_BASE / #define X ((T *) X_BASE))
  * register offsets                (typedef struct { ... } T_TypeDef layout)
  * bit-field position and width    (#define PFX_REG_FIELD_Pos / _Msk)

Names differ between SVD and CMSIS in places; only names found on both sides
are compared, the rest is counted as "unmatched" (listed with -v).

Usage:
  python check_regs.py --regs ../../mcore/chip/stm32h755/regs.hpp --cmsis ../nvic/stm32h755xx.h [-v]
Exit code 1 when a mismatch is found.
"""
import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path


# ----------------------------------------------------------------------------- MCORE header
WRONG_OWNER = []   # fields declared on one register but bound to another


def parse_mcore(text):
    """-> {instance: {"base": int, "regs": {reg: (offset, {field: (pos, width)})}}}"""
    bodies = {}      # struct/template name -> regs
    plain_base = {}  # plain struct name -> base
    instances = {}   # instance -> (struct name, base)
    cur = None
    reg = None
    for line in text.splitlines():
        m = re.match(r"\s*struct (\w+) \{$", line)
        if m and cur is None:
            cur = m.group(1)
            bodies[cur] = {}
            continue
        m = re.match(r"\s*static constexpr uint32_t \w+_BASE = (0x[0-9A-Fa-f]+);", line)
        if m and cur is not None:
            plain_base[cur] = int(m.group(1), 16)
            continue
        m = re.match(r"\s*struct _(\w+) : Register<\w+ \+ (0x[0-9A-Fa-f]+), \w+, _\w+> \{", line)
        if m and cur is not None:
            reg = m.group(1)
            bodies[cur][reg] = (int(m.group(2), 16), {})
            continue
        m = re.match(r"\s*using (\w+) = Field<_(\w+),\s*(\d+),\s*(\d+)\s*>;", line)
        if m and cur is not None and reg is not None:
            bodies[cur][reg][1][m.group(1)] = (int(m.group(3)), int(m.group(4)))
            if m.group(2) != reg:
                WRONG_OWNER.append(f"{cur}.{reg}.{m.group(1)}: Field<> names register _{m.group(2)}")
            continue
        if re.match(r"\}; // struct (\w+)", line):
            cur = reg = None
            continue
        m = re.match(r"using (\w+) = (\w+)<(0x[0-9A-Fa-f]+)>;", line)
        if m:
            instances[m.group(1)] = (m.group(2), int(m.group(3), 16))
    out = {}
    for name, base in plain_base.items():
        if name not in instances and bodies.get(name):
            out[name] = {"base": base, "regs": bodies[name]}
    for inst, (tname, base) in instances.items():
        if tname in bodies:
            out[inst] = {"base": base, "regs": bodies[tname]}
    return out


# ----------------------------------------------------------------------------- CMSIS header
SIZES = {"uint32_t": 4, "uint16_t": 2, "uint8_t": 1, "int32_t": 4}


def strip_comments(text):
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


def parse_typedefs(code):
    """-> {type: {"size": n, "regs": {name: offset}}} (arrays: NAME[i] entries)"""
    types = {}
    for m in re.finditer(r"typedef struct\s*\{(.*?)\}\s*(\w+)\s*;", code, flags=re.S):
        body, tname = m.group(1), m.group(2)
        if "union" in body or "struct" in body:
            continue
        off, regs, ok = 0, {}, True
        for decl in body.split(";"):
            decl = decl.strip()
            if not decl:
                continue
            dm = re.match(r"(?:__IO|__I|__O|__IM|__OM|__IOM|const|volatile|\s)*\s*(\w+)\s+(\w+)\s*(?:\[\s*(\w+)\s*\])?$",
                          decl)
            if not dm:
                ok = False
                break
            ctype, name, count = dm.group(1), dm.group(2), dm.group(3)
            if ctype in SIZES:
                size = SIZES[ctype]
            elif ctype in types:
                size = types[ctype]["size"]
            else:
                ok = False
                break
            align = min(size, 4)
            off = (off + align - 1) // align * align
            n = int(count, 0) if count else 1
            if ctype in SIZES and not name.upper().startswith("RESERVED"):
                if count:
                    for i in range(n):
                        regs[f"{name}[{i}]"] = off + i * size
                else:
                    regs[name] = off
            off += size * n
        if ok:
            types[tname] = {"size": off, "regs": regs}
    return types


def parse_defines(code):
    defs = {}
    for m in re.finditer(r"^\s*#define\s+(\w+)\s+(.+?)\s*$", code, flags=re.M):
        defs[m.group(1)] = m.group(2)
    return defs


def eval_addr(expr, defs, depth=0):
    if depth > 20:
        raise ValueError(expr)
    expr = re.sub(r"(0x[0-9A-Fa-f]+|\d+)U?L{0,2}\b", r"\1", expr)

    def sub(mm):
        return str(eval_addr(defs[mm.group(0)], defs, depth + 1)) if mm.group(0) in defs else mm.group(0)

    expr = re.sub(r"\b[A-Za-z_]\w*\b", sub, expr)
    if re.search(r"[A-Za-z_]", re.sub(r"0x[0-9A-Fa-f]+", "", expr)):
        raise ValueError(expr)
    return int(eval(expr, {"__builtins__": {}}))


def parse_cmsis(text):
    code = strip_comments(text)
    defs = parse_defines(code)
    types = parse_typedefs(code)
    insts = {}
    for name, val in defs.items():
        m = re.fullmatch(r"\(\(\s*(\w+_TypeDef)\s*\*\s*\)\s*(\w+)\s*\)", val)
        if m and m.group(1) in types:
            try:
                insts[name] = (m.group(1), eval_addr(m.group(2), defs))
            except (ValueError, KeyError, SyntaxError):
                pass
    bits = defaultdict(dict)   # "PFX_REG" -> {field: (pos, width)}
    for name, val in defs.items():
        if not name.endswith("_Pos"):
            continue
        stem = name[:-4]
        msk = defs.get(stem + "_Msk")
        pm = re.fullmatch(r"\(?\s*(\d+)U?\s*\)?", val)
        mm = re.fullmatch(r"\(\s*(0x[0-9A-Fa-f]+)U?L{0,2}\s*<<\s*\w+\s*\)", msk or "")
        if not pm or not mm:
            continue
        pos, mask = int(pm.group(1)), int(mm.group(1), 16)
        if mask == 0 or (mask & (mask + 1)) != 0:
            continue   # not a contiguous field
        bits[stem][None] = None
        bits[stem]["__pos__"] = (pos, mask.bit_length())
    return types, insts, bits


# ----------------------------------------------------------------------------- compare
# SVD peripheral name -> CMSIS instance name
INSTANCE_ALIASES = {
    "Ethernet_MAC": "ETH", "Flash": "FLASH", "DAC": "DAC1", "SWPMI": "SWPMI1",
    "ADC12_Common": "ADC12_COMMON", "ADC3_Common": "ADC3_COMMON",
    "OTG1_HS_GLOBAL": "USB1_OTG_HS", "OTG2_HS_GLOBAL": "USB2_OTG_FS",
    "FMC": "FMC_Bank1_R", "HRTIM_Master": "HRTIM1",
}

# Differences that are not errors of the generated header, with the reason.
KNOWN = {
    "COMP1:base": "CMSIS COMP1 points at CFGR1 (COMP12 common registers are a separate block)",
    r"TIM(1|3|4|6|7|8|9|10|11|12|13|14|15|16|17)\.(CNT\.CNT|ARR\.ARR)":
        "16-bit timer; CMSIS has one TIM_CNT/TIM_ARR definition sized for the 32-bit TIM2/TIM5",
    "EXTI.D3PMR1.MR25": "CMSIS typo: EXTI_D3PMR1_MR25_Pos is 24 (MR25 is bit 25, as in the SVD)",
    "Ethernet_MAC.MACHWF2R.RXCHCNT": "CMSIS typo: RXCHCNT is bits 15:12 (Synopsys MAC_HW_Feature2, like the SVD)",
    "FMC.SDCMR.MRD": "CMSIS mask is 13 bits, RM0399 MRD[13:0] is bits 22:9 (as in the SVD)",
}

# Registers that legitimately share an offset (alternate views of one register)
ALTERNATE_VIEWS = [
    r"CCMR\d_(Input|Output)", r"OTG_(HS|FS)_\w+_(Host|Device)", r"DR_\d\d",
]


def overlaps(ours):
    """Registers of one peripheral at the same offset that are not alternate views."""
    found = []
    for inst, p in ours.items():
        by_off = defaultdict(list)
        for reg, (off, _) in p["regs"].items():
            by_off[off].append(reg)
        for off, regs in by_off.items():
            if len(regs) > 1 and not all(any(re.fullmatch(a, r) for a in ALTERNATE_VIEWS) for r in regs):
                found.append(f"{inst}: {', '.join(regs)} share offset 0x{off:X}")
    return found


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--regs", required=True)
    ap.add_argument("--cmsis", required=True)
    ap.add_argument("-v", action="store_true", help="list unmatched names too")
    args = ap.parse_args()

    ours = parse_mcore(Path(args.regs).read_text(encoding="utf-8"))
    types, insts, bits = parse_cmsis(Path(args.cmsis).read_text(encoding="utf-8", errors="replace"))
    fields = {stem: v["__pos__"] for stem, v in bits.items()}

    problems = []
    stats = defaultdict(int)
    unmatched_periph = []
    known_hits = []

    def report(key, text):
        why = KNOWN.get(key) or next((v for k, v in KNOWN.items() if re.fullmatch(k, key)), None)
        if why:
            known_hits.append(f"{text}  -- {why}")
        else:
            problems.append(text)

    for inst, p in sorted(ours.items()):
        cm = INSTANCE_ALIASES.get(inst, inst)
        if cm not in insts:
            unmatched_periph.append(inst)
            continue
        tname, base = insts[cm]
        stats["peripherals"] += 1
        if base != p["base"]:
            report(f"{inst}:base", f"{inst}: base 0x{p['base']:08X}, CMSIS 0x{base:08X}")
        layout = types[tname]["regs"]
        tstem = re.sub(r"_?(Global|Common)?_?TypeDef$", "", tname)
        prefixes = {tstem, tstem.split("_")[0], re.sub(r"\d+$", "", cm), cm}
        for reg, (off, flds) in sorted(p["regs"].items(), key=lambda kv: kv[1][0]):
            if reg in layout:
                stats["registers"] += 1
                if layout[reg] != off:
                    report(f"{inst}.{reg}", f"{inst}.{reg}: offset 0x{off:X}, CMSIS 0x{layout[reg]:X}")
            else:
                stats["registers unmatched"] += 1
            for fname, (pos, wid) in flds.items():
                hit = None
                for pfx in prefixes:
                    hit = fields.get(f"{pfx}_{reg}_{fname}")
                    if hit:
                        break
                if hit is None:
                    stats["fields unmatched"] += 1
                    continue
                stats["fields"] += 1
                if hit != (pos, wid):
                    report(f"{inst}.{reg}.{fname}", f"{inst}.{reg}.{fname}: [{pos}+{wid}], CMSIS [{hit[0]}+{hit[1]}]")

    problems += overlaps(ours) + WRONG_OWNER
    seen = set()
    for pr in problems:
        key = re.sub(r"^\w+?(\d+)?\.", ".", pr)  # same issue in every instance -> once
        if key in seen:
            continue
        seen.add(key)
        print("MISMATCH", pr)
    for k in known_hits:
        print("known   ", k)
    print(", ".join(f"{k}: {v}" for k, v in stats.items()),
          f"| peripherals not in CMSIS: {len(unmatched_periph)}")
    if args.v:
        print("not in CMSIS:", " ".join(unmatched_periph))
    print(f"{len(problems)} mismatches ({len(seen)} distinct)")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
