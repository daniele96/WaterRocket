// Genera gli SVG «con dati» delle slide (grafici e diagrammi temporali).
// Uso: node genera-grafici.js  (riscrive filtro, saturazione, profilo-volo, i2c-tempi, colonna-aria)
const fs = require('fs');
const OUT = __dirname;


const FONT = 'Segoe UI, Helvetica, Arial, sans-serif';
const C = {
  navy: '#0b1d3a', ink: '#1f2937', muted: '#5b6475', orange: '#ff7a1a', orangeD: '#e8630a',
  blue: '#1d6fd8', amber: '#c98a00', green: '#2a9d50', red: '#d64545', grid: '#d5dbe3',
  lblue: '#e3f0ff', lorange: '#fff1e6', lgreen: '#e6f6ec', lgray: '#eef1f5',
};
const f = (n) => +n.toFixed(1);
const svg = (w, h, title, body) =>
  `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${w} ${h}" font-family="${FONT}">\n` +
  `  <title>${title}</title>\n${body}</svg>\n`;
const save = (name, s) => { fs.writeFileSync(`${OUT}/${name}`, s); console.log('scritto', name); };

function rng(seed) {
  let s = seed >>> 0;
  return () => { s = (s * 1664525 + 1013904223) >>> 0; return s / 4294967296; };
}
function gauss(r) {
  let u = 0; while (u === 0) u = r();
  return Math.sqrt(-2 * Math.log(u)) * Math.cos(2 * Math.PI * r());
}
const it = (n) => String(n).replace('.', ',');

function axes({ x0, y0, x1, y1, xd, yd, xticks, yticks, xfmt, yfmt, xlabel, ylabel, zeroLine }) {
  const X = (v) => x0 + ((v - xd[0]) / (xd[1] - xd[0])) * (x1 - x0);
  const Y = (v) => y1 - ((v - yd[0]) / (yd[1] - yd[0])) * (y1 - y0);
  let s = '';
  for (const t of yticks) {
    s += `  <line x1="${x0}" y1="${f(Y(t))}" x2="${x1}" y2="${f(Y(t))}" stroke="${C.grid}" stroke-width="1.5"/>\n`;
    s += `  <text x="${x0 - 12}" y="${f(Y(t) + 7)}" text-anchor="end" font-size="20" fill="${C.muted}">${yfmt(t)}</text>\n`;
  }
  for (const t of xticks) {
    s += `  <line x1="${f(X(t))}" y1="${y1}" x2="${f(X(t))}" y2="${y1 + 8}" stroke="${C.ink}" stroke-width="2"/>\n`;
    s += `  <text x="${f(X(t))}" y="${y1 + 34}" text-anchor="middle" font-size="20" fill="${C.muted}">${xfmt(t)}</text>\n`;
  }
  const yAxis = zeroLine ? Y(0) : y1;
  s += `  <line x1="${x0}" y1="${y1}" x2="${x1}" y2="${y1}" stroke="${C.ink}" stroke-width="2"/>\n`;
  if (zeroLine) s += `  <line x1="${x0}" y1="${f(yAxis)}" x2="${x1}" y2="${f(yAxis)}" stroke="${C.muted}" stroke-width="1.5"/>\n`;
  s += `  <line x1="${x0}" y1="${y0 - 10}" x2="${x0}" y2="${y1}" stroke="${C.ink}" stroke-width="2"/>\n`;
  s += `  <text x="${x1}" y="${y1 + 66}" text-anchor="end" font-size="20" fill="${C.muted}">${xlabel}</text>\n`;
  s += `  <text x="${x0 - 12}" y="${y0 - 22}" text-anchor="start" font-size="20" font-weight="700" fill="${C.ink}">${ylabel}</text>\n`;
  return { X, Y, s };
}
const poly = (pts, X, Y) => pts.map(([t, v], i) => `${i ? 'L' : 'M'}${f(X(t))},${f(Y(v))}`).join(' ');

