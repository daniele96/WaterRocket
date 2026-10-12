#include "web_server.h"

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <SD.h>
#include <WiFi.h>
#include <esp_camera.h>

#include <atomic>
#include <memory>

#include "audio_rec.h"
#include "avi_export.h"
#include "baro.h"
#include "buzzer.h"
#include "camera_rec.h"
#include "flight.h"
#include "led_ring.h"
#include "storage.h"
#include "web_page.h"
#include "zip_export.h"

// Rete esistente a cui collegarsi (opzionale): vedi include/secrets.example.h
#if __has_include("secrets.h")
#include "secrets.h"
#endif

// Rete creata dallo XIAO: funziona anche al campo di lancio, senza router.
static constexpr const char *AP_SSID = "WaterRocket";
static constexpr const char *AP_PASS = "razzo1234";  // almeno 8 caratteri
static constexpr const char *HOSTNAME = "rocket";    // http://rocket.local
static constexpr size_t MAX_META_BYTES = 4096;
// Ora italiana con l'ora legale automatica: date dei file sulla SD e dentro lo zip.
// L'epoch (info.txt, /api/status) resta in UTC, indipendente dal fuso.
static constexpr const char *TIMEZONE = "CET-1CEST,M3.5.0,M10.5.0/3";

// Il server è asincrono: gli handler girano nel task di AsyncTCP, in parallelo
// al loop() e ai task di registrazione. Possono leggere/scrivere la SD (FATFS è
// thread-safe) ma avvio/stop della registrazione vengono passati al loop().

