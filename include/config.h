// =====================================================================
//  config.h — toutes les broches et tous les réglages du parking
//
//  Règles du projet :
//   - aucun module n'écrit de numéro de broche ou de réglage en dur :
//     tout vient d'ici, donc changer une broche = UNE modification ;
//   - l'unité est dans le nom (_MS, _US, _CM, _DEG, _HZ) pour éviter
//     les erreurs d'unité ;
//   - constantes typées (constexpr) plutôt que #define : le compilateur
//     connaît leur type et peut signaler une erreur ;
//   - ce fichier n'inclut PAS Arduino.h : l'automate (fichier 10)
//     l'utilisera dans des tests compilés sur le PC, sans la carte.
// =====================================================================
#pragma once   // empêche d'inclure ce fichier deux fois dans un même .cpp

#include <stdint.h>   // types à taille fixe (uint8_t, uint32_t...) : standard C, OK sur PC

// ---------------------------------------------------------------------
//  Broches (numéros de GPIO de l'ESP32)
// ---------------------------------------------------------------------
// Signalisation
constexpr uint8_t PIN_LED_VERTE = 27;   // LED verte + résistance 220 Ω
constexpr uint8_t PIN_LED_JAUNE = 26;   // LED jaune + résistance 220 Ω
constexpr uint8_t PIN_LED_ROUGE = 25;   // LED rouge + résistance 220 Ω
constexpr uint8_t PIN_BUZZER    = 33;   // buzzer actif

// Boutons (reliés à la masse, résistance de tirage interne)
constexpr uint8_t PIN_BOUTON_AU    = 16;   // arrêt d'urgence
constexpr uint8_t PIN_BOUTON_REARM = 17;   // réarmement

// Capteurs ultrason HC-SR04 (ECHO passe par le pont diviseur 5 V -> 3,3 V)
constexpr uint8_t PIN_US1_TRIG = 5;    // capteur 1 (avant la barrière) : déclenchement
constexpr uint8_t PIN_US1_ECHO = 18;   // capteur 1 : retour d'écho
constexpr uint8_t PIN_US2_TRIG = 19;   // capteur 2 (après la barrière) : déclenchement
constexpr uint8_t PIN_US2_ECHO = 23;   // capteur 2 : retour d'écho

// Détecteur d'obstacle infrarouge (alimenté en 3,3 V, PAS en 5 V)
constexpr uint8_t PIN_IR_OUT = 32;     // sortie OUT du module

// Servomoteur SG90
constexpr uint8_t PIN_SERVO = 13;      // fil orange (signal de commande)

// Bus I2C de l'écran OLED
constexpr uint8_t PIN_I2C_SDA = 21;    // ligne de données
constexpr uint8_t PIN_I2C_SCL = 22;    // ligne d'horloge

// ---------------------------------------------------------------------
//  Barrière (servomoteur)
// ---------------------------------------------------------------------
constexpr int      BARRIERE_ANGLE_FERME_DEG  = 0;     // provisoire : à régler au fichier 8
constexpr int      BARRIERE_ANGLE_OUVERT_DEG = 90;    // provisoire : à régler au fichier 8
constexpr uint32_t BARRIERE_PAS_MS           = 12;    // délai entre deux pas de 1° (mouvement doux)
constexpr uint16_t SERVO_IMPULSION_MIN_US    = 500;   // largeur d'impulsion pour 0° (SG90)
constexpr uint16_t SERVO_IMPULSION_MAX_US    = 2400;  // largeur d'impulsion pour 180° (SG90)
constexpr uint16_t SERVO_FREQUENCE_HZ        = 50;    // fréquence de commande standard d'un servo

// ---------------------------------------------------------------------
//  Capteurs ultrason
// ---------------------------------------------------------------------
constexpr float    US_SEUIL_PRESENCE_CM   = 12.0f;   // provisoire : à régler au fichier 6
constexpr float    US_HYSTERESIS_CM       = 2.0f;    // marge pour déclarer « voiture partie »
constexpr uint8_t  US_NB_CONFIRMATIONS    = 3;       // mesures d'affilée pour changer de décision
constexpr float    US_DISTANCE_MIN_CM     = 2.0f;    // en dessous : mesure invalide
constexpr float    US_DISTANCE_MAX_CM     = 200.0f;  // au-dessus : mesure invalide
constexpr uint8_t  US_NB_INVALIDES_DEFAUT = 5;       // mesures invalides d'affilée -> capteur en défaut
constexpr uint32_t US_PERIODE_MESURE_MS   = 60;      // une mesure toutes les 60 ms (alternance C1/C2)
constexpr uint32_t US_TIMEOUT_ECHO_US     = 12000;   // attente max de l'écho (≈ 2 m aller-retour)

// ---------------------------------------------------------------------
//  Boutons
// ---------------------------------------------------------------------
constexpr uint32_t BOUTON_ANTIREBOND_MS = 20;   // un changement doit durer 20 ms pour être validé

// ---------------------------------------------------------------------
//  Temps (automate, journal, écran)
// ---------------------------------------------------------------------
constexpr uint32_t DELAI_FERMETURE_APRES_PASSAGE_MS  = 1500;   // zone libre après un passage
constexpr uint32_t DELAI_FERMETURE_SANS_PASSAGE_MS   = 10000;  // voiture qui renonce, ou démarrage
constexpr uint32_t PERIODE_MESSAGE_ETAT_MS           = 500;    // une ligne HB envoyée au PC
constexpr uint32_t ECRAN_PERIODE_RAFRAICHISSEMENT_MS = 250;    // ≈ 4 images par seconde

// ---------------------------------------------------------------------
//  Divers
// ---------------------------------------------------------------------
constexpr bool     IR_ACTIF_BAS        = true;    // OUT = LOW quand obstacle (à vérifier au fichier 7)
constexpr uint8_t  ECRAN_ADRESSE_I2C   = 0x3C;    // à confirmer par le scan I2C du fichier 5
constexpr uint8_t  ECRAN_LARGEUR_PX    = 128;     // largeur de l'écran OLED
constexpr uint8_t  ECRAN_HAUTEUR_PX    = 64;      // mettre 32 si seule la moitié de l'image s'affiche
constexpr uint32_t SERIE_VITESSE_BAUDS = 115200;  // doit être égal à monitor_speed de platformio.ini

// Durées de clignotement (en millisecondes)
const unsigned long PERIODE_CLIGNO_RAPIDE_MS  = 250;
const unsigned long PERIODE_CLIGNO_LENT_MS    = 1000;
const unsigned long DUREE_BIP_COURT_MS        = 100;