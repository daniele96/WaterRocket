"""Genera montaggio.html: l'animazione passo passo del montaggio della cintura.

Esporta da OpenSCAD ogni pezzo nella sua posizione di montaggio, con le misure di
default di cintura.scad, e li inserisce in montaggio_modello.html. La pagina che ne
esce si apre col browser (serve internet: scarica three.js).

Va rilanciato dopo ogni modifica al modello:   python montaggio.py
OpenSCAD si cerca in C:\\Program Files\\OpenSCAD (Nightly); altrimenti indicalo con
la variabile d'ambiente OPENSCAD.
"""
import base64
import json
import os
import struct
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

QUI = Path(__file__).resolve().parent
SCAD = QUI / 'cintura.scad'
MODELLO = QUI / 'montaggio_modello.html'
USCITA = QUI / 'montaggio.html'
OPENSCAD = os.environ.get('OPENSCAD', r'C:\Program Files\OpenSCAD (Nightly)\openscad.com')

# nome: (codice OpenSCAD, colore, gruppo). Gruppi: el = pod elettronica e ciò che
# contiene, bat = pod batteria, mondo = riferimento della bottiglia
PEZZI = {
    'pod_el': ('pod("elettronica");', '#f5a524', 'el'),
    'slitta': ('in_scheda() slitta();', '#e2bf2a', 'el'),
    'xiao_base': ('in_scheda() xiao_base();', '#59606b', 'el'),
    'xiao_sense': ('in_scheda() xiao_sense();', '#33363c', 'el'),
    'cavo': ('cavetto();', '#121212', 'el'),
    'antenna': ('antenna();', '#b8763a', 'el'),
    'perni': ('perni();', '#f4f4f4', 'el'),
    'mpu': ('mpu6050();', '#3466d6', 'el'),
    'bmp': ('bmp180();', '#7a52c7', 'el'),
    'cicalino': ('cicalino();', '#2a2c30', 'el'),
    'anello': ('anello();', '#161616', 'el'),
    'pod_bat': ('pod("batteria");', '#f5a524', 'bat'),
    'batteria': ('batteria();', '#c9ccd1', 'bat'),
    'elastico': ('elastico();', '#fafafa', 'mondo'),
}

DATI = 'echo(str("DATI_MONTAGGIO=", [R, xa(zb(W / 2)) - 40, X_NB + 40, E, D]));'


def openscad(codice, uscita, cartella):
    sorgente = Path(cartella) / (Path(uscita).stem + '.scad')
    sorgente.write_text(f'include <{SCAD.as_posix()}>\n{codice}\n', encoding='utf-8')
    cmd = [OPENSCAD, '--backend=manifold', '-o', str(uscita), '-D', 'parte="nessuna"', str(sorgente)]
    if str(uscita).endswith('.stl'):
        cmd[1:1] = ['--export-format', 'binstl']
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit(f'OpenSCAD non è riuscito a generare {Path(uscita).name}:\n{r.stderr[-2000:]}')


def leggi_stl(file):
    """STL binario -> vertici unici (float32) e indici dei triangoli (uint32)."""
    dati = Path(file).read_bytes()
    n = struct.unpack_from('<I', dati, 80)[0]
    vertici, indici, posizioni = {}, [], []
    for i in range(n):
        base = 84 + i * 50 + 12
        for k in range(3):
            v = struct.unpack_from('<3f', dati, base + 12 * k)
            chiave = tuple(round(c * 100) for c in v)
            if chiave not in vertici:
                vertici[chiave] = len(vertici)
                posizioni.extend(c / 100 for c in chiave)
            indici.append(vertici[chiave])
    return posizioni, indici, n


def b64(formato, valori):
    return base64.b64encode(struct.pack(f'<{len(valori)}{formato}', *valori)).decode('ascii')


def main():
    if not Path(OPENSCAD).exists():
        sys.exit(f'OpenSCAD non trovato in {OPENSCAD}: indicalo con la variabile OPENSCAD')
    with tempfile.TemporaryDirectory() as cartella:
        lavori = {nome: (cod, Path(cartella) / f'{nome}.stl') for nome, (cod, _, _) in PEZZI.items()}
        echo = Path(cartella) / 'dati.echo'
        with ThreadPoolExecutor(max_workers=os.cpu_count() or 4) as pool:
            attesa = [pool.submit(openscad, cod, stl, cartella) for cod, stl in lavori.values()]
            attesa.append(pool.submit(openscad, DATI, echo, cartella))
            for a in attesa:
                a.result()
        riga = next(r for r in echo.read_text(encoding='utf-8').splitlines() if 'DATI_MONTAGGIO=' in r)
        r, bx0, bx1, e, d = json.loads(riga.split('DATI_MONTAGGIO=')[1].rstrip('"'))
        pezzi = {}
        for nome, (_, colore, gruppo) in PEZZI.items():
            pos, idx, n = leggi_stl(lavori[nome][1])
            pezzi[nome] = {'pos': b64('f', pos), 'idx': b64('I', idx), 'col': colore, 'gruppo': gruppo}
            print(f'  {nome:<11} {n:>6} triangoli')
    dati = {'dati': {'R': r, 'bottiglia': [bx0, bx1], 'E': e, 'D': d}, 'pezzi': pezzi}
    pagina = MODELLO.read_text(encoding='utf-8').replace('/*DATI*/null', json.dumps(dati, separators=(',', ':')))
    USCITA.write_text(pagina, encoding='utf-8')
    print(f'Scritto {USCITA.name} ({USCITA.stat().st_size / 1024:.0f} kB)')


if __name__ == '__main__':
    main()
