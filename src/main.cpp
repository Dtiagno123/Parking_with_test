#include <Arduino.h>
#include "config.h"
#include "journal.h"
#include "signalisation.h"
#include "boutons.h"
#include "capteur_ultrason.h"
#include "detecteur_ir.h"
#include "ecran.h"
#include "barriere.h"
#include "automate.h"

static CapteurUltrason capteur_entree("C1", PIN_US1_TRIG, PIN_US1_ECHO);
static CapteurUltrason capteur_sortie("C2", PIN_US2_TRIG, PIN_US2_ECHO);

static bool         interroger_us1   = true;
static uint32_t     dernier_us_ms    = 0;
static EtatAutomate dernier_etat     = ETAT_REPOS;
static uint32_t     dernier_hb_ms    = 0;
static const char*  dernier_msg_ecran = "Systeme pret";

static void appliquer_signalisation_etat(EtatAutomate etat) {
    switch (etat) {
        case ETAT_REPOS:
            signalisation_set_mode(SIG_MODE_FERME);
            break;

        case ETAT_OUVERTURE:
        case ETAT_FERMETURE:
        case ETAT_REOUVERTURE_OBSTACLE:
            signalisation_set_mode(SIG_MODE_EN_MOUVEMENT);
            break;

        case ETAT_ATTENTE_PASSAGE:
            signalisation_set_mode(SIG_MODE_OUVERT);
            break;

        case ETAT_DEFAUT:
            signalisation_set_mode(SIG_MODE_DEFAUT);
            break;

        case ETAT_ARRET_URGENCE:
            signalisation_set_mode(SIG_MODE_ARRET_URGENCE);
            break;
    }
}

void setup() {
    journal_init();
    signalisation_init();
    boutons_init();
    capteur_entree.init();
    capteur_sortie.init();
    detecteur_ir_init();
    ecran_init();
    barriere_init();
    automate_init(static_cast<int>(PARKING_CAPACITE_MAX));

    appliquer_signalisation_etat(ETAT_REPOS);
    journal_evt("DEMARRAGE", "Systeme parking initialise");
}

void loop() {
    const uint32_t maintenant = millis();

    // 1. Mise à jour non bloquante des boutons
    boutons_update();

    // 2. Alternance des mesures ultrasons (C1 puis C2 toutes les US_PERIODE_ALTERNANCE_MS)
    if (maintenant - dernier_us_ms >= US_PERIODE_ALTERNANCE_MS) {
        dernier_us_ms = maintenant;
        if (interroger_us1) {
            capteur_entree.mesurer();
        } else {
            capteur_sortie.mesurer();
        }
        interroger_us1 = !interroger_us1;
    }

    // 3. Réarmement : effacement éventuel d'un défaut capteur verrouillé
    const bool appui_rearm = bouton_rearm_appui();
    if (appui_rearm) {
        capteur_entree.effacerDefaut();
        capteur_sortie.effacerDefaut();
    }

    // 4. Construction du vecteur d'entrées de l'automate
    EntreesAutomate ent;
    ent.temps_ms             = maintenant;
    ent.presence_entree      = capteur_entree.estVoiturePresente();
    ent.presence_sortie      = capteur_sortie.estVoiturePresente();
    ent.defaut_capteur       = capteur_entree.estEnDefaut() || capteur_sortie.estEnDefaut();
    ent.obstacle_ir          = detecteur_ir_obstacle();
    ent.impulsion_urgence    = bouton_au_appui();
    ent.impulsion_rearmement = appui_rearm;
    ent.impulsion_force      = false;
    ent.barriere_arrivee     = barriere_est_arrivee();

    // 5. Exécution d'un pas de la machine à états
    const SortiesAutomate out = automate_pas(ent);

    // 6. Journalisation et signalisation sur transition
    if (out.evenement != nullptr) {
        journal_evt(out.evenement, out.message_evt);
        if (out.message_evt != nullptr && out.message_evt[0] != '\0') {
            dernier_msg_ecran = out.message_evt;
        }
    }

    if (out.etat != dernier_etat) {
        dernier_etat = out.etat;
        appliquer_signalisation_etat(out.etat);
        if (out.etat == ETAT_ARRET_URGENCE || out.etat == ETAT_DEFAUT) {
            signalisation_bip_court();
        }
    }

    // 7. Pilotage non bloquant de la barrière et de la signalisation
    if (out.figer_barriere) {
        barriere_figer();
    } else {
        barriere_set_cible(out.angle_cible_barriere);
    }
    barriere_update();
    signalisation_update();

    // 8. Mise à jour des données et rafraîchissement non bloquant de l'écran OLED
    const int passages_occupes = out.capacite_max - out.places_libres;
    ecran_afficher(automate_nom_etat(out.etat),
                   passages_occupes,
                   ent.presence_entree,
                   ent.presence_sortie,
                   ent.obstacle_ir,
                   dernier_msg_ecran);
    ecran_update();

    // 9. Trame télémétrie périodique HB (PERIODE_MESSAGE_ETAT_MS = 500 ms)
    if (maintenant - dernier_hb_ms >= PERIODE_MESSAGE_ETAT_MS) {
        dernier_hb_ms = maintenant;
        journal_etat(automate_nom_etat(out.etat),
                     barriere_get_angle_actuel(),
                     static_cast<int>(capteur_entree.getDistanceCm()),
                     static_cast<int>(capteur_sortie.getDistanceCm()),
                     ent.obstacle_ir,
                     passages_occupes);
    }
}