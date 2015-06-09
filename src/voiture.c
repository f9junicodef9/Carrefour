/**
 * \file voiture.c
 * \brief Contient les fonctions utiles au fonctionnement et a la representation des voitures.
 */
#include <stdio.h>
#include <stdlib.h>
#include "project.h"
#include "voiture.h"

/*! Represente les 12 voies du carrefour. */
Voie voies[12] = {	{1,{15,16,17,13,8,3},{11,7,10,4,7,8},{HO,HO,VE,VE,VE,VE}},
					{2,{20,21,23,24,-1},{11,7,4,5,-1},{HO,HO,HO,HO}},
					{3,{-1},{-1}},
					{4,{23,18,13,7,6,5},{2,10,1,7,10,11},{VE,VE,HO,HO,HO,HO}},
					{5,{24,19,9,4,-1},{2,10,7,8,-1},{VE,VE,VE,VE}},
					{6,{-1},{-1}},
					{7,{9,8,7,11,16,21},{5,1,4,10,1,2},{HO,HO,VE,VE,VE,VE}},
					{8,{4,3,1,0,-1},{5,1,10,11,-1},{HO,HO,HO,HO}},
					{9,{-1},{-1}},
					{10,{1,6,11,17,18,19},{8,4,7,1,4,5},{VE,VE,HO,HO,HO,HO}},
					{11,{0,5,15,20,-1},{8,4,1,2,-1},{VE,VE,VE,VE}},
					{12,{-1},{-1}}	};

int assoc_voies[] = {4,2,12,7,5,3,10,8,6,1,11,9};
int assoc_carrefours[][12] = {	{-1,2,3,-1,-1,2,3,-1,-1,2,3,-1},
										{-1,-1,4,1,-1,-1,4,1,-1,-1,4,1},
										{1,4,-1,-1,1,4,-1,-1,1,4,-1,-1},
										{2,-1,-1,3,2,-1,-1,3,2,-1,-1,3}	};

/**
 * \fn void initRand()
 * \brief Permet de generer de nouveaux nombres aleatoires.
 * Necessaire pour que les differents processus (les voitures) ne generent pas les memes nombres aleatoires (les memes voies).
 */
void initRand()
{
	srand(getpid());
}

/**
 * \fn void affiche_voiture(Voiture *v)
 * \brief Affiche les informations d'une voiture.
 *
 * \param v Pointeur sur la voiture a afficher.
 */
void affiche_voiture(Voiture *v)
{
	printf("Numero : %d\n", v->numero);
	printf("Voie : %d\n", v->voie->numero);

	int i=0;
	printf("Croisements : {");
	while (v->voie->sem_num[i] != -1) {
		printf("%d,", v->voie->sem_num[i]);
		i++;
	}
	printf("}\n");
}

void affiche_carrefours()
{
	int i,j;
	
	for (i=0;i<4;i++) {
		printf("---------------------------\n");
		printf("Carrefour %d :\n", i+1);
		
		for (j=0;j<12;j++) {
			printf("Voie %d sort sur carrefour %d voie %d\n", j+1, assoc_carrefours[i][j], assoc_voies[j]);
		}
		
		printf("---------------------------\n");
	}
}

/**
 * \fn void voiture(int numero, int voie)
 * \brief Fonction realisee par chaque processus (chaque voiture).
 * Comprend 2 phases :
 * - Initialisation de la voiture, de deux facons :
 *  - Avec une voie aleatoire
 *  - Avec une voie choisie par l'utilisateur
 * - Parcours de la voie par la voiture.
 *
 * \param numero Le numero de la voiture.
 * \param voie Le numero de la voie. Peut valoir :
 * - -1 : la voiture prendra une voie dont le numero est genere aleatoirement (1<=voie<=12)
 * - >0 : la voiture prendra la voie correspondant a ce numero
 */
