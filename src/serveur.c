/**
 * \file serveur.c
 * \brief Contient le code du serveur dans le modele client-serveur (le serveur).
 */
#include <stdio.h>
#include <stdlib.h>
#include "project.h"
#include "serveur.h"

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

	if (croisements[req->croisement] == PASTRAVERSE || croisements[req->croisement] == ATRAVERSE)
		rep->autorisation = 1;
	else
		rep->autorisation = 0;
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

}
