#ifndef CAPTEUR_ULTRASON_H
#define CAPTEUR_ULTRASON_H

#include <Arduino.h>

enum ModeSimuCapteur {
    SIMU_CAPTEUR_INACTIVE,
    SIMU_CAPTEUR_VOITURE_ABSENTE,
    SIMU_CAPTEUR_VOITURE_PRESENTE,
    SIMU_CAPTEUR_PANNE
};

class CapteurUltrason {
public:
    /**
     * @brief Construit un capteur ultrason avec son nom et ses broches.
     */
    CapteurUltrason(const char* nom, uint8_t pin_trig, uint8_t pin_echo);

    /**
     * @brief Configure les broches matérielles du capteur.
     */
    void init();

    /**
     * @brief Réalise une mesure et applique les 4 règles (mesure, validité, défaut, présence).
     */
    void mesurer();

    /**
     * @brief Efface le défaut verrouillé (utilisé par le bouton de réarmement).
     */
    void effacerDefaut();

    /**
     * @brief Force une valeur simulée pour le banc de test (Fichier 11).
     */
    void setSimulation(ModeSimuCapteur mode);

    // Accesseurs d'état
    float getDistanceCm() const;
    bool estVoiturePresente() const;
    bool estEnDefaut() const;
    bool derniereMesureValide() const;
    uint8_t getCompteurEchecs() const;

private:
    const char* _nom;
    uint8_t _pin_trig;
    uint8_t _pin_echo;

    float _derniere_distance_cm;
    bool _derniere_valide;
    bool _voiture_presente;
    bool _en_defaut;

    uint8_t _compteur_echecs;
    uint8_t _compteur_present;
    uint8_t _compteur_absent;

    ModeSimuCapteur _mode_simu;
};

#endif // CAPTEUR_ULTRASON_H