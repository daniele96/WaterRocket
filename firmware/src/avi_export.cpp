#include "avi_export.h"

#include "audio_rec.h"

// Struttura del file generato (AVI 1.0 / RIFF):
//   RIFF 'AVI '
//     LIST 'hdrl'  avih + LIST 'strl' video (+ LIST 'strl' audio)
//     LIST 'movi'  per ogni frame: '00dc' (JPEG) seguito da '01wb' (audio
//                  dello stesso intervallo di tempo, preso da frames.csv)
//     idx1         indice dei chunk, per poter saltare avanti/indietro
// Tutte le dimensioni sono calcolate prima, così il browser vede la
// Content-Length esatta e la barra di avanzamento del download.

namespace {

constexpr uint32_t WAV_HEADER = 44;
constexpr uint32_t AUDIO_BYTES_PER_MS = AudioRec::SAMPLE_RATE * 2 / 1000;
constexpr uint32_t AVIIF_KEYFRAME = 0x10;
constexpr uint32_t AVIF_HASINDEX = 0x10;
constexpr uint64_t MAX_AVI_SIZE = 0x7FFFFFFF;  // oltre 2 GB molti player non leggono AVI 1.0

struct Frame {
  uint32_t t;     // ms dall'inizio della registrazione
  uint32_t size;  // byte del JPEG
};

uint32_t pad2(uint32_t n) { return (n + 1) & ~1u; }

class Buf {
 public:
  explicit Buf(std::vector<uint8_t> &v) : v_(v) {}
  void u32(uint32_t x) { for (int i = 0; i < 4; i++) v_.push_back(x >> (8 * i)); }
  void u16(uint16_t x) { v_.push_back(x); v_.push_back(x >> 8); }
  void fcc(const char *s) { v_.insert(v_.end(), s, s + 4); }

