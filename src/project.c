#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h> 
#include <errno.h>

#define MUTEX 21

#define MAXPAUSE 2

#define MESSARRIVE 1
#define MESSDEMANDE 2
#define MESSTRAVERSE 3
#define MESSATRAVERSE 4
#define MESSSORT 5

#define PASTRAVERSE -1
#define TRAVERSE 0
#define ATRAVERSE 1

typedef struct Voie {
	int numero;
	int sem_num[6];
	int croisements[6];
} Voie;

typedef struct Voiture {
	int numero;
	Voie *voie;
	int position;
	int position_traversee;
} Voiture;

typedef struct {
	long  type;
	pid_t pidEmetteur;
	Voiture v;
	int croisement;
} trequete;

typedef struct {
	long  type;
	Voiture v;
	int autorisation;
} treponse;

int msgid;
int sem_id;
int tailleReq = sizeof(trequete) - sizeof(long);
int tailleRep = sizeof(treponse);
char buffer[1024];
char output[10240];
FILE *file;
struct sembuf sem_oper ;  /* Operation P & V */

Voie voies[12] = {	{1,{0,1,2,3,4,5},{11,7,10,4,7,8}},
							{2,{6,7,8,9},{11,7,4,5}},
							{3,{-1},{-1}},
							{4,{8,10,3,11,12,13},{2,10,1,7,10,11}},
							{5,{9,14,15,16},{2,10,7,8}},
							{6,{-1},{-1}},
							{7,{15,4,11,17,1,7},{5,1,4,10,1,2}},
							{8,{16,5,18,19},{5,1,10,11}},
							{9,{-1},{-1}},
							{10,{18,12,17,2,10,14},{8,4,7,1,4,5}},
							{11,{19,13,0,6},{8,4,1,2}},
							{12,{-1},{-1}}	};
							
void P(int semnum)
{
	//printf("Blocage semaphore %d\n",semnum);
	sem_oper.sem_num = semnum-1;	// Num�ro du s�maphore
	sem_oper.sem_op = -1 ;	// Op�ration (P(s))
	sem_oper.sem_flg = 0 ;	// NULL

	semop(sem_id,&sem_oper,1);
}

void V(int semnum)
{
	//printf("Deblocage semaphore %d\n",semnum);
	sem_oper.sem_num = semnum-1;	// Num�ro du s�maphore
	sem_oper.sem_op = 1 ;	// Op�ration (V(s))
	sem_oper.sem_flg = 0 ;	// NULL

	semop(sem_id,&sem_oper,1);
}

int initsem() 
{
   sem_id = semget(IPC_PRIVATE, 16, IPC_CREAT | IPC_EXCL | 0666);
    
   union semun {
			int val;
			struct semid_ds *stat;
			ushort *array;
	} ctl_arg;

	ushort array[21] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};	// 20 => MUTEX
	ctl_arg.array = array;
	semctl(sem_id, 0, SETALL, ctl_arg);

	return (sem_id);
}

void message(int i, char* s) {
   #define COLONNE 20
   int j, NbBlanc;
   file = fopen("./test.txt", "a");
   NbBlanc=i*COLONNE;
   for (j=0; j<NbBlanc; j++) {
		putchar(' ');
		fprintf(file, " ");
	}
   printf("%s",s);
   fprintf(file, "%s", s);
   fflush(stdout);
   fclose(file);
}

void initRand()
{
	srand(getpid());
}

void erreurFin(const char* msg){ perror(msg); exit(1); }

/*---------------- CODE CLIENT -----------------------*/

void constructionRequete(trequete *req, Voiture *v, int croisement, int type)
{
	req->pidEmetteur = getpid();
	req->v = *v;
	req->croisement = croisement;
	req->type = type;
}

void affichageRequete(trequete *req)
{
	if (req->type == MESSARRIVE)
		snprintf(buffer, sizeof(buffer), "Arrive voie %d\n", req->v.voie->numero);
	else if (req->type == MESSDEMANDE)
		snprintf(buffer, sizeof(buffer), "Dem. int. %d\n", req->croisement);
	else if (req->type == MESSTRAVERSE)
		snprintf(buffer, sizeof(buffer), "Traverse voie %d\n", req->croisement);
	else if (req->type == MESSATRAVERSE)
		snprintf(buffer, sizeof(buffer), "A traverse voie %d\n", req->croisement);
	else if (req->type == MESSSORT)
		snprintf(buffer, sizeof(buffer), "Sort voie %d\n", req->v.voie->numero);

	message(req->v.numero, buffer);
}

void affichageReponse(trequete *req,treponse *rep)
{

}

void maj_position(Voiture *v, int position, int traverse)
{
	v->position = position;
	v->position_traversee = traverse;	
}

