// MIT License. Alash Electronics.
#include "AlashMLX90614.h"

#define T(ru, kk, en) (_lang == LANG_KK ? F(kk) : _lang == LANG_EN ? F(en) : F(ru))

// Регистры (RAM 0x00..0x1F, EEPROM 0x20..0x3F, адрес команды = регистр)
static const uint8_t REG_TA = 0x06, REG_TOBJ1 = 0x07, REG_TOBJ2 = 0x08, REG_EMISS = 0x24, REG_ID1 = 0x3C;

uint8_t AlashMLX90614::pec(const uint8_t *d, uint8_t n) {
  uint8_t crc = 0;
  while (n--) {
    crc ^= *d++;
    for (uint8_t i = 0; i < 8; i++) crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x07) : (uint8_t)(crc << 1);
  }
  return crc;
}

bool AlashMLX90614::begin(TwoWire &wire, uint8_t address, bool startWire) {
  _wire = &wire;
  _addr = address;
  _err = ERR_OK;
  if (startWire) _wire->begin();
  uint16_t raw;
  return readWord(REG_TA, raw);
}

#if defined(ESP32) || defined(ESP8266)
bool AlashMLX90614::begin(int sdaPin, int sclPin, uint8_t address) {
  Wire.begin(sdaPin, sclPin);
  return begin(Wire, address, false);
}
#endif

// SMBus «чтение слова»: команда, повторный старт, три байта (младший, старший, PEC). До двух попыток при ошибке суммы.
bool AlashMLX90614::readWord(uint8_t reg, uint16_t &value) {
  for (uint8_t attempt = 0; attempt < 2; attempt++) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    uint8_t r = _wire->endTransmission(false);             // повторный старт обязателен: без него датчик не отвечает
    if (r != 0) { _err = (r == 2 || r == 3) ? ERR_NO_DEVICE : ERR_I2C; return false; }
    if (_wire->requestFrom(_addr, (uint8_t)3) != 3) { _err = ERR_I2C; return false; }
    uint8_t lo = _wire->read(), hi = _wire->read(), got = _wire->read();
    uint8_t frame[5] = {(uint8_t)(_addr << 1), reg, (uint8_t)((_addr << 1) | 1), lo, hi};
    if (pec(frame, 5) == got) { value = (uint16_t)lo | ((uint16_t)hi << 8); _err = ERR_OK; return true; }
    _err = ERR_PEC;
  }
  return false;
}

bool AlashMLX90614::writeWord(uint8_t reg, uint16_t value) {
  uint8_t frame[4] = {(uint8_t)(_addr << 1), reg, (uint8_t)value, (uint8_t)(value >> 8)};
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(frame[2]);
  _wire->write(frame[3]);
  _wire->write(pec(frame, 4));
  uint8_t r = _wire->endTransmission();
  if (r != 0) { _err = (r == 2 || r == 3) ? ERR_NO_DEVICE : ERR_WRITE; return false; }
  return true;
}

// 0.02 К на единицу. Допустимо по даташиту: объект -70.01..382.19 °C (0x27AD..0x7FFF), окружение -38.2..125 °C. Бит 15 регистра объекта = ошибка датчика.
float AlashMLX90614::toCelsius(uint16_t raw, bool object) {
  if (object && (raw & 0x8000)) { _err = ERR_SENSOR; return NAN; }
  if (raw < 0x27AD || raw > 0x7FFF) { _err = ERR_SENSOR; return NAN; }
  return raw * 0.02f - 273.15f;
}

float AlashMLX90614::objectTemperature() {
  uint16_t raw;
  if (!readWord(REG_TOBJ1, raw)) return NAN;
  float t = toCelsius(raw, true);
  return t == t ? t + _offset : NAN;
}

float AlashMLX90614::object2Temperature() {
  uint16_t raw;
  if (!readWord(REG_TOBJ2, raw)) return NAN;
  float t = toCelsius(raw, true);
  return t == t ? t + _offset : NAN;
}

