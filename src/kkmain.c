/*--------------------------------------------------------------------*\
|- Programme principal                                                -|
\*--------------------------------------------------------------------*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>                               // For handling signal

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
void SelectPlusMoins(void);
void SelectMoins(void);
void CreateKKD(void);
void EditFile(char *s);
void CreateDirectory(void);
void WinCD(void);
void HistDir(void);
void PathDir(void);
void WinRename(FENETRE *F1);
void SwitchScreen(void);
void ChangeType(char n);
void ChangeDrive(void);
void EditNewFile(void);
int EnterArchive(void);
void SwapWin(long a,long b);
void SwapLong(long *a,long *b);
void HistCom(void);
void AffLonger(void);

void RBSetup(void);

void SaveRawPage(void);


/*--------------------------------------------------------------------*\
|-  Declaration des variables                                         -|
\*--------------------------------------------------------------------*/

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

char *ShellAdr=(char*)0xBB000;

char *Screen_Adr=(char*)0xB8000;

FENETRE *Fenetre[NBWIN];

FENETRE *DFen;

extern int IOver;
extern int IOerr;

struct kkconfig *KKCfg;
struct kkfichier *KKFics;

char QuitKKC=0;


/*--------------------------------------------------------------------*\
|- Gestion macros                                                     -|
\*--------------------------------------------------------------------*/

short FctStack[128];
short NbrFunct;


/*--------------------------------------------------------------------*\
|- Init Function Stack                                                -|
\*--------------------------------------------------------------------*/
void InitFctStack(void)
{
NbrFunct=0;
}

/*--------------------------------------------------------------------*\
|- Put a Function LIFO                                                -|
\*--------------------------------------------------------------------*/
void PutLIFOFct(int fct)
{
if (NbrFunct>128) return;
FctStack[NbrFunct]=fct;
NbrFunct++;
}

/*--------------------------------------------------------------------*\
|- Get a Function LIFO                                                -|
\*--------------------------------------------------------------------*/
int GetLIFOFct(void)
{
if (NbrFunct==0) return 0;
NbrFunct--;
return FctStack[NbrFunct];
}


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/





void HelpOnError(void)
{
static char buf[3][81];
int i,j;

for(j=0;j<3;j++)
    {
    for(i=0;i<80;i++)
        buf[j][i]=Screen_Buffer[(i+(PosY-j-1)*OldX)*2];
    buf[j][80]=0;
    }

if  ( (!strncmp(buf[2],"Stub exec failed:",17)) &
      (!strncmp(buf[1],"dos4gw.exe",10)) &
      (!strncmp(buf[0],"No such file or directory",25)) )
        HelpTopic("dos4gw");
}

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


if (n==-1) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

strcpy(nom,Dir);
strcpy(Dir,Fen->path);
Path2Abs(Dir,nom);

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
                FicSelect(Fen,i,1);                            // Select
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
|- Change le type d'affichage                                         -|
\*--------------------------------------------------------------------*/
void SwitchScreen(void)
{
int nbr;
static struct barmenu bar[3];
int retour;
MENU menu;

nbr=3;

bar[0].Titre="Normal Mode ";
bar[0].Help=NULL;
bar[0].fct=1;

bar[1].Titre="Ansi Mode   ";
bar[1].Help=NULL;
bar[1].fct=2;

bar[2].Titre="Doorway Mode";
bar[2].Help=NULL;
bar[2].fct=3;

menu.x=((Cfg->TailleX)-10)/2;
menu.y=((Cfg->TailleY)-2*(nbr-2))/2;
menu.attr=0;
menu.cur=0;


do
    {
    retour=PannelMenu(bar,nbr,&menu);

    if (retour==2)
        {
        DesinitScreen();
        Cfg->display=bar[menu.cur].fct-1;
        if (InitScreen(Cfg->display)) break;
        }
    }
while (retour!=0);

if (retour!=0)
    {
    UseCfg();
    }
}

