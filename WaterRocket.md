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
| Camera | OV2640 2 MP (staccabile, opz. OV5640) |
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
| **Buzzer passivo** | GPIO PWM | Beep ritrovamento + codici di stato | ⭐⭐ |
| **LED ultra-bright** | GPIO | Visibilità slow-mo, stato armato/disarmato | ⭐ |
| **Microfono PDM** | I2S | Acustica del getto d'acqua, evento apertura paracadute | ⭐ |
| **RTC DS3231** (lab) | I2C | Timestamp assoluti, sincronizzazione multi-razzo | ⭐ |
| **BH1750 / TSL2591** (lab) | I2C | Profilo luminosità lungo la traiettoria (didattico) | ⭐ |
| **GPS NEO-M8N** | UART | Ritrovamento a distanza (≥70 m). Costa ~20 g | ⭐ opz |
| **ADXL375 ±200 g** | I2C | Vero picco accel lancio + impatto (MPU6050 satura a ±16 g) | ⭐ opz |

### Schema collegamenti minimo

```
XIAO ESP32-S3 Sense
├── I2C (SDA/SCL)
│   ├── BMP280   (addr 0x76 o 0x77)
│   └── MPU6050  (addr 0x68)
├── microSD       → slot integrato (shield camera)
├── Camera OV2640 → FPC dedicato
├── BAT+ / BAT-   → LiPo 1S 300–400 mAh (saldatura)
├── GPIO          → buzzer passivo + LED
└── USB-C         → programmazione + ricarica
```

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

### Da acquistare (kit volante elettronica) — ~71 €

| # | Articolo | Q.tà | € | Link |
|---|---|---|---|---|
| 1 | **Seeed XIAO ESP32-S3 Sense** | 2 | 34 | [Amazon.it ✓](https://www.amazon.it/Seeed-Studio-XIAO-ESP32S3-Sense/dp/B0C69FFVHH) |
| 2 | **LiPo 1S 300–400 mAh JST 1.25** | 2 | 12 | [Amazon.it](https://www.amazon.it/s?k=LiPo+1S+400mAh+JST+1.25) · [AliExpress](https://it.aliexpress.com/w/wholesale-lipo-1s-400mah-jst-1.25.html) |
| 3 | **MPU6050 / GY-521** | 2 | 8 | [Amazon.it](https://www.amazon.it/s?k=GY-521+MPU6050) · [AliExpress](https://it.aliexpress.com/w/wholesale-gy-521-mpu6050.html) |
| 4 | **Buzzer passivo 5V** (multipack) | 1 kit | 6 | [Amazon.it](https://www.amazon.it/s?k=buzzer+passivo+5V+arduino) |
| 5 | **Antenna IPEX 2.4 GHz** (opzionale) | 2 | 6 | [Amazon.it](https://www.amazon.it/s?k=antenna+IPEX+UFL+2.4GHz) · [Seeed ufficiale](https://www.seeedstudio.com/2-4GHz-2-81dBi-Antenna-for-XIAO-ESP32C3-p-5475.html) |
| 6 | **LED + resistenze** (se mancanti) | 1 kit | 5 | [Amazon.it](https://www.amazon.it/s?k=led+kit+5mm+arduino) |
| | | **Totale** | **~71 €** | |

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
