#!/usr/bin/env python3
"""
regsgen.py — generate mcore/core/regs.hpp from three sources:

  1. the vendor SVD (peripherals),
  2. ARM core_reg XML files (Cortex-M core blocks: SCB, NVIC, MPU, cache...),
     which the SVD format deliberately leaves out,
  3. patches.toml — hand-written fixes for what both of them miss or get wrong.

Peripherals that share one register layout (derivedFrom in the SVD, or simply
identical, like GPIOA/GPIOB/GPIOD) become one `template <uint32_t BASE_ADDR>`
plus a `using` per instance; everything else is a plain struct.

Usage (from the repository root):
    python codegen/registers/regsgen.py            # write mcore/core/regs.hpp
    python codegen/registers/regsgen.py --check    # exit 1 if regs.hpp is stale
    python codegen/registers/regsgen.py --diff     # semantic diff vs regs.hpp
"""
from __future__ import annotations

import argparse
import copy
import re
import sys
import tomllib
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
import regsmodel  # noqa: E402

SVD = HERE / "xml-svd" / "STM32F767.svd"
CORE_XML = [HERE / "xml-svd" / n for n in (
    "system_registers_m7.xml", "nvic_registers_v7m.xml",
    "cache_registers_v7em.xml", "mpu_registers_v7m.xml")]
PATCHES = HERE / "patches.toml"
OUT = ROOT / "mcore" / "core" / "regs.hpp"

ARM_NS = {"arm": "http://www.arm.com/core_reg"}
ACCESS = {"read-only": "ReadOnly", "write-only": "WriteOnly",
          "RO": "ReadOnly", "WO": "WriteOnly", "RW": "ReadWrite",
          "ro": "ReadOnly", "wo": "WriteOnly", "rw": "ReadWrite"}


# --------------------------------------------------------------------------
# Model
# --------------------------------------------------------------------------
@dataclass
class Field_:
    name: str
    pos: int
    width: int
    desc: str = ""


@dataclass
class Reg:
    name: str
    offset: int          # relative to the peripheral base (absolute for core blocks)
    access: str          # ReadWrite / ReadOnly / WriteOnly
    desc: str = ""
    fields: list[Field_] = field(default_factory=list)


@dataclass
class Periph:
    name: str
    base: int            # 0 for core blocks: their register offsets are absolute
    desc: str = ""
    group: str = ""
    derived_from: str | None = None
    regs: list[Reg] = field(default_factory=list)
    core: bool = False

    def reg(self, name: str) -> Reg:
        for r in self.regs:
            if r.name == name:
                return r
        raise KeyError(f"{self.name} has no register {name}")

    def signature(self) -> tuple:
        return tuple(sorted(
            (r.name, r.offset, r.access, tuple(sorted((f.name, f.pos, f.width) for f in r.fields)))
            for r in self.regs))


def clean(text: str | None) -> str:
    return re.sub(r"\s+", " ", text or "").strip()


def ident(name: str) -> str:
    return re.sub(r"[^\w]", "_", name)


# --------------------------------------------------------------------------
# Readers
# --------------------------------------------------------------------------
def read_svd(path: Path) -> list[Periph]:
    root = ET.parse(path).getroot()
    default_access = root.findtext("access") or "read-write"
    out = []
    for p in root.findall("peripherals/peripheral"):
        per = Periph(name=ident(p.findtext("name")), base=int(p.findtext("baseAddress"), 0),
                     desc=clean(p.findtext("description")), group=clean(p.findtext("groupName")),
                     derived_from=p.get("derivedFrom"))
        p_access = p.findtext("access") or default_access
        for r in p.findall("registers/register"):
            access = ACCESS.get(r.findtext("access") or p_access, "ReadWrite")
            reg = Reg(name=ident(r.findtext("name")), offset=int(r.findtext("addressOffset"), 0),
                      access=access, desc=clean(r.findtext("description")))
            for f in r.findall("fields/field"):
                if f.findtext("bitOffset") is not None:
                    pos, width = int(f.findtext("bitOffset")), int(f.findtext("bitWidth"))
                elif f.findtext("bitRange") is not None:
                    hi, lo = map(int, f.findtext("bitRange").strip("[]").split(":"))
                    pos, width = lo, hi - lo + 1
                else:
                    pos = int(f.findtext("lsb"))
                    width = int(f.findtext("msb")) - pos + 1
                reg.fields.append(Field_(ident(f.findtext("name")), pos, width, clean(f.findtext("description"))))
            per.regs.append(reg)
        out.append(per)
    return out


