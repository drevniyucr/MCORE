#!/usr/bin/env python3
"""Merge the compile databases of all builds into one for clangd.

MCORE has several independent CMake builds (projects/stm32h755 with the CM7
and CM4 images, projects/stm32f767, tests). clangd reads one database, so this
writes build-index/compile_commands.json with the entries of all of them.
A file built by several of them (mcore/net/tcp.cpp: CM4, F767, host tests)
keeps the flags of the first build in BUILDS: the STM32H755 is the main target.

Usage (from the repository root, after configuring the builds):
    python cmake/compile_db.py
Missing builds are skipped. clangd reloads the database when it changes.
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BUILDS = ["build-h7", "build-f767", "build-tests"]
OUT = ROOT / "build-index" / "compile_commands.json"


def main():
    merged, seen, used = [], set(), []
    for build in BUILDS:
        db = ROOT / build / "compile_commands.json"
        if not db.is_file():
            continue
        used.append(build)
        for entry in json.loads(db.read_text(encoding="utf-8")):
            key = str(Path(entry["directory"], entry["file"]).resolve()).lower()
            if key in seen:
                continue
            seen.add(key)
            merged.append(entry)
    if not merged:
        print("no compile_commands.json found: configure a build first "
              "(cmake -S projects/stm32h755 -B build-h7, ...)", file=sys.stderr)
        return 1
    OUT.parent.mkdir(exist_ok=True)
    OUT.write_text(json.dumps(merged, indent=1), encoding="utf-8")
    print(f"{OUT.relative_to(ROOT)}: {len(merged)} files from {', '.join(used)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
