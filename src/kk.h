#define VERSION "0.8"
#define RBTitle "Ketchup Killers Commander V0.8"

#define CALLING __cdecl

#define TOTFIC 2000

#define GAUCHE 0x4B
#define DROITE 0x4D

#define BYTE unsigned char
#define WORD unsigned short
#define ULONG unsigned long

#define ushort unsigned short
#define ulong unsigned long
#define uchar unsigned char

#define DOOR 1

struct kkfichier
     {
     char *menu;                                              // kkc.mnu
     char *ficscreen;                                         // kkc.scr

     char *trash;                                    // repertoire trash
     char *FicIdfFile;                                      // idfext.rb
     char *CfgFile;                                          // kkrb.cfg
     char *temp;                                             // temp.tmp
     char *log;                                               // logfile
     };

extern struct kkfichier *KKFics;

struct PourMask
     {
     char Ignore_Case;                         // 1 si on ignore la case
     char Other_Col;                  // 1 si on colorie les autres noms
     char chaine[1024];  // chaine de comparaison EX: "asm break case @"
     char title[40];                                 // nom de ce masque
     };

extern struct PourMask **Mask;

struct RB_info
     {
     int temps;          // Temps entre le main() et l'appel utilisateur
     };

extern struct RB_info *Info;

struct file
    {
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
     long x,y;                     // position haut droite de la fenˆtre
     long xl,yl;                    // position bas gauche de la fenetre
     long y2;                            // position de name - size ....
     long yl2;                         // Nombre de fichiers par fenˆtre


     long IDFSpeed;      // Nombre de clock avant de lire la description
     long nbrfic;
     long taillefic;
     long nbrsel;
     long taillesel;
     struct file **F;

     long nopcur;                  // Fichier a copier si c'est noprompt
     long pcur;                               // positon dans le tableau
     short scur;                         // positon du curseur … l'‚cran
     long oldscur;                               // dernier scur affiche
     long oldpcur;                               // dernier pcur affiche
     char path[256];               // path complete (disk et repertoire)
                  // La lettre du disque ne peut prendre que 1 caractere
                           //                  et doit etre en majuscule
                 // Le repertoire ne doit pas finir par '/' sauf le ROOT
     short FenTyp;                // Type de fenetre: 0: normal -------
                                                   // 1: info file -----
                                                   // 2: user screen ---
                                                   // 3: info all file -
                                                   // 4: information ---
                                                   // 5: file_id.diz ---
                                                   // 6: change dir ----

     short init;         // si =1 --> r‚initialise fenˆtre (bordure ...)
     struct _fenetre *Fen2;

     // Affichage
     short order;                               // 1: normal, 2: inverse
     short sorting;       // 1: Name, 2: ext, 3: date, 4: size, 0: unsort

     // Systeme
     char system;      // 0: DOS, 1: RAR, 2: ARJ, 3: ZIP, 4: LHA, 5: KKD

     char VolName[255];                                 // Nom du volume
     char nfen;                                  // Numero de la fenetre
                                                   // 0: Celle de gauche
                                                   // 1: Celle de droite

     char KKDdrive;                     // Numero du drive pour les .KKD

     char InfoPos;                       // On commence o— dans l'info ?


} FENETRE;

