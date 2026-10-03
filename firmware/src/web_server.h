#pragma once
#include <Arduino.h>

namespace Web {

// Azioni sulla registrazione, fornite da main.cpp
struct RecorderHooks {
  void (*start)();
  void (*stop)();
  bool (*isRecording)();
  const String &(*sessionDir)();
};

// Avvia l'access point WiFi e il server HTTP sulla porta 80.
bool begin(const RecorderHooks &hooks);
// Da chiamare nel loop(): gestisce le richieste HTTP.
void handle();
void printInfo();

}  // namespace Web
