#pragma once
#include <Arduino.h>
#include <WebServer.h>

namespace AviExport {

// Invia al client un file AVI (video MJPEG + audio PCM) costruito al volo
// da <dir>/video.mjpeg, <dir>/frames.csv e <dir>/audio.wav.
// I file sulla SD non vengono modificati.
void stream(WebServer &server, const String &dir);

}  // namespace AviExport