// ------------------------------------------------------------------ filtro.svg
{
  const r = rng(7);
  // 100 Hz come sul razzo; nel grafico i punti grezzi sono uno ogni 4 (25 Hz)
  const n = 600, dt = 6 / n;
  const truth = (t) => (t < 3 ? 0 : t < 3.4 ? ((t - 3) / 0.4) * 0.735 : 0.735);
  const all = [];
  for (let i = 0; i < n; i++) {
    const t = i * dt;
    all.push([t, truth(t) + gauss(r) * 0.3]);
  }
  const outlAll = [152, 244, 448];
  for (const i of outlAll) all[i][1] = -0.92;
  // mediana di 5 + media che insegue (alfa 0,25), come in baro.cpp
  const filt = []; const win = []; let y = NaN;
  for (const [t, v] of all) {
    win.push(v); if (win.length > 5) win.shift();
    const med = [...win].sort((a, b) => a - b)[Math.floor(win.length / 2)];
    y = isNaN(y) ? med : y + (med - y) * 0.25;
    filt.push([t, y]);
  }
  const raw = all.filter((_, i) => i % 4 === 0);
  const outl = outlAll.map((i) => i / 4);
  const a = axes({
    x0: 120, y0: 70, x1: 1070, y1: 350, xd: [0, 6], yd: [-1, 1.5],
    xticks: [0, 1, 2, 3, 4, 5, 6], yticks: [-0.5, 0, 0.5, 1],
    xfmt: (t) => `${t} s`, yfmt: (v) => `${it(v)} m`, xlabel: 'tempo', ylabel: 'quota letta', zeroLine: false,
  });
  let b = a.s;
  for (const [i, [t, v]] of raw.entries()) {
    if (outl.includes(i)) continue;
    b += `  <circle cx="${f(a.X(t))}" cy="${f(a.Y(v))}" r="4.5" fill="#9aa5b4"/>\n`;
  }
  for (const i of outl) {
    const [t] = raw[i];
    b += `  <circle cx="${f(a.X(t))}" cy="${f(a.Y(-0.92))}" r="7" fill="${C.red}"/>\n`;
    b += `  <path d="M${f(a.X(t))},${f(a.Y(-0.92) + 10)} v14 m-6,-6 l6,6 l6,-6" stroke="${C.red}" stroke-width="2.5" fill="none"/>\n`;
  }
  b += `  <path d="${poly(filt.filter((_, i) => i % 2 === 0), a.X, a.Y)}" fill="none" stroke="${C.orange}" stroke-width="5" stroke-linejoin="round"/>\n`;
  // legenda
  b += `  <circle cx="300" cy="28" r="6" fill="#9aa5b4"/><text x="314" y="35" font-size="20" fill="${C.ink}">misura grezza</text>\n`;
  b += `  <circle cx="480" cy="28" r="7" fill="${C.red}"/><text x="494" y="35" font-size="20" fill="${C.ink}">lettura «pazza» (anche −9 m!)</text>\n`;
  b += `  <line x1="790" y1="28" x2="830" y2="28" stroke="${C.orange}" stroke-width="5"/><text x="840" y="35" font-size="20" fill="${C.ink}">dopo il filtro</text>\n`;
  b += `  <text x="${f(a.X(4.7))}" y="${f(a.Y(1.36))}" text-anchor="middle" font-size="20" fill="${C.ink}">scheda alzata di 73,5 cm</text>\n`;
  b += `  <text x="${f(a.X(1.5))}" y="${f(a.Y(0.8))}" text-anchor="middle" font-size="20" fill="${C.ink}">ferma sul pavimento</text>\n`;
  b += `  <text x="120" y="425" font-size="18" fill="${C.muted}" font-style="italic">esempio illustrativo, con il rumore misurato sul banco</text>\n`;
  save('filtro.svg', svg(1100, 440, 'Misure grezze del barometro e misura filtrata', b));
}

