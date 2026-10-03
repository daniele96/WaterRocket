#include "camera_rec.h"

#include <SD.h>

#include <atomic>
#include <esp_camera.h>

#include "board_pins.h"

// Risoluzione/qualità: VGA a qualità 12 dà ~20-40 KB per frame,
// sostenibile su SD via SPI. Per più dettaglio prova FRAMESIZE_SVGA/HD.
static constexpr framesize_t FRAME_SIZE = FRAMESIZE_VGA;
static constexpr int JPEG_QUALITY = 12;  // 0-63, più basso = migliore
static constexpr uint32_t FLUSH_MS = 1000;

namespace CameraRec {

static bool s_ready = false;
static uint16_t s_pid = 0;
static std::atomic<bool> s_run = false;
static std::atomic<uint32_t> s_frames = 0;
static std::atomic<uint32_t> s_errors = 0;
static std::atomic<uint32_t> s_startMs = 0;
static std::atomic<uint32_t> s_stopMs = 0;
static TaskHandle_t s_task = nullptr;
static SemaphoreHandle_t s_done = nullptr;
static String s_dir;

static bool initCamera(framesize_t size) {
  camera_config_t c = {};
  c.ledc_channel = LEDC_CHANNEL_0;
  c.ledc_timer = LEDC_TIMER_0;
  c.pin_d0 = CAM_PIN_Y2;
  c.pin_d1 = CAM_PIN_Y3;
  c.pin_d2 = CAM_PIN_Y4;
  c.pin_d3 = CAM_PIN_Y5;
  c.pin_d4 = CAM_PIN_Y6;
  c.pin_d5 = CAM_PIN_Y7;
  c.pin_d6 = CAM_PIN_Y8;
  c.pin_d7 = CAM_PIN_Y9;
  c.pin_xclk = CAM_PIN_XCLK;
  c.pin_pclk = CAM_PIN_PCLK;
  c.pin_vsync = CAM_PIN_VSYNC;
  c.pin_href = CAM_PIN_HREF;
  c.pin_sccb_sda = CAM_PIN_SIOD;
  c.pin_sccb_scl = CAM_PIN_SIOC;
  c.pin_pwdn = CAM_PIN_PWDN;
  c.pin_reset = CAM_PIN_RESET;
  c.xclk_freq_hz = 20000000;
  c.pixel_format = PIXFORMAT_JPEG;
  c.frame_size = size;
  c.jpeg_quality = JPEG_QUALITY;
  c.fb_count = 2;
  c.fb_location = CAMERA_FB_IN_PSRAM;
  c.grab_mode = CAMERA_GRAB_LATEST;

  const esp_err_t err = esp_camera_init(&c);
  if (err != ESP_OK) {
    log_e("esp_camera_init fallita: 0x%x", err);
    return false;
  }

  sensor_t *s = esp_camera_sensor_get();
  s_pid = s->id.PID;
  if (s_pid == OV3660_PID) {
    // Stesse correzioni dell'esempio CameraWebServer
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }

  // I primi frame dopo l'init sono spesso sovraesposti: li scarto
  for (int i = 0; i < 5; i++) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) esp_camera_fb_return(fb);
  }
  return true;
}

bool begin() {
  if (!initCamera(FRAME_SIZE)) return false;
  s_done = xSemaphoreCreateBinary();
  s_ready = true;
  return true;
}

void benchmark(uint32_t sdBytesPerSec) {
  if (!s_ready || s_task) return;
  struct Mode { framesize_t size; const char *name; };
  static const Mode modes[] = {
      {FRAMESIZE_QVGA, "QVGA 320x240"},  {FRAMESIZE_VGA, "VGA 640x480"},
      {FRAMESIZE_SVGA, "SVGA 800x600"},  {FRAMESIZE_HD, "HD 1280x720"},
      {FRAMESIZE_SXGA, "SXGA 1280x1024"}, {FRAMESIZE_UXGA, "UXGA 1600x1200"},
      {FRAMESIZE_QXGA, "QXGA 2048x1536"},
  };
  Serial.printf("Benchmark camera %s, qualità JPEG %d (3 s per risoluzione)\n", sensorName(),
                JPEG_QUALITY);
  Serial.println("Risoluzione        fps camera   KB/frame   fps max con SD");
  for (const Mode &m : modes) {
    esp_camera_deinit();
    if (!initCamera(m.size)) {
      Serial.printf("%-18s  init fallita\n", m.name);
      continue;
    }
    uint32_t frames = 0;
    uint64_t bytes = 0;
    const uint32_t t0 = millis();
    while (millis() - t0 < 3000) {
      camera_fb_t *fb = esp_camera_fb_get();
      if (!fb) continue;
      bytes += fb->len;
      frames++;
      esp_camera_fb_return(fb);
    }
    const float fps = frames * 1000.0f / (millis() - t0);
    const float avg = frames ? (float)bytes / frames : 0;
    const float sdFps = avg > 0 ? sdBytesPerSec / avg : 0;
    Serial.printf("%-18s  %8.1f   %8.1f   %8.1f\n", m.name, fps, avg / 1024, min(fps, sdFps));
  }
  esp_camera_deinit();
  initCamera(FRAME_SIZE);
  Serial.println("Ripristinata la risoluzione di registrazione");
}

bool isReady() { return s_ready; }

const char *sensorName() {
  switch (s_pid) {
    case OV2640_PID: return "OV2640";
    case OV3660_PID: return "OV3660";
    case OV5640_PID: return "OV5640";
    default: return "sconosciuto";
  }
}

static void recordTask(void *) {
  File vid = SD.open(s_dir + "/video.mjpeg", FILE_WRITE);
  File idx = SD.open(s_dir + "/frames.csv", FILE_WRITE);
  if (!vid || !idx) {
    log_e("Impossibile creare i file video in %s", s_dir.c_str());
    s_errors++;
    s_run = false;
  } else {
    idx.println("frame,t_ms,bytes");
  }

  uint32_t lastFlush = millis();
  while (s_run) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      s_errors++;
      delay(5);
      continue;
    }
    const size_t len = fb->len;
    const size_t written = vid.write(fb->buf, len);
    esp_camera_fb_return(fb);

    if (written != len) {
      log_e("Scrittura SD fallita (SD piena o rimossa?)");
      s_errors++;
      break;
    }
    idx.printf("%lu,%lu,%u\n", (unsigned long)s_frames,
               (unsigned long)(millis() - s_startMs), (unsigned)len);
    s_frames++;

    // Flush periodico: se l'alimentazione salta (atterraggio...) si perde al massimo ~1 s
    if (millis() - lastFlush >= FLUSH_MS) {
      vid.flush();
      idx.flush();
      lastFlush = millis();
    }
  }

  s_stopMs = millis();
  if (vid) vid.close();
  if (idx) idx.close();
  s_run = false;
  s_task = nullptr;
  xSemaphoreGive(s_done);
  vTaskDelete(nullptr);
}

bool start(const String &dir) {
  if (!s_ready || s_task) return false;
  s_dir = dir;
  s_frames = 0;
  s_errors = 0;
  s_startMs = millis();
  s_stopMs = 0;
  s_run = true;
  // Core 0: il loop Arduino gira sul core 1
  if (xTaskCreatePinnedToCore(recordTask, "cam_rec", 8192, nullptr, 5, &s_task, 0) != pdPASS) {
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
uint32_t frames() { return s_frames; }
uint32_t errors() { return s_errors; }
uint32_t elapsedMs() {
  const uint32_t stopMs = s_stopMs;
  return (stopMs ? stopMs : millis()) - s_startMs;
}

}  // namespace CameraRec
