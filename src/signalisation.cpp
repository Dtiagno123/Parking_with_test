#include "signalisation.h"
#include "config.h"

static ModeSignalisation mode_actuel = SIG_MODE_FERME;
static unsigned long dernier_bip_court_ms = 0;
static bool bip_court_en_cours = false;

void signalisation_init(void) {
    pinMode(PIN_LED_VERTE, OUTPUT);
    pinMode(PIN_LED_JAUNE, OUTPUT);
    pinMode(PIN_LED_ROUGE, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    digitalWrite(PIN_LED_VERTE, LOW);
    digitalWrite(PIN_LED_JAUNE, LOW);
    digitalWrite(PIN_LED_ROUGE, LOW);
    digitalWrite(PIN_BUZZER, LOW);
}

void signalisation_set_mode(ModeSignalisation mode) {
    mode_actuel = mode;
}

void signalisation_bip_court(void) {
    bip_court_en_cours = true;
    dernier_bip_court_ms = millis();
    digitalWrite(PIN_BUZZER, HIGH);
}

void signalisation_update(void) {
    unsigned long ms = millis();

    // 1. Gérer la fin d'un bip court autonome s'il y en a un
    if (bip_court_en_cours) {
        if (ms - dernier_bip_court_ms >= DUREE_BIP_COURT_MS) {
            bip_court_en_cours = false;
            digitalWrite(PIN_BUZZER, LOW);
        }
    }

    // Calcul des phases de clignotement (vrai/faux) selon l'heure
    bool etat_cligno_rapide = (ms / (PERIODE_CLIGNO_RAPIDE_MS / 2)) % 2 == 0;
    bool etat_cligno_lent   = (ms / (PERIODE_CLIGNO_LENT_MS / 2)) % 2 == 0;

    // 2. Traitement selon le mode
    switch (mode_actuel) {
        case SIG_MODE_FERME:
            digitalWrite(PIN_LED_VERTE, LOW);
            digitalWrite(PIN_LED_JAUNE, LOW);
            digitalWrite(PIN_LED_ROUGE, HIGH);
            if (!bip_court_en_cours) digitalWrite(PIN_BUZZER, LOW);
            break;

        case SIG_MODE_EN_MOUVEMENT:
            digitalWrite(PIN_LED_VERTE, LOW);
            digitalWrite(PIN_LED_JAUNE, etat_cligno_rapide ? HIGH : LOW);
            digitalWrite(PIN_LED_ROUGE, LOW);
            if (!bip_court_en_cours) digitalWrite(PIN_BUZZER, LOW);
            break;

        case SIG_MODE_OUVERT:
            digitalWrite(PIN_LED_VERTE, HIGH);
            digitalWrite(PIN_LED_JAUNE, LOW);
            digitalWrite(PIN_LED_ROUGE, LOW);
            if (!bip_court_en_cours) digitalWrite(PIN_BUZZER, LOW);
            break;

        case SIG_MODE_DEFAUT:
            digitalWrite(PIN_LED_VERTE, LOW);
            digitalWrite(PIN_LED_JAUNE, etat_cligno_lent ? HIGH : LOW);
            digitalWrite(PIN_LED_ROUGE, etat_cligno_lent ? HIGH : LOW);
            // Bip toutes les 2 secondes (pendant 100 ms)
            if (!bip_court_en_cours) {
                bool bip_2s = (ms % 2000) < 100;
                digitalWrite(PIN_BUZZER, bip_2s ? HIGH : LOW);
            }
            break;

        case SIG_MODE_ARRET_URGENCE:
            digitalWrite(PIN_LED_VERTE, LOW);
            digitalWrite(PIN_LED_JAUNE, LOW);
            digitalWrite(PIN_LED_ROUGE, etat_cligno_rapide ? HIGH : LOW);
            // Bip court chaque seconde (pendant 100 ms)
            if (!bip_court_en_cours) {
                bool bip_1s = (ms % 1000) < 100;
                digitalWrite(PIN_BUZZER, bip_1s ? HIGH : LOW);
            }
            break;
    }
}