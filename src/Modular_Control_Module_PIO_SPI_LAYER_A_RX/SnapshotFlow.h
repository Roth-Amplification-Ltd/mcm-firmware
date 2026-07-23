#pragma once
#include <Arduino.h>

/*****************************************************************************************
 *
 * SnapshotFlowState
 * =================
 * Tiny shared state object used to coordinate "snapshot in progress" logic
 * between:
 *   - CommandDispatcher (producer of snapshot events)
 *   - StatePublisher    (serializer of snapshot events -> wire packets)
 *
 * Why do we need this?
 * --------------------
 * A snapshot is NOT a single packet; it is an ordered sequence:
 *
 *    SNAPSHOT_BEGIN
 *    PARAM_STATE x N
 *    SNAPSHOT_END
 *
 * We must ensure we never interleave two snapshots like this:
 *
 *    SNAPSHOT_BEGIN (A)
 *    PARAM_STATE ...
 *    SNAPSHOT_BEGIN (B)   <-- BAD: overlapping snapshots
 *    ...
 *    SNAPSHOT_END (A)
 *    SNAPSHOT_END (B)
 *
 * The simplest robust rule is:
 *   "Once we have queued a snapshot, do not queue another snapshot until
 *    the previous snapshot END event has been serialized by StatePublisher."
 *
 * NOTE:
 * - We intentionally clear the 'in progress' flag when SNAPSHOT_END is
 *   SERIALIZED (i.e., converted into a wire packet). We do NOT wait until the
 *   master has physically clocked the END packet. That's okay because:
 *     - ordering in the TX FIFO remains correct
 *     - snapshots will appear back-to-back, not interleaved
 *     - the master can always issue RESYNC if it loses its place
 *
 *****************************************************************************************/

struct SnapshotFlowState {
  volatile bool snapshot_in_progress = false;

  // Reserved for future expansion:
  // - sequence ID (increment per snapshot)
  // - snapshot checksum accumulation
  // - rate limiting / throttling
  uint8_t seq_id = 0;
};
