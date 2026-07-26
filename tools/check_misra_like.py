# SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
# SPDX-License-Identifier: MPL-2.0
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.


"""Enforce the lightweight MCM native-code profile.

This is deliberately not a formal MISRA analyzer. It catches project decisions
that are cheap, objective, and valuable to enforce continuously.
"""

from __future__ import annotations

import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE_ROOT = ROOT / "src" / "MCM_Firmware"

CPP_SUFFIXES = {".h", ".hpp", ".c", ".cpp", ".ino"}
ALL_SUFFIXES = CPP_SUFFIXES | {".pio"}

FORBIDDEN = {
    r"\bnew\b": "dynamic allocation operator new",
    r"\bdelete\b": "dynamic allocation operator delete",
    r"\bmalloc\s*\(": "malloc",
    r"\bcalloc\s*\(": "calloc",
    r"\brealloc\s*\(": "realloc",
    r"\bfree\s*\(": "free",
    r"\bString\b": "Arduino String",
    r"std::vector\b": "dynamic STL vector",
    r"std::string\b": "dynamic STL string",
    r"std::map\b": "dynamic STL map",
    r"std::unordered_": "dynamic STL unordered container",
    r"\bthrow\b": "exception throw",
    r"\bcatch\s*\(": "exception catch",
    r"\bdynamic_cast\s*<": "RTTI dynamic_cast",
    r"\bgoto\b": "goto",
    r"\(void\)\s*[A-Za-z_]": "ignored return value cast",
}


def strip_comments_and_literals(text: str) -> str:
    pattern = re.compile(
        r'("(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*.*?\*/)',
        re.DOTALL,
    )

    def replace(match: re.Match[str]) -> str:
        token = match.group(0)
        return "\n" * token.count("\n")

    return pattern.sub(replace, text)


def check_file(path: pathlib.Path) -> list[str]:
    errors: list[str] = []
    text = path.read_text(encoding="utf-8")
    relative = path.relative_to(ROOT)

    if "SPDX-License-Identifier: MPL-2.0" not in text:
        errors.append(f"{relative}: missing MPL-2.0 SPDX header")

    for number, line in enumerate(text.splitlines(), start=1):
        if "\t" in line:
            errors.append(f"{relative}:{number}: tab character")
        if line.rstrip() != line:
            errors.append(f"{relative}:{number}: trailing whitespace")
        if len(line) > 120:
            errors.append(f"{relative}:{number}: line exceeds 120 characters")

    if path.suffix in CPP_SUFFIXES:
        code = strip_comments_and_literals(text)
        for expression, description in FORBIDDEN.items():
            for match in re.finditer(expression, code):
                line = code.count("\n", 0, match.start()) + 1
                errors.append(f"{relative}:{line}: forbidden {description}")

        for match in re.finditer(r"\benum\s+(?!class\b|struct\b)", code):
            line = code.count("\n", 0, match.start()) + 1
            errors.append(f"{relative}:{line}: use a scoped enum class")

    return errors


def main() -> int:
    if not SOURCE_ROOT.is_dir():
        print(f"ERROR: canonical source directory missing: {SOURCE_ROOT}")
        return 1

    paths = sorted(
        path
        for path in SOURCE_ROOT.rglob("*")
        if path.is_file() and path.suffix in ALL_SUFFIXES
    )

    errors: list[str] = []
    for path in paths:
        errors.extend(check_file(path))

    required_assert_files = {
        SOURCE_ROOT / "HardwareConfig.h",
        SOURCE_ROOT / "ControlMap.h",
        SOURCE_ROOT / "TransportProtocol.h",
    }
    for path in required_assert_files:
        if "static_assert" not in path.read_text(encoding="utf-8"):
            errors.append(f"{path.relative_to(ROOT)}: expected compile-time assertions")

    if errors:
        print("MCM MISRA-like profile check FAILED")
        for error in errors:
            print(f" - {error}")
        return 1

    print(f"MCM MISRA-like profile check passed for {len(paths)} native source files.")
    print("This result is a project-profile check, not a formal MISRA compliance claim.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
