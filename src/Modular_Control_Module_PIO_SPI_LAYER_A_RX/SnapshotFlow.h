#pragma once
#include <Arduino.h>

/**
 * @struct SnapshotFlowState
 * @brief Shared guard preventing two snapshot event sequences from interleaving.
 *
 * CommandDispatcher sets snapshot_in_progress before enqueueing BEGIN/state/END.
 * StatePublisher clears it when the END event is serialized into a packet.
 *
 * This guarantees ordering in the software publication stream, but it does not
 * prove the END packet was physically clocked by the master. The transport may
 * still hold the END bytes in its software queue or PIO FIFO.
 */
struct SnapshotFlowState {
  volatile bool snapshot_in_progress = false;

  /**
   * Reserved sequence counter.
   *
   * The dispatcher increments this field, but the current packet serializer
   * does not transmit it. The coherent snapshot v1.1 proposal defines how a
   * sequence ID should appear on the wire.
   */
  uint8_t seq_id = 0;
};
