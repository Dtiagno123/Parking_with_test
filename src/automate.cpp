#include "automate.h"
#include "config.h"

static EtatAutomate etat_courant         = ETAT_OUVERTE;
static uint32_t     debut_zone_libre_ms  = 0;
static bool         zone_libre_en_cours  = false;
static bool         voiture_vue_c2       = false;
static int          compteur_passages    = 0;
static uint32_t     timeout_sans_passage = TEMPO_TIMEOUT_PASSAGE_MS;

static void changer_etat(EtatAutomate nouvel_etat, const char* evt, const char* msg,
                         SortiesAutomate& out) {
    etat_courant        = nouvel_etat;
    zone_libre_en_cours = false;
    voiture_vue_c2      = false;
    out.evenement       = evt;
    out.message_evt     = msg;
}

void automate_init(void) {
    etat_courant         = ETAT_OUVERTE;
    debut_zone_libre_ms  = 0;
    zone_libre_en_cours  = false;
    voiture_vue_c2       = false;
    compteur_passages    = 0;
    timeout_sans_passage = TEMPO_TIMEOUT_PASSAGE_MS;
}

void automate_set_timeout_sans_passage(uint32_t timeout_ms) {
    timeout_sans_passage = timeout_ms;
}

const char* automate_nom_etat(EtatAutomate etat) {
    switch (etat) {
        case ETAT_FERMEE:        return "FERMEE";
        case ETAT_OUVERTURE:     return "OUVERTURE";
        case ETAT_OUVERTE:       return "OUVERTE";
        case ETAT_FERMETURE:     return "FERMETURE";
        case ETAT_DEFAUT:        return "DEFAUT";
        case ETAT_ARRET_URGENCE: return "ARRET_URGENCE";
        default:                 return "INCONNU";
    }
}

SortiesAutomate automate_pas(const EntreesAutomate& ent) {
    SortiesAutomate out;
    out.etat                     = etat_courant;
    out.angle_cible_barriere     = BARRIERE_ANGLE_FERME_DEG;
    out.figer_barriere           = false;
    out.effacer_defauts_capteurs = false;
    out.bip_court                = false;
    out.passages                 = compteur_passages;
    out.evenement                = nullptr;
    out.message_evt              = "";

    const bool un_defaut_capteur = ent.defaut_c1 || ent.defaut_c2;
    const bool zone_libre        = (!ent.voiture_c1 && !ent.voiture_c2 && !ent.obstacle_ir);

    // 1. Priorité absolue : arrêt d'urgence depuis n'importe quel état
    if (ent.appui_urgence && etat_courant != ETAT_ARRET_URGENCE) {
        changer_etat(ETAT_ARRET_URGENCE, "ARRET_URGENCE", "Appui arret urgence", out);
    }
    // 2. Priorité haute : défaut capteur depuis n'importe quel état sauf arrêt d'urgence
    else if (un_defaut_capteur &&
             etat_courant != ETAT_ARRET_URGENCE &&
             etat_courant != ETAT_DEFAUT) {
        changer_etat(ETAT_DEFAUT, "DEFAUT", "Defaut capteur detecte", out);
    }
    // 3. Transitions selon l'état courant
    else {
        switch (etat_courant) {
            case ETAT_FERMEE:
                if (ent.voiture_c1) {
                    changer_etat(ETAT_OUVERTURE, "OUVERTURE", "Voiture devant C1", out);
                }
                break;

            case ETAT_OUVERTURE:
                if (ent.angle_actuel_barriere >= BARRIERE_ANGLE_OUVERT_DEG) {
                    changer_etat(ETAT_OUVERTE, "OUVERTE", "Barriere ouverte", out);
                }
                break;

            case ETAT_OUVERTE:
                if (ent.voiture_c2) {
                    voiture_vue_c2 = true;
                }

                if (!zone_libre) {
                    zone_libre_en_cours = false;
                } else {
                    if (!zone_libre_en_cours) {
                        zone_libre_en_cours = true;
                        debut_zone_libre_ms = ent.temps_ms;
                    } else if (voiture_vue_c2 &&
                               (ent.temps_ms - debut_zone_libre_ms >= TEMPO_ATTENTE_PASSAGE_MS)) {
                        compteur_passages++;
                        changer_etat(ETAT_FERMETURE, "PASSAGE", "Passage comptabilise -> fermeture", out);
                    } else if (!voiture_vue_c2 &&
                               (ent.temps_ms - debut_zone_libre_ms >= timeout_sans_passage)) {
                        changer_etat(ETAT_FERMETURE, "FERMETURE", "Delai sans passage ecoule", out);
                    }
                }
                break;

            case ETAT_FERMETURE:
                if (ent.obstacle_ir || ent.voiture_c1 || ent.voiture_c2) {
                    out.bip_court = true;
                    changer_etat(ETAT_OUVERTURE, "REOUVERTURE", "Obstacle ou voiture -> reouverture", out);
                } else if (ent.angle_actuel_barriere <= BARRIERE_ANGLE_FERME_DEG) {
                    changer_etat(ETAT_FERMEE, "FERMEE", "Barriere fermee", out);
                }
                break;

            case ETAT_DEFAUT:
                if (ent.appui_rearmement) {
                    if (!un_defaut_capteur && ent.capteurs_sains) {
                        out.effacer_defauts_capteurs = true;
                        changer_etat(ETAT_OUVERTURE, "REARMEMENT", "Defaut acquitte -> ouverture", out);
                    } else {
                        out.evenement   = "REARMEMENT_REFUSE";
                        out.message_evt = "Capteur toujours en defaut";
                    }
                }
                break;

            case ETAT_ARRET_URGENCE:
                if (ent.appui_rearmement) {
                    if (un_defaut_capteur) {
                        changer_etat(ETAT_DEFAUT, "DEFAUT", "Rearmement AU avec capteur en defaut", out);
                    } else {
                        changer_etat(ETAT_OUVERTURE, "REARMEMENT", "Sortie arret urgence -> ouverture", out);
                    }
                }
                break;
        }
    }

    // 4. Sorties cohérentes avec l'état final
    out.etat     = etat_courant;
    out.passages = compteur_passages;

    if (etat_courant == ETAT_OUVERTURE ||
        etat_courant == ETAT_OUVERTE ||
        etat_courant == ETAT_DEFAUT) {
        out.angle_cible_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    } else {
        out.angle_cible_barriere = BARRIERE_ANGLE_FERME_DEG;
    }

    out.figer_barriere = (etat_courant == ETAT_ARRET_URGENCE);

    return out;
}