#include <unity.h>
#include "automate.h"
#include "config.h"

static EntreesAutomate entrees_base(uint32_t temps_ms) {
    EntreesAutomate e;
    e.temps_ms              = temps_ms;
    e.voiture_c1            = false;
    e.voiture_c2            = false;
    e.obstacle_ir           = false;
    e.defaut_c1             = false;
    e.defaut_c2             = false;
    e.capteurs_sains        = true;
    e.appui_urgence         = false;
    e.appui_rearmement      = false;
    e.angle_actuel_barriere = BARRIERE_ANGLE_FERME_DEG;
    return e;
}

void setUp(void) {
    automate_init();
}

void tearDown(void) {}

// 1. Au démarrage, l'automate est « ouverte » et se ferme après 10 s de zone libre.
void test_01_demarrage_ouverte_puis_fermeture_10s(void) {
    EntreesAutomate e = entrees_base(1000);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_OUVERTE, s.etat);
    TEST_ASSERT_EQUAL(BARRIERE_ANGLE_OUVERT_DEG, s.angle_cible_barriere);

    e = entrees_base(1000 + TEMPO_TIMEOUT_PASSAGE_MS);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_FERMETURE, s.etat);
    TEST_ASSERT_EQUAL(BARRIERE_ANGLE_FERME_DEG, s.angle_cible_barriere);
}

// 2. Passage nominal: fermée -> ouverture -> ouverte -> fermeture -> fermée, passages = 1.
void test_02_passage_nominal_compteur_1(void) {
    // On amène d'abord l'automate en ETAT_FERMEE
    EntreesAutomate e = entrees_base(0);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    automate_pas(e);
    e = entrees_base(TEMPO_TIMEOUT_PASSAGE_MS);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    automate_pas(e);
    e = entrees_base(12000);
    e.angle_actuel_barriere = BARRIERE_ANGLE_FERME_DEG;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_FERMEE, s.etat);

    // Voiture devant C1 -> OUVERTURE
    e = entrees_base(13000);
    e.voiture_c1 = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_OUVERTURE, s.etat);

    // Angle atteint -> OUVERTE
    e = entrees_base(14200);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_OUVERTE, s.etat);

    // Voiture devant C2
    e = entrees_base(15000);
    e.voiture_c2 = true;
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    automate_pas(e);

    // Zone libre pendant 1,5 s -> FERMETURE et passages == 1
    e = entrees_base(16000);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    automate_pas(e);

    e = entrees_base(16000 + TEMPO_ATTENTE_PASSAGE_MS);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_FERMETURE, s.etat);
    TEST_ASSERT_EQUAL(1, s.passages);

    // Fin de fermeture -> FERMEE
    e = entrees_base(19000);
    e.angle_actuel_barriere = BARRIERE_ANGLE_FERME_DEG;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_FERMEE, s.etat);
}

// 3. Voiture qui renonce (capteur 1 puis plus rien): fermeture après 10 s, compteur inchangé.
void test_03_voiture_renonce_fermeture_10s(void) {
    // Mise en état FERMEE
    EntreesAutomate e = entrees_base(0);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    automate_pas(e);
    e = entrees_base(TEMPO_TIMEOUT_PASSAGE_MS);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    automate_pas(e);
    e = entrees_base(12000);
    automate_pas(e);

    // Voiture devant C1 puis recule sans toucher C2
    e = entrees_base(13000); e.voiture_c1 = true; automate_pas(e);
    e = entrees_base(14000); e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG; automate_pas(e);
    e = entrees_base(15000); e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG; automate_pas(e);

    e = entrees_base(15000 + TEMPO_TIMEOUT_PASSAGE_MS);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_FERMETURE, s.etat);
    TEST_ASSERT_EQUAL(0, s.passages);
}

// 4. Obstacle pendant la fermeture: retour immédiat en ouverture, avec un bip.
void test_04_obstacle_fermeture_reouvre_avec_bip(void) {
    EntreesAutomate e = entrees_base(0);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    automate_pas(e);
    e = entrees_base(TEMPO_TIMEOUT_PASSAGE_MS);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    automate_pas(e); // Passe en ETAT_FERMETURE

    e = entrees_base(10500);
    e.angle_actuel_barriere = 45;
    e.obstacle_ir = true;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_OUVERTURE, s.etat);
    TEST_ASSERT_TRUE(s.bip_court);
    TEST_ASSERT_EQUAL(BARRIERE_ANGLE_OUVERT_DEG, s.angle_cible_barriere);
}

