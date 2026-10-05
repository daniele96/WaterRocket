#pragma once
#include <Arduino.h>

// Anello WS2812B su D3. Il loop() decide l'aspetto (colore, effetto, luminosità)
// in base alla fase del volo e lo passa a handle(), che anima e ridisegna.
namespace LedRing {

enum class Effect : uint8_t {
  Off,
  Solid,  // colore fisso
  Pulse,  // respiro lento (registrazione in corso)
  Flash,  // lampo di 200 ms ogni secondo (ritrovamento, insieme al cicalino)
};

struct Look {
  Effect effect;
  uint32_t rgb;    // 0xRRGGBB
  uint8_t pct;     // luminosità 0-100. Sull'USB stare bassi: 8 LED bianchi al 100%
                   // chiedono ~480 mA, più di quanto dia la porta del PC
  uint32_t t0;     // millis() di riferimento per Pulse/Flash (allinea il lampo al cicalino)

  bool operator==(const Look &o) const {
    return effect == o.effect && rgb == o.rgb && pct == o.pct && t0 == o.t0;
  }
};

// Inizializza l'anello e lo lascia spento.
void begin();

// Da chiamare a ogni giro del loop() con l'aspetto voluto.
void handle(const Look &look);

// Controllo manuale dal pannello di prova: scavalca i colori automatici delle fasi.
// Thread-safe (lo chiama il web server, lo legge il loop()).
struct Manual {
  bool on;
  uint32_t rgb;
  uint8_t pct;  // 0 = spento
};
void setManual(bool on, uint32_t rgb, uint8_t pct);
Manual manual();

// Diagnostica (bloccante, 20 s): D3 fisso alto 10 s poi basso 10 s, da misurare col multimetro.
void pinTest();

}  // namespace LedRing
