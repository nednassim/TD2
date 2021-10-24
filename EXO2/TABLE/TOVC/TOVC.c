#include "machine.h"


// variables generales
TOVC *F;
Buffer buf;

/* Le modele */
// fonction retourne un champ de l'entete
int entete(TOVC *F, int i) {
	switch (i) {
		case 1: {
			return(F->entete.nb);
			break;
		}
		case 2: {
			return (F->entete.cpt);
			break;
		}
		case 3: {
			return (F->entete.cpt_inser);
			break;
		}
		case 4: {
		  return (F->entete.cpt_supp);
		  break;
		}
	}
} 

// procedure pour  modifier l'entete
void Aff_entete(TOVC *F,int i , int val) {  
	switch (i) {
		case 1: {
			F->entete.nb = val;
			break;
		}
		case 2: {
			F->entete.pos = val;
			break;
		}
		case 3: {
			F->entete.cpt_inser = val;	  
			break;
		}
		case 4: {
			F->entete.cpt_supp = val;
			break;
		}
	}
}

// procedure pour lire un buffer du fichier
void LireDir(TOVC *F, int i , Buffer *buf) {
	fseek(F->fich, sizeof(Entete) + (i - 1) * sizeof(Buffer), SEEK_SET);
	fread(buf, sizeof(Buffer), 1, F->fich);
}

// procedure pour ecrire un buffer dans fichier
void EcrireDir(TOVC *F, int i, Buffer *buf) {
	fseek(F->fich, sizeof(Entete) + (i - 1) * sizeof(Buffer), SEEK_SET);
	fwrite(buf, sizeof(Buffer), 1, F->fich);
}
	

// procedure pour allouer un bloc dans un fichier TOVC
void Alloc_Bloc(TOVC *F) { 
	Buffer *buf = (Buffer*) calloc(MAX, sizeof(Buffer));
//	LireDir(fichier, entete(fichier, 1) + 1, buf);
	fseek(F->fich, sizeof(Entete) + (entete(F, 1)) * sizeof(Buffer), SEEK_SET);
	EcrireDir(F, entete(F, 1), buf);
	Aff_entete(F, 1, entete(F, 1) + 1);
}

// fonction pour ouvrir un fichier
TOVC *Ouvrir(char *nom_fichier, char mode) {
	F = (TOVC*) malloc(sizeof(TOVC));
	if ((mode == 'a') || (mode == 'A')) {
		F->fich = fopen(nom_fichier, "rb+");
		if (F->fich != NULL) {
			rewind(F->fich);
			fread(&(F->entete), sizeof(Entete), 1, F->fich);
		} else {
			perror("Fichier mal ouvert \n");
			exit(1);
		}
	} else if ((mode == 'n') || (mode == 'N')) {
		F->fich = fopen(nom_fichier, "wb+");
		if (F->fich != NULL) {
			Aff_entete(F, 1, 0);
			Aff_entete(F, 2, 0);
			Aff_entete(F, 3, 0);
			Aff_entete(F, 4, 0);
			rewind(F->fich);
			fwrite(&(F->entete), sizeof(Entete), 1, F->fich);
		} else {
			perror("Fichier mal cree \n");
			exit(1);
		}
	} else {
		perror("Format d'ouverture non disponible\n");
		exit(1);
	}
	return (F);
}

// fonction pour fermer un fichier
void Fermer(TOVC *F) {
	rewind(F->fich);
	fwrite(&(F->entete), sizeof(Entete), 1, F->fich);
	fclose(F->fich);
	free(F);
}

// transforme un int num en un string s de longueur max la difference est comblée par des zéros 
void num_chaine(int num, int max, char * s) {
    char s_num[4]; // num est sur 3 positions
    sprintf(s_num, "%d", num);
    int j = max - strlen(s_num) ;
    sprintf(s, "%s", "");
    while (j--) {
		 sprintf(s,"%s0",s);
	 }
    sprintf(s, "%s%s", s, s_num);
}

//  copier de la chaine s a partir de la position i une sous chaine r de longueur max */
void copier_chaine(char *s, int i, int max, char *r) {
   sprintf(r,"%s","");
   while (i < strlen(s) && max > 0) {
       sprintf(r, "%s%c", r, s[i]);
       i++;
       max--;
   }
}

// supprimer une partie de la chaine s de longueur max à partir de la position i 
void couper_chaine(char *s, int i, int max) {
   char part_droite[MAX + 1];
   char part_gauche[MAX + 1];
   copier_chaine(s, 0, i, part_gauche);
   copier_chaine(s, i + max, strlen(s), part_droite);
	sprintf(s,"%s%s",part_gauche, part_droite);
}

// transformer une chaine en un enregistrement
void semi_to_enreg (Chaine se, Tnreg *e) {
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

// transformer un enregistrement en un semi_enregistrement 
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
	for (k = 0; k < (int atoi(taille)) + 4; k++) {
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



/*	La manipulation du fichier */
// procedure de recherche dans un fichier TOVC
void Recherche_TOVC(TOVC *F,int cle, int *i, int *j, int *trouve) {
	*trouve = 0;
	*i = 1;
  	*j = 0;
	int i1 = 1, j1 = 0;
	Semi se;
	Tenreg e;
	if (entete(F, 1) > 0) {		// le fichier n'est pas vide
		while (!(*trouve) && (*i) <= entete(F, 1)) {	
			sprintf(se, "%s", "");				// initialiser en chaine vide
			recuperer_se(F, &i1, &j1, se);	// recuperer l'enregisrement et se positionner sur le prochain
			semi_enreg(se, &e);					// enreg -> chaine
			if (!e.supp && e.cle >= cle) {	//	la cle trouvee ou inexistante
				break;
			}
			if (!(*trouve)) {		// positionner sur le prochain enregistrement
				(*i) = i1;
				(*j) = j1;
			}
			if (((*i) == entete(F, 1)) && (*j) == entete(F, 2)) {	 // la fin sans trouver la cle
				break;
			}
		}
		if ((!e.supp) && (e.cle == cle)) {	//	la cle trouvee
			(*trouve) = 1;
		}
	}
}

// procedure d'insertion dans un fichier TOVC
void Insertion_TOVC(Tenreg e, char *nom_fichier) {
	bool trouve, continu;
	int i, j, k;
	Tenreg  x;
	
	Recherche_TOVC(e.cle, nom_fichier, trouve, &i, &j);
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
void Suppression_Logique_TOVC(TOVC *F, int cle) {
	int i, j, trouve;
	bool trouve;
	char taille[4];
	
	Recherche_TOVC(cle, nom_fichier, trouve, i, j);
	if (trouve) {
		F = Ouvrir(nom_fichier, 'A');
		LireDir(F, *i, buf);
		buf.tab[*j].efface = true;
		EcrireDir(F, *i, buf);
		Fermer(F);
	}
}


// procedure de suppression physique
//void Suppression_Physique_TOVC(TOVC* fichier, char *nom_fichier) 

		
/*	L'affichage */
int main () {
	printf("\nTime elapsed : %.3f s.\n",1.0 * clock() /CLOCKS_PER_SEC);
	return 0;
}

