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

#include <signal.h> // For handling signal

#include <dos.h>    //  Pour redirection des I/O

#include "idf.h"
#include "win.h"
#include "kk.h"

/*---------------------*
 * Declaration interne *
 *---------------------*/

void Fin(void);

void SaveSel(struct fenetre *F1);
void LoadSel(struct fenetre *F1,int n);

void Move(struct fenetre *F1,struct fenetre *F2);

/*---------------------------*
 * Declaration des variables *
 *---------------------------*/

int OldCol;            // Ancienne couleur du texte
char OldY,PosX,PosY;


sig_atomic_t signal_count;

char Select_Chaine[16]="*.*";

char *Screen_Buffer;
char *Screen_Adr=(char*)0xB8000;

char *Keyboard_Flag1=(char*)0x417;

char *ShellAdr=(char*)0xBA000;

struct fenetre *Fenetre[3];

struct fenetre *DFen;


char *RBTitle="Ketchup Killers Commander V"VERSION" / RedBug";

extern int IOver;
extern int IOerr;


/*-------------------------*
 * Procedure en Assembleur *
 *-------------------------*/

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


/*------------------*
 * Other Procedures *
 *------------------*/

// c=0 --> change from selection
//   1 --> default set of color
//   2 --> norton set
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

#define NBRS 3

char defcol[NBRS][48]={ {43,37,30, 31,22,17,  0, 0, 0, 58,58,50,
                      44,63,63, 63,63,21, 43,37,30,  0, 0, 0,
                      63,63, 0, 63,63,63, 43,37,30, 63, 0, 0,
                       0,63, 0,  0, 0,63,  0, 0, 0,  0, 0, 0},
                     { 0, 0, 0, 43,43,43,  0, 0, 0, 63,63,63,
                      63,63,63, 63,63,21, 50,58,55,  0, 0,43,
                      63,63, 0, 63,63,63,  0, 0,43, 63, 0, 0,
                       0,63, 0,  0, 0,63,  0, 0, 0,  0, 0, 0},
                     {25,36,29, 36,18,15,  0, 0, 0, 49,39,45,
                      44,63,63, 42,37,63, 45,39,35,  0, 0, 0,
                       0,63,63, 63,63,63, 25,36,29, 63, 0, 0,
                       0,63, 0,  0, 0,63,  0, 0, 0,  0, 0, 0} };

char *Style[NBRS]={"Default Style","Norton Style","Cyan Style"};
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
ColWin(1,1,78,(Cfg->TailleY)-3,10*16+1);
ChrWin(1,1,78,(Cfg->TailleY)-3,32);

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
    PrintAt(5,46,RBTitle);

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
        ColWin(x,y,x+8,y+2,1*16+5);
        ChrWin(x,y,x+8,y+2,32);

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
        case 0x47:  // HOME
            ChrWin(x,y+m,x+8,y+m,32);
            Cfg->palette[n*3+m]=0;
            break;
        case 0X4F: // END
            ChrWin(x,y+m,x+8,y+m,32);
            Cfg->palette[n*3+m]=63;
            break;
        case 80:    // bas
            m++;
            break;
        case 72:    // haut
            m--;
            break;
        case 0x4B:  // gauche
            if (Cfg->palette[n*3+m]!=0)
                Cfg->palette[n*3+m]--;
            break;
        case 0x4D:  // droite
            if (Cfg->palette[n*3+m]!=63)
                Cfg->palette[n*3+m]++;
            break;
        case 0xF:   // SHIFT-TAB
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
        case 80:    // bas
            n++;
            break;
        case 72:    // haut
            n--;
            break;
        case 0xF:   // SHIFT-TAB
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

/*--------------------*
 - Programme de setup -
 *--------------------*/



void Signal_Handler(int sig_no)
{
signal_count++;

WinCadre(29,9,48,14,0);
ColWin(30,10,47,13,0*16+1);
ChrWin(30,10,47,13,32);

PrintAt(31,10,"Signal: %d",sig_no);
PrintAt(31,12,"Press a key when");
PrintAt(31,13,"  it is correct");
Wait(0,0,0);

exit(1);
}

/* Gestion de toutes les fonctions

  0: ?
  1: Help
  2: Invert Selection
  3: Select Group of file
  4: Unselect Group of file
  5: Search File
  6: Create a KKD file
  7: View file
  8: Quick view file
  9: Edit file
 10: Copy
 11: Move
 12: Create Directory
 13: Delete selection
 14: Close left window
 15: Close right window
 16: Select current file
 17: Change palette
 18: About
 19: Select temporary file
 20: Quit KKC
 21: Fenetre DIZ
 22: Sort by name
 23: Sort by extension
 24: Sort by date
 25: Sort by size
 26: Sort by unsort ;)
 27: Reload Directory
 28: ASCII Table
 29: Win CD
 30: Put file on command line
 31: Appel du programme de configuration
 32: Switch les fontes
 33: Switch special sort
 34: Efface la trash
 35: Affiche les infos
 36: Ligne suivante
 37: Ligne precedente
 38: Fenˆtre information
*/

