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

int msgid;
int sem_id;

main(int argc,char* argv[])
{
	key_t cle;

	if (argc-1 == 0) {
		printf("Syntaxe : ""./project Nb1 Nb2"" OR ""./project Nb1""\n");
		exit(-1);
	}
	
	initsem();

	file = fopen("./test.txt", "w");
	fclose(file);

	if ((cle = ftok(argv[0],'0')) == -1)
	erreurFin("Pb ftok");
	if ((msgid = msgget(cle,IPC_CREAT  | IPC_EXCL | 0600)) == -1)
	erreurFin("Pb msgget");

	signal(SIGINT,traitantSIGINT);

	if (argc-1 == 1) {
		premiere_ligne(atoi(argv[1]));
		forkn(atoi(argv[1]), NULL, voiture);
	} else {
		premiere_ligne(argc-1);
		forkn(argc-1, argv, voiture);
	}
		
	carrefour();

	while (1) {
		pid_t done = wait();
		if (done == -1)
		if (errno == ECHILD) break;
	}

	msgctl(msgid, IPC_RMID, NULL);
	semctl(sem_id, 0, IPC_RMID, NULL);
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
				(*fonction) (i, -1);
				exit(0);
			}
	} else {
		for (i=0;i<nbFils;i++)
			if (fork()==0) {
				(*fonction) (i, atoi(voies[i+1]));
				exit(0);
			}
	}
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
	msgctl(msgid,IPC_RMID,NULL);
	semctl(sem_id, 0, IPC_RMID, NULL);
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
