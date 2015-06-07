/**
 * \file carrefour.c
 * \brief Contient les fonctions utiles au fonctionnement et a la representation des carrefours.
 */
#include <stdio.h>
#include <stdlib.h>
#include "project.h"
#include "carrefour.h"

/*! Represente la taille du carrefour. La valeur indique le nombre maximal de sections critiques par ligne/colonne (le carrefour est suppose carre et toutes les voies sont identiques). */
#define TAILLE 5

/*! Represente le carrefour. Les valeurs 0 representent des sections critiques ; Les valeurs -1 representent des sections non critiques, donc facultatives, mais tout de meme representees par soucis de simplicite. */
int croisements[25] = {0,0,-1,0,0,0,0,0,0,0,-1,0,-1,0,-1,0,0,0,0,0,0,0,-1,0,0};

/**
 * \fn void carrefour()
 * \brief Fonction realisee par chaque carrefour.
 */
void carrefour()
{
	Requete req;
	Reponse rep;

	initRand();

	while (1) {
		msgrcv(msgid,&req,tailleReq,0,0);

		if ((req.type == MESSDEMANDE) || (req.type == MESSTRAVERSE) || (req.type == MESSATRAVERSE))
			maj_carrefour(&req);

		if (req.type == MESSDEMANDE) {
			constructionReponse(&req,&rep);
			msgsnd(msgid,&rep,tailleRep,0);
		}
	}
}

/**
 * \fn void maj_carrefour(Requete *req)
 * \brief Met a jour le carrefour avec les informations de la requete recue.
 *
 * \param req Pointeur sur la requete recue.
 */
void maj_carrefour(Requete *req)
{
	P(MUTEX);
	int i = req->croisement;
	int etat = req->traverse;
	croisements[i] = etat;
//	affiche_carrefour();
	V(MUTEX);
}

/**
 * \fn void affiche_carrefour()
 * \brief Affiche les informations d'un carrefour.
 */
void affiche_carrefour()
{
	int i, j;

	printf("\n--------------------------------------------\n");
	for (i=0;i<TAILLE;i++) {
		for (j=0;j<TAILLE;j++) {
			if (croisements[i*TAILLE+j] == -1)
				printf(" \t");
			else
				printf("%d\t", croisements[i*TAILLE+j]);
		}
		printf("\n");
	}
	printf("--------------------------------------------\n");
}
