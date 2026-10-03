#pragma once
#include <Arduino.h>

namespace AudioRec {

static constexpr uint32_t SAMPLE_RATE = 16000;

// Inizializza il microfono PDM. Da chiamare una volta.
bool begin();
bool isReady();

// Avvia un task che registra in <dir>/audio.wav (16 kHz, 16 bit, mono).
bool start(const String &dir);
// Ferma il task e chiude il file (bloccante finché il file è chiuso).
void stop();

bool isRunning();
float seconds();
uint32_t errors();

}  // namespace AudioRec
