#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
# SPDX-License-Identifier: MPL-2.0

set -euo pipefail

MCM_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MCM_SKETCH_DIR="${MCM_ROOT}/src/MCM_Firmware"
MCM_BUILD_DIR="${MCM_ROOT}/build/arduino"
MCM_TOOLS_DIR="${MCM_ROOT}/.tools"
MCM_CLI_LOCAL="${MCM_TOOLS_DIR}/bin/arduino-cli"

export ARDUINO_DIRECTORIES_DATA="${MCM_TOOLS_DIR}/arduino-data"
export ARDUINO_DIRECTORIES_DOWNLOADS="${MCM_TOOLS_DIR}/arduino-downloads"
export ARDUINO_DIRECTORIES_USER="${MCM_TOOLS_DIR}/arduino-user"
export ARDUINO_UPDATER_ENABLE_NOTIFICATION=false

MCM_PACKAGE_URL="${MCM_PACKAGE_URL:-https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json}"
MCM_CORE_VERSION="${MCM_CORE_VERSION:-5.6.0}"

# MCM uses an RP2040 and W25Q16JV 16-Mbit (2-Mbyte) QSPI flash.
MCM_FQBN_DEFAULT="rp2040:rp2040:generic:flash=2097152_0,freq=133,opt=Small,os=none,rtti=Disabled,stackprotect=Disabled,exceptions=Disabled,dbgport=Disabled,dbglvl=None,usbstack=picosdk,boot2=boot2_w25q16jvxq_4_padded_checksum"
MCM_FQBN="${MCM_FQBN:-${MCM_FQBN_DEFAULT}}"

mcm_cli() {
    if [[ -x "${MCM_CLI_LOCAL}" ]]; then
        printf '%s\n' "${MCM_CLI_LOCAL}"
        return 0
    fi

    if command -v arduino-cli >/dev/null 2>&1; then
        command -v arduino-cli
        return 0
    fi

    return 1
}

mcm_require_cli() {
    local cli
    if ! cli="$(mcm_cli)"; then
        echo "arduino-cli is not installed." >&2
        echo "Run ./setup.sh once, then run ./build.sh again." >&2
        exit 1
    fi
    printf '%s\n' "${cli}"
}
