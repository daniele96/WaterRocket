// =====================================================================
// Cintura portaelettronica del razzo ad acqua
// =====================================================================
// Due pod a 180° sulla bottiglia, tenuti da due giri di elastico:
//   - pod elettronica: XIAO ESP32-S3 Sense con la camera inclinata verso
//     l'ugello, MPU6050 e BMP180 sul tetto, cicalino sul fianco, anello LED
//     fuori sul tetto, antenna fuori sulla faccia della camera, accanto all'obiettivo
//   - pod batteria: LiPo 602040, stessa sagoma esterna (resistenza simmetrica)
//   - slitta: piastrina su cui si monta la XIAO, si infila nel pod e si
//     blocca con due spezzoni di filamento da 1,75 infilati da fuori
//
// Riferimento del pod: X lungo l'asse del razzo (+ verso l'ogiva), Y tangente
// alla bottiglia, Z radiale verso l'esterno. Z = 0 è la superficie della
// bottiglia sotto la mezzeria del pod; la bottiglia è il cilindro di raggio R
// con asse lungo X a Z = -R.
//
// Il pod è aperto verso la bottiglia: il fondo è la bottiglia stessa.
// L'elastico entra dalle finestre sui fianchi, attraversa il pod rasente la
// bottiglia e preme sulla parete sotto la finestra ("ponte").
//
// Stampa (PETG): pod capovolti col tetto sul piatto, slitta in piano.
// Nessun supporto finché inclinazione_camera e angolo_muso restano >= 45°.
// =====================================================================

/* [Cosa generare] */
// assieme = vista d'insieme; le altre parti escono già orientate per la stampa
parte = "assieme"; // [assieme, spaccato, spaccato_antenna, spaccato_batteria, pod_elettronica, pod_batteria, slitta, stampa_tutto]
mostra_componenti = true;
// Porta la vista sul pezzo a ogni anteprima (F5); toglila per ruotare liberamente
inquadratura_automatica = true;
// Controlli di ingombro (devono risultare vuoti): vedi README
verifica = "nessuna"; // [nessuna, componenti, estrazione_slitta, batteria, quote]

/* [Bottiglia] */
// Parte dritta: circonferenza misurata / 3,1416
diametro_bottiglia = 65;
// Striscia di gomma incollata sotto i bordi (camera d'aria di bici); 0 = niente
spessore_gomma = 0.8;

/* [Elastico] */
elastico_larghezza = 20;
elastico_spessore = 2;
// Gioco dell'elastico nelle finestre
gioco_elastico = 0.8;
// Parete sotto la finestra su cui preme l'elastico
altezza_ponte = 2.5;

/* [Camera] */
// Verso l'ugello: 45 = a metà, 90 = dritta lungo la bottiglia. Sotto 45 servono supporti
inclinazione_camera = 45; // [45:5:90]
// Dal fondo della scheda XIAO alla faccia interna del pod (sopra il flat della camera)
quota_faccia = 11.4;
diametro_foro_obiettivo = 8.6;

/* [Guscio] */
larghezza_pod = 36;
parete = 1.6;
raggio_spigoli = 2;
// Inclinazione del muso: sotto i 45 gradi serve il supporto
angolo_muso = 45; // [45:5:75]

/* [Antenna] */
// Antenna FPC Seeed: fuori sulla faccia della camera, accanto all'obiettivo, in una sede
// da 0,4 mm, col lato saldato contro la faccia e la saldatura in basso; sopra il nastro.
// Se serve il tetto si alza da solo per farla stare.
antenna_lunghezza = 37;
antenna_larghezza = 17;
// Centro della saldatura del cavetto, guardando l'antenna dal lato saldato con la
// saldatura in basso: distanza dal bordo corto in basso e dal bordo lungo di destra
antenna_attacco = 13.7;
antenna_attacco_lato = 10.5;
// Cavetto, dallo spinotto alla saldatura
antenna_cavo = 30;

/* [Batteria] */
batteria_lunghezza = 42;  // compresa la protezione (PCM)
batteria_larghezza = 20;
batteria_spessore = 6;
// Spazio davanti alla batteria per i connettori JST
spazio_connettori = 10;

/* [Fissaggi] */
// Perni della slitta: spezzoni di filamento
diametro_perno = 1.75;
// Foro pilota per viti autofilettanti nei sensori (M2.5: 2.0, M2: 1.6)
foro_vite = 2.0;
// Rivetti dell'anello LED: filamento da 1,75 ribattuto a caldo, o vite M2
foro_rivetto = 1.9;
angolo_fori_anello = 60;