namespace Web {

// Comandi della pagina eseguiti dal loop(); Sim* = pannello di simulazione
enum class Cmd : int { None, Start, Stop, Silence, SimLaunch, SimLand, SimBuzzOn, SimBuzzOff };

static AsyncWebServer s_server(80);
static RecorderHooks s_hooks;
static bool s_started = false;
static std::atomic<Cmd> s_cmd{Cmd::None};

// Accetta solo file dentro le cartelle dei lanci, niente path traversal
static bool isSafePath(const String &p) {
  return p.startsWith("/REC_") && p.indexOf("..") < 0;
}

// Cartella di un lancio: /REC_xxxx (senza sottocartelle)
static bool isLaunchDir(const String &d) {
  return isSafePath(d) && d.indexOf('/', 1) < 0;
}

static bool clockValid() { return time(nullptr) > 1700000000; }  // dopo nov 2023

static String param(AsyncWebServerRequest *req, const char *name) {
  const AsyncWebParameter *p = req->getParam(name);
  return p ? p->value() : String();
}

static String jsonEscape(const String &s) {
  String out;
  out.reserve(s.length() + 8);
  for (char c : s) {
    if (c == '"' || c == '\\') out += '\\';
    if (c == '\n') { out += "\\n"; continue; }
    if (c == '\r') continue;
    out += c;
  }
  return out;
}

static String readSmallFile(const String &path) {
  File f = SD.open(path);
  if (!f) return "";
  String s = f.readString();
  f.close();
  return s;
}

// Invia un file aperto a pezzi; il File resta vivo finché serve alla risposta
static AsyncWebServerResponse *fileResponse(std::shared_ptr<File> f, uint32_t len, const char *type) {
  return new AsyncCallbackResponse(type, len, [f](uint8_t *buf, size_t maxLen, size_t index) -> size_t {
    if (f->position() != index) f->seek(index);
    return f->read(buf, maxLen);
  });
}

static void handleStatus(AsyncWebServerRequest *req) {
  const bool rec = s_hooks.isRecording();
  const float secs = CameraRec::elapsedMs() / 1000.0f;
  const String dir = rec ? s_hooks.sessionDir() : String();
  const String ip = WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : String();
  // Fase del volo: 0 a terra, 1 in volo, 2 atterrato (-1 = accelerometro assente)
  const int phase = Flight::isReady() ? (int)Flight::state() : -1;
  const Flight::Sample a = Flight::latest();
  const Flight::Calibration cal = Flight::calibration();
  // Età dello zero in secondi (-1 = mai preso, il sensore non è stato fermo sulla rampa)
  const long calAge = cal.rawG > 0 ? (long)((millis() - cal.ms) / 1000) : -1;
  // Altimetro: oggetto "baro" solo se c'è (null altrimenti)
  char baro[150] = "null";
  if (Flight::hasBaro() && !isnan(a.altM)) {
    snprintf(baro, sizeof(baro),
             "{\"model\":\"%s\",\"alt\":%.2f,\"press\":%.2f,\"temp\":%.1f,\"apogee\":%.1f}",
             Baro::model(), a.altM, a.pressHpa, a.baroTempC, Flight::apogeeM());
  }
  // Calibrazione pre-lancio: stato, avanzamento, età dell'ultima riuscita, rumore della quota
  static const char *const PRECAL_NAMES[] = {"idle", "running", "ok", "fail"};
  const Flight::PrecalStatus pc = Flight::precal();
  char noise[16] = "null";
  if (!isnan(cal.altNoiseM)) snprintf(noise, sizeof(noise), "%.3f", cal.altNoiseM);
  char precal[200];
  snprintf(precal, sizeof(precal),
           "{\"state\":\"%s\",\"progress\":%u,\"age\":%ld,\"msg\":\"%s\",\"noise\":%s}",
           PRECAL_NAMES[(int)pc.state], (unsigned)pc.progress,
           pc.doneMs ? (long)((millis() - pc.doneMs) / 1000) : -1L, pc.msg, noise);
  char sensors[520];
  snprintf(sensors, sizeof(sensors),
           "\"acc\":[%.2f,%.2f,%.2f],\"g\":%.2f,\"net\":%.2f,\"gyro\":[%.1f,%.1f,%.1f],"
           "\"calAge\":%ld,\"calRawG\":%.3f,\"baro\":%s,\"precal\":%s",
           a.ax, a.ay, a.az, Flight::magnitude(a), a.net, a.gx, a.gy, a.gz, calAge, cal.rawG, baro,
           precal);
  const LedRing::Manual led = LedRing::manual();
  char buf[1400];  // ~900 caratteri con tutti i campi
  snprintf(buf, sizeof(buf),
           "{\"time\":%lld,\"staIp\":\"%s\",\"recording\":%s,\"dir\":\"%s\",\"secs\":%.1f,"
           "\"frames\":%lu,\"fps\":%.1f,\"audio\":%.1f,\"camera\":\"%s\",\"sd\":%s,"
           "\"sdTotalMB\":%llu,\"sdFreeMB\":%llu,\"phase\":%d,\"peakG\":%.1f,\"buzzer\":%s,"
           "\"testMode\":%s,\"led\":{\"manual\":%s,\"color\":\"%06lx\",\"pct\":%u},%s}",
           clockValid() ? (long long)time(nullptr) : 0LL, ip.c_str(), rec ? "true" : "false",
           dir.c_str(), rec ? secs : 0.0f, (unsigned long)(rec ? CameraRec::frames() : 0),
           rec && secs > 0 ? CameraRec::frames() / secs : 0.0f, rec ? AudioRec::seconds() : 0.0f,
           CameraRec::isReady() ? CameraRec::sensorName() : "-",
           Storage::isMounted() ? "true" : "false",
           Storage::isMounted() ? SD.totalBytes() / (1024 * 1024) : 0ULL,
           Storage::isMounted() ? (SD.totalBytes() - SD.usedBytes()) / (1024 * 1024) : 0ULL,
           phase, Flight::peakG(), Buzzer::isActive() ? "true" : "false",
           Flight::testMode() ? "true" : "false", led.on ? "true" : "false",
           (unsigned long)led.rgb, (unsigned)led.pct, Flight::isReady() ? sensors : "\"g\":null");
  req->send(200, "application/json", buf);
}

// Elenco dei lanci: info.txt (scritto dal firmware), launch.json (scritto
// dalla pagina: nome, parametri, note) e file presenti nella cartella
static void handleLaunches(AsyncWebServerRequest *req) {
  if (!Storage::isMounted()) {
    req->send(200, "application/json", "[]");
    return;
  }
  const String recDir = s_hooks.isRecording() ? s_hooks.sessionDir() : String();
  String out = "[";
  bool firstDir = true;
  File root = SD.open("/");
  for (File dir = root.openNextFile(); dir; dir = root.openNextFile()) {
    const String name = String("/") + dir.name();
    if (!dir.isDirectory() || !name.startsWith("/REC_")) continue;
    if (!firstDir) out += ",";
    firstDir = false;
    String meta = readSmallFile(name + "/launch.json");
    meta.trim();
    if (!meta.startsWith("{")) meta = "null";
    out += "{\"dir\":\"" + name + "\",\"recording\":" + (name == recDir ? "true" : "false") +
           ",\"info\":\"" + jsonEscape(readSmallFile(name + "/info.txt")) + "\",\"meta\":" + meta +
           ",\"files\":[";
    bool firstFile = true;
    for (File f = dir.openNextFile(); f; f = dir.openNextFile()) {
      if (!firstFile) out += ",";
      firstFile = false;
      out += "{\"name\":\"" + String(f.name()) + "\",\"size\":" + String((unsigned)f.size()) + "}";
    }
    out += "]}";
  }
  out += "]";
  req->send(200, "application/json", out);
}

static const char *contentType(const String &path) {
  if (path.endsWith(".wav")) return "audio/wav";
  if (path.endsWith(".csv")) return "text/csv";
  if (path.endsWith(".txt")) return "text/plain";
  if (path.endsWith(".json")) return "application/json";
  return "application/octet-stream";
}

static void handleFile(AsyncWebServerRequest *req) {
  const String path = param(req, "path");
  if (!isSafePath(path)) {
    req->send(400, "text/plain", "path non valido");
    return;
  }
  auto f = std::make_shared<File>(SD.open(path));
  if (!*f || f->isDirectory()) {
    req->send(404, "text/plain", "file non trovato");
    return;
  }

  // Richiesta parziale "Range: bytes=a-b": il player scarica solo i frame che servono
  // e il browser fa lo streaming dell'audio, invece di scaricare tutto il file
  const uint32_t size = f->size();
  const AsyncWebHeader *range = req->getHeader("Range");
  if (range && range->value().startsWith("bytes=")) {
    const String spec = range->value().substring(6);
    const int dash = spec.indexOf('-');
    uint32_t start = spec.substring(0, dash).toInt();
    uint32_t end = dash >= 0 && dash + 1 < (int)spec.length() ? spec.substring(dash + 1).toInt() : size - 1;
    if (end >= size) end = size - 1;
    if (dash < 0 || start > end) {
      AsyncWebServerResponse *res = req->beginResponse(416, "text/plain", "range non valido");
      res->addHeader("Content-Range", "bytes */" + String(size));
      req->send(res);
      return;
    }
    const uint32_t len = end - start + 1;
    AsyncWebServerResponse *res = new AsyncCallbackResponse(
        contentType(path), len, [f, start](uint8_t *buf, size_t maxLen, size_t index) -> size_t {
          if (f->position() != start + index) f->seek(start + index);
          return f->read(buf, maxLen);
        });
    res->setCode(206);
    res->addHeader("Accept-Ranges", "bytes");
    res->addHeader("Content-Range", "bytes " + String(start) + "-" + String(end) + "/" + String(size));
    req->send(res);
    return;
  }

  AsyncWebServerResponse *res = fileResponse(f, size, contentType(path));
  res->addHeader("Accept-Ranges", "bytes");
  if (req->hasParam("dl")) {
    // Nome scaricato: REC_0001_video.mjpeg
    String fname = path.substring(1);
    fname.replace('/', '_');
    res->addHeader("Content-Disposition", "attachment; filename=\"" + fname + "\"");
  }
  req->send(res);
}

// Miniatura: il primo JPEG di video.mjpeg (dimensione presa da frames.csv)
static void handleThumb(AsyncWebServerRequest *req) {
  const String dir = param(req, "dir");
  if (!isLaunchDir(dir)) {
    req->send(400, "text/plain", "lancio non valido");
    return;
  }
  File csv = SD.open(dir + "/frames.csv");
  auto vid = std::make_shared<File>(SD.open(dir + "/video.mjpeg"));
  if (!csv || !*vid) {
    req->send(404, "text/plain", "nessun video");
    return;
  }
  csv.readStringUntil('\n');                       // intestazione
  const String first = csv.readStringUntil('\n');  // 0,t_ms,bytes
  csv.close();
  const uint32_t len = first.substring(first.lastIndexOf(',') + 1).toInt();
  if (len == 0 || len > vid->size()) {
    req->send(404, "text/plain", "nessun frame");
    return;
  }
  // Il primo frame non cambia più: il browser può tenerlo in cache
  AsyncWebServerResponse *res = new AsyncCallbackResponse(
      "image/jpeg", len, [vid, len](uint8_t *buf, size_t maxLen, size_t index) -> size_t {
        if (index >= len) return 0;
        if (vid->position() != index) vid->seek(index);
        return vid->read(buf, min(maxLen, (size_t)(len - index)));
      });
  res->addHeader("Cache-Control", "max-age=86400");
  req->send(res);
}

// Tutti i file del lancio in un unico .zip generato al volo
static void handleZip(AsyncWebServerRequest *req) {
  const String dir = param(req, "dir");
  if (!isLaunchDir(dir)) {
    req->send(400, "text/plain", "lancio non valido");
    return;
  }
  // Durante la registrazione i file crescono: le dimensioni dello zip sarebbero sbagliate
  if (s_hooks.isRecording() && dir == s_hooks.sessionDir()) {
    req->send(409, "text/plain", "registrazione in corso: fermala prima di scaricare lo zip");
    return;
  }
  auto src = std::make_shared<ZipSource>();
  if (!src->open(dir)) {
    req->send(404, "text/plain", src->error());
    return;
  }
  AsyncWebServerResponse *res = new AsyncCallbackResponse(
      "application/zip", src->size(),
      [src](uint8_t *buf, size_t maxLen, size_t index) -> size_t { return src->read(index, buf, maxLen); });
  res->addHeader("Content-Disposition", "attachment; filename=\"" + dir.substring(1) + ".zip\"");
  req->send(res);
}

static void handleAvi(AsyncWebServerRequest *req) {
  const String dir = param(req, "dir");
  if (!isLaunchDir(dir)) {
    req->send(400, "text/plain", "lancio non valido");
    return;
  }
  auto src = std::make_shared<AviSource>();
  if (!src->open(dir)) {
    req->send(404, "text/plain", src->error());
    return;
  }
  AsyncWebServerResponse *res = new AsyncCallbackResponse(
      "video/x-msvideo", src->size(),
      [src](uint8_t *buf, size_t maxLen, size_t index) -> size_t { return src->read(index, buf, maxLen); });
  res->addHeader("Content-Disposition", "attachment; filename=\"" + dir.substring(1) + ".avi\"");
  req->send(res);
}

// Salva launch.json (nome, parametri, note). Il contenuto è deciso dalla pagina.
static void handleSaveLaunchBody(AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index,
                                 size_t total) {
  if (total > MAX_META_BYTES) return;
  if (index == 0) {
    req->_tempObject = calloc(total + 1, 1);  // liberato dalla libreria a fine richiesta
  }
  if (req->_tempObject) memcpy(static_cast<char *>(req->_tempObject) + index, data, len);
}

static void handleSaveLaunch(AsyncWebServerRequest *req) {
  const String dir = param(req, "dir");
  const char *body = static_cast<const char *>(req->_tempObject);
  if (!isLaunchDir(dir) || !SD.exists(dir)) {
    req->send(400, "text/plain", "lancio non valido");
    return;
  }
  if (!body || body[0] != '{') {
    req->send(400, "text/plain", "dati non validi (max 4 KB)");
    return;
  }
  File f = SD.open(dir + "/launch.json", FILE_WRITE);
  const size_t n = strlen(body);
  const bool ok = f && f.write(reinterpret_cast<const uint8_t *>(body), n) == n;
  if (f) f.close();
  req->send(ok ? 200 : 500, "text/plain", ok ? "ok" : "salvataggio fallito");
}

// Il browser comunica l'ora quando lo XIAO non l'ha presa da internet (NTP)
static void handleTime(AsyncWebServerRequest *req) {
  const long long epoch = atoll(param(req, "epoch").c_str());
  if (!clockValid() && epoch > 1700000000LL) {
    timeval tv = {(time_t)epoch, 0};
    settimeofday(&tv, nullptr);
  }
  req->send(200, "text/plain", clockValid() ? "ok" : "ora non impostata");
}

static void handleDelete(AsyncWebServerRequest *req) {
  const String dir = param(req, "dir");
  if (!isLaunchDir(dir)) {
    req->send(400, "text/plain", "lancio non valido");
    return;
  }
  if (s_hooks.isRecording() && dir == s_hooks.sessionDir()) {
    req->send(409, "text/plain", "registrazione in corso in questo lancio");
    return;
  }
  File d = SD.open(dir);
  if (!d || !d.isDirectory()) {
    req->send(404, "text/plain", "lancio non trovato");
    return;
  }
  for (File f = d.openNextFile(); f; f = d.openNextFile()) {
    const String p = dir + "/" + f.name();
    f.close();
    SD.remove(p);
  }
  d.close();
  const bool ok = SD.rmdir(dir);
  req->send(ok ? 200 : 500, "text/plain", ok ? "ok" : "eliminazione fallita");
}

// Foto singola per puntare la camera; non disponibile durante la registrazione
static void handleSnapshot(AsyncWebServerRequest *req) {
  if (!CameraRec::isReady() || s_hooks.isRecording()) {
    req->send(409, "text/plain", "camera occupata");
    return;
  }
  // Con 2 buffer in modalità GRAB_LATEST il primo frame può essere vecchio
  camera_fb_t *fb = esp_camera_fb_get();
  if (fb) esp_camera_fb_return(fb);
  fb = esp_camera_fb_get();
  if (!fb) {
    req->send(500, "text/plain", "acquisizione fallita");
    return;
  }
  // Copia in PSRAM e restituisco subito il buffer alla camera
  const size_t len = fb->len;
  std::shared_ptr<uint8_t> jpg(static_cast<uint8_t *>(ps_malloc(len)), free);
  if (jpg) memcpy(jpg.get(), fb->buf, len);
  esp_camera_fb_return(fb);
  if (!jpg) {
    req->send(500, "text/plain", "memoria insufficiente");
    return;
  }
  AsyncWebServerResponse *res = new AsyncCallbackResponse(
      "image/jpeg", len, [jpg, len](uint8_t *buf, size_t maxLen, size_t index) -> size_t {
        const size_t n = min(maxLen, len - index);
        memcpy(buf, jpg.get() + index, n);
        return n;
      });
  res->addHeader("Cache-Control", "no-store");
  req->send(res);
}

bool begin(const RecorderHooks &hooks) {
  s_hooks = hooks;
  // Anche senza internet: l'ora arriva dal browser come epoch, il fuso si applica qui
  setenv("TZ", TIMEZONE, 1);
  tzset();

#ifdef WIFI_STA_SSID
  // Access point + client: la rete "WaterRocket" resta sempre disponibile
  // (al campo di lancio), e in più ci si collega alla rete configurata.
  // Nota: l'AP si sposta sul canale della rete a cui ci si collega.
  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);
  // Ora da internet appena la rete è collegata
  configTzTime(TIMEZONE, "pool.ntp.org", "time.google.com");
#else
  WiFi.mode(WIFI_AP);
#endif
  if (!WiFi.softAP(AP_SSID, AP_PASS)) {
    log_e("Avvio access point fallito");
    return false;
  }
  WiFi.setSleep(false);
  MDNS.begin(HOSTNAME);

