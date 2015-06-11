/**
 * \file project.h
 * \brief Contient les structures et variables globales liees au projet.
 */
#ifndef __PROJECT__ 
#define __PROJECT__

#define MUTEX 1

#define MAXPAUSE 500000
#define MINPAUSE 100

#define MAXFILE 1

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
 * Chaque voie possede un maximum de 6 sections critiques representant les intersections (et donc collisions possibles) avec les autres voies : les croisements.
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
 * 5 croisements sont "inutiles" car ne croisent aucune autre voie (N°2, 10, 12, 14 et 22), mais gardes par soucis de simplicite d'affichage.
 * Chaque croisement possede :
 * - 4 compteurs indiquant :
 *  - Le nombre de vehicules en attente avant le croisement horizontal.
 *  - Le nombre de vehicules en attente apres le croisement horizontal.
 *  - Le nombre de vehicules en attente avant le croisement vertical.
 *  - Le nombre de vehicules en attente apres le croisement vertical.
 * - Une variable indiquant l'etat du croisement. Peut valoir :
 *  - -1 : Le croisement est inutile car ne croise aucune autre voie (utilise uniquement a des fins d'affichage).
 *  - 0 : Personne n'est en train de traverser le croisement.
 *  - 1 : Quelqu'un est en train de traverser le croisement.
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
 * - Une voie qui lui est attribuee et qu'elle suivra depuis son entree dans le carrefour jusqu'a sa sortie (1<=voie<=12).
 * - Un carrefour sur lequel elle se trouve (1<=carrefour<=4).
 */
typedef struct Voiture {
	int numero;	/*!< Le numero de la voiture. */
	Voie *voie;	/*!< La voie attribuee a la voiture. */
	int carrefour;	/*!< Le carrefour ou se trouve la voiture. */
} Voiture;

/**
 * \struct Requete
 * \brief Represente une requete du client pour le serveur (d'une voiture pour un carrefour / le serveur).
 *
 * Une requete est caracterisee par :
 * - Son type. Peut valoir :
 *  - MESSARRIVE : message d'information d'arrivee sur un carrefour. N'implique aucun traitement, affiche juste l'information.
 *  - MESSDEMANDE : demande d'arrivee en zone avant, pendant, ou apres un croisement. Implique une demande au serveur (depuis le carrefour) qui va analyser le carrefour pour prendre une decision.
 *  - MESSINFO : message d'information d'arrivee avant, pendant, ou apres un croisement. N'implique aucun traitement, affiche juste l'information.
 *  - MESSSORT : message d'information de sortie d'un carrefour. Implique de mettre a jour la file d'attente "apres" le croisement precedent.
 * - Le pid du processus qui emet la requete (la voiture si entre voiture-carrefour, le carrefour si entre carrefour-serveur).
 * - Une copie de la voiture qui emet la requete.
 * - L'indice du croisement ou se trouve la voiture.
 * - Le numero de la voie, selon le cas :
 *  - ou se trouve la voiture (pour les informations d'entree / sortie de carrefour)
 *  - qui est traversee au croisement indique (pour les demandes au serveur et informations aux carrefours)
 * - L'indice du croisement ou se trouve la voiture.
 * - L'etat de traversement du croisement (directement lie aux informations de traversement). Peut valoir :
 *  - AVANT
 *  - PENDANT
 *  - APRES
 *  - -1 si inaproprié (arrivee / sortie d'un carrefour => pas de croisement)
 */
typedef struct {
	long  type;	/*!< Le type de message. */
	pid_t pidEmetteur;	/*!< Le pid du processus emetteur. */
	Voiture v;	/*!< La copie de la voiture qui emet la requete. */
	int carrefour;	/*!< Numero du carrefour ou se trouve la voiture effectuant la requete. */
	int croisement;	/*!< L'indice du croisement ou se trouve la voiture. */
	int croisement_orientation;	/*!< L'orientation (horizontal / vertical) du croisement. */
	int croisement_precedent;	/*!< L'indice du croisement precedent. */
	int croisement_precedent_orientation;	/*!< L'orientation (horizontal / vertical) du croisement precedent. */
	int voie;	/*!< Le numero de la voie. */
	int traverse;	/*!< L'etat de traversement du croisement. */
} Requete;

/**
 * \struct Reponse
 * \brief Represente une reponse du serveur pour le client (du serveur pour une voiture).
 *
 * Une requete est caracterisee par :
 * - Son type. Vaut le pid du client ayant emis la requete (le carrefour quand la reponse part du serveur ; la voiture quand la reponse repart du carrefour).
 * - La reponse a la requete. Peut valoir :
 *  - 1 : autorise la voiture a avancer / traverser dans la zone / le croisement demande.
 *  - 0 : interdit la voiture de avancer / traverser dans la zone / le croisement demande.
 */
typedef struct {
	long  type;	/*!< Le type de message. */
	int autorisation;	/*!< La reponse a la requete. */
	Voiture v;	/*!< Variable necessaire pour eviter un plantage (explication a trouver). */
} Reponse;

/**
 * \struct Carrefour
 * \brief Represente un carrefour.
 *
 * Un carrefour est caracterise par 25 croisements.
 * 5 croisements sont "inutiles" car ne croisent aucune autre voie (N°2, 10, 12, 14 et 22), mais gardes par soucis de simplicite d'affichage.
 */
typedef struct Carrefour {
	Croisement croisements[25];
} Carrefour;

extern int tailleReq;	/*!< La taille d'une requete. */
extern int tailleRep;	/*!< La taille d'une reponse. */

extern int msg_serveur;	/*!< La file de message utilisee pour communiquer entre le serveur et les carrefour. */
extern int msg_carrefour[];	/*!< Les files de message utilisees pour communiquer entre les voitures et les carrefour (4 files). */
extern int carrefours[];	/*!< Les 4 carrefours accessibles par les autres processus en tant que segments de memoire partagee. */
extern int sem;	/*!< L'identifiant de l'ensemble de semaphores (1 semaphore). */
extern int compteur;	/*!< Le compteur de voitures sorties, accessible par les autres processus en tant que segment de memoire partagee. */

#endif