/* [Hidden] */
$fn = 48;
R = diametro_bottiglia / 2;
W = larghezza_pod;
Wi = W - 2 * parete;
t = inclinazione_camera;
st = sin(t);
ct = cos(t);
E = [ct, 0, st];   // lungo la scheda, dal bordo basso a quello alto
D = [-st, 0, ct];  // asse dell'obiettivo: verso l'esterno e verso l'ugello
cot_m = cos(angolo_muso) / sin(angolo_muso);
ew = elastico_larghezza + gioco_elastico;

function zb(y) = sqrt(R * R - y * y) - R;  // superficie della bottiglia

// ---------------------------------------------------------------------
// XIAO ESP32-S3 Sense: misure dal modello STEP ufficiale Seeed.
// Coordinate della scheda: u = larghezza (17,78), v = lunghezza dal lato
// opposto all'USB-C (20,95), w = altezza dal fondo della scheda principale.
// ---------------------------------------------------------------------
PCB_U = 17.78;
PCB_V = 20.95;
PCB_W = 1.25;
SD_V1 = 24.06;     // fine della microSD inserita: sporge oltre l'USB-C
CAM_U = 8.25;      // centro dell'obiettivo
CAM_V = 17.42;
// [u0, u1, v0, v1, w0, w1]
XIAO_BOXES = [
  [0, 17.78, 0, 20.95, 0, 1.25],            // scheda principale
  [2.59, 15.19, 3.64, 14.24, 0.55, 3.25],   // schermatura
  [4.41, 13.35, 15.18, 22.48, 0.26, 4.46],  // USB-C
  [0.5, 17.28, -0.3, 14.37, 4.18, 5.43],    // scheda Sense
  [2.03, 14.12, 8.77, 24.06, 5.37, 8.5],    // microSD
  [0.81, 17.03, 0.49, 5.83, 5.42, 7.88],    // connettore della camera
  [2.25, 14.25, 6.46, 13.46, 5.85, 11.08],  // flat della camera (ad arco)
  [4.25, 12.25, 13.42, 21.42, 8.2, 10.3]    // corpo della camera
];
LENS = [[7.85, 10.3, 12.2], [6.32, 12.2, 12.95], [5.3, 12.95, 13.96]];  // [diametro, w0, w1]

// Slitta
SO = 2.2;      // distanziali: sotto la scheda passano i fili della batteria
SL_T = 1.4;    // spessore della piastra
LIP = 1.0;     // bordini che tengono la scheda agli angoli
LIP_H = 0.9;
CLR = 0.15;
R_PIN = diametro_perno / 2;
PIN_U = [2.0, PCB_U - 2.0];
SL_U0 = -(CLR + LIP);
SL_U1 = PCB_U + CLR + LIP;
SL_W0 = -(SO + SL_T);
SL_W1 = -SO;
W_PIN = SL_W0 + 0.7 - R_PIN;  // il perno affonda 0,7 mm nella gola della slitta

// Altezza del fondo della scheda: la slitta e i perni restano sopra l'elastico
Z_MIN = elastico_spessore + 1.0;
Z_O = max(Z_MIN - SL_U0 * st - SL_W0 * ct,
          Z_MIN - PIN_U[0] * st - W_PIN * ct + R_PIN + 0.2);
// La faccia della camera (esterna) passa per X = 0, Z = 0
K_O = (Z_O + (parete + quota_faccia) * ct) / st;
O = [(parete + quota_faccia) * st + K_O * ct, 0, Z_O];
Y_V0 = Wi / 2 - 0.4 - SD_V1;  // la microSD arriva a 0,4 mm dal fianco Y+
SL_V0 = -Wi / 2 + 0.3 - Y_V0;
SL_V1 = Wi / 2 - 0.3 - Y_V0;

function bp(c) = [O.x + c[0] * E.x + c[2] * D.x, Y_V0 + c[1], O.z + c[0] * E.z + c[2] * D.z];
function corners(b) = [for (u = [b[0], b[1]], v = [b[2], b[3]], w = [b[4], b[5]]) [u, v, w]];
SLED_BOXES = [[SL_U0, SL_U1, SL_V0, SL_V1, SL_W0, SL_W1],
              [SL_U0, SL_U1, SL_V0, SL_V1, SL_W1, LIP_H]];
