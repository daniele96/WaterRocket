#include <Arduino.h>

// XIAO ESP32S3: LED utente arancione su GPIO21, attivo BASSO.
// Nota: sulla scheda Sense GPIO21 è anche il CS della microSD,
// quindi quando userai la SD non usare il LED.
static constexpr uint8_t PIN_LED = LED_BUILTIN;
static constexpr uint32_t BLINK_MS = 500;
static constexpr uint32_t HEARTBEAT_MS = 2000;

static void printChipInfo() {
  Serial.println();
  Serial.println("=== Water Rocket - XIAO ESP32S3 Sense ===");
  Serial.printf("Chip:      %s rev %d, %d core @ %lu MHz\n",
                ESP.getChipModel(), ESP.getChipRevision(),
                ESP.getChipCores(), (unsigned long)ESP.getCpuFreqMHz());
  Serial.printf("Flash:     %lu KB\n", (unsigned long)(ESP.getFlashChipSize() / 1024));
  Serial.printf("Heap:      %lu KB liberi\n", (unsigned long)(ESP.getFreeHeap() / 1024));
  if (psramFound()) {
    Serial.printf("PSRAM:     %lu KB (%lu KB liberi)\n",
                  (unsigned long)(ESP.getPsramSize() / 1024),
                  (unsigned long)(ESP.getFreePsram() / 1024));
  } else {
    Serial.println("PSRAM:     non trovata!");
  }
  Serial.printf("SDK:       %s\n", ESP.getSdkVersion());
  Serial.printf("Arduino:   %d.%d.%d\n", ESP_ARDUINO_VERSION_MAJOR,
                ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);  // spento

  Serial.begin(115200);
  // USB CDC: aspetta fino a 3 s che si apra il monitor seriale
  const uint32_t start = millis();
  while (!Serial && millis() - start < 3000) {
    delay(10);
  }

  printChipInfo();
}

void loop() {
  static uint32_t lastToggle = 0;
  static bool ledOn = false;

  if (millis() - lastToggle >= BLINK_MS) {
    lastToggle = millis();
    ledOn = !ledOn;
    digitalWrite(PIN_LED, ledOn ? LOW : HIGH);
  }

  // Heartbeat periodico: utile perché il messaggio di avvio si perde
  // se il monitor seriale si collega dopo il reset
  static uint32_t lastBeat = 0;
  if (millis() - lastBeat >= HEARTBEAT_MS) {
    lastBeat = millis();
    Serial.printf("[%8lu ms] alive - heap %lu KB, PSRAM libera %lu KB (Invio = info chip)\n",
                  (unsigned long)millis(), (unsigned long)(ESP.getFreeHeap() / 1024),
                  (unsigned long)(ESP.getFreePsram() / 1024));
  }

  // Qualsiasi carattere ricevuto ristampa le info del chip
  if (Serial.available()) {
    while (Serial.available()) Serial.read();
    printChipInfo();
  }
}
