# Carnet d'essais — Parking ESP32

## Fichier 1 : Configuration et démarrage (`platformio.ini` et `include/config.h`)
- **Date** : 11 Novembre 2026
- **Matériel testé** : Carte ESP32 DevKit v1 sur port série `/dev/ttyUSB0` (115 200 bauds)[cite: 4, 6].

### Essais réalisés :
1. **Compilation et téléversement** : Succès sans avertissement via `pio run -e essai_demarrage -t upload`[cite: 7].
2. **Exécution et redémarrage** : Affichage d'une ligne par seconde dans le moniteur série avec remise à zéro du compteur temps lors de l'appui sur le bouton `EN`[cite: 7].
3. **Dépôt Git** : Initialisation du dépôt, exclusion de `.pio/` dans `.gitignore` et premier push vers GitHub[cite: 6, 7].

### Résultat :
- Module validé le 11/11/2026[cite: 7].

---

## Fichier 2 : Journalisation (`journal.h` / `.cpp`)
- **Date** : 11 Novembre 2026
- **Matériel testé** : Liaison série USB à 115 200 bauds[cite: 7].

### Essais réalisés :
1. **Format des messages** : Contrat fixé dans `docs/format_messages.md` (séparateur `,`, sans accent ni espace dans les types)[cite: 7].
2. **Test d'émission** : Émission d'un événement `DEMARRAGE` au boot, suivie de 2 lignes d'état `HB` par seconde (toutes les 500 ms)[cite: 7].

### Problème rencontré et résolu :
- Échec initial du téléversement (`esptool: Unable to verify flash chip connection`) causé par une vitesse d'upload trop élevée (`190800` bauds)[cite: 28]. Résolu en passant `upload_speed = 115200` dans `platformio.ini`[cite: 29].

### Résultat :
- Module validé le 11/11/2026[cite: 8].

---

## Fichier 3 : Signalisation (`signalisation.h` / `.cpp`)
- **Date** : 11 Novembre 2026
- **Matériel testé** : LED Verte (GPIO 27), LED Jaune (GPIO 26), LED Rouge (GPIO 25) avec résistances de 220 Ω, Buzzer actif (GPIO 33)[cite: 8].

### Essais réalisés :
1. **Mode Fermé** : LED Rouge allumée fixe, autres éteintes, silence[cite: 8]. (OK)
2. **Mode En mouvement** : LED Jaune clignote rapidement, autres éteintes, silence[cite: 8]. (OK)
3. **Mode Ouvert** : LED Verte allumée fixe, autres éteintes, silence[cite: 8]. (OK)
4. **Mode Défaut** : LEDs Jaune et Rouge clignotent lentement, bip du buzzer toutes les 2 s[cite: 9]. (OK)
5. **Mode Arrêt d'urgence** : LED Rouge clignote rapidement, bip court du buzzer chaque seconde[cite: 9]. (OK)

### Mesures et observations :
- **Tours de boucle par seconde** : Restent très élevés dans tous les modes, confirmant l'absence de `delay()` et de blocage[cite: 9].
- **Problème rencontré** : La LED jaune ne s'allumait pas initialement (faux contact sur la breadboard corrigé).

### Résultat :
- Module validé le 11/11/2026[cite: 9].

---

## Fichier 4 : Boutons (`boutons.h` / `.cpp`)
- **Date** : 19 Novembre 2026
- **Matériel testé** : Bouton Arrêt d'urgence (GPIO 16), Bouton Réarmement (GPIO 17), câblés en `INPUT_PULLUP` vers GND[cite: 9].

### Essais réalisés :
- **Comparaison sans anti-rebond (`DELAI_ANTIREBOND_MS = 0`)** : Sur 20 appuis francs, les rebonds mécaniques provoquent plusieurs détections par appui (compteur supérieur à 20)[cite: 9, 10].
- **Avec anti-rebond (`DELAI_ANTIREBOND_MS = 20`)**[cite: 9, 10] :
  1. **20 appuis francs** : compteur exactement à 20 sur chaque bouton[cite: 10]. (OK)
  2. **20 appuis très rapides** : compteur à 20 (aucun double comptage)[cite: 10]. (OK)
  3. **Bouton maintenu 5 s** : 1 seul appui comptabilisé ; LED rouge allumée tant que l'AU est maintenu[cite: 10]. (OK)
  4. **Appui simultané sur les 2 boutons** : les deux compteurs s'incrémentent chacun de 1[cite: 10]. (OK)