def read_core_xml(path: Path) -> list[Periph]:
    root = ET.parse(path).getroot()
    out = []
    for p in root.findall("arm:peripheral", ARM_NS):
        per = Periph(name=ident(p.get("name")), base=0, core=True,
                     desc=clean(p.findtext("arm:description", namespaces=ARM_NS)))
        for r in p.findall("arm:register", ARM_NS):
            reg = Reg(name=ident(r.get("name")), offset=int(r.get("offset"), 0),
                      access=ACCESS.get(r.get("access", "RW"), "ReadWrite"),
                      desc=clean(r.findtext("arm:description", namespaces=ARM_NS)))
            for bf in r.findall("arm:bitField", ARM_NS):
                bits = (bf.findtext("arm:definition", namespaces=ARM_NS) or "").strip()
                if not (bits.startswith("[") and bits.endswith("]")):
                    continue
                hi, _, lo = bits[1:-1].partition(":")
                hi, lo = int(hi), int(lo or hi)
                reg.fields.append(Field_(ident(bf.get("name")), lo, hi - lo + 1,
                                         clean(bf.findtext("arm:description", namespaces=ARM_NS))))
            per.regs.append(reg)
        out.append(per)
    return out


# --------------------------------------------------------------------------
# Patches
# --------------------------------------------------------------------------
def _fields(spec: list) -> list[Field_]:
    return [Field_(f[0], f[1], f[2], f[3] if len(f) > 3 else "") for f in spec]


def _reg(spec: dict) -> Reg:
    return Reg(name=spec["name"], offset=spec.get("offset", spec.get("addr", 0)),
               access=ACCESS[spec.get("access", "rw")], desc=spec.get("description", ""),
               fields=_fields(spec.get("fields", [])))


def apply_patches(periphs: list[Periph], patches: dict) -> list[Periph]:
    by_name = {p.name: p for p in periphs}

    def get(name: str) -> Periph:
        if name not in by_name:
            raise KeyError(f"patch targets unknown peripheral {name}")
        p = by_name[name]
        if p.derived_from and not p.regs:
            raise KeyError(f"patch targets {name}, which is derivedFrom {p.derived_from}: patch the base")
        return p

    for old, new in patches.get("rename", {}).items():
        p = get(old)
        p.name = new
        by_name[new] = by_name.pop(old)

    for spec in patches.get("add_peripheral", []):
        p = Periph(name=spec["name"], base=spec.get("base", 0), desc=spec.get("description", ""),
                   core=spec.get("core", False), regs=[_reg(r) for r in spec.get("register", [])])
        after = spec.get("after")
        idx = periphs.index(by_name[after]) + 1 if after else len(periphs)
        periphs.insert(idx, p)
        by_name[p.name] = p

    for spec in patches.get("add_register", []):
        p = get(spec["peripheral"])
        new = _reg(spec)
        if any(r.name == new.name for r in p.regs):
            raise ValueError(f"{p.name}::{new.name} already exists — use add_field / set_access")
        p.regs.append(new)
        p.regs.sort(key=lambda r: r.offset)

    for spec in patches.get("add_field", []):
        for pname in spec.get("peripherals", [spec.get("peripheral")]):
            reg = get(pname).reg(spec["register"])
            for f in _fields(spec["fields"]):
                if any(x.name == f.name for x in reg.fields):
                    raise ValueError(f"{pname}::{reg.name}::{f.name} already exists")
                reg.fields.append(f)

    for spec in patches.get("set_fields", []):
        get(spec["peripheral"]).reg(spec["register"]).fields = _fields(spec["fields"])

    for spec in patches.get("merge_field", []):
        reg = get(spec["peripheral"]).reg(spec["register"])
        parts = [next((f for f in reg.fields if f.name == n), None) for n in spec["parts"]]
        if None in parts:
            raise KeyError(f"{spec['peripheral']}::{reg.name}: missing one of {spec['parts']}")
        parts.sort(key=lambda f: f.pos)
        for a, b in zip(parts, parts[1:]):
            if a.pos + a.width != b.pos:
                raise ValueError(f"{spec['peripheral']}::{reg.name}: {spec['parts']} are not contiguous")
        merged = Field_(spec["name"], parts[0].pos, sum(f.width for f in parts),
                        spec.get("description", parts[0].desc))
        at = reg.fields.index(parts[0])
        reg.fields = [f for f in reg.fields if f not in parts]
        reg.fields.insert(min(at, len(reg.fields)), merged)

    for spec in patches.get("rename_field", []):
        for pname in spec.get("peripherals", [spec.get("peripheral")]):
            reg = get(pname).reg(spec["register"])
            # "pos" picks one of several same-named fields (source typos)
            f = next((f for f in reg.fields if f.name == spec["from"]
                      and spec.get("pos", f.pos) == f.pos), None)
            if f is None:
                raise KeyError(f"{pname}::{reg.name} has no field {spec['from']}")
            f.name = spec["to"]

    for spec in patches.get("set_access", []):
        get(spec["peripheral"]).reg(spec["register"]).access = ACCESS[spec["access"]]

    omit = set(patches.get("omit", []))
    return [p for p in periphs if p.name not in omit]


