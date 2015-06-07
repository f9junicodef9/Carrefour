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
	rep->type = req->pidEmetteur;

	int i = req->croisement;
	int traverse = req->traverse;
	int orientation = req->croisement_orientation;

	rep->autorisation = 0;
	
	if (traverse == AVANT) {
		if (orientation == HO) {
			if (croisements[i].avantH < MAXFILE) {
				rep->autorisation = 1;
			}
		} else {
			if (croisements[i].avantV < MAXFILE) {
				rep->autorisation = 1;
			}
		}
	} else if (traverse == PENDANT) {
			if (croisements[i].etat == 0) {
				rep->autorisation = 1;
			}
	} else if (traverse == APRES) {
		if (orientation == HO) {
			if (croisements[i].apresH < MAXFILE) {
				rep->autorisation = 1;
			}
		} else {
			if (croisements[i].apresV < MAXFILE) {
				rep->autorisation = 1;
			}	
		}
	}
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
		if (rep->autorisation == 1) snprintf(buffer, sizeof(buffer), "Autorise. avant voie %d\n", req->voie);
		else if (rep->autorisation == 0) snprintf(buffer, sizeof(buffer), "Interdit. avant voie %d\n", req->voie);
	} else if (traverse == PENDANT) {
		if (rep->autorisation == 1) snprintf(buffer, sizeof(buffer), "Autorise. traverse voie %d\n", req->voie);
		else if (rep->autorisation == 0) snprintf(buffer, sizeof(buffer), "Interdit. traverse voie %d\n", req->voie);
	} else if (traverse == APRES) {
		if (rep->autorisation == 1) snprintf(buffer, sizeof(buffer), "Autorise. apres voie %d\n", req->voie);
		else if (rep->autorisation == 0) snprintf(buffer, sizeof(buffer), "Interdit. apres voie %d\n", req->voie);
	}
				
	message(req->v.numero, buffer);
}
