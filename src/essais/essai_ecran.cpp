#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "journal.h"
#include "ecran.h"

static unsigned long dernier_changement = 0;
static unsigned long tours_de_boucle = 0;
static int compteur = 0;

static void lancer_scan_i2c(void) {
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    int trouves = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            char detail[32];
            snprintf(detail, sizeof(detail), "Peripherique I2C trouve: 0x%02X", addr);
            journal_evt("SCAN_I2C", detail);
            trouves++;
        }
    }
    if (trouves == 0) {
        journal_evt("SCAN_I2C", "Aucun peripherique trouve");
    }
}

void setup() {
    journal_init();
    delay(100);
    journal_evt("DEMARRAGE", "Test du module ecran");

    lancer_scan_i2c();
    
    if (ecran_init()) {
        journal_evt("ECRAN_OK", "OLED initialise avec succes");
    }
}

void loop() {
    ecran_update();
    tours_de_boucle++;

    unsigned long ms = millis();
    if (ms - dernier_changement >= 1000) {
        dernier_changement = ms;
        compteur++;

        char msg[32];
        snprintf(msg, sizeof(msg), "Cycle test #%d", compteur);
        
        // Alternance fictive des états pour tester l'affichage
        bool c1 = (compteur % 2 == 0);
        bool c2 = (compteur % 3 == 0);
        bool ir = (compteur % 5 == 0);
        const char* etat = (compteur % 2 == 0) ? "OUVERT" : "FERME";

        ecran_afficher(etat, compteur, c1, c2, ir, msg);
        journal_etat(etat, 0, 20, 20, ir, (int)tours_de_boucle);
        
        tours_de_boucle = 0;
    }
}