### Note de sûreté (AMDEC) :
- Le câblage actuel utilise des boutons normalement ouverts (NO) au repos (passage à `LOW` lors de l'appui)[cite: 9, 10].
- **Mode de défaillance identifié** : En cas de coupure de fil ou de faux contact sur le bouton d'arrêt d'urgence, le système ne détecte pas la panne et l'arrêt d'urgence devient inopérant[cite: 10]. Sur un système industriel, un arrêt d'urgence doit impérativement être câblé avec un contact normalement fermé (NF) au repos pour garantir une sécurité positive (fail-safe)[cite: 10].

### Résultat :
- Module validé le 19/11/2026[cite: 10].

---

## Fichier 5 : Écran OLED (`ecran.h` / `.cpp`)
- **Date** : 19 Novembre 2026
- **Matériel testé** : Écran OLED 0,96" 128×64 sur rail 3,3 V, bus I2C (SDA sur GPIO 21, SCL sur GPIO 22)[cite: 10, 11].

### Essais réalisés :
1. **Scan I2C et affichage nominal** : Adresse `0x3C` détectée, affichage lisible des 4 lignes (état, passages, capteurs, message) et mise à jour fluide toutes les secondes[cite: 11].
2. **Mode dégradé (SDA débranché + bouton EN)** : L'événement `ECRAN_ABSENT` est bien émis au démarrage via `journal` et le programme continue sans blocage ni plantage[cite: 11].
3. **Performance (tours de boucle/s)** : Cadence de boucle élevée maintenue grâce au rafraîchissement limité à 4 Hz (`PERIODE_ECRAN_MS = 250`)[cite: 11, 30]
---

## Fichier 6 : Capteurs ultrasons (capteur_ultrason.h / .cpp)
- **Date** : 22 Novembre 2026
- **Matériel testé** : 2 capteurs HC-SR04 alimentés en 5 V (TRIG sur GPIO 5 et 19, ECHO via ponts diviseurs 1 kΩ / 2 kΩ vers GPIO 18 et 23) + murs de référence en carton à ~20 cm[cite: 12].

### Essais réalisés (les 8 cas du cahier des charges) :
1. **Rien devant** : distance stable correspondant au mur (~20 cm), présence à 0, aucun défaut[cite: 13].
2. **Voiture posée devant** : détection confirmée après 3 mesures consécutives (< 0,5 s)[cite: 13].
3. **Voiture déplacée lentement autour du seuil** : l'hystérésis (2 cm) empêche tout clignotement de la décision[cite: 13].
4. **Fil ECHO débranché** : passage en défaut verrouillé après 5 mesures invalides consécutives ; événement `DEFAUT_CAPTEUR` émis une seule fois[cite: 13].
5. **Fil ECHO rebranché** : le défaut reste verrouillé tant qu'il n'y a pas de réarmement[cite: 13].
6. **Appui réarmement avec fil rebranché** : défaut effacé et reprise normale[cite: 13].
7. **Appui réarmement avec fil toujours débranché** : défaut maintenu (réarmement refusé tant que la mesure n'est pas valide)[cite: 13].
8. **Main devant C1 uniquement** : seul C1 détecte la présence, aucune interférence acoustique sur C2 grâce à l'alternance toutes les 60 ms[cite: 13].

### Problèmes rencontrés et résolus :
- **Téléversement bloqué (`esptool`)** : résolu en complétant explicitement `[env:essai_ultrasons]` dans `platformio.ini` avec `upload_speed = 115200`[cite: 31].
- **Valeurs identiques entre C1 et C2 lors des premiers essais** : corrigé en séparant physiquement les deux ponts diviseurs sur la breadboard et en orientant chaque capteur vers son propre mur de référence.

### Résultat :
- Module validé le 22/11/2026.
---

## Fichier 7 : Détecteur d'obstacle infrarouge (detecteur_ir.h / .cpp)
- **Date** : 23 Novembre 2026
- **Matériel testé** : Module détecteur d'obstacle IR alimenté sur le rail 3,3 V, sortie OUT reliée au GPIO 32[cite: 14].
- **Logique vérifiée** : `IR_ACTIF_BAS = true` (OUT passe à `LOW` et la LED du module s'allume lorsqu'un obstacle est détecté)[cite: 14, 15].

### Essais réalisés :
1. **Rien sous la barrière** : affiche `libre` en permanence, compteur de changements immobile à 0.
2. **Voiture posée dessous** : affiche `obstacle` en permanence après réglage de la sensibilité via le potentiomètre (détecte la voiture sans détecter le sol)[cite: 14, 15].
3. **Main passée 10 fois** : compteur de changements exactement à 20 (10 apparitions + 10 disparitions)[cite: 15].
4. **Fil OUT débranché (GPIO 32)** : le système affiche `obstacle` (l'entrée non tirée lit `LOW`).

### Note de sûreté (AMDEC) :
- **Coupure du fil OUT** : comportement sûr (*fail-safe*), car l'absence de signal est interprétée comme un obstacle, ce qui bloque la fermeture de la barrière.
- **Mode de défaillance critique non détecté** : si le capteur tombe en panne avec sa sortie bloquée à l'état haut (`HIGH` / « libre ») ou si sa LED IR grille, rien ne permet à l'ESP32 de détecter la panne ; la barrière risquerait de descendre sur un véhicule[cite: 14]. Piste d'amélioration (V2) : effectuer un autotest en vérifiant que le capteur détecte la barrière elle-même lorsqu'elle est fermée[cite: 14].

### Résultat :
- Module validé le 23/11/2026[cite: 15].
---

## Fichier 8 : Barrière motorisée (barriere.h / .cpp)
- **Date** : 23 Novembre 2026
- **Matériel testé** : Servomoteur SG90 alimenté en 5 V (VIN) et GND, signal PWM sur GPIO 13 (50 Hz, impulsions 500–2400 µs), accompagné de la LED jaune de signalisation (GPIO 26).

### Essais réalisés :
1. **Déplacement de `0°` à `90°`** : mouvement fluide degré par degré sans blocage ni à-coup, suivi de l'émission de `BARRIERE_ARRIVEE`.
2. **Inversion de consigne en cours de mouvement** : envoi de `90` pendant une descente vers `0` ; la barrière repart immédiatement dans l'autre sens.
3. **Arrêt d'urgence (`stop`) en cours de mouvement** : figement instantané de la position (`BARRIERE_STOP`).
4. **Exécution non bloquante** : la LED jaune continue de clignoter régulièrement pendant toute la durée des déplacements.
5. **Endurance (20 allers-retours consécutifs)** : fonctionnement stable sans redémarrage de l'ESP32 ni surchauffe.

### Problèmes rencontrés et résolus :
- **Configuration `platformio.ini`** : ajout explicite des paramètres de carte et de `upload_speed = 115200` dans `[env:essai_barriere]`.
- **Délai entre pas** : réglé à `BARRIERE_DELAI_PAS_MS = 20` ms dans `include/config.h` pour adoucir le déplacement et limiter l'appel de courant au démarrage.

### Résultat :
- Module validé le 23/11/2026.
---

## Fichiers 9 et 10 : Automate du parking et tests unitaires PC
- **Date** : 23 Novembre 2026
- **Environnement de test** : Compilation native sur PC (`[env:native]`, framework Unity) sans dépendance matérielle ESP32.

### Essais unitaires réalisés (`pio test -e native`) :
1. **`test_etat_initial_repos`** : vérification de l'état `REPOS`, barrière à `0°` et compteur initialisé à `capacite_max`.
2. **`test_cycle_entree_complet_decremente_places`** : séquence complète entrée -> ouverture -> passage -> temporisation -> fermeture -> décrémentation des places.
3. **`test_entree_sans_franchissement_ne_decremente_pas`** : vérification qu'un véhicule qui recule avant le second capteur ne fausse pas le compteur.
4. **`test_parking_complet_refuse_entree_mais_accepte_sortie`** : blocage des entrées à `0` place libre tout en autorisant les sorties.
5. **`test_obstacle_ir_pendant_fermeture_reouvre`** : réouverture immédiate en cas de détection infrarouge durant la descente.
6. **`test_arret_urgence_fige_et_rearmement`** : figement immédiat de la barrière et sortie par réarmement.
7. **`test_defaut_capteur_bloque_et_rearmement`** : passage en `DEFAUT`, blocage du réarmement tant que le défaut persiste et retour au `REPOS` après acquittement.

### Problèmes rencontrés et résolus :
- **Cohérence des sorties lors d'une transition** : mise à jour de `angle_cible_barriere` et `figer_barriere` calculée à la fin de `automate_pas()` d'après l'état final.
- **Configuration `platformio.ini`** : ajout de `test_build_src = yes` et isolation de `[env:native]` vis-à-vis de la configuration ESP32.

### Résultat :
- 7 tests sur 7 validés le 23/11/2026 (`0 failed`).
---

## Fichier 9 : Sûreté matérielle — watchdog et cause de redémarrage (surete.h / .cpp)
- **Date** : 24 Novembre 2026
- **Matériel testé** : Carte ESP32 DOIT DevKit V1 seule sur port USB.

### Essais réalisés (`essai_surete`) :
1. **Branchement USB** : événement `DEMARRAGE,MISE_SOUS_TENSION` émis au démarrage.
2. **Blocage volontaire (`bloque`)** : gel de `loop()`, déclenchement du watchdog au bout d'environ 5 s et redémarrage automatique avec la cause `DEMARRAGE,WATCHDOG`.
3. **Bouton matériel `EN`** : redémarrage immédiat avec la cause `MISE_SOUS_TENSION`.

### Problèmes rencontrés et résolus :
- **Vitesse de téléversement dans `platformio.ini`** : correction de `upload_speed = 265200` en `115200` pour éviter la perte de connexion pendant le flashage.

### Résultat :
- Module validé le 24/11/2026.