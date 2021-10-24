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
#include <stdbool.h>
#include <time.h>


#ifdef LOCAL
#define debug(format, ...) { printf("[%s",#__VA_ARGS__);printf("] : ") ; printf(format"\n",__VA_ARGS__);}
#else
#define debug(...) 42
#endif

#define b 30	// capacite maximale des blocs(nombre d'enregistrements)

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
   int NB;           // nombre d'enregistrements dans tab (<= b)  (4 bytes)
   // la taille du bloc est : 844 bytes (fixe toujours!!)
} Buffer;

// structure de l'entete
typedef struct Entete {
   int nb;      // nombre de blocs utilises (4 bytes) (l'adresse du dernier bloc)
   // la taille de l'entete est : 4 bytes (fixe)
} Entete;

// structure du fichier TObarF (fichier vu comme tableau, ordonne avec enregistrements de taille fixe)
typedef struct TObarF {
   FILE *fich;    // 8 bytes 
   Entete entete;    // 8 bytes
   // la taille de la structure du fichier est : 16 bytes (fixe)
} TObarF;


TObarF *F;
Buffer buf, buf1, buf2, buf3;	// les zones tampons en MC

// La machine abstraite d'un fichier TObarF
// procedure pour  modifier l'entete
void Aff_entete(TObarF *F, int valeur) {  
	F->entete.nb = valeur;
}

// fonction retourne un champ de l'entete
int entete(TObarF *F) {
	return(F->entete.nb);
} 

// procedure pour afficher l'entete d'un fichier TObarF
void Afficher_Entete(TObarF *F) {
	printf("\nL'entete : \n");
	printf("Le nombre de blocs utilises est : %i\n", entete(F));
}

// procedure pour lire un enregistrement
Tenreg Lire() {
	Tenreg enreg;
	printf("Entrer le nom : \n");
	scanf("%s", enreg.nom);
	printf("Entrer le prenom :\n");
	scanf("%s", enreg.prenom);
	printf("Entrer la cle :\n");
	scanf("%i", &(enreg.cle));
	return enreg;
}


// procedure pour lire un buffer du fichier
void LireDir(TObarF *F, int i , Buffer buf) {
	fseek(F->fich, sizeof(Entete) + (i - 1) * sizeof(Buffer), SEEK_SET);
	fread(&buf, sizeof(Buffer), 1, F->fich);
	rewind(F->fich);
}

// procedure pour ecrire un buffer dans fichier
void EcrireDir(TObarF *F, int i, Buffer buf) {
	fseek(F->fich, sizeof(Entete) + (i - 1) * sizeof(Buffer), SEEK_SET);
	fwrite(&buf, sizeof(Buffer), 1, F->fich);
}
	

// procedure pour allouer un bloc dans un fichier TObarF
void Alloc_Bloc(TObarF *F) { 
	Buffer *buf = (Buffer*) calloc(b, sizeof(Buffer));
//	LireDir(fichier, entete(fichier, 1) + 1, buf);
	fseek(F->fich, sizeof(Entete) + entete(F) * sizeof(Buffer), SEEK_SET);
	EcrireDir(F, entete(F), *buf);
	Aff_entete(F, entete(F) + 1);
}

// procedure pour afficher un bloc dans un fichier TObarF
void Afficher_Bloc(TObarF *F,int i) {
	Tenreg enreg;
	LireDir(F, i, buf);
	for (int i = 0; i < buf.NB; i++) {
		printf("L'enregistrement : %i\n", enreg.cle);
		printf("Le nom : %s\n", enreg.nom);
		printf("Le prenom : %s\n", enreg.prenom);
	}
}


// fonction pour ouvrir un fichier
TObarF *Ouvrir(char *nom_fichier, char mode) {
	TObarF *F = malloc(sizeof(TObarF));
	if ((mode == 'a') || (mode == 'A')) {
		F->fich = fopen(nom_fichier, "rb+");
		fread(&(F->entete), sizeof(Entete), 1, F->fich);
	} else if ((mode == 'n') || (mode == 'N')) {
		F->fich = fopen(nom_fichier, "wb+");
		Aff_entete(F, 1);
		Aff_entete(F, 2);
		fwrite(&(F->entete), sizeof(Entete), 1, F->fich);
	} else {
		perror("Format d'ouverture non disponible\n");
		exit(1);
	}
	return (F);
}

// procedure pour afficher un fichier TObarF
void Afficher_Fichier(TObarF *F) {
	for (int i = 0; i < entete(F); i++) {
		printf("Le bloc : %i \n", i + 1);
		Afficher_Bloc(F, i + 1);
		printf("**********************\n");
	}
}
// fonction pour fermer un fichier
void Fermer(TObarF *F) {
	rewind(F->fich);
	fwrite(&(F->entete), sizeof(Entete), 1, F->fich);
	rewind(F->fich);
	fclose(F->fich);
}


// procedure de recherche dans un fichier TObarF
void Recherche_TObarF(int cle, char *nom_fichier,bool trouve, int *i, int *j) {
	F = Ouvrir(nom_fichier, 'A');
	int N = entete(F);
	*i = 1; *j = 1;
	trouve = false;
	while (i <= N && !trouve) {
		LireDir(F, *i, buf);
		j = 1;
		while (j <= buf.NB && !trouve) {
			if (cle == buf.tab[*j].cle) {
				trouve = true;
			} else {
				j++;
			}
		}
	}
	Fermer(F);
}

// procedure d'insertion dans un fichier TObarF
void Insertion_TObarF(Tenreg e, char *nom_fichier) {
	int i, j;
	bool trouve;
	Recherche_TObarF(e.cle, nom_fichier, trouve, &i, &j);
	if (!trouve) {
		F = Ouvrir(nom_fichier, 'A');
		int N = entete(F);
		if (N) {		// si le fichier n'est pas vide
			LireDir(F, N, buf);	// lire le dernier bloc
		} else {		// si le fichier est vide
			N = 1;	// initialiser le premier bloc
			Aff_entete(F, 1);	 
			buf.NB = 0;	  // a vide
		}
		if (buf.NB < b) {		// place disponible dans ce bloc
			buf.NB++;	// inserer e dans ce bloc
			buf.tab[buf.NB] = e;
			EcrireDir(F, N, buf);
		} else {		// le dernier bloc est plein
			buf.NB = 1;		// inserer e dans un bloc N + 1
			buf.tab[1] = e;
			EcrireDir(F, N + 1, buf);
			Aff_entete(F, N + 1);	// mettre a jour l'entete
		}
		Fermer(F);
	}
}

void Suppression_Physique_TObarF(int cle, char *nom_fichier) {
	int i, j;
	bool trouve;
	Recherche_TObarF(cle, nom_fichier, trouve, &i, &j);
	if (trouve) {
		F = Ouvrir(nom_fichier, 'A');
		int N = entete(F);
		if (i != N) {
			LireDir(F, i, buf1);
			LireDir(F, N, buf2);
			buf.tab[j] = buf2.tab[buf2.NB];
			buf2.NB--;
			EcrireDir(F, i, buf1);
			if (buf2.NB > 0) {
				EcrireDir(F, N, buf2);
			} else {
				Aff_entete(F, N - 1);
			}
		} else {
			LireDir(F, N, buf);
			buf.tab[j] = buf.tab[buf.NB];
			buf.NB--;
			if (buf.NB > 0) {
				EcrireDir(F, N, buf);
			} else {
				Aff_entete(F, N - 1);
			}
		}
	}
}

int main () {
	printf("\nTime elapsed : %.3f s.\n",1.0 * clock() /CLOCKS_PER_SEC);
	return 0;
}

