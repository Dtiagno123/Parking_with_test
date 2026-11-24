#ifndef SURETE_H
#define SURETE_H

/**
 * @brief Active le watchdog matériel sur la boucle principale (loop).
 *        Si un tour de boucle dépasse environ 5 secondes, l'ESP32 redémarre.
 */
void surete_activer_watchdog(void);

/**
 * @brief Renvoie la cause du dernier redémarrage sous forme de texte court :
 *        MISE_SOUS_TENSION, CHUTE_DE_TENSION, WATCHDOG, PLANTAGE,
 *        REDEMARRAGE_LOGICIEL ou AUTRE.
 */
const char* surete_cause_redemarrage(void);

#endif // SURETE_H