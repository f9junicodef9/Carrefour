#ifndef __VOITURE__ 
#define __VOITURE__

#include "project.h"

void affiche_voiture(Voiture *v);

void initRand();

void voiture(int numero, int voie);

void maj_position(Voiture *v, int position, int traverse);

void create_question(Requete *req, Voiture *v, int croisement_precedent, int croisement_precedent_orientation, int croisement, int croisement_orientation, int voie, int traverse, int type);

void receive_answer(Requete *req, Reponse *rep);

#endif
