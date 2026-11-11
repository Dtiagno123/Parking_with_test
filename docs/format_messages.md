# Format des messages du Parking ESP32

Tous les messages sont transmis sur le port série à **115 200 bauds** avec la virgule `,` comme séparateur.

## 1. Événements (EVT)
Format : `EVT,<temps_ms>,<TYPE>,<DETAIL>`

- **temps_ms** : Temps écoulé depuis le démarrage (`millis()`).
- **TYPE** : Libellé en MAJUSCULES (ex: `DEMARRAGE`, `DEFAUT_CAPTEUR`, `PASSAGE`).
- **DETAIL** : Description complémentaire sans virgule ni accent.

## 2. État du système / Heartbeat (HB)
Format : `HB,<temps_ms>,<ETAT>,<ANGLE>,<DIST_C1>,<DIST_C2>,<IR>,<PASSAGES>`

- **temps_ms** : Temps écoulé en ms.
- **ETAT** : État actuel de l'automate (ex: `FERME`, `OUVERTURE`).
- **ANGLE** : Position actuelle du servomoteur (en degrés).
- **DIST_C1** : Distance mesurée par le capteur 1 (en cm).
- **DIST_C2** : Distance mesurée par le capteur 2 (en cm).
- **IR** : Obstacle détecté sous la barrière (`1` oui, `0` non).
- **PASSAGES** : Compteur de passages validés.