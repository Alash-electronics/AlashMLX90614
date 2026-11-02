// AlashMLX90614: бесконтактный инфракрасный термометр MLX90614 (I2C / SMBus): температура объекта и окружения,
// коэффициент излучения, поправка, проверка контрольной суммы PEC. Без зависимостей, только встроенная Wire.
// MIT License. Alash Electronics, https://github.com/Alash-electronics/AlashMLX90614
#ifndef ALASH_MLX90614_H
#define ALASH_MLX90614_H

#include <Arduino.h>
#include <Wire.h>

class AlashMLX90614 {
 public:
  enum Error : uint8_t {
    ERR_OK = 0,
    ERR_NO_DEVICE,       // на шине нет ответа по адресу датчика (обычно 0x5A)
    ERR_I2C,             // ошибка обмена по I2C
    ERR_PEC,             // контрольная сумма ответа не сошлась (помехи, слишком быстрая шина, длинные провода)
    ERR_SENSOR,          // датчик сообщил об ошибке или значение вне диапазона (ИК-элемент не работает)
    ERR_BAD_VALUE,       // коэффициент излучения вне 0.1..1.0 или число не определено
    ERR_WRITE            // датчик не подтвердил запись
  };
  enum Language : uint8_t { LANG_RU = 0, LANG_KK = 1, LANG_EN = 2 };

  // ---- Запуск --------------------------------------------------------------------------
  bool begin(TwoWire &wire = Wire, uint8_t address = 0x5A, bool startWire = true);
#if defined(ESP32) || defined(ESP8266)
  bool begin(int sdaPin, int sclPin, uint8_t address = 0x5A);
#endif

  // ---- Температура (°C; NAN, если прочитать не удалось) ------------------------------------
  float objectTemperature();                  // температура объекта, на который смотрит датчик (с учётом поправки)
  float ambientTemperature();                 // температура самого датчика (окружения)
  float object2Temperature();                 // второй ИК-элемент: только у двухзонных датчиков (MLX90614xxA), у остальных число не имеет смысла
  float objectTemperatureF() { return objectTemperature() * 1.8f + 32.0f; }
  float ambientTemperatureF() { float t = ambientTemperature(); return t * 1.8f + 32.0f; }
  uint16_t rawObject();                       // сырое значение (0.02 К на единицу), 0 при ошибке
  uint16_t rawAmbient();

  // ---- Настройка ---------------------------------------------------------------------------
  void setOffset(float degrees) { _offset = degrees; }   // поправка к температуре объекта (подгонка по эталону); не пишется в датчик
  float offset() const { return _offset; }
  float emissivity();                         // коэффициент излучения из датчика (0.1..1.0); NAN при ошибке
  bool setEmissivity(float e);                // ЗАПИСЬ В EEPROM датчика (ресурс около 100 000 записей); вступает в силу после выключения и включения питания датчика

  // ---- Состояние и диагностика -------------------------------------------------------------------
  bool readId(uint16_t id[4]);                // 64-битный номер датчика
  bool isConnected();
  uint8_t address() const { return _addr; }
  Error error() const { return _err; }
  void clearError() { _err = ERR_OK; }
  void setLanguage(Language lang) { _lang = lang; }
  const __FlashStringHelper *errorText();
  void diagnose(Print &out);

  static uint8_t pec(const uint8_t *data, uint8_t len);   // CRC-8, полином x^8 + x^2 + x + 1

 private:
  bool readWord(uint8_t reg, uint16_t &value);
  bool writeWord(uint8_t reg, uint16_t value);
  float toCelsius(uint16_t raw, bool object);
  TwoWire *_wire = &Wire;
  uint8_t _addr = 0x5A;
  float _offset = 0;
  Error _err = ERR_OK;
  Language _lang = LANG_RU;
};

#endif  // ALASH_MLX90614_H
