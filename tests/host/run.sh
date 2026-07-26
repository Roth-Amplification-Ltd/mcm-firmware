#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
# SPDX-License-Identifier: MPL-2.0
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.


set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
build_dir="${repo_root}/build/host-tests"
mkdir -p "${build_dir}"

compiler="${CXX:-g++}"

"${compiler}" \
    -std=c++17 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Wconversion \
    -Wsign-conversion \
    -Wshadow \
    -Werror \
    -I"${repo_root}/tests/host/stubs" \
    -I"${repo_root}/src/MCM_Firmware" \
    "${repo_root}/tests/host/test_protocol.cpp" \
    "${repo_root}/src/MCM_Firmware/CommandDispatcher.cpp" \
    "${repo_root}/src/MCM_Firmware/ControlScanner.cpp" \
    "${repo_root}/src/MCM_Firmware/DebouncedButton.cpp" \
    "${repo_root}/src/MCM_Firmware/EncoderPIO.cpp" \
    "${repo_root}/src/MCM_Firmware/McmApplication.cpp" \
    "${repo_root}/src/MCM_Firmware/SnapshotPublisher.cpp" \
    "${repo_root}/src/MCM_Firmware/TransportSPI.cpp" \
    -o "${build_dir}/test_protocol"

"${compiler}" \
    -std=c++17 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Wconversion \
    -Wsign-conversion \
    -Wshadow \
    -Werror \
    -I"${repo_root}/tests/host/stubs" \
    -I"${repo_root}/src/MCM_Firmware" \
    -x c++ \
    -c "${repo_root}/src/MCM_Firmware/MCM_Firmware.ino" \
    -o "${build_dir}/MCM_Firmware.ino.o"


"${build_dir}/test_protocol"

"${compiler}" \
    -std=c++17 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Wconversion \
    -Wsign-conversion \
    -Wshadow \
    -Werror \
    -I"${repo_root}/tests/host/stubs" \
    -I"${repo_root}/src/MCM_Firmware" \
    -x c++ \
    -c "${repo_root}/src/MCM_Firmware/MCM_Firmware.ino" \
    -o "${build_dir}/MCM_Firmware.ino.o"

