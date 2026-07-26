# MCM Firmware Licensing

## Governing license

The current MCM firmware repository is licensed under the **Mozilla Public
License 2.0**, identified by SPDX as **`MPL-2.0`**.

The complete license text is in [`../LICENSE`](../LICENSE). Source files use
both an SPDX identifier and Mozilla's standard Source Code Form notice.

## Why MPL-2.0 fits this platform

MPL-2.0 applies copyleft at the **file level**. A company may combine MCM files
with separate proprietary or differently licensed files in a larger product,
but when it distributes a modified MPL-covered file, that file and its source
must remain available under MPL-2.0.

That is the intended balance for MCM:

- manufacturers may adopt the interface and integrate it into commercial
  pedals;
- proprietary application code may remain in separate files;
- improvements made directly to MCM's covered files remain available to the
  community;
- contributors provide a patent grant for their contributions under the terms
  of MPL-2.0;
- the project can participate in larger works without forcing every unrelated
  file into the same license.

## Standard, not Exhibit-B-restricted

This project uses the standard MPL-2.0 license and does **not** attach the
Exhibit B "Incompatible With Secondary Licenses" notice. Do not add Exhibit B
without a deliberate project-wide licensing decision.

## Distribution obligations in practical terms

When distributing this firmware or a product containing it:

1. keep the MPL notices intact;
2. provide recipients with the MPL-2.0 license text or a clear way to obtain it;
3. make the Source Code Form of the MPL-covered files available;
4. publish modifications to MPL-covered files under MPL-2.0;
5. do not impose terms that remove recipients' MPL rights in those source files;
6. preserve third-party notices and licenses;
7. do not imply that the firmware license grants permission to use project
   trademarks or branding.

This page is an engineering summary, not legal advice. The actual license text
controls.

## Relicensing history

Earlier repository revisions were distributed under GNU GPL version 2. Those
historical grants are not revoked: copies obtained under GPLv2 remain usable
under GPLv2. The repository's current and future covered source files are
published under MPL-2.0 from the relicensing commit onward.

Relicensing is valid only to the extent that Roth Amplification Ltd owns the
relevant copyright or has permission from every copyright holder. Before
merging third-party code, maintainers must verify that its license is compatible
and preserve its notices.

## Contributions

Unless a contribution explicitly states otherwise and is accepted with a
compatible license arrangement, contributions to this repository are submitted
under MPL-2.0. Contributors must have the right to submit their work.

## Firmware versus hardware

MPL-2.0 covers the firmware and repository materials identified by the project
notices. It does not automatically cover separate PCB, schematic, enclosure, or
manufacturing repositories. Hardware should use an explicit CERN Open Hardware
Licence variant selected for that hardware repository.

## Source header

New source files must contain:

```text
SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
SPDX-License-Identifier: MPL-2.0
```

They should also contain Mozilla's standard notice:

```text
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
```

Run `python3 tools/check_license_headers.py` before committing.
