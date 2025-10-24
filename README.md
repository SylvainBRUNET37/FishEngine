# FishEngine

## Étapes à suivre pour Jolt

1 - Suivre les instructions pour générer la solution Jolt (lancer le bon .bat dans "Jolt\Build"): [How to build Jolt](https://github.com/jrouwe/JoltPhysics/blob/master/Build/README.md)

2 - Compiler la solution Jolt générée (Debug et/ou Release)

3 - Lancer la solution PhysicsEngine et appuyer sur F5 (Debug et/ou Release)

## Étapes à suivre pour Assimp

1 - Dans la racine du projet, utiliser la commande: git clone https://github.com/assimp/assimp.git

2 - Créer un dossier build dans le dossier 'assimp' qui a été créé

3 - Dans le dossier 'assimp'
```powershell
cmake -S . -B . -G "Visual Studio 17 2022" -A x64
```
Build en release:
```powershell
cmake --build . --config Release
```
Build en debug  :
```powershell
cmake --build . --config Debug
```