// 5. Voiture arrêtée sous la barrière : jamais de fermeture tant que l'IR la voit.
void test_05_voiture_arretee_sous_barriere_pas_de_fermeture(void) {
    EntreesAutomate e = entrees_base(0);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    e.obstacle_ir = true;
    automate_pas(e);

    e = entrees_base(50000); // Bien au-delà de 10 s
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    e.obstacle_ir = true;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_OUVERTE, s.etat);
}

// 6. Défaut capteur en position fermée: état défaut, angle cible = ouvert.
void test_06_defaut_capteur_en_fermee_repli_ouvert(void) {
    EntreesAutomate e = entrees_base(0);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    automate_pas(e);
    e = entrees_base(TEMPO_TIMEOUT_PASSAGE_MS);
    e.angle_actuel_barriere = BARRIERE_ANGLE_OUVERT_DEG;
    automate_pas(e);
    e = entrees_base(12000);
    automate_pas(e); // ETAT_FERMEE

    e = entrees_base(13000);
    e.defaut_c1 = true;
    e.capteurs_sains = false;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_DEFAUT, s.etat);
    TEST_ASSERT_EQUAL(BARRIERE_ANGLE_OUVERT_DEG, s.angle_cible_barriere);
}

// 7. Réarmement pendant que le capteur est encore en panne: refusé, reste en défaut.
void test_07_rearmement_refuse_si_capteur_en_panne(void) {
    EntreesAutomate e = entrees_base(100);
    e.defaut_c1 = true;
    e.capteurs_sains = false;
    automate_pas(e);

    e = entrees_base(200);
    e.defaut_c1 = true;
    e.capteurs_sains = false;
    e.appui_rearmement = true;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_DEFAUT, s.etat);
    TEST_ASSERT_FALSE(s.effacer_defauts_capteurs);
}

// 8. Réarmement après réparation: sortie du défaut et ordre d'effacer les défauts capteurs.
void test_08_rearmement_accepte_apres_reparation(void) {
    EntreesAutomate e = entrees_base(100);
    e.defaut_c1 = true;
    e.capteurs_sains = false;
    automate_pas(e);

    e = entrees_base(200);
    e.defaut_c1 = false;
    e.capteurs_sains = true;
    e.appui_rearmement = true;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_OUVERTURE, s.etat);
    TEST_ASSERT_TRUE(s.effacer_defauts_capteurs);
}

// 9. Arrêt d'urgence en pleine ouverture: barrière figée, puis reprise au réarmement.
void test_09_arret_urgence_fige_et_reprise(void) {
    EntreesAutomate e = entrees_base(100);
    e.appui_urgence = true;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_ARRET_URGENCE, s.etat);
    TEST_ASSERT_TRUE(s.figer_barriere);

    e = entrees_base(500);
    e.appui_rearmement = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_OUVERTURE, s.etat);
    TEST_ASSERT_FALSE(s.figer_barriere);
}

// 10. Arrêt d'urgence puis réarmement alors qu'un capteur est en défaut : passage en défaut.
void test_10_arret_urgence_rearmement_avec_defaut_capteur(void) {
    EntreesAutomate e = entrees_base(100);
    e.appui_urgence = true;
    automate_pas(e);

    e = entrees_base(500);
    e.defaut_c2 = true;
    e.capteurs_sains = false;
    e.appui_rearmement = true;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_DEFAUT, s.etat);
}

// 11. Un défaut capteur pendant un arrêt d'urgence ne fait pas quitter l'arrêt d'urgence.
void test_11_defaut_pendant_arret_urgence_reste_en_urgence(void) {
    EntreesAutomate e = entrees_base(100);
    e.appui_urgence = true;
    automate_pas(e);

    e = entrees_base(300);
    e.defaut_c1 = true;
    e.capteurs_sains = false;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_ARRET_URGENCE, s.etat);
    TEST_ASSERT_TRUE(s.figer_barriere);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_01_demarrage_ouverte_puis_fermeture_10s);
    RUN_TEST(test_02_passage_nominal_compteur_1);
    RUN_TEST(test_03_voiture_renonce_fermeture_10s);
    RUN_TEST(test_04_obstacle_fermeture_reouvre_avec_bip);
    RUN_TEST(test_05_voiture_arretee_sous_barriere_pas_de_fermeture);
    RUN_TEST(test_06_defaut_capteur_en_fermee_repli_ouvert);
    RUN_TEST(test_07_rearmement_refuse_si_capteur_en_panne);
    RUN_TEST(test_08_rearmement_accepte_apres_reparation);
    RUN_TEST(test_09_arret_urgence_fige_et_reprise);
    RUN_TEST(test_10_arret_urgence_rearmement_avec_defaut_capteur);
    RUN_TEST(test_11_defaut_pendant_arret_urgence_reste_en_urgence);
    return UNITY_END();
}