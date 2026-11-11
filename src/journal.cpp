#include "journal.h"

void journal_init(void) {
    Serial.begin(115200);
}

void journal_evt(const char* type, const char* detail) {
    unsigned long ms = millis();
    Serial.print("EVT,");
    Serial.print(ms);
    Serial.print(",");
    Serial.print(type);
    Serial.print(",");
    Serial.println(detail);
}

void journal_etat(const char* etat_auto, int angle, int dist_c1, int dist_c2, bool ir_obstacle, int passages) {
    unsigned long ms = millis();
    Serial.print("HB,");
    Serial.print(ms);
    Serial.print(",");
    Serial.print(etat_auto);
    Serial.print(",");
    Serial.print(angle);
    Serial.print(",");
    Serial.print(dist_c1);
    Serial.print(",");
    Serial.print(dist_c2);
    Serial.print(",");
    Serial.print(ir_obstacle ? "1" : "0");
    Serial.print(",");
    Serial.println(passages);
}