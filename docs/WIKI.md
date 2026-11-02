# AlashMLX90614: руководство / нұсқаулық / guide

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для **бесконтактного ИК-термометра MLX90614 / GY-906** (I2C). Температура объекта, на который смотрит датчик, и окружения: кружка, радиатор, лоб, стена. Проверяет контрольную сумму, ловит выход за диапазон, умеет коэффициент излучения и поправку, объясняет, что не так. Только встроенная `Wire`. Сделана для уроков и наборов Alash Electronics.

### Установка
Arduino IDE: Скетч, Подключить библиотеку, Управлять библиотеками, найти **AlashMLX90614** и нажать Установить. Или скачайте ZIP с GitHub: Скетч, Подключить библиотеку, Добавить .ZIP библиотеку.

### Платы
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 и другие, где работает Arduino-ядро.

### Подключение
| GY-906 | Uno / Nano | ESP32 |
|---|---|---|
| VIN | 3.3 В или 5 В | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

Голый датчик MLX90614ESF-BAA питается только от 3.3 В; модуль GY-906 имеет стабилизатор и работает от 3.3 и 5 В. Нужны подтяжки 4.7 кОм на SDA и SCL (на модуле они есть). Шина должна быть 100 кГц, провода короткие.

### Быстрый старт
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

### Справочник по методам
| Метод | Что делает |
|---|---|
| `begin(wire, address)` | Запуск; адрес по умолчанию 0x5A |
| `objectTemperature() / ambientTemperature()` | Температура объекта / окружения, °C; NAN при ошибке |
| `objectTemperatureF() / ambientTemperatureF()` | То же в градусах Фаренгейта |
| `object2Temperature()` | Второй ИК-элемент (двухзонные модели) |
| `setOffset(deg) / offset()` | Поправка к температуре объекта (не пишется в датчик) |
| `emissivity() / setEmissivity(e)` | Коэффициент излучения 0.1..1.0; запись в EEPROM датчика |
| `rawObject() / rawAmbient()` | Сырые значения (0.02 К на единицу) |
| `readId(id[4]) / isConnected()` | Номер датчика / отвечает ли |
| `error() / errorText() / setLanguage()` | Ошибка и сообщение на ru/kk/en |
| `diagnose(Serial)` | Самопроверка с подсказками |

### Если что-то не работает
**Датчик не отвечает**

Проверьте питание (голый MLX90614ESF-BAA только 3.3 В), SDA и SCL, подтяжки 4.7 кОм и адрес 0x5A. Запустите пример `Diagnose`: он покажет, видит ли плата датчик.

**Ошибка контрольной суммы (ERR_PEC) или нестабильные значения**

Помехи на шине: укоротите провода (до 30 см), держите шину на 100 кГц (`Wire.setClock(100000)`), проверьте общий GND. MLX90614 работает по SMBus, быстрые 400 кГц ему не подходят.

**Блестящий предмет показывает неверную температуру**

У блестящих металлов и стекла малый коэффициент излучения (0.1-0.3): датчик видит отражения окружающих предметов. Заклейте точку измерения матовой лентой или чёрной краской, либо задайте коэффициент через `setEmissivity()`.

**Температура зависит от расстояния до объекта**

У датчика угол обзора (90 градусов у BAA, 35 у BCC): чем дальше, тем больше «лишнего» попадает в кадр и показание усредняется с окружением. Держите в 2-5 см или берите модель с узким углом.

**Новый коэффициент излучения не применился**

По даташиту значение записывается в EEPROM и вступает в силу после выключения и включения питания датчика; сразу после записи датчик ещё отдаёт старое. Не вызывайте `setEmissivity()` в `loop()`: ресурс EEPROM около 100 000 записей.

### Самопроверка
В библиотеке есть `diagnose(Serial)`: она печатает, что видит плата, и подсказывает, что проверить. Язык подсказок: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Примеры
`Basic`, `Thermometer`, `Emissivity`, `Diagnose`, `ESP32_CustomPins`

