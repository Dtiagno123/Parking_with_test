# Carnet d'essais — Parking ESP32

## Fichier 3 : Signalisation (signalisation.h / .cpp)
- **Date** : 11 Novembre 2026
- **Matériel testé** : LED Verte (GPIO 27), LED Jaune (GPIO 26), LED Rouge (GPIO 25), Buzzer (GPIO 33)

### Essais réalisés :
1. **Mode Fermé** : LED Rouge allumée fixe, autres éteintes, silence. (OK)
2. **Mode En mouvement** : LED Jaune clignote rapidement, autres éteintes, silence. (OK)
3. **Mode Ouvert** : LED Verte allumée fixe, autres éteintes, silence. (OK)
4. **Mode Défaut** : LEDs Jaune et Rouge clignotent lentement, bip du buzzer toutes les 2 s. (OK)
5. **Mode Arrêt d'urgence** : LED Rouge clignote rapidement, bip court du buzzer chaque seconde. (OK)

### Mesures et observations :
- **Tours de boucle par seconde** : Restent élevés (> 100 000 tours/s) dans tous les modes, confirmant l'absence de `delay()` et de blocage.
- **Problème rencontré** : La LED jaune ne s'allumait pas initialement (problème de faux contact / câblage résolu sur la breadboard).

### Résultat :
- Module validé le 11/11/2026.