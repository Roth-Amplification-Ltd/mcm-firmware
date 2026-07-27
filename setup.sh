#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
# SPDX-License-Identifier: MPL-2.0

set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${ROOT}/tools/arduino-common.sh"

ARDUINO_CLI_VERSION="${ARDUINO_CLI_VERSION:-1.5.1}"

mkdir -p "${MCM_TOOLS_DIR}/bin" \
         "${ARDUINO_DIRECTORIES_DATA}" \
         "${ARDUINO_DIRECTORIES_DOWNLOADS}" \
         "${ARDUINO_DIRECTORIES_USER}"

if [[ ! -x "${MCM_CLI_LOCAL}" ]]; then
    if ! command -v curl >/dev/null 2>&1; then
        echo "curl is required to install Arduino CLI." >&2
        exit 1
    fi

    echo "Installing Arduino CLI ${ARDUINO_CLI_VERSION} locally..."
    curl -fsSL \
        https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh |
        BINDIR="${MCM_TOOLS_DIR}/bin" sh -s "${ARDUINO_CLI_VERSION}"
fi

CLI="$(mcm_require_cli)"

echo "Installing Arduino-Pico core ${MCM_CORE_VERSION}..."
"${CLI}" core update-index --additional-urls "${MCM_PACKAGE_URL}"
"${CLI}" core install "rp2040:rp2040@${MCM_CORE_VERSION}" \
    --additional-urls "${MCM_PACKAGE_URL}"

echo
echo "Toolchain ready:"
"${CLI}" version
"${CLI}" core list
echo
echo "Build with: ./build.sh"
