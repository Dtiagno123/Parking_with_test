#include <Arduino.h>
#include "config.h"
#include "journal.h"
#include "surete.h"

static uint32_t dernier_tick_ms = 0;
static uint32_t compteur_sec    = 0;
static String   buffer_serie    = "";

void setup() {
    journal_init();
    delay(200);

    journal_evt("DEMARRAGE", surete_cause_redemarrage());
    surete_activer_watchdog();
    Serial.println("Tapez 'bloque' puis Entree pour tester le watchdog (env. 5 s).");
}

void loop() {
    const uint32_t maintenant = millis();

    // Lecture non bloquante de la commande 'bloque'
    while (Serial.available() > 0) {
        const char c = static_cast<char>(Serial.read());
        if (c == '\n' || c == '\r') {
            buffer_serie.trim();
            buffer_serie.toLowerCase();
            if (buffer_serie == "bloque") {
                Serial.println("Blocage volontaire : attente du declenchement watchdog...");
                while (true) {
                    // Boucle infinie sans rendre la main a loop()
                }
            }
            buffer_serie = "";
        } else if (buffer_serie.length() < 32) {
            buffer_serie += c;
        }
    }

    // Affichage d'un compteur chaque seconde
    if (maintenant - dernier_tick_ms >= 1000) {
        dernier_tick_ms = maintenant;
        compteur_sec++;
        Serial.print("Fonctionnement normal : ");
        Serial.print(compteur_sec);
        Serial.println(" s");
    }
}