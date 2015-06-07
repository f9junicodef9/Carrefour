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
 * \fn void constructionRequete(Requete *req, Voiture *v, int croisement_precedent, int croisement_precedent_orientation, int croisement, int croisement_orientation, int voie, int traverse, int type)
 * \brief Construit une requete pour le serveur.
 * La requete peut prendre 5 formes :
 * - Information d'arrivee : une voiture arrive par la voie mentionnee
 * - Demande de traverser : une voiture demande a traverser une zone critique (<=> un croisement avec une autre voie)
 * - Information de debut de traversement : une voiture est en train de traverser une zone critique
 * - Information de fin de traversement : une voiture a fini de traverser une zone critique
 * - Information de depart : une voiture sort par la voie mentionnee
 *
 * \param req Pointeur vers la requete qui va etre renseignee.
 * \param v Pointeur vers la voiture effectuant la requete.
 * \param croisement_precedent L'indice de la zone critique (<=> le croisement) precedemment visitee le cas echeant (utile lorsque la voiture quitte un croisement pour modifier ce dernier).
 * \param croisement_precedent_orientation L'orientation (horizontal / vertical) du croisement precedent (utile pour mettre a jour les files d'attente du croisement precedent quand une voiture arrive a un nouveau croisement).
 * \param croisement L'indice de la zone critique (<=> le croisement) le cas echeant.
 * \param croisement_orientation L'orientation (horizontal / vertical) du croisement (utile pour mettre a jour les files d'attente du croisement).
 * \param voie L'indice de la voie d'arrivee, de depart, ou de croisement (<=> la voie qui croise avec celle de la voiture a l'endroit critique).
 * \param traverse L'etat de traversement (directement lie aux informations de traversement). Peut valoir :
 * - AVANT
 * - PENDANT
 * - APRES
 * \param type Le type de requete. Peut valoir :
 * - MESSARRIVE
 * - MESSINFO
 * - MESSSORT
 */
void constructionRequete(Requete *req, Voiture *v, int croisement_precedent, int croisement_precedent_orientation, int croisement, int croisement_orientation, int voie, int traverse, int type)
{
	req->pidEmetteur = getpid();
	req->v = *v;
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
 * \param req La requete a afficher
 */
void affichageRequete(Requete *req)
{
	if (req->type == MESSARRIVE) {
		snprintf(buffer, sizeof(buffer), "Arrive voie %d\n", req->voie);
	} else if (req->type == MESSDEMANDE) {
		if (req->traverse == AVANT) snprintf(buffer, sizeof(buffer), "Dem. avant voie %d\n", req->voie);
		else if (req->traverse == PENDANT) snprintf(buffer, sizeof(buffer), "Dem. traverse voie %d\n", req->voie);
		else if (req->traverse == APRES) snprintf(buffer, sizeof(buffer), "Dem. apres voie %d\n", req->voie);		
	} else if (req->type == MESSINFO) {
		if (req->traverse == AVANT) snprintf(buffer, sizeof(buffer), "Arrive. avant voie %d\n", req->voie);
		else if (req->traverse == PENDANT) snprintf(buffer, sizeof(buffer), "Traverse voie %d\n", req->voie);
		else if (req->traverse == APRES) snprintf(buffer, sizeof(buffer), "Arrive. apres voie %d\n", req->voie);		
	} else if (req->type == MESSSORT) {
		snprintf(buffer, sizeof(buffer), "Sort voie %d\n", req->voie);
	}

	message(req->v.numero, buffer);
}
