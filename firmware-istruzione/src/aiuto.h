#pragma once

// Strumenti comuni a tutte le demo. Non serve guardarli per capire le demo.

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

extern bool modoGrafico;           // true = righe «>nome:valore» per Teleplot, false = testo
extern Adafruit_NeoPixel anello;   // l'anello di 8 LED, usato da più demo
                                   // (begin() solo in setup: ripeterlo stacca il pin dall'RMT)

// Aspetta ms millisecondi, ma smette se arriva un tasto: restituisce true se è stata interrotta
bool aspetta(uint32_t ms);

// Il titolo di una demo, ben visibile
void titolo(const char *nome);

// L'uscita di una misura. Testo: «nome valore unità» sulla stessa riga.
// Grafico: una riga «>nome:valore» per ogni misura, che Teleplot disegna.
void campo(const char *nome, float valore, const char *unita, int decimali = 2);
void barretta(float valore, float minimo, float massimo);   // [-----|###-----], solo nel testo
void aCapo();

// Bus I2C
void avviaBus();                    // libera il bus e lo avvia su D4 (SDA) e D5 (SCL)
bool presente(uint8_t indirizzo);   // qualcuno risponde «presente!» a questo indirizzo?
bool scriviRegistro(uint8_t indirizzo, uint8_t registro, uint8_t valore);
bool leggiRegistri(uint8_t indirizzo, uint8_t registro, uint8_t *dati, size_t quanti);

void preparaCicalino();             // pin D1 in uscita, con la massima corrente (40 mA)
void spegniTutto();                 // cicalino zitto, anello spento
