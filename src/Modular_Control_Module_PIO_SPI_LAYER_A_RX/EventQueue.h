/*
 * SPDX-FileCopyrightText: 2026 Roth Amplification Ltd
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include <Arduino.h>

/** Internal event categories consumed by StatePublisher. */
enum EventType : uint8_t {
  EVT_NONE = 0,
  EVT_PARAM_CHANGED,
  EVT_SNAPSHOT_BEGIN,
  EVT_SNAPSHOT_END
};

/**
 * One transport-independent state publication request.
 *
 * `index` and `value` are interpreted according to `type`:
 * - PARAM_CHANGED: parameter index and 24-bit value source;
 * - SNAPSHOT_BEGIN/END: index carries parameter count; value is reserved.
 */
struct Event {
  EventType type;
  uint8_t index;
  int32_t value;
};

/**
 * Fixed-capacity, allocation-free, single-context ring buffer.
 *
 * This class is deliberately small enough for embedded use, but it provides no
 * locking. Do not access one instance concurrently from interrupts, both RP2040
 * cores, and the main loop without adding synchronization.
 *
 * Callers must check push(). Silent overflow can produce an incomplete reset or
 * snapshot stream that the master cannot safely accept.
 */
template <uint8_t N>
class EventQueue {
public:
  EventQueue() : h(0), t(0), c(0) {}

  /** Append one event. Returns false when the queue is full. */
  bool push(const Event& e) {
    if (c >= N) return false;

    b[h] = e;
    h = (h + 1) % N;
    c++;
    return true;
  }

  /** Remove the oldest event. Returns false when the queue is empty. */
  bool pop(Event& o) {
    if (c == 0) return false;

    o = b[t];
    t = (t + 1) % N;
    c--;
    return true;
  }

  /** Drop every pending event. Used during RESYNC recovery. */
  void clear() {
    h = 0;
    t = 0;
    c = 0;
  }

  bool empty() const { return c == 0; }
  uint8_t count() const { return c; }

private:
  Event b[N];
  uint8_t h; ///< next insertion index
  uint8_t t; ///< next removal index
  uint8_t c; ///< number of occupied entries
};
