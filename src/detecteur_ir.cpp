#include "detecteur_ir.h"
#include "config.h"

static ModeSimuIR mode_simu = SIMU_IR_INACTIVE;

void detecteur_ir_init(void) {
    pinMode(PIN_IR_OBSTACLE, INPUT);
}

bool detecteur_ir_obstacle(void) {
    if (mode_simu == SIMU_IR_OBSTACLE) {
        return true;
    }
    if (mode_simu == SIMU_IR_LIBRE) {
        return false;
    }

    int niveau = digitalRead(PIN_IR_OBSTACLE);
    if (IR_ACTIF_BAS) {
        return (niveau == LOW);
    }
    return (niveau == HIGH);
}

void detecteur_ir_set_simulation(ModeSimuIR mode) {
    mode_simu = mode;
}