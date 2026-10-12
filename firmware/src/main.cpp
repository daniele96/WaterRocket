// Water Rocket - registratore di volo per XIAO ESP32S3 Sense
//
// Registra video (MJPEG), audio (WAV) e accelerometro (sensors.csv) sulla microSD in /REC_xxxx/.
// Avvio/stop: pulsante BOOT oppure comandi seriali:
//   r = avvia, s = ferma, i = info, l = elenca file,
//   h = aiuto, F = formatta SD (con conferma)
//   a = dati accelerometro in continuo, v = simula decollo, q = riarma / zittisci
// Oppure dal browser: rete WiFi "WaterRocket" -> http://192.168.4.1
//
// L'MPU6050 riconosce decollo e atterraggio (flight.cpp): da atterrato il
// cicalino suona 200 ms ogni secondo per il ritrovamento, finché non si invia 'q'.
//
// Nota: il LED utente (GPIO21) non si usa perché condivide il pin con il CS della SD.

#include <Arduino.h>
#include <SD.h>

#include <mutex>

#include "audio_rec.h"
#include "baro.h"
#include "board_pins.h"
#include "buzzer.h"
#include "camera_rec.h"
#include "flight.h"
#include "led_ring.h"
#include "storage.h"
#include "web_server.h"

// true = inizia a registrare appena acceso (utile sul razzo, senza PC)
static constexpr bool AUTO_RECORD = false;
// true = crea la rete WiFi con la pagina web (scarica file, anteprima, player)
static constexpr bool WIFI_ENABLED = true;
static constexpr uint32_t STATUS_MS = 2000;
static constexpr uint32_t DEBOUNCE_MS = 50;
static constexpr uint32_t IMU_STREAM_MS = 100;  // comando 'a': 10 righe al secondo

static bool s_imuStream = false;

// Cartella del lancio in corso. Scritta solo dal loop(), letta anche dal task
// del web server: per questo l'accesso dall'esterno passa da un mutex.
static String s_sessionDir;
static std::mutex s_sessionMutex;

static String sessionDir() {
  std::lock_guard<std::mutex> lock(s_sessionMutex);
  return s_sessionDir;
}
static time_t s_startEpoch = 0;
static uint32_t s_launchesAtStart = 0;  // decolli contati all'avvio della registrazione

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
  if (Flight::isReady()) {
    Serial.printf("MPU6050:   ok (I2C 0x%02X)\n", Flight::address());
    if (Flight::hasBaro()) {
      Serial.printf("Altimetro: %s ok (I2C 0x%02X)\n", Baro::model(), Baro::address());
    } else {
      Serial.println("Altimetro: non trovato (BMP180 o BMP280)");
    }
  } else {
    Serial.println("MPU6050:   non trovato");
  }
  Storage::printInfo();
  Web::printInfo();
}

static void printHelp() {
  Serial.println("Comandi: r = avvia registrazione, s = ferma, i = info, h = aiuto");
  Serial.println("         l = elenca file su SD, b = benchmark fps/SD");
  Serial.println("         F = formatta SD in FAT32 (chiede conferma)");
  Serial.println("         a = dati accelerometro in continuo (on/off)");
  Serial.println("         v = simula il decollo, q = riarma a terra e zittisci il cicalino");
  Serial.println("         p = test del pin D3 dell'anello (10 s alto, 10 s basso)");
  Serial.println("         z = calibrazione pre-lancio (razzo fermo sulla rampa per 3 s)");
  Serial.println("Pulsante BOOT: avvia/ferma registrazione");
}

static bool isRecording() {
  return CameraRec::isRunning() || AudioRec::isRunning() || Flight::isLogging();
}

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

  s_launchesAtStart = Flight::launchCount();
  // Prima i sensori: il loro tempo 0 coincide (a pochi ms) con quello del video
  const bool imu = Flight::startLog(s_sessionDir);
  const bool cam = CameraRec::start(s_sessionDir);
  const bool mic = AudioRec::start(s_sessionDir);
  Serial.printf(">> REC avviata in %s (video: %s, audio: %s, sensori: %s)\n", s_sessionDir.c_str(),
                cam ? "si" : "no", mic ? "si" : "no", imu ? "si" : "no");
}

