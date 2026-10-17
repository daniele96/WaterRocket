// Demo 4 · Giroscopio: quanto velocemente gira, e sommando nel tempo, di quanto ha girato.

#include "aiuto.h"
#include "demo.h"
#include "mpu6050.h"

static float zeroX, zeroY, zeroZ;   // quello che dice da fermo: il suo errore
static float angoloZ;               // la somma delle rotazioni attorno a Z
static uint32_t ultimaLettura;

bool giroscopioInizio() {
  titolo("4 · GIROSCOPIO");
  if (!mpuSveglia(0)) return false;
  Serial.println("Fondo scala ±250 °/s: 1 grado al secondo vale 131 punti.");

  // Da fermo dovrebbe dire 0: quello che dice è il suo errore, e lo toglieremo da ogni misura
  Serial.println("Tieni ferma la scheda 1 secondo: misuro l'errore di zero...");
  long sx = 0, sy = 0, sz = 0;
  int n = 0;
  for (int i = 0; i < 50; i++) {
    int16_t g[7];
    if (mpuLeggi(g)) {
      sx += g[4];
      sy += g[5];
      sz += g[6];
      n++;
    }
    delay(20);
  }
  if (n == 0) return false;
  zeroX = sx / (MPU_PUNTI_PER_GRADO * n);
  zeroY = sy / (MPU_PUNTI_PER_GRADO * n);
  zeroZ = sz / (MPU_PUNTI_PER_GRADO * n);
  Serial.printf("Errore di zero: X %.2f  Y %.2f  Z %.2f °/s. Lo tolgo da ogni misura.\n", zeroX, zeroY, zeroZ);
  Serial.println("Ora ruota la scheda appoggiata sul tavolo: l'angolo Z conta di quanto hai girato.");
  angoloZ = 0;
  ultimaLettura = millis();
  return true;
}

void giroscopioRipeti() {
  float x = 0, y = 0, z = 0;
  // 5 letture ravvicinate (50 al secondo) per sommare bene l'angolo, una riga sola stampata
  for (int i = 0; i < 5; i++) {
    int16_t g[7];
    if (!mpuLeggi(g)) {
      Serial.println("Lettura fallita");
      aspetta(500);
      return;
    }
    const uint32_t adesso = millis();
    const float secondi = (adesso - ultimaLettura) / 1000.0f;
    ultimaLettura = adesso;
    x = g[4] / MPU_PUNTI_PER_GRADO - zeroX;
    y = g[5] / MPU_PUNTI_PER_GRADO - zeroY;
    z = g[6] / MPU_PUNTI_PER_GRADO - zeroZ;
    // velocità × tempo = quanto ho girato in questo pezzetto; sommando sempre si ha l'angolo
    angoloZ += z * secondi;
    if (aspetta(20)) return;
  }
  campo("X", x, "°/s", 1);
  campo("Y", y, "°/s", 1);
  campo("Z", z, "°/s", 1);
  campo("angolo Z", angoloZ, "°", 0);
  barretta(z, -250, 250);
  aCapo();
}
