#pragma once

// La «ricetta» della Bosch per passare dai numeri grezzi del barometro a gradi e pascal.
// Ogni chip ha i suoi numeri di taratura, scritti in fabbrica: la ricetta li usa per
// correggere le misure. Le formule sono quelle dei datasheet di BMP180 e BMP280.

#include <Arduino.h>

namespace Ricetta {

// BMP180: 11 numeri di taratura (22 byte dal cassetto 0xAA)
bool bmp180Taratura(const uint8_t dati[22]);
float bmp180Temperatura(int32_t grezzo);             // °C; va chiamata prima della pressione
float bmp180Pressione(int32_t grezzo, uint8_t oss);  // Pa; oss = la precisione chiesta

// BMP280: 12 numeri di taratura (24 byte dal cassetto 0x88)
bool bmp280Taratura(const uint8_t dati[24]);
float bmp280Temperatura(int32_t grezzo);             // °C; va chiamata prima della pressione
float bmp280Pressione(int32_t grezzo);               // Pa

// Dalla pressione alla quota sopra il punto dove la pressione era p0
float quota(float pascal, float p0);

}  // namespace Ricetta
