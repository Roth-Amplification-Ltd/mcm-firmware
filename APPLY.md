# Apply the MCM Heavy Documentation Overlay

This archive is laid out from the repository root. Extract it directly inside your local `mcm-firmware` checkout.

```bash
cd ~/GitHub/mcm-firmware
unzip -o ~/Downloads/mcm-firmware-heavy-documentation-overlay.zip

git status
git diff --stat
git diff -- README.md docs/ CONTRIBUTING.md Doxyfile src/

git add README.md CONTRIBUTING.md Doxyfile docs/ src/
git commit -m "Heavily document MCM firmware architecture and protocol"
git push origin main
```

The overlay changes comments and documentation only. It does not intentionally change executable firmware behavior. The documentation does, however, call out structural and implementation problems discovered in the current repository.
