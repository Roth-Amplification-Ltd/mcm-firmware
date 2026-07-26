# ADR-0006: Firmware is licensed under MPL-2.0

- **Status:** Accepted
- **Date:** 2026-07-25

## Context

MCM is intended to be adopted, modified, manufactured, and integrated by other
pedal builders. The project wants modifications to its own firmware files to
remain open without requiring every separate file in a commercial product to
use the same license.

The repository previously carried GNU GPL version 2, which applies copyleft to
the combined program more broadly than the platform intends.

## Decision

Current and future MCM firmware is licensed under the Mozilla Public License
2.0 (`MPL-2.0`). Source files carry SPDX and standard MPL notices. The project
does not use the Exhibit B incompatible-secondary-license notice.

## Consequences

- Modified MPL-covered files remain MPL-2.0 when distributed.
- Separate files in a Larger Work may use other licenses.
- Commercial use and commercial distribution are permitted.
- MPL-covered source must remain available when executable forms are
  distributed.
- Contributor patent grants and notice-preservation requirements apply.
- Historic GPLv2 releases remain available under the license under which they
  were distributed.
- Hardware design licensing remains a separate decision.
