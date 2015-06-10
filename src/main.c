/**
 * \file main.c
 * \brief Effectue les premieres operation du programme.
 *
 * Cree les processus fils, les objets IPC, affiche les premieres informations.
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

int tailleReq = sizeof(Requete) - sizeof(long);
int tailleRep = sizeof(Reponse);

int msgid_serveur;
int msgid_carrefour[4];
int sem_id;
int shm_id[4];
int compteur;

main(int argc,char* argv[])
{
	key_t cle[5];
	pid_t pid_Serveur;

	if (argc-1 == 0) {
		printf("Syntaxe : ""./project Nb1 Nb2"" OR ""./project Nb1""\n");
		exit(-1);
	}
	
	initsem();

	file = fopen("./test.txt", "w");
	fclose(file);

	if (
			((cle[0] = ftok(argv[0],'0')) == -1) ||
			((cle[1] = ftok(argv[0],'1')) == -1) ||		
			((cle[2] = ftok(argv[0],'2')) == -1) ||
			((cle[3] = ftok(argv[0],'3')) == -1) ||
			((cle[4] = ftok(argv[0],'4')) == -1)
		)
		erreurFin("Pb ftok");
	if (
			((msgid_serveur = msgget(cle[0],IPC_CREAT  | IPC_EXCL | 0600)) == -1) ||
			((msgid_carrefour[0] = msgget(cle[1],IPC_CREAT  | IPC_EXCL | 0600)) == -1) ||
			((msgid_carrefour[1] = msgget(cle[2],IPC_CREAT  | IPC_EXCL | 0600)) == -1) ||
			((msgid_carrefour[2] = msgget(cle[3],IPC_CREAT  | IPC_EXCL | 0600)) == -1) ||
			((msgid_carrefour[3] = msgget(cle[4],IPC_CREAT  | IPC_EXCL | 0600)) == -1)
		)
		erreurFin("Pb msgget");

	initialise_carrefours();
	initialise_compteur();
	
	signal(SIGINT,traitantSIGINT);

	pid_Serveur = forkServeur();
	forkCarrefours(pid_Serveur);

	if (argc-1 == 1) {
		premiere_ligne(atoi(argv[1]));
		forkn(atoi(argv[1]), NULL, voiture);
	} else {
		premiere_ligne(argc-1);
		forkn(argc-1, argv, voiture);
	}

	while (1) {
		pid_t done = wait();
		if (done == -1)
		if (errno == ECHILD) break;
	}

	msgctl(msgid_serveur, IPC_RMID, NULL);
	msgctl(msgid_carrefour[0], IPC_RMID, NULL);
	msgctl(msgid_carrefour[1], IPC_RMID, NULL);
	msgctl(msgid_carrefour[2], IPC_RMID, NULL);
	msgctl(msgid_carrefour[3], IPC_RMID, NULL);
	semctl(sem_id, 0, IPC_RMID, NULL);
	shmctl(shm_id, IPC_RMID, NULL);
}

/**
 * \fn void erreurFin(const char* msg)
 * \brief Affiche un message d'erreur systeme.
 *
 * \param msg Le message d'erreur a afficher.
 */
void erreurFin(const char* msg){ perror(msg); exit(1); }

/**
 * \fn void forkn(int nbFils, char *voies[], void (*fonction)())
 * \brief Cree nbFils processus fils (voitures) qui executent la meme fonction.
 *
 * Si voies est NULL, cree nbFils voitures avec une voie aleatoire.
 * Si voies est non NULL, cree nbFils voitures avec une la voie correspondant aux differents membres de voies.
 *
 * \param nbFils Le nombre de voitures a creer.
 * \param voies Le tableau des voies a affecter aux voitures.
 * \param fonction La fonction qui sera exectuee par les voitures.
 */
void forkn(int nbFils, char *voies[], void (*fonction)())
{
	int i;
	
	if (voies == NULL) {
		for (i=0;i<nbFils;i++)
			if (fork()==0) {
				(*fonction) (i, -1, -1);
				exit(0);
			}
	} else {
		for (i=0;i<nbFils;i++)
			if (fork()==0) {
				(*fonction) (i, atoi(voies[i+1]), -1);
				exit(0);
			}
	}
}

void forkCarrefours(pid_t pid_Serveur)
{
	int i;
	
	for (i=1;i<5;i++) {
		if (fork() == 0) {
			carrefour(i, pid_Serveur);
		}
	}
}

pid_t forkServeur()
{
	pid_t pid_Serveur;
	pid_Serveur = fork();
	
	if (pid_Serveur == 0) {
		serveur();
	} else return pid_Serveur;
}
/**
 * \fn void traitantSIGINT(int s)
 * \brief Redefini le signal SIGINT.
 * Supprime les objets IPC lors de l'interception d'un signal SIGINT.
 *
 * \param s Le numero du signal intercepte.
 */ 
void traitantSIGINT(int s)
{
	msgctl(msgid_serveur, IPC_RMID, NULL);
	msgctl(msgid_carrefour[0], IPC_RMID, NULL);
	msgctl(msgid_carrefour[1], IPC_RMID, NULL);
	msgctl(msgid_carrefour[2], IPC_RMID, NULL);
	msgctl(msgid_carrefour[3], IPC_RMID, NULL);
	semctl(sem_id, 0, IPC_RMID, NULL);
	shmctl(shm_id, IPC_RMID, NULL);
	exit(0);
}

/**
 * \fn void premiere_ligne(int num)
 * \brief Affiche la premiere ligne d'informations.
 * Affiche sous forme de colonnes les differentes voitures creees.
 *
 * \param num Le nombre de voitures creees (<=> le nombre de colonnes)
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

void initialise_carrefours()
{
	int i,j;
	Carrefour base = {{{0},{0},{-1},{0},{0},{0},{0},{0},{0},{0},{-1},{0},{-1},{0},{-1},{0},{0},{0},{0},{0},{0},{0},{-1},{0},{0}}};
	Carrefour *c;

	for (i=0;i<4;i++) {
		shm_id[i] = shmget(IPC_PRIVATE, sizeof(Carrefour), IPC_CREAT | 0666);
		c = (Carrefour *) shmat(shm_id[i], NULL, 0);
		
		for (j=0;j<25;j++) {
			c->croisements[j].etat = base.croisements[j].etat;
		}
	}
}

void initialise_compteur()
{
	compteur = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
}
