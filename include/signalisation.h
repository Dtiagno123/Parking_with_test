#ifndef SIGNALISATION_H
#define SIGNALISATION_H

#include <Arduino.h>

// Énumération des modes de signalisation
enum ModeSignalisation {
    SIG_MODE_FERME,
    SIG_MODE_EN_MOUVEMENT,
    SIG_MODE_OUVERT,
    SIG_MODE_DEFAUT,
    SIG_MODE_ARRET_URGENCE
};

/**
 * @brief Initialise les broches (sorties) et éteint toutes les LED et le buzzer.
 */
void signalisation_init(void);

/**
 * @brief Change le mode de signalisation actuel.
 */
void signalisation_set_mode(ModeSignalisation mode);

/**
 * @brief Déclenche un bip sonore court instantané.
 */
void signalisation_bip_court(void);

/**
 * @brief Mise à jour non bloquante des LED et du buzzer.
 *        À appeler obligatoirement à chaque tour de boucle (loop).
 */
void signalisation_update(void);

#endif