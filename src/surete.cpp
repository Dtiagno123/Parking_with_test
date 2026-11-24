#include "surete.h"
#include <Arduino.h>
#include <esp_system.h>

void surete_activer_watchdog(void) {
    enableLoopWDT();
}

const char* surete_cause_redemarrage(void) {
    switch (esp_reset_reason()) {
        case ESP_RST_POWERON:
            return "MISE_SOUS_TENSION";

        case ESP_RST_BROWNOUT:
            return "CHUTE_DE_TENSION";

        case ESP_RST_TASK_WDT:
        case ESP_RST_INT_WDT:
        case ESP_RST_WDT:
            return "WATCHDOG";

        case ESP_RST_PANIC:
            return "PLANTAGE";

        case ESP_RST_SW:
            return "REDEMARRAGE_LOGICIEL";

        default:
            return "AUTRE";
    }
}