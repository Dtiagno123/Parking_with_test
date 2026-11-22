#include "capteur_ultrason.h"
#include "config.h"
#include "journal.h"

CapteurUltrason::CapteurUltrason(const char* nom, uint8_t pin_trig, uint8_t pin_echo)
    : _nom(nom),
      _pin_trig(pin_trig),
      _pin_echo(pin_echo),
      _derniere_distance_cm(-1.0f),
      _derniere_valide(false),
      _voiture_presente(false),
      _en_defaut(false),
      _compteur_echecs(0),
      _compteur_present(0),
      _compteur_absent(0),
      _mode_simu(SIMU_CAPTEUR_INACTIVE) {}

void CapteurUltrason::init() {
    pinMode(_pin_trig, OUTPUT);
    pinMode(_pin_echo, INPUT);
    digitalWrite(_pin_trig, LOW);
}

void CapteurUltrason::mesurer() {
    // Gestion du mode simulation (prioritaire si activé au Fichier 11)
    if (_mode_simu != SIMU_CAPTEUR_INACTIVE) {
        if (_mode_simu == SIMU_CAPTEUR_PANNE) {
            _derniere_distance_cm = -1.0f;
            _derniere_valide = false;
            _compteur_echecs++;
            if (_compteur_echecs >= US_MAX_ECHECS_AVANT_DEFAUT && !_en_defaut) {
                _en_defaut = true;
                char detail[48];
                snprintf(detail, sizeof(detail), "Panne simulee sur %s", _nom);
                journal_evt("DEFAUT_CAPTEUR", detail);
            }
            return;
        }

        _derniere_valide = true;
        _compteur_echecs = 0;
        if (_mode_simu == SIMU_CAPTEUR_VOITURE_PRESENTE) {
            _derniere_distance_cm = US_SEUIL_PRESENCE_CM - 4.0f;
            _voiture_presente = true;
        } else {
            _derniere_distance_cm = US_SEUIL_PRESENCE_CM + US_HYSTERESIS_CM + 5.0f;
            _voiture_presente = false;
        }
        return;
    }

    // 1. Mesurer : impulsion de 10 us sur TRIG
    digitalWrite(_pin_trig, LOW);
    delayMicroseconds(2);
    digitalWrite(_pin_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(_pin_trig, LOW);

    unsigned long duree_us = pulseIn(_pin_echo, HIGH, US_TIMEOUT_ECHO_US);

    // 2. Valider la mesure
    float dist = -1.0f;
    bool valide = false;
    if (duree_us > 0) {
        dist = (duree_us * 0.0343f) / 2.0f;
        if (dist >= US_DIST_MIN_CM && dist <= US_DIST_MAX_CM) {
            valide = true;
        }
    }

    _derniere_distance_cm = valide ? dist : -1.0f;
    _derniere_valide = valide;

    // 3. Détecter la panne (verrouillée)
    if (!valide) {
        _compteur_echecs++;
        if (_compteur_echecs >= US_MAX_ECHECS_AVANT_DEFAUT && !_en_defaut) {
            _en_defaut = true;
            char detail[48];
            snprintf(detail, sizeof(detail), "Perte echo sur %s", _nom);
            journal_evt("DEFAUT_CAPTEUR", detail);
        }
        // Une mesure invalide ne change pas la décision de présence
        return;
    }

    // Mesure valide : remise à zéro du compteur d'échecs
    _compteur_echecs = 0;

    // 4. Décider de la présence avec hystérésis et confirmation
    if (!_voiture_presente) {
        if (dist < US_SEUIL_PRESENCE_CM) {
            _compteur_present++;
            if (_compteur_present >= US_NB_CONFIRMATIONS) {
                _voiture_presente = true;
                _compteur_present = 0;
            }
        } else {
            _compteur_present = 0;
        }
    } else {
        if (dist > (US_SEUIL_PRESENCE_CM + US_HYSTERESIS_CM)) {
            _compteur_absent++;
            if (_compteur_absent >= US_NB_CONFIRMATIONS) {
                _voiture_presente = false;
                _compteur_absent = 0;
            }
        } else {
            _compteur_absent = 0;
        }
    }
}

void CapteurUltrason::effacerDefaut() {
    _en_defaut = false;
    _compteur_echecs = 0;
}

void CapteurUltrason::setSimulation(ModeSimuCapteur mode) {
    _mode_simu = mode;
}

float CapteurUltrason::getDistanceCm() const {
    return _derniere_distance_cm;
}

bool CapteurUltrason::estVoiturePresente() const {
    return _voiture_presente;
}

bool CapteurUltrason::estEnDefaut() const {
    return _en_defaut;
}

bool CapteurUltrason::derniereMesureValide() const {
    return _derniere_valide;
}

uint8_t CapteurUltrason::getCompteurEchecs() const {
    return _compteur_echecs;
}