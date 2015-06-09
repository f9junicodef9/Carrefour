#ifndef __PROJECT__ 
#define __PROJECT__

#define MUTEX 1

#define MAXPAUSE 3

#define MESSARRIVE 1
#define MESSDEMANDE 2
#define MESSINFO 3
#define MESSSORT 4

#define AVANT 1
#define PENDANT 2
#define APRES 3

#define HO 0
#define VE 1

/**
 * \struct Voie
 * \brief Represente une voie d'un carrefour.
 *
 * Une voie d'un carrefour est caracterisee par un numero (de 1 a 12).
 * Chaque voie possede un maximum de 6 sections critiques representant les intersections (et donc collisions possibles) avec les autres voies.
 */
typedef struct Voie {
	int numero;	/*!< Le numero de la voie. */
	int sem_num[6];	/*!< Les indices des croisements avec les autres voies (si la voie possede moins de 6 intersections, le premier indice "vide" vaut -1) */
	int croisements[6];	/*!< Les numeros des voies croisees au niveau des indices de croisements respectifs (si la voie possede moins de 6 intersections, le premier numero "vide" vaut -1) */
	int orientation[6];	/*!< Les orientations des croisements respectifs (horizontal / vertical) */
} Voie;

/**
 * \struct Croisement
 * \brief Represente un croisement d'un carrefour.
 *
 * 5 croisements sont "inutiles" car ne croisent aucune autre voie (N°2, 10, 12, 14 et 22).
 * Chaque croisement possede deux compteurs :
 * - avant : Le nombre de vehicules en attente avant le croisement.
 * - apres : Le nombre de vehicules en attente apres le croisement.
 * Et une variable indiquant l'etat du croisement. Peut valoir :
 * - 0 : Personne n'est en train de traverser le croisement
 * - 1 : Quelqu'un est en train de traverser le croisement
 */
typedef struct Croisement {
	int etat;	/*!< L'etat du croisement. */
	int avantH;	/*!< Le nombre de vehicules en attente avant le croisement horizontal. */
	int apresH;	/*!< Le nombre de vehicules en attente apres le croisement horizontal. */
	int avantV;	/*!< Le nombre de vehicules en attente avant le croisement vertical. */
	int apresV;	/*!< Le nombre de vehicules en attente apres le croisement vertical. */
} Croisement;

/**
 * \struct Voiture
 * \brief Represente une voiture.
 *
 * Une voiture est caracterisee par :
 * - Un numero
 * - Une voie qui lui est attribuee et qu'elle suivra depuis son entree dans le carrefour jusqu'a sa sortie
 * - L'indice du croisement ou elle se trouve dans le carrefour (n'a pas de sens lorsqu'elle est en entree/sortie du carrefour)
 * L'etat de traversement (directement lie aux informations de traversement). Peut valoir :
 *  - AVANT
 *  - PENDANT
 *  - APRES
 */
typedef struct Voiture {
	int numero;	/*!< Le numero de la voiture. */
	Voie *voie;	/*!< La voie attribuee a la voiture. */
	int carrefour;
	int position;	/*!< Le croisement ou se trouve la voiture le cas echeant. */
	int position_traversee;	/*!< L'etat de traversement du croisement le cas echeant. */
} Voiture;

/**
 * \struct Requete
 * \brief Represente une requete du client pour le serveur (d'une voiture pour un carrefour).
 *
 * Une requete est caracterisee par :
 * - Son type. Peut valoir :
 *  - MESSARRIVE : Indique que la voiture arrive sur le carrefour par la voie indiquee.
 *  - MESSDEMANDE : Demande au serveur (carrefour) la permission de traverser le croisement indique.
 *  - MESSTRAVERSE : Indique que la voiture debute le traversement du croisement indique.
 *  - MESSATRAVERSE : Indique que la voiture a traverse le croisement indique.
 *  - MESSSORT : Indique que la voiture quitte le carrefour par la voie indiquee.
 * - Le pid du processus qui emet la requete (la voiture).
 * - Une copie de la voiture qui emet la requete.
 * - L'indice du croisement ou se trouve la voiture.
 * - Le numero de la voie, selon le cas :
 *  - ou se trouve la voiture
 *  - qui est traversee au croisement indique
 * - L'indice du croisement ou se trouve la voiture.
 * - L'etat de traversement (directement lie aux informations de traversement). Peut valoir :
 *  - AVANT
 *  - PENDANT
 *  - APRES
 */
typedef struct {
	long  type;	/*!< Le type de message. */
	pid_t pidEmetteur;	/*!< Le pid du processus emetteur. */
	Voiture v;	/*!< La copie de la voiture qui emet la requete. */
	int croisement;	/*!< L'indice du croisement. */
	int croisement_orientation;	/*!< L'orientation (horizontal / vertical) du croisement. */
	int croisement_precedent;	/*!< L'indice du croisement precedent. */
	int croisement_precedent_orientation;	/*!< L'orientation (horizontal / vertical) du croisement precedent. */
	int voie;	/*!< Le numero de la voie. */
	int traverse;	/*!< L'etat de traversement du croisement. */
} Requete;

/**
 * \struct Reponse
 * \brief Represente une reponse du serveur pour le client (d'un carrefour pour une voiture).
 *
 * Une requete est caracterisee par :
 * - Son type. Vaut le pid du client ayant emis la requete.
 * - La reponse a la requete. Peut valoir :
 *  - 1 : autorise la voiture a traverser le croisement demande
 *  - 2 : interdit la voiture de traverser le croisement demande
 */
typedef struct {
	long  type;	/*!< Le type de message. */
	int autorisation;	/*!< La reponse a la requete. */
	Voiture v;
} Reponse;

extern int tailleReq;	/*!< La taille d'une requete. */
extern int tailleRep;	/*!< La taille d'une reponse. */

extern Croisement croisements[];	/*!< La representation d'un carrefour. */

extern int msgid;	/*!< La file de message utilisee pour communiquer entre les voitures et le carrefour */
extern int sem_id;	/*!< L'identifiant de l'ensemble de semaphores */

#endif
