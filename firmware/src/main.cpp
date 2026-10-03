// Water Rocket - registratore di volo per XIAO ESP32S3 Sense
//
// Registra video (MJPEG) e audio (WAV) sulla microSD in /REC_xxxx/.
// Avvio/stop: pulsante BOOT oppure comandi seriali:
//   r = avvia, s = ferma, i = info, l = elenca file,
//   h = aiuto, F = formatta SD (con conferma)
// Oppure dal browser: rete WiFi "WaterRocket" -> http://192.168.4.1
//
// Nota: il LED utente (GPIO21) non si usa perché condivide il pin con il CS della SD.

#include <Arduino.h>
#include <SD.h>

#include <mutex>

#include "audio_rec.h"
#include "board_pins.h"
#include "camera_rec.h"
#include "storage.h"
#include "web_server.h"

// true = inizia a registrare appena acceso (utile sul razzo, senza PC)
static constexpr bool AUTO_RECORD = false;
// true = crea la rete WiFi con la pagina web (scarica file, anteprima, player)
static constexpr bool WIFI_ENABLED = true;
static constexpr uint32_t STATUS_MS = 2000;
static constexpr uint32_t DEBOUNCE_MS = 50;

// Cartella del lancio in corso. Scritta solo dal loop(), letta anche dal task
// del web server: per questo l'accesso dall'esterno passa da un mutex.
static String s_sessionDir;
static std::mutex s_sessionMutex;

static String sessionDir() {
  std::lock_guard<std::mutex> lock(s_sessionMutex);
  return s_sessionDir;
}
static time_t s_startEpoch = 0;

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
  Serial.printf("Camera:    %s\n", CameraRec::isReady() ? CameraRec::sensorName() : "non inizializzata");
  Serial.printf("Microfono: %s\n", AudioRec::isReady() ? "ok" : "non inizializzato");
  Storage::printInfo();
  Web::printInfo();
}

static void printHelp() {
  Serial.println("Comandi: r = avvia registrazione, s = ferma, i = info, h = aiuto");
  Serial.println("         l = elenca file su SD, b = benchmark fps/SD");
  Serial.println("         F = formatta SD in FAT32 (chiede conferma)");
  Serial.println("Pulsante BOOT: avvia/ferma registrazione");
}

static bool isRecording() { return CameraRec::isRunning() || AudioRec::isRunning(); }

static void startRecording() {
  if (isRecording()) return;
  // La SD potrebbe essere stata inserita dopo l'avvio: riprova a montarla
  if (!Storage::isMounted() && !Storage::begin()) {
    Serial.println("Impossibile registrare: SD non montata (inserita? formattata FAT32?)");
    return;
  }
  const String dir = Storage::newSessionDir();
  {
    std::lock_guard<std::mutex> lock(s_sessionMutex);
    s_sessionDir = dir;
  }
  if (s_sessionDir.isEmpty()) {
    Serial.println("Impossibile creare la cartella di sessione");
    return;
  }
  // Ora di inizio (0 se l'orologio non è ancora stato impostato da NTP o dal browser).
  // info.txt viene riscritto completo allo stop; se salta la corrente resta almeno questo.
  const time_t now = time(nullptr);
  s_startEpoch = now > 1700000000 ? now : 0;
  File info = SD.open(s_sessionDir + "/info.txt", FILE_WRITE);
  if (info) {
    info.printf("started_epoch=%lld\nstatus=recording\n", (long long)s_startEpoch);
    info.close();
  }

  const bool cam = CameraRec::start(s_sessionDir);
  const bool mic = AudioRec::start(s_sessionDir);
  Serial.printf(">> REC avviata in %s (video: %s, audio: %s)\n", s_sessionDir.c_str(),
                cam ? "si" : "no", mic ? "si" : "no");
}

static void stopRecording() {
  if (!isRecording()) return;
  CameraRec::stop();
  AudioRec::stop();

  const uint32_t frames = CameraRec::frames();
  const float secs = CameraRec::elapsedMs() / 1000.0f;
  const float fps = secs > 0 ? frames / secs : 0;

  // Riepilogo della sessione, utile per convertire il video con il giusto framerate
  File info = SD.open(s_sessionDir + "/info.txt", FILE_WRITE);
  if (info) {
    info.printf("started_epoch=%lld\nstatus=complete\n", (long long)s_startEpoch);
    info.printf("camera=%s\nframes=%lu\nduration_s=%.2f\nfps=%.2f\naudio_s=%.2f\n",
                CameraRec::sensorName(), (unsigned long)frames, secs, fps, AudioRec::seconds());
    info.printf("errors_video=%lu\nerrors_audio=%lu\n", (unsigned long)CameraRec::errors(),
                (unsigned long)AudioRec::errors());
    info.close();
  }

  Serial.printf(">> REC fermata: %lu frame in %.1f s (%.1f fps), audio %.1f s\n",
                (unsigned long)frames, secs, fps, AudioRec::seconds());
  Serial.printf("   Converti con: ffmpeg -framerate %.1f -i video.mjpeg -i audio.wav "
                "-c:v libx264 -pix_fmt yuv420p -c:a aac -shortest volo.mp4\n", fps);
}

