#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#ifdef LOCAL
#define debug(format, ...) { printf("[%s",#__VA_ARGS__);printf("] : ") ; printf(format"\n",__VA_ARGS__);}
#else
#define debug(...) 42
#endif

#define MAX 1024

/* structure d'un enregistrement */
typedef struct Tenreg {
//   char delim = "$";	// 1 byte + 3 padding
   int cle;             // 4 bytes -> 3 caracteres
   int supp;            // 4 byte -> 1 caractere (booleen)
   char info[MAX - 7];  // (MAX - 7) bytes (7 = taille(cle) + taille(supp) + taille(supp))  
   // la taille du buffer dans la MC : 4 + 4 + 4 + (MAX - 7) =  bytes
   // la taille du buffer dans le fichier : 3 + 3 + 1 + MAX - 7 = MAX bytes
} Tenreg;

typedef char Semi[MAX + 1]; // l'enregistrement sous forme de chaine de caracteres

/* structure d'un bloc */
typedef struct Tbloc {
   char tab[MAX + 1];       // un tableau d'enregistrements d'une capacite MAX 
   int NB;              // nombre d'enregistrements dans tab
   // la taille du bloc est : 844 bytes (fixe)
} Buffer;


// structure de l'entete
typedef struct Entete {
   int nb;              // nombre de blocs utilises (4 bytes)
   int pos;             // la position libre au dernier bloc (4 bytes)
   int cpt_inser;       // compteur d'insertions (4 bytes) 
   int cpt_supp;        // compteur de suppression (4 bytes)
   // la taille de l'entete est : 16 bytes (fixe)
} Entete;

/* structure du fichier TOVC (fichier vu comme tableau, ordonne avec enregistrements de taille variable 
 avec chevauchement)*/
typedef struct TOVC {
   FILE *fich;          // 12 bytes (8 bytes + 4 bytes du padding)
   Entete entete;       // 12 bytes
   // la taille de la structure du fichier est : 24 bytes
} TOVC;

// variables generales
Buffer buf;
TOVC *F;
// procedure pour lire un buffer du fichier
void LireDir(TOVC *F, int i , Buffer *buf) {
   fseek(F->fich, sizeof(Entete) + (i - 1) * sizeof(Buffer), SEEK_SET);
   fread(buf, sizeof(Buffer), 1, F->fich);
}

// procedure pour afficher un bloc
void Afficher_Bloc(Buffer buf, int i) {
   printf("+------------+\n");
   char nb[5];
   sprintf(nb, "% 4d", i);
   printf("|  Bloc %s |\n", nb);
   char s[70] = "+----------------------------------------------------------------+\n";
   printf("%s", s);
   int count = 0;
   const int LINE = 64;
//   LireDir(F, i, &buf);
   while (count < MAX) {
      printf("|");
      for (int i = 0; i < LINE; i++) {
         printf("%c", buf.tab[count++]);
      }
      printf("|\n");
   }
   printf("%s", s);
   printf("Le nombre d'enregistrements : %d\n", buf.NB);

}


//char *recuperer_se(TOVC *F, int i, int j) {
void recuperer_se(Buffer buf,  int j, Semi se) {
   sprintf(se, "%s", "");	// initialser en chaine vide
//   LireDir(F, i, &buf);

	if (j > buf.NB) {	// enregistrement introuvable
		printf("Enregistrement non disponible dans ce bloc\n");
		exit(1);
	} else {	// recherche de l'enregistrement dans le bloc
		char *pos = strchr(buf.tab , '#') ;	
		int index = 0;
		int count = 0;
		while ((pos != NULL) && count < j) {
			index = (pos - buf.tab) + 1;
			pos = strchr(buf.tab + index, '#'); 
			count++;
			debug("%d %d", count, index);
		}
		debug("%d", index);
		int m = index;
		int n = 0;
		while (buf.tab[m] != '#') {
			se[n] = buf.tab[m];
			n++; m++;
		}
		se[n] = '\0';
		debug("%d", n);
	}
}

