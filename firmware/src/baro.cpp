#include "baro.h"

#include <Wire.h>

// Registri comuni ai due chip
static constexpr uint8_t REG_ID = 0xD0;
static constexpr uint8_t REG_RESET = 0xE0;  // scrivendo 0xB6 riparte come all'accensione
static constexpr uint8_t ID_BMP180 = 0x55, ID_BMP280 = 0x58, ID_BME280 = 0x60;

// Fuori da questo intervallo (da ~9000 m a ~-700 m di quota) la lettura è un errore del bus
static constexpr float P_MIN_PA = 30000, P_MAX_PA = 110000;

// --- BMP280 (datasheet Bosch BST-BMP280-DS001) ---
static constexpr uint8_t BMP280_REG_CALIB = 0x88;  // 24 byte di coefficienti di fabbrica
static constexpr uint8_t BMP280_REG_STATUS = 0xF3;
static constexpr uint8_t BMP280_REG_CTRL_MEAS = 0xF4;
static constexpr uint8_t BMP280_REG_CONFIG = 0xF5;
static constexpr uint8_t BMP280_REG_DATA = 0xF7;   // pressione (3 byte) + temperatura (3 byte)
// Modo continuo, pressione x8 e temperatura x1: una misura ogni ~23 ms (~44 Hz),
// rumore ~0,1 m. Filtro IIR a 2: smussa le raffiche senza ritardare troppo l'apogeo.
static constexpr uint8_t BMP280_CTRL_MEAS = (1 << 5) | (4 << 2) | 3;  // osrs_t x1, osrs_p x8, normal
static constexpr uint8_t BMP280_CONFIG = (0 << 5) | (1 << 2);          // standby 0,5 ms, IIR 2

// --- BMP180 (datasheet Bosch BST-BMP180-DS000) ---
// Non misura da solo: si avvia una conversione, si aspetta, si legge il risultato.
static constexpr uint8_t BMP180_REG_CALIB = 0xAA;  // 22 byte: 11 coefficienti big-endian
static constexpr uint8_t BMP180_REG_CTRL = 0xF4;
static constexpr uint8_t BMP180_REG_DATA = 0xF6;   // risultato dell'ultima conversione
static constexpr uint8_t BMP180_CMD_TEMP = 0x2E;
static constexpr uint8_t BMP180_CMD_PRESS = 0x34;
// Sovracampionamento 1 ("standard"): conversione in 7,5 ms al massimo, rumore ~0,4 m.
// Sta dentro i 10 ms del task di volo, quindi una pressione nuova a ogni giro (100 Hz).
static constexpr uint8_t BMP180_OSS = 1;
static constexpr uint32_t BMP180_TEMP_US = 4500, BMP180_PRESS_US = 7500;
// La temperatura cambia lentamente: il datasheet consiglia di rileggerla una volta al
// secondo. Quel giro non dà una pressione nuova e resta l'ultima.
static constexpr uint32_t BMP180_TEMP_EVERY_MS = 1000;
// Passa-basso sulla pressione, l'equivalente dell'IIR interno del BMP280: il rumore scende
// da ~0,4 a ~0,15 m con ~30 ms di ritardo. All'apogeo la velocità è ~0, il ritardo non conta.
static constexpr float BMP180_IIR_ALPHA = 0.25f;