STACK_PTS = [for (b = concat(XIAO_BOXES, SLED_BOXES)) for (c = corners(b)) bp(c)];

// Antenna fuori sulla faccia della camera, lungo la pendenza, fra il fianco Y- e
// l'obiettivo (coordinate della scheda: la faccia esterna è a w = W_FACCIA)
W_FACCIA = parete + quota_faccia;
function u_faccia(z) = (z - O.z - W_FACCIA * ct) / st;
ANT_V0 = -W / 2 + 0.7 - Y_V0;              // bordo verso il fianco Y-
ANT_V1 = ANT_V0 + antenna_larghezza;      // bordo verso l'obiettivo
// Il fondo della faccia segue la bottiglia: è più alto vicino al centro del pod
ANT_U0 = u_faccia(-R + sqrt(pow(R + spessore_gomma, 2) - pow(Y_V0 + ANT_V1 + 0.3, 2))) + 0.8;
ANT_U1 = ANT_U0 + antenna_lunghezza;
H_ANT = O.z + W_FACCIA * ct + (ANT_U1 + 0.8) * st;  // tetto che serve all'antenna
// Fessura del cavetto sotto la saldatura, aperta verso la bottiglia: il cavetto entra
// ed esce di lato, perché l'U.FL della XIAO sta sotto la scheda Sense e da montata non
// si stacca
FESSURA = 3.5;
U_FES = ANT_U0 + antenna_attacco + 1.0;
V_FES = ANT_V1 - antenna_attacco_lato;
// Spinotto U.FL (dal modello Seeed) e percorso del cavetto: esce dal lato lungo della
// scheda verso la bottiglia, sale verso la faccia ed entra nella fessura da dentro
UFL = [2.61, 5.71, 0.22, 3.22, 1.25, 3.65];
CAVO_PTS = [for (c = [[4.16, 1.72, 3.0], [-0.8, 1.5, 3.0], [-3, (1.5 + V_FES) / 2, 8],
                      [ANT_U0 + antenna_attacco + 0.5, V_FES, quota_faccia - 0.6],
                      [ANT_U0 + antenna_attacco, V_FES, W_FACCIA - 0.6]]) bp(c)];
CAVO_MIN = sum_len(CAVO_PTS, 0);
function sum_len(p, i) = i >= len(p) - 1 ? 0 : norm(p[i + 1] - p[i]) + sum_len(p, i + 1);

H_STACK = max([for (p = STACK_PTS) p.z]) + 0.6;
H_IN = max(H_STACK, H_ANT - parete);           // soffitto interno
H = H_IN + parete;                            // tetto esterno

// Facce inclinate: posteriore (camera) e muso
function xa(z) = z * ct / st;                 // faccia della camera, esterna
function xai(z) = (z * ct + parete) / st;     // faccia della camera, interna

// Finestre dell'elastico
Z_BAR = zb(W / 2) + spessore_gomma + altezza_ponte;
Z_WIN1 = Z_BAR + elastico_spessore + gioco_elastico;

// Anello LED sul tetto, il più indietro possibile
RING_D = 32;
X_RING = xa(H) + RING_D / 2 + 1.0;
X_RING_HOLE = X_RING - 5;  // passaggio dei fili, dentro il foro centrale dell'anello

// Sensori sul soffitto, davanti alla XIAO (anche durante l'estrazione della slitta)
MPU_L = 15.5; MPU_WD = 20.5; MPU_SO = 3; MPU_DOWN = MPU_SO + 1.6 + 1.5;
BMP_L = 13;   BMP_WD = 10;   BMP_SO = 2.6;
BAND = H_IN - MPU_DOWN - 0.8;
function max_x_sopra(pts, z, dx, dz) = max(concat([-100], [for (p = pts) if (p.z + dz >= z) p.x + dx]));
X_CLEAR = max(max_x_sopra(STACK_PTS, BAND, 0, 0), max_x_sopra(STACK_PTS, BAND, 3 * st, -3 * ct)) + 1.0;
MPU_X0 = max(X_CLEAR, X_RING_HOLE + 3.5 + 1.0);
MPU_Y1 = Wi / 2 - 0.6;
MPU_Y0 = MPU_Y1 - MPU_WD;
BMP_X0 = MPU_X0;
BMP_Y0 = -Wi / 2 + 0.6;
// Fori di fissaggio dei moduli (dalle foto, Ø ~3 mm): le colonnine con le viti ci stanno sopra
FORO_MODULI = 3.2;
MPU_FORI = [for (y = [MPU_Y0 + 2.6, MPU_Y0 + 2.6 + 15.25]) [MPU_X0 + MPU_L - 2.3, y]];
BMP_FORO = [BMP_X0 + BMP_L - 2.4, BMP_Y0 + 2.3];

