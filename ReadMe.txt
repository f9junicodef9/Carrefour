Pour utiliser le programme :

1. Compiler avec le MakeFile dans le dossier src/

2. Exécuter le programme selon les syntaxes suivantes :
	- "./project N" : Lance la simulation avec N voitures placées aléatoirement sur la carte (N > 0).
	- "./project Voie1 Carrefour1 ... VoieN CarrefourN" : Lance la simulation avec N voitures placées aux voies et carrefours respectifs (0 < Voie < 13 et 0 < Carrefour < 5). Remarque : Si Voie et/ou Carrefour valent -1, des valeurs aléatoires sont générées.

Il est possible de modifier 3 constantes du programme dans "project.h" pour influer sur 2 facteurs :
	- la taille du carrefour : modifier MAXFILE pour ajuster la capacité d'accueil de véhicules du carrefour à un instant t (MAXFILE > 0).
	- la vitesse des véhicules / d'affichage du programme : modifier MINPAUSE et/ou MAXPAUSE pour ajuster la durée minimale et/ou maximale des actions des véhicules sur le carrefour (durées en microseconde).
Remarque : Il faut recompiler le programme par un "make clean" puis "make" pour prendre en compte les changements de constantes.
