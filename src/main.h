#ifndef __MAIN__ 
#define __MAIN__

void erreurFin(const char* msg);

void forkVoitures(int nbFils, char *voies[], void (*fonction)());

void traitantSIGINT(int s);

void premiere_ligne(int num);

void forkCarrefours(pid_t pid_Serveur);

pid_t forkServeur();

void initialise_carrefours();

void initialise_compteur();

#endif
