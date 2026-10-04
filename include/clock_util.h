#pragma once
// Einfache Software-Uhr ohne RTC/WLAN:
// Startet mit der Kompilierzeit (__TIME__) und zählt dann mit millis() weiter.
// Später kann man das durch NTP oder eine RTC ersetzen.

#include <Arduino.h>

// Millisekunden seit Mitternacht
inline uint32_t clockMillis() {
  static const uint32_t startSec =
      ((__TIME__[0] - '0') * 10 + (__TIME__[1] - '0')) * 3600 +
      ((__TIME__[3] - '0') * 10 + (__TIME__[4] - '0')) * 60 +
      ((__TIME__[6] - '0') * 10 + (__TIME__[7] - '0'));
  return (uint32_t)(((uint64_t)startSec * 1000 + millis()) % 86400000ULL);
}
