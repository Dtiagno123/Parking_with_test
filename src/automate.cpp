#include "automate.h"
#include "config.h"

enum SensPassage {
    SENS_AUCUN,
    SENS_ENTREE,
    SENS_SORTIE
};

static EtatAutomate etat_courant      = ETAT_REPOS;
static SensPassage  sens_en_cours     = SENS_AUCUN;
static int          places_max        = PARKING_CAPACITE_MAX;
static int          places_dispo      = PARKING_CAPACITE_MAX;
static uint32_t     debut_tempo_ms    = 0;
static bool         vehicule_vu_apres = false;

static void changer_etat(EtatAutomate nouvel_etat, uint32_t temps_ms,
                         const char* evt, const char* msg,
                         SortiesAutomate& out) {
    etat_courant    = nouvel_etat;
    debut_tempo_ms  = temps_ms;
    out.evenement   = evt;
    out.message_evt = msg;
}

void automate_init(int capacite_max) {
    places_max        = (capacite_max > 0) ? capacite_max : 1;
    places_dispo      = places_max;
    etat_courant      = ETAT_REPOS;
    sens_en_cours     = SENS_AUCUN;
    debut_tempo_ms    = 0;
    vehicule_vu_apres = false;
}

const char* automate_nom_etat(EtatAutomate etat) {
    switch (etat) {
        case ETAT_REPOS:                return "REPOS";
        case ETAT_OUVERTURE:            return "OUVERTURE";
        case ETAT_ATTENTE_PASSAGE:      return "PASSAGE";
        case ETAT_FERMETURE:            return "FERMETURE";
        case ETAT_REOUVERTURE_OBSTACLE: return "OBSTACLE";
        case ETAT_DEFAUT:               return "DEFAUT";
        case ETAT_ARRET_URGENCE:        return "URGENCE";
        default:                        return "INCONNU";
    }
}

SortiesAutomate automate_pas(const EntreesAutomate& ent) {
    SortiesAutomate out;
    out.etat                 = etat_courant;
    out.angle_cible_barriere = BARRIERE_ANGLE_FERME_DEG;
    out.figer_barriere       = false;
    out.places_libres        = places_dispo;
    out.capacite_max         = places_max;
    out.evenement            = nullptr;
    out.message_evt          = "";

    // 1. Priorité absolue : arrêt d'urgence
    if (ent.impulsion_urgence && etat_courant != ETAT_ARRET_URGENCE) {
        sens_en_cours = SENS_AUCUN;
        changer_etat(ETAT_ARRET_URGENCE, ent.temps_ms,
                     "ARRET_URGENCE", "Appui bouton urgence", out);
    }
    // 2. Priorité haute : défaut capteur (sauf si déjà en urgence)
    else if (ent.defaut_capteur &&
             etat_courant != ETAT_ARRET_URGENCE &&
             etat_courant != ETAT_DEFAUT) {
        sens_en_cours = SENS_AUCUN;
        changer_etat(ETAT_DEFAUT, ent.temps_ms,
                     "DEFAUT_SYSTEME", "Capteur en defaut verrouille", out);
    }

    // 3. Transitions selon l'état courant
    switch (etat_courant) {
        case ETAT_ARRET_URGENCE:
            if (ent.impulsion_rearmement && !ent.defaut_capteur) {
                changer_etat(ETAT_FERMETURE, ent.temps_ms,
                             "REARMEMENT", "Sortie arret urgence -> fermeture", out);
            }
            break;

        case ETAT_DEFAUT:
            if (ent.impulsion_rearmement && !ent.defaut_capteur) {
                changer_etat(ETAT_REPOS, ent.temps_ms,
                             "REARMEMENT", "Defaut acquitte -> repos", out);
            }
            break;

        case ETAT_REPOS:
            vehicule_vu_apres = false;

            if (ent.impulsion_force) {
                sens_en_cours = SENS_AUCUN;
                changer_etat(ETAT_OUVERTURE, ent.temps_ms,
                             "OUVERTURE_MANUELLE", "Bouton ouverture forcee", out);
            } else if (ent.presence_entree && places_dispo > 0) {
                sens_en_cours = SENS_ENTREE;
                changer_etat(ETAT_OUVERTURE, ent.temps_ms,
                             "DEMANDE_ENTREE", "Vehicule detecte en entree", out);
            } else if (ent.presence_sortie) {
                sens_en_cours = SENS_SORTIE;
                changer_etat(ETAT_OUVERTURE, ent.temps_ms,
                             "DEMANDE_SORTIE", "Vehicule detecte en sortie", out);
            }
            break;

        case ETAT_OUVERTURE:
            if (ent.barriere_arrivee) {
                changer_etat(ETAT_ATTENTE_PASSAGE, ent.temps_ms,
                             "BARRIERE_OUVERTE", "Attente passage vehicule", out);
            }
            break;

        case ETAT_ATTENTE_PASSAGE:
            // Détection du franchissement du second capteur
            if (sens_en_cours == SENS_ENTREE && ent.presence_sortie) {
                vehicule_vu_apres = true;
            } else if (sens_en_cours == SENS_SORTIE && ent.presence_entree) {
                vehicule_vu_apres = true;
            }

            // Maintien du timer tant qu'un capteur ou l'IR est occupé
            if (ent.presence_entree || ent.presence_sortie || ent.obstacle_ir) {
                debut_tempo_ms = ent.temps_ms;
            } else if (ent.temps_ms - debut_tempo_ms >= TEMPO_ATTENTE_PASSAGE_MS) {
                // Mise à jour du compteur si le véhicule a bien franchi la barrière
                if (vehicule_vu_apres) {
                    if (sens_en_cours == SENS_ENTREE && places_dispo > 0) {
                        places_dispo--;
                    } else if (sens_en_cours == SENS_SORTIE && places_dispo < places_max) {
                        places_dispo++;
                    }
                }
                sens_en_cours = SENS_AUCUN;
                changer_etat(ETAT_FERMETURE, ent.temps_ms,
                             "FERMETURE_AUTO", "Voie libre -> fermeture", out);
            }
            break;

        case ETAT_FERMETURE:
            if (ent.obstacle_ir) {
                changer_etat(ETAT_REOUVERTURE_OBSTACLE, ent.temps_ms,
                             "OBSTACLE_FERMETURE", "Obstacle IR -> reouverture", out);
            } else if (ent.barriere_arrivee) {
                changer_etat(ETAT_REPOS, ent.temps_ms,
                             "BARRIERE_FERMEE", "Cycle termine -> repos", out);
            }
            break;

        case ETAT_REOUVERTURE_OBSTACLE:
            if (ent.barriere_arrivee) {
                changer_etat(ETAT_ATTENTE_PASSAGE, ent.temps_ms,
                             "REOUVERTURE_TERMINEE", "Attente degagement obstacle", out);
            }
            break;
    }

    // 4. Mise à jour des sorties cohérente avec l'état final après transition
    out.etat          = etat_courant;
    out.places_libres = places_dispo;

    if (etat_courant == ETAT_OUVERTURE ||
        etat_courant == ETAT_ATTENTE_PASSAGE ||
        etat_courant == ETAT_REOUVERTURE_OBSTACLE) {
        out.angle_cible_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    } else {
        out.angle_cible_barriere = BARRIERE_ANGLE_FERME_DEG;
    }

    out.figer_barriere = (etat_courant == ETAT_ARRET_URGENCE);

    return out;
}