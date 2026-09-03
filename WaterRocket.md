# Progetto Water Rocket 🚀💧

Documento di progettazione per un razzo ad acqua (water rocket): principi fisici,
componenti, materiali, calcoli, costruzione, lancio e sicurezza.

---

## 1. Come funziona

Un water rocket sfrutta il **principio di azione-reazione** (3ª legge di Newton).
La bottiglia viene riempita parzialmente d'acqua e pressurizzata con aria.
All'apertura del tappo, l'aria compressa spinge fuori l'acqua ad alta velocità:
l'espulsione della massa d'acqua genera una spinta che accelera il razzo verso l'alto.

Equazione della spinta (semplificata):

```
F = ṁ · v_e        (massa espulsa al secondo × velocità di espulsione)
v_e = sqrt(2 · ΔP / ρ_acqua)
```

dove:
- `ΔP` = differenza di pressione tra interno bottiglia e atmosfera [Pa]
- `ρ_acqua` ≈ 1000 kg/m³

---

## 2. Componenti principali

| Componente        | Funzione                                          |
|-------------------|---------------------------------------------------|
| **Corpo (bottiglia)** | Serbatoio pressione + acqua. PET da bibita gassata |
| **Ugello (nozzle)**   | Apertura del tappo da cui esce l'acqua            |
| **Ogiva (nose cone)** | Punta aerodinamica, riduce la resistenza          |
| **Alette (fins)**     | Stabilità aerodinamica in volo                    |
| **Zavorra**           | Massa in punta per spostare il baricentro avanti  |
| **Sistema di lancio** | Tappo di tenuta + meccanismo di rilascio          |
| **Recupero (opzionale)** | Paracadute per atterraggio dolce               |

---

## 3. Materiali (lista della spesa)

- 1–2 **bottiglie PET** da 1,5 L o 2 L (per bibite gassate — resistono alla pressione)
- **Cartoncino / polipropilene / Depron** per le alette
- **Pallina da tennis** o cono in cartoncino per l'ogiva
- **Nastro adesivo resistente** (telato/Tesa) e **colla a caldo**
- **Plastilina o sabbia** per la zavorra
- **Tappo forato + valvola** (es. tappo gonfiabile / adattatore per pompa)
- **Pompa da bicicletta** con manometro
- **O-ring / guarnizione** per la tenuta
- Fascette, tubo in PVC per la rampa di lancio

---

## 4. Parametri di progetto consigliati

| Parametro                  | Valore tipico / di partenza        |
|----------------------------|------------------------------------|
| Volume bottiglia           | 1,5 – 2 L                          |
| Riempimento d'acqua        | **~1/3 del volume (25–40%)**       |
| Pressione di lancio        | 4 – 6 bar (max ~8 bar con PET)     |
| N° alette                  | 3 o 4, distribuite simmetricamente |
| Posizione baricentro (CG)  | **davanti** al centro di pressione (CP) |
| Margine di stabilità       | CG-CP ≈ 1–2 calibri (diametri)     |

> ⚠️ **Regola d'oro stabilità:** il baricentro (CG) deve stare *davanti* (verso
> l'ogiva) rispetto al centro di pressione aerodinamica (CP). Per questo si aggiunge
> zavorra in punta. Verifica con la "prova della cordicella": appendi il razzo
> orizzontalmente al CG e fallo ruotare — deve volare con la punta in avanti.

---

## 5. Calcoli rapidi

**Velocità di espulsione acqua** (a 6 bar = 600 000 Pa):
```
v_e = sqrt(2 · 600000 / 1000) ≈ 34,6 m/s
```

**Stima frazione d'acqua ottimale:** il massimo della quota si ottiene quando c'è
abbastanza acqua da spingere a lungo, ma non così tanta da appesantire il razzo.
In pratica **~30% del volume** è un ottimo punto di partenza; ottimizza con i test.

**Quota:** dipende da pressione, massa a vuoto, aerodinamica e attrito.
Per simulazioni più precise puoi usare un foglio di calcolo o software dedicati
(vedi sezione 8).

---

## 6. Costruzione (passo-passo)

1. **Corpo:** scegli una bottiglia integra, senza ammaccature o graffi profondi.
2. **Alette:** ritaglia 3–4 alette identiche; incollale a 120° (o 90°) vicino
   all'ugello, ben allineate all'asse. Allineamento preciso = volo dritto.
3. **Ogiva:** fissa il cono/pallina in punta; inserisci la zavorra all'interno.
4. **Tenuta:** monta il tappo con valvola e o-ring; verifica che tenga la pressione.
5. **Bilanciamento:** trova il CG e controlla il margine di stabilità (sez. 4).
6. **Recupero (opz.):** aggiungi un paracadute con meccanismo a tempo o aerodinamico.

---

## 7. Lancio e procedura di test

1. Posiziona la rampa su terreno aperto, lontano da persone, edifici, linee elettriche.
2. Riempi d'acqua (~1/3) e fissa il razzo al sistema di lancio.
3. Pressurizza **gradualmente** controllando il manometro.
4. **Allontana tutti** ad almeno 5–10 m. Conto alla rovescia.
5. Rilascia da distanza di sicurezza (cordino/leva).
6. **Registra i dati** ad ogni lancio per ottimizzare:

| Lancio | Acqua (%) | Pressione (bar) | Massa (g) | Quota stimata (m) | Note |
|--------|-----------|------------------|-----------|-------------------|------|
| 1      |           |                  |           |                   |      |
| 2      |           |                  |           |                   |      |
| 3      |           |                  |           |                   |      |

---

## 8. Strumenti utili

- **Simulatori:** *Water Rocket Fun*, *Clive's Water Rocket Simulator*, fogli Excel dedicati.
- **Misura quota:** app inclinometro + triangolazione, o altimetro a bordo (es. modulo BMP280).
- **Slow motion** dello smartphone per analizzare il distacco e la traiettoria.

---

## 9. ⚠️ Sicurezza (leggere SEMPRE)

- L'aria compressa è **pericolosa**: una bottiglia che cede a 6 bar può ferire.
- Usa **solo** bottiglie PET per bibite gassate, mai bottiglie danneggiate o riutilizzate troppe volte.
- **Non superare** le pressioni consigliate; non sostare mai sopra/davanti al razzo carico.
- Indossa **occhiali protettivi** durante la pressurizzazione.
- Lancia all'aperto, lontano da persone, animali e ostacoli. Mai al chiuso.
- Attività adatta a ragazzi **solo sotto supervisione adulta**.

---

## 10. Idee di miglioramento

- Razzo **bi-stadio** (due bottiglie in sequenza).
- **Tromboncino/booster** con bottiglie unite per più volume.
- Sistema di **recupero a paracadute** automatico.
- **Telemetria** a bordo (altimetro, accelerometro, GPS).
- Ottimizzazione aerodinamica dell'ogiva e delle alette.

---

## 11. Link di documentazione e risorse

> ⚠️ Gli URL possono cambiare nel tempo: se un link è morto, cerca il nome del sito/progetto.

### Guide, teoria e community
- **NASA – Water Rocket / Bottle Rocket** (principi fisici e attività didattiche):
  <https://www.grc.nasa.gov/www/k-12/rocket/BottleRocket/about.htm>
- **Air Command Water Rockets** (uno dei riferimenti storici più completi,
  con build avanzate, multistadio, recupero): <https://www.aircommandrockets.com/>
  - Quanta acqua mettere: <http://www.aircommandrockets.com/water.htm>
  - Costruire un lanciatore: <http://www.aircommandrockets.com/rocket_launcher.htm>
  - Costruzione del razzo: <http://www.aircommandrockets.com/construction.htm>
  - Meccanismo di recupero paracadute (DetMech): <http://www.aircommandrockets.com/howitworks_3.htm>
  - Galleria progetti: <http://www.aircommandrockets.com/rocket_gallery.htm>
  > Nota: il sito funziona anche su `http://` se l'`https://` dà problemi.