void RBSetup(void)
{
char bartxt[81];
int car;

if (Wait(0,0)!='R') return;
if (Wait(0,0)!='B') return;

SaveScreen();

PrintAt(0,0,"%-40s%*s","RedBug's own setup",Cfg->TailleX-40,"Hi Me !");
ColLin( 0,0,40,Cfg->col[7]);
ColLin(40,0,(Cfg->TailleX)-40,Cfg->col[11]);

strcpy(bartxt,
        " ----  ----  ---- SpTest ----  ----  ----  ----  ----  Quit ");

do
{
memcpy(bartxt+ 6,KKCfg->savekey ? "Key On" : "KeyOff",6);
memcpy(bartxt+12,Cfg->debug     ? "Debug " : "No deb",6);

Bar(bartxt);

car=Wait(0,0);

switch(HI(car))
    {
    case 0x3C: //--- F2 ------------------------------------------------
        KKCfg->savekey^=1;
        break;
    case 0x3D: //--- F3 ------------------------------------------------
        Cfg->debug^=1;
        break;
    case 0x3E: //--- F4 ------------------------------------------------
        SpeedTest();
        break;
    case 0x44: //--- F10 -----------------------------------------------
        car=27;
        break;
    }




}
while(LO(car)!=27);


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
|- 45: Longernal editor                                               -|
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
|- 80: Help on Error                                                  -|
|- 81: Switch la taille horizontale                                   -|
|- 82: Change les couleurs                                            -|
|- 83: Fenetre changement de dir d'apres PATH                         -|
|- 84: ENTER                                                          -|
|- 85: R‚solution de s‚curit‚                                         -|
|- 86: Eject le support                                               -|
|- 87: Pannel Menu                                                    -|
|- 88: Previent le centre de contr“le que l'on veut quitter KKC       -|
|- 89: Va dans l'autre fenˆtre                                        -|
|- 90: Sauvegarde et affiche le fond avec le viewer ansi              -|
|- 91: Appelle le mode console                                        -|
|- 92: Selection ou deselectionne des fichiers                        -|
\*--------------------------------------------------------------------*/

void GestionFct(int fct)
{
FENETRE *FenOld;
static char buffer[256],buf2[256];
char *ext;
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
            FicSelect(DFen,i,2);
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
        if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
            break;

        if (DFen->FenTyp!=0) return;

        strcpy(buffer,DFen->path);
        Path2Abs(buffer,DFen->F[DFen->pcur]->name);

        switch(DFen->system)
            {
            case 0:
                View(&(KKCfg->V),buffer,0);
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
            DFen->FenTyp=2;
        break;
    case 15:                                       // Close right window
        DFen=Fenetre[1];
        if (DFen->FenTyp==2)
            {
            DFen->init=1;
            DFen->FenTyp=0;
            }
        else
            DFen->FenTyp=2;
        break;
    case 16:                                      // Select current file
        FicSelect(DFen,DFen->pcur,2);

        if (!strcmp(DFen->F[DFen->pcur]->name,"."))
            for (i=0;i<DFen->nbrfic;i++)
                if (!WildCmp(DFen->F[i]->name,Select_Chaine))
                    FicSelect(DFen,i,1);

        if (KKCfg->insdown==1)
            GestionFct(36);  // Descent d'une ligne --------------------
        break;
    case 17:                                           // Change palette
        strcpy(buffer,Fics->path);
        Path2Abs(buffer,"kksetup.exe -PALETTE");
        KKCfg->scrrest=0;
        CommandLine("#%s",buffer);
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
                      FicSelect(DFen,i,1);                // Select file
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
    case 27:                                            // Reload CTRL-R
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
        strcpy(buffer,KKFics->trash);
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

        strcpy(buffer,KKFics->trash);
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
        CommandLine("#CD %s",KKFics->trash);
        break;
    case 43:
        KKCfg->FenAct=(KKCfg->FenAct)&1;
        DFen=Fenetre[KKCfg->FenAct];
        DFen->ChangeLine=1;                            // Affichage Path
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
        DFen->init=1;
        DFen=FenOld;

        DFen->ChangeLine=1;                            // Affichage Path
        break;
    case 50:                          // Change le drive de la fenetre 2
        FenOld=DFen;
        DFen=Fenetre[1];

        ChangeDrive();
        DFen->init=1;
        DFen=FenOld;

        DFen->ChangeLine=1;                            // Affichage Path
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
            DFen->ChangeLine=1;                        // Affichage Path
            }
        break;
    case 57:
        if (DFen->Fen2->FenTyp==3)
            DFen->InfoPos+=5;
            else
            {
            KKCfg->FenAct=1;
            DFen=Fenetre[KKCfg->FenAct];
            DFen->ChangeLine=1;                        // Affichage Path
            }
        break;
    case 58:        // Change la fenˆtre avec la path de l'autre fenˆtre
        CommandLine("#CD %s",DFen->Fen2->path);
        break;
    case 59:
        EditNewFile();
        break;
    case 60:
        SwitchScreen();
        break;
    case 61:
#ifdef DOOR
        ServerMode();
#endif
        break;
    case 62:                                                  // KKSETUP
        strcpy(buffer,Fics->path);
        Path2Abs(buffer,"kksetup.exe");
        KKCfg->scrrest=0;
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
        RBSetup();
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
        DFen->ChangeLine=1;                            // Affichage Path
        break;
    case 73:
        ExecCom();
        break;
    case 74:
        View(&(KKCfg->V),KKFics->temp,0);
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
    case 80:
        HelpOnError();
        break;
    case 81:
        if (Cfg->TailleX==80)
            Cfg->TailleX=90;
            else
            Cfg->TailleX=80;
        ChangeTaille(Cfg->TailleY);
        break;
    case 82:
        strcpy(buffer,Fics->path);
        Path2Abs(buffer,"kksetup.exe -COLOR");
        KKCfg->scrrest=0;
        CommandLine("#%s",buffer);
        break;
    case 83:
        PathDir();
        break;
    case 84:
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
                strcpy(buf2,DFen->path);
                Path2Abs(buf2,DFen->F[DFen->pcur]->name);
                i=FicIdf(buffer,buf2,i,0);

                switch(i)
                    {
                    case 0: //-- Execution de la ligne de commande -----
                        CommandLine(buffer);
                        CommandLine("\n");
                        break;
                    case 1: //-- Pas de fichier IDFEXT.RB --------------
                        WinError("You must search player in setup");
                        break;
                    case 2: //-- No Player for this file ---------------
//                         CommandLine(DFen->F[DFen->pcur]->name);
                         break;
                    case 3: //-- ESCape --------------------------------
                        break;
                    case 4: //-- Player Management ---------------------
                        FicIdfMan(-1,buffer);
                        break;
                    }
                break;
            }
        break;
    case 85:
        {
        static char oldpal[48],oldcol[64];
        static char old=0;
        static int tx,ty,font,type,dispath,pathdown;

        char pal[]="\x00\x00\x00\x00\x00\x2A\x00\x2A\x00\x00\x2A\x2A"
                   "\x2A\x00\x00\x2A\x00\x2A\x2A\x15\x00\x2A\x2A\x2A"
                   "\x15\x15\x15\x15\x15\x3F\x15\x3F\x15\x15\x3F\x3F"
                   "\x3F\x15\x15\x3F\x15\x3F\x3F\x3F\x15\x3F\x3F\x3F";
        char col[]="\x1B\x30\x1E\x3E\x1E\x03\x30\x30\x0F\x30\x3F\x3E"
                   "\x0F\x0E\x30\x19\x1B\x13\x30\x3F\x3E\x0F\x15\x12"
                   "\x30\x3F\x0F\x3E\x4F\x4E\x70\x00\x14\x13\xB4\x60"
                   "\x70\x1B\x1B\x1B\x1E\x30\x30\x0F\x3F\x3F\x4F\x4F"
                   "\x3F\x0F\x80\x90\x30\x30\x0E\x1B\x1B\xA0\xB0\xC0"
                   "\x04\x03\x05\x07";

        if (!old)
            {
            memcpy(oldpal,Cfg->palette,48);
            memcpy(oldcol,Cfg->col,64);
            tx=Cfg->TailleX;
            ty=Cfg->TailleY;
            font=Cfg->font;
            type=KKCfg->fentype;
            dispath=KKCfg->dispath;
            pathdown=KKCfg->pathdown;

            memcpy(Cfg->palette,pal,48);
            memcpy(Cfg->col,col,64);

            Cfg->TailleY=25;
            Cfg->TailleX=80;
            Cfg->font=0;

            KKCfg->fentype=1;
            KKCfg->dispath=1;
            KKCfg->pathdown=0;

            DesinitScreen();
            Cfg->display=0; //--- Normal Output ----------------------------
            InitScreen(0);
            old=1;

            }
        else
            {
            memcpy(Cfg->palette,oldpal,48);
            memcpy(Cfg->col,oldcol,64);

            Cfg->TailleY=ty;
            Cfg->TailleX=tx;
            Cfg->font=font;

            KKCfg->fentype=type;

            KKCfg->dispath=dispath;
            KKCfg->pathdown=pathdown;
            old=0;
            }

        GestionFct(67);                            // Rafraichit l'ecran
        }
        break;
    case 86:
        EjectCD(DFen);
        break;
    case 87:
        i=GestionBar(0);

        if (i==20)
            GestionFct(88);                                       // F10
            else
            GestionFct(i);
        break;
    case 88:
        QuitKKC=1;
        break;
    case 89:
        KKCfg->FenAct= (KKCfg->FenAct==1) ? 0:1;
        DFen=Fenetre[KKCfg->FenAct];
        DFen->ChangeLine=1; //--- Affichage path plus tard -------------
        break;
    case 90:
        SaveRawPage();
        GestionFct(27);
        break;
    case 91:
        Console();
        break;
    case 92:
        SelectPlusMoins();
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
MENU menu;

short fin;

static int cpos[20],poscur;

SaveScreen();

if (i==0)
    retour=0;
/*
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
*/

do
{
bar[0].Titre="File";
bar[1].Titre="Panel";
bar[2].Titre="Disk";
bar[3].Titre="Selection";
bar[4].Titre="Tools";
bar[5].Titre="Options";
bar[6].Titre="Help";


bar[0].Help=NULL;
bar[1].Help=NULL;
bar[2].Help=NULL;
bar[3].Help=NULL;
bar[4].Help=NULL;
bar[5].Help=NULL;
bar[6].Help=NULL;


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
   bar[0].Titre="View                F3";bar[0].fct=7;
   bar[0].Help="View";
   bar[1].Titre="Quick View    Shift-F3";bar[1].fct=8;
   bar[1].Help="View";
   bar[2].Titre="Edit                F4";bar[2].fct=9;
   bar[2].Help="Edit";
   bar[3].Titre=NULL;                    bar[3].fct=0;
   bar[3].Help=NULL;
   bar[4].Titre="Copy                F5";bar[4].fct=10;
   bar[4].Help="Copy";
   bar[5].Titre="Move                F6";bar[5].fct=11;
   bar[5].Help="Move";
   bar[6].Titre="Create Directory... F7";bar[6].fct=12;
   bar[6].Help="Create directory";
   bar[7].Titre="Delete              F8";bar[7].fct=13;
   bar[7].Help="Delete";
   bar[8].Titre=NULL;                    bar[8].fct=0;
   bar[8].Help=NULL;
   bar[9].Titre="Exit               F10";bar[9].fct=20;
   bar[9].Help=NULL;
   nbmenu=10;
   break;
 case 1:
   bar[0].Titre="Close left window    CTRL-F1";  bar[0].fct=14;
   bar[0].Help=NULL;
   bar[1].Titre="Close right window   CTRL-F2";  bar[1].fct=15;
   bar[1].Help=NULL;
   bar[2].Titre=NULL;                            bar[2].fct=0;
   bar[2].Help=NULL;
   bar[3].Titre="DIZ Window            ALT-F3";  bar[3].fct=21;
   bar[3].Help=NULL;
   bar[4].Titre="Info Window           CTRL-L";  bar[4].fct=63;
   bar[4].Help=NULL;
   bar[5].Titre=NULL;                            bar[5].fct=0;
   bar[5].Help=NULL;
   bar[6].Titre="Name                 CTRL-F3";  bar[6].fct=22;
   bar[6].Help=NULL;
   bar[7].Titre="Extension            CTRL-F4";  bar[7].fct=23;
   bar[7].Help=NULL;
   bar[8].Titre="Time/Date            CTRL-F5";  bar[8].fct=24;
   bar[8].Help=NULL;
   bar[9].Titre="Size                 CTRL-F6";  bar[9].fct=25;
   bar[9].Help=NULL;
   bar[10].Titre="Unsort               CTRL-F7"; bar[10].fct=26;
   bar[10].Help=NULL;
   nbmenu=11;
   break;
 case 2:
   bar[0].Titre="Create KKD";           bar[0].fct=6;
   bar[0].Help="KKD";
   bar[1].Titre=NULL;                   bar[1].fct=0;
   bar[1].Help=NULL;
   bar[2].Titre="Erase files in trash"; bar[2].fct=34;
   bar[2].Help=NULL;
   nbmenu=3;
   break;
 case 3:
   bar[0].Titre="Select group...     Gray '+'";  bar[0].fct=3;
   bar[0].Help="Selection of files";
   bar[1].Titre="Unselect group...   Gray '-'";  bar[1].fct=4;
   bar[1].Help="Deselection of files";
   bar[2].Titre="Invert Selection    Gray '*'";  bar[2].fct=2;
   bar[2].Help="Inverse";
   nbmenu=3;
   break;
 case 4:
   bar[0].Titre="Search File...     Alt-F7"; bar[0].fct=5;
   bar[0].Help=NULL;
   bar[1].Titre="Select temporary File   ý"; bar[1].fct=19;
   bar[1].Help=NULL;
   bar[2].Titre=NULL;                        bar[2].fct=0;
   bar[2].Help=NULL;
   bar[3].Titre="ASCII Table        CTRL-A"; bar[3].fct=28;
   bar[3].Help=NULL;
   bar[4].Titre="Screen Saver             "; bar[4].fct=76;
   bar[4].Help=NULL;
   bar[5].Titre=NULL;                        bar[5].fct=0;
   bar[5].Help=NULL;
   bar[6].Titre="View/Save Background     "; bar[6].fct=90;
   bar[6].Help=NULL;
   nbmenu=7;
   break;
 case 5:
   bar[0].Titre="Configuration   ";   bar[0].fct=31;
   bar[0].Help=NULL;
   bar[1].Titre="Palette Setup   ";   bar[1].fct=17;
   bar[1].Help=NULL;
   bar[2].Titre="Color Definition";   bar[2].fct=82;
   bar[2].Help=NULL;
   bar[3].Titre="Screen Setup    ";   bar[3].fct=66;
   bar[3].Help=NULL;
   bar[4].Titre=NULL;                 bar[4].fct=0;
   bar[4].Help=NULL;
   bar[5].Titre="Main Setup      ";   bar[5].fct=62;
   bar[5].Help=NULL;
   nbmenu=6;
   break;
 case 6:
   bar[0].Titre="Help ";    bar[0].fct=1;
   bar[0].Help=NULL;
   bar[1].Titre="About";    bar[1].fct=18;
   bar[1].Help=NULL;
   nbmenu=2;
   break;
   }

s=2;

menu.x=x;
menu.y=s;
menu.attr=0;
menu.cur=cpos[poscur];

retour=PannelMenu(bar,nbmenu,&menu);      // (x,y)=(t,s)

cpos[poscur]=menu.cur;

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

CalcSizeWin(Fenetre[0]);
CalcSizeWin(Fenetre[1]);
}


