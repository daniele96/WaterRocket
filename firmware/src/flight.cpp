#include "flight.h"

#include <SD.h>
#include <Wire.h>

#include <algorithm>
#include <atomic>
#include <mutex>

#include "board_pins.h"

// --- Soglie del riconoscimento delle fasi (da tarare con i primi lanci) ---
// Decollo: la spinta di un razzo ad acqua supera largamente i 10 g; 2.5 g per
// 30 ms bastano a escludere urti e maneggiamenti, e si ottengono anche con uno
// scossone deciso a mano per provarlo a terra.
static constexpr float LAUNCH_G = 2.5f;
static constexpr uint32_t LAUNCH_MS = 30;
// Modalità prova (dalla pagina web): basta un colpo secco della mano, che dà
// picchi di 2-3 g lunghi solo 10-20 ms. Sul razzo vero farebbe scattare il
// decollo anche con un urto sulla rampa, per questo all'avvio è sempre spenta.
static constexpr float TEST_LAUNCH_G = 2.0f;
static constexpr uint32_t TEST_LAUNCH_MS = 10;
// Atterraggio: vettore accelerazione stabile entro LANDED_DELTA_G per LANDED_STILL_MS.
// 0.1 g corrisponde a ~6° di inclinazione. Senza paracadute la discesa è balistica
// (caduta libera ~0 g, rotolamenti) e finisce con l'impatto: poi resta fermo a 1 g.
static constexpr float LANDED_DELTA_G = 0.10f;
static constexpr float LANDED_ONE_G_TOL = 0.20f;  // |a| deve stare in 1 g ± questo
static constexpr uint32_t LANDED_STILL_MS = 3000;
static constexpr uint32_t MIN_FLIGHT_MS = 2000;    // prima non può essere già atterrato
// Zero sulla rampa: finché è A TERRA e fermo, ogni CAL_SAMPLES campioni (1.5 s) di
// immobilità diventano la nuova calibrazione. Al decollo vale l'ultima presa.
// Fermo = vettore entro CAL_DELTA_G e giroscopio entro CAL_GYRO_DPS dal primo campione.
static constexpr uint32_t CAL_SAMPLES = 150;
static constexpr float CAL_DELTA_G = 0.03f;
static constexpr float CAL_GYRO_DPS = 3.0f;
static constexpr float CAL_MIN_G = 0.7f, CAL_MAX_G = 1.4f;  // fuori = non è appoggiato fermo

static constexpr uint32_t SAMPLE_PERIOD_MS = 10;  // 100 Hz
// 100 kHz: con fili volanti e i pull-up del modulo i 400 kHz davano errori.
// Una lettura (14 byte) dura ~2 ms, ampiamente dentro i 10 ms del periodo.
static constexpr uint32_t I2C_FREQ = 100000;
static constexpr int INIT_ATTEMPTS = 5;

// Log su SD: la coda copre 2 s di campioni se la SD rallenta (camera e audio scrivono insieme)
static constexpr size_t LOG_QUEUE_LEN = 200;
static constexpr uint32_t LOG_FLUSH_MS = 1000;
static constexpr size_t LOG_BUF_SIZE = 2048;

// Registri MPU6050
static constexpr uint8_t REG_SMPLRT_DIV = 0x19;
static constexpr uint8_t REG_CONFIG = 0x1A;
static constexpr uint8_t REG_GYRO_CONFIG = 0x1B;
static constexpr uint8_t REG_ACCEL_CONFIG = 0x1C;
static constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;
static constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;
static constexpr uint8_t REG_WHO_AM_I = 0x75;

// Fondo scala ±16 g e ±2000 °/s: in spinta il razzo supera di molto gli 8 g
static constexpr float ACCEL_LSB_PER_G = 2048.0f;
static constexpr float GYRO_LSB_PER_DPS = 16.4f;

