#include "barriere.h"
#include "config.h"
#include <ESP32Servo.h>

static Servo servo_barriere;
static int angle_actuel = BARRIERE_ANGLE_OUVERT_DEG;
static int angle_cible  = BARRIERE_ANGLE_OUVERT_DEG;
static bool figee       = false;
static unsigned long dernier_pas_ms = 0;

void barriere_init(void) {
    servo_barriere.setPeriodHertz(50); // Fréquence standard 50 Hz pour SG90
    servo_barriere.attach(PIN_SERVO_BARRIERE, 500, 2400);

    angle_actuel = BARRIERE_ANGLE_OUVERT_DEG;
    angle_cible  = BARRIERE_ANGLE_OUVERT_DEG;
    figee        = false;

    servo_barriere.write(angle_actuel);
    dernier_pas_ms = millis();
}

void barriere_set_cible(int angle_deg) {
    if (angle_deg < 0) angle_deg = 0;
    if (angle_deg > 180) angle_deg = 180;

    angle_cible = angle_deg;
    figee = false;
}

void barriere_figer(void) {
    figee = true;
    angle_cible = angle_actuel;
}

int barriere_get_angle_actuel(void) {
    return angle_actuel;
}

bool barriere_est_arrivee(void) {
    return (angle_actuel == angle_cible);
}

void barriere_update(void) {
    if (figee || angle_actuel == angle_cible) {
        return;
    }

    unsigned long ms = millis();
    if (ms - dernier_pas_ms >= BARRIERE_DELAI_PAS_MS) {
        dernier_pas_ms = ms;

        if (angle_actuel < angle_cible) {
            angle_actuel++;
        } else if (angle_actuel > angle_cible) {
            angle_actuel--;
        }

        servo_barriere.write(angle_actuel);
    }
}