# Carnet d'essais — Parking ESP32

## Fichier 3 : Signalisation (signalisation.h / .cpp)
- **Date** : 11 Novembre 2026
- **Matériel testé** : LED Verte (GPIO 27), LED Jaune (GPIO 26), LED Rouge (GPIO 25), Buzzer (GPIO 33)[cite: 8]

### Essais réalisés :
1. **Mode Fermé** : LED Rouge allumée fixe, autres éteintes, silence. (OK)[cite: 8]
2. **Mode En mouvement** : LED Jaune clignote rapidement, autres éteintes, silence. (OK)[cite: 8]
3. **Mode Ouvert** : LED Verte allumée fixe, autres éteintes, silence. (OK)[cite: 8]
4. **Mode Défaut** : LEDs Jaune et Rouge clignotent lentement, bip du buzzer toutes les 2 s. (OK)[cite: 9]
5. **Mode Arrêt d'urgence** : LED Rouge clignote rapidement, bip court du buzzer chaque seconde. (OK)[cite: 9]

### Mesures et observations :
- **Tours de boucle par seconde** : Restent élevés dans tous les modes, confirmant l'absence de `delay()` et de blocage[cite: 9].
- **Problème rencontré** : La LED jaune ne s'allumait pas initialement (problème de faux contact / câblage résolu sur la breadboard).

### Résultat :
- Module validé le 11/11/2026.

---

## Fichier 4 : Boutons (boutons.h / .cpp)
- **Date** : 19 Novembre 2026
- **Matériel testé** : Bouton Arrêt d'urgence (GPIO 16), Bouton Réarmement (GPIO 17), câblés en `INPUT_PULLUP` vers GND[cite: 9].

### Essais réalisés :
- **Comparaison sans anti-rebond (`DELAI_ANTIREBOND_MS = 0`)** : Sur 20 appuis francs, les rebonds mécaniques provoquent plusieurs détections par appui (compteur supérieur à 20, entre 25 et 35 selon la pression)[cite: 9, 10].
- **Avec anti-rebond (`DELAI_ANTIREBOND_MS = 20`)**[cite: 9, 10] :
  1. **20 appuis francs** : compteur exactement à 20 sur chaque bouton. (OK)
  2. **20 appuis très rapides** : compteur à 20 (aucun double comptage). (OK)
  3. **Bouton maintenu 5 s** : 1 seul appui comptabilisé ; LED rouge allumée tant que l'AU est maintenu. (OK)[cite: 10]
  4. **Appui simultané sur les 2 boutons** : les deux compteurs s'incrémentent chacun de 1. (OK)[cite: 10]

### Note de sûreté (AMDEC) :
- Le câblage actuel utilise des boutons normalement ouverts (NO) au repos (passage à `LOW` lors de l'appui)[cite: 9, 10].
- **Mode de défaillance identifié** : En cas de coupure de fil ou de faux contact sur le bouton d'arrêt d'urgence, le système ne détecte pas la panne et l'arrêt d'urgence devient inopérant[cite: 10]. Sur un système industriel, un arrêt d'urgence doit impérativement être câblé avec un contact normalement fermé (NF) au repos pour garantir une sécurité positive (fail-safe)[cite: 10].

### Résultat :
- Module validé le 19/11/2026[cite: 10].