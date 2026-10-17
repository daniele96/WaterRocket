// Demo 0 · Decollo: il cuore del firmware di volo, in piccolo.
// Una «macchina a stati»: il razzo è sempre in uno di tre stati e cambia solo
// quando una regola diventa vera.

#include "aiuto.h"
#include "demo.h"
#include "mpu6050.h"
#include "pin.h"

enum Stato { A_TERRA, IN_VOLO, ATTERRATO };

// Sul razzo vero: più di 2,5 g per 4 letture di fila. A mano non ci si arriva:
// qui bastano 1,5 g per 2 letture (un'alzata decisa della scheda)
static constexpr float SOGLIA_G = 1.5f;
static constexpr int LETTURE_DI_FILA = 2;
static constexpr uint32_t FERMO_PER_MS = 3000;   // fermo a 1 g per 3 secondi = atterrato

static Stato stato;
static int sopraSoglia;     // quante letture di fila oltre la soglia
static uint32_t partenza;   // quando è decollato
static uint32_t fermoDa;    // da quando è fermo (0 = si sta muovendo)
static uint32_t atterrato;  // quando è atterrato
static float picco;         // l'accelerazione più forte del «volo»
static int giri;

static void colore(uint8_t rosso, uint8_t verde, uint8_t blu) {
  anello.fill(anello.Color(rosso, verde, blu));
  anello.show();
}

static void vaiATerra() {
  stato = A_TERRA;
  sopraSoglia = 0;
  digitalWrite(PIN_CICALINO, LOW);
  colore(0, 20, 0);   // verde: pronto
}

bool decolloInizio() {
  titolo("0 · DECOLLO: LA MACCHINA A STATI DEL RAZZO");
  if (!mpuSveglia(2)) return false;   // ±8 g: una scossa forte non va fuori scala
  if (!mpuTara()) return false;       // da fermo deve valere 1 g, se no «fermo» non si riconosce
  preparaCicalino();
  Serial.println("A TERRA → (più di 1,5 g per 2 letture) → IN VOLO → (fermo a 1 g per 3 s) → ATTERRATO");
  Serial.println("Sul razzo vero la soglia è 2,5 g per 4 letture.");
  Serial.println("Alza la scheda di scatto, muovila come un razzo, poi appoggiala e non toccarla.");
  vaiATerra();
  return true;
}

void decolloRipeti() {
  int16_t grezzi[7];
  if (!mpuLeggi(grezzi)) {
    aspetta(100);
    return;
  }
  const float x = grezzi[0] / mpuPuntiPerG();
  const float y = grezzi[1] / mpuPuntiPerG();
  const float z = grezzi[2] / mpuPuntiPerG();
  const float g = sqrtf(x * x + y * y + z * z);
  const uint32_t adesso = millis();

  switch (stato) {
    case A_TERRA:
      // Una lettura alta da sola può essere un urto: ne servono alcune di fila
      sopraSoglia = g > SOGLIA_G ? sopraSoglia + 1 : 0;
      if (sopraSoglia >= LETTURE_DI_FILA) {
        stato = IN_VOLO;
        partenza = adesso;
        fermoDa = 0;
        picco = g;
        colore(60, 60, 60);   // bianco: in volo
        Serial.printf(">> DECOLLO! %.1f g\n", g);
      }
      break;

    case IN_VOLO: {
      picco = max(picco, g);
      // Fermo = vicino a 1 g. Se si muove, il conto dei 3 secondi riparte da capo
      const bool fermo = fabsf(g - 1.0f) < 0.1f;
      if (!fermo) {
        fermoDa = 0;
      } else if (fermoDa == 0) {
        fermoDa = adesso;
      } else if (adesso - fermoDa >= FERMO_PER_MS) {
        stato = ATTERRATO;
        atterrato = adesso;
        Serial.printf(">> ATTERRATO dopo %.1f s di volo, picco %.1f g. Bip, bip!\n",
                      (fermoDa - partenza) / 1000.0f, picco);
      }
      break;
    }

    case ATTERRATO: {
      // Lampo e bip insieme, 200 ms ogni secondo: come al ritrovamento del razzo
      const bool acceso = (adesso - atterrato) % 1000 < 200;
      digitalWrite(PIN_CICALINO, acceso ? HIGH : LOW);
      colore(acceso ? 60 : 0, acceso ? 60 : 0, acceso ? 60 : 0);
      if (adesso - atterrato > 6000) {
        vaiATerra();
        Serial.println(">> Riarmato: di nuovo A TERRA");
      }
      break;
    }
  }

  // 100 letture al secondo come sul razzo; la riga di stato 4 volte al secondo
  if (modoGrafico) {
    campo("accelerazione", g, "g");
    campo("stato", stato, "", 0);
  } else if (++giri % 25 == 0) {
    static const char *NOMI[] = {"A TERRA  ", "IN VOLO  ", "ATTERRATO"};
    Serial.printf("%s  ", NOMI[stato]);
    campo("accelerazione", g, "g");
    barretta(g, 0, 4);
    aCapo();
  }
  aspetta(10);
}
