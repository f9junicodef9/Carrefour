/**
 * \file carrefour.c
 * \brief Contient les fonctions utiles au fonctionnement et a la representation des carrefours.
 *
 * Ici, les carrefours sont a la fois clients et serveurs, selon le modele client-serveur.
 * Pour les voitures, ils font office de serveur : ils recoivent des requetes en renvoient des reponses.
 * Pour le serveur, ils font office de clients : ils envoient des requetes et recoivent des reponses.
 * Ce sont des intermediaires entre les voitures et le serveur.
 */
#include <stdio.h>
#include <stdlib.h>
#include "project.h"
#include "carrefour.h"

/*! Represente la taille du carrefour. La valeur indique le nombre maximal de sections critiques par ligne/colonne (le carrefour est suppose carre et toutes les voies sont identiques). */
#define TAILLE 5

/**
 * \fn void carrefour(int numero, pid_t pid_Serveur)
 * \brief Fonction realisee par chaque carrefour.
 *
 * Comprend 2 phases :
 * - Se connecte a la file de message correspondant au carrefour (1 seule parmis les 4).
 * - Receptionne sans arret les requetes des voitures dans sa file de message (1 par carrefour), les transmet au serveur si necessaire et retourne les reponses de ce dernier dans sa file le cas echeant.
 *
 * \param numero Le numero du carrefour (1<=numero<=4).
 * \param pid_Serveur Le pid du processus serveur. Utile pour adresser les requetes au serveur, dans la file de message du serveur.
 */
void carrefour(int numero, pid_t pid_Serveur)
{
	Requete req, req_serveur;
	Reponse rep;

	Carrefour *c;
	
	c = (Carrefour *) shmat(carrefours[numero-1], NULL, 0);
	
	while (1) {
		msgrcv(msg_carrefour[numero-1],&req,tailleReq,0,0);
		if (req.type == MESSSORT) {
			maj_carrefour(&req, c);
		}

		if (req.type == MESSDEMANDE) {
			req_serveur = req;
			req_serveur.type = pid_Serveur;
			req_serveur.pidEmetteur = getpid();
			msgsnd(msg_serveur,&req_serveur,tailleReq,0);
			msgrcv(msg_serveur,&rep,tailleRep,getpid(),0);
			if (rep.autorisation == 1) {
				maj_carrefour(&req, c);
			}
			rep.type = req.pidEmetteur;
			usleep(MINPAUSE);
			msgsnd(msg_carrefour[numero-1],&rep,tailleRep,0);
		}
	}
}

/**
 * \fn void maj_carrefour(Requete *req, Carrefour *c)
 * \brief Met a jour le carrefour avec les informations de la requete recue.
 *
 * \param req Pointeur sur la requete recue.
 * \param c Pointeur sur le carrefour correspondant.
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
 * \fn void affiche_carrefour(Carrefour *c)
 * \brief Affiche les informations d'un carrefour.
 *
 * \param c Pointeur sur le carrefour correspondant.
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
