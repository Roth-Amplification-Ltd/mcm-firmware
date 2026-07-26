#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
# SPDX-License-Identifier: MPL-2.0
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

"""Fail when an MCM source file lacks its MPL-2.0 SPDX identifier.

This deliberately checks source-like files only. Binary assets and prose are
covered by the repository-level licensing declaration unless they carry a more
specific notice.
"""

from __future__ import annotations

import sys
from pathlib import Path

REQUIRED = "SPDX-License-Identifier: MPL-2.0"
SOURCE_SUFFIXES = {
    ".c", ".cc", ".cpp", ".h", ".hh", ".hpp", ".ino", ".pio",
    ".py", ".sh", ".yml", ".yaml", ".svg",
}
SCAN_ROOTS = ("src", "examples", "tests", "tools", ".github")
IGNORED_PARTS = {".git", "build", "dist", "vendor", "third_party", "generated"}


def candidates(repo: Path):
    for relative_root in SCAN_ROOTS:
        base = repo / relative_root
        if not base.exists():
            continue
        for path in base.rglob("*"):
            if not path.is_file() or path.suffix.lower() not in SOURCE_SUFFIXES:
                continue
            if any(part in IGNORED_PARTS for part in path.parts):
                continue
            yield path


def main() -> int:
    repo = Path(__file__).resolve().parents[1]
    missing: list[Path] = []
    unreadable: list[Path] = []

    for path in candidates(repo):
        try:
            head = path.read_text(encoding="utf-8")[:4096]
        except UnicodeDecodeError:
            unreadable.append(path)
            continue
        if REQUIRED not in head:
            missing.append(path)

    if unreadable:
        print("ERROR: source-like files were not valid UTF-8:", file=sys.stderr)
        for path in unreadable:
            print(f"  {path.relative_to(repo)}", file=sys.stderr)

    if missing:
        print("ERROR: files missing MPL-2.0 SPDX headers:", file=sys.stderr)
        for path in missing:
            print(f"  {path.relative_to(repo)}", file=sys.stderr)

    if unreadable or missing:
        return 1

    checked = sum(1 for _ in candidates(repo))
    print(f"License header check passed for {checked} source files.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