// ------------------------------------------------------------- saturazione.svg
{
  const truth = (t) => {
    if (t < 0) return 1;
    if (t < 0.005) return 1 + (t / 0.005) * 61;
    if (t < 0.18) return 62 * Math.exp(-(t - 0.005) / 0.058);
    return -1.4 + (t - 0.18) * 1.6;
  };
  const pts = [];
  for (let t = -0.05; t <= 0.5001; t += 0.001) pts.push([t, truth(t)]);
  const meas = pts.map(([t, v]) => [t, Math.max(-16, Math.min(16, v))]);
  const a = axes({
    x0: 120, y0: 70, x1: 1070, y1: 360, xd: [-0.05, 0.5], yd: [-5, 68],
    xticks: [0, 0.1, 0.2, 0.3, 0.4, 0.5], yticks: [0, 20, 40, 60],
    xfmt: (t) => `${it(t)} s`, yfmt: (v) => `${v} g`, xlabel: 'tempo dal decollo', ylabel: 'accelerazione', zeroLine: false,
  });
  let b = a.s;
  const xb = a.X(0.18);
  b += `  <rect x="${f(a.X(0))}" y="70" width="${f(xb - a.X(0))}" height="290" fill="${C.lorange}"/>\n`;
  b += `  <text x="${f((a.X(0) + xb) / 2)}" y="${f(a.Y(-3.2))}" text-anchor="middle" font-size="18" fill="${C.orangeD}" font-weight="700">spinta</text>\n`;
  b += `  <line x1="120" y1="${f(a.Y(16))}" x2="1070" y2="${f(a.Y(16))}" stroke="${C.red}" stroke-width="2.5" stroke-dasharray="10 7"/>\n`;
  b += `  <text x="1064" y="${f(a.Y(16) - 10)}" text-anchor="end" font-size="20" font-weight="700" fill="${C.red}">limite del sensore: 16 g</text>\n`;
  b += `  <path d="${poly(pts, a.X, a.Y)}" fill="none" stroke="#7d8aa0" stroke-width="3" stroke-dasharray="8 6"/>\n`;
  b += `  <path d="${poly(meas, a.X, a.Y)}" fill="none" stroke="${C.orange}" stroke-width="5" stroke-linejoin="round"/>\n`;
  b += `  <text x="${f(a.X(0.012))}" y="${f(a.Y(62) + 6)}" font-size="20" fill="${C.ink}">~60 g veri (stimati con la fisica)</text>\n`;
  b += `  <text x="${f(a.X(0.2))}" y="${f(a.Y(29))}" font-size="20" fill="${C.ink}">il sensore legge solo 16 g:</text>\n`;
  b += `  <text x="${f(a.X(0.2))}" y="${f(a.Y(29) + 26)}" font-size="20" fill="${C.ink}">la cima del grafico è «tagliata»</text>\n`;
  b += `  <line x1="${f(a.X(0.035))}" y1="${f(a.Y(16) - 2)}" x2="${f(a.X(0.195))}" y2="${f(a.Y(25))}" stroke="${C.ink}" stroke-width="1.5"/>\n`;
  b += `  <text x="${f(a.X(-0.045))}" y="${f(a.Y(1) - 12)}" font-size="16" fill="${C.muted}">1 g sulla rampa</text>\n`;
  // legenda
  b += `  <line x1="520" y1="28" x2="560" y2="28" stroke="#7d8aa0" stroke-width="3" stroke-dasharray="8 6"/><text x="570" y="35" font-size="20" fill="${C.ink}">realtà</text>\n`;
  b += `  <line x1="680" y1="28" x2="720" y2="28" stroke="${C.orange}" stroke-width="5"/><text x="730" y="35" font-size="20" fill="${C.ink}">cosa registra l'MPU6050</text>\n`;
  b += `  <text x="120" y="430" font-size="18" fill="${C.muted}" font-style="italic">curva stimata dal modello della spinta, non ancora misurata in volo</text>\n`;
  save('saturazione.svg', svg(1100, 440, 'Saturazione dell\'accelerometro al decollo', b));
}