int main () {
/*
	 Tenreg e;
	 debug("%li", sizeof(Tenreg));
	 e.taille = 13;
	 e.cle = 1;
	 e.supp = 0;
	 strcpy(e.info, "Nassim NEDJAR");
	 debug("%i %i %i %s", e.taille, e.cle, e.supp, e.info);

	 Semi se = "";
	 enreg_semi(e, &se);
	 debug("%i %i %i %s", taille_semi(se), cle_semi(se), supp_semi(se), info_semi(se));

	 Tenreg *en = malloc(sizeof(Tenreg));
	 semi_enreg(en, se);
	 debug("%i %i %i %s", en->taille, en->cle, en->supp, en->info);
	 free(en);
	 rintf("\n");

	 Entete ent;
	 ent.nb = 100;
	 ent.pos = 100;
	 ent.cpt_inser = 100;
	 ent.cpt_supp = 100;
	 afficher_entete(ent);

	 printf("\n");
*/	 
	 Buffer buf;
	 buf.NB = 10;
	 strcpy(buf.tab, "nJyGkYn#uiVbXpWewuS#hdZAMxAuka#dRTOXlNH$z#RrZCrMwbsGNTLAIcdcPDn#KJaEGqPfLthZLPxeFYovdIWgIOHOSmynxaicDZBXILdBXZXcUhWpaclpjFnXya$VLRtAtdPhewwgMYtJBRIGcnltolXraUNSLICftjgPIFUgtYccleGcIamcyxSZsXCpyybcPrcnDIEaUWUAIRLMbagEWfFpgZDcRmTsphqpsqAxjIWYelWNlpAzWPpgHaZWZqnIlLawIzyCBqGZEKZWpsPLFbcQZleCHelkzVzifkagvOJEoDjHLRbcmEsgwNevjPUjPpiKFxgjJqgESwHrohbEyPoHAyvUbbTkKkXCPYerCYZREbsjhGxUCZahuNAYQovoNVkHqMBjPMVmLqawWHZOJgRpkRzCNpInCIKAoKuYKbiFzWkLgexZkXOGXbRgfynSpwWZCDedSTSKJCrzBWCjaOJzpoetHIvExlUgNzQHsGLdqlTxYnMRvsDHzLtmRaiOrpOceBlMayGnXjqFiLhdQnlNQBpBDAYcgntLIxOHAyBupNVUijCrEOFAKEFcbTAwZLpzayihtoEpQdEMbmnfhXSPckSHaxNTLItEZRkkoIkIWFxejLbnirOXkSSaesTbErAnQiLKYnPgwsYqKqjnlCsOwsQCLitCONoBfoNDJzGBEhHhfTdqjsfbMXFuGwGpeScbBZWliGBOtpxEFZvHLAIqJgZvuvlkDMIvJXPnuXogsSiwoCfSrtAhnPLeNFGDFYcHfCaOnXFRJFauIIMGetxybejXgRTHmZVmeYPOSZSbMUbjydRVlkIFkGGnKgORGjNVQZQGL$LYYxTYJfJgWKLsdMSutRsBptCVyLYQtAXEvhynFQzAootQeejsUEXObmQPayXTtzqXiTAQdkAyjrMIUzgpORyyJBqwzIresoeengKWvDhBfUGPHUesMafSphslUPlmU$QqKsyXlOaIKVyWpeZqzckohRTsCqjLVARqabHyzywXWgiTwKQb");
	 Afficher_Bloc(buf, 4);
	 Semi se;
	 sprintf(se, "%s", ""); 
	 recuperer_se(buf, 4, se);
	 printf("%s", se);
	 printf("\nTime elapsed : %.3f s.\n",1.0 * clock() /CLOCKS_PER_SEC);
	 return 0;
}
