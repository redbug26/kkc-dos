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

void SaveSel(FENETRE *F1);
void LoadSel(int n);

/*--------------------------------------------------------------------*\
|-  Declaration des variables                                         -|
\*--------------------------------------------------------------------*/

int OldCol;                                 // Ancienne couleur du texte
char OldY,PosX,PosY;


sig_atomic_t signal_count;

char *SpecMessy=NULL;
char SpecSortie[256];

char Select_Chaine[32]="*.*";

char *Screen_Buffer;

char *Keyboard_Flag1=(char*)0x417;

char *ShellAdr=(char*)0xBA000;

char *Screen_Adr=(char*)0xB8000;

FENETRE *Fenetre[NBWIN];

FENETRE *DFen;


extern int IOver;
extern int IOerr;


/*--------------------------------------------------------------------*\
|-  Procedure en Assembleur                                           -|
\*--------------------------------------------------------------------*/

char GetDriveReady(char i);
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
    parm [dl] \
    value [cl];


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
                     { 0, 0, 0, 25,25,25,  0, 0, 0, 63,63,63,
                      63,63,63, 63,32,32, 40,40,56,  0, 0,43,
                      63,63, 0, 63,63,63,  0, 0,43, 57,12,28,
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

SaveEcran();
PutCur(32,0);

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


ChargeEcran();

}

/*--------------------------------------------------------------------*\
|-  Programme de setup                                                -|
\*--------------------------------------------------------------------*/



void Signal_Handler(int sig_no)
{
signal_count++;

SpecMessy="You have pressed on Control Break :(";

memset(SpecSortie,0,256);

Fin();
}

