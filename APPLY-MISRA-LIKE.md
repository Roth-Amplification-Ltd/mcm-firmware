# Apply the MCM deterministic/MISRA-like hardening overlay

This overlay creates a single canonical Arduino-Pico sketch in
`src/MCM_Firmware/`, adds a documented MISRA C++:2023-inspired native-code
profile, installs host-side protocol tests and CI checks, and provides a safe
layout migration script for the older source trees.

This is **not** a formal claim of MISRA C++:2023 compliance. Formal compliance
requires a licensed guideline set, a qualified/configured analysis tool,
recorded rule classifications, deviation approvals, adopted-code evidence, and
a compliance summary. This pass establishes the code structure and engineering
discipline needed to pursue that claim honestly.

## Apply

```bash
cd ~/GitHub/mcm-firmware
git pull --rebase origin main
unzip -o ~/Downloads/mcm-firmware-misra-like-overlay.zip

# Move the superseded root and historical source trees into archive/ while
# preserving Git history. The new canonical sketch remains in src/MCM_Firmware/.
bash tools/apply_misra_like_layout.sh

python3 tools/check_license_headers.py
python3 tools/check_misra_like.py
bash tests/host/run.sh
git diff --check
git status
```

Review the changes, then commit:

```bash
git add README.md docs/ src/MCM_Firmware/ archive/ tools/ tests/ .github/
git commit -m "Harden MCM firmware with deterministic MISRA-like profile"
git push origin main
```

## Arduino build target

Open this sketch, not the archived development folders:

```text
src/MCM_Firmware/MCM_Firmware.ino
```

The overlay does not alter the MPL-2.0 project license.
