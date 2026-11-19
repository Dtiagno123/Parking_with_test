#include "boutons.h"
#include "config.h"

struct EtatBouton {
    uint8_t pin;
    bool etat_stable;           // Vrai si enfoncé (validé)
    bool dernier_etat_brut;     // Dernière lecture instantanée
    unsigned long dernier_changement_ms;
    bool appui_detecte;         // Front montant (1 seul tour de boucle)
};

static EtatBouton btn_au    = {PIN_BOUTON_AU, false, false, 0, false};
static EtatBouton btn_rearm = {PIN_BOUTON_REARM, false, false, 0, false};

static void mettre_a_jour_bouton(EtatBouton &btn, unsigned long maintenant) {
    // Réinitialise l'impulsion d'appui à chaque tour de boucle
    btn.appui_detecte = false;

    // Conversion logique inversée (INPUT_PULLUP : LOW = enfoncé)
    bool lecture_brut = (digitalRead(btn.pin) == LOW);

    // Si l'état instantané change, on relance le chronomètre anti-rebond
    if (lecture_brut != btn.dernier_etat_brut) {
        btn.dernier_changement_ms = maintenant;
        btn.dernier_etat_brut = lecture_brut;
    }

    // Si l'état est resté stable plus longtemps que DELAI_ANTIREBOND_MS
    if ((maintenant - btn.dernier_changement_ms) >= DELAI_ANTIREBOND_MS) {
        if (lecture_brut != btn.etat_stable) {
            btn.etat_stable = lecture_brut;
            // Si le nouvel état validé est "enfoncé", on signale un appui unique
            if (btn.etat_stable) {
                btn.appui_detecte = true;
            }
        }
    }
}

void boutons_init(void) {
    pinMode(PIN_BOUTON_AU, INPUT_PULLUP);
    pinMode(PIN_BOUTON_REARM, INPUT_PULLUP);
}

void boutons_update(void) {
    unsigned long ms = millis();
    mettre_a_jour_bouton(btn_au, ms);
    mettre_a_jour_bouton(btn_rearm, ms);
}

bool bouton_au_enfonce(void) {
    return btn_au.etat_stable;
}

bool bouton_au_appui(void) {
    return btn_au.appui_detecte;
}

bool bouton_rearm_enfonce(void) {
    return btn_rearm.etat_stable;
}

bool bouton_rearm_appui(void) {
    return btn_rearm.appui_detecte;
}