static void stopRecording() {
  if (!isRecording()) return;
  CameraRec::stop();
  AudioRec::stop();
  Flight::stopLog();

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
    info.printf("sensor_samples=%lu\nsensor_dropped=%lu\n",
                (unsigned long)Flight::loggedSamples(), (unsigned long)Flight::logDropped());
    // Picco e apogeo valgono per l'ultimo decollo: si scrivono solo se è avvenuto durante
    // questa registrazione, altrimenti sarebbero quelli di un volo precedente
    const uint32_t flights = Flight::launchCount() - s_launchesAtStart;
    info.printf("flights=%lu\n", (unsigned long)flights);
    if (flights) info.printf("peak_g=%.2f\n", Flight::peakG());
    // Zero preso sulla rampa: con questi valori si risale ai dati grezzi
    const Flight::Calibration c = Flight::calibration();
    info.printf("cal_raw_g=%.4f\ncal_scale=%.4f\ncal_gyro_bias_dps=%.2f,%.2f,%.2f\n"
                "cal_gravity_dir=%.3f,%.3f,%.3f\n",
                c.rawG, c.scale, c.gx, c.gy, c.gz, c.rx, c.ry, c.rz);
    if (Flight::hasBaro()) {
      // Il modello serve all'analisi: rumore e frequenza della quota cambiano fra i due
      info.printf("baro=%s\npad_pressure_hpa=%.2f\n", Baro::model(), Flight::padPressureHpa());
      if (flights) info.printf("apogee_m=%.2f\n", Flight::apogeeM());
      if (!isnan(c.altNoiseM)) info.printf("pad_alt_noise_m=%.3f\n", c.altNoiseM);
    }
    info.close();
  }

  Serial.printf(">> REC fermata: %lu frame in %.1f s (%.1f fps), audio %.1f s, sensori %lu campioni\n",
                (unsigned long)frames, secs, fps, AudioRec::seconds(),
                (unsigned long)Flight::loggedSamples());
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

// Razzo ritrovato: zittisce il cicalino e torna ad A TERRA, pronto per il lancio successivo
// (comando seriale 'q' o pulsante "Spegni cicalino" della pagina web)
static void silenceAndRearm() {
  Flight::reset();
  Buzzer::stop();
  Serial.println("Cicalino spento, riarmato: A TERRA, in attesa del decollo");
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
      case 'a': case 'A':
        s_imuStream = !s_imuStream;
        Serial.printf("Dati accelerometro in continuo: %s\n", s_imuStream ? "on" : "off");
        break;
      case 'v': case 'V':
        Flight::forceLaunch();
        Serial.println("Decollo simulato: tieni fermo il sensore per farlo 'atterrare'");
        break;
      case 'q': case 'Q': silenceAndRearm(); break;
      case 'p': case 'P':
        LedRing::pinTest();
        break;
      case 'z': case 'Z': Flight::startPrecal(); break;
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
  if (Flight::isReady()) {
    const Flight::Sample s = Flight::latest();
    Serial.printf("           %s | |a| %.2f g | picco %.1f g",
                  Flight::stateName(Flight::state()), Flight::magnitude(s), Flight::peakG());
    if (Flight::hasBaro()) Serial.printf(" | quota %.1f m | apogeo %.1f m", s.altM, Flight::apogeeM());
    Serial.println();
  }
}

static void printImu() {
  const Flight::Sample s = Flight::latest();
  Serial.printf("acc g %6.2f %6.2f %6.2f |a| %5.2f netta %5.2f | giro dps %7.1f %7.1f %7.1f | %.1f C",
                s.ax, s.ay, s.az, Flight::magnitude(s), s.net, s.gx, s.gy, s.gz, s.tempC);
  if (Flight::hasBaro()) Serial.printf(" | quota %6.2f m %7.2f hPa", s.altM, s.pressHpa);
  Serial.printf(" | %s\n", Flight::stateName(Flight::state()));
}

// Annuncia i cambi di fase e fa suonare il cicalino da atterrato
static void handleFlight() {
  static Flight::State last = Flight::State::Ground;
  const Flight::State now = Flight::state();
  if (now == last) return;
  last = now;

  switch (now) {
    case Flight::State::Flying:
      Buzzer::stop();
      Serial.println(">> DECOLLO");
      break;
    case Flight::State::Landed:
      Serial.printf(">> ATTERRATO dopo %.1f s di volo, picco %.1f g",
                    (Flight::landedMs() - Flight::launchMs()) / 1000.0f, Flight::peakG());
      if (Flight::hasBaro()) Serial.printf(", quota massima %.1f m", Flight::apogeeM());
      Serial.println();
      Buzzer::beep(200, 800, 0);  // ritrovamento: 200 ms on / 800 ms off
      break;
    case Flight::State::Ground:
      break;
  }
}

// Esito della calibrazione pre-lancio: sul seriale e col cicalino, che sul campo si sente
// anche con il telefono in tasca (2 beep corti = ok, 1 lungo = fallita)
static void handlePrecal() {
  static Flight::Precal last = Flight::Precal::Idle;
  const Flight::PrecalStatus p = Flight::precal();
  if (p.state == last) return;
  last = p.state;

  if (p.state == Flight::Precal::Ok) {
    const Flight::Calibration c = Flight::calibration();
    Serial.printf(">> CALIBRATO: accelerometro a riposo %.3f g (corretto a 1,000)", c.rawG);
    if (!isnan(c.altNoiseM)) {
      Serial.printf(", quota 0 a %.2f hPa, rumore ±%.2f m", c.p0Pa / 100.0f, c.altNoiseM);
    }
    Serial.println();
    Buzzer::beep(80, 120, 2);
  } else if (p.state == Flight::Precal::Failed) {
    Serial.printf(">> CALIBRAZIONE FALLITA: %s\n", p.msg);
    Buzzer::beep(600, 0, 1);
  } else if (p.state == Flight::Precal::Running) {
    Serial.println(">> Calibrazione pre-lancio: tieni fermo il razzo per 3 s");
  }
}

