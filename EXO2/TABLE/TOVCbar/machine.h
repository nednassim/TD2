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
   // la taille de l'enregistrement est : 28 bytes
} Tenreg;

// structure d'un bloc
typedef struct Tbloc {
   Tenreg tab[b];    // un tableau d'enregistrements d'une capacite b (28 * 30 = 840 bytes)
   int nb;           // nombre d'enregistrements dans tab (<= b)  (4 bytes)
   // la taille du bloc est : 844 bytes
} Tbloc;


// structure de l'entete
typedef struct Entete {
   int nb;      // nombre de bloc utilises (4 bytes)
   int cpt;     // compteur d'insertions (4 bytes) 
   int fact;    // le facteur de chargement (4 bytes)
   // la taille de l'entete est : 12 bytes
} Entete;

// structure du fichier TOVCbar (fichier vu comme tableau, ordonne avec enregistrements de taille variable
// sans chevauchement)
typedef struct TOVCbar {
   FILE *fichier;    // 12 bytes (8 bytes + 4 bytes du padding)
   Entete entete;    // 12 bytes
   // la taille de la structure du fichier est : 24 bytes
} TOVCbar;



// La machine abstraite d'un fichier TOVCbar
void Aff_Entete(TOVCbar *fichier,int i , int valeur);	// procedure pour  modifier l'entete
int Entete(TOVCbar *fichier, int i);		// fonction retourne un champ de l'entete
void Afficher_Entete(TOVCbar *fichier);		// procedure pour afficher l'entete d'un fichier TOVCbar
void LireDir(TOVCbar *fichier, int i , Buffer *buf);	// procedure pour lire un buffer du fichier
void EcrireDir(TOVCbar *fichier, int i, Buffer *buf);	// procedure pour ecrire un buffer dans fichier
void Alloc_Bloc(TOVCbar *fichier);	// procedure pour allouer un bloc dans un fichier TOVCbar
void Afficher_Bloc(TOVCbar *fichier,int i);	// procedure pour afficher un bloc dans un fichier TOVCbar
void Creation_Fichier(TOVCbar *fichier,int n);	// procedure pour creer un fichier 
TOVCbar *Ouvrir(char nom_physique[], char mode);		// fonction pour ouvrir un fichier
void Afficher_Fichier(TOVCbar *fichier);	//	procedure pour afficher un fichier TOVCbar
void Fermer(TOVCbar *fichier);		// fonction pour fermer un fichier
void Recherche_TOVCbar(TOVCbar *fichier,int cle,int *trouv,int *i, int *j );// procedure de recherche dans un fichier TOVCbar
void Insertion_TOVCbar(TOVCbar *fichier, int cle, char *info);	// procedure d'insertion dans un fichier TOVCbar
void Suppression_Logique_TOVCbar(TOVCbar *fichier, int cle);	// procedure de suppression logique 
void Suppression_Physique_TOVCbar(TOVCbar *fichier,char *nom_physique); // procedure pour suppression physique
void Afficher_info_chevau(TOVCbar *fichier);	// procedure pour afficher les informations sur le chevauchement
void Reordonner_Fichier(TOVCbar *fichier1,char *nom_physique);	// procedure pour reorganiser un fichier TOVCbar

// procedures sur les chaines de caracteres 
void turn_to_string(char chaine[], int n, int longueur); 
void recuperer_chaine(TOVCbar *fichier,int n , int *i, int *j, char chaine[],Buffer *buf);	
void ecrire_chaine(TOVCbar *fichier,int n , int *i, int *j, char chaine[],int *cpt,Buffer *buf);