// Cicalino sul fianco Y+, sotto l'MPU e davanti alla slitta
BZ_D = 12.4; BZ_CUP = 4.4; BZ_OUT = BZ_D + 2.4;
SLED_X_MAX = max([for (p = STACK_PTS) p.x]);
X_BZ = max(MPU_X0 + BZ_OUT / 2, SLED_X_MAX + 1.0 + BZ_OUT / 2);
Z_BZ = H_IN - MPU_DOWN - 1.0 - BZ_OUT / 2;
VENTS = [for (dx = [0, 3.5], dz = [-3, 3]) [X_BZ + BZ_OUT / 2 + 2.5 + dx, Z_BZ + dz]];


// Batteria
Z_ROOF = elastico_spessore + batteria_spessore - 0.4;  // l'elastico la spinge contro le mensole
BATT_SPAN = batteria_lunghezza + spazio_connettori;

// Lunghezza: il muso parte dove finisce l'ultimo ingombro
X_NB = max([
  MPU_X0 + MPU_L + 0.8 + parete / sin(angolo_muso) + H_IN * cot_m,
  X_BZ + BZ_OUT / 2 + 0.8 + parete / sin(angolo_muso) + (Z_BZ + BZ_OUT / 2) * cot_m,
  X_RING + RING_D / 2 + 0.5 + H * cot_m,
  VENTS[3][0] + 2 + parete / sin(angolo_muso) + VENTS[3][1] * cot_m,
  xai(Z_ROOF) + BATT_SPAN + 2 * 1.6 + 1.0 + parete / sin(angolo_muso) + Z_ROOF * cot_m,
  xa(Z_WIN1) + 2 * (2.5 + ew) + 10 + Z_WIN1 * cot_m
]);
function xn(z) = X_NB - z * cot_m;                                  // muso, esterno
function xni(z) = X_NB - parete / sin(angolo_muso) - z * cot_m;     // muso, interno

BELTS = [xa(Z_WIN1) + 2.5 + ew / 2, xn(Z_WIN1) - 2.5 - ew / 2];

// Batteria centrata nello spazio libero, connettori davanti
X_B0 = (xai(Z_ROOF) + xni(Z_ROOF)) / 2 - BATT_SPAN / 2;
X_B1 = X_B0 + batteria_lunghezza;

echo(str("Pod: lunghezza ", X_NB - xa(zb(W / 2)), " mm, larghezza ", W, " mm, altezza ", H,
         " mm (+3,2 anello LED). Elastici a X = ", BELTS[0], " e ", BELTS[1]));
echo(str("Perni della slitta: spezzoni di filamento lunghi ", W / 2 + Wi / 2 + 0.7, " mm"));
echo(str("Cavetto dell'antenna: il percorso è di ", round(CAVO_MIN), " mm su ", antenna_cavo,
         ", il resto fa un'ansa sotto la scheda"));
if (H_ANT - parete > H_STACK) echo(str("Tetto alzato di ", H_ANT - parete - H_STACK, " mm per far stare l'antenna sulla faccia"));
if (CAVO_MIN > antenna_cavo - 5) echo("ATTENZIONE: il cavetto dell'antenna resta teso");
if (ANT_V1 + 0.3 > CAM_V - diametro_foro_obiettivo / 2 - 0.5) echo("ATTENZIONE: l'antenna arriva sul foro dell'obiettivo");

// ---------------------------------------------------------------------
// Guscio
// ---------------------------------------------------------------------
Z_LOW = -20;

module profilo(est) {
  pts = est
    ? [[xa(Z_LOW), Z_LOW], [xa(H), H], [xn(H), H], [xn(Z_LOW), Z_LOW]]
    : [[xai(Z_LOW - 5), Z_LOW - 5], [xai(H_IN), H_IN], [xni(H_IN), H_IN], [xni(Z_LOW - 5), Z_LOW - 5]];
  rotate([90, 0, 0]) linear_extrude(height = 2 * W, center = true) polygon(pts);
}

