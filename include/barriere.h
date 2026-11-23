#ifndef BARRIERE_H
#define BARRIERE_H

#include <Arduino.h>

/**
 * @brief Initialise le servomoteur (50 Hz, impulsions 500-2400 us) et le place à l'angle ouvert (position sûre).
 */
void barriere_init(void);

/**
 * @brief Définit un nouvel angle cible (en degrés) et débloque le mouvement s'il était figé.
 */
void barriere_set_cible(int angle_deg);

/**
 * @brief Fige immédiatement la barrière à son angle actuel (arrêt d'urgence).
 */
void barriere_figer(void);

/**
 * @brief Renvoie l'angle actuel commandé au servomoteur (en degrés).
 */
int barriere_get_angle_actuel(void);

/**
 * @brief Indique si la barrière a atteint son angle cible (ou est figée).
 */
bool barriere_est_arrivee(void);

/**
 * @brief Fait avancer la barrière d'un degré si le délai est écoulé (non bloquant).
 *        À appeler à chaque tour de boucle (loop).
 */
void barriere_update(void);

#endif // BARRIERE_H