def validate(periphs: list[Periph]) -> None:
    """Catch source errors that would otherwise surface as C++ redefinitions or
    silent overlaps: duplicate names, fields past bit 31."""
    errors = []
    for p in periphs:
        seen_regs = set()
        for r in p.regs:
            if r.name in seen_regs:
                errors.append(f"{p.name}: duplicate register {r.name}")
            seen_regs.add(r.name)
            seen = set()
            for f in r.fields:
                if f.name in seen:
                    errors.append(f"{p.name}::{r.name}: duplicate field {f.name} - add a rename_field patch")
                if f.pos + f.width > 32:
                    errors.append(f"{p.name}::{r.name}::{f.name}: bits past 31")
                seen.add(f.name)
    if errors:
        raise SystemExit("regsgen: invalid model\n  " + "\n  ".join(errors))


def expand_derived(periphs: list[Periph]) -> None:
    by_name = {p.name: p for p in periphs}
    for p in periphs:
        if p.derived_from:
            base = by_name[p.derived_from]
            p.regs = copy.deepcopy(base.regs)
            p.group = p.group or base.group
            p.desc = p.desc or base.desc


# --------------------------------------------------------------------------
# Emitter
# --------------------------------------------------------------------------
def hx(v: int) -> str:
    return f"0x{v:X}"


def emit_regs(p: Periph, base_const: str | None) -> list[str]:
    out = []
    for r in p.regs:
        addr = f"{base_const} + {hx(r.offset)}" if base_const else f"0x{r.offset:08X}"
        if r.desc:
            out.append(f"    // {r.name}: {r.desc}")
        out.append(f"    struct _{r.name} : Register<{addr}, {r.access}, _{r.name}> {{")
        for f in r.fields:
            bits = f"[{f.pos + f.width - 1}:{f.pos}]" if f.width > 1 else f"[{f.pos}]"
            tail = f" // {bits} {f.desc}" if f.desc else f" // {bits}"
            out.append(f"        using {f.name} = Field<_{r.name}, {f.pos}, {f.width}>;{tail}")
        out.append("    };")
        out.append("")
    return out


