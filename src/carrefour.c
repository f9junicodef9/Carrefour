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

/**
 * \fn void carrefour()
 * \brief Fonction realisee par chaque carrefour.
 */
void carrefour(int numero, pid_t pid_Serveur)
{
	Requete req, req_serveur;
	Reponse rep;
	
//	long pidVoiture;

	Carrefour *c;
	
	c = (Carrefour *) shmat(shm_id[numero-1], NULL, 0);

	while (1) {
		msgrcv(msgid_carrefour[numero-1],&req,tailleReq,0,0);
		if (req.type == MESSSORT) {
			maj_carrefour(&req, c);
		}

		if (req.type == MESSDEMANDE) {
			req_serveur = req;
			req_serveur.type = pid_Serveur;
			req_serveur.pidEmetteur = getpid();
			msgsnd(msgid_serveur,&req_serveur,tailleReq,0);
			msgrcv(msgid_serveur,&rep,tailleRep,getpid(),0);
			if (rep.autorisation == 1) {
				maj_carrefour(&req, c);
			}
			rep.type = req.pidEmetteur;
			usleep(MINPAUSE);
			msgsnd(msgid_carrefour[numero-1],&rep,tailleRep,0);
		}
	}
}

/**
 * \fn void maj_carrefour(Requete *req)
 * \brief Met a jour le carrefour avec les informations de la requete recue.
 *
 * \param req Pointeur sur la requete recue.
 */
void maj_carrefour(Requete *req, Carrefour *c)
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
				c->croisements[j].apresH--;
			} else {
				c->croisements[j].apresV--;
			}		
		}
	}
	
	if (etat == AVANT) {
		if (j != -1) {
			if (orientation_precedent == HO) {
				c->croisements[j].apresH--;
			} else {
				c->croisements[j].apresV--;
			}
		}
		
		if (orientation == HO) {
			c->croisements[i].avantH++;
		} else {
			c->croisements[i].avantV++;
		}
	} else if (etat == PENDANT) {
		if (orientation == HO) {
			c->croisements[i].avantH--;
		} else {
			c->croisements[i].avantV--;
		}
		c->croisements[i].etat = 1;
	} else if (etat == APRES) {
		c->croisements[i].etat = 0;
		if (orientation == HO) {
			c->croisements[i].apresH++;
		} else {
			c->croisements[i].apresV++;
		}
	}
	
//	affiche_carrefour();
	V(MUTEX);
}

/**
 * \fn void affiche_carrefour()
 * \brief Affiche les informations d'un carrefour.
 */
void affiche_carrefour(Carrefour *c)
{
	int i, j;

	printf("\n--------------------------------------------\n");
	for (i=0;i<TAILLE;i++) {
		for (j=0;j<TAILLE;j++) {
			if (c->croisements[i*TAILLE+j].etat == -1)
				printf(" \t");
			else
				printf("%d\t", c->croisements[i*TAILLE+j].etat);
		}
		printf("\n");
	}
	printf("--------------------------------------------\n");
}
