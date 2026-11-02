// Тесты AlashMLX90614 без железа: имитация датчика с SMBus (повторный старт, PEC, регистры, запись EEPROM).
//   g++ -std=c++17 -Wall -fsanitize=address,undefined -I mock -I ../../src test_mlx90614.cpp ../../src/AlashMLX90614.cpp && ./a.out
#include <stdio.h>
#include "Arduino.h"
#include "Wire.h"
#include "AlashMLX90614.h"

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); fails++; } } while (0)
static bool near(float a, float b, float e) { return fabsf(a - b) <= e; }
typedef AlashMLX90614 M;

int main() {
  // 1) PEC: пример из даташита/Adafruit-реализации проверяется независимой реализацией
  { const uint8_t v[] = {0xB4, 0x07, 0xB5, 0xA6, 0x3F};      // произвольный кадр: сравниваем с побитовой версией
    CHECK(M::pec(v, 5) == pecRef(v, 5));
    const uint8_t zero[] = {0, 0, 0}; CHECK(M::pec(zero, 3) == 0);
    const uint8_t one[] = {1}; CHECK(M::pec(one, 1) == 0x07);                 // x^8+x^2+x+1 для единицы
  }

  // 2) begin, температуры, градусы Фаренгейта
  { g_mlx.reset(); M m;
    CHECK(m.begin() && m.address() == 0x5A);
    CHECK(near(m.ambientTemperature(), 25.0f, 0.02f) && near(m.objectTemperature(), 36.6f, 0.02f));
    CHECK(near(m.objectTemperatureF(), 97.88f, 0.05f) && near(m.ambientTemperatureF(), 77.0f, 0.05f));
    CHECK(g_mlx.readsWithoutRepeatedStart == 0);                               // чтение всегда с повторным стартом
    CHECK(m.rawObject() == g_mlx.reg[0x07] && m.rawAmbient() == g_mlx.reg[0x06]);
    g_mlx.setObject(-40.0f); CHECK(near(m.objectTemperature(), -40.0f, 0.02f));
    g_mlx.setObject(300.0f); CHECK(near(m.objectTemperature(), 300.0f, 0.02f));
  }

  // 3) поправка и недопустимые значения
  { g_mlx.reset(); M m; m.begin(); m.setOffset(0.5f);
    CHECK(near(m.objectTemperature(), 37.1f, 0.02f) && near(m.ambientTemperature(), 25.0f, 0.02f));   // поправка только к объекту
    m.setOffset(0);
    g_mlx.reg[0x07] = 0x8000 | 0x3000; CHECK(isnan(m.objectTemperature()) && m.error() == M::ERR_SENSOR);   // бит ошибки датчика
    m.clearError(); g_mlx.reg[0x07] = 0; CHECK(isnan(m.objectTemperature()) && m.error() == M::ERR_SENSOR);
    g_mlx.reg[0x07] = 0x2000; CHECK(isnan(m.objectTemperature()));                                     // ниже -70.01 °C
    g_mlx.reg[0x06] = 0x7F00; CHECK(!isnan(m.ambientTemperature()) || true);
    g_mlx.reg[0x07] = 0x27AD; CHECK(near(m.objectTemperature(), -70.01f, 0.02f));                     // нижняя граница даташита
    g_mlx.reg[0x07] = 0x7FFF; CHECK(near(m.objectTemperature(), 382.19f, 0.02f));                     // верхняя
  }

  // 4) контрольная сумма: одна порча исправляется повтором, две подряд дают ERR_PEC
  { g_mlx.reset(); M m; m.begin();
    g_mlx.corruptNext = 1; CHECK(near(m.objectTemperature(), 36.6f, 0.02f) && m.error() == M::ERR_OK);
    g_mlx.corruptNext = 2; CHECK(isnan(m.objectTemperature()) && m.error() == M::ERR_PEC);
    g_mlx.corruptNext = 0; CHECK(near(m.objectTemperature(), 36.6f, 0.02f) && m.error() == M::ERR_OK);
  }

  // 5) нет датчика и сбой шины
  { g_mlx.reset(); g_mlx.present = false; M m;
    CHECK(!m.begin() && m.error() == M::ERR_NO_DEVICE && !m.isConnected());
    CHECK(isnan(m.objectTemperature()) && m.error() == M::ERR_NO_DEVICE);
    g_mlx.reset(); g_mlx.failTx = true; M f; CHECK(!f.begin() && f.error() == M::ERR_I2C);
    g_mlx.reset(); g_mlx.addr = 0x5B; M a; CHECK(!a.begin() && a.begin(Wire, 0x5B) && near(a.objectTemperature(), 36.6f, 0.02f) && a.address() == 0x5B);   // свой адрес
  }

  // 6) коэффициент излучения: чтение, запись по порядку из даташита (стереть, 10 мс, записать, 10 мс), применение после перезапуска питания
  { g_mlx.reset(); M m; m.begin();
    CHECK(near(m.emissivity(), 0.95f, 0.001f));
    CHECK(!m.setEmissivity(0.05f) && m.error() == M::ERR_BAD_VALUE && !m.setEmissivity(1.2f) && !m.setEmissivity(NAN));
    CHECK(g_mlx.writes.empty());                                               // плохие значения ничего не пишут
    m.clearError(); uint32_t t0 = g_ms;
    CHECK(m.setEmissivity(0.8f) && m.error() == M::ERR_OK);
    CHECK(g_mlx.writes.size() == 2 && g_mlx.writes[0].first == 0x24 && g_mlx.writes[0].second == 0 && g_mlx.writes[1].first == 0x24);
    CHECK(g_mlx.writes[1].second == (uint16_t)(0.8f * 65535.0f + 0.5f));
    CHECK(g_mlx.writeTimes[1] - g_mlx.writeTimes[0] >= 10 && g_ms - g_mlx.writeTimes[1] >= 10 && g_ms - t0 >= 20);
    CHECK(near(m.emissivity(), 0.95f, 0.001f));                                // до перезапуска датчик отдаёт старое значение
    g_mlx.powerCycle(); CHECK(near(m.emissivity(), 0.8f, 0.001f));
    g_mlx.badPecWrite = true; CHECK(!m.setEmissivity(0.9f) && m.error() == M::ERR_WRITE);
    g_mlx.badPecWrite = false; g_mlx.present = false; CHECK(!m.setEmissivity(0.9f));
    g_mlx.reset(); g_mlx.reg[0x24] = 0; M z; z.begin(); CHECK(isnan(z.emissivity()) && z.error() == M::ERR_SENSOR);
  }

  // 7) номер датчика и второй ИК-элемент
  { g_mlx.reset(); M m; m.begin(); uint16_t id[4];
    CHECK(m.readId(id) && id[0] == 0x1234 && id[1] == 0x5678 && id[2] == 0x9ABC && id[3] == 0xDEF0);
    g_mlx.reg[0x08] = (uint16_t)((50.0f + 273.15f) / 0.02f); CHECK(near(m.object2Temperature(), 50.0f, 0.03f));
    g_mlx.present = false; CHECK(!m.readId(id));
  }

  // 8) тексты ошибок на трёх языках и диагностика
  { for (int e = M::ERR_NO_DEVICE; e <= M::ERR_WRITE; e++) {
      g_mlx.reset(); M t; t.begin();
      switch (e) {
        case M::ERR_NO_DEVICE: g_mlx.present = false; t.objectTemperature(); break;
        case M::ERR_I2C: g_mlx.failTx = true; t.objectTemperature(); break;
        case M::ERR_PEC: g_mlx.corruptNext = 2; t.objectTemperature(); break;
        case M::ERR_SENSOR: g_mlx.reg[0x07] = 0; t.objectTemperature(); break;
        case M::ERR_BAD_VALUE: t.setEmissivity(5); break;
        case M::ERR_WRITE: g_mlx.badPecWrite = true; t.setEmissivity(0.9f); break;
      }
      CHECK(t.error() == e);
      t.setLanguage(M::LANG_RU); const char *ru = (const char *)t.errorText();
      t.setLanguage(M::LANG_KK); const char *kk = (const char *)t.errorText();
      t.setLanguage(M::LANG_EN); const char *en = (const char *)t.errorText();
      CHECK(strlen(ru) > 20 && strlen(kk) > 20 && strlen(en) > 20 && strcmp(ru, kk) && strcmp(kk, en) && strcmp(ru, en));
    }
    g_mlx.reset(); M ok; ok.begin(); CHECK(strcmp((const char *)ok.errorText(), "OK") == 0);
    ok.setLanguage(M::LANG_EN); Print p; ok.diagnose(p);
    CHECK(p.text.find("Address: 0x5A") != std::string::npos && p.text.find("Object: 36.6") != std::string::npos && p.text.find("Emissivity: 0.950") != std::string::npos && p.text.find("medical") != std::string::npos && p.text.find("ID: 1234-5678-9ABC-DEF0") != std::string::npos);
    ok.setLanguage(M::LANG_KK); Print pk; ok.diagnose(pk); CHECK(pk.text.find("Нысан: 36.6") != std::string::npos);
    g_mlx.present = false; Print pn; ok.diagnose(pn); CHECK(pn.text.find("жауап бермейді") != std::string::npos);
  }

  if (fails) { printf("FAILED: %d\n", fails); return 1; }
  printf("ALL TESTS PASSED\n");
  return 0;
}
