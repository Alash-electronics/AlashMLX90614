# AlashMLX90614

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для **бесконтактного ИК-термометра MLX90614 / GY-906** (I2C). Температура объекта, на который смотрит датчик, и окружения: кружка, радиатор, лоб, стена. Проверяет контрольную сумму, ловит выход за диапазон, умеет коэффициент излучения и поправку, объясняет, что не так. Только встроенная `Wire`. Сделана для уроков и наборов Alash Electronics.

```cpp
#include <AlashMLX90614.h>
AlashMLX90614 ir;

void setup() { Serial.begin(115200); ir.begin(); }
void loop() {
  Serial.println(ir.objectTemperature());   // °C того, на что смотрит датчик
  delay(1000);
}
```

### Что умеет
- **`objectTemperature()` и `ambientTemperature()`:** температура объекта и окружения в °C (и в °F), `NAN`, если прочитать не удалось.
- **Проверка данных:** контрольная сумма PEC с повтором, диапазон по даташиту (-70..382 °C для объекта) и бит ошибки датчика.
- **Коэффициент излучения:** `emissivity()` и `setEmissivity()` с записью в EEPROM по порядку из даташита (стереть, подождать, записать).
- **Поправка:** `setOffset()` подгоняет показания по эталонному термометру, не расходуя EEPROM.
- **Номер и двухзонные модели:** `readId()`, `object2Temperature()`.
- **Свой адрес и пины:** `begin(Wire, 0x5B)`, `begin(SDA, SCL)` на ESP.
- **Понятные причины:** `errorText()` и `diagnose(Serial)` с подсказками (питание, подтяжки, длина проводов, угол обзора, блестящие материалы) на трёх языках.
- Работает на **Arduino AVR, ESP32, ESP8266**.

### Подключение
| GY-906 | Uno / Nano | ESP32 |
|---|---|---|
| VIN | 3.3 В или 5 В | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

Голый датчик MLX90614ESF-BAA питается только от 3.3 В; модуль GY-906 имеет стабилизатор и работает от 3.3 и 5 В. Нужны подтяжки 4.7 кОм на SDA и SCL (на модуле они есть). Шина должна быть 100 кГц, провода короткие.

### Чем отличается от других библиотек
Мы изучили Adafruit MLX90614 (BSD), SparkFun MLX90614 и DFRobot:
- Adafruit не проверяет контрольную сумму PEC и возвращает NAN только для нуля; у нас PEC с повтором, диапазон и бит ошибки.
- Запись коэффициента излучения у Adafruit не проверяет ответ датчика и не предупреждает про EEPROM и перезапуск питания; у нас порядок из даташита и предупреждение.
- Adafruit тянет Adafruit_BusIO; у нас только `Wire`.
- Нет сообщений и самопроверки на русском и казахском; у нас `diagnose()` на ru/kk/en с подсказками про угол обзора и блестящие материалы.

### Примеры
`Basic`, `Thermometer`, `Emissivity`, `Diagnose`, `ESP32_CustomPins`

### Тесты
Без железа (с AddressSanitizer) на имитации датчика с SMBus: CRC8 PEC, повторный старт, температуры и граница диапазона (-70.01 и 382.19 °C), бит ошибки, одна и две испорченные суммы, нет датчика, свой адрес, запись коэффициента излучения (стереть, 10 мс, записать; старое значение до перезапуска питания), номер датчика, ошибки и диагностика: `extras/test`.

Подробное руководство: [docs/WIKI.md](docs/WIKI.md).

---

## Қазақша

**MLX90614 / GY-906 жанаспайтын ИК-термометріне** (I2C) арналған қарапайым кітапхана. Датчик қараған нысанның және қоршаған ортаның температурасы: кружка, радиатор, маңдай, қабырға. Бақылау сомасын тексереді, аралықтан шығуды ұстайды, сәуле шығару коэффициенті мен түзетуді біледі, не дұрыс емесін түсіндіреді. Тек кірістірілген `Wire`.

```cpp
#include <AlashMLX90614.h>
AlashMLX90614 ir;

void setup() { Serial.begin(115200); ir.begin(); }
void loop() {
  Serial.println(ir.objectTemperature());   // °C того, на что смотрит датчик
  delay(1000);
}
```

### Мүмкіндіктері
- **`objectTemperature()` және `ambientTemperature()`:** нысан мен қоршаған орта температурасы °C-пен (және °F-пен), оқу сәтсіз болса `NAN`.
- **Деректерді тексеру:** қайталауы бар PEC бақылау сомасы, даташит бойынша аралық (нысан үшін -70..382 °C) және датчик қате биті.
- **Сәуле шығару коэффициенті:** `emissivity()` және даташиттегі ретпен EEPROM-ға жазатын `setEmissivity()` (өшіру, күту, жазу).
- **Түзету:** `setOffset()` көрсеткіштерді эталон термометр бойынша түзетеді, EEPROM-ды жемейді.
- **Нөмір және екі аймақты модельдер:** `readId()`, `object2Temperature()`.
- **Өз мекенжайы және пиндері:** `begin(Wire, 0x5B)`, ESP-де `begin(SDA, SCL)`.
- **Түсінікті себептер:** `errorText()` және үш тілде кеңестері бар `diagnose(Serial)` (қуат, тартқыштар, сым ұзындығы, көру бұрышы, жылтыр материалдар).
- **Arduino AVR, ESP32, ESP8266** тақталарында жұмыс істейді.

