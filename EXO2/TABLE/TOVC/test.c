#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define MAX 30

/* structure d'un enregistrement */
typedef struct Tenreg {
   int longueur;        // 4 bytes -> 3 caracteres
   int cle;             // 4 bytes -> 3 caracteres
   int supp;            // 4 byte -> 3 caracteres (booleen)
   char info[MAX - 6];  // (MAX - 6) bytes (6 = taille(cle) + taille(supp))  
   // la taille de l'enregistrement est variable
} Tenreg;

typedef char Semi[MAX];

/* structure d'un bloc */
typedef struct Tbloc {
   char tab[MAX];       // un tableau d'enregistrements d'une capacite MAX (28 * 30 = 840 bytes)
   int NB;              // nombre d'enregistrements dans tab (<= b)  (4 bytes)
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

Buffer buf;
void LireDir(TOVC *F, int i , Buffer *buf) {
   fseek(F->fich, sizeof(Entete) + (i - 1) * sizeof(Buffer), SEEK_SET);
   fread(buf, sizeof(Buffer), 1, F->fich);
}


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

void copier_chaine(char *s, int i, int max, char *r) {
  sprintf(r,"%s","");
  while (i < strlen(s) && max > 0) {
      sprintf(r, "%s%c", r, s[i]);
      i++;
      max--;
  }
}

void couper_chaine(char *s, int i, int max) {
  char part_droite[MAX + 1];
  char part_gauche[MAX + 1];
  copier_chaine(s, 0, i, part_gauche);
  copier_chaine(s, i + max, strlen(s), part_droite);
  sprintf(s,"%s%s",part_gauche, part_droite);
}

void semi_to_enreg (Semi se, Tenreg *e) {
   char chaine[MAX + 1];
   // la cle
   sprintf(chaine,"%s", "");
   copier_chaine(se, 3, 3, chaine);
   e->cle = atoi(chaine);
   // le booleen
   sprintf(chaine, "%c", se[6]);
   e->supp = atoi(chaine);
   // l'info
   copier_chaine(se, 7, strlen(se) - 7, e->info);
}

void enreg_to_semi (Tenreg e, Semi se) {
   char chaine[4];
   int taille = strlen(e.info);

   sprintf(se,"%s","");
   // ecriture de la taille de l'info dans le semi enregistrement
   num_chaine(taille, 3, chaine);
   sprintf(se,"%s%s", se, chaine);
   // écriture de la clé dans le semi enregistrement
   num_chaine(e.cle, 3, chaine);
   sprintf(se,"%s%s", se, chaine);
   // ecriture du booleen supp
   num_chaine(e.supp, 1, chaine);
   sprintf(se,"%s%s", se, chaine);
   // ecriture de l'info
   sprintf(se,"%s%s", se, e.info);
}

void recuperer_se(TOVC *F, int *i, int *j, Semi se) {
  char taille[4];
  int k;

  sprintf(taille, "%s", "");
  sprintf(se, "%s", "");
  LireDir(F, (*i), &buf);

  for (k = 0; k < 3; k++) {
     if ((*j) < MAX) {
        sprintf(taille, "%s%c", taille, buf.tab[*j]);
        sprintf(se, "%s%c", se, buf.tab[*j]);
        (*j)++;
     } else {
        (*i)++;
        LireDir(F, (*i), &buf);
        sprintf(taille, "%s%c", taille, buf.tab[0]);
        sprintf(se, "%s%c", se, buf.tab[0]);
        (*j) = 1;
     }
  }
  for (k = 0; k < (atoi(taille)) + 4; k++) {
     if ((*j) < MAX) {
        sprintf(se, "%s%c", se, buf.tab[*j]);
        (*j)++;
     } else {
        (*i)++;
        LireDir(F, (*i), &buf);
        sprintf(se, "%s%c", se, buf.tab[0]);
        (*j) = 1;
     }
  }
}

int main () {

 	 return 0; 
}
