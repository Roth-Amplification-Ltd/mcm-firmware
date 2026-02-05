#include "TransportSPI.h"
#include "SpiSlaveM0.pio.h"

TransportSPI::TransportSPI()
: _pio(pio1), _sm(2), _offset(0),
  _txHead(0), _txTail(0), _txCount(0), _txByteIndex(0),
  _rxIndex(0), _rxReady(false),
  _csWasLow(false)
{}

void TransportSPI::begin()
{
  // ---------------- GPIO init ----------------
  pinMode(PIN_SCK, INPUT);
  pinMode(PIN_CS, INPUT_PULLUP);
  pinMode(PIN_MOSI, INPUT);

  pinMode(PIN_IRQ, OUTPUT);
  setIrqLevel(false); // idle

  // ---------------- PIO program load ----------------
  _offset = pio_add_program(_pio, &spi_slave_m0_msb_rx_program);

  // MISO under PIO control
  pio_gpio_init(_pio, PIN_MISO);
  pio_sm_set_consecutive_pindirs(_pio, _sm, PIN_MISO, 1, true);

  // Configure state machine
  pio_sm_config c = spi_slave_m0_msb_rx_program_get_default_config(_offset);

  // "IN pins" reads from in_base. We set it to MOSI.
  sm_config_set_in_pins(&c, PIN_MOSI);

  // "OUT pins" drives MISO
  sm_config_set_out_pins(&c, PIN_MISO, 1);

  // TX: shift LEFT, and we load bytes into bits31..24 (byte << 24)
  sm_config_set_out_shift(&c, false /*shift_right?*/, false /*autopull*/, 32);

  // RX: we want MSB-first assembly into bits31..24, so set IN shift_right = true.
  // With shift_right=true, each 'in pins,1' inserts new bit into MSB side.
  // After 8 bits, the received byte resides in bits31..24.
  sm_config_set_in_shift(&c, true /*shift_right*/, false /*autopush*/, 32);

  // Run PIO fast; SPI timing comes from external SCK edges anyway.
  sm_config_set_clkdiv(&c, 1.0f);

  pio_sm_init(_pio, _sm, _offset, &c);
  pio_sm_set_enabled(_pio, _sm, true);

  // Clear FIFOs to start clean
  pio_sm_clear_fifos(_pio, _sm);

  resetRxFrame();
}

bool TransportSPI::enqueueTxPacket(const uint8_t* pkt8)
{
  if (_txCount >= TX_Q_DEPTH) {
    return false;
  }

  memcpy(_txQ[_txHead], pkt8, PKT_LEN);
  _txHead = (_txHead + 1) % TX_Q_DEPTH;
  _txCount++;

  // If we just transitioned empty->nonempty, assert IRQ.
  setIrqLevel(true);
  return true;
}

bool TransportSPI::hasRxPacket() const
{
  return _rxReady;
}

bool TransportSPI::popRxPacket(uint8_t* pkt8)
{
  if (!_rxReady) return false;
  memcpy(pkt8, _rxPkt, PKT_LEN);
  _rxReady = false;
  return true;
}

void TransportSPI::service()
{
  // ----------------------------------------------------
  // 1) CS tracking for robust RX framing
  // ----------------------------------------------------
  // We treat CS as the ONLY authoritative frame boundary.
  //
  // Rules:
  //   - When CS goes LOW: begin a new RX frame, reset rxIndex
  //   - While CS is LOW: collect bytes into rxPkt[]
  //   - When CS goes HIGH:
  //       - If rxIndex == 8 -> mark rxReady (complete packet)
  //       - Else discard partial (rxIndex != 0)
  //       - Reset rxIndex for next frame
  //
  // This prevents desync if the master aborts early or glitches CS.
  // ----------------------------------------------------
  bool csLow = (digitalRead(PIN_CS) == LOW);

  if (csLow && !_csWasLow) {
    // CS falling edge: start of frame
    resetRxFrame();
    _csWasLow = true;
  } else if (!csLow && _csWasLow) {
    // CS rising edge: end of frame
    if (_rxIndex == PKT_LEN) {
      _rxReady = true;
    } else {
      // Partial frame -> discard
      _rxReady = false;
    }
    _rxIndex = 0;
    _csWasLow = false;

    // NOTE: Any stray bytes arriving after CS high are ignored by the
    // RX collector below, because it only stores bytes while csLow==true.
  }

  // ----------------------------------------------------
  // 2) RX FIFO drain (PIO -> software)
  // ----------------------------------------------------
  // The PIO pushes one word per received byte.
  // The received byte is in bits31..24 (due to shift_right IN config).
  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
    uint32_t w = pio_sm_get(_pio, _sm);
    uint8_t  b = (uint8_t)((w >> 24) & 0xFF);

    // Only accept bytes while CS is LOW (within a frame).
    if (csLow) {
      if (_rxIndex < PKT_LEN) {
        _rxPkt[_rxIndex++] = b;
      } else {
        // Overrun within a frame (master violated contract)
        // We discard extra bytes and wait for CS to rise to reset.
      }
    } else {
      // CS high: ignore any stray bytes.
    }
  }

  // ----------------------------------------------------
  // 3) TX feeding (software -> PIO TX FIFO)
  // ----------------------------------------------------
  // We stream bytes from the head packet into PIO TX FIFO as space allows.
  //
  // IMPORTANT ASSUMPTION:
  //   Master clocks exactly 8 bytes per CS frame.
  //
  // With that contract, "byte count consumption" stays aligned to packets.
  while (_txCount > 0 && !pio_sm_is_tx_fifo_full(_pio, _sm)) {
    // Current head packet is at _txTail
    uint8_t b = _txQ[_txTail][_txByteIndex];
    pushTxByte(b);
    _txByteIndex++;

    if (_txByteIndex >= PKT_LEN) {
      // Finished one packet
      _txByteIndex = 0;
      _txTail = (_txTail + 1) % TX_Q_DEPTH;
      _txCount--;

      if (_txCount == 0) {
        // Queue empty => deassert IRQ
        setIrqLevel(false);
      }
    }
  }
}

void TransportSPI::setIrqLevel(bool asserted)
{
  if (IRQ_ACTIVE_HIGH) {
    digitalWrite(PIN_IRQ, asserted ? HIGH : LOW);
  } else {
    digitalWrite(PIN_IRQ, asserted ? LOW : HIGH);
  }
}

void TransportSPI::pushTxByte(uint8_t b)
{
  // Align byte into bits31..24 so shift-left OUT emits MSB-first.
  pio_sm_put(_pio, _sm, ((uint32_t)b) << 24);
}

void TransportSPI::resetRxFrame()
{
  _rxIndex = 0;
  _rxReady = false;
}
