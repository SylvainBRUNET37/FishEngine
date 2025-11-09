# Devoir 3 Jolt -> FishEngine

## Crédits

- Sylvain Brunet (brus4186)
- Thierry Demers-Landry (demt6496)
- Alexandre Gagnon (gaga1716)
- Nell Truong (trun8382)

## Étapes à suivre pour les profs

1. Dans le répertoire "FishEngine", exécuter :
   ```bash
   ./build.bat
   ```
2. Aller faire un café (~5min de build pour Assimp + Jolt)
3. Lancer la solution "FishEngine.sln"
4. Choisir "Need4Fish" comme projet de démarrage
5. Lancer le projet (Debug ou Release)

## Contrôles

- ESC pour mettre le jeu en pause (et récupérer la souris) ou sorrtir de pause
- R pour redémarrer la partie

- Mouvements de la souris pour pivoter dans la direction désirée
- W et S pour avancer et reculer
- A et D pour "strafe" à gauche et à droite

- Roulette de la souris pour ajuster le zoom de la caméra
- ALT + Entrée pour activer/désactiver le mode pleine écran

## Gameplay loop

- L'objectif est de manger le grand mosasaure à l'extérieur du monticule
- Il faut manger d'autres poissons plus petits avant, sinon c'est le grand mosasaure qui VOUS mange
- Pour manger : simplement entrer en contact avec les autres êtres vivants.
- Si on a plus de masse, l'autre disparaît et on gagne en masse.
- S'il y a égalité, l'autre se fait "pousser".
- Si l'autre a plus de masse, l'écran de fin de jeu apparaît et il faut recommencer ('R').

- Dans le moment, il n'est pas possible de savoir si on a plus de masse que l'autre avant d'entrer en contact. 
- De plus, le joueur ne grandit pas encore lorqu'il mange des proies.

## Étapes à suivre pour cloner (devs)

1.  ```bash
    git clone -b develop --recurse-submodules https://github.com/SylvainBRUNET37/FishEngine.git
    ```
2.  Exécuter la commande :
    ```bash
    ./build.bat
    ```
