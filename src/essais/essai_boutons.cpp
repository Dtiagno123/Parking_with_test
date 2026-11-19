#include <Arduino.h>
#include "journal.h"
#include "signalisation.h"
#include "boutons.h"

static int compteur_au = 0;
static int compteur_rearm = 0;

void setup() {
    journal_init();
    signalisation_init();
    boutons_init();
    
    // Tout éteint au départ
    signalisation_set_mode(SIG_MODE_OUVERT); 
    journal_evt("DEMARRAGE", "Test du module boutons");
}

void loop() {
    boutons_update();
    signalisation_update();

    // Détection des appuis uniques
    if (bouton_au_appui()) {
        compteur_au++;
        char msg[40];
        snprintf(msg, sizeof(msg), "Appui AU num %d", compteur_au);
        journal_evt("BOUTON_AU", msg);
    }

    if (bouton_rearm_appui()) {
        compteur_rearm++;
        char msg[40];
        snprintf(msg, sizeof(msg), "Appui REARM num %d", compteur_rearm);
        journal_evt("BOUTON_REARM", msg);
    }

    // Bonus : LED rouge allumée tant que l'AU est maintenu enfoncé
    if (bouton_au_enfonce()) {
        signalisation_set_mode(SIG_MODE_FERME); // Mode Fermé = LED rouge allumée fixe
    } else {
        signalisation_set_mode(SIG_MODE_OUVERT); // Mode Ouvert = LED verte fixe
    }
}