// =====================================================================
//  essai_demarrage.cpp — programme d'essai du fichier 1
//
//  Ce qu'il vérifie : la compilation, le téléversement, le port série
//  et la lecture d'une valeur dans config.h.
//  Résultat attendu : une ligne par seconde, le temps augmente d'environ
//  1000 ms, la broche affichée est 27. Bouton EN -> le temps repart de 0.
// =====================================================================
#include <Arduino.h>
#include "config.h"   // trouvé automatiquement : PlatformIO cherche dans include/

// Instant (en ms) du dernier envoi.
// "static" : la variable n'est visible que dans ce fichier.
static uint32_t dernierEnvoiMs = 0;

void setup() {
    Serial.begin(SERIE_VITESSE_BAUDS);   // vitesse lue dans config.h, pas écrite en dur
    Serial.println();                    // saute les caractères parasites du démarrage
    Serial.println("=== essai_demarrage ===");
}

void loop() {
    const uint32_t maintenantMs = millis();   // temps écoulé depuis le démarrage

    // Envoi toutes les 1000 ms SANS delay() : on compare l'heure actuelle
    // au dernier envoi. C'est la technique utilisée dans tous les modules.
    // La soustraction reste juste même quand millis() repasse à 0 (après ~49 jours).
    if (maintenantMs - dernierEnvoiMs >= 1000) {
        dernierEnvoiMs = maintenantMs;

        // %lu attend un unsigned long : on convertit explicitement pour
        // éviter un avertissement de -Wall
        Serial.printf("temps=%lu ms  PIN_LED_VERTE=%u  seuil=%.1f cm\n",
                      static_cast<unsigned long>(maintenantMs),
                      PIN_LED_VERTE,
                      static_cast<double>(US_SEUIL_PRESENCE_CM));
    }
}