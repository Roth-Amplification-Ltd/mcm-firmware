# RP2040 PIO Implementation

## Why PIO is used

The external master controls SPI clock timing. PIO can wait directly on SCK and CS edges, sample MOSI, and drive MISO without depending on main-loop instruction latency. A second PIO program can reduce CPU work by reporting encoder pin changes only when a phase changes.

## SPI PIO configuration

The corrected root transport uses:

```text
PIO block: PIO1
State machine: 2
IN base: GPIO15 / MOSI
OUT base: GPIO14 / MISO
JMP pin: GPIO13 / CS
OUT shift: left, manual pull
IN shift: right, manual push
Clock divider: 1.0
```

The `wait` instructions use absolute GPIO syntax:

```pio
wait 0 gpio 13
wait 0 gpio 12
```

The `jmp pin` instruction is different: it tests the state machine's configured jump pin. `TransportSPI::begin()` therefore calls `sm_config_set_jmp_pin(..., GPIO13)`.

## SPI instruction walkthrough

```pio
wait_cs_low:
    wait 0 gpio 13
```

Block until CS is asserted.

```pio
    wait 0 gpio 12
```

Ensure SCK is at Mode-0 idle level before the byte begins.

```pio
byte_loop:
    pull noblock
    out pins, 1
```

Attempt to load the next TX byte. `noblock` means the OSR is retained when no word is available. Present the first MISO bit before the master's first rising edge.

```pio
    mov isr, null
    set x, 7
```

Clear RX assembly and configure eight loop iterations.

```pio
bit_loop:
    wait 1 gpio 12
    in pins, 1
    wait 0 gpio 12
    out pins, 1
    jmp x-- bit_loop
```

Sample MOSI on the rising edge and change MISO on the falling edge.

```pio
    push block
```

Push the assembled RX byte. With the configured right-shifting input, software extracts bits 31–24.

```pio
    jmp pin wait_cs_low
    jmp byte_loop
```

If CS is high, return to frame wait. Otherwise process another byte.

## First-bit caveat

The first MISO bit must already be stable before the first rising clock edge. That is why the program executes one `out pins, 1` immediately after `pull noblock` rather than waiting for the first falling edge.

## TX FIFO and packet ownership

The software currently considers a packet retired after its eight bytes have been handed to the PIO TX FIFO. This is not the same as proving that the master physically clocked the packet. IRQ correctness must eventually be based on consumed clocked data or an explicit transaction-complete mechanism.

## RX frame assembly

PIO supplies bytes. C++ supplies packet framing by watching CS transitions and accepting a packet only when exactly eight bytes arrived between falling and rising CS edges.

Because CS edges are sampled with `digitalRead()` in `service()`, the service loop must run often enough not to miss an entire short CS pulse. A future design could have PIO push explicit frame markers or raise an IRQ to remove this software timing dependency.

## Encoder PIO walkthrough

The encoder program samples two adjacent pins, stores the previous sample in X, and pushes a new sample when it differs:

```pio
.program encoder_state_stream
.wrap_target
    in pins, 2
    mov x, isr
loop:
    in pins, 2
    mov y, isr
    jmp x == y, loop
    mov x, y
    mov isr, y
    push block
    jmp loop
.wrap
```

The C++ transition table determines direction and rejects invalid state jumps.

## Generated headers

C++ includes `SpiSlaveM0.pio.h` and `EncoderPIO.pio.h`, not the `.pio` source directly. The selected Arduino-Pico build workflow must generate those headers. Do not commit a generated header from a different PIO source revision without proving they match.

## Historical nested PIO copy

The nested Layer-A folder contains an older PIO file using `wait ... pin 12/13` and `jmp pin 13 ...` syntax. The corrected root PIO file is authoritative for current hardware-aligned work.
