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

// structure du fichier TOF (fichier vu comme tableau, ordonne avec enregistrements de taille fixe)
typedef struct TOF {
   FILE *fich;    // 8 bytes 
   Entete entete;    // 8 bytes
   // la taille de la structure du fichier est : 16 bytes (fixe)
} TOF;


TOF *F;
Buffer buf;

// La machine abstraite d'un fichier TOF
// procedure pour  modifier l'entete
void Aff_entete(TOF *F,int i , int valeur) {  
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
int entete(TOF *F, int i) {
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

// procedure pour afficher l'entete d'un fichier TOF
void Afficher_Entete(TOF *F) {
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
void LireDir(TOF *F, int i , Buffer buf) {
	fseek(F->fich, sizeof(Entete) + (i - 1) * sizeof(Buffer), SEEK_SET);
	fread(&buf, sizeof(Buffer), 1, F->fich);
	rewind(F->fich);
}

// procedure pour ecrire un buffer dans fichier
void EcrireDir(TOF *F, int i, Buffer buf) {
	fseek(F->fich, sizeof(Entete) + (i - 1) * sizeof(Buffer), SEEK_SET);
	fwrite(&buf, sizeof(Buffer), 1, F->fich);
}
	

// procedure pour allouer un bloc dans un fichier TOF
void Alloc_Bloc(TOF *F) { 
	Buffer *buf = (Buffer*) calloc(b, sizeof(Buffer));
//	LireDir(fichier, entete(fichier, 1) + 1, buf);
	fseek(F->fich, sizeof(Entete) + (entete(F, 1)) * sizeof(Buffer), SEEK_SET);
	EcrireDir(F, entete(F, 1), *buf);
	Aff_entete(F, 1, entete(F, 1) + 1);
}

// procedure pour afficher un bloc dans un fichier TOF
void Afficher_Bloc(TOF *F,int i) {
	Tenreg enreg;
	LireDir(F, i, buf);
	for (int i = 0; i < buf.nb; i++) {
		printf("L'enregistrement : %i\n", enreg.cle);
		printf("Le nom : %s\n", enreg.nom);
		printf("Le prenom : %s\n", enreg.prenom);
	}
}


// fonction pour ouvrir un fichier
TOF *Ouvrir(char *nom_fichier, char mode) {
	F = (TOF*) malloc(sizeof(TOF));
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

// procedure pour afficher un fichier TOF
void Afficher_Fichier(TOF *F) {
	for (int i = 0; i < entete(F, 1); i++) {
		printf("Le bloc : %i \n", i + 1);
		Afficher_Bloc(F, i + 1);
		printf("**********************\n");
	}
}
// fonction pour fermer un fichier
void Fermer(TOF *F) {
	rewind(F->fich);
	fwrite(&(F->entete), sizeof(Entete), 1, F->fich);
	rewind(F->fich);
	fclose(F->fich);
}


// procedure de recherche dans un fichier TOF
void Recherche_TOF(int cle, char *nom_fichier,bool trouve, int *i, int *j) {
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


// procedure d'insertion dans un fichier TOF
void Insertion_TOF(Tenreg e, char *nom_fichier) {
	bool trouve, continu;
	int i, j, k;
	Tenreg  x;
	
	Recherche_TOF(e.cle, nom_fichier, trouve, &i, &j);
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
void Suppression_Logique_TOF(char *nom_fichier, int cle) {
	bool trouve;
	int *i, *j;
	
	Recherche_TOF(cle, nom_fichier, trouve, i, j);
	if (trouve) {
		F = Ouvrir(nom_fichier, 'A');
		LireDir(F, *i, buf);
		buf.tab[*j].efface = true;
		EcrireDir(F, *i, buf);
		Fermer(F);
	}
}


// procedure de suppression physique
//void Suppression_Physique_TOF(TOF* fichier, char *nom_fichier) 

void Chargement_Initial_TOF(char *nom_fichier, int n, long u) {
	Tenreg e;
	int i, j, k;

	F = Ouvrir(nom_fichier, 'N');
	i = 1;
	j = 1;
	printf("Donner les enregistrements en ordre croissant suivant la cle :\n");
	for (k  = 1; k < n; k++) {
		e = Lire();
		if (j <= u * b) {
			buf.tab[j] = e;
			j++;
		} else {
			buf.nb = j - 1;
			EcrireDir(F, i, buf);
			buf.tab[i] = e;
			i++;
			j = 2;
		}
	}
	buf.nb = j - 1;
	EcrireDir(F, i, buf);
	Aff_entete(F, 1, i);
	Aff_entete(F, 2, n);
	Fermer(F);
}

// procedure pour reorganiser un fichier TOF
void Reordonner_Fichier(TOF *F1,char *nom_fichier) {
}

void Fusion(char *nom_fichier1, char *nom_fichier2, char *nom_fichier3) {
	TOF *F1, *F2, *F3;
	int i1, i2, i3;
	int j1, j2, j3;
	int N;
	bool continu;
	Tenreg e, e1, e2;
	Buffer buf1, buf2, buf3;
	int i, j, indice;

	F1 = Ouvrir(nom_fichier1, 'A');
	F2 = Ouvrir(nom_fichier2, 'A');
	F3 = Ouvrir(nom_fichier3, 'N');
	
	i1 = i2 = i3 = 1;
	j1 = j2 = j3 = 1;

	LireDir(F1, 1, buf1);
	LireDir(F2, 1, buf2);
	continu = true;

	while (continu) {
		if (j <= buf1.nb && j2 <= buf2.nb) {
			e1 = buf1.tab[j1];
			e2 = buf2.tab[j2];
			if (e1.cle <= e2.cle) {
				e = e1;
				j1++;
			} else {
				e = e2;
				j2++;
			}
			if (j3 <= b) {
				buf3.tab[j3] = e;
				j3++;
			} else {
				buf3.nb = j3 - 1;
				EcrireDir(F3, i3, buf3);
				i3++;
				buf3.tab[1] = e;
				j3 = 2;
			}				
		} else {
			if (j1 > buf1.nb) {
				if (i1 < entete(F1, 1)) {
					i1++;
					LireDir(F1, i1, buf1);
					j1 = 1;
				} else {
					continu = false;
					i = i2;
					j = j2;
					N = entete(F2, 1);
					buf = buf2;
					indice = 2;
				}
			} else {
				if (i1 < entete(F2, 1)) {
					i2++;
					LireDir(F1, i2, buf2);
					j2 = 1;
				} else {
					continu = false;
					i = i1;
					j = j1;
					N = entete(F1, 1);
					buf = buf1;
					indice = 1;		
				}
			}
		}
	}
	continu = true;
	while (continu) {
		if (j <= buf.nb) {
			if (j3 <= b) {
				buf3.tab[j3] = buf.tab[j];
				j3++;
			} else {
				buf3.nb = j3 - 1;
				EcrireDir(F3, i3, buf3);
				i3++;
				buf3.tab[1] = buf.tab[j];
				j3 = 2;
			}
			j++;
		} else {
			if (i <= N) {
				i++;
				if (indice == 1) {
					LireDir(F1, i, buf);
				} else {
					LireDir(F2, i, buf);
				}
				j = 1;
			} else {
				continu = false;
			}
		}
	}
	buf3.nb = j3 - 1;
	EcrireDir(F3, i3, buf3);
	Aff_entete(F3, 1, i3);
	Aff_entete(F3, 2, entete(F1, 1) + entete(F2, 1));

	Fermer(F1);
	Fermer(F2);
	Fermer(F3);
}


int main () {
	printf("\nTime elapsed : %.3f s.\n",1.0 * clock() /CLOCKS_PER_SEC);
	return 0;
}