module sezione(est) {
  w = est ? W : Wi;
  top = est ? H : H_IN;
  r = est ? raggio_spigoli : max(raggio_spigoli - parete, 0.3);
  translate([X_NB / 2, 0, 0]) rotate([90, 0, 90])
    linear_extrude(height = 3 * X_NB + 60, center = true)
      offset(r = r) offset(delta = -r) translate([-w / 2, Z_LOW - 10]) square([w, top - Z_LOW + 10]);
}

module guscio_pieno() intersection() { profilo(true); sezione(true); }
module vuoto() intersection() { profilo(false); sezione(false); }

module bottiglia_taglio() {
  translate([-60, 0, -R]) rotate([0, 90, 0]) cylinder(r = R + spessore_gomma, h = X_NB + 120, $fn = 360);
}

module finestre() {
  for (x = BELTS) translate([x - ew / 2, -W, Z_BAR]) cube([ew, 2 * W, Z_WIN1 - Z_BAR]);
}

module pod(tipo) {
  difference() {
    union() {
      difference() { guscio_pieno(); vuoto(); }
      intersection() {
        guscio_pieno();
        if (tipo == "elettronica") interni_elettronica(); else interni_batteria();
      }
    }
    bottiglia_taglio();
    finestre();
    if (tipo == "elettronica") fori_elettronica();
    else fori_batteria();
  }
}

// ---------------------------------------------------------------------
// Pod elettronica
// ---------------------------------------------------------------------
module in_scheda() {
  multmatrix([[E.x, 0, D.x, O.x],
              [0, 1, 0, Y_V0],
              [E.z, 0, D.z, O.z],
              [0, 0, 0, 1]]) children();
}

module box(b) translate([b[0], b[2], b[4]]) cube([b[1] - b[0], b[3] - b[2], b[5] - b[4]]);

module colonna(x, y, z0, d, foro) {
  difference() {
    translate([x, y, z0]) cylinder(d = d, h = H_IN - z0 + 0.5);
    if (foro) translate([x, y, z0 - 0.1]) cylinder(d = foro_vite, h = H_IN - z0 + 0.9);
  }
}

module interni_elettronica() {
  // MPU6050: due viti sul lato dei fori, due appoggi sul lato dei pin (verso la XIAO)
  zm = H_IN - MPU_SO;
  for (f = MPU_FORI) colonna(f.x, f.y, zm, 5, true);
  for (y = [MPU_Y0 + 4, MPU_Y1 - 4]) colonna(MPU_X0 + 4.5, y, zm, 3.5, false);
  // BMP180 col dorso liscio contro gli appoggi e il sensore verso la bottiglia
  // (va coperto con un pezzetto di spugna: dal fondo trasparente entra luce)
  zb_ = H_IN - BMP_SO;
  colonna(BMP_FORO.x, BMP_FORO.y, zb_, 4.5, true);
  colonna(BMP_X0 + 3.5, BMP_Y0 + BMP_WD / 2, zb_, 3, false);
  // Coppa del cicalino sul fianco Y+
  translate([X_BZ, Wi / 2 + 0.5, Z_BZ]) rotate([90, 0, 0])
    difference() {
      cylinder(d = BZ_OUT, h = BZ_CUP + 0.5);
      translate([0, 0, -1]) cylinder(d = BZ_D, h = BZ_CUP + 3);
    }
}

module fori_elettronica() {
  // Obiettivo
  in_scheda() translate([CAM_U, CAM_V, quota_faccia - 1]) cylinder(d = diametro_foro_obiettivo, h = parete + 2);
  // USB-C e microSD sul fianco Y+ (in volo: nastro adesivo sopra)
  in_scheda() box([1.5, 15.5, Wi / 2 - 0.01 - Y_V0, W / 2 + 1 - Y_V0, -1.2, 9.0]);
  // Perni della slitta: passanti sul fianco Y+, ciechi (0,7 mm) sul fianco Y-
  in_scheda() for (pu = PIN_U)
    translate([pu, -Wi / 2 - 0.7 - Y_V0, W_PIN]) rotate([-90, 0, 0])
      cylinder(r = R_PIN + 0.12, h = W + 2, $fn = 24);
  // Anello LED: due rivetti e il passaggio dei fili
  for (s = [-1, 1])
    translate([X_RING + s * 14 * cos(angolo_fori_anello), s * 14 * sin(angolo_fori_anello), H_IN - 1])
      cylinder(d = foro_rivetto, h = parete + 2, $fn = 24);
  translate([X_RING_HOLE, 0, H_IN - 1]) cylinder(d = 7, h = parete + 2);
  // Suono del cicalino, di lato
  translate([X_BZ, Wi / 2 - 1, Z_BZ]) rotate([-90, 0, 0]) cylinder(d = 3, h = parete + 3);
  // Sfiati laterali per il barometro (pressione statica)
  for (v = VENTS) translate([v[0], Wi / 2 - 1, v[1]]) rotate([-90, 0, 0]) cylinder(d = 1.5, h = parete + 3, $fn = 16);
  // Sede dell'antenna sulla faccia della camera e fessura del cavetto, aperta in basso
  in_scheda() {
    box([ANT_U0 - 0.3, ANT_U1 + 0.3, ANT_V0 - 0.3, ANT_V1 + 0.3, W_FACCIA - 0.4, W_FACCIA + 1]);
    translate([U_FES, V_FES, quota_faccia - 1]) cylinder(d = FESSURA, h = parete + 2, $fn = 24);
    box([U_FES - 40, U_FES, V_FES - FESSURA / 2, V_FES + FESSURA / 2, quota_faccia - 1, W_FACCIA + 1]);
  }
}

