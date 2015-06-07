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
Croisement croisements[25] = {{0},{0},{-1},{0},{0},{0},{0},{0},{0},{0},{-1},{0},{-1},{0},{-1},{0},{0},{0},{0},{0},{0},{0},{-1},{0},{0}};

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

		if (req.type == MESSSORT) {
			maj_carrefour(&req);
		}

		if (req.type == MESSDEMANDE) {
			constructionReponse(&req,&rep);
			if (rep.autorisation == 1)
				maj_carrefour(&req);
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
	int j = req->croisement_precedent;
	int etat = req->traverse;
	int type = req->type;
	int orientation = req->croisement_orientation;
	int orientation_precedent = req->croisement_precedent_orientation;
	
	if (type == MESSSORT) {
		if (j != -1) {
			if (orientation_precedent == HO) {
				croisements[j].apresH--;
			} else {
				croisements[j].apresV--;
			}		
		}
	}
	
	if (etat == AVANT) {
		if (j != -1) {
			if (orientation_precedent == HO) {
				croisements[j].apresH--;
			} else {
				croisements[j].apresV--;
			}
		}
		
		if (orientation == HO) {
			croisements[i].avantH++;
		} else {
			croisements[i].avantV++;
		}
	} else if (etat == PENDANT) {
		if (orientation == HO) {
			croisements[i].avantH--;
		} else {
			croisements[i].avantV--;
		}
		croisements[i].etat = 1;
	} else if (etat == APRES) {
		croisements[i].etat = 0;
		if (orientation == HO) {
			croisements[i].apresH++;
		} else {
			croisements[i].apresV++;
		}
	}
	
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
			if (croisements[i*TAILLE+j].etat == -1)
				printf(" \t");
			else
				printf("%d\t", croisements[i*TAILLE+j].etat);
		}
		printf("\n");
	}
	printf("--------------------------------------------\n");
}
