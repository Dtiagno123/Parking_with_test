#include <Arduino.h>
#include "config.h"
#include "journal.h"
#include "signalisation.h"
#include "barriere.h"

static char buffer_serie[24];
static uint8_t idx_buffer = 0;
static bool en_mouvement_precedent = false;

static void traiter_ligne(const char* ligne) {
    if (strcasecmp(ligne, "stop") == 0) {
        barriere_figer();
        char msg[40];
        snprintf(msg, sizeof(msg), "Barriere figee a %d deg", barriere_get_angle_actuel());
        journal_evt("BARRIERE_STOP", msg);
        return;
    }

    char* fin = nullptr;
    long angle = strtol(ligne, &fin, 10);
    if (fin != ligne && *fin == '\0' && angle >= 0 && angle <= 180) {
        barriere_set_cible((int)angle);
        en_mouvement_precedent = true;
        char msg[40];
        snprintf(msg, sizeof(msg), "Nouvelle cible: %ld deg", angle);
        journal_evt("BARRIERE_CIBLE", msg);
    } else {
        journal_evt("COMMANDE_INCONNUE", ligne);
    }
}

void setup() {
    journal_init();
    signalisation_init();
    barriere_init();

    // Clignotement rapide de la LED jaune pour prouver le non-blocage
    signalisation_set_mode(SIG_MODE_EN_MOUVEMENT);

    delay(100);
    journal_evt("DEMARRAGE", "Test du module barriere (taper un angle 0..180 ou 'stop')");
}

void loop() {
    signalisation_update();
    barriere_update();

    // Signale une seule fois quand la barrière atteint sa cible
    if (en_mouvement_precedent && barriere_est_arrivee()) {
        en_mouvement_precedent = false;
        char msg[40];
        snprintf(msg, sizeof(msg), "Arrivee a %d deg", barriere_get_angle_actuel());
        journal_evt("BARRIERE_ARRIVEE", msg);
    }

    // Lecture non bloquante du moniteur série
    while (Serial.available() > 0) {
        char c = (char)Serial.read();
        if (c == '\r') continue;
        if (c == '\n') {
            buffer_serie[idx_buffer] = '\0';
            if (idx_buffer > 0) {
                traiter_ligne(buffer_serie);
            }
            idx_buffer = 0;
        } else if (idx_buffer < sizeof(buffer_serie) - 1) {
            buffer_serie[idx_buffer++] = c;
        }
    }
}