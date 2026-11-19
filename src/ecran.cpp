#include "ecran.h"
#include "config.h"
#include "journal.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LARGEUR_ECRAN 128
#define HAUTEUR_ECRAN 64
#define OLED_RESET    -1

static Adafruit_SSD1306 display(LARGEUR_ECRAN, HAUTEUR_ECRAN, &Wire, OLED_RESET);
static bool dispo = false;
static unsigned long dernier_rafraichissement_ms = 0;
static bool donnees_modifiees = false;

// Tampon des données à afficher
static char buf_etat[24]    = "INIT";
static int  buf_passages    = 0;
static bool buf_c1          = false;
static bool buf_c2          = false;
static bool buf_ir          = false;
static char buf_message[32] = "";

bool ecran_init(void) {
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    // Vérification matérielle sur le bus I2C avant d'appeler Adafruit
    Wire.beginTransmission(ADRESSE_ECRAN_I2C);
    uint8_t erreur = Wire.endTransmission();

    if (erreur != 0) {
        dispo = false;
        journal_evt("ECRAN_ABSENT", "Pas de reponse I2C a 0x3C - mode degrade");
        return false;
    }

    if (!display.begin(SSD1306_SWITCHCAPVCC, ADRESSE_ECRAN_I2C)) {
        dispo = false;
        journal_evt("ECRAN_ABSENT", "Echec allocation SSD1306");
        return false;
    }

    dispo = true;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("PARKING ESP32");
    display.println("Initialisation...");
    display.display();
    
    return true;
}

bool ecran_disponible(void) {
    return dispo;
}

void ecran_afficher(const char* etat, int passages, bool c1_voiture, bool c2_voiture, bool ir_obstacle, const char* message) {
    if (!dispo) return;

    snprintf(buf_etat, sizeof(buf_etat), "%s", etat);
    buf_passages = passages;
    buf_c1 = c1_voiture;
    buf_c2 = c2_voiture;
    buf_ir = ir_obstacle;
    snprintf(buf_message, sizeof(buf_message), "%s", message);
    
    donnees_modifiees = true;
}

void ecran_update(void) {
    if (!dispo || !donnees_modifiees) return;

    unsigned long ms = millis();
    if (ms - dernier_rafraichissement_ms < PERIODE_ECRAN_MS) {
        return;
    }

    dernier_rafraichissement_ms = ms;
    donnees_modifiees = false;

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Ligne 1 : État principal
    display.setCursor(0, 0);
    display.print("ETAT: ");
    display.println(buf_etat);

    // Ligne 2 : Compteur de passages
    display.setCursor(0, 16);
    display.print("Passages: ");
    display.println(buf_passages);

    // Ligne 3 : État des capteurs
    display.setCursor(0, 32);
    display.print("C1:");
    display.print(buf_c1 ? "OUI" : "NON");
    display.print(" C2:");
    display.print(buf_c2 ? "OUI" : "NON");
    display.print(" IR:");
    display.println(buf_ir ? "OBS" : "OK");

    // Ligne 4 : Message d'information ou défaut
    display.setCursor(0, 48);
    display.println(buf_message);

    display.display();
}