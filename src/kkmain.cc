/*--------------------------------------------------------------------*\
|- Programme principal												  -|
\*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
//#include <fcntl.h>
#include <signal.h> 							  // For handling signal

// #include <conio.h>

#include "kk.h"

#include "idf.h"
#include "win.h"

#include "language.h"

#if defined(__WC32__) | defined(DJGPP)
#include <bios.h>
#include <io.h> 	//--- Pour unlink ----------------------------------
#endif


//--- KKT --------------------------------------------------------------
#include "select.h"
#include "editfile.h"
#include "viewfile.h"
#include "unselect.h"
#include "seluns.h"
#include "loadsave.h"
#include "dircmp.h"
#include "chgdir.h"
#include "quit.h"

#define FNDCOL Cfg->col[0]


/*--------------------------------------------------------------------*\
|-	Declaration interne 											  -|
\*--------------------------------------------------------------------*/

void RunDFenMacro(char *path,char *file);

int TestPreMacro(FENETRE *Fen,char *tnom);

void WinSelect(FENETRE *Fen);
void SaveSel(FENETRE *F1);
void LoadSel(int n);

FUNCTION GestionBar(int i);

void SelectPlus(void);
void SelectPlusMoins(void);
void SelectMoins(void);
void CreateKKD(void);
void CreateDirectory(void);
void WinCD(void);
void HistDir(void);
void PathDir(void);
void SwitchScreen(void);
void ChangeType(char n);
void ChangeDrive(char *);

void EditNewFile(void);
void ViewNewFile(void);

void EditFile(char *s);
void ViewFile(char *s);

void SwapWin(long a,long b);
void SwapLong(long *a,long *b);
void HistCom(void);
void AffLonger(void);
void About(void);

void Information(char *s);

void RBSetup(void);

void SaveRawPage(void);
void ViewBKGPage(void);

void QuickSearch(int key,BYTE *c,BYTE *c2);

/*--------------------------------------------------------------------*\
|- Gestion des pannels												  -|
\*--------------------------------------------------------------------*/
void CreateNewFen(FENETRE *Fen,int n);
void CloseNewFen(FENETRE *Fen);
void InitDispNewFen(FENETRE *Fen);
void DispNewFen(FENETRE *Fen);

/*--------------------------------------------------------------------*\
|-	Declaration des variables										  -|
\*--------------------------------------------------------------------*/

int SPosX,SPosY;		//-- Position du curseur dans la fenetre shell -

long PosX,PosY;
char OldScr[MaxZ];
char OldFont[256*16];

sig_atomic_t signal_count;

char *SpecMessy=NULL;
char SpecSortie[256];
char saveconfig=1;

char Select_Chaine[32]="*.*";

char *Screen_Buffer;

#ifndef LINUX
char *ShellAdr=(char*)0xBB000;
#else
char ShellAdr[256]="";
#endif


FENETRE *Fenetre[NBWIN];

FENETRE *DFen;

extern int IOver;
extern int IOerr;

struct kkconfig *KKCfg;
struct kkfichier *KKFics;

char QuitKKC=0;

static char oldhist[256];	//--- Ancienne ligne de commande -----------
static int oldhistnum=0;

struct config *OldCfg,*NewCfg;


struct RB_info *Info;


void KKInit(void)
{
Info=(struct RB_info*)GetMem(sizeof(struct RB_info));	// Starting time

Info->temps=GetClock();
Info->defform=1;
Info->macro=0;
}


/*--------------------------------------------------------------------*\
|- Gestion macros													  -|
\*--------------------------------------------------------------------*/

FUNCTION FctStack[128];
short NbrFunct=0;


/*--------------------------------------------------------------------*\
|- Init Function Stack												  -|
\*--------------------------------------------------------------------*/
void InitFctStack(void)
{
NbrFunct=0;
}

/*--------------------------------------------------------------------*\
|- Put a Function LIFO												  -|
\*--------------------------------------------------------------------*/
void PutLIFOFct(FUNCTION fct)
{
if (fct!=0)
	{
	if (NbrFunct>128) return;
	FctStack[NbrFunct]=fct;
	NbrFunct++;
	}
}

/*--------------------------------------------------------------------*\
|- Get a Function LIFO												  -|
\*--------------------------------------------------------------------*/
FUNCTION GetLIFOFct(void)
{
if (NbrFunct==0) return 0;
NbrFunct--;
return FctStack[NbrFunct];
}


FUNCTION GetKeyFct(ushort c)
{
int i;
FUNCTION fct=0;

for(i=0;KKCfg->keydef[i].scan!=0;i++)
	{
	if (KKCfg->keydef[i].scan==c)
		fct=KKCfg->keydef[i].fct;
	}
return fct;
}

ushort GetKeyScan(FUNCTION c)
{
int i;
ushort fct=0;

for(i=0;KKCfg->keydef[i].fct!=0;i++)
	{
	if (KKCfg->keydef[i].fct==c)
		fct=KKCfg->keydef[i].scan;
	}
return fct;
}


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


void DeplPCurFen(FENETRE *Fen,int depl)
{
if (Fen->pcur+depl<0)
	depl=-Fen->pcur;

if (Fen->pcur+depl>=Fen->nbrfic)
	depl=Fen->nbrfic-Fen->pcur-1;

Fen->pcur+=depl;
Fen->scur+=depl;
}


void HelpOnError(void)
{
static char buf[3][81];
int i,j;

for(j=0;j<3;j++)
	{
	for(i=0;i<80;i++)
		buf[j][i]=Screen_Buffer[(i+(PosY-j-1)*MaxX)*2];
	buf[j][80]=0;
	}

if	( (!strncmp(buf[2],"Stub exec failed:",17)) &
	  (!strncmp(buf[1],"dos4gw.exe",10)) &
	  (!strncmp(buf[0],"No such file or directory",25)) )
		HelpTopic(502);
}

/*--------------------------------------------------------------------*\
|- Gestion des selections											  -|
\*--------------------------------------------------------------------*/
void WinSelect(FENETRE *Fen)
{
FILE *fic;
int i,n;
char nom[256];

MacAlloc(10,80);

if (!(Info->macro))
	{
	strcpy(_sbuf[10],Fen->path);
	Path2Abs(_sbuf[10],"KKSELECT.TMP");
	}


n=MWinTraite(loadsave_kkt);

if (n==-1)
	{
	MacFree(10);
	return; 											//--- ESCape ---
	}

strcpy(nom,Fen->path);
Path2Abs(nom,_sbuf[10]);

if (n==1)
	{
	fic=fopen(nom,"rt");
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
				FicSelect(Fen,i,1); 						   // Select
		}
	fclose(fic);
	}

if (n==2)
	{
	fic=fopen(nom,"wt");
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

MacFree(10);
}


#ifndef NOMOUSE
void MouseShortCut(void)
{
static BARMENU bar[10];
int retour;
MENU menu;
int nbr,max;
int n,x,y;

if (DFen->F[DFen->pcur]->name[0]=='*')
	{
	nbr=1;
	bar[0].Titre="Run Function";   bar[0].fct=84;
	}
else
if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
	{
	nbr=6;
	bar[0].Titre="Enter";   bar[0].fct=84;
	bar[1].Titre=NULL;
	bar[2].Titre="Copy to ...";   bar[2].fct=10;
	bar[3].Titre="Move to ...";   bar[3].fct=11;
	bar[4].Titre="Rename"; bar[4].fct=44;
	bar[5].Titre="Delete"; bar[5].fct=13;
	}

else
	{
	nbr=9;
	bar[0].Titre="Open";   bar[0].fct=84;
	bar[1].Titre=NULL;
	bar[2].Titre="Edit";   bar[2].fct=9;
	bar[3].Titre="View";   bar[3].fct=7;
	bar[4].Titre=NULL;
	bar[5].Titre="Copy to ...";   bar[5].fct=10;
	bar[6].Titre="Move to ...";   bar[6].fct=11;
	bar[7].Titre="Rename"; bar[7].fct=44;
	bar[8].Titre="Delete"; bar[8].fct=13;
	}


//--- Commun … tous ---

max=0;
for(n=0;n<nbr;n++)
	if (bar[n].Titre!=NULL)
		if (strlen(bar[n].Titre)>max)
			max=strlen(bar[n].Titre);

x=MousePosX();
y=MousePosY();

if (x+max+2>=Cfg->TailleX)
	x=Cfg->TailleX-max-2;

if (y+nbr+2>=Cfg->TailleY)
	y=Cfg->TailleY-nbr-2;

menu.x=x+1;
menu.y=y;
menu.attr=8;
menu.cur=0;

retour=PannelMenu(bar,nbr,&menu);

if (retour==2)
	GestionFct((FUNCTION)(bar[menu.cur].fct));
}
#endif


/*--------------------------------------------------------------------*\
|- Change le type d'affichage                                         -|
\*--------------------------------------------------------------------*/
void SwitchScreen(void)
{
int n,nbr;
static BARMENU bar[32];
int retour;
MENU menu;
int (*old_system)(int,char *);

nbr=givesyst(-1);


old_system=disp_system;

for(n=0;n<nbr;n++)
	{
	givesyst(n);
	bar[n].Titre=(char*)GetMem(80);
	bar[n].fct=disp_system(1,bar[n].Titre)+1;
	bar[n].Help=0;
	}

disp_system=old_system;

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
		Cfg->display=(char)(bar[menu.cur].fct-1);
		if (InitScreen(Cfg->display)) break;
		}
	}
while (retour!=0);

if (retour!=0)
	UseCfg();

for(n=0;n<nbr;n++)
	LibMem(bar[n].Titre);
}

void RBSetup(void)
{
char buffer[256];
char bartxt[81];
int car;

/*
if (Wait(0,0)!='R') return;
if (Wait(0,0)!='B') return;
*/

KKWin Win;

PrintAt(0,0,"%-40s%*s","RedBug's own setup",Cfg->TailleX-40,"Hi Me !");
ColLin( 0,0,40,Cfg->col[7]);
ColLin(40,0,(Cfg->TailleX)-40,Cfg->col[11]);

strcpy(bartxt,
		" ----  ----  ---- SpTest Color sKKR  Font  ----  ----  Quit ");
	   //0	   6	12	  18	 24   30	36	  42	48	  54
do
{
memcpy(bartxt+ 6,KKCfg->savekey ? "Key On" : "KeyOff",6);   // F2
memcpy(bartxt+12,Cfg->debug 	? "Debug " : "No deb",6);   // F3

Bar(bartxt);

car=Wait(0,0);

switch(car)
	{
	case KEY_F(2): //--- F2 ------------------------------------------------
		KKCfg->savekey^=1;
		break;
	case KEY_F(3): //--- F3 ------------------------------------------------
		Cfg->debug^=1;
		break;
	case KEY_F(4): //--- F4 ------------------------------------------------
		SpeedTest();
		break;
	case KEY_F(5): //--- F5 ------------------------------------------------
		{
		FILE *fic;
		int n;

		strcpy(buffer,DFen->path);
		Path2Abs(buffer,"color.tmp");

		fic=fopen(buffer,"wt");

		fprintf(fic,"char pal[]=\"\n");
		for(n=0;n<48;n++)
			fprintf(fic,"\\x%02X",Cfg->palette[n]);
		fprintf(fic,"\"\n");

		fprintf(fic,"char col[]=\"\n");
		for(n=0;n<64;n++)
			fprintf(fic,"\\x%02X",Cfg->col[n]);
		fprintf(fic,"\"\n");

		fclose(fic);
		}
		break;
	case KEY_F(6): //--- F6 ------------------------------------------------
		GestionFct(106);
		break;
	case KEY_F(7): //--- F7 ------------------------------------------------
		#ifndef NOFONT
			{
			FILE *fic;
			int x,y;

			char *font;

			font=(char*)GetMem(4096);
			Font2Buf(font);

			strcpy(buffer,DFen->path);
			Path2Abs(buffer,"font.tmp");

			fic=fopen(buffer,"wt");

			fprintf(fic,"char font[]=\"\n");
			for(x=0;x<256;x++)
				{
				for(y=0;y<16;y++)
					fprintf(fic,"\\x%02X",font[y+x*16]);
				fprintf(fic,"\"\n\"");
				}

			fprintf(fic,"\"\n");

			fclose(fic);

			LibMem(font);
			}
		#endif
		break;

	case KEY_F(10): //--- F10 -----------------------------------------------
		car=27;
		break;
	}
}
while(LO(car)!=27);
}


void Search4KKR(void)
{
char buffer[256];

strcpy(buffer,Fics->path);
Path2Abs(buffer,"kksetup.exe /AREA");
KKCfg->scrrest=0;
CommandLine("#%s",buffer);
}


int sortfct(const void *a,const void *b)
{
BARMENU *a1,*b1;

a1=(BARMENU*)a;
b1=(BARMENU*)b;

if (((a1->fct)&31)!=((b1->fct)&31))
	return (((a1->fct)&31)-((b1->fct)&31));

if (a1->Titre[0]==196)
	return -1;

if (b1->Titre[0]==196)
	return 1;

return strcmp(a1->Titre,b1->Titre);
}

void PlayerFromExt(void)
{
int retour,i;
char buffer[256];
BARMENU *bar;
MENU menu;
int max1,max2;
char enter;

max1=0;
max2=0;

for(i=0;i<nbrkey;i++)
	{
	if (strlen(K[i].ext)>max1)
		max1=strlen(K[i].ext);

	if (strlen(K[i].format)>max2)
		max2=strlen(K[i].format);
	}

bar=(BARMENU*)GetMem(sizeof(BARMENU)*nbrkey);


for(i=0;i<nbrkey;i++)
	{
	if (K[i].ext[0]!='*')
		{
		sprintf(buffer,"%-*s - %*s",max1,K[i].ext,max2,K[i].format);
		bar[i].fct=(K[i].type)+(K[i].numero)*32;
		}
		else
		{
		memset(buffer,196,max1+max2+3);
		buffer[max1+max2+3]=0;
		memcpy(buffer+3,K[i].format,strlen(K[i].format));
		bar[i].fct=(K[i].type);
		}

	bar[i].Titre=(char*)GetMem(strlen(buffer)+1);
	strcpy(bar[i].Titre,buffer);
	}

qsort((void*)bar,nbrkey,sizeof(BARMENU),sortfct);

menu.x=2;
menu.y=2;
menu.attr=2;
menu.cur=1; //--- On passe la premiere barre ---------------------------

retour=PannelMenu(bar,nbrkey,&menu);

if (retour!=0)
	{
	enter=(retour==2);


	retour=(bar[menu.cur].fct)/32;

	if (retour>0)
		{
		i=FicIdf(buffer,"",retour,1);

		switch(i)
			{
			case 1: 						 // Pas de fichier IDFEXT.RB
				CommandLine("@ ERROR WITH FICIDF @");
				break;
			case 0:
				if (enter)
					CommandLine(buffer);
					else
					CommandLine(buffer+1);
				break;
			}
		}
	}

for(i=0;i<nbrkey;i++)
	LibMem(bar[i].Titre);

LibMem(bar);
}


/*--------------------------------------------------------------------*\
|-	Programme de setup												  -|
\*--------------------------------------------------------------------*/

void Abort_Handler(int sig_no)
{
signal_count+=sig_no;		//--- ne sert … rien -----------------------

exit(1);

}


void Signal_Handler(int sig_no)
{
signal_count+=sig_no;		//--- ne sert … rien -----------------------

SpecMessy="You have pressed Control Break :(";

memset(SpecSortie,0,256);
saveconfig=0;
Fin();
}