- **NPL / The Water Rocket Achievement World Record Association (WRA2)**
  (record, regole, tecnica): <https://www.wra2.org/>
- **r/WaterRockets** (community Reddit con build e consigli):
  <https://www.reddit.com/r/WaterRockets/>
- **Wikipedia – Water rocket** (panoramica generale e riferimenti):
  <https://en.wikipedia.org/wiki/Water_rocket>

### Simulatori e calcolo
- **Clive's Water Rocket Simulator (Excel)** — molto usato per stimare quota e profilo di volo.
- **OpenRocket** (più orientato ai razzi a propellente solido, ma utile per concetti di
  stabilità/CG-CP): <https://openrocket.info/>
- **Water Rocket Fun** — app/simulatore per il calcolo del riempimento ottimale.

### Video tutorial (cerca su YouTube)
- "Water rocket build" / "water rocket launcher Gardena" (lanciatore con attacco rapido da giardino)
- "Water rocket parachute deployment" per i sistemi di recupero
- Canale **Air Command Rockets** su YouTube

---

## 12. Stampa 3D 🖨️

La stampa 3D è perfetta per le parti **non in pressione**: ogiva, alette, anelli
fermaaletta, adattatori per la rampa e meccanismi di recupero. **La camera in pressione
deve restare la bottiglia PET** (il PLA/PETG stampato non è affidabile per contenere
4–8 bar).

### Cosa conviene stampare
| Parte                        | Materiale consigliato | Note                                  |
|------------------------------|-----------------------|---------------------------------------|
| Ogiva / nose cone            | PLA o PETG            | Leggera; eventualmente cava per zavorra |
| Anello porta-alette (fin can)| PETG / ABS / ASA      | Si infila sul collo o sul corpo       |
| Alette                       | PETG / PLA            | Anche stampabili a parte e incastrate |
| Adattatore ugello / launcher | PETG / ABS            | Per accoppiare la rampa al collo bottiglia |
| Vano paracadute / nose lock  | PLA / PETG            | Per il sistema di recupero            |

> Per parti soggette a sole e calore preferisci **PETG/ASA** (il PLA si deforma al caldo
> e diventa fragile dopo gli urti).

### Dove trovare i modelli (STL già pronti)
- **Printables** (Prusa): <https://www.printables.com/> → cerca *"water rocket"*, *"water rocket fins"*, *"nose cone"*
- **Thingiverse**: <https://www.thingiverse.com/search?q=water+rocket> → moltissimi fin can e ogive
- **Thangs**: <https://thangs.com/> → motore di ricerca multi-piattaforma di modelli 3D
- **MakerWorld** (Bambu Lab): <https://makerworld.com/> → cerca *"water rocket"*
- **Cults3D**: <https://cults3d.com/> → modelli gratuiti e a pagamento
- **GrabCAD** (modelli CAD parametrici, utili da modificare):
  <https://grabcad.com/library> → cerca *"water rocket"*

### Termini di ricerca utili
`water rocket fins`, `water rocket nose cone`, `bottle rocket fin can`,
`water rocket launcher adapter`, `Gardena water rocket launcher`,
`PET bottle rocket parachute`, `water rocket coupling 2 liter`.

### Suggerimenti di stampa
- Stampa le alette **piatte sul piatto** per la massima resistenza lungo la corda.
- Per il fin can usa **alta densità di riempimento** vicino agli attacchi (o pareti spesse).
- Modella l'**interno dell'ogiva cavo** per inserire la zavorra (plastilina/sabbia) e
  regolare il baricentro.
- Verifica il **diametro del collo bottiglia standard PCO-1810/PCO-1881** quando progetti
  adattatori parametrici.

---

## 13. Elettronica di bordo 🔌

Sistema di telemetria + log dati + video da montare nell'**ogiva** del razzo
(scende col paracadute passivo del kit — vedi §3 e nota sotto).

### Architettura

- **MCU di volo**: ESP32-S3 con camera, in ogiva
- **Sensori I2C** sullo stesso bus: BMP280 + MPU6050
- **Storage**: microSD per log ad alta frequenza + video
- **Recupero**: passivo (paracadute del kit) + buzzer ritrovamento
- **Telemetria**: WiFi/BLE live a terra (range utile ~50 m)

### Board scelta: Seeed XIAO ESP32-S3 Sense

| Caratteristica | Valore |
|---|---|
| Dimensioni / peso | 21×17,5 mm, ~3 g |
| Camera | OV2640 2 MP (staccabile, opz. OV5640; i cloni AliExpress montano spesso **OV3660 3 MP**) |
| Microfono | PDM digitale |
| microSD slot | Sì, sulla shield camera |
| PSRAM / Flash | 8 MB / 8 MB |
| Carica LiPo | Sì, pad **BAT+/BAT-** da saldare |
| Wireless | WiFi 2,4 GHz + BLE 5.0, antenna ceramica + IPEX |
| Riferimento firmware | usata dal progetto `tritonFC` |

### Sensori e funzionalità sbloccate

| Sensore | Bus | Cosa abilita | Priorità |
|---|---|---|---|
| **BMP280** | I2C | Quota live, apogeo, profilo di volo, T/P cabina | ⭐⭐⭐ |
| **MPU6050** | I2C | Accelerazione lancio, giroscopio (roll/yaw/pitch), rilevazione apogeo via segno accel | ⭐⭐⭐ |
| **microSD** (slot integrato) | SPI | Log ≥100 Hz + video AVI | ⭐⭐⭐ |
| **Camera OV2640** | DVP | Video 640×480 a 15–25 fps, foto, analisi traiettoria | ⭐⭐ |
| **Buzzer attivo** | GPIO on/off | Beep ritrovamento (vedi *Buzzer* sotto) | ⭐⭐ |
| **LED ultra-bright** | GPIO | Visibilità slow-mo, stato armato/disarmato | ⭐ |
| **Microfono PDM** | I2S | Acustica del getto d'acqua, evento apertura paracadute | ⭐ |
| **RTC DS3231** (lab) | I2C | Timestamp assoluti, sincronizzazione multi-razzo | ⭐ |
| **BNO085** IMU 9-DOF | I2C/UART | Assetto assoluto già fuso — ma satura a ±8 g: vedi *BNO085* | ❌ sconsigliato |
| **BH1750 / TSL2591** (lab) | I2C | Profilo luminosità lungo la traiettoria (didattico) | ⭐ |
| **GPS NEO-M8N** | UART | Ritrovamento a distanza (≥70 m). Costa ~20 g | ⭐ opz |
| **Accelerometro high-g** (H3LIS331DL / ADXL375) | I2C | Vero picco accel al lancio: l'MPU6050 satura a ±16 g e il razzo ne fa molti di più — vedi *Accelerometro high-g* | ⭐ opz |

### Pin disponibili e assegnazione

La XIAO espone **11 pad utente** (D0–D10). Camera e microfono PDM della shield Sense
girano su GPIO interni al connettore board-to-board e **non consumano pad**. La
microSD invece sì.

#### Camera + microSD in contemporanea

**Si può fare** — è esattamente ciò che fa `tritonFC` (video AVI + log sulla stessa
board). Costa però due cose:

1. **D8/D9/D10 (GPIO7/8/9) diventano il bus SPI della microSD.** Da 11 pad liberi
   si scende a 8.
