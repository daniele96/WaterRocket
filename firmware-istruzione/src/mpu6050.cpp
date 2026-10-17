#include "mpu6050.h"

#include <Wire.h>

#include "aiuto.h"

uint8_t mpuIndirizzo = 0x68;
static float puntiPerG = 16384;

bool mpuSveglia(uint8_t fondoScala) {
  if (presente(0x68)) {
    mpuIndirizzo = 0x68;
  } else if (presente(0x69)) {
    mpuIndirizzo = 0x69;
  } else {
    Serial.println("Nessun MPU6050 a 0x68 né a 0x69: controlla i fili (prova la demo 1).");
    return false;
  }

  // «Chi sei?»: il cassetto 0x75 contiene il nome del chip
  uint8_t chi = 0;
  leggiRegistri(mpuIndirizzo, 0x75, &chi, 1);
  Serial.printf("MPU6050 all'indirizzo 0x%02X. Cassetto 0x75 («chi sei?») = 0x%02X%s\n", mpuIndirizzo, chi,
                chi == 0x68 ? ", un originale" : ", un clone: funziona uguale");

  // Di fabbrica dorme: si sveglia scrivendo 0 nel cassetto 0x6B.
  // Cassetto 0x1C: fondo scala dell'accelerometro. 0x1B: del giroscopio (0 = ±250 °/s).
  // Cassetto 0x1A: filtro contro i tremolii (3 = taglia sopra i ~44 Hz).
  const bool ok = scriviRegistro(mpuIndirizzo, 0x6B, 0x00) &&
                  scriviRegistro(mpuIndirizzo, 0x1C, fondoScala << 3) &&
                  scriviRegistro(mpuIndirizzo, 0x1B, 0x00) &&
                  scriviRegistro(mpuIndirizzo, 0x1A, 0x03);
  if (!ok) {
    Serial.println("L'MPU6050 non accetta i comandi: controlla i fili.");
    return false;
  }
  puntiPerG = 16384 >> fondoScala;   // ±2 g: 16384 punti per g, ±4 g: 8192, ...
  delay(50);                         // il primo dato dopo il risveglio non è buono
  return true;
}

bool mpuLeggi(int16_t grezzi[7]) {
  // 1. «0x68, voglio leggere a partire dal cassetto 0x3B»
  Wire.beginTransmission(mpuIndirizzo);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) return false;

  // 2. «ecco 14 byte»: 7 numeri da 2 byte ciascuno, prima il byte alto poi il basso
  if (Wire.requestFrom((uint16_t)mpuIndirizzo, (size_t)14) != 14) return false;
  for (int i = 0; i < 7; i++) {
    const uint8_t alto = Wire.read();
    const uint8_t basso = Wire.read();
    grezzi[i] = (int16_t)((alto << 8) | basso);   // da -32768 a +32767
  }
  return true;
}

float mpuRiposo[3] = {0, 0, 1};

bool mpuTara() {
  Serial.println("Tieni ferma la scheda 1 secondo: taratura...");
  long somma[3] = {0, 0, 0};
  int n = 0;
  for (int i = 0; i < 50; i++) {
    int16_t g[7];
    if (mpuLeggi(g)) {
      for (int k = 0; k < 3; k++) somma[k] += g[k];
      n++;
    }
    delay(20);
  }
  if (n == 0) {
    Serial.println("Nessuna lettura riuscita: controlla i fili.");
    return false;
  }
  const float x = (float)somma[0] / n, y = (float)somma[1] / n, z = (float)somma[2] / n;
  const float totale = sqrtf(x * x + y * y + z * z);   // in punti
  Serial.printf("Da fermo dovrebbe leggere 1.00 g, e invece legge %.2f g.\n", totale / puntiPerG);
  Serial.printf("Correggo: d'ora in poi 1 g vale %.0f punti invece di %.0f.\n", totale, puntiPerG);
  puntiPerG = totale;
  mpuRiposo[0] = x / totale;
  mpuRiposo[1] = y / totale;
  mpuRiposo[2] = z / totale;
  return true;
}

float mpuPuntiPerG() { return puntiPerG; }