/*--------------------------------------------------------------------*\
|-	Gestion de toutes les fonctions 								  -|
|----------------------------------------------------------------------|
|-	0: ?															  -|
|-	1: Help 														  -|
|-	2: Invert Selection 											  -|
|-	3: Select Group of file 										  -|
|-	4: Unselect Group of file										  -|
|-	5: Search File													  -|
|-	6: Create a KKD file											  -|
|-	7: View file													  -|
|-	8: Quick view file (internal viewer)							  -|
|-	9: Edit file													  -|
|- 10: Copy 														  -|
|- 11: Move 														  -|
|- 12: Create Directory 											  -|
|- 13: Delete selection 											  -|
|- 14: Close left window											  -|
|- 15: Close right window											  -|
|- 16: Select current file											  -|
|- 17: Change palette												  -|
|- 18: About														  -|
|- 19: Select temporary file										  -|
|- 20: Quit KKC 													  -|
|- 21: Fenetre DIZ													  -|
|- 22: Sort by name 												  -|
|- 23: Sort by extension											  -|
|- 24: Sort by date 												  -|
|- 25: Sort by size 												  -|
|- 26: Sort by unsort ;)											  -|
|- 27: Reload Directory 											  -|
|- 28: ASCII Table													  -|
|- 29: Win CD														  -|
|- 30: Put file on command line 									  -|
|- 31: Appel du programme de configuration							  -|
|- 32: Switch les fontes											  -|
|- 33: Switch special sort											  -|
|- 34: Efface la trash												  -|
|- 35: Affiche les infos											  -|
|- 36: Ligne suivante												  -|
|- 37: Ligne precedente 											  -|
|- 38: Fenˆtre information											  -|
|- 39: Changement d'attribut                                          -|
|- 40: Fenetre history des directories								  -|
|- 41: Va dans le r‚p‚rtoire pere									  -|
|- 42: Va dans le r‚p‚rtoire trash									  -|
|- 43: Active la fenetre principal (Cfg->FenAct)					  -|
|- 44: Rename Window												  -|
|- 45: Internal editor												  -|
|- 46: Selection du fichier en tant que repertoire courant			  -|
|- 47: Switch le mode ecran (nombre de lignes)						  -|
|- 48: Switch le type d'ecran (watcom, norton, ...)                   -|
|- 49: Change le drive de la fenetre 0								  -|
|- 50: Change le drive de la fenetre 1								  -|
|- 51: Va dans le repertoire d'o— l'on a lanc‚ KK                     -|
|- 52: 10 lignes avant												  -|
|- 53: 10 lignes apres												  -|
|- 54: Premiere ligne												  -|
|- 55: Derniere ligne												  -|
|- 56: Va dans la premiere fenˆtre									  -|
|- 57: Va dans la deuxieme fenˆtre									  -|
|- 58: Change la fenˆtre avec la path de l'autre fenˆtre              -|
|- 59: Edit New File												  -|
|- 60: Switch Display Mode											  -|
|- 61: Mode serveur 												  -|
|- 62: Appelle KKSETUP.EXE											  -|
|- 63: Info on disk 												  -|
|- 64: Appelle l'application lie au fichier sans les lier             -|
|- 65: Menu Login													  -|
|- 66: Screen Setup 												  -|
|- 67: Rafraichit l'ecran                                             -|
|- 68: Echange la fenetre 0 avec la 1								  -|
|- 69: History of command											  -|
|- 70: Get Previous Command Line									  -|
|- 71: Fenetre affichage de file_id.diz 							  -|
|- 72: Change le drive de la fenetre par default					  -|
|- 73: Execute une commande redirige vers le fichier temporaire 	  -|
|- 74: Affiche le fichier temporaire								  -|
|- 75: Directory tree												  -|
|- 76: Screen Saver 												  -|
|- 77: Support prot‚ge, lance l'installation                          -|
|- 78: Sauve ou charge une selection								  -|
|- 79: Appelle le menu F2											  -|
|- 80: Help on Error												  -|
|- 81: Switch la taille horizontale 								  -|
|- 82: Change les couleurs											  -|
|- 83: Fenetre changement de dir d'apres PATH                         -|
|- 84: ENTER														  -|
|- 85: R‚solution de s‚curit‚										  -|
|- 86: Eject le support 											  -|
|- 87: Pannel Menu													  -|
|- 88: Previent le centre de contr“le que l'on veut quitter KKC       -|
|- 89: Va dans l'autre fenˆtre                                        -|
|- 90: Sauvegarde et affiche le fond avec le viewer ansi			  -|
|- 91: Appelle le mode console										  -|
|- 92: Selection ou deselectionne des fichiers						  -|
|- 93: Get Next Command Line										  -|
|- 94: Recherche le premier fichier commencant par gnagna			  -|
|- 95: Switch l'‚tat de la barre de fonction et la barre IDF          -|
|- 96: MessageBox(sbuf0,sbuf1,cbuf0)								  -|
|- 97: Copie les options ecrans courrantes vers le mode DOS 		  -|
|- 98: Appelle le File Setup										  -|
|- 99: History viewer												  -|
|-100: Compare directory											  -|
|-101: Setup pour la gestion des noms longs 						  -|
|-102: Inverse l'‚tat de command line                                 -|
|-103: Accede a un fichier 'pour link macro'                          -|
|-104: Display ShortCut Mouse Menu									  -|
|-105: Ferme les deux fenetres										  -|
|-106: Recherche des nouvelles applications 						  -|
|-107: Lance un player selon un type								  -|
|-108: Extended copie												  -|
|-109: View Background												  -|
|-110: View every file (internal or external)						  -|
|-111: Compute the size of the current directory					  -|
|-112: Wired Compo													  -|
|-113: Wired compo configuration									  -|
\*--------------------------------------------------------------------*/

#define CURRENTNAME DFen->F[DFen->pcur]->name

void GestionFct(FUNCTION fct)
{
FENETRE *FenOld;
static char buffer[256],buf2[256];
char *ext;
int i,j;
FILE *fic;

if (KKCfg->key!=0)
	PutLIFOFct(KKCfg->key);

KKCfg->key=fct;

if ( ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)!=RB_SUBDIR) &
		 (CURRENTNAME[0]!='*') )
	{
	switch(KKCfg->key)
		{
		case 84:	//--- Enter
		case  7:	//--- View file
		case  9:	//--- Edit file
		case  8:	//--- Quick view file
			strcpy(KKCfg->FileName,CURRENTNAME);
			AccessFile(DFen->pcur);
			break;
		}
	}

//printf("Traite Commandline after dos %d FIN)\n\n",fct); exit(1);
//
InitDispNewFen(DFen);
//DispNewFen(DFen);
                                                                                              

KKCfg->key=GetLIFOFct();


switch(fct)
	{
	case 0:
		break;
	case 1:
//		  PrintAt(0,0,"Info->macro: %d",Info->macro); Wait(0,0);
		if (Info->macro)
			HelpTopic(_lbuf[0]);
			else
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
		Search(DFen);
		break;
	case 6:
		CreateKKD();
		break;
	case 7:
		ViewFile(CURRENTNAME);
		break;
	case 8:
		switch(DFen->system)
			{
			case 0:
				strcpy(buffer,DFen->path);
				Path2Abs(buffer,CURRENTNAME);
				View(&(KKCfg->V),buffer,0);
				break;
			}
		break;
	case 9:
		EditFile(CURRENTNAME);
		break;
	case 10:													 // Copy
		Copie(DFen,DFen->Fen2->path);	// Fen[2] non visible
		if (KKCfg->autoreload==1)
			{
			DFen=Fenetre[0];
			GestionFct(27);
			DFen=Fenetre[1];
			GestionFct(27);
			}
		GestionFct(43);   // Active la fenetre principal (KKCfg->FenAct)
		break;
	case 11:													 // Move
		Move(DFen,DFen->Fen2->path);	// Fen[2] non visible
		if (KKCfg->autoreload==1)
			{
			DFen=Fenetre[0];
			GestionFct(27);
			DFen=Fenetre[1];
			GestionFct(27);
			}
		GestionFct(43);   // Active la fenetre principal (KKCfg->FenAct)
		break;
	case 12:										 // Create Directory
		CreateDirectory();
		break;
	case 13:										 // Delete selection
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
	case 14:										// Close left window
		DFen=Fenetre[0];

		if (DFen->FenTyp==2)
			CloseNewFen(DFen);
			else
			CreateNewFen(DFen,2);

		break;
	case 15:									   // Close right window
		DFen=Fenetre[1];

		if (DFen->FenTyp==2)
			CloseNewFen(DFen);
			else
			CreateNewFen(DFen,2);

		break;
	case 16:									  // Select current file
		FicSelect(DFen,DFen->pcur,2);

		if (!strcmp(CURRENTNAME,"."))
			for (i=0;i<DFen->nbrfic;i++)
				if (!WildCmp(DFen->F[i]->name,Select_Chaine))
					FicSelect(DFen,i,1);

		if (KKCfg->insdown==1)
			GestionFct(36);  // Descent d'une ligne --------------------
		break;
	case 17:										   // Change palette
		strcpy(buffer,Fics->path);
		Path2Abs(buffer,"kksetup.exe -PALETTE");
		KKCfg->scrrest=0;
		CommandLine("#%s",buffer);
		break;
	case 18:													// About
		About();

		break;
	case 19:									// Select temporary file
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
					  FicSelect(DFen,i,1);				  // Select file
		break;
	case 20:												 // Quit KKC
		break;
	case 21:											  // Fenetre DIZ
		DFen=DFen->Fen2;

		if (DFen->FenTyp==1)
			CloseNewFen(DFen);
			else
			CreateNewFen(DFen,1);

		DFen=DFen->Fen2;
		break;
	case 22:											 // Sort by name
		DFen->order&=16;
		DFen->order|=1;
		SaveSel(DFen);
		SortFic(DFen);
		LoadSel(0);
		MenuBar(4);
		break;
	case 23:										// Sort by extension
		DFen->order&=16;
		DFen->order|=2;
		SaveSel(DFen);
		SortFic(DFen);
		LoadSel(0);
		MenuBar(4);
		break;
	case 24:											 // Sort by date
		DFen->order&=16;
		DFen->order|=3;
		SaveSel(DFen);
		SortFic(DFen);
		LoadSel(0);
		MenuBar(4);
		break;
	case 25:											 // Sort by size
		DFen->order&=16;
		DFen->order|=4;
		SaveSel(DFen);
		SortFic(DFen);
		LoadSel(0);
		MenuBar(4);
		break;
	case 26:										// Sort by unsort ;)
		DFen->order=0;
		GestionFct(27); 		//--- Reload ---------------------------
		MenuBar(4);
		break;
	case 27:											// Reload CTRL-R
		SaveSel(DFen);
		CommandLine("#cd .");
		LoadSel(0);
		break;
	case 28:											  // Table ASCII
		ASCIItable();
		break;
	case 29:													// WINCD
		WinCD();
		break;
	case 30:								 // Put file on command line
		CommandLine("%s ",CURRENTNAME);
		break;
	case 31:											// Configuration
		Setup();
		break;
	case 32:										// Switch les fontes
		Cfg->font^=1;
		GestionFct(67);
		break;
	case 33:									  // Switch Special Sort
		DFen->order^=16;
		SaveSel(DFen);
		SortFic(DFen);
		LoadSel(0);
		MenuBar(4);
		break;
	case 34:										 // Nettoie la trash
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
		if (DFen->pcur<DFen->nbrfic-1)
			{
			DFen->scur++;
			DFen->pcur++;
			}
		break;
	case 37:
		if (DFen->pcur>0)
			{
			DFen->scur--;
			DFen->pcur--;
			}
		break;
	case 38:
		DFen=DFen->Fen2;

		if (DFen->FenTyp==3)
			CloseNewFen(DFen);
			else
			CreateNewFen(DFen,3);

		DFen=DFen->Fen2;
		break;
	case 39:									 // Change les attributs
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
		DFen->ChangeLine=1; 						   // Affichage Path
		break;
	case 44:							   // Fenetre pour renomation ;)
		WinRename2(DFen);
		if (KKCfg->autoreload==1)
			{
			DFen=Fenetre[0];
			GestionFct(27);
			DFen=Fenetre[1];
			GestionFct(27);
			}
		GestionFct(43);   // Active la fenetre principal (KKCfg->FenAct)
		break;
	case 45:										  // Internal editor
		switch(DFen->system)
			{
			case 0:
				strcpy(buffer,DFen->path);
				Path2Abs(buffer,CURRENTNAME);
				TxtEdit(&(KKCfg->E),buffer);
				break;
			}
		break;
	case 46:	  // Selection du fichier en tant que repertoire courant
		if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
				CommandLine("#cd %s",CURRENTNAME);
		break;
	case 47:				  // Switch le mode ecran (nombre de lignes)
		ChangeTaille(0);
		break;
	case 48:			  //Switch le type d'ecran (watcom, norton, ...)
		ChangeType(0);
		AfficheTout();
		break;
	case 49:						  // Change le drive de la fenetre 1
		FenOld=DFen;
		DFen=Fenetre[0];

		ChangeDrive(FenOld->path);
		DFen->init=1;
		DFen=FenOld;

		DFen->ChangeLine=1; 						   // Affichage Path
		break;
	case 50:						  // Change le drive de la fenetre 2
		FenOld=DFen;
		DFen=Fenetre[1];

		ChangeDrive(FenOld->path);
		DFen->init=1;
		DFen=FenOld;

		DFen->ChangeLine=1; 						   // Affichage Path
		break;
	case 51:			   // Va dans le repertoire d'o— l'on a lanc‚ KK
		CommandLine("#CD %s",Fics->LastDir);
		break;
	case 52:
		DeplPCurFen(DFen,-10);
		break;
	case 53:
		DeplPCurFen(DFen,10);
		break;
	case 54:
		DFen->scur=0;
		DFen->pcur=0;
		break;
	case 55:
		DFen->scur=DFen->nbrfic-1;
		DFen->pcur=DFen->nbrfic-1;
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
			DFen->ChangeLine=1; 					   // Affichage Path
			}
		break;
	case 57:
		if (DFen->Fen2->FenTyp==3)
			DFen->InfoPos+=5;
			else
			{
			KKCfg->FenAct=1;
			DFen=Fenetre[KKCfg->FenAct];
			DFen->ChangeLine=1; 					   // Affichage Path
			}
		break;
	case 58:		// Change la fenˆtre avec la path de l'autre fenˆtre
		CommandLine("#CD %s",DFen->Fen2->path);
		break;
	case 59:
		EditNewFile();
		break;
	case 60:
		SwitchScreen();
		break;
	case 61:
#ifndef NOCOM
		ServerMode();
