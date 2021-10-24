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
   int nb;           // nombre d'enregistrements dans tab (<= b)  (4 bytes)
   // la taille du bloc est : 844 bytes (fixe toujours!!)
} Buffer;

// structure de l'entete
typedef struct Entete {
   int nb;      // nombre de blocs utilises (4 bytes)
   int cpt;     // compteur d'insertions (4 bytes) 
   // la taille de l'entete est : 8 bytes (fixe)
} Entete;

// structure du fichier TObarVC (fichier vu comme tableau, ordonne avec enregistrements de taille fixe)
typedef struct TObarVC {
   FILE *fich;    // 8 bytes 
   Entete entete;    // 8 bytes
   // la taille de la structure du fichier est : 16 bytes (fixe)
} TObarVC;


TObarVC *F;
Buffer buf, bu1, buf2;

// La machine abstraite d'un fichier TObarVC
// procedure pour  modifier l'entete
void Aff_entete(TObarVC *F,int i , int valeur) {  
	switch (i) {
		case 1: {
			F->entete.nb = valeur;
			break;
		}
		case 2: {
			F->entete.cpt = valeur;
			break;
		}
	}
}

// fonction retourne un champ de l'entete
int entete(TObarVC *F, int i) {
	switch (i) {
		case 1: {
			return(F->entete.nb);
			break;
		}
		case 2: {
			return (F->entete.cpt);
			break;
		}
	}
} 

