#include <Arduino.h>
#include "journal.h"
#include "signalisation.h"

static unsigned long dernier_changement_mode = 0;
static unsigned long dernier_calcul_tours = 0;
static unsigned long tours_de_boucle = 0;
static int mode_index = 0;

const char* noms_modes[] = {
    "MODE_FERME",
    "MODE_EN_MOUVEMENT",
    "MODE_OUVERT",
    "MODE_DEFAUT",
    "MODE_ARRET_URGENCE"
};

void setup() {
    journal_init();
    signalisation_init();
    journal_evt("DEMARRAGE", "Test du module signalisation");
    signalisation_set_mode(SIG_MODE_FERME);
}

void loop() {
    // 1. Toujours mettre à jour le module
    signalisation_update();
    tours_de_boucle++;

    unsigned long ms = millis();

    // 2. Changer de mode toutes les 3 secondes
    if (ms - dernier_changement_mode >= 3000) {
        dernier_changement_mode = ms;
        mode_index = (mode_index + 1) % 5;
        
        ModeSignalisation nouveau_mode = static_cast<ModeSignalisation>(mode_index);
        signalisation_set_mode(nouveau_mode);
        journal_evt("SIGNALISATION", noms_modes[mode_index]);
    }

    // 3. Afficher le nombre de tours de boucle par seconde (vérification du non-blocage)
    if (ms - dernier_calcul_tours >= 1000) {
        dernier_calcul_tours = ms;
        journal_etat(noms_modes[mode_index], 0, 0, 0, false, (int)tours_de_boucle);
        tours_de_boucle = 0;
    }
}