#endif
		break;
	case 62:												  // KKSETUP
		strcpy(buffer,Fics->path);
		Path2Abs(buffer,"kksetup.exe");
		KKCfg->scrrest=0;
		CommandLine("#%s",buffer);
		break;
	case 63:										// Fenetre info disk
		DFen=DFen->Fen2;

		if (DFen->FenTyp==4)
			CloseNewFen(DFen);
			else
			CreateNewFen(DFen,4);

		DFen=DFen->Fen2;
		break;
	case 64: //---	Appelle l'application lie au fichier sans les lier -
		strcpy(buffer,DFen->path);
		Path2Abs(buffer,CURRENTNAME);
		i=NameIDF(buffer);

		i=FicIdf(buffer,"",i,1);

		switch(i)
			{
			case 0:
				CommandLine(buffer);
				CommandLine("\n");
				break;
			case 1: 						 // Pas de fichier IDFEXT.RB
				CommandLine("@ ERROR WITH FICIDF @");
				break;
			case 2:
				CommandLine(buffer);
				CommandLine("%s",CURRENTNAME);
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
		Cfg->reinit=0;
		ChangeTaille(Cfg->TailleY); 			   // Rafraichit l'ecran
		break;
	case 68:
		SwapWin(0,1);
		break;
	case 69:
		HistCom();
		break;
	case 70:
		if (oldhist[0]==32)
			strcpy(oldhist,CLstr);
		History2Line(GetPrevHistCom(oldhist),buffer);
		CommandLine("0\r%s",buffer);
		break;
	case 71:						 // Fenetre affichage de file_id.diz
		DFen=DFen->Fen2;

		if (DFen->FenTyp==5)
			CloseNewFen(DFen);
			else
			CreateNewFen(DFen,5);

		DFen=DFen->Fen2;
		break;
	case 72:				// Change le drive de la fenetre par default
		ChangeDrive(DFen->path);
		DFen->ChangeLine=1; 						   // Affichage Path
		break;
	case 73:
		ExecCom();
		break;
	case 74:
		View(&(KKCfg->V),KKFics->temp,0);
		break;
	case 75:								   // Fenetre directory tree
		DFen=DFen->Fen2;

		if (DFen->FenTyp==6)
			CloseNewFen(DFen);
			else
			CreateNewFen(DFen,6);

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
		GestionFct(27); 		//--- Reload ---------------------------
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
			if (CommandLine("\n")!=0) //--- Pas de car. sur la ligne ---
				{
				break;
				}

			if (DFen->F[DFen->pcur]->name[0]=='*')
				{
				FUNCTION d;
				sscanf(DFen->F[DFen->pcur]->name+1,"%d",&d);
				GestionFct(d);
				break;
				}

		   if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
				{
				CommandLine("#cd %s",DFen->F[DFen->pcur]->name);
				break;
				}

		ext=getext(CURRENTNAME);
		if ( (!stricmp(ext,"COM")) | (!stricmp(ext,"BAT")) |
			 ((!stricmp(ext,"BTM")) & (KKCfg->_4dos==1)) )
			{
			CommandLine("%s\n",CURRENTNAME);
			break;
			}

		switch(i=EnterArcFile(DFen,DFen->F[DFen->pcur]->name))
			{
//			  case -2:				  break;
			case -1:											   // OK
				CommandLine("#cd .");
				break;
			case 173:
				RunDFenMacro(DFen->path,CURRENTNAME);
				break;
			case 57:									   // Executable
				CommandLine("%s\n",CURRENTNAME);
				break;
			default:


				strcpy(buf2,DFen->path);
				Path2Abs(buf2,CURRENTNAME);
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
//						   CommandLine(CURRENTNAME);
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
		if (KKCfg->ncmode)
			{
			memcpy(Cfg->palette,KKCfg->oldpal,48);
			memcpy(Cfg->col,KKCfg->oldcol,64);

			Cfg->TailleY=KKCfg->oldty;
			Cfg->TailleX=KKCfg->oldtx;
			Cfg->font=KKCfg->oldfont;
			KKCfg->lift=KKCfg->oldlift;

			Cfg->windesign=KKCfg->oldtype;

			KKCfg->dispath=KKCfg->olddispath;
			KKCfg->pathdown=KKCfg->oldpathdown;

			KKCfg->ncmode=0;
			}
		else
			{
			memcpy(KKCfg->oldpal,Cfg->palette,48);
			memcpy(KKCfg->oldcol,Cfg->col,64);
			KKCfg->oldtx=Cfg->TailleX;
			KKCfg->oldty=Cfg->TailleY;
			KKCfg->oldfont=Cfg->font;
			KKCfg->oldlift=KKCfg->lift;
			KKCfg->oldtype=Cfg->windesign;
			KKCfg->olddispath=KKCfg->dispath;
			KKCfg->oldpathdown=KKCfg->pathdown;

			memcpy(Cfg->palette,NORTONPAL,48);
			memcpy(Cfg->col,NORTONCOL,64);

			Cfg->TailleY=25;
			Cfg->TailleX=80;
			Cfg->font=0;
			KKCfg->lift=0;

			Cfg->windesign=1;
			KKCfg->dispath=1;
			KKCfg->pathdown=0;

			DesinitScreen();
			Cfg->display=0; //--- Normal Output ----------------------------
			InitScreen(0);

			KKCfg->ncmode=1;
			}


		GestionFct(67); 						   // Rafraichit l'ecran
		}
		break;
	case 86:
#ifndef NOINT
		EjectCD(DFen);
#endif
		break;
	case 87:
		GestionFct(GestionBar(0));
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
	case 93:
		if (oldhist[0]==32)
			strcpy(oldhist,CLstr);
		History2Line(GetNextHistCom(oldhist),buffer);
		CommandLine("0\r%s",buffer);
		break;
	case 94:
		if (oldhist[0]==32)
			strcpy(oldhist,CLstr);

		ext=oldhist;
		for(i=0;i<strlen(oldhist);i++)
			if ((oldhist[i]==32) | (oldhist[i]=='\\'))
				ext=oldhist+i+1;

		if (strlen(ext)<64)
			{
			sprintf(buf2,"%s*.*",ext);

			if (ext==oldhist)
				sprintf(buffer,"%s*.exe;%s*.com;%s*.bat;%s*.cmd",
													   ext,ext,ext,ext);
				else
				sprintf(buffer,buf2);

			for(i=oldhistnum;i<DFen->nbrfic;i++)
				if ( (!WildCmp(DFen->F[i]->name,buffer)) |
					 ((!WildCmp(DFen->F[i]->name,buf2)) &
					 ((DFen->F[i]->attrib & RB_SUBDIR)==RB_SUBDIR)) )
					{
					j=strlen(CLstr+strlen(oldhist)-strlen(ext));
					if (j>0)
						memset(buffer,8,j);
					buffer[j]=0;

					strcat(buffer,DFen->F[i]->name);
					strlwr(buffer);

					CommandLine(buffer);
					if ((DFen->F[i]->attrib & RB_SUBDIR)==RB_SUBDIR)
						CommandLine("\\");

					oldhistnum=i+1;
					break;
					}
			}
		break;
	case 95:
		KKCfg->isbar=(char)((KKCfg->isbar)^1);
		KKCfg->isidf=(char)((KKCfg->isidf)^1);
		GestionFct(67); 						   // Rafraichit l'ecran
		break;
	case 96:
		if (Info->macro)
			_cbuf[0]=WinMesg(_sbuf[0],_sbuf[1],_cbuf[0]);
		break;
	case 97:
#ifndef NOFONT
		Font2Buf(OldFont);
#endif

		for(i=0;i<MaxY;i++)
			for(j=0;j<MaxX;j++)
				{
				Screen_Buffer[((i*MaxX)+j)*2]=32;
				Screen_Buffer[((i*MaxX)+j)*2+1]=7;
				}

		OldCfg->TailleX=Cfg->TailleX;
		OldCfg->TailleY=Cfg->TailleY;
		PosX=0;
		PosY=(OldCfg->TailleY)-1;
		memcpy(OldCfg->palette,Cfg->palette,48);
		break;
	case 98:
		FileSetup();
		break;
	case 99:
		strcpy(buffer,"");
		View(&(KKCfg->V),buffer,0);
		break;
	case 100:
		CmpDirectory();
		break;
	case 101:
		LongnameSetup();
		break;
	case 102:
		GestionFct(14);
		GestionFct(15);
		if (KKCfg->cmdshell==1)
			{
			KKCfg->isidf=KKCfg->oldisidf;
			KKCfg->isbar=KKCfg->oldisbar;
			KKCfg->cmdshell=0;
			}
			else
			{
			KKCfg->oldisidf=KKCfg->isidf;
			KKCfg->oldisbar=KKCfg->isbar;
			KKCfg->isidf=0;
			KKCfg->isbar=0;
			KKCfg->cmdshell=1;
			}

		GestionFct(67);
		break;
	case 103:
		if (Info->macro)
			{
			DFen=Fenetre[2];

			strcpy(buffer,_sbuf[10]);
			Path2Abs(buffer,"..");
			CommandLine("#cd %s",buffer);

			FileinPath(_sbuf[10],buffer);

			_cbuf[0]=1; //--- Erreur -----------------------------------

			for (i=0;i<DFen->nbrfic;i++)
				if (!WildCmp(DFen->F[i]->name,buffer))
					{
					_cbuf[0]=0; //--- Pas d'erreur ---------------------
					DFen->scur=DFen->pcur=i;
					break;
					}
			}
		break;
	case 104:
#ifndef NOMOUSE
		MouseShortCut();
#endif
		break;
	case 105:
		GestionFct(14);
		GestionFct(15);
		break;
	case 106:
		Search4KKR();
		break;
	case 107:
		PlayerFromExt();
		break;
	case 108:
		ExtendedCopy();
		break;
	case 109:
		ViewBKGPage();
		break;
	case 110:
		ViewNewFile();
		break;
	case 111:
		DFen->sizedir=1;
		break;
	case 112:
		Wired();
		break;
	case 113:
		WiredCfg();
		break;
	}
}

void GetBufKey(int key,char *buffer);
/*--------------------------------------------------------------------*\
|- GetBufKey														  -|
\*--------------------------------------------------------------------*/
void GetBufKey(int key,char *buffer)
{
if (key==0)
	{
	buffer[0]=0;
	return;
	}

sprintf(buffer,"Unknown");

if ((key<=256) & (key>26))
	sprintf(buffer,"%c",key);

if (key<=26)
	sprintf(buffer,"Ctrl-%c",key+'A'-1);

switch(key)
	{
	case 27:  strcpy(buffer,"ESC");  break;

	case KEY_F(1):	 strcpy(buffer,"F1");  break;
	case KEY_F(2):	 strcpy(buffer,"F2");  break;
	case KEY_F(3):	 strcpy(buffer,"F3");  break;
	case KEY_F(4):	 strcpy(buffer,"F4");  break;
	case KEY_F(5):	 strcpy(buffer,"F5");  break;
	case KEY_F(6):	 strcpy(buffer,"F6");  break;
	case KEY_F(7):	 strcpy(buffer,"F7");  break;
	case KEY_F(8):	 strcpy(buffer,"F8");  break;
	case KEY_F(9):	 strcpy(buffer,"F9");  break;
	case KEY_F(10):  strcpy(buffer,"F10"); break;

	case KEY_F(1) + 0x1900:   strcpy(buffer,"Shift-F1");  break;
	case KEY_F(2) + 0x1900:   strcpy(buffer,"Shift-F2");  break;
	case KEY_F(3) + 0x1900:   strcpy(buffer,"Shift-F3");  break;
	case KEY_F(4) + 0x1900:   strcpy(buffer,"Shift-F4");  break;
	case KEY_F(5) + 0x1900:   strcpy(buffer,"Shift-F5");  break;
	case KEY_F(6) + 0x1900:   strcpy(buffer,"Shift-F6");  break;
	case KEY_F(7) + 0x1900:   strcpy(buffer,"Shift-F7");  break;
	case KEY_F(8) + 0x1900:   strcpy(buffer,"Shift-F8");  break;
	case KEY_F(9) + 0x1900:   strcpy(buffer,"Shift-F9");  break;
	case KEY_F(10) + 0x1900:  strcpy(buffer,"Shift-F10"); break;

	case KEY_F(1) + 0x2300:   strcpy(buffer,"Ctrl-F1");  break;
	case KEY_F(2) + 0x2300:   strcpy(buffer,"Ctrl-F2");  break;
	case KEY_F(3) + 0x2300:   strcpy(buffer,"Ctrl-F3");  break;
	case KEY_F(4) + 0x2300:   strcpy(buffer,"Ctrl-F4");  break;
	case KEY_F(5) + 0x2300:   strcpy(buffer,"Ctrl-F5");  break;
	case KEY_F(6) + 0x2300:   strcpy(buffer,"Ctrl-F6");  break;
	case KEY_F(7) + 0x2300:   strcpy(buffer,"Ctrl-F7");  break;
	case KEY_F(8) + 0x2300:   strcpy(buffer,"Ctrl-F8");  break;
	case KEY_F(9) + 0x2300:   strcpy(buffer,"Ctrl-F9");  break;
	case KEY_F(10) + 0x2300:  strcpy(buffer,"Ctrl-F10"); break;

	case KEY_F(1) + 0x2D00:   strcpy(buffer,"Alt-F1");  break;
	case KEY_F(2) + 0x2D00:   strcpy(buffer,"Alt-F2");  break;
	case KEY_F(3) + 0x2D00:   strcpy(buffer,"Alt-F3");  break;
	case KEY_F(4) + 0x2D00:   strcpy(buffer,"Alt-F4");  break;
	case KEY_F(5) + 0x2D00:   strcpy(buffer,"Alt-F5");  break;
	case KEY_F(6) + 0x2D00:   strcpy(buffer,"Alt-F6");  break;
	case KEY_F(7) + 0x2D00:   strcpy(buffer,"Alt-F7");  break;
	case KEY_F(8) + 0x2D00:   strcpy(buffer,"Alt-F8");  break;
	case KEY_F(9) + 0x2D00:   strcpy(buffer,"Alt-F9");  break;
	case KEY_F(10) + 0x2D00:  strcpy(buffer,"Alt-F10"); break;


#ifndef LINUX

	case KEY_F11 :	strcpy(buffer,"F11");  break;
	case KEY_F12 :	strcpy(buffer,"F12");  break;
	case KEY_F11 + 0x0200:	 strcpy(buffer,"Shift-F11");  break;
	case KEY_F12 + 0x0200:	 strcpy(buffer,"Shift-F12");  break;
	case KEY_F11 + 0x0400:	 strcpy(buffer,"Ctrl-F11");  break;
	case KEY_F12 + 0x0400:	 strcpy(buffer,"Ctrl-F12");  break;
	case KEY_F11 + 0x0600:	 strcpy(buffer,"Alt-F11");  break;
	case KEY_F12 + 0x0600:	 strcpy(buffer,"Alt-F12");  break;

	case KEY_INS: strcpy(buffer,"INS"); break;
    case KEY_A_ENTER: strcpy(buffer,"Alt-ENTER"); break;

	case KEY_C_PPAGE : strcpy(buffer,"Ctrl-PGUP");   break;
	case KEY_C_NPAGE : strcpy(buffer,"Ctrl-PGDN");   break;
	case KEY_C_UP	 : strcpy(buffer,"Ctrl-UP");     break;
	case KEY_C_DOWN  : strcpy(buffer,"Ctrl-DOWN");   break;
	case KEY_C_RIGHT : strcpy(buffer,"Ctrl-RIGHT");  break;
	case KEY_C_LEFT  : strcpy(buffer,"Ctrl-LEFT");   break;

//	  case KEY_S_PPAGE : strcpy(buffer,"Shift-PGUP");   break;
//	  case KEY_S_NPAGE : strcpy(buffer,"Shift-PGDN");   break;
//	  case KEY_S_UP    : strcpy(buffer,"Shift-UP");     break;
//	  case KEY_S_DOWN  : strcpy(buffer,"Shift-DOWN");   break;
//	  case KEY_S_RIGHT : strcpy(buffer,"Shift-RIGHT");  break;
//	  case KEY_S_LEFT  : strcpy(buffer,"Shift-LEFT");   break;
//	  case KEY_S_END   : strcpy(buffer,"Shift-END");    break;
//	  case KEY_S_HOME  : strcpy(buffer,"Shift-HOME");   break;

//	  case KEY_C_END   : strcpy(buffer,"Ctrl-END");    break;
//	  case KEY_C_HOME  : strcpy(buffer,"Ctrl-HOME");   break;

//	  case KEY_A_PPAGE : strcpy(buffer,"Alt-PGUP");   break;
	case KEY_A_NPAGE : strcpy(buffer,"Alt-PGDN");   break;
//	  case KEY_A_UP    : strcpy(buffer,"Alt-UP");     break;
//	  case KEY_A_DOWN  : strcpy(buffer,"Alt-DOWN");   break;
//	  case KEY_A_RIGHT : strcpy(buffer,"Alt-RIGHT");  break;
//	  case KEY_A_LEFT  : strcpy(buffer,"Alt-LEFT");   break;
//	  case KEY_A_END   : strcpy(buffer,"Alt-END");    break;
//	  case KEY_A_HOME  : strcpy(buffer,"Alt-HOME");   break;

#endif 

	case KEY_PPAGE : strcpy(buffer,"PGUP");   break;
	case KEY_NPAGE : strcpy(buffer,"PGDN");   break;
	case KEY_UP    : strcpy(buffer,"UP");     break;
	case KEY_DOWN  : strcpy(buffer,"DOWN");   break;
	case KEY_RIGHT : strcpy(buffer,"RIGHT");  break;
	case KEY_LEFT  : strcpy(buffer,"LEFT");   break;
	case KEY_END   : strcpy(buffer,"END");    break;
	case KEY_HOME  : strcpy(buffer,"HOME");   break;
	}
}





/*--------------------------------------------------------------------*\
|-	Gestion de la barre de menu du haut 							  -|
\*--------------------------------------------------------------------*/

