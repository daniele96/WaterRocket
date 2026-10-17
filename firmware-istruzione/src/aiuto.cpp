#include "aiuto.h"

#include <Wire.h>
#include <driver/gpio.h>

#include "pin.h"

bool modoGrafico = false;
Adafruit_NeoPixel anello(NUM_LED, PIN_ANELLO, NEO_GRB + NEO_KHZ800);

bool aspetta(uint32_t ms) {
  const uint32_t inizio = millis();
  do {
    // Gli «a capo» che alcuni terminali mandano dopo ogni tasto non contano
    while (Serial.available() && (Serial.peek() == '\r' || Serial.peek() == '\n')) Serial.read();
    if (Serial.available()) return true;
    delay(1);
  } while (millis() - inizio < ms);
  return false;
}

void titolo(const char *nome) {
  Serial.println();
  Serial.println("==================================================");
  Serial.printf("  %s\n", nome);
  Serial.println("==================================================");
  Serial.println("(m = torna al menu)");
}

void campo(const char *nome, float valore, const char *unita, int decimali) {
  if (modoGrafico) {
    Serial.print('>');
    for (const char *c = nome; *c; c++) Serial.print(*c == ' ' ? '_' : *c);
    Serial.printf(":%.*f\n", decimali, valore);
  } else {
    Serial.printf("%s %*.*f %s   ", nome, decimali + 5, decimali, valore, unita);
  }
}

void barretta(float valore, float minimo, float massimo) {
  if (modoGrafico) return;
  constexpr int L = 24;
  auto cella = [&](float v) { return constrain((int)lroundf((v - minimo) / (massimo - minimo) * L), 0, L); };
  const bool conZero = minimo < 0 && massimo > 0;   // con lo zero nel mezzo si riempie dallo zero
  const int p = cella(valore), z = conZero ? cella(0) : 0;
  char riga[L + 3];
  riga[0] = '[';
  for (int i = 0; i < L; i++) riga[i + 1] = (i >= min(p, z) && i < max(p, z)) ? '#' : '-';
  if (conZero) riga[min(z, L - 1) + 1] = '|';
  riga[L + 1] = ']';
  riga[L + 2] = 0;
  Serial.print(riga);
}

void aCapo() {
  if (!modoGrafico) Serial.println();
}

// Un reset a metà di una lettura può lasciare un sensore a metà byte, con SDA tenuto basso.
// 9 colpi di orologio gli fanno finire il byte, poi uno STOP libera il bus.
static void liberaBus() {
  pinMode(PIN_SDA, INPUT_PULLUP);
  pinMode(PIN_SCL, OUTPUT_OPEN_DRAIN);
  digitalWrite(PIN_SCL, HIGH);
  for (int i = 0; i < 9; i++) {
    delayMicroseconds(5);
    digitalWrite(PIN_SCL, LOW);
    delayMicroseconds(5);
    digitalWrite(PIN_SCL, HIGH);
  }
  pinMode(PIN_SDA, OUTPUT_OPEN_DRAIN);
  digitalWrite(PIN_SDA, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_SDA, HIGH);   // STOP: SDA sale mentre SCL è alto
  delayMicroseconds(5);
  pinMode(PIN_SDA, INPUT);
  pinMode(PIN_SCL, INPUT);
}

void avviaBus() {
  liberaBus();
  Wire.begin(PIN_SDA, PIN_SCL, 100000);   // 100 000 colpi di orologio al secondo
}

bool presente(uint8_t indirizzo) {
  Wire.beginTransmission(indirizzo);
  return Wire.endTransmission() == 0;   // 0 = qualcuno ha risposto «presente!» (ACK)
}

bool scriviRegistro(uint8_t indirizzo, uint8_t registro, uint8_t valore) {
  Wire.beginTransmission(indirizzo);   // «indirizzo, ascoltami»
  Wire.write(registro);                // «nel cassetto registro...»
  Wire.write(valore);                  // «...metti questo numero»
  return Wire.endTransmission() == 0;
}

bool leggiRegistri(uint8_t indirizzo, uint8_t registro, uint8_t *dati, size_t quanti) {
  Wire.beginTransmission(indirizzo);   // «indirizzo, voglio leggere...»
  Wire.write(registro);                // «...a partire dal cassetto registro»
  if (Wire.endTransmission(false) != 0) return false;   // false: il bus resta nostro, ora si legge
  if (Wire.requestFrom((uint16_t)indirizzo, quanti) != quanti) return false;   // «dammi quanti byte»
  for (size_t i = 0; i < quanti; i++) dati[i] = Wire.read();
  return true;
}

void preparaCicalino() {
  pinMode(PIN_CICALINO, OUTPUT);
  digitalWrite(PIN_CICALINO, LOW);
  // Il cicalino chiede ~25-30 mA: il pin deve dare il massimo (40 mA)
  gpio_set_drive_capability((gpio_num_t)PIN_CICALINO, GPIO_DRIVE_CAP_3);
}

void spegniTutto() {
  digitalWrite(PIN_CICALINO, LOW);
  anello.clear();
  anello.show();
}
