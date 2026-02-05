#pragma once
#include <Arduino.h>
struct Param {
  const char* name;
  int32_t value, defValue;
  int32_t minValue, maxValue;
  int32_t baseStep, fineStep;
  bool wrap;
  void resetToDefault() { value = defValue; }
};