FUNCTION GestionBar(int i)
{
int retour,nbmenu;
int n,u,s,x;
int max;

BARMENU bar[20];
MENU menu;

FUNCTION fin;

static int cpos[20],poscur;

KKWin Win;

retour=0;

do
{
for(n=0;n<7;n++)
	bar[n].Titre=_BarHeader[n];

/*
bar[0].Titre="File";
bar[1].Titre="Panel";
bar[2].Titre="Disk";
bar[3].Titre="Selection";
bar[4].Titre="Tools";
bar[5].Titre="Options";
bar[6].Titre="Help";
*/


bar[0].Help=0;
bar[1].Help=0;
bar[2].Help=0;
bar[3].Help=0;
bar[4].Help=0;
bar[5].Help=0;
bar[6].Help=0;

u=BarMenu(bar,7,&poscur,&x,i);

							   // Renvoit t: position du machin surligne
			// Renvoit v: 0 si rien, autre si position dans sous fenetre
if (u==0)
	{
	fin=0;
	break;
	}

switch (poscur) 	// Marjo
	{
	case 0:
		bar[0].fct=7;
		bar[1].fct=8;
		bar[2].fct=9;
		bar[3].fct=0;
		bar[4].fct=10;
		bar[5].fct=11;
		bar[6].fct=12;
		bar[7].fct=13;
		bar[8].fct=0;
		bar[9].fct=64;
		bar[10].fct=0;
		bar[11].fct=88;
		nbmenu=12;
		break;
	case 1:
		bar[0].fct=14;
		bar[1].fct=15;
		bar[2].fct=0;
		bar[3].fct=21;
		bar[4].fct=63;
		bar[5].fct=0;
		bar[6].fct=22;
		bar[7].fct=23;
		bar[8].fct=24;
		bar[9].fct=25;
		bar[10].fct=26;
		nbmenu=11;
		break;
	case 2:
		bar[0].fct=6;
		bar[1].fct=0;
		bar[2].fct=34;
		nbmenu=3;
		break;
	case 3:
		bar[0].fct=3;
		bar[1].fct=4;
		bar[2].fct=2;
		nbmenu=3;
		break;
	case 4:
		bar[0].fct=5;
		bar[1].fct=19;
		bar[2].fct=100;
		bar[3].fct=111;
		bar[4].fct=0;
		bar[5].fct=28;
		bar[6].fct=76;
		bar[7].fct=0;
		bar[8].fct=90;
		nbmenu=9;
		break;
	case 5:
		bar[0].fct=31;
		bar[1].fct=66;
		bar[2].fct=0;
		bar[3].fct=17;
		bar[4].fct=82;
		bar[5].fct=62;
		nbmenu=6;
		break;
	case 6:
		bar[0].fct=1;
		bar[1].fct=18;
		nbmenu=2;
		break;
	}

max=0;

for(n=0;n<nbmenu;n++)
	{
	if ((bar[n].fct)!=0)
		{
		ushort key;
		int len;
		char keybuf[20];

		key=GetKeyScan((FUNCTION)(bar[n].fct));
		GetBufKey(key,keybuf);

		if (keybuf[0]!=0)
			len=strlen(InfoFct[bar[n].fct-1].info)+strlen(keybuf)+2;
		else
			len=strlen(InfoFct[bar[n].fct-1].info);

		if (len>max)
			max=len;
		}
	}

for(n=0;n<nbmenu;n++)
	{
	if ((bar[n].fct)!=0)
		{
		ushort key;
		int len;
		char buf[80],keybuf[20];

		key=GetKeyScan((FUNCTION)(bar[n].fct));

		GetBufKey(key,keybuf);

		len=max-strlen(InfoFct[bar[n].fct-1].info);

		sprintf(buf,"%s%*s",InfoFct[bar[n].fct-1].info,len,keybuf);

		bar[n].Titre=(char*)GetMem(strlen(buf)+1);
		strcpy(bar[n].Titre,buf);
		bar[n].Help=0;
		}
	else
		{
		bar[n].Help=0;
		bar[n].Titre=NULL;
		}
	}

s=2;

menu.x=x;
menu.y=s;
menu.attr=0;
menu.cur=cpos[poscur];

retour=PannelMenu(bar,nbmenu,&menu);	  // (x,y)=(t,s)

for(n=0;n<nbmenu;n++)
	{
	LibMem(bar[n].Titre);
	}

cpos[poscur]=menu.cur;

if (retour==2)
	{
	fin=(FUNCTION)(bar[cpos[poscur]].fct);
	break;
	}
	else
	{
	poscur+=retour;
	fin=0;
	}

if ((retour==1) | (retour==-1))
	PutKey(KEY_DOWN);

//PrintAt(0,2,"(%d,%d)",poscur,retour);
//Wait(0,0);

i=0;	//--- plus de retour ym!=0 -------------------------------------
}
while(1);


return fin;
}

/*--------------------------------------------------------------------*/



/*--------------------------------------------------------------------*\
|- Fonction ChangeType: 											  -|
|-	   Change le type des fenˆtres									  -|
|- (0 pour incrementer le type des fenˆtres 						  -|
\*--------------------------------------------------------------------*/
void ChangeType(char n)
{
if (n==0)
   Cfg->windesign++;
   else
   Cfg->windesign=n;

if (Cfg->windesign>4) Cfg->windesign=1;

CalcSizeWin(Fenetre[0]);
CalcSizeWin(Fenetre[1]);
}


/*--------------------------------------------------------------------*\
|-						  Fenetre Select File						  -|
\*--------------------------------------------------------------------*/

void SelectPlus(void)
{
int n;

MacAlloc(10,80);

if (!(Info->macro))
	strcpy(_sbuf[10],Select_Chaine);

NumHelp(3);
n=MWinTraite(select_kkt);
if (n==-1)
	{
	MacFree(10);
	return;
	}

strcpy(Select_Chaine,_sbuf[10]);
MacFree(10);

for (n=0;n<DFen->nbrfic;n++)
	if ( (KKCfg->seldir==1) |
						 ((DFen->F[n]->attrib & RB_SUBDIR)!=RB_SUBDIR) )
			if (!WildCmp(DFen->F[n]->name,Select_Chaine))
				FicSelect(DFen,n,1);
}

/*--------------------------------------------------------------------*\
|-						 Fenetre Deselect File						  -|
\*--------------------------------------------------------------------*/

void SelectMoins(void)
{
int n;

MacAlloc(10,80);

if (!(Info->macro))
	strcpy(_sbuf[10],Select_Chaine);

NumHelp(4);
n=MWinTraite(select_kkt);
if (n==-1)
	{
	MacFree(10);
	return;
	}

strcpy(Select_Chaine,_sbuf[10]);
MacFree(10);

for (n=0;n<DFen->nbrfic;n++)
	if ( (KKCfg->seldir==1) |
						 ((DFen->F[n]->attrib & RB_SUBDIR)!=RB_SUBDIR) )
		if (!WildCmp(DFen->F[n]->name,Select_Chaine))
			FicSelect(DFen,n,0);
}


void SelectPlusMoins(void)
{
int m,n;

MacAlloc(10,80);

if (!(Info->macro))
	strcpy(_sbuf[10],Select_Chaine);


n=MWinTraite(seluns_kkt);
if (n==-1)
	{
	MacFree(10);
	return;
	}

strcpy(Select_Chaine,_sbuf[10]);
MacFree(10);

for (m=0;m<DFen->nbrfic;m++)
	if ( (KKCfg->seldir==1) |
						 ((DFen->F[m]->attrib & RB_SUBDIR)!=RB_SUBDIR) )
		if (!WildCmp(DFen->F[m]->name,Select_Chaine))
			{
			if (n==2)
				FicSelect(DFen,m,0);

			if (n==1)
				FicSelect(DFen,m,1);
			}
}


