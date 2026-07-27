#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
# SPDX-License-Identifier: MPL-2.0

set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${ROOT}/tools/arduino-common.sh"

CLI="$(mcm_require_cli)"

required=(
    "${MCM_SKETCH_DIR}/MCM_Firmware.ino"
    "${MCM_SKETCH_DIR}/EncoderPIO.pio.h"
    "${MCM_SKETCH_DIR}/SpiSlaveM0.pio.h"
)

for path in "${required[@]}"; do
    if [[ ! -f "${path}" ]]; then
        echo "Missing required build input: ${path}" >&2
        exit 1
    fi
done

if ! "${CLI}" core list | grep -q '^rp2040:rp2040'; then
    echo "Arduino-Pico core is not installed in the project tool directory." >&2
    echo "Run ./setup.sh once." >&2
    exit 1
fi

if [[ -f "${ROOT}/tools/check_license_headers.py" ]]; then
    python3 "${ROOT}/tools/check_license_headers.py"
fi

if [[ -f "${ROOT}/tools/check_misra_like.py" ]]; then
    python3 "${ROOT}/tools/check_misra_like.py"
fi

if [[ -x "${ROOT}/tests/host/run.sh" ]]; then
    "${ROOT}/tests/host/run.sh"
fi

rm -rf "${MCM_BUILD_DIR}"
mkdir -p "${MCM_BUILD_DIR}"

echo
echo "Building MCM firmware"
echo "FQBN: ${MCM_FQBN}"
echo

"${CLI}" compile \
    --fqbn "${MCM_FQBN}" \
    --warnings all \
    --clean \
    --jobs 0 \
    --output-dir "${MCM_BUILD_DIR}" \
    "${MCM_SKETCH_DIR}"

UF2="$(find "${MCM_BUILD_DIR}" -maxdepth 1 -type f -name '*.uf2' -print -quit)"
if [[ -z "${UF2}" ]]; then
    echo "Build completed but no UF2 was produced." >&2
    exit 1
fi

cp -f "${UF2}" "${MCM_BUILD_DIR}/mcm-firmware.uf2"

echo
echo "Build passed."
echo "UF2: ${MCM_BUILD_DIR}/mcm-firmware.uf2"
sha256sum "${MCM_BUILD_DIR}/mcm-firmware.uf2"
