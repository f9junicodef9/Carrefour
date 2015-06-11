#ifndef __MAIN__ 
#define __MAIN__

void erreurFin(const char* msg);

void forkVoitures(int nb, char *argv[], void (*fonction)());

void traitantSIGINT(int s);

void premiere_ligne(int num);

void forkCarrefours(void (*fonction)());

void forkServeur(void (*fonction)());

void initialise_carrefours();

void initialise_compteur();

void initialise_files();

#endif