2. **Si perde il LED onboard.** Il CS della microSD è su **GPIO21**, che è lo stesso
   pin del LED utente della XIAO: conflitto hardware noto e documentato. Se si vuole
   un indicatore armato/disarmato va messo un **LED esterno** (riga 5 della BOM);
   altrimenti si rinuncia al LED e non si perde nulla di funzionale.

⚠️ **Trappola documentale**: lo **schematico** Seeed indica SD_CS su **GPIO3**, mentre
il **tutorial e il codice di esempio** usano **GPIO21**. Vale il secondo. Per non
finire in mezzo alla discrepanza, **lasciare D2/GPIO3 libero** — è anche un pin di
strapping (selezione JTAG), quindi comunque da evitare.

#### Mappa pin con camera + SD attive

| Pad | GPIO | Stato | Assegnazione |
|---|---|---|---|
| D1 | 2 | **usato** | **Buzzer attivo 12095** |
| D4 | 5 | **usato** | **I2C SDA** → BMP280 + MPU6050 |
| D5 | 6 | **usato** | **I2C SCL** → BMP280 + MPU6050 |
| D3 | 4 | libero | LED di stato esterno *(opzionale)* |
| D0 | 1 | libero (ADC1) | Partitore tensione batteria *(opzionale)* |
| D6 | 43 | libero (UART0 TX) | — *(GPS NEO-M8N se mai servisse)* |
| D7 | 44 | libero (UART0 RX) | — *(idem)* |
| D2 | 3 | ⚠️ strapping + ambiguità SD_CS | **non usare** |
| D8 | 7 | 🔒 SPI SCK | microSD |
| D9 | 8 | 🔒 SPI MISO | microSD |
| D10 | 9 | 🔒 SPI MOSI | microSD |
| — | 21 | 🔒 SD_CS (era il LED) | microSD |

**Bilancio: il progetto usa 3 pad su 8 disponibili.** Restano liberi D0, D3, D6, D7
(più D2 da non toccare). Margine ampio: la configurazione non è nemmeno stretta.

- **Niente UART** nella configurazione scelta: D6/D7 restano liberi. Se un giorno
  servisse il GPS per il ritrovamento a distanza, i pad ci sono già.
- **Partitore batteria: opzionale, non previsto.** Se lo si aggiunge deve stare su un
  pin ADC1 (GPIO1–10) perché l'**ADC2 è inutilizzabile con il WiFi attivo** — D0/GPIO1
  è la scelta giusta. Senza partitore non c'è preavviso di batteria scarica: la board
  si spegne e basta. Non è un rischio concreto (400 mAh = 1,5–2,5 h contro pochi
  minuti di sessione), e la cella è comunque protetta dal suo PCM contro la scarica
  profonda. Basta partire carichi.
- **LED di stato: opzionale.** Serve solo perché quello onboard è occupato da SD_CS;
  se non serve un indicatore armato/disarmato visivo, si può saltare.

#### WiFi + BLE accesi insieme

Si può, l'ESP32-S3 supporta la coesistenza — ma la radio 2,4 GHz è **una sola** e
viene divisa a slot temporali fra i due stack. Conseguenze da mettere in conto:

- **Throughput WiFi ridotto** e latenza più irregolare quando il BLE è attivo. Se si
  streamma video via WiFi mentre il BLE trasmette telemetria, il video ne risente.
- **Consumo più alto**: entrambi gli stack accesi portano la media verso i 200 mA e
  oltre. Sui 400 mAh restano comunque 1,5–2 h, abbondanti.
- **ADC2 inutilizzabile** finché il WiFi è acceso (vale già senza BLE).

💡 **Divisione dei compiti consigliata**, invece di far lavorare entrambi a pieno
regime insieme: **BLE** per la telemetria a bassa frequenza (quota e accelerazione in
tempo reale sul telefono, più beacon di ritrovamento post-atterraggio — l'RSSI del BLE
è un ottimo radiogoniometro che completa il buzzer, e consuma pochissimo); **WiFi**
per il travaso pesante a razzo recuperato (scarico di video e log, la XIAO in AP mode).

#### Sì, l'I2C c'è ed è libero

**D4 = SDA (GPIO5), D5 = SCL (GPIO6)**, i pin I2C di default. BMP280 e MPU6050 stanno
sullo stesso bus senza conflitti, avendo indirizzi diversi. La camera usa un suo bus
SCCB separato (GPIO39/40): **non interferisce**.

⚠️ **Conflitto di indirizzo da conoscere**: **DS3231 e MPU6050 sono entrambi 0x68.**
Se in un secondo momento si aggiunge l'RTC al bus, spostare l'MPU6050 a **0x69**
portando il suo pin **AD0 a 3V3** (sui breakout GY-521 è un pad dedicato).

Indirizzi dei sensori previsti: BMP280 `0x76/0x77` · MPU6050 `0x68` (o `0x69`) ·
DS3231 `0x68` · BH1750 `0x23/0x5C` · TSL2591 `0x29`.

I breakout GY-521 e BMP280 hanno già le **pull-up** a bordo: con 2–3 moduli in
parallelo il valore risultante resta accettabile a 3,3 V, non serve aggiungerne.

### Schema collegamenti

```
XIAO ESP32-S3 Sense
├── D4 / D5       → I2C: BMP280 (0x76) + MPU6050 (0x68)
├── D1            → buzzer attivo 12095 (diretto, 25 mA)
├── D8/D9/D10+G21 → 🔒 microSD (SPI, slot sulla shield Sense)
│
│   -- opzionali, pad già liberi --
├── D3            → LED di stato esterno (quello onboard è occupato da SD_CS)
├── D0            → partitore 2×100 kΩ da BAT+ (lettura tensione, ADC1)
├── D6 / D7       → UART GPS NEO-M8N
├── Camera OV3660 → FPC dedicato, GPIO interni (nessun pad occupato)
├── BAT+ / BAT-   → pigtail JST 1.25 saldato → LiPo 1S 400 mAh (602040)
└── USB-C         → programmazione + ricarica (50 mA)
```

### Accelerometro high-g: serve davvero?

**L'ADXL345 non vale la spesa.** Satura a **±16 g esatti come l'MPU6050**, quindi non
risolve il problema per cui lo si comprerebbe; e in più **non ha giroscopio**, per cui
rispetto all'MPU6050 è un passo indietro, non avanti. 5 pezzi di un componente
ridondante non aggiungono nulla al progetto.

#### Quanti g fa davvero un razzo ad acqua

Vale la pena fare il conto con i numeri del progetto (§4), perché il risultato è
sorprendente. Per un getto d'acqua incomprimibile la spinta è `F = 2 · A · ΔP`:

```
Ugello (collo bottiglia)  A = π · (0,011 m)²  = 3,8 · 10⁻⁴ m²
Pressione                 ΔP = 6 bar          = 6 · 10⁵ Pa
Spinta                    F  = 2 · 3,8e-4 · 6e5 ≈ 456 N   (!)
Massa al distacco         m  ≈ 0,75 kg  (bottiglia + 0,6 L acqua + elettronica)
Accelerazione di picco    a  = 456 / 0,75 ≈ 610 m/s² ≈ 62 g
```

**Circa 60 g nei primi millisecondi**, che decadono in fretta col calare della
pressione. Con meno acqua a bordo si sfiorano gli 85 g. Il punto è che **±16 g vengono
sfondati di quasi quattro volte**: sul log dell'MPU6050 la fase di spinta apparirà come
un plateau tosato a 16 g, e il valore vero resterà ignoto.

#### Quindi conviene o no?

