#define VERSION "0.91"
#define RBTitle "Ketchup Killers Commander V0.91"

#define CALLING __cdecl

#define TOTFIC 2000

#define MAXPATH 256

#define GAUCHE 0x4B
#define DROITE 0x4D

#define BYTE unsigned char
#define WORD unsigned short
#define ULONG unsigned long

#define ushort unsigned short
#define ulong unsigned long
#define uchar unsigned char

#define DOOR 1

#include "view.h"

#define NBWIN 4


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

struct RB_info
     {
     int temps;          // Temps entre le main() et l'appel utilisateur
     char macro;
     char *sbuf0;
     char *sbuf1;
     char cbuf0;
     char cbuf1;
     };

extern struct RB_info *Info;

/*--------------------------------------------------------------------*\
|- Gestion macros (fake in kksetup)                                   -|
\*--------------------------------------------------------------------*/

extern short FctStack[128];
extern short NbrFunct;

void InitFctStack(void);
void PutLIFOFct(int fct);
int GetLIFOFct(void);


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

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
     short FenTyp;                 // Type de fenetre: 0: normal -------
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

     char ChangeLine;    // Vaut 1 si on doit r‚afficher la command line


} FENETRE;

struct kkconfig
     {
    // Selon user
    //-----------
     char userfont; // vaut 1 si on charge les fonts dans la trash --16
     char sizewin;  // Taille de la fenˆtre en hauteur (0=max) ------10
     char fentype; // Type de fenˆtre, 1=NC, 2=WATCOM, 3=KKC, 4=Font-03
     char pntrep;          // vaut 1 si on affiche le repertoire "."-17
     char hidfil;        // vaut 1 si on affiche les fichiers caches-18
     char logfile;                // vaut 1 si on utilise un logfile-19
     char autoreload; // Reload auto quand les 2 fen sont identiques-20
     char verifhist;          // Verify history at any loading of KK-21
     char palafter;          // Load the palette only when cfg is ok-22
     char currentdir;  // Va dans le repertoire courant au demarrage-24
     char alcddir;   // Va dans le repertoire chang‚ par une applic.-25
     char dispcolor;    // Highlight les fichiers svt les extensions-13
     char insdown;   // = 1 si on descent quand on appuie sur insert-26
     char seldir;    // = 1 si on selectionne les repertoires avec +-27
     char enterkkd;       // entre dans les kkd pendant la recherche-29
     char editeur[64];           // ligne de commande pour l'editeur-30
     char vieweur[64];           // ligne de commande pour le viewer-31
     char ssaver[64];      // ligne de commande pour le screen saver-32
     char ExtTxt[64],Enable_Txt;
     char ExtBmp[64],Enable_Bmp;
     char ExtSnd[64],Enable_Snd;
     char ExtArc[64],Enable_Arc;
     char ExtExe[64],Enable_Exe;
     char ExtUsr[64],Enable_Usr;
     char Qmenu[48];
     short Nmenu[8];
     char isbar;  // vaut 1 si on affiche la barre en bas -----------12
     char isidf;  // vaut 1 si on affiche la barre en haut ----------11
     char Esc2Close;  // = 1 si on doit fermer les fenˆtres avec ESC-07
     char cnvhist;     // 1: si on convertit a chaque fois l'history-34
     char esttime;             // estime le temps pendant la copie ?-35
     char confexit;  // vaut 1: si on doit confirmer avant de sortir-36
     char dispath;          // Affiche la path en haut de la fenˆtre-09
     char pathdown;                   // Affiche la path tout en bas-08
     char savekey; // =1 si on sauvegarde les touches dans un buffer-37
     char KeyAfterShell;        // Vaut 1 si wait key after dosshell-38
     char addselect; //=1 on rajoute une ligne select dans le pannel-39
     long mtrash;                      // taille maximum de la trash-40

     char lift; //--- Ascensceur ------------------------------------45

    //--- Variable pour le viewer --------------------------------------

     KKVIEW V;

    //--- Don't look this ----------------------------------------------

     BYTE noprompt; // Si x&1 vaut 1 alors on ne prompt pdt la copie----
     char cmdline; //---------------------------------------------------
     long strash;                               // Actual Size of trash-

     char scrrest;

     long FenAct;                         // Quelle fenˆtre est active ?
     char _4dos;                                // equal 1 if 4DOS found
     char _Win95;                                    // Support nom long

     char HistDir[256];                                // History of dir
     char overflow1;

     char HistCom[512];                            // History of command
     char overflow2;                       // Vaut tjs 0 (pour overflow)
     short posinhist;                  // Position in history of command

     char extens[39];              // extension qui viennent tout devant

     short FenTyp[NBWIN];                     // Type des fenˆtres SHELL

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
#include "edit.h"
#include "util.h"
#include "reddies.h"
#include "driver.h"
#include "win.h"
#include "rbdos.h"

#ifdef DEBUG
#define PUTSERR(__Ch) PrintAt(0,0,"DEBUG MODE ERROR: %s",__Ch);  getch()
#else
#define PUTSERR(__Ch)
#endif



/*--------------------------------------------------------------------*\
|- prototype de kkfct.c                                               -|
\*--------------------------------------------------------------------*/
void DefaultKKCfg(void);
void FileSetup(void);
void SetDefaultKKPath(char *path);

void RemplisVide(void);  // Remplissage du vide pour les plus de 80 col.

void Console(void);

/*--------------------------------------------------------------------*\
|- prototype de macro.c                                               -|
\*--------------------------------------------------------------------*/
void RunMacro(char*);
void RunAutoMacro(void);
