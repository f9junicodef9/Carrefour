/**
 * \file main.c
 * \brief Effectue les premieres operation du programme.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <signal.h>
#include <errno.h>
#include "project.h"
#include "main.h"
#include "interface.h"
#include "voiture.h"
#include "carrefour.h"
#include "serveur.h"

int tailleReq = sizeof(Requete) - sizeof(long);
int tailleRep = sizeof(Reponse);

int msg_serveur;
int msg_carrefour[4];
int sem;
int compteur;

int carrefours[4];

pid_t pid_Serveur;
pid_t pid_Carrefour[4];

/**
 * \fn main(int argc,char* argv[])
 * \brief Cree les processus fils (voitures, carrefours, serveur), les objets IPC, affiche les premieres informations.
 */
main(int argc,char* argv[])
{

	if (argc-1 == 0) {
		printf("Syntaxe : ""./project Nb1 Nb2"" OR ""./project Nb1""\n");
		exit(-1);
	}
	

	file = fopen("./output.txt", "w");
	fclose(file);

	initialise_semaphore();
	initialise_files();
	initialise_carrefours();
	initialise_compteur();

	forkServeur(serveur);
	forkCarrefours(carrefour);

	if (argc-1 == 1) {
		premiere_ligne(atoi(argv[1]));
		forkVoitures(atoi(argv[1]), NULL, voiture);
	} else {
		premiere_ligne((argc-1)/2);
		forkVoitures((argc-1)/2, argv, voiture);
	}

	signal(SIGINT,traitantSIGINT);
	
	while (1) {
		pid_t done = wait();
		if (done == -1)
		if (errno == ECHILD) break;
	}
}

/**
 * \fn void erreurFin(const char* msg)
 * \brief Affiche un message d'erreur systeme.
 *
 * \param msg Le message d'erreur a afficher.
 */
void erreurFin(const char* msg){ perror(msg); exit(1); }

/**
 * \fn void forkVoitures(int nb, char *argv[], void (*fonction)())
 * \brief Cree nbFils processus fils (voitures) qui executent la meme fonction.
 *
 * Si voies est NULL, cree nbFils voitures avec une voie aleatoire.
 * Si voies est non NULL, cree nbFils voitures avec une la voie correspondant aux differents membres de voies.
 *
 * \param nbVoitures Le nombre de voitures a creer.
 * \param argv Le tableau des voies et carrefours a affecter aux voitures le cas echeant.
 * \param fonction Pointeur sur la fonction qui sera exectuee par les voitures.
 */
void forkVoitures(int nbVoitures, char *argv[], void (*fonction)())
{
	int i;

	if (argv == NULL) {
		for (i=0;i<nb;i++)
			if (fork()==0)
				(*fonction) (i, -1, -1, 1);
	} else {
		for (i=0;i<nb;i++)
			if (fork()==0)
				(*fonction) (i, atoi(argv[1+(i*2)]), atoi(argv[2+(i*2)]), 1);
	}
}

/**
 * \fn void forkCarrefours(void (*fonction)())
 * \brief Cree 4 processus fils (carrefours) qui executent la meme fonction.
 *
 * \param fonction Pointeur sur la fonction qui sera exectuee par les carrefours.
 */
void forkCarrefours(void (*fonction)())
{
	int i;
	
	for (i=0;i<4;i++) {
		pid_Carrefour[i] = fork();

		if (pid_Carrefour[i] == 0) {
			(*fonction) (i+1, pid_Serveur);
		}
	}
}

/**
 * \fn void forkServeur(void (*fonction)())
 * \brief Cree 1 processus fils (serveur) qui executent une fonction.
 *
 * \param fonction Pointeur sur la fonction qui sera exectuee par le serveur.
 */
void forkServeur(void (*fonction)())
{
	pid_Serveur = fork();
	
	if (pid_Serveur == 0) {
		(*fonction) ();
	}
}

