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

/**
 * @struct Param
 * @brief Minimal authoritative parameter record.
 *
 * Only resetToDefault() is used by the current integrated sketch. Range, step,
 * fine-step, and wrap metadata are foundations for the later encoder-binding
 * layer and should not be mistaken for already implemented behavior.
 */
struct Param {
  const char* name;
  int32_t value;
  int32_t defValue;
  int32_t minValue;
  int32_t maxValue;
  int32_t baseStep;
  int32_t fineStep;
  bool wrap;

  /** Restore the authoritative value to its configured default. */
  void resetToDefault() { value = defValue; }
};