float AlashMLX90614::ambientTemperature() {
  uint16_t raw;
  if (!readWord(REG_TA, raw)) return NAN;
  return toCelsius(raw, false);
}

uint16_t AlashMLX90614::rawObject() { uint16_t r; return readWord(REG_TOBJ1, r) ? r : 0; }
uint16_t AlashMLX90614::rawAmbient() { uint16_t r; return readWord(REG_TA, r) ? r : 0; }

float AlashMLX90614::emissivity() {
  uint16_t raw;
  if (!readWord(REG_EMISS, raw)) return NAN;
  if (raw == 0) { _err = ERR_SENSOR; return NAN; }
  return raw / 65535.0f;
}

// Порядок записи EEPROM из даташита Melexis: записать 0x0000 (стереть), подождать не менее 5 мс (берём 10), записать новое значение, снова подождать.
// Прочитать значение обратно сразу нельзя: датчик отдаёт старое до перезапуска питания.
bool AlashMLX90614::setEmissivity(float e) {
  if (!(e >= 0.1f && e <= 1.0f)) { _err = ERR_BAD_VALUE; return false; }
  uint16_t raw = (uint16_t)(e * 65535.0f + 0.5f);
  if (!writeWord(REG_EMISS, 0)) return false;
  delay(10);
  if (!writeWord(REG_EMISS, raw)) return false;
  delay(10);
  _err = ERR_OK;
  return true;
}

bool AlashMLX90614::readId(uint16_t id[4]) {
  for (uint8_t i = 0; i < 4; i++) if (!readWord(REG_ID1 + i, id[i])) return false;
  return true;
}

bool AlashMLX90614::isConnected() {
  _wire->beginTransmission(_addr);
  return _wire->endTransmission() == 0;
}

const __FlashStringHelper *AlashMLX90614::errorText() {
  switch (_err) {
    case ERR_OK: return F("OK");
    case ERR_NO_DEVICE:
      return T("Датчик не отвечает. Проверьте питание (модуль GY-906 на 3.3 В или 5 В, голый датчик MLX90614ESF-BAA только 3.3 В), провода SDA и SCL, адрес (обычно 0x5A) и подтяжки 4.7 кОм на SDA и SCL.",
               "Датчик жауап бермейді. Қуатты (GY-906 модулі 3.3 В немесе 5 В, жалаңаш MLX90614ESF-BAA тек 3.3 В), SDA және SCL сымдарын, мекенжайды (әдетте 0x5A) және SDA мен SCL-дегі 4.7 кОм тартқыштарды тексеріңіз.",
               "The sensor does not answer. Check the power (a GY-906 module takes 3.3 V or 5 V, a bare MLX90614ESF-BAA only 3.3 V), the SDA and SCL wires, the address (usually 0x5A) and the 4.7 kOhm pull-ups on SDA and SCL.");
    case ERR_I2C:
      return T("Ошибка обмена по I2C: датчик ответил не полностью. Проверьте контакты и длину проводов (до 30 см).",
               "I2C алмасу қатесі: датчик толық жауап бермеді. Түйіспелер мен сым ұзындығын (30 см-ге дейін) тексеріңіз.",
               "I2C exchange error: the sensor answered incompletely. Check the contacts and the wire length (up to 30 cm).");
    case ERR_PEC:
      return T("Контрольная сумма ответа не сошлась: помехи на шине. Укоротите провода, держите шину на 100 кГц (Wire.setClock(100000)), проверьте подтяжки и общий GND.",
               "Жауаптың бақылау сомасы сәйкес келмеді: шинада кедергі. Сымдарды қысқартыңыз, шинаны 100 кГц-те ұстаңыз (Wire.setClock(100000)), тартқыштар мен ортақ GND тексеріңіз.",
               "The reply checksum did not match: noise on the bus. Shorten the wires, keep the bus at 100 kHz (Wire.setClock(100000)), check the pull-ups and the common GND.");
    case ERR_SENSOR:
      return T("Значение вне допустимого диапазона или датчик сообщил об ошибке. Возможно, ИК-элемент повреждён, либо питание нестабильно (просадка при включении).",
               "Мән жарамды аралықтан тыс немесе датчик қате туралы хабарлады. ИК элемент бүлінген болуы мүмкін немесе қуат тұрақсыз.",
               "The value is out of range or the sensor reported an error. The IR element may be damaged, or the power is unstable (a dip at switch-on).");
    case ERR_BAD_VALUE:
      return T("Коэффициент излучения должен быть числом от 0.1 до 1.0 (кожа 0.98, чёрная краска 0.95, дерево 0.9, полированный металл 0.1-0.3).",
               "Сәуле шығару коэффициенті 0.1-ден 1.0-ге дейінгі сан болуы керек (тері 0.98, қара бояу 0.95, ағаш 0.9, жылтыратылған металл 0.1-0.3).",
               "The emissivity must be a number from 0.1 to 1.0 (skin 0.98, black paint 0.95, wood 0.9, polished metal 0.1-0.3).");
    case ERR_WRITE:
      return T("Датчик не подтвердил запись в EEPROM. Проверьте провода и питание, повторите.",
               "Датчик EEPROM-ға жазуды растамады. Сымдар мен қуатты тексеріп, қайталаңыз.",
               "The sensor did not acknowledge the EEPROM write. Check the wires and power and retry.");
  }
  return F("");
}

