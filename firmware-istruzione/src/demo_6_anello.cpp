// Demo 6 · Anello LED WS2812B: i colori e i bit che viaggiano sul filo.

#include "aiuto.h"
#include "demo.h"
#include "pin.h"

struct Colore {
  const char *nome;
  uint8_t rosso, verde, blu;   // da 0 a 255
};

// Valori bassi (40 su 255): si vede bene e la porta USB non si affatica
static const Colore COLORI[] = {
    {"rosso", 40, 0, 0},    {"verde", 0, 40, 0},     {"blu", 0, 0, 40},    {"giallo", 40, 40, 0},
    {"ciano", 0, 40, 40},   {"magenta", 40, 0, 40},  {"bianco", 40, 40, 40},
};

// Un numero da 0 a 255 scritto con i suoi 8 bit
static void inBit(uint8_t valore, char testo[9]) {
  for (int i = 0; i < 8; i++) testo[i] = (valore >> (7 - i)) & 1 ? '1' : '0';
  testo[8] = 0;
}

bool anelloInizio() {
  titolo("6 · ANELLO LED WS2812B");
  Serial.println("Ogni LED riceve 24 bit: 8 per il verde, 8 per il rosso, 8 per il blu, in quest'ordine.");
  Serial.println("Luce che si somma: rosso + verde = giallo, tutti e tre = bianco.");
  return true;
}

void anelloRipeti() {
  for (const Colore &c : COLORI) {
    anello.fill(anello.Color(c.rosso, c.verde, c.blu));
    anello.show();   // adesso partono 8 LED × 24 bit = 192 bit sul filo D3
    char g[9], r[9], b[9];
    inBit(c.verde, g);
    inBit(c.rosso, r);
    inBit(c.blu, b);
    Serial.printf("%-8s rosso %2d + verde %2d + blu %2d  → bit per ogni LED: %s %s %s\n", c.nome, c.rosso,
                  c.verde, c.blu, g, r, b);
    if (aspetta(1500)) return;
  }

  Serial.println("Ora un LED alla volta: ognuno tiene il suo pacchetto e passa avanti gli altri.");
  for (int giro = 0; giro < 2; giro++) {
    for (int i = 0; i < NUM_LED; i++) {
      anello.clear();
      anello.setPixelColor(i, anello.Color(0, 0, 60));   // solo il LED numero i è acceso
      anello.show();
      if (aspetta(150)) return;
    }
  }
}
