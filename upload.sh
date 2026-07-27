#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
# SPDX-License-Identifier: MPL-2.0

set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${ROOT}/tools/arduino-common.sh"

if [[ $# -ne 1 ]]; then
    echo "Usage: ./upload.sh <port>" >&2
    echo "Example: ./upload.sh /dev/ttyACM0" >&2
    exit 2
fi

PORT="$1"
CLI="$(mcm_require_cli)"

"${ROOT}/build.sh"

"${CLI}" upload \
    --fqbn "${MCM_FQBN}" \
    --port "${PORT}" \
    --input-dir "${MCM_BUILD_DIR}" \
    "${MCM_SKETCH_DIR}"