  s_server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send(200, "text/html; charset=utf-8", reinterpret_cast<const uint8_t *>(WEB_PAGE),
              strlen(WEB_PAGE));
  });
  s_server.on("/api/status", HTTP_GET, handleStatus);
  s_server.on("/api/launches", HTTP_GET, handleLaunches);
  s_server.on("/api/launch", HTTP_POST, handleSaveLaunch, nullptr, handleSaveLaunchBody);
  s_server.on("/api/time", HTTP_POST, handleTime);
  s_server.on("/api/delete", HTTP_POST, handleDelete);
  s_server.on("/api/rec/start", HTTP_POST, [](AsyncWebServerRequest *req) {
    s_cmd = Cmd::Start;
    req->send(202, "text/plain", "ok");
  });
  s_server.on("/api/rec/stop", HTTP_POST, [](AsyncWebServerRequest *req) {
    s_cmd = Cmd::Stop;
    req->send(202, "text/plain", "ok");
  });
  s_server.on("/api/buzzer/stop", HTTP_POST, [](AsyncWebServerRequest *req) {
    s_cmd = Cmd::Silence;
    req->send(202, "text/plain", "ok");
  });
  // Pannello di simulazione: decollo, atterraggio, interruzione (torna a terra), cicalino
  s_server.on("/api/sim", HTTP_POST, [](AsyncWebServerRequest *req) {
    const String a = param(req, "action");
    Cmd c = a == "launch"     ? Cmd::SimLaunch
            : a == "land"     ? Cmd::SimLand
            : a == "abort"    ? Cmd::Silence
            : a == "buzz_on"  ? Cmd::SimBuzzOn
            : a == "buzz_off" ? Cmd::SimBuzzOff
                              : Cmd::None;
    if (c == Cmd::None) {
      req->send(400, "text/plain", "azione sconosciuta");
      return;
    }
    s_cmd = c;
    req->send(202, "text/plain", "ok");
  });
  // Anello LED: mode=auto (colori delle fasi) o mode=manual&color=RRGGBB&pct=0-100.
  // Valori atomici letti dal loop(), che è l'unico a disegnare sull'anello.
  s_server.on("/api/led", HTTP_POST, [](AsyncWebServerRequest *req) {
    if (param(req, "mode") == "manual") {
      const uint32_t rgb = strtoul(param(req, "color").c_str(), nullptr, 16);
      LedRing::setManual(true, rgb, constrain(param(req, "pct").toInt(), 0, 100));
    } else {
      const LedRing::Manual m = LedRing::manual();
      LedRing::setManual(false, m.rgb, m.pct);  // ricorda colore e luminosità per la prossima volta
    }
    req->send(200, "text/plain", "ok");
  });
  // Soglia di decollo: è un flag atomico letto dal task del sensore, non serve passare dal loop()
  // Calibrazione pre-lancio: la richiesta è un flag atomico raccolto dal task del sensore
  s_server.on("/api/precal", HTTP_POST, [](AsyncWebServerRequest *req) {
    Flight::startPrecal();
    req->send(202, "text/plain", "ok");
  });
  s_server.on("/api/testmode", HTTP_POST, [](AsyncWebServerRequest *req) {
    Flight::setTestMode(param(req, "on") == "1");
    req->send(200, "text/plain", "ok");
  });
  s_server.on("/file", HTTP_GET, handleFile);
  s_server.on("/thumb", HTTP_GET, handleThumb);
  s_server.on("/avi", HTTP_GET, handleAvi);
  s_server.on("/zip", HTTP_GET, handleZip);
  s_server.on("/snapshot.jpg", HTTP_GET, handleSnapshot);
  s_server.onNotFound([](AsyncWebServerRequest *req) { req->send(404, "text/plain", "non trovato"); });
  s_server.begin();
  MDNS.addService("http", "tcp", 80);

  s_started = true;
  return true;
}

