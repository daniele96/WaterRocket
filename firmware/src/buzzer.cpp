#include "buzzer.h"

#include <driver/gpio.h>

#include "board_pins.h"

namespace Buzzer {

static uint32_t s_onMs = 0;
static uint32_t s_offMs = 0;
static uint32_t s_left = 0;  // beep ancora da fare (0 con s_forever = infinito)
static bool s_forever = false;
static bool s_active = false;
static bool s_on = false;
static uint32_t s_since = 0;

static void set(bool on) {
  s_on = on;
  s_since = millis();
  digitalWrite(BUZZER_PIN, on ? HIGH : LOW);
}

void begin() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  // Il cicalino chiede ~25-30 mA: massima capacità di corrente del pin (40 mA)
  gpio_set_drive_capability((gpio_num_t)BUZZER_PIN, GPIO_DRIVE_CAP_3);
}

void beep(uint32_t onMs, uint32_t offMs, uint32_t count) {
  s_onMs = onMs;
  s_offMs = offMs;
  s_forever = count == 0;
  s_left = count;
  s_active = true;
  set(true);
}

void stop() {
  s_active = false;
  set(false);
}

bool isActive() { return s_active; }

void handle() {
  if (!s_active) return;
  const uint32_t elapsed = millis() - s_since;
  if (s_on) {
    if (elapsed < s_onMs) return;
    set(false);
    if (!s_forever && --s_left == 0) s_active = false;
  } else if (elapsed >= s_offMs) {
    set(true);
  }
}

}  // namespace Buzzer
