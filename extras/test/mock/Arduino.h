// Имитация Arduino.h для тестов AlashMLX90614: millis, delay (с учётом времени), Print.
#pragma once
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
class __FlashStringHelper;
#define F(x) (reinterpret_cast<const __FlashStringHelper *>(x))
#define PROGMEM
#define HEX 16
inline uint32_t g_ms = 1000;
inline uint32_t millis() { return g_ms; }
inline void delay(uint32_t ms) { g_ms += ms; }
class Print {
 public:
  std::string text;
  void put(const char *s) { printf("%s", s); text += s; }
  void print(const char *s) { put(s); }
  void print(const __FlashStringHelper *s) { put((const char *)s); }
  void println(const __FlashStringHelper *s) { put((const char *)s); put("\n"); }
  void print(long v) { char b[24]; snprintf(b, sizeof b, "%ld", v); put(b); }
  void println(long v) { print(v); put("\n"); }
  void print(double v, int d = 2) { char b[40]; snprintf(b, sizeof b, "%.*f", d, v); put(b); }
  void println(double v, int d = 2) { print(v, d); put("\n"); }
  void print(unsigned int v, int base) { char b[16]; snprintf(b, sizeof b, base == 16 ? "%X" : "%u", v); put(b); }
  void println(unsigned char v, int base) { char b[16]; snprintf(b, sizeof b, base == 16 ? "%X" : "%u", v); put(b); put("\n"); }
  void println(const char *s = "") { put(s); put("\n"); }
};
