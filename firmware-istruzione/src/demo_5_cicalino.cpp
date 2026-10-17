// Demo 5 · Cicalino: la comunicazione più semplice che esista, un bit.

#include "aiuto.h"
#include "demo.h"
#include "pin.h"

bool cicalinoInizio() {
  titolo("5 · CICALINO");
  preparaCicalino();
  Serial.println("Il cicalino attivo ha dentro il suo oscillatore: basta dargli 1 (suona) o 0 (zitto).");
  Serial.println("Suona come al ritrovamento del razzo: 200 ms acceso, 800 ms spento.");
  return true;
}

void cicalinoRipeti() {
  Serial.println("pin D1 = 1 (3,3 V) → suona");
  digitalWrite(PIN_CICALINO, HIGH);
  if (aspetta(200)) return;

  Serial.println("pin D1 = 0 (0 V)   → silenzio");
  digitalWrite(PIN_CICALINO, LOW);
  aspetta(800);
}
