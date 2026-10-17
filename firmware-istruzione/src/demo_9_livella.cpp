// Demo 9 · Livella: accelerometro + anello. Sento, decido, agisco.
// Si accende il LED che sta più in basso, come una pallina che rotola.

#include "aiuto.h"
#include "demo.h"
#include "mpu6050.h"
#include "pin.h"

// Dipende da come sono montati anello e accelerometro. Se la luce va dalla parte
// sbagliata, cambia ROTAZIONE (da 0 a 7) oppure VERSO (1 o -1).
static constexpr int ROTAZIONE = 0;
static constexpr int VERSO = 1;

static int giri = 0;

bool livellaInizio() {
  titolo("9 · LIVELLA: ACCELEROMETRO + ANELLO");
  if (!mpuSveglia(0)) return false;
  Serial.println("Appoggiala dove vuoi la «bolla»: la posizione di adesso diventa il piano.");
  if (!mpuTara()) return false;
  Serial.println("Sento (l'accelerometro) → decido (dov'è il basso?) → agisco (accendo quel LED).");
  Serial.println("Inclina la scheda: la luce rotola verso il basso. In piano diventa tutto verde.");
  return true;
}

void livellaRipeti() {
  int16_t grezzi[7];
  if (!mpuLeggi(grezzi)) {
    aspetta(200);
    return;
  }

  // SENTO: dove punta la gravità adesso
  const float x = grezzi[0] / mpuPuntiPerG();
  const float y = grezzi[1] / mpuPuntiPerG();
  const float z = grezzi[2] / mpuPuntiPerG();
  const float totale = sqrtf(x * x + y * y + z * z);

  // DECIDO: di quanto è inclinata rispetto alla «bolla» (l'angolo fra la gravità di adesso
  // e quella della taratura) e da che parte (come si è spostata lungo X e Y)
  const float coseno = (x * mpuRiposo[0] + y * mpuRiposo[1] + z * mpuRiposo[2]) / totale;
  const float gradi = acosf(constrain(coseno, -1.0f, 1.0f)) * 180 / PI;
  const float dx = x / totale - mpuRiposo[0];
  const float dy = y / totale - mpuRiposo[1];
  // L'accelerometro sente il tavolo che spinge in su: il basso è dalla parte opposta
  const float direzione = atan2f(-dy, -dx);   // da -π a +π
  const int led = ((int)lroundf(direzione / (2 * PI) * NUM_LED) * VERSO + ROTAZIONE + 2 * NUM_LED) % NUM_LED;

  // AGISCO
  anello.clear();
  if (gradi < 3) {
    anello.fill(anello.Color(0, 25, 0));   // in bolla: tutto verde
  } else {
    const int verde = max(0, 40 - (int)gradi);   // poco inclinata = gialla, tanto = rossa
    anello.setPixelColor(led, anello.Color(60, verde, 0));
  }
  anello.show();

  // La luce si aggiorna 20 volte al secondo, la scritta 5
  if (++giri % 4 == 0) {
    campo("inclinazione", gradi, "°", 0);
    if (!modoGrafico) {
      if (gradi < 3) Serial.print("in bolla!  ");
      else Serial.printf("→ LED %d    ", led + 1);
    }
    barretta(gradi, 0, 90);
    aCapo();
  }
  aspetta(50);
}