void voiture(int numero, int voie)
{
	Voiture v;
	trequete req;
	treponse rep;
	int croisement, i;
	
	if (voie == -1) {
		initRand();
		v.numero = numero;
		int voie_random = rand()%12+1;
		v.voie = &voies[voie_random-1];
	} else {
		v.numero = numero;
		v.voie = &voies[voie-1];
	}

/*
	tailleRep = sizeof(int);
	break;
	msgrcv(msgid,&rep,tailleRep,getpid(),0);
	affichageReponse(&req,&rep);
	break;
	}
*/

	P(MUTEX);
	constructionRequete(&req, &v, 0, MESSARRIVE);
	affichageRequete(&req);
	msgsnd(msgid,&req,tailleReq,0);
	V(MUTEX);

	for (i=0 ; i < 6 ; i++) {
			croisement = v.voie->croisements[i];
			
			if (croisement == -1) break;
			
			P(MUTEX);
			maj_position(&v, croisement, PASTRAVERSE);
			constructionRequete(&req, &v, croisement, MESSDEMANDE);
			affichageRequete(&req);
			V(MUTEX);
			
			do {
				msgsnd(msgid,&req,tailleReq,0);
				msgrcv(msgid,&rep,tailleRep,getpid(),0);
			} while (rep.autorisation == 0);
			
			P(MUTEX);
			maj_position(&v, croisement, TRAVERSE);
			constructionRequete(&req, &v, croisement, MESSTRAVERSE);
			affichageRequete(&req);
			msgsnd(msgid,&req,tailleReq,0);
			V(MUTEX);
			
			sleep(rand()%MAXPAUSE);
			
			P(MUTEX);
			maj_position(&v, croisement, ATRAVERSE);
			constructionRequete(&req, &v, croisement, MESSATRAVERSE);
			affichageRequete(&req);
			msgsnd(msgid,&req,tailleReq,0);
			V(MUTEX);
	}

	P(MUTEX);	
	constructionRequete(&req, &v, 0, MESSSORT);
	affichageRequete(&req);
	msgsnd(msgid,&req,tailleReq,0);
	V(MUTEX);
}

/*---------------- CODE SERVEUR -----------------------*/

void constructionReponse(trequete *req,treponse *rep)
{

	rep->type = req->pidEmetteur;
	rep->autorisation = 1;
	
//	if (req->type == 1) printf("Voiture %d proc %d arrive voie %d\n", req->v.numero, req->pidEmetteur, req->v.voie->numero);
//	else if (req->type == 2) printf("Voiture %d proc %d demande a passer intersection %d voie %d\n", req->v.numero, req->pidEmetteur, req->croisement, req->v.voie->numero);
	
	
/*			val = semctl(sem_id, croisement-1, GETVAL, NULL);
			if (val == 0) {
				P(21);
				snprintf(buffer, sizeof(buffer), "Priorite a voie %d\n", croisement);
				message(voiture.num, buffer);
				V(21);
				P(s);
				P(21);
				snprintf(buffer, sizeof(buffer), "Repart voie %d\n", croisement);
				message(voiture.num, buffer);
				V(21);
			} else {
				P(s);
			}
			P(21);
			snprintf(buffer, sizeof(buffer), "Traverse voie %d\n", croisement);
			message(voiture.num, buffer);
			V(21);
			sleep(rand()%MAXPAUSE);
			P(21);
			snprintf(buffer, sizeof(buffer), "A traverse voie %d\n", croisement);
			message(voiture.num, buffer);
			V(21);
			V(s);
*/

}

void maj_carrefour(trequete *req)
{

}

void serveur()
{
	trequete req;
	treponse rep;

	initRand();

	while (1) {
		msgrcv(msgid,&req,tailleReq,0,0);

		maj_carrefour(&req);
		
		if (req.type == MESSDEMANDE) {
			constructionReponse(&req,&rep);
			msgsnd(msgid,&rep,tailleRep,0);
		}
	}
}

/*------------- CODE GESTION PROCESSUS -----------------------*/

/* fonction permettant de creer nbFils qui executent la meme fonction */
void forkn(int nbFils, int voie, void (*pauBoulot)())
{
	int i;
	for (i=0;i<nbFils;i++)
		if (fork()==0) {
			(*pauBoulot) (i, -1);
			exit(0);
		}
}

void traitantSIGINT(int s)
{
	msgctl(msgid,IPC_RMID,NULL);
	semctl(sem_id, 0, IPC_RMID, NULL);
	exit(0);
}

main(int argc,char* argv[])
{
	int num;
	key_t cle;

	if (argc-1 == 0) {
		printf("Syntaxe : ""./project Nb1 Nb2"" OR ""./project Nb1""\n");
		exit(-1);
	} else {
		initsem();
		
		file = fopen("./test.txt", "w");
		fclose(file);
	
		if ((cle = ftok(argv[0],'0')) == -1)
			erreurFin("Pb ftok");
		if ((msgid = msgget(cle,IPC_CREAT  | IPC_EXCL | 0600)) == -1)
			erreurFin("Pb msgget");

		signal(SIGINT,traitantSIGINT);
	}
	
	if (argc-1 == 1) {
		for (num = 0 ; num < atoi(argv[1]) ; num++) {
			snprintf(buffer, sizeof(buffer), "     %s  %d     ", "Voiture", num);
			message(0, buffer);
		}
		message(0, "\n\n");

		forkn(atoi(argv[1]), -1, voiture);
	} else {
		for (num = 0 ; num < argc-1 ; num++) {
			snprintf(buffer, sizeof(buffer), "     %s  %d     ", "Voiture", num);
			message(0, buffer);
		}
		message(0, "\n");

		for (num = 0 ; num < argc-1 ; num++) {
			if (fork() == 0) {
					voiture(num, atoi(argv[num+1]));
					return;
			}
		}
	}
	
	serveur();

	while (1) {
		pid_t done = wait();
		if (done == -1)
			if (errno == ECHILD) break;
	}

	msgctl(msgid, IPC_RMID, NULL);
	semctl(sem_id, 0, IPC_RMID, NULL);
}
