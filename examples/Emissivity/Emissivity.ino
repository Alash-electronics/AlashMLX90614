// [ru] AlashMLX90614: коэффициент излучения. Блестящие и прозрачные материалы показывают неверно, пока не указан их коэффициент:
//      кожа 0.98, чёрная краска 0.95, дерево 0.9, бумага 0.93, вода 0.96, окисленная сталь 0.8, полированный металл 0.1-0.3.
//      Отправьте число в монитор порта (115200), например 0.85. ВНИМАНИЕ: значение пишется в EEPROM датчика (около 100 000 записей),
//      действует после выключения и включения питания датчика и остаётся после перезагрузки. Не вызывайте setEmissivity() в loop().
// [kk] AlashMLX90614: сәуле шығару коэффициенті. Жылтыр және мөлдір материалдар коэффициентін көрсетпейінше қате көрсетеді:
//      тері 0.98, қара бояу 0.95, ағаш 0.9, қағаз 0.93, су 0.96, тотыққан болат 0.8, жылтыратылған металл 0.1-0.3.
//      Порт мониторына (115200) сан жіберіңіз, мысалы 0.85. НАЗАР: мән датчиктің EEPROM-ына жазылады (шамамен 100 000 жазу),
//      датчик қуатын өшіріп-қосқаннан кейін күшіне енеді және қайта жүктегеннен кейін қалады. setEmissivity() функциясын loop() ішінде шақырмаңыз.
// [en] AlashMLX90614: emissivity. Shiny and transparent materials read wrongly until their emissivity is set:
//      skin 0.98, black paint 0.95, wood 0.9, paper 0.93, water 0.96, oxidized steel 0.8, polished metal 0.1-0.3.
//      Send a number to the serial monitor (115200), for example 0.85. WARNING: the value is written to the sensor EEPROM (about 100,000 writes),
//      takes effect after the sensor power is cycled and stays after a reboot. Do not call setEmissivity() in loop().
#include <AlashMLX90614.h>

AlashMLX90614 ir;

void setup() {
  Serial.begin(115200);
  if (!ir.begin()) Serial.println(ir.errorText());
  Serial.print(F("Emissivity in the sensor: ")); Serial.println(ir.emissivity(), 3);
  Serial.println(F("Send a number 0.1 .. 1.0 to change it"));
}

void loop() {
  if (Serial.available()) {
    float e = Serial.parseFloat();
    while (Serial.available()) Serial.read();
    if (ir.setEmissivity(e)) Serial.println(F("Written. Power the sensor off and on to apply."));
    else Serial.println(ir.errorText());
  }
  float t = ir.objectTemperature();
  if (t == t) { Serial.print(t, 1); Serial.println(F(" C")); }
  delay(500);
}
