#pragma once
#include <Arduino.h>

struct Param {
  const char* name;
  int32_t value, defValue;
  int32_t minValue, maxValue;
  int32_t baseStep, fineStep;
  bool wrap;

  void applyDetentDelta(int32_t detentDelta, bool fineMode, uint8_t accelMul) {
    if (detentDelta == 0) return;
    int32_t step = (fineMode ? fineStep : baseStep) * (int32_t)accelMul;
    int32_t next = value + detentDelta * step;
    if (!wrap) {
      if (next < minValue) next = minValue;
      if (next > maxValue) next = maxValue;
      value = next;
      return;
    }
    int32_t range = (maxValue - minValue) + 1;
    if (range <= 0) { value = next; return; }
    int32_t offset = next - minValue;
    int32_t mod = offset % range;
    if (mod < 0) mod += range;
    value = minValue + mod;
  }

  void resetToDefault() { value = defValue; }
};

struct EncoderAcceleration {
  uint32_t lastMoveMs = 0;
  static uint8_t multiplierFromDt(uint32_t dtMs) {
    if (dtMs <= 30) return 8;
    if (dtMs <= 80) return 4;
    if (dtMs <= 160) return 2;
    return 1;
  }
  uint8_t onDetentEvent(uint32_t nowMs) {
    uint32_t dt = (lastMoveMs == 0) ? 100000 : (nowMs - lastMoveMs);
    lastMoveMs = nowMs;
    return multiplierFromDt(dt);
  }
};
