#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
# SPDX-License-Identifier: MPL-2.0
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.


set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${repo_root}"

archive_root="archive/pre-misra-canonicalization"
mkdir -p "${archive_root}/src-root"

move_preserving_git() {
    local source_path="$1"
    local destination_path="$2"

    if [[ ! -e "${source_path}" ]]; then
        return 0
    fi

    mkdir -p "$(dirname "${destination_path}")"

    if git ls-files --error-unmatch "${source_path}" >/dev/null 2>&1; then
        git mv "${source_path}" "${destination_path}"
    else
        mv "${source_path}" "${destination_path}"
    fi
}

move_preserving_git "src/HardwareConfig.h" \
    "${archive_root}/src-root/HardwareConfig.h"
move_preserving_git "src/SpiSlaveM0.pio" \
    "${archive_root}/src-root/SpiSlaveM0.pio"
move_preserving_git "src/TransportSPI.h" \
    "${archive_root}/src-root/TransportSPI.h"
move_preserving_git "src/TransportSPI.cpp" \
    "${archive_root}/src-root/TransportSPI.cpp"
move_preserving_git "src/Modular_Control_Module_PIO_SPI.ino" \
    "${archive_root}/src-root/Modular_Control_Module_PIO_SPI.ino"
move_preserving_git "src/Modular_Control_Module_PIO_SPI_LAYER_A_RX" \
    "${archive_root}/Modular_Control_Module_PIO_SPI_LAYER_A_RX"

printf '%s\n' \
    "Canonical MCM sketch: src/MCM_Firmware/MCM_Firmware.ino" \
    "Previous source trees: ${archive_root}/"
