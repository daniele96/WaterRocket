# Firmware «istruzione» 🎓

Piccole demo dei sensori del razzo, **una alla volta**, sul monitor seriale. Si sceglie la demo
con un tasto e la scheda stampa solo quella: come si parla col sensore e cosa risponde.

È un firmware a parte, non una modalità di quello di volo: il codice va **mostrato** ai ragazzi,
e quello di volo (task, code, mutex) non si legge se si parte da zero. Qui ogni demo è un file
corto, commentato in italiano, con il suo `...Inizio()` e il suo `...Ripeti()`: gli stessi
`setup()` e `loop()` di Arduino. Si flasha sulla stessa XIAO del razzo, già cablata
([cablaggio.md](../cablaggio.md)); per tornare al volo si riflasha [../firmware](../firmware).

## Flash e monitor

Da PowerShell (non da Git Bash), nella cartella `firmware-istruzione/`:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run -t upload -t monitor
```

oppure dai pulsanti di pioarduino in VS Code, aprendo questa cartella come progetto. Se l'upload
non parte: tieni premuto **BOOT**, premi e rilascia **RESET**, rilascia **BOOT**.

`pio` non è nel PATH di Windows: da solo il comando non viene trovato. Solo monitor, dal prompt
dei comandi (`cmd`), con la porta che dà `pio device list`:

```bat
"%USERPROFILE%\.platformio\penv\Scripts\pio.exe" device monitor -p COM7 -b 115200
```

I tasti partono subito, senza Invio; si esce con `Ctrl+C`. Il monitor va chiuso prima di ogni flash.

Per **tornare al firmware di volo**: stesso comando nella cartella `firmware/`.

## Il menu

| Tasto | Demo | File da mostrare | Cosa far provare |
|---|---|---|---|
| `1` | Chi c'è sul bus I²C | `demo_1_scanner.cpp` | L'appello: rispondono 0x68 e 0x77. Si stacca un filo e si rifà |
| `2` | Barometro | `demo_2_barometro.cpp` | Chi sei (0xD0), gli 11 numeri di taratura, poi alzare la scheda dal pavimento al tavolo: quota grezza e filtrata. Un dito caldo vicino al sensore: sale la temperatura |
| `3` | Accelerometro | `mpu6050.cpp` e `demo_3_accelerometro.cpp` | Piatta = 1 g su Z, sul fianco, capovolta; farla cadere su un cuscino: 0 g |
| `4` | Giroscopio | `demo_4_giroscopio.cpp` | Ruotarla di 90° sul tavolo e leggere l'angolo; lasciarla ferma e vedere l'angolo «scivolare» (deriva) |
| `5` | Cicalino | `demo_5_cicalino.cpp` | Il bit 1 suona, il bit 0 tace |
| `6` | Anello LED | `demo_6_anello.cpp` | I colori sommati e i 24 bit di ogni LED; poi un LED alla volta |
| `7` | Microfono | `demo_7_microfono.cpp` | Parlare, fischiare, battere le mani: il volume in decibel |
| `8` | Radio WiFi | `demo_8_wifi.cpp` | Coprire l'antenna con la mano o con una bottiglia d'acqua e guardare i dBm scendere |
| `9` | Livella | `demo_9_livella.cpp` | Sento, decido, agisco: inclinare la scheda, la luce rotola verso il basso |
| `0` | Decollo | `demo_0_decollo.cpp` | La macchina a stati del razzo: alzarla di scatto (DECOLLO), appoggiarla, dopo 3 s lampo e bip |

Altri tasti: **`m`** ferma la demo e torna al menu, **`g`** accende o spegne il formato per i
grafici. Si può passare da una demo all'altra direttamente col suo numero.

Il codice che mostra **la conversazione I²C** vera, la stessa delle slide («0x68, cassetto 0x3B»
→ «ecco 14 byte»), è `mpuLeggi()` in [src/mpu6050.cpp](src/mpu6050.cpp). Le funzioni
`scriviRegistro()` e `leggiRegistri()` in [src/aiuto.cpp](src/aiuto.cpp) fanno lo stesso per gli
altri sensori. La «ricetta» Bosch del barometro (dai numeri grezzi ai pascal) sta a parte, in
[src/barometro_ricetta.cpp](src/barometro_ricetta.cpp): sono le formule del datasheet, le stesse
del firmware di volo.

## Uscita in testo e grafici

Di base ogni misura è una riga di testo con una barretta, leggibile su qualunque monitor seriale
e sul proiettore:

```
grezzo   85023 → pressione 100482 Pa   temperatura   20.7 °C   quota    0.51 m   filtrata    0.39 m   [--------|##-------------]
```

Con **`g`** ogni valore diventa una riga `>nome:valore`, il formato di **Teleplot** (estensione di
VS Code `alexnesnes.teleplot`), che disegna un grafico per ogni grandezza. Bello col barometro
(quota grezza e filtrata una sopra l'altra, come nella slide dei filtri) e col decollo
(accelerazione e stato). Teleplot apre la porta seriale da solo: chiudi prima il monitor di
PlatformIO. Per cambiare demo si manda il tasto dalla casella di invio di Teleplot, oppure si
chiude Teleplot e si riapre il monitor.

## Note

- **Barometro**: riconosce da solo BMP180 (quello del razzo) e BMP280, a 0x77 o 0x76. Col BMP280
  il filtro interno è spento apposta: si vede il rumore e si vede il nostro filtro che lo toglie.
- **Accelerometro**: il nostro MPU6050 è un clone (al «chi sei?» risponde 0x74 invece di 0x68)
  e da fermo legge **1,32 g** invece di 1. Le demo 3, 9 e 0 cominciano quindi con 1 secondo di
  taratura, come il razzo sulla rampa, e lo dicono a schermo: «dovrebbe leggere 1,00 g e invece
  legge 1,32 g». È un ottimo esempio dal vivo della slide sulla taratura.
- **Anello**: luminosità bassa (40 su 255) perché sul banco va tutto dalla USB. `anello.begin()`
  si chiama una volta sola, in `setup()`: ripeterlo stacca il pin dalla periferica RMT e l'anello
  si spegne fino al riavvio.
- **Livella**: la luce dipende da come sono montati anello e MPU6050. Se va dalla parte
  sbagliata, cambia `ROTAZIONE` (0–7) o `VERSO` (1 o −1) in cima a `demo_9_livella.cpp`.
- **Decollo**: soglie da mano (1,5 g per 2 letture), più basse di quelle del razzo (2,5 g per 4):
  il commento nel file lo dice, ed è un buon spunto («perché sul razzo sono più alte?»).
- **Microfono**: serve la scheda Sense con la camera montata (il microfono è lì).
- **WiFi**: serve l'antenna innestata. Mostra i nomi delle reti intorno: in un posto pubblico
  sono quelli che vede qualunque telefono.
