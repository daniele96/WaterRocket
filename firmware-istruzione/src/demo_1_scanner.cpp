// Demo 1 · Chi c'è sul bus I2C?
// La XIAO fa l'appello: chiama ogni indirizzo da 1 a 127 e ascolta chi risponde.

#include "aiuto.h"
#include "demo.h"

// Gli indirizzi che conosciamo, come i nomi sul registro di classe
static const char *chiE(uint8_t indirizzo) {
  switch (indirizzo) {
    case 0x68: return "MPU6050: accelerometro e giroscopio";
    case 0x69: return "MPU6050 con il pin AD0 a 3,3 V";
    case 0x76: return "BMP280: barometro";
    case 0x77: return "BMP180 o BMP280: barometro";
    case 0x23: return "BH1750: sensore di luce";
    case 0x29: return "TSL2591: sensore di luce";
    default:   return "non lo conosco";
  }
}

bool scannerInizio() {
  titolo("1 · CHI C'È SUL BUS I2C?");
  Serial.println("La XIAO chiama uno per uno gli indirizzi da 1 a 127.");
  Serial.println("Chi c'è risponde «presente!» (ACK) tirando giù il filo SDA.");
  return true;
}

void scannerRipeti() {
  Serial.println("Appello:");
  int trovati = 0;
  for (uint8_t indirizzo = 1; indirizzo < 127; indirizzo++) {
    if (!presente(indirizzo)) continue;   // silenzio: non c'è nessuno
    trovati++;
    char bit[8];   // l'indirizzo come lo vede il filo: 7 bit
    for (int i = 0; i < 7; i++) bit[i] = (indirizzo >> (6 - i)) & 1 ? '1' : '0';
    bit[7] = 0;
    Serial.printf("  0x%02X  in bit %s  → %s\n", indirizzo, bit, chiE(indirizzo));
  }
  if (trovati == 0) {
    Serial.println("  Nessuno! Controlla alimentazione e fili: SDA su D4, SCL su D5.");
  }
  Serial.printf("Presenti: %d. Rifaccio l'appello fra 3 secondi.\n\n", trovati);
  aspetta(3000);
}
