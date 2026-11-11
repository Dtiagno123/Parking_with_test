#include <Arduino.h>
#include "journal.h"

static unsigned long dernier_hb = 0;
static int compteur_fictif = 0;

void setup() {
    journal_init();
    delay(100); // Court temps de stabilisation du port série
    journal_evt("DEMARRAGE", "Test du module journal");
}

void loop() {
    unsigned long maintenant = millis();

    // Envoi du message HB toutes les 500 ms
    if (maintenant - dernier_hb >= 500) {
        dernier_hb = maintenant;
        compteur_fictif++;
        
        // Test avec des valeurs fictives de simulation
        journal_etat("FERME", 0, 20, 20, false, compteur_fictif);
    }
}