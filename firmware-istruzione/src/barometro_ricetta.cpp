#include "barometro_ricetta.h"

// Le stesse formule del firmware di volo (../firmware/src/baro.cpp), già provate sul razzo.

namespace Ricetta {

// ------------------------------------------------------------------- BMP180

// I nomi sono quelli del datasheet
static int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;
static uint16_t AC4, AC5, AC6;
static int32_t B5;   // calcolato dalla temperatura, serve alla pressione

bool bmp180Taratura(const uint8_t c[22]) {
  auto parola = [&](int i) { return (uint16_t)((c[i] << 8) | c[i + 1]); };
  // Nessun numero vale 0x0000 o 0xFFFF: sarebbe una memoria vuota o un filo staccato
  for (int i = 0; i < 22; i += 2) {
    if (parola(i) == 0x0000 || parola(i) == 0xFFFF) return false;
  }
  AC1 = parola(0);  AC2 = parola(2);  AC3 = parola(4);  AC4 = parola(6);
  AC5 = parola(8);  AC6 = parola(10); B1 = parola(12);  B2 = parola(14);
  MB = parola(16);  MC = parola(18);  MD = parola(20);
  Serial.printf("  AC1=%d AC2=%d AC3=%d AC4=%u AC5=%u AC6=%u\n", AC1, AC2, AC3, AC4, AC5, AC6);
  Serial.printf("  B1=%d B2=%d MB=%d MC=%d MD=%d\n", B1, B2, MB, MC, MD);
  return true;
}

float bmp180Temperatura(int32_t ut) {
  const int32_t x1 = ((ut - (int32_t)AC6) * (int32_t)AC5) >> 15;
  if (x1 + MD == 0) return NAN;
  const int32_t x2 = ((int32_t)MC * 2048) / (x1 + MD);
  B5 = x1 + x2;
  return ((B5 + 8) >> 4) / 10.0f;
}

float bmp180Pressione(int32_t up, uint8_t oss) {
  const int32_t b6 = B5 - 4000;
  int32_t x1 = (B2 * ((b6 * b6) >> 12)) >> 11;
  int32_t x2 = (AC2 * b6) >> 11;
  int32_t x3 = x1 + x2;
  const int32_t b3 = (((int32_t)AC1 * 4 + x3) * (1 << oss) + 2) >> 2;
  x1 = (AC3 * b6) >> 13;
  x2 = (B1 * ((b6 * b6) >> 12)) >> 16;
  x3 = (x1 + x2 + 2) >> 2;
  const uint32_t b4 = ((uint32_t)AC4 * (uint32_t)(x3 + 32768)) >> 15;
  if (b4 == 0) return NAN;
  const uint32_t b7 = (uint32_t)(up - b3) * (uint32_t)(50000 >> oss);
  int32_t p = b7 < 0x80000000 ? (int32_t)((b7 * 2) / b4) : (int32_t)((b7 / b4) * 2);
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  p += (x1 + x2 + 3791) >> 4;
  return p;
}

// ------------------------------------------------------------------- BMP280

static uint16_t T1, P1;
static int16_t T2, T3, P2, P3, P4, P5, P6, P7, P8, P9;
static int32_t tFine;   // calcolato dalla temperatura, serve alla pressione

bool bmp280Taratura(const uint8_t c[24]) {
  auto parola = [&](int i) { return (uint16_t)(c[i] | (c[i + 1] << 8)); };   // qui prima il byte basso
  if (parola(0) == 0x0000 || parola(0) == 0xFFFF) return false;
  T1 = parola(0);  T2 = parola(2);  T3 = parola(4);
  P1 = parola(6);  P2 = parola(8);  P3 = parola(10); P4 = parola(12); P5 = parola(14);
  P6 = parola(16); P7 = parola(18); P8 = parola(20); P9 = parola(22);
  Serial.printf("  T1=%u T2=%d T3=%d\n", T1, T2, T3);
  Serial.printf("  P1=%u P2=%d P3=%d P4=%d P5=%d P6=%d P7=%d P8=%d P9=%d\n", P1, P2, P3, P4, P5, P6, P7, P8, P9);
  return true;
}

float bmp280Temperatura(int32_t adcT) {
  const int32_t v1 = ((((adcT >> 3) - ((int32_t)T1 << 1))) * T2) >> 11;
  const int32_t v2 = (((((adcT >> 4) - (int32_t)T1) * ((adcT >> 4) - (int32_t)T1)) >> 12) * T3) >> 14;
  tFine = v1 + v2;
  return ((tFine * 5 + 128) >> 8) / 100.0f;
}

float bmp280Pressione(int32_t adcP) {
  int64_t a = (int64_t)tFine - 128000;
  int64_t b = a * a * P6;
  b += (a * P5) << 17;
  b += (int64_t)P4 << 35;
  a = ((a * a * P3) >> 8) + ((a * P2) << 12);
  a = ((((int64_t)1) << 47) + a) * P1 >> 33;
  if (a == 0) return NAN;
  int64_t p = 1048576 - adcP;
  p = (((p << 31) - b) * 3125) / a;
  a = ((int64_t)P9 * (p >> 13) * (p >> 13)) >> 25;
  b = ((int64_t)P8 * p) >> 19;
  p = ((p + a + b) >> 8) + ((int64_t)P7 << 4);
  return p / 256.0f;
}

// ------------------------------------------------------------------- quota

float quota(float pascal, float p0) {
  return 44330.0f * (1.0f - powf(pascal / p0, 0.190295f));
}

}  // namespace Ricetta