 private:
  std::vector<uint8_t> &v_;
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
  return frames;
}

// Larghezza/altezza dal marker SOF del primo JPEG
void jpegSize(File &vid, uint32_t len, uint16_t &w, uint16_t &h) {
  w = 800;
  h = 600;
  uint8_t b[1024];
  vid.seek(0);
  const size_t n = vid.read(b, min((size_t)len, sizeof(b)));
  for (size_t i = 0; i + 8 < n; i++) {
    if (b[i] == 0xFF && (b[i + 1] == 0xC0 || b[i + 1] == 0xC2)) {
      h = (b[i + 5] << 8) | b[i + 6];
      w = (b[i + 7] << 8) | b[i + 8];
      return;
    }
  }
}

void writeStrh(Buf &o, const char *type, const char *handler, uint32_t scale, uint32_t rate,
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

AviSource::~AviSource() {
  free(chunks_);
}

bool AviSource::open(const String &dir) {
  size_t n = 0;
  Frame *frames = loadFrames(dir + "/frames.csv", n);
  vid_ = SD.open(dir + "/video.mjpeg");
  if (!frames || !vid_ || n == 0) {
    free(frames);
    error_ = "registrazione incompleta o vuota";
    return false;
  }

  // Se è saltata la corrente il CSV può elencare frame non ancora scritti nel video
  uint64_t sum = 0;
  size_t valid = 0;
  uint32_t maxFrame = 0;
  while (valid < n && sum + frames[valid].size <= vid_.size()) {
    sum += frames[valid].size;
    maxFrame = max(maxFrame, frames[valid].size);
    valid++;
  }
  n = valid;
  if (n == 0) {
    free(frames);
    error_ = "nessun frame valido";
    return false;
  }

  wav_ = SD.open(dir + "/audio.wav");
  // Uso la dimensione reale del file: l'header potrebbe non essere aggiornato
  const uint32_t audioLen = (wav_ && wav_.size() > WAV_HEADER) ? (wav_.size() - WAV_HEADER) & ~1u : 0;
  const bool hasAudio = audioLen > 0;

  // Audio da accodare a ogni frame: [audioAt(i), audioAt(i+1))
  auto audioAt = [&](size_t i) -> uint32_t {
    if (i == 0) return 0;
    if (i >= n) return audioLen;
    return min(frames[i].t * AUDIO_BYTES_PER_MS, audioLen) & ~1u;
  };

  const uint32_t durMs = n > 1 ? frames[n - 1].t - frames[0].t : 1000;
  const float fps = n > 1 && durMs ? (n - 1) * 1000.0f / durMs : 1.0f;
  uint16_t w, h;
  jpegSize(vid_, frames[0].size, w, h);

  // --- Tabella dei chunk (posizioni nel file AVI e nei file sorgente) ---
  chunks_ = static_cast<Chunk *>(ps_malloc(2 * n * sizeof(Chunk)));
  if (!chunks_) {
    free(frames);
    error_ = "memoria insufficiente";
    return false;
  }
  const uint32_t strlVideo = 4 + (8 + 56) + (8 + 40);
  const uint32_t strlAudio = 4 + (8 + 56) + (8 + 18);
  const uint32_t hdrl = 4 + (8 + 56) + (8 + strlVideo) + (hasAudio ? 8 + strlAudio : 0);
  moviFcc_ = 12 + 8 + hdrl + 8;  // RIFF(12) + LIST hdrl + 'LIST'+size, poi 'movi'

  uint64_t off = moviFcc_ + 4;
  uint32_t vsrc = 0;
  nChunks_ = 0;
  for (size_t i = 0; i < n; i++) {
    chunks_[nChunks_++] = {(uint32_t)off, vsrc, frames[i].size, false};
    off += 8 + pad2(frames[i].size);
    vsrc += frames[i].size;
    const uint32_t a0 = audioAt(i), a1 = audioAt(i + 1);
    if (hasAudio && a1 > a0) {
      chunks_[nChunks_++] = {(uint32_t)off, WAV_HEADER + a0, a1 - a0, true};
      off += 8 + (a1 - a0);
    }
    if (off > MAX_AVI_SIZE) break;
  }
  free(frames);
  if (off + 8 + 16ULL * nChunks_ > MAX_AVI_SIZE) {
    error_ = "registrazione troppo lunga per un AVI: scarica i file separati";
    return false;
  }
  idxStart_ = off;
  const uint32_t movi = idxStart_ - moviFcc_;
  total_ = idxStart_ + 8 + 16 * nChunks_;

  // --- Header ---
  header_.clear();
  header_.reserve(moviFcc_ + 4);
  Buf o(header_);
  o.fcc("RIFF"); o.u32(total_ - 8); o.fcc("AVI ");
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

  o.fcc("LIST"); o.u32(movi); o.fcc("movi");
  return true;
}

size_t AviSource::read(uint32_t offset, uint8_t *buf, size_t len) {
  size_t done = 0;
  while (done < len && offset < total_) {
    size_t n = 0;
    if (offset < header_.size()) {
      n = min(len - done, (size_t)(header_.size() - offset));
      memcpy(buf + done, header_.data() + offset, n);
    } else if (offset < idxStart_) {
      // Ricerca binaria del chunk che contiene offset
      size_t lo = 0, hi = nChunks_ - 1;
      while (lo < hi) {
        const size_t mid = (lo + hi + 1) / 2;
        if (chunks_[mid].off <= offset) lo = mid; else hi = mid - 1;
      }
      n = readChunk(chunks_[lo], offset - chunks_[lo].off, buf + done, len - done);
    } else {
      n = readIndex(offset - idxStart_, buf + done, len - done);
    }
    if (n == 0) break;
    done += n;
    offset += n;
  }
  return done;
}

// Byte di un chunk: header (8) + dati dal file sorgente + eventuale byte di padding
size_t AviSource::readChunk(const Chunk &c, uint32_t rel, uint8_t *buf, size_t len) {
  if (rel < 8) {
    uint8_t h[8];
    memcpy(h, c.audio ? "01wb" : "00dc", 4);
    for (int i = 0; i < 4; i++) h[4 + i] = c.len >> (8 * i);
    const size_t n = min(len, (size_t)(8 - rel));
    memcpy(buf, h + rel, n);
    return n;
  }
  const uint32_t dataRel = rel - 8;
  if (dataRel < c.len) {
    File &f = c.audio ? wav_ : vid_;
    const uint32_t pos = c.src + dataRel;
    if (f.position() != pos) f.seek(pos);
    const size_t want = min(len, (size_t)(c.len - dataRel));
    const size_t got = f.read(buf, want);
    if (got < want) memset(buf + got, 0, want - got);  // file più corto: zeri per non sfasare il RIFF
    return want;
  }
  buf[0] = 0;  // padding a dimensione pari
  return 1;
}

// Byte di idx1: header (8) + 16 byte per chunk (offset relativi al fourcc 'movi')
size_t AviSource::readIndex(uint32_t rel, uint8_t *buf, size_t len) {
  size_t done = 0;
  while (done < len) {
    uint8_t e[16];
    uint32_t base, size;
    if (rel < 8) {
      memcpy(e, "idx1", 4);
      const uint32_t s = 16 * nChunks_;
      for (int i = 0; i < 4; i++) e[4 + i] = s >> (8 * i);
      base = 0;
      size = 8;
    } else {
      const size_t k = (rel - 8) / 16;
      if (k >= nChunks_) break;
      const Chunk &c = chunks_[k];
      const uint32_t v[4] = {0, AVIIF_KEYFRAME, c.off - moviFcc_, c.len};
      memcpy(e, c.audio ? "01wb" : "00dc", 4);
      for (int j = 1; j < 4; j++)
        for (int i = 0; i < 4; i++) e[4 * j + i] = v[j] >> (8 * i);
      base = 8 + 16 * k;
      size = 16;
    }
    const size_t n = min(len - done, (size_t)(size - (rel - base)));
    memcpy(buf + done, e + (rel - base), n);
    done += n;
    rel += n;
  }
  return done;
}
