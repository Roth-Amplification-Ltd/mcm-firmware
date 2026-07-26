# RP2040 PIO Implementation

## SPI program

`SpiSlaveM0.pio` performs Mode-0, MSB-first bit transfer:

1. wait for CS low;
2. wait for SCK low;
3. present the first MISO bit;
4. sample MOSI on each rising edge;
5. update MISO on each falling edge;
6. push one received byte in word bits 31..24;
7. after CS is high at a byte boundary, push `0xFFFFFFFF` as a frame marker.

The marker means C++ does not need to poll CS fast enough to observe the rising
edge. It accepts only exactly eight byte words before a marker.

### Important throughput limitation

The PIO RX and TX FIFOs are finite and share one state machine. The CPU must
service them during a transaction at the selected SPI clock. A maximum supported
clock must be measured on target hardware. The current source deliberately does
not claim one.

## Encoder program

`EncoderPIO.pio`:

1. samples the two adjacent phase pins;
2. keeps the previous sample in X;
3. loops while the sample is unchanged;
4. pushes the new two-bit state when it changes;
5. blocks if the RX FIFO is full, providing hardware backpressure.

The C++ service drains a fixed maximum number of samples per loop iteration, so
native execution remains bounded. At extreme rotation or prolonged CPU stalls,
PIO backpressure may delay later transitions; this must be tested on hardware.

## Generated headers

The Arduino-Pico build creates `EncoderPIO.pio.h` and `SpiSlaveM0.pio.h`.
Generated output is not manually edited. PIO source, generator version, and
logic-analyzer evidence form the verification unit.
