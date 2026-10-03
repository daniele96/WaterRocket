#include "avi_export.h"

#include <SD.h>
#include <WiFi.h>

#include "audio_rec.h"

// Struttura del file generato (AVI 1.0 / RIFF):
//   RIFF 'AVI '
//     LIST 'hdrl'  avih + LIST 'strl' video (+ LIST 'strl' audio)
//     LIST 'movi'  per ogni frame: '00dc' (JPEG) seguito da '01wb' (audio
//                  dello stesso intervallo di tempo, preso da frames.csv)
//     idx1         indice dei chunk, per poter saltare avanti/indietro
// Tutte le dimensioni sono calcolate prima, così il browser vede la
// Content-Length esatta e la barra di avanzamento del download.

namespace AviExport {

namespace {

constexpr uint32_t WAV_HEADER = 44;
constexpr uint32_t AUDIO_BYTES_PER_MS = AudioRec::SAMPLE_RATE * 2 / 1000;
constexpr uint32_t AVIIF_KEYFRAME = 0x10;
constexpr uint32_t AVIF_HASINDEX = 0x10;
constexpr uint32_t MAX_AVI_SIZE = 0x7FFFFFFF;  // oltre 2 GB molti player non leggono AVI 1.0

struct Frame {
  uint32_t t;     // ms dall'inizio della registrazione
  uint32_t size;  // byte del JPEG
};

uint32_t pad2(uint32_t n) { return (n + 1) & ~1u; }

// Scrittura bufferizzata verso il client HTTP
class Out {
 public:
  explicit Out(WiFiClient &c) : c_(c) {}

  bool ok() const { return ok_; }

  void put(const void *p, size_t n) {
    const uint8_t *b = static_cast<const uint8_t *>(p);
    while (n && ok_) {
      if (len_ == sizeof(buf_)) flush();
      const size_t k = min(n, sizeof(buf_) - len_);
      memcpy(buf_ + len_, b, k);
      len_ += k;
      b += k;
      n -= k;
    }
  }
  void u32(uint32_t v) {
    const uint8_t b[4] = {(uint8_t)v, (uint8_t)(v >> 8), (uint8_t)(v >> 16), (uint8_t)(v >> 24)};
    put(b, 4);
  }
  void u16(uint16_t v) {
    const uint8_t b[2] = {(uint8_t)v, (uint8_t)(v >> 8)};
    put(b, 2);
  }
  void fcc(const char *s) { put(s, 4); }

  // Copia n byte dal file corrente
  void copy(File &f, uint32_t n) {
    while (n && ok_) {
      if (len_ == sizeof(buf_)) flush();
      const size_t k = min((size_t)n, sizeof(buf_) - len_);
      const size_t r = f.read(buf_ + len_, k);
      if (r != k) {  // file più corto del previsto: riempio di zeri per non sfasare il RIFF
        memset(buf_ + len_ + r, 0, k - r);
      }
      len_ += k;
      n -= k;
    }
  }

  void flush() {
    size_t off = 0;
    while (ok_ && off < len_) {
      const size_t w = c_.write(buf_ + off, len_ - off);
      if (w == 0) ok_ = false;  // client disconnesso
      off += w;
    }
    len_ = 0;
  }

