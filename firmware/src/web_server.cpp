#include "web_server.h"

#include <ESPmDNS.h>
#include <SD.h>
#include <WebServer.h>
#include <WiFi.h>
#include <esp_camera.h>

#include "audio_rec.h"
#include "avi_export.h"
#include "camera_rec.h"
#include "storage.h"
#include "web_page.h"

// Rete creata dallo XIAO: funziona anche al campo di lancio, senza router.
static constexpr const char *AP_SSID = "WaterRocket";
static constexpr const char *AP_PASS = "razzo1234";  // almeno 8 caratteri
static constexpr const char *HOSTNAME = "rocket";    // http://rocket.local

namespace Web {

static WebServer s_server(80);
static RecorderHooks s_hooks;
static bool s_started = false;

// Accetta solo file dentro le cartelle di sessione, niente path traversal
static bool isSafePath(const String &p) {
  return p.startsWith("/REC_") && p.indexOf("..") < 0;
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

static void handleRoot() {
  s_server.send_P(200, "text/html; charset=utf-8", WEB_PAGE);
}

static void handleStatus() {
  const bool rec = s_hooks.isRecording();
  const float secs = CameraRec::elapsedMs() / 1000.0f;
  char buf[320];
  snprintf(buf, sizeof(buf),
           "{\"recording\":%s,\"dir\":\"%s\",\"secs\":%.1f,\"frames\":%lu,\"fps\":%.1f,"
           "\"audio\":%.1f,\"camera\":\"%s\",\"sd\":%s,\"sdTotalMB\":%llu,\"sdFreeMB\":%llu}",
           rec ? "true" : "false", rec ? s_hooks.sessionDir().c_str() : "", rec ? secs : 0.0f,
           (unsigned long)(rec ? CameraRec::frames() : 0),
           rec && secs > 0 ? CameraRec::frames() / secs : 0.0f, rec ? AudioRec::seconds() : 0.0f,
           CameraRec::isReady() ? CameraRec::sensorName() : "-",
           Storage::isMounted() ? "true" : "false",
           Storage::isMounted() ? SD.totalBytes() / (1024 * 1024) : 0ULL,
           Storage::isMounted() ? (SD.totalBytes() - SD.usedBytes()) / (1024 * 1024) : 0ULL);
  s_server.send(200, "application/json", buf);
}

static void handleSessions() {
  if (!Storage::isMounted()) {
    s_server.send(200, "application/json", "[]");
    return;
  }
  String out = "[";
  bool firstDir = true;
  File root = SD.open("/");
  for (File dir = root.openNextFile(); dir; dir = root.openNextFile()) {
    const String name = String("/") + dir.name();
    if (!dir.isDirectory() || !name.startsWith("/REC_")) continue;
    if (!firstDir) out += ",";
    firstDir = false;
    out += "{\"dir\":\"" + name + "\",\"info\":\"" + jsonEscape(readSmallFile(name + "/info.txt")) +
           "\",\"files\":[";
    bool firstFile = true;
    for (File f = dir.openNextFile(); f; f = dir.openNextFile()) {
      if (!firstFile) out += ",";
      firstFile = false;
      out += "{\"name\":\"" + String(f.name()) + "\",\"size\":" + String((unsigned)f.size()) + "}";
    }
    out += "]}";
  }
  out += "]";
  s_server.send(200, "application/json", out);
}

static const char *contentType(const String &path) {
  if (path.endsWith(".wav")) return "audio/wav";
  if (path.endsWith(".csv")) return "text/csv";
  if (path.endsWith(".txt")) return "text/plain";
  return "application/octet-stream";
}

static void handleFile() {
  const String path = s_server.arg("path");
  if (!isSafePath(path)) {
    s_server.send(400, "text/plain", "path non valido");
    return;
  }
  File f = SD.open(path);
  if (!f || f.isDirectory()) {
    s_server.send(404, "text/plain", "file non trovato");
    return;
  }
  if (s_server.hasArg("dl")) {
    // Nome scaricato: REC_0001_video.mjpeg
    String fname = path.substring(1);
    fname.replace('/', '_');
    s_server.sendHeader("Content-Disposition", "attachment; filename=\"" + fname + "\"");
  }
  s_server.streamFile(f, contentType(path));
  f.close();
}

static void handleAvi() {
  const String dir = s_server.arg("dir");
  if (!isSafePath(dir) || dir.indexOf('/', 1) >= 0) {
    s_server.send(400, "text/plain", "cartella non valida");
    return;
  }
  AviExport::stream(s_server, dir);
}

static void handleDelete() {
  const String dir = s_server.arg("dir");
  if (!isSafePath(dir) || dir.indexOf('/', 1) >= 0) {
    s_server.send(400, "text/plain", "cartella non valida");
    return;
  }
  if (s_hooks.isRecording() && dir == s_hooks.sessionDir()) {
    s_server.send(409, "text/plain", "registrazione in corso in questa cartella");
    return;
  }
  File d = SD.open(dir);
  if (!d || !d.isDirectory()) {
    s_server.send(404, "text/plain", "cartella non trovata");
    return;
  }
  for (File f = d.openNextFile(); f; f = d.openNextFile()) {
    const String p = dir + "/" + f.name();
    f.close();
    SD.remove(p);
  }
  d.close();
  const bool ok = SD.rmdir(dir);
  s_server.send(ok ? 200 : 500, "text/plain", ok ? "ok" : "eliminazione fallita");
}

static void handleRecStart() {
  s_hooks.start();
  s_server.send(s_hooks.isRecording() ? 200 : 500, "text/plain",
                s_hooks.isRecording() ? "ok" : "avvio fallito (SD?)");
}

static void handleRecStop() {
  s_hooks.stop();
  s_server.send(200, "text/plain", "ok");
}

// Foto singola per puntare la camera; non disponibile durante la registrazione
static void handleSnapshot() {
  if (!CameraRec::isReady() || s_hooks.isRecording()) {
    s_server.send(409, "text/plain", "camera occupata");
    return;
  }
  // Con 2 buffer in modalità GRAB_LATEST il primo frame può essere vecchio
  camera_fb_t *fb = esp_camera_fb_get();
  if (fb) esp_camera_fb_return(fb);
  fb = esp_camera_fb_get();
  if (!fb) {
    s_server.send(500, "text/plain", "acquisizione fallita");
    return;
  }
  s_server.sendHeader("Cache-Control", "no-store");
  s_server.setContentLength(fb->len);
  s_server.send(200, "image/jpeg", "");
  s_server.client().write(fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

bool begin(const RecorderHooks &hooks) {
  s_hooks = hooks;

  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(AP_SSID, AP_PASS)) {
    log_e("Avvio access point fallito");
    return false;
  }
  WiFi.setSleep(false);
  MDNS.begin(HOSTNAME);

  s_server.on("/", HTTP_GET, handleRoot);
  s_server.on("/api/status", HTTP_GET, handleStatus);
  s_server.on("/api/sessions", HTTP_GET, handleSessions);
  s_server.on("/api/rec/start", HTTP_POST, handleRecStart);
  s_server.on("/api/rec/stop", HTTP_POST, handleRecStop);
  s_server.on("/api/delete", HTTP_POST, handleDelete);
  s_server.on("/file", HTTP_GET, handleFile);
  s_server.on("/avi", HTTP_GET, handleAvi);
  s_server.on("/snapshot.jpg", HTTP_GET, handleSnapshot);
  s_server.onNotFound([] { s_server.send(404, "text/plain", "non trovato"); });
  s_server.begin();
  MDNS.addService("http", "tcp", 80);

  s_started = true;
  return true;
}

void handle() {
  if (s_started) s_server.handleClient();
}

void printInfo() {
  if (!s_started) {
    Serial.println("WiFi:      spento");
    return;
  }
  Serial.printf("WiFi:      rete \"%s\" (password \"%s\") -> http://%s o http://%s.local\n",
                AP_SSID, AP_PASS, WiFi.softAPIP().toString().c_str(), HOSTNAME);
}

}  // namespace Web
