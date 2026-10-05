#include "zip_export.h"

#include <time.h>

// Struttura dell'archivio (ZIP 2.0, metodo 0 = nessuna compressione):
//   per ogni file: header locale + dati + data descriptor (CRC e dimensioni)
//   directory centrale (un record per file) + record di fine archivio
// Il bit 3 dei flag dice che il CRC arriva nel data descriptor, dopo i dati:
// è ciò che permette di generare l'archivio in un solo passaggio.

namespace {

constexpr uint32_t SIG_LOCAL = 0x04034b50;
constexpr uint32_t SIG_DESCRIPTOR = 0x08074b50;
constexpr uint32_t SIG_CENTRAL = 0x02014b50;
constexpr uint32_t SIG_END = 0x06054b50;
constexpr uint16_t VERSION = 20;            // 2.0
constexpr uint16_t FLAG_DESCRIPTOR = 0x0008;
constexpr uint32_t LOCAL_FIXED = 30;
constexpr uint32_t DESCRIPTOR_LEN = 16;
constexpr uint32_t CENTRAL_FIXED = 46;
constexpr uint32_t END_LEN = 22;

uint32_t s_crcTable[256];

void initCrcTable() {
  if (s_crcTable[1]) return;
  for (uint32_t i = 0; i < 256; i++) {
    uint32_t c = i;
    for (int k = 0; k < 8; k++) c = c & 1 ? 0xEDB88320 ^ (c >> 1) : c >> 1;
    s_crcTable[i] = c;
  }
}

// CRC32 dello ZIP (stesso di zlib), aggiornabile a pezzi partendo da 0
uint32_t crc32(uint32_t crc, const uint8_t *p, size_t n) {
  crc = ~crc;
  while (n--) crc = s_crcTable[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
  return ~crc;
}

void put16(std::vector<uint8_t> &v, uint16_t x) { v.push_back(x); v.push_back(x >> 8); }
void put32(std::vector<uint8_t> &v, uint32_t x) { for (int i = 0; i < 4; i++) v.push_back(x >> (8 * i)); }
void putStr(std::vector<uint8_t> &v, const String &s) { v.insert(v.end(), s.c_str(), s.c_str() + s.length()); }

// Data e ora in formato MS-DOS; senza orologio impostato (prima del 1980) resta 1/1/1980
void dosDateTime(time_t t, uint16_t &time, uint16_t &date) {
  struct tm tm = {};
  if (t <= 0 || !localtime_r(&t, &tm) || tm.tm_year < 80) {
    time = 0;
    date = (1 << 5) | 1;
    return;
  }
  time = (tm.tm_hour << 11) | (tm.tm_min << 5) | (tm.tm_sec / 2);
  date = ((tm.tm_year - 80) << 9) | ((tm.tm_mon + 1) << 5) | tm.tm_mday;
}

}  // namespace

bool ZipSource::open(const String &dir) {
  initCrcTable();
  File d = SD.open(dir);
  if (!d || !d.isDirectory()) {
    error_ = "cartella del lancio non trovata";
    return false;
  }
  const String folder = dir.substring(dir.lastIndexOf('/') + 1);  // REC_xxxx
  for (File f = d.openNextFile(); f; f = d.openNextFile()) {
    if (!f.isDirectory()) {
      Entry e;
      e.path = dir + "/" + f.name();
      e.name = folder + "/" + f.name();
      e.size = f.size();
      dosDateTime(f.getLastWrite(), e.dosTime, e.dosDate);
      e.crc = 0;
      e.crcPos = 0;
      entries_.push_back(e);
    }
    f.close();
  }
  d.close();
  if (entries_.empty()) {
    error_ = "nessun file nel lancio";
    return false;
  }

  uint32_t off = 0;
  uint32_t centralLen = END_LEN;
  for (size_t i = 0; i < entries_.size(); i++) {
    Entry &e = entries_[i];
    e.localOff = off;
    const uint32_t lh = LOCAL_FIXED + e.name.length();
    segments_.push_back({off, lh, Part::Local, (int)i});
    off += lh;
    segments_.push_back({off, e.size, Part::Data, (int)i});
    off += e.size;
    segments_.push_back({off, DESCRIPTOR_LEN, Part::Descriptor, (int)i});
    off += DESCRIPTOR_LEN;
    centralLen += CENTRAL_FIXED + e.name.length();
  }
  segments_.push_back({off, centralLen, Part::Central, -1});
  total_ = off + centralLen;
  return true;
}

void ZipSource::buildLocal(const Entry &e, std::vector<uint8_t> &v) const {
  put32(v, SIG_LOCAL);
  put16(v, VERSION);
  put16(v, FLAG_DESCRIPTOR);
  put16(v, 0);  // metodo: nessuna compressione
  put16(v, e.dosTime);
  put16(v, e.dosDate);
  put32(v, 0);  // CRC: nel data descriptor
  // Le dimensioni sono già note: metterle anche qui aiuta i lettori che non usano la directory centrale
  put32(v, e.size);
  put32(v, e.size);
  put16(v, e.name.length());
  put16(v, 0);  // campo extra
  putStr(v, e.name);
}

void ZipSource::buildDescriptor(const Entry &e, std::vector<uint8_t> &v) const {
  put32(v, SIG_DESCRIPTOR);
  put32(v, e.crc);
  put32(v, e.size);
  put32(v, e.size);
}

void ZipSource::buildCentral() {
  std::vector<uint8_t> &v = central_;
  uint32_t cdStart = segments_.back().start;
  for (const Entry &e : entries_) {
    put32(v, SIG_CENTRAL);
    put16(v, VERSION);  // creato con
    put16(v, VERSION);  // necessario per estrarre
    put16(v, FLAG_DESCRIPTOR);
    put16(v, 0);
    put16(v, e.dosTime);
    put16(v, e.dosDate);
    put32(v, e.crc);
    put32(v, e.size);
    put32(v, e.size);
    put16(v, e.name.length());
    put16(v, 0);  // extra
    put16(v, 0);  // commento
    put16(v, 0);  // disco
    put16(v, 0);  // attributi interni
    put32(v, 0);  // attributi esterni
    put32(v, e.localOff);
    putStr(v, e.name);
  }
  const uint32_t cdLen = v.size();
  put32(v, SIG_END);
  put16(v, 0);
  put16(v, 0);
  put16(v, entries_.size());
  put16(v, entries_.size());
  put32(v, cdLen);
  put32(v, cdStart);
  put16(v, 0);  // commento
}

size_t ZipSource::readData(Entry &e, uint32_t rel, uint8_t *buf, size_t len) {
  const int idx = &e - entries_.data();
  if (openEntry_ != idx) {
    if (file_) file_.close();
    file_ = SD.open(e.path);
    openEntry_ = idx;
  }
  if (!file_) return 0;
  if (file_.position() != rel) file_.seek(rel);
  const int n = file_.read(buf, len);
  if (n <= 0) return 0;
  // CRC solo sui byte nuovi, nell'ordine: un pezzo ri-letto non viene contato due volte
  if (rel <= e.crcPos && rel + n > e.crcPos) {
    const uint32_t skip = e.crcPos - rel;
    e.crc = crc32(e.crc, buf + skip, n - skip);
    e.crcPos = rel + n;
  }
  if (e.crcPos >= e.size) {  // file finito: lo chiudo subito
    file_.close();
    openEntry_ = -1;
  }
  return n;
}

size_t ZipSource::read(uint32_t offset, uint8_t *buf, size_t len) {
  size_t done = 0;
  std::vector<uint8_t> tmp;
  for (const Segment &s : segments_) {
    if (done == len) break;
    const uint32_t pos = offset + done;
    if (pos < s.start || pos >= s.start + s.len) continue;
    const uint32_t rel = pos - s.start;
    const size_t want = std::min<size_t>(len - done, s.len - rel);

    if (s.part == Part::Data) {
      const size_t n = readData(entries_[s.entry], rel, buf + done, want);
      done += n;
      if (n < want) break;  // file più corto del previsto: meglio fermarsi che inviare spazzatura
      continue;
    }

    const std::vector<uint8_t> *src = &tmp;
    tmp.clear();
    if (s.part == Part::Local) {
      buildLocal(entries_[s.entry], tmp);
    } else if (s.part == Part::Descriptor) {
      buildDescriptor(entries_[s.entry], tmp);
    } else {
      if (central_.empty()) buildCentral();
      src = &central_;
    }
    memcpy(buf + done, src->data() + rel, want);
    done += want;
  }
  return done;
}
