#include <Arduino.h>
#include "journal.h"
#include "detecteur_ir.h"

static unsigned long dernier_affichage_ms = 0;
static bool dernier_etat = false;
static int compteur_changements = 0;

void setup() {
    journal_init();
    detecteur_ir_init();

    delay(100);
    dernier_etat = detecteur_ir_obstacle();
    journal_evt("DEMARRAGE", "Test du module detecteur_ir");
}

void loop() {
    bool etat_actuel = detecteur_ir_obstacle();

    if (etat_actuel != dernier_etat) {
        dernier_etat = etat_actuel;
        compteur_changements++;
    }

    unsigned long ms = millis();
    if (ms - dernier_affichage_ms >= 100) {
        dernier_affichage_ms = ms;
        char ligne[48];
        snprintf(ligne, sizeof(ligne), "Etat: %s | Changements: %d",
                 etat_actuel ? "obstacle" : "libre", compteur_changements);
        Serial.println(ligne);
    }
}