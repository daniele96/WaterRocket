// Water Rocket · firmware «istruzione»
//
// Piccole demo dei sensori del razzo, una alla volta, sul monitor seriale (115200).
// Si sceglie con un tasto: da 1 a 9 e 0 le demo, m il menu, g il formato per i grafici.
// Ogni demo sta nel suo file demo_N_nome.cpp: è quello da mostrare.

#include <Arduino.h>

#include "aiuto.h"
#include "demo.h"
#include "pin.h"

struct Demo {
  char tasto;
  const char *nome;
  bool (*inizio)();   // si prepara una volta, come setup()
  void (*ripeti)();   // si ripete finché non si sceglie altro, come loop()
  void (*fine)();     // rimette a posto (può mancare)
};

static const Demo DEMO[] = {
    {'1', "chi c'è sul bus I2C", scannerInizio, scannerRipeti, nullptr},
    {'2', "barometro: pressione e quota", barometroInizio, barometroRipeti, nullptr},
    {'3', "accelerometro", accelerometroInizio, accelerometroRipeti, nullptr},
    {'4', "giroscopio", giroscopioInizio, giroscopioRipeti, nullptr},
    {'5', "cicalino", cicalinoInizio, cicalinoRipeti, nullptr},
    {'6', "anello LED", anelloInizio, anelloRipeti, nullptr},
    {'7', "microfono", microfonoInizio, microfonoRipeti, microfonoFine},
    {'8', "radio: le reti WiFi intorno", wifiInizio, wifiRipeti, wifiFine},
    {'9', "livella: accelerometro + anello", livellaInizio, livellaRipeti, nullptr},
    {'0', "decollo: la macchina a stati del razzo", decolloInizio, decolloRipeti, nullptr},
};

static const Demo *attiva = nullptr;

static void menu() {
  Serial.println();
  Serial.println("==================================================");
  Serial.println("  WATER ROCKET · MODALITÀ ISTRUZIONE");
  Serial.println("==================================================");
  for (const Demo &d : DEMO) Serial.printf("  %c   %s\n", d.tasto, d.nome);
  Serial.println();
  Serial.printf("  g   grafico per Teleplot (adesso %s)\n", modoGrafico ? "acceso" : "spento");
  Serial.println("  m   ferma la demo e torna qui");
  Serial.println("Premi un tasto.");
}

static void ferma() {
  if (attiva && attiva->fine) attiva->fine();
  attiva = nullptr;
  spegniTutto();
}

void setup() {
  // Per primo: finché il pin non è basso il cicalino potrebbe suonare
  preparaCicalino();
  anello.begin();
  anello.clear();
  anello.show();

  // Buffer grande: il menu e i titoli escono tutti insieme e senza spazio andrebbero persi
  Serial.setTxBufferSize(4096);
  Serial.begin(115200);
  const uint32_t inizio = millis();
  while (!Serial && millis() - inizio < 3000) delay(10);

  avviaBus();
  menu();
}

void loop() {
  if (Serial.available()) {
    const char tasto = Serial.read();
    if (tasto == '\r' || tasto == '\n' || tasto == ' ') return;

    if (tasto == 'g' || tasto == 'G') {
      modoGrafico = !modoGrafico;
      Serial.println(modoGrafico ? "Grafico acceso: righe «>nome:valore» per Teleplot."
                                 : "Grafico spento: torno al testo.");
      return;
    }

    ferma();
    const Demo *scelta = nullptr;
    for (const Demo &d : DEMO) {
      if (d.tasto == tasto) scelta = &d;
    }
    if (!scelta) {
      menu();
      return;
    }
    if (scelta->inizio()) {
      attiva = scelta;
    } else {
      Serial.println("La demo non è partita. m = menu");
      spegniTutto();
    }
    return;
  }

  if (attiva) attiva->ripeti();
}
