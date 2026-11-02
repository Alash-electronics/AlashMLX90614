// [ru] AlashMLX90614: самый простой пример. Температура объекта (на что смотрит датчик) и окружения раз в секунду.
//      Датчик измеряет без контакта: направьте его на кружку, лоб, радиатор или стену.
//
//      Подключение (модуль GY-906): VIN к 3.3 В или 5 В, GND к GND, SDA к A4 (ESP32: GPIO21), SCL к A5 (ESP32: GPIO22).
//      Держите датчик в 2-5 см от объекта.
// [kk] AlashMLX90614: ең қарапайым мысал. Нысанның (датчик қараған жердің) және қоршаған ортаның температурасы секунд сайын.
//      Датчик жанаспай өлшейді: оны кружкаға, маңдайға, радиаторға немесе қабырғаға бағыттаңыз.
//
//      Қосу (GY-906 модулі): VIN 3.3 В немесе 5 В-қа, GND GND-ға, SDA A4-ке (ESP32: GPIO21), SCL A5-ке (ESP32: GPIO22).
//      Датчикті нысаннан 2-5 см-де ұстаңыз.
// [en] AlashMLX90614: the simplest example. The temperature of the object (what the sensor looks at) and of the surroundings once a second.
//      The sensor measures without contact: point it at a mug, a forehead, a radiator or a wall.
//
//      Wiring (GY-906 module): VIN to 3.3 V or 5 V, GND to GND, SDA to A4 (ESP32: GPIO21), SCL to A5 (ESP32: GPIO22).
//      Hold the sensor 2-5 cm from the object.
#include <AlashMLX90614.h>

AlashMLX90614 ir;

void setup() {
  Serial.begin(115200);
  if (!ir.begin()) Serial.println(ir.errorText());   // подсказка, что проверить
}

void loop() {
  float obj = ir.objectTemperature();    // °C, NAN если не удалось
  float amb = ir.ambientTemperature();
  if (obj == obj) {
    Serial.print(F("Object: ")); Serial.print(obj, 1);
    Serial.print(F(" C   Ambient: ")); Serial.print(amb, 1); Serial.println(F(" C"));
  } else {
    Serial.println(ir.errorText());
  }
  delay(1000);
}
