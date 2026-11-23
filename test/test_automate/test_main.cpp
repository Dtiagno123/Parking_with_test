#include <unity.h>
#include "automate.h"
#include "config.h"

static EntreesAutomate entrees_vierges(uint32_t temps_ms) {
    EntreesAutomate e;
    e.temps_ms             = temps_ms;
    e.presence_entree      = false;
    e.presence_sortie      = false;
    e.defaut_capteur       = false;
    e.obstacle_ir          = false;
    e.impulsion_urgence    = false;
    e.impulsion_rearmement = false;
    e.impulsion_force      = false;
    e.barriere_arrivee     = false;
    return e;
}

void setUp(void) {
    automate_init(5);
}

void tearDown(void) {}

void test_etat_initial_repos(void) {
    EntreesAutomate e = entrees_vierges(0);
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_REPOS, s.etat);
    TEST_ASSERT_EQUAL(BARRIERE_ANGLE_FERME_DEG, s.angle_cible_barriere);
    TEST_ASSERT_EQUAL(5, s.places_libres);
}

void test_cycle_entree_complet_decremente_places(void) {
    EntreesAutomate e = entrees_vierges(100);
    e.presence_entree = true;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_OUVERTURE, s.etat);
    TEST_ASSERT_EQUAL(BARRIERE_ANGLE_OUVERT_DEG, s.angle_cible_barriere);

    // Fin ouverture
    e = entrees_vierges(1200);
    e.barriere_arrivee = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_ATTENTE_PASSAGE, s.etat);

    // Véhicule franchit le capteur sortie
    e = entrees_vierges(1500);
    e.presence_sortie = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_ATTENTE_PASSAGE, s.etat);

    // Voie dégagée puis attente de TEMPO_ATTENTE_PASSAGE_MS
    e = entrees_vierges(2000);
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_ATTENTE_PASSAGE, s.etat);

    e = entrees_vierges(2000 + TEMPO_ATTENTE_PASSAGE_MS);
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_FERMETURE, s.etat);
    TEST_ASSERT_EQUAL(4, s.places_libres);

    // Fin fermeture
    e = entrees_vierges(6500);
    e.barriere_arrivee = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_REPOS, s.etat);
}

void test_entree_sans_franchissement_ne_decremente_pas(void) {
    EntreesAutomate e = entrees_vierges(100);
    e.presence_entree = true;
    automate_pas(e);

    e = entrees_vierges(1000);
    e.barriere_arrivee = true;
    automate_pas(e);

    // La voiture recule sans passer devant le capteur sortie
    e = entrees_vierges(1200);
    automate_pas(e);

    e = entrees_vierges(1200 + TEMPO_ATTENTE_PASSAGE_MS);
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_FERMETURE, s.etat);
    TEST_ASSERT_EQUAL(5, s.places_libres);
}

void test_parking_complet_refuse_entree_mais_accepte_sortie(void) {
    automate_init(1); // 1 seule place

    // Entrée d'un véhicule -> 0 place
    EntreesAutomate e = entrees_vierges(100);
    e.presence_entree = true;
    automate_pas(e);
    e = entrees_vierges(500); e.barriere_arrivee = true; automate_pas(e);
    e = entrees_vierges(600); e.presence_sortie = true; automate_pas(e);
    e = entrees_vierges(700); automate_pas(e);
    e = entrees_vierges(700 + TEMPO_ATTENTE_PASSAGE_MS); automate_pas(e);
    e = entrees_vierges(5000); e.barriere_arrivee = true;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(0, s.places_libres);

    // Deuxième véhicule en entrée -> refusé (reste au REPOS)
    e = entrees_vierges(5100);
    e.presence_entree = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_REPOS, s.etat);

    // Véhicule en sortie -> autorisé !
    e = entrees_vierges(5200);
    e.presence_sortie = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_OUVERTURE, s.etat);
}

void test_obstacle_ir_pendant_fermeture_reouvre(void) {
    EntreesAutomate e = entrees_vierges(100);
    e.impulsion_force = true;
    automate_pas(e);

    e = entrees_vierges(1000); e.barriere_arrivee = true; automate_pas(e);
    e = entrees_vierges(1100); automate_pas(e);
    e = entrees_vierges(1100 + TEMPO_ATTENTE_PASSAGE_MS);
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_FERMETURE, s.etat);

    // Obstacle IR détecté en pleine fermeture
    e = entrees_vierges(4500);
    e.obstacle_ir = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_REOUVERTURE_OBSTACLE, s.etat);
    TEST_ASSERT_EQUAL(BARRIERE_ANGLE_OUVERT_DEG, s.angle_cible_barriere);
}

void test_arret_urgence_fige_et_rearmement(void) {
    EntreesAutomate e = entrees_vierges(100);
    e.presence_entree = true;
    automate_pas(e);

    // Appui arrêt d'urgence en pleine ouverture
    e = entrees_vierges(300);
    e.impulsion_urgence = true;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_ARRET_URGENCE, s.etat);
    TEST_ASSERT_TRUE(s.figer_barriere);

    // Réarmement -> passe en fermeture
    e = entrees_vierges(800);
    e.impulsion_rearmement = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_FERMETURE, s.etat);
    TEST_ASSERT_FALSE(s.figer_barriere);
}

void test_defaut_capteur_bloque_et_rearmement(void) {
    EntreesAutomate e = entrees_vierges(100);
    e.defaut_capteur = true;
    SortiesAutomate s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_DEFAUT, s.etat);

    // Réarmement tant que le défaut est présent -> bloqué en DEFAUT
    e = entrees_vierges(200);
    e.defaut_capteur = true;
    e.impulsion_rearmement = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_DEFAUT, s.etat);

    // Défaut disparu + réarmement -> retour au REPOS
    e = entrees_vierges(300);
    e.defaut_capteur = false;
    e.impulsion_rearmement = true;
    s = automate_pas(e);
    TEST_ASSERT_EQUAL(ETAT_REPOS, s.etat);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_etat_initial_repos);
    RUN_TEST(test_cycle_entree_complet_decremente_places);
    RUN_TEST(test_entree_sans_franchissement_ne_decremente_pas);
    RUN_TEST(test_parking_complet_refuse_entree_mais_accepte_sortie);
    RUN_TEST(test_obstacle_ir_pendant_fermeture_reouvre);
    RUN_TEST(test_arret_urgence_fige_et_rearmement);
    RUN_TEST(test_defaut_capteur_bloque_et_rearmement);
    return UNITY_END();
}