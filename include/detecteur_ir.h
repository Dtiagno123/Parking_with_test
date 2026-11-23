#ifndef DETECTEUR_IR_H
#define DETECTEUR_IR_H

#include <Arduino.h>

enum ModeSimuIR {
    SIMU_IR_INACTIVE,
    SIMU_IR_LIBRE,
    SIMU_IR_OBSTACLE
};

/**
 * @brief Initialise la broche du détecteur infrarouge en entrée.
 */
void detecteur_ir_init(void);

/**
 * @brief Indique s'il y a un obstacle sous la barrière (déjà converti selon IR_ACTIF_BAS).
 * @return true si un obstacle est présent, false sinon.
 */
bool detecteur_ir_obstacle(void);

/**
 * @brief Force une valeur simulée pour le banc de test (Fichier 11).
 */
void detecteur_ir_set_simulation(ModeSimuIR mode);

#endif // DETECTEUR_IR_H