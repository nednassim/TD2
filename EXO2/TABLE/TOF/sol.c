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
#define debug(...) 42
#endif

#define b 30
// structure d'un enregistrement
typedef struct Tenreg {
   char nom[10];  	// 10 * 1 = 10 bytes
   char prenom[10];  // 10 * 1 = 10 bytes
   int cle;   		  	// 4 bytes
   bool efface;   	// 1 bytes
   // + 3 bytes pour le padding a fin que la taille de l'enreg soit multiple de 4(taille de int)
   // la taille de l'enregistrement est : 28 bytes
} Tenreg;

// structure d'un bloc
typedef struct Tbloc {
   Tenreg tab[b];    // un tableau d'enregistrements d'une capacite b (28 * 30 = 840 bytes)
   int nb;   			// nombre d'enregistrements dans tab (<= b)  (4 bytes)
   // la taille du bloc est : 844 bytes
} Buffer;
 

// structure de l'entete
typedef struct Entete {
   int nb;      // nombre de bloc utilises (4 byets)
   int cpt;   	 // compteur d'insertions (4 bytes) 
   // la taille de l'entete est : 12 byets
} Entete;

// structure du fichier TOF
typedef struct TOF {
   FILE *fichier;		// 12 bytes (8 bytes + 4 bytes du padding)
   Entete entete;		// 12 bytes
	// la taille de la structure du fichier est : 24 bytes
} TOF;



int main () {
	Tenreg * enreg = malloc(sizeof(Tenreg));
	strcpy(enreg->prenom,"Nassim");
	strcpy(enreg->nom, "NEDJAR");
	enreg->cle = 1;
	enreg->efface = 0;
//	printf("%li   %li   %li   %li\n", sizeof(enreg->nom), sizeof(enreg->prenom), sizeof(enreg->cle),sizeof( enreg->efface));
	printf("la taille de l'enregistrement : %li\n", sizeof(*enreg));
/*	printf("la taille du bloc : %li\n", sizeof(Buffer));

	printf("la taille de l'entete : %li\n", sizeof(Entete));
	printf("la taille du fichier  : %li\n", sizeof(TOF));
	
*/
	printf("\nTime elapsed : %.3f s.\n",1.0 * clock() /CLOCKS_PER_SEC);
	return 0;
}