module slitta() {
  difference() {
    union() {
      box([SL_U0, SL_U1, SL_V0, SL_V1, SL_W0, SL_W1]);
      // distanziali agli angoli della scheda, dove non ci sono piazzole
      for (u = [0, PCB_U - 2.6], v = [0, PCB_V - 2.6]) box([u, u + 2.6, v, v + 2.6, SL_W1 - 0.01, 0]);
      // bordini a L attorno agli angoli (sul lato USB lasciano libero il connettore)
      for (cu = [0, 1], cv = [0, 1]) {
        ue = cu == 0 ? [SL_U0, -CLR] : [PCB_U + CLR, SL_U1];
        uc = cu == 0 ? [SL_U0, 3] : [PCB_U - 3, SL_U1];
        ve = cv == 0 ? [-CLR - LIP, -CLR] : [PCB_V + CLR, PCB_V + CLR + LIP];
        vc = cv == 0 ? [-CLR - LIP, 3] : [PCB_V - 3, PCB_V + CLR + LIP];
        box([ue[0], ue[1], vc[0], vc[1], SL_W1 - 0.01, LIP_H]);
        box([uc[0], uc[1], ve[0], ve[1], SL_W1 - 0.01, LIP_H]);
      }
    }
    // gole dei perni sul retro
    for (pu = PIN_U) translate([pu, SL_V0 - 1, W_PIN]) rotate([-90, 0, 0])
      cylinder(r = R_PIN + 0.1, h = SL_V1 - SL_V0 + 2, $fn = 24);
  }
}

// ---------------------------------------------------------------------
// Pod batteria
// ---------------------------------------------------------------------
module interni_batteria() {
  yb = batteria_larghezza / 2 + 0.3;
  zg = Z_ROOF - 4;  // le guide coprono i 4 mm superiori della cella
  // guide laterali con mensola a 45 gradi su cui appoggia la faccia superiore
  for (s = [-1, 1]) {
    translate([X_B0 - 0.3, s > 0 ? yb : -yb - 1.6, zg]) cube([batteria_lunghezza + 0.6, 1.6, H_IN - zg + 0.5]);
    translate([X_B0, 0, 0]) rotate([90, 0, 90]) linear_extrude(height = batteria_lunghezza)
      polygon([[s * yb, Z_ROOF], [s * (yb - 2), Z_ROOF], [s * yb, Z_ROOF + 2]]);
  }
  // testate: dietro tiene la spinta del decollo, davanti lascia passare i fili
  translate([X_B0 - 0.3 - 1.6, -yb - 1.6, zg]) cube([1.6, 2 * yb + 3.2, H_IN - zg + 0.5]);
  difference() {
    translate([X_B1 + 0.3, -yb - 1.6, zg]) cube([1.6, 2 * yb + 3.2, H_IN - zg + 0.5]);
    translate([X_B1, -4, zg - 1]) cube([3, 8, Z_ROOF - zg + 2]);
  }
}

module fori_batteria() {}

// ---------------------------------------------------------------------
// Componenti (solo per la vista d'insieme e i controlli)
// ---------------------------------------------------------------------
// Scheda principale (con USB-C) e scheda Sense con la camera, separate per l'animazione
// di montaggio: l'antenna si innesta con la Sense tolta
module xiao_base() for (i = [0 : 2]) box(XIAO_BOXES[i]);
module xiao_sense() {
  for (i = [3 : len(XIAO_BOXES) - 1]) box(XIAO_BOXES[i]);
  for (l = LENS) translate([CAM_U, CAM_V, l[1]]) cylinder(d = l[0], h = l[2] - l[1]);
}
module xiao() { xiao_base(); xiao_sense(); }

