/**
 * \file client.c
 * \brief Contient le code du client dans le modele client-serveur (les voitures).
 */
#include <stdio.h>
#include <stdlib.h>
#include "project.h"
#include "interface.h"
#include "client.h"

/**
 * \fn void constructionRequete(Requete *req, Voiture *v, int carrefour, int croisement_precedent, int croisement_precedent_orientation, int croisement, int croisement_orientation, int voie, int traverse, int type)
 * \brief Construit une requete pour le carrefour, qui sera eventuellement retransmise au serveur si necessaire.
 *
 * La requete peut prendre 4 formes :
 * - MESSARRIVE : message d'information d'arrivee sur un carrefour. N'implique aucun traitement, affiche juste l'information.
 * - MESSDEMANDE : demande d'arrivee en zone avant, pendant, ou apres un croisement. Implique une demande au serveur (depuis le carrefour) qui va analyser le carrefour pour prendre une decision.
 * - MESSINFO : message d'information d'arrivee avant, pendant, ou apres un croisement. N'implique aucun traitement, affiche juste l'information.
 * - MESSSORT : message d'information de sortie d'un carrefour. Implique de mettre a jour la file d'attente "apres" le croisement precedent.
 *
 * \param req Pointeur vers la requete qui va etre renseignee.
 * \param v Pointeur vers la voiture effectuant la requete.
 * \param croisement_precedent L'indice de la zone critique (<=> le croisement) precedemment visitee le cas echeant (utile lorsque la voiture quitte un croisement pour modifier le precedent).
 * \param croisement_precedent_orientation L'orientation (horizontal / vertical) du croisement precedent (utile pour mettre a jour les files d'attente du croisement precedent quand une voiture arrive a un nouveau croisement). Peut valoir :
 * - HO : horizontal.
 * - VE : vertical.
 * \param croisement L'indice de la zone critique (<=> le croisement) le cas echeant.
 * \param croisement_orientation L'orientation (horizontal / vertical) du croisement (utile pour mettre a jour les files d'attente du croisement). Peut valoir :
 * - HO : horizontal.
 * - VE : vertical.
 * \param voie L'indice de la voie d'arrivee, de depart, ou de croisement (<=> la voie qui croise avec celle de la voiture a l'endroit critique).
 * \param traverse L'etat de traversement (directement lie aux informations de traversement). Peut valoir :
 * - AVANT : avant la zone critique (file de voitures avant le croisement).
 * - PENDANT : pendant la zone critique (le croisement en lui meme).
 * - APRES : apres la zone critique (file de voitures apres le croisement).
 * \param type Le type de requete. Peut valoir :
 * - MESSARRIVE.
 * - MESSDEMANDE.
 * - MESSINFO.
 * - MESSSORT.
 */
void constructionRequete(Requete *req, Voiture *v, int carrefour, int croisement_precedent, int croisement_precedent_orientation, int croisement, int croisement_orientation, int voie, int traverse, int type)
{
	req->pidEmetteur = getpid();
	req->v = *v;
	req->carrefour = carrefour;
	req->croisement = croisement;
	req->croisement_orientation = croisement_orientation;
	req->croisement_precedent = croisement_precedent;
	req->croisement_precedent_orientation = croisement_precedent_orientation;
	req->voie = voie;
	req->traverse = traverse;
	req->type = type;
}

/**
 * \fn void affichageRequete(Requete *req)
 * \brief Affiche la nature et les informations de la requete.
 *
 * \param req Pointeur sur la requete a afficher.
 */
void affichageRequete(Requete *req)
{
	if (req->type == MESSARRIVE) {
		message(req->v.numero, "---------------------\n");		
		sprintf(buffer, "Arrive carrefour %d\n\n", req->carrefour);
		message(req->v.numero, buffer);		
		sprintf(buffer, "Arrive voie %d\n\n", req->voie);		
	} else if (req->type == MESSDEMANDE) {
		if (req->traverse == AVANT) sprintf(buffer, "Dem. av. voie %d\n", req->voie);
		else if (req->traverse == PENDANT) sprintf(buffer, "Dem. trav. voie %d\n", req->voie);
		else if (req->traverse == APRES) sprintf(buffer, "Dem. ap. voie %d\n", req->voie);		
	} else if (req->type == MESSINFO) {
		if (req->traverse == AVANT) sprintf(buffer, "Arrive av. voie %d\n", req->voie);
		else if (req->traverse == PENDANT) sprintf(buffer, "Trav. voie %d\n", req->voie);
		else if (req->traverse == APRES) sprintf(buffer, "Arrive ap. voie %d\n", req->voie);		
	} else if (req->type == MESSSORT) {
		message(req->v.numero, "\n");	
		sprintf(buffer, "Sort voie %d\n\n", req->voie);
		message(req->v.numero, buffer);		
		sprintf(buffer, "Sort carrefour %d\n", req->carrefour);
		message(req->v.numero, buffer);
		message(req->v.numero, "---------------------\n");
		sprintf(buffer, "#####################\n");
	}

	message(req->v.numero, buffer);
}