void SecretPart(void)
{
int py=1,log=0;
int x;
char buffer[78];
char fin=0;

log=0;

SaveEcran();
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

ChargeEcran();
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
|- 38: Fen�tre information                                            -|
|- 39: Changement d'attribut                                          -|
|- 40: Fenetre history des directories                                -|
|- 41: Va dans le r�p�rtoire pere                                     -|
|- 42: Va dans le r�p�rtoire trash                                    -|
|- 43: Active la fenetre principal (Cfg->FenAct)                      -|
|- 44: Rename Window                                                  -|
|- 45: Internal editor                                                -|
|- 46: Selection du fichier en tant que repertoire courant            -|
|- 47: Switch le mode ecran (nombre de lignes)                        -|
|- 48: Switch le type d'ecran (watcom, norton, ...)                   -|
|- 49: Change le drive de la fenetre 0                                -|
|- 50: Change le drive de la fenetre 1                                -|
|- 51: Va dans le repertoire d'o� l'on a lanc� KK                     -|
|- 52: 10 lignes avant                                                -|
|- 53: 10 lignes apres                                                -|
|- 54: Premiere ligne                                                 -|
|- 55: Derniere ligne                                                 -|
|- 56: Va dans la premiere fen�tre                                    -|
|- 57: Va dans la deuxieme fen�tre                                    -|
|- 58: Change la fen�tre avec la path de l'autre fen�tre              -|
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
\*--------------------------------------------------------------------*/

void GestionFct(int fct)
{
FENETRE *FenOld;
char buffer[256];
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
        if (Cfg->vieweur[0]==0)
           GestionFct(8);
           else
           CommandLine("#%s %s",Cfg->vieweur,DFen->F[DFen->pcur]->name);
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
        if (Cfg->autoreload==1)
            {
            DFen=Fenetre[0];
            GestionFct(27);
            DFen=Fenetre[1];
            GestionFct(27);
            }
        GestionFct(43);     // Active la fenetre principal (Cfg->FenAct)
        break;
    case 11:                                                     // Move
        Move(DFen,Fenetre[2],DFen->Fen2->path);    // Fen[2] non visible
        if (Cfg->autoreload==1)
            {
            DFen=Fenetre[0];
            GestionFct(27);
            DFen=Fenetre[1];
            GestionFct(27);
            }
        GestionFct(43);     // Active la fenetre principal (Cfg->FenAct)
        break;
    case 12:                                         // Create Directory
        CreateDirectory();
        break;
    case 13:                                         // Delete selection
        SaveSel(DFen);
        Delete(DFen);
        CommandLine("#cd .");
        LoadSel(0);
        if (Cfg->autoreload==1)
            {
            DFen=DFen->Fen2;
            GestionFct(27);
            }
        GestionFct(43);     // Active la fenetre principal (Cfg->FenAct)
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
            Cfg->FenAct=1;
            DFen=Fenetre[Cfg->FenAct];
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
            Cfg->FenAct=0;
            DFen=Fenetre[Cfg->FenAct];
            ChangeLine();                              // Affichage Path
            }
        break;
    case 16:                                      // Select current file
        FicSelect(DFen->pcur,2);

        if (!strcmp(DFen->F[DFen->pcur]->name,"."))
            for (i=0;i<DFen->nbrfic;i++)
                if (!WildCmp(DFen->F[i]->name,Select_Chaine))
                    FicSelect(i,1);

        if (Cfg->insdown==1)
            GestionFct(36);
        break;
    case 17:                                           // Change palette
        ChangePalette(0);
        break;
    case 18:                                                    // About
        WinMesg("About %s /RedBug",RBTitle);
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
        break;
    case 23:                                        // Sort by extension
        DFen->order&=16;
        DFen->order|=2;
        SortFic(DFen);
        break;
    case 24:                                             // Sort by date
        DFen->order&=16;
        DFen->order|=3;
        SortFic(DFen);
        break;
    case 25:                                             // Sort by size
        DFen->order&=16;
        DFen->order|=4;
        SortFic(DFen);
        break;
    case 26:                                        // Sort by unsort ;)
        DFen->order&=16;
        SortFic(DFen);
        GestionFct(27);         //--- Reload ---------------------------
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
        break;
    case 34:                                         // Nettoie la trash
        strcpy(buffer,Fics->trash);
        Path2Abs(buffer,"kktrash.sav");

        fic=fopen(buffer,"rt");
        while(fgets(buffer,256,fic)!=NULL)
            {
            buffer[strlen(buffer)-1]=0;     // Retire le caractere ENTER
            remove(buffer);
            }
        fclose(fic);

        strcpy(buffer,Fics->trash);
        Path2Abs(buffer,"kktrash.sav");
        remove(buffer);

        if (Cfg->autoreload==1)
            {
            DFen=Fenetre[0];
            GestionFct(27);
            DFen=Fenetre[1];
            GestionFct(27);
            }
        Cfg->strash=0;
        GestionFct(43);     // Active la fenetre principal (Cfg->FenAct)
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
        DFen->init=1;

        if (DFen->FenTyp==3)
            {
            DFen->FenTyp=0;
            DFen->Fen2->FenTyp=0;
            DFen->Fen2->init=1;
            }
        else
            {
            DFen->FenTyp=3;
            DFen->Fen2->FenTyp=2;
            }
        break;
    case 39:                                     // Change les attributs
        WinAttrib();
        if (Cfg->autoreload==1)
            {
            DFen=Fenetre[0];
            GestionFct(27);
            DFen=Fenetre[1];
            GestionFct(27);
            }
        GestionFct(43);     // Active la fenetre principal (Cfg->FenAct)
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
        Cfg->FenAct=(Cfg->FenAct)&1;
        DFen=Fenetre[Cfg->FenAct];
        ChangeLine();                                  // Affichage Path
        break;
    case 44:                               // Fenetre pour renomation ;)
        WinRename(DFen,DFen->Fen2);
        if (Cfg->autoreload==1)
            {
            DFen=Fenetre[0];
            GestionFct(27);
            DFen=Fenetre[1];
            GestionFct(27);
            }
        GestionFct(43);     // Active la fenetre principal (Cfg->FenAct)
        break;
    case 45:                                          // Internal editor
        switch(DFen->system)
            {
            case 0:
                Edit(DFen);
                break;
            }
        break;
    case 46:      // Selection du fichier en tant que repertoire courant
        if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
                CommandLine("#cd %s",DFen->F[DFen->pcur]->name);
        break;
    case 47:                  // Switch le mode ecran (nombre de lignes)
        switch(Cfg->TailleY)
            {
            case 25:  Cfg->TailleY=30;  break;
            case 30:  Cfg->TailleY=50;  break;
            default:  Cfg->TailleY=25;  break;
            }
        GestionFct(67);
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
    case 51:               // Va dans le repertoire d'o� l'on a lanc� KK
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
        Cfg->FenAct=0;
        DFen=Fenetre[Cfg->FenAct];
        ChangeLine();                                  // Affichage Path
        break;
    case 57:
        Cfg->FenAct=1;
        DFen=Fenetre[Cfg->FenAct];
        ChangeLine();                                  // Affichage Path
        break;
    case 58:        // Change la fen�tre avec la path de l'autre fen�tre
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
        switch(FicIdf(DFen->F[DFen->pcur]->name,i,1))
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
        Fenetre[0]->yl=(Cfg->TailleY)-4;
        Fenetre[1]->yl=(Cfg->TailleY)-4;
        ChangeLine();
        AfficheTout();
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

SaveEcran();

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

strcpy(bar[0].help,"Various");
strcpy(bar[1].help,"File");
strcpy(bar[2].help,"Disk");
strcpy(bar[3].help,"Commands");
strcpy(bar[4].help,"Tools");
strcpy(bar[5].help,"Archiver");
strcpy(bar[6].help,"Options");

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
   strcpy(bar[1].titre,"Select temporary File   �");  bar[1].fct=19;
   strcpy(bar[2].titre,"                         ");  bar[2].fct=0;
   strcpy(bar[3].titre,"ASCII Table        CTRL-A");  bar[3].fct=28;
   nbmenu=4;
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
   strcpy(bar[1].titre,"About");   bar[1].fct=18;
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

ChargeEcran();

return fin;
}

/*--------------------------------------------------------------------*/



/*--------------------------------------------------------------------*\
|- Fonction ChangeType:                                               -|
|-     Change le type des fen�tres                                    -|
|- (0 pour incrementer le type des fen�tres                           -|
\*--------------------------------------------------------------------*/
void ChangeType(int n)
{
if (n==0)
   Cfg->fentype++;
   else
   Cfg->fentype=n;

if (Cfg->fentype>4) Cfg->fentype=1;
}


/*--------------------------------------------------------------------*\
|-                        Fenetre Select File                         -|
\*--------------------------------------------------------------------*/