---

## Қазақша

**MLX90614 / GY-906 жанаспайтын ИК-термометріне** (I2C) арналған қарапайым кітапхана. Датчик қараған нысанның және қоршаған ортаның температурасы: кружка, радиатор, маңдай, қабырға. Бақылау сомасын тексереді, аралықтан шығуды ұстайды, сәуле шығару коэффициенті мен түзетуді біледі, не дұрыс емесін түсіндіреді. Тек кірістірілген `Wire`.

### Орнату
Arduino IDE: Sketch, Include Library, Manage Libraries, **AlashMLX90614** табып, Install басыңыз. Немесе GitHub-тан ZIP жүктеп, Sketch, Include Library, Add .ZIP Library.

### Тақталар
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 және Arduino ядросы бар басқалар.

### Қосу
| GY-906 | Uno / Nano | ESP32 |
|---|---|---|
| VIN | 3.3 В или 5 В | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

Жалаңаш MLX90614ESF-BAA тек 3.3 В-тан қоректенеді; GY-906 модулінде тұрақтандырғыш бар және 3.3 және 5 В-тан жұмыс істейді. SDA және SCL-ге 4.7 кОм тартқыштар керек (модульде бар). Шина 100 кГц, сымдар қысқа болуы керек.

### Жылдам бастау
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

### Әдістер анықтамасы
| Әдіс | Не істейді |
|---|---|
| `begin(wire, address)` | Іске қосу |
| `objectTemperature() / ambientTemperature()` | Нысан / орта температурасы |
| `objectTemperatureF() / ambientTemperatureF()` | Фаренгейтпен |
| `object2Temperature()` | Екінші ИК элемент |
| `setOffset(deg) / offset()` | Нысан температурасына түзету |
| `emissivity() / setEmissivity(e)` | Сәуле шығару коэффициенті |
| `rawObject() / rawAmbient()` | Шикі мәндер |
| `readId(id[4]) / isConnected()` | Датчик нөмірі |
| `error() / errorText() / setLanguage()` | Қате және хабарлама |
| `diagnose(Serial)` | Кеңестермен өзін-өзі тексеру |

### Бірдеңе жұмыс істемесе
**Датчик жауап бермейді**

Қуатты (жалаңаш MLX90614ESF-BAA тек 3.3 В), SDA және SCL, 4.7 кОм тартқыштар мен 0x5A мекенжайын тексеріңіз. `Diagnose` мысалын іске қосыңыз.

**Бақылау сомасы қатесі (ERR_PEC) немесе тұрақсыз мәндер**

Шинадағы кедергі: сымдарды қысқартыңыз (30 см-ге дейін), шинаны 100 кГц-те ұстаңыз (`Wire.setClock(100000)`), ортақ GND тексеріңіз.

**Жылтыр зат қате температура көрсетеді**

Жылтыр металдар мен шыныда сәуле шығару коэффициенті аз (0.1-0.3): датчик айналадағы заттардың шағылысын көреді. Өлшеу нүктесіне күңгірт таспа жапсырыңыз немесе қара бояумен бояңыз не `setEmissivity()` арқылы коэффициент беріңіз.

**Температура нысанға дейінгі қашықтыққа тәуелді**

Датчиктің көру бұрышы бар (BAA-да 90 градус, BCC-де 35): қашықтаған сайын кадрға артық нәрсе көп түсіп, көрсеткіш қоршаған ортамен орташаланады. 2-5 см-де ұстаңыз немесе тар бұрышты модельді алыңыз.

**Жаңа сәуле шығару коэффициенті қолданылмады**

Даташит бойынша мән EEPROM-ға жазылады және датчик қуатын өшіріп-қосқаннан кейін күшіне енеді; жазғаннан кейін бірден датчик ескісін береді. `setEmissivity()` функциясын `loop()` ішінде шақырмаңыз: EEPROM ресурсы шамамен 100 000 жазу.

