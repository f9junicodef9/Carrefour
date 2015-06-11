#ifndef __SEM__ 
#define __SEM__

#include <sys/sem.h>

struct sembuf sem_oper;

void P(int semnum);

void V(int semnum);

int initialise_semaphore();

#endif