module mpu6050() difference() {
  translate([MPU_X0, MPU_Y0, H_IN - MPU_SO - 1.6 - 1.5]) cube([MPU_L, MPU_WD, 1.6 + 1.5 - 0.05]);
  for (f = MPU_FORI) translate([f.x, f.y, H_IN - MPU_SO - 4]) cylinder(d = FORO_MODULI, h = 5, $fn = 24);
}
module bmp180() difference() {
  union() {
    translate([BMP_X0, BMP_Y0, H_IN - BMP_SO - 1.6]) cube([BMP_L, BMP_WD, 1.6 - 0.05]);
    translate([BMP_X0 + 8, BMP_Y0 + 5, H_IN - BMP_SO - 2.6]) cube([4, 4, 1]);
  }
  translate([BMP_FORO.x, BMP_FORO.y, H_IN - BMP_SO - 3]) cylinder(d = FORO_MODULI, h = 4, $fn = 24);
}
module cicalino() translate([X_BZ, Wi / 2 - 0.05, Z_BZ]) rotate([90, 0, 0]) cylinder(d = 12, h = 9.6);
module sensori() { mpu6050(); bmp180(); cicalino(); }

module perni() in_scheda() for (pu = PIN_U)
  translate([pu, -Wi / 2 - 0.7 - Y_V0, W_PIN]) rotate([-90, 0, 0]) cylinder(r = R_PIN, h = W / 2 + Wi / 2 + 0.7, $fn = 24);

module anello() translate([X_RING, 0, H + 0.05]) difference() {
  cylinder(d = RING_D, h = 1.6);
  translate([0, 0, -1]) cylinder(d = 18, h = 4);
  // fori dei rivetti, sopra quelli del tetto
  for (s = [-1, 1])
    translate([s * 14 * cos(angolo_fori_anello), s * 14 * sin(angolo_fori_anello), -1]) cylinder(d = foro_rivetto, h = 4, $fn = 24);
}

module antenna() in_scheda() translate([ANT_U0, ANT_V0, W_FACCIA - 0.25]) cube([antenna_lunghezza, antenna_larghezza, 0.2]);

module cavetto() {
  in_scheda() box(UFL);
  for (i = [0 : len(CAVO_PTS) - 2]) hull() {
    translate(CAVO_PTS[i]) sphere(d = 1.13, $fn = 12);
    translate(CAVO_PTS[i + 1]) sphere(d = 1.13, $fn = 12);
  }
}

module batteria() translate([X_B0, -batteria_larghezza / 2, Z_ROOF - batteria_spessore])
  cube([batteria_lunghezza, batteria_larghezza, batteria_spessore]);

module componenti_elettronica(alfa_antenna = 1) {
  color("dimgray") in_scheda() xiao();
  color("gold") in_scheda() slitta();
  color("royalblue") sensori();
  color("white") perni();
  color("black") anello();
  color("black") cavetto();
  color("peru", alfa_antenna) antenna();  // per ultima: se è trasparente non copre il cavetto
}

// ---------------------------------------------------------------------
// Uscite
// ---------------------------------------------------------------------
module per_stampa(h) translate([0, 0, h]) rotate([180, 0, 0]) children();

module elastico() for (x = BELTS) translate([x - elastico_larghezza / 2, 0, 0]) rotate([0, 90, 0])
  difference() {
    cylinder(r = R + elastico_spessore, h = elastico_larghezza, $fn = 180);
    translate([0, 0, -1]) cylinder(r = R, h = elastico_larghezza + 2, $fn = 180);
  }

module assieme() {
  translate([0, 0, R]) {
    color("orange") pod("elettronica");
    if (mostra_componenti) componenti_elettronica();
  }
  rotate([180, 0, 0]) translate([0, 0, R]) {
    color("orange") pod("batteria");
    if (mostra_componenti) color("silver") batteria();
  }
  color("white") elastico();
  %translate([xa(zb(W / 2)) - 40, 0, 0]) rotate([0, 90, 0]) cylinder(r = R, h = X_NB + 80, $fn = 120);
}

