// Имитация I2C и датчика MLX90614 (SMBus): регистры, PEC, повторный старт, запись EEPROM с «применением после перезапуска питания».
#pragma once
#include "Arduino.h"
#include <map>
#include <vector>
inline uint8_t pecRef(const uint8_t *d, int n) {          // независимая реализация CRC-8 (x^8+x^2+x+1) из даташита
  uint8_t crc = 0;
  for (int i = 0; i < n; i++) { crc ^= d[i]; for (int b = 0; b < 8; b++) crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : crc << 1; }
  return crc;
}
struct FakeMlx {
  bool present = true; uint8_t addr = 0x5A;
  std::map<uint8_t, uint16_t> reg;           // действующие значения
  std::map<uint8_t, uint16_t> eepromNew;     // записано в EEPROM, действует после перезапуска питания
  int corruptNext = 0;                       // сколько ближайших ответов испортить (PEC)
  bool failTx = false;                       // шина возвращает ошибку
  int readsWithoutRepeatedStart = 0;         // сколько раз чтение шло без повторного старта (на настоящем датчике так не работает)
  std::vector<std::pair<uint8_t, uint16_t>> writes; std::vector<uint32_t> writeTimes;
  bool badPecWrite = false;
  FakeMlx() { reset(); }
  void reset() { reg.clear(); eepromNew.clear(); present = true; addr = 0x5A; corruptNext = 0; failTx = false; readsWithoutRepeatedStart = 0; writes.clear(); writeTimes.clear(); badPecWrite = false;
    setAmbient(25.0f); setObject(36.6f); reg[0x24] = (uint16_t)(0.95f * 65535.0f + 0.5f);
    reg[0x3C] = 0x1234; reg[0x3D] = 0x5678; reg[0x3E] = 0x9ABC; reg[0x3F] = 0xDEF0; }
  void setAmbient(float c) { reg[0x06] = (uint16_t)((c + 273.15f) / 0.02f + 0.5f); }
  void setObject(float c) { reg[0x07] = (uint16_t)((c + 273.15f) / 0.02f + 0.5f); }
  void powerCycle() { for (auto &kv : eepromNew) reg[kv.first] = kv.second; eepromNew.clear(); }
};
inline FakeMlx g_mlx;
class TwoWire {
 public:
  void begin() {}
  void begin(int, int) {}
  void beginTransmission(uint8_t a) { _a = a; _tx.clear(); }
  size_t write(uint8_t v) { _tx.push_back(v); return 1; }
  uint8_t endTransmission(bool stop = true) {
    if (!g_mlx.present || _a != g_mlx.addr) return 2;
    if (g_mlx.failTx) return 4;
    _repeated = !stop; _cmd = _tx.empty() ? 0 : _tx[0];
    if (stop && _tx.size() == 4) {                      // запись слова: команда, младший, старший, PEC
      uint8_t frame[4] = {(uint8_t)(_a << 1), _tx[0], _tx[1], _tx[2]};
      uint8_t want = pecRef(frame, 4);
      if (_tx[3] != want || g_mlx.badPecWrite) return 4;    // датчик не принимает запись с неверной суммой
      uint16_t v = _tx[1] | (_tx[2] << 8);
      g_mlx.writes.push_back({_tx[0], v}); g_mlx.writeTimes.push_back(g_ms);
      if (_tx[0] >= 0x20) g_mlx.eepromNew[_tx[0]] = v; else g_mlx.reg[_tx[0]] = v;
    }
    return 0;
  }
  uint8_t requestFrom(uint8_t a, uint8_t len) {
    if (!g_mlx.present || a != g_mlx.addr || len != 3) return 0;
    if (!_repeated) { g_mlx.readsWithoutRepeatedStart++; return 0; }
    _repeated = false;
    uint16_t v = g_mlx.reg.count(_cmd) ? g_mlx.reg[_cmd] : 0;
    _buf[0] = v & 0xFF; _buf[1] = v >> 8;
    uint8_t frame[5] = {(uint8_t)(a << 1), _cmd, (uint8_t)((a << 1) | 1), _buf[0], _buf[1]};
    _buf[2] = pecRef(frame, 5);
    if (g_mlx.corruptNext > 0) { g_mlx.corruptNext--; _buf[2] ^= 0x5A; }
    _pos = 0; _len = 3; return 3;
  }
  int read() { return _pos < _len ? _buf[_pos++] : -1; }
 private:
  uint8_t _a = 0, _cmd = 0, _buf[4], _len = 0, _pos = 0; bool _repeated = false;
  std::vector<uint8_t> _tx;
};
inline TwoWire Wire;