// ------------------------------------------------------------ profilo-volo.svg
{
  const g = 9.81, k = 0.0062; // resistenza dell'aria (1/m), tarata per un apogeo di ~50 m
  let t = 0, h = 0, v = 0; const pts = [[0, 0]];
  const dt = 0.002;
  // spinta: 0,2 s fino a 35 m/s
  while (t < 0.2) { v += (35 / 0.2) * dt; h += v * dt; t += dt; pts.push([t, h]); }
  while (v > 0) { v += (-g - k * v * v) * dt; h += v * dt; t += dt; pts.push([t, h]); }
  const tApo = t, hApo = h;
  // niente paracadute: dall'apogeo cade, frenato solo dalla resistenza dell'aria
  while (h > 0) { v += (-g - k * v * Math.abs(v)) * dt; h += v * dt; t += dt; pts.push([t, Math.max(h, 0)]); }
  const tLand = t, vImpact = -v;
  const tStill = tLand + 3; // il firmware lo dà per atterrato dopo 3 s di immobilità
  const sparse = pts.filter((_, i) => i % 10 === 0).concat([[tLand, 0], [10, 0]]);
  const a = axes({
    x0: 120, y0: 80, x1: 1070, y1: 360, xd: [0, 10], yd: [0, 60],
    xticks: [0, 2, 4, 6, 8, 10], yticks: [0, 20, 40, 60],
    xfmt: (x) => `${x} s`, yfmt: (y) => `${y} m`, xlabel: 'tempo', ylabel: 'quota', zeroLine: false,
  });
  let b = '';
  b += `  <rect x="${f(a.X(0))}" y="80" width="${f(a.X(0.2) - a.X(0))}" height="280" fill="${C.orange}" opacity="0.35"/>\n`;
  b += `  <rect x="${f(a.X(0.2))}" y="80" width="${f(a.X(tApo) - a.X(0.2))}" height="280" fill="${C.lblue}"/>\n`;
  b += `  <rect x="${f(a.X(tApo))}" y="80" width="${f(a.X(tLand) - a.X(tApo))}" height="280" fill="#fdeaea"/>\n`;
  b += `  <rect x="${f(a.X(tLand))}" y="80" width="${f(a.X(10) - a.X(tLand))}" height="280" fill="${C.lgreen}"/>\n`;
  b += a.s;
  b += `  <path d="${poly(sparse, a.X, a.Y)}" fill="none" stroke="${C.navy}" stroke-width="5" stroke-linejoin="round"/>\n`;
  b += `  <circle cx="${f(a.X(tApo))}" cy="${f(a.Y(hApo))}" r="9" fill="${C.orange}" stroke="#fff" stroke-width="3"/>\n`;
  b += `  <text x="${f(a.X(tApo))}" y="112" text-anchor="middle" font-size="20" font-weight="700" fill="${C.ink}">apogeo ≈ ${Math.round(hApo)} m, velocità zero</text>\n`;
  b += `  <circle cx="${f(a.X(tLand))}" cy="${f(a.Y(0))}" r="9" fill="${C.red}" stroke="#fff" stroke-width="3"/>\n`;
  b += `  <text x="${f(a.X(tLand) + 14)}" y="${f(a.Y(0) - 14)}" font-size="20" fill="${C.ink}">impatto a ~${Math.round(vImpact)} m/s</text>\n`;
  b += `  <circle cx="${f(a.X(tStill))}" cy="${f(a.Y(0))}" r="9" fill="${C.green}" stroke="#fff" stroke-width="3"/>\n`;
  b += `  <line x1="${f(a.X(tStill))}" y1="${f(a.Y(16))}" x2="${f(a.X(tStill))}" y2="${f(a.Y(0) - 10)}" stroke="${C.green}" stroke-width="2" stroke-dasharray="5 5"/>\n`;
  b += `  <text x="1062" y="${f(a.Y(28))}" text-anchor="end" font-size="20" font-weight="700" fill="${C.green}">fermo da 3 s:</text>\n`;
  b += `  <text x="1062" y="${f(a.Y(28) + 24)}" text-anchor="end" font-size="20" fill="${C.ink}">lampo e bip ogni secondo</text>\n`;
  b += `  <text x="${f(a.X(0.25) + 4)}" y="${f(a.Y(1.2))}" font-size="18" font-weight="700" fill="${C.orangeD}">← spinta 0,2 s</text>\n`;
  b += `  <text x="${f((a.X(0.2) + a.X(tApo)) / 2 + 30)}" y="${f(a.Y(10))}" text-anchor="middle" font-size="18" font-weight="700" fill="${C.blue}">salita per inerzia</text>\n`;
  b += `  <text x="${f(a.X(tApo) + 10)}" y="${f(a.Y(8))}" font-size="18" font-weight="700" fill="${C.red}">caduta, senza paracadute</text>\n`;
  b += `  <text x="120" y="430" font-size="18" fill="${C.muted}" font-style="italic">profilo tipico calcolato con un modello semplice: il primo volo vero lo dirà</text>\n`;
  save('profilo-volo.svg', svg(1100, 440, 'Profilo di quota tipico di un volo senza paracadute', b));
  console.log(`  apogeo ${hApo.toFixed(1)} m a ${tApo.toFixed(2)} s, impatto a ${tLand.toFixed(1)} s a ${vImpact.toFixed(1)} m/s`);
}

