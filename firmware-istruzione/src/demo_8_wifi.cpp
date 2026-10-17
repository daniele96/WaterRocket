// Demo 8 · Radio: quali reti WiFi sente la XIAO, e quanto forte.

#include <WiFi.h>

#include <algorithm>

#include "aiuto.h"
#include "demo.h"

bool wifiInizio() {
  titolo("8 · RADIO: LE RETI WIFI INTORNO");
  WiFi.mode(WIFI_STA);   // solo ascolto: non si collega a niente
  WiFi.disconnect();
  Serial.println("La XIAO ascolta le onde radio a 2,4 GHz e misura quanto forte arriva ogni rete.");
  Serial.println("In dBm: -30 fortissimo, -60 buono, -90 quasi niente.");
  Serial.println("Prova a coprire l'antenna con la mano, o con una bottiglia d'acqua: l'acqua assorbe!");
  return true;
}

void wifiRipeti() {
  Serial.println("Ascolto...");
  const int n = WiFi.scanNetworks();   // dura un paio di secondi: ascolta tutti i canali
  if (n <= 0) {
    Serial.println("Nessuna rete sentita. L'antenna è collegata?");
    aspetta(2000);
    return;
  }

  // In ordine dalla più forte
  int ordine[32];
  const int quante = min(n, 32);
  for (int i = 0; i < quante; i++) ordine[i] = i;
  std::sort(ordine, ordine + quante, [](int a, int b) { return WiFi.RSSI(a) > WiFi.RSSI(b); });

  if (modoGrafico) {
    campo("rete più forte", WiFi.RSSI(ordine[0]), "dBm", 0);
  } else {
    for (int k = 0; k < min(quante, 6); k++) {
      const int i = ordine[k];
      String nome = WiFi.SSID(i);
      if (nome.isEmpty()) nome = "(nascosta)";
      if (nome.length() > 24) nome = nome.substring(0, 24);
      Serial.printf("  %4ld dBm ", (long)WiFi.RSSI(i));
      barretta(WiFi.RSSI(i), -100, -20);
      Serial.printf("  %-24s canale %ld\n", nome.c_str(), (long)WiFi.channel(i));
    }
    Serial.println();
  }
  WiFi.scanDelete();
  aspetta(1000);
}

void wifiFine() { WiFi.mode(WIFI_OFF); }