/*--------------------------------------------------------------------*\
|-                        Fenetre Select File                         -|
\*--------------------------------------------------------------------*/

void SelectPlus(void)
{
static int DirLength=32,Width=2;
static char CadreLength=32;
int n;

struct Tmt T[] = {
     { 2,3,1,Select_Chaine,&DirLength},
     { 3,5,2,NULL,NULL},                                       // le OK
     {20,5,3,NULL,NULL},                                   // le CANCEL
     { 3,2,0,"File Mask",NULL},
     { 1,1,9,&CadreLength,&Width} };

struct TmtWin F = {-1,5,36,12,"Selection of files"};

n=WinTraite(T,5,&F,0);
if (n==-1) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

for (n=0;n<DFen->nbrfic;n++)
    if ( (KKCfg->seldir==1) |
                         ((DFen->F[n]->attrib & RB_SUBDIR)!=RB_SUBDIR) )
            if (!WildCmp(DFen->F[n]->name,Select_Chaine))
                FicSelect(DFen,n,1);
}

/*--------------------------------------------------------------------*\
|-                       Fenetre Deselect File                        -|
\*--------------------------------------------------------------------*/

void SelectMoins(void)
{
static int DirLength=32,Width=2;
static char CadreLength=32;
int n;

struct Tmt T[] = {
     { 2,3,1,Select_Chaine,&DirLength},
     { 3,5,2,NULL,NULL},                                       // le OK
     {20,5,3,NULL,NULL},                                   // le CANCEL
     { 3,2,0,"File Mask",NULL},
     { 1,1,9,&CadreLength,&Width} };

struct TmtWin F = {-1,5,36,12,"Deselection of files"};

n=WinTraite(T,5,&F,0);

if (n==-1) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

for (n=0;n<DFen->nbrfic;n++)
    if ( (KKCfg->seldir==1) |
                         ((DFen->F[n]->attrib & RB_SUBDIR)!=RB_SUBDIR) )
        if (!WildCmp(DFen->F[n]->name,Select_Chaine))
            FicSelect(DFen,n,0);
}


void SelectPlusMoins(void)
{
int n;
int res;

//--- Internal Variable ---

static int KKTmp0=32;
static char KKTmpX1=32;
static int KKTmpY1=1;

//--- User Field ---

static char KKField0[33];

struct TmtWin F = {-1,5,46,11,"Select/Unselect"};

struct Tmt T[] = {
    {12, 2, 1,KKField0,&KKTmp0},
    {11, 1, 9,&KKTmpX1,&KKTmpY1},
    { 1, 4, 5,"    Select",NULL},
    {16, 4, 5,"  Unselect",NULL},
    {31, 4, 3,NULL,NULL},
    { 1, 2, 0,"Selection:",NULL}
   };

strcpy(KKField0,"*.*");

res=WinTraite(T,6,&F,0);

if (res==-1) return;                                           // ESCape
if (T[res].type==3) return;                                    // Cancel

for (n=0;n<DFen->nbrfic;n++)
    if ( (KKCfg->seldir==1) |
                         ((DFen->F[n]->attrib & RB_SUBDIR)!=RB_SUBDIR) )
        if (!WildCmp(DFen->F[n]->name,KKField0))
            {
            if (res==3)
                FicSelect(DFen,n,0);

            if (res==2)
                FicSelect(DFen,n,1);

            }
}


/*--------------------------------------------------------------------*\
|-                      Fenetre History Directory                     -|
\*--------------------------------------------------------------------*/



void HistDir(void)
{
MENU menu;

int i,j;
static struct barmenu dir[86];  // 256:3

j=0;
for (i=0;i<86;i++)
    {
    dir[i].Titre=KKCfg->HistDir+j;
    dir[i].Help=NULL;
    dir[i].fct=i+1;
    dir[i].Titre=StrUpr(dir[i].Titre);
    if (strlen(dir[i].Titre)==0) break;
    while ( (j!=256) & (KKCfg->HistDir[j]!=0) ) j++;
    j++;
    }


menu.x=2;
menu.y=2;

menu.attr=2+8;

menu.cur=i-1;

if (i!=0)
    {
    if (PannelMenu(dir,i,&menu)==2)
        CommandLine("#cd %s",dir[menu.cur].Titre);
    }
}

void PathDir(void)
{
MENU menu;

int i,j,k;
static struct barmenu dir[50];
char *TPath;
static char path[2048];

TPath=getenv("PATH");
strcpy(path,TPath);
k=strlen(path);

for(i=0;i<k;i++)
    if (path[i]==';')
        path[i]=0;

j=0;
for (i=0;i<50;i++)
    {
    dir[i].Titre=path+j;
    dir[i].Help=NULL;
    dir[i].fct=i+1;
    dir[i].Titre=strupr(dir[i].Titre);
    if (strlen(dir[i].Titre)==0) break;
    while ( (j!=k) & (path[j]!=0) ) j++;
    j++;
    }

menu.x=2;
menu.y=2;

menu.attr=2+8;

menu.cur=0;

if (i!=0)
    {
    if (PannelMenu(dir,i,&menu)==2)
        CommandLine("#cd %s",dir[menu.cur].Titre);
    }
}

