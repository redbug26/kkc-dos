/*--------------------------------------------------------------------*\
|- Programme principal                                                -|
\*--------------------------------------------------------------------*/
#include <direct.h>
#include <i86.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <bios.h>
#include <io.h>
#include <graph.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include <signal.h>                               // For handling signal

#include <dos.h>                            //  Pour redirection des I/O

#include "kk.h"

#include "idf.h"
#include "win.h"


/*--------------------------------------------------------------------*\
|-  Declaration interne                                               -|
\*--------------------------------------------------------------------*/

void Fin(void);

void WinSelect(FENETRE *Fen);
void SaveSel(FENETRE *F1);
void LoadSel(int n);

void AffFen(FENETRE *Fen);

void SelectPlus(void);
void SelectMoins(void);
void CreateKKD(void);
void EditFile(char *s);
void CreateDirectory(void);
void WinCD(void);
void HistDir(void);
void WinRename(FENETRE *F1);
void ChangeType(char n);
void ChangeDrive(void);
void EditNewFile(void);
int EnterArchive(void);
void SwapWin(long a,long b);
void SwapLong(long *a,long *b);
void HistCom(void);
void _4DOSShistdir(void);
void AffLonger(void);

/*--------------------------------------------------------------------*\
|-  Declaration des variables                                         -|
\*--------------------------------------------------------------------*/

long OldCol;                                // Ancienne couleur du texte
long OldY,OldX,PosX,PosY;


sig_atomic_t signal_count;

char *SpecMessy=NULL;
char SpecSortie[256];
char saveconfig=1;
//char placedrive=1;
//char placepath=1;

char Select_Chaine[32]="*.*";

char *Screen_Buffer;

char *Keyboard_Flag1=(char*)0x417;

char *ShellAdr=(char*)0xBA000;

char *Screen_Adr=(char*)0xB8000;

FENETRE *Fenetre[NBWIN];

FENETRE *DFen;

extern int IOver;
extern int IOerr;

struct kkconfig *KKCfg;
struct PourMask **Mask;
struct kkfichier *KKFics;

/*--------------------------------------------------------------------*\
|-  Procedure en Assembleur                                           -|
\*--------------------------------------------------------------------*/

char GetDriveReady(int i);
#pragma aux GetDriveReady = \
    "mov ah,19h" \
    "int 21h" \
    "mov ch,al" \
    "mov ah,0Eh" \
    "int 21h" \
    "mov ah,19h" \
    "int 21h" \
    "sub al,dl" \
    "mov cl,al" \
    "mov dl,ch" \
    "mov ah,0Eh" \
    "int 21h" \
    modify [eax ebx ecx edx] \
    parm [edx] \
    value [cl];

