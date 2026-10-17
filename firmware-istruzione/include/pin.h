#pragma once

// I pin della XIAO ESP32-S3 Sense, collegati come sul razzo (vedi ../cablaggio.md).
// «D4» è il nome scritto sulla scheda, 5 è il numero che usa il programma (GPIO).

constexpr int PIN_SDA = 5;        // D4: i dati del bus I2C
constexpr int PIN_SCL = 6;        // D5: l'orologio del bus I2C
constexpr int PIN_CICALINO = 2;   // D1: il cicalino attivo
constexpr int PIN_ANELLO = 4;     // D3: i dati dell'anello WS2812B
constexpr int NUM_LED = 8;        // LED dell'anello

// Microfono digitale della scheda Sense: collegato già sulla scheda, non si cabla
constexpr int PIN_MIC_CLK = 42;
constexpr int PIN_MIC_DATI = 41;