namespace Baro {

enum class Chip : uint8_t { None, Bmp180, Bmp280, Bme280 };

static uint8_t s_addr = 0;
static Chip s_chip = Chip::None;
static bool s_ready = false;

// Coefficienti del BMP280
static uint16_t T1, P1;
static int16_t T2, T3, P2, P3, P4, P5, P6, P7, P8, P9;

// Coefficienti del BMP180 (MB c'è ma non entra nelle formule) e misura in corso
static int16_t AC1, AC2, AC3, B1, B2, MC, MD;
static uint16_t AC4, AC5, AC6;
enum class Conv : uint8_t { None, Temp, Press };
static Conv s_conv = Conv::None;
static uint32_t s_convStartUs = 0;
static uint32_t s_tempMs = 0;                 // millis() dell'ultima temperatura avviata
static int32_t s_b5 = 0;                      // dalla temperatura, serve alla pressione
static float s_pressPa = NAN, s_tempC = NAN;  // ultimi valori (pressione già filtrata)

static bool writeReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(s_addr);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

static bool readRegs(uint8_t reg, uint8_t *buf, size_t len) {
  Wire.beginTransmission(s_addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((uint16_t)s_addr, len) != len) return false;
  for (size_t i = 0; i < len; i++) buf[i] = Wire.read();
  return true;
}

// ---------------------------------------------------------------- BMP280

static bool beginBmp280() {
  writeReg(REG_RESET, 0xB6);
  delay(5);
  uint8_t st = 1;
  for (int i = 0; i < 20 && (st & 1); i++) {  // attende la copia dei coefficienti
    readRegs(BMP280_REG_STATUS, &st, 1);
    delay(2);
  }

  uint8_t c[24];
  if (!readRegs(BMP280_REG_CALIB, c, sizeof(c))) {
    log_e("BMP280: lettura dei coefficienti fallita");
    return false;
  }
  auto u16 = [&](int i) { return (uint16_t)(c[i] | (c[i + 1] << 8)); };
  T1 = u16(0);  T2 = u16(2);  T3 = u16(4);
  P1 = u16(6);  P2 = u16(8);  P3 = u16(10); P4 = u16(12); P5 = u16(14);
  P6 = u16(16); P7 = u16(18); P8 = u16(20); P9 = u16(22);

  if (!writeReg(BMP280_REG_CONFIG, BMP280_CONFIG) || !writeReg(BMP280_REG_CTRL_MEAS, BMP280_CTRL_MEAS)) {
    log_e("BMP280: configurazione fallita");
    return false;
  }
  delay(30);  // prima misura completa
  return true;
}

static bool readBmp280(float &pressPa, float &tempC) {
  uint8_t d[6];
  if (!readRegs(BMP280_REG_DATA, d, sizeof(d))) return false;
  const int32_t adcP = (d[0] << 12) | (d[1] << 4) | (d[2] >> 4);
  const int32_t adcT = (d[3] << 12) | (d[4] << 4) | (d[5] >> 4);
  if (adcP == 0x80000 || adcT == 0x80000) return false;  // valore di "nessuna misura"

  // Compensazione intera del datasheet (§8.2): temperatura in 0,01 °C, t_fine serve alla pressione
  int32_t v1 = ((((adcT >> 3) - ((int32_t)T1 << 1))) * T2) >> 11;
  int32_t v2 = (((((adcT >> 4) - (int32_t)T1) * ((adcT >> 4) - (int32_t)T1)) >> 12) * T3) >> 14;
  const int32_t tFine = v1 + v2;
  tempC = ((tFine * 5 + 128) >> 8) / 100.0f;

  int64_t a = (int64_t)tFine - 128000;
  int64_t b = a * a * P6;
  b += (a * P5) << 17;
  b += (int64_t)P4 << 35;
  a = ((a * a * P3) >> 8) + ((a * P2) << 12);
  a = ((((int64_t)1) << 47) + a) * P1 >> 33;
  if (a == 0) return false;
  int64_t p = 1048576 - adcP;
  p = (((p << 31) - b) * 3125) / a;
  a = ((int64_t)P9 * (p >> 13) * (p >> 13)) >> 25;
  b = ((int64_t)P8 * p) >> 19;
  p = ((p + a + b) >> 8) + ((int64_t)P7 << 4);
  pressPa = p / 256.0f;  // Q24.8
  return pressPa >= P_MIN_PA && pressPa <= P_MAX_PA;
}

// ---------------------------------------------------------------- BMP180

// Compensazione intera del datasheet (§3.5) nella forma del codice di riferimento Bosch.
// Gli spostamenti a sinistra di valori che possono essere negativi sono scritti come
// moltiplicazioni: in C++ shiftare a sinistra un negativo non è definito.
static bool bmp180Temp(int32_t ut, int32_t &b5, float &tempC) {
  const int32_t x1 = ((ut - (int32_t)AC6) * (int32_t)AC5) >> 15;
  if (x1 + MD == 0) return false;
  const int32_t x2 = ((int32_t)MC * 2048) / (x1 + MD);
  b5 = x1 + x2;
  tempC = ((b5 + 8) >> 4) / 10.0f;  // in 0,1 °C
  return tempC >= -40 && tempC <= 85;  // campo di funzionamento del sensore
}

static bool bmp180Press(int32_t up, int32_t b5, uint8_t oss, float &pressPa) {
  const int32_t b6 = b5 - 4000;
  int32_t x1 = (B2 * ((b6 * b6) >> 12)) >> 11;
  int32_t x2 = (AC2 * b6) >> 11;
  int32_t x3 = x1 + x2;
  const int32_t b3 = (((int32_t)AC1 * 4 + x3) * (1 << oss) + 2) >> 2;
  x1 = (AC3 * b6) >> 13;
  x2 = (B1 * ((b6 * b6) >> 12)) >> 16;
  x3 = (x1 + x2 + 2) >> 2;
  const uint32_t b4 = ((uint32_t)AC4 * (uint32_t)(x3 + 32768)) >> 15;
  if (b4 == 0) return false;
  const uint32_t b7 = (uint32_t)(up - b3) * (uint32_t)(50000 >> oss);
  int32_t p = b7 < 0x80000000 ? (int32_t)((b7 * 2) / b4) : (int32_t)((b7 / b4) * 2);
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  p += (x1 + x2 + 3791) >> 4;
  pressPa = p;
  return pressPa >= P_MIN_PA && pressPa <= P_MAX_PA;
}

static bool finishBmp180Temp() {
  uint8_t d[2];
  if (!readRegs(BMP180_REG_DATA, d, sizeof(d))) return false;
  float t;
  if (!bmp180Temp((d[0] << 8) | d[1], s_b5, t)) return false;
  s_tempC = t;
  return true;
}

static bool finishBmp180Press() {
  uint8_t d[3];
  if (!readRegs(BMP180_REG_DATA, d, sizeof(d))) return false;
  const int32_t up = ((d[0] << 16) | (d[1] << 8) | d[2]) >> (8 - BMP180_OSS);
  float p;
  if (!bmp180Press(up, s_b5, BMP180_OSS, p)) return false;
  s_pressPa = isnan(s_pressPa) ? p : s_pressPa + (p - s_pressPa) * BMP180_IIR_ALPHA;
  return true;
}

// Un passo della misura a comando: raccoglie la conversione avviata al giro precedente
// (se ha avuto il tempo di finire) e avvia la prossima. Così il task non resta mai in attesa.
static bool stepBmp180() {
  bool ok = true;
  if (s_conv != Conv::None) {
    const bool isTemp = s_conv == Conv::Temp;
    if (micros() - s_convStartUs < (isTemp ? BMP180_TEMP_US : BMP180_PRESS_US)) return true;
    ok = isTemp ? finishBmp180Temp() : finishBmp180Press();
    s_conv = Conv::None;
  }
  const bool tempDue = millis() - s_tempMs >= BMP180_TEMP_EVERY_MS;
  if (!writeReg(BMP180_REG_CTRL, tempDue ? BMP180_CMD_TEMP : BMP180_CMD_PRESS | (BMP180_OSS << 6))) {
    return false;
  }
  s_conv = tempDue ? Conv::Temp : Conv::Press;
  s_convStartUs = micros();
  if (tempDue) s_tempMs = millis();
  return ok;
}

static bool beginBmp180() {
  writeReg(REG_RESET, 0xB6);
  delay(10);  // tempo di avvio dopo il reset

  uint8_t c[22];
  if (!readRegs(BMP180_REG_CALIB, c, sizeof(c))) {
    log_e("BMP180: lettura dei coefficienti fallita");
    return false;
  }
  auto w = [&](int i) { return (uint16_t)((c[i] << 8) | c[i + 1]); };
  // Nessun coefficiente vale 0x0000 o 0xFFFF: sarebbe una EEPROM vuota o un bus guasto
  for (int i = 0; i < (int)sizeof(c); i += 2) {
    if (w(i) == 0x0000 || w(i) == 0xFFFF) {
      log_e("BMP180: coefficiente %d non valido (0x%04X)", i / 2, w(i));
      return false;
    }
  }
  AC1 = w(0);  AC2 = w(2);  AC3 = w(4);  AC4 = w(6);  AC5 = w(8);  AC6 = w(10);
  B1 = w(12);  B2 = w(14);  MC = w(18);  MD = w(20);

  // Prima misura completa a giri di 10 ms, come farà il task: temperatura, poi pressione
  s_conv = Conv::None;
  s_pressPa = s_tempC = NAN;
  s_tempMs = millis() - BMP180_TEMP_EVERY_MS;
  for (int i = 0; i < 3; i++) {
    if (!stepBmp180()) {
      log_e("BMP180: prima misura fallita");
      return false;
    }
    delay(10);
  }
  if (isnan(s_pressPa)) {
    log_e("BMP180: prima misura fuori scala");
    return false;
  }
  return true;
}

// ---------------------------------------------------------------- comune

bool begin() {
  uint8_t id = 0;
  for (uint8_t addr : {0x76, 0x77}) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() != 0) continue;
    s_addr = addr;
    if (readRegs(REG_ID, &id, 1) && (id == ID_BMP180 || id == ID_BMP280 || id == ID_BME280)) break;
    log_w("A 0x%02X risponde un dispositivo con ID 0x%02X: non è un BMP180/BMP280", addr, id);
    s_addr = 0;
  }
  if (!s_addr) {
    log_i("Altimetro non trovato (BMP180 a 0x77, BMP280 a 0x76 o 0x77)");
    return false;
  }

  s_chip = id == ID_BMP180 ? Chip::Bmp180 : id == ID_BMP280 ? Chip::Bmp280 : Chip::Bme280;
  s_ready = s_chip == Chip::Bmp180 ? beginBmp180() : beginBmp280();
  if (!s_ready) {
    s_chip = Chip::None;
    return false;
  }
  log_i("Altimetro %s a 0x%02X", model(), s_addr);
  return true;
}

bool isReady() { return s_ready; }
uint8_t address() { return s_addr; }

const char *model() {
  switch (s_chip) {
    case Chip::Bmp180: return "BMP180";
    case Chip::Bmp280: return "BMP280";
    case Chip::Bme280: return "BME280";
    default: return "nessuno";
  }
}

bool read(float &pressPa, float &tempC) {
  if (!s_ready) return false;
  if (s_chip != Chip::Bmp180) return readBmp280(pressPa, tempC);
  // Tra due pressioni nuove (giro della temperatura) si ripete l'ultima, come col BMP280
  if (!stepBmp180() || isnan(s_pressPa)) return false;
  pressPa = s_pressPa;
  tempC = s_tempC;
  return true;
}

float altitude(float pressPa, float p0Pa) {
  if (p0Pa <= 0) return 0;
  return 44330.0f * (1.0f - powf(pressPa / p0Pa, 0.190295f));
}

}  // namespace Baro