/*--------------------------------------------------------------------*\
|- Gestion des selections                                             -|
\*--------------------------------------------------------------------*/
void WinSelect(FENETRE *Fen)
{
FILE *fic;
int i,n;
static char CadreLength=70;
static char Dir[70];
static int DirLength=70;
char nom[256];

struct Tmt T[] =
    { { 2,3,1, Dir, &DirLength},
      {15,5,5,"    LOAD     ",NULL},
      {45,5,5,"     SAVE    ",NULL},
      { 2,2,0,"Would you load/save selection in",NULL},
      { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,5,74,12,"Load/Save Selection"};

strcpy(Dir,Fen->path);
Path2Abs(Dir,"KKSELECT.TMP");

n=WinTraite(T,5,&F,1);

if (n==27) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

if (n==1)
    {
    fic=fopen(Dir,"rt");
    if (fic==NULL)
        {
        WinError("File don't exist");
        return;
        }
    while(!feof(fic))
        {
        if (fgets(nom,256,fic)==NULL) break;
        nom[strlen(nom)-1]=0;

        for(i=0;i<Fen->nbrfic;i++)
            if (!WildCmp(Fen->F[i]->name,nom))
                FicSelect(i,1);                                // Select
        }
    fclose(fic);
    }

if (n==2)
    {
    fic=fopen(Dir,"wt");
    if (fic==NULL)
        {
        WinError("File couldn't be created");
        return;
        }
    for(i=0;i<Fen->nbrfic;i++)
        {
        if ((Fen->F[i]->select)==1)
            fprintf(fic,"%s\n",Fen->F[i]->name);
        }
    fclose(fic);
    }


}



/*--------------------------------------------------------------------*\
|-  Other Procedures                                                  -|
|-   c=0 --> change from selection                                    -|
|-     1 --> default set of color                                     -|
|-     2 --> norton set                                               -|
\*--------------------------------------------------------------------*/
void ChangePalette(char c)
{
int x,y,i;
char rec;

int n,m,ntot;
int nt,mt;
int car;

int ex,ey;
int x1,y1;

char *titre="Palette configuration";

#define NBRS 4

char defcol[NBRS][48]={ RBPALDEF ,
                     { 0, 0, 0, 42,42,42,  0, 0, 0, 63,63,63,
                      63,63,63, 63,63,32, 42,63,63,  0, 0,43,
                      63,63, 0, 63,63,63,  0, 0,43, 57,63, 0,
                      30,60,30,  0,40,63,  0, 0, 0,  0, 0, 0},
                     {25,36,29, 36,18,15,  0, 0, 0, 49,39,45,
                      44,63,63, 42,37,63, 45,39,35,  0, 0, 0,
                       0,63,63, 63,63,63, 25,36,29, 63, 0, 0,
                       0,63, 0,  0, 0,63,  0, 0, 0,  0, 0, 0},
                     {42,37,63, 14,22,17,  0, 0, 0, 58,58,50,
                      18, 1,36, 63,63,21, 58,42,49, 16,16,32,
                      63,63, 0, 63,63,63, 43,37,63, 63,20,20,
                      20,40,20,  0,40,40,  0, 0, 0,  0, 0, 0}
                       };

char *Style[NBRS]={"Default Style","Norton Style","Cyan Style",
                                                             "Venus "};
int posx[NBRS],posy[NBRS];

if (c>0)
    {
    memcpy(Cfg->palette,defcol[c-1],48);
    LoadPal();
    return;
    }

SaveScreen();
PutCur(32,0);

RemplisVide();

WinCadre(0,0,79,(Cfg->TailleY)-2,2);
Window(1,1,78,(Cfg->TailleY)-3,10*16+1);

if (Cfg->TailleY==50)
    {
    x1=9;
    y1=7;
    ey=7;
    ex=24;
    for (n=0;n<NBRS;n++)
        {
        posx[n]=56;
        posy[n]=n*3+24;
        }
    }
    else
    {
    x1=4;
    y1=2;
    ey=4;
    ex=19;
    for (n=0;n<NBRS;n++)
        {
        posx[n]=n*19+4;
        posy[n]=(Cfg->TailleY)-4;
        }
    }



ntot=15+NBRS;

for(n=0;n<ntot-15;n++)
    {
    WinCadre(posx[n]-2,posy[n]-1,posx[n]+15,posy[n]+1,2);
    ColLin(posx[n],posy[n],strlen(Style[n]),10*16+1);
    PrintAt(posx[n],posy[n],Style[n]);
    }

if (Cfg->TailleY==50)
    {
    WinCadre(2,2,77,4,2);
    WinCadre(2,5,77,44,2);
    PrintAt(30,3,titre);

    WinCadre(2,45,77,47,2);

    WinCadre(52,16,73,40,2);
    PrintAt(5,46,"%s /RedBug",RBTitle);

    WinCadre(53,17,72,21,2);
    PrintAt(54,19,"Predefined palette");
    }
    else
    {
    PrintAt(30,0,titre);
    }


n=0;
m=0;


i=0;

rec=1;

do
{
if (rec==1)
    {
    rec=0;
    for (nt=0;nt<16;nt++)
        {
        x=(nt/ey)*ex+x1;
        y=(nt%ey)*5+y1;

        WinCadre(x-1,y-1,x+9,y+3,1);
        Window(x,y,x+8,y+2,1*16+5);

        WinCadre(x+10,y-1,x+14,y+3,1);
        ColWin(x+11,y,x+13,y+2,nt*16+nt);
        ChrWin(x+11,y,x+13,y+2,220);

        for(mt=0;mt<3;mt++)
            {
            Gradue(x,y+mt,8,0,Cfg->palette[nt*3+mt],64);
            ColWin(x,y+mt,x+8,y+mt,1*16+2);
            PrintAt(x-3,y+mt,"%02d",Cfg->palette[nt*3+mt]);
            }
        }
    }

if (n<16)
    {
    i=Cfg->palette[n*3+m];
    if (i!=0) i--;

    x=(n/ey)*ex+x1;
    y=(n%ey)*5+y1;

    Gradue(x,y+m,8,i,Cfg->palette[n*3+m],64);
    PrintAt(x-3,y+m,"%02d",Cfg->palette[n*3+m]);

    ColWin(x,y+m,x+8,y+m,1*16+5);

    car=Wait(0,0,0);

    ColWin(x,y+m,x+8,y+m,1*16+2);

    switch(HI(car))
        {
        case 0x47:                                               // HOME
            ChrWin(x,y+m,x+8,y+m,32);
            Cfg->palette[n*3+m]=0;
            break;
        case 0X4F:                                                // END
            ChrWin(x,y+m,x+8,y+m,32);
            Cfg->palette[n*3+m]=63;
            break;
        case 80:                                                  // bas
            m++;
            break;
        case 72:                                                 // haut
            m--;
            break;
        case 0x4B:                                             // gauche
            if (Cfg->palette[n*3+m]!=0)
                Cfg->palette[n*3+m]--;
            break;
        case 0x4D:                                             // droite
            if (Cfg->palette[n*3+m]!=63)
                Cfg->palette[n*3+m]++;
            break;
        case 0xF:                                           // SHIFT-TAB
            n--;
            break;
        }

    switch(LO(car))
        {
        case 9:
            n++;
            break;
        }

    if (m==3) n++,m=0;
    if (m<0)  n--,m=2;
    if (n<0)  n=ntot;

    LoadPal();
    }
    else
    {
    ColLin(posx[n-16],posy[n-16],strlen(Style[n-16]),10*16+5);

    car=Wait(0,0,0);

    ColLin(posx[n-16],posy[n-16],strlen(Style[n-16]),10*16+1);

    switch(HI(car))
        {
        case 80:                                                  // bas
            n++;
            break;
        case 72:                                                 // haut
            n--;
            break;
        case 0xF:                                           // SHIFT-TAB
            n--;
            break;
        }

    switch(LO(car))
        {
        case 9:
            n++;
            break;
        case 13:
            memcpy(Cfg->palette,defcol[n-16],48);
            LoadPal();
            rec=1;
            break;
        }

    if (n<0)        n=ntot;
    if (n>ntot)     n=0;

    }
}
while(car!=27);


LoadScreen();

}

/*--------------------------------------------------------------------*\
|-  Programme de setup                                                -|
\*--------------------------------------------------------------------*/


void Signal_Handler(int sig_no)
{
signal_count+=sig_no;       //--- ne sert … rien -----------------------

SpecMessy="You have pressed on Control Break :(";

memset(SpecSortie,0,256);
saveconfig=0;
Fin();
}

void SecretPart(void)
{
int py=1,log=0;
int x;
char buffer[78];
char fin=0;

log=0;

SaveScreen();
PutCur(32,0);

WinCadre(0,0,79,(Cfg->TailleY)-2,2);
Window(1,1,78,(Cfg->TailleY)-3,10*16+1);


do
{
if (log==0)
    {
    PrintAt(1,py,"Login: ");
    *buffer=0;
    InputAt(8,py,buffer,71);
    if (!stricmp(buffer,"louve")) log=1;
    }
if (log==1)
    {
    PrintAt(1,py,"> ");
    *buffer=0;
    InputAt(3,py,buffer,76);
    if (*buffer==0) fin=1;
    if (!stricmp(buffer,"logout")) fin=1;
    if (!strnicmp(buffer,"exec",4))
        {
        sscanf(buffer+4,"%d",&x);
        GestionFct(x);
        }
    }
py++;
if (py>Cfg->TailleY-3) fin=1;
}
while(!fin);

LoadScreen();
}

/*--------------------------------------------------------------------*\
|-  Gestion de toutes les fonctions                                   -|
|----------------------------------------------------------------------|
|-  0: ?                                                              -|
|-  1: Help                                                           -|
|-  2: Invert Selection                                               -|
|-  3: Select Group of file                                           -|
|-  4: Unselect Group of file                                         -|
|-  5: Search File                                                    -|
|-  6: Create a KKD file                                              -|
|-  7: View file                                                      -|
|-  8: Quick view file                                                -|
|-  9: Edit file                                                      -|
|- 10: Copy                                                           -|
|- 11: Move                                                           -|
|- 12: Create Directory                                               -|
|- 13: Delete selection                                               -|
|- 14: Close left window                                              -|
|- 15: Close right window                                             -|
|- 16: Select current file                                            -|
|- 17: Change palette                                                 -|
|- 18: About                                                          -|
|- 19: Select temporary file                                          -|
|- 20: Quit KKC                                                       -|
|- 21: Fenetre DIZ                                                    -|
|- 22: Sort by name                                                   -|
|- 23: Sort by extension                                              -|
|- 24: Sort by date                                                   -|
|- 25: Sort by size                                                   -|
|- 26: Sort by unsort ;)                                              -|
|- 27: Reload Directory                                               -|
|- 28: ASCII Table                                                    -|
|- 29: Win CD                                                         -|
|- 30: Put file on command line                                       -|
|- 31: Appel du programme de configuration                            -|
|- 32: Switch les fontes                                              -|
|- 33: Switch special sort                                            -|
|- 34: Efface la trash                                                -|
|- 35: Affiche les infos                                              -|
|- 36: Ligne suivante                                                 -|
|- 37: Ligne precedente                                               -|
|- 38: Fenˆtre information                                            -|
|- 39: Changement d'attribut                                          -|
|- 40: Fenetre history des directories                                -|
|- 41: Va dans le r‚p‚rtoire pere                                     -|
|- 42: Va dans le r‚p‚rtoire trash                                    -|
|- 43: Active la fenetre principal (Cfg->FenAct)                      -|
|- 44: Rename Window                                                  -|
|- 45: Longernal editor                                                -|
|- 46: Selection du fichier en tant que repertoire courant            -|
|- 47: Switch le mode ecran (nombre de lignes)                        -|
|- 48: Switch le type d'ecran (watcom, norton, ...)                   -|
|- 49: Change le drive de la fenetre 0                                -|
|- 50: Change le drive de la fenetre 1                                -|
|- 51: Va dans le repertoire d'o— l'on a lanc‚ KK                     -|
|- 52: 10 lignes avant                                                -|
|- 53: 10 lignes apres                                                -|
|- 54: Premiere ligne                                                 -|
|- 55: Derniere ligne                                                 -|
|- 56: Va dans la premiere fenˆtre                                    -|
|- 57: Va dans la deuxieme fenˆtre                                    -|
|- 58: Change la fenˆtre avec la path de l'autre fenˆtre              -|
|- 59: Edit New File                                                  -|
|- 60: Switch Display Mode                                            -|
|- 61: Mode serveur                                                   -|
|- 62: Appelle KKSETUP.EXE                                            -|
|- 63: Info on disk                                                   -|
|- 64: Appelle l'application lie au fichier sans les lier             -|
|- 65: Menu Login                                                     -|
|- 66: Screen Setup                                                   -|
|- 67: Rafraichit l'ecran                                             -|
|- 68: Echange la fenetre 0 avec la 1                                 -|
|- 69: History of command                                             -|
|- 70: Get Last command                                               -|
|- 71: Fenetre affichage de file_id.diz                               -|
|- 72: Change le drive de la fenetre par default                      -|
|- 73: Execute une commande redirige vers le fichier temporaire       -|
|- 74: Affiche le fichier temporaire                                  -|
|- 75: Directory tree                                                 -|
|- 76: Screen Saver                                                   -|
|- 77: Support prot‚ge, lance l'installation                          -|
|- 78: Sauve ou charge une selection                                  -|
|- 79: Appelle le menu F2                                             -|
\*--------------------------------------------------------------------*/

void GestionFct(int fct)
{
FENETRE *FenOld;
static char buffer[256],buf2[256];
int i;
FILE *fic;

switch(fct)
    {
    case 0:
        break;
    case 1:
        Help();
        break;
    case 2:
        for (i=0;i<DFen->nbrfic;i++)
            FicSelect(i,2);
        break;
    case 3:
        SelectPlus();
        break;
    case 4:
        SelectMoins();
        break;
    case 5:
        Search(Fenetre[2],DFen);              // Fenetre[2]: Non visible
        break;
    case 6:
        CreateKKD();
        break;
    case 7:
        if (KKCfg->vieweur[0]==0)
           GestionFct(8);
           else
           CommandLine("#%s %s",KKCfg->vieweur,DFen->F[DFen->pcur]->name);
        break;
    case 8:
        switch(DFen->system)
            {
            case 0:
                View(DFen);
                break;
            }
        break;
    case 9:
        EditFile(DFen->F[DFen->pcur]->name);
        break;
    case 10:                                                     // Copy
        Copie(DFen,Fenetre[2],DFen->Fen2->path);   // Fen[2] non visible
        if (KKCfg->autoreload==1)
            {
            DFen=Fenetre[0];
            GestionFct(27);
            DFen=Fenetre[1];
            GestionFct(27);
            }
        GestionFct(43);   // Active la fenetre principal (KKCfg->FenAct)
        break;
    case 11:                                                     // Move
        Move(DFen,Fenetre[2],DFen->Fen2->path);    // Fen[2] non visible
        if (KKCfg->autoreload==1)
            {
            DFen=Fenetre[0];
            GestionFct(27);
            DFen=Fenetre[1];
            GestionFct(27);
            }
        GestionFct(43);   // Active la fenetre principal (KKCfg->FenAct)
        break;
    case 12:                                         // Create Directory
        CreateDirectory();
        break;
    case 13:                                         // Delete selection
        SaveSel(DFen);
        Delete(DFen);
        CommandLine("#cd .");
        LoadSel(0);
        if (KKCfg->autoreload==1)
            {
            DFen=DFen->Fen2;
            GestionFct(27);
            }
        GestionFct(43);   // Active la fenetre principal (KKCfg->FenAct)
        break;
    case 14:                                        // Close left window
        DFen=Fenetre[0];
        if (DFen->FenTyp==2)
            {
            DFen->init=1;
            DFen->FenTyp=0;
            }
        else
            {
            DFen->FenTyp=2;
            KKCfg->FenAct=1;
            DFen=Fenetre[KKCfg->FenAct];
            ChangeLine();                              // Affichage Path
            }
        break;
    case 15:                                       // Close right window
        DFen=Fenetre[1];
        if (DFen->FenTyp==2)
            {
            DFen->init=1;
            DFen->FenTyp=0;
            }
        else
            {
            DFen->FenTyp=2;
            KKCfg->FenAct=0;
            DFen=Fenetre[KKCfg->FenAct];
            ChangeLine();                              // Affichage Path
            }
        break;
    case 16:                                      // Select current file
        FicSelect(DFen->pcur,2);

        if (!strcmp(DFen->F[DFen->pcur]->name,"."))
            for (i=0;i<DFen->nbrfic;i++)
                if (!WildCmp(DFen->F[i]->name,Select_Chaine))
                    FicSelect(i,1);

        if (KKCfg->insdown==1)
            GestionFct(36);
        break;
    case 17:                                           // Change palette
        ChangePalette(0);
        break;
    case 18:                                                    // About
        WinMesg("About",RBTitle" /RedBug",0);
        break;
    case 19:                                    // Select temporary file
        for (i=0;i<DFen->nbrfic;i++)
            if ( (!WildCmp(DFen->F[i]->name,"*.bak")) |
                 (!WildCmp(DFen->F[i]->name,"*.old")) |
                 (!WildCmp(DFen->F[i]->name,"*.tmp")) |
                 (!WildCmp(DFen->F[i]->name,"*.map")) |
                 (!WildCmp(DFen->F[i]->name,"*.lst")) |
                 (!WildCmp(DFen->F[i]->name,"*.obj")) |
                 (!WildCmp(DFen->F[i]->name,"cache.its")) |
                 (!WildCmp(DFen->F[i]->name,"cache.iti")) |
                 (!WildCmp(DFen->F[i]->name,"chklist.ms")) |
                 (!WildCmp(DFen->F[i]->name,"file*.chk")) |
                 (!WildCmp(DFen->F[i]->name,"*.$")) )
                      FicSelect(i,1);                     // Select file
        break;
    case 20:                                                 // Quit KKC
        break;
    case 21:                                              // Fenetre DIZ
        DFen=DFen->Fen2;

        if (DFen->FenTyp==1)
            {
            DFen->init=1;
            DFen->FenTyp=0;
            }
            else
            DFen->FenTyp=1;

        DFen=DFen->Fen2;
        break;
    case 22:                                             // Sort by name
        DFen->order&=16;
        DFen->order|=1;
        SortFic(DFen);
        MenuBar(4);
        break;
    case 23:                                        // Sort by extension
        DFen->order&=16;
        DFen->order|=2;
        SortFic(DFen);
        MenuBar(4);
        break;
    case 24:                                             // Sort by date
        DFen->order&=16;
        DFen->order|=3;
        SortFic(DFen);
        MenuBar(4);
        break;
    case 25:                                             // Sort by size
        DFen->order&=16;
        DFen->order|=4;
        SortFic(DFen);
        MenuBar(4);
        break;
    case 26:                                        // Sort by unsort ;)
        DFen->order&=16;
        SortFic(DFen);
        GestionFct(27);         //--- Reload ---------------------------
        MenuBar(4);
        break;
    case 27:                                                   // Reload
        SaveSel(DFen);
        CommandLine("#cd .");
        LoadSel(0);
        break;
    case 28:                                              // Table ASCII
        ASCIItable();
        break;
    case 29:                                                    // WINCD
        WinCD();
        break;
    case 30:                                 // Put file on command line
        CommandLine("%s ",DFen->F[DFen->pcur]->name);
        break;
    case 31:                                            // Configuration
        Setup();
        break;
    case 32:                                        // Switch les fontes
        Cfg->font^=1;
        GestionFct(67);
        break;
    case 33:                                      // Switch Special Sort
        DFen->order^=16;
        SortFic(DFen);
        MenuBar(4);
        break;
    case 34:                                         // Nettoie la trash
        strcpy(buffer,Fics->trash);
        Path2Abs(buffer,"kktrash.sav");

        fic=fopen(buffer,"rt");
        if (fic!=NULL)
            {
            while(fgets(buffer,256,fic)!=NULL)
                {
                buffer[strlen(buffer)-1]=0; // Retire le caractere ENTER
                remove(buffer);
                }
            fclose(fic);
            }

        strcpy(buffer,Fics->trash);
        Path2Abs(buffer,"kktrash.sav");
        remove(buffer);

        if (KKCfg->autoreload==1)
            {
            DFen=Fenetre[0];
            GestionFct(27);
            DFen=Fenetre[1];
            GestionFct(27);
            }
        KKCfg->strash=0;
        GestionFct(43);   // Active la fenetre principal (KKCfg->FenAct)
        break;
    case 35:
       // WinInfo(Fenetre);
        break;
    case 36:
        DFen->scur++;
        DFen->pcur++;
        break;
    case 37:
        DFen->scur--;
        DFen->pcur--;
        break;
    case 38:
        DFen=DFen->Fen2;

        DFen->init=1;

        if (DFen->FenTyp==3)
            DFen->FenTyp=0;
            else
            DFen->FenTyp=3;

        DFen=DFen->Fen2;
        break;
    case 39:                                     // Change les attributs
        WinAttrib();
        if (KKCfg->autoreload==1)
            {
            DFen=Fenetre[0];
            GestionFct(27);
            DFen=Fenetre[1];
            GestionFct(27);
            }
        GestionFct(43);   // Active la fenetre principal (KKCfg->FenAct)
        break;
    case 40:
        HistDir();
        break;
    case 41:
        CommandLine("#CD ..");
        break;
    case 42:
        CommandLine("#CD %s",Fics->trash);
        break;
    case 43:
        KKCfg->FenAct=(KKCfg->FenAct)&1;
        DFen=Fenetre[KKCfg->FenAct];
        ChangeLine();                                  // Affichage Path
        break;
    case 44:                               // Fenetre pour renomation ;)
        WinRename(DFen);
        if (KKCfg->autoreload==1)
            {
            DFen=Fenetre[0];
            GestionFct(27);
            DFen=Fenetre[1];
            GestionFct(27);
            }
        GestionFct(43);   // Active la fenetre principal (KKCfg->FenAct)
        break;
    case 45:                                          // Longernal editor
        switch(DFen->system)
            {
            case 0:
//                Edit(DFen);
                break;
            }
        break;
    case 46:      // Selection du fichier en tant que repertoire courant
        if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
                CommandLine("#cd %s",DFen->F[DFen->pcur]->name);
        break;
    case 47:                  // Switch le mode ecran (nombre de lignes)
        ChangeTaille(0);
        break;
    case 48:              //Switch le type d'ecran (watcom, norton, ...)
        ChangeType(0);
        AfficheTout();
        break;
    case 49:                          // Change le drive de la fenetre 1
        FenOld=DFen;
        DFen=Fenetre[0];

        ChangeDrive();
        DFen=FenOld;

        ChangeLine();                                  // Affichage Path
        break;
    case 50:                          // Change le drive de la fenetre 2
        FenOld=DFen;
        DFen=Fenetre[1];

        ChangeDrive();
        DFen=FenOld;

        ChangeLine();                                  // Affichage Path
        break;
    case 51:               // Va dans le repertoire d'o— l'on a lanc‚ KK
        CommandLine("#CD %s",Fics->LastDir);
        break;
    case 52:
        DFen->scur-=10;
        DFen->pcur-=10;
        break;
    case 53:
        DFen->scur+=10;
        DFen->pcur+=10;
        break;
    case 54:
        DFen->scur=0;
        DFen->pcur=0;
        break;
    case 55:
        DFen->scur=DFen->nbrfic;
        DFen->pcur=DFen->nbrfic;
        break;
    case 56:
        if (DFen->Fen2->FenTyp==3)
            {
            if (DFen->InfoPos>=5)
                DFen->InfoPos-=5;
            }
            else
            {
            KKCfg->FenAct=0;
            DFen=Fenetre[KKCfg->FenAct];
            ChangeLine();                              // Affichage Path
            }
        break;
    case 57:
        if (DFen->Fen2->FenTyp==3)
            DFen->InfoPos+=5;
            else
            {
            KKCfg->FenAct=1;
            DFen=Fenetre[KKCfg->FenAct];
            ChangeLine();                              // Affichage Path
            }
        break;
    case 58:        // Change la fenˆtre avec la path de l'autre fenˆtre
        CommandLine("#CD %s",DFen->Fen2->path);
        break;
    case 59:
        EditNewFile();
        break;
    case 60:
        DesinitScreen();
        do
            {
            Cfg->display++;
            if (Cfg->display>16) Cfg->display=0;
            }
        while(!InitScreen(Cfg->display));
        UseCfg();
        break;
    case 61:
        ServerMode();
        break;
    case 62:                                                  // KKSETUP
        strcpy(buffer,Fics->path);
        Path2Abs(buffer,"kksetup.exe");
        CommandLine("#%s",buffer);
        break;
    case 63:                                        // Fenetre info disk
        DFen=DFen->Fen2;

        DFen->init=1;

        if (DFen->FenTyp==4)
            DFen->FenTyp=0;
            else
            DFen->FenTyp=4;

        DFen=DFen->Fen2;
        break;
    case 64:
        i=EnterArchive();
        strcpy(buf2,DFen->path);
        Path2Abs(buf2,DFen->F[DFen->pcur]->name);
        i=FicIdf(buffer,buf2,i,1);
        CommandLine(buffer);

        switch(i)
            {
            case 0:
                CommandLine("\n");
                break;
            case 1:                          // Pas de fichier IDFEXT.RB
                CommandLine("@ ERROR WITH FICIDF @");
                break;
            case 2:
                 CommandLine(DFen->F[DFen->pcur]->name);
                 break;
            }
        break;
    case 65:
        SecretPart();
        break;
    case 66:
        ScreenSetup();
        break;
    case 67:
        ChangeTaille(Cfg->TailleY);                // Rafraichit l'ecran
        break;
    case 68:
        SwapWin(0,1);
        break;
    case 69:
        HistCom();
        break;
    case 70:
        History2Line(GetLastHistCom(),buffer);
        CommandLine("0\r%s",buffer);
        break;
    case 71:                         // Fenetre affichage de file_id.diz
        DFen=DFen->Fen2;

        DFen->init=1;

        if (DFen->FenTyp==5)
            DFen->FenTyp=0;
            else
            DFen->FenTyp=5;

        DFen=DFen->Fen2;
        break;
    case 72:                // Change le drive de la fenetre par default
        ChangeDrive();
        ChangeLine();                                  // Affichage Path
        break;
    case 73:
        ExecCom();
        break;
    case 74:
        DFen=Fenetre[2];                                // Fenetre trash
        CommandLine("#CD %s",Fics->trash);
        FileinPath(Fics->temp,buffer);
        for (i=0;i<DFen->nbrfic;i++)
            if (!WildCmp(buffer,DFen->F[i]->name))
                {
                DFen->pcur=i;
                DFen->scur=i;
                break;
                }
        GestionFct(8);
        break;
    case 75:                                   // Fenetre directory tree
        DFen=DFen->Fen2;

        DFen->init=1;

        if (DFen->FenTyp==6)
            DFen->FenTyp=0;
            else
            DFen->FenTyp=6;

        DFen=DFen->Fen2;
        break;
    case 76:
        if (KKCfg->ssaver[0]!=0)
            CommandLine("#%s",KKCfg->ssaver);
        else
            ScreenSaver();
        break;
    case 77:
        WinError("Access Denied");
        SpecMessy="You must copy files on no write protected support";

        memset(SpecSortie,0,256);
        saveconfig=0;
        Fin();
        break;
    case 78:
        WinSelect(DFen);
        if (KKCfg->autoreload==1)
            {
            DFen=DFen->Fen2;
            GestionFct(27);
            }
        GestionFct(43);   // Active la fenetre principal (KKCfg->FenAct)
        GestionFct(27);         //--- Reload ---------------------------
        break;
    case 79:
        Menu();
        break;
    }


}

/*--------------------------------------------------------------------*\
|-  Gestion de la barre de menu du haut                               -|
\*--------------------------------------------------------------------*/

int GestionBar(int i)
{
int retour,nbmenu;
int u,v,s,x;

struct barmenu bar[20];

short fin;

static int cpos[20],poscur;

SaveScreen();

if (i==0)
    retour=0;
    else
    {
    retour=1;
    switch(i)
        {
        case 0x21:  poscur=0;  break;                           // ALT-F
        case 0x19:  poscur=1;  break;                           // ALT-P
        case 0x20:  poscur=2;  break;                           // ALT-D
        case 0x1F:  poscur=3;  break;                           // ALT-S
        case 0x14:  poscur=4;  break;                           // ALT-T
        case 0x18:  poscur=5;  break;                           // ALT-O
        case 0x23:  poscur=6;  break;                           // ALT-H
        }
    }

do
{
strcpy(bar[0].titre,"File");
strcpy(bar[1].titre,"Panel");
strcpy(bar[2].titre,"Disk");
strcpy(bar[3].titre,"Selection");
strcpy(bar[4].titre,"Tools");
strcpy(bar[5].titre,"Options");
strcpy(bar[6].titre,"Help");

/*
strcpy(bar[0].help,"Various");
strcpy(bar[1].help,"File");
strcpy(bar[2].help,"Disk");
strcpy(bar[3].help,"Commands");
strcpy(bar[4].help,"Tools");
strcpy(bar[5].help,"Archiver");
strcpy(bar[6].help,"Options");
*/

if (retour==0)                             // Navigation sur bar de menu
    v=1;
    else
    v=0;

u=BarMenu(bar,7,&poscur,&x,&v);
                               // Renvoit t: position du machin surligne
            // Renvoit v: 0 si rien, autre si position dans sous fenetre
if (u==0)
    {
    fin=0;
    break;
    }

switch (poscur)
 {
 case 0:
   strcpy(bar[0].titre,"View                F3");  bar[0].fct=7;
   strcpy(bar[1].titre,"Quick View    Shift-F3");  bar[1].fct=8;
   strcpy(bar[2].titre,"Edit                F4");  bar[2].fct=9;
   strcpy(bar[3].titre,"");                        bar[3].fct=0;
   strcpy(bar[4].titre,"Copy                F5");  bar[4].fct=10;
   strcpy(bar[5].titre,"Move                F6");  bar[5].fct=11;
   strcpy(bar[6].titre,"Create Directory... F7");  bar[6].fct=12;
   strcpy(bar[7].titre,"Delete              F8");  bar[7].fct=13;
   strcpy(bar[8].titre,"");                        bar[8].fct=0;
   strcpy(bar[9].titre,"Exit               F10");  bar[9].fct=20;
   nbmenu=10;
   break;
 case 1:
   strcpy(bar[0].titre, "Close left window    CTRL-F1");  bar[0].fct=14;
   strcpy(bar[1].titre, "Close right window   CTRL-F2");  bar[1].fct=15;
   strcpy(bar[2].titre, "");                              bar[2].fct=0;
   strcpy(bar[3].titre, "DIZ Window            ALT-F3");  bar[3].fct=21;
   strcpy(bar[4].titre, "Info Window           CTRL-L");  bar[4].fct=63;
   strcpy(bar[5].titre, "");                              bar[5].fct=0;
   strcpy(bar[6].titre, "Name                 CTRL-F3");  bar[6].fct=22;
   strcpy(bar[7].titre, "Extension            CTRL-F4");  bar[7].fct=23;
   strcpy(bar[8].titre, "Time/Date            CTRL-F5");  bar[8].fct=24;
   strcpy(bar[9].titre, "Size                 CTRL-F6");  bar[9].fct=25;
   strcpy(bar[10].titre,"Unsort               CTRL-F7"); bar[10].fct=26;
   nbmenu=11;
   break;
 case 2:
   strcpy(bar[0].titre,"Create KKD");           bar[0].fct=6;
   strcpy(bar[1].titre,"");                     bar[1].fct=0;
   strcpy(bar[2].titre,"Erase files in trash"); bar[2].fct=34;
   nbmenu=3;
   break;
 case 3:
   strcpy(bar[0].titre, "Select group...     Gray '+'");  bar[0].fct=3;
   strcpy(bar[1].titre, "Unselect group...   Gray '-'");  bar[1].fct=4;
   strcpy(bar[2].titre, "Invert Selection    Gray '*'");  bar[2].fct=2;
   nbmenu=3;
   break;
 case 4:
   strcpy(bar[0].titre,"Search File...     Alt-F7");  bar[0].fct=5;
   strcpy(bar[1].titre,"Select temporary File   ý");  bar[1].fct=19;
   strcpy(bar[2].titre,"                         ");  bar[2].fct=0;
   strcpy(bar[3].titre,"ASCII Table        CTRL-A");  bar[3].fct=28;
   strcpy(bar[4].titre,"Screen Saver             ");  bar[4].fct=76;
   nbmenu=5;
   break;
 case 5:
   strcpy(bar[0].titre,"Configuration      ");   bar[0].fct=31;
   strcpy(bar[1].titre,"Color Configuration");   bar[1].fct=17;
   strcpy(bar[2].titre,"Screen Setup");          bar[2].fct=66;
   strcpy(bar[3].titre,"");                      bar[3].fct=0;
   strcpy(bar[4].titre,"Main Setup         ");   bar[4].fct=62;
   nbmenu=5;
   break;
 case 6:
   strcpy(bar[0].titre,"Help ");    bar[0].fct=1;
   strcpy(bar[1].titre,"About");    bar[1].fct=18;
   nbmenu=2;
   break;
   }

s=2;
retour=PannelMenu(bar,nbmenu,&(cpos[poscur]),&x,&s);      // (x,y)=(t,s)

if (retour==2)
    {
    fin=bar[cpos[poscur]].fct;
    break;
    }
    else
    {
    poscur+=retour;
    fin=0;
    }

}
while(1);

LoadScreen();

return fin;
}

/*--------------------------------------------------------------------*/



/*--------------------------------------------------------------------*\
|- Fonction ChangeType:                                               -|
|-     Change le type des fenˆtres                                    -|
|- (0 pour incrementer le type des fenˆtres                           -|
\*--------------------------------------------------------------------*/
void ChangeType(char n)
{
if (n==0)
   KKCfg->fentype++;
   else
   KKCfg->fentype=n;

if (KKCfg->fentype>4) KKCfg->fentype=1;
}


/*--------------------------------------------------------------------*\
|-                        Fenetre Select File                         -|
\*--------------------------------------------------------------------*/

void SelectPlus(void)
{
static int DirLength=32;
int n;

struct Tmt T[] = {
     { 2,2,1,Select_Chaine,&DirLength},
     { 3,4,2,NULL,NULL},                                       // le OK
     {20,4,3,NULL,NULL} };                                 // le CANCEL

struct TmtWin F = {-1,5,36,11,"Selection of files"};

n=WinTraite(T,3,&F,0);
if (n==27) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

for (n=0;n<DFen->nbrfic;n++)
    if ( (KKCfg->seldir==1) |
                         ((DFen->F[n]->attrib & RB_SUBDIR)!=RB_SUBDIR) )
            if (!WildCmp(DFen->F[n]->name,Select_Chaine))
                FicSelect(n,1);
}

/*--------------------------------------------------------------------*\
|-                       Fenetre Deselect File                        -|
\*--------------------------------------------------------------------*/

void SelectMoins(void)
{
static int DirLength=32;
int n;

struct Tmt T[] = {
     { 2,2,1,Select_Chaine,&DirLength},
     { 3,4,2,NULL,NULL},                                       // le OK
     {20,4,3,NULL,NULL} };                                 // le CANCEL

struct TmtWin F = {-1,5,36,11,"Deselection of files"};

n=WinTraite(T,3,&F,0);

if (n==27) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

for (n=0;n<DFen->nbrfic;n++)
    if ( (KKCfg->seldir==1) |
                         ((DFen->F[n]->attrib & RB_SUBDIR)!=RB_SUBDIR) )
        if (!WildCmp(DFen->F[n]->name,Select_Chaine))
            FicSelect(n,0);
}

/*--------------------------------------------------------------------*\
|-                      Fenetre History Directory                     -|
\*--------------------------------------------------------------------*/

void HistDir(void)
{
int Mlen;

int i,j;
static char **dir;

dir=(char**)GetMem(100*sizeof(char *));

j=0;
Mlen=0;
for (i=0;i<100;i++)
    {
    dir[i]=KKCfg->HistDir+j;
    dir[i]=strupr(dir[i]);
    if (strlen(dir[i])>Mlen) Mlen=strlen(dir[i]);
    if (strlen(dir[i])==0) break;
    while ( (j!=256) & (KKCfg->HistDir[j]!=0) ) j++;
    j++;
    }

if (i!=0)
    {
    int x=2,y=2,pos=i-1,car,max,prem;

    SaveScreen();
    PutCur(32,0);

    max=i;
    if (max>Cfg->TailleY-4) max=Cfg->TailleY-4;

    WinCadre(x-2,y-1,x+Mlen+1,y+max,4+0);
    Window(x-1,y,x+Mlen,y+max-1,14*16+7);

    prem=0;

    do {

    while((pos-prem)>=max) prem++;
    while((pos-prem)<0) prem--;


    for (j=0;j<max;j++)
        PrintAt(x,y+j,"%-*s",Mlen,dir[j+prem]);

    ColLin(x-1,y+(pos-prem),Mlen+2,7*16+4);

    car=Wait(0,0,0);

    ColLin(x-1,y+(pos-prem),Mlen+2,14*16+7);

    if (car==0)
        {
        int px,py,pz;

        px=MousePosX();
        py=MousePosY();
        pz=MouseButton();

        if ((pz&2)==2)
            car=27;

        if ((pz&4)==4)
            car=13;

        if ((pz&1)==1)
            {
            if (py<=y-1) car=72*256;
                else
                if (py>=y+max) car=80*256;
                    else
                    {
                    pos=py-y+prem;
                    }
            }
        }

    switch(HI(car))
        {
        case 72:                                                   // UP
            pos--;
            if (pos==-1) pos=i-1;
            break;
        case 80:                                                 // DOWN
            pos++;
            if (pos==i) pos=0;
            break;
        case 0x47:                                               // HOME
            pos=0;
            break;
        case 0x4F:                                                // END
            pos=i-1;
            break;
        case 0x49:                                               // PGUP
            pos-=5;
            if (pos<0) pos=0;
            break;
        case 0x51:                                               // PGDN
            pos+=5;
            if (pos>=i) pos=i-1;
            break;
        }
    }
    while ( (car!=13) & (car!=27) & (HI(car)!=0x8D) );

    LoadScreen();

    if (car==13)
        CommandLine("#cd %s",dir[pos]);
    }

LibMem(dir);
}

/*--------------------------------------------------------------------*\
|-                      Fenetre History Commande                      -|
\*--------------------------------------------------------------------*/

void HistCom(void)
{
int Mlen;
char dest[256];
int i,j;
static char **com;

com=(char**)GetMem(100*sizeof(char *));

j=0;
Mlen=0;
for (i=0;i<100;i++)
    {
    History2Line(KKCfg->HistCom+j,dest);
    dest[76]=0;
    com[i]=(char*)GetMem(strlen(dest)+1);
    strcpy(com[i],dest);

    if (strlen(com[i])>Mlen) Mlen=strlen(com[i]);
    if (strlen(com[i])==0) break;
    while ( (j!=512) & (KKCfg->HistCom[j]!=0) ) j++;
    j++;
    }

if (i!=0)
    {
    int x=2,y=2,pos=i-1,car,max,prem;

    SaveScreen();
    PutCur(32,0);

    max=i;
    if (max>Cfg->TailleY-4) max=Cfg->TailleY-4;

    WinCadre(x-2,y-1,x+Mlen+1,y+max,4+0);
    Window(x-1,y,x+Mlen,y+max-1,14*16+7);

    prem=0;

    do {

    while((pos-prem)>=max) prem++;
    while((pos-prem)<0) prem--;


    for (j=0;j<max;j++)
        PrintAt(x,y+j,"%-*s",Mlen,com[j+prem]);

    ColLin(x-1,y+(pos-prem),Mlen+2,7*16+4);

    car=Wait(0,0,0);

    ColLin(x-1,y+(pos-prem),Mlen+2,14*16+7);

    switch(HI(car))
        {
        case 72:                                                   // UP
            pos--;
            if (pos==-1) pos=i-1;
            break;
        case 80:                                                 // DOWN
            pos++;
            if (pos==i) pos=0;
            break;
        case 0x47:                                               // HOME
            pos=0;
            break;
        case 0x4F:                                                // END
            pos=i-1;
            break;
        case 0x49:                                               // PGUP
            pos-=5;
            if (pos<0) pos=0;
            break;
        case 0x51:                                               // PGDN
            pos+=5;
            if (pos>=i) pos=i-1;
            break;
        }
    }
    while ( (car!=13) & (car!=27) & (HI(car)!=0x8D) );

    LoadScreen();

    if (car==13)
        {
        History2Line(com[pos],dest);
        CommandLine("%s\n",dest);
        }
    }

for(j=0;j<=i;j++)
    LibMem(com[j]);

LibMem(com);
}


/*--------------------------------------------------------------------*\
|-                         Fenetre CD machin                          -|
\*--------------------------------------------------------------------*/

void WinCD(void)
{
static char Dir[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[5] =
      { { 2,3,1, Dir, &DirLength},
        {15,5,2,NULL,NULL},
        {45,5,3,NULL,NULL},
        { 5,2,0,"Change to which directory",NULL},
        { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,10,74,17, "Change Directory" };

int n;

n=WinTraite(T,5,&F,0);

if (n!=27)
    if (T[n].type!=3)
        CommandLine("#cd %s",Dir);
}


/*--------------------------------------------------------------------*\
|-                Create directory in current window                  -|
\*--------------------------------------------------------------------*/

void CreateDirectory(void)
{
static char Dir[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[5] =
    { { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the directory to be created",NULL},
      { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,10,74,17, "Create Directory" };

int n;

n=WinTraite(T,5,&F,0);

if (n!=27)
    if (T[n].type!=3)
        {
        CommandLine("#md %s",Dir);
        CommandLine("#cd .");
        }
}


/*--------------------------------------------------------------------*\
|-                             Edit File                              -|
\*--------------------------------------------------------------------*/
void EditFile(char *s)
{
if (KKCfg->editeur[0]!=0)
    CommandLine("#%s %s",KKCfg->editeur,s);
    else    GestionFct(45);
}

/*--------------------------------------------------------------------*\
|-                           Edit New File                            -|
\*--------------------------------------------------------------------*/

void EditNewFile(void)
{
static char Dir[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[5] =
    { { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the file to be edited",NULL},
      { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,10,74,17, "Edit New File" };

int n;

n=WinTraite(T,5,&F,0);

if (n!=27)
    if (T[n].type!=3)
        EditFile(Dir);
}

/*--------------------------------------------------------------------*\
|-                       Create KKD disk                              -|
\*--------------------------------------------------------------------*/
void CreateKKD(void)
{
static char Name[256];
static char Dir[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[5] =
    { { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the KKD file to be created",NULL},
      { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,10,74,17,"Create KKD File" };

int n;

n=WinTraite(T,5,&F,0);

if (n!=27)
    if (T[n].type!=3)
        {
        strcpy(Name,DFen->Fen2->path);

        for (n=strlen(Dir);n>0;n--)
            {
            if (Dir[n]==DEFSLASH) break;
            if (Dir[n]=='.')
                {
                Dir[n]=0;
                break;
                }
            }
        strcat(Dir,".kkd");

        Path2Abs(Name,Dir);
        MakeKKD(DFen,Name);
        }
}

/*--------------------------------------------------------------------*\
|-                    Change drive of current window                  -|
\*--------------------------------------------------------------------*/
void ChangeDrive(void)
{
static char path[256],path2[256];
long fpos,cpos,pos,p1,p2;

char drive[26];
long m,n,x,l,nbr;
long x1;
long i,d,olddrive;

int car;

chdir(DFen->path);

nbr=0;

for (i=0;i<26;i++)
    {
    drive[i]=GetDriveReady(i);
    if (drive[i]==0)
        nbr++;
    }

l=36/nbr;
if (l>3) l=3;
if (nbr<5) l=6;
if (nbr<2) l=9;
x=(40-(l*nbr))/2+DFen->x;


SaveScreen();

x1=DFen->x+1;

WinCadre(x1,6,x1+37,21,3);
Window(x1+1,7,x1+36,20,10*16+1);
WinCadre(x-1,8,x+l*nbr,10,1);

WinCadre(x1+1,11,x1+36,20,3);

PrintAt(x1+2,7, "Choose a drive");
PrintAt(x1+2,11,"Directory...");

m=x+l/2;
for (n=0;n<26;n++)
    {
    if (drive[n]==0)
        {
        drive[n]=(char)m;
        AffChr(m,9,(char)(n+'A'));
        m+=l;
        }
        else
        drive[n]=0;
    }


cpos=12;
fpos=0;

p1=DFen->path[0];
p1=toupper(p1)-'A';
p1=p1-1;

i=p1;
car=DROITE*256;

do  {
    do
        {
        

        switch(HI(car))
            {
            case GAUCHE: i--,cpos=50; break;                     // LEFT
            case DROITE: i++,cpos=50; break;                    // RIGHT
            case 80: cpos++; break;                              // DOWN
            case 72: cpos--; break;                                // UP
            case 0x47: cpos=12; break;                           // HOME
            case 0x4F: cpos=pos-1; break;                         // END
            }

        if (i==26) i=0;
        if (i<0) i=25;

        if ( (drive[i]!=0) & (cpos==50) )
            if (VerifyDisk(i+1)==0)
                {
                unsigned nbrdrive,ii;

                Window(x1+2,12,x1+35,19,10*16+1);

                ii=i+1;
                _dos_setdrive(ii,&nbrdrive);

                p1=DFen->path[0];
                p1=toupper(p1)-'A';

                if (p1==i)
                    strcpy(path,DFen->path);
                    else
                    getcwd(path,256);

                if (path[strlen(path)-1]!=DEFSLASH)
                    strcat(path,"\\");


             //Calcule le nombre de position occup‚e par les repertoires
                pos=12;
                for (p2=0;p2<strlen(path);p2++)
                    if (path[p2]==DEFSLASH)
                        pos++;
                break;
                }
                else
                {
                if ( (HI(car)!=GAUCHE) & (HI(car)!=DROITE) )
                    {
                    i=olddrive;
                    cpos=0;
                    }
                }
        }
    while (cpos==50);

    if (cpos>=pos)   cpos=pos-1;
    if (cpos<12)     cpos=12;

    while (cpos-fpos>19) fpos++;
    while (cpos-fpos<12) fpos--;

    pos=12;
    p1=0;
    p2=0;

    for (p2=0;p2<strlen(path);p2++)
        {
        if (path[p2]==DEFSLASH)
            {
            memset(path2,32,255);
            memcpy(path2+(pos-12)*2,path+p1,p2-p1+1);
            path2[31]=0;

            p1=p2+1;

            if ( (pos-fpos>=12) & (pos-fpos<=19) )
                PrintAt(x1+4,pos-fpos,"%s",path2);

            pos++;
            }
        }

    if (cpos!=50)
        ColLin(x1+3,cpos-fpos,32,7*16+4);

    AffCol(drive[i],9,10*16+5);
    car=Wait(0,0,0);

    if (cpos!=50)
        ColLin(x1+3,cpos-fpos,32,10*16+1);

    if (car==0)
        {
        int px,py,pz,n;

        px=MousePosX();
        py=MousePosY();
        pz=MouseButton();

        if (py==9)
            {
            for(n=0;n<26;n++)
                if (drive[n]==px) car=n+'A';
            }
            else
            if ((px>=x1) & (px<=x1+32))
                {
                cpos=py+fpos;
                }


        if ((pz&2)==2)
            car=27;

        if ((pz&4)==4)
            car=13;
        }

    AffCol(drive[i],9,10*16+1);

    if (HI(car)==0)
        {
        d=(toupper(car)-'A');
        if ( (d>=0) & (d<26) )
            if (drive[d]!=0)
                {
                ColLin(x1+3,cpos-fpos,32,10*16+1);
                olddrive=i;
                i=d;
                cpos=50;
                }
        }

} while ( (LO(car)!=27) & (LO(car)!=13));

pos=12;
p1=p2=0;

for (p2=0;p2<strlen(path);p2++)
    {
    if (path[p2]==DEFSLASH)
        {
        if (pos==cpos)
            path[p2]=0;
        pos++;
        }
    }
if (path[strlen(path)-1]==':') strcat(path,"\\");

LoadScreen();

if (car==13)
   CommandLine("#cd %s",path);
}


/*--------------------------------------------------------------------*\
|-                Change drive of current window for KKD              -|
\*--------------------------------------------------------------------*/
int ChangeToKKD(void)
{
char drive[26];
long m,n,x,l,nbr;
long i;

int car;

nbr=0;

for (i=0;i<26;i++)
    {
    drive[i]=GetDriveReady(i);
    if (drive[i]==0)
        nbr++;
    }

l=36/nbr;
if (l>3) l=3;
if (nbr<5) l=6;
if (nbr<2) l=9;
x=(40-(l*nbr))/2+DFen->x;


SaveScreen();

WinCadre(x-2,6,x+l*nbr+1,11,0);
Window(x-1,7,x+l*nbr,10,0*16+1);

WinCadre(x-1,8,x+l*nbr,10,1);

PrintAt(x,7,"Select KKDdrive");

m=x+l/2;
for (n=0;n<26;n++)
    {
    if (drive[n]==0)
        {
        drive[n]=(char)m;
        AffChr(m,9,n+'A');
        m+=l;
        }
        else
        drive[n]=0;
    }


i=-1;
car=DROITE*256;
do  {
    do {
        if (HI(car)==GAUCHE) i--;
        if (HI(car)==DROITE) i++;
        if (i==26) i=0;
        if (i<0) i=25;
        } while (drive[i]==0);

    if (HI(car)==0) {
        car=(toupper(car)-'A');
        if ( (car>=0) & (car<26) )
            if (drive[car]!=0)
                {
                i=car;
                car=13;
                break;
                }
       }

    AffCol(drive[i],9,1*16+5);

    car=Wait(0,0,0);
    AffCol(drive[i],9,0*16+1);

} while (LO(car)!=13);

LoadScreen();

CommandLine("#cd %c:\\",i+'A');

return i;
}

void QuickSearch(char *c,char *c2)
{
static char chaine[32];
static int lng=0;

char fin,fin2;
int i,x,y;
char car,car2;
signed char vit;

int c1;

x=DFen->Fen2->x+3;
y=DFen->Fen2->y+3;

WinCadre(x-1,y-1,x+24,y+1,0);
ColLin(x,y,24,0*16+1);
ChrLin(x,y,24,32);

if (lng!=0)
    if (chaine[lng-1]!='*')
        strcat(chaine,"*");

if (WildCmp(DFen->F[DFen->pcur]->name,chaine)!=0)
    lng=0;

chaine[lng]=0;

do
    {
    if (chaine[lng-1]!='*')
        PrintAt(x,y,"%23s*",chaine);
        else
        PrintAt(x,y,"%24s",chaine);

    c1=Wait(x+23,y,0);

    car=LO(c1);
    car2=HI(c1);

    car=(char)tolower(car);

    fin=1;
    vit=0;

    if ( ((car>='a') & (car<='z')) | ((car>='0') & (car<='9')) |
         (car=='.') | (car=='?') | (car=='*') )
        {
        if (lng<22)
            {
            chaine[lng]=car;
            lng++;
            }
        fin=0;
        }

    if (car==27)
        {
     //   if (lng!=0) lng=0,fin=0;
        car=0;
        }

    if ( (car==8) & (lng!=0) ) //--- BACKSPACE -------------------------
        lng--,fin=0;

    if (car2==80) //--- BAS --------------------------------------------
        vit=1,fin=0;

    if (car2==72) //--- HAUT -------------------------------------------
        vit=-1,fin=0;

    if (DFen->pcur+vit<0) DFen->pcur++;
    if (DFen->pcur+vit>=DFen->nbrfic) DFen->pcur--;

    chaine[lng]=0;

    if (fin==0)
        {
        fin2=0;

        if (lng==0)
            strcat(chaine,"*");
            else
            if (chaine[lng-1]!='*')
                strcat(chaine,"*");

        i=DFen->pcur+vit;

        while ((i>=0) & (i<DFen->nbrfic))
            {
            if (!WildCmp(DFen->F[i]->name,chaine))
                {
                DFen->scur=DFen->pcur=i;
                fin2=1;
                break;
                }
            (vit==-1) ? i-- : i++;
            }

        if ((vit==0) & (fin2==0))
            {
            for (i=0;i<DFen->nbrfic;i++)
                if (!WildCmp(DFen->F[i]->name,chaine))
                    {
                    DFen->scur=DFen->pcur=i;
                    fin2=1;
                    break;
                    }
            if ((fin2==0) & (lng!=0)) lng--;
            }

        chaine[lng]=0;
        }

    AffFen(DFen);
    } while(!fin);

ChangeLine();

*c=car;
*c2=car2;
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-                      Access fichier suivant system                 -|
\*--------------------------------------------------------------------*/

char *AccessFile(int n)
{
FENETRE *FenOld;
FILE *fic;
static char nom[256];
static char tnom[256];
static char buffer[256];

char ChangePos=0;

int i;


FenOld=DFen;

strcpy(tnom,DFen->F[n]->name);

switch (DFen->system)
    {
    case 0:                                                       // DOS
        strcpy(nom,DFen->path);
        Path2Abs(nom,tnom);
        break;
    case 1:                                                       // RAR
    case 2:                                                       // ARJ
    case 3:                                                       // ZIP
    case 4:                                                       // LHA
    case 5:                                                       // KKD
    case 6:                                                       // DFP
        DFen=Fenetre[2];  // Copie de l'autre c“t‚ (t'es content Phil ?)
        CommandLine("#cd %s",Fics->trash);

        KKCfg->FenAct= (KKCfg->FenAct)+2;

        DFen=FenOld;

                                 // Mets le nom du fichier dans la trash
        strcpy(buffer,Fics->trash);
        Path2Abs(buffer,"kktrash.sav");
        fic=fopen(buffer,"at");
        strcpy(buffer,Fics->trash);
        Path2Abs(buffer,DFen->F[n]->name);
        fprintf(fic,"%s\n",buffer);
        fclose(fic);
        KKCfg->strash+=DFen->F[n]->size;

                                     // Copie les fichiers dans la trash
        (KKCfg->noprompt)=(char)((KKCfg->noprompt)|1);
        DFen->nopcur=n;
        Copie(DFen,Fenetre[2],Fenetre[2]->path);         // Quitte aprŠs

        DFen=Fenetre[2];
        CommandLine("#cd %s",Fics->trash);

        ChangePos=1;
        break;
    default:
        break;
    }

if (ChangePos)
    {
    strcpy(nom,DFen->path);
    Path2Abs(nom,tnom);
    for (i=0;i<DFen->nbrfic;i++)
        if (!strncmp(tnom,DFen->F[i]->name,strlen(tnom)))
            {
            DFen->pcur=i;
            DFen->scur=i;
            break;
            }
    }
return nom;
}


/*--------------------------------------------------------------------*\
|-  Retourne 0 si OK                                                  -|
|- Sinon retourne numero de IDF                                       -|
\*--------------------------------------------------------------------*/
int EnterArchive(void)
{
int i;

static char buf[256];

strcpy(buf,DFen->path);
Path2Abs(buf,DFen->F[DFen->pcur]->name);

switch (i=NameIDF(buf))
    {
    case 30:                                                      // ARJ
    case 34:                                                      // RAR
    case 35:                                                      // ZIP
    case 32:                                                      // LHA
    case 102:                                                     // KKD
    case 139:                                                     // DFP
        strcpy(DFen->VolName,DFen->path);
        Path2Abs(DFen->VolName,DFen->F[DFen->pcur]->name);

        strcpy(DFen->path,DFen->VolName);
        break;
    }

switch (i)
    {
    case 34:                                                      // RAR
        DFen->system=1;        break;
    case 30:                                                      // ARJ
        DFen->system=2;        break;
    case 35:                                                      // ZIP
        DFen->system=3;        break;
    case 32:                                                      // LHA
        DFen->system=4;        break;
    case 139:                                                     // DFP
        DFen->system=6;        break;
    case 102:                                                     // KKD
        DFen->KKDdrive=0;
        DFen->system=5;        break;
    default:
        return i;
    }

CommandLine("#cd .");
return 0;                                                          // OK
}



/*--------------------------------------------------------------------*\
|-  Sauvegarde la selection sur disque                                -|
\*--------------------------------------------------------------------*/
void SaveSel(FENETRE *F1)
{
struct file *F;
FILE *fic;
int i;

fic=fopen(Fics->temp,"wt");
if (fic==NULL)
    GestionFct(77);

fprintf(fic,"%s\n",F1->F[F1->pcur]->name);

fprintf(fic,"%d %d\n",F1->pcur,F1->scur);          // position a l'ecran


for(i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];

    if ((F->select)==1)
        fprintf(fic,"%s\n",F->name);
    }
fclose(fic);
}

/*--------------------------------------------------------------------*\
|-  Charge la selection du disque                                     -|
\*--------------------------------------------------------------------*/
void LoadSel(int n)
{
char nom[256];
FILE *fic;
int i,j;

fic=fopen(Fics->temp,"rt");
if (fic==NULL) return;

fgets(nom,256,fic);
nom[strlen(nom)-1]=0;

fscanf(fic,"%d%d",&(DFen->pcur),&(DFen->scur));

j=1;
for (i=0;i<DFen->nbrfic;i++)
    if (!strncmp(nom,DFen->F[i]->name,strlen(nom)))
        {
        DFen->pcur=i;

        j=0;
        break;
        }

while(!feof(fic))
    {
    fgets(nom,256,fic);
    nom[strlen(nom)-1]=0;

    for(i=0;i<DFen->nbrfic;i++)
        {
        if (!WildCmp(DFen->F[i]->name,nom))
            {
            switch(n)
                {
                case 0:
                    FicSelect(i,1);                            // Select
                    break;
                case 1:
                    FicSelect(i,2);                 // Inverse Selection
                    break;
                }
            }
        }
    }
fclose(fic);
}

/*--------------------------------------------------------------------*\
|-                          Fonction RENAME                           -|
\*--------------------------------------------------------------------*/
void WinRename(FENETRE *F1)
{
static char Dir[70];
static char Name[256],Temp[256];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[5] = {
      { 2,3,1, Dir, &DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Rename file to",NULL},
      { 1,1,4,&CadreLength,NULL}
      };

struct TmtWin F = {-1,10,74,17,"Rename"};

int n;

strcpy(Dir,F1->path);
Path2Abs(Dir,F1->F[DFen->pcur]->name);

strcpy(Name,F1->path);
Path2Abs(Name,F1->F[DFen->pcur]->name);

n=WinTraite(T,5,&F,0);

strcpy(Temp,F1->path);
Path2Abs(Temp,Dir);


if (n!=27)
    if (T[n].type!=3)
        {
        if (rename(Name,Temp)!=0)
            WinError("Couldn't rename file");
        }
}

/*--------------------------------------------------------------------*\
|-                          Programme Principal                       -|
\*--------------------------------------------------------------------*/
void Gestion(void)
{
char *ext;

clock_t Cl,Cl_Start;
char car,car2;

long car3,c;

int oldzm=0,xm,ym,zm;

int i;  //--- Compteur -------------------------------------------------

Info->temps=clock()-Info->temps;

do
    {
    (KKCfg->noprompt)=(char)((KKCfg->noprompt)&126);
                                                // Retire le dernier bit

    if ( (KKCfg->key==0) & (KKCfg->strash>=KKCfg->mtrash) &
                                                    (KKCfg->mtrash!=0) )
        GestionFct(34);                 // Efface la trash si trop plein

    if ( (KKCfg->key==0) & (KKCfg->FenAct>1) )
        {
        GestionFct(43);
        }
        else
        {
        if (KKCfg->FenAct>1)
            {
            DFen=Fenetre[2];
            ChangeLine();                              // Affichage Path
            }
            else
            DFen=Fenetre[KKCfg->FenAct];
        }

    if (KKCfg->key==0)             // Switch si le buffer clavier est vide
        {
        switch(DFen->FenTyp)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                if ( ((DFen->Fen2->FenTyp)==1) |
                     ((DFen->Fen2->FenTyp)==2) |
                     ((DFen->Fen2->FenTyp)==3) |
                     ((DFen->Fen2->FenTyp)==4) |
                     ((DFen->Fen2->FenTyp)==5) ) break;
                KKCfg->FenAct= (KKCfg->FenAct==1) ? 0:1;
                DFen=Fenetre[KKCfg->FenAct];
                ChangeLine();                          // Affichage Path
                break;
            }
        }

    DFen->actif=1;
    DFen->Fen2->actif=0;

    if (Fenetre[1]->FenTyp==0)
        for (i=1;i>=0;i--)
            AffFen(Fenetre[i]);
        else
        for (i=0;i<2;i++)
            AffFen(Fenetre[i]);

    Cl_Start=clock();
    Cl=clock();

    if (KKCfg->key==0)
        {
        c=0;

        while ( (!KbHit()) & (c==0) )
            {
            GetPosMouse(&xm,&ym,&zm);

            if (zm==oldzm)
                zm=0;
                else
                oldzm=zm;

            if ((zm&1)==1)    //--- Bouton droit de la souris ----------
                {
                if (ym==Cfg->TailleY-1)
                    if (Cfg->TailleX==90)
                        c=((0x3B+(xm/9))*256);
                        else
                        c=((0x3B+(xm/8))*256);
                    else
                if (ym==0)
                    c=0x43*256;
                    else
                if ((xm>=DFen->x) & (xm<=DFen->x+DFen->xl))
                    {
                    if (ym>(DFen->scur+DFen->y2+3)) c=80*256;
                    if (ym<(DFen->scur+DFen->y2+3)) c=72*256;
                    oldzm=0;     // On peut laisser le bouton appuy‚ ---
                    }
                    else
                if ((xm>=DFen->Fen2->x) &
                                     (xm<=DFen->Fen2->x+DFen->Fen2->xl))
                    {
                    oldzm=0;     // On peut laisser le bouton appuy‚ ---
                    c=9;
                    }
                    else
                    {
                    int n;

                    n=(ym-1)/3;
                    if ( (n>=0) & (n<6) )
                        GestionFct(KKCfg->Nmenu[n]);
                    c=3;                       //--- On ne fait rien ---
                    }
                }

            if ((zm&4)==4)
                {
                if ( (ym==(DFen->scur+DFen->y2+3)) &
                     ((xm>=DFen->x) & (xm<=DFen->x+DFen->Fen2->xl)) )
                    c=13;
                }

            if ( ((clock()-Cl_Start)>DFen->IDFSpeed)  & (Cl_Start!=0))
                {
                Cl_Start=0;
                InfoIDF(DFen);// information idf sur fichier selectionn‚

                if (DFen->FenTyp==1) FenDIZ(DFen);
                if (DFen->Fen2->FenTyp==1) FenDIZ(DFen->Fen2);

                if (DFen->FenTyp==5) FenFileID(DFen);
                if (DFen->Fen2->FenTyp==5) FenFileID(DFen->Fen2);
                }

            if ( ((clock()-Cl)>Cfg->SaveSpeed*CLOCKS_PER_SEC)
                & (Cfg->SaveSpeed!=0) )
                GestionFct(76);

            car=*Keyboard_Flag1;

            if ((car&1)==1)  MenuBar(1);
                else
            if ((car&2)==2)  MenuBar(1);
                else
            if ((car&4)==4)  MenuBar(2);
                else
            if ((car&8)==8)  MenuBar(3);
                else
                MenuBar(0);

            }

        car3=(_bios_keybrd(0x11)/256);

        if (c==0)
            c=Wait(0,0,0);

        KKCfg->key=(short)c;

        //--- Positionne le pointeur sur FILE_ID.DIZ si on est sur .. --
        switch(HI(c))  
            {
            case 0x3D:  //--- F3 ---------------------------------------
            case 0x3E:  //--- F4 ---------------------------------------
            case 0x56:  //--- SHIFT-F3 ---------------------------------
            case 0x8D:  //--- CTRL-UP ----------------------------------
                if (!strcmp(DFen->F[DFen->pcur]->name,".."))
                    {
                    car=0;
                    for (i=0;i<DFen->nbrfic;i++)
                       if (!strnicmp("FILE_ID.DIZ",DFen->F[i]->name,13))
                            {
                            DFen->pcur=i;
                            DFen->scur=i;
                            car=1;
                            break;
                            }
                    if (car==0)
                        {
                        c=0;
                        WinError("No FILE_ID.DIZ found");
                        }
                    }
                break;
            case 0x85:  //--- F11 --------------------------------------
                GestionFct(73);
                c=0;
                break;

            }

        strcpy(KKCfg->FileName,DFen->F[DFen->pcur]->name);

        switch(LO(c))
            {
            case 13:
               KKCfg->key=0;
               if (CommandLine("\n")!=0)
                   {
                   c=0;
                   break;
                   }

               if (DFen->F[DFen->pcur]->name[1]=='*')
                    {
                    EjectCD(DFen);
                    c=0;
                    break;
                    }

               if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
                    {
                    CommandLine("#cd %s",DFen->F[DFen->pcur]->name);
                    c=0;
                    break;
                    }
                KKCfg->key=(short)c;
                AccessFile(DFen->pcur);
                break;
            }

        switch(HI(c))
            {
            case 0x3D:                                             // F3
            case 0x3E:                                             // F4
            case 0x56:                                       // SHIFT-F3
            case 0x8D:                                        // CTRL-UP
                AccessFile(DFen->pcur);
                break;
            }
        }
    else
        {
        c=KKCfg->key;
        car3=0;

        for (i=0;i<DFen->nbrfic;i++)
            if (!strnicmp(KKCfg->FileName,DFen->F[i]->name,
                strlen(KKCfg->FileName)))
                {
                DFen->pcur=i;
                DFen->scur=i;
                break;
                }
        }

    KKCfg->key=0;

    car=LO(c);
    car2=HI(c);

//-Quick search (CTRL-TAB)----------------------------------------------
    if (car2==0x94)
        QuickSearch(&car,&car2);

//-Pour accelerer la vitesse d'affichage des fichiers ------------------
    switch(car2)
    {
    case 72:
    case 80:
        break;
    default:
        DFen->oldscur=0;
        DFen->oldpcur=-1;
        DFen->Fen2->oldscur=0;
        DFen->Fen2->oldpcur=-1;
        break;
    }

//-Switch car3 (BIOS_KEYBOARD)------------------------------------------
    switch (car3)
    {
    case 0xE0:                                          // Extended code
        switch(car)
            {
            case '/':           // '/' --> Sauve ou charge une selection
                GestionFct(78);
                car=car2=0;
                break;
            }
        break;
    case 0x37:                              // '*' --> Inverse selection
        GestionFct(2);
        car=car2=0;
        break;
    case 0x4E:                                    // '+' --> Selectionne
        GestionFct(3);
        car=car2=0;
        break;
    case 0x4A:                                  // '-' --> Deselectionne
        GestionFct(4);
        car=car2=0;
        break;
    default:
//      PrintAt(0,0,"%04X",car3);
        break;
    }

//-Switch car-----------------------------------------------------------
    switch (car)
    {
    case 0x12:                                                 // CTRL-R
        GestionFct(27);        break;
    case 1:                                                    // CTRL-A
        GestionFct(28);        break;
    case 2:                                                    // CTRL-B
        i=GestionBar(0);

        if (i==20)
            car2=0x44;                                            // F10
            else
            GestionFct(i);
        break;
    case 3:
        break;

    case 5:                                                    // CTRL-E
        GestionFct(70);        break;
    case 4:                                                    // CTRL-D
        GestionFct(29);        break;
    case 6:                                                    // CTRL-F
        GestionFct(32);        break;
    case 9:                                                       // TAB
        KKCfg->FenAct= (KKCfg->FenAct==1) ? 0:1;
        DFen=Fenetre[KKCfg->FenAct];
        ChangeLine();                                  // Affichage Path
        break;
    case 13:                                                    // ENTER
        ext=getext(DFen->F[DFen->pcur]->name);
        if ( (!stricmp(ext,"COM")) | (!stricmp(ext,"BAT")) |
             ((!stricmp(ext,"BTM")) & (KKCfg->_4dos==1)) )
            {
            CommandLine("%s\n",DFen->F[DFen->pcur]->name);
            break;
            }
        switch(i=EnterArchive())
            {
            case 0:                                                // OK
                break;
            case 57:                                       // Executable
                CommandLine("%s\n",DFen->F[DFen->pcur]->name);
                break;
            default:
                {
                static char buffer[256],buf2[256];
                strcpy(buf2,DFen->path);
                Path2Abs(buf2,DFen->F[DFen->pcur]->name);
                i=FicIdf(buffer,buf2,i,0);
                CommandLine(buffer);

                switch(i)
                    {
                    case 0:
                        CommandLine("\n");
                        break;
                    case 1:                  // Pas de fichier IDFEXT.RB
                        CommandLine("@ ERROR WITH FICIDF @");
                        break;
                    case 2:
                         CommandLine(DFen->F[DFen->pcur]->name);
                         break;
                    }
                }
            }
        break;
    case 0x0A:                                             // CTRL-ENTER
        GestionFct(30);
        break;
    case 0x0C:                                                 // CTRL-L
        GestionFct(63);
        break;
    case 0x0F:                                                 // CTRL-O
        GestionFct(14);
        GestionFct(15);
        break;
    case 0x14:                                                 // CTRL-T
//        GestionFct(75);
        break;
    case 0x15:                                                 // CTRL-U
        GestionFct(68);
        break;
    case 27:                                                   // ESCAPE
        if (CommandLine("\r")==0)
            if (KKCfg->Esc2Close==1)
                GestionFct(14),GestionFct(15);
        break;
    case 'ý':
        GestionFct(19);
        break;
    case 0:
        break;
    case 32:
        if (CommandLine(" ")==0)
            GestionFct(16);
        break;
//-Retour Switch car----------------------------------------------------
    default:                                    // default du switch car
        CommandLine("%c",car);
        break;
    }                                                     // switch(car)

//-Switch car2----------------------------------------------------------
    switch(car2)
    {                                                   // Switch (car2)
    case 0:
        break;
    case 0x85:                                                    // F11
        GestionFct(74);
        break;
    case 0x1C:                                              // ALT-ENTER
        GestionFct(58);           break;
    case 72:                                                     // HAUT
        GestionFct(37);           break;
    case 0x52:                                                 // Insert
        GestionFct(16);           break;
    case 80:                                                      // BAS
        GestionFct(36);           break;
    case 0x4B:                                                   // LEFT
        GestionFct(56);           break;
    case 0x4D:                                                  // RIGHT
        GestionFct(57);           break;
    case 0x49:                                                // PAGE UP
        GestionFct(52);           break;
    case 0x51:                                              // PAGE DOWN
        GestionFct(53);           break;
    case 0x47:                                                   // HOME
        GestionFct(54);           break;
    case 0x4F:                                                    // END
        GestionFct(55);           break;
    case 0x3B:                                                     // F1
        GestionFct(1);            break;
    case 0x3C:                                                     // F2
        GestionFct(79);           break;
    case 0x3D:                                                     // F3
        GestionFct(7);            break;
    case 0x3E:                                                     // F4
        GestionFct(9);            break;
    case 0x3F:                                                     // F5
        GestionFct(10);           break;
    case 0x40:                                                     // F6
        GestionFct(11);           break;
    case 0x41:                                                     // F7
        GestionFct(12);           break;
    case 0x42:                                                     // F8
        GestionFct(13);           break;
    case 0x55:                                               // SHIFT-F2
        GestionFct(39);           break;
    case 0x56:                                               // SHIFT-F3
        GestionFct(8);            break;
    case 0x8D:                                                // CTRL-UP
        GestionFct(8);            break;
    case 0x57:                                               // SHIFT-F4
        GestionFct(59);           break;
    case 0x58:                                               // SHIFT-F5
        GestionFct(61);           break;
    case 0x59:                                               // SHIFT-F6
        GestionFct(44);           break;
    case 0x5C:                                               // SHIFT-F9
        if (Cfg->TailleX==80)
            Cfg->TailleX=90;
            else
            Cfg->TailleX=80;
        ChangeTaille(Cfg->TailleY);
        break;
    case 0x5E:                                                // CTRL-F1
        GestionFct(14);           break;
    case 0x5F:                                                // CTRL-F2
        GestionFct(15);           break;
    case 0x60:                                                // CTRL-F3
        GestionFct(22);           break;
    case 0x61:                                                // CTRL-F4
        GestionFct(23);           break;
    case 0x62:                                                // CTRL-F5
        GestionFct(24);           break;
    case 0x63:                                                // CTRL-F6
        GestionFct(25);           break;
    case 0x64:                                                // CTRL-F7
        GestionFct(26);           break;
    case 0x65:                                                // CTRL-F8
        GestionFct(33);           break;
    case 0x68:                                                 // ALT-F1
        GestionFct(49);           break;
    case 0x69:                                                 // ALT-F2
        GestionFct(50);           break;
    case 0x6A:                                                 // ALT-F3
        GestionFct(21);           break;
    case 0x6B:                                                 // ALT-F4
        GestionFct(71);           break;
    case 0x6D:                                                 // ALT-F6
        GestionFct(69);           break;
    case 0x6E:                                                 // ALT-F7
        GestionFct(5);            break;
    case 0x6F:                                                 // ALT-F8
        GestionFct(48);           break;
    case 0x70:                                                 // ALT-F9
        GestionFct(47);           break;
    case 0x71:                                                 // ALT-F9
        GestionFct(60);           break;
    case 0x73:                                              // CTRL LEFT
        GestionFct(41);           break;
    case 0x74:                                             // CTRL RIGHT
        GestionFct(46);           break;
    case 0x76:                                              // CTRL PGDN
        GestionFct(40);           break;
    case 0x84:                                              // CTRL PGUP
        GestionFct(41);           break;
    case 0x86:                                                    // F12
        GestionFct(42);           break;
    case 0x88:                                              // SHIFT-F12
        GestionFct(6);            break;
    case 0x8A:                                               // CTRL-F12
        GestionFct(17);           break;
    case 0x91:                                              // CTRL-DOWN
        GestionFct(64);           break;
    case 0x17:                                                  // ALT-I
        GestionFct(38);           break;
    case 0xA1:                                               // ALT-PGDN
        GestionFct(51);           break;
    case 0x8C:                                                // ALT-F12
        GestionFct(65);           break;

    //case 0x3C:
    //     EjectCD(DFen);
    //     break;
    case 0xB6:                                   //
    case 0xB7:                                   //  Windows 95 keyboard
    case 0xB8:                                   //
        GestionFct(69);
        break;

    case 0x2F:                                                  // ALT-V
        SpeedTest();               break;

    case 0x21:                                                  // ALT-F
    case 0x19:                                                  // ALT-P
    case 0x20:                                                  // ALT-D
    case 0x1F:                                                  // ALT-S
    case 0x14:                                                  // ALT-T
    case 0x18:                                                  // ALT-O
    case 0x23:                                                  // ALT-H
        i=GestionBar(car2);
        if (i==20)
            car2=0x44;                                            // F10
            else
            GestionFct(i);
        break;
    case 0x43:                                                     // F9
        i=GestionBar(0);

        if (i==20)
            car2=0x44;                                            // F10
            else
            GestionFct(i);
        break;

    default:
        if (Cfg->debug==1)
               PrintAt(78,0,"%02X",car2);
        break;
    }                                                   // switch (car2)

if ( (car2==0x44) & (KKCfg->confexit==1) )
    {
    if (WinMesg("Quit KKC","Do you really want to quit KKC ?",1)!=0)
        car2=0;
    }
}
while(car2!=0x44);      // F10
}


/*--------------------------------------------------------------------*\
|-                              DOS Shell                             -|
\*--------------------------------------------------------------------*/
void Shell(char *string,...)
{
char sortie[256];
va_list arglist;
char *suite;
// int n,m,l;
// unsigned ndrv;
// static char Ch[256];

suite=sortie;

va_start(arglist,string);
vsprintf(sortie,string,arglist);
va_end(arglist);

/*
for(n=1;n<strlen(suite);n++)
    if (suite[n]==32)
        {
        m=n;
        break;
        }
l=0;
for(n=1;n<m;n++)
    if (suite[n]=='\\')
        l=n;

if (l!=0)
    {
    strcpy(Ch,suite);
    Ch[l]=0;
    if (Ch[2]==':')
        {
        _dos_setdrive(toupper(Ch[1])-'A'+1,&ndrv);
        placedrive=0;
        }
    chdir(Ch+1);
    strcpy(suite+1,suite+l+1);
    WinMesg(Ch+1,suite,0);
    placepath=0;
    }
*/

if (strlen(suite+1)>120)    //-- La ligne de commande est trop grande --
    {
    static char filename[256];
    FILE *fic;

    strcpy(filename,Fics->trash);
    Path2Abs(filename,"z.bat");

    fic=fopen(filename,"wt");
    fprintf(fic,
          "@REM *-------------------------------------------------*\n");
    fprintf(fic,
          "@REM * Batch file created by Ketchup Killers Commander *\n");
    fprintf(fic,
          "@REM * when command line is too high                   *\n");
    fprintf(fic,
          "@REM *-------------------------------------------------*\n");

    fprintf(fic,"@%s\n",suite+1);
    fclose(fic);

//    WinError("command line too high");

    strcpy(suite+1,filename);
    }

if (KKCfg->KeyAfterShell==0)
    suite[0]='#';

memcpy(SpecSortie,suite,256);
Fin();
}



void PlaceDrive(void)
{
unsigned ndrv;

//if (placedrive)
_dos_setdrive(toupper(DFen->path[0])-'A'+1,&ndrv);

//if (placepath)
chdir(DFen->path);
}


/*--------------------------------------------------------------------*\
|-                        Fin d'execution                             -|
\*--------------------------------------------------------------------*/
void Fin(void)
{
int n;

if (saveconfig)
    SaveCfg();

PlaceDrive();

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

TXTMode();                              // Retablit le mode texte normal

for (n=0;n<8000;n++)
    Screen_Adr[n]=Screen_Buffer[n];

if (KKCfg->_4dos==1)
    _4DOSShistdir();

GotoXY(0,PosY);

if (*SpecSortie==0)
    {
    #ifdef DEBUG
        cprintf("%s / RedBug (DEBUG MODE)\n\r",RBTitle);
    #else
        cprintf("%s / RedBug\n\r",RBTitle);
    #endif
    }

if (SpecMessy!=NULL)
    cprintf("\n\r%s\n\r",SpecMessy);

memcpy(ShellAdr,SpecSortie,256);

DesinitScreen();

exit(1);
}

/*--------------------------------------------------------------------*\
|- A appeller pour chaque cas de changement de layout                 -|
\*--------------------------------------------------------------------*/
void AfficheTout(void)
{
int n;

DFen=Fenetre[(KKCfg->FenAct)&1];

CommandLine("##INIT 0 %d %d\n",(Cfg->TailleY)-2,Cfg->TailleX);

for(n=0;n<NBWIN;n++)
    {
    if (Fenetre[n]->x!=0)
        Fenetre[n]->x=Cfg->TailleX-40;
    }

PrintAt(0,0,"%-40s%*s",RBTitle,Cfg->TailleX-40,"RedBug");
ColLin( 0,0,40,1*16+5);
ColLin(40,0,(Cfg->TailleX)-40,1*16+3);
ColLin(0,(Cfg->TailleY)-2,Cfg->TailleX,5);

DFen->init=1;
DFen->Fen2->init=1;

ChangeLine();

MenuBar(4);
}


/*--------------------------------------------------------------------*\
|-                         Gestion 4DOS                               -|
|-  Put KKCfg->_4dos on if 4dos found                                 -|
\*--------------------------------------------------------------------*/
/*
void _4DOSverif(void)
{
union REGS R;

R.w.ax=0xD44D;
R.h.bh=0;

int386(0x2F,&R,&R);

if (R.w.ax==0x44DD)
    {
    KKCfg->_4dos=1;
//  PrintAt(0,0,"Found 4DOS V%d.%d",R.h.bl,R.h.bh);
    }
    else
    {
    KKCfg->_4dos=0;
    }
}

void _4DOSLhistdir(void)
{
unsigned short seg;
unsigned short *adr;

char a;

register unsigned char n;

union REGS R;

R.w.ax=0xD44D;
R.h.bh=0;

int386(0x2F,&R,&R);

if (R.w.ax==0x44DD)
    {
    KKCfg->_4dos=1;

    seg=R.w.cx;
    adr=(unsigned short*)(seg*16+0x290);

    seg=adr[1];
    adr=(unsigned short*)(seg*16+0x4C60);

    for (n=0;n<255;n++)
        {
        a=((char*)adr)[n];
        KKCfg->HistDir[n]=a;
        }
    }
    else
    {
    KKCfg->_4dos=0;
    }
}
*/

void _4DOSShistdir(void)
{
unsigned short seg;
unsigned short *adr;

char a;

register unsigned char n;

union REGS R;

R.w.ax=0xD44D;
R.h.bh=0;

int386(0x2F,&R,&R);

if (R.w.ax==0x44DD)
    {
    KKCfg->_4dos=1;

    seg=R.w.cx;
    adr=(unsigned short*)(seg*16+0x290);

    seg=adr[1];
    adr=(unsigned short*)(seg*16+0x4C60);

    for (n=0;n<255;n++)  {
        a=KKCfg->HistDir[n];
        ((char*)adr)[n]=a;
        }
    }
    else
    {
    KKCfg->_4dos=0;
    }
}

/*--------------------------------------------------------------------*\
|-                      Save Configuration File                       -|
\*--------------------------------------------------------------------*/
void SaveCfg(void)
{
int m,n,t,ns;
// short court;
FILE *fic;
FENETRE *Fen;
long taille;

for (t=0;t<NBWIN;t++)
    KKCfg->FenTyp[t]=Fenetre[t]->FenTyp;

fic=fopen(Fics->CfgFile,"wb");
if (fic==NULL)
    GestionFct(77);

fwrite((void*)Cfg,sizeof(struct config),1,fic);

fwrite((void*)KKCfg,sizeof(struct kkconfig),1,fic);

for(n=0;n<16;n++)
    {
    fwrite(&(Mask[n]->Ignore_Case),1,1,fic);
    fwrite(&(Mask[n]->Other_Col),1,1,fic);
    taille=strlen(Mask[n]->chaine);
    fwrite(&taille,2,1,fic);
    fwrite(Mask[n]->chaine,taille,1,fic);
    taille=strlen(Mask[n]->title);
    fwrite(&taille,2,1,fic);
    fwrite(Mask[n]->title,taille,1,fic);
    }


for(t=0;t<NBWIN;t++)
    {
    Fen=Fenetre[t];

    fwrite(Fen->path,256,1,fic);                   // Repertoire courant
    fwrite(&(Fen->order),sizeof(short),1,fic);       // Ordre du sorting
    fwrite(&(Fen->sorting),sizeof(short),1,fic);      // Type de sorting

    fwrite(&(Fen->nbrsel),4,1,fic);     // Nombre de fichier selectionne
    ns=Fen->nbrsel;

    for (n=0;n<Fen->nbrfic;n++)
        {
        if (Fen->F[n]->select==1)
            {
            ns--;
            m=strlen(Fen->F[n]->name);
            fwrite(&m,4,1,fic);                       // Longueur du nom
            fwrite(Fen->F[n]->name,1,m,fic);      // Fichier selectionn‚
            if (ns==0) break;
            }
        }

    m=strlen(Fen->F[Fen->pcur]->name);
    fwrite(&m,4,1,fic);                               // Longueur du nom
    fwrite(Fen->F[Fen->pcur]->name,1,m,fic);          // Fichier courant

    fwrite(&(Fen->scur),sizeof(short),1,fic);// Pos du fichier … l'ecran
    }

fclose(fic);
}


/*--------------------------------------------------------------------*\
|-                     Load Configuration File                        -|
|- Retourne -1 en cas d'erreur                                        -|
|-           0 si tout va bien                                        -|
\*--------------------------------------------------------------------*/
int LoadCfg(void)
{
int m,n,i,t,nbr;
FILE *fic;
char nom[256];
short taille;

fic=fopen(Fics->CfgFile,"rb");
if (fic==NULL) return -1;

if (fread((void*)Cfg,sizeof(struct config),1,fic)==0)
    {
    fclose(fic);
    return -1;
    }

Cfg->UseFont=0;

if (Cfg->crc!=0x69)
    {
    fclose(fic);
    return -1;
    }

if (fread((void*)KKCfg,sizeof(struct kkconfig),1,fic)==0)
    {
    fclose(fic);
    return -1;
    }

if ( (KKCfg->overflow1!=0) | (KKCfg->crc!=0x69) )
    {
    fclose(fic);
    return -1;
    }

if (KKCfg->palafter!=1)
    LoadPal();

for(n=0;n<16;n++)
    {
    fread(&(Mask[n]->Ignore_Case),1,1,fic);
    fread(&(Mask[n]->Other_Col),1,1,fic);
    fread(&taille,2,1,fic);
    fread(Mask[n]->chaine,taille,1,fic);
    Mask[n]->chaine[taille]=0;
    fread(&taille,2,1,fic);
    fread(Mask[n]->title,taille,1,fic);
    Mask[n]->title[taille]=0;
    }

for (t=0;t<NBWIN;t++)
    {
    DFen=Fenetre[t];

    DFen->FenTyp=KKCfg->FenTyp[t];

    fread(DFen->path,256,1,fic);                   // Repertoire courant
    fread(&(DFen->order),sizeof(short),1,fic);       // Ordre du sorting
    fread(&(DFen->sorting),sizeof(short),1,fic);      // Type de sorting

    IOver=1;
    IOerr=0;

    if (DFen->path[1]!=':')
        strcpy(DFen->path,Fics->LastDir);

    CommandLine("#cd .");

    IOver=0;

    fread(&nbr,4,1,fic);                // Nombre de fichier selectionne

    DFen->nbrsel=0;

    for (i=0;i<nbr;i++)
        {
        fread(&m,4,1,fic);                            // Longueur du nom
        fread(nom,m,1,fic);                       // Fichier selectionn‚
        nom[m]=0;

        for (n=0;n<DFen->nbrfic;n++)
            if (!stricmp(nom,DFen->F[n]->name))
                FicSelect(n,1);
        }

    fread(&m,4,1,fic);                                // Longueur du nom
    fread(nom,m,1,fic);                               // Fichier courant
    nom[m]=0;

    fread(&(DFen->scur),sizeof(short),1,fic); //Pos du fichier … l'ecran

    for (n=0;n<DFen->nbrfic;n++)
        if (!stricmp(nom,DFen->F[n]->name))
            DFen->pcur=n;
    }

fclose(fic);

return 0;
}

/*--------------------------------------------------------------------*\
|-   Affichage des fenˆtres                                           -|
\*--------------------------------------------------------------------*/
void AffFen(FENETRE *Fen)
{
int x,y;

int nbuf,nscr;

int x1,x2;

if ( (Fen->init==1) | (Fen->FenTyp==2) )
    AffLonger();

switch (Fen->FenTyp)
    {
    case 0:
        FenNor(Fen);
        break;
    case 1:                        // FenDIZ --> A lieu au moment de IDF
        // FenDIZ(Fen);
        break;
    case 2:      // OFF
        x1=Fen->x;
        x2=Fen->x+Fen->xl;

        if (x2>=OldX)
            {
            Window(OldX,Fen->y,x2,Fen->y+Fen->yl,10*16+1);
            x2=OldX-1;
            }

        for(y=0;y<=Fen->yl;y++)
            {
            nscr=(y+Fen->y)*160+(Fen->x)*2;
            nbuf=(y-Fen->yl+OldY-1)*OldX+(Fen->x);

            for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
            if (nbuf<0)
                AffCol(x+x1,y+Fen->y,7);
            else
                AffCol(x+x1,y+Fen->y,Screen_Buffer[nbuf*2+1]);
            }
        for(y=0;y<=Fen->yl;y++)
            {
            nscr=(y+Fen->y)*160+(Fen->x)*2;
            nbuf=(y-Fen->yl+OldY-1)*OldX+(Fen->x);

            for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
            if (nbuf<0)
              AffChr(x+x1,y+Fen->y,32);
            else
              AffChr(x+x1,y+Fen->y,CnvASCII(0,Screen_Buffer[nbuf*2]));
            }

        break;
    case 3:
        FenInfo(Fen);
        break;
    case 4:
        FenDisk(Fen);
        break;
    case 5:               // Fen FILE_ID.DIZ --> A lieu au moment de IDF
        // FenFileID(Fen);
        break;
    case 6:
        FenTree(Fen);
        break;
    }

}

/*--------------------------------------------------------------------*\
|- Affichage de la fenetre du milieu                                  -|
\*--------------------------------------------------------------------*/
void AffLonger(void)
{
int nbuf,nscr;
int x,y;
char chaine[9];

int x1,x2;

x1=40;
x2=49;

if (Cfg->TailleX==80) return;

if ( (DFen->Fen2->FenTyp==2) & (DFen->FenTyp==2) )
    {
    for(y=0;y<=DFen->yl;y++)
        {
        nscr=(y+DFen->y)*160+x1*2;
        nbuf=(y-DFen->yl+OldY-1)*OldX+x1;

        for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
            if (nbuf<0)
                AffCol(x+x1,y+DFen->y,7);
            else
                AffCol(x+x1,y+DFen->y,Screen_Buffer[nbuf*2+1]);
        }
    for(y=0;y<=DFen->yl;y++)
        {
        nscr=(y+DFen->y)*160+x1*2;
        nbuf=(y-DFen->yl+OldY-1)*OldX+x1;

        for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
            if (nbuf<0)
                AffChr(x+x1,y+DFen->y,32);
            else
               AffChr(x+x1,y+DFen->y,CnvASCII(0,Screen_Buffer[nbuf*2]));
        }
    }
    else
    {
    chaine[8]=0;

    for (x=0;x<6;x++)
        {
        WinCadre(x1, 1+x*3,x2, 3+x*3,2);
        ColLin(x1+1,2+x*3,8,10*16+4);
        memcpy(chaine,KKCfg->Qmenu+x*8,8);
        PrintAt(x1+1,2+x*3 ,chaine);
        }

    Window(x1,19,x2,Cfg->TailleY-6,10*16+4);          // Efface le reste

    WinCadre(x1,Cfg->TailleY-5,x2,Cfg->TailleY-3,2);
    ColLin(x1+1,Cfg->TailleY-4,8,10*16+4);

    }

}



/*--------------------------------------------------------------------*\
 -                                                                    -
 -     ----------------------------------------------------------     -
 -     -                         MAIN                           -     -
 -     ----------------------------------------------------------     -
 -                                                                    -
\*--------------------------------------------------------------------*/

void main(int argc,char **argv)
{
char *path;
int n;
char *LC;

/*--------------------------------------------------------------------*\
|-                     Initialisation de l'ecran                      -|
\*--------------------------------------------------------------------*/

/*
printf("char : %d\n",sizeof(char));
printf("short: %d\n",sizeof(short));
printf("long : %d\n",sizeof(long));
printf("int  : %d\n",sizeof(int));
printf("config : %d\n",sizeof(struct config));
getch();
*/

Cfg=(struct config*)GetMem(sizeof(struct config));

OldX=(*(char*)(0x44A));
OldY=(*(char*)(0x484))+1;

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;                  // Initialisation de la taille ecran

InitScreen(0);                     // Initialise toutes les donn‚es HARD

WhereXY(&PosX,&PosY);

/*--------------------------------------------------------------------*\
|- Initialisation du temps                                            -|
\*--------------------------------------------------------------------*/

Info=(struct RB_info*)GetMem(sizeof(struct RB_info));   // Starting time
Info->temps=clock();

/*--------------------------------------------------------------------*\
|-                       Initialise les buffers                       -|
\*--------------------------------------------------------------------*/

KKCfg=(struct kkconfig*)GetMem(sizeof(struct kkconfig));

for(n=0;n<NBWIN;n++)
    {
    Fenetre[n]=(FENETRE*)GetMem(sizeof(FENETRE));
    Fenetre[n]->F=(struct file**)GetMem(TOTFIC*sizeof(void *));
    }


Fics=(struct fichier*)GetMem(sizeof(struct fichier));
KKFics=(struct kkfichier*)GetMem(sizeof(struct kkfichier));

Mask=(struct PourMask**)GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    Mask[n]=(struct PourMask*)GetMem(sizeof(struct PourMask));

Screen_Buffer=(char*)GetMem(8000);

for (n=0;n<8000;n++)
    Screen_Buffer[n]=Screen_Adr[n];

path=(char*)GetMem(256);

/*--------------------------------------------------------------------*\
|-                Lecture et verification des arguments               -|
\*--------------------------------------------------------------------*/

strcpy(ShellAdr+128,*argv);
strcpy(path,*argv);
for (n=strlen(path);n>0;n--)
    {
    if (path[n]==DEFSLASH)
        {
        path[n]=0;
        break;
        }
    }

LC=*(argv+argc-1);

if (strncmp(LC,"6969",4))
    {
    printf("This program cannot be run in DOS mode\n");
    printf("This program cannot be run in WINDOWS mode\n");
    printf("This program cannot be run in OS/2 mode\n");
    printf("This program cannot be run in LINUX mode\n\n");
    printf("This program required KK.EXE\n\n");
    exit(1);
    }



/*--------------------------------------------------------------------*\
|-                          Gestion des erreurs                       -|
\*--------------------------------------------------------------------*/

_harderr(Error_handler);

signal(SIGBREAK,Signal_Handler);
signal(SIGABRT,Signal_Handler);
signal(SIGFPE,Signal_Handler);
signal(SIGILL,Signal_Handler);
signal(SIGINT,Signal_Handler);
signal(SIGSEGV,Signal_Handler);
signal(SIGTERM,Signal_Handler);


/*--------------------------------------------------------------------*\
|-                      Initialisation des fichiers                   -|
\*--------------------------------------------------------------------*/

SetDefaultPath(path);

Fics->help=(char*)GetMem(256);
strcpy(Fics->help,path);
Path2Abs(Fics->help,"kkc.hlp");

KKFics->menu=(char*)GetMem(256);
strcpy(KKFics->menu,Fics->trash);
Path2Abs(KKFics->menu,"kkc.mnu");


/*--------------------------------------------------------------------*\
|-                               Default                              -|
\*--------------------------------------------------------------------*/

ChangeType(4);

/*
KKCfg->_4dos=0;
_4DOSverif();

if (KKCfg->_4dos==1)
    {
    _4DOSLhistdir();
    }
    else */
    memset(KKCfg->HistDir,0,256);




/*--------------------------------------------------------------------*\
|-                            Initialisation                          -|
\*--------------------------------------------------------------------*/

Fenetre[0]->x=0;
Fenetre[0]->actif=1;
Fenetre[0]->nfen=0;
Fenetre[0]->FenTyp=0;
Fenetre[0]->Fen2=Fenetre[1];

Fenetre[1]->x=40;
Fenetre[1]->actif=0;
Fenetre[1]->nfen=1;
Fenetre[1]->FenTyp=0;
Fenetre[1]->Fen2=Fenetre[0];

Fenetre[2]->x=40;
Fenetre[2]->actif=0;
Fenetre[2]->nfen=2;
Fenetre[2]->FenTyp=0;                      // Fenˆtre ferm‚e par default
Fenetre[2]->Fen2=Fenetre[2];

Fenetre[3]->x=40;
Fenetre[3]->actif=0;
Fenetre[3]->nfen=3;
Fenetre[3]->FenTyp=0;                      // Fenˆtre ferm‚e par default
Fenetre[3]->Fen2=Fenetre[3];


for (n=0;n<NBWIN;n++)
    {
    Fenetre[n]->y=1;
    Fenetre[n]->yl=(Cfg->TailleY)-4;
    Fenetre[n]->xl=39;

    Fenetre[n]->order=17;
    Fenetre[n]->pcur=0;
    Fenetre[n]->scur=0;
    }


/*--------------------------------------------------------------------*\
|-                Chargement du fichier config (s'il existe)          -|
\*--------------------------------------------------------------------*/

if (LoadCfg()==-1)
    {
    remove(Fics->CfgFile);
    saveconfig=0;
    GestionFct(62);
    }

/*--------------------------------------------------------------------*\
|- verification system                                                -|
\*--------------------------------------------------------------------*/

KKCfg->_Win95=Verif95();

if (KKCfg->_Win95==1)
    SetWindowsTitle();

InitMouse();

/*--------------------------------------------------------------------*\
|- Chargement du repertoire courant si necessaire                     -|
\*--------------------------------------------------------------------*/

if ( (KKCfg->currentdir==1) & (LC[4]!='0') )
    {
    DFen=Fenetre[(KKCfg->FenAct)&1];

    if ( (strcmp(Fics->LastDir,Fenetre[0]->path)!=0) &
         (strcmp(Fics->LastDir,Fenetre[1]->path)!=0) )
        CommandLine("#CD %s",Fics->LastDir);
    }

UseCfg();                      // Emploi les parametres de configuration

InitScreen(Cfg->display);

Fenetre[0]->yl=(Cfg->TailleY)-4;
Fenetre[1]->yl=(Cfg->TailleY)-4;

if (KKCfg->verifhist==1)
    VerifHistDir();            // Verifie l'history pour les repertoires

Gestion();

/*--------------------------------------------------------------------*\
|-                                  FIN                               -|
\*--------------------------------------------------------------------*/

memset(SpecSortie,0,256);

Fin();
}


void GetFreeMem(char *buffer);
#pragma aux GetFreeMem = \
    "mov ax,0500h" \
    "int 31h" \
    modify [edi eax] \
    parm [edi];

void PrintMem(void)
{
int tail[12];
GetFreeMem((char*)tail);  // inconsistent ?
PrintAt(0,0,"Memory: %d octets",tail[0]);
}

void SwapLong(long *a,long *b)
{
long c;

c=(*a);
(*a)=(*b);
(*b)=c;
}

void SwapWin(long a,long b)
{
FENETRE *tfen;

SwapLong(&(Fenetre[a]->x),&(Fenetre[b]->x));
SwapLong(&(Fenetre[a]->y),&(Fenetre[b]->y));
SwapLong(&(Fenetre[a]->xl),&(Fenetre[b]->xl));
SwapLong(&(Fenetre[a]->yl),&(Fenetre[b]->yl));
SwapLong(&(Fenetre[a]->x2),&(Fenetre[b]->x2));
SwapLong(&(Fenetre[a]->y2),&(Fenetre[b]->y2));
SwapLong(&(Fenetre[a]->xl2),&(Fenetre[b]->xl2));
SwapLong(&(Fenetre[a]->yl2),&(Fenetre[b]->yl2));
SwapLong(&(Fenetre[a]->x3),&(Fenetre[b]->x3));
SwapLong(&(Fenetre[a]->y3),&(Fenetre[b]->y3));
Fenetre[a]->init=1;
Fenetre[b]->init=1;

tfen=Fenetre[a];
Fenetre[a]=Fenetre[b];
Fenetre[b]=tfen;

Fenetre[a]->nfen=(char)a;
Fenetre[b]->nfen=(char)b;
}


