#define VERSION "0.23"

#define ENTIER short

#define CALLING __cdecl

#define TOTFIC 1000

#define GAUCHE 0x4B
#define DROITE 0x4D

#include "driver.h"

struct file {
    char *name;
    char attrib;
    char select;
    int size;
    unsigned short time;
    unsigned short date;
    char *info;
};


void Shell(char *string,...);

void AffFen(struct fenetre *Fen);
void SortFic(struct fenetre *Fen);

void InfoSupport(void);


struct fenetre {
     ENTIER IDFSpeed;   // Nombre de clock avant de lire la description
     long nbrfic;
     long taillefic;
     long nbrsel;
     long taillesel;
     char actif;
     struct file **F;
     ENTIER x,y;        // position haut droite de la fenˆtre
     ENTIER xl,yl;      // taille de la fenˆtre
     ENTIER x2,y2;      // position calculee
     ENTIER xl2,yl2;    // taille calculee
     ENTIER x3,y3;      // position de l'infoselect
     ENTIER pcur,paff;  // positon dans le tableau
     ENTIER scur,saff;  // positon du curseur … l'‚cran
     char path[256];    // path complete (disk et repertoire)
                        // La lettre du disque ne peut prendre que 1 caractere
                        //                  et doit etre en majuscule
                        // Le repertoire ne doit pas finir par '/' sauf le ROOT
     ENTIER FenTyp;    // Type de fenetre: 0:norm, 1:diz
     ENTIER init;      // si =1 --> r‚initialise fenˆtre (bordure ...)
     struct fenetre *Fen2;

     // Affichage
     ENTIER order;        // 1: normal, 2: inverse
     ENTIER sorting;      // 1: Name, 2: ext, 3: date, 4: size, 0: unsort

     // Systeme
     ENTIER system;      // 0: DOS, 1: RAR, 2: ARJ, 3: ZIP, 4: LHA, 5: KKD

     char VolName[255];  // Nom du volume

};

extern struct fenetre *DFen;

extern char *Screen_Buffer;

extern int IOver;
extern int IOerr;

#include "ficidf.h"
#include "hard.h"
#include "gestion.h"

#include "copie.h"
#include "delete.h"

#include "search.h"

#include "help.h"

#include "view.h"

#ifdef DEBUG
#define PUTSERR(__Chaine) PrintAt(0,0,__Chaine); getch()
#else
#define PUTSERR(__Chaine)
#endif
