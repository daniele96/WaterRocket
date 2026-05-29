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

*Buoni lanci! 🚀*
```
