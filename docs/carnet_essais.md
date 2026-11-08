# Carnet d'essais — Parking ESP32

## Fichier 1 — platformio.ini et include/config.h

**Date :** 08/11/2026
**Environnement :** essai_demarrage
**Matériel :** carte ESP32 DevKit V1 (puce ESP32-D0WD-V3 rev 3.1) seule, branchée en USB, aucun composant câblé
**Poste :** Ubuntu, VS Code + PlatformIO, port /dev/ttyUSB0

### Ce qui a été testé

| # | Test | Résultat attendu | Résultat obtenu | OK ? |
|---|------|------------------|-----------------|------|
| 1 | Compilation `pio run -e essai_demarrage` | 0 erreur, 0 avertissement | 0 erreur, 0 avertissement | ✅ |
| 2 | Téléversement sans sudo | upload réussi | échec au 1er essai, réussi après correction (voir problèmes) | ✅ |
| 3 | Moniteur série à 115200 bauds | 1 ligne/s, temps +1000 ms, PIN_LED_VERTE=27 | conforme (ex. temps=351000 → 367000 ms) | ✅ |
| 4 | Bouton EN | redémarrage, temps repart de 0 | … | … |
| 5 | config.h compilé sur PC avec g++ (sans Arduino) | affiche OK | … | … |

### Problèmes rencontrés et solutions

1. **Mauvaise carte dans le projet généré**
   - Constat : platformio.ini créé avec `board = esp32dev` au lieu de `esp32doit-devkit-v1`.
   - Cause : projet créé depuis l'interface PIO Home.
   - Solution : contenu remplacé par la configuration du projet (carte esp32doit-devkit-v1).

2. **Échec du téléversement**
   - Constat : `Failed to start stub flasher. There was no response.`
     + avertissement « Detected crystal freq 15.55 MHz ».
   - Cause probable : liaison série USB non fiable à la vitesse par défaut (921 600 bauds).
   - Solution : ajout de `upload_speed = 115200` dans la section [env].
   - Résultat : téléversement réussi.

### Bilan
Fichier 1 validé. La chaîne de compilation, de téléversement et de communication série fonctionne.
À retenir : téléverser à 115200 bauds avec cette carte et ce câble.