void GestionFct(int fct)
{
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
        Search();
        break;
    case 6:
        CreateKKD();
        break;
    case 7:
        CommandLine("#%s %s",Fics->view,DFen->F[DFen->pcur]->name);
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
        CommandLine("#%s %s",Fics->edit,DFen->F[DFen->pcur]->name);
        break;
    case 10:    // Copy
        Copie(DFen,DFen->Fen2);
        break;
    case 11:    // Move
        Move(DFen,DFen->Fen2);
        break;
    case 12:    // Create Directory
        CreateDirectory();
        break;
    case 13:    // Delete selection
        SaveSel(DFen);
        Delete(DFen);
        CommandLine("#cd .");
        LoadSel(DFen,0);
        break;
    case 14:    // Close left window
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
            ChangeLine();      // Affichage Path
            }
        break;
    case 15:    // Close right window
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
            ChangeLine();      // Affichage Path
            }
        break;
    case 16:    // Select current file
        FicSelect(DFen->pcur,2);

        if (!strcmp(DFen->F[DFen->pcur]->name,"."))
            for (i=0;i<DFen->nbrfic;i++)
                if (!WildCmp(DFen->F[i]->name,Select_Chaine))
                    FicSelect(i,1);
        break;
    case 17:    // Change palette
        ChangePalette(0);
        break;
    case 18:   // About
        WinError(RBTitle);
        break;
    case 19:   // Select temporary file
        for (i=0;i<DFen->nbrfic;i++)
            {
            if (DFen->F[i]->select==0)
                {
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
                    {
                    DFen->F[i]->select=1;
                    DFen->taillesel+=DFen->F[i]->size;
                    DFen->nbrsel++;
                    }
                }
            }
        break;
    case 20:    // Quit KKC
        break;
    case 21:  // Fenetre DIZ
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
    case 22:    // Sort by name
        DFen->order&=16;
        DFen->order|=1;
        SortFic(DFen);
        break;
    case 23:    // Sort by extension
        DFen->order&=16;
        DFen->order|=2;
        SortFic(DFen);
        break;
    case 24:    // Sort by date
        DFen->order&=16;
        DFen->order|=3;
        SortFic(DFen);
        break;
    case 25:    // Sort by size
        DFen->order&=16;
        DFen->order|=4;
        SortFic(DFen);
        break;
    case 26:    // Sort by unsort ;)
        DFen->order&=16;
        SortFic(DFen);
        break;
    case 27:    // Reload
        CommandLine("#CD .");
        break;
    case 28:    // Table ASCII
        ASCIItable();
        break;
    case 29:    // WINCD
        WinCD();
        break;
    case 30:    // Put file on command line
        CommandLine("%s ",DFen->F[DFen->pcur]->name);
        break;
    case 31:    // Configuration
        Setup();
        /*
        strcpy(buffer,Fics->path);
        Path2Abs(buffer,"kksetup.ini");
        CommandLine("#%s %s",Fics->edit,buffer);
        */
        break;
    case 32:    // Switch les fontes
        Cfg->font^=1;
        ChangeTaille(Cfg->TailleY); // Rafraichit l'ecran
        ChangeLine();
        break;
    case 33:    // Switch Special Sort
        DFen->order^=16;
        SortFic(DFen);
        break;
    case 34:    // Nettoie la trash
        strcpy(buffer,Fics->trash);
        Path2Abs(buffer,"kktrash.sav");

        fic=fopen(buffer,"rt");
        while(fgets(buffer,256,fic)!=NULL)
            {
            buffer[strlen(buffer)-1]=0; // Retire le caractere ENTER
            remove(buffer);

//          if (nanana!=0) WinError(buffer);
            }
        fclose(fic);

        strcpy(buffer,Fics->trash);
        Path2Abs(buffer,"kktrash.sav");
        remove(buffer);

        DFen=DFen->Fen2;
        if (!stricmp(DFen->path,Fics->trash))
            CommandLine("#cd .");
        DFen=DFen->Fen2;
        if (!stricmp(DFen->path,Fics->trash))
            CommandLine("#cd .");

        Cfg->strash=0;
        break;
    case 35:
        WinInfo();
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
    }


}

