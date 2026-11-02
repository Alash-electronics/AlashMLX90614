// [ru] AlashMLX90614: датчик не работает или врёт? Скетч покажет, отвечает ли датчик, температуры, коэффициент излучения, номер и подскажет, что проверить.
//      Язык подсказок: LANG_RU, LANG_KK или LANG_EN.
// [kk] AlashMLX90614: датчик жұмыс істемей ме немесе өтірік айта ма? Скетч датчиктің жауап беретінін, температураларды, сәуле шығару коэффициентін, нөмірді көрсетеді және нені тексеру керегін айтады.
//      Кеңес тілі: LANG_RU, LANG_KK немесе LANG_EN.
// [en] AlashMLX90614: the sensor does not work or lies? The sketch shows whether the sensor answers, the temperatures, the emissivity, the ID and tells what to check.
//      Tips language: LANG_RU, LANG_KK or LANG_EN.
#include <AlashMLX90614.h>

AlashMLX90614 ir;

void setup() {
  Serial.begin(115200);
  ir.setLanguage(AlashMLX90614::LANG_RU);
  ir.begin();
  ir.diagnose(Serial);
}

void loop() {}