// --------------------------------------------------------------- i2c-tempi.svg
{
  const SCL_H = 100, SCL_L = 160, SDA_H = 250, SDA_L = 310;
  const X0 = 190, W = 92;
  const bits = [1, 1, 1, 0, 1, 1, 1, 0, 0];
  let b = '';
  // zone colorate
  b += `  <rect x="${X0}" y="70" width="${7 * W}" height="300" fill="${C.lblue}"/>\n`;
  b += `  <rect x="${X0 + 7 * W}" y="70" width="${W}" height="300" fill="${C.lorange}"/>\n`;
  b += `  <rect x="${X0 + 8 * W}" y="70" width="${W}" height="300" fill="${C.lgreen}"/>\n`;
  b += `  <rect x="130" y="70" width="${X0 - 130}" height="300" fill="${C.lgray}"/>\n`;
  b += `  <text x="${X0 + 3.5 * W}" y="56" text-anchor="middle" font-size="21" font-weight="700" fill="${C.blue}">indirizzo 0x77 = «BMP180, dico a te»</text>\n`;
  b += `  <text x="${X0 + 7.5 * W}" y="56" text-anchor="middle" font-size="21" font-weight="700" fill="${C.orangeD}">scrivo</text>\n`;
  b += `  <text x="${X0 + 8.5 * W}" y="56" text-anchor="middle" font-size="21" font-weight="700" fill="${C.green}">«ci sono!»</text>\n`;
  b += `  <text x="${(130 + X0) / 2}" y="56" text-anchor="middle" font-size="19" font-weight="700" fill="${C.muted}">START</text>\n`;
  // SCL
  let scl = `M70,${SCL_H} H${X0}`;
  for (let i = 0; i < bits.length; i++) {
    const x = X0 + i * W;
    scl += ` V${SCL_L} H${x + 30} V${SCL_H} H${x + 62} V${SCL_L} H${x + W}`;
  }
  scl += ` H1070`;
  // SDA
  let sda = `M70,${SDA_H} H150 L158,${SDA_L}`;
  let lvl = 0;
  for (let i = 0; i < bits.length; i++) {
    const x = X0 + i * W;
    const y = bits[i] ? SDA_H : SDA_L;
    if (bits[i] !== lvl) sda += ` H${x + 6} L${x + 14},${y}`;
    lvl = bits[i];
  }
  sda += ` H${X0 + 9 * W + 6} L${X0 + 9 * W + 14},${SDA_H} H1070`;
  // istanti di lettura
  for (let i = 0; i < bits.length; i++) {
    const x = X0 + i * W + 46;
    b += `  <line x1="${x}" y1="${SCL_H}" x2="${x}" y2="${SDA_L + 6}" stroke="#9aa5b4" stroke-width="1.5" stroke-dasharray="4 5"/>\n`;
  }
  b += `  <path d="${scl}" fill="none" stroke="${C.amber}" stroke-width="5" stroke-linejoin="round"/>\n`;
  b += `  <path d="${sda}" fill="none" stroke="${C.blue}" stroke-width="5" stroke-linejoin="round"/>\n`;
  for (let i = 0; i < bits.length; i++) {
    const x = X0 + i * W + 46;
    const y = bits[i] ? SDA_H : SDA_L;
    b += `  <circle cx="${x}" cy="${y}" r="6" fill="${C.navy}"/>\n`;
    b += `  <text x="${x}" y="356" text-anchor="middle" font-size="30" font-weight="800" fill="${i === 8 ? C.green : i === 7 ? C.orangeD : C.navy}">${bits[i]}</text>\n`;
  }
  b += `  <text x="20" y="${(SCL_H + SCL_L) / 2 + 8}" font-size="24" font-weight="800" fill="${C.amber}">SCL</text>\n`;
  b += `  <text x="20" y="${(SDA_H + SDA_L) / 2 + 8}" font-size="24" font-weight="800" fill="${C.blue}">SDA</text>\n`;
  b += `  <text x="1080" y="${SDA_H + 40}" text-anchor="end" font-size="28" fill="${C.muted}">…</text>\n`;
  b += `  <circle cx="40" cy="410" r="6" fill="${C.navy}"/>\n`;
  b += `  <text x="56" y="417" font-size="20" fill="${C.ink}">= il bit viene letto quando SCL è alto · SDA cambia solo quando SCL è basso · 100 000 tic al secondo</text>\n`;
  save('i2c-tempi.svg', svg(1100, 440, 'Diagramma temporale I2C: start, indirizzo 0x77, scrittura, ACK', b));
}

