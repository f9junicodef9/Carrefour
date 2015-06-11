Pour utiliser le programme :

1. Compiler avec le MakeFile dans le dossier src/

2. Ex�cuter le programme selon les syntaxes suivantes :
	- "./project N" : Lance la simulation avec N voitures plac�es al�atoirement sur la carte (N > 0).
	- "./project Voie1 Carrefour1 ... VoieN CarrefourN" : Lance la simulation avec N voitures plac�es aux voies et carrefours respectifs (0 < Voie < 13 et 0 < Carrefour < 5). Remarque : Si Voie et/ou Carrefour valent -1, des valeurs al�atoires sont g�n�r�es.

Il est possible de modifier 3 constantes du programme dans "project.h" pour influer sur 2 facteurs :
	- la taille du carrefour : modifier MAXFILE pour ajuster la capacit� d'accueil de v�hicules du carrefour � un instant t (MAXFILE > 0).
	- la vitesse des v�hicules / d'affichage du programme : modifier MINPAUSE et/ou MAXPAUSE pour ajuster la dur�e minimale et/ou maximale des actions des v�hicules sur le carrefour (dur�es en microseconde).
Remarque : Il faut recompiler le programme par un "make clean" puis "make" pour prendre en compte les changements de constantes.