void SelectPlus(void)
{
static int DirLength=32;
int i;

struct Tmt T[1] = { { 2,2,1,Select_Chaine,&DirLength} };

struct TmtWin F = {22,8,57,11,"Selection of files"};

if (WinTraite(T,1,&F)==27) return;                             // ESCape

for (i=0;i<DFen->nbrfic;i++)
    if ( (Cfg->seldir==1) |
                         ((DFen->F[i]->attrib & RB_SUBDIR)!=RB_SUBDIR) )
            if (!WildCmp(DFen->F[i]->name,Select_Chaine))
                FicSelect(i,1);
}

/*--------------------------------------------------------------------*\
|-                       Fenetre Deselect File                        -|
\*--------------------------------------------------------------------*/

void SelectMoins(void)
{
static int DirLength=32;
int i;

struct Tmt T[1] = { { 2,2,1,Select_Chaine,&DirLength} };

struct TmtWin F = {22,8,57,11,"Deselection of files"};

if (WinTraite(T,1,&F)==27) return;                             // ESCape

for (i=0;i<DFen->nbrfic;i++)
    if ( (Cfg->seldir==1) |
                         ((DFen->F[i]->attrib & RB_SUBDIR)!=RB_SUBDIR) )
        if (!WildCmp(DFen->F[i]->name,Select_Chaine))
            FicSelect(i,0);
}

/*--------------------------------------------------------------------*\
|-                      Fenetre History Directory                     -|
\*--------------------------------------------------------------------*/