// Modalità prova = registrazione automatica di ogni prova, come "Nuovo lancio" dalla pagina:
// parte quando si attiva la modalità (o si riarma a terra), così il video comprende il
// decollo, e si ferma TEST_REC_TAIL_MS dopo l'atterraggio. Le registrazioni avviate a mano
// non vengono toccate.
static constexpr uint32_t TEST_REC_TAIL_MS = 3000;

static void handleTestRecording() {
  static bool autoRec = false;  // registrazione in corso avviata da qui
  static bool lastTest = false;
  static Flight::State lastState = Flight::State::Ground;

  const bool test = Flight::testMode();
  const Flight::State state = Flight::state();
  const bool armed = (test && !lastTest) ||  // modalità appena attivata
                     (test && state == Flight::State::Ground && lastState != Flight::State::Ground);
  if (autoRec && !isRecording()) autoRec = false;  // fermata a mano nel frattempo

  if (armed && state == Flight::State::Ground && !isRecording()) {
    Serial.println(">> Modalità prova: registrazione automatica");
    startRecording();
    autoRec = isRecording();
  } else if (autoRec && !test) {
    stopRecording();
    autoRec = false;
  } else if (autoRec && state == Flight::State::Landed &&
             millis() - Flight::landedMs() >= TEST_REC_TAIL_MS) {
    stopRecording();
    autoRec = false;
  }
  lastTest = test;
  lastState = state;
}

// Aspetto dell'anello secondo la situazione (cablaggio.md §7), dal più importante:
// manuale dal pannello > errore > atterrato > in volo > registrazione > pronto
static constexpr uint32_t RED = 0xFF0000, GREEN = 0x00FF00, BLUE = 0x0040FF, WHITE = 0xFFFFFF;
// In modalità prova si è quasi sempre sull'USB: il bianco al 100% (~480 mA)
// supererebbe la porta del PC e farebbe riavviare la scheda
static constexpr uint8_t TEST_MAX_PCT = 25;

static LedRing::Look ringLook() {
  using LedRing::Effect;
  const LedRing::Manual m = LedRing::manual();
  if (m.on) return {m.pct ? Effect::Solid : Effect::Off, m.rgb, m.pct, 0};

  LedRing::Look l;
  if (!Storage::isMounted() || !Flight::isReady()) {
    l = {Effect::Solid, RED, 10, 0};  // SD mancante o accelerometro non trovato
  } else if (Flight::state() == Flight::State::Landed) {
    l = {Effect::Flash, WHITE, 100, Flight::landedMs()};  // in sincrono col cicalino
  } else if (Flight::state() == Flight::State::Flying) {
    l = {Effect::Solid, WHITE, 100, 0};
  } else if (Flight::precal().state == Flight::Precal::Running) {
    l = {Effect::Pulse, BLUE, 20, 0};  // calibrazione pre-lancio: non toccare il razzo
  } else if (isRecording()) {
    l = {Effect::Pulse, RED, 20, 0};
  } else {
    l = {Effect::Solid, GREEN, 10, 0};
  }
  if (Flight::testMode()) l.pct = min(l.pct, TEST_MAX_PCT);
  return l;
}

void setup() {
  // Per primo: finché il pin non è configurato a LOW il cicalino può suonare
  // (prima durava tutta l'attesa del seriale qui sotto, fino a 3 s)
  Buzzer::begin();
  pinMode(BTN_PIN_BOOT, INPUT_PULLUP);
  LedRing::begin();

  Serial.begin(115200);
  // USB collegato al PC ma monitor chiuso: senza questo ogni print aspettava che il
  // PC ritirasse i dati e il loop() restava fermo ~20 s (cicalino acceso, comandi
  // web e pulsante ignorati). Così se nessuno legge i messaggi vengono scartati.
  Serial.setTxTimeoutMs(0);
  // USB CDC: aspetta fino a 3 s che si apra il monitor seriale
  const uint32_t start = millis();
  while (!Serial && millis() - start < 3000) {
    delay(10);
  }

  Storage::begin();
  CameraRec::begin();
  AudioRec::begin();
  // Sensore ok: 1 beep corto. Non trovato: 3 beep lunghi
  if (Flight::begin()) {
    Buzzer::beep(80, 0, 1);
  } else {
    Buzzer::beep(500, 300, 3);
  }
  if (WIFI_ENABLED) {
    Web::begin({startRecording, stopRecording, isRecording, sessionDir, silenceAndRearm});
  }

  printChipInfo();
  printHelp();

  if (AUTO_RECORD) startRecording();
}

void loop() {
  handleButton();
  handleSerial();
  Web::handle();
  handleFlight();
  handleTestRecording();
  handlePrecal();
  Buzzer::handle();
  LedRing::handle(ringLook());

  static uint32_t lastImu = 0;
  if (s_imuStream && Flight::isReady() && millis() - lastImu >= IMU_STREAM_MS) {
    lastImu = millis();
    printImu();
  }

  static uint32_t lastStatus = 0;
  if (millis() - lastStatus >= STATUS_MS) {
    lastStatus = millis();
    printStatus();
  }
  delay(5);
}
