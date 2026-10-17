// Demo 7 · Microfono: 16 000 numeri al secondo, e da lì il volume.

#include <ESP_I2S.h>

#include "aiuto.h"
#include "demo.h"
#include "pin.h"

static I2SClass microfono;
static int16_t campioni[1600];   // 1600 campioni = 0,1 secondi di suono

bool microfonoInizio() {
  titolo("7 · MICROFONO");
  microfono.setPinsPdmRx(PIN_MIC_CLK, PIN_MIC_DATI);
  if (!microfono.begin(I2S_MODE_PDM_RX, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("Il microfono non parte: è una XIAO Sense, con la scheda della camera montata?");
    return false;
  }
  // Appena acceso il microfono dà un colpo: i primi 0,3 secondi si buttano
  for (int i = 0; i < 3; i++) microfono.readBytes((char *)campioni, sizeof(campioni));
  Serial.println("Il microfono manda 16 000 numeri al secondo: quanto è spinta l'aria in quell'istante.");
  Serial.println("Ogni decimo di secondo calcolo quanto «ballano» quei numeri: è il volume.");
  Serial.println("Prova a parlare, a fischiare, a battere le mani!");
  return true;
}

void microfonoRipeti() {
  const size_t quanti = microfono.readBytes((char *)campioni, sizeof(campioni)) / 2;
  if (quanti == 0) {
    aspetta(100);
    return;
  }

  // Il silenzio non è sempre 0: prima trovo il centro (la media)...
  double media = 0;
  for (size_t i = 0; i < quanti; i++) media += campioni[i];
  media /= quanti;
  // ...poi quanto si allontanano dal centro: la radice della media dei quadrati
  double somma = 0;
  int picco = 0;
  for (size_t i = 0; i < quanti; i++) {
    const double distanza = campioni[i] - media;
    somma += distanza * distanza;
    picco = max(picco, (int)fabs(distanza));
  }
  const float volume = sqrt(somma / quanti);

  // In decibel: 0 dB è il massimo che il microfono può dare, più è negativo più è piano
  const float decibel = 20 * log10f(max(volume, 1.0f) / 32768.0f);

  campo("volume", decibel, "dB", 0);
  campo("picco", picco, "", 0);
  barretta(decibel, -80, 0);
  aCapo();
}

void microfonoFine() { microfono.end(); }
