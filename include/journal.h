#ifndef JOURNAL_H
#define JOURNAL_H

#include <Arduino.h>

/**
 * @brief Initialise la liaison série pour le journal de bord à 115200 bauds.
 */
void journal_init(void);

/**
 * @brief Envoie une ligne d'événement au format: EVT,<temps_ms>,<TYPE>,<DETAIL>
 * 
 * @param type Libellé du type d'événement (en MAJUSCULES, sans espace ni virgule)
 * @param detail Information complémentaire (sans accent ni virgule)
 */
void journal_evt(const char* type, const char* detail);

/**
 * @brief Envoie la ligne d'état complet (Heartbeat) au format:
 *        HB,<temps_ms>,<etat_auto>,<angle>,<dist_c1>,<dist_c2>,<ir>,<passages>
 * 
 * @param etat_auto Nom ou numéro de l'état actuel de l'automate
 * @param angle Angle actuel du servomoteur
 * @param dist_c1 Distance mesurée par le capteur 1 (cm)
 * @param dist_c2 Distance mesurée par le capteur 2 (cm)
 * @param ir_obstacle Vrai si un obstacle IR est présent
 * @param passages Nombre total de passages comptabilisés
 */
void journal_etat(const char* etat_auto, int angle, int dist_c1, int dist_c2, bool ir_obstacle, int passages);

#endif // JOURNAL_H