#include "TransportSPI.h"
#include "SpiSlaveM0.pio.h"

/**
 * Construct a transport with a fixed PIO allocation.
 *
 * PIO1 state machine 2 is reserved for external host SPI in the current
 * design. A future resource allocator should make collisions with six encoder
 * state machines impossible at compile time.
 */
TransportSPI::TransportSPI()
: _pio(pio1), _sm(2), _offset(0),
  _txHead(0), _txTail(0), _txCount(0), _txByteIndex(0),
  _rxIndex(0), _rxReady(false),
  _csWasLow(false)
{}

void TransportSPI::begin()
{
  // SCK, CS, and MOSI are driven by the master. The CS pull-up keeps the MCM
  // deselected while the host is resetting, disconnected, or tri-stated.
  pinMode(PIN_SCK, INPUT);
  pinMode(PIN_CS, INPUT_PULLUP);
  pinMode(PIN_MOSI, INPUT);

  // IRQ begins inactive. It becomes active when a complete packet enters the
  // software TX ring.
  pinMode(PIN_IRQ, OUTPUT);
  setIrqLevel(false);

  // Load the generated PIO program into PIO1 instruction memory. The returned
  // offset is required when constructing the state-machine configuration.
  _offset = pio_add_program(_pio, &spi_slave_m0_msb_rx_program);

  // Hand MISO ownership to PIO and configure it as the single output pin used
  // by the program's OUT instruction.
  pio_gpio_init(_pio, PIN_MISO);
  pio_sm_set_consecutive_pindirs(_pio, _sm, PIN_MISO, 1, true);

  pio_sm_config config = spi_slave_m0_msb_rx_program_get_default_config(_offset);

  // IN reads MOSI. OUT drives MISO. JMP PIN tests CS.
  sm_config_set_in_pins(&config, PIN_MOSI);
  sm_config_set_out_pins(&config, PIN_MISO, 1);
  sm_config_set_jmp_pin(&config, PIN_CS);

  // TX shifts left. Software places the byte in bits 31..24, allowing the PIO
  // OUT instruction to emit the most-significant bit first.
  sm_config_set_out_shift(&config, false, false, 32);

  // RX shifts right. Eight sampled bits accumulate in bits 31..24, which
  // service() extracts with (word >> 24).
  sm_config_set_in_shift(&config, true, false, 32);

  // The PIO program synchronizes itself to external SCK/CS edges. Running the
  // state machine at full peripheral clock minimizes reaction latency.
  sm_config_set_clkdiv(&config, 1.0f);

  pio_sm_init(_pio, _sm, _offset, &config);
  pio_sm_clear_fifos(_pio, _sm);
  pio_sm_set_enabled(_pio, _sm, true);

  resetRxFrame();
}

bool TransportSPI::enqueueTxPacket(const uint8_t* packet)
{
  // The caller owns overflow policy. Returning false is not sufficient unless
  // the application records a diagnostic or schedules a later RESYNC.
  if (_txCount >= TX_Q_DEPTH) return false;

  memcpy(_txQ[_txHead], packet, PKT_LEN);
  _txHead = (_txHead + 1) % TX_Q_DEPTH;
  _txCount++;

  // Level assertion means the host can remain busy temporarily without losing
  // a short interrupt pulse.
  setIrqLevel(true);
  return true;
}

bool TransportSPI::hasRxPacket() const
{
  return _rxReady;
}

bool TransportSPI::popRxPacket(uint8_t* packet)
{
  if (!_rxReady) return false;

  memcpy(packet, _rxPkt, PKT_LEN);
  _rxReady = false;
  return true;
}

void TransportSPI::service()
{
  // -----------------------------------------------------------------------
  // 1. Software CS frame tracking
  // -----------------------------------------------------------------------
  // PIO shifts bytes, but this C++ layer decides which eight received bytes
  // constitute one complete command. A falling edge starts a clean frame. A
  // rising edge accepts the frame only when exactly PKT_LEN bytes arrived.
  const bool csLow = (digitalRead(PIN_CS) == LOW);

  if (csLow && !_csWasLow) {
    resetRxFrame();
    _csWasLow = true;
  } else if (!csLow && _csWasLow) {
    _rxReady = (_rxIndex == PKT_LEN);
    _rxIndex = 0;
    _csWasLow = false;
  }

  // -----------------------------------------------------------------------
  // 2. Drain PIO RX FIFO
  // -----------------------------------------------------------------------
  // Each PIO PUSH supplies one word containing one byte in bits 31..24. Bytes
  // observed while CS is high are ignored. Bytes beyond the eighth are also
  // ignored until CS rises; production firmware should count this condition.
  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
    const uint32_t word = pio_sm_get(_pio, _sm);
    const uint8_t byte = static_cast<uint8_t>((word >> 24) & 0xFFu);

    if (csLow && _rxIndex < PKT_LEN) {
      _rxPkt[_rxIndex++] = byte;
    }
  }

  // -----------------------------------------------------------------------
  // 3. Feed PIO TX FIFO
  // -----------------------------------------------------------------------
  // Bytes are streamed packet-by-packet. Retiring a packet here means all of
  // its bytes have entered the PIO FIFO; it does NOT prove that SCK physically
  // shifted them. This distinction matters for exact IRQ semantics.
  while (_txCount > 0 && !pio_sm_is_tx_fifo_full(_pio, _sm)) {
    pushTxByte(_txQ[_txTail][_txByteIndex]);
    _txByteIndex++;

    if (_txByteIndex >= PKT_LEN) {
      _txByteIndex = 0;
      _txTail = (_txTail + 1) % TX_Q_DEPTH;
      _txCount--;

      if (_txCount == 0) setIrqLevel(false);
    }
  }
}

void TransportSPI::setIrqLevel(bool asserted)
{
  digitalWrite(PIN_IRQ,
               IRQ_ACTIVE_HIGH ? (asserted ? HIGH : LOW)
                               : (asserted ? LOW : HIGH));
}

void TransportSPI::pushTxByte(uint8_t byte)
{
  // The PIO OUT shifter is configured for left shift, so placing the byte in
  // bits 31..24 produces MSB-first output.
  pio_sm_put(_pio, _sm, static_cast<uint32_t>(byte) << 24);
}

void TransportSPI::resetRxFrame()
{
  _rxIndex = 0;
  _rxReady = false;
}
