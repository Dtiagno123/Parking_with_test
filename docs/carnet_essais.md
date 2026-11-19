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