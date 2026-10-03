#pragma once

// Pin della XIAO ESP32S3 Sense (scheda di espansione camera/mic/microSD).
// Camera: da camera_pins.h (CAMERA_MODEL_XIAO_ESP32S3) dell'esempio
// CameraWebServer di arduino-esp32. Mic e SD: wiki Seeed.

// --- Camera OV2640 / OV3660 (DVP) ---
#define CAM_PIN_PWDN   -1
#define CAM_PIN_RESET  -1
#define CAM_PIN_XCLK   10
#define CAM_PIN_SIOD   40
#define CAM_PIN_SIOC   39
#define CAM_PIN_Y9     48
#define CAM_PIN_Y8     11
#define CAM_PIN_Y7     12
#define CAM_PIN_Y6     14
#define CAM_PIN_Y5     16
#define CAM_PIN_Y4     18
#define CAM_PIN_Y3     17
#define CAM_PIN_Y2     15
#define CAM_PIN_VSYNC  38
#define CAM_PIN_HREF   47
#define CAM_PIN_PCLK   13

// --- Microfono PDM (MSM261D3526H1CPM) ---
#define MIC_PIN_CLK    42
#define MIC_PIN_DATA   41

// --- microSD (SPI: SCK 7, MISO 8, MOSI 9 = pin SPI di default della variant) ---
// GPIO21 è anche il LED utente: con la SD attiva il LED non va usato.
#define SD_PIN_CS      21

// --- Pulsante BOOT (attivo basso), utilizzabile come input dopo l'avvio ---
#define BTN_PIN_BOOT   0
