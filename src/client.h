#ifndef __CLIENT__ 
#define __CLIENT__

void constructionRequete(Requete *req, Voiture *v, int carrefour, int croisement_precedent, int croisement_precedent_orientation, int croisement, int croisement_orientation, int voie, int traverse, int type);

void affichageRequete(Requete *req);

#endif