Dipende da cosa si vuole. **Per la missione principale non serve**: il rilevamento
dell'apogeo usa il *segno* dell'accelerazione e il barometro, non il modulo, e ±16 g
sono più che sufficienti. Quota, apogeo, profilo di volo, assetto e video sono tutti
coperti da BMP280 + MPU6050.

**Serve se interessa il numero in sé** — che in chiave didattica è uno dei più belli da
mostrare ai ragazzi ("il razzo ha preso 60 g, un pilota di caccia ne regge 9"). In quel
caso i candidati sono:

| Modulo | Fondo scala | Dove | Prezzo |
|---|---|---|---|
| **[Adafruit H3LIS331 #4627](https://www.adafruit.com/product/4627)** | ±100 / **200** / 400 g *selezionabile* | [Robot Italy](https://robot-italy.com/) — disponibile, 4–6 gg | **36,53 €** |
| [Adafruit ADXL375 #5374](https://www.adafruit.com/product/5374) | ±200 g fisso | [Robot Italy](https://robot-italy.com/products/5374-adxl375-high-g-accelerometer-200g-with-i2c-and-spi) — disponibile | **36,53 €** |
| [SparkFun H3LIS331DL SEN-14480](https://www.sparkfun.com/sparkfun-triple-axis-accelerometer-breakout-h3lis331dl.html) | ±100/200/400 g | rivenditori USA/EU | ~25–30 € |
| ~~ADXL377~~ (±200 g analogico) | — | ❌ **EOL**, Adafruit rimanda all'ADXL375 | — |
| ~~ADXL345~~ | ~~±16 g~~ | ❌ inutile qui | — |

**Il breakout H3LIS331DL esiste** (Adafruit 4627, SparkFun SEN-14480): quello che non
esiste è il modulo GY cinese da 6 €. Il chip è di nicchia e non ha cloni AliExpress
diffusi come MPU6050 o BMP280 — da qui il salto di prezzo.

Nessuno dei due costa pin: vanno sull'**I2C già cablato** (D4/D5) e i loro indirizzi
(H3LIS331 `0x18/0x19`, ADXL375 `0x53/0x1D`) non collidono con BMP280 né MPU6050.

⚠️ **Se si trova un "ADXL375" economico su AliExpress, diffidare.** ADXL375 e ADXL345
sono quasi identici a livello di registri: un ADXL345 rietichettato funzionerebbe,
risponderebbe correttamente all'interrogazione software e **si scoprirebbe solo in volo**,
tosato a 16 g. Non è verificabile da codice — solo con una prova d'urto reale.

👉 **A parità di prezzo prenderei l'H3LIS331**: impostato a ±100 g dà il doppio della
risoluzione dell'ADXL375 proprio nella finestra 60–85 g che ci interessa. L'ADXL375 ha
in cambio un vantaggio software — essendo compatibile a registri con l'ADXL345, eredita
librerie ed esempi molto più diffusi.

⚠️ **Attenzione, però: il sensore da solo non basta.** Il picco dura **20–50 ms**. A
100 Hz di log lo si campiona 2–5 volte e il massimo vero si perde quasi sicuramente —
comprare un ±200 g e loggare a 100 Hz è buttare i soldi. Serve alzare l'ODR a
**500–1000 Hz** almeno durante la fase di spinta (entrambi i sensori ci arrivano),
bufferizzando in RAM e scaricando su SD dopo il burnout.

#### 🛑 Raccomandazione: rimandare

**Non comprarlo adesso.** A 36,53 € un sensore opzionale peserebbe oltre un terzo
dell'intera BOM da ~99 €, per aggiungere *un numero* a una missione che è già
completamente coperta.

L'ordine sensato è: **volare prima**. Con BMP280 + MPU6050 + video si scopre se il
numero serve davvero al laboratorio o se era curiosità. Se poi lo si vuole, il modulo
si aggiunge in qualsiasi momento senza toccare nulla — è I2C sul bus già cablato.

💡 **E intanto il picco si stima gratis.** Anche tosato a 16 g, il log dell'MPU6050 dice
per **quanto tempo** l'accelerazione è rimasta sopra soglia; incrociandolo con la
velocità di burnout ricavata dal barometro si ottiene l'accelerazione media della fase
di spinta, e da lì una stima del picco. Come esercizio di fisica per i ragazzi vale
probabilmente più del numero letto direttamente sul sensore.

### 📌 Decisioni sui sensori — stato al 4 settembre 2026

**Configurazione di volo confermata**: **MPU6050 + BMP280** (entrambi già in
inventario) + microSD + camera OV3660. Si vola con questa e si valuta sui dati reali.

| Candidato | Costo | Decisione |
|---|---|---|
| ADXL345 | ~15 € / 5 pz | ❌ **scartato** — satura a ±16 g come l'MPU6050, e senza giroscopio |
| BNO085 | ~40 € | ❌ **scartato** — ±8 g, fusione inutile su 5 s di volo |
| H3LIS331 / ADXL375 | ~36 € | ⏸️ **rimandato** — unico modo di avere il picco vero, ma si decide dopo i primi voli |
| ICM-20649 | ~15–20 € | ⏸️ **rimandato** — interessa più per il giroscopio ±4000 °/s che per i ±30 g |
| DPS310 | ~4 € | ⏸️ **rimandato** — miglior rapporto valore/prezzo se il rumore del BMP280 si rivelasse limitante |

👉 **Prima dei sensori, le due azioni a costo zero**: i **fori di sfiato** (l'errore da
pressione dinamica vale ~6 m, sei volte il rumore del BMP280 — conta molto di più di
qualsiasi upgrade) e l'uso del **microfono PDM** già a bordo per il timing degli eventi.

### Cosa si ricava davvero dal log (senza sensori aggiuntivi)

Con BMP280 + MPU6050 tosato a ±16 g si perde **soltanto il picco istantaneo**. Velocità
e *accelerazione media* sono entrambe recuperabili, ed è una distinzione importante.

| Grandezza | Recuperabile? | Come |
|---|---|---|
| Durata della spinta `Δt` | ✅ **misurata** | Il log passa da saturo a caduta libera in modo netto: si legge direttamente |
| Velocità di burnout `Δv` | ✅ **misurata** (±10–15 %) | Barometro, tre strade indipendenti — vedi sotto |
| **Accelerazione media** | ✅ **misurata** | `a_media = Δv / Δt` |
| Accelerazione post-burnout | ✅ **misurata** | Coast e discesa stanno in 0–2 g, ampiamente in scala |
| **Picco istantaneo** | ❌ non misurato | Ma **calcolabile** da un modello vincolato, vedi sotto |

#### Tre strade per la velocità di burnout

Conviene incrociarle, perché sbagliano in direzioni note:

1. **Dall'apogeo**: `v² = 2·g·h_coast`. Ignora la resistenza → **sottostima**.
2. **Dal tempo di salita**: `v ≈ g·t_coast`. Anche questa sottostima (con drag la
   decelerazione è > g, quindi la salita dura meno).
3. **Dalla derivata del barometro** subito dopo il burnout: la più diretta. Con rumore
   di ~1 m e finestra 0,2 s si sta sui ±5 m/s su ~35 m/s.

🎓 La strada elegante — e un bellissimo esercizio di laboratorio — è **fittare la fase
di coast con un modello di resistenza**: si ottengono in un colpo solo la velocità di
burnout *e* il coefficiente di drag del razzo.

⚠️ **Errore sistematico da non ignorare**: a 35 m/s la pressione dinamica vale
`½·ρ·v² ≈ 735 Pa`, che il BMP280 legge come **~6 m di quota fantasma**. Se il sensore
"sente" il flusso, le velocità risultano falsate proprio nella fase veloce. Rimedio
standard della missilistica: **fori di sfiato** piccoli, laterali, a 90° rispetto al
flusso e lontani dall'ogiva, così la cabina vede pressione statica.

#### E il picco? Si calcola, non si tira a indovinare

Il punto è che `Δv` e `Δt` sono **due misure che vincolano il modello**. Note la
pressione al lancio (la si legge sulla pompa) e l'area dell'ugello, il modello di spinta
`F = 2·A·ΔP` deve riprodurre *entrambe*. Se lo fa, la sua previsione del picco è
credibile; se non lo fa, si è sbagliato qualcosa. Non è una stima campata in aria: è un
modello validato su dati propri.

Un vincolo in più, gratis: **l'integrale dell'accelerometro tosato è un limite
inferiore di `Δv`**. La differenza rispetto al `Δv` barometrico misura l'area tagliata
via dalla saturazione, e quindi quanto si è andati sopra i 16 g.

⚙️ **Requisito di log**: per avere `Δt` con precisione decente su una spinta di ~0,18 s
servono almeno **200–500 Hz** durante la fase di boost. A 100 Hz si hanno ~18 campioni:
si sopravvive, ma il margine è risicato.

### Cosa si compra utile con ~10 €

Premessa netta: **a 10 € non si compra il picco a 60 g.** Quel fondo scala parte da
~25–36 € (H3LIS331 / ADXL375). Nella fascia dei 10 € si compra **precisione su ciò che
si misura già**, non un range nuovo. Detto questo, due cose valgono davvero i soldi.

#### 🥇 DPS310 — barometro migliore, ~3–6 € · **il miglior rapporto valore/prezzo**

È l'upgrade più sensato di tutti, perché il barometro è **il fattore che limita
l'intera catena di misura**: apogeo, velocità e quindi accelerazione media derivano
tutti dalla curva di quota.

| | BMP280 (in inventario) | **DPS310** |
|---|---|---|
| Risoluzione di pressione | 0,18 Pa | **0,06 Pa** |
| Accuratezza relativa | ±12 Pa ≈ **±1 m** | ±2 cm rilevabili |
| Sensibilità a max oversampling | riferimento | **>10× migliore** |
| Tecnologia | piezoresistiva | capacitiva, meno sensibile alla temperatura |
| ODR | fino a ~157 Hz (ma rumorosa a rate alti) | fino a 128 Hz |

Migliorare di un ordine di grandezza il rumore di quota **migliora direttamente la
velocità ricavata per derivazione**, che è esattamente il numero che interessa.

- I2C, nessun pin aggiuntivo. ⚠️ Indirizzo `0x76/0x77`, **lo stesso del BMP280**: o si
  sostituisce il BMP280, o si usa l'indirizzo alternativo via pin SDO.
- Nota: il **BMP280 è fuori produzione** (Bosch). I 5 in inventario restano ottimi per
  il laboratorio e la stazione a terra, ma per il razzo il DPS310 è meglio.
- Alternativa un gradino sopra: **BMP390** (~8–12 €), ±0,25 m e 200 Hz di ODR.

#### 🥈 ICM-20649 — IMU wide-range, ~15–20 €

Sopra i 10 €, ma risolve **due** limiti insieme:

- **Accelerometro ±30 g** (contro i ±16 g dell'MPU6050). Non basta per i ~60 g di
  picco, ma trasforma "tosato per tutta la spinta" in "tosato solo sulla cuspide": si
  cattura buona parte della curva invece di un plateau piatto.
- **Giroscopio ±4000 °/s** — e qui c'è un rischio che il progetto non aveva ancora
  considerato: l'MPU6050 si ferma a **±2000 °/s, cioè 333 giri/min**. Un razzo con le
  pinne anche solo leggermente disallineate può girare più veloce, e allora **anche il
  dato di rollio satura**. Il ±4000 °/s raddoppia il margine.

Breakout Adafruit [#4464](https://www.adafruit.com/product/4464) a $14,95; il chip nudo
sta sui 6 $, e su eBay/AliExpress girano moduli più economici.

#### 🥉 Gratis: il microfono PDM già a bordo

La XIAO Sense ha un microfono PDM che il progetto non sta usando. A 16 kHz dà la
**cronologia degli eventi con risoluzione sotto il millisecondo**: istante di lancio,
burnout (il getto smette di rumoreggiare), apertura del paracadute, impatto. È il modo
più preciso e più economico — costa zero — per misurare il `Δt` della spinta, che è
metà del calcolo dell'accelerazione media.

#### ❌ Da non inseguire: il tubo di Pitot

Sembra l'idea giusta — misura la velocità *direttamente* — ma il sensore economico del
caso, l'**MPXV7002DP** (~8–12 €), vuole 5 V e restituisce 0,5–4,5 V analogici: a
batteria il rail 5 V non esiste (§*Alimentazione*) e 4,5 V friggerebbero l'ADC a 3,3 V.
Servirebbero boost converter e partitore, e a quel punto non è più una soluzione da
10 €. Le versioni I2C a 3,3 V (SDP810, MS4525DO) partono da 25 € abbondanti.

### BNO085: ottimo sensore, problema sbagliato

Il [BNO085](https://www.adafruit.com/product/4754) (CEVA/Hillcrest, ex Bosch BNO080) è
una **IMU 9-DOF con fusione a bordo**: accelerometro + giroscopio + magnetometro *più*
un Cortex-M0+ che gira il firmware SH-2 e restituisce direttamente il **quaternione di
orientamento assoluto**, già filtrato. Niente Madgwick o Mahony da scrivere.

| Spec | Valore |
|---|---|
| Accelerometro | **±8 g** ⚠️ |
| Giroscopio | ±2000 °/s |
| Magnetometro | ±8 gauss |
| Uscite fusione | Rotation Vector 400 Hz · Gyro RV **1000 Hz** · gravità, accel lineare, quaternione |
| Interfacce | I2C · SPI · **UART-RVC** |
| Alimentazione | 1,8–3,6 V |
| Formato / peso | 25,6 × 22,7 × 4,6 mm · 2,5 g |
| Prezzo | ~$29,50 → **35–45 €** in Europa |

#### Perché qui non conviene

**1. L'accelerometro satura a ±8 g — la metà dell'MPU6050.** Sulla domanda del picco di
lancio (~60 g, vedi sopra) è un **passo indietro**, non avanti.

**2. La fusione risolve un problema che questo volo non ha.** Il valore del BNO085 è
l'assetto assoluto senza deriva su tempi lunghi. Ma il volo dura **~5 secondi**: su
quella finestra la deriva del giroscopio dell'MPU6050 è trascurabile, e la semplice
**integrazione del giroscopio grezzo** dà un assetto ottimo senza alcun filtro. Si
pagherebbero 40 € per una capacità che qui vale poco.

**3. Il magnetometro è inutile e in più disturbato.** La calibrazione a otto non
converge mai su un razzo, e soprattutto: a pochi centimetri c'è il **buzzer 12095, che è
magnetico** e genera campo quando suona. Si finirebbe a usare il *Game Rotation Vector*
(solo giro + accel, mag escluso) — cioè a disattivare il sensore in più che si è pagato.

**4. La fusione presume moti "normali".** Un lancio a 60 g e un atterraggio duro violano
le ipotesi dell'algoritmo: il BNO08x è documentato come bisognoso di re-inizializzazione
dopo shock forti, e il Rotation Vector può degradare **proprio durante la spinta**, la
fase che interessa di più.

**5. L'I2C del BNO08x è capriccioso.** Il protocollo SHTP con pacchetti lunghi e clock
stretching dà problemi di affidabilità noti. Se lo si comprasse, andrebbe cablato in
**UART-RVC** su D6/D7 (liberi): stream seriale semplice, molto più robusto — ma dà solo
yaw/pitch/roll a 100 Hz, non il quaternione completo.

#### Verdetto e priorità

**Non prenderlo per questo progetto.** È il sensore giusto per un drone, un visore AR o
un robot che devono tenere l'assetto stabile per ore. Qui, MPU6050 + integrazione del
giroscopio su 5 secondi dà la stessa risposta per 4 €.

Ordine di priorità se un giorno si volesse spendere su un sensore opzionale:

| | Cosa | Perché |
|---|---|---|
| 1️⃣ | **Niente — volare prima** | Il kit attuale copre già quota, apogeo, assetto e video |
| 2️⃣ | High-g (H3LIS331, ~36 €) | Risponde a una domanda che il kit attuale **non può** rispondere |
| 3️⃣ | BNO085 (~40 €) | Risponde a una domanda a cui il kit attuale **già risponde** abbastanza bene |

### Alimentazione e ricarica della LiPo

**Sì, la XIAO ricarica la batteria da sola**: ha un caricabatterie 1S integrato
(chip **SGM40567**, cutoff a 4,2 V) alimentato dal VBUS della USB-C. Non serve
nessun modulo esterno tipo TP4056.

| | |
|---|---|
| Pad | **BAT+ / BAT-** sul retro della board (da saldare) |
| Polarità | **BAT- è il lato vicino alla USB-C**, BAT+ quello lontano. Invertirla è fatale |
| Corrente di carica | **50 mA** (fast) / 3,8 mA (trickle) — *non* regolabile |
| LED di stato | Rosso: acceso all'inserimento USB, **lampeggia in carica**, si spegne a fine carica |
| Lettura tensione | ❌ nessun GPIO collegato alla batteria — vedi sotto |
| Pin 5V | **non alimentato** quando si va a batteria |

**Implicazione sui tempi di ricarica** (a 50 mA):

| Batteria | Tempo di ricarica |
|---|---|
| 300 mAh | ~7–8 h |
| 400 mAh | ~9–10 h |
| 2000 mAh | **~45 h** — impraticabile, ennesimo motivo per lasciarla a terra |

👉 In giornata di lanci non ci si ricarica dalla XIAO. La strategia è **batterie in
rotazione**: si portano in campo 4–5 LiPo già cariche (per questo in §14 il lotto è da
5 pezzi) e si ricaricano tutte la sera prima. In alternativa, un caricatore LiPo 1S
esterno (~0,5C) per rabboccare tra un lancio e l'altro.

**Tensione batteria in telemetria** *(opzionale, non prevista)*: Seeed non collega la
batteria a nessun ADC, quindi via software la tensione non è leggibile. Se un domani
la si volesse nel log serve un **partitore esterno** — 2× 100 kΩ da BAT+ a GND, presa
centrale su **A0/GPIO1** (dev'essere ADC1: l'ADC2 non funziona col WiFi acceso), ADC in
attenuazione 12 dB → `V_bat = 2 × V_adc`. Costo: 2 resistenze e ~30 µA.

⚠️ La XIAO **non ha protezione di sottotensione** in scarica: usare solo LiPo con
**PCM integrato**, altrimenti una sessione dimenticata accesa uccide la cella.

📖 Fonte: [Seeed Wiki — XIAO ESP32-S3 getting started](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)

### Buzzer: attivo o passivo?

**Per il razzo serve l'ATTIVO.** È quello che "ha bisogno solo di alimentazione":
ha l'oscillatore dentro, gli dai tensione e suona.

| | **Attivo** ✅ | Passivo |
|---|---|---|
| Oscillatore interno | ✅ sì | ❌ no |
| Come si pilota | `digitalWrite(pin, HIGH)` | onda quadra PWM (`ledcWriteTone`) |
| Se togli il segnale | tace | tace |
| Tono | **fisso**, ~2,3–2,7 kHz | qualunque frequenza → melodie |
| Occupa un timer LEDC | ❌ no | ✅ sì |
| Se il firmware crasha | **continua a suonare** (il pin resta alto) | ammutolisce |

Le tre ragioni per cui vince l'attivo in volo:

1. **Robustezza** — è l'argomento decisivo. Il buzzer serve a ritrovare il razzo
   *dopo* un atterraggio violento. Se l'ESP32 va in panic o watchdog reset, il
   passivo smette di suonare perché nessuno genera più il PWM; l'attivo con il pin
   rimasto alto continua a fischiare. Il razzo lo ritrovi lo stesso.
2. **Volume a parità di taglia** — l'attivo lavora sempre sulla sua frequenza di
   risonanza (~2,7 kHz), dove rende il massimo dei dB. Col passivo devi azzeccarla
   a mano, e fuori risonanza perdi anche 10–15 dB.
3. **Zero costo CPU** — non serve un canale LEDC né un task che lo alimenti mentre
   la camera scrive su SD.

**Come distinguerli** se ne hai di misti: il passivo ha il fondo aperto, si vede la
piastrina verde/bobina; l'attivo è **sigillato** con un adesivo ed è più alto. Prova
del nove: 3 V dalle batterie stilo sui piedini — se fischia è attivo, se fa un
singolo "tic" è passivo.

### Modello scelto: cicalino attivo 12095 3 V

[Lotto da 10 su AliExpress](https://it.aliexpress.com/item/1005012110010590.html) — vedi §14.

| Spec | Valore | Note |
|---|---|---|
| Tipo | attivo, magnetico | oscillatore interno ✅ |
| Frequenza | **2500 ± 200 Hz** | in pieno centro della banda 2–4 kHz dove l'orecchio localizza meglio |
| Tensione | **3 V** | ✅ suona dal rail 3V3 della XIAO |
| Corrente | **25 mA** (a 3 V) | ✅ sotto i 40 mA/pin dell'S3 → GPIO diretto |
| Formato | 12 × 9,5 mm, passo pin 7,5 mm, ABS | ~2 g, entra ovunque in ogiva |

⚠️ **Pilotarlo dal 3V3, NON da BAT+.** Nella versione precedente di questa nota
consigliavo BAT+ (3,7–4,2 V) per guadagnare volume: **vale solo per un cicalino dato
5 V**. Questo è dato **3 V**, e 4,2 V lo manderebbero il 40 per cento sopra specifica
— a lungo andare lo brucia. Alimentarlo dal **3V3** (una sovratensione del 10 per
cento, del tutto normale per un magnetico) e commutare il negativo col transistor.

✅ **Si attacca direttamente al GPIO — il transistor non serve.**

I numeri dell'ESP32-S3: il limite *assoluto* per pin è **40 mA**, quello *consigliato*
20 mA. I 25 mA del 12095 stanno in mezzo — sopra la raccomandazione conservativa, ma
con il 37 per cento di margine sul limite di danneggiamento. Il pin regge.

Due precisazioni che tolgono i dubbi residui:

- **La "drive strength" non è un fusibile.** I 20 mA di default (`GPIO_DRIVE_CAP_2`)
  sono la corrente entro cui il pin garantisce la V_OH di specifica, non la soglia a
  cui si rompe. Superandola l'uscita si abbassa un po', tutto qui. Alzandola a
  `GPIO_DRIVE_CAP_3` (~40 mA) il calo sparisce e il cicalino suona al massimo:

  ```c
  #include "driver/gpio.h"
  gpio_set_drive_capability(GPIO_NUM_x, GPIO_DRIVE_CAP_3);
  ```

- **Essendo ATTIVO, non è un carico induttivo visto da fuori.** La bobina è pilotata
  dall'oscillatore *interno* al cicalino: la commutazione induttiva avviene dentro
  l'involucro, non sul tuo GPIO, che vede sostanzialmente un DC. Il flyback esterno
  perde quasi tutto il suo senso. (Con un buzzer **passivo** magnetico, dove la bobina
  la commuti tu, il discorso sarebbe opposto.)

Il **1N4148** in parallelo (catodo verso 3V3) resta consigliato come assicurazione da
5 centesimi, ma è opzionale.

🔬 **L'unica verifica che vale la pena fare**: al primo collegamento misura la corrente
reale con il multimetro in serie. I 25 mA sono dichiarati a 3 V; a 3,3 V saliranno a
~28–30 mA. Se il tuo esemplare superasse i **35 mA**, allora sì, metti l'NPN:

```
3V3 ──── buzzer ──┬── C  [NPN BC337 / 2N2222]
                  │      E ── GND
      1N4148 ─────┘      B ── 1 kΩ ── GPIO
```

⚠️ Sul **duty cycle**: il 200/800 fa risparmiare batteria e aiuta termicamente, ma non
è l'argomento che rende sicuro il collegamento diretto — quando il pin è alto vede
comunque 25–30 mA istantanei, ed è quel valore a contare per l'integrità del pin. Il
collegamento diretto è sicuro perché 25 mA < 40 mA, non perché sono intermittenti.

⚠️ Scegliere un **GPIO libero** (la shield camera della Sense ne occupa parecchi) e
diverso da quello del partitore batteria. Non caricare altri assorbimenti pesanti sui
pin adiacenti: oltre al limite per pin esiste un limite cumulativo di chip.

💡 **Pattern di ricerca**: non un fischio continuo ma **200 ms on / 800 ms off**.
L'orecchio localizza molto meglio un suono intermittente, e consumi un quinto — a
25 mA significa 5 mA medi, irrilevanti sui 400 mAh della cella.

🎓 **Nota lab**: il lotto scelto è di soli **attivi**. Se in laboratorio servono
melodie o codici a toni diversi serve un passivo a parte (~3 €), ma non è nulla di
urgente — dei 10 cicalini, 2–3 vanno in volo e gli altri restano per la didattica
"accendi e suona".

### Sensori del lab da NON portare in volo

| Sensore | Perché no | Uso alternativo |
|---|---|---|
| DHT11 | Risposta 2 s, volo dura ~5 s | Stazione meteo a terra |
| MQ-135 | Preriscaldo 30 s, alto consumo | Stazione a terra |
| LiPo 2000 mAh | ~40 g, penalizza la quota | Ground station / banco |

### Posizionamento meccanico

Elettronica **nell'ogiva** (verde del kit AliExpress, o sostitutiva stampata 3D più
capiente — vedi §12). L'ogiva si stacca all'apogeo per inerzia, gli elastici cedono
e tira fuori il paracadute; scende legata al corpo via shock cord. **Annegare PCB e
LiPo in spugna densa** dentro un vano dedicato. LiPo **sempre fissata** con biadesivo
o fascette, mai libera.

### Architettura firmware suggerita

Sposa il modello didattico "un gruppo = un sensore" con task FreeRTOS separati:

```
[Task BMP280] ─┐
[Task MPU6050]─┼→ [Queue dati] → [Task Logger SD] + [Task Telemetria WiFi/BLE]
[Task Camera] ─┘
```

**Firmware di riferimento (open source):**
- **[zerneo85/ESP-Controlled-Rocket](https://github.com/zerneo85/ESP-Controlled-Rocket)** (GPL-3.0, attivo): BMP280 + MPU6050, Web UI, WebSocket live, 3D viewer, log SD. **Punto di partenza consigliato.**
- **[MaelStudio/tritonFC](https://github.com/MaelStudio/tritonFC)**: gira nativamente su XIAO ESP32-S3 Sense, gestisce video AVI, PCB di riferimento.
- **[nodebotsau/water-rocket](https://github.com/nodebotsau/water-rocket)** (MIT): ground station Node.js + MQTT, riusabile.

---

## 14. Acquisti e inventario 🛒

### Hardware fisico (razzo + lanciatore)

- [Kit lanciatore + paracadute AliExpress](https://it.aliexpress.com/item/1005008517820572.html)
  — paracadute passivo a *nose-cone pop-off*, fino a ~50–70 m reali

### Da acquistare (kit volante elettronica) — ~99 €

| # | Articolo | Q.tà | € | Link |
|---|---|---|---|---|
| 1 | **Seeed XIAO ESP32-S3 Sense** | 3 | 60 | [AliExpress ✓ **scelta**](https://it.aliexpress.com/item/1005006988111963.html) (lotto da 3, camera **OV3660** 3 MP) · alt: [Amazon.it](https://www.amazon.it/Seeed-Studio-XIAO-ESP32S3-Sense/dp/B0C69FFVHH) (32 €/pz) |
| 2 | **LiPo 1S 400 mAh 602040 JST 1.25** | 5 | 16 | [AliExpress ✓ **scelta, verificata**](https://it.aliexpress.com/item/1005012195250525.html) (lotto da 5, promo; listino 20,77 €) |
| 3 | **MPU6050 / GY-521** | 2 | 8 | [Amazon.it](https://www.amazon.it/s?k=GY-521+MPU6050) · [AliExpress](https://it.aliexpress.com/w/wholesale-gy-521-mpu6050.html) |
| 4 | **Cicalino attivo 12095 3 V** | 10 | 6 | [AliExpress ✓ **scelto, verificato**](https://it.aliexpress.com/item/1005012110010590.html) (2,46 € in promo — 6 € coprono spedizione/soglia) |
| 5 | **LED + resistenze** (se mancanti) | 1 kit | 5 | [Amazon.it](https://www.amazon.it/s?k=led+kit+5mm+arduino) |
| 6 | **Pigtail JST 1.25** (5 paia M/F, 28 AWG, 15 cm) | 5 paia | 4 | AliExpress ✓ **scelto** (1,59 €) — per lo swap batterie, vedi nota |
| — | ~~Antenna IPEX 2.4 GHz~~ | — | **0** | ✅ **Già inclusa nella confezione della XIAO** — vedi nota sotto |
| | | **Totale** | **~99 €** | |

#### Note sulle scelte di acquisto

**XIAO ESP32-S3 Sense — [lotto da 3 con OV3660 scelto](https://it.aliexpress.com/item/1005006988111963.html) (60 €, 20 €/pz)**

| | Amazon.it | AliExpress |
|---|---|---|
| Prezzo unitario | **32 €** | **20 €** (3 pz / 60 €) |
| Spesa per il kit | 64 € (2 board) | 60 € (**3** board) |
| Camera | OV2640 2 MP | **OV3660 3 MP** |
| Consegna / resi | 1–2 gg, reso facile | 2–4 settimane |
| Rischio clone | nullo (Seeed ufficiale) | possibile PSRAM/flash inferiori |

- 👉 **AliExpress vince nettamente**: 3 board a 60 € contro le 2 board a 64 € di
  Amazon. Destinazione: **2 di volo + 1 di scorta / banco** — una board che parte con
  l'ogiva prima o poi si rompe o si perde.
- 🔁 *Fallback*: se le 2–4 settimane di spedizione bloccano lo sviluppo, si prende
  **1 pz da Amazon** (32 €, consegna 1–2 gg) per iniziare a scrivere firmware mentre
  il lotto viaggia.
- **OV3660** è supportata nativamente da `esp32-camera` (rilevata via I2C all'init,
  nessuna patch necessaria). Risoluzione max 2048×1536, ma per il volo si resta
  comunque a **640×480 @ 25 fps** — il collo di bottiglia è la scrittura su microSD,
  non il sensore.
- ⚠️ Resta un **rolling shutter**: su un razzo che rolla si vede l'effetto "jello".
  L'OV3660 non lo risolve, è un limite fisico di entrambi i sensori.
- ⚠️ Verificare nella descrizione che sia la versione **Sense** (shield con slot
  microSD + PSRAM 8 MB), non la XIAO ESP32-S3 liscia.

**Antenna 2.4 GHz — non serve comprarla**

Seeed spedisce la XIAO ESP32-S3 **con l'antenna IPEX/U.FL 2,4 GHz nella scatola**:
va solo estratta e innestata sul connettore (premere prima da un lato, poi
dall'altro — il connettore U.FL si rompe se lo si spinge storto). Con l'antenna
montata Seeed dichiara **100 m+** di portata, ben oltre i ~50–70 m di quota utile
del razzo. Riga azzerata: **−6 €**.

- ⚠️ Il lotto AliExpress è l'unica incognita: **verificare nelle foto dell'inserzione
  che le antenne siano incluse**. Se non ci sono, si recuperano a ~3 €/pz
  ([Seeed ufficiale](https://www.seeedstudio.com/2-4GHz-2-81dBi-Antenna-for-XIAO-ESP32C3-p-5475.html)
  o [Amazon](https://www.amazon.it/s?k=antenna+IPEX+UFL+2.4GHz)).
- L'antenna ceramica onboard funziona senza la U.FL, ma con portata molto ridotta:
  in volo va montata l'esterna, orientata **lungo l'asse dell'ogiva** e lontana dalla
  LiPo (la busta metallizzata scherma).

**Batteria — [LiPo 602040 400 mAh, lotto da 5](https://it.aliexpress.com/item/1005012195250525.html) ✅ verificata (15,99 €, 3,20 €/pz)**

Scheda tecnica confrontata coi requisiti: **passa su tutta la linea.**

| Requisito | Richiesto | Dichiarato | |
|---|---|---|---|
| Celle / tensione | 1S, 3,7 V | Li-Po **3,7 V** nominali | ✅ |
| Capacità | 250–500 mAh | **400 mAh** (1,48 Wh stampati sulla cella) | ✅ |
| Connettore | JST 1.25 | **JST 1.25 mm** | ✅ |
| Protezione | PCM integrato | overcharge / **over-discharge** / overcurrent / short circuit | ✅ |
| Peso | ≤ 12 g | non dichiarato → **~9–11 g** stimati dal volume | ✅ stimato |
| Ingombro | vano ogiva | **6 × 20 × 42 mm** | ✅ vedi nota |
| Scarica | ≥ 5C | non dichiarato | ⚠️ non è un problema, vedi sotto |

**Sul C-rating mancante.** È una cella da elettronica di consumo (auricolari), non una
LiPo da drone: nessuno le dichiara il C-rating. I conti però tornano lo stesso. Un
602040 ha resistenza interna tipica di **~150 mΩ**; il picco peggiore della XIAO
(camera attiva + burst WiFi TX) sta sui **500 mA**, che producono una caduta di
`0,5 A × 0,15 Ω ≈ 75 mV`. Da 3,9 V si scende a 3,83 V — l'LDO della XIAO va in dropout
solo sotto ~3,4 V. **Nessun rischio di brownout.** A 400 mAh il picco è appena 1,25C.

**Sull'ingombro.** I 42 mm di lunghezza sfondano il limite di 35 mm che avevo scritto,
ma quel numero era una stima cautelativa mia: in un'ogiva da bottiglia la lunghezza non
è mai il vincolo (l'ogiva è lunga 80–120 mm), lo sono **larghezza e spessore**. Con
20 × 6 mm la cella è sottilissima e ci sta comodamente, spugna inclusa. ⚠️ Da verificare
solo se si passa all'**ogiva stampata 3D** (§12): misurare il vano prima di stampare.

**Autonomia e ricarica**: 400 mAh ≈ **1,5–2,5 h** armato con camera + WiFi, abbondanti
per una giornata di lanci. Ricarica dalla XIAO a 50 mA ≈ **9–10 h**, cioè una notte:
si caricano tutte e 5 la sera prima e si ruotano in campo.

⚠️ **Prezzo e tempi**: 15,99 € è **promo in scadenza il 7 settembre** (listino 20,77 €),
spedizione gratuita, **consegna stimata 23–29 settembre**. Se le board XIAO arrivano
prima, si può iniziare a sviluppare a USB.

🔌 **Pigtail JST 1.25** (riga 6 della BOM — 5 paia M/F, 28 AWG, 15 cm). La XIAO ha
solo i pad BAT+/BAT- nudi: tagliare il connettore della cella e saldare i fili
funziona, ma **butta via la strategia di rotazione** — con 5 celle vuoi poterle
scambiare al volo. Si salda invece sui pad la **controparte** del connettore, così le
batterie si innestano. Il kit contiene entrambi i generi, quindi qualunque sia quello
montato sulla cella hai il suo mate. 28 AWG regge oltre 1 A: abbondante per i 500 mA
di picco. **Accorciare i 15 cm** a 3–4 cm, in ogiva il filo in eccesso è solo ingombro.

⚠️ **Verificare la polarità col multimetro prima del primo innesto.** Rosso = +,
nero = − è la convenzione, ma su pigtail e celle di fornitori diversi non è garantita:
capita di trovare housing cablati al contrario. Sulla XIAO **BAT- è il pad vicino alla
USB-C**. Un'inversione distrugge la board — 30 secondi di continuità valgono 20 €.

✅ **Check alla consegna**: pesare una cella (dev'essere ≤ 12 g) e misurare la tensione
a vuoto — deve arrivare a **~3,8 V di storage charge**. Sotto 3,0 V è da scartare.

### 🔖 Link da valutare

Parcheggio per inserzioni segnalate ma non ancora decise.

| Link | Cos'è | Stato |
|---|---|---|
| [AliExpress 1005009579301045](https://it.aliexpress.com/item/1005009579301045.html) | ❓ da identificare | salvato il 4 set 2026 — specifiche non leggibili in automatico (AliExpress blocca la lettura della scheda prodotto) |

### Già in possesso (riusabile per il razzo)

| Pezzo | Q.tà | Uso nel razzo |
|---|---|---|
| ✅ BMP280 breakout | 5 | Altimetria di volo |
| ✅ microSD 32 GB | 5 | Log + video (slot della XIAO Sense) |
| ✅ ESP32-S3 DevKitC-1 (DUBEUYEW) | 2 | Ground station, test a banco, kit didattici |
| ✅ Freenove ESP32-WROVER + camera | 1 | Ground station / test pipeline camera |
| ✅ TTGO T-Display | 1 | Display a terra per telemetria live |
| ✅ BH1750, TSL2591 (luce) | 3 + 1 | Sensori opzionali (didattica) |
| ✅ RTC DS3231 | 5 | Timestamp log (opzionale) |
| ✅ LiPo 2000 mAh JST 1.25 | 4 | **Solo ground station** — troppo pesanti per volare |
| ✅ Breadboard + jumper + USB-C | — | Prototipazione |
| ✅ Stagno | — | Saldature (pad BAT della XIAO) |

### Sensori del lab esclusi dal volo

❌ DHT11, MQ-135, LiPo 2000 mAh, pannello solare — vedi §13 *"da NON portare in volo"*.

---

*Buoni lanci! 🚀*
```
