/**
 * \file voiture.c
 * \brief Contient les fonctions utiles au fonctionnement et a la representation des voitures.
 *
 * Ici, les voitures sont des clients pour les carrefours, selon le modele client-serveur. Elles envoient des requetes aux carrefours et en recoivent des reponses.
 */
#include <stdio.h>
#include <stdlib.h>
#include "project.h"
#include "voiture.h"
#include "interface.h"
#include <sys/time.h>

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

/*! Represente les voies "d'entree" associees aux voies "de sortie" respectives. */
int assoc_voies[] = {4,2,12,7,5,3,10,8,6,1,11,9};

/*! Represente les carrefours associes aux voies "de sorties" respectives. */
int assoc_carrefours[][12] = {	{-1,2,3,-1,-1,2,3,-1,-1,2,3,-1},
										{-1,-1,4,1,-1,-1,4,1,-1,-1,4,1},
										{1,4,-1,-1,1,4,-1,-1,1,4,-1,-1},
										{2,-1,-1,3,2,-1,-1,3,2,-1,-1,3}	};

/**
 * \fn void initRand()
 * \brief Permet de generer de nouveaux nombres aleatoires.
 *
 * Necessaire pour que les differents processus (les voitures) ne generent pas les memes nombres aleatoires (les memes voies, carrefours, temps de pause, etc...).
 * Genere un seed base sur 3 variables pour assurer l'aspect aleatoire.
 */
void initRand()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	srand(t.tv_usec * t.tv_sec * getpid());
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

/**
 * \fn void affiche_carrefours()
 * \brief Affiche les informations generales des carrefours.
 *
 * Indique pour chaque voie de chaque carrefour, la voie et le carrefour sur lequel elle debouche.
 */
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
 * \fn void voiture(int numero, int voie, int carrefour)
 * \brief Fonction realisee par chaque voiture.
 *
 * Comprend 2 phases :
 * - Initialisation de la voiture, deux cas de figure :
 *  - Lancement du programme :
 *   - Le carrefour ou elle arrive sera aleatoire parmis les 4 (amelioration possible : permettre a l'utilisateur de choisir le carrefour).
 *   - La voie, deux cas de figure :
 *    - Avec une voie aleatoire.
 *    - Avec une voie choisie par l'utilisateur.
 *  - Programme deja lance, la voiture change de carrefour, elle rappelle cette fonction. Alors :
 *   - Le carrefour ou elle arrive sera celui qui "correspond dans la realite".
 *   - La voie ou elle arrive sera aleatoire parmis les 3 "possibles dans la realite" selon la direction d'arrivee sur le carrefour (Nord, Ouest, Est, Sud) (<=> simule un changement de voie par le conducteur).
 * - Parcours de la voie par la voiture.
 *
 * \param numero Le numero de la voiture.
 * \param voie Le numero de la voie. Peut valoir :
 * - -1 : la voiture prendra une voie dont le numero est genere aleatoirement :
 *  - 1<=voie<=12 si c'est le lancement du programme (la voiture arrive n'importe ou).
 *  - 1<=voie<=3 ; 4<=voie<=6 ; 7<=voie<=9 ; 10<=voie<=12 si la voiture venait respectivement des carrefours Ouest ; Sud ; Est ; Nord.
 * - >0 : la voiture prendra la voie correspondant a ce numero.
 */
void voiture(int numero, int voie, int carrefour)
{
	Voiture v;
	Requete req;
	Reponse rep;
	int croisement_numero, croisement_voie, croisement_orientation, croisement_precedent_numero, croisement_precedent_orientation, i;

	v.numero = numero;

	initRand();

	if (carrefour == -1) {
		int carrefour_random = rand()%4+1;
		v.carrefour = carrefour_random;

		if (voie == -1) {
			int voie_random = rand()%12+1;
			v.voie = &voies[voie_random-1];
		} else
			v.voie = &voies[voie-1];
	} else {
		v.carrefour = carrefour;
		
		int voie_random = random_voie(voie);
		v.voie = &voies[voie_random-1];
	}

	create_question(&req, &v, v.carrefour, -1, -1, -1, -1, v.voie->numero, -1, MESSARRIVE);		
	
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

		create_question(&req, &v, v.carrefour, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, AVANT, MESSDEMANDE);
		receive_answer(&req,v.carrefour);
		
		create_question(&req, &v, v.carrefour, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, AVANT, MESSINFO);

		usleep(rand()%MAXPAUSE+MINPAUSE);
		
		create_question(&req, &v, v.carrefour, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, PENDANT, MESSDEMANDE);		
		receive_answer(&req,v.carrefour);		

		create_question(&req, &v, v.carrefour, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, PENDANT, MESSINFO);
		
		usleep(rand()%MAXPAUSE+MINPAUSE);

		create_question(&req, &v, v.carrefour, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, APRES, MESSDEMANDE);		
		receive_answer(&req,v.carrefour);

		create_question(&req, &v, v.carrefour, croisement_precedent_numero, croisement_precedent_orientation, croisement_numero, croisement_orientation, croisement_voie, APRES, MESSINFO);

		usleep(rand()%MAXPAUSE+MINPAUSE);
	}
	
	croisement_precedent_numero = v.voie->sem_num[i-1];
	croisement_precedent_orientation = v.voie->orientation[i-1];

	if (i == 0) {
		create_question(&req, &v, v.carrefour, -1, -1, -1, -1, v.voie->numero, -1, MESSSORT);		
	} else {
		create_question(&req, &v, v.carrefour, croisement_precedent_numero, croisement_precedent_orientation, -1, -1, v.voie->numero, -1, MESSSORT);			
	}

	if (assoc_carrefours[v.carrefour-1][v.voie->numero-1] == -1) {
		P(MUTEX);
		int *c = (int *) shmat(compteur, NULL, 0);
		(*c)++;
		sprintf(buffer, "%d Voitures sont sorties\n", *c);
		message(0, buffer);
		V(MUTEX);
		exit(0);
	}
	
	usleep(rand()%MAXPAUSE+MINPAUSE);
			
	voiture(v.numero, assoc_voies[v.voie->numero-1], assoc_carrefours[v.carrefour-1][v.voie->numero-1]);
}