if (verifica == "componenti") {
  // Componenti montati contro il pod: deve risultare vuoto
  intersection() {
    pod("elettronica");
    union() { in_scheda() { xiao(); slitta(); } sensori(); antenna(); cavetto(); }
  }
  intersection() { in_scheda() xiao(); in_scheda() slitta(); }
  intersection() { cavetto(); in_scheda() { xiao(); slitta(); } }
} else if (verifica == "estrazione_slitta") {
  // La slitta con la XIAO esce staccandosi di 3 mm dalla faccia e scivolando
  // parallela ad essa verso il basso: il percorso non deve toccare nulla.
  // Ogni pezzo spazza il suo volume (l'inviluppo unico darebbe falsi allarmi).
  intersection() {
    union() { pod("elettronica"); sensori(); }
    for (k = [0, 1]) for (b = concat(XIAO_BOXES, SLED_BOXES)) hull() {
      translate(-3 * k * D) in_scheda() box(b);
      translate(-3 * D - 40 * k * E) in_scheda() box(b);
    }
  }
} else if (verifica == "batteria") {
  intersection() { pod("batteria"); batteria(); }
} else if (parte == "assieme") {
  assieme();
} else if (parte == "spaccato") {
  // Pod elettronica senza il fianco Y+, per vedere come sta dentro tutto
  color("orange") difference() {
    pod("elettronica");
    translate([-50, 2, -50]) cube([X_NB + 100, W, 100]);
  }
  componenti_elettronica();
  %translate([-30, 0, -R]) rotate([0, 90, 0]) cylinder(r = R, h = X_NB + 60, $fn = 120);
} else if (parte == "spaccato_antenna") {
  // Pod elettronica senza il fianco Y-, visto da quel lato: percorso del cavetto
  // (l'antenna è disegnata trasparente al suo posto)
  color("orange") difference() {
    pod("elettronica");
    translate([-50, -W - Wi / 2 + 0.4, -50]) cube([X_NB + 100, W, 100]);
  }
  componenti_elettronica(0.35);
} else if (parte == "spaccato_batteria") {
  color("orange") difference() {
    pod("batteria");
    translate([-50, 0, -50]) cube([X_NB + 100, W, 100]);
  }
  color("silver") batteria();
  %translate([-30, 0, -R]) rotate([0, 90, 0]) cylinder(r = R, h = X_NB + 60, $fn = 120);
} else if (parte == "pod_elettronica") {
  per_stampa(H) pod("elettronica");
} else if (parte == "pod_batteria") {
  per_stampa(H) pod("batteria");
} else if (parte == "slitta") {
  translate([0, 0, -SL_W0]) slitta();
} else if (parte == "stampa_tutto") {
  per_stampa(H) pod("elettronica");
  translate([0, W + 8, 0]) per_stampa(H) pod("batteria");
  translate([0, -W - 8 - SL_V1, -SL_W0]) slitta();
}

if (verifica == "quote") echo(str("Z_O=", Z_O, " O.x=", O.x, " H_IN=", H_IN, " X_RING=", X_RING, " MPU_X0=", MPU_X0,
  " X_BZ=", X_BZ, " Z_BZ=", Z_BZ, " ANT_U0=", ANT_U0, " V_FES=", V_FES, " H_STACK=", H_STACK, " X_NB=", X_NB, " X_B0=", X_B0,
  " Z_ROOF=", Z_ROOF, " lente=", bp([CAM_U, CAM_V, 13.96]), " finestre=", BELTS));

// Inquadratura della finestra di OpenSCAD: [punto guardato, rotazione, distanza]
VISTE = [
  ["assieme",           [42, 0, 10], [62, 0, 328], 380],
  ["spaccato",          [44, 0, 10], [72, 0, 205], 175],
  ["spaccato_antenna",  [42, 0, 10], [75, 0, 15],  170],
  ["spaccato_batteria", [44, 0, 8],  [72, 0, 205], 175],
  ["slitta",            [9, 8, 2],   [45, 0, 25],  90],
];
VISTA = [for (v = VISTE) if (v[0] == parte) v];
$vpt = !inquadratura_automatica ? $vpt : len(VISTA) ? VISTA[0][1] : [42, parte == "stampa_tutto" ? -10 : 0, 12];
$vpr = !inquadratura_automatica ? $vpr : len(VISTA) ? VISTA[0][2] : [50, 0, 30];
$vpd = !inquadratura_automatica ? $vpd : len(VISTA) ? VISTA[0][3] : (parte == "stampa_tutto" ? 330 : 230);