/*--------------------------------------------------------------------*\
|-                      Fenetre History Commande                      -|
\*--------------------------------------------------------------------*/

int HistComFct(struct barmenu *bar)
{
MenuCreat(bar->Titre,bar->Titre,DFen->path);
return 0;
}

void HistCom(void)
{
MENU menu;

char dest[256];
int i,j;
static struct barmenu dir[100];

j=0;
for (i=0;i<100;i++)
    {
    History2Line(KKCfg->HistCom+j,dest);
    dest[76]=0;
    if (strlen(dest)==0) break;
    dir[i].Titre=(char*)GetMem(strlen(dest)+1);
    strcpy(dir[i].Titre,dest);
    dir[i].fct=i+1;
    dir[i].Help="histcom";

    while ( (j!=512) & (KKCfg->HistCom[j]!=0) ) j++;
    j++;
    }

NewEvents(HistComFct,"CrMenu",2);

menu.x=2;
menu.y=2;

menu.attr=2+8;

menu.cur=0;

if (i!=0)
    {
    if (PannelMenu(dir,i,&menu)==2)
        {
        History2Line(dir[menu.cur].Titre,dest);
        CommandLine("%s\n",dest);
        }
    }

ClearEvents();

for(j=0;j<i;j++)
    LibMem(dir[j].Titre);
}


/*--------------------------------------------------------------------*\
|-                         Fenetre CD machin                          -|
\*--------------------------------------------------------------------*/

