#pragma once
#include <Arduino.h>
#include <SD.h>

#include <vector>

// Archivio ZIP "virtuale" di tutti i file di una cartella di lancio, costruito
// al volo senza scrivere niente sulla SD. I file sono archiviati senza
// compressione (JPEG e WAV non si comprimono quasi): così la dimensione totale
// è nota subito e il browser mostra la barra di avanzamento. Il CRC32 di ogni
// file si calcola mentre i dati passano e finisce nel data descriptor e nella
// directory centrale, che nel formato ZIP vengono dopo i dati.
// read() va chiamata con offset crescenti, come fa il web server asincrono.
class ZipSource {
 public:
  // false (con il motivo in error()) se la cartella non è leggibile o è vuota
  bool open(const String &dir);
  const char *error() const { return error_; }
  uint32_t size() const { return total_; }

  // Copia fino a len byte a partire da offset; ritorna i byte copiati
  size_t read(uint32_t offset, uint8_t *buf, size_t len);

 private:
  struct Entry {
    String path;     // percorso sulla SD
    String name;     // nome nell'archivio: REC_xxxx/file
    uint32_t size;
    uint16_t dosTime, dosDate;
    uint32_t localOff;  // posizione dell'header locale nell'archivio
    uint32_t crc;
    uint32_t crcPos;    // byte già conteggiati nel CRC (solo letture in ordine)
  };
  enum class Part : uint8_t { Local, Data, Descriptor, Central };
  struct Segment {
    uint32_t start, len;
    Part part;
    int entry;
  };

  size_t readData(Entry &e, uint32_t rel, uint8_t *buf, size_t len);
  void buildLocal(const Entry &e, std::vector<uint8_t> &out) const;
  void buildDescriptor(const Entry &e, std::vector<uint8_t> &out) const;
  void buildCentral();

  const char *error_ = "";
  std::vector<Entry> entries_;
  std::vector<Segment> segments_;
  std::vector<uint8_t> central_;  // directory centrale + fine archivio, creata a fine dati
  File file_;
  int openEntry_ = -1;
  uint32_t total_ = 0;
};