void AlashMLX90614::diagnose(Print &out) {
  out.println(T("AlashMLX90614: диагностика", "AlashMLX90614: диагностика", "AlashMLX90614: diagnostics"));
  out.print(T("Адрес: 0x", "Мекенжай: 0x", "Address: 0x")); out.println(_addr, HEX);
  if (!isConnected()) {
    _err = ERR_NO_DEVICE;
    out.println(errorText());
    return;
  }
  out.println(T("Датчик отвечает по I2C.", "Датчик I2C арқылы жауап береді.", "The sensor answers over I2C."));
  float ta = ambientTemperature(), to = objectTemperature();
  if (ta == ta) { out.print(T("Окружение: ", "Қоршаған орта: ", "Ambient: ")); out.print(ta, 2); out.println(F(" C")); } else out.println(errorText());
  if (to == to) { out.print(T("Объект: ", "Нысан: ", "Object: ")); out.print(to, 2); out.println(F(" C")); } else out.println(errorText());
  float em = emissivity();
  if (em == em) { out.print(T("Коэффициент излучения: ", "Сәуле шығару коэффициенті: ", "Emissivity: ")); out.println(em, 3); }
  uint16_t id[4];
  if (readId(id)) { out.print(F("ID: ")); for (uint8_t i = 0; i < 4; i++) { out.print((unsigned int)id[i], HEX); out.print(i < 3 ? F("-") : F("")); } out.println(F("")); }
  out.println(T("Подсказки: держите датчик в 2-5 см от объекта (у MLX90614 угол обзора 90 градусов у BAA и 35 градусов у BCC, чем дальше, тем больше «лишнего» в кадре). Блестящие металлы и стекло показывают неверно (коэффициент излучения 0.1-0.3). Медицинским термометром такой датчик без калибровки по эталону считать нельзя.",
                "Кеңестер: датчикті нысаннан 2-5 см-де ұстаңыз (MLX90614-тің көру бұрышы BAA-да 90, BCC-де 35 градус; қашықтаған сайын кадрға артық нәрсе көп түседі). Жылтыр металдар мен шыны қате көрсетеді (сәуле шығару коэффициенті 0.1-0.3). Мұндай датчикті эталон бойынша калибрлемей медициналық термометр деп санауға болмайды.",
                "Tips: hold the sensor 2-5 cm from the object (the MLX90614 field of view is 90 degrees for BAA and 35 degrees for BCC, the farther away the more of the surroundings gets in). Shiny metals and glass read wrongly (emissivity 0.1-0.3). Without calibration against a reference this sensor must not be considered a medical thermometer."));
}
