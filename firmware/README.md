# Firmware — XIAO ESP32S3 Sense

Progetto PlatformIO per il Seeed Studio XIAO ESP32S3 Sense (ESP32-S3, 8 MB flash,
8 MB PSRAM, camera OV2640/OV3660, microfono PDM, slot microSD).

## Setup

1. In VS Code installa l'estensione **pioarduino IDE** (`pioarduino.pioarduino-ide`).
   Non installare anche *PlatformIO IDE*: le due estensioni vanno in conflitto.
2. Apri la cartella `firmware/` come progetto (PIO Home → Open Project).
3. Al primo build vengono scaricati toolchain e core Arduino 3.x (qualche minuto).

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
