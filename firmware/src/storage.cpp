#include "storage.h"

#include <SD.h>
#include <SPI.h>

#include "board_pins.h"

namespace Storage {

static bool s_mounted = false;

bool begin() {
  // Prima prova a 20 MHz (serve banda per il video), poi ripiega sul default
  const uint32_t freqs[] = {20000000, 4000000};
  for (uint32_t f : freqs) {
    if (SD.begin(SD_PIN_CS, SPI, f)) {
      s_mounted = SD.cardType() != CARD_NONE;
      if (s_mounted) {
        log_i("SD montata a %lu MHz", (unsigned long)(f / 1000000));
        return true;
      }
    }
    SD.end();
  }
  log_e("SD non trovata");
  return false;
}

bool isMounted() { return s_mounted; }

bool format() {
  SD.end();
  s_mounted = false;
  // Il driver formatta solo se non trova un volume FAT (es. SD exFAT o vergine).
  // Con exFAT disabilitato nel core, f_mkfs(FM_ANY) crea FAT32 (FAT16 su schede piccole).
  if (!SD.begin(SD_PIN_CS, SPI, 20000000, "/sd", 5, true)) {
    log_e("Formattazione fallita");
    return false;
  }
  s_mounted = SD.cardType() != CARD_NONE;
  return s_mounted;
}

String newSessionDir() {
  if (!s_mounted) return "";
  char path[16];
  for (int i = 1; i <= 9999; i++) {
    snprintf(path, sizeof(path), "/REC_%04d", i);
    if (!SD.exists(path)) {
      return SD.mkdir(path) ? String(path) : String();
    }
  }
  return "";
}

void printInfo() {
  if (!s_mounted) {
    Serial.println("SD:        non montata");
    return;
  }
  static const char *types[] = {"NONE", "MMC", "SD", "SDHC", "UNKNOWN"};
  const uint8_t t = SD.cardType();
  Serial.printf("SD:        %s, %llu MB, liberi %llu MB\n", types[t <= 4 ? t : 4],
                SD.totalBytes() / (1024 * 1024),
                (SD.totalBytes() - SD.usedBytes()) / (1024 * 1024));
}

uint32_t writeSpeedTest() {
  if (!s_mounted) return 0;
  static const char *path = "/speedtest.bin";
  constexpr size_t CHUNK = 32 * 1024;  // simile a un frame VGA/HD
  constexpr size_t TOTAL = 4 * 1024 * 1024;
  uint8_t *buf = static_cast<uint8_t *>(ps_malloc(CHUNK));
  if (!buf) return 0;
  memset(buf, 0xA5, CHUNK);

  File f = SD.open(path, FILE_WRITE);
  if (!f) {
    free(buf);
    return 0;
  }
  const uint32_t t0 = millis();
  size_t written = 0;
  while (written < TOTAL) {
    const size_t w = f.write(buf, CHUNK);
    if (w != CHUNK) break;
    written += w;
  }
  f.close();
  const uint32_t ms = millis() - t0;
  SD.remove(path);
  free(buf);
  return written == TOTAL && ms ? (uint32_t)((uint64_t)TOTAL * 1000 / ms) : 0;
}

void listFiles() {
  if (!s_mounted) {
    Serial.println("SD non montata");
    return;
  }
  File root = SD.open("/");
  for (File dir = root.openNextFile(); dir; dir = root.openNextFile()) {
    if (!dir.isDirectory()) {
      Serial.printf("  %-24s %10u B\n", dir.name(), (unsigned)dir.size());
      continue;
    }
    Serial.printf("%s/\n", dir.name());
    for (File f = dir.openNextFile(); f; f = dir.openNextFile()) {
      Serial.printf("  %-24s %10u B\n", f.name(), (unsigned)f.size());
    }
  }
}

}  // namespace Storage