namespace Flight {

static uint8_t s_addr = 0;
static bool s_mpu6500Temp = false;  // clone famiglia MPU6500: altra formula della temperatura
static bool s_ready = false;
static std::mutex s_mutex;  // protegge s_sample e s_peak
static Sample s_sample = {};
static float s_peak = 0;
static std::atomic<State> s_state = State::Ground;
static std::atomic<uint32_t> s_launchMs = 0;
static std::atomic<uint32_t> s_landedMs = 0;
// Richieste dal loop() al task, che è l'unico a modificare la macchina a stati
static std::atomic<bool> s_reqLaunch = false;
static std::atomic<bool> s_reqReset = false;
static std::atomic<bool> s_reqLand = false;
static std::atomic<bool> s_testMode = false;
static Calibration s_cal;  // protetta da s_mutex

struct LogRecord {
  Sample s;
  State state;
};
static QueueHandle_t s_logQueue = nullptr;
static SemaphoreHandle_t s_logDone = nullptr;
static TaskHandle_t s_logTask = nullptr;
static std::atomic<bool> s_logRun = false;
static std::atomic<uint32_t> s_logStartMs = 0;
static std::atomic<uint32_t> s_logSamples = 0;
static std::atomic<uint32_t> s_logDropped = 0;
static String s_logDir;

static bool writeReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(s_addr);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

static bool readRegs(uint8_t reg, uint8_t *buf, size_t len) {
  Wire.beginTransmission(s_addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((uint16_t)s_addr, len) != len) return false;
  for (size_t i = 0; i < len; i++) buf[i] = Wire.read();
  return true;
}

static bool probe(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission() == 0;
}

static bool readSample(Sample &s) {
  uint8_t b[14];
  if (!readRegs(REG_ACCEL_XOUT_H, b, sizeof(b))) return false;
  // 14 byte tutti uguali (0x00, 0xFF o altro) non sono una misura ma un bus guasto:
  // scartarli evita che dati spazzatura facciano scattare un falso decollo
  if (std::all_of(b + 1, b + sizeof(b), [&](uint8_t v) { return v == b[0]; })) return false;
  auto raw = [&](int i) { return (int16_t)((b[i] << 8) | b[i + 1]); };
  s.ax = raw(0) / ACCEL_LSB_PER_G;
  s.ay = raw(2) / ACCEL_LSB_PER_G;
  s.az = raw(4) / ACCEL_LSB_PER_G;
  s.tempC = s_mpu6500Temp ? raw(6) / 333.87f + 21.0f : raw(6) / 340.0f + 36.53f;
  s.gx = raw(8) / GYRO_LSB_PER_DPS;
  s.gy = raw(10) / GYRO_LSB_PER_DPS;
  s.gz = raw(12) / GYRO_LSB_PER_DPS;
  s.ms = millis();
  return true;
}

float magnitude(const Sample &s) { return sqrtf(s.ax * s.ax + s.ay * s.ay + s.az * s.az); }

// Applica la calibrazione a un campione grezzo e calcola l'accelerazione netta
static Sample calibrated(const Sample &raw, const Calibration &c) {
  Sample s = raw;
  s.ax *= c.scale;
  s.ay *= c.scale;
  s.az *= c.scale;
  s.gx -= c.gx;
  s.gy -= c.gy;
  s.gz -= c.gz;
  const float dx = s.ax - c.rx, dy = s.ay - c.ry, dz = s.az - c.rz;
  s.net = sqrtf(dx * dx + dy * dy + dz * dz);
  return s;
}

// Media dei campioni grezzi di una finestra di immobilità sulla rampa
struct CalWindow {
  Sample first = {};
  double ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;
  uint32_t n = 0;

  void restart() { n = 0; ax = ay = az = gx = gy = gz = 0; }

