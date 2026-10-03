#pragma once
#include <Arduino.h>

// Pagina servita su "/": stato, registrazione, anteprima, sessioni e player.
// Il player ricostruisce il video da video.mjpeg usando le dimensioni dei
// frame in frames.csv e lo sincronizza con audio.wav.
static const char WEB_PAGE[] PROGMEM = R"HTML(<!doctype html>
<html lang="it">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Water Rocket</title>
<style>
:root{--bg:#f4f6f8;--card:#fff;--fg:#1b2430;--mut:#64707d;--acc:#0a7cff;--rec:#e0283b;--bd:#dde3e9}
@media (prefers-color-scheme:dark){:root{--bg:#10151b;--card:#1a222b;--fg:#e8edf2;--mut:#93a0ad;--acc:#4ea1ff;--rec:#ff5466;--bd:#2a3440}}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--fg);font:15px/1.4 system-ui,-apple-system,Segoe UI,Roboto,sans-serif}
main{max-width:720px;margin:0 auto;padding:16px}
h1{font-size:22px;margin:4px 0 16px}
h2{font-size:16px;margin:0 0 10px}
.card{background:var(--card);border:1px solid var(--bd);border-radius:12px;padding:14px;margin-bottom:14px}
.row{display:flex;gap:8px;flex-wrap:wrap;align-items:center}
.mut{color:var(--mut);font-size:13px}
button,.btn{appearance:none;border:1px solid var(--bd);background:var(--card);color:var(--fg);border-radius:8px;padding:9px 14px;font:inherit;cursor:pointer;text-decoration:none;display:inline-block}
button.pri{background:var(--acc);border-color:var(--acc);color:#fff}
button.rec{background:var(--rec);border-color:var(--rec);color:#fff}
button:disabled{opacity:.5;cursor:default}
.dot{width:10px;height:10px;border-radius:50%;background:var(--mut);display:inline-block}
.dot.on{background:var(--rec);animation:b 1s infinite}
@keyframes b{50%{opacity:.3}}
img.view{width:100%;border-radius:8px;background:#000;display:block;aspect-ratio:4/3;object-fit:contain}
table{width:100%;border-collapse:collapse;font-size:14px}
td{padding:5px 0;border-top:1px solid var(--bd)}
td.r{text-align:right;white-space:nowrap}
.sess{border-top:1px solid var(--bd);padding-top:12px;margin-top:12px}
.sess:first-of-type{border-top:0;margin-top:0;padding-top:0}
pre{white-space:pre-wrap;margin:6px 0;font-size:12px;color:var(--mut)}
#player{display:none}
</style>
</head>
<body>
<main>
<h1>🚀 Water Rocket</h1>

<div class="card">
  <div class="row" style="justify-content:space-between">
    <div class="row"><span id="dot" class="dot"></span><b id="state">...</b></div>
    <div class="row">
      <button id="bStart" class="rec" onclick="rec('start')">● Registra</button>
      <button id="bStop" onclick="rec('stop')">■ Stop</button>
    </div>
  </div>
  <div id="detail" class="mut" style="margin-top:8px"></div>
</div>

<div class="card" id="player">
  <h2 id="pTitle">Player</h2>
  <img id="pImg" class="view" alt="">
  <div class="row" style="margin-top:8px;justify-content:space-between">
    <span id="pTime" class="mut"></span>
    <div class="row"><button onclick="replay()">↺ Da capo</button><button onclick="closePlayer()">Chiudi</button></div>
  </div>
</div>

<div class="card">
  <div class="row" style="justify-content:space-between">
    <h2 style="margin:0">Anteprima camera</h2>
    <button id="bSnap" onclick="snap()">📷 Scatta</button>
  </div>
  <img id="snap" class="view" alt="" style="margin-top:10px;display:none">
</div>

<div class="card">
  <div class="row" style="justify-content:space-between;margin-bottom:10px">
    <h2 style="margin:0">Registrazioni</h2>
    <button onclick="loadSessions()">↻</button>
  </div>
  <div id="sessions" class="mut">Caricamento...</div>
</div>
</main>

<script>
const $ = id => document.getElementById(id);
const fileUrl = (p, dl) => '/file?path=' + encodeURIComponent(p) + (dl ? '&dl=1' : '');
const fmtSize = b => b > 1048576 ? (b / 1048576).toFixed(1) + ' MB' : b > 1024 ? (b / 1024).toFixed(0) + ' KB' : b + ' B';
let recording = false;

async function status() {
  try {
    const s = await (await fetch('/api/status')).json();
    recording = s.recording;
    $('dot').className = 'dot' + (s.recording ? ' on' : '');
    $('state').textContent = s.recording ? 'Registrazione in corso' : 'Pronto';
    $('bStart').disabled = s.recording || !s.sd;
    $('bStop').disabled = !s.recording;
    $('bSnap').disabled = s.recording;
    $('detail').textContent = s.recording
      ? `${s.dir} · ${s.secs.toFixed(1)} s · ${s.frames} frame (${s.fps.toFixed(1)} fps) · audio ${s.audio.toFixed(1)} s`
      : `Camera ${s.camera} · SD ${s.sd ? s.sdFreeMB + ' MB liberi di ' + s.sdTotalMB : 'non montata'}`;
  } catch (e) {
    $('state').textContent = 'Non connesso';
  }
}

async function rec(action) {
  const r = await fetch('/api/rec/' + action, {method: 'POST'});
  if (!r.ok) alert(await r.text());
  await status();
  if (action === 'stop') loadSessions();
}

function snap() {
  const img = $('snap');
  img.style.display = 'block';
  img.src = '/snapshot.jpg?t=' + Date.now();
}

async function loadSessions() {
  const list = await (await fetch('/api/sessions')).json();
  list.sort((a, b) => b.dir.localeCompare(a.dir));
  if (!list.length) { $('sessions').textContent = 'Nessuna registrazione.'; return; }
  $('sessions').innerHTML = list.map(s => `
    <div class="sess">
      <div class="row" style="justify-content:space-between">
        <b>${s.dir.slice(1)}</b>
        <div class="row">
          <button class="pri" onclick="play('${s.dir}')">▶ Guarda</button>
          <button onclick="del('${s.dir}')">🗑</button>
        </div>
      </div>
      ${s.info ? `<pre>${s.info.trim()}</pre>` : ''}
      <table>${s.files.map(f => `<tr><td><a href="${fileUrl(s.dir + '/' + f.name, true)}">${f.name}</a></td>
        <td class="r mut">${fmtSize(f.size)}</td></tr>`).join('')}</table>
    </div>`).join('');
}

async function del(dir) {
  if (!confirm(`Eliminare ${dir.slice(1)}? I file andranno persi.`)) return;
  const r = await fetch('/api/delete?dir=' + encodeURIComponent(dir), {method: 'POST'});
  if (!r.ok) alert(await r.text());
  loadSessions();
}

// --- Player: frame JPEG da video.mjpeg + timestamp da frames.csv + audio.wav ---
let frames = [], audio = null, raf = 0, t0 = 0, lastUrl = null, idx = 0;

async function play(dir) {
  closePlayer();
  $('player').style.display = 'block';
  $('pTitle').textContent = dir.slice(1);
  $('pTime').textContent = 'Scaricamento...';
  $('player').scrollIntoView({behavior: 'smooth'});
  try {
    // Una richiesta alla volta: il server dell'ESP32 è single-thread
    const csv = await (await fetch(fileUrl(dir + '/frames.csv'))).text();
    const vid = await (await fetch(fileUrl(dir + '/video.mjpeg'))).arrayBuffer();
    const wavRes = await fetch(fileUrl(dir + '/audio.wav'));
    const wav = wavRes.ok ? await wavRes.blob() : null;

    let off = 0;
    frames = [];
    for (const line of csv.trim().split('\n').slice(1)) {
      const [, t, bytes] = line.split(',').map(Number);
      if (!bytes || off + bytes > vid.byteLength) break;  // CSV e video possono differire se è saltata la corrente
      frames.push({t, blob: new Blob([vid.slice(off, off + bytes)], {type: 'image/jpeg'})});
      off += bytes;
    }
    if (!frames.length) { $('pTime').textContent = 'Nessun frame'; return; }
    audio = wav ? new Audio(URL.createObjectURL(wav)) : null;
    replay();
  } catch (e) {
    $('pTime').textContent = 'Errore: ' + e;
  }
}

function replay() {
  cancelAnimationFrame(raf);
  idx = 0;
  if (lastUrl) { URL.revokeObjectURL(lastUrl); lastUrl = null; }  // forza il ridisegno del primo frame
  t0 = performance.now();
  if (audio) { audio.currentTime = 0; audio.play().catch(() => {}); }
  tick();
}

function tick() {
  // L'audio fa da orologio, così video e audio restano sincronizzati
  const now = audio && !audio.paused ? audio.currentTime * 1000 : performance.now() - t0;
  if (idx > 0 && now < frames[idx - 1].t) idx = 0;
  let next = idx;
  while (next < frames.length - 1 && frames[next + 1].t <= now) next++;
  if (next !== idx || !lastUrl) {
    idx = next;
    if (lastUrl) URL.revokeObjectURL(lastUrl);
    lastUrl = URL.createObjectURL(frames[idx].blob);
    $('pImg').src = lastUrl;
  }
  const end = frames[frames.length - 1].t;
  $('pTime').textContent = `${(Math.min(now, end) / 1000).toFixed(1)} / ${(end / 1000).toFixed(1)} s · frame ${idx + 1}/${frames.length}`;
  if (now < end) raf = requestAnimationFrame(tick);
}

function closePlayer() {
  cancelAnimationFrame(raf);
  if (audio) { audio.pause(); URL.revokeObjectURL(audio.src); audio = null; }
  if (lastUrl) { URL.revokeObjectURL(lastUrl); lastUrl = null; }
  frames = [];
  $('player').style.display = 'none';
}

status();
loadSessions();
setInterval(status, 2000);
</script>
</body>
</html>
)HTML";
