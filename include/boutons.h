#ifndef BOUTONS_H
#define BOUTONS_H

#include <Arduino.h>

/**
 * @brief Initialise les broches des boutons en mode INPUT_PULLUP.
 */
void boutons_init(void);

/**
 * @brief Met à jour l'état des boutons avec filtrage anti-rebond non bloquant.
 *        À appeler à chaque tour de boucle (loop).
 */
void boutons_update(void);

/**
 * @brief Indique si le bouton d'arrêt d'urgence est actuellement maintenu enfoncé.
 */
bool bouton_au_enfonce(void);

/**
 * @brief Indique si le bouton d'arrêt d'urgence vient d'être appuyé (vrai 1 seule fois par appui).
 */
bool bouton_au_appui(void);

/**
 * @brief Indique si le bouton de réarmement est actuellement maintenu enfoncé.
 */
bool bouton_rearm_enfonce(void);

/**
 * @brief Indique si le bouton de réarmement vient d'être appuyé (vrai 1 seule fois par appui).
 */
bool bouton_rearm_appui(void);

#endif // BOUTONS_H