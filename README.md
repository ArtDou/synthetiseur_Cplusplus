# Projet synthetiseur (écouter, produire, transformer et visualiser le son)

## Touches à utiliser :
- s, d, f, g, h, j, k, e, r, y, u , i pour les notes
- "=" ou "+" et "-" ou "\_" pour augmenter et diminuer le son
- w, x, c, v pour choisir la proportion de bruit dans le signal (0, 25, 50 et 100%)
- 1, 2, 3 pour choisir la forme de signal (sinus, carré, dent de scie)
- "\*" et "/" pour rajouter ou enlever une harmonique (i.e., modifier la brillance)

## Compiler et lancer 
Ce projet nécessite openFrameworks https://openframeworks.cc/
```bash
git clone git@github.com:ArtDou/synthetiseur_Cplusplus.git
cd synthetiseur_Cplusplus
make
make run
```

## Fichier de config

Assurez vous d'avoir un fichier nommé _config.make_ à la racine du dossier.  
Ce fichier doit assigner le chemin d'accès au dossier OpenFramework à la variable OF_ROOT.  
Par exemple `OF_ROOT = /home/myname/OF` ou si vous vous apellez Arthur `OF_ROOT = /home/artdou@Digital-Gre.local/Downloads/of_v0.11.2_linux64gcc6_release`
