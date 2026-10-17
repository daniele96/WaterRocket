#pragma once

// L'MPU6050 (accelerometro + giroscopio) sul bus I2C: come ci si parla.

#include <Arduino.h>

extern uint8_t mpuIndirizzo;   // 0x68, oppure 0x69 se il pin AD0 è collegato a 3,3 V

// Lo trova, gli chiede chi è e lo sveglia.
// fondoScala: 0 = ±2 g, 1 = ±4 g, 2 = ±8 g, 3 = ±16 g (il giroscopio sta sempre a ±250 °/s)
bool mpuSveglia(uint8_t fondoScala);

// Legge i 7 numeri grezzi: accelerazione X Y Z, temperatura, rotazione X Y Z
bool mpuLeggi(int16_t grezzi[7]);

// Taratura, come sul razzo: da fermo il totale deve valere 1 g. Si misura per 1 secondo quanto
// vale davvero e da lì in poi quello è «1 g». Ricorda anche la direzione del basso da fermo.
bool mpuTara();

// Quanti «punti» valgono 1 g: 16384 a ±2 g per il datasheet, il valore misurato dopo mpuTara()
float mpuPuntiPerG();

// La gravità da ferma, in g (dopo mpuTara): dove stava il «basso» al momento della taratura
extern float mpuRiposo[3];

// Quanti «punti» valgono 1 °/s (131 a ±250 °/s)
constexpr float MPU_PUNTI_PER_GRADO = 131.0f;
