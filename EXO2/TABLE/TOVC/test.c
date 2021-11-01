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
   int taille;        // 4 bytes -> 3 caracteres
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

/* le semi-enregistrement */
// fonction pour recuperer la taille d'un semi
int taille_semi(Semi se) {
	 char taille[4] = "";
	 strncpy(taille, se , 3);
	 return (atoi(taille));
}
// fonction pour recuperer la cle d'un semi
int cle_semi(Semi se) {
	 char cle[4] = "";
	 strncpy(cle, &se[3] , 3);
	 return (atoi(cle));
}
// fonction pour recuperer le supp d'un semi
int supp_semi(Semi se) {
	 char supp = '\0';
	 supp = se[6];
	 return (supp - '0');
}
// fonction pour recuperer l'info d'un semi
char *info_semi(Semi se) {
	 char *info = calloc(taille_semi(se) + 1, sizeof(char));
	 sprintf(info,"%s", "");
	 debug("%i", taille_semi(se));
	 strncpy(info, &se[7] , taille_semi(se));
	 return (info);
}

/* procedure de conversion */
// procedure pour convertir un entier en chaine de caracteres 
void num_chaine(int num, int max, char *s) {
	 char s_num[4]; // num est sur 3 positions
	 sprintf(s_num, "%d", num);
	 int j = max - strlen(s_num) ;
	 sprintf(s, "%s", "");
	 while (j--) {
		sprintf(s,"%s0",s);
	 }
	 sprintf(s, "%s%s", s, s_num);
}
// procedure pour convertir un enregistrement en semi-enregistrement
void enreg_semi (Tenreg e, Semi *se) {
    char chaine[4];
    int taille = strlen(e.info);
               
    sprintf(*se,"%s","");
    // ecrire la taille de l'info dans le semi-enregistrement
    num_chaine(taille, 3, chaine);
    sprintf(*se,"%s%s", *se, chaine);  // concatener l
    // écriture de la clé dans le semi enregistrement
    num_chaine(e.cle, 3, chaine);
    sprintf(*se,"%s%s", *se, chaine);
    // ecriture du booleen supp
    num_chaine(e.supp, 1, chaine);
    sprintf(*se,"%s%s", *se, chaine);
    // ecriture de l'info
    sprintf(*se,"%s%s", *se, e.info);
}
// procedure pour convertir un semi-enregistrement en enregistrement
void semi_enreg(Tenreg *e, Semi se) {
	 e->taille = taille_semi(se);
	 e->cle = cle_semi(se);
	 e->supp = supp_semi(se);
	 strcpy(e->info, info_semi(se));
}

 

// procedure pour afficher un bloc
void afficher_bloc(Buffer buf, int i) {
	printf("+------------+\n");
	char nb[5];
	sprintf(nb, "% 4d", i);
	printf("|  Bloc %s |\n", nb);
	char s[70] = "+----------------------------------------------------------------+\n";
	printf("%s", s);
	int count = 0;
	const int LINE = 64;
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


// procedure pour afficher l'entete d'un fichier
void afficher_entete(Entete e) {
	char s[50] = "+----------+----------+-----------+----------+\n";
	char nb[5];
	sprintf(nb, "% 4d", e.nb);
	char pos[5];
	sprintf(pos, "% 4d", e.pos);
	char cpt_inser[5];
	sprintf(cpt_inser, "% 4d", e.cpt_inser);
	char cpt_supp[5];
	sprintf(cpt_supp, "% 4d", e.cpt_supp);
	printf("Entete\n");
	printf("%s", s);
	printf("|    nb    |    pos   | cpt_inser | cpt_supp |\n");
	printf("%s", s);
	printf("|	   |	      |		  |	     |\n");
	printf("|  %s	   |  %s    |  %s 	  |  %s    |\n", nb, pos, cpt_inser, cpt_supp);
	printf("|	   |	      |		  |	     |\n");
	printf("%s", s);
	printf("|__ nb : le nombre de blocs utilises\n");
	printf("|__ pos : la position libre au dernier bloc\n");
	printf("|__ cpt_inser : le compteur d'insertions\n");
	printf("|__ cpt_supp : le compteur de suppression\n");
}

int main () {
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
	 printf("\n");

	 Entete ent;
	 ent.nb = 100;
	 ent.pos = 100;
	 ent.cpt_inser = 100;
	 ent.cpt_supp = 100;
	 afficher_entete(ent);

	 printf("\n");
	 Buffer buf;
	 buf.NB = 10;
	 strcpy(buf.tab, "nJyGkYnKuiVbXpWewuSfhdZAMxAukaxdRTOXlNHjznRrZCrMwbsGNTLAIcdcPDnUKJaEGqPfLthZLPxeFYovdIWgIOHOSmynxaicDZBXILdBXZXcUhWpaclpjFnXyarVLRtAtdPhewwgMYtJBRIGcnltolXraUNSLICftjgPIFUgtYccleGcIamcyxSZsXCpyybcPrcnDIEaUWUAIRLMbagEWfFpgZDcRmTsphqpsqAxjIWYelWNlpAzWPpgHaZWZqnIlLawIzyCBqGZEKZWpsPLFbcQZleCHelkzVzifkagvOJEoDjHLRbcmEsgwNevjPUjPpiKFxgjJqgESwHrohbEyPoHAyvUbbTkKkXCPYerCYZREbsjhGxUCZahuNAYQovoNVkHqMBjPMVmLqawWHZOJgRpkRzCNpInCIKAoKuYKbiFzWkLgexZkXOGXbRgfynSpwWZCDedSTSKJCrzBWCjaOJzpoetHIvExlUgNzQHsGLdqlTxYnMRvsDHzLtmRaiOrpOceBlMayGnXjqFiLhdQnlNQBpBDAYcgntLIxOHAyBupNVUijCrEOFAKEFcbTAwZLpzayihtoEpQdEMbmnfhXSPckSHaxNTLItEZRkkoIkIWFxejLbnirOXkSSaesTbErAnQiLKYnPgwsYqKqjnlCsOwsQCLitCONoBfoNDJzGBEhHhfTdqjsfbMXFuGwGpeScbBZWliGBOtpxEFZvHLAIqJgZvuvlkDMIvJXPnuXogsSiwoCfSrtAhnPLeNFGDFYcHfCaOnXFRJFauIIMGetxybejXgRTHmZVmeYPOSZSbMUbjydRVlkIFkGGnKgORGjNVQZQGLjLYYxTYJfJgWKLsdMSutRsBptCVyLYQtAXEvhynFQzAootQeejsUEXObmQPayXTtzqXiTAQdkAyjrMIUzgpORyyJBqwzIresoeengKWvDhBfUGPHUesMafSphslUPlmUoQqKsyXlOaIKVyWpeZqzckohRTsCqjLVARqabHyzywXWgiTwKQb");
	 afficher_bloc(buf, 4);
	 printf("\nTime elapsed : %.3f s.\n",1.0 * clock() /CLOCKS_PER_SEC);
	 return 0;
}
