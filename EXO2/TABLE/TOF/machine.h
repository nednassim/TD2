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


#define b 30
// structure d'un enregistrement
typedef struct Tenreg {
   char nom[10];     // 10 * 1 = 10 bytes
   char prenom[10];  // 10 * 1 = 10 bytes
   int cle;          // 4 bytes
   bool efface;      // 1 byte
   // + 3 bytes pour le padding a fin que la taille de l'enreg soit multiple de 4(taille de int)
   // la taille de l'enregistrement est : 28 bytes (fixe)
} Tenreg;

// structure d'un bloc
typedef struct Tbloc {
   Tenreg tab[b];    // un tableau d'enregistrements d'une capacite b (28 * 30 = 840 bytes)
   int nb;           // nombre d'enregistrements dans tab (<= b)  (4 bytes)
   // la taille du bloc est : 844 bytes (fixe toujours!!)
} Tbloc;


// structure de l'entete
typedef struct Entete {
   int nb;      // nombre de bloc utilises (4 bytes)
   int cpt;     // compteur d'insertions (4 bytes) 
   int fact;    // le facteur de chargement (4 bytes)
   // la taille de l'entete est : 12 bytes (fixe)
} Entete;

// structure du fichier TOF (fichier vu comme tableau, ordonne avec enregistrements de taille fixe)
typedef struct TOF {
   FILE *fichier;    // 12 bytes (8 bytes + 4 bytes du padding)
   Entete entete;    // 12 bytes
   // la taille de la structure du fichier est : 24 bytes (fixe)
} TOF;


// La machine abstraite d'un fichier TOF
void Aff_Entete(TOF *fichier,int i , int valeur);	// procedure pour  modifier l'entete
int Entete(TOF *fichier, int i);		// fonction retourne un champ de l'entete
void Afficher_Entete(TOF *fichier);		// procedure pour afficher l'entete d'un fichier TOF
void LireDir(TOF *fichier, int i , Buffer *buf);	// procedure pour lire un buffer du fichier
void EcrireDir(TOF *fichier, int i, Buffer *buf);	// procedure pour ecrire un buffer dans fichier
void Alloc_Bloc(TOF *fichier);	// procedure pour allouer un bloc dans un fichier TOF
void Afficher_Bloc(TOF *fichier,int i);	// procedure pour afficher un bloc dans un fichier TOF
void Creation_Fichier(TOF *fichier,int n);	// procedure pour creer un fichier 
TOF *Ouvrir(char nom_physique[], char mode);		// fonction pour ouvrir un fichier
void Afficher_Fichier(TOF *fichier);	//	procedure pour afficher un fichier TOF
void Fermer(TOF *fichier);		// fonction pour fermer un fichier
void Recherche_TOF(TOF *fichier,int cle,int *trouv,int *i, int *j );// procedure de recherche dans un fichier TOF
void Insertion_TOF(TOF *fichier, int cle, char *info);	// procedure d'insertion dans un fichier TOF
void Suppression_Logique_TOF(TOF *fichier, int cle);	// procedure de suppression logique 
void Suppression_Physique_TOF(TOF *fichier,char *nom_physique); // procedure pour suppression physique
void Afficher_info_chevau(TOF *fichier);	// procedure pour afficher les informations sur le chevauchement
void Reordonner_Fichier(TOF *fichier1,char *nom_physique);	// procedure pour reorganiser un fichier TOF

// procedures sur les chaines de caracteres 
void turn_to_string(char chaine[], int n, int longueur); 
void recuperer_chaine(TOF *fichier,int n , int *i, int *j, char chaine[],Buffer *buf);	
void ecrire_chaine(TOF *fichier,int n , int *i, int *j, char chaine[],int *cpt,Buffer *buf);