// procedure pour afficher l'entete d'un fichier TObarVC
void Afficher_Entete(TObarVC *F) {
	printf("\nL'entete : \n");
	printf("Le nombre de blocs utilises est : %i\n", entete(F, 1));
	printf("Le nombre d'enregistrements inseres est : %i\n", entete(F, 2));
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
void LireDir(TObarVC *F, int i , Buffer buf) {
	fseek(F->fich, sizeof(Entete) + (i - 1) * sizeof(Buffer), SEEK_SET);
	fread(&buf, sizeof(Buffer), 1, F->fich);
	rewind(F->fich);
}

// procedure pour ecrire un buffer dans fichier
void EcrireDir(TObarVC *F, int i, Buffer buf) {
	fseek(F->fich, sizeof(Entete) + (i - 1) * sizeof(Buffer), SEEK_SET);
	fwrite(&buf, sizeof(Buffer), 1, F->fich);
}
	

// procedure pour allouer un bloc dans un fichier TObarVC
void Alloc_Bloc(TObarVC *F) { 
	Buffer *buf = (Buffer*) calloc(b, sizeof(Buffer));
//	LireDir(fichier, entete(fichier, 1) + 1, buf);
	fseek(F->fich, sizeof(Entete) + (entete(F, 1)) * sizeof(Buffer), SEEK_SET);
	EcrireDir(F, entete(F, 1), *buf);
	Aff_entete(F, 1, entete(F, 1) + 1);
}

// procedure pour afficher un bloc dans un fichier TObarVC
void Afficher_Bloc(TObarVC *F,int i) {
	Tenreg enreg;
	LireDir(F, i, buf);
	for (int i = 0; i < buf.nb; i++) {
		printf("L'enregistrement : %i\n", enreg.cle);
		printf("Le nom : %s\n", enreg.nom);
		printf("Le prenom : %s\n", enreg.prenom);
	}
}


// fonction pour ouvrir un fichier
TObarVC *Ouvrir(char *nom_fichier, char mode) {
	F = (TObarVC*) malloc(sizeof(TObarVC));
	if ((mode == 'a') || (mode == 'A')) {
		F->fich = fopen(nom_fichier, "rb+");
		fread(&(F->entete), sizeof(Entete), 1, F->fich);
	} else if ((mode == 'n') || (mode == 'N')) {
		F->fich = fopen(nom_fichier, "wb+");
		Aff_entete(F, 1, 0);
		Aff_entete(F, 2, 0);
		fwrite(&(F->entete), sizeof(Entete), 1, F->fich);
	} else {
		perror("Format d'ouverture non disponible\n");
		exit(1);
	}
	return (F);
}

// procedure pour afficher un fichier TObarVC
void Afficher_Fichier(TObarVC *F) {
	for (int i = 0; i < entete(F, 1); i++) {
		printf("Le bloc : %i \n", i + 1);
		Afficher_Bloc(F, i + 1);
		printf("**********************\n");
	}
}
// fonction pour fermer un fichier
void Fermer(TObarVC *F) {
	rewind(F->fich);
	fwrite(&(F->entete), sizeof(Entete), 1, F->fich);
	rewind(F->fich);
	fclose(F->fich);
}


// procedure de recherche dans un fichier TObarVC
void Recherche_TObarVC(int cle, char *nom_fichier,bool trouve, int *i, int *j) {
	int bi, bs, inf, sup;
	bool stop;
	
	F = Ouvrir(nom_fichier, 'A');
	bi = 1;
	bs = entete(F, 1);
	trouve = stop = false;
	*j = 1;

	while (bi <= bs && !trouve && !stop) {		// recherche externe
		*i = (bi + bs) / 2;	// le bloc du milieu entre bi et bs
		LireDir(F, *i, buf);
		if (cle >= buf.tab[1].cle && cle <= buf.tab[buf.nb].cle) {
			// recherche dichotomique a l'interieur du bloc
			inf = 1;
			sup = buf.nb;
			while (inf <= sup && !trouve) {	// recherche interne
				*j = (inf + sup) / 2;
				if (cle = buf.tab[*j].cle) {
					trouve = true;
				} else {
					if (cle < buf.tab[*j].cle) {
						sup = *j - 1;
					} else {
						inf = *j + 1;
					}
				}
			}
			if (inf > sup) {	// la fin de la recherche interne
				*j = inf;
			}
			stop = true;			
		} else {
			if (cle < buf.tab[1].cle) {
				bs = *i - 1;
			} else {
				bi = *i + 1;
			}
		}
	}
	if (bi > bs) {
		*i = bi;
		*j = 1;	
	}
	Fermer(F);
}


// procedure d'insertion dans un fichier TObarVC
void Insertion_TObarVC(Tenreg e, char *nom_fichier) {
	bool trouve, continu;
	int i, j, k;
	Tenreg  x;
	
	Recherche_TObarVC(e.cle, nom_fichier, trouve, &i, &j);
	if (!trouve) {
		F = Ouvrir(nom_fichier, 'A');
		continu = true;
		while (continu && i <= entete(F, 1)) {	// sans ajouter un bloc
			LireDir(F, i, buf);	
			x = buf.tab[buf.nb];	// sauvegarder le dernier enregistrement 
			k = buf.nb;	//	decalage a l'interieur du buf
			while (k > j) {
				buf.tab[k] = buf.tab[k - 1];
				k--;
			}
			buf.tab[j] = e;
			if (buf.nb < b) {	// si buf n'est pas plein
				buf.nb++;
				buf.tab[buf.nb] = x;
				EcrireDir(F, i, buf);
				continu = false;
			} else {	// si buf est plein
				EcrireDir(F, i, buf);
				i++; 
				j = 1;
				e = x;
			}
		}
		if (i > entete(F, 1)) {	// si on depasse la fin du fichier, on rajoute un nouveau bloc
			buf.tab[1] = e;
			buf.nb = 1;
			EcrireDir(F, i, buf);
			Aff_entete(F, 1, i);	
		}
		Aff_entete(F, 2, entete(F, 2) + 1);	// incrementation du compteur d'insertions
		Fermer(F);
	}
}

// procedure de suppression logique
void Suppression_Logique_TObarVC(char *nom_fichier, int cle) {
	bool trouve;
	int *i, *j;
	
	Recherche_TObarVC(cle, nom_fichier, trouve, i, j);
	if (trouve) {
		F = Ouvrir(nom_fichier, 'A');
		LireDir(F, *i, buf);
		buf.tab[*j].efface = true;
		EcrireDir(F, *i, buf);
		Fermer(F);
	}
}


// procedure de suppression physique

int main () {
	printf("\nTime elapsed : %.3f s.\n",1.0 * clock() /CLOCKS_PER_SEC);
	return 0;
}

