/**
 * \file serveur.c
 * \brief Contient le code du serveur dans le modele client-serveur (le serveur).
 */
#include <stdio.h>
#include <stdlib.h>
#include "project.h"
#include "serveur.h"
#include "interface.h"

#define MAXFILE 1

void serveur()
{
	Requete req;
	Reponse rep;

	while (1) {
		msgrcv(msgid_serveur,&req,tailleReq,getpid(),0);
		constructionReponse(&req, &rep);
		msgsnd(msgid_serveur,&rep,tailleRep,0);
	}
}

/**
 * \fn void constructionReponse(Requete *req,Reponse *rep)
 * \brief Construit une reponse pour le client.
 * La reponse peut prendre 2 formes :
 * - Autorisation : autorise la voiture a traverser le croisement demande
 * - Interdiction : interdit la voiture de traverser le croisement demande
 *
 * \param req Pointeur vers la requete qui a ete recue.
 * \param rep Pointeur vers la reponse qui va etre renseignee.
 */
void constructionReponse(Requete *req,Reponse *rep)
{
	P(MUTEX);
	rep->type = req->pidEmetteur;

	int i = req->croisement;
	int traverse = req->traverse;
	int orientation = req->croisement_orientation;
	int numero = req->carrefour;
	
	rep->autorisation = 0;
	
	Carrefour *c;
	
	c = (Carrefour *) shmat(shm_id[numero-1], NULL, 0);
	
	
	if (traverse == AVANT) {
		if (orientation == HO) {
			if (c->croisements[i].avantH < MAXFILE) {
				rep->autorisation = 1;
			}
		} else {
			if (c->croisements[i].avantV < MAXFILE) {
				rep->autorisation = 1;
			}
		}
	} else if (traverse == PENDANT) {
			if (c->croisements[i].etat == 0) {
				rep->autorisation = 1;
			}
	} else if (traverse == APRES) {
		if (orientation == HO) {
			if (c->croisements[i].apresH < MAXFILE) {
				rep->autorisation = 1;
			}
		} else {
			if (c->croisements[i].apresV < MAXFILE) {
				rep->autorisation = 1;
			}	
		}
	}
	
	shmdt(c);
	
	V(MUTEX);
}

/**
 * \fn void affichageReponse(Requete *req,Reponse *rep)
 * \brief Affiche la nature et les informations de la reponse a la requete.
 *
 * \param req Pointeur vers la requete qui a ete recue.
 * \param rep Pointeur vers la reponse a afficher.
 */
void affichageReponse(Requete *req,Reponse *rep)
{
	int traverse = req->traverse;
	
	if (traverse == AVANT) {
		if (rep->autorisation == 1) sprintf(buffer, "   Aut. av. voie %d\n", req->voie);
		else if (rep->autorisation == 0) sprintf(buffer, "   Int. av. voie %d\n", req->voie);
	} else if (traverse == PENDANT) {
		if (rep->autorisation == 1) sprintf(buffer, "   Aut. trav. voie %d\n", req->voie);
		else if (rep->autorisation == 0) sprintf(buffer, "   Int. trav. voie %d\n", req->voie);
	} else if (traverse == APRES) {
		if (rep->autorisation == 1) sprintf(buffer, "   Aut. ap. voie %d\n", req->voie);
		else if (rep->autorisation == 0) sprintf(buffer, "   Int. ap. voie %d\n", req->voie);
	}
				
	message(req->v.numero, buffer);
}
