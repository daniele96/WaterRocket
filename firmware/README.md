# Firmware — XIAO ESP32S3 Sense

Progetto PlatformIO per il Seeed Studio XIAO ESP32S3 Sense (ESP32-S3, 8 MB flash,
8 MB PSRAM, camera OV2640/OV3660, microfono PDM, slot microSD).

## Setup

1. In VS Code installa l'estensione **pioarduino IDE** (`pioarduino.pioarduino-ide`).
   Non installare anche *PlatformIO IDE*: le due estensioni vanno in conflitto.
2. Apri la cartella `firmware/` come progetto (PIO Home → Open Project).
3. Al primo build vengono scaricati toolchain e core Arduino 3.x (qualche minuto).

## Registratore di volo

Il firmware registra **video** (camera, MJPEG) e **audio** (microfono PDM, WAV 16 kHz)
sulla microSD, in una cartella nuova per ogni sessione: `/REC_0001/`, `/REC_0002/`, ...

| File          | Contenuto                                              |
|---------------|--------------------------------------------------------|
| `video.mjpeg` | frame JPEG concatenati (SVGA 800×600, ~28 fps)          |
| `frames.csv`  | indice frame con timestamp in ms dall'inizio           |
| `audio.wav`   | audio mono 16 kHz / 16 bit                             |
| `info.txt`    | riepilogo: frame, durata, fps medi, errori             |

**Avvio/stop:** pulsante **BOOT**, oppure da monitor seriale `r` (avvia), `s` (ferma),
`i` (info), `l` (elenca file su SD), `h` (aiuto), `F` (formatta la SD in FAT32 se
non è già FAT, con conferma `Y`: utile se non hai un lettore di microSD). Per partire da soli all'accensione (sul razzo, senza PC)
imposta `AUTO_RECORD = true` in `src/main.cpp`.

I file vengono salvati su SD ogni ~1-2 s, quindi se l'alimentazione salta si perde
solo l'ultimo secondo.

**Requisiti SD:** formattata **FAT32** (exFAT non è supportato). Le schede da 64 GB in
su escono formattate exFAT: riformattale FAT32, ad esempio da prompt
`format X: /FS:FAT32 /Q` (Windows 11 recenti) oppure con *guiformat* / *Rufus*.

### Piattaforma web dei lanci (WiFi)

All'accensione lo XIAO crea la rete WiFi **`WaterRocket`** (password **`razzo1234`**).
Collegati con telefono o PC e apri **http://192.168.4.1** (o http://rocket.local).
Funziona senza internet (nessuna libreria esterna nella pagina).

Ogni registrazione è un **lancio**:

- **Elenco lanci**: schede con miniatura, data/ora, durata e parametri principali;
  avvio/stop di un nuovo lancio dall'header; anteprima camera; export di tutti i lanci
  in un'unica tabella CSV per confrontarli
- **Pagina del lancio**: player con audio, barra di scorrimento, avanzamento
  frame per frame (←/→, spazio) e rallentatore fino a 0.1×; parametri di lancio
  (bottiglia, acqua, pressione, massa, quota, luogo, note) salvati in `launch.json`;
  download del video con audio (.avi), dei dati del lancio (.json/.csv) e dei file grezzi
- **Dati sensori**: se nella cartella del lancio c'è `sensors.csv` (prima colonna =
  tempo in ms dall'inizio della registrazione, poi una colonna per grandezza, es.
  `t_ms,alt_m,acc_g`) ogni colonna diventa un grafico sincronizzato con il video

Data e ora vengono da internet (NTP) quando lo XIAO è collegato a una rete, altrimenti
dal browser al primo accesso alla pagina.

Il server HTTP è asincrono (ESPAsyncWebServer): serve più richieste in parallelo
(stato, miniature, download) mentre la registrazione continua sui task dedicati.

**Collegarsi a una rete esistente** (es. hotspot del telefono): copia
`include/secrets.example.h` in `include/secrets.h` (escluso da git) e inserisci nome
e password. Lo XIAO si collega a quella rete *e* tiene attiva `WaterRocket`; l'IP
ottenuto viene stampato sulla seriale (comando `i`).

SSID e password della rete `WaterRocket` sono in `src/web_server.cpp`; per spegnere il WiFi imposta
`WIFI_ENABLED = false` in `src/main.cpp`.

**Conversione in MP4** (sul PC, con ffmpeg; il valore di fps è in `info.txt`):

```sh
ffmpeg -framerate 15 -i video.mjpeg -i audio.wav -c:v libx264 -pix_fmt yuv420p -c:a aac -shortest volo.mp4
```

`video.mjpeg` si apre anche direttamente con VLC.

## Comandi

```sh
pio run                    # compila
pio run -t upload          # flash via USB-C
pio device monitor         # monitor seriale (115200)
pio run -t upload -t monitor
```

## Se l'upload non va

Tieni premuto **BOOT**, premi e rilascia **RESET**, poi rilascia **BOOT**: la scheda
entra in modalità download. Dopo il flash premi RESET.

## Pin utili (scheda Sense)

| Funzione         | GPIO                          |
|------------------|-------------------------------|
| LED utente       | 21 (attivo basso)             |
| microSD (SPI)    | CS 21, SCK 7, MISO 8, MOSI 9  |
| I2C (D4/D5)      | SDA 5, SCL 6                  |
| Microfono PDM    | CLK 42, DATA 41               |

⚠️ GPIO21 è condiviso tra LED e CS della microSD.
