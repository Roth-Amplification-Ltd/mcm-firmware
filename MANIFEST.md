# MCM Heavy Documentation Overlay Manifest

This repo-root overlay was generated from an audit of `Roth-Amplification-Ltd/mcm-firmware` at commit `0734fb17a53a8457c1db6c18560c552f6e7ac451` and the uploaded KiCad project.

## Scope

- Replaces the short top-level README with a full engineering entry point.
- Adds a complete `docs/` handbook.
- Adds five architectural decision records.
- Adds KiCad-derived encoder/button and SPI hardware maps.
- Adds annotated board images.
- Integrates the coherent six-encoder/six-button snapshot design package as an explicitly not-yet-implemented protocol target.
- Adds source-level explanatory comments to the corrected root transport slice and the historical nested protocol tree.
- Adds a Doxygen configuration.
- Does not intentionally change executable control-flow or packet behavior.

## Validation completed

- All local Markdown links resolve.
- Portable snapshot CRC/packet self-test compiles with `-Wall -Wextra -Werror` and passes.
- Canonical snapshot example decodes successfully.
- Overlay contains 63 files and approximately 3,728 documented source/Markdown lines.

## Critical repository findings documented

- No single canonical build-complete sketch currently exists.
- The newer root sketch references support headers moved into the nested tree.
- The nested tree contains two `.ino` entry points.
- Physical encoder/button scanners are not wired into the active parameter stream.
- The full button snapshot protocol remains a design target.
- Current IRQ deassertion tracks software queue retirement rather than final physical transfer.
