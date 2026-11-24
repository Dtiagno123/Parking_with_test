#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <stdint.h>
#include <stdbool.h>

enum EtatAutomate {
    ETAT_FERMEE,
    ETAT_OUVERTURE,
    ETAT_OUVERTE,
    ETAT_FERMETURE,
    ETAT_DEFAUT,
    ETAT_ARRET_URGENCE
};

struct EntreesAutomate {
    uint32_t temps_ms;
    bool voiture_c1;
    bool voiture_c2;
    bool obstacle_ir;
    bool defaut_c1;
    bool defaut_c2;
    bool capteurs_sains;       // Dernières mesures valides sur C1 et C2
    bool appui_urgence;
    bool appui_rearmement;
    int  angle_actuel_barriere;
};

struct SortiesAutomate {
    EtatAutomate etat;
    int          angle_cible_barriere;
    bool         figer_barriere;
    bool         effacer_defauts_capteurs;
    bool         bip_court;
    int          passages;
    const char*  evenement;    // nullptr si aucun événement à journaliser
    const char*  message_evt;
};

/**
 * @brief Initialise l'automate en position sûre (ETAT_OUVERTE), compteur à 0.
 */
void automate_init(void);

/**
 * @brief Modifie le délai de fermeture sans passage (en ms), utilisé par TIMEOUT.
 */
void automate_set_timeout_sans_passage(uint32_t timeout_ms);

/**
 * @brief Exécute un pas de la machine à états à partir des entrées courantes.
 */
SortiesAutomate automate_pas(const EntreesAutomate& ent);

/**
 * @brief Renvoie le libellé texte de l'état (pour le journal et l'écran OLED).
 */
const char* automate_nom_etat(EtatAutomate etat);

#endif // AUTOMATE_H