//-----------------------------------------------------------------------
int GestionBar(int i)
{
int retour;
int nbmenu;

int u,v,s,x;
struct barmenu bar[20];

short fin;

static cpos[20];
static int poscur;

SaveEcran();

if (i==0)
    retour=0;
    else
    {
    retour=1;
    switch(i)
        {
        case 0x21:  poscur=0;  break;  // ALT-F
        case 0x19:  poscur=1;  break;  // ALT-P
        case 0x20:  poscur=2;  break;  // ALT-D
        case 0x1F:  poscur=3;  break;  // ALT-S
        case 0x14:  poscur=4;  break;  // ALT-T
        case 0x18:  poscur=5;  break;  // ALT-O
        case 0x23:  poscur=6;  break;  // ALT-H
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

if (retour==0)      // Navigation sur bar de menu
    v=1;
    else
    v=0;

u=BarMenu(bar,7,&poscur,&x,&v);  // Renvoit t: position du machin surligne
                                 // Renvoit v: 0 si rien, autre si position dans sous fenetre
if (u==0)
    break;

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
        strcpy(bar[4].titre, "");                              bar[4].fct=0;
        strcpy(bar[5].titre, "Name                 CTRL-F3");  bar[5].fct=22;
        strcpy(bar[6].titre, "Extension            CTRL-F4");  bar[6].fct=23;
        strcpy(bar[7].titre, "Time/Date            CTRL-F5");  bar[7].fct=24;
        strcpy(bar[8].titre, "Size                 CTRL-F6");  bar[8].fct=25;
        strcpy(bar[9].titre, "Unsort               CTRL-F7");  bar[9].fct=26;
        nbmenu=10;
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
        nbmenu=2;
        break;
    case 5:
        strcpy(bar[0].titre,"Configuration");         bar[0].fct=31;
        strcpy(bar[1].titre,"Color Configuration");   bar[1].fct=17;
        nbmenu=2;
        break;
    case 6:
        strcpy(bar[0].titre,"Help ");    bar[0].fct=1;
        strcpy(bar[1].titre,"About");   bar[1].fct=18;
        nbmenu=2;
        break;

    }

s=2;
retour=PannelMenu(bar,nbmenu,&(cpos[poscur]),&x,&s);  // (x,y)=(t,s)

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

//-----------------------------------------------------------------------



// Type 0: incremente de 1
// autre type: type donn‚
void ChangeType(int n)
{
if (n==0)
   Cfg->fentype++;
   else
   Cfg->fentype=n;

if (Cfg->fentype>4) Cfg->fentype=1;

Cfg->colnor=1*16+9;
Cfg->inscol=1*16+14;
Cfg->bkcol=3*16+7;

}


//--------------
// Select File -
//--------------

void SelectPlus(void)
{
int i;
char fin;

SaveEcran();

WinCadre(29,6,51,11,0);
ColWin(30,7,50,10,0*16+1);
ChrWin(30,7,50,10,32);

WinCadre(30,8,50,10,1);

PrintAt(30,7,"Selection of files");

fin=InputAt(31,9,Select_Chaine,12);

ChargeEcran();

if (fin!=1)
    for (i=0;i<DFen->nbrfic;i++)
        if (!WildCmp(DFen->F[i]->name,Select_Chaine))
            FicSelect(i,1);
}

//---------------
// Select Moins -
//---------------

void SelectMoins(void)
{
int i;
char fin;

SaveEcran();

WinCadre(29,6,51,11,0);
ColWin(30,7,50,10,0*16+1);
ChrWin(30,7,50,10,32);

WinCadre(30,8,50,10,1);

PrintAt(30,7,"Deselection of files");

fin=InputAt(31,9,Select_Chaine,12);

ChargeEcran();

if (fin!=1)
    for (i=0;i<DFen->nbrfic;i++)
        if (!WildCmp(DFen->F[i]->name,Select_Chaine))
            FicSelect(i,0);
}

//--------------
// History Dir -
//--------------

void HistDir(void)
{
int Mlen;

int i,j;
static char **dir;

dir=GetMem(25*sizeof(char *));

j=0;
Mlen=0;
for (i=0;i<25;i++)  {
    dir[i]=Cfg->HistDir+j;
    dir[i]=strupr(dir[i]);
    if (strlen(dir[i])>Mlen) Mlen=strlen(dir[i]);
    if (strlen(dir[i])==0) break;
    while ( (j!=256) & (Cfg->HistDir[j]!=0) ) j++;
    j++;
    }

if (i!=0)
    {
    int x=2,y=2;
    int pos=i-1;
    int car;

    SaveEcran();
    PutCur(32,0);

    WinCadre(x-2,y-1,x+Mlen+1,y+i,0);
    ColWin(x-1,y,x+Mlen,y+i-1,10*16+1);
    ChrWin(x-1,y,x+Mlen,y+i-1,32);

    for (j=0;j<i;j++)
        PrintAt(x,y+j,dir[j]);

    do {

    ColLin(x-1,y+pos,Mlen+2,7*16+5);

    car=Wait(0,0,0);

    ColLin(x-1,y+pos,Mlen+2,0*16+1);

    switch(HI(car))  {
        case 72:        // UP
            pos--;
            if (pos==-1) pos=i-1;
            break;
        case 80:        // DOWN
            pos++;
            if (pos==i) pos=0;
            break;
        case 0x47:      // HOME
            pos=0;
            break;
        case 0x4F:      // END
            pos=i-1;
            break;
        case 0x49:      // PGUP
            pos-=5;
            if (pos<0) pos=0;
            break;
        case 0x51:      // PGDN
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





//-----------------------
// Win Change directory -
//-----------------------
void WinCD(void)
{
static char Dir[70];
static int DirLength=70;
static int CadreLength=71;

struct Tmt T[5] = {
      { 2,3,1,
        Dir,
        &DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Change to which directory",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,10,76,17,
    "Change Directory"};

int n;

n=WinTraite(T,5,&F);

if ( (n==0) | (n==1) ) {
    CommandLine("#cd %s",Dir);
    }
}

//-------------------------------------
// Create directory in current window -
//-------------------------------------
void CreateDirectory(void)
{
static char Dir[70];
static int DirLength=70;
static int CadreLength=71;

struct Tmt T[5] = {
      { 2,3,1,
        Dir,
        &DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the directory to be created",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,10,76,17,
    "Create Directory"};

int n;

n=WinTraite(T,5,&F);

if ( (n==0) | (n==1) ) {
    CommandLine("#md %s",Dir);
    CommandLine("#cd .");
    }
}

//------------------
// Create KKD disk -
//------------------
void CreateKKD(void)
{
static char Name[256];
static char Dir[70];
static int DirLength=70;
static int CadreLength=71;

struct Tmt T[5] = {
      { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the KKD file to be created",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,10,76,17,
    "Create KKD File"};

int n;

n=WinTraite(T,5,&F);

strcpy(Name,DFen->Fen2->path);

for (n=strlen(Dir);n>0;n--)  {
    if (Dir[n]=='\\') break;
    if (Dir[n]=='.')
        {
        Dir[n]=0;
        break;
        }
    }
strcat(Dir,".kkd");

if ( (n==0) | (n==1) )
    {
    Path2Abs(Name,Dir);
    MakeKKD(DFen,Name);
    }
}


//---------------------------------
// Change drive of current window -
//---------------------------------
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
ColWin(x1+1,7,x1+36,20,10*16+1);
ChrWin(x1+1,7,x1+36,20,32);
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

do	{
    do
        {
        if (cpos!=50)
            ColLin(x1+3,cpos-fpos,32,10*16+1);

        switch(HI(car))
            {
            case GAUCHE: i--,cpos=50; break;    // LEFT
            case DROITE: i++,cpos=50; break;    // RIGHT
            case 80: cpos++; break;             // DOWN
            case 72: cpos--; break;             // UP
            case 0x47: cpos=12; break;          // HOME
            case 0x4F: cpos=pos-1; break;       // END
            }

        if (i==26) i=0;
        if (i<0) i=25;

        if ( (drive[i]!=0) & (cpos==50) )
            if (VerifyDisk(i+1)==0)
                {
                unsigned nbrdrive,ii;

                ChrWin(x1+2,12,x1+35,19,32);
                ColWin(x1+2,12,x1+35,19,10*16+1);

                ii=i+1;
                _dos_setdrive(ii,&nbrdrive);

                p1=DFen->path[0];
                p1=toupper(p1)-'A';

                if (p1==i)
                    strcpy(path,DFen->path);
                    else
                    getcwd(path,256);

                if (path[strlen(path)-1]!='\\') strcat(path,"\\");


                /* Calcule le nombre de position occup‚e par les repertoires */
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
p1=0;
p2=0;

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

//---------------------------------
// Change drive of current window -
//---------------------------------
void ChangeToKKD(void)
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
ColWin(x-1,7,x+l*nbr,10,0*16+1);
ChrWin(x-1,7,x+l*nbr,10,32);

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
do	{
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
}

void QuickSearch(char *c,char *c2)
{
char fin;
char chaine[20];
int i,lng=0;
int x,y;
char car,car2;

int c1;

x=DFen->Fen2->x+3;
y=DFen->Fen2->y+3;

SaveEcran();

WinCadre(x-1,y-1,x+24,y+1,0);
ColLin(x,y,24,0*16+1);
ChrLin(x,y,24,32);

do
    {
    c1=Wait(x+23,y,0);

    car=LO(c1);
    car2=HI(c1);

    car=tolower(car);

    fin=1;

    if ( ((car>='a') & (car<='z')) | (car=='.') | ((car>='0') & (car<='9')) )
        {
        chaine[lng]=car;
        lng++;
        fin=0;
        }

    if (lng!=0)
        {
        lng--;

        for (i=0;i<DFen->nbrfic;i++)
            if (!strnicmp(chaine,DFen->F[i]->name,lng+1))
                {
                DFen->pcur=i;
                DFen->scur=i;
                lng++;
                break;
                }
        chaine[lng]=0;
        }
    PrintAt(x,y,"%24s",chaine);

    AffFen(DFen);
            
    } while(!fin);

ChargeEcran();

*c=car;
*c2=car2;
}


/*****************************************************************************/


/**********************************
 -  Access fichier suivant system -
 **********************************/

char *AccessFile(void)
{
FILE *fic;
static char nom[256];
static char tnom[256];
static char buffer[256];

char ChangePos=0;

int i;

strcpy(tnom,DFen->F[DFen->pcur]->name);

if (!strcmp(DFen->F[DFen->pcur]->name,".."))
    {
    strcpy(tnom,"FILE_ID.DIZ");
    ChangePos=1;
    }

switch (DFen->system)   {
    case 0: // DOS
        strcpy(nom,DFen->path);
        Path2Abs(nom,tnom);
        break;
    case 1: // RAR
    case 2: // ARJ
    case 3: // ZIP
    case 4: // LHA
        DFen=DFen->Fen2;

        CommandLine("#cd %s",Fics->trash);

        Cfg->FenAct= (Cfg->FenAct==1) ? 0:1;
        DFen=Fenetre[Cfg->FenAct];
        ChangeLine();      // Affichage Path

        DFen=DFen->Fen2;

        for (i=0;i<DFen->nbrfic;i++)
            if (!strncmp(tnom,DFen->F[i]->name,strlen(tnom)))
                {
                DFen->pcur=i;
                DFen->scur=i;

                DFen->F[DFen->pcur]->select=1;
                DFen->nbrsel++;
                DFen->taillesel+=DFen->F[DFen->pcur]->size;
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
        DFen=DFen->Fen2;
        Copie(DFen->Fen2,DFen);         // Quitte aprŠs

        break;
    case 5: // KKD
        if (strlen(DFen->path)==strlen(DFen->VolName))
            strcpy(nom,"\\");
            else
            {
            strcpy(nom,"\\");
            strcat(nom,(DFen->path)+strlen(DFen->VolName)+1);
            }

        DFen=DFen->Fen2;

        ChangeToKKD();
        CommandLine("#cd %s",nom);

        Cfg->FenAct= (Cfg->FenAct==1) ? 0:1;
        DFen=Fenetre[Cfg->FenAct];
        ChangeLine();      // Affichage Path

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
        if (!strncmp(tnom,DFen->F[i]->name,strlen(tnom)))  {
            DFen->pcur=i;
            DFen->scur=i;
            break;
            }
    }
return nom;
}


/***************************************************************************
 - Retourne 0 si OK                                                        -
 - Sinon retourne numero de IDF                                            -
 ***************************************************************************/
int EnterArchive(void)
{
int i;

switch (i=InfoIDF(DFen))
    {
    case 34:       // RAR
        InstallRAR();
        break;
    case 30:       // ARJ
        InstallARJ();
        break;
    case 35:       // ZIP
        InstallZIP();
        break;
    case 32:       // LHA
        InstallLHA();
        break;
    case 102:       // KKD
        InstallKKD();
        break;
    default:
        return i;
        break;
    }

CommandLine("#cd .");
return 0;               // OK
}

void SaveSel(struct fenetre *F1)
{
struct file *F;
FILE *fic;
int i;

fic=fopen(Fics->temp,"wt");

fprintf(fic,"%s\n",F1->F[F1->pcur]->name);

fprintf(fic,"%d\n",F1->pcur);   // position a l'ecran



for(i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];

    if ((F->select)==1)
        fprintf(fic,"%s\n",F->name);
    }
fclose(fic);
}

void LoadSel(struct fenetre *F1,int n)
{
char nom[256];
struct file *F;
FILE *fic;
int i,j;


fic=fopen(Fics->temp,"rt");
if (fic==NULL) return;

fgets(nom,256,fic);
nom[strlen(nom)-1]=0;

fscanf(fic,"%d",&(F1->scur));

j=1;
for (i=0;i<F1->nbrfic;i++)
    if (!strncmp(nom,F1->F[i]->name,strlen(nom)))  {
        F1->pcur=i;

        j=0;
        break;
        }

if (j==1)
    F1->pcur=F1->scur;

while(!feof(fic))
    {
    fgets(nom,256,fic);
    nom[strlen(nom)-1]=0;

    for(i=0;i<F1->nbrfic;i++)
        {
        F=F1->F[i];

        if (!stricmp(F->name,nom))
            {
            switch(n)   {
                case 0:
                    F->select=1;
                    break;
                case 1:
                    if (F->select==0)
                        F->select=1;
                        else
                        F->select=0;
                    break;
                }
            }
        }
    }
fclose(fic);
}


/*******************
 - Fonction RENAME -
 *******************/
void WinRename(struct fenetre *F1,struct fenetre *F2)
{
static char Dir[70];
static char Name[256];
static int DirLength=70;
static int CadreLength=71;
struct fenetre *Fen;

struct Tmt T[5] = {
      { 2,3,1,
        Dir,
        &DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Move/rename file to",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,10,76,17,
    "Move/rename"};

int n;

strcpy(Dir,F2->path);
Path2Abs(Dir,F1->F[DFen->pcur]->name);

strcpy(Name,F1->path);
Path2Abs(Name,F1->F[DFen->pcur]->name);

n=WinTraite(T,5,&F);

if ( (n==0) | (n==1) )
    {
    if (rename(Name,Dir)!=0)
        WinError("Couldn't rename file");
    }

Fen=DFen;

DFen=F1;
CommandLine("#cd .");
DFen=F2;
CommandLine("#cd .");

DFen=Fen;
}




/*************************************************************************
 -                         Programme Principal                           -
 *************************************************************************/
void Gestion(void)
{
struct fenetre *FenOld;
char *ext;

clock_t Cl,Cl_Start;
char car,car2;

unsigned short car3,c;

int i;  // Compteur

do
    {
    if ( (Cfg->key==0) & (Cfg->strash>=Cfg->mtrash) & (Cfg->mtrash!=0) )
        GestionFct(34);  // Efface la trash si trop plein

    DFen=Fenetre[Cfg->FenAct];

    switch(DFen->FenTyp)
        {
        case 1:
        case 2:
            if ( ((DFen->Fen2->FenTyp)==1) | ((DFen->Fen2->FenTyp)==2) ) break;
            Cfg->FenAct= (Cfg->FenAct==1) ? 0:1;
            DFen=Fenetre[Cfg->FenAct];
            ChangeLine();      // Affichage Path
            break;
        }

    DFen->actif=1;
    DFen->Fen2->actif=0;

    if (DFen->Fen2->FenTyp!=3)
        AffFen(DFen);
    if (DFen->FenTyp!=3)
        AffFen(DFen->Fen2);

    Cl_Start=clock();
    Cl=clock();

    if (Cfg->key==0)
        {
        c=0;

        while ( (!kbhit()) & (c==0) )
            {
            if ( ((clock()-Cl_Start)>DFen->IDFSpeed)  & (Cl_Start!=0))
                {
                Cl_Start=0;
                InfoIDF(DFen); // information idf sur fichier selectionn‚

                if (DFen->FenTyp==1) FenDIZ(DFen);
                if (DFen->Fen2->FenTyp==1) FenDIZ(DFen->Fen2);
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

        switch(HI(c))   {
            case 0x3D:  // F3
            case 0x3E:  // F4
            case 0x56:  // SHIFT-F3
            case 0x8D:  // CTRL-UP
                AccessFile();
                break;
            }
        }
    else
        {
        c=Cfg->key;
        car3=0;

        for (i=0;i<DFen->nbrfic;i++)
            if (!strnicmp(Cfg->FileName,DFen->F[i]->name,strlen(Cfg->FileName)))
                {
                DFen->pcur=i;
                DFen->scur=i;
                break;
                }
        }

    Cfg->key=0;

    car=LO(c);
    car2=HI(c);

    if (car2==0x94)
        QuickSearch(&car,&car2);


//-Switch car3 (BIOS_KEYBOARD)-------------------------------------------------
    switch (car3)
        {
        case 0x37:  // '*' --> Inverse selection
            GestionFct(2);
            car=car2=0;
            break;
        case 0x4E:  // '+' --> Selectionne
            GestionFct(3);
            car=car2=0;
            break;
        case 0x4A:  // '-' --> Deselectionne
            GestionFct(4);
            car=car2=0;
            break;
        default:
//          PrintAt(0,0,"%04X",car3);
            break;
        }

//-Switch car------------------------------------------------------------------
    switch (car)  {
        case 0x12:    // CTRL-R
            GestionFct(27);
            break;
        case 1:     // CTRL-A
            GestionFct(28);
            break;
        case 4:     // CTRL-D
            GestionFct(29);
            break;
        case 6:     // CTRL-F
            GestionFct(32);
            break;
        case 9:     // TAB
            Cfg->FenAct= (Cfg->FenAct==1) ? 0:1;
            DFen=Fenetre[Cfg->FenAct];
            ChangeLine();      // Affichage Path
            break;
        case 13:    // ENTER
            ext=getext(DFen->F[DFen->pcur]->name);
            if ( (!stricmp(ext,"COM")) | (!stricmp(ext,"BAT")) |
                 ((!stricmp(ext,"BTM")) & (Cfg->_4dos==1)) )
                {
                CommandLine("%s\n",DFen->F[DFen->pcur]->name);
                break;
                }
            switch(i=EnterArchive())
                {
                case 0:
                    // OK
                    break;
                case 57: // Executable
                    CommandLine("%s\n",DFen->F[DFen->pcur]->name);
                    break;
                default:

                    switch(FicIdf(DFen->F[DFen->pcur]->name,i)) {
                        case 0:
                            CommandLine("\n");
                            break;
                        case 1:  // Pas de fichier IDFEXT.RB
                            CommandLine("@ ERROR WITH FICIDF @");
                            break;
                        case 2:
                             CommandLine(DFen->F[DFen->pcur]->name);
                             break;
                        }
                }
            break;

        case 0x0A:  // CTRL-ENTER
            GestionFct(30);
            break;
        case 0x0C:  // CTRL-L
            GestionFct(35);
            break;
        case 0x0F:  // CTRL-O
            GestionFct(14);
            GestionFct(15);
            break;
        case 27:    // ESCAPE
            CommandLine("\r");
            break;
        case 'ý':
            GestionFct(19);
            break;
        case 32:
            if (CommandLine(" ")==0)
                {
                GestionFct(16);
                GestionFct(36);
                }
            break;

//-Switch car2-----------------------------------------------------------------
        case 0:
        switch(car2)  {       // Switch (car2)
        

        case 72:         // HAUT
            GestionFct(37);
            break;

        case 0x52:       // Insert
            GestionFct(16);       // pas de break car ---/
            GestionFct(36);
            break;
        case 80:         // BAS
            GestionFct(36);
            break;
        case 0x4B:       // LEFT
            Cfg->FenAct=0;
            DFen=Fenetre[Cfg->FenAct];
            ChangeLine();      // Affichage Path
            break;
        case 0x4D:       // RIGHT
            Cfg->FenAct=1;
            DFen=Fenetre[Cfg->FenAct];
            ChangeLine();      // Affichage Path
            break;
        case 0x49:       // PAGE UP
            DFen->scur-=10;
            DFen->pcur-=10;
            break;
        case 0x51:       // PAGE DOWN
            DFen->scur+=10;
            DFen->pcur+=10;
            break;
        case 0x47:       // HOME
            DFen->scur=0;
            DFen->pcur=0;
            break;
        case 0x4F:       // END
            DFen->scur=DFen->nbrfic;
            DFen->pcur=DFen->nbrfic;
            break;
        case 0x3B:       // F1
            GestionFct(1);            break;
        case 0x3D:       // F3
            GestionFct(7);            break;
        case 0x3E:       // F4
            GestionFct(9);            break;
        case 0x3F:       // F5
            GestionFct(10);           break;
        case 0x40:       // F6
            GestionFct(11);           break;
        case 0x41:       // F7
            GestionFct(12);           break;
        case 0x42:       // F8
            GestionFct(13);           break;
        case 0x43:       // F9
            i=GestionBar(0);
            if (i==20)
                car2=0x44;      // F10
                else
                GestionFct(i);
            break;
        case 0x56:      // SHIFT-F3
        case 0x8D:      // CTRL-UP
            GestionFct(8);            break;
        case 0x59:       // SHIFT-F6
            WinRename(DFen,DFen->Fen2);
            break;
        case 0x5E:       // CTRL-F1
            GestionFct(14);           break;
        case 0x5F:       // CTRL-F2
            GestionFct(15);           break;
        case 0x60:       // CTRL-F3
            GestionFct(22);           break;
        case 0x61:       // CTRL-F4
            GestionFct(23);           break;
        case 0x62:       // CTRL-F5
            GestionFct(24);           break;
        case 0x63:       // CTRL-F6
            GestionFct(25);           break;
        case 0x64:       // CTRL-F7
            GestionFct(26);           break;
        case 0x65:       // CTRL-F8
            GestionFct(33);           break;
        case 0x68:       // ALT-F1
               FenOld=DFen;
               DFen=Fenetre[0];

			   ChangeDrive();
			   DFen=FenOld;

               ChangeLine();      // Affichage Path
			   break;
        case 0x69:       // ALT-F2
               FenOld=DFen;
               DFen=Fenetre[1];

			   ChangeDrive();
			   DFen=FenOld;

               ChangeLine();      // Affichage Path
			   break;
        case 0x6A:       // ALT-F3
            GestionFct(21);
            break;
        case 0x6E:       // ALT-F7
            GestionFct(5);
            break;
        case 0x6F:       // ALT-F8
            ChangeType(0);
            AfficheTout();
            break;
        case 0x70:       // ALT-F9
            ChangeTaille(0);
            ChangeLine();
            break;
        case 0x73:       // CTRL LEFT
            CommandLine("#CD ..");
            break;
        case 0x74:       // CTRL RIGHT
            if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
                CommandLine("#cd %s",DFen->F[DFen->pcur]->name);
            break;
        case 0x76:       // CTRL PGDN
            HistDir();
            break;
        case 0x84:       // CTRL PGUP
            CommandLine("#CD ..");
            break;
        case 0x86:       // F12
            CommandLine("#CD %s",Fics->trash);
            break;
        case 0x88:       // SHIFT-F12
            GestionFct(6);
            break;
        case 0x8A:       // CTRL-F12
            GestionFct(17);
            break;
        case 0xA1:
            CommandLine("#CD %s",Fics->LastDir);
            break;

        case 0x21:       // ALT-F
        case 0x19:       // ALT-P
        case 0x20:       // ALT-D
        case 0x1F:       // ALT-S
        case 0x14:       // ALT-T
        case 0x18:       // ALT-O
        case 0x23:       // ALT-H
            i=GestionBar(car2);
            if (i==20)
                car2=0x44;      // F10
                else
                GestionFct(i);
            break;
        case 0x17:      // ALT-I
            GestionFct(38);
            break;
        case 0xB6:  //
        case 0xB7:  //  Windows 95 keyboard
        case 0xB8:  //
			   break;

        default:
            if (Cfg->debug==1)
               PrintAt(78,0,"%02X",car2);
            break;
        }    // switch (car2);
        break;
//-Retour Switch car-----------------------------------------------------------
        default:  // default du switch car
            CommandLine("%c",car);
            break;
    }  // switch(car);

/*
    switch(inp(0x60))
        {
        case 72:         // HAUT
            DFen->scur--;
            DFen->pcur--;
            break;
        case 80:         // BAS
            DFen->scur++;
            DFen->pcur++;
            break;
        }
*/
    }
while(car2!=0x44);      // F10
}


/*******************************************************
 -                      DOS Shell                      -
 *******************************************************/

void Shell(char *string,...)
{
char sortie[255];
va_list arglist;
char *suite;

TXTMode(OldY);  // Anciennement Cfg->TailleY

suite=sortie;

va_start(arglist,string);
vsprintf(sortie,string,arglist);
va_end(arglist);

if (Cfg->KeyAfterShell==0)
    suite[0]='#';

strcpy(ShellAdr,suite);

Fin();
}



void PlaceDrive(void)
{
unsigned ndrv;

// chdir(DFen->Fen2->path);     // Vraiment pas utile
_dos_setdrive(toupper(DFen->path[0])-'A'+1,&ndrv);
chdir(DFen->path);

}


/*******************
 - Fin d'execution -
 *******************/

void Fin(void)
{
int n;

SaveCfg();

PlaceDrive();

for (n=0;n<8000;n++)
    Screen_Adr[n]=Screen_Buffer[n];

if (Cfg->_4dos==1)
    {
    _4DOSShistdir();
    }

GotoXY(0,PosY);
_settextcolor(OldCol);
cprintf("%s\n\r",RBTitle);

exit(1);
}

void AfficheTout(void)
{
PrintAt(0,0,"%-40s%40s","Ketchup Killers Commander","RedBug");
ColLin( 0,0,40,1*16+5);
ColLin(40,0,40,1*16+3);
ColLin(0,(Cfg->TailleY)-2,80,7);

DFen=Fenetre[Cfg->FenAct];

CommandLine("##INIT 0 %d 80\n",(Cfg->TailleY)-2);

DFen->init=1;
DFen->Fen2->init=1;

ChangeLine();

MenuBar(3);
}

// TXTMode(OldY);


/**************************
 - Affichage des fenetres -
 **************************/

void AffFen(struct fenetre *Fen)
{
int x,y;

int nbuf,nscr;

switch (Fen->FenTyp) {
    case 0:
        FenNor(Fen);
        break;
    case 1:      // FenDIZ --> A lieu au moment de IDF
        break;
    case 2:      // OFF
        for(y=0;y<=Fen->yl;y++)
            {
            nscr=(y+Fen->y)*160;
            nbuf=(y-Fen->yl+OldY-1)*160;

            nbuf+=(Fen->x)*2;
            nscr+=(Fen->x)*2;
            for(x=0;x<(Fen->xl+1)*2;x++)
                {
                if (nbuf<0)
                    {
                    if ((x&1)==0)
                        Screen_Adr[nscr]=32;
                        else
                        Screen_Adr[nscr]=7;
                    }
                    else
                    {
                    if ((x&1)==0)
                        Screen_Adr[nscr]=CnvASCII(Screen_Buffer[nbuf]);
                        else
                        Screen_Adr[nscr]=Screen_Buffer[nbuf];
                    }
                nbuf++;
                nscr++;
                }
            }
        break;
    case 3:
        FenNor(Fen);
        break;
    }
}



/****************
 - Gestion 4DOS -
 ****************/

// Put Cfg->_4dos on if 4dos found
void _4DOSverif(void)
{
union REGS R;

R.w.ax=0xD44D;
R.h.bh=0;

int386(0x2F,&R,&R);

if (R.w.ax==0x44DD)  {
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

if (R.w.ax==0x44DD)  {
    Cfg->_4dos=1;

    seg=R.w.cx;
    adr=(unsigned short*)(seg*16+0x290);

    seg=adr[1];
    adr=(unsigned short*)(seg*16+0x4C60);

    for (n=0;n<255;n++)  {
        a=((char*)adr)[n];
        Cfg->HistDir[n]=a;
        }
    }
    else
    {
    Cfg->_4dos=0;
    }
}

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

if (R.w.ax==0x44DD)  {
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

/***************************
 - Save Configuration File -
 ***************************/

void SaveCfg(void)
{
int m,n,t,ns;
FILE *fic;
struct fenetre *Fen;
short taille;

Cfg->FenTyp[0]=Fenetre[0]->FenTyp;
Cfg->FenTyp[1]=Fenetre[1]->FenTyp;

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


for(t=0;t<2;t++)
{
Fen=Fenetre[t];

fwrite(Fen->path,256,1,fic);
fwrite(&(Fen->order),sizeof(ENTIER),1,fic);
fwrite(&(Fen->sorting),sizeof(ENTIER),1,fic);


fwrite(&(Fen->nbrsel),4,1,fic);
ns=Fen->nbrsel;

for (n=0;n<Fen->nbrfic;n++)
    {
    if (Fen->F[n]->select==1)
        {
        ns--;
        m=strlen(Fen->F[n]->name);
        fwrite(&m,4,1,fic);
        fwrite(Fen->F[n]->name,1,m,fic);
        if (ns==0) break;
        }
    }

m=strlen(Fen->F[Fen->pcur]->name);
fwrite(&m,4,1,fic);
fwrite(Fen->F[Fen->pcur]->name,1,m,fic);

fwrite(&(Fen->scur),sizeof(ENTIER),1,fic);
}

fclose(fic);
}


/***************************
 - Load Configuration File -
 ***************************/

// Retourne -1 en cas d'erreur
//           0 si tout va bien
int LoadCfg(void)
{
int m,n,i,nbr;
int t;
FILE *fic;
char nom[256],tnom[256];
short taille;


fic=fopen(Fics->CfgFile,"rb");
if (fic==NULL) return -1;

if (fread((void*)Cfg,sizeof(struct config),1,fic)==0)
    {
    fclose(fic);
    return -1;
    }

if ( (Cfg->overflow!=0) |
     (Cfg->crc!=0x69) )
    {
    fclose(fic);
    return -1;
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

for (t=0;t<2;t++)
    {
    DFen=Fenetre[t];

    DFen->FenTyp=Cfg->FenTyp[t];
    
    fread(DFen->path,256,1,fic);
    fread(&(DFen->order),sizeof(ENTIER),1,fic);
    fread(&(DFen->sorting),sizeof(ENTIER),1,fic);

    IOver=1;
    IOerr=0;

    if (chdir(DFen->path)!=0)
        {
        strcpy(nom,DFen->path);
        strcpy(tnom,"");
        do
            {
            n=0;
            for (i=0;i<strlen(DFen->path);i++)
                if (DFen->path[i]=='\\') n=i;

            if (n==0) break;

            strcpy(tnom,DFen->path+n+1);
            DFen->path[n]=0;
            if (chdir(DFen->path)==0) break;
            }
        while(1);

        if (n==0)
            {
            memcpy(DFen->path,"C:\\",4);
            CommandLine("#cd .");
            }
            else
            {
            CommandLine("#cd .");
            for (i=0;i<DFen->nbrfic;i++)
                if (!strncmp(tnom,DFen->F[i]->name,strlen(tnom)))  {
                    DFen->pcur=i;
                    DFen->scur=i;
                    n=0;
                    break;
                    }
            if (n==0)
                {
                EnterArchive();
                if (strlen(nom)!=strlen(DFen->VolName))
                    CommandLine("#cd %s",nom+strlen(DFen->VolName)+1);
                }
            }
        }
        else
        {
        CommandLine("#cd .");
        }


    IOver=0;

    fread(&nbr,4,1,fic);

    DFen->nbrsel=0;

    for (i=0;i<nbr;i++)
        {
        fread(&m,4,1,fic);
        fread(nom,m,1,fic);
        nom[m]=0;

        for (n=0;n<DFen->nbrfic;n++)
            {
            if (!stricmp(nom,DFen->F[n]->name))
                {
                DFen->F[n]->select=1;
                DFen->nbrsel++;
                DFen->taillesel+=DFen->F[n]->size;
                }
            }
        }

    fread(&m,4,1,fic);
    fread(nom,m,1,fic);
    nom[m]=0;

    fread(&(DFen->scur),sizeof(ENTIER),1,fic);

    for (n=0;n<DFen->nbrfic;n++)
        {
        if (!stricmp(nom,DFen->F[n]->name))
            DFen->pcur=n;
        }
    }

fclose(fic);

return 0;
}



/* ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ *\
   ³                                                                       ³
   ³                                                                       ³
   ³   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿   ³
   ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´  MAIN  ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´   ³
   ³   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ   ³
   ³                                                                       ³
   ³                                                                       ³
\* ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ */

void main(int argc,char **argv)
{
char *path;
int n;
char *LC;

OldY=(*(char*)(0x484))+1;
WhereXY(&PosX,&PosY);
OldCol=_gettextcolor();



/**************************
 - Initialise les buffers -
 **************************/

Fenetre[0]=GetMem(sizeof(struct fenetre));
Fenetre[0]->F=GetMem(TOTFIC*sizeof(void *));        // allocation des pointeurs

Fenetre[1]=GetMem(sizeof(struct fenetre));
Fenetre[1]->F=GetMem(TOTFIC*sizeof(void *));        // allocation des pointeurs

Fenetre[2]=GetMem(sizeof(struct fenetre));
Fenetre[2]->F=GetMem(TOTFIC*sizeof(void *));        // allocation des pointeurs

Cfg=GetMem(sizeof(struct config));
Fics=GetMem(sizeof(struct fichier));

Mask=GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    Mask[n]=GetMem(sizeof(struct PourMask));


Screen_Buffer=GetMem(8000);
for (n=0;n<8000;n++)
    Screen_Buffer[n]=Screen_Adr[n];

path=GetMem(256);

/*****************************************
 - Lecture et verification des arguments -
 *****************************************/


strcpy(ShellAdr+128,*argv);
strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
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


/***********************
 - Gestion des erreurs -
 ***********************/

_harderr(Error_handler);

signal(SIGABRT,Signal_Handler);
signal(SIGFPE,Signal_Handler);
signal(SIGILL,Signal_Handler);
signal(SIGINT,Signal_Handler);
signal(SIGSEGV,Signal_Handler);
signal(SIGTERM,Signal_Handler);



/*******************************
 - Initialisation des fichiers -
 *******************************/

Fics->LastDir=GetMem(256);
getcwd(Fics->LastDir,256);

Fics->FicIdfFile=GetMem(256);
strcpy(Fics->FicIdfFile,path);
strcat(Fics->FicIdfFile,"\\idfext.rb");

Fics->CfgFile=GetMem(256);
strcpy(Fics->CfgFile,path);
strcat(Fics->CfgFile,"\\kkrb.cfg");

Fics->view=GetMem(256);
strcpy(Fics->view,path);
strcat(Fics->view,"\\view");

Fics->edit=GetMem(256);
strcpy(Fics->edit,path);
strcat(Fics->edit,"\\edit");

Fics->path=GetMem(256);
strcpy(Fics->path,path);

Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\kkc.hlp");

Fics->temp=GetMem(256);
strcpy(Fics->temp,path);
strcat(Fics->temp,"\\temp.tmp");

Fics->trash=GetMem(256);
strcpy(Fics->trash,path);
strcat(Fics->trash,"\\trash");      // repertoire trash

Fics->log=GetMem(256);
strcpy(Fics->log,path);
strcat(Fics->log,"\\trash\\logfile");      // logfile trash



/*************
 -  Default  -
 *************/

Cfg->TailleY=50;

ChangeType(4);
Cfg->SaveSpeed=7200;

Cfg->_4dos=0;
_4DOSverif();

if (Cfg->_4dos==1)  {
    _4DOSLhistdir();
    }
    else
    memset(Cfg->HistDir,0,256);



/******************
 - Initialisation -
 ******************/
Fenetre[0]->x=0;
Fenetre[0]->y=1;
Fenetre[0]->yl=(Cfg->TailleY)-4;
Fenetre[0]->xl=39;
Fenetre[0]->actif=1;

Fenetre[0]->pcur=0;
Fenetre[0]->scur=0;

Fenetre[0]->FenTyp=0;

Fenetre[0]->Fen2=Fenetre[1];
Fenetre[0]->order=1;


Fenetre[1]->x=40;
Fenetre[1]->y=1;
Fenetre[1]->yl=(Cfg->TailleY)-4;
Fenetre[1]->xl=39;
Fenetre[1]->actif=0;

Fenetre[1]->pcur=0;
Fenetre[1]->scur=0;

Fenetre[1]->FenTyp=0;

Fenetre[1]->Fen2=Fenetre[0];
Fenetre[1]->order=1;


/**********************************************
 - Chargement du fichier config (s'il existe) -
 **********************************************/

if (LoadCfg()==-1)
    {
    DFen=Fenetre[0];
    CommandLine("#c:");
    DFen=Fenetre[1];
    CommandLine("#c:");

    DefaultCfg();
    }

/*
if (LC[4]=='0')
    {
    cprintf("Press a key to return %s",RBTitle);
    Wait(0,0,0);
    }
*/


VerifHistDir();                 // Verifie l'history pour les repertoires

ChangeTaille(Cfg->TailleY);     // Change de taille et affiche tout

Gestion();


/*********
 -  FIN  -
 *********/

TXTMode(OldY);          // Retablit le mode texte normal

ShellAdr[0]=0;
Fin();
}



