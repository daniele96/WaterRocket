#pragma once
#include <Arduino.h>

namespace Storage {

// Monta la microSD. Ritorna false se la scheda manca o non è leggibile.
bool begin();
bool isMounted();

// Se la microSD non ha un volume FAT valido (es. exFAT), la formatta in FAT32
// (CANCELLA TUTTO) e la monta. Una SD già FAT viene solo montata.
bool format();

// Crea una nuova cartella di sessione /REC_0001, /REC_0002, ... e ne ritorna il path
// (stringa vuota in caso di errore).
String newSessionDir();

void printInfo();

// Scrive un file di prova da 4 MB e ritorna la velocità in byte/s (0 se fallisce).
uint32_t writeSpeedTest();

// Elenca le cartelle di sessione e i file contenuti, con le dimensioni.
void listFiles();

}  // namespace Storage
