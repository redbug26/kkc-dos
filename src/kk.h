#define VERSION "0.6"
#define RBTitle "Ketchup Killers Commander V0.6"

#define CALLING __cdecl

#define TOTFIC 2000

#define GAUCHE 0x4B
#define DROITE 0x4D

#define BYTE unsigned char
#define WORD unsigned short
#define ULONG unsigned long


struct file {
    char *name;
    char attrib;
    char select;
    int size;
    unsigned short time;
    unsigned short date;
    char *info;
    char desc;
};



typedef struct _fenetre
     {
     short x,y;                    // position haut droite de la fenˆtre
     short xl,yl;                                // taille de la fenˆtre
     short x2,y2;                                   // position calculee
     short xl2,yl2;                                   // taille calculee
     short x3,y3;                            // position de l'infoselect

     short IDFSpeed;     // Nombre de clock avant de lire la description
     long nbrfic;
     long taillefic;
     long nbrsel;
     long taillesel;
     char actif;
     struct file **F;

     short nopcur;                 // Fichier a copier si c'est noprompt
     short pcur;                              // positon dans le tableau
     short scur;                         // positon du curseur … l'‚cran
     short oldscur;                              // dernier scur affiche
     short oldpcur;                              // dernier pcur affiche
     char path[256];               // path complete (disk et repertoire)
                  // La lettre du disque ne peut prendre que 1 caractere
                           //                  et doit etre en majuscule
                 // Le repertoire ne doit pas finir par '/' sauf le ROOT
     short FenTyp;                     // Type de fenetre: 0:norm, 1:diz
     short init;         // si =1 --> r‚initialise fenˆtre (bordure ...)
     struct _fenetre *Fen2;

     // Affichage
     short order;                               // 1: normal, 2: inverse
     short sorting;      // 1: Name, 2: ext, 3: date, 4: size, 0: unsort

     // Systeme
     short system;     // 0: DOS, 1: RAR, 2: ARJ, 3: ZIP, 4: LHA, 5: KKD

     char VolName[255];                                 // Nom du volume
     char nfen;                                  // Numero de la fenetre

     char KKDdrive;                     // Numero du drive pour les .KKD

} FENETRE;


void Shell(char *string,...);
void SaveCfg(void);
int LoadCfg(void);
void AffFen(FENETRE *Fen);

int ChangeToKKD(void);

void GestionFct(int fct);
char *AccessFile(int n);

extern FENETRE *DFen;
extern char *Screen_Buffer;
extern int IOver;        // Vaut 1 si on veut pas faire de verifications
extern int IOerr;

#include "ficidf.h"
#include "hard.h"
#include "gestion.h"
#include "copie.h"
#include "delete.h"
#include "search.h"
#include "view.h"
#include "edit.h"
#include "util.h"
#include "reddies.h"
#include "driver.h"
#include "win.h"
#include "longname.h"

#ifdef DEBUG
#define PUTSERR(__Ch) PrintAt(0,0,"DEBUG MODE ERROR: %s",__Ch);  getch()
#else
#define PUTSERR(__Ch)
#endif

#define NBWIN 4


