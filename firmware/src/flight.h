#pragma once
#include <Arduino.h>

// Accelerometro/giroscopio MPU6050 (GY-521) e riconoscimento delle fasi del volo.
// Un task legge il sensore a 100 Hz e fa avanzare la macchina a stati:
//   A_TERRA --(spinta > LAUNCH_G)--> IN_VOLO --(fermo per LANDED_STILL_MS)--> ATTERRATO
// "Fermo" = il vettore accelerazione non cambia più di LANDED_DELTA_G e vale ~1 g
// (in caduta libera vale ~0 g ed è anche lui costante: non va scambiato per un atterraggio).
// Durante la registrazione i campioni finiscono anche in <dir>/sensors.csv.
namespace Flight {

enum class State { Ground, Flying, Landed };

// Valori già calibrati con lo zero preso sulla rampa (vedi Calibration)
struct Sample {
  float ax, ay, az;  // g
  float gx, gy, gz;  // gradi/s
  float net;         // |a - gravità a riposo| in g: 0 sulla rampa, in volo spinta/frenata
  float tempC;
  uint32_t ms;       // millis() della lettura
};

// Zero preso in automatico sulla rampa: media di 1.5 s di immobilità mentre è A TERRA.
// Corregge la scala dell'accelerometro (a riposo |a| = 1 g), lo scarto del giroscopio
// (a riposo 0 °/s) e memorizza la direzione della gravità per l'accelerazione netta.
// L'accelerazione netta vale rispetto all'orientamento sulla rampa.
struct Calibration {
  float scale = 1.0f;          // fattore per l'accelerometro
  float gx = 0, gy = 0, gz = 0;  // scarto del giroscopio (°/s)
  float rx = 0, ry = 0, rz = 0;  // direzione della gravità a riposo (vettore unitario)
  float rawG = 0;              // |a| grezzo a riposo (0 = zero non ancora preso)
  float tempC = 0;
  uint32_t ms = 0;             // millis() della presa dello zero
};

// Inizializza I2C e sensore e avvia il task di lettura. false se l'MPU6050 non risponde.
bool begin();
bool isReady();
uint8_t address();

Sample latest();
float magnitude(const Sample &s);  // |a| in g
float peakG();                     // accelerazione massima dall'ultimo decollo

State state();
const char *stateName(State s);
uint32_t launchMs();   // millis() del decollo (0 se mai partito)
uint32_t landedMs();   // millis() dell'atterraggio (0 se non ancora atterrato)

Calibration calibration();

// Per le prove a terra: simula decollo e atterraggio / torna ad A_TERRA (riarma).
void forceLaunch();
void forceLand();
void reset();

// Modalità prova: soglia di decollo bassa (2 g per 10 ms), basta un colpo della mano.
// Spenta all'avvio: sul razzo vero un urto sulla rampa la farebbe scattare.
void setTestMode(bool on);
bool testMode();

// Registra ogni campione in <dir>/sensors.csv (tempo in ms da questa chiamata,
// stesso riferimento di frames.csv). stopLog() è bloccante finché il file è chiuso.
bool startLog(const String &dir);
void stopLog();
bool isLogging();
uint32_t loggedSamples();
uint32_t logDropped();  // campioni persi perché la SD non stava al passo

}  // namespace Flight
