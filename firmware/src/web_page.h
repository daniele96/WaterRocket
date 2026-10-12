#pragma once
#include <Arduino.h>

// Applicazione web servita su "/": elenco lanci (#/) e dettaglio lancio
// (#/lancio/REC_xxxx) con player, parametri, grafici sensori e download.
// Nessuna libreria esterna: deve funzionare anche senza internet (rete WaterRocket).
//
// Dati usati per ogni lancio (cartella /REC_xxxx sulla SD):
//   info.txt     scritto dal firmware (ora di inizio, frame, fps, durata...)
//   launch.json  scritto dalla pagina (nome, parametri, note)
//   video.mjpeg + frames.csv + audio.wav  registrazione
//   sensors.csv  accelerometro/giroscopio (flight.cpp), prima colonna = tempo in ms, poi una colonna per grandezza:
//                ogni colonna diventa un grafico in automatico
static const char WEB_PAGE[] PROGMEM = R"HTML(<!doctype html>
<html lang="it">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Water Rocket</title>
<style>
:root{--bg:#f3f5f8;--card:#fff;--fg:#17202b;--mut:#66727f;--acc:#0a6cff;--acc2:#e6f0ff;--rec:#e0283b;--ok:#13894f;--bd:#dfe4ea;--sh:0 1px 2px rgba(16,24,40,.06)}
@media (prefers-color-scheme:dark){:root{--bg:#0e1319;--card:#18202a;--fg:#e8edf2;--mut:#93a0ad;--acc:#4ea1ff;--acc2:#1b2b40;--rec:#ff5466;--ok:#3ccf8e;--bd:#28323e;--sh:none}}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--fg);font:15px/1.45 system-ui,-apple-system,Segoe UI,Roboto,sans-serif}
a{color:var(--acc);text-decoration:none}
header{position:sticky;top:0;z-index:5;background:var(--card);border-bottom:1px solid var(--bd)}
.hin{max-width:1800px;margin:0 auto;padding:10px 16px;display:flex;gap:12px;align-items:center;justify-content:space-between}
.brand{font-weight:700;font-size:17px;color:var(--fg);white-space:nowrap}
main{max-width:1800px;margin:0 auto;padding:16px}
h2{font-size:18px;margin:0}
h3{font-size:15px;margin:0 0 10px}
.card{background:var(--card);border:1px solid var(--bd);border-radius:12px;padding:14px;margin-bottom:14px;box-shadow:var(--sh)}
.row{display:flex;gap:8px;flex-wrap:wrap;align-items:center}
.between{justify-content:space-between}
.mut{color:var(--mut);font-size:13px}
.hidden{display:none!important}
button,.btn{appearance:none;border:1px solid var(--bd);background:var(--card);color:var(--fg);border-radius:8px;padding:8px 13px;font:inherit;font-size:14px;cursor:pointer;display:inline-flex;align-items:center;gap:6px;white-space:nowrap}
button:hover,.btn:hover{border-color:var(--acc)}
button.pri{background:var(--acc);border-color:var(--acc);color:#fff}
button.rec{background:var(--rec);border-color:var(--rec);color:#fff}
button.danger{color:var(--rec)}
button:disabled{opacity:.45;cursor:default}
[hidden]{display:none!important}
.dot{width:9px;height:9px;border-radius:50%;background:var(--mut);display:inline-block;flex:none}
.dot.on{background:var(--rec);animation:b 1s infinite}
@keyframes b{50%{opacity:.25}}
#recinfo{font-size:13px;color:var(--mut);overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(230px,1fr));gap:14px}
.lcard{background:var(--card);border:1px solid var(--bd);border-radius:12px;overflow:hidden;color:var(--fg);box-shadow:var(--sh);display:block}
.lcard:hover{border-color:var(--acc)}
.thumb{position:relative;aspect-ratio:4/3;background:#0b0f14;display:flex;align-items:center;justify-content:center;font-size:38px}
.thumb img{position:absolute;inset:0;width:100%;height:100%;object-fit:cover;display:block}
.badge{position:absolute;bottom:8px;right:8px;background:rgba(0,0,0,.65);color:#fff;font-size:12px;padding:2px 7px;border-radius:6px}
.badge.rec{left:8px;right:auto;top:8px;bottom:auto;background:var(--rec)}
.badge.warn{left:8px;right:auto;top:8px;bottom:auto;background:#b26a00}
.lbody{padding:10px 12px 12px}
.ltitle{font-weight:600;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.chips{display:flex;gap:6px;flex-wrap:wrap;margin-top:8px}
.chip{background:var(--acc2);color:var(--acc);font-size:12px;padding:2px 8px;border-radius:999px}
.empty{text-align:center;padding:40px 16px}
.view{width:100%;border-radius:8px;background:#000;display:block;aspect-ratio:4/3;object-fit:contain}
#pimg{max-height:72vh}
.cols{display:grid;grid-template-columns:minmax(0,1fr) 340px;gap:14px;align-items:start}
@media (max-width:820px){.cols{grid-template-columns:1fr}}
.stats{display:grid;grid-template-columns:repeat(auto-fill,minmax(120px,1fr));gap:10px}
.stat b{display:block;font-size:16px}
.stat span{font-size:12px;color:var(--mut)}
.stat.click{cursor:pointer}
.stat.click:hover b{color:var(--acc)}
label{display:block;font-size:12px;color:var(--mut);margin-bottom:3px}
input,textarea,select{width:100%;font:inherit;font-size:14px;color:var(--fg);background:var(--bg);border:1px solid var(--bd);border-radius:8px;padding:7px 9px}
textarea{min-height:70px;resize:vertical}
.form{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px}
.form .full{grid-column:1/-1}
.unit{position:relative}
.unit em{position:absolute;right:9px;top:50%;transform:translateY(-50%);font-style:normal;font-size:12px;color:var(--mut);pointer-events:none}
.unit input{padding-right:38px}
input.title{font-size:20px;font-weight:700;background:transparent;border-color:transparent;padding:4px 6px;margin-left:-6px}
input.title:hover,input.title:focus{border-color:var(--bd);background:var(--card)}
.ctrl{display:flex;gap:6px;align-items:center;margin-top:10px;flex-wrap:wrap}
.ctrl input[type=range]{flex:1;min-width:140px;padding:0;accent-color:var(--acc);background:none;border:0}
.ctrl select{width:auto}
.tnum{font-variant-numeric:tabular-nums}
.progress{height:6px;background:var(--bd);border-radius:3px;overflow:hidden;margin-top:10px}
.progress div{height:100%;width:0;background:var(--acc);transition:width .2s}
.dl a{display:flex;justify-content:space-between;gap:8px;padding:7px 0;border-top:1px solid var(--bd)}
.dl a:first-child{border-top:0}
.chart{margin-top:12px}
.chart canvas{width:100%;height:220px;display:block;cursor:crosshair;touch-action:pan-y}
.toast{position:fixed;left:50%;bottom:20px;transform:translateX(-50%);background:var(--fg);color:var(--bg);padding:9px 16px;border-radius:8px;font-size:14px;z-index:9;opacity:0;transition:opacity .2s;pointer-events:none}
.toast.show{opacity:1}
footer{max-width:1800px;margin:0 auto;padding:4px 16px 24px;color:var(--mut);font-size:12px}
</style>
</head>
<body>
<header><div class="hin">
  <a class="brand" href="#/">🚀 Water Rocket</a>
  <div class="row" style="flex-wrap:nowrap;min-width:0">
    <span id="recdot" class="dot"></span><span id="recinfo">...</span>
    <button id="buzzbtn" class="rec" onclick="stopBuzzer()" hidden>🔕 Spegni cicalino</button>
    <button id="recbtn" class="rec" onclick="toggleRec()">● Nuovo lancio</button>
  </div>
</div></header>
<main id="app"></main>
<footer id="foot"></footer>
<div id="toast" class="toast"></div>

<script>
// ---------- utilità ----------
const $ = (s, el = document) => el.querySelector(s);
const esc = s => String(s ?? '').replace(/[&<>"']/g, c => ({'&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;'}[c]));
const fileUrl = (p, dl) => '/file?path=' + encodeURIComponent(p) + (dl ? '&dl=1' : '');
const fmtSize = b => b >= 1048576 ? (b / 1048576).toFixed(1) + ' MB' : b >= 1024 ? Math.round(b / 1024) + ' KB' : b + ' B';
const fmtSec = s => s == null ? '–' : s < 60 ? s.toFixed(1) + ' s' : Math.floor(s / 60) + ':' + String(Math.floor(s % 60)).padStart(2, '0') + ' min';
const fmtDate = d => d ? d.toLocaleString('it-IT', {weekday: 'short', day: 'numeric', month: 'short', year: 'numeric', hour: '2-digit', minute: '2-digit'}) : 'data sconosciuta';
const fmtNum = (v, unit) => v == null || v === '' ? '–' : `${v} ${unit}`;

function toast(msg) {
  const t = $('#toast');
  t.textContent = msg;
  t.classList.add('show');
  clearTimeout(toast.h);
  toast.h = setTimeout(() => t.classList.remove('show'), 2200);
}

function download(name, content, type) {
  const a = document.createElement('a');
  a.href = URL.createObjectURL(new Blob([content], {type}));
  a.download = name;
  a.click();
  setTimeout(() => URL.revokeObjectURL(a.href), 1000);
}

function parseInfo(txt) {
  const o = {};
  for (const line of (txt || '').split('\n')) {
    const i = line.indexOf('=');
    if (i > 0) o[line.slice(0, i)] = line.slice(i + 1).trim();
  }
  return o;
}

// Campi della scheda lancio (dalla tabella "Lancio e test" della documentazione)
const FIELDS = [
  {k: 'bottle_l', label: 'Bottiglia', unit: 'L', step: '0.1'},
  {k: 'water_pct', label: 'Acqua', unit: '%', step: '1'},
  {k: 'pressure_bar', label: 'Pressione', unit: 'bar', step: '0.1'},
  {k: 'mass_g', label: 'Massa a vuoto', unit: 'g', step: '1'},
  {k: 'altitude_m', label: 'Quota stimata', unit: 'm', step: '0.1'},
  {k: 'location', label: 'Luogo', text: true},
];

// Normalizza un lancio dall'API in un oggetto comodo per la UI
function toLaunch(raw) {
  const info = parseInfo(raw.info);
  const meta = raw.meta || {};
  const n = parseInt(raw.dir.slice(5), 10);
  const epoch = +info.started_epoch || 0;
  const has = name => raw.files.some(f => f.name === name);
  return {
    dir: raw.dir, id: raw.dir.slice(1), n, meta, info, files: raw.files, recording: raw.recording,
    name: meta.name || `Lancio #${n}`,
    date: epoch ? new Date(epoch * 1000) : null,
    duration: info.duration_s ? +info.duration_s : null,
    fps: info.fps ? +info.fps : null,
    frames: info.frames ? +info.frames : null,
    size: raw.files.reduce((a, f) => a + f.size, 0),
    hasVideo: has('video.mjpeg') && has('frames.csv'),
    hasSensors: has('sensors.csv'),
    interrupted: !raw.recording && info.status === 'recording',
  };
}

// ---------- stato e API ----------
const S = {status: null, launches: [], timeSent: false, loaded: false};

async function api(path, opts) {
  const r = await fetch(path, opts);
  if (!r.ok) throw new Error(await r.text() || r.statusText);
  return r;
}

async function loadLaunches() {
  const list = await (await api('/api/launches')).json();
  S.launches = list.map(toLaunch).sort((a, b) => b.n - a.n);
  S.loaded = true;
}

async function pollStatus() {
  let s;
  try {
    s = await (await api('/api/status')).json();
  } catch (e) {
    $('#recinfo').textContent = 'Non connesso';
    return;
  }
  const wasRec = S.status?.recording;
  S.status = s;
  // Lo XIAO non ha orologio: se non ha l'ora da internet gliela passa il browser
  if (!s.time && !S.timeSent) {
    S.timeSent = true;
    fetch('/api/time?epoch=' + Math.floor(Date.now() / 1000), {method: 'POST'});
  }
  $('#recdot').className = 'dot' + (s.recording ? ' on' : '');
  // Fase del volo dall'accelerometro: 0 a terra, 1 in volo, 2 atterrato
  const phase = s.phase === 1 ? ' · 🚀 IN VOLO'
    : s.phase === 2 ? ` · 🎯 ATTERRATO (picco ${s.peakG.toFixed(1)} g` +
      (s.baro ? `, quota max ${s.baro.apogee.toFixed(1)} m)` : ')') : '';
  $('#recinfo').textContent = (s.testMode ? '🧪 PROVA · ' : '') + (s.recording
    ? `REC ${s.secs.toFixed(1)} s · ${s.frames} frame · ${s.fps.toFixed(1)} fps`
    : s.sd ? 'Pronto' : 'SD non montata') + phase;
  updateSim(s);
  const tb = $('#testbtn');
  if (tb) {
    tb.textContent = testLabel();
    tb.className = s.testMode ? 'pri' : '';
  }
  // Il cicalino suona da atterrato finché qualcuno non lo spegne
  $('#buzzbtn').hidden = !s.buzzer;
  const b = $('#recbtn');
  b.textContent = s.recording ? '■ Stop' : '● Nuovo lancio';
  b.className = s.recording ? '' : 'rec';
  b.disabled = !s.recording && !s.sd;
  $('#foot').textContent = [
    `Camera ${s.camera}`,
    s.sd ? `SD ${(s.sdFreeMB / 1024).toFixed(1)} GB liberi su ${(s.sdTotalMB / 1024).toFixed(1)} GB` : 'SD non montata',
    s.staIp ? `IP ${s.staIp}` : 'rete WaterRocket · 192.168.4.1',
  ].join(' · ');
  if (wasRec !== undefined && wasRec !== s.recording) {
    await loadLaunches();
    if (route().view === 'home') render();
  }
}

async function toggleRec() {
  const rec = S.status?.recording;
  try {
    await api('/api/rec/' + (rec ? 'stop' : 'start'), {method: 'POST'});
    toast(rec ? 'Registrazione fermata' : 'Registrazione avviata');
  } catch (e) {
    toast('Errore: ' + e.message);
  }
  // Il comando viene eseguito dallo XIAO nel giro di pochi ms: poi aggiorno lo stato
  await new Promise(r => setTimeout(r, 400));
  await pollStatus();
}

async function stopBuzzer() {
  try {
    await api('/api/buzzer/stop', {method: 'POST'});
    toast('Cicalino spento · pronto per il prossimo lancio');
  } catch (e) {
    toast('Errore: ' + e.message);
  }
  await new Promise(r => setTimeout(r, 400));
  await pollStatus();
}

// ---------- routing ----------
function route() {
  const m = location.hash.match(/^#\/lancio\/(REC_\d+)/);
  return m ? {view: 'launch', id: m[1]} : {view: 'home'};
}

function render() {
  Player.close();
  Sensors.series = [];
  Sensors.t = null;
  const r = route();
  if (!S.loaded) {
    $('#app').innerHTML = '<p class="mut">Caricamento...</p>';
    return;
  }
  if (r.view === 'launch') {
    const l = S.launches.find(x => x.id === r.id);
    l ? renderLaunch(l) : ($('#app').innerHTML = '<div class="card empty">Lancio non trovato. <a href="#/">Torna ai lanci</a></div>');
  } else {
    renderHome();
  }
  window.scrollTo(0, 0);
}

// ---------- home: elenco lanci ----------
function launchCard(l) {
  // Quota massima e picco li scrive il firmware in info.txt solo se nella registrazione c'è un volo
  const flew = +l.info.flights > 0;
  const chips = [
    flew && l.info.apogee_m != null ? `↑ ${(+l.info.apogee_m).toFixed(1)} m` : '',
    flew && l.info.peak_g != null ? `${(+l.info.peak_g).toFixed(1)} g` : '',
    l.meta.pressure_bar != null && l.meta.pressure_bar !== '' ? `${l.meta.pressure_bar} bar` : '',
    l.meta.water_pct != null && l.meta.water_pct !== '' ? `${l.meta.water_pct}% acqua` : '',
    l.meta.altitude_m != null && l.meta.altitude_m !== '' ? `${l.meta.altitude_m} m` : '',
    l.hasSensors ? 'sensori' : '',
  ].filter(Boolean);
  return `<a class="lcard" href="#/lancio/${l.id}">
    <div class="thumb">🚀
      ${l.hasVideo && !l.recording ? `<img loading="lazy" src="/thumb?dir=${encodeURIComponent(l.dir)}" alt="" onerror="this.remove()">` : ''}
      ${l.recording ? '<span class="badge rec">● REC</span>' : l.interrupted ? '<span class="badge warn">interrotto</span>' : ''}
      ${l.duration ? `<span class="badge">${fmtSec(l.duration)}</span>` : ''}
    </div>
    <div class="lbody">
      <div class="ltitle">${esc(l.name)}</div>
      <div class="mut">${fmtDate(l.date)}${l.meta.location ? ' · ' + esc(l.meta.location) : ''}</div>
      ${chips.length ? `<div class="chips">${chips.map(c => `<span class="chip">${esc(c)}</span>`).join('')}</div>` : ''}
    </div>
  </a>`;
}

function renderHome() {
  const L = S.launches;
  $('#app').innerHTML = `
    <div class="row between" style="margin-bottom:14px">
      <h2>Lanci <span class="mut">(${L.length})</span></h2>
      <div class="row">
        <span id="precalinfo" class="mut" style="font-size:13px">${precalText(S.status)}</span>
        <button id="precalbtn" class="pri" onclick="startPrecal()" title="Con il razzo fermo sulla rampa: 3 s per azzerare quota e accelerometro">📏 Calibra prima del lancio</button>
        <button onclick="toggleSim()">🧪 Pannello prova</button>
        <button onclick="togglePreview()">📷 Anteprima camera</button>
        <button onclick="exportTable()" ${L.length ? '' : 'disabled'}>⬇ Tabella lanci (CSV)</button>
      </div>
    </div>
    <div id="simpanel" class="card ${S.simOpen ? '' : 'hidden'}">
      <div class="row between"><b>🧪 Pannello di prova</b><span id="simphase" class="mut"></span></div>
      <div class="row" style="margin-top:10px;flex-wrap:wrap">
        <button class="pri" onclick="sim('launch')">🚀 Simula decollo</button>
        <button onclick="sim('land')">🎯 Simula atterraggio</button>
        <button class="danger" onclick="sim('abort')">⛔ Interrompi (torna a terra)</button>
      </div>
      <div class="row" style="margin-top:8px;flex-wrap:wrap">
        <button onclick="sim('buzz_on')">🔔 Accendi cicalino</button>
        <button onclick="sim('buzz_off')">🔕 Spegni cicalino</button>
        <button id="testbtn" onclick="toggleTestMode()" title="Decollo alzando la scheda con decisione (1,5 g per 2 letture) invece della spinta vera (2,5 g per 4 letture). Registra da sola ogni prova: parte all'attivazione e si ferma 3 s dopo l'atterraggio. Anello LED al massimo al 25% (sull'USB)">${testLabel()}</button>
      </div>
      <div class="row" style="margin-top:8px;flex-wrap:wrap">
        <b style="font-size:14px">💡 Anello</b>
        <button id="ledauto" onclick="ledAuto()" title="Colori secondo la fase: verde pronto, rosso pulsante in registrazione, bianco in volo, lampo all'atterraggio, rosso fisso se manca SD o sensore">Automatico</button>
        <input id="ledcolor" type="color" value="#ffffff" oninput="ledManual()" title="Colore manuale">
        <input id="ledpct" type="range" min="0" max="100" value="10" oninput="ledManual()" style="width:140px" title="Luminosità manuale">
        <span id="ledpctv" class="mut tnum">10%</span>
        <button onclick="ledOff()">Spegni</button>
      </div>
      <div id="simdata" class="mut tnum" style="margin-top:10px;font-size:13px"></div>
    </div>
    <div id="preview" class="card hidden">
      <img id="snap" class="view" alt="">
      <div class="row between" style="margin-top:8px">
        <span class="mut">Foto singola per puntare la camera (non disponibile durante la registrazione)</span>
        <button onclick="snap()">↻ Scatta</button>
      </div>
    </div>
    ${L.length ? `<div class="grid">${L.map(launchCard).join('')}</div>`
      : `<div class="card empty"><div style="font-size:40px">🚀</div><p>Nessun lancio registrato.</p>
         <p class="mut">Premi <b>● Nuovo lancio</b> qui sopra o il pulsante BOOT sulla scheda.</p></div>`}`;
}

// ---------- pannello di prova: simulazione del volo e cicalino ----------
const PHASES = {0: '🟢 A TERRA', 1: '🚀 IN VOLO', 2: '🎯 ATTERRATO'};

function toggleSim() {
  S.simOpen = !S.simOpen;
  $('#simpanel').classList.toggle('hidden', !S.simOpen);
  // Pannello aperto: stato ogni 0,5 s per vedere i valori del sensore dal vivo
  clearInterval(S.simTimer);
  if (S.simOpen) S.simTimer = setInterval(pollStatus, 500);
  pollStatus();
}

async function sim(action) {
  const msg = {launch: 'Decollo simulato', land: 'Atterraggio simulato',
    abort: 'Interrotto: di nuovo A TERRA', buzz_on: 'Cicalino acceso', buzz_off: 'Cicalino spento'};
  try {
    await api('/api/sim?action=' + action, {method: 'POST'});
    toast(msg[action]);
  } catch (e) {
    toast('Errore: ' + e.message);
  }
  await new Promise(r => setTimeout(r, 300));
  await pollStatus();
}

// ---------- anello LED: automatico (fasi del volo) o manuale ----------
let ledTimer = 0;

function ledSend(query) {
  return api('/api/led?' + query, {method: 'POST'}).catch(e => toast('Errore: ' + e.message));
}

// Il cursore manda decine di eventi al secondo: si invia solo l'ultimo valore ogni 120 ms
function ledManual() {
  $('#ledpctv').textContent = $('#ledpct').value + '%';
  clearTimeout(ledTimer);
  ledTimer = setTimeout(() => {
    ledTimer = 0;
    ledSend(`mode=manual&color=${$('#ledcolor').value.slice(1)}&pct=${$('#ledpct').value}`).then(pollStatus);
  }, 120);
}

function ledOff() {
  $('#ledpct').value = 0;
  ledManual();
}

async function ledAuto() {
  await ledSend('mode=auto');
  toast('Anello in automatico: colori secondo la fase del volo');
  await pollStatus();
}

function updateLed(s) {
  if (!s.led || !$('#ledauto')) return;
  $('#ledauto').className = s.led.manual ? '' : 'pri';
  // Non sovrascrivo i controlli mentre l'utente li sta usando
  const busy = ledTimer || ['ledcolor', 'ledpct'].includes(document.activeElement?.id);
  if (!busy) {
    $('#ledcolor').value = '#' + s.led.color;
    $('#ledpct').value = s.led.pct;
    $('#ledpctv').textContent = s.led.pct + '%';
  }
}

// ---------- calibrazione pre-lancio: zero di quota e accelerometro con il razzo sulla rampa ----------
const PRECAL_OLD_S = 600;   // dopo 10 minuti meteo e temperatura possono aver spostato lo zero
const PRECAL_NOISY_M = 0.5; // oltre, la quota è troppo agitata (vento sul sensore, colpi)
let precalTimer = 0;

function precalText(s) {
  const p = s?.precal;
  if (!p) return '';
  if (p.state === 'running') return `⏳ calibrazione ${p.progress}%: ${p.msg}`;
  if (p.state === 'fail') return `❌ ${p.msg}`;
  if (p.state !== 'ok') return 'Calibrazione pre-lancio non ancora fatta';
  const age = p.age < 90 ? `${p.age} s fa` : `${Math.round(p.age / 60)} min fa`;
  const noise = p.noise == null ? '' : ` · quota ±${p.noise.toFixed(2)} m`;
  const warn = p.age > PRECAL_OLD_S ? ' ⚠ conviene rifarla'
    : p.noise != null && p.noise > PRECAL_NOISY_M ? ' ⚠ quota rumorosa: ripara il sensore dal vento' : '';
  return `✅ calibrato ${age}${noise}${warn}`;
}

async function startPrecal() {
  try {
    await api('/api/precal', {method: 'POST'});
    toast('Calibrazione: lascia il razzo fermo per 3 secondi');
  } catch (e) {
    return toast('Errore: ' + e.message);
  }
  // Durante la calibrazione lo stato si aggiorna ogni 0,5 s, poi si torna al ritmo normale
  clearInterval(precalTimer);
  precalTimer = setInterval(async () => {
    await pollStatus();
    if (S.status?.precal?.state !== 'running') {
      clearInterval(precalTimer);
      const p = S.status?.precal;
      if (p?.state === 'ok') toast('✅ Calibrato, pronto al lancio');
      if (p?.state === 'fail') toast('❌ Calibrazione fallita: ' + p.msg);
    }
  }, 500);
}

function updatePrecal(s) {
  const el = $('#precalinfo');
  if (!el) return;
  el.textContent = precalText(s);
  const b = $('#precalbtn');
  b.disabled = s.precal?.state === 'running' || s.phase !== 0;
  b.title = s.phase !== 0 ? 'Si calibra solo a terra, prima del decollo'
    : 'Con il razzo fermo sulla rampa: 3 s per azzerare quota e accelerometro';
}

function updateSim(s) {
  updatePrecal(s);
  updateLed(s);
  const box = $('#simdata');
  if (!box) return;
  $('#simphase').textContent = s.phase >= 0 ? PHASES[s.phase] : 'accelerometro non trovato';
  if (s.g == null) {
    box.textContent = 'MPU6050 non trovato: controlla i collegamenti';
    return;
  }
  const zero = s.calAge < 0 ? '⚠ non ancora preso: lascia il sensore fermo 2 s'
    : `preso ${s.calAge} s fa (a riposo leggeva ${s.calRawG.toFixed(3)} g, ora corretto a 1,000)`;
  box.innerHTML = [
    `Accelerazione: |a| <b>${s.g.toFixed(2)} g</b> · netta <b>${s.net.toFixed(2)} g</b> · ` +
      `x ${s.acc[0].toFixed(2)} y ${s.acc[1].toFixed(2)} z ${s.acc[2].toFixed(2)}`,
    `Giroscopio: x ${s.gyro[0].toFixed(1)} y ${s.gyro[1].toFixed(1)} z ${s.gyro[2].toFixed(1)} °/s` +
      ` · picco dall'ultimo decollo ${s.peakG.toFixed(1)} g`,
    `Zero sulla rampa: ${zero}`,
    s.baro ? `Altimetro ${s.baro.model}: quota <b>${s.baro.alt.toFixed(2)} m</b> · ${s.baro.press.toFixed(2)} hPa · ` +
      `${s.baro.temp.toFixed(1)} °C · quota massima dall'ultimo decollo ${s.baro.apogee.toFixed(1)} m`
      : 'Altimetro: non collegato',
    `Cicalino: ${s.buzzer ? '🔔 suona' : 'spento'}`,
    `Anello: ${!s.led ? '-' : s.led.manual ? `manuale, #${s.led.color} al ${s.led.pct}%`
      : 'automatico' + (s.testMode ? ' (max 25% in modalità prova)' : '')}`,
  ].join('<br>');
}

// ---------- modalità prova: decollo con un colpo della mano ----------
function testLabel() {
  return S.status?.testMode ? '🧪 Modalità prova: ON' : '🧪 Modalità prova: OFF';
}

async function toggleTestMode() {
  const on = !S.status?.testMode;
  try {
    await api('/api/testmode?on=' + (on ? 1 : 0), {method: 'POST'});
    toast(on ? 'Modalità prova attiva: registrazione avviata, alza la scheda con decisione per il decollo'
             : 'Modalità prova disattivata: soglia di volo normale');
  } catch (e) {
    toast('Errore: ' + e.message);
  }
  await pollStatus();
}

function togglePreview() {
  const p = $('#preview');
  p.classList.toggle('hidden');
  if (!p.classList.contains('hidden')) snap();
}

function snap() {
  if (S.status?.recording) return toast('Camera occupata dalla registrazione');
  $('#snap').src = '/snapshot.jpg?t=' + Date.now();
}

function csvCell(v) {
  const s = String(v ?? '');
  return /[",\n;]/.test(s) ? '"' + s.replace(/"/g, '""') + '"' : s;
}

function exportTable() {
  const head = ['lancio', 'nome', 'data', 'durata_s', 'fps', 'frame', ...FIELDS.map(f => f.k), 'note'];
  const rows = S.launches.slice().reverse().map(l => [
    l.id, l.name, l.date ? l.date.toISOString() : '', l.duration ?? '', l.fps ?? '', l.frames ?? '',
    ...FIELDS.map(f => l.meta[f.k] ?? ''), l.meta.notes ?? '',
  ]);
  download('lanci_water_rocket.csv', [head, ...rows].map(r => r.map(csvCell).join(',')).join('\n'), 'text/csv');
}

// ---------- dettaglio lancio ----------
function renderLaunch(l) {
  const m = l.meta;
  const water = m.bottle_l && m.water_pct ? Math.round(m.bottle_l * m.water_pct * 10) + ' ml' : null;
  $('#app').innerHTML = `
    <div class="row between" style="margin-bottom:8px">
      <a href="#/">← Tutti i lanci</a>
      <span class="mut">${l.id}</span>
    </div>
    <input class="title" id="f_name" value="${esc(m.name || '')}" placeholder="${esc(l.name)}" onchange="saveMeta()">
    <div class="mut" style="margin:0 0 14px">${fmtDate(l.date)}${l.recording ? ' · <b style="color:var(--rec)">registrazione in corso</b>' : ''}${l.interrupted ? ' · registrazione interrotta (alimentazione persa?)' : ''}</div>

    <div class="cols">
      <div>
        <div class="card">
          <div id="pwrap">
            ${l.hasVideo && !l.recording ? `
              <div style="position:relative">
                <img id="pimg" class="view" src="/thumb?dir=${encodeURIComponent(l.dir)}" alt="">
              </div>
              <div id="pload" class="mut" style="margin-top:10px">Preparazione del video in streaming...</div>
              <div id="pctrl" class="hidden">
                <div class="ctrl">
                  <button onclick="Player.step(-1)" title="Frame precedente (←)">⏮</button>
                  <button id="pplay" class="pri" onclick="Player.toggle()" title="Play/pausa (spazio)">▶</button>
                  <button onclick="Player.step(1)" title="Frame successivo (→)">⏭</button>
                  <input id="pseek" type="range" min="0" max="1000" value="0" oninput="Player.seek(+this.value)">
                  <select id="pspeed" onchange="Player.setSpeed(+this.value)" title="Velocità">
                    <option value="1">1×</option><option value="0.5">0.5×</option><option value="0.25">0.25×</option><option value="0.1">0.1×</option>
                  </select>
                </div>
                <div class="row between mut tnum" style="margin-top:6px">
                  <span id="ptime">0.00 s</span><span id="pframe"></span>
                </div>
              </div>`
            : `<div class="view" style="display:flex;align-items:center;justify-content:center;color:#888">${l.recording ? 'Registrazione in corso...' : 'Nessun video'}</div>`}
          </div>
        </div>

        <div class="card">
          <h3>Dati sensori</h3>
          <div id="sensors" class="mut">${l.hasSensors ? 'Caricamento...' :
            'Nessun dato sensori per questo lancio. Quando collegheremo altimetro/accelerometro, i dati compariranno qui come grafici sincronizzati con il video.'}</div>
        </div>
      </div>

      <div>
        <div class="card" id="flightcard">
          <h3>🚀 Volo</h3>
          <div class="fbody mut">${l.recording ? 'Registrazione in corso...'
            : l.hasSensors ? 'Analisi dei dati...' : 'Nessun dato sensori per questo lancio.'}</div>
        </div>

        <div class="card">
          <h3>Registrazione</h3>
          <div class="stats">
            <div class="stat"><b>${fmtSec(l.duration)}</b><span>durata</span></div>
            <div class="stat"><b>${l.frames ?? '–'}</b><span>frame</span></div>
            <div class="stat"><b>${l.fps ? l.fps.toFixed(1) : '–'}</b><span>fps</span></div>
            <div class="stat"><b>${fmtSize(l.size)}</b><span>su SD</span></div>
          </div>
        </div>

        <div class="card">
          <div class="row between" style="margin-bottom:10px">
            <h3 style="margin:0">Parametri di lancio</h3>
            ${water ? `<span class="chip">${water} d'acqua</span>` : ''}
          </div>
          <div class="form">
            ${FIELDS.map(f => `<div class="${f.text ? 'full' : ''}">
              <label for="f_${f.k}">${f.label}</label>
              ${f.text ? `<input id="f_${f.k}" value="${esc(m[f.k] ?? '')}">`
                : `<div class="unit"><input id="f_${f.k}" type="number" inputmode="decimal" step="${f.step}" value="${esc(m[f.k] ?? '')}"><em>${f.unit}</em></div>`}
            </div>`).join('')}
            <div class="full"><label for="f_notes">Note</label><textarea id="f_notes" placeholder="Meteo, vento, comportamento in volo, paracadute...">${esc(m.notes || '')}</textarea></div>
          </div>
          <div class="row" style="justify-content:flex-end;margin-top:10px">
            <button class="pri" onclick="saveMeta()">Salva</button>
          </div>
        </div>

        <div class="card">
          <h3>Download</h3>
          <div class="dl">
            ${l.hasVideo && !l.recording ? `<a href="/avi?dir=${encodeURIComponent(l.dir)}"><span>🎬 Video con audio (.avi)</span><span class="mut">~${fmtSize(l.size)}</span></a>` : ''}
            ${!l.recording ? `<a href="/zip?dir=${encodeURIComponent(l.dir)}"><span>🗜 Tutti i file (.zip)</span><span class="mut">${fmtSize(l.files.reduce((s, f) => s + f.size, 0))}</span></a>` : ''}
            <a href="javascript:exportLaunch('json')"><span>📋 Dati del lancio (.json)</span><span class="mut">parametri + statistiche</span></a>
            <a href="javascript:exportLaunch('csv')"><span>📊 Dati del lancio (.csv)</span><span class="mut">parametri + statistiche</span></a>
            ${l.files.map(f => `<a href="${fileUrl(l.dir + '/' + f.name, true)}"><span>${esc(f.name)}</span><span class="mut">${fmtSize(f.size)}</span></a>`).join('')}
          </div>
        </div>

        <div class="row" style="justify-content:flex-end">
          <button class="danger" onclick="deleteLaunch()" ${l.recording ? 'disabled' : ''}>🗑 Elimina lancio</button>
        </div>
      </div>
    </div>`;
  Player.attach(l);
  if (l.hasSensors) Sensors.load(l);
}

function current() {
  const r = route();
  return S.launches.find(x => x.id === r.id);
}

async function saveMeta() {
  const l = current();
  if (!l) return;
  const meta = {...l.meta, name: $('#f_name').value.trim(), notes: $('#f_notes').value.trim(), updated: Math.floor(Date.now() / 1000)};
  for (const f of FIELDS) {
    const v = $('#f_' + f.k).value.trim();
    meta[f.k] = f.text ? v : v === '' ? null : parseFloat(v);
  }
  try {
    await api('/api/launch?dir=' + encodeURIComponent(l.dir), {method: 'POST', body: JSON.stringify(meta)});
    l.meta = meta;
    l.name = meta.name || `Lancio #${l.n}`;
    toast('Salvato');
  } catch (e) {
    toast('Errore: ' + e.message);
  }
}

function exportLaunch(fmt) {
  const l = current();
  if (!l) return;
  const data = {
    lancio: l.id, nome: l.name, data: l.date ? l.date.toISOString() : null,
    parametri: Object.fromEntries([...FIELDS.map(f => [f.k, l.meta[f.k] ?? null]), ['notes', l.meta.notes ?? '']]),
    // Riepilogo calcolato dai grafici (tempi in ms dall'inizio della registrazione)
    volo: l.flight ?? null,
    registrazione: l.info,
    file: l.files,
  };
  if (fmt === 'json') return download(`${l.id}.json`, JSON.stringify(data, null, 2), 'application/json');
  const rows = [['campo', 'valore'], ['lancio', l.id], ['nome', l.name], ['data', data.data ?? ''],
    ...Object.entries(data.parametri),
    ...Object.entries(l.flight ?? {}).map(([k, v]) => ['volo_' + k, v ?? '']),
    ...Object.entries(l.info)];
  download(`${l.id}.csv`, rows.map(r => r.map(csvCell).join(',')).join('\n'), 'text/csv');
}

async function deleteLaunch() {
  const l = current();
  if (!l || !confirm(`Eliminare "${l.name}"? Video, audio e dati andranno persi.`)) return;
  try {
    await api('/api/delete?dir=' + encodeURIComponent(l.dir), {method: 'POST'});
    toast('Lancio eliminato');
    await loadLaunches();
    location.hash = '#/';
  } catch (e) {
    toast('Errore: ' + e.message);
  }
}

// ---------- player in streaming: frame JPEG da video.mjpeg (offset da frames.csv) + audio.wav ----------
// Niente download completo: frames.csv (pochi KB) dice dove sta ogni frame nel file, poi i
// frame si scaricano a blocchi di CHUNK con richieste HTTP Range, precaricando i successivi
// durante la riproduzione. L'audio lo scarica il browser in streaming (anche lui con Range).
const Player = {
  CHUNK: 10,        // frame per richiesta (~200 KB in SVGA)
  AHEAD: 3,         // blocchi precaricati davanti durante la riproduzione (~1 s)
  MAX_CACHE: 60,    // blocchi tenuti in memoria (~12 MB), i più vecchi si scartano
  MAX_INFLIGHT: 2,  // richieste in parallelo verso l'ESP32
  l: null, frames: [], audio: null, dur: 0, idx: -1, url: null, want: -1, pending: -1,
  cache: new Map(), inflight: 0, gen: 0,
  playing: false, pos: 0, base: 0, wall: 0, speed: 1, raf: 0,

  attach(l) {
    this.l = l;
    if (l.hasVideo && !l.recording) this.load();
  },

  close() {
    cancelAnimationFrame(this.raf);
    if (this.audio) { this.audio.pause(); this.audio.src = ''; }
    if (this.url) URL.revokeObjectURL(this.url);
    this.gen++;  // le risposte ancora in viaggio del lancio precedente vengono ignorate
    Object.assign(this, {l: null, frames: [], audio: null, url: null, idx: -1, want: -1, pending: -1,
      cache: new Map(), inflight: 0, playing: false, pos: 0, speed: 1});
  },

  async load() {
    const l = this.l, gen = this.gen;
    try {
      const csv = await (await api(fileUrl(l.dir + '/frames.csv'))).text();
      if (gen !== this.gen) return;
      const vsize = l.files.find(f => f.name === 'video.mjpeg').size;
      let off = 0;
      this.frames = [];
      for (const line of csv.trim().split('\n').slice(1)) {
        const [, t, bytes] = line.split(',').map(Number);
        if (!bytes || off + bytes > vsize) break;  // CSV e video possono differire se è saltata la corrente
        this.frames.push({t, off, len: bytes});
        off += bytes;
      }
      if (!this.frames.length) throw new Error('nessun frame');
      if (l.files.some(f => f.name === 'audio.wav')) {
        this.audio = new Audio(fileUrl(l.dir + '/audio.wav'));
        this.audio.preload = 'auto';
      }
      this.dur = this.frames[this.frames.length - 1].t;
      $('#pload').classList.add('hidden');
      $('#pctrl').classList.remove('hidden');
      $('#pseek').max = this.dur;
      this.show(0);
    } catch (e) {
      $('#pload').textContent = 'Video non disponibile: ' + e.message;
    }
  },

  // Blocco k di frame: una richiesta Range sola per CHUNK frame consecutivi
  chunk(k) {
    if (this.cache.has(k)) {
      const p = this.cache.get(k);
      this.cache.delete(k);  // in fondo alla Map = usato di recente
      this.cache.set(k, p);
      return p;
    }
    const first = this.frames[k * this.CHUNK];
    const last = this.frames[Math.min(this.frames.length, (k + 1) * this.CHUNK) - 1];
    const gen = this.gen;
    this.inflight++;
    const p = fetch(fileUrl(this.l.dir + '/video.mjpeg'),
                    {headers: {Range: `bytes=${first.off}-${last.off + last.len - 1}`}})
      .then(r => {
        if (r.status !== 206) throw new Error('il server non supporta lo streaming (HTTP ' + r.status + ')');
        return r.arrayBuffer();
      })
      .then(buf => ({base: first.off, buf}))
      .catch(e => { this.cache.delete(k); throw e; })
      .finally(() => {
        if (gen !== this.gen) return;
        this.inflight--;
        if (this.want >= 0 && this.want !== this.idx) this.show(this.want);  // richiesta rimasta in attesa
        this.prefetch();
      });
    this.cache.set(k, p);
    while (this.cache.size > this.MAX_CACHE) this.cache.delete(this.cache.keys().next().value);
    return p;
  },

  // Durante la riproduzione tiene pronti i blocchi successivi
  prefetch() {
    if (!this.playing || this.idx < 0) return;
    const k0 = Math.floor(this.idx / this.CHUNK);
    const kMax = Math.floor((this.frames.length - 1) / this.CHUNK);
    for (let k = k0; k <= Math.min(kMax, k0 + this.AHEAD) && this.inflight < this.MAX_INFLIGHT; k++) {
      if (!this.cache.has(k)) this.chunk(k);
    }
  },

  frameAt(t) {
    let lo = 0, hi = this.frames.length - 1;
    while (lo < hi) {
      const mid = (lo + hi + 1) >> 1;
      if (this.frames[mid].t <= t) lo = mid; else hi = mid - 1;
    }
    return lo;
  },

  show(i) {
    this.want = i;
    $('#ptime').textContent = `${(this.pos / 1000).toFixed(2)} / ${(this.dur / 1000).toFixed(2)} s`;
    $('#pseek').value = this.pos;
    Sensors.cursor(this.pos);
    if (i === this.idx || i === this.pending) return;  // già mostrato o già in arrivo

    const k = Math.floor(i / this.CHUNK);
    $('#pframe').textContent = `frame ${i + 1} / ${this.frames.length} ⏳`;
    // Trascinando veloce non si accodano decine di richieste: se le linee sono occupate
    // si aspetta, e alla prima risposta si chiede l'ultimo frame voluto (vedi chunk())
    if (!this.cache.has(k) && this.inflight >= this.MAX_INFLIGHT) return;
    const gen = this.gen;
    this.pending = i;
    this.chunk(k).then(({base, buf}) => {
      if (gen !== this.gen || this.want !== i) return;  // nel frattempo si è spostato altrove
      const f = this.frames[i];
      this.idx = i;
      if (this.url) URL.revokeObjectURL(this.url);
      this.url = URL.createObjectURL(new Blob([buf.slice(f.off - base, f.off - base + f.len)], {type: 'image/jpeg'}));
      $('#pimg').src = this.url;
      $('#pframe').textContent = `frame ${i + 1} / ${this.frames.length}`;
      this.prefetch();
    }).catch(e => {
      if (gen === this.gen) $('#pframe').textContent = 'errore: ' + e.message;
    }).finally(() => {
      if (this.pending === i) this.pending = -1;
    });
  },

  // Orologio: a velocità 1× segue l'audio (sincronia perfetta), altrimenti il tempo reale scalato
  clock() {
    if (!this.playing) return this.pos;
    if (this.audio && this.speed === 1 && !this.audio.paused && !this.audio.ended) return this.audio.currentTime * 1000;
    return this.base + (performance.now() - this.wall) * this.speed;
  },

  play() {
    if (!this.frames.length) return;
    if (this.pos >= this.dur) this.pos = 0;
    this.playing = true;
    this.base = this.pos;
    this.wall = performance.now();
    if (this.audio && this.speed === 1) {
      this.audio.currentTime = this.pos / 1000;
      this.audio.play().catch(() => {});
    }
    $('#pplay').textContent = '⏸';
    this.tick();
  },

  pause() {
    this.pos = this.clock();
    this.playing = false;
    if (this.audio) this.audio.pause();
    cancelAnimationFrame(this.raf);
    const b = $('#pplay');
    if (b) b.textContent = '▶';
  },

  toggle() { this.playing ? this.pause() : this.play(); },

  tick() {
    if (!this.playing) return;
    const t = this.clock();
    if (t >= this.dur) {
      this.pos = this.dur;
      this.pause();
      this.show(this.frames.length - 1);
      return;
    }
    this.pos = t;
    this.show(this.frameAt(t));
    this.raf = requestAnimationFrame(() => this.tick());
  },

  seek(t) {
    if (!this.frames.length) return;
    const was = this.playing;
    if (was) this.pause();
    this.pos = Math.max(0, Math.min(this.dur, t));
    this.show(this.frameAt(this.pos));
    if (was) this.play();
  },

  step(d) {
    if (!this.frames.length) return;
    this.pause();
    const i = Math.max(0, Math.min(this.frames.length - 1, (this.want >= 0 ? this.want : this.idx) + d));
    this.pos = this.frames[i].t;
    this.show(i);
  },

  setSpeed(s) {
    const was = this.playing;
    if (was) this.pause();
    this.speed = s;
    if (was) this.play();
  },
};

document.addEventListener('keydown', e => {
  if (!Player.frames.length || /INPUT|TEXTAREA|SELECT/.test(document.activeElement.tagName)) return;
  if (e.key === ' ') { e.preventDefault(); Player.toggle(); }
  if (e.key === 'ArrowLeft') Player.step(-1);
  if (e.key === 'ArrowRight') Player.step(1);
});

// ---------- riepilogo del volo, calcolato da sensors.csv ----------
// Soglie, coerenti con il firmware (flight.cpp): la spinta è il tratto iniziale con |a| oltre
// THRUST_G; l'impatto è l'ultimo istante "mosso" (|a| lontano da 1 g) prima dei 3 s di quiete
// che fanno dichiarare l'atterraggio. FULL_SCALE_G = fondo scala dell'MPU6050 (±16 g).
const THRUST_G = 2, MOVING_G = 0.3, FULL_SCALE_G = 15.5;

function flightStats(head, rows) {
  const col = p => head.findIndex(h => h.startsWith(p));
  const iAcc = col('|acc|'), iAlt = col('quota');
  // "in volo (1 = sì)"; nei primi lanci la colonna era "fase (0 terra 1 volo 2 atterrato)"
  const iFly = col('in volo') >= 0 ? col('in volo') : col('fase');
  if (iAcc < 0 || iFly < 0 || !rows.length) return null;
  const n = rows.length, t = i => rows[i][0], a = i => rows[i][iAcc];
  const flying = i => rows[i][iFly] === 1;

  let flights = 0;
  for (let i = 0; i < n; i++) if (flying(i) && (i === 0 || !flying(i - 1))) flights++;
  const launch = rows.findIndex((r, i) => flying(i));
  if (launch < 0) return {flights: 0};

  // Il firmware dichiara il decollo dopo 30 ms di spinta: l'inizio vero è qualche campione prima
  let start = launch;
  while (start > 0 && a(start - 1) > THRUST_G) start--;
  let end = launch;
  while (end < n && flying(end)) end++;
  const landed = end < n;
  let impact = end - 1;
  if (landed) while (impact > start && Math.abs(a(impact) - 1) <= MOVING_G) impact--;

  let peak = start, burn = start, apo = -1;
  for (let i = start; i <= impact; i++) if (a(i) > a(peak)) peak = i;
  while (burn + 1 < n && a(burn + 1) > THRUST_G) burn++;
  if (iAlt >= 0) {
    for (let i = start; i <= impact; i++) {
      if (!isNaN(rows[i][iAlt]) && (apo < 0 || rows[i][iAlt] > rows[apo][iAlt])) apo = i;
    }
  }
  const t0 = t(start);
  return {
    flights, landed, launchT: t0,
    burnS: (t(burn) - t0) / 1000,
    peakG: a(peak), peakT: t(peak),
    apogeeM: apo >= 0 ? rows[apo][iAlt] : null, apogeeT: apo >= 0 ? t(apo) : null,
    impactT: landed ? t(impact) : null,
    flightS: landed ? (t(impact) - t0) / 1000 : null,
    descentMs: landed && apo >= 0 && t(impact) > t(apo) ? rows[apo][iAlt] / ((t(impact) - t(apo)) / 1000) : null,
  };
}

function seekTo(t) {
  Player.frames.length ? Player.seek(t) : Sensors.cursor(t);
}

function renderFlight(l, f) {
  const box = $('#flightcard .fbody');
  if (!box) return;
  box.className = 'fbody';
  if (!f) {
    box.innerHTML = '<span class="mut">In questi dati non c\'è la colonna del volo: riepilogo non disponibile.</span>';
    return;
  }
  if (!f.flights) {
    box.innerHTML = '<span class="mut">Nessun decollo riconosciuto in questa registrazione.</span>';
    return;
  }
  const s = ms => (ms / 1000).toFixed(2) + ' s';
  const stat = (v, label, t) => t == null ? `<div class="stat"><b>${v}</b><span>${label}</span></div>`
    : `<div class="stat click" onclick="seekTo(${t})" title="Vai a questo istante"><b>${v}</b><span>${label}</span></div>`;
  const items = [
    f.apogeeM != null ? stat(`${f.apogeeM.toFixed(1)} m`, `quota massima, a ${s(f.apogeeT - f.launchT)}`, f.apogeeT) : '',
    stat(`${f.peakG.toFixed(1)} g`, `picco di accelerazione, a ${s(f.peakT - f.launchT)}`, f.peakT),
    stat(`${f.burnS.toFixed(2)} s`, `spinta (|a| oltre ${THRUST_G} g)`, f.launchT),
    stat(f.flightS != null ? `${f.flightS.toFixed(1)} s` : '–',
      f.landed ? 'tempo di volo, fino all\'impatto' : 'volo non concluso nella registrazione', f.impactT),
    f.descentMs != null ? stat(`${f.descentMs.toFixed(1)} m/s`, 'discesa media', null) : '',
  ];
  const notes = [];
  const est = l.meta.altitude_m;
  if (f.apogeeM != null && est) {
    const d = f.apogeeM - est;
    notes.push(`Quota stimata ${est} m: misurata ${d >= 0 ? '+' : ''}${d.toFixed(1)} m (${d >= 0 ? '+' : ''}${(d / est * 100).toFixed(0)}%).`);
  }
  if (f.peakG >= FULL_SCALE_G) notes.push('⚠ Il picco ha raggiunto il fondo scala del sensore (16 g): quello vero può essere più alto.');
  if (f.flights > 1) notes.push(`La registrazione contiene ${f.flights} voli: il riepilogo è del primo.`);
  box.innerHTML = `<div class="stats">${items.join('')}</div>` +
    (notes.length ? `<div class="mut" style="margin-top:10px;font-size:13px">${notes.join('<br>')}</div>` : '') +
    `<div class="mut" style="margin-top:8px;font-size:12px">Clic su un valore per vedere quell'istante nel video.</div>`;
}

// ---------- grafici sensori: sensors.csv, prima colonna = tempo (ms) ----------
const Sensors = {
  series: [], xs: [], t: null,
  events: [],  // istanti notevoli del volo, disegnati come linee verticali: {t, label}

  async load(l) {
    this.series = [];
    this.events = [];
    try {
      const txt = await (await api(fileUrl(l.dir + '/sensors.csv'))).text();
      const lines = txt.trim().split(/\r?\n/);
      const head = lines[0].split(',').map(s => s.trim());
      const rows = lines.slice(1).map(r => r.split(',').map(Number)).filter(r => r.length === head.length && !isNaN(r[0]));
      this.xs = rows.map(r => r[0]);
      const f = flightStats(head, rows);
      l.flight = f;
      renderFlight(l, f);
      if (f?.flights) {
        this.events = [{t: f.launchT, label: 'decollo'}, {t: f.apogeeT, label: 'apogeo'},
          {t: f.impactT, label: 'impatto'}].filter(e => e.t != null);
      }
      const box = $('#sensors');
      box.className = '';
      box.innerHTML = `<div class="mut">${rows.length} campioni · clic sul grafico per spostare il video a quell'istante</div>`;
      for (let c = 1; c < head.length; c++) {
        const ys = rows.map(r => r[c]);
        const div = document.createElement('div');
        div.className = 'chart';
        const valid = ys.filter(v => !isNaN(v));
        div.innerHTML = `<div class="row between"><b style="font-size:14px">${esc(head[c])}</b>
          <span class="mut tnum">min ${Math.min(...valid).toFixed(2)} · max ${Math.max(...valid).toFixed(2)}</span></div><canvas></canvas>`;
        box.appendChild(div);
        const canvas = $('canvas', div);
        this.bindScrub(canvas);
        this.series.push({canvas, ys, name: head[c]});
      }
      this.drawAll();
    } catch (e) {
      $('#sensors').textContent = 'Impossibile leggere sensors.csv: ' + e.message;
      renderFlight(l, null);
    }
  },

  // Clic o trascinamento (mouse e dito) sul grafico: video e cursore seguono il puntatore.
  // Gli spostamenti si raggruppano a uno per fotogramma dello schermo, e durante il
  // trascinamento la riproduzione resta in pausa (riparte al rilascio).
  bindScrub(canvas) {
    let dragging = false, resume = false, pending = null;
    const timeAt = e => {
      const s = this.series.find(x => x.canvas === canvas);
      const x = Math.max(s.padL, Math.min(s.padL + s.w, e.offsetX));
      return s.x0 + (x - s.padL) / s.w * (s.x1 - s.x0);
    };
    const go = e => {
      const first = pending == null;
      pending = timeAt(e);
      if (!first) return;
      requestAnimationFrame(() => {
        Player.frames.length ? Player.seek(pending) : this.cursor(pending);
        pending = null;
      });
    };
    canvas.onpointerdown = e => {
      dragging = true;
      canvas.setPointerCapture(e.pointerId);
      resume = Player.playing;
      if (resume) Player.pause();
      go(e);
    };
    canvas.onpointermove = e => { if (dragging) go(e); };
    canvas.onpointerup = canvas.onpointercancel = () => {
      if (!dragging) return;
      dragging = false;
      if (resume) Player.play();
    };
  },

  cursor(t) {
    this.t = t;
    if (this.series.length) this.drawAll();
  },

  drawAll() {
    for (const s of this.series) this.draw(s);
  },

  draw(s) {
    const c = s.canvas, dpr = window.devicePixelRatio || 1;
    const W = c.clientWidth, H = c.clientHeight;
    if (!W) return;
    c.width = W * dpr;
    c.height = H * dpr;
    const g = c.getContext('2d');
    g.scale(dpr, dpr);
    const css = getComputedStyle(document.documentElement);
    const acc = css.getPropertyValue('--acc'), mut = css.getPropertyValue('--mut'), bd = css.getPropertyValue('--bd');
    const xs = this.xs, ys = s.ys;
    const padL = 46, padR = 8, padT = 8, padB = 20;
    const w = W - padL - padR, h = H - padT - padB;
    const x0 = xs[0], x1 = xs[xs.length - 1] || x0 + 1;
    const valid = ys.filter(v => !isNaN(v));
    let y0 = Math.min(...valid), y1 = Math.max(...valid);
    if (y0 === y1) { y0 -= 1; y1 += 1; }
    const m = (y1 - y0) * 0.08;
    y0 -= m; y1 += m;
    Object.assign(s, {padL, w, x0, x1});
    const X = x => padL + (x - x0) / (x1 - x0) * w;
    const Y = y => padT + (1 - (y - y0) / (y1 - y0)) * h;

    g.font = '11px system-ui, sans-serif';
    g.fillStyle = mut;
    g.strokeStyle = bd;
    g.lineWidth = 1;
    for (let i = 0; i <= 4; i++) {
      const v = y0 + (y1 - y0) * i / 4, y = Y(v);
      g.beginPath(); g.moveTo(padL, y); g.lineTo(W - padR, y); g.stroke();
      g.textAlign = 'right'; g.textBaseline = 'middle';
      g.fillText(Math.abs(y1 - y0) < 10 ? v.toFixed(2) : v.toFixed(0), padL - 6, y);
    }
    g.textAlign = 'center'; g.textBaseline = 'top';
    for (let i = 0; i <= 5; i++) {
      const v = x0 + (x1 - x0) * i / 5;
      g.fillText((v / 1000).toFixed(1) + ' s', X(v), H - padB + 5);
    }

    // Decollo, apogeo, impatto: linee tratteggiate sotto la curva
    g.save();
    g.setLineDash([4, 4]);
    g.strokeStyle = mut;
    g.textAlign = 'left'; g.textBaseline = 'top';
    for (const e of this.events) {
      if (e.t < x0 || e.t > x1) continue;
      const x = X(e.t);
      g.beginPath(); g.moveTo(x, padT); g.lineTo(x, padT + h); g.stroke();
      g.fillText(e.label, x + 3, padT + h - 14);
    }
    g.restore();

    g.strokeStyle = acc;
    g.lineWidth = 1.6;
    g.beginPath();
    let pen = false;
    for (let i = 0; i < xs.length; i++) {
      if (isNaN(ys[i])) { pen = false; continue; }
      pen ? g.lineTo(X(xs[i]), Y(ys[i])) : g.moveTo(X(xs[i]), Y(ys[i]));
      pen = true;
    }
    g.stroke();

    if (this.t != null && this.t >= x0 && this.t <= x1) {
      const x = X(this.t);
      g.strokeStyle = css.getPropertyValue('--rec');
      g.lineWidth = 1;
      g.beginPath(); g.moveTo(x, padT); g.lineTo(x, padT + h); g.stroke();
      let i = 0;
      while (i < xs.length - 1 && xs[i + 1] <= this.t) i++;
      if (!isNaN(ys[i])) {
        g.fillStyle = css.getPropertyValue('--rec');
        g.beginPath(); g.arc(x, Y(ys[i]), 3.5, 0, 7); g.fill();
        g.textAlign = x > W - 80 ? 'right' : 'left'; g.textBaseline = 'top';
        g.fillText(ys[i].toFixed(2), x + (x > W - 80 ? -6 : 6), padT);
      }
    }
  },
};
window.addEventListener('resize', () => Sensors.drawAll());

// ---------- avvio ----------
window.addEventListener('hashchange', render);
(async () => {
  render();
  await pollStatus();
  try {
    await loadLaunches();
  } catch (e) {
    toast('Errore: ' + e.message);
  }
  render();
  setInterval(pollStatus, 2000);
})();
</script>
</body>
</html>
)HTML";
