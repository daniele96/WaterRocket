// Demo 2 · Barometro: dalla pressione dell'aria alla quota.
// Funziona sia col BMP180 (quello del razzo) sia col BMP280: lo riconosce da solo.

#include <algorithm>

#include "aiuto.h"
#include "barometro_ricetta.h"
#include "demo.h"

static uint8_t indirizzo = 0;   // 0x77 o 0x76
static bool eBmp180 = false;    // altrimenti è un BMP280
static float p0 = 0;            // la pressione di partenza: lì la quota è 0

static float ultime[5];         // le ultime 5 pressioni, per la mediana
static int quante = 0, prossima = 0;
static float filtrata = NAN;

// BMP180: misura solo a comando. Si scrive il comando nel cassetto 0xF4,
// si aspetta che finisca, si legge il risultato dal cassetto 0xF6.
static bool misuraBmp180(int32_t &grezzo, float &pascal, float &gradi) {
  uint8_t d[3];
  // Temperatura: comando 0x2E, pronta in 4,5 ms, 2 byte
  if (!scriviRegistro(indirizzo, 0xF4, 0x2E)) return false;
  delay(5);
  if (!leggiRegistri(indirizzo, 0xF6, d, 2)) return false;
  gradi = Ricetta::bmp180Temperatura((d[0] << 8) | d[1]);
  // Pressione: comando 0x34 con precisione 1 (sui 2 bit alti), pronta in 7,5 ms, 3 byte
  if (!scriviRegistro(indirizzo, 0xF4, 0x34 | (1 << 6))) return false;
  delay(8);
  if (!leggiRegistri(indirizzo, 0xF6, d, 3)) return false;
  grezzo = ((d[0] << 16) | (d[1] << 8) | d[2]) >> (8 - 1);
  pascal = Ricetta::bmp180Pressione(grezzo, 1);
  return !isnan(pascal);
}

// BMP280: misura da solo, di continuo. Basta leggere 6 byte dal cassetto 0xF7:
// 3 di pressione e 3 di temperatura.
static bool misuraBmp280(int32_t &grezzo, float &pascal, float &gradi) {
  uint8_t d[6];
  if (!leggiRegistri(indirizzo, 0xF7, d, 6)) return false;
  grezzo = (d[0] << 12) | (d[1] << 4) | (d[2] >> 4);
  gradi = Ricetta::bmp280Temperatura((d[3] << 12) | (d[4] << 4) | (d[5] >> 4));
  pascal = Ricetta::bmp280Pressione(grezzo);
  return !isnan(pascal);
}

static bool misura(int32_t &grezzo, float &pascal, float &gradi) {
  return eBmp180 ? misuraBmp180(grezzo, pascal, gradi) : misuraBmp280(grezzo, pascal, gradi);
}

bool barometroInizio() {
  titolo("2 · BAROMETRO: PRESSIONE E QUOTA");
  if (presente(0x77)) {
    indirizzo = 0x77;
  } else if (presente(0x76)) {
    indirizzo = 0x76;
  } else {
    Serial.println("Nessun barometro a 0x77 né a 0x76: controlla i fili (prova la demo 1).");
    return false;
  }

  // «Chi sei?»: nel cassetto 0xD0 c'è il numero del modello
  uint8_t chi = 0;
  leggiRegistri(indirizzo, 0xD0, &chi, 1);
  if (chi == 0x55) {
    eBmp180 = true;
  } else if (chi == 0x58 || chi == 0x60) {
    eBmp180 = false;
  } else {
    Serial.printf("A 0x%02X risponde un chip con numero 0x%02X: non è un BMP180 né un BMP280.\n", indirizzo, chi);
    return false;
  }
  Serial.printf("Indirizzo 0x%02X. Cassetto 0xD0 («chi sei?») = 0x%02X → è un %s\n", indirizzo, chi,
                eBmp180 ? "BMP180" : "BMP280");

  // I numeri di taratura scritti in fabbrica: ogni chip ha i suoi
  Serial.println("Numeri di taratura scritti in fabbrica:");
  bool ok;
  if (eBmp180) {
    uint8_t c[22];
    ok = leggiRegistri(indirizzo, 0xAA, c, 22) && Ricetta::bmp180Taratura(c);
  } else {
    uint8_t c[24];
    ok = leggiRegistri(indirizzo, 0x88, c, 24) && Ricetta::bmp280Taratura(c);
    // Misura continua: temperatura ×1, pressione ×4, nessun filtro interno (il rumore lo vogliamo vedere)
    ok = ok && scriviRegistro(indirizzo, 0xF5, 0x00) && scriviRegistro(indirizzo, 0xF4, (1 << 5) | (3 << 2) | 3);
    delay(50);
  }
  if (!ok) {
    Serial.println("Taratura illeggibile: controlla i fili.");
    return false;
  }

  // La quota zero: la pressione media di adesso, a scheda ferma
  Serial.println("Tieni ferma la scheda: prendo la quota zero...");
  double somma = 0;
  int n = 0;
  float gradi = NAN;
  for (int i = 0; i < 20; i++) {
    int32_t grezzo;
    float pascal;
    if (misura(grezzo, pascal, gradi)) {
      somma += pascal;
      n++;
    }
    delay(20);
  }
  if (n == 0) {
    Serial.println("Nessuna misura riuscita: controlla i fili.");
    return false;
  }
  p0 = somma / n;
  quante = prossima = 0;
  filtrata = NAN;
  Serial.printf("Quota zero a %.0f Pa (%.1f hPa). Temperatura %.1f °C: serve alla ricetta per correggere.\n",
                p0, p0 / 100, gradi);
  Serial.println("Ora alza e abbassa la scheda!  grezzo = il numero del sensore, Pa = dopo la ricetta Bosch");
  Serial.println("Prova anche ad avvicinare un dito caldo al sensore, senza toccarlo: guarda la temperatura.");
  return true;
}

void barometroRipeti() {
  int32_t grezzo;
  float pascal, gradi;
  if (!misura(grezzo, pascal, gradi)) {
    Serial.println("Lettura fallita");
    aspetta(500);
    return;
  }

  // Filtro 1, la mediana: delle ultime 5 misure tiene quella di mezzo, così le letture pazze spariscono
  ultime[prossima] = pascal;
  prossima = (prossima + 1) % 5;
  if (quante < 5) quante++;
  float ordinate[5];
  std::copy(ultime, ultime + quante, ordinate);
  std::sort(ordinate, ordinate + quante);
  const float mediana = ordinate[quante / 2];

  // Filtro 2, la media che insegue: ogni misura nuova sposta il risultato solo di un quarto
  filtrata = isnan(filtrata) ? mediana : filtrata + (mediana - filtrata) * 0.25f;

  if (!modoGrafico) Serial.printf("grezzo %7ld → ", (long)grezzo);
  campo("pressione", pascal, "Pa", 0);
  campo("temperatura", gradi, "°C", 1);
  campo("quota", Ricetta::quota(pascal, p0), "m");
  campo("filtrata", Ricetta::quota(filtrata, p0), "m");
  barretta(Ricetta::quota(filtrata, p0), -1, 2);
  aCapo();
  aspetta(80);
}