/*--------------------------------------------------------------------*\
|-						Fenetre History Directory					  -|
\*--------------------------------------------------------------------*/
void HistDir(void)
{
MENU menu;

int i,j;
static BARMENU dir[86];  // 256:3

j=0;
for (i=0;i<86;i++)
	{
	dir[i].Titre=KKCfg->HistDir+j;
	dir[i].Help=0;
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

/*--------------------------------------------------------------------*\
|- Change to a directory in the path								  -|
\*--------------------------------------------------------------------*/
void PathDir(void)
{
MENU menu;

int i,j,k;
static BARMENU dir[50];
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
	dir[i].Help=0;
	dir[i].fct=i+1;
//	strupr(dir[i].Titre); 
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
|-						Fenetre History Commande					  -|
\*--------------------------------------------------------------------*/
int HistComFct(BARMENU *bar);

int HistComFct(BARMENU *bar)
{
MenuCreat(bar->Titre,bar->Titre,DFen->path);
return 0;
}

void HistCom(void)
{
MENU menu;

char dest[256];
int i,j;
static BARMENU dir[100];

j=0;
for (i=0;i<100;i++)
	{
	History2Line(KKCfg->HistCom+j,dest);
	dest[76]=0;
	if (strlen(dest)==0) break;
	dir[i].Titre=(char*)GetMem(strlen(dest)+1);
	strcpy(dir[i].Titre,dest);
	dir[i].fct=i+1;
	dir[i].Help=69;

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
|-						   Fenetre CD machin						  -|
\*--------------------------------------------------------------------*/

void WinCD(void)
{
static char Dir[70]="";
int n;

MacAlloc(10,80);

if (!(Info->macro))
	strcpy(_sbuf[10],Dir);

NumHelp(29);
n=MWinTraite(chgdir_kkt);

if (n!=-1)
	{
	strcpy(Dir,_sbuf[10]);

	switch(n)
		{
		case 0: //--- OK -----------------------------------------------
			if (Dir[strlen(Dir)-1]!='*')
				strcat(Dir,"*");
			ExecLCD(DFen,Dir);
			break;
		case 1: //--- Normal CD ----------------------------------------
			CommandLine("#cd %s",Dir);
			break;
		case 2: //--- Create Tree --------------------------------------
			MakeNCD();
			break;
		}
	}

MacFree(10);
}


/*--------------------------------------------------------------------*\
|-				  Create directory in current window				  -|
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

NumHelp(12);
n=WinTraite(T,5,&F,0);

if (n!=-1)
	if (T[n].type!=3)
		{
		CommandLine("#md %s",Dir);
		CommandLine("#cd %s",Dir);
		CommandLine("#cd ..");
		}
}


/*--------------------------------------------------------------------*\
|-							   Edit File							  -|
\*--------------------------------------------------------------------*/
void EditFile(char *s)
{
char buffer[256];

strcpy(buffer,DFen->path);
Path2Abs(buffer,s);

if (KKCfg->editeur[0]!=0)
	CommandLine("#%s %s",KKCfg->editeur,buffer);
	else
	TxtEdit(&(KKCfg->E),buffer);
}

/*--------------------------------------------------------------------*\
|-							 Edit New File							  -|
\*--------------------------------------------------------------------*/

void EditNewFile(void)
{
int n;
char buffer[256];

MacAlloc(10,256);

if (!(Info->macro))
	{
	strcpy(_sbuf[10],DFen->path);
	Path2Abs(_sbuf[10],DFen->F[DFen->pcur]->name);
	}


n=MWinTraite(editfile_kkt);
if (n==-1)
	{
	MacFree(10);
	return;
	}

strcpy(buffer,DFen->path);
Path2Abs(buffer,_sbuf[10]);
MacFree(10);

switch(n)
	{
	case 1: //--- External editor --------------------------------------
		EditFile(buffer);
		break;
	case 2: //--- Internal editor --------------------------------------
		TxtEdit(&(KKCfg->E),buffer);
		break;
	}
}

void ViewNewFile(void)
{
int n;
char buffer[256];

MacAlloc(10,256);

if (!(Info->macro))
	{
	strcpy(_sbuf[10],DFen->path);
	Path2Abs(_sbuf[10],DFen->F[DFen->pcur]->name);
	}


n=MWinTraite(viewfile_kkt);
if (n==-1)
	{
	MacFree(10);
	return;
	}

strcpy(buffer,DFen->path);
Path2Abs(buffer,_sbuf[10]);
MacFree(10);

switch(n)
	{
	case 1: //--- External editor --------------------------------------
		ViewFile(buffer);
		break;
	case 2: //--- Internal editor --------------------------------------
		View(&(KKCfg->V),buffer,0);
		break;
	case 3: //--- Internal text editor ---------------------------------
		View(&(KKCfg->V),buffer,4);
		break;
	}
}

void ViewFile(char *fichier)
{
char buffer[256];

if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
	return;

if (DFen->FenTyp!=0)
	return;

// TestPreMacro(DFen,fichier);

strcpy(buffer,DFen->path);
Path2Abs(buffer,fichier);

//printf("Traite Commandline after dos (%d) FIN)\n\n",DFen->system); exit(1);

if (KKCfg->vieweur[0]!=0)
	CommandLine("#%s %s",KKCfg->vieweur,buffer);
	else
	{
	switch(DFen->system)
		{
		case 0:
		   View(&(KKCfg->V),buffer,0);
			break;
		}
	}
}

/*--------------------------------------------------------------------*\
|-						 Create KKD disk							  -|
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
			if (IsSlash(Dir[n])) break;
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
|-					  Change drive of current window				  -|
\*--------------------------------------------------------------------*/
void ChangeDrive(char *OldPath)
{
static char path[256],path2[256];
char buffer[32],volume[32];
long fpos,cpos,pos;

char drive[26];
long m,n,x,l,nbr;
long x1;
char d;
int i,olddrive;

int p1,p2;
int car;

DriveSet(DFen->path);

nbr=0;

for (i=0;i<26;i++)
	{
	drive[i]=(char)DriveExist((char)i);
	if (drive[i]==1)
		nbr++;
	}

l=36/nbr;
if (l>3) l=3;
if (nbr<5) l=6;
if (nbr<2) l=9;
x=(40-(l*nbr))/2+DFen->x;

KKWin Win;

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


cpos=50;
fpos=0;

i=toupper(OldPath[0])-'A';
olddrive=-1;

car=0;

do	{
	do
		{
		switch(car)
			{
			case KEY_LEFT:	i--,cpos=50; break; 				 // LEFT
			case KEY_RIGHT: i++,cpos=50; break; 				// RIGHT
			case KEY_DOWN:	cpos++; break;						 // DOWN
			case KEY_UP:	cpos--; break;						   // UP
			case KEY_HOME:	cpos=12; break; 					 // HOME
			case KEY_END:	cpos=pos-1; break;					  // END
			}

		if (i>=26) i=0;
		if (i<0) i=25;

		if ( (drive[i]!=0) & (cpos==50) )
			if (DriveReady((char)i)==1)
				{
				Window(x1+2,12,x1+35,19,Cfg->col[16]);

				if (OldPath[0]==i+'A')
					strcpy(path,OldPath);
					else
					DrivePath((char)i,path);

				if (path[strlen(path)-1]!=DEFSLASH)
					strcat(path,"\\");

				DriveInfo((char)i,buffer);

				sprintf(volume,"[%s]",buffer);

	 //--- Calcule le nombre de position occup‚e par les repertoires ---
				pos=12;
				for (p2=0;p2<strlen(path);p2++)
					if (IsSlash(path[p2]))
						pos++;
				break;
				}
				else
				{
				if ( (HI(car)!=GAUCHE) & (HI(car)!=DROITE) )
					{
					if (olddrive!=-1)
						i=olddrive;
						else
						car=DROITE;
					cpos=0;
					}
				}
		}
	while (cpos==50);

	if (cpos>=pos)
		cpos=pos-1;

	if (cpos<12)
		cpos=12;

	while (cpos-fpos>19)
		fpos++;

	while (cpos-fpos<12)
		fpos--;

	pos=12;
	p1=0;
	p2=0;

	for (p2=0;p2<strlen(path);p2++)
		{
		if (IsSlash(path[p2]))
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

#ifndef NOMOUSE
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
#endif

	AffCol(drive[i],9,Cfg->col[16]);

	if (LO(car)==0)
		{
		switch(HI(car))
			{
			case 0x3B:
				HelpTopic(72);
				break;
			}
		}

	if (HI(car)==0)
		{
		d=(uchar)(toupper(car)-'A');
		if (d<26)	//--- d is always > 0 ------------------------------
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
	if (IsSlash(path[p2]))
		{
		if (pos==cpos)
			path[p2]=0;
		pos++;
		}
	}
if (path[strlen(path)-1]==':') strcat(path,"\\");

if (car==13)
	{
	CommandLine("#cd %s",path);
	DFen->FenTyp=0;
	}
}


/*--------------------------------------------------------------------*\
|-				  Change drive of current window for KKD			  -|
\*--------------------------------------------------------------------*/
int ChangeToKKD(void)
{
char drive[26];
long m,n,x,l,nbr;
signed char i;

int car;

nbr=0;

for (i=0;i<26;i++)
	{
	drive[i]=(char)DriveExist(i);
	if (drive[i]==1)
		nbr++;
	}

l=36/nbr;
if (l>3) l=3;
if (nbr<5) l=6;
if (nbr<2) l=9;
x=(40-(l*nbr))/2+DFen->x;


KKWin Win;

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
do	{
	do {
		if (HI(car)==GAUCHE) i--;
		if (HI(car)==DROITE) i++;
		if (i>=26) i=0;
		if (i<0) i=25;
		} while (drive[i]==0);

	if (HI(car)==0)
		{
		car=(toupper(car)-'A');
		if ( (car>=0) & (car<26) )
			if (drive[car]!=0)
				{
				i=(char)car;
				car=13;
				break;
				}
	   }

	AffCol(drive[i],9,Cfg->col[17]);

	car=Wait(0,0);
	AffCol(drive[i],9,Cfg->col[16]);

} while (LO(car)!=13);

CommandLine("#cd %c:\\",i+'A');

return i;
}

void QuickSearch(int key,BYTE *c,BYTE *c2)
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
			if (((char)(car3/256))==bkey[n])
				car=(char)(n+'a');
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

	InitDispNewFen(DFen);
	DispNewFen(DFen);
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

SFen=AllocWin();

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

FreeWin(SFen);

if (k==-1)
	return NULL;
	else
	return buf;
}


/*--------------------------------------------------------------------*\
|-						Access fichier suivant system				  -|
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
	case 0: 													  // DOS
		strcpy(nom,DFen->path);
		Path2Abs(nom,tnom);
		break;
	case 1: 													  // RAR
	case 2: 													  // ARJ
	case 3: 													  // ZIP
	case 4: 													  // LHA
	case 5: 													  // KKD
	case 6: 													  // DFP
	case 8: 													  // RAW
	case 9: 													  // DKF
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
		Copie(DFen,Fenetre[2]->path);		  // Quitte aprŠs

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
		if (!WildCmp(tnom,DFen->F[i]->name))
			{
			DFen->pcur=i;
			DFen->scur=i;
			break;
			}
	}
return nom;
}

int TestPreMacro(FENETRE *Fen,char *tnom)
{
RB_IDF Info;

strcpy(Info.path,Fen->path);
Path2Abs(Info.path,tnom);

assert(Info.path[0]!=0); //--- Stop program if this condition arrive ---

Traitefic(&Info);

if (Info.numero==173)
	{
	if ((Info.message[0][0])!=0)		//--- Link ---------------------
		{
		RunMacro(DFen->path,Info.path);

		if (_cbuf[0]==1)	//--- Erreur -------------------------------
			return -2;

		strcpy(Info.path,DFen->path);
		Path2Abs(Info.path,DFen->F[DFen->pcur]->name);

		assert(Info.path[0]!=0); //--- Stop prog if this cdt arrive ----

		Traitefic(&Info);
		}
	}

return Info.numero;
}




int EnterArcFile(FENETRE *Fen,char *tnom)
{
RB_IDF Info;

strcpy(Info.path,Fen->path);
Path2Abs(Info.path,tnom);

assert(Info.path[0]!=0); //--- Stop program if this condition arrive ---

Traitefic(&Info);

// ColLin(0,0,40,Cfg->col[7]);	 PrintAt(0,0,"EAF1"); Wait(0,0);

if (Info.numero==173)
	{
	if ((Info.message[0][0])!=0)
		{
		RunMacro(DFen->path,Info.path);

		if (_cbuf[0]==1)	//--- Erreur -------------------------------
			return -2;

		strcpy(Info.path,DFen->path);
		Path2Abs(Info.path,DFen->F[DFen->pcur]->name);

		assert(Info.path[0]!=0); //--- Stop prog if this cdt arrive ----

		Traitefic(&Info);
		}
	}

switch (Info.numero)
	{
	case 30:	// ARJ
	case 34:	// RAR
	case 35:	// ZIP
	case 32:	// LHA
	case 102:	// KKD
	case 139:	// DFP
	case 195:	// DKF
	case 155:	// RAW
		strcpy(Fen->VolName,Info.path);
		strcpy(Fen->path,DFen->VolName);
		break;
	default:
		return Info.numero;
	}

switch (Info.numero)
	{
	case 34:	// RAR
		Fen->system=1;
		break;
	case 30:	// ARJ
		Fen->system=2;
		break;
	case 35:	// ZIP
		Fen->system=3;
		break;
	case 32:	// LHA
		Fen->system=4;
		break;
	case 139:	// DFP
		Fen->system=6;
		break;
	case 155:	// RAW
		Fen->system=8;
		break;
	case 195:	// DKF
		Fen->system=9;
		break;
	case 102:	// KKD
		Fen->KKDdrive=0;
		Fen->system=5;
		break;
	}

return -1;
}




/*--------------------------------------------------------------------*\
|-	Retourne 0 si OK												  -|
|- Sinon retourne numero de IDF 									  -|
\*--------------------------------------------------------------------*/
int EnterHost(void)
{
static char buf[256];

strcpy(buf,DFen->path);
Path2Abs(buf,DFen->F[DFen->pcur]->name);

strcpy(DFen->VolName,DFen->path);

strcpy(DFen->path,"*1-C:\\");

DFen->system=7;

CommandLine("#cd .");
return 0;														   // OK
}




/*--------------------------------------------------------------------*\
|- Sauvegarde selection en m‚moire									  -|
\*--------------------------------------------------------------------*/
char *pcurname;
char **selname;
int F1pcur,F1scur,F1nbrsel;

/*--------------------------------------------------------------------*\
|-	Sauvegarde la selection sur disque								  -|
\*--------------------------------------------------------------------*/
void SaveSel(FENETRE *F1)
{
struct file *F;
int i,j;

pcurname=(char*)GetMem(strlen(F1->F[F1->pcur]->name)+1);
strcpy(pcurname,F1->F[F1->pcur]->name);

F1pcur=F1->pcur;
F1scur=F1->scur;
F1nbrsel=F1->nbrsel;

if (F1nbrsel!=0)
	{
	selname=(char**)GetMem(sizeof(char*)*F1->nbrsel);
	j=0;

	for(i=0;i<F1->nbrfic;i++)
		{
		F=F1->F[i];

		if ((F->select)==1)
			{
			selname[j]=(char*)GetMem(strlen(F->name)+1);
			strcpy(selname[j],F->name);
			j++;
			}
		}
	}
}

/*--------------------------------------------------------------------*\
|-	Charge la selection du disque									  -|
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
	if (!WildCmp(DFen->F[i]->name,nom))
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
						FicSelect(DFen,i,1);				   // Select
						break;
					case 1:
						FicSelect(DFen,i,2);		// Inverse Selection
						break;
					}
				}
			}
		}
	LibMem(selname);
	}
}





/*--------------------------------------------------------------------*\
|-							Programme Principal 					  -|
\*--------------------------------------------------------------------*/
void Gestion(void)
{
FUNCTION fct;
clock_k Cl,Cl_Start;
BYTE car,car2;
long car3,c;
int oldsel=-1;	   //--- Dernier ‚tat du selectfile --------------------
int i;	//--- Compteur -------------------------------------------------

#ifndef NOMOUSE
int oldxm,oldym,oldzm2,oldzm=0,xm,ym,zm;
#endif

Info->temps=GetClock()-Info->temps;

do
{
do
{
KKCfg->scrrest=1;

KKCfg->noprompt&=126;						// Met le dernier bit … z‚ro

if ( (KKCfg->key==0) & (KKCfg->FenAct>1) )
	{
	GestionFct(43); //--- Active la fenetre principale -----------------
	}
	else
	{
	if (KKCfg->FenAct>1)
		{
		DFen=Fenetre[2];
		DFen->ChangeLine=1; 						   // Affichage Path
		}
		else
		DFen=Fenetre[KKCfg->FenAct];
	}

if (KKCfg->key==0)			  //--- Switch if key buffer is void ---
	{
	if (DFen->FenTyp==5)
		ViewFileID(DFen);

	if ( (DFen->FenTyp!=0) & (DFen->Fen2->FenTyp==0) )
		GestionFct(89);
	}

if (Fenetre[1]->FenTyp==0)
	for (i=1;i>=0;i--)
		InitDispNewFen(Fenetre[i]);
	else
	for (i=0;i<2;i++)
		InitDispNewFen(Fenetre[i]);


fct=GetLIFOFct();

if (fct!=0)
	{
	DFen->oldscur=0;			//--- On retire l'acceleration ---------
	DFen->oldpcur=-1;
	DFen->Fen2->oldscur=0;
	DFen->Fen2->oldpcur=-1;

	GestionFct(fct);
	}
}
while(fct!=0);


if (KKCfg->key==0)
	{
	if ((KKCfg->strash>=KKCfg->mtrash) & (KKCfg->mtrash!=0))
		GestionFct(34); 				// Efface la trash si trop plein
	}

if (KKCfg->key==0)
	{
	Cl_Start=GetClock();
	Cl=GetClock();

	c=0;

	AffCmdLine();

	car=0;	//--- Keyboard flag nul par default ------------------------

#ifndef NOMOUSE
	GetPosMouse(&oldxm,&oldym,&oldzm2);
#endif

	while ( (!KbHit()) & (c==0) ) // RedBug
		{
#ifndef NOMOUSE
		int n,xm2,ym2,zm2;

		GetPosMouse(&xm,&ym,&zm);

		if (DFen->sizedir==1)
		for(n=0;n<DFen->nbrfic;n++)
			{
			char path[256];
			int size;
			struct file *F;

			F=DFen->F[n];

			if ((F->info==NULL) & ((F->attrib & RB_SUBDIR)==RB_SUBDIR)
				 & (F->name[0]!='.') )
				{
				strcpy(path,DFen->path);
				Path2Abs(path,DFen->F[n]->name);

				size=RepSize(path);
				if (size>=0)
					{
					F->size=size;
					F->info=(char*)GetMem(82);
					sprintf(F->info,"%cDirectory",0);

					DFen->oldscur=0;  //--- On retire l'acceleration ---
					DFen->oldpcur=-1;

					InitDispNewFen(DFen);
					DispNewFen(DFen);
					break;
					}
				}
			}

		if ( (ym==0) & ((oldym!=0) | (oldxm!=xm)) )
			GestionFct(GestionBar(1));

		if (zm==oldzm)
			zm=0;
			else
			oldzm=zm;

		oldxm=xm;
		oldym=ym;

		if ((zm&3)!=0)	  //--- Bouton droit ou gauche de la souris ----
			{
			//--- Va dans l'autre fenˆtre ------------------------------
			if ((xm>=DFen->Fen2->x) & (xm<=DFen->Fen2->xl) &
								  (ym<=DFen->Fen2->yl2+DFen->Fen2->y2) &
													(ym>DFen->Fen2->y2))
				{
				c=9;
				oldzm=0;
				}
				else

			//--- Gestion ascensceur -----------------------------------
			if ((ym<DFen->yl) & (ym>=DFen->y2) & (xm==DFen->xl) &
														  (KKCfg->lift))
				{
				int n;

				n=ym-DFen->y2;
				n=n*DFen->nbrfic+(DFen->nbrfic-1);
				n=n/(DFen->yl-DFen->y2);
				n=n-DFen->pcur;

				DeplPCurFen(DFen,n);

				c=3;	//--- On ne fait rien d'autre ------------------
				oldzm=0;
				}
				else

			//--- Monte d'une ligne ------------------------------------
			if ((ym<=DFen->y2) & (ym>=DFen->y) & (xm>=DFen->x) &
														 (xm<=DFen->xl))
				{
				GestionFct(37);
				oldzm=0;
				c=3;
				}
				else
			if ((ym>DFen->yl2+DFen->y2) & (ym<=DFen->yl) &
										 (xm>=DFen->x) & (xm<=DFen->xl))
				{
				GestionFct(36);
				oldzm=0;
				c=3;
				}
				else
			if ( (iskeypressed(SHIFT)) & (xm>=DFen->x) & (xm<=DFen->xl) &
				 (ym<=DFen->yl2+DFen->y2) & (ym>DFen->y2) )  //- Shift -
				{
				n=ym-(DFen->y2+1)-DFen->scur;

				if ( ((DFen->pcur+n)>=0) &	((DFen->scur+n)>=0))
					{
					DeplPCurFen(DFen,n);

					switch (oldsel)
						{
						case -1:
							oldsel=DFen->F[DFen->pcur]->select;
							FicSelect(DFen,DFen->pcur,2);
							break;
						case 0:
							FicSelect(DFen,DFen->pcur,1);
							break;
						case 1:
							FicSelect(DFen,DFen->pcur,0);
							break;
						}
					c=3;					   //--- On ne fait rien ---

					oldzm=0;
					}
				}
				else
				{
				if (oldsel==-1)
				{
				if ((ym==Cfg->TailleY-1) & (KKCfg->isbar))
					GetMouseFctBar(0);
					else
				if ( ((xm<DFen->Fen2->x)|(xm>DFen->Fen2->xl))&
					 ((xm<DFen->x)|(xm>DFen->xl))&
					 (ym<=DFen->yl2+DFen->y2) & (ym>DFen->y2))
					{
					n=(ym-1)/3;
					if ( (n>=0) & (n<6) )
						Cadre(40,1+n*3,49,3+n*3,2,
											 Cfg->col[56],Cfg->col[55]);
					}

				do
					{
					GetPosMouse(&xm2,&ym2,&zm2);
					if ((ym==Cfg->TailleY-1) & (KKCfg->isbar)) // -FX---
						GetMouseFctBar(1);
					}
				while ((zm2&3)!=0);

				if ( ((xm>=DFen->x) & (xm<=DFen->xl)) &
					 ((xm2>=DFen->Fen2->x) & (xm2<=DFen->Fen2->xl)) &
					 ((ym<=DFen->yl2+DFen->y2) & (ym>DFen->y2)) &
					 ((ym2<=DFen->Fen2->yl2+DFen->Fen2->y2) &
					  (ym2>DFen->Fen2->y2)) )
					 c=0x3F00;		// GestionFct(10) pour copie -------
					else
				if ((ym==Cfg->TailleY-1) & (KKCfg->isbar))	//- Fx key -
					{
					c=GetMouseFctBar(2);

					if (c!=0)
						{
						if (iskeypressed(SHIFT))
							c=c+0x1900; 	 //--- shift ---------------
						if (iskeypressed(CTRL))
							c=c+0x2300; 	 //--- ctrl ----------------
						if (iskeypressed(ALT))
							c=c+0x2D00; 	 //--- alt -----------------
						}
					}
					else
				if (ym==0)
					c=0x4300; //--- GestionFct(87) ---------------------
					else
				if ((xm>=DFen->x) & (xm<=DFen->xl) &
							   (ym<=DFen->yl2+DFen->y2) & (ym>DFen->y2))
					{
					n=ym-(DFen->y2+1)-DFen->scur;

					if ( ((DFen->pcur+n)>=0) &	((DFen->scur+n)>=0) &
						 ((DFen->pcur+n)<DFen->nbrfic) )
						{
						DeplPCurFen(DFen,n);

						if (c==0)
							{
							if ((zm&2)==2)	//--- Bouton droit -----
								{
								PutLIFOFct(104);
								}
								else
								{
								if (n==0)
									PutLIFOFct(84);
								}
							c=3;
							}
						}
					}
					else

					{
					int n;

					if ( (ym<DFen->yl-3) //--- Barre du milieu ---------
						& ((xm<DFen->x) | (xm>DFen->xl))
						& ((xm<DFen->Fen2->x) | (xm>DFen->Fen2->xl)) )
						{
						n=(ym-1)/3;
						if ( (n>=0) & (n<6) )
							{
							GestionFct(KKCfg->Nmenu[n]);
							Cadre(40,1+n*3,49,3+n*3,2,
											 Cfg->col[55],Cfg->col[56]);
							}
						}
					c=3;					   //--- On ne fait rien ---
					}
				}
				}
			}
			else
			{
			oldsel=-1;
			}
#endif

		DispNewFen(DFen);
		DispNewFen(DFen->Fen2);

//			   (DFen->init==1) | (DFen->Fen2->init==1) )
		if ( ((GetClock()-Cl_Start)>DFen->IDFSpeed) & (Cl_Start!=0))
			{
			if (KKCfg->isidf)
				{
				InfoIDF(DFen);// information idf sur fichier selectionn‚
				Cl_Start=0;
				}
			}

		if (Cfg->SaveSpeed!=0)
			{
			if ((GetClock()-Cl)>Cfg->SaveSpeed*CLOCKK_PER_SEC)
				{
				GestionFct(76);
				Cl=GetClock()+(int)(10*CLOCKK_PER_SEC);
				}
			}

		if (iskeypressed(SHIFT))  MenuBar(1);
			else
		if (iskeypressed(ALT))	MenuBar(2);
			else
		if (iskeypressed(CTRL))  MenuBar(3);
			else
			MenuBar(0);

		}

	car3=_bios_keybrd(0x11);

	if (c==0)
		c=Wait(0,0);

    if (c!=0) {
           // printf("Traite Commandline after dos %d FIN)\n\n",c); exit(1);
            //PrintAt(0,0,"%d",c);
    }

	KKCfg->key=GetKeyFct((ushort)c);

	if (iskeypressed(SHIFT))		//--- Left shift or right shift ----
		{
		switch (c)
			{
			case 13:	 //--- shift-ENTER -----------------------------
				c=0;
				GestionFct(85);
				break;
			case 27:	 //--- shift-escape ----------------------------
				GestionFct(102);
				c=0;
				break;
			}
		}

	//--- Positionne le pointeur sur FILE_ID.DIZ si on est sur .. ------
	switch(HI(c))
		{
		case 0x3D:	//--- F3 -------------------------------------------
		case 0x3E:	//--- F4 -------------------------------------------
		case 0x56:	//--- SHIFT-F3 -------------------------------------
		case 0x8D:	//--- CTRL-UP --------------------------------------
			if (!strcmp(DFen->F[DFen->pcur]->name,".."))
				{
				int sel,selnbr;

				sel=0;

				car=0;
				for (i=0;i<DFen->nbrfic;i++)
					{
					if (!WildCmp(DFen->F[i]->name,"FILE_ID.DIZ"))
						sel=16,selnbr=i;

					if (sel<16)
						{
						if (!WildCmp(DFen->F[i]->name,"README.1ST"))
							sel=15,selnbr=i;

					if (sel<15)
						{
						if (!WildCmp(DFen->F[i]->name,"README.TXT"))
							sel=14,selnbr=i;

					if (sel<14)
						{
						if (!WildCmp(DFen->F[i]->name,"README.*"))
							sel=13,selnbr=i;

					if (sel<13)
						{
						if (!WildCmp(DFen->F[i]->name,"*.NFO"))
							sel=12,selnbr=i;

					if (sel<12)
						{
						if (!WildCmp(DFen->F[i]->name,"*.TXT"))
							sel=11,selnbr=i;
						}

					if (sel<11)
						{
						if (!WildCmp(DFen->F[i]->name,"*.DOC"))
							sel=10,selnbr=i;
						}
						}
						}
						}
						}
					}

				if (sel!=0)
					{
					DFen->pcur=selnbr;
					DFen->scur=selnbr;
					car=1;
					}

				if (car==0)
					{
					c=0;
					WinError("Description file not found");
					}
				}
			break;

		case 0x85:	//--- F11 --------------------------------------
			GestionFct(73);
			c=0;
			break;

		}

	strcpy(KKCfg->FileName,DFen->F[DFen->pcur]->name);

//--- accessfile


//---

	}
else	//--- Retour d'un shell ----------------------------------------
	{
	FUNCTION fct;

	c=-1;
	for (i=0;i<DFen->nbrfic;i++)
		if (!WildCmp(KKCfg->FileName,DFen->F[i]->name))
			{
			DFen->pcur=i;
			DFen->scur=i;
			c=0;
			break;
			}
	if (c==-1)
		{
		WinError("Couldn't access to this file");   //--- Error --------
		InitFctStack();
		KKCfg->key=0;
		}

	fct=KKCfg->key;
	KKCfg->key=0;
	GestionFct(fct);

	c=0;
	car3=0;
	}

KKCfg->key=0;

/*--------------------------------------------------------------------*\
|- Gestion du clavier quand le pannel est ferm‚ 					  -|
\*--------------------------------------------------------------------*/
if ((DFen->FenTyp==2) & (DFen->Fen2->FenTyp==2))
	{
	switch (c)
		{
		case 0x4800: //--- UP ------------------------------------------
			GestionFct(70);
			c=0;
			break;
		case 0x5000: //--- DOWN ----------------------------------------
			c=0;
			GestionFct(93);
			break;
		case 9: //--- TAB ----------------------------------------------
			c=0;
			GestionFct(94);
			break;
		}
	}

car=LO(c);
car2=HI(c);

//-Quick search (CTRL-TAB)----------------------------------------------
if (car2==0x94)
	QuickSearch(0,&car,&car2);

/*--------------------------------------------------------------------*\
|- Pour accelerer la vitesse d'affichage des fichiers                 -|
\*--------------------------------------------------------------------*/
switch(car2)
	{
	case 72:		// GetKeyFct(72)=36
	case 80:		// GetKeyFct(80)=37
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
	case 0xE0:											// Extended code
		switch(car)
			{
			case '/':           // '/' --> Sauve ou charge une selection
				GestionFct(78);
				car=car2=0;
				break;
			}
		break;
	case 0x37:								// '*' --> Inverse selection
		GestionFct(2);
		car=car2=0;
		break;
	case 0x4E:									  // '+' --> Selectionne
		GestionFct(3);
		car=car2=0;
		break;
	case 0x4A:									// '-' --> Deselectionne
		GestionFct(4);
		car=car2=0;
		break;
	default:
//		PrintAt(0,0,"%04X",car3);
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
		{
		QuickSearch(key,&car,&car2);
		c=3;
		PutLIFOFct(GetKeyFct((ushort)(car+car2*256)));
		}
	}


//-Switch car-----------------------------------------------------------
switch (c)
	{
	case 3: //--- Rien du tout -----------------------------------------
		break;
	case 27:												   // ESCAPE
		oldhist[0]=32;
		oldhistnum=0;
		KKCfg->posinhist=512;
		if (CommandLine("\r")==0)
			if (KKCfg->Esc2Close==1)
				GestionFct(14),GestionFct(15);
		break;
	case 0:
		break;
	case 32:
		if (CommandLine(" ")==0)
			GestionFct(16);
		break;
//-Retour Switch car----------------------------------------------------
	default:									// default du switch car
		fct=GetKeyFct((ushort)c);

		if (fct==0)
			{
			CommandLine("%c",car);
			KKCfg->posinhist=512;
			oldhist[0]=32;
			oldhistnum=0;
			}
			else
			{
			GestionFct(fct);
			}

		break;
	}													  // switch(car)

if ( (QuitKKC) & (KKCfg->confexit==1) )
	{
	switch (MWinTraite(quit_kkt))
		{
		case 1:
			if (_cbuf[10]==1)
				KKCfg->confexit=0;
			break;
		default:
			QuitKKC=0;
			break;
		}
	}
}
while(!QuitKKC);
}



/*--------------------------------------------------------------------*\
|- A appeller pour chaque cas de changement de layout				  -|
\*--------------------------------------------------------------------*/
void AfficheTout(void)
{
int n;

//printf("Traite Commandline after dos %s FIN)\n\n","affichetout"); exit(1);


DFen=Fenetre[(KKCfg->FenAct)&1];

for(n=0;n<NBWIN;n++)
	{
	if (Fenetre[n]->x!=0)
		{
		Fenetre[n]->x=Cfg->TailleX-40;
		Fenetre[n]->xl=Fenetre[n]->x+39;
		}
	}

//printf("Traite Commandline after dos %d %d FIN)\n\n",Cfg->TailleX, Fenetre[0]->xl); exit(1);

if (KKCfg->isidf)
	{
	PrintAt(0,0,"%-40s%*s",RBTitle,Cfg->TailleX-40,"RedBug");
	ColLin( 0,0,40,Cfg->col[7]);
	ColLin(40,0,(Cfg->TailleX)-40,Cfg->col[11]);
	}

if	(KKCfg->isbar)								   //-- Command Line ---
	ColLin(0,KKCfg->cmdline,Cfg->TailleX,Cfg->col[63]);
	else
	ColLin(0,KKCfg->cmdline,Cfg->TailleX,FNDCOL);

DFen->init=1;
DFen->Fen2->init=1;

DFen->ChangeLine=1;

MenuBar(4);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

unsigned long crc32buf(char *buffer,int length);

/*--------------------------------------------------------------------*\
|---- Crc - 32 BIT ANSI X3.66 CRC checksum files ----------------------|
\*--------------------------------------------------------------------*/

static unsigned long int crc_32_tab[] = {	// CRC polynomial 0xedb88320
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

unsigned long crc32buf(char *buffer,int length)
{
unsigned long oldcrc32;
unsigned long crc32;
unsigned long oldcrc;
int n,c;
long charcnt;

oldcrc32 = 0xFFFFFFFF;
charcnt = 0;

for(n=0;n<length;n++)
	{
	c=buffer[n];
	++charcnt;
	oldcrc32 = (crc_32_tab[(oldcrc32^c) & 0xff]^(oldcrc32 >> 8));
	}

crc32 = oldcrc32;
oldcrc = oldcrc32 = ~oldcrc32;

return oldcrc;
}

static char *cfgbuffer;
static int cfgpos;

void cfgwopen(void);
int cfgwclose(void);				// return 1: ok, 0: error
void cfgwrite(void *,int n);

int cfgropen(void); 				// return 1: ok, 0: error
void cfgrclose(void);
void cfgread(void *,int n);

void cfgwopen(void)
{
cfgbuffer=(char*)GetMem(32768);
cfgpos=0;
}

int cfgwclose(void)
{
FILE *fic;
unsigned long crc;

fic=fopen(KKFics->CfgFile,"wb");
if (fic==NULL)
	{
	LibMem(cfgbuffer);
	return 0;
	}

crc=crc32buf(cfgbuffer,cfgpos);

fprintf(fic,"KCFG");
fwrite(&crc,4,1,fic);
fwrite(&cfgpos,4,1,fic);
fwrite(cfgbuffer,cfgpos,1,fic);
fclose(fic);

LibMem(cfgbuffer);
return 1;
}

void cfgwrite(void *buf,int n)
{
memcpy((void*)(cfgbuffer+cfgpos),buf,n);
cfgpos+=n;
}

int cfgropen(void)
{
FILE *fic;
char buffer[4];
int crc,length;


fic=fopen(KKFics->CfgFile,"rb");
if (fic==NULL) return 0;

fread(buffer,4,1,fic);
if (memcmp(buffer,"KCFG",4)!=0)
	{
	fclose(fic);
	return 0;
	}
fread(&crc,4,1,fic);
fread(&length,4,1,fic);

if (length<32768)
	{
	cfgbuffer=(char*)GetMem(32768);
	fread(cfgbuffer,length,1,fic);
	}
	else
	{
	fclose(fic);
	return 0;
	}

fclose(fic);

if (crc!=crc32buf(cfgbuffer,length))
	{
	LibMem(cfgbuffer);
	return 0;
	}

cfgpos=0;
return 1;
}

void cfgrclose(void)
{
LibMem(cfgbuffer);
}

void cfgread(void *buf,int n)
{
memcpy(buf,(void*)(cfgbuffer+cfgpos),n);
cfgpos+=n;
}


/*--------------------------------------------------------------------*\
|-						Save Configuration File 					  -|
\*--------------------------------------------------------------------*/
void SaveCfg(void)
{
int m,n,t,ns;
FENETRE *Fen;
long taille;

for (t=0;t<NBWIN;t++)
	KKCfg->FenTyp[t]=(short)(Fenetre[t]->FenTyp);


cfgwopen();


cfgwrite((void*)Cfg,sizeof(struct config));

cfgwrite((void*)KKCfg,sizeof(struct kkconfig));


for(n=0;n<16;n++)
	{
	cfgwrite(&(KKCfg->V.Mask[n]->Ignore_Case),1);
	cfgwrite(&(KKCfg->V.Mask[n]->Other_Col),1);
	taille=strlen(KKCfg->V.Mask[n]->chaine);
	cfgwrite(&taille,2);
	cfgwrite(KKCfg->V.Mask[n]->chaine,taille);
	taille=strlen(KKCfg->V.Mask[n]->title);
	cfgwrite(&taille,2);
	cfgwrite(KKCfg->V.Mask[n]->title,taille);
	}




for(t=0;t<NBWIN;t++)
	{
	Fen=Fenetre[t];

	cfgwrite(Fen->path,256);				   // Repertoire courant
	cfgwrite(&(Fen->order),sizeof(short));		 // Ordre du sorting
	cfgwrite(&(Fen->sorting),sizeof(short));	  // Type de sorting

	cfgwrite(&(Fen->nbrsel),4); 	// Nombre de fichier selectionne
	ns=Fen->nbrsel;

	for (n=0;n<Fen->nbrfic;n++)
		{
		if (Fen->F[n]->select==1)
			{
			ns--;
			m=strlen(Fen->F[n]->name);
			cfgwrite(&m,4); 					  // Longueur du nom
			cfgwrite(Fen->F[n]->name,m);	  // Fichier selectionn‚
			if (ns==0) break;
			}
		}

    if (Fen->F[Fen->pcur] != NULL) {
	    m=strlen(Fen->F[Fen->pcur]->name);
	    cfgwrite(&m,4); 							  // Longueur du nom
	    cfgwrite(Fen->F[Fen->pcur]->name,m);		  // Fichier courant
        } else {
        cfgwrite(&(Fen->F[Fen->pcur]),4);     
        }

	cfgwrite(&(Fen->scur),sizeof(short));// Pos du fichier … l'ecran
	}

cfgwrite(&NbrFunct,2);
if (NbrFunct!=0)
	cfgwrite(FctStack,2*NbrFunct);

cfgwclose();
}


/*--------------------------------------------------------------------*\
|-					   Load Configuration File						  -|
|- Retourne -1 en cas d'erreur                                        -|
|-			 0 si tout va bien										  -|
\*--------------------------------------------------------------------*/
int LoadCfg(struct config *Cfg2)
{
int m,n,i,t,nbr;
char nom[256];
short taille;
struct PourMask **Mask;
char *viewhist;

if (cfgropen()==0)
	return -1;

cfgread((void*)Cfg2,sizeof(struct config));

Cfg2->UseFont=0;

if (Cfg2->crc!=0x69)
	{
	cfgrclose();
	return -1;
	}

Mask=KKCfg->V.Mask;
viewhist=KKCfg->V.viewhist;

cfgread((void*)KKCfg,sizeof(struct kkconfig));

KKCfg->V.Mask=Mask;
KKCfg->V.viewhist=viewhist;

if ( (KKCfg->overflow1!=0) | (KKCfg->crc!=0x69) )
	{
	cfgrclose();
	return -1;
	}

for(n=0;n<16;n++)
	{
	cfgread(&(KKCfg->V.Mask[n]->Ignore_Case),1);
	cfgread(&(KKCfg->V.Mask[n]->Other_Col),1);
	cfgread(&taille,2);
	cfgread(KKCfg->V.Mask[n]->chaine,taille);
	KKCfg->V.Mask[n]->chaine[taille]=0;
	cfgread(&taille,2);
	cfgread(KKCfg->V.Mask[n]->title,taille);
	KKCfg->V.Mask[n]->title[taille]=0;
	}

for (t=0;t<NBWIN;t++)
	{
	DFen=Fenetre[t];

	DFen->FenTyp=KKCfg->FenTyp[t];

	cfgread(nom,256);						   // Repertoire courant
	cfgread(&(DFen->order),sizeof(short));		 // Ordre du sorting
	cfgread(&(DFen->sorting),sizeof(short));	  // Type de sorting

	IOerr=1;	//--- On ignore les erreurs de disques -----------------

	strcpy(DFen->path,Fics->LastDir);

	CommandLine("#cd %s",nom);

	IOerr=0;
	IOver=0;

	cfgread(&nbr,4);				// Nombre de fichier selectionne

	DFen->nbrsel=0;

	for (i=0;i<nbr;i++)
		{
		cfgread(&m,4);							  // Longueur du nom
		cfgread(nom,m); 					  // Fichier selectionn‚
		nom[m]=0;

		for (n=0;n<DFen->nbrfic;n++)
			if (!stricmp(nom,DFen->F[n]->name))
				FicSelect(DFen,n,1);
		}


	cfgread(&m,4);								  // Longueur du nom
	cfgread(nom,m); 							  // Fichier courant
	nom[m]=0;

	cfgread(&(DFen->scur),sizeof(short)); //Pos du fichier … l'ecran

	for (n=0;n<DFen->nbrfic;n++)
		if (!stricmp(nom,DFen->F[n]->name))
			DFen->pcur=n;
	}

cfgread(&NbrFunct,2);
if (NbrFunct!=0)
	cfgread(FctStack,2*NbrFunct);

cfgrclose();

return 0;
}

/*--------------------------------------------------------------------*\
|- Affichage de la fenetre du milieu								  -|
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
	for(y=Fenetre[0]->y;y<=Fenetre[0]->yl;y++)
		{
		nscr=(y+Fenetre[0]->y)*160+x1*2;
		nbuf=(y-((KKCfg->cmdline)-1)+MaxY-1)*MaxX+x1;

		for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
			if (nbuf<0)
				AffCol(x+x1,y,FNDCOL);
			else
				AffCol(x+x1,y,Screen_Buffer[nbuf*2+1]);
		}
	for(y=Fenetre[0]->y;y<=Fenetre[0]->yl;y++)
		{
		nscr=(y+Fenetre[0]->y)*160+x1*2;
		nbuf=(y-((KKCfg->cmdline)-1)+MaxY-1)*MaxX+x1;

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

	if	(!(KKCfg->isidf))
		ColLin(x1,0,10,Cfg->col[17]);

	for (x=0;x<6;x++)
		{
		if (3+x*3<Fenetre[0]->yl-3)
			{
			Cadre(x1, 1+x*3,x2, 3+x*3,2,Cfg->col[55],Cfg->col[56]);
			ColLin(x1+1,2+x*3,8,Cfg->col[17]);
			memcpy(chaine,KKCfg->Qmenu+x*8,8);
			PrintAt(x1+1,2+x*3 ,chaine);
			}
		}

	Window(x1,19,x2,Fenetre[0]->yl-3,Cfg->col[56]); 	// Efface le reste ---

	Cadre(x1,Fenetre[0]->yl-2,x2,Fenetre[0]->yl,2,Cfg->col[55],Cfg->col[56]);
	ColLin(x1+1,Fenetre[0]->yl-1,8,Cfg->col[17]); //- Emplacement de l'heure -
	}

}

void Screen2Buffer(char *Screen,char *Buffer)
{
int i,j,n;

n=0;
for(i=0;i<MaxX*MaxY;i++)
	{
	Buffer[n]=177;	n++;
	Buffer[n]=7;	n++;
	}

n=0;
for(i=0;i<OldCfg->TailleY;i++)
	for(j=0;j<(OldCfg->TailleX)*2;j++,n++)
		Buffer[(i+MaxY-(OldCfg->TailleY))*MaxX*2+j]=Screen[n];

SPosX=PosX;
SPosY=PosY+MaxY-(OldCfg->TailleY);
}

void Buffer2Screen(char *Buffer,char *Screen)
{
int i,j,n;

n=0;
for(i=0;i<OldCfg->TailleY;i++)
	for(j=0;j<(OldCfg->TailleX)*2;j++,n++)
		Screen[n]=Buffer[(i+MaxY-(OldCfg->TailleY))*MaxX*2+j];

PosX=SPosX;
PosY=SPosY+(OldCfg->TailleY)-MaxY;
}

void HelpHandler(void);
void HelpTopicHandler(char *topic);

static int inhelp=0;

void HelpHandler(void)
{
HelpTopicHandler("index");
}

void HelpTopicHandler(char *topic)
{
char buf[256];
char old;

if (inhelp)
	return;

inhelp=1;

old=KKCfg->V.ajustview;
KKCfg->V.ajustview=1;

strcpy(buf,Fics->path);
Path2Abs(buf,"kkc.htm#");
strcat(buf,topic);

View(&(KKCfg->V),buf,0);

KKCfg->V.ajustview=old;

inhelp=0;
}


/*--------------------------------------------------------------------*\
 -																	  -
 -	   ----------------------------------------------------------	  -
 -	   -						 MAIN							-	  -
 -	   ----------------------------------------------------------	  -
 -																	  -
\*--------------------------------------------------------------------*/

int main(int argc,char **argv)
{
char *path;
int n;
char *LC;
char startfromkkc;
int oldx,oldy;

/*--------------------------------------------------------------------*\
|- Initiation generale de hard.cc									  -|
\*--------------------------------------------------------------------*/
Redinit();

KKInit();

OldCfg=Cfg;

/*--------------------------------------------------------------------*\
|-					   Initialisation de l'ecran                      -|
\*--------------------------------------------------------------------*/

ReadCfg(OldCfg);		   //--- Lit la config courrante ---------------

NewCfg=(struct config*)GetMem(sizeof(struct config));

InitScreen(0);

/*--------------------------------------------------------------------*\
|- Sauvegarde des donn‚es ‚crans									  -|
\*--------------------------------------------------------------------*/

Screen_Buffer=(char*)GetMem(MaxX*MaxY*2);			// maximum 90x50

oldx=Cfg->TailleX;
oldy=Cfg->TailleY;

WhereXY(&PosX,&PosY);

Scr2Buf(OldScr);
Screen2Buffer(OldScr,Screen_Buffer);

#ifndef NOFONT
Font2Buf(OldFont);
#endif

Cfg->col[7]=7;
// Information("Starting Ketchup Killers Commander");

/*--------------------------------------------------------------------*\
|-						 Initialise les buffers 					  -|
\*--------------------------------------------------------------------*/

KKCfg=(struct kkconfig*)GetMem(sizeof(struct kkconfig));

for(n=0;n<NBWIN;n++)
	{
	Fenetre[n]=(FENETRE*)GetMem(sizeof(FENETRE));
	Fenetre[n]->F=(struct file**)GetMem(TOTFIC*sizeof(void *));
	}

KKFics=(struct kkfichier*)GetMem(sizeof(struct kkfichier));

LoadHelpHandler(HelpHandler);
LoadHelpTopicHandler(HelpTopicHandler);

KKCfg->V.Mask=(struct PourMask**)GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
	KKCfg->V.Mask[n]=(struct PourMask*)GetMem(sizeof(struct PourMask));

path=(char*)GetMem(256);

/*--------------------------------------------------------------------*\
|-				  Lecture et verification des arguments 			  -|
\*--------------------------------------------------------------------*/

strcpy(path,*argv);

Path2Abs(path,"..");

LC=*(argv+argc-1);

startfromkkc=(strncmp(LC,"6969",4)==0);

/*
if (!startfromkkc)
	{
	printf("This program cannot be run in DOS mode\n");
	printf("This program cannot be run in WINDOWS mode\n");
	printf("This program cannot be run in OS/2 mode\n");
	printf("This program cannot be run in LINUX mode\n\n");
	printf("This program required KK.EXE\n\n");
	exit(1);
	}
*/


/*--------------------------------------------------------------------*\
|-							Gestion des erreurs 					  -|
\*--------------------------------------------------------------------*/
LoadErrorHandler();

#ifdef __WC32__
signal(SIGBREAK,Signal_Handler);
#endif

#ifndef LINUX
signal(SIGABRT,Abort_Handler);
signal(SIGFPE,Signal_Handler);
signal(SIGILL,Signal_Handler);
signal(SIGINT,Signal_Handler);
signal(SIGSEGV,Signal_Handler);
signal(SIGTERM,Signal_Handler);
#endif

/*--------------------------------------------------------------------*\
|-						Initialisation des fichiers 				  -|
\*--------------------------------------------------------------------*/

SetDefaultPath(path);
SetDefaultKKPath(path);

/*--------------------------------------------------------------------*\
|-								 Default							  -|
\*--------------------------------------------------------------------*/

#ifndef NO4DOS
KKCfg->_4dos=(char)_4DOSverif();
#else
KKCfg->_4dos=0;
#endif

memset(KKCfg->HistDir,0,256);


/*--------------------------------------------------------------------*\
|-							  Initialisation						  -|
\*--------------------------------------------------------------------*/

CreateNewFen(Fenetre[0],0);
Fenetre[0]->x=0;
Fenetre[0]->nfen=0;
Fenetre[0]->Fen2=Fenetre[1];

CreateNewFen(Fenetre[1],0);
Fenetre[1]->x=40;
Fenetre[1]->nfen=1;
Fenetre[1]->Fen2=Fenetre[0];

CreateNewFen(Fenetre[2],0); 			   // Fenˆtre ferm‚e par default
Fenetre[2]->x=40;
Fenetre[2]->nfen=2;
Fenetre[2]->Fen2=Fenetre[2];


for (n=0;n<NBWIN;n++)
	{
	Fenetre[n]->y=1;
	Fenetre[n]->xl=Fenetre[n]->x+39;

	Fenetre[n]->order=17;
	Fenetre[n]->pcur=0;
	Fenetre[n]->scur=0;
	}

ChangeType(4);

/*--------------------------------------------------------------------*\
|-				  Chargement du fichier config (s'il existe)          -|
\*--------------------------------------------------------------------*/

#ifndef NOSAVE

{
char old[256];
char cont;

KKCfg->nosave=0;

strcpy(old,KKFics->trash);
Path2Abs(old,"kkrb.old");

if (LoadCfg(NewCfg)==-1)  //---------------- Erreur! -------------------
	{
	unlink(KKFics->CfgFile);
	rename(old,KKFics->CfgFile);
	if (LoadCfg(NewCfg)==-1)   //----------- Erreur! -------------------
		cont=0;
		else
		cont=1;
	}
	else
	{
	unlink(old);
	if (rename(KKFics->CfgFile,old)!=0) //--- Erreur! ------------------
		cont=0;
		else
		cont=1;
	}


if (!cont)	//--- Erreur! On verifie que le support n'est pas proteg‚ --
	{
	char buffer[256];
	FILE *fic;

	IOerr=0;
	for (n=0;n<NBWIN;n++)
		{
		DFen=Fenetre[n];
		strcpy(DFen->path,"c:\\");
		CommandLine("#cd %s",DFen->path);
		DFen->pcur=0;
		}
	IOver=0;

	DFen=Fenetre[0];

	remove(KKFics->CfgFile);
	saveconfig=0;

	DefaultKKCfg();
	DefaultCfg(NewCfg);

	strcpy(buffer,Fics->path);
	Path2Abs(buffer,"kktest.rb");

	fic=fopen(buffer,"wb");
	if (fic!=NULL)
		{
		fclose(fic);

		remove(buffer);

		strcpy(buffer,Fics->path);
		Path2Abs(buffer,"kksetup.exe");
		fic=fopen(buffer,"rb");
		if (fic!=NULL)
			{
			fclose(fic);
			GestionFct(62); //--- On lance KKsetup ---------------------
			}
		}

	Information("No save");
	KKCfg->nosave=1;
	}
}

#else

// passe bien ici
//

IOerr=0;
for (n=0;n<NBWIN;n++)
	{
	DFen=Fenetre[n];
#ifdef __WC32__
	strcpy(DFen->path,"c:\\");
#else
    strcpy(DFen->path,"/");
#endif
	CommandLine("#cd %s",DFen->path);
	DFen->pcur=0;
	}
IOver=0;

DFen=Fenetre[0];
saveconfig=0;

//printf("ok\n\n"); exit(1);

DefaultKKCfg();
DefaultCfg(NewCfg);

startfromkkc = 0;

#endif

//printf("ok\n\n"); exit(1); // ne passe pas ici

Cfg=NewCfg;

if (startfromkkc)
	KKCfg->internshell=0;
	else
	KKCfg->internshell=1;


if ((KKCfg->KeyAfterShell) & (LC[4]=='0'))
	{
	PrintAt(0,(OldCfg->TailleY)-1,"Press a key to continue to return in "RBTitle);
	ColLin(0,(OldCfg->TailleY)-1,OldCfg->TailleX,Cfg->col[4]);
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

	fread((void*)OldCfg,1,sizeof(struct config),fic);

	fread(&PosX,1,sizeof(PosX),fic);
	fread(&PosY,1,sizeof(PosY),fic);

	fread(OldScr,1,MaxZ,fic);
	Screen2Buffer(OldScr,Screen_Buffer);

	fread(OldFont,256*16,1,fic);

	fclose(fic);
	}
	else
	{
	OldCfg->TailleX=oldx;
	OldCfg->TailleY=oldy;
	}


/*--------------------------------------------------------------------*\
|- verification system												  -|
\*--------------------------------------------------------------------*/
#ifndef NOWIN95
KKCfg->_Win95=Verif95();
#else
KKCfg->_Win95=0;
#endif

/*
 if (KKCfg->_Win95==1)
	SetWindowsTitle();	//--- Change le titre de l'application ---------
*/

#ifndef NOMOUSE
// Information("Mouse Init");
InitMouse();
#endif

/*--------------------------------------------------------------------*\
|- Chargement du repertoire courant si necessaire					  -|
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

InitScreen(Cfg->display);
UseCfg();					   // Emploi les parametres de configuration


if (KKCfg->verifhist)
	VerifHistDir(); 		   // Verifie l'history pour les repertoires

if (KKCfg->firsttime)
	{
	RunFirstTime();
	KKCfg->firsttime=0;
	}


/*--------------------------------------------------------------------*\
|- Sauvegarde des touches											  -|
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

RunAutoMacro();

//TitleBox("Ketchup Killers Commander");

WinShellColor(Cfg->col[0]);

Gestion();

/*--------------------------------------------------------------------*\
|-									FIN 							  -|
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
	  {30,5,5,"  Save/View  ",NULL},
	  {50,5,5,"    Save     ",NULL},
	  { 2,2,0,"File to use: ",NULL},
	  { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,5,74,12,"Background"};

strcpy(Dir,DFen->path);
Path2Abs(Dir,"KKSCREEN.RAW");

n=WinTraite(T,6,&F,1);

if (n==-1) return;											   // ESCape
if (T[n].type==3) return;									   // Cancel

//if (n==1)    strcpy(Dir,KKFics->temp);

strcpy(nom,Dir);
strcpy(Dir,DFen->path);
Path2Abs(Dir,nom);

if ((n==2) | (n==3))
	{
	fic=fopen(Dir,"wb");
	if (fic==NULL)
		{
		WinError("File couldn't be created");
		return;
		}
	Buffer2Screen(Screen_Buffer,OldScr);
	fwrite(OldScr,(OldCfg->TailleX)*(OldCfg->TailleY)*2,1,fic);
	fclose(fic);
	}
if ((n==1) | (n==2))
	View(&(KKCfg->V),Dir,5);
}


void ViewBKGPage(void)
{
FILE *fic;
char Dir[256];

strcpy(Dir,KKFics->trash);
Path2Abs(Dir,"KKSCREEN.RAW");

fic=fopen(Dir,"wb");
if (fic==NULL)
	{
	WinError("File couldn't be created");
	return;
	}
Buffer2Screen(Screen_Buffer,OldScr);
fwrite(OldScr,(OldCfg->TailleX)*(OldCfg->TailleY)*2,1,fic);
fclose(fic);

View(&(KKCfg->V),Dir,5);
}


void CmpDirectory(void)
{
int n,i,j;
struct file *F1,*F2;

if (!(Info->macro))
	{
	_cbuf[10]=1;	//--- Compare size
	_cbuf[11]=1;	//--- Compare date
	_cbuf[12]=0;	//--- compare attrib
	_cbuf[13]=0;	//--- compare contents

	_cbuf[14]=1;	//--- 1: selection fichier, 2: deselectionne fichier

	_cbuf[15]=1;	//--- Select dir
	}


n=MWinTraite(dircmp_kkt);
if (n==-1) return;

for(i=0;i<DFen->nbrfic;i++)
	{
	n=-1;

	F1=DFen->F[i];

	for(j=0;j<DFen->Fen2->nbrfic;j++)
		if (!WildCmp(F1->name,DFen->Fen2->F[j]->name))
			n=j;

	if (n!=-1)
		{
		F2=DFen->Fen2->F[n];

		if ( (_cbuf[10]==1) & ((F1->size)!=(F2->size)) )
			n=-1;

		if ( (_cbuf[11]==1) &
			   (((F1->date)!=(F2->date)) | ((F1->time)!=(F2->time)))  )
			n=-1;

		if ( (_cbuf[12]==1) & ((F1->attrib)!=(F2->attrib)) )
			n=-1;
		}

	if ((IsDir(F1)) & (_cbuf[15]==1))
		n=0;

	if (n==-1)
		{
		if (_cbuf[14]==1)
			FicSelect(DFen,i,1);
			else
			FicSelect(DFen,i,0);
		}
	}
}


/*--------------------------------------------------------------------*\
|-								DOS Shell							  -|
\*--------------------------------------------------------------------*/
void Shell(char *string,...)
{
char sortie[256];
va_list arglist;
char *suite;
int ret;
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

if (strlen(suite+1)>120)	//-- La ligne de commande est trop grande --
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

//	  WinError("command line too high");

	strcpy(suite+1,filename);
	}

// if (KKCfg->KeyAfterShell==0)
suite[0]='#';

memcpy(SpecSortie,suite,256);

//printf("Traite Commandline %s %d FIN)\n\n",suite,KKCfg->internshell); exit(1);

if (KKCfg->internshell)
	{
	int x,y;

	DriveSet(DFen->path);

	if (KKCfg->scrrest)
		{
		x=Cfg->TailleX;
		y=Cfg->TailleY;

		Cfg->TailleX=OldCfg->TailleX;
		Cfg->TailleY=OldCfg->TailleY;

		SetMode();

		Buffer2Screen(Screen_Buffer,OldScr);
		Buf2Scr(OldScr);

#ifndef NOFONT
		Buf2Font(OldFont);
#endif
		Buf2Pal(OldCfg->palette);

		GotoXY(0,PosY);
		}

//printf("Traite Commandline %s %d FIN)\n\n",suite,KKCfg->internshell); exit(1);
	ret=system(suite+1);

	Buffer_Clr();

	if (KKCfg->scrrest)
		{
		Scr2Buf(OldScr);
		Screen2Buffer(OldScr,Screen_Buffer);

		Cfg->TailleX=x;
		Cfg->TailleY=y;
		}

	GestionFct(67);

	if (ret!=0)
		WinError(suite+1),
		WinError("System couldn't be loaded");

	}
	else
	Fin();

}





/*--------------------------------------------------------------------*\
|-						  Fin d'execution                             -|
\*--------------------------------------------------------------------*/
void Fin(void)
{

IOerr=1;	//--- On ignore les prochaines erreurs hard ----------------

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

//printf("FIN2\n\n"); exit(1);

DriveSet(DFen->path);

Cfg->reinit=(*SpecSortie==0);

if (KKCfg->scrrest) // & (saveconfig) )
	{
	Cfg=OldCfg;

	SetMode();							// Retablit le mode texte normal

	Buffer2Screen(Screen_Buffer,OldScr);
	Buf2Scr(OldScr);
#ifndef NOFONT
	Buf2Font(OldFont);
#endif
	Buf2Pal(OldCfg->palette);
	}
	else
	{
	FILE *fic;

	fic=fopen(KKFics->ficscreen,"wb");
	if (fic!=NULL)
		{
		fwrite((void*)OldCfg,1,sizeof(struct config),fic);

		fwrite(&PosX,1,sizeof(PosX),fic);
		fwrite(&PosY,1,sizeof(PosY),fic);

		Buffer2Screen(Screen_Buffer,OldScr);
		fwrite(OldScr,MaxZ,1,fic);

		fwrite(OldFont,256*16,1,fic);

		fclose(fic);
		}
	}

//Cfg=OldCfg;
//UseCfg();

if ( (KKCfg->scrrest==0) & (saveconfig) )
	GotoXY(0,0); //-- Empeche l'affichage de la path-err command.com ---
else
	GotoXY(0,PosY);

// if (KKCfg->internshell) return;

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

Cfg=NewCfg;
DesinitScreen();


exit(1);
}


FENETRE *AllocWin(void)
{
FENETRE *SFen;

SFen=(FENETRE*)GetMem(sizeof(FENETRE));

CreateNewFen(SFen,0);

SFen->F=(struct file**)GetMem(TOTFIC*sizeof(void *));

SFen->x=40;
SFen->nfen=7;
SFen->Fen2=SFen;
SFen->y=1;
SFen->yl=(Cfg->TailleY)-4;
SFen->xl=39;
SFen->order=17;
SFen->pcur=0;
SFen->scur=0;

SFen->nbrfic=0;

SFen->OldDFen=DFen;

return SFen;
}

void FreeWin(FENETRE *SFen)
{
int n;

DFen=SFen->OldDFen;

for(n=0;n<SFen->nbrfic;n++)
	{
	if (SFen->F[n]->info!=NULL)
		LibMem(SFen->F[n]->info);
	if (SFen->F[n]->longname!=NULL)
		LibMem(SFen->F[n]->longname);

	LibMem(SFen->F[n]->name);
	LibMem(SFen->F[n]);
	}

LibMem(SFen->F);

CloseNewFen(SFen);

LibMem(SFen);
}

/*--------------------------------------------------------------------*\
|- Gestion de la fenˆtre ext‚rieure 								  -|
\*--------------------------------------------------------------------*/

char _shellcolor;
char _shellmore,_shellmoretype=0;

void WinShellInitMore(char a)
{
_shellmore=0;
_shellmoretype=a;
}

void WinShellCloseMore(void)
{
_shellmoretype=0;
}


void NextLine(void)
{
int x,y,n;
char col;

if (_shellmoretype!=0)
	{
	_shellmore++;
	if (_shellmore==KKCfg->shelly)
		{
		WinShellRefresh();

		KKWin Win;

		ColLin(0,KKCfg->cmdline,KKCfg->shellx,Cfg->col[2]);
		ChrLin(0,KKCfg->cmdline,KKCfg->shellx,32);
		PrintAt(0,KKCfg->cmdline,"Press ESC to quit or another key to continue...");
		Wait(0,0);
		_shellmore=0;
		}
	}


SPosY++;
SPosX=0;

if (SPosY==MaxY)
	{
	SPosY--;

	for(y=MaxY-(KKCfg->shelly);y<MaxY-1;y++)
		{
		n=y*MaxX*2;
		for(x=0;x<(KKCfg->shellx)*2;x++)
			{
			Screen_Buffer[n]=Screen_Buffer[n+MaxX*2];
			n++;
			}
		}

	n=(MaxY-1)*MaxX*2;

	col=Screen_Buffer[n-1];

	for(x=0;x<KKCfg->shellx;x++)
		{
		Screen_Buffer[n]=32;
		Screen_Buffer[n+1]=_shellcolor;
		n+=2;
		}
	}
}

void WinShellClearLine(void)
{
int n,x;

SPosX=0;

n=SPosY*MaxX*2;

for(x=0;x<KKCfg->shellx;x++)
		{
		Screen_Buffer[n]=32;
		Screen_Buffer[n+1]=_shellcolor;
		n+=2;
		}
}

void WinShellColor(char col)
{
_shellcolor=col;
}


void WinShellPrint(char *string,...)
{
char Buffer[256];
va_list arglist;
char *suite;
int n;

suite=Buffer;

va_start(arglist,string);
vsprintf(Buffer,string,arglist);
va_end(arglist);

n=SPosY*MaxX*2+SPosX*2;

while (*suite!=0)
	{
	switch(*suite)
		{
		case '\r':
			WinShellClearLine();
			break;

		case '\n':
			NextLine();
			n=SPosY*MaxX*2+SPosX*2;
			break;
		default:
			Screen_Buffer[n]=*suite;
			Screen_Buffer[n+1]=_shellcolor;
			n+=2;
			SPosX++;
			break;
		}
	suite++;
	}
}

void WinShellPrCol(char *col,char *string,...)
{
char Buffer[256];
va_list arglist;
char *suite,*scol;
int n;

suite=Buffer;
scol=col;

va_start(arglist,string);
vsprintf(Buffer,string,arglist);
va_end(arglist);

n=SPosY*MaxX*2+SPosX*2;

while (*suite!=0)
	{
	switch(*suite)
		{
		case '\r':
			WinShellClearLine();
			break;

		case '\n':
			NextLine();
			n=SPosY*MaxX*2+SPosX*2;
			break;
		default:
			Screen_Buffer[n]=*suite;
			Screen_Buffer[n+1]=*scol;
			n+=2;
			SPosX++;
			break;
		}
	suite++;
	scol++;
	}
}

void WinShellRefresh(void)
{
int i;

if (Fenetre[1]->FenTyp==0)
	for (i=1;i>=0;i--)
		InitDispNewFen(Fenetre[i]);
	else
	for (i=0;i<2;i++)
		InitDispNewFen(Fenetre[i]);
}


void WinShellClrScr(void)
{
int i,j;

for(i=0;i<KKCfg->shelly;i++)
	for(j=0;j<(KKCfg->shellx);j++)
		{
		Screen_Buffer[(i+MaxY-(KKCfg->shelly))*MaxX*2+j*2]=32;
		Screen_Buffer[(i+MaxY-(KKCfg->shelly))*MaxX*2+j*2+1]=Cfg->col[0];
		}

SPosX=0;
SPosY=MaxY-(KKCfg->shelly);
}


void Information(char *s)
{
ColLin( 0,0,Cfg->TailleX,Cfg->col[7]);
PrintAt(0,0,"%-*s",Cfg->TailleX,s);
}

void About(void)
{
WinMesg("About",RBTitle" /RedBug",2);
}

void CalcSizeWin(FENETRE *Fen)
{
int sb;

sb=1+(KKCfg->pathdown);

if (KKCfg->isbar)
	KKCfg->cmdline=(char)(Cfg->TailleY-2);
	else
	KKCfg->cmdline=(char)(Cfg->TailleY-1);

KKCfg->shellx=(char)(MIN(OldCfg->TailleX,Cfg->TailleX));
KKCfg->shelly=(char)(MIN(OldCfg->TailleY,Cfg->TailleY-1));


if (KKCfg->isidf)
	Fen->y=1;
	else
	Fen->y=0;

if (KKCfg->sizewin==0)
	Fen->yl=(KKCfg->cmdline)-1;
	else
	Fen->yl=Fen->y+KKCfg->sizewin-1;

if (Fen->yl>(KKCfg->cmdline)-1)
	Fen->yl=(KKCfg->cmdline)-1;

switch (Cfg->windesign)
	{
	case 1:
	case 4:
		Fen->y2=Fen->y+1;
		Fen->yl2=Fen->yl-Fen->y-sb-3;
		break;
	case 2:
	case 3:
		Fen->y2=Fen->y;
		Fen->yl2=Fen->yl-Fen->y-sb-2;
	}
}


void RunDFenMacro(char *path,char *file)
{
FENETRE *OldFen;

OldFen=DFen;

if (DFen->nfen>=2)
	DFen=Fenetre[0];

RunMacro(path,file);

DFen=OldFen;
}

/*--------------------------------------------------------------------*\
|- Gestion des fenetres 											  -|
\*--------------------------------------------------------------------*/


void CreateNewFen(FENETRE *Fen,int n)
{
Fen->FenTyp=n;
Fen->init=1;
}

void CloseNewFen(FENETRE *Fen)
{
Fen->FenTyp=0;
Fen->init=1;
}

void InitDispNewFen(FENETRE *Fen)
{
Fen->Info.delay=GetClock();
}

void DispNewFen(FENETRE *Fen)
{
int x,y;

int nbuf,nscr;

int x1,x2;

if ( (Fen->init==1) | (Fen->FenTyp==2) )
	AffLonger();

switch(Fen->FenTyp)
	{
	case 0: 	 // 1 FOIS
		if (Fen->Info.delay==0)
			break;

		FenNor(Fen);
		Fen->Info.delay=0;
		break;

	case 1: 	 // X FOIS
		if ((GetClock()-Fen->Info.delay)<=DFen->IDFSpeed)
			break;

		FenDIZ(Fen);
		Fen->Info.delay=GetClock();
		break;

	case 2: 	 // UNE FOIS
		if (Fen->Info.delay==0)
			break;

		x1=Fen->x;
		x2=Fen->xl;

		for(y=Fen->y;y<=Fen->yl;y++)
			{
			nscr=(y+Fen->y)*160+(Fen->x)*2;
			nbuf=(y-((KKCfg->cmdline)-1)+MaxY-1)*MaxX+(Fen->x);

			for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
			if (nbuf<0)
				AffCol(x+x1,y,FNDCOL);
			else
				AffCol(x+x1,y,Screen_Buffer[nbuf*2+1]);
			}
		for(y=Fen->y;y<=Fen->yl;y++)
			{
			nscr=(y+Fen->y)*160+(Fen->x)*2;
			nbuf=(y-((KKCfg->cmdline)-1)+MaxY-1)*MaxX+(Fen->x);

			for(x=0;x<x2-x1+1;x++,nbuf++,nscr++)
			if (nbuf<0)
			  AffChr(x+x1,y,32);
			else
			  AffChr(x+x1,y,CnvASCII(0,Screen_Buffer[nbuf*2]));
			}

		Fen->Info.delay=0;
		break;
	case 3: 	 // 1 FOIS
		if (Fen->Info.delay==0)
			break;

		FenInfo(Fen);
		Fen->Info.delay=0;
		break;
	case 4: 	 // TJS
		FenDisk(Fen);
		break;

	case 5: 	 // X FOIS
		if ((GetClock()-Fen->Info.delay)<=DFen->IDFSpeed)
			break;

		FenFileID(Fen);
		Fen->Info.delay=GetClock();
		break;
	}

if ((Fen->nfen==1) & (Fen->yl<(KKCfg->cmdline)-1))
	{
	x2=Cfg->TailleX;

	for(y=Fen->yl+1;y<KKCfg->cmdline;y++)
		{
		nscr=(y+Fen->y)*160;
		nbuf=(y-((KKCfg->cmdline)-1)+MaxY-1)*MaxX;

		for(x=0;x<x2;x++,nbuf++,nscr++)
		if (nbuf<0)
			AffCol(x,y,FNDCOL);
		else
			AffCol(x,y,Screen_Buffer[nbuf*2+1]);
		}
	for(y=Fen->yl+1;y<KKCfg->cmdline;y++)
		{
		nscr=(y+Fen->y)*160;
		nbuf=(y-((KKCfg->cmdline)-1)+MaxY-1)*MaxX;

		for(x=0;x<x2;x++,nbuf++,nscr++)
		if (nbuf<0)
			AffChr(x,y,32);
		else
			AffChr(x,y,CnvASCII(0,Screen_Buffer[nbuf*2]));
		}
	}


}