### Қосу
| GY-906 | Uno / Nano | ESP32 |
|---|---|---|
| VIN | 3.3 В или 5 В | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

Жалаңаш MLX90614ESF-BAA тек 3.3 В-тан қоректенеді; GY-906 модулінде тұрақтандырғыш бар және 3.3 және 5 В-тан жұмыс істейді. SDA және SCL-ге 4.7 кОм тартқыштар керек (модульде бар). Шина 100 кГц, сымдар қысқа болуы керек.

### Басқа кітапханалардан айырмашылығы
Adafruit MLX90614 (BSD), SparkFun MLX90614 және DFRobot қарадық:
- Adafruit PEC бақылау сомасын тексермейді және NAN-ды тек нөл үшін қайтарады; бізде қайталауы бар PEC, аралық және қате биті.
- Adafruit-тің сәуле шығару коэффициентін жазуы датчик жауабын тексермейді және EEPROM мен қуатты қайта қосу туралы ескертпейді; бізде даташит реті және ескерту.
- Adafruit Adafruit_BusIO тартады; бізде тек `Wire`.
- Орысша және қазақша хабарлар мен өзін-өзі тексеру жоқ; бізде ru/kk/en `diagnose()`.

### Мысалдар
`Basic`, `Thermometer`, `Emissivity`, `Diagnose`, `ESP32_CustomPins`

### Тесттер
Құрылғысыз (AddressSanitizer-мен) SMBus бар датчик имитациясында: PEC CRC8, қайталама старт, температуралар және аралық шекарасы, қате биті, бір және екі бүлінген сома, датчик жоқ, өз мекенжайы, сәуле шығару коэффициентін жазу, датчик нөмірі, қателер: `extras/test`.

Толық нұсқаулық: [docs/WIKI.md](docs/WIKI.md).

---

## English

A simple library for the **MLX90614 / GY-906 non-contact IR thermometer** (I2C). The temperature of the object the sensor looks at and of the surroundings: a mug, a radiator, a forehead, a wall. Checks the checksum, catches out-of-range values, supports emissivity and an offset, and explains what is wrong. Only the built-in `Wire`. Made for classes and Alash Electronics kits.

```cpp
#include <AlashMLX90614.h>
AlashMLX90614 ir;

void setup() { Serial.begin(115200); ir.begin(); }
void loop() {
  Serial.println(ir.objectTemperature());   // °C того, на что смотрит датчик
  delay(1000);
}
```

### Features
- **`objectTemperature()` and `ambientTemperature()`:** the temperature of the object and of the surroundings in °C (and °F), `NAN` if reading failed.
- **Data checks:** the PEC checksum with a retry, the data-sheet range (-70..382 °C for the object) and the sensor error bit.
- **Emissivity:** `emissivity()` and `setEmissivity()` writing the EEPROM in the data-sheet order (erase, wait, write).
- **Offset:** `setOffset()` trims the readings against a reference thermometer without using up the EEPROM.
- **ID and dual-zone models:** `readId()`, `object2Temperature()`.
- **Own address and pins:** `begin(Wire, 0x5B)`, `begin(SDA, SCL)` on ESP.
- **Clear reasons:** `errorText()` and `diagnose(Serial)` with tips (power, pull-ups, wire length, field of view, shiny materials) in three languages.
- Works on **Arduino AVR, ESP32, ESP8266**.

### Wiring
| GY-906 | Uno / Nano | ESP32 |
|---|---|---|
| VIN | 3.3 В или 5 В | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

A bare MLX90614ESF-BAA runs only on 3.3 V; the GY-906 module has a regulator and works on 3.3 and 5 V. 4.7 kOhm pull-ups on SDA and SCL are needed (the module has them). The bus should run at 100 kHz with short wires.

### How it differs from other libraries
We studied Adafruit MLX90614 (BSD), SparkFun MLX90614 and DFRobot:
- Adafruit does not check the PEC checksum and returns NAN only for zero; ours has PEC with a retry, the range and the error bit.
- Adafruit's emissivity write does not check the sensor reply and does not warn about the EEPROM and the power cycle; ours follows the data-sheet order and warns.
- Adafruit pulls in Adafruit_BusIO; ours needs only `Wire`.
- No messages or self-check in Russian and Kazakh; ours has `diagnose()` in ru/kk/en.

### Examples
`Basic`, `Thermometer`, `Emissivity`, `Diagnose`, `ESP32_CustomPins`

### Tests
Without hardware (with AddressSanitizer) on a simulated SMBus sensor: PEC CRC8, repeated start, temperatures and the range limits (-70.01 and 382.19 °C), the error bit, one and two corrupted sums, no sensor, a custom address, the emissivity write (erase, 10 ms, write; the old value until a power cycle), the sensor ID, errors and diagnostics: `extras/test`.

Full guide: [docs/WIKI.md](docs/WIKI.md).

---

MIT License, Alash Electronics.