def emit(periphs: list[Periph]) -> str:
    # Families: same register layout -> one template. SVD order is kept; the
    # template is emitted where its first member appears.
    families: dict[tuple, list[Periph]] = {}
    for p in periphs:
        if not p.core:
            families.setdefault(p.signature(), []).append(p)
    multi = [m for m in families.values() if len(m) > 1]
    group_count: dict[str, int] = {}
    for m in multi:
        g = next((x.group for x in m if x.group), "")
        group_count[g] = group_count.get(g, 0) + 1
    names = {p.name for p in periphs}

    def template_name(members: list[Periph]) -> tuple[str, str]:
        g = next((x.group for x in members if x.group), "")
        root = members[0].derived_from or members[0].name
        if g and group_count[g] == 1 and g not in names:
            return g, f"{g}x_BASE"
        return f"{root}_Regs", f"{g or root}x_BASE"

    lines = [
        "#pragma once",
        '#include "core/utils.hpp"',
        "",
        "// GENERATED by codegen/registers/regsgen.py — do not edit by hand.",
        "// Sources: the vendor SVD, ARM core_reg XML for the Cortex-M7 blocks, and",
        "// codegen/registers/patches.toml for everything those two miss or get wrong.",
        "",
        "// Host builds: mingw's _mingw.h defines UNALIGNED, which collides with the",
        "// SCB CFSR field name below. Hide the macro while this header is parsed.",
        '#pragma push_macro("UNALIGNED")',
        "#undef UNALIGNED",
        "",
    ]
    emitted: set[int] = set()
    for p in periphs:
        if id(p) in emitted:
            continue
        members = families.get(p.signature(), [p]) if not p.core else [p]
        lines.append("// " + "-" * 76)
        if len(members) == 1:
            lines.append(f"// {p.name}: {p.desc}")
            lines.append("// " + "-" * 76)
            if p.core:
                lines.append(f"struct {p.name} {{")
                lines += [""] + emit_regs(p, None)
            else:
                const = f"{p.name}_BASE"
                lines.append(f"struct {p.name} {{")
                lines.append(f"    static constexpr uint32_t {const} = {hx(p.base)};")
                lines += [""] + emit_regs(p, const)
            lines.append(f"}}; // struct {p.name}")
            lines.append("")
            emitted.add(id(p))
            continue

        tname, const = template_name(members)
        lines.append(f"// {tname}: {p.desc}")
        lines.append(f"// Instances: {', '.join(m.name for m in members)}")
        lines.append("// " + "-" * 76)
        lines.append("template <uint32_t BASE_ADDR>")
        lines.append(f"struct {tname} {{")
        lines.append(f"    static constexpr uint32_t {const} = BASE_ADDR;")
        lines += [""] + emit_regs(p, const)
        lines.append(f"}}; // struct {tname}")
        lines.append("")
        for m in members:
            lines.append(f"using {m.name} = {tname}<{hx(m.base)}>;")
            emitted.add(id(m))
        lines.append("")

    lines.append('#pragma pop_macro("UNALIGNED")')
    return "\n".join(lines) + "\n"


# --------------------------------------------------------------------------
def build(svd: Path = SVD, patches_path: Path = PATCHES) -> str:
    patches = tomllib.loads(patches_path.read_text(encoding="utf-8"))
    periphs = [p for x in CORE_XML for p in read_core_xml(x)] + read_svd(svd)
    periphs = apply_patches(periphs, patches)
    expand_derived(periphs)
    validate(periphs)
    return emit(periphs)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--check", action="store_true", help="fail if regs.hpp differs from the generated text")
    ap.add_argument("--diff", action="store_true", help="semantic diff of regs.hpp -> generated")
    ap.add_argument("-o", "--out", type=Path, default=OUT)
    ap.add_argument("--svd", type=Path, default=SVD, help="vendor SVD (another STM32: its own SVD)")
    ap.add_argument("--patches", type=Path, default=PATCHES, help="patch file for that SVD")
    args = ap.parse_args()

    text = build(args.svd, args.patches)
    if args.check:
        stale = not args.out.exists() or args.out.read_text(encoding="utf-8") != text
        print(f"{args.out}: {'STALE — run regsgen.py' if stale else 'up to date'}")
        return 1 if stale else 0
    if args.diff:
        tmp = args.out.with_suffix(".generated.hpp")
        tmp.write_text(text, encoding="utf-8", newline="\n")
        for line in regsmodel.diff(regsmodel.parse(args.out), regsmodel.parse(tmp)):
            print(line)
        tmp.unlink()
        return 0
    args.out.write_text(text, encoding="utf-8", newline="\n")
    model = regsmodel.parse(args.out)
    print(f"wrote {args.out}: {len(model)} instances, {sum(len(v) for v in model.values())} registers")
    for line in regsmodel.owner_mismatches(model):
        print("owner mismatch:", line)
    return 0


if __name__ == "__main__":
    sys.exit(main())