struct kkconfig
     {
    // Selon user
    //-----------

     char sizewin;     // Taille de la fenˆtre en hauteur (0=max) ------

     char fentype;     // Type de fenˆtre, 1=NC, 2=WATCOM, 3=KKC, 4=Font

     char pntrep;              // vaut 1 si on affiche le repertoire "."
     char hidfil;            // vaut 1 si on affiche les fichiers caches

     char logfile;                    // vaut 1 si on utilise un logfile

     char autoreload;   // Reload auto. quand les 2 fen. sont identiques
     char verifhist;  // Verify history at any loading of KK (CTRL-PGDN)
     char palafter;    // Load the palette only when configuration is ok
     BYTE noprompt;    // Si x&1 vaut 1 alors on ne prompte pdt la copie
     char currentdir;      // Va dans le repertoire courant au demarrage
     char alcddir;       // Va dans le repertoire chang‚ par une applic.

     char dispcolor;    // Highlight les fichiers suivant les extensions

     char insdown;    // vaut 1 si on descent quand on appuie sur insert
     char seldir;     // vaut 1 si on selectionne les repertoires avec +

     long strash;                                       // Size of trash

     char enterkkd;           // entre dans les kkd pendant la recherche

     char editeur[64];               // ligne de commande pour l'editeur
     char vieweur[64];               // ligne de commande pour le viewer
     char ssaver[64];          // ligne de commande pour le screen saver

     char ExtTxt[64],Enable_Txt;
     char ExtBmp[64],Enable_Bmp;
     char ExtSnd[64],Enable_Snd;
     char ExtArc[64],Enable_Arc;
     char ExtExe[64],Enable_Exe;
     char ExtUsr[64],Enable_Usr;

     char Qmenu[48];
     short Nmenu[8];

     char Esc2Close;   // vaut 1 si on doit fermer les fenˆtres avec ESC

     char cnvhist;         // 1: si on convertit a chaque fois l'history
     char esttime;                 // estime le temps pendant la copie ?

     char confexit;      // vaut 1: si on doit confirmer avant de sortir

     char dispath;              // Affiche la path en haut de la fenˆtre

     char pathdown;                       // Affiche la path tout en bas

     char savekey; // Vaut 1 si on sauvegarde les touches dans un buffer

    //--- Variable pour le viewer --------------------------------------

     char warp;                    // 0: pas de warp, 1: word, 2: entier
     char cnvtable;                // Table de conversion pour le viewer
     char autotrad;                  // Traduit automatiquement un table
     char ajustview;                  // Fit the width of file in viewer
     char saveviewpos;                // Save position of file in viewer
     unsigned char wmask;             // C'est quel masque kon emploie ?

     char lnfeed;                // 0: CR/LF  1:CR  2:LF  3:user 4:CR|LF
     char userfeed;                                       // Si lnfeed=3

     long AnsiSpeed;

    //--- Don't look this ----------------------------------------------

     char scrrest;

     long mtrash;                          // taille maximum de la trash
     long FenAct;                         // Quelle fenˆtre est active ?
     char _4dos;                                // equal 1 if 4DOS found
     char _Win95;                                    // Support nom long

     char HistDir[256];                                // History of dir
     char overflow1;

     char HistCom[512];                            // History of command
     char overflow2;                       // Vaut tjs 0 (pour overflow)
     short posinhist;                  // Position in history of command

     char extens[39];              // extension qui viennent tout devant

     short FenTyp[3];                         // Type des fenˆtres SHELL
     short KeyAfterShell;           // Vaut 1 si wait key after dosshell

     short key;                              // code touche a reutiliser
     char FileName[256]; // Nom du dernier fichier select. for F3 on arc
     char crc;                           // Vaut tjs 0x69 (genre de crc)
     };


extern struct kkconfig *KKCfg;

void Shell(char *string,...);
void SaveCfg(void);
int LoadCfg(void);


int ChangeToKKD(void);

void GestionFct(int fct);
char *AccessFile(int n);
char *AccessAbsFile(char *fic);

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
#include "rbdos.h"

#ifdef DEBUG
#define PUTSERR(__Ch) PrintAt(0,0,"DEBUG MODE ERROR: %s",__Ch);  getch()
#else
#define PUTSERR(__Ch)
#endif

#define NBWIN 4

/*--------------------------------------------------------------------*\
|- prototype de kkfct.c                                               -|
\*--------------------------------------------------------------------*/
void DefaultKKCfg(void);
void FileSetup(void);
void SetDefaultKKPath(char *path);

void RemplisVide(void);  // Remplissage du vide pour les plus de 80 col.