void voiture(int numero, int voie)
{
	Voiture v;
	Requete req;
	Reponse rep;
	int croisement_numero, croisement_voie, croisement_orientation, croisement_precedent_numero, croisement_precedent_orientation, i;

	v.numero = numero;

	initRand();

	if (voie == -1) {
		int voie_random = rand()%12+1;
		v.voie = &voies[voie_random-1];
	} else
		v.voie = &voies[voie-1];

	create_question(&req, &v, -1, -1, -1, -1, v.voie->numero, -1, MESSARRIVE);		
	
	for (i=0 ; i < 6 ; i++) {
		croisement_numero = v.voie->sem_num[i];

		if (croisement_numero == -1) break;
		
		croisement_voie = v.voie->croisements[i];
		croisement_orientation = v.voie->orientation[i];
		if (i == 0) {
			croisement_precedent_numero = -1;
			croisement_precedent_orientation = -1;
		} else {
			croisement_precedent_numero = v.voie->sem_num[i-1];
			croisement_precedent_orientation = v.voie->orientation[i-1];
		}

		create_question(&req, &v, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, AVANT, MESSDEMANDE);
		receive_answer(&req,&rep);
		
		create_question(&req, &v, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, AVANT, MESSINFO);

		sleep(rand()%MAXPAUSE);
		
		create_question(&req, &v, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, PENDANT, MESSDEMANDE);		
		receive_answer(&req,&rep);		

		create_question(&req, &v, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, PENDANT, MESSINFO);
		
		sleep(rand()%MAXPAUSE);

		create_question(&req, &v, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, APRES, MESSDEMANDE);		
		receive_answer(&req,&rep);

		create_question(&req, &v, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, APRES, MESSINFO);

		sleep(rand()%MAXPAUSE);
	}
	
	croisement_precedent_numero = v.voie->sem_num[i-1];
	croisement_precedent_orientation = v.voie->orientation[i-1];

	if (i == 0) {
		create_question(&req, &v, -1, -1, -1, -1, v.voie->numero, -1, MESSSORT);		
	} else {
		create_question(&req, &v, croisement_precedent_numero, croisement_precedent_orientation, -1, -1, v.voie->numero, -1, MESSSORT);			
	}
}

/**
 * \fn void maj_position(Voiture *v, int position, int traverse)
 * \brief Met a jour la position de la voiture dans sa structure.
 *
 *					!!!! INUTILISEE !!!! => a voir si reellement utile, sinon supprimer et mettre a jour structure Voiture
 *
 * \param v Pointeur sur la voiture a mettre a jour.
 * \param position L'indice du croisement ou se trouve la voiture.
 * \param traverse L'etat de traversement (directement lie aux informations de traversement). Peut valoir :
 * - AVANT
 * - PENDANT
 * - APRES
 */
void maj_position(Voiture *v, int position, int traverse)
{
	v->position = position;
	v->position_traversee = traverse;
}


/**
 * \fn void create_question(Requete *req, Voiture *v, int croisement_precedent, int croisement_precedent_orientation, int croisement, int croisement_orientation, int voie, int traverse, int type)
 * \brief Effectue les operations permettant la construction d'une requete vers le serveur, affiche la requete, et l'envoie au serveur.
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
void create_question(Requete *req, Voiture *v, int croisement_precedent, int croisement_precedent_orientation, int croisement, int croisement_orientation, int voie, int traverse, int type)
{
	P(MUTEX);
	constructionRequete(req, v, croisement_precedent, croisement_precedent_orientation, croisement, croisement_orientation, voie, traverse, type);
	affichageRequete(req);
	msgsnd(msgid,req,tailleReq,0);
	V(MUTEX);
}

/**
 * \fn void receive_answer(Requete *req, Reponse *rep)
 * \brief Effectue les operations permettant la reception d'une reponse depuis le serveur, et affiche la reponse.
 *
 * Ne retourne que lorsque la reponse du serveur aura ete possitive.
 * Permet de bloquer la voiture tant qu'elle n'a pas eu la permission d'avancer.
 * Tant qu'elle est bloquee, la voiture renvoie des demandes au serveur a intervalles reguliers.
 *
 * \param req Pointeur vers la requete qui a ete envoyee.
 * \param rep Pointeur vers la reponse a afficher.
 */
void receive_answer(Requete *req, Reponse *rep)
{
	msgrcv(msgid,rep,tailleRep,getpid(),0);
	P(MUTEX);
	affichageReponse(req,rep);
	V(MUTEX);
	if (rep->autorisation == 0) {
		while (rep->autorisation == 0) {
			msgsnd(msgid,req,tailleReq,0);
			msgrcv(msgid,rep,tailleRep,getpid(),0);
			sleep(1);
		}
		P(MUTEX);
		affichageRequete(req);
		affichageReponse(req,rep);
		V(MUTEX);
	}
}