### Өзін-өзі тексеру
Кітапханада `diagnose(Serial)` бар: тақта не көргенін басып шығарады және нені тексеру керегін айтады. Кеңес тілі: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Мысалдар
`Basic`, `Thermometer`, `Emissivity`, `Diagnose`, `ESP32_CustomPins`

---

## English

A simple library for the **MLX90614 / GY-906 non-contact IR thermometer** (I2C). The temperature of the object the sensor looks at and of the surroundings: a mug, a radiator, a forehead, a wall. Checks the checksum, catches out-of-range values, supports emissivity and an offset, and explains what is wrong. Only the built-in `Wire`. Made for classes and Alash Electronics kits.

### Installation
Arduino IDE: Sketch, Include Library, Manage Libraries, search **AlashMLX90614** and click Install. Or download the ZIP from GitHub: Sketch, Include Library, Add .ZIP Library.

### Boards
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 and others with an Arduino core.

### Wiring
| GY-906 | Uno / Nano | ESP32 |
|---|---|---|
| VIN | 3.3 В или 5 В | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |

A bare MLX90614ESF-BAA runs only on 3.3 V; the GY-906 module has a regulator and works on 3.3 and 5 V. 4.7 kOhm pull-ups on SDA and SCL are needed (the module has them). The bus should run at 100 kHz with short wires.

### Quick start
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

### Method reference
| Method | What it does |
|---|---|
| `begin(wire, address)` | Start; the default address is 0x5A |
| `objectTemperature() / ambientTemperature()` | Object / ambient temperature, °C; NAN on error |
| `objectTemperatureF() / ambientTemperatureF()` | The same in Fahrenheit |
| `object2Temperature()` | The second IR element (dual-zone models) |
| `setOffset(deg) / offset()` | Offset for the object temperature (not written to the sensor) |
| `emissivity() / setEmissivity(e)` | Emissivity 0.1..1.0; writes the sensor EEPROM |
| `rawObject() / rawAmbient()` | Raw values (0.02 K per unit) |
| `readId(id[4]) / isConnected()` | Sensor ID / does it answer |
| `error() / errorText() / setLanguage()` | Error and message in ru/kk/en |
| `diagnose(Serial)` | Self-check with tips |

### If something does not work
**The sensor does not answer**

Check the power (a bare MLX90614ESF-BAA takes only 3.3 V), SDA and SCL, the 4.7 kOhm pull-ups and the address 0x5A. Run the `Diagnose` example: it shows whether the board sees the sensor.

**Checksum error (ERR_PEC) or unstable values**

Noise on the bus: shorten the wires (up to 30 cm), keep the bus at 100 kHz (`Wire.setClock(100000)`), check the common GND. The MLX90614 speaks SMBus and does not like fast 400 kHz.

**A shiny object shows a wrong temperature**

Shiny metals and glass have a low emissivity (0.1-0.3): the sensor sees reflections of the surroundings. Cover the spot with matte tape or black paint, or set the coefficient with `setEmissivity()`.

**The temperature depends on the distance to the object**

The sensor has a field of view (90 degrees for BAA, 35 for BCC): the farther away, the more of the surroundings enters the view and the reading averages with it. Hold it 2-5 cm away or take a narrow-angle model.

**The new emissivity did not apply**

By the data sheet the value is written to the EEPROM and takes effect after the sensor power is cycled; right after the write the sensor still returns the old one. Do not call `setEmissivity()` in `loop()`: the EEPROM endurance is about 100,000 writes.

### Self-check
The library has `diagnose(Serial)`: it prints what the board sees and tells what to check. Tips language: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Examples
`Basic`, `Thermometer`, `Emissivity`, `Diagnose`, `ESP32_CustomPins`

---

MIT License, Alash Electronics. https://github.com/Alash-electronics/AlashMLX90614
