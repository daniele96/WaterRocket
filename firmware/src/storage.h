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

// Elenca le cartelle di sessione e i file contenuti, con le dimensioni.
void listFiles();

}  // namespace Storage