void HistDir(void)
{
int Mlen;

int i,j;
static char **dir;

dir=GetMem(100*sizeof(char *));

j=0;
Mlen=0;
for (i=0;i<100;i++)
    {
    dir[i]=Cfg->HistDir+j;
    dir[i]=strupr(dir[i]);
    if (strlen(dir[i])>Mlen) Mlen=strlen(dir[i]);
    if (strlen(dir[i])==0) break;
    while ( (j!=256) & (Cfg->HistDir[j]!=0) ) j++;
    j++;
    }

if (i!=0)
    {
    int x=2,y=2,pos=i-1,car,max,prem;

    SaveEcran();
    PutCur(32,0);

    max=i;
    if (max>Cfg->TailleY-4) max=Cfg->TailleY-4;

    WinCadre(x-2,y-1,x+Mlen+1,y+max,0);
    Window(x-1,y,x+Mlen,y+max-1,10*16+1);

    prem=0;

    do {

    while((pos-prem)>=max) prem++;
    while((pos-prem)<0) prem--;


    for (j=0;j<max;j++)
        PrintAt(x,y+j,"%-*s",Mlen,dir[j+prem]);

    ColLin(x-1,y+(pos-prem),Mlen+2,7*16+5);

    car=Wait(0,0,0);

    ColLin(x-1,y+(pos-prem),Mlen+2,0*16+1);

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

    ChargeEcran();

    if (car==13)
        CommandLine("#cd %s",dir[pos]);
    }

free(dir);
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

com=GetMem(100*sizeof(char *));

j=0;
Mlen=0;
for (i=0;i<100;i++)
    {
    History2Line(Cfg->HistCom+j,dest);
    dest[76]=0;
    com[i]=GetMem(strlen(dest)+1);
    strcpy(com[i],dest);

    if (strlen(com[i])>Mlen) Mlen=strlen(com[i]);
    if (strlen(com[i])==0) break;
    while ( (j!=512) & (Cfg->HistCom[j]!=0) ) j++;
    j++;
    }

if (i!=0)
    {
    int x=2,y=2,pos=i-1,car,max,prem;

    SaveEcran();
    PutCur(32,0);

    max=i;
    if (max>Cfg->TailleY-4) max=Cfg->TailleY-4;

    WinCadre(x-2,y-1,x+Mlen+1,y+max,0);
    Window(x-1,y,x+Mlen,y+max-1,10*16+1);

    prem=0;

    do {

    while((pos-prem)>=max) prem++;
    while((pos-prem)<0) prem--;


    for (j=0;j<max;j++)
        PrintAt(x,y+j,"%-*s",Mlen,com[j+prem]);

    ColLin(x-1,y+(pos-prem),Mlen+2,7*16+5);

    car=Wait(0,0,0);

    ColLin(x-1,y+(pos-prem),Mlen+2,0*16+1);

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

    ChargeEcran();

    if (car==13)
        {
        History2Line(com[pos],dest);
        CommandLine("%s\n",dest);
        }
    }

for(j=0;j<=i;j++)
    free(com[j]);

free(com);
}


/*--------------------------------------------------------------------*\
|-                         Fenetre CD machin                          -|
\*--------------------------------------------------------------------*/

void WinCD(void)
{
static char Dir[70];
static int DirLength=70;
static int CadreLength=71;

struct Tmt T[5] =
      { { 2,3,1, Dir, &DirLength},
        {15,5,2,NULL,NULL},
        {45,5,3,NULL,NULL},
        { 5,2,0,"Change to which directory",NULL},
        { 1,1,4,NULL,&CadreLength} };

struct TmtWin F = { 3,10,76,17, "Change Directory" };

int n;

n=WinTraite(T,5,&F);

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
static int CadreLength=71;

struct Tmt T[5] =
    { { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the directory to be created",NULL},
      { 1,1,4,NULL,&CadreLength} };

struct TmtWin F = { 3,10,76,17, "Create Directory" };

int n;

n=WinTraite(T,5,&F);

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
if (Cfg->editeur[0]!=0)
    CommandLine("#%s %s",Cfg->editeur,s);
//    else    GestionFct(45);
}

/*--------------------------------------------------------------------*\
|-                           Edit New File                            -|
\*--------------------------------------------------------------------*/

void EditNewFile(void)
{
static char Dir[70];
static int DirLength=70;
static int CadreLength=71;

struct Tmt T[5] =
    { { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the file to be edited",NULL},
      { 1,1,4,NULL,&CadreLength} };

struct TmtWin F = { 3,10,76,17, "Edit New File" };

int n;

n=WinTraite(T,5,&F);

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
static int CadreLength=71;

struct Tmt T[5] =
    { { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the KKD file to be created",NULL},
      { 1,1,4,NULL,&CadreLength} };

struct TmtWin F = { 3,10,76,17,"Create KKD File" };

int n;

n=WinTraite(T,5,&F);

if (n!=27)
    if (T[n].type!=3)
        {
        strcpy(Name,DFen->Fen2->path);

        for (n=strlen(Dir);n>0;n--)
            {
            if (Dir[n]=='\\') break;
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
int fpos,cpos,pos,p1,p2;

char drive[26];
short m,n,x,l,nbr;
short x1;
signed char i,d,olddrive;

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


SaveEcran();

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
        drive[n]=m;
        AffChr(m,9,n+'A');
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
        if (cpos!=50)
            ColLin(x1+3,cpos-fpos,32,10*16+1);

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

                if (path[strlen(path)-1]!='\\') strcat(path,"\\");


             //Calcule le nombre de position occup�e par les repertoires
                pos=12;
                for (p2=0;p2<strlen(path);p2++)
                    if (path[p2]=='\\')
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
        if (path[p2]=='\\')
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



    AffCol(drive[i],9,1*16+5);
    car=Wait(0,0,0);
    AffCol(drive[i],9,0*16+1);

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
    if (path[p2]=='\\')
        {
        if (pos==cpos)
            path[p2]=0;
        pos++;
        }
    }
if (path[strlen(path)-1]==':') strcat(path,"\\");

ChargeEcran();

if (car==13)
   CommandLine("#cd %s",path);
}


/*--------------------------------------------------------------------*\
|-                Change drive of current window for KKD              -|
\*--------------------------------------------------------------------*/
int ChangeToKKD(void)
{
char drive[26];
short m,n,x,l,nbr;
signed char i;

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


SaveEcran();

WinCadre(x-2,6,x+l*nbr+1,11,0);
Window(x-1,7,x+l*nbr,10,0*16+1);

WinCadre(x-1,8,x+l*nbr,10,1);

PrintAt(x,7,"Select KKDdrive");

m=x+l/2;
for (n=0;n<26;n++)
    {
    if (drive[n]==0)
        {
        drive[n]=m;
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

ChargeEcran();

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

    car=tolower(car);

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

    if (car==8) //--- BACKSPACE ----------------------------------------
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

char *AccessFile(void)
{
FENETRE *FenOld;
FILE *fic;
static char nom[256];
static char tnom[256];
static char buffer[256];

char ChangePos=0;

int i;

FenOld=DFen;

strcpy(tnom,DFen->F[DFen->pcur]->name);

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
        DFen=Fenetre[2];  // Copie de l'autre c�t� (t'es content Phil ?)
        CommandLine("#cd %s",Fics->trash);

        Cfg->FenAct= (Cfg->FenAct)+2;

        DFen=FenOld;
        for (i=0;i<DFen->nbrfic;i++)
            FicSelect(i,0);                       // Delesect every file

        for (i=0;i<DFen->nbrfic;i++)
            if (!strncmp(tnom,DFen->F[i]->name,strlen(tnom)))
                {
                DFen->pcur=i;
                DFen->scur=i;

                FicSelect(DFen->pcur,1);
                break;
                }

                                 // Mets le nom du fichier dans la trash
        strcpy(buffer,Fics->trash);
        Path2Abs(buffer,"kktrash.sav");
        fic=fopen(buffer,"at");
        strcpy(buffer,Fics->trash);
        Path2Abs(buffer,DFen->F[DFen->pcur]->name);
        fprintf(fic,"%s\n",buffer);
        fclose(fic);
        Cfg->strash+=DFen->F[DFen->pcur]->size;

                                     // Copie les fichiers dans la trash
        (Cfg->noprompt)=(Cfg->noprompt)|1;
        Copie(DFen,Fenetre[2],Fenetre[2]->path);         // Quitte apr�s

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
    case 102:                                                     // KKD
        DFen->KKDdrive=0;
        DFen->system=5;        break;
    default:
        return i;              break;
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
        if (!stricmp(DFen->F[i]->name,nom))
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
void WinRename(FENETRE *F1,FENETRE *F2)
{
static char Dir[70];
static char Name[256],Temp[256];
static int DirLength=70;
static int CadreLength=71;

struct Tmt T[5] = {
      { 2,3,1, Dir, &DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Move/rename file to",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {3,10,76,17,"Move/rename"};

int n;

strcpy(Dir,F2->path);
Path2Abs(Dir,F1->F[DFen->pcur]->name);

strcpy(Name,F1->path);
Path2Abs(Name,F1->F[DFen->pcur]->name);

n=WinTraite(T,5,&F);

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

unsigned short car3,c;

int i;  //--- Compteur -------------------------------------------------

Info->temps=clock()-Info->temps;

do
    {
    (Cfg->noprompt)=(Cfg->noprompt)&126;        // Retire le dernier bit

    if ( (Cfg->key==0) & (Cfg->strash>=Cfg->mtrash) & (Cfg->mtrash!=0) )
        GestionFct(34);                 // Efface la trash si trop plein

    if ( (Cfg->key==0) & (Cfg->FenAct>1) )
        {
        GestionFct(43);
        }
        else
        {
        if (Cfg->FenAct>1)
            {
            DFen=Fenetre[2];
            ChangeLine();                              // Affichage Path
            }
            else
            DFen=Fenetre[Cfg->FenAct];
        }

    if (Cfg->key==0)             // Switch si le buffer clavier est vide
        {
        switch(DFen->FenTyp)
            {
            case 1:
            case 2:
            case 4:
            case 5:
                if ( ((DFen->Fen2->FenTyp)==1) |
                     ((DFen->Fen2->FenTyp)==2) |
                     ((DFen->Fen2->FenTyp)==4) |
                     ((DFen->Fen2->FenTyp)==5) ) break;
                Cfg->FenAct= (Cfg->FenAct==1) ? 0:1;
                DFen=Fenetre[Cfg->FenAct];
                ChangeLine();                          // Affichage Path
                break;
            }
        }

    DFen->actif=1;
    DFen->Fen2->actif=0;

    for (i=0;i<2;i++)
        if (Fenetre[i]->FenTyp!=3)
            AffFen(Fenetre[i]);

    Cl_Start=clock();
    Cl=clock();

    if (Cfg->key==0)
        {
        c=0;

        while ( (!KbHit()) & (c==0) )
            {
            // PrintMem();

            if ( ((clock()-Cl_Start)>DFen->IDFSpeed)  & (Cl_Start!=0))
                {
                Cl_Start=0;
                InfoIDF(DFen);// information idf sur fichier selectionn�

                if (DFen->FenTyp==1) FenDIZ(DFen);
                if (DFen->Fen2->FenTyp==1) FenDIZ(DFen->Fen2);

                if (DFen->FenTyp==5) FenFileID(DFen);
                if (DFen->Fen2->FenTyp==5) FenFileID(DFen->Fen2);
                }

            if ( ((clock()-Cl)>Cfg->SaveSpeed) & (Cfg->SaveSpeed!=0) )
                c=ScreenSaver();

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

        car3=_bios_keybrd(0x11)/256;

        if (c==0)
            c=Wait(0,0,0);

        Cfg->key=c;

        //--- Positionne le pointeur sur FILE_ID.DIZ si on est sur .. --
        switch(HI(c))  
            {
            case 0x3D:  // F3
            case 0x3E:  // F4
            case 0x56:  // SHIFT-F3
//            case 0x57:  // SHIFT-F4
            case 0x8D:  // CTRL-UP
                if (!strcmp(DFen->F[DFen->pcur]->name,".."))
                    {
                    for (i=0;i<DFen->nbrfic;i++)
                       if (!strnicmp("FILE_ID.DIZ",DFen->F[i]->name,13))
                            {
                            DFen->pcur=i;
                            DFen->scur=i;
                            break;
                            }
                    }
                break;
            }
/*
        //--- Positionne le pointeur sur FILE_ID.DIZ dans fenetre[2] ---
        if (HI(c)==0x6B)
            {
            CommandLine("#CD %s",Fics->trash);
            }
*/


        strcpy(Cfg->FileName,DFen->F[DFen->pcur]->name);

        switch(LO(c))
            {
            case 13:
                Cfg->key=0;
                if (CommandLine("\n")!=0)
                    {
                    c=0;
                    break;
                    }

               if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
                    {
                    CommandLine("#cd %s",DFen->F[DFen->pcur]->name);
                    c=0;
                    break;
                    }
                Cfg->key=c;
                AccessFile();
                break;
            }

        switch(HI(c))
            {
            case 0x3D:                                             // F3
            case 0x3E:                                             // F4
            case 0x56:                                       // SHIFT-F3
//            case 0x57:                                     // SHIFT-F4
            case 0x8D:                                        // CTRL-UP
                AccessFile();
                break;
            }
        }
    else
        {
        c=Cfg->key;
        car3=0;

        for (i=0;i<DFen->nbrfic;i++)
            if (!strnicmp(Cfg->FileName,DFen->F[i]->name,
                strlen(Cfg->FileName)))
                {
                DFen->pcur=i;
                DFen->scur=i;
                break;
                }
        }

    Cfg->key=0;

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
    case 5:                                                    // CTRL-E
        GestionFct(70);        break;
    case 4:                                                    // CTRL-D
        GestionFct(29);        break;
    case 6:                                                    // CTRL-F
        GestionFct(32);        break;
    case 9:                                                       // TAB
        Cfg->FenAct= (Cfg->FenAct==1) ? 0:1;
        DFen=Fenetre[Cfg->FenAct];
        ChangeLine();                                  // Affichage Path
        break;
    case 13:                                                    // ENTER
        ext=getext(DFen->F[DFen->pcur]->name);
        if ( (!stricmp(ext,"COM")) | (!stricmp(ext,"BAT")) |
             ((!stricmp(ext,"BTM")) & (Cfg->_4dos==1)) )
            {
            CommandLine("%s\n",DFen->F[DFen->pcur]->name);
            break;
            }
        switch(i=EnterArchive())
        {
        case 0:                                                    // OK
            break;
        case 57:                                           // Executable
            CommandLine("%s\n",DFen->F[DFen->pcur]->name);
            break;
        default:
            switch(FicIdf(DFen->F[DFen->pcur]->name,i,0))
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
    case 0x15:                                                 // CTRL-U
        GestionFct(68);
        break;
    case 27:                                                   // ESCAPE
        if (CommandLine("\r")==0)
            if (Cfg->Esc2Close==1)
                GestionFct(14),GestionFct(15);
        break;
    case '�':
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
    case 0x54:                                               // SHIFT-F1
        break;
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
    case 0xB6:                                   //
    case 0xB7:                                   //  Windows 95 keyboard
    case 0xB8:                                   //
        GestionFct(69);
//        PacNoe();
        break;

    case 0x2F:                                                  // ALT-V
        SpeedTest();               break;

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

suite=sortie;

va_start(arglist,string);
vsprintf(sortie,string,arglist);
va_end(arglist);

if (Cfg->KeyAfterShell==0)
    suite[0]='#';

memcpy(SpecSortie,suite,256);

Fin();
}



void PlaceDrive(void)
{
unsigned ndrv;

// chdir(DFen->Fen2->path);     // Vraiment pas utile
_dos_setdrive(toupper(DFen->path[0])-'A'+1,&ndrv);
chdir(DFen->path);
}


/*--------------------------------------------------------------------*\
|-                        Fin d'execution                             -|
\*--------------------------------------------------------------------*/
void Fin(void)
{
int n;

SaveCfg();

PlaceDrive();

TXTMode(OldY);                          // Retablit le mode texte normal

for (n=0;n<8000;n++)
    Screen_Adr[n]=Screen_Buffer[n];

if (Cfg->_4dos==1)
    _4DOSShistdir();

GotoXY(0,PosY);

#ifdef DEBUG
    cprintf("%s / RedBug (DEBUG MODE)\n\r",RBTitle);
#else
    cprintf("%s / RedBug\n\r",RBTitle);
#endif

if (SpecMessy!=NULL)
    cprintf("\n\r%s\n\r",SpecMessy);

memcpy(ShellAdr,SpecSortie,256);

DesinitScreen();

exit(1);
}

void AfficheTout(void)
{
DFen=Fenetre[(Cfg->FenAct)&1];

CommandLine("##INIT 0 %d 80\n",(Cfg->TailleY)-2);


PrintAt(0,0,"%-40s%40s",RBTitle,"RedBug");
ColLin( 0,0,40,1*16+5);
ColLin(40,0,40,1*16+3);
ColLin(0,(Cfg->TailleY)-2,80,5);

DFen->init=1;
DFen->Fen2->init=1;

ChangeLine();

MenuBar(3);
}


/*--------------------------------------------------------------------*\
|-                         Gestion 4DOS                               -|
|-  Put Cfg->_4dos on if 4dos found                                   -|
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
    Cfg->_4dos=1;
//  PrintAt(0,0,"Found 4DOS V%d.%d",R.h.bl,R.h.bh);
    }
    else
    {
    Cfg->_4dos=0;
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
    Cfg->_4dos=1;

    seg=R.w.cx;
    adr=(unsigned short*)(seg*16+0x290);

    seg=adr[1];
    adr=(unsigned short*)(seg*16+0x4C60);

    for (n=0;n<255;n++)
        {
        a=((char*)adr)[n];
        Cfg->HistDir[n]=a;
        }
    }
    else
    {
    Cfg->_4dos=0;
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
    Cfg->_4dos=1;

    seg=R.w.cx;
    adr=(unsigned short*)(seg*16+0x290);

    seg=adr[1];
    adr=(unsigned short*)(seg*16+0x4C60);

    for (n=0;n<255;n++)  {
        a=Cfg->HistDir[n];
        ((char*)adr)[n]=a;
        }
    }
    else
    {
    Cfg->_4dos=0;
    }
}

/*--------------------------------------------------------------------*\
|-                      Save Configuration File                       -|
\*--------------------------------------------------------------------*/
void SaveCfg(void)
{
int m,n,t,ns;
FILE *fic;
FENETRE *Fen;
short taille;

for (t=0;t<NBWIN;t++)
    Cfg->FenTyp[t]=Fenetre[t]->FenTyp;

fic=fopen(Fics->CfgFile,"wb");
fwrite((void*)Cfg,sizeof(struct config),1,fic);

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
            fwrite(Fen->F[n]->name,1,m,fic);      // Fichier selectionn�
            if (ns==0) break;
            }
        }

    m=strlen(Fen->F[Fen->pcur]->name);
    fwrite(&m,4,1,fic);                               // Longueur du nom
    fwrite(Fen->F[Fen->pcur]->name,1,m,fic);          // Fichier courant

    fwrite(&(Fen->scur),sizeof(short),1,fic);// Pos du fichier � l'ecran
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

if ( (Cfg->overflow1!=0) | (Cfg->crc!=0x69) )
    {
    fclose(fic);
    return -1;
    }

if (Cfg->palafter!=1)
    {
    NoFlash();
    LoadPal();
    }

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

    DFen->FenTyp=Cfg->FenTyp[t];

    fread(DFen->path,256,1,fic);                   // Repertoire courant
    fread(&(DFen->order),sizeof(short),1,fic);       // Ordre du sorting
    fread(&(DFen->sorting),sizeof(short),1,fic);      // Type de sorting

    IOver=1;
    IOerr=0;

    CommandLine("#cd .");

    IOver=0;

    fread(&nbr,4,1,fic);                // Nombre de fichier selectionne

    DFen->nbrsel=0;

    for (i=0;i<nbr;i++)
        {
        fread(&m,4,1,fic);                            // Longueur du nom
        fread(nom,m,1,fic);                       // Fichier selectionn�
        nom[m]=0;

        for (n=0;n<DFen->nbrfic;n++)
            if (!stricmp(nom,DFen->F[n]->name))
                FicSelect(n,1);
        }

    fread(&m,4,1,fic);                                // Longueur du nom
    fread(nom,m,1,fic);                               // Fichier courant
    nom[m]=0;

    fread(&(DFen->scur),sizeof(short),1,fic); //Pos du fichier � l'ecran

    for (n=0;n<DFen->nbrfic;n++)
        if (!stricmp(nom,DFen->F[n]->name))
            DFen->pcur=n;
    }

fclose(fic);

return 0;
}

/*--------------------------------------------------------------------*\
|-   Affichage des fen�tres                                           -|
\*--------------------------------------------------------------------*/
void AffFen(FENETRE *Fen)
{
int x,y;

int nbuf,nscr;

switch (Fen->FenTyp) {
    case 0:
        FenNor(Fen);
        break;
    case 1:                        // FenDIZ --> A lieu au moment de IDF
        break;
    case 2:      // OFF
        for(y=0;y<=Fen->yl;y++)
            {
            nscr=(y+Fen->y)*160+(Fen->x)*2;
            nbuf=(y-Fen->yl+OldY-1)*80+(Fen->x);

            for(x=0;x<(Fen->xl+1);x++,nbuf++,nscr++)
            if (nbuf<0)
                AffCol(x+Fen->x,y+Fen->y,7);
            else
                AffCol(x+Fen->x,y+Fen->y,Screen_Buffer[nbuf*2+1]);
            }
        for(y=0;y<=Fen->yl;y++)
            {
            nscr=(y+Fen->y)*160+(Fen->x)*2;
            nbuf=(y-Fen->yl+OldY-1)*80+(Fen->x);

            for(x=0;x<(Fen->xl+1);x++,nbuf++,nscr++)
            if (nbuf<0)
              AffChr(x+Fen->x,y+Fen->y,32);
            else
              AffChr(x+Fen->x,y+Fen->y,CnvASCII(Screen_Buffer[nbuf*2]));
            }
        break;
    case 3:
        FenNor(Fen);
        break;
    case 4:
        FenDisk(Fen);
        break;
    case 5:               // Fen FILE_ID.DIZ --> A lieu au moment de IDF
        break;
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

InitScreen(0);                     // Initialise toutes les donn�es HARD

OldY=(*(char*)(0x484))+1;
WhereXY(&PosX,&PosY);

/*--------------------------------------------------------------------*\
|- Initialisation du temps                                            -|
\*--------------------------------------------------------------------*/

Info=GetMem(sizeof(struct RB_info));                // Heure de demarage
Info->temps=clock();


/*--------------------------------------------------------------------*\
|-                       Initialise les buffers                       -|
\*--------------------------------------------------------------------*/

Fenetre[0]=GetMem(sizeof(FENETRE));
Fenetre[0]->F=GetMem(TOTFIC*sizeof(void *));

Fenetre[1]=GetMem(sizeof(FENETRE));
Fenetre[1]->F=GetMem(TOTFIC*sizeof(void *));

Fenetre[2]=GetMem(sizeof(FENETRE));
Fenetre[2]->F=GetMem(TOTFIC*sizeof(void *));

Fenetre[3]=GetMem(sizeof(FENETRE));
Fenetre[3]->F=GetMem(TOTFIC*sizeof(void *));

Cfg=GetMem(sizeof(struct config));
Fics=GetMem(sizeof(struct fichier));

Mask=GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    Mask[n]=GetMem(sizeof(struct PourMask));


Screen_Buffer=GetMem(8000);

for (n=0;n<8000;n++)
    Screen_Buffer[n]=Screen_Adr[n];

path=GetMem(256);

/*--------------------------------------------------------------------*\
|-                Lecture et verification des arguments               -|
\*--------------------------------------------------------------------*/

strcpy(ShellAdr+128,*argv);
strcpy(path,*argv);
for (n=strlen(path);n>0;n--)
    {
    if (path[n]=='\\')
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

Fics->LastDir=GetMem(256);
getcwd(Fics->LastDir,256);

Fics->FicIdfFile=GetMem(256);
strcpy(Fics->FicIdfFile,path);
Path2Abs(Fics->FicIdfFile,"idfext.rb");

Fics->CfgFile=GetMem(256);
strcpy(Fics->CfgFile,path);
Path2Abs(Fics->CfgFile,"kkrb.cfg");

Fics->path=GetMem(256);
strcpy(Fics->path,path);

Fics->help=GetMem(256);
strcpy(Fics->help,path);
Path2Abs(Fics->help,"kkc.hlp");

Fics->temp=GetMem(256);
strcpy(Fics->temp,path);
Path2Abs(Fics->temp,"temp.tmp");

Fics->trash=GetMem(256);
strcpy(Fics->trash,path);
Path2Abs(Fics->trash,"trash");                       // repertoire trash

Fics->log=GetMem(256);
strcpy(Fics->log,path);
Path2Abs(Fics->log,"trash\\logfile");                   // logfile trash



/*--------------------------------------------------------------------*\
|-                               Default                              -|
\*--------------------------------------------------------------------*/

ChangeType(4);

/*
Cfg->_4dos=0;
_4DOSverif();

if (Cfg->_4dos==1)
    {
    _4DOSLhistdir();
    }
    else */
    memset(Cfg->HistDir,0,256);



/*--------------------------------------------------------------------*\
|-                            Initialisation                          -|
\*--------------------------------------------------------------------*/

Fenetre[0]->x=0;
Fenetre[0]->y=1;
Fenetre[0]->yl=(Cfg->TailleY)-4;
Fenetre[0]->xl=39;
Fenetre[0]->actif=1;
Fenetre[0]->nfen=0;

Fenetre[0]->pcur=0;
Fenetre[0]->scur=0;

Fenetre[0]->FenTyp=0;

Fenetre[0]->Fen2=Fenetre[1];
Fenetre[0]->order=17;


Fenetre[1]->x=40;
Fenetre[1]->y=1;
Fenetre[1]->yl=(Cfg->TailleY)-4;
Fenetre[1]->xl=39;
Fenetre[1]->actif=0;
Fenetre[1]->nfen=1;

Fenetre[1]->pcur=0;
Fenetre[1]->scur=0;

Fenetre[1]->FenTyp=0;

Fenetre[1]->Fen2=Fenetre[0];
Fenetre[1]->order=17;


Fenetre[2]->x=40;
Fenetre[2]->y=1;
Fenetre[2]->yl=(Cfg->TailleY)-4;                 // Toute petite fen�tre
Fenetre[2]->xl=39;
Fenetre[2]->actif=0;
Fenetre[2]->nfen=2;

Fenetre[2]->pcur=0;
Fenetre[2]->scur=0;

Fenetre[2]->FenTyp=2;                      // Fen�tre ferm�e par default

Fenetre[2]->Fen2=Fenetre[2];
Fenetre[2]->order=17;

Fenetre[3]->x=40;
Fenetre[3]->y=1;
Fenetre[3]->yl=(Cfg->TailleY)-4;                 // Toute petite fen�tre
Fenetre[3]->xl=39;
Fenetre[3]->actif=0;
Fenetre[3]->nfen=2;

Fenetre[3]->pcur=0;
Fenetre[3]->scur=0;

Fenetre[3]->FenTyp=2;                      // Fen�tre ferm�e par default

Fenetre[3]->Fen2=Fenetre[3];
Fenetre[3]->order=17;


/*--------------------------------------------------------------------*\
|-                Chargement du fichier config (s'il existe)          -|
\*--------------------------------------------------------------------*/

if (LoadCfg()==-1)
    {
    for (n=0;n<NBWIN;n++)
        {
        DFen=Fenetre[n];
        CommandLine("#c:");
        }
    DefaultCfg();
    }

if ( (Cfg->currentdir==1) & (LC[4]!='0') )
    {
    DFen=Fenetre[(Cfg->FenAct)&1];

    if (strcmp(Fics->LastDir,DFen->path)!=0)
        CommandLine("#CD %s",Fics->LastDir);
    }

UseCfg();                      // Emploi les parametres de configuration

InitScreen(Cfg->display);

Fenetre[0]->yl=(Cfg->TailleY)-4;
Fenetre[1]->yl=(Cfg->TailleY)-4;

if (Cfg->verifhist==1)
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
    parm [edi];

void PrintMem(void)
{
int tail[12];
GetFreeMem((void*)tail);  // inconsistent ?
PrintAt(0,0,"Memory: %d octets",tail[0]);
}

void SwapShort(short *a,short *b)
{
short c;

c=(*a);
(*a)=(*b);
(*b)=c;
}

void SwapWin(int a,int b)
{
FENETRE *tfen;

SwapShort(&(Fenetre[a]->x),&(Fenetre[b]->x));
SwapShort(&(Fenetre[a]->y),&(Fenetre[b]->y));
SwapShort(&(Fenetre[a]->xl),&(Fenetre[b]->xl));
SwapShort(&(Fenetre[a]->yl),&(Fenetre[b]->yl));
SwapShort(&(Fenetre[a]->x2),&(Fenetre[b]->x2));
SwapShort(&(Fenetre[a]->y2),&(Fenetre[b]->y2));
SwapShort(&(Fenetre[a]->xl2),&(Fenetre[b]->xl2));
SwapShort(&(Fenetre[a]->yl2),&(Fenetre[b]->yl2));
SwapShort(&(Fenetre[a]->x3),&(Fenetre[b]->x3));
SwapShort(&(Fenetre[a]->y3),&(Fenetre[b]->y3));
Fenetre[a]->init=1;
Fenetre[b]->init=1;

tfen=Fenetre[a];
Fenetre[a]=Fenetre[b];
Fenetre[b]=tfen;

Fenetre[a]->nfen=a;
Fenetre[b]->nfen=b;

}

