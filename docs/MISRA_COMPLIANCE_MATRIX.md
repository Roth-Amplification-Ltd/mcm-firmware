# MCM MISRA-Like Compliance Matrix

| Objective | Evidence | Status |
|---|---|---|
| One canonical build target | `src/MCM_Firmware/MCM_Firmware.ino` | Met |
| No project-owned dynamic allocation | Profile checker and source review | Met |
| Fixed-width protocol types | `McmTypes.h`, `TransportProtocol.h` | Met |
| Scoped finite-state enumerations | Application, button, snapshot, RX frame states | Met |
| Checked bounded queues | `EventQueue`, publisher and transport call sites | Met |
| Atomic snapshot capture | `ControlScanner::captureSnapshot()` copied into publisher | Met |
| Packet validation before state change | `CommandDispatcher` | Met |
| CRC test vectors | `tests/host/test_protocol.cpp` | Met |
| Strict host compiler warnings | `tests/host/run.sh` | Met |
| Automated native-profile check | `tools/check_misra_like.py` and CI | Met |
| License metadata enforcement | Existing license checker and CI | Met |
| Adopted-code boundary | `MISRA_DEVIATIONS.md` | Documented |
| PIO source review | Documentation and manual review | Partial |
| Maximum SPI clock | Target measurement required | Open |
| Worst-case loop execution | Target measurement required | Open |
| Stack high-water measurement | Target measurement required | Open |
| Watchdog/brownout recovery | Design and test required | Open |
| Qualified MISRA analyzer report | Commercial/qualified tool required | Open |
| Formal rule classification and deviation approval | MISRA compliance process required | Open |
| Hardware-in-loop conformance suite | Planned | Open |

## Honest release language

Acceptable:

> The MCM reference firmware follows the published Roth Amplification
> deterministic MISRA C++:2023-inspired native-code profile.

Not acceptable yet:

> The MCM firmware is MISRA C++:2023 compliant.

The second statement requires the open items above to be completed and formally
reviewed.
