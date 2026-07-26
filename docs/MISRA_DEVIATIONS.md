# MCM Deviation and Adopted-Code Register

## D-001: Arduino-Pico core

- **Classification:** adopted code
- **Reason:** board startup, Arduino APIs, timing, USB/serial, and build
  integration are supplied by the external Arduino-Pico project.
- **Control:** project code calls a narrow set of APIs from dedicated hardware
  and application modules. Native ownership and protocol logic do not depend on
  Arduino dynamic objects.
- **Evidence still required:** pinned toolchain/core version and supplier change
  review for production releases.

## D-002: Raspberry Pi Pico SDK and hardware headers

- **Classification:** adopted low-level code
- **Reason:** PIO configuration and FIFO access require SDK types, macros, and
  memory-mapped peripheral APIs.
- **Control:** SDK interaction is isolated in `EncoderPIO` and `TransportSPI`.
- **Evidence still required:** target compiler configuration and analyzer model
  for RP2040 register access.

## D-003: PIO assembly and generated headers

- **Classification:** generated/non-C++ code
- **Reason:** RP2040 PIO programs are written in their own assembly language and
  compiled to generated C headers.
- **Control:** source is checked into the canonical sketch, reviewed as a state
  machine, and exercised with target timing tests.
- **Evidence still required:** logic-analyzer captures, malformed-frame tests,
  and maximum supported SPI clock.

## D-004: Hardware pointer type `PIO`

- **Classification:** necessary hardware interface
- **Reason:** Pico SDK represents a PIO peripheral as a pointer to a
  memory-mapped hardware block.
- **Control:** pointers are never owned, freed, arithmetically modified, or
  exposed through the application protocol.

## D-005: Arduino entry-point functions

- **Classification:** framework-mandated interface
- **Reason:** Arduino requires global `setup()` and `loop()` functions.
- **Control:** both functions contain only delegation to one statically allocated
  `McmApplication` object.

## D-006: Native source uses C++ standard-library fixed containers

- **Classification:** accepted project practice
- **Reason:** `std::array` provides compile-time-sized storage without allocation.
- **Control:** dynamically sized containers are prohibited by the profile.

## Deviation workflow

Any new deviation must include:

1. exact file and construct;
2. engineering necessity;
3. risk analysis;
4. containment mechanism;
5. verification evidence;
6. reviewer and approval date before a formal compliance claim.
