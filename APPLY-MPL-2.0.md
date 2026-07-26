# Apply the MPL-2.0 relicensing overlay

Extract this archive at the root of `mcm-firmware`. It replaces the old GPLv2
`LICENSE`, adds project licensing documentation, adds MPL/SPDX headers to source
files, and installs a CI check for future source files.

```bash
cd ~/GitHub/mcm-firmware
git pull --rebase origin main
unzip -o ~/Downloads/mcm-firmware-mpl-2.0-overlay.zip
python3 tools/check_license_headers.py
git diff --check
git status
git add LICENSE NOTICE README.md CONTRIBUTING.md APPLY-MPL-2.0.md \
        docs/ src/ tools/ .github/
git commit -m "Relicense MCM firmware under MPL-2.0"
git push origin main
```

Before committing, confirm that Roth Amplification Ltd owns the copyright in
all relicensed files or has permission from all contributors. Existing copies
of historical GPLv2 revisions remain licensed under GPLv2.
