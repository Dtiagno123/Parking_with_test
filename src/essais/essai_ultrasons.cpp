#include <Arduino.h>
#include "config.h"
#include "journal.h"
#include "boutons.h"
#include "capteur_ultrason.h"

static CapteurUltrason capteur1("C1", PIN_US1_TRIG, PIN_US1_ECHO);
static CapteurUltrason capteur2("C2", PIN_US2_TRIG, PIN_US2_ECHO);

static unsigned long derniere_mesure_ms = 0;
static unsigned long dernier_affichage_ms = 0;
static bool tour_capteur_1 = true;

void setup() {
    journal_init();
    boutons_init();
    capteur1.init();
    capteur2.init();

    delay(100);
    journal_evt("DEMARRAGE", "Test du module capteur_ultrason");
}

void loop() {
    boutons_update();
    unsigned long ms = millis();

    // Alternance stricte toutes les 60 ms entre C1 et C2
    if (ms - derniere_mesure_ms >= US_PERIODE_ALTERNANCE_MS) {
        derniere_mesure_ms = ms;
        if (tour_capteur_1) {
            capteur1.mesurer();
        } else {
            capteur2.mesurer();
        }
        tour_capteur_1 = !tour_capteur_1;
    }

    // Bouton réarmement : efface les défauts uniquement si la dernière mesure est valide
    if (bouton_rearm_appui()) {
        if (capteur1.estEnDefaut() && capteur1.derniereMesureValide()) {
            capteur1.effacerDefaut();
            journal_evt("REARM_CAPTEUR", "Defaut C1 efface");
        }
        if (capteur2.estEnDefaut() && capteur2.derniereMesureValide()) {
            capteur2.effacerDefaut();
            journal_evt("REARM_CAPTEUR", "Defaut C2 efface");
        }
    }

    // Affichage toutes les 200 ms
    if (ms - dernier_affichage_ms >= 200) {
        dernier_affichage_ms = ms;
        char ligne[96];
        snprintf(ligne, sizeof(ligne),
                 "C1: %.1fcm pres=%d def=%d ech=%d | C2: %.1fcm pres=%d def=%d ech=%d",
                 capteur1.getDistanceCm(), capteur1.estVoiturePresente(), capteur1.estEnDefaut(), capteur1.getCompteurEchecs(),
                 capteur2.getDistanceCm(), capteur2.estVoiturePresente(), capteur2.estEnDefaut(), capteur2.getCompteurEchecs());
        Serial.println(ligne);
    }
}