  // true quando la finestra è completa e ha prodotto una nuova calibrazione in out
  bool add(const Sample &r, Calibration &out) {
    const float g = magnitude(r);
    if (n > 0) {
      const float dx = r.ax - first.ax, dy = r.ay - first.ay, dz = r.az - first.az;
      const bool still = sqrtf(dx * dx + dy * dy + dz * dz) < CAL_DELTA_G &&
                         fabsf(r.gx - first.gx) < CAL_GYRO_DPS &&
                         fabsf(r.gy - first.gy) < CAL_GYRO_DPS &&
                         fabsf(r.gz - first.gz) < CAL_GYRO_DPS;
      if (!still) restart();  // si è mosso: si ricomincia da questo campione
    }
    if (g < CAL_MIN_G || g > CAL_MAX_G) {
      restart();
      return false;
    }
    if (n == 0) first = r;
    ax += r.ax; ay += r.ay; az += r.az;
    gx += r.gx; gy += r.gy; gz += r.gz;
    if (++n < CAL_SAMPLES) return false;

    const float mx = ax / n, my = ay / n, mz = az / n;
    const float rawG = sqrtf(mx * mx + my * my + mz * mz);
    out.scale = 1.0f / rawG;                      // a riposo |a| deve valere 1 g
    out.gx = gx / n; out.gy = gy / n; out.gz = gz / n;  // a riposo il giroscopio deve dare 0
    out.rx = mx / rawG; out.ry = my / rawG; out.rz = mz / rawG;  // direzione della gravità
    out.rawG = rawG;
    out.tempC = r.tempC;
    out.ms = r.ms;
    restart();
    return true;
  }
};

static void enterState(State st, uint32_t now) {
  if (st == State::Flying) {
    s_launchMs = now;
    s_landedMs = 0;
    std::lock_guard<std::mutex> lock(s_mutex);
    s_peak = 0;
  } else if (st == State::Landed) {
    if (!s_launchMs) s_launchMs = now;  // atterraggio simulato senza decollo
    s_landedMs = now;
  } else {
    s_launchMs = 0;
    s_landedMs = 0;
  }
  s_state = st;
}

static void flightTask(void *) {
  uint32_t aboveSince = 0;  // da quando |a| supera LAUNCH_G (0 = sotto soglia)
  uint32_t stillSince = 0;  // da quando il razzo è fermo
  Sample ref = {};          // campione di riferimento della finestra "fermo"
  uint32_t readErrors = 0;
  CalWindow calWin;

  TickType_t wake = xTaskGetTickCount();
  for (;;) {
    vTaskDelayUntil(&wake, pdMS_TO_TICKS(SAMPLE_PERIOD_MS));

    if (s_reqReset.exchange(false)) {
      enterState(State::Ground, millis());
      aboveSince = 0;
    }
    if (s_reqLaunch.exchange(false)) {
      enterState(State::Flying, millis());
      stillSince = 0;
    }
    if (s_reqLand.exchange(false)) enterState(State::Landed, millis());

    Sample raw;
    if (!readSample(raw)) {
      if (readErrors++ % 100 == 0) log_w("Lettura MPU6050 fallita (%lu errori)", (unsigned long)readErrors);
      continue;
    }

    // Lo zero si prende solo sulla rampa: in volo e dopo l'atterraggio resta quello del decollo
    Calibration newCal;
    const bool calDone = s_state == State::Ground && calWin.add(raw, newCal);
    if (s_state != State::Ground) calWin.restart();

    Sample s;
    {
      std::lock_guard<std::mutex> lock(s_mutex);
      if (calDone) s_cal = newCal;
      s = calibrated(raw, s_cal);
      s_sample = s;
      if (s_state == State::Flying && magnitude(s) > s_peak) s_peak = magnitude(s);
    }
    const float g = magnitude(s);

    switch (s_state.load()) {
      case State::Ground:
        if (g < (s_testMode ? TEST_LAUNCH_G : LAUNCH_G)) {
          aboveSince = 0;
        } else if (!aboveSince) {
          aboveSince = s.ms;
        } else if (s.ms - aboveSince >= (s_testMode ? TEST_LAUNCH_MS : LAUNCH_MS)) {
          aboveSince = 0;
          enterState(State::Flying, s.ms);
          stillSince = 0;
        }
        break;

      case State::Flying: {
        const float dx = s.ax - ref.ax, dy = s.ay - ref.ay, dz = s.az - ref.az;
        const bool steady = sqrtf(dx * dx + dy * dy + dz * dz) < LANDED_DELTA_G;
        const bool oneG = fabsf(g - 1.0f) < LANDED_ONE_G_TOL;
        if (!stillSince || !steady || !oneG) {
          // Si è mosso: la finestra riparte con questo campione come riferimento
          ref = s;
          stillSince = s.ms;
        } else if (s.ms - stillSince >= LANDED_STILL_MS &&
                   s.ms - s_launchMs >= MIN_FLIGHT_MS) {
          enterState(State::Landed, s.ms);
        }
        break;
      }

      case State::Landed:
        break;
    }

    if (s_logRun) {
      const LogRecord rec = {s, s_state};
      if (xQueueSend(s_logQueue, &rec, 0) != pdTRUE) s_logDropped++;
    }
  }
}

static void logTask(void *) {
  File f = SD.open(s_logDir + "/sensors.csv", FILE_WRITE);
  if (!f) {
    log_e("Impossibile creare sensors.csv in %s", s_logDir.c_str());
    s_logRun = false;
  } else {
    // Ogni colonna dopo il tempo diventa un grafico nella pagina web
    f.println("t_ms,acc X (g),acc Y (g),acc Z (g),|acc| (g),acc netta (g),giro X (°/s),"
              "giro Y (°/s),giro Z (°/s),in volo (1 = sì)");
  }

  static char buf[LOG_BUF_SIZE];
  size_t used = 0;
  uint32_t lastFlush = millis();
  LogRecord rec;
  // Finita la registrazione svuota comunque la coda, poi chiude
  while (f && (s_logRun || uxQueueMessagesWaiting(s_logQueue))) {
    if (xQueueReceive(s_logQueue, &rec, pdMS_TO_TICKS(100)) == pdTRUE) {
      const Sample &s = rec.s;
      used += snprintf(buf + used, sizeof(buf) - used,
                       "%lu,%.3f,%.3f,%.3f,%.3f,%.3f,%.1f,%.1f,%.1f,%d\n",
                       (unsigned long)(s.ms - s_logStartMs), s.ax, s.ay, s.az, magnitude(s),
                       s.net, s.gx, s.gy, s.gz, rec.state == State::Flying ? 1 : 0);
      s_logSamples++;
    }
    // Scrive a blocchi: una riga alla volta sulla SD sarebbe troppo lenta
    if (used > sizeof(buf) - 128 || (used && millis() - lastFlush >= LOG_FLUSH_MS)) {
      if (f.write((uint8_t *)buf, used) != used) {
        log_e("Scrittura sensors.csv fallita (SD piena o rimossa?)");
        break;
      }
      used = 0;
      if (millis() - lastFlush >= LOG_FLUSH_MS) {
        f.flush();  // se l'alimentazione salta (impatto...) si perde al massimo ~1 s
        lastFlush = millis();
      }
    }
  }

  if (f) {
    if (used) f.write((uint8_t *)buf, used);
    f.close();
  }
  s_logRun = false;
  xQueueReset(s_logQueue);
  s_logTask = nullptr;
  xSemaphoreGive(s_logDone);
  vTaskDelete(nullptr);
}

// Sensore non trovato: indizi per capire se è un problema di cablaggio
static void diagnoseBus() {
  Wire.end();
  // Senza pull-up interni: le linee stanno alte solo grazie ai pull-up del modulo,
  // quindi se il GY-521 è alimentato e collegato devono leggere 1
  pinMode(I2C_PIN_SDA, INPUT);
  pinMode(I2C_PIN_SCL, INPUT);
  delay(5);
  const int sda = digitalRead(I2C_PIN_SDA), scl = digitalRead(I2C_PIN_SCL);
  log_e("Linee a riposo: SDA(D4)=%d SCL(D5)=%d (attese 1 e 1; 0 = modulo non alimentato,"
        " filo staccato o in corto a GND)", sda, scl);

  Wire.begin(I2C_PIN_SDA, I2C_PIN_SCL, 100000);
  String found;
  for (uint8_t a = 1; a < 127; a++) {
    if (probe(a)) found += String(" 0x") + String(a, HEX);
  }
  log_e("Scansione I2C: %s", found.isEmpty() ? "nessun dispositivo" : found.c_str());
  if (!found.isEmpty()) return;

  // Errore più comune: SDA e SCL scambiati (le linee restano alte ma nessuno risponde)
  Wire.end();
  Wire.begin(I2C_PIN_SCL, I2C_PIN_SDA, 100000);
  for (uint8_t a = 1; a < 127; a++) {
    if (probe(a)) found += String(" 0x") + String(a, HEX);
  }
  if (!found.isEmpty()) {
    log_e("Con SDA e SCL scambiati risponde:%s -> i fili di D4 e D5 sono invertiti", found.c_str());
  }
  Wire.end();
}

// Un tentativo completo: ricerca dell'indirizzo, configurazione, prima lettura
static bool initSensor() {
  // AD0 a GND (o scollegato, c'è il pull-down) = 0x68; AD0 a 3V3 = 0x69
  s_addr = 0;
  for (uint8_t addr : {0x68, 0x69}) {
    if (probe(addr)) {
      s_addr = addr;
      break;
    }
  }
  if (!s_addr) return false;

  // MPU6050 originale = 0x68, ma i cloni rispondono con valori diversi (0x70, 0x72,
  // 0x74, 0x98...). Conta che il valore sia stabile: se cambia da una lettura
  // all'altra, o è 0x00/0xFF, i byte arrivano corrotti ed è un problema di fili.
  uint8_t who = 0, who2 = 0;
  if (!readRegs(REG_WHO_AM_I, &who, 1) || !readRegs(REG_WHO_AM_I, &who2, 1)) return false;
  if (who != who2 || who == 0x00 || who == 0xFF) {
    log_w("MPU6050 a 0x%02X: WHO_AM_I instabile 0x%02X/0x%02X (dati corrotti sul bus?)",
          s_addr, who, who2);
    return false;
  }
  if (who != 0x68) log_i("MPU6050 a 0x%02X: WHO_AM_I = 0x%02X (clone)", s_addr, who);
  // Accelerometro e giroscopio hanno gli stessi registri e scale; cambia solo la temperatura
  s_mpu6500Temp = who != 0x68;

  const bool ok = writeReg(REG_PWR_MGMT_1, 0x01) &&   // sveglia, clock dal giroscopio X
                  writeReg(REG_CONFIG, 0x03) &&        // filtro passa-basso ~44 Hz
                  writeReg(REG_SMPLRT_DIV, 9) &&       // 1 kHz / (1 + 9) = 100 Hz
                  writeReg(REG_GYRO_CONFIG, 0x18) &&   // ±2000 °/s
                  writeReg(REG_ACCEL_CONFIG, 0x18);    // ±16 g
  if (!ok) {
    log_w("Configurazione MPU6050 fallita");
    return false;
  }
  delay(50);  // il primo campione dopo il risveglio non è valido

  Sample s;
  if (!readSample(s)) {
    log_w("Prima lettura MPU6050 fallita");
    return false;
  }
  s_sample = calibrated(s, s_cal);
  return true;
}

bool begin() {
  Wire.begin(I2C_PIN_SDA, I2C_PIN_SCL, I2C_FREQ);

  bool ok = false;
  for (int i = 1; i <= INIT_ATTEMPTS && !ok; i++) {
    ok = initSensor();
    if (!ok) delay(100);
    else if (i > 1) log_w("MPU6050 inizializzato al tentativo %d: controlla i contatti", i);
  }
  if (!ok) {
    log_e("MPU6050 non risponde su I2C (SDA=D4, SCL=D5): controlla i collegamenti");
    diagnoseBus();
    return false;
  }

  s_logQueue = xQueueCreate(LOG_QUEUE_LEN, sizeof(LogRecord));
  s_logDone = xSemaphoreCreateBinary();

  // Core 1 con priorità sopra il loop(): camera e audio girano sul core 0
  if (xTaskCreatePinnedToCore(flightTask, "flight", 4096, nullptr, 3, nullptr, 1) != pdPASS) {
    log_e("Impossibile avviare il task dell'accelerometro");
    return false;
  }
  s_ready = true;
  return true;
}

bool isReady() { return s_ready; }
uint8_t address() { return s_addr; }

Sample latest() {
  std::lock_guard<std::mutex> lock(s_mutex);
  return s_sample;
}

float peakG() {
  std::lock_guard<std::mutex> lock(s_mutex);
  return s_peak;
}

State state() { return s_state; }

const char *stateName(State s) {
  switch (s) {
    case State::Ground: return "A TERRA";
    case State::Flying: return "IN VOLO";
    case State::Landed: return "ATTERRATO";
  }
  return "?";
}

uint32_t launchMs() { return s_launchMs; }
uint32_t landedMs() { return s_landedMs; }

void forceLaunch() { s_reqLaunch = true; }
void forceLand() { s_reqLand = true; }
void reset() { s_reqReset = true; }

Calibration calibration() {
  std::lock_guard<std::mutex> lock(s_mutex);
  return s_cal;
}

void setTestMode(bool on) {
  if (s_testMode.exchange(on) != on) log_i("Modalità prova %s", on ? "attiva" : "disattivata");
}
bool testMode() { return s_testMode; }

bool startLog(const String &dir) {
  if (!s_ready || s_logTask) return false;
  s_logDir = dir;
  s_logSamples = 0;
  s_logDropped = 0;
  s_logStartMs = millis();
  s_logRun = true;
  // Core 0 come camera e audio, con priorità più bassa: il log può aspettare, la coda assorbe
  if (xTaskCreatePinnedToCore(logTask, "imu_log", 4096, nullptr, 4, &s_logTask, 0) != pdPASS) {
    s_logRun = false;
    return false;
  }
  return true;
}

void stopLog() {
  if (!s_logTask) return;
  s_logRun = false;
  xSemaphoreTake(s_logDone, portMAX_DELAY);
}

bool isLogging() { return s_logTask != nullptr; }
uint32_t loggedSamples() { return s_logSamples; }
uint32_t logDropped() { return s_logDropped; }

}  // namespace Flight