void WinCD(void)
{
static char Dir[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[] =
      { { 2,3,1, Dir, &DirLength},
        { 4,5,2,NULL,NULL},
        {21,5,5,"  Normal CD  ",&DirLength},     // "normal cd "
        {38,5,5," Create Tree ",NULL},     // "create lcd"
        {55,5,3,NULL,NULL},     // Cancel
        { 5,2,0,"Change to which directory",NULL},
        { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,10,74,17, "Change Directory" };

int n;

n=WinTraite(T,7,&F,0);

if (n!=-1)
    {
    if ((T[n].type==2) | (T[n].type==1))
        {
        if (Dir[strlen(Dir)-1]!='*')
            strcat(Dir,"*");
        ExecLCD(DFen,Dir);
        }
    if (T[n].type==5)
        {
        if (T[n].entier==&DirLength)
            CommandLine("#cd %s",Dir);
            else
            MakeNCD();
        }
    }
}


/*--------------------------------------------------------------------*\
|-                Create directory in current window                  -|
\*--------------------------------------------------------------------*/

void CreateDirectory(void)
{
static char Dir[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[] =
    { { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the directory to be created",NULL},
      { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,10,74,17, "Create Directory" };

int n;

n=WinTraite(T,5,&F,0);

if (n!=-1)
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
    else
    GestionFct(45);
}

/*--------------------------------------------------------------------*\
|-                           Edit New File                            -|
\*--------------------------------------------------------------------*/

void EditNewFile(void)
{
static char Dir[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[] =
    { { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the file to be edited",NULL},
      { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,10,74,17, "Edit New File" };

int n;

n=WinTraite(T,5,&F,0);

if (n!=-1)
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

if (n!=-1)
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
char buffer[32],volume[32];
long fpos,cpos,pos,p1,p2;

char drive[26];
long m,n,x,l,nbr;
long x1;
long i,d,olddrive;

int car;

DriveSet(DFen->path);

nbr=0;

for (i=0;i<26;i++)
    {
    drive[i]=DriveExist(i);
    if (drive[i]==1)
        nbr++;
    }

l=36/nbr;
if (l>3) l=3;
if (nbr<5) l=6;
if (nbr<2) l=9;
x=(40-(l*nbr))/2+DFen->x;


SaveScreen();

Bar(" Help  ----  ----  ----  ----  ----  ----  ----  ----  ---- ");

x1=DFen->x+1;

Cadre(x1,6,x1+37,21,3,Cfg->col[55],Cfg->col[56]);
Window(x1+1,7,x1+36,20,Cfg->col[16]);
Cadre(x-1,8,x+l*nbr,10,1,Cfg->col[55],Cfg->col[56]);

Cadre(x1+1,11,x1+36,20,3,Cfg->col[55],Cfg->col[56]);

PrintAt(x1+2,7, "Choose a drive");
PrintAt(x1+2,11,"Directory...");

m=x+l/2;
for (n=0;n<26;n++)
    {
    if (drive[n]==1)
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
            if (DriveReady(i)==1)
                {
                Window(x1+2,12,x1+35,19,Cfg->col[16]);

                DrivePath(i,path);
                DriveInfo(i,buffer);

                sprintf(volume,"[%s]",buffer);

     //--- Calcule le nombre de position occup‚e par les repertoires ---
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

            if (pos==12)
                {
                sprintf(path2+10,"%21s",volume);
                PrintAt(x1+4,pos,"%s",path2);
                }
                else
                {
                if ( (pos-fpos>12) & (pos-fpos<=19) )
                    PrintAt(x1+4,pos-fpos,"%s",path2);
                }

            pos++;
            }
        }

    if (cpos!=50)
        ColLin(x1+3,cpos-fpos,32,Cfg->col[18]);

    AffCol(drive[i],9,Cfg->col[17]);
    car=Wait(0,0);

    if (cpos!=50)
        ColLin(x1+3,cpos-fpos,32,Cfg->col[16]);

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

    AffCol(drive[i],9,Cfg->col[16]);

    if (LO(car)==0)
        {
        switch(HI(car))
            {
            case 0x3B:
                HelpTopic("chdrive");
                break;
            }
        }

    if (HI(car)==0)
        {
        d=(toupper(car)-'A');
        if ( (d>=0) & (d<26) )
            if (drive[d]!=0)
                {
                if (!DriveReady(d))
                    EjectDrive(d);

                ColLin(x1+3,cpos-fpos,32,Cfg->col[16]);
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
    {
    CommandLine("#cd %s",path);
    DFen->FenTyp=0;
    }
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
    drive[i]=DriveExist(i);
    if (drive[i]==1)
        nbr++;
    }

l=36/nbr;
if (l>3) l=3;
if (nbr<5) l=6;
if (nbr<2) l=9;
x=(40-(l*nbr))/2+DFen->x;


SaveScreen();

Cadre(x-2,6,x+l*nbr+1,11,0,Cfg->col[55],Cfg->col[56]);
Window(x-1,7,x+l*nbr,10,Cfg->col[16]);

Cadre(x-1,8,x+l*nbr,10,1,Cfg->col[55],Cfg->col[56]);

PrintAt(x,7,"Select KKDdrive");

m=x+l/2;
for (n=0;n<26;n++)
    {
    if (drive[n]==1)
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

    AffCol(drive[i],9,Cfg->col[17]);

    car=Wait(0,0);
    AffCol(drive[i],9,Cfg->col[16]);

} while (LO(car)!=13);

LoadScreen();

CommandLine("#cd %c:\\",i+'A');

return i;
}

void QuickSearch(int key,char *c,char *c2)
{
static char chaine[32];
static int lng=0;

char fin,fin2;
int i,x,y,n;
char car,car2;
int car3;
signed char vit;
char bkey[]=
    {0x1E,0x30,0x2E,0x20,0x12,0x21,0x22,0x23,0x17,0x24,0x25,0x26,0x32,
      0x31,0x18,0x19,0x10,0x13,0x1F,0x14,0x16,0x2F,0x11,0x2D,0x15,0x2C};

int c1;

x=DFen->Fen2->x+3;
y=DFen->Fen2->y+3;

Cadre(x-1,y-1,x+24,y+1,0,Cfg->col[55],Cfg->col[56]);
ChrLin(x,y,24,32);
ColLin(x,y,24,Cfg->col[16]);

if (lng!=0)
    if (chaine[lng-1]!='*')
        strcat(chaine,"*");

if (WildCmp(DFen->F[DFen->pcur]->name,chaine)!=0)
    lng=0;

if (key!=0)
    lng=0;

chaine[lng]=0;

do
    {
    if (chaine[lng-1]!='*')
        PrintAt(x,y,"%23s*",chaine);
        else
        PrintAt(x,y,"%24s",chaine);

    GotoXY(x+23,y);
    if (key==0)
        {
        while(!KbHit());
        car3=_bios_keybrd(0x11);
        c1=Wait(x+23,y);
        }
        else
        {
        c1=key;
        key=0;
        }

    car=LO(c1);

    if ( ((car3&255)==0) & (car==0))
        {
        car=0;
        for(n=0;n<26;n++)
            if ((car3/256)==bkey[n])
                car=n+'a';
        }

    car2=HI(c1);

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

DFen->ChangeLine=1;

*c=car;
*c2=car2;
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

char *AccessAbsFile(char *fic)
{
FENETRE *SFen,*OldFen;
static char buf[256];
int i,k;
FILE *infic;

OldFen=DFen;

SFen=GetMem(sizeof(FENETRE));
SFen->F=GetMem(TOTFIC*sizeof(void *));

SFen->x=40;
SFen->nfen=7;
SFen->FenTyp=0;
SFen->Fen2=SFen;
SFen->y=1;
SFen->yl=(Cfg->TailleY)-4;
SFen->xl=39;
SFen->order=17;
SFen->pcur=0;
SFen->scur=0;

DFen=SFen;

strcpy(buf,fic);
Path2Abs(buf,"..");

IOver=1;
IOerr=0;

CommandLine("#CD %s\n",buf);

IOver=0;

FileinPath(fic,buf);

k=-1;
for (i=0;i<DFen->nbrfic;i++)
    if (!WildCmp(buf,DFen->F[i]->name))
        {
        k=i;
        break;
        }

if ( (DFen->system!=0) & (k!=-1) )
    {
    strcpy(buf,KKFics->trash);
    Path2Abs(buf,DFen->F[k]->name);
    infic=fopen(buf,"rb");
    if (infic!=NULL)
        {
        i=flength(fileno(infic));
        if (i==DFen->F[k]->size)
            k=-2;
        fclose(infic);
        }
    }

if (k>=0)
    strcpy(buf,AccessFile(k));

DFen=OldFen;

LibMem(SFen->F);
LibMem(SFen);

if (k==-1)
    return NULL;
    else
    return buf;
}


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
        CommandLine("#cd %s",KKFics->trash);

        KKCfg->FenAct= (KKCfg->FenAct)+2;

        DFen=FenOld;
                                 // Mets le nom du fichier dans la trash
        strcpy(buffer,KKFics->trash);
        Path2Abs(buffer,"kktrash.sav");
        fic=fopen(buffer,"at");
        strcpy(buffer,KKFics->trash);
        Path2Abs(buffer,DFen->F[n]->name);
        fprintf(fic,"%s\n",buffer);
        fclose(fic);
        KKCfg->strash+=DFen->F[n]->size;

                                     // Copie les fichiers dans la trash
        (KKCfg->noprompt)=(char)((KKCfg->noprompt)|1);
        DFen->nopcur=n;
        Copie(DFen,Fenetre[2],Fenetre[2]->path);         // Quitte aprŠs

        DFen=Fenetre[2];
        CommandLine("#cd %s",KKFics->trash);

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
|- Sauvegarde selection en m‚moire                                    -|
\*--------------------------------------------------------------------*/
char *pcurname;
char **selname;
int F1pcur,F1scur,F1nbrsel;

/*--------------------------------------------------------------------*\
|-  Sauvegarde la selection sur disque                                -|
\*--------------------------------------------------------------------*/
void SaveSel(FENETRE *F1)
{
struct file *F;
int i,j;

pcurname=GetMem(strlen(F1->F[F1->pcur]->name)+1);
strcpy(pcurname,F1->F[F1->pcur]->name);

F1pcur=F1->pcur;
F1scur=F1->scur;
F1nbrsel=F1->nbrsel;

if (F1nbrsel!=0)
    {
    selname=GetMem(sizeof(char*)*F1->nbrsel);
    j=0;

    for(i=0;i<F1->nbrfic;i++)
        {
        F=F1->F[i];

        if ((F->select)==1)
            {
            selname[j]=GetMem(strlen(F->name)+1);
            strcpy(selname[j],F->name);
            j++;
            }
        }
    }
}

/*--------------------------------------------------------------------*\
|-  Charge la selection du disque                                     -|
\*--------------------------------------------------------------------*/
void LoadSel(int n)
{
char nom[256];
int i,j;

strcpy(nom,pcurname);
LibMem(pcurname);

DFen->pcur=F1pcur;
DFen->scur=F1scur;

j=1;
for (i=0;i<DFen->nbrfic;i++)
    if (!strncmp(nom,DFen->F[i]->name,strlen(nom)))
        {
        DFen->pcur=i;

        j=0;
        break;
        }

if (F1nbrsel!=0)
    {
    for(j=0;j<F1nbrsel;j++)
        {
        strcpy(nom,selname[j]);
        LibMem(selname[j]);

        for(i=0;i<DFen->nbrfic;i++)
            {
            if (!WildCmp(DFen->F[i]->name,nom))
                {
                switch(n)
                    {
                    case 0:
                        FicSelect(DFen,i,1);                   // Select
                        break;
                    case 1:
                        FicSelect(DFen,i,2);        // Inverse Selection
                        break;
                    }
                }
            }
        }
    LibMem(selname);
    }
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


if (n!=-1)
    if (T[n].type!=3)
        if (rename(Name,Temp)!=0)
            WinError("Couldn't rename file");
}

/*--------------------------------------------------------------------*\
|-                          Programme Principal                       -|
\*--------------------------------------------------------------------*/
void Gestion(void)
{
short fct;

clock_t Cl,Cl_Start;
BYTE car,car2;

long car3,c;

int oldzm=0,xm,ym,zm;

int i;  //--- Compteur -------------------------------------------------

Info->temps=clock()-Info->temps;

do
{
do
{
KKCfg->scrrest=1;

KKCfg->noprompt&=126;                       // Met le dernier bit … z‚ro

if ( (KKCfg->key==0) & (KKCfg->FenAct>1) )
    {
    GestionFct(43); //--- Active la fenetre principale -----------------
    }
    else
    {
    if (KKCfg->FenAct>1)
        {
        DFen=Fenetre[2];
        DFen->ChangeLine=1;                            // Affichage Path
        }
        else
        DFen=Fenetre[KKCfg->FenAct];
    }

if (KKCfg->key==0)            //--- Switch if key buffer is void ---
    {
    if (DFen->FenTyp==5)
        ViewFileID(DFen);

    if ( (DFen->FenTyp!=0) & (DFen->Fen2->FenTyp==0) )
        GestionFct(89);
    }

if (Fenetre[1]->FenTyp==0)
    for (i=1;i>=0;i--)
        AffFen(Fenetre[i]);
    else
    for (i=0;i<2;i++)
        AffFen(Fenetre[i]);

fct=GetLIFOFct();

if (fct!=0)
    GestionFct(fct);
}
while(fct!=0);


if (KKCfg->key==0)
    {
    if ((KKCfg->strash>=KKCfg->mtrash) & (KKCfg->mtrash!=0))
        GestionFct(34);                 // Efface la trash si trop plein
    }

if (KKCfg->key==0)
    {
    Cl_Start=clock();
    Cl=clock();

    c=0;

    AffCmdLine();

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
            if ((xm>=DFen->x) & (xm<=DFen->xl))
                {
                if (ym>(DFen->scur+DFen->y2+1)) c=80*256;
                if (ym<(DFen->scur+DFen->y2+1)) c=72*256;
                oldzm=0;     // On peut laisser le bouton appuy‚ ---
                }
                else
            if ((xm>=DFen->Fen2->x) & (xm<=DFen->Fen2->xl))
                {
                oldzm=0;     // On peut laisser le bouton appuy‚ ---
                c=9;
                }
                else
                {
                int n;

                if (ym<DFen->yl-3)
                    {
                    n=(ym-1)/3;
                    if ( (n>=0) & (n<6) )
                        {
                        Cadre(40,1+n*3,49,3+n*3,2,
                                             Cfg->col[56],Cfg->col[55]);
                        GestionFct(KKCfg->Nmenu[n]);
                        Cadre(40,1+n*3,49,3+n*3,2,
                                             Cfg->col[55],Cfg->col[56]);
                        }
                    }
                c=3;                       //--- On ne fait rien ---
                }
            }

        if ((zm&4)==4)
            {
            if ( (ym==(DFen->scur+DFen->y2+1)) &
                 ((xm>=DFen->x) & (xm<=DFen->xl)) )
                {
                c=13;
                ReleaseButton();
                }
            }

        if ( ( ((clock()-Cl_Start)>DFen->IDFSpeed) & (Cl_Start!=0)) |
             (DFen->init==1) | (DFen->Fen2->init==1) )
            {
            InfoIDF(DFen);// information idf sur fichier selectionn‚

            if (DFen->FenTyp==1) FenDIZ(DFen);

            if (DFen->FenTyp==5) FenFileID(DFen);

            Cl_Start=0;

            if (DFen->Fen2->FenTyp==1) FenDIZ(DFen->Fen2);

            if (DFen->Fen2->FenTyp==5) FenFileID(DFen->Fen2);
            }

        if (Cfg->SaveSpeed!=0)
            {
            if ((clock()-Cl)>Cfg->SaveSpeed*CLOCKS_PER_SEC)
                GestionFct(76);
            }

        car=*Keyboard_Flag1;  //--- Etat des shifts, ctrl et alt -------

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

    car3=_bios_keybrd(0x11);

    if (c==0)
        c=Wait(0,0);

    KKCfg->key=(short)c;

    if (((car&3)!=0) & (c==13))     //--- Left shift or right shift ----
        c=0,        GestionFct(85);


    //--- Positionne le pointeur sur FILE_ID.DIZ si on est sur .. ------
    switch(HI(c))
        {
        case 0x3D:  //--- F3 -------------------------------------------
        case 0x3E:  //--- F4 -------------------------------------------
        case 0x56:  //--- SHIFT-F3 -------------------------------------
        case 0x8D:  //--- CTRL-UP --------------------------------------
            if (!strcmp(DFen->F[DFen->pcur]->name,".."))
                {
                car=0;
                for (i=0;i<DFen->nbrfic;i++)
                   if (!WildCmp("FILE_ID.DIZ",DFen->F[i]->name))
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

            if (DFen->F[DFen->pcur]->name[0]=='*')
                {
                int d;
                sscanf(DFen->F[DFen->pcur]->name+1,"%d",&d);
                GestionFct(d);
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
        if (!WildCmp(KKCfg->FileName,DFen->F[i]->name))
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
    QuickSearch(0,&car,&car2);

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
switch (car3/256)
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

if ( ((car3&255)==0) & (car3!=0) )
    {
    char bkey[]=
      {0x1E,0x30,0x2E,0x20,0x12,0x21,0x22,0x23,0x17,0x24,0x25,0x26,0x32,
      0x31,0x18,0x19,0x10,0x13,0x1F,0x14,0x16,0x2F,0x11,0x2D,0x15,0x2C};
    int key,n;

    key=0;
    for(n=0;n<26;n++)
        if (bkey[n]==(car3/256))
            key=n+'a';
    if (key!=0)
        QuickSearch(key,&car,&car2);
    }


//-Switch car-----------------------------------------------------------
switch (car)
    {
    case 0x01:                                                 // CTRL-A
        GestionFct(28);        break;
    case 0x02:                                                 // CTRL-B
        GestionFct(87);        break;
    case 0x03:                                                 // CTRL-C
        break;
    case 0x04:                                                 // CTRL-D
        GestionFct(29);        break;
    case 0x05:                                                 // CTRL-E
        GestionFct(70);        break;
    case 0x06:                                                 // CTRL-F
        GestionFct(32);        break;
    case 0x09:                                           // TAB & CTRL-I
        GestionFct(89);        break;
    case 0x0A:                                    // CTRL-ENTER & CTRL-J
        GestionFct(30);        break;
    case 0x0C:                                                 // CTRL-L
        GestionFct(63);        break;
    case 0x0D:                                         // ENTER & CTRL-M
        GestionFct(84);        break;
    case 0x0F:                                                 // CTRL-O
        GestionFct(14);
        GestionFct(15);
        break;
    case 0x12:                                                 // CTRL-R
        GestionFct(27);        break;
    case 0x14:                                                 // CTRL-T
//        GestionFct(75);
        break;
    case 0x15:                                                 // CTRL-U
        GestionFct(68);        break;
    case 0x19:                                                 // CTRL-Y
        GestionFct(38);        break;
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
    case 236:                                                 // ALT-236
        GestionFct(65);        break;
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
        GestionFct(74);           break;
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
    case 0x54:                                               // SHIFT-F1
        GestionFct(80);           break;
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
        GestionFct(81);           break;
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
    case 0x6C:                                                 // ALT-F5
        GestionFct(83);           break;
    case 0x6D:                                                 // ALT-F6
        GestionFct(69);           break;
    case 0x6E:                                                 // ALT-F7
        GestionFct(5);            break;
    case 0x6F:                                                 // ALT-F8
        GestionFct(48);           break;
    case 0x70:                                                 // ALT-F9
        GestionFct(47);           break;
    case 0x71:                                                // ALT-F10
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
    case 0xA1:                                               // ALT-PGDN
        GestionFct(51);           break;
    case 0x8C:                                                // ALT-F12
        GestionFct(65);           break;
    case 0x43:                                                     // F9
        GestionFct(87);           break;
    case 0x44:                                                    // F10
        GestionFct(88);           break;
    case 0x5D:                                              // SHIFT-F10
        GestionFct(91);           break;


    case 0xB6:                                   //
    case 0xB7:                                   //  Windows 95 keyboard
    case 0xB8:                                   //
        break;


    default:
        if (Cfg->debug==1)
               PrintAt(78,0,"%02X",car2);
        break;
    }                                                   // switch (car2)

if ( (QuitKKC) & (KKCfg->confexit==1) )
    {
    if (WinMesg("Quit KKC","Do you really want to quit KKC ?",1)!=0)
        QuitKKC=0;
    }
}
while(!QuitKKC);
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

    strcpy(filename,KKFics->trash);
    Path2Abs(filename,"z.bat");

    fic=fopen(filename,"wt");
    fprintf(fic,
          "@REM *-------------------------------------------------*\n");
    fprintf(fic,
          "@REM * Batch file created by Ketchup Killers Commander *\n");
    fprintf(fic,
          "@REM * when command line is too long                   *\n");
    fprintf(fic,
          "@REM *-------------------------------------------------*\n");

    fprintf(fic,"@%s\n",suite+1);
    fclose(fic);

//    WinError("command line too high");

    strcpy(suite+1,filename);
    }

// if (KKCfg->KeyAfterShell==0)
suite[0]='#';

memcpy(SpecSortie,suite,256);
Fin();
}



/*--------------------------------------------------------------------*\
|-                        Fin d'execution                             -|
\*--------------------------------------------------------------------*/
void Fin(void)
{
int n;

if ( (KKCfg->scrrest==0) & (saveconfig) )
    {
    int x;

    x=(Cfg->TailleX-14)/2;

    Window(x,9,x+13,9,Cfg->col[28]);
    Cadre(x-1,8,x+14,10,0,Cfg->col[46],Cfg->col[47]);

    PrintAt(x+1,9,"Please  Wait");
    }

if (saveconfig)
    SaveCfg();

DriveSet(DFen->path);

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

if (KKCfg->scrrest) // & (saveconfig) )
    {
    TXTMode();                          // Retablit le mode texte normal

    for (n=0;n<9000;n++)
        Screen_Adr[n]=Screen_Buffer[n];

    }
    else
    {
    FILE *fic;

    fic=fopen(KKFics->ficscreen,"wb");
    if (fic!=NULL)
        {
        fwrite(&OldX,1,sizeof(OldX),fic);
        fwrite(&OldY,1,sizeof(OldY),fic);

        fwrite(&PosX,1,sizeof(PosX),fic);
        fwrite(&PosY,1,sizeof(PosY),fic);

        fwrite(Screen_Buffer,9000,1,fic);

        fclose(fic);
        }
    }

/*
if (KKCfg->_4dos==1)
    _4DOSShistdir(KKCfg->HistDir);
*/

GotoXY(0,PosY);

if (*SpecSortie==0)
    {
    #ifdef DEBUG
        printf("%s / RedBug (DEBUG MODE)\n",RBTitle);
    #else
        printf("%s / RedBug\n",RBTitle);
    #endif
    }

if (SpecMessy!=NULL)
    printf("\n%s\n",SpecMessy);

if ((SpecSortie[0]=='#') & (KKCfg->savekey==1))
    SpecSortie[0]='@';

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

for(n=0;n<NBWIN;n++)
    {
    if (Fenetre[n]->x!=0)
        {
        Fenetre[n]->x=Cfg->TailleX-40;
        Fenetre[n]->xl=Fenetre[n]->x+39;
        }
    }

PrintAt(0,0,"%-40s%*s",RBTitle,Cfg->TailleX-40,"RedBug");
ColLin( 0,0,40,Cfg->col[7]);
ColLin(40,0,(Cfg->TailleX)-40,Cfg->col[11]);

ColLin(0,Cfg->TailleY-2,Cfg->TailleX,Cfg->col[63]); //-- Command Line --

DFen->init=1;
DFen->Fen2->init=1;

DFen->ChangeLine=1;

MenuBar(4);
}

/*--------------------------------------------------------------------*\
|-                      Save Configuration File                       -|
\*--------------------------------------------------------------------*/
void SaveCfg(void)
{
int m,n,t,ns;
FILE *fic;
FENETRE *Fen;
long taille;

for (t=0;t<NBWIN;t++)
    KKCfg->FenTyp[t]=Fenetre[t]->FenTyp;

fic=fopen(KKFics->CfgFile,"wb");
if (fic==NULL)
    GestionFct(77);

fwrite((void*)Cfg,sizeof(struct config),1,fic);

fwrite((void*)KKCfg,sizeof(struct kkconfig),1,fic);

for(n=0;n<16;n++)
    {
    fwrite(&(KKCfg->V.Mask[n]->Ignore_Case),1,1,fic);
    fwrite(&(KKCfg->V.Mask[n]->Other_Col),1,1,fic);
    taille=strlen(KKCfg->V.Mask[n]->chaine);
    fwrite(&taille,2,1,fic);
    fwrite(KKCfg->V.Mask[n]->chaine,taille,1,fic);
    taille=strlen(KKCfg->V.Mask[n]->title);
    fwrite(&taille,2,1,fic);
    fwrite(KKCfg->V.Mask[n]->title,taille,1,fic);
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

fwrite(&NbrFunct,2,1,fic);
if (NbrFunct!=0)
    fwrite(FctStack,2,NbrFunct,fic);

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
struct PourMask **Mask;
char *viewhist;

fic=fopen(KKFics->CfgFile,"rb");
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

Mask=KKCfg->V.Mask;
viewhist=KKCfg->V.viewhist;


if (fread((void*)KKCfg,sizeof(struct kkconfig),1,fic)==0)
    {
    fclose(fic);
    return -1;
    }

KKCfg->V.Mask=Mask;
KKCfg->V.viewhist=viewhist;

if ( (KKCfg->overflow1!=0) | (KKCfg->crc!=0x69) )
    {
    fclose(fic);
    return -1;
    }

if (KKCfg->palafter!=1)
    LoadPal(Cfg->palette);

for(n=0;n<16;n++)
    {
    fread(&(KKCfg->V.Mask[n]->Ignore_Case),1,1,fic);
    fread(&(KKCfg->V.Mask[n]->Other_Col),1,1,fic);
    fread(&taille,2,1,fic);
    fread(KKCfg->V.Mask[n]->chaine,taille,1,fic);
    KKCfg->V.Mask[n]->chaine[taille]=0;
    fread(&taille,2,1,fic);
    fread(KKCfg->V.Mask[n]->title,taille,1,fic);
    KKCfg->V.Mask[n]->title[taille]=0;
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
                FicSelect(DFen,n,1);
        }

    fread(&m,4,1,fic);                                // Longueur du nom
    fread(nom,m,1,fic);                               // Fichier courant
    nom[m]=0;

    fread(&(DFen->scur),sizeof(short),1,fic); //Pos du fichier … l'ecran

    for (n=0;n<DFen->nbrfic;n++)
        if (!stricmp(nom,DFen->F[n]->name))
            DFen->pcur=n;
    }

fread(&NbrFunct,2,1,fic);
if (NbrFunct!=0)
    fread(FctStack,2,NbrFunct,fic);

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
        x2=Fen->xl;

        if (x2>=OldX)
            {
            Window(OldX,Fen->y,x2,Fen->yl,7);
            x2=OldX-1;
            }

        for(y=Fen->y;y<=Fen->yl;y++)
            {
            nscr=(y+Fen->y)*160+(Fen->x)*2;
            nbuf=(y-(Cfg->TailleY-3)+OldY-1)*OldX+(Fen->x);

            for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
            if (nbuf<0)
                AffCol(x+x1,y,7);
            else
                AffCol(x+x1,y,Screen_Buffer[nbuf*2+1]);
            }
        for(y=Fen->y;y<=Fen->yl;y++)
            {
            nscr=(y+Fen->y)*160+(Fen->x)*2;
            nbuf=(y-(Cfg->TailleY-3)+OldY-1)*OldX+(Fen->x);

            for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
            if (nbuf<0)
              AffChr(x+x1,y,32);
            else
              AffChr(x+x1,y,CnvASCII(0,Screen_Buffer[nbuf*2]));
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
//        FenTree(Fen);
        break;
    }

if ((Fen->nfen==1) & (Fen->yl!=Cfg->TailleY-3))
    {
    x2=Cfg->TailleX;

    if (x2>OldX)
        {
        Window(OldX,Fen->yl+1,x2-1,Cfg->TailleY-3,10*16+1);
        x2=OldX;
        }

    for(y=Fen->yl+1;y<=Cfg->TailleY-3;y++)
        {
        nscr=(y+Fen->y)*160;
        nbuf=(y-(Cfg->TailleY-3)+OldY-1)*OldX;

        for(x=0;x<x2;x++,nbuf++,nscr++)
        if (nbuf<0)
            AffCol(x,y,7);
        else
            AffCol(x,y,Screen_Buffer[nbuf*2+1]);
        }
    for(y=Fen->yl+1;y<=Cfg->TailleY-3;y++)
        {
        nscr=(y+Fen->y)*160;
        nbuf=(y-(Cfg->TailleY-3)+OldY-1)*OldX;

        for(x=0;x<x2;x++,nbuf++,nscr++)
        if (nbuf<0)
            AffChr(x,y,32);
        else
            AffChr(x,y,CnvASCII(0,Screen_Buffer[nbuf*2]));
        }
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
    for(y=DFen->y;y<=DFen->yl;y++)
        {
        nscr=(y+DFen->y)*160+x1*2;
        nbuf=(y-(Cfg->TailleY-3)+OldY-1)*OldX+x1;

        for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
            if (nbuf<0)
                AffCol(x+x1,y,7);
            else
                AffCol(x+x1,y,Screen_Buffer[nbuf*2+1]);
        }
    for(y=DFen->y;y<=DFen->yl;y++)
        {
        nscr=(y+DFen->y)*160+x1*2;
        nbuf=(y-(Cfg->TailleY-3)+OldY-1)*OldX+x1;

        for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
            if (nbuf<0)
                AffChr(x+x1,y,32);
            else
               AffChr(x+x1,y,CnvASCII(0,Screen_Buffer[nbuf*2]));
        }
    }
    else
    {
    chaine[8]=0;

    for (x=0;x<6;x++)
        {
        if (3+x*3<DFen->yl-3)
            {
            Cadre(x1, 1+x*3,x2, 3+x*3,2,Cfg->col[55],Cfg->col[56]);
            ColLin(x1+1,2+x*3,8,Cfg->col[17]);
            memcpy(chaine,KKCfg->Qmenu+x*8,8);
            PrintAt(x1+1,2+x*3 ,chaine);
            }
        }

    Window(x1,19,x2,DFen->yl-3,Cfg->col[17]);     // Efface le reste ---

    Cadre(x1,DFen->yl-2,x2,DFen->yl,2,Cfg->col[55],Cfg->col[56]);
    ColLin(x1+1,DFen->yl-1,8,Cfg->col[17]);

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
|- Initialisation du temps                                            -|
\*--------------------------------------------------------------------*/

Info=(struct RB_info*)GetMem(sizeof(struct RB_info));   // Starting time
Info->temps=clock();


/*--------------------------------------------------------------------*\
|-                     Initialisation de l'ecran                      -|
\*--------------------------------------------------------------------*/

Cfg=(struct config*)GetMem(sizeof(struct config));

Cfg->TailleX=(*(char*)(0x44A));
Cfg->TailleY=(*(char*)(0x484))+1;

InitScreen(0);

/*--------------------------------------------------------------------*\
|- Sauvegarde des donn‚es ‚crans                                      -|
\*--------------------------------------------------------------------*/

Screen_Buffer=(char*)GetMem(9000);                      // maximum 80x50

OldX=(*(char*)(0x44A));
OldY=(*(char*)(0x484))+1;

WhereXY(&PosX,&PosY);

for (n=0;n<9000;n++)
    Screen_Buffer[n]=Screen_Adr[n];

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

KKCfg->V.Mask=(struct PourMask**)GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    KKCfg->V.Mask[n]=(struct PourMask*)GetMem(sizeof(struct PourMask));


path=(char*)GetMem(256);

/*--------------------------------------------------------------------*\
|-                Lecture et verification des arguments               -|
\*--------------------------------------------------------------------*/

strcpy(path,*argv);

LC=strrchr(path,DEFSLASH);
if (LC!=NULL)
    *LC=0;

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
LoadErrorHandler();

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
SetDefaultKKPath(path);

Fics->help=(char*)GetMem(256);
strcpy(Fics->help,path);
Path2Abs(Fics->help,"kkc.hlp");

/*--------------------------------------------------------------------*\
|-                               Default                              -|
\*--------------------------------------------------------------------*/

ChangeType(4);

KKCfg->_4dos=_4DOSverif();

/*
    {
    memcpy(KKCfg->HistDir,_4DOSLhistdir(),256);
    }
    else */

memset(KKCfg->HistDir,0,256);


/*--------------------------------------------------------------------*\
|-                            Initialisation                          -|
\*--------------------------------------------------------------------*/

Fenetre[0]->x=0;
Fenetre[0]->nfen=0;
Fenetre[0]->FenTyp=0;
Fenetre[0]->Fen2=Fenetre[1];

Fenetre[1]->x=40;
Fenetre[1]->nfen=1;
Fenetre[1]->FenTyp=0;
Fenetre[1]->Fen2=Fenetre[0];

Fenetre[2]->x=40;
Fenetre[2]->nfen=2;
Fenetre[2]->FenTyp=0;                      // Fenˆtre ferm‚e par default
Fenetre[2]->Fen2=Fenetre[2];

Fenetre[3]->x=40;
Fenetre[3]->nfen=3;
Fenetre[3]->FenTyp=0;                      // Fenˆtre ferm‚e par default
Fenetre[3]->Fen2=Fenetre[3];


for (n=0;n<NBWIN;n++)
    {
    Fenetre[n]->y=1;
    Fenetre[n]->xl=Fenetre[n]->x+39;

    Fenetre[n]->order=17;
    Fenetre[n]->pcur=0;
    Fenetre[n]->scur=0;

    CalcSizeWin(Fenetre[n]);    // Ajuste les autres parametres --------
    }


/*--------------------------------------------------------------------*\
|-                Chargement du fichier config (s'il existe)          -|
\*--------------------------------------------------------------------*/

if (LoadCfg()==-1)
    {
    remove(KKFics->CfgFile);
    saveconfig=0;
    GestionFct(62);
    }

if ((KKCfg->KeyAfterShell) & (LC[4]=='0'))
    {
    PrintAt(0,OldY-1,"Press a key to continue to return in "RBTitle);
    ColLin(0,OldY-1,OldX,Cfg->col[4]);
    Wait(0,0);
    }


/*--------------------------------------------------------------------*\
|- Sauvegarde de l'ecran                                              -|
\*--------------------------------------------------------------------*/

if (KKCfg->scrrest==0)
    {
    FILE *fic;

    fic=fopen(KKFics->ficscreen,"rb");
    if (fic==NULL)
        WinError("It's bizarre ?!");

    fread(&OldX,1,sizeof(OldX),fic);
    fread(&OldY,1,sizeof(OldY),fic);

    fread(&PosX,1,sizeof(PosX),fic);
    fread(&PosY,1,sizeof(PosY),fic);

    fread(Screen_Buffer,1,9000,fic);

    fclose(fic);
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

if ( ( (KKCfg->currentdir==1) & (LC[4]!='0') ) |
     ( (KKCfg->alcddir==1) & (LC[4]=='0') ) )
    {
    DFen=Fenetre[(KKCfg->FenAct)&1];

    if ( (strcmp(Fics->LastDir,Fenetre[0]->path)!=0) &
         (strcmp(Fics->LastDir,Fenetre[1]->path)!=0) )
        CommandLine("#CD %s",Fics->LastDir);
    }

Cfg->reinit=0;
UseCfg();                      // Emploi les parametres de configuration

InitScreen(Cfg->display);

if (KKCfg->verifhist==1)
    VerifHistDir();            // Verifie l'history pour les repertoires


/*--------------------------------------------------------------------*\
|- Sauvegarde des touches                                             -|
\*--------------------------------------------------------------------*/

if (KKCfg->savekey)
    {
    static char file[256];
    char *buf;
    unsigned short *adrint;
    int m;
    FILE *fic;

    adrint=(unsigned short*)(0x60*4);
    buf=(char*)(adrint[0]+adrint[1]*0x10);

    adrint=(unsigned short*)(buf+5);
    buf=(char*)(adrint[0]+adrint[1]*0x10);

    strcpy(file,KKFics->trash);
    Path2Abs(file,"key.rb");

    fic=fopen(file,"ab");

    for(m=0;m<20*16;m+=16)
        {
        for(n=0;n<16;n++)
            if (buf[n+m]<0x80)
                fputc(buf[n+m]+140,fic);
        }

    fprintf(fic,"\n\n");

    fclose(fic);
    }

Gestion();

/*--------------------------------------------------------------------*\
|-                                  FIN                               -|
\*--------------------------------------------------------------------*/

memset(SpecSortie,0,256);

Fin();
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
SwapLong(&(Fenetre[a]->y2),&(Fenetre[b]->y2));
SwapLong(&(Fenetre[a]->yl2),&(Fenetre[b]->yl2));
Fenetre[a]->init=1;
Fenetre[b]->init=1;

tfen=Fenetre[a];
Fenetre[a]=Fenetre[b];
Fenetre[b]=tfen;

Fenetre[a]->nfen=(char)a;
Fenetre[b]->nfen=(char)b;

if (KKCfg->FenAct==a)
    KKCfg->FenAct=b;
    else
    if (KKCfg->FenAct==b)
        KKCfg->FenAct=a;
}


void SaveRawPage(void)
{
FILE *fic;
int n;
static char CadreLength=70;
static char Dir[70];
static int DirLength=70;
char nom[256];

struct Tmt T[] =
    { { 2,3,1, Dir, &DirLength},
      {10,5,5,"    View     ",NULL},
      {30,5,5,"  View/Save  ",NULL},
      {50,5,5,"    Save     ",NULL},
      { 2,2,0,"File to use: ",NULL},
      { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,5,74,12,"Background"};

strcpy(Dir,DFen->path);
Path2Abs(Dir,"KKSCREEN.RAW");

n=WinTraite(T,6,&F,1);

if (n==-1) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

if (n==1)
    strcpy(Dir,KKFics->temp);

strcpy(nom,Dir);
strcpy(Dir,DFen->path);
Path2Abs(Dir,nom);

if ((n==1) | (n==2) | (n==3))
    {
    fic=fopen(Dir,"wb");
    if (fic==NULL)
        {
        WinError("File couldn't be created");
        return;
        }
    fwrite(Screen_Buffer,OldX*OldY*2,1,fic);
    fclose(fic);
    }
if ((n==1) | (n==2))
    View(&(KKCfg->V),Dir,5);
}
