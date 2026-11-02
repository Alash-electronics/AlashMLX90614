// [ru] AlashMLX90614: бесконтактный термометр с порогом. Светодиод (пин 13 или встроенный) загорается, когда объект горячее 40 градусов,
//      например кружка с чаем или радиатор. Печатается среднее по 5 измерениям: так число меньше прыгает.
//      Это учебный пример, не медицинский прибор: для измерения температуры тела датчик нужно калибровать по эталонному термометру (setOffset).
// [kk] AlashMLX90614: шегі бар жанаспайтын термометр. Нысан 40 градустан ыстық болғанда жарықдиод (13 пин немесе кіріктірілген) жанады,
//      мысалы шай құйылған кружка немесе радиатор. 5 өлшемнің орташасы басылады: сонда сан аз секіреді.
//      Бұл оқу мысалы, медициналық құрал емес: дене температурасын өлшеу үшін датчикті эталон термометр бойынша калибрлеу керек (setOffset).
// [en] AlashMLX90614: a non-contact thermometer with a threshold. An LED (pin 13 or the built-in one) lights when the object is hotter than 40 degrees,
//      for example a cup of tea or a radiator. The average of 5 measurements is printed: the number jumps less.
//      This is a teaching example, not a medical device: to measure body temperature the sensor must be calibrated against a reference thermometer (setOffset).
#include <AlashMLX90614.h>

AlashMLX90614 ir;
const uint8_t LED = 13;                  // на ESP32 укажите свой пин светодиода
const float LIMIT = 40.0f;               // порог, °C

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  ir.begin();
  // ir.setOffset(-0.3);                 // подгонка по эталонному термометру: прибавляется к температуре объекта
}

void loop() {
  float sum = 0; uint8_t n = 0;
  for (uint8_t i = 0; i < 5; i++) {
    float t = ir.objectTemperature();
    if (t == t) { sum += t; n++; }
    delay(100);
  }
  if (n == 0) { Serial.println(ir.errorText()); delay(1000); return; }
  float avg = sum / n;
  digitalWrite(LED, avg > LIMIT ? HIGH : LOW);
  Serial.print(avg, 1); Serial.println(avg > LIMIT ? F(" C   HOT") : F(" C"));
}
