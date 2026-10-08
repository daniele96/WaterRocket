#pragma once
#include <Arduino.h>

// Altimetro sul bus I2C dell'MPU6050, riconosciuto all'avvio dal registro di identificazione:
//   BMP180 (modulo GY-68, 4 pin)     solo 0x77, misura a comando, pressione nuova a 100 Hz
//   BMP280 / BME280 (6 pin)           0x76 (SDO a GND) o 0x77, misura continua a ~44 Hz
// read() va chiamata da un solo task (quello di flight.cpp), che è l'unico a parlare sul bus.
// model() e address() sono fissati da begin() e si possono leggere da qualunque task.
namespace Baro {

// Cerca il sensore a 0x76 e 0x77 e lo configura. Wire già avviato.
bool begin();
bool isReady();
uint8_t address();
const char *model();  // "BMP180", "BMP280", "BME280" o "nessuno"

// Ultima misura disponibile: pressione in Pa, temperatura in °C. false se la lettura fallisce.
// Col BMP180 ogni chiamata fa anche avanzare la misura successiva: va chiamata a intervalli
// regolari di almeno 8 ms (il task di volo la chiama ogni 10 ms).
bool read(float &pressPa, float &tempC);

// Quota rispetto alla pressione di riferimento p0 (formula barometrica standard)
float altitude(float pressPa, float p0Pa);

}  // namespace Baro
