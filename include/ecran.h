#ifndef ECRAN_H
#define ECRAN_H

#include <Arduino.h>

/**
 * @brief Vérifie la présence de l'écran sur le bus I2C et l'initialise.
 *        Passe en mode dégradé (et journalise ECRAN_ABSENT) s'il ne répond pas.
 * @return true si l'écran est présent et prêt, false sinon.
 */
bool ecran_init(void);

/**
 * @brief Indique si l'écran est disponible.
 */
bool ecran_disponible(void);

/**
 * @brief Met à jour les données à afficher sur l'écran.
 */
void ecran_afficher(const char* etat, int passages, bool c1_voiture, bool c2_voiture, bool ir_obstacle, const char* message);

/**
 * @brief Rafraîchit physiquement l'affichage OLED au maximum 4 fois par seconde (non bloquant).
 *        À appeler à chaque tour de boucle (loop).
 */
void ecran_update(void);

#endif // ECRAN_H