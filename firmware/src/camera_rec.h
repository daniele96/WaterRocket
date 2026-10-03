#pragma once
#include <Arduino.h>

namespace CameraRec {

// Inizializza il sensore (OV2640 o OV3660). Da chiamare una volta.
bool begin();
bool isReady();
const char *sensorName();

// Avvia un task che salva i frame JPEG in <dir>/video.mjpeg
// e i timestamp in <dir>/frames.csv.
bool start(const String &dir);
// Ferma il task e chiude i file (bloccante finché i file sono chiusi).
void stop();

// Misura gli fps reali a ogni risoluzione e stima il limite imposto dalla SD
// (sdBytesPerSec = velocità di scrittura misurata). Solo a registrazione ferma.
void benchmark(uint32_t sdBytesPerSec);

bool isRunning();
uint32_t frames();
uint32_t errors();
uint32_t elapsedMs();

}  // namespace CameraRec
