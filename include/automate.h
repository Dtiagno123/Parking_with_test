#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <stdint.h>
#include <stdbool.h>

enum EtatAutomate {
    ETAT_REPOS,
    ETAT_OUVERTURE,
    ETAT_ATTENTE_PASSAGE,
    ETAT_FERMETURE,
    ETAT_REOUVERTURE_OBSTACLE,
    ETAT_DEFAUT,
    ETAT_ARRET_URGENCE
};

struct EntreesAutomate {
    uint32_t temps_ms;
    bool presence_entree;      // Capteur 1 (avant barrière)
    bool presence_sortie;      // Capteur 2 (après barrière)
    bool defaut_capteur;       // Défaut verrouillé sur C1 ou C2
    bool obstacle_ir;          // Détecteur IR sous la barrière
    bool impulsion_urgence;    // Appui sur bouton arrêt d'urgence
    bool impulsion_rearmement; // Appui sur bouton réarmement
    bool impulsion_force;      // Appui sur bouton ouverture manuelle
    bool barriere_arrivee;     // True quand le servo a atteint son angle cible
};

struct SortiesAutomate {
    EtatAutomate etat;
    int angle_cible_barriere;
    bool figer_barriere;
    int places_libres;
    int capacite_max;
    const char* evenement;     // Nom de l'événement journalisable (nullptr si aucun)
    const char* message_evt;   // Détail de l'événement
};

/**
 * @brief Initialise l'automate au repos, barrière fermée et parking vide.
 */
void automate_init(int capacite_max);

/**
 * @brief Calcule un pas de la machine à états à partir des entrées courantes.
 */
SortiesAutomate automate_pas(const EntreesAutomate& ent);

/**
 * @brief Renvoie le libellé texte d'un état (pour l'affichage OLED et le journal).
 */
const char* automate_nom_etat(EtatAutomate etat);

#endif // AUTOMATE_H