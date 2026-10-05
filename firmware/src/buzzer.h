#pragma once
#include <Arduino.h>

namespace Buzzer {

// Configura il pin del cicalino (attivo: basta dargli tensione).
void begin();

// count beep da onMs, separati da offMs. count = 0: ripete finché non si chiama stop().
void beep(uint32_t onMs, uint32_t offMs, uint32_t count);
void stop();
bool isActive();

// Da chiamare spesso dal loop(): accende e spegne secondo la sequenza in corso.
void handle();

}  // namespace Buzzer
