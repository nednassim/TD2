#pragma GCC optimize("-Ofast","-funroll-all-loops","-ffast-math")
#pragma GCC optimize("-fno-math-errno")
#pragma GCC optimize("-funsafe-math-optimizations")
#pragma GCC optimize("-freciprocal-math")
#pragma GCC optimize("-fno-trapping-math")
#pragma GCC optimize("-ffinite-math-only")
#pragma GCC optimize("-fno-stack-protector")
#pragma GCC target("sse4","avx2")
#include <immintrin.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>


#ifdef LOCAL
#define debug(format, ...) { printf("[%s",#__VA_ARGS__);printf("] : ") ; printf(format"\n",__VA_ARGS__);}
#else
#define debuf(...) 42
#endif

#define MAX 30

/* structure d'un enregistrement */
typedef struct Tenreg {
	int longueur;			// 4 bytes -> 3 caracteres
   int cle; 	         // 4 bytes -> 3 caracteres
   int supp;   	  		// 4 byte -> 3 caracteres (booleen)
	char info[MAX - 6];	// (MAX - 6) bytes (6 = taille(cle) + taille(supp))  
	// la taille de l'enregistrement est variable
} Tenreg;

typedef char Semi[MAX];

/* structure d'un bloc */
typedef struct Tbloc {
   char tab[MAX]; 	   // un tableau d'enregistrements d'une capacite MAX (28 * 30 = 840 bytes)
   int NB;          		// nombre d'enregistrements dans tab (<= b)  (4 bytes)
   // la taille du bloc est : 844 bytes (fixe)
} Tbloc;


// structure de l'entete
typedef struct Entete {
   int nb;     			// nombre de blocs utilises (4 bytes)
   int pos;     			// la position libre au dernier bloc (4 bytes)
   int cpt_inser;       // compteur d'insertions (4 bytes) 
   int cpt_supp;      	// compteur de suppression (4 bytes)
   // la taille de l'entete est : 16 bytes (fixe)
} Entete;

/* structure du fichier TOVC (fichier vu comme tableau, ordonne avec enregistrements de taille variable 
 avec chevauchement)*/
typedef struct TOVC {
   FILE *fich; 		   // 12 bytes (8 bytes + 4 bytes du padding)
   Entete entete;  	   // 12 bytes
   // la taille de la structure du fichier est : 24 bytes
} TOVC;



/* Le modele */
int entete(TOVC *fichier, int i);		// fonction retourne un champ de l'entete
void Aff_Entete(TOVC *fichier,int i , int val);	// procedure pour  modifier l'entete
void LireDir(TOVC *F, int i , Buffer *buf);	// procedure pour lire un buffer du fichier
void EcrireDir(TOVC *F, int i, Buffer buf);	// procedure pour ecrire un buffer dans fichier
void Alloc_Bloc(TOVC *F);	// procedure pour allouer un bloc dans un fichier TOVC
TOVC *Ouvrir(char nom_fichier[20], char mode);		// fonction pour ouvrir un fichier
void Fermer(TOVC *fichier);		// fonction pour fermer un fichier

/* La manipulation du fichier*/
void Recherche_TOVC(TOVC *fichier, int cle, int *i, int *j , int *trouve);// procedure de recherche dans un fichier TOVC
void Insertion_TOVC(TOVC *fichier, int cle, char *info);	// procedure d'insertion dans un fichier TOVC
void Suppression_Logique_TOVC(TOVC *fichier, int cle);	// procedure de suppression logique 
void Suppression_Physique_TOVC(TOVC *fichier,char nom_fichier[20]); // procedure pour suppression physique
void Reorganisation_TOVC(TOVC *fichier,char nom_fichier[20]);	// procedure pour reorganiser un fichier TOVC

// procedures sur les chaines de caracteres 
void num_chaine(int num, int max, char *s);
void copier_chaine(char *s, int i, int max, char *r);
void couper_chaine(char *s, int i, int max);
void recuperer_chaine(TOVC *fichier,int n , int *i, int *j, Semi se);	
void ecrire_chaine(TOVC *fichier,int n , int *i, int *j, Semi se);
void semi_enreg(Tenreg *e, Semi semi );
void enreg_semi(Tenreg e, Semi semi);

/* L'affichage */
void Afficher_Bloc(TOVC *fichier,int i);	// procedure pour afficher un bloc dans un fichier TOVC
void Afficher_Entete(TOVC *fichier);		// procedure pour afficher l'entete d'un fichier TOVC
void Afficher__chevauchement(TOVC *fichier);	// procedure pour afficher les informations sur le chevauchement
void Afficher_Fichier(TOVC *fichier);	//	procedure pour afficher un fichier TOVC
