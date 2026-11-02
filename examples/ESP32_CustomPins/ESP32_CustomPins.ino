// [ru] AlashMLX90614 на ESP32 со своими пинами I2C: begin(SDA, SCL).
// [kk] AlashMLX90614 ESP32-де өз I2C пиндерімен: begin(SDA, SCL).
// [en] AlashMLX90614 on ESP32 with its own I2C pins: begin(SDA, SCL).
#include <AlashMLX90614.h>

AlashMLX90614 ir;

void setup() {
  Serial.begin(115200);
  if (!ir.begin(21, 22)) Serial.println(ir.errorText());   // SDA = GPIO21, SCL = GPIO22
}

void loop() {
  Serial.println(ir.objectTemperature(), 1);
  delay(1000);
}