/**
 void create_question(Requete *req, Voiture *v, int carrefour, int croisement_precedent, int croisement_precedent_orientation, int croisement, int croisement_orientation, int voie, int traverse, int type)
 * \brief Effectue les operations permettant la construction d'une requete vers le carrefour, affiche la requete, et l'envoie au carrefour.
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
void create_question(Requete *req, Voiture *v, int carrefour, int croisement_precedent, int croisement_precedent_orientation, int croisement, int croisement_orientation, int voie, int traverse, int type)
{
	P(MUTEX);
	constructionRequete(req, v, carrefour, croisement_precedent, croisement_precedent_orientation, croisement, croisement_orientation, voie, traverse, type);
	affichageRequete(req);
	msgsnd(msgid_carrefour[carrefour-1],req,tailleReq,0);
	V(MUTEX);
}

/**
 * \fn void receive_answer(Requete *req, int carrefour)
 * \brief Effectue les operations permettant la reception d'une reponse du serveur (depuis le carrefour), et affiche la reponse.
 *
 * Ne retourne que lorsque la reponse du serveur aura ete positive.
 * Permet de bloquer la voiture tant qu'elle n'a pas eu la permission d'avancer.
 * Tant qu'elle est bloquee, la voiture renvoie des demandes au serveur (via le carrefour) a intervalles reguliers.
 *
 * \param req Pointeur vers la requete qui a ete envoyee.
 * \param carrefour Le numero du carrefour ou se trouve la voiture. Utile pour n'ecouter que la file de message du carrefour concerne.
 */
void receive_answer(Requete *req, int carrefour)
{
	Reponse rep;
	msgrcv(msgid_carrefour[carrefour-1],&rep,tailleRep,getpid(),0);
	P(MUTEX);
	affichageReponse(req,&rep);
	V(MUTEX);
	if (rep.autorisation == 0) {
		do {
			msgsnd(msgid_carrefour[carrefour-1],req,tailleReq,0);
			msgrcv(msgid_carrefour[carrefour-1],&rep,tailleRep,getpid(),0);
			usleep(MINPAUSE);
		} while (rep.autorisation == 0);
		P(MUTEX);
		affichageRequete(req);
		affichageReponse(req,&rep);
		V(MUTEX);
	}
}

/**
 * \fn int random_voie(int v)
 * \brief Retourne un numero de voie aleatoire mais coherent selon la voie d'ou arrive la voiture.
 *
 * Deduit, selon la voie ou se trouvait la voiture sur le carrefour precedent, un numero de voie aleatoire en accord avec son arrivee sur le nouveau carrefour (<=> simule un changement de voie par le conducteur).
 *
 * Trois cas de figure :
 * - voie d'arrivee : 1, 2 ou 3 => voie generee : 1, 2, ou 3.
 * - voie d'arrivee : 4, 5 ou 6 => voie generee : 4, 5 ou 6.
 * - voie d'arrivee : 7, 8 ou 9 => voie generee : 7, 8, ou 9.
 * - voie d'arrivee : 10, 11 ou 12 => voie generee : 10, 11 ou 12.
 *
 * \param v Numero de la voie d'arrivee.
 *
 * \return Le numero de voie aleatoire parmis les voies possibles.
 */
int random_voie(int v)
{
		if (v == 1 || v == 2 || v == 3)
			return rand()%3+1;
		if (v == 4 || v == 5 || v == 6)
			return rand()%3+4;
		if (v == 7 || v == 8 || v == 9)
			return rand()%3+7;
		return rand()%3+10;
}
