#include "audio_rec.h"

#include <ESP_I2S.h>
#include <SD.h>

#include <atomic>

#include "board_pins.h"

// Il mic PDM della Sense ha un livello basso: amplifico in software.
// Se l'audio satura (es. durante la spinta) abbassa il guadagno.
static constexpr int32_t GAIN = 4;
static constexpr uint32_t HEADER_UPDATE_MS = 2000;
static constexpr size_t CHUNK_SAMPLES = 1024;

namespace AudioRec {

static I2SClass s_i2s;
static bool s_ready = false;
static std::atomic<bool> s_run = false;
static std::atomic<uint32_t> s_dataBytes = 0;
static std::atomic<uint32_t> s_errors = 0;
static TaskHandle_t s_task = nullptr;
static SemaphoreHandle_t s_done = nullptr;
static String s_dir;

static void writeWavHeader(File &f, uint32_t dataBytes) {
  const uint32_t byteRate = SAMPLE_RATE * 2;  // mono, 16 bit
  uint8_t h[44] = {'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E',
                   'f', 'm', 't', ' ', 16, 0, 0, 0, 1, 0, 1, 0};
  auto put32 = [&](int off, uint32_t v) {
    h[off] = v; h[off + 1] = v >> 8; h[off + 2] = v >> 16; h[off + 3] = v >> 24;
  };
  put32(4, 36 + dataBytes);
  put32(24, SAMPLE_RATE);
  put32(28, byteRate);
  h[32] = 2;   // block align
  h[34] = 16;  // bit per sample
  memcpy(&h[36], "data", 4);
  put32(40, dataBytes);
  f.write(h, sizeof(h));
}

bool begin() {
  s_i2s.setPinsPdmRx(MIC_PIN_CLK, MIC_PIN_DATA);
  if (!s_i2s.begin(I2S_MODE_PDM_RX, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    log_e("Init microfono PDM fallita");
    return false;
  }
  s_done = xSemaphoreCreateBinary();
  s_ready = true;
  return true;
}

bool isReady() { return s_ready; }

static void recordTask(void *) {
  static int16_t buf[CHUNK_SAMPLES];
  File f = SD.open(s_dir + "/audio.wav", FILE_WRITE);
  if (!f) {
    log_e("Impossibile creare audio.wav in %s", s_dir.c_str());
    s_errors++;
    s_run = false;
  } else {
    writeWavHeader(f, 0);
  }

  uint32_t lastHeader = millis();
  while (s_run) {
    const size_t n = s_i2s.readBytes((char *)buf, sizeof(buf));
    if (n == 0) continue;

    for (size_t i = 0; i < n / 2; i++) {
      const int32_t v = (int32_t)buf[i] * GAIN;
      buf[i] = (int16_t)constrain(v, -32768, 32767);
    }
    if (f.write((uint8_t *)buf, n) != n) {
      log_e("Scrittura audio fallita (SD piena o rimossa?)");
      s_errors++;
      break;
    }
    s_dataBytes += n;

    // Aggiorna l'header ogni tanto, così il WAV resta leggibile anche se
    // l'alimentazione salta prima dello stop
    if (millis() - lastHeader >= HEADER_UPDATE_MS) {
      const size_t pos = f.position();
      f.seek(0);
      writeWavHeader(f, s_dataBytes);
      f.seek(pos);
      f.flush();
      lastHeader = millis();
    }
  }

  if (f) {
    f.seek(0);
    writeWavHeader(f, s_dataBytes);
    f.close();
  }
  s_run = false;
  s_task = nullptr;
  xSemaphoreGive(s_done);
  vTaskDelete(nullptr);
}

bool start(const String &dir) {
  if (!s_ready || s_task) return false;
  s_dir = dir;
  s_dataBytes = 0;
  s_errors = 0;
  s_run = true;
  if (xTaskCreatePinnedToCore(recordTask, "audio_rec", 6144, nullptr, 6, &s_task, 0) != pdPASS) {
    s_run = false;
    return false;
  }
  return true;
}

void stop() {
  if (!s_task) return;
  s_run = false;
  xSemaphoreTake(s_done, portMAX_DELAY);
}

bool isRunning() { return s_task != nullptr; }
float seconds() { return s_dataBytes / (2.0f * SAMPLE_RATE); }
uint32_t errors() { return s_errors; }

}  // namespace AudioRec
