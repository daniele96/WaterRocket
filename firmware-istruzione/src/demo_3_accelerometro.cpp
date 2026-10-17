// Demo 3 · Accelerometro: la massa sulle molle, in numeri.
// Il dialogo con il chip è in mpu6050.cpp.

#include "aiuto.h"
#include "demo.h"
#include "mpu6050.h"

bool accelerometroInizio() {
  titolo("3 · ACCELEROMETRO");
  if (!mpuSveglia(0)) return false;   // fondo scala ±2 g: il più sensibile
  Serial.println("Fondo scala ±2 g: per il datasheet 1 g vale 16384 punti. g = l'accelerazione con cui cadono le cose.");
  if (!mpuTara()) return false;
  Serial.println("Prova: piatta sul tavolo (Z circa 1 g), sul fianco, capovolta, in caduta sul cuscino (0 g)!");
  return true;
}

void accelerometroRipeti() {
  int16_t grezzi[7];
  if (!mpuLeggi(grezzi)) {
    Serial.println("Lettura fallita");
    aspetta(500);
    return;
  }
  // Dai punti ai g: si divide per quanti punti vale 1 g
  const float x = grezzi[0] / mpuPuntiPerG();
  const float y = grezzi[1] / mpuPuntiPerG();
  const float z = grezzi[2] / mpuPuntiPerG();
  // Il totale, qualunque sia la direzione (Pitagora in 3D)
  const float totale = sqrtf(x * x + y * y + z * z);

  if (!modoGrafico) Serial.printf("Z grezzo %6d → ", grezzi[2]);
  campo("X", x, "g");
  campo("Y", y, "g");
  campo("Z", z, "g");
  campo("totale", totale, "g");
  barretta(totale, 0, 2);
  aCapo();
  aspetta(100);
}
