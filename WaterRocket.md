# Progetto Water Rocket 🚀💧

Documento di progettazione per un razzo ad acqua (water rocket): principi fisici,
componenti, materiali, calcoli, costruzione, lancio e sicurezza.

---

## Indice

1. Come funziona
2. Componenti principali
3. Materiali (lista della spesa)
4. Parametri di progetto consigliati
5. Calcoli rapidi
6. Costruzione (passo-passo)
7. Lancio e procedura di test
8. Strumenti utili
9. ⚠️ Sicurezza (leggere SEMPRE)
10. Idee di miglioramento
11. Link di documentazione e risorse
12. Stampa 3D 🖨️
13. Elettronica di bordo 🔌
14. Acquisti e inventario 🛒
15. Sensori opzionali: analisi e decisioni 🔬

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

### Board scelta: Seeed XIAO ESP32-S3 Sense

| Caratteristica | Valore |
|---|---|
| Dimensioni / peso | 21×17,5 mm, ~3 g |
| Camera | OV2640 2 MP (staccabile, opz. OV5640; i cloni AliExpress montano spesso **OV3660 3 MP**) |
| Microfono | PDM digitale |
| microSD slot | Sì, sulla shield camera |
| PSRAM / Flash | 8 MB / 8 MB |
| Carica LiPo | Sì, pad **BAT+/BAT-** da saldare |
| Wireless | WiFi 2,4 GHz + BLE 5.0, **solo connettore U.FL/IPEX** (nessuna antenna sulla board): senza l'antenna esterna inclusa funziona solo a pochi cm–2 m |
| Riferimento firmware | usata dal progetto `tritonFC` |

### Sensori e funzionalità sbloccate