/**
 * \fn void traitantSIGINT(int s)
 * \brief Redefini le traitant du signal SIGINT.
 *
 * Supprime les objets IPC et termine le programme lors de l'interception d'un signal SIGINT.
 *
 * \param s Le numero du signal intercepte (SIGINT).
 */ 
void traitantSIGINT(int s)
{
	if (
			((msgctl(msg_serveur, IPC_RMID, NULL)) == -1) ||
			((msgctl(msg_carrefour[0], IPC_RMID, NULL)) == -1) ||
			((msgctl(msg_carrefour[1], IPC_RMID, NULL)) == -1) ||
			((msgctl(msg_carrefour[2], IPC_RMID, NULL)) == -1) ||
			((msgctl(msg_carrefour[3], IPC_RMID, NULL)) == -1) ||
			((semctl(sem, 0, IPC_RMID, NULL)) == -1) ||
			((shmctl(compteur, IPC_RMID, NULL)) == -1) ||
			((shmctl(carrefours[0], IPC_RMID, NULL)) == -1) ||
			((shmctl(carrefours[1], IPC_RMID, NULL)) == -1) ||
			((shmctl(carrefours[2], IPC_RMID, NULL)) == -1) ||
			((shmctl(carrefours[3], IPC_RMID, NULL)) == -1)												
		)
		erreurFin("Pb ipc_rmid");
		
	exit(0);
}

/**
 * \fn void premiere_ligne(int num)
 * \brief Affiche la premiere ligne d'informations.
 *
 * Affiche sous forme de colonnes les differentes voitures creees (forme "Voiture i    Voiture j...").
 *
 * \param num Le nombre de voitures creees (<=> le nombre de colonnes).
 */
void premiere_ligne(int num)
{
	int i;
	P(MUTEX);
	for (i = 0 ; i < num ; i++) {
		snprintf(buffer, sizeof(buffer), "     %s  %d     ", "Voiture", i);
		message(0, buffer);
	}
	message(0, "\n\n");
	V(MUTEX);
}

/**
 * \fn void initialise_carrefours()
 * \brief Cree un segment de memoire partagee pour chaque carrefour.
 *
 * Permet d'acceder aux informations des carrefours et de les modifier depuis les autres processus.
 * Initialise les carrefours avec certaines informations de base.
 */
void initialise_carrefours()
{
	int i,j;
	Carrefour base = {{{0},{0},{-1},{0},{0},{0},{0},{0},{0},{0},{-1},{0},{-1},{0},{-1},{0},{0},{0},{0},{0},{0},{0},{-1},{0},{0}}};
	Carrefour *c;

	for (i=0;i<4;i++) {
		carrefours[i] = shmget(IPC_PRIVATE, sizeof(Carrefour), IPC_CREAT | 0666);
		c = (Carrefour *) shmat(carrefours[i], NULL, 0);
		
		for (j=0;j<25;j++) {
			c->croisements[j].etat = base.croisements[j].etat;
		}
	}
}

/**
 * \fn void initialise_compteur()
 * \brief Cree un segment de memoire partagee pour le compteur de voitures sorties.
 *
 * Permet a chaque voiture d'incrementer un compteur lorsqu'elle quitte la carte (<=> un processus voiture se termine), pour verifier si aucune voiture n'est bloquee sur un carrefour.
 */
void initialise_compteur()
{
	compteur = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
}

/**
 * \fn void initialise_files()
 * \brief Cree les files de messages du projet.
 *
 * Les files creees sont :
 * - 4 files pour les communications entre les voitures et les carrefours (1 par carrefour).
 * - 1 file pour les communications entre les carrefours et le serveur.
 *
 */
void initialise_files()
{
	if (
			((msg_serveur = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) == -1) ||
			((msg_carrefour[0] = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) == -1) ||
			((msg_carrefour[1] = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) == -1) ||
			((msg_carrefour[2] = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) == -1) ||
			((msg_carrefour[3] = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) == -1)
		)
		erreurFin("Pb msgget");
}