 private:
  WiFiClient &c_;
  uint8_t buf_[4096];
  size_t len_ = 0;
  bool ok_ = true;
};

// Legge frames.csv (frame,t_ms,bytes) in un array in PSRAM
Frame *loadFrames(const String &path, size_t &count) {
  count = 0;
  File f = SD.open(path);
  if (!f) return nullptr;
  // Ogni riga è lunga almeno 6 caratteri ("0,0,1\n"): stima per eccesso
  const size_t cap = f.size() / 6 + 1;
  Frame *frames = static_cast<Frame *>(ps_malloc(cap * sizeof(Frame)));
  if (!frames) return nullptr;

  char line[48];
  size_t ll = 0;
  bool header = true;
  uint8_t buf[512];
  for (int r; (r = f.read(buf, sizeof(buf))) > 0;) {
    for (int i = 0; i < r; i++) {
      const char c = buf[i];
      if (c != '\n') {
        if (ll < sizeof(line) - 1) line[ll++] = c;
        continue;
      }
      line[ll] = 0;
      ll = 0;
      if (header) { header = false; continue; }
      char *p = strchr(line, ',');
      if (!p) continue;
      char *q = strchr(p + 1, ',');
      if (!q) continue;
      const uint32_t size = strtoul(q + 1, nullptr, 10);
      if (size && count < cap) frames[count++] = {(uint32_t)strtoul(p + 1, nullptr, 10), size};
    }
  }
  f.close();
  return frames;
}

// Larghezza/altezza dal marker SOF del primo JPEG
void jpegSize(File &vid, uint32_t len, uint16_t &w, uint16_t &h) {
  w = 640;
  h = 480;
  uint8_t b[1024];
  const size_t n = vid.read(b, min((size_t)len, sizeof(b)));
  vid.seek(0);
  for (size_t i = 0; i + 8 < n; i++) {
    if (b[i] == 0xFF && (b[i + 1] == 0xC0 || b[i + 1] == 0xC2)) {
      h = (b[i + 5] << 8) | b[i + 6];
      w = (b[i + 7] << 8) | b[i + 8];
      return;
    }
  }
}

void writeStrh(Out &o, const char *type, const char *handler, uint32_t scale, uint32_t rate,
               uint32_t length, uint32_t bufSize, uint32_t sampleSize, uint16_t w, uint16_t h) {
  o.fcc("strh");
  o.u32(56);
  o.fcc(type);
  o.fcc(handler);
  o.u32(0);           // flags
  o.u16(0);           // priority
  o.u16(0);           // language
  o.u32(0);           // initial frames
  o.u32(scale);
  o.u32(rate);        // rate/scale = campioni (o frame) al secondo
  o.u32(0);           // start
  o.u32(length);
  o.u32(bufSize);
  o.u32(0xFFFFFFFF);  // quality: default
  o.u32(sampleSize);
  o.u16(0); o.u16(0); o.u16(w); o.u16(h);  // rcFrame
}

}  // namespace

void stream(WebServer &server, const String &dir) {
  size_t n = 0;
  Frame *frames = loadFrames(dir + "/frames.csv", n);
  File vid = SD.open(dir + "/video.mjpeg");
  if (!frames || !vid || n == 0) {
    free(frames);
    server.send(404, "text/plain", "registrazione incompleta o vuota");
    return;
  }

  // Se è saltata la corrente il CSV può elencare frame non ancora scritti nel video
  uint64_t sum = 0;
  size_t valid = 0;
  uint32_t maxFrame = 0;
  while (valid < n && sum + frames[valid].size <= vid.size()) {
    sum += frames[valid].size;
    maxFrame = max(maxFrame, frames[valid].size);
    valid++;
  }
  n = valid;
  if (n == 0) {
    free(frames);
    server.send(404, "text/plain", "nessun frame valido");
    return;
  }

  File wav = SD.open(dir + "/audio.wav");
  // Uso la dimensione reale del file: l'header potrebbe non essere aggiornato
  const uint32_t audioLen = (wav && wav.size() > WAV_HEADER) ? (wav.size() - WAV_HEADER) & ~1u : 0;
  const bool hasAudio = audioLen > 0;

  // Inizio dell'audio da accodare a ogni frame: [audioAt(i), audioAt(i+1))
  auto audioAt = [&](size_t i) -> uint32_t {
    if (i == 0) return 0;
    if (i >= n) return audioLen;
    return min(frames[i].t * AUDIO_BYTES_PER_MS, audioLen) & ~1u;
  };

  const uint32_t durMs = n > 1 ? frames[n - 1].t - frames[0].t : 1000;
  const float fps = n > 1 && durMs ? (n - 1) * 1000.0f / durMs : 1.0f;
  uint16_t w, h;
  jpegSize(vid, frames[0].size, w, h);

  // --- Calcolo delle dimensioni ---
  uint32_t moviData = 0, chunks = 0;
  for (size_t i = 0; i < n; i++) {
    moviData += 8 + pad2(frames[i].size);
    chunks++;
    const uint32_t a0 = audioAt(i), a1 = audioAt(i + 1);
    if (hasAudio && a1 > a0) {
      moviData += 8 + (a1 - a0);
      chunks++;
    }
  }
  const uint32_t strlVideo = 4 + (8 + 56) + (8 + 40);
  const uint32_t strlAudio = 4 + (8 + 56) + (8 + 18);
  const uint32_t hdrl = 4 + (8 + 56) + (8 + strlVideo) + (hasAudio ? 8 + strlAudio : 0);
  const uint32_t movi = 4 + moviData;
  const uint32_t idx1 = 16 * chunks;
  const uint64_t riff = 4ULL + (8 + hdrl) + (8 + movi) + (8 + idx1);
  if (8 + riff > MAX_AVI_SIZE) {
    free(frames);
    server.send(413, "text/plain", "registrazione troppo lunga per un AVI: scarica i file separati");
    return;
  }

  String fname = dir.substring(1) + ".avi";
  server.sendHeader("Content-Disposition", "attachment; filename=\"" + fname + "\"");
  server.setContentLength(8 + riff);
  server.send(200, "video/x-msvideo", "");

  WiFiClient client = server.client();
  Out o(client);

  // --- Header ---
  o.fcc("RIFF"); o.u32(riff); o.fcc("AVI ");
  o.fcc("LIST"); o.u32(hdrl); o.fcc("hdrl");

  o.fcc("avih"); o.u32(56);
  o.u32((uint32_t)(1000000.0f / fps));  // microsecondi per frame
  o.u32(0);                              // max bytes/s
  o.u32(0);                              // padding granularity
  o.u32(AVIF_HASINDEX);
  o.u32(n);                              // frame totali
  o.u32(0);                              // initial frames
  o.u32(hasAudio ? 2 : 1);               // stream
  o.u32(maxFrame);                       // buffer suggerito
  o.u32(w); o.u32(h);
  o.u32(0); o.u32(0); o.u32(0); o.u32(0);

  o.fcc("LIST"); o.u32(strlVideo); o.fcc("strl");
  writeStrh(o, "vids", "MJPG", 1000, (uint32_t)(fps * 1000), n, maxFrame, 0, w, h);
  o.fcc("strf"); o.u32(40);  // BITMAPINFOHEADER
  o.u32(40); o.u32(w); o.u32(h);
  o.u16(1); o.u16(24);
  o.fcc("MJPG");
  o.u32((uint32_t)w * h * 3);
  o.u32(0); o.u32(0); o.u32(0); o.u32(0);

  if (hasAudio) {
    o.fcc("LIST"); o.u32(strlAudio); o.fcc("strl");
    writeStrh(o, "auds", "\0\0\0\0", 2, AudioRec::SAMPLE_RATE * 2, audioLen / 2, 8192, 2, 0, 0);
    o.fcc("strf"); o.u32(18);  // WAVEFORMATEX, PCM mono 16 bit
    o.u16(1); o.u16(1);
    o.u32(AudioRec::SAMPLE_RATE); o.u32(AudioRec::SAMPLE_RATE * 2);
    o.u16(2); o.u16(16); o.u16(0);
  }

  // --- Dati ---
  o.fcc("LIST"); o.u32(movi); o.fcc("movi");
  if (hasAudio) wav.seek(WAV_HEADER);
  for (size_t i = 0; i < n && o.ok(); i++) {
    o.fcc("00dc"); o.u32(frames[i].size);
    o.copy(vid, frames[i].size);
    if (frames[i].size & 1) o.put("\0", 1);
    const uint32_t a0 = audioAt(i), a1 = audioAt(i + 1);
    if (hasAudio && a1 > a0) {
      o.fcc("01wb"); o.u32(a1 - a0);
      o.copy(wav, a1 - a0);
    }
  }

  // --- Indice (offset relativi al fourcc 'movi') ---
  o.fcc("idx1"); o.u32(idx1);
  uint32_t off = 4;
  for (size_t i = 0; i < n && o.ok(); i++) {
    o.fcc("00dc"); o.u32(AVIIF_KEYFRAME); o.u32(off); o.u32(frames[i].size);
    off += 8 + pad2(frames[i].size);
    const uint32_t a0 = audioAt(i), a1 = audioAt(i + 1);
    if (hasAudio && a1 > a0) {
      o.fcc("01wb"); o.u32(AVIIF_KEYFRAME); o.u32(off); o.u32(a1 - a0);
      off += 8 + (a1 - a0);
    }
  }
  o.flush();

  vid.close();
  if (wav) wav.close();
  free(frames);
}

}  // namespace AviExport
