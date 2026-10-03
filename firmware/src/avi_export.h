#pragma once
#include <Arduino.h>
#include <SD.h>

#include <vector>

// File AVI (video MJPEG + audio PCM) "virtuale", costruito al volo da
// <dir>/video.mjpeg, <dir>/frames.csv e <dir>/audio.wav senza modificarli.
// La struttura viene calcolata in open(); read() restituisce i byte del file
// a qualsiasi offset, così il web server asincrono può inviarlo a pezzi.
class AviSource {
 public:
  ~AviSource();

  // Ritorna false (con il motivo in error()) se la registrazione non è utilizzabile
  bool open(const String &dir);
  const char *error() const { return error_; }
  uint32_t size() const { return total_; }

  // Copia fino a len byte a partire da offset; ritorna i byte copiati
  size_t read(uint32_t offset, uint8_t *buf, size_t len);

 private:
  struct Chunk {
    uint32_t off;  // posizione dell'header del chunk nel file AVI
    uint32_t src;  // posizione dei dati nel file sorgente
    uint32_t len;  // dimensione dei dati (senza padding)
    bool audio;
  };

  size_t readChunk(const Chunk &c, uint32_t rel, uint8_t *buf, size_t len);
  size_t readIndex(uint32_t rel, uint8_t *buf, size_t len);

  const char *error_ = "";
  File vid_, wav_;
  std::vector<uint8_t> header_;  // RIFF + hdrl + header LIST movi
  Chunk *chunks_ = nullptr;      // in PSRAM
  size_t nChunks_ = 0;
  uint32_t moviFcc_ = 0;  // posizione del fourcc 'movi' (base degli offset in idx1)
  uint32_t idxStart_ = 0;
  uint32_t total_ = 0;
};