void handle() {
  if (!s_started) return;

  // Comandi dalla pagina, eseguiti qui per non sovrapporsi a pulsante e seriale
  switch (s_cmd.exchange(Cmd::None)) {
    case Cmd::Start: s_hooks.start(); break;
    case Cmd::Stop: s_hooks.stop(); break;
    case Cmd::Silence: s_hooks.silence(); break;
    case Cmd::SimLaunch: Flight::forceLaunch(); break;
    case Cmd::SimLand: Flight::forceLand(); break;
    case Cmd::SimBuzzOn: Buzzer::beep(200, 800, 0); break;
    case Cmd::SimBuzzOff: Buzzer::stop(); break;
    default: break;
  }

#ifdef WIFI_STA_SSID
  // Segnala sulla seriale quando la connessione alla rete esterna cambia
  static bool wasConnected = false;
  const bool connected = WiFi.status() == WL_CONNECTED;
  if (connected != wasConnected) {
    wasConnected = connected;
    if (connected) {
      Serial.printf("WiFi:      collegato a \"%s\" -> http://%s\n", WIFI_STA_SSID,
                    WiFi.localIP().toString().c_str());
    } else {
      Serial.printf("WiFi:      scollegato da \"%s\"\n", WIFI_STA_SSID);
    }
  }
#endif
}

void printInfo() {
  if (!s_started) {
    Serial.println("WiFi:      spento");
    return;
  }
  Serial.printf("WiFi:      rete \"%s\" (password \"%s\") -> http://%s o http://%s.local\n",
                AP_SSID, AP_PASS, WiFi.softAPIP().toString().c_str(), HOSTNAME);
#ifdef WIFI_STA_SSID
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("           collegato a \"%s\" -> http://%s\n", WIFI_STA_SSID,
                  WiFi.localIP().toString().c_str());
  } else {
    Serial.printf("           non collegato a \"%s\"\n", WIFI_STA_SSID);
  }
#endif
}

}  // namespace Web