static void toggleRecording() {
  if (isRecording()) {
    stopRecording();
  } else {
    startRecording();
  }
}

static void handleButton() {
  static bool lastStable = HIGH;
  static bool lastRead = HIGH;
  static uint32_t lastChange = 0;

  const bool now = digitalRead(BTN_PIN_BOOT);
  if (now != lastRead) {
    lastRead = now;
    lastChange = millis();
  }
  if (millis() - lastChange >= DEBOUNCE_MS && now != lastStable) {
    lastStable = now;
    if (now == LOW) toggleRecording();  // fronte di pressione
  }
}

static void runBenchmark() {
  if (isRecording()) {
    Serial.println("Ferma prima la registrazione");
    return;
  }
  const uint32_t sd = Storage::writeSpeedTest();
  Serial.printf("Scrittura SD: %.0f KB/s\n", sd / 1024.0f);
  CameraRec::benchmark(sd);
}

static void formatCard() {
  if (isRecording()) {
    Serial.println("Ferma prima la registrazione");
    return;
  }
  Serial.println("Formattazione SD in FAT32... (può richiedere fino a un minuto)");
  if (Storage::format()) {
    Serial.println("SD pronta");
    Storage::printInfo();
  } else {
    Serial.println("Formattazione fallita");
  }
}

static void handleSerial() {
  // Il formato richiede conferma: 'F' arma, poi 'Y' entro 15 s
  static uint32_t formatArmedAt = 0;

  while (Serial.available()) {
    const char c = Serial.read();
    if (formatArmedAt && millis() - formatArmedAt > 15000) formatArmedAt = 0;
    if (formatArmedAt && c != '\r' && c != '\n') {
      formatArmedAt = 0;
      if (c == 'Y') {
        formatCard();
      } else {
        Serial.println("Formattazione annullata");
      }
      continue;
    }
    switch (c) {
      case 'r': case 'R': startRecording(); break;
      case 's': case 'S': stopRecording(); break;
      case 'i': case 'I': printChipInfo(); break;
      case 'l': case 'L': Storage::listFiles(); break;
      case 'b': case 'B': runBenchmark(); break;
      case 'h': case 'H': case '?': printHelp(); break;
      case 'F':
        Serial.println("ATTENZIONE: se la SD non è FAT (es. exFAT) verrà formattata e");
        Serial.println("tutti i dati andranno persi. Invia 'Y' (maiuscola) entro 15 s per confermare.");
        formatArmedAt = millis();
        break;
      default: break;
    }
  }
}

static void printStatus() {
  if (isRecording()) {
    const float secs = CameraRec::elapsedMs() / 1000.0f;
    Serial.printf("[REC %s] %6.1f s | video %lu frame (%.1f fps, err %lu) | audio %.1f s\n",
                  s_sessionDir.c_str(), secs, (unsigned long)CameraRec::frames(),
                  secs > 0 ? CameraRec::frames() / secs : 0.0f,
                  (unsigned long)CameraRec::errors(), AudioRec::seconds());
  } else {
    Serial.printf("[%8lu ms] in attesa - premi BOOT o invia 'r' per registrare (h = aiuto)\n",
                  (unsigned long)millis());
  }
}

void setup() {
  pinMode(BTN_PIN_BOOT, INPUT_PULLUP);

  Serial.begin(115200);
  // USB CDC: aspetta fino a 3 s che si apra il monitor seriale
  const uint32_t start = millis();
  while (!Serial && millis() - start < 3000) {
    delay(10);
  }

  Storage::begin();
  CameraRec::begin();
  AudioRec::begin();
  if (WIFI_ENABLED) {
    Web::begin({startRecording, stopRecording, isRecording, sessionDir});
  }

  printChipInfo();
  printHelp();

  if (AUTO_RECORD) startRecording();
}

void loop() {
  handleButton();
  handleSerial();
  Web::handle();

  static uint32_t lastStatus = 0;
  if (millis() - lastStatus >= STATUS_MS) {
    lastStatus = millis();
    printStatus();
  }
  delay(5);
}