| Sensore | Bus | Cosa abilita | Priorità |
|---|---|---|---|
| **BMP280** | I2C | Quota live, apogeo, profilo di volo, T/P cabina | ⭐⭐⭐ |
| **MPU6050** | I2C | Accelerazione lancio, giroscopio (roll/yaw/pitch), rilevazione apogeo via segno accel | ⭐⭐⭐ |
| **microSD** (slot integrato) | SPI | Log ≥100 Hz + video AVI | ⭐⭐⭐ |
| **Camera OV2640** | DVP | Video 640×480 a 15–25 fps, foto, analisi traiettoria | ⭐⭐ |
| **Buzzer attivo** | GPIO on/off | Beep di ritrovamento | ⭐⭐ |
| **LED ultra-bright** | GPIO | Visibilità slow-mo, stato armato/disarmato | ⭐ |
| **Microfono PDM** | I2S | Acustica del getto d'acqua, evento apertura paracadute | ⭐ |
| **RTC DS3231** (lab) | I2C | Timestamp assoluti, sincronizzazione multi-razzo | ⭐ |
| **BH1750 / TSL2591** (lab) | I2C | Profilo luminosità lungo la traiettoria (didattico) | ⭐ |
| **GPS NEO-M8N** | UART | Ritrovamento a distanza (≥70 m). Costa ~20 g | ⭐ opz |
| **Accelerometro high-g** | I2C | Picco reale al lancio (l'MPU6050 satura a ±16 g) — vedi §15 | ⭐ opz |

La configurazione di volo attuale è **BMP280 + MPU6050**, entrambi già in inventario.
I sensori marcati *opz* sono valutati in **§15**, con il registro delle decisioni.

### Sensori del lab da NON portare in volo

| Sensore | Perché no | Uso alternativo |
|---|---|---|
| DHT11 | Risposta 2 s, volo dura ~5 s | Stazione meteo a terra |
| MQ-135 | Preriscaldo 30 s, alto consumo | Stazione a terra |
| LiPo 2000 mAh | ~40 g, penalizza la quota | Ground station / banco |

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

### Pinout e collegamenti

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
| D3 | 4 | **usato** | **Anello LED WS2812B 8 LED** (dati, via 330 Ω) — vedi [cablaggio.md](cablaggio.md) |
| D0 | 1 | libero (ADC1) | Partitore tensione batteria *(opzionale)* |
| D6 | 43 | libero (UART0 TX) | — *(GPS NEO-M8N se mai servisse)* |
| D7 | 44 | libero (UART0 RX) | — *(idem)* |
| D2 | 3 | ⚠️ strapping + ambiguità SD_CS | **non usare** |
| D8 | 7 | 🔒 SPI SCK | microSD |
| D9 | 8 | 🔒 SPI MISO | microSD |
| D10 | 9 | 🔒 SPI MOSI | microSD |
| — | 21 | 🔒 SD_CS (era il LED) | microSD |

**Bilancio: il progetto usa 4 pad su 8 disponibili.** Restano liberi D0, D6, D7
(più D2 da non toccare). Margine ampio: la configurazione non è nemmeno stretta.

- **Niente UART** nella configurazione scelta: D6/D7 restano liberi. Se un giorno
  servisse il GPS per il ritrovamento a distanza, i pad ci sono già.
- **Partitore batteria: opzionale, non previsto.** Se lo si aggiunge deve stare su un
  pin ADC1 (GPIO1–10) perché l'**ADC2 è inutilizzabile con il WiFi attivo** — D0/GPIO1
  è la scelta giusta. Senza partitore non c'è preavviso di batteria scarica: la board
  si spegne e basta. Non è un rischio concreto (400 mAh = 1,5–2,5 h contro pochi
  minuti di sessione), e la cella è comunque protetta dal suo PCM contro la scarica
  profonda. Basta partire carichi.
- **Indicatore visivo: anello WS2812B 8 LED su D3.** Sostituisce il LED onboard
  (occupato da SD_CS) e fa anche da luce di ritrovamento e di visibilità in volo.
  Va alimentato da BAT+, non dal 3V3: vedi [cablaggio.md §7](cablaggio.md#7-anello-led-ws2812b-8-led).

#### Bus I2C

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

#### Schema di cablaggio

📐 **Guida passo-passo con schemi, collaudo e checklist: [cablaggio.md](cablaggio.md).**

```
XIAO ESP32-S3 Sense
├── D4 / D5       → I2C: BMP280 (0x76) + MPU6050 (0x68)
├── D1            → buzzer attivo 12095 (diretto, 25 mA)
├── D8/D9/D10+G21 → 🔒 microSD (SPI, slot sulla shield Sense)
│
├── D3            → anello LED WS2812B 8 LED (dati via 330 Ω, VDD da BAT+)
│
│   -- opzionali, pad già liberi --
├── D0            → partitore 2×100 kΩ da BAT+ (lettura tensione, ADC1)
├── D6 / D7       → UART GPS NEO-M8N
├── Camera OV3660 → FPC dedicato, GPIO interni (nessun pad occupato)
├── BAT+ / BAT-   → pigtail JST 1.25 saldato → LiPo 1S 400 mAh (602040)
└── USB-C         → programmazione + ricarica (50 mA)
```

### Buzzer di ritrovamento

**Modello scelto: cicalino attivo 12095, 3 V** ([lotto da 10](https://it.aliexpress.com/item/1005012110010590.html), §14).

| Spec | Valore |
|---|---|
| Tipo | attivo, magnetico — oscillatore interno |
| Frequenza | 2500 ± 200 Hz, centro della banda dove l'orecchio localizza meglio |
| Tensione | 3 V, si alimenta dal rail 3V3 |
| Corrente | 25 mA a 3 V |
| Formato | 12 × 9,5 mm, passo pin 7,5 mm, ~2 g |

#### Perché attivo e non passivo

L'attivo ha l'oscillatore dentro: `digitalWrite(pin, HIGH)` e suona. Il passivo è un
altoparlantino nudo che senza onda quadra PWM non fa nulla.

| | Attivo | Passivo |
|---|---|---|
| Come si pilota | `digitalWrite` | PWM (`ledcWriteTone`) |
| Tono | fisso, ~2,5 kHz | qualunque, quindi melodie |
| Timer LEDC | non serve | ne occupa uno |
| Se il firmware crasha | **continua a suonare** | ammutolisce |

L'ultima riga è l'argomento decisivo: il buzzer serve a ritrovare il razzo *dopo* un
atterraggio violento, cioè proprio quando il firmware ha buone probabilità di essere
morto. Con il pin rimasto alto l'attivo fischia lo stesso. In più lavora sempre sulla
propria frequenza di risonanza, dove rende il massimo dei dB.

#### Collegamento: diretto al GPIO

I 25 mA stanno sotto il limite assoluto di **40 mA per pin** dell'ESP32-S3 (il valore
"consigliato" di 20 mA è la soglia entro cui il pin garantisce la V_OH di specifica,
non quella a cui si rompe). Alzando la drive strength il calo di tensione sparisce:

```c
#include "driver/gpio.h"
gpio_set_drive_capability(GPIO_NUM_2, GPIO_DRIVE_CAP_3);   // ~40 mA
```

Nessun transistor, nessun diodo di flyback: essendo **attivo**, la bobina è pilotata
dall'oscillatore interno al cicalino e il GPIO vede sostanzialmente un DC. Con un
passivo magnetico, dove la bobina la commuti tu, servirebbero entrambi.

- ⚠️ Alimentarlo dal **3V3, non da BAT+**. È dato 3 V: i 4,2 V della LiPo carica lo
  manderebbero il 40 % sopra specifica.
- 🔬 **Piano B**: misurare la corrente reale al primo collegamento. A 3,3 V salirà a
  ~28–30 mA; se superasse i 35 mA, interporre un NPN (BC337 / 2N2222, 1 kΩ di base)
  con un 1N4148 di flyback.

#### Pattern di ricerca

Non un fischio continuo ma **200 ms on / 800 ms off**: l'orecchio localizza molto meglio
un suono intermittente, e il consumo scende a ~5 mA medi, irrilevanti sui 400 mAh.

Il lotto è di soli cicalini attivi. Per le melodie del laboratorio servirebbe un passivo
a parte (~3 €), ma non è urgente: dei 10, 2–3 vanno in volo e gli altri restano per la
didattica.

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

**XIAO ESP32-S3 Sense — lotto da 3 con OV3660, 60 €**

| | Amazon.it | AliExpress ✓ |
|---|---|---|
| Prezzo unitario | 32 € | **20 €** (3 pz / 60 €) |
| Spesa per il kit | 64 € (2 board) | 60 € (**3** board) |
| Camera | OV2640 2 MP | **OV3660 3 MP** |
| Consegna / resi | 1–2 gg, reso facile | 2–4 settimane |
| Rischio clone | nullo (Seeed ufficiale) | possibile PSRAM/flash inferiori |

Destinazione delle 3 board: **2 di volo + 1 di scorta/banco**. Una board che parte con
l'ogiva prima o poi si rompe o si perde.

- 🔁 *Fallback*: se le 2–4 settimane bloccano lo sviluppo, 1 pz da Amazon (32 €,
  consegna 1–2 gg) per iniziare a scrivere firmware mentre il lotto viaggia.
- L'**OV3660** è supportata nativamente da `esp32-camera` (rilevata via I2C all'init).
  Max 2048×1536, ma in volo si resta a 640×480 @ 25 fps: il collo di bottiglia è la
  scrittura su microSD, non il sensore. Resta un **rolling shutter**, quindi su un razzo
  che rolla si vede l'effetto "jello" e l'OV3660 non lo risolve.
- ⚠️ Verificare che l'inserzione sia la versione **Sense** (shield con microSD + 8 MB
  PSRAM), non la XIAO ESP32-S3 liscia.

**Antenna 2.4 GHz — non serve comprarla**

Seeed la spedisce **nella scatola**: va estratta e innestata sul connettore U.FL
(premere prima da un lato, poi dall'altro — si rompe se spinta storta). Con l'antenna
montata Seeed dichiara 100 m+, ben oltre i ~50–70 m di quota del razzo. Riga azzerata:
−6 €.

- ⚠️ Verificare nelle foto dell'inserzione AliExpress che le antenne siano incluse. Se
  non ci sono, ~3 €/pz ([Seeed](https://www.seeedstudio.com/2-4GHz-2-81dBi-Antenna-for-XIAO-ESP32C3-p-5475.html)
  o [Amazon](https://www.amazon.it/s?k=antenna+IPEX+UFL+2.4GHz)).
- In volo orientarla **lungo l'asse dell'ogiva** e lontana dalla LiPo: la busta
  metallizzata scherma.

**Batteria LiPo 602040 400 mAh — verificata ✅**

Scheda tecnica confrontata coi requisiti: passa su tutta la linea.

| Requisito | Richiesto | Dichiarato | |
|---|---|---|---|
| Celle / tensione | 1S, 3,7 V | Li-Po 3,7 V | ✅ |
| Capacità | 250–500 mAh | 400 mAh (1,48 Wh sulla cella) | ✅ |
| Connettore | JST 1.25 | JST 1.25 mm | ✅ |
| Protezione | PCM | overcharge / **over-discharge** / overcurrent / short | ✅ |
| Peso | ≤ 12 g | ~9–11 g stimati dal volume | ✅ |
| Ingombro | vano ogiva | 6 × 20 × 42 mm | ✅ |
| Scarica | ≥ 5C | non dichiarato | ⚠️ vedi sotto |

- **Il C-rating mancante non è un problema.** È una cella da elettronica di consumo, non
  da drone: nessuno lo dichiara. Ma un 602040 ha resistenza interna tipica ~150 mΩ, e il
  picco peggiore della XIAO (~500 mA) produce una caduta di appena `0,5 × 0,15 ≈ 75 mV`.
  L'LDO va in dropout solo sotto ~3,4 V: **nessun rischio di brownout**. A 400 mAh quel
  picco è 1,25C.
- **I 42 mm non sono un problema.** In un'ogiva da bottiglia (lunga 80–120 mm) il
  vincolo non è mai la lunghezza ma larghezza e spessore, e 20 × 6 mm è sottilissimo.
  ⚠️ Da riverificare solo passando all'**ogiva stampata 3D** (§12).
- **Autonomia** 1,5–2,5 h con camera + WiFi; ricarica dalla XIAO ~9–10 h, cioè una
  notte. Si caricano tutte e 5 la sera prima e si ruotano in campo.
- ⚠️ **Promo in scadenza il 7 settembre** (listino 20,77 €), consegna stimata 23–29 set.

**Pigtail JST 1.25** — 5 paia M/F, 28 AWG, 15 cm. La XIAO ha solo i pad BAT nudi:
tagliare il connettore della cella e saldare i fili funziona, ma butta via la strategia
di rotazione. Si salda sui pad la **controparte** del connettore. Il kit contiene
entrambi i generi, quindi qualunque sia quello sulla cella si ha il suo mate. 28 AWG
regge oltre 1 A. **Accorciare i 15 cm a 3–4 cm.**

⚠️ **Verificare la polarità col multimetro prima del primo innesto.** Rosso = +,
nero = − è la convenzione ma non è garantita fra fornitori diversi. Sulla XIAO **BAT- è
il pad vicino alla USB-C**. Un'inversione distrugge la board.

✅ **Check alla consegna delle celle**: pesarne una (≤ 12 g) e misurarne la tensione a
vuoto (~3,8 V di storage charge). Sotto 3,0 V è da scartare.

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

```

---

## 15. Sensori opzionali: analisi e decisioni 🔬

### Stato delle decisioni — 4 settembre 2026

**Si vola con MPU6050 + BMP280** (entrambi già in inventario) e si valuta sui dati reali.

| Candidato | Costo | Decisione | Perché |
|---|---|---|---|
| ADXL345 | ~15 € / 5 pz | ❌ scartato | Satura a ±16 g come l'MPU6050, e senza giroscopio |
| BNO085 | ~40 € | ❌ scartato | ±8 g (peggio dell'MPU6050), fusione inutile su 5 s di volo |
| H3LIS331 / ADXL375 | ~36 € | ⏸️ rimandato | Unico modo di avere il picco vero, si decide dopo i primi voli |
| ICM-20649 | ~15–20 € | ⏸️ rimandato | Interessa più per il giroscopio ±4000 °/s che per i ±30 g |
| DPS310 | ~4 € | ⏸️ rimandato | Miglior rapporto valore/prezzo se il BMP280 si rivelasse limitante |

**Prima di comprare qualsiasi cosa, due azioni a costo zero**: i **fori di sfiato**
(vedi sotto, valgono più di qualsiasi upgrade) e l'uso del **microfono PDM** già a
bordo per il timing degli eventi.

### Il problema: il lancio fa ~60 g

Con i numeri del progetto (§4) e la spinta di un getto incomprimibile `F = 2·A·ΔP`:

```
Ugello (collo bottiglia)  A  = π·(0,011 m)²     = 3,8·10⁻⁴ m²
Pressione                 ΔP = 6 bar            = 6·10⁵ Pa
Spinta                    F  = 2·3,8e-4·6e5     ≈ 456 N
Massa al distacco         m  ≈ 0,75 kg
Picco                     a  = 456 / 0,75       ≈ 62 g
```

Circa **60 g nei primi millisecondi** (fino a ~85 g con meno acqua a bordo), che decadono
in fretta col calare della pressione. I ±16 g dell'MPU6050 vengono sfondati di quasi
quattro volte: sul log la spinta apparirà come un plateau tosato.

Per la missione questo **non è un problema**: il rilevamento dell'apogeo usa il *segno*
dell'accelerazione e il barometro, non il modulo. Diventa un problema solo se interessa
il numero in sé, che in chiave didattica è però uno dei più belli — "il razzo ha preso
60 g, un pilota di caccia ne regge 9".

### Cosa si ricava dal log senza comprare nulla

Si perde **soltanto il picco istantaneo**. Velocità e accelerazione *media* sono
entrambe recuperabili.

| Grandezza | | Come |
|---|---|---|
| Durata spinta `Δt` | ✅ misurata | Il log passa da saturo a caduta libera in modo netto |
| Velocità di burnout `Δv` | ✅ misurata (±10–15 %) | Barometro, tre strade — vedi sotto |
| **Accelerazione media** | ✅ misurata | `Δv / Δt` |
| Accelerazione post-burnout | ✅ misurata | Coast e discesa stanno in 0–2 g |
| Picco istantaneo | ❌ non misurato | Ma calcolabile da modello vincolato |

**Tre strade per `Δv`**, da incrociare perché sbagliano in direzioni note:

1. Dall'apogeo, `v² = 2·g·h_coast` — ignora la resistenza, quindi sottostima.
2. Dal tempo di salita, `v ≈ g·t_coast` — sottostima anche questa.
3. Dalla derivata del barometro subito dopo il burnout — la più diretta, ±5 m/s su ~35.

🎓 La strada elegante, e un bell'esercizio di laboratorio, è **fittare la fase di coast
con un modello di resistenza**: dà in un colpo solo la velocità di burnout *e* il
coefficiente di drag del razzo.

**Il picco si calcola, non si indovina.** `Δv` e `Δt` sono due misure che vincolano il
modello: note la pressione al lancio (si legge sulla pompa) e l'area dell'ugello, il
modello `F = 2·A·ΔP` deve riprodurle entrambe. Se ci riesce, la sua previsione del picco
è credibile. Un vincolo gratis in più: **l'integrale dell'accelerometro tosato è un
limite inferiore di `Δv`**, e la differenza rispetto al `Δv` barometrico misura l'area
tagliata via dalla saturazione.

⚠️ **Fori di sfiato — l'errore che conta più di ogni sensore.** A 35 m/s la pressione
dinamica vale `½·ρ·v² ≈ 735 Pa`, che il BMP280 legge come **~6 m di quota fantasma**:
sei volte il suo rumore. Rimedio standard della missilistica: fori piccoli, laterali, a
90° rispetto al flusso e lontani dall'ogiva, così la cabina vede pressione statica.

⚙️ **Requisito di log**: per avere `Δt` con precisione decente su una spinta di ~0,18 s
servono **200–500 Hz** durante il boost. A 100 Hz si hanno ~18 campioni: si sopravvive,
ma con margine risicato.

### Se un giorno si vuole il picco vero

Il breakout H3LIS331DL **esiste**; quello che non esiste è il modulo GY cinese da 6 €.
È un chip di nicchia senza cloni AliExpress diffusi, da qui il salto di prezzo.

| Modulo | Fondo scala | Dove | Prezzo |
|---|---|---|---|
| [Adafruit H3LIS331 #4627](https://www.adafruit.com/product/4627) | ±100 / 200 / 400 g selezionabile | [Robot Italy](https://robot-italy.com/), disponibile | 36,53 € |
| [Adafruit ADXL375 #5374](https://www.adafruit.com/product/5374) | ±200 g fisso | Robot Italy, disponibile | 36,53 € |
| [SparkFun SEN-14480](https://www.sparkfun.com/sparkfun-triple-axis-accelerometer-breakout-h3lis331dl.html) | ±100/200/400 g | rivenditori USA/EU | ~25–30 € |
| ~~ADXL377~~ analogico | — | EOL, Adafruit rimanda all'ADXL375 | — |

A parità di prezzo meglio l'**H3LIS331**: impostato a ±100 g dà il doppio della
risoluzione dell'ADXL375 nella finestra 60–85 g che interessa. L'ADXL375 ha in cambio un
vantaggio software, essendo compatibile a registri con l'ADXL345.

Nessuno dei due costa pin: I2C sul bus già cablato, indirizzi non collidenti
(H3LIS331 `0x18/0x19`, ADXL375 `0x53/0x1D`).

⚠️ **Diffidare degli "ADXL375" economici su AliExpress.** ADXL375 e ADXL345 sono quasi
identici a livello di registri: un ADXL345 rietichettato risponderebbe correttamente
all'interrogazione software e si scoprirebbe **solo in volo**, tosato a 16 g. Non è
verificabile da codice, solo con una prova d'urto reale.

### La fascia dei 10 €

A 10 € non si compra il picco a 60 g: quel fondo scala parte da ~25 €. Si compra
**precisione su ciò che si misura già**.

**DPS310 (~3–6 €) — il miglior rapporto valore/prezzo.** Il barometro è il collo di
bottiglia dell'intera catena: apogeo, velocità e accelerazione media derivano tutti
dalla curva di quota.

| | BMP280 (in inventario) | DPS310 |
|---|---|---|
| Risoluzione di pressione | 0,18 Pa | **0,06 Pa** |
| Accuratezza relativa | ±12 Pa ≈ ±1 m | ±2 cm rilevabili |
| Sensibilità a max oversampling | riferimento | **>10×** |
| Tecnologia | piezoresistiva | capacitiva, meno derive termiche |

I2C, nessun pin aggiuntivo. ⚠️ Indirizzo `0x76/0x77`, **lo stesso del BMP280**: o si
sostituisce, o si usa l'indirizzo alternativo via pin SDO. Il BMP280 è peraltro fuori
produzione; i 5 in inventario restano ottimi per lab e stazione a terra. Un gradino
sopra c'è il **BMP390** (~8–12 €), ±0,25 m e 200 Hz.

**ICM-20649 (~15–20 €).** Sopra i 10, ma risolve due limiti insieme: accelerometro
**±30 g** (non basta per i 60, ma trasforma "tosato per tutta la spinta" in "tosato solo
sulla cuspide") e giroscopio **±4000 °/s**. Quest'ultimo copre un rischio non ovvio:
l'MPU6050 si ferma a ±2000 °/s, cioè **333 giri/min**, e un razzo con pinne anche solo
leggermente disallineate può girare più veloce, saturando pure il dato di rollio.
Breakout [Adafruit #4464](https://www.adafruit.com/product/4464), $14,95.

**Gratis: il microfono PDM già a bordo.** A 16 kHz dà la cronologia degli eventi con
risoluzione sotto il millisecondo: lancio, burnout, apertura del paracadute, impatto.
È il modo più preciso ed economico di misurare il `Δt` della spinta, cioè metà del
calcolo dell'accelerazione media.

**Da non inseguire: il tubo di Pitot.** Sembra la risposta giusta, ma l'**MPXV7002DP**
(~8–12 €) vuole 5 V e restituisce 0,5–4,5 V analogici: a batteria il rail 5 V non
esiste e 4,5 V friggerebbero l'ADC a 3,3 V. Le versioni I2C a 3,3 V partono da 25 €
abbondanti.

### Perché il BNO085 è stato scartato

Il [BNO085](https://www.adafruit.com/product/4754) è una IMU 9-DOF con **fusione a
bordo**: un Cortex-M0+ con firmware SH-2 restituisce direttamente il quaternione di
orientamento assoluto, già filtrato. Accelerometro ±8 g, giroscopio ±2000 °/s,
magnetometro ±8 gauss, I2C/SPI/UART-RVC, ~35–45 € in Europa.

Ottimo sensore, problema sbagliato:

1. **±8 g è la metà dell'MPU6050** — sulla domanda del picco è un passo indietro.
2. **La fusione risolve un problema che questo volo non ha.** Il suo valore è l'assetto
   assoluto senza deriva su tempi lunghi; ma il volo dura **5 secondi**, e su quella
   finestra la semplice integrazione del giroscopio grezzo dà già un assetto ottimo.
3. **Il magnetometro è inutile e disturbato**: la calibrazione a otto non converge su un
   razzo, e a pochi centimetri c'è il buzzer 12095, che è **magnetico**. Si finirebbe a
   usare il *Game Rotation Vector*, cioè a disattivare il sensore in più che si è pagato.
4. **La fusione presume moti normali**: 60 g al lancio e un atterraggio duro violano le
   ipotesi, e il BNO08x è documentato come bisognoso di re-inizializzazione dopo shock
   forti — proprio nella fase che interessa.
5. **L'I2C del BNO08x è capriccioso** (protocollo SHTP, pacchetti lunghi, clock
   stretching). Andrebbe cablato in UART-RVC su D6/D7, robusto ma limitato a
   yaw/pitch/roll a 100 Hz.

È il sensore giusto per un drone o un visore AR. Qui, MPU6050 più integrazione del
giroscopio dà la stessa risposta per 4 €.

---

*Buoni lanci! 🚀*
