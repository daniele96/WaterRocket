# Slide — Meet and Code 2026 🎤

Presentazione del progetto per ragazzi dai 15 ai 24 anni, anche senza nessuna esperienza di
programmazione: più principi fisici che dettagli tecnici. È scritta in
[Marp](https://marp.app/), cioè in Markdown: [water-rocket.md](water-rocket.md).

## Percorso

| # | Capitolo | Cosa si spiega |
|---|---|---|
| 1 | Il razzo | azione e reazione, i numeri del volo, sicurezza, la cintura portaelettronica |
| 2 | Il cervello | cos'è un microcontrollore, bit e byte, famiglia ESP32, XIAO ESP32-S3 Sense, WiFi, camera |
| 3 | Quanto in alto? | pressione dell'aria, membrana piezoresistiva del BMP180, formula della quota, filtri |
| 4 | Come parlano i chip | bus I²C: SDA e SCL, indirizzi, diagramma dei bit, pull-up, registri |
| 5 | Come si muove? | accelerometro MEMS, giroscopio (Coriolis), fondo scala, macchina a stati del volo, taratura |
| 6 | Farsi trovare | cicalino magnetico attivo, LED RGB, protocollo a un filo del WS2812B |
| 7 | Tutto insieme | schema a blocchi, confronto dei modi di comunicare, profilo di volo atteso |

58 slide, circa un'ora con le domande. Ogni slide ha le **note del relatore** (i commenti
`<!-- ... -->`): cosa dire, analogie, risposte alle domande probabili.

**Demo dal vivo:** nelle note, «DEMO n» indica il tasto del
[firmware «istruzione»](../firmware-istruzione/README.md) da premere in quel punto (scanner
I²C, barometro, accelerometro, giroscopio, cicalino, anello, microfono, WiFi, livella, decollo).
Va flashato sulla scheda prima dell'evento.

**Versione corta (~35 min):** si possono togliere *Computer o microcontrollore?*, *Quanto è
veloce?*, *La camera*, *Trappole da ingegneri*, *Il trucco: nessuno spinge*, *MEMS*, *Prima del
lancio: la taratura*, *Un filo solo, senza orologio* e *Un secondo di volo*: il filo del
discorso regge lo stesso.

## Vedere e presentare

- **VS Code**: estensione *Marp for VS Code* (`marp-team.marp-vscode`), poi l'anteprima del file
  `.md`. Non serve nessuna impostazione: il deck non usa HTML.
- **Presentare dal browser** con le note del relatore:

  ```sh
  marp water-rocket.md -o water-rocket.html --allow-local-files
  ```

  Si apre l'HTML, `F` per lo schermo intero, `P` per la vista relatore con le note.
  L'HTML va tenuto nella cartella `slide/` (le immagini sono file accanto, anche in
  `../3dModels/cintura/img/`).
- **PDF** (un file solo, con le immagini incorporate, da portare su chiavetta):

  ```sh
  marp water-rocket.md --pdf --allow-local-files
  ```

  Con `--pdf-notes` le note diventano annotazioni del PDF.
- **PowerPoint**: `marp water-rocket.md --pptx --allow-local-files` (ogni slide è un'immagine,
  non modificabile).

Marp CLI si installa con `npm install -g @marp-team/marp-cli`.

## Immagini

- `img/*.svg`: schemi disegnati per questa presentazione. Si modificano con un editor di testo
  o con Inkscape.
- I grafici con dati (`filtro`, `saturazione`, `profilo-volo`, `i2c-tempi`, `colonna-aria`) li
  genera [img/genera-grafici.js](img/genera-grafici.js): `node img/genera-grafici.js`.
- `img/foto/`: foto da Wikimedia Commons con licenza libera. Autori, licenze e link in
  [img/foto/CREDITS.md](img/foto/CREDITS.md); l'attribuzione è anche in basso su ogni slide
  che le usa e nell'ultima slide. Due foto non sono il componente esatto del razzo: il modulo
  di pressione è un **BMP280** (non esiste una foto libera del BMP180) e l'immagine al
  microscopio è un **giroscopio** MEMS. Il testo delle slide lo dice.

## Da aggiungere quando ci saranno

- Uno **screenshot della pagina web** del lancio (slide *Dal razzo al telefono*), o una demo
  dal vivo con la scheda accesa.
- Il **primo volo vero**: video e grafici reali al posto del profilo atteso (slide *Cosa ci
  aspettiamo di vedere*) e conferma dei numeri stimati (~60 g, ~50 m).
