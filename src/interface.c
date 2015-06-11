/**
 * \file interface.c
 * \brief Affiche les informations a l'ecran et dans un fichier texte.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

/**
 * \fn void message(int i, char* s)
 * \brief Affiche un message dans la console et l'ecrit dans un fichier texte.
 *
 * Affiche les informations de maniere decalee sous forme de colonne pour une meilleure lisibilite.
 * Ecrit a la fois dans la console et dans un fichier texte (pour etude appronfondie et lorsque les colonnes depassent la largeur de la console).
 *
 * \param i L'indice de la colonne. Permet de decaler le texte horizontalement.
 * \param s Le message a afficher.
 */
void message(int i, char* s)
{
   #define COLONNE 20
   int j, NbBlanc;
   file = fopen("./output.txt", "a");
   NbBlanc=i*COLONNE;
   for (j=0; j<NbBlanc; j++) {
		putchar(' ');
		fprintf(file, " ");
	}
   printf("%s",s);
   fprintf(file, "%s", s);
   fflush(stdout);
   fclose(file);
}
