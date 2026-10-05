#include "led_ring.h"

#include <Adafruit_NeoPixel.h>

#include <atomic>

#include "board_pins.h"

namespace LedRing {

// Ogni quanto si ripete l'immagine anche se non cambia: se l'anello si accende dopo la
// XIAO o perde un comando (falso contatto, disturbo) si rimette a posto da solo.
// Senza questo, all'accensione il primo comando andava perso e l'anello restava spento.
static constexpr uint32_t REFRESH_MS = 500;
static constexpr uint32_t ANIM_MS = 20;      // 50 aggiornamenti al secondo per gli effetti
static constexpr uint32_t PULSE_MS = 1500;   // periodo del respiro
static constexpr uint32_t FLASH_ON_MS = 200;  // come il cicalino: 200 ms ogni secondo
static constexpr uint32_t FLASH_PERIOD_MS = 1000;

// La libreria usa la periferica RMT dell'ESP32-S3: libera anche con camera e microfono attivi
static Adafruit_NeoPixel s_ring(LED_RING_COUNT, LED_RING_PIN, NEO_GRB + NEO_KHZ800);
static Look s_last = {Effect::Off, 0, 0, 0};
static uint32_t s_lastShow = 0;
static std::atomic<bool> s_manualOn = false;
static std::atomic<uint32_t> s_manualRgb = 0xFFFFFF;
static std::atomic<uint8_t> s_manualPct = 10;

void setManual(bool on, uint32_t rgb, uint8_t pct) {
  s_manualRgb = rgb & 0xFFFFFF;
  s_manualPct = min<uint8_t>(pct, 100);
  s_manualOn = on;
}

Manual manual() { return {s_manualOn, s_manualRgb, s_manualPct}; }

void begin() {
  s_ring.begin();
  s_ring.clear();
  s_ring.show();
  s_lastShow = millis();
}

// Luminosità del momento (0-255) per l'effetto, a partire da quella massima
static uint8_t level(const Look &l, uint32_t now) {
  const uint32_t max = l.pct * 255 / 100;
  switch (l.effect) {
    case Effect::Off: return 0;
    case Effect::Solid: return max;
    case Effect::Pulse: {
      // Da 15% a 100% della luminosità massima, con andamento a coseno
      const float ph = (float)((now - l.t0) % PULSE_MS) / PULSE_MS;
      return max * (0.15f + 0.85f * (0.5f - 0.5f * cosf(2 * PI * ph)));
    }
    case Effect::Flash: return (now - l.t0) % FLASH_PERIOD_MS < FLASH_ON_MS ? max : 0;
  }
  return 0;
}

void handle(const Look &look) {
  const uint32_t now = millis();
  const bool animated = look.effect == Effect::Pulse || look.effect == Effect::Flash;
  const uint32_t every = animated ? ANIM_MS : REFRESH_MS;
  if (look == s_last && now - s_lastShow < every) return;
  s_last = look;

  const uint8_t v = level(look, now);
  // Luminosità applicata ai canali (setBrightness() della libreria è distruttivo, meglio evitarlo)
  const uint8_t r = ((look.rgb >> 16) & 0xFF) * v / 255;
  const uint8_t g = ((look.rgb >> 8) & 0xFF) * v / 255;
  const uint8_t b = (look.rgb & 0xFF) * v / 255;
  s_ring.fill(s_ring.Color(r, g, b));
  s_ring.show();
  s_lastShow = now;
}

void pinTest() {
  // D3 come uscita normale: livello fisso misurabile col multimetro tra D3 (o DI) e GND
  pinMode(LED_RING_PIN, OUTPUT);
  digitalWrite(LED_RING_PIN, HIGH);
  Serial.println("Test D3: ALTO (3,3 V) per 10 s - misura tra DI dell'anello e GND");
  delay(10000);
  digitalWrite(LED_RING_PIN, LOW);
  Serial.println("Test D3: BASSO (0 V) per 10 s");
  delay(10000);
  begin();  // il pin torna alla libreria NeoPixel; il loop() ridisegna subito
  s_last = {Effect::Off, 0, 0, 0};
  Serial.println("Test D3 finito: anello di nuovo pilotato");
}

}  // namespace LedRing
