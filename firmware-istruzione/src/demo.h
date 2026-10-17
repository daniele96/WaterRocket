#pragma once

// Ogni demo è un piccolo programma Arduino a sé:
//   ...Inizio()  si prepara una volta, come setup(). Se qualcosa manca restituisce false.
//   ...Ripeti()  si ripete finché non si sceglie un'altra demo, come loop().
//   ...Fine()    rimette a posto (solo dove serve).

bool scannerInizio();        void scannerRipeti();         // 1
bool barometroInizio();      void barometroRipeti();       // 2
bool accelerometroInizio();  void accelerometroRipeti();   // 3
bool giroscopioInizio();     void giroscopioRipeti();      // 4
bool cicalinoInizio();       void cicalinoRipeti();        // 5
bool anelloInizio();         void anelloRipeti();          // 6
bool microfonoInizio();      void microfonoRipeti();       void microfonoFine();   // 7
bool wifiInizio();           void wifiRipeti();            void wifiFine();        // 8
bool livellaInizio();        void livellaRipeti();         // 9
bool decolloInizio();        void decolloRipeti();         // 0
