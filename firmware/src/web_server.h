#pragma once
#include <Arduino.h>

namespace Web {

// Azioni sulla registrazione, fornite da main.cpp.
// start/stop vengono chiamate solo dal loop() (vedi handle()), mai dal task
// del web server; isRecording e sessionDir devono essere thread-safe.
struct RecorderHooks {
  void (*start)();
  void (*stop)();
  bool (*isRecording)();
  String (*sessionDir)();
};

// Avvia l'access point WiFi (ed eventualmente la connessione alla rete di
// secrets.h) e il server HTTP asincrono sulla porta 80.
bool begin(const RecorderHooks &hooks);
// Da chiamare nel loop(): esegue i comandi arrivati dalla pagina web.
void handle();
void printInfo();

}  // namespace Web