// ------------------------------------------------------------ colonna-aria.svg
{
  const r = rng(11);
  let b = '';
  b += `  <defs><linearGradient id="aria" x1="0" y1="1" x2="0" y2="0">` +
    `<stop offset="0" stop-color="${C.blue}" stop-opacity="0.45"/>` +
    `<stop offset="1" stop-color="${C.blue}" stop-opacity="0.03"/></linearGradient></defs>\n`;
  b += `  <rect x="70" y="40" width="240" height="370" rx="6" fill="url(#aria)"/>\n`;
  // molecole: più fitte in basso
  for (let i = 0; i < 260; i++) {
    const hgt = -Math.log(1 - r() * 0.985) / 3.2; // 0..~1.3, esponenziale
    if (hgt > 1) continue;
    const y = 404 - hgt * 360, x = 80 + r() * 220;
    b += `  <circle cx="${f(x)}" cy="${f(y)}" r="4" fill="${C.navy}" opacity="0.55"/>\n`;
  }
  b += `  <rect x="40" y="410" width="300" height="16" rx="4" fill="#7a9a46"/>\n`;
  b += `  <text x="190" y="28" text-anchor="middle" font-size="20" fill="${C.muted}">… fino a decine di km</text>\n`;
  // freccia peso
  b += `  <path d="M355,90 V370" stroke="${C.navy}" stroke-width="4"/><path d="M343,354 l12,20 l12,-20 z" fill="${C.navy}"/>\n`;
  b += `  <text x="368" y="240" font-size="20" fill="${C.ink}" transform="rotate(90 368 240)" text-anchor="middle">ogni strato preme su quello sotto</text>\n`;
  const row = (y, big, l1, l2) =>
    `  <text x="440" y="${y}" font-size="48" font-weight="800" fill="${C.orangeD}">${big}</text>\n` +
    `  <text x="440" y="${y + 36}" font-size="24" fill="${C.ink}">${l1}</text>\n` +
    `  <text x="440" y="${y + 66}" font-size="24" fill="${C.muted}">${l2}</text>\n`;
  b += row(80, '1013 hPa', 'la pressione a livello del mare', 'come 10 tonnellate appoggiate su ogni m²');
  b += row(215, '−12 Pa', 'ogni metro che si sale', '(vicino al suolo: 1 hPa ≈ 8 m)');
  b += row(350, '1 su 8 000', 'quanto cambia la pressione in 1 metro:', 'serve un sensore sensibilissimo');
  save('colonna-aria.svg', svg(1100, 440, 'La pressione dell\'aria diminuisce salendo', b));
}
