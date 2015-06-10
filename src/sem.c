/**
 * \file sem.c
 * \brief Gere les semaphores et les operations sur ces dernieres
 */
#include <stdio.h>
#include <stdlib.h>
#include "project.h"
#include "sem.h"

/**
 * \fn void P(int semnum)
 * \brief Decremente le semaphore (blocage)
 *
 * \param semnum Le numero du semaphore.
 */
void P(int semnum)
{
	sem_oper.sem_num = semnum-1;	// Numéro du sémaphore
	sem_oper.sem_op = -1 ;	// Opération (P(s))
	sem_oper.sem_flg = 0 ;	// NULL

	semop(sem_id,&sem_oper,1);
}

/**
 * \fn void V(int semnum)
 * \brief Incremente le semaphore (deblocage)
 *
 * \param semnum Le numero du semaphore.
 */
void V(int semnum)
{
	sem_oper.sem_num = semnum-1;	// Numéro du sémaphore
	sem_oper.sem_op = 1 ;	// Opération (V(s))
	sem_oper.sem_flg = 0 ;	// NULL

	semop(sem_id,&sem_oper,1);
}

/**
 * \fn int initsem()
 * \brief Initialise les semaphores. Un seul semaphore est utilise ici.
 */
int initsem()
{
	sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666);

	semctl(sem_id, 0, SETVAL, 1);

	return (sem_id);
}