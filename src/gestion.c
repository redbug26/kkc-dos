#include <time.h>           // Heure presente

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <bios.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include <dos.h>

#include "gestion.h"

#include "kk.h"

#include "win.h"

void GetFreeMem(char *buffer);
#pragma aux GetFreeMem = \
    "mov ax,0500h" \
	"int 31h" \
	parm [edi];

void PutInHistDir(void);

// 1-> erreur
// 0-> OK
int FicSelect(int n,char q)
{
if (DFen->F[n]->name[0]=='.') return 1;
if (DFen->F[n]->name[1]==':') return 1;

if ((DFen->F[n]->attrib & _A_VOLID)==_A_VOLID) return 1;

switch(q)
    {
    case 0: // Deselect
        if (DFen->F[n]->select==1)
            {
            DFen->F[n]->select=0;
            DFen->taillesel-=DFen->F[n]->size;
            DFen->nbrsel--;
            }
        break;
    case 1: // Select
        if (DFen->F[n]->select==0)
            {
            DFen->F[n]->select=1;
            DFen->taillesel+=DFen->F[n]->size;
            DFen->nbrsel++;
            }
        break;
    case 2: // inverse selection
        if (DFen->F[n]->select==0)
            {
            DFen->F[n]->select=1;
            DFen->taillesel+=DFen->F[n]->size;
            DFen->nbrsel++;
            }
            else
            {
            DFen->F[n]->select=0;
            DFen->taillesel-=DFen->F[n]->size;
            DFen->nbrsel--;
            }
        break;
    }

return 0;
}

// Comparaison entre un nom et un autre
int Maskcmp(char *src,char *mask)
{
int n;

for (n=0;n<strlen(mask);n++)
    {
    if ( (src[n]=='/') & (mask[n]=='/') ) continue;
    if ( (src[n]=='\\') & (mask[n]=='/') ) continue;
    if ( (src[n]=='\\') & (mask[n]=='\\') ) continue;
    if ( (src[n]=='/') & (mask[n]=='\\') ) continue;
    if (toupper(src[n])==toupper(mask[n])) continue;

    return 1;
    }
return 0;
}

// Envoie le nom du fichier src dans dest si le mask est bon
// Renvoit 1 dans ce cas, sinon renvoie 0
int find1st(char *src,char *dest,char *mask)
{
int i;

if (!Maskcmp(src,mask)) {
    if ( (src[strlen(mask)]=='\\') | (src[strlen(mask)]=='/') )
        memcpy(dest,src+strlen(mask)+1,strlen(src+strlen(mask)));
        else
        memcpy(dest,src+strlen(mask),strlen(src+strlen(mask))+1);
    for (i=0;i<strlen(dest);i++)
        {
        if (dest[i]=='\\') return 0;
        if (dest[i]=='/') return 0;
        }
    return 1;
    }

return 0;
}


// Comparaison avec WildCards
//----------------------------

int WildCmp(char *a,char *b)
{
short p1=0,p2=0;
int ok;
char c1,c2,c3;

ok=0;

while (1)
    {
    c1=a[p1];
    c2=b[p2];
    c3=b[p2+1];

    if ( (c1>='a') & (c1<='z') ) c1-=32;
    if ( (c2>='a') & (c2<='z') ) c2-=32;
    if ( (c3>='a') & (c3<='z') ) c3-=32;

    if ( ((c1==0) & (c2==0)) |
         ((c1==0) & (c2=='*') & (c3==0)) |
         ((c1==0) & (c2=='*') & (c3=='.') & (b[p2+2]=='*')) |
         ((c1==0) & (c2=='.') & (c3=='*'))
         )
        break;

    if (c1==0)
        {
        ok=1;
        break;
        }

    if ( (c1==c2) | (c2=='?') )
        {
        p1++;
        p2++;
        continue;
        }
    if  (c2=='*')
        {
        if (c1==c3)
            p2+=2;
        p1++;
        continue;
        }

    ok=1;
    break;
    }

return ok;
}

// Give the name include in a path
// E.G. c:\kkcom\kkc.c --> "kkc.c"
//      c:\            --> ""
//      c:\kkcom       --> "kkcom"

char *FileinPath(char *p,char *Ficname)
{
int n;
char *s;

for (n=0;n<strlen(p);n++)
    if ( (p[n]=='\\') | (p[n]=='/') )
        s=p+n+1;

strcpy(Ficname,s);

return s;
}

// Make absolue path from old path and ficname
//---------------------------------------------
void Path2Abs(char *p,char *Ficname)
{
int n;
static char old[256];     // Path avant changement

memcpy(old,p,256);

if (p[strlen(p)-1]=='\\') p[strlen(p)-1]=0;

if ( (!strcmp(Ficname,"..")) & (p[0]!=0) ) {
    for (n=strlen(p);n>0;n--)
        if (p[n]=='\\') {
            p[n]=0;
            break;
            }
    if (p[strlen(p)-1]==':') strcat(p,"\\");
    return;
    }

if (Ficname[0]!='.') {
    if  ( (Ficname[1]==':') & (Ficname[2]=='\\') )  {
        strcpy(p,Ficname);
        if (p[strlen(p)-1]==':') strcat(p,"\\");
        return;
        }

    if (Ficname[0]=='\\')  {
        strcpy(p+2,Ficname);
        if (p[strlen(p)-1]==':') strcat(p,"\\");
        return;
        }

    if (p[strlen(p)-1]!='\\') strcat(p,"\\");
    strcat(p,Ficname);
   }

if (p[strlen(p)-1]==':') strcat(p,"\\");
}

void ChangeDir(char *Ficname)
{
static char nom[256];
static char old[256];     // Path avant changement
int n;
int err;
char *p;

p=DFen->path;
memcpy(old,p,256);

for(n=0;n<strlen(Ficname);n++)
    if (Ficname[n]=='/') Ficname[n]='\\';

if (!strcmp(Ficname,"..")) {
    for (n=strlen(p);n>0;n--)
        if (p[n]=='\\') break;
    strcpy(nom,p+n+1);
    }

Path2Abs(DFen->path,Ficname);

if (DFen->system!=0)
    {
    if (strlen(DFen->VolName)>strlen(DFen->path))
        DFen->system=0;
    }

// PrintAt(0,0,"(System:%d)",DFen->system);
// PrintAt(0,1,"(%s)",DFen->path);
// PrintAt(0,2,"(%s)",DFen->VolName);

switch(DFen->system)  {
    case 0:
        if (chdir(DFen->path)!=0)   {
            WinError("Invalid Path");
            strcpy(DFen->path,GetLastHistDir());
            }
        err=DOSlitfic();
        strupr(DFen->path);

        PutInHistDir();
        break;
    case 1:
        err=RARlitfic();
        if (err==1)
            err=RARlitfic();
        strupr(DFen->path);
        break;
    case 2:
        err=ARJlitfic();
        if (err==1)
            err=ARJlitfic();
        strupr(DFen->path);
        break;
    case 3:
        err=ZIPlitfic();
        if (err==1)
            err=ZIPlitfic();
        strupr(DFen->path);
        break;
    case 4:
        err=LHAlitfic();
        if (err==1)
            err=LHAlitfic();
        strupr(DFen->path);
        break;
    case 5:
        err=KKDlitfic();
        if (err==1)
            err=KKDlitfic();
        strupr(DFen->path);
        break;
    default:
        sprintf(nom,"ChangeDir on system: %d",DFen->system);
        YouMad(nom);
        break;
    }

SortFic(DFen);

if (err==0)
    {
    if (!strcmp(Ficname,".."))
        {
        for (n=0;n<DFen->nbrfic;n++)
            if (!stricmp(DFen->F[n]->name,nom)) {
                DFen->pcur=n;
                DFen->scur=n;
                }
        }
    }
    else
    memcpy(p,old,256);
}


//--------------------------------------------------------------
//                     Gestion history
//--------------------------------------------------------------

// Give Last Directory in History
char *GetLastHistDir(void)
{
char *dir,*dir2;
int j;

j=0;

while(1) {
    dir=&(Cfg->HistDir[j]);
    if (strlen(dir)==0) break;

    dir2=dir;

    while ( (j!=256) & (Cfg->HistDir[j]!=0) ) j++;
    j++;
    }
return dir2;
}

// Verify history
void VerifHistDir(void)
{
char *dir;
int j;

j=0;

while(1) {
    dir=&(Cfg->HistDir[j]);
    if ( (dir[0]==0) | (j>=256) ) break;

    dir=strupr(dir);

    IOver=1;
    IOerr=0;

    if (chdir(dir)!=0)
        RemoveHistDir(j,j+strlen(dir)+1);
        else
        {
        while ( (j!=256) & (Cfg->HistDir[j]!=0) ) j++;
        j++;
        }

    IOver=0;
    }
if (j<256)
    memset(&(Cfg->HistDir[j]),0,256-j);
}

// Retire directory de a0 jusque a1
void RemoveHistDir(int a0,int a1)
{
int i;

for (i=a0;i<256;i++)
    {
    if (i<256-(a1-a0))
        Cfg->HistDir[i]=Cfg->HistDir[a1-a0+i];
        else
        Cfg->HistDir[i]=0;
    }
}


void PutInHistDir(void)
{
int i,j,k;
static int TabDir[25];

int a;

do
    {
    a=0;

    j=0;
    for (i=0;i<25;i++)
        {
        TabDir[i]=j;
        if (Cfg->HistDir[j]==0) break;
        while ( (j!=256) & (Cfg->HistDir[j]!=0) ) j++;
        j++;
        }

    // Retire a qui empeche le nouveau de se placer

    if ((j+strlen(DFen->path))>=256)
        a=1;

    for (k=0;k<i;k++)
        {
        if (!memcmp(DFen->path, &(Cfg->HistDir[TabDir[k]]),strlen(DFen->path)+1))
            {
            a=k+1;
            break;
            }
        }

    // Retire celui qui se trouve en a-1

    if (a!=0)
        {
        RemoveHistDir(TabDir[a-1],TabDir[a]);
        }
    }
while (a!=0);


memcpy(&(Cfg->HistDir[TabDir[i]]),DFen->path,strlen(DFen->path)+1);
}

void MakeDir(char *Ficname)
{
Path2Abs(DFen->path,Ficname);

switch(DFen->system)  {
    case 0:
        if (mkdir(DFen->path)!=0) {
            WinError("This directory couldn't be created");
            strcpy(DFen->path,GetLastHistDir());
            }
        break;
    default:
        YouMad("Makedir");
        break;
        }
}


/************************************************************************\
 *  Gestion ligne de commande                                           *
\************************************************************************/

// Commandes globales
// ------------------
int x0,py,xmax;       // position en X,Y ,X initial, Taille de X max.
int x1;               // longueur de la path
char str[256];        // commande interne
int px;               // Chaipus
char flag;            // direction flag pour plein de choses

// Execution d'une commande
// ------------------------
int Run(char *chaine)
{
// To Logfile
/*
time_t t;
t=time(NULL);
fprintf(stderr,"%-50s @ %s",chaine,ctime(&t));
*/


if (!strnicmp(chaine,"CD -",4)) {
   ChangeDir(GetLastHistDir());
   ChangeLine();      // Affichage Path
   return 1;
   }
if (!strnicmp(chaine,"CD ",3)) {
   ChangeDir(chaine+3);
   ChangeLine();      // Affichage Path
   return 1;
   }
if (!strnicmp(chaine,"CD..",4)) {
   ChangeDir(chaine+2);
   ChangeLine();      // Affichage Path
   return 1;
   }
if (!strnicmp(chaine,"MD ",3)) {
   MakeDir(chaine+3);
   ChangeLine();      // Affichage Path
   return 1;
   }
if (!strnicmp(chaine,"#INIT",5)) {
    sscanf(chaine+5,"%d %d %d",&x0,&py,&xmax);
    ChangeLine();
    return 1;
    }
if (!strnicmp(chaine,"#MEM",4)) {
    int tail[12];
    GetFreeMem((void*)tail);  // inconsistent ?
    PrintAt(0,0,"Memory: %d octets",tail[0]);
    getch();
    return 1;
    }
if (!strnicmp(chaine,"#DF",4)) {
    sscanf(chaine+3,"%d",&flag);
    return 1;
    }

return 0;
}

// Affichage de la ligne de commande
// ---------------------------------
void ChangeLine(void)
{
int n;

PrintAt(x0,py,"%s>",DFen->path);
x1=strlen(DFen->path)+1;
PrintAt(x0+x1,py,str);
GotoXY(x1+x0+strlen(str),py);
for (n=strlen(str);n<xmax-x1;n++)
    AffChr(x1+x0+n,py,32);
}


// affiche le message en command line
//	  si la premiere lettre est '#', alors on n'affiche pas.

// Commandes
// ---------
// #INIT x0 y0 px
//				 position initial du curseur et longueur
// #MEM
//				 m‚moire restante
// #DF x
//				 direction flag

// Valeur de car
// -------------
//	0: Commande normale
//	1: Structure fenˆtre

// Code de retour
// --------------
//	- 1: Execution du commande externe OK
//	- 0: Pas d'execution

int CommandLine(char *string,...)
{
char sortie[256];
char *suite;
va_list parameter;

int n;
char a;                       // caracter lu

char *chaine;                 // pointeur sur chaine pour traitement
char affich;                  // vaut 1 si on doit afficher
int pos;                      // position dans chaine

char inter[256];              // commande interne

char FctType;                 // =1 suite[0]=='#'
                              // =2 commande de ligne

int traite=0;                 // vaut 1 si on doit traiter la ligne

suite=sortie;

va_start(parameter,string);
vsprintf(sortie,string,parameter);
va_end(parameter);


if (suite[0]=='#')  {
   pos=0;           // position dans commande interne
   chaine=inter;    // commande interne
   affich=0;        // ne pas afficher
   FctType=1;
   n=1;             // commencer … la deuxieme lettre
   }
   else   {
   pos=px;          // position dans command
   chaine=str;      // commande externe
   affich=1;        // afficher
   FctType=2;
   n=0;             // commencer … la premiere lettre
   }

traite=0;

if ( (suite[0]=='\n') & (px==0)) return 0;

do {
   a=suite[n];

   switch (a) {
		case '\r':
			 while (pos!=0) {
				 pos--;
				 if (affich==1)
					AffChr(pos+x0+x1,py,32);
				 }
			 break;
		case 8: 			// delete
			if (pos>0) pos--;
			break;
		case 0X7F:
			while (pos!=0) {
				pos--;
				if (chaine[pos]==32) break;
				chaine[pos]=0;
				if (affich==1)
					AffChr(pos+x0+x1,py,32);
				}
			break;
		case '\n':          // passage … la ligne
			traite=1;
			break;
		default:
			if (affich==1)
				 AffChr(pos+x0+x1,py,a);
			chaine[pos]=a;
			pos++;
			break;
		}
   chaine[pos]=0;
   if (affich==1)
	   AffChr(pos+x0+x1,py,32);
   n++;
   } while (suite[n]!=0);


if (affich==1)
    GotoXY(pos+x0+x1,py);

// Traite les commandes normales (mˆme si traite==0) mais mettre … 1 aprŠs

if (FctType==1) traite=1;

if (traite==1) {
    if (Run(chaine)==0)
        {
        Shell(">%s",chaine);
        }
    goto Ligne_Traite;
    }

if ( (chaine[1]==':') & (chaine[0]!=0) )
    {
    unsigned nbrdrive;
    char error;
    struct diskfree_t d;

    error=0;

    _dos_setdrive(toupper(chaine[0])-'A'+1,&nbrdrive);

    _dos_getdrive(&nbrdrive);

    IOver=1;
    IOerr=0;

    if (nbrdrive!=(toupper(chaine[0])-'A'+1) )
        error=1;
    if (_dos_getdiskfree(nbrdrive,&d)!=0)
        error=1;
    if (IOerr!=0)
        error=1;

    IOver=0;

    if (error==0)
        {
        InstallDOS();              // drive 1 for A, 2 for B

        _dos_getdrive(&(DFen->drive));
        DFen->FenTyp=0;

        traite=1;
        }
        else
        {
        _dos_setdrive(toupper(DFen->path[0])-'A'+1,&nbrdrive);
        WinError("Invalid Drive");
        }


    Run("cd .");
    traite=1;
    goto Ligne_Traite;
    }

Ligne_Traite:

if (traite==1)
    {
    switch (FctType)  {
        case 1:
            break;  // Commande Interne (Rien … faire)
        case 2:
            str[0]=0;
            px=0;
            ChangeLine();
            break;
        default:
            break;
        }
    }
    else
    {
    switch (FctType)  {
        case 1:
            break;  // Commande Interne (Rien … faire)
        case 2:
            px=pos;
            break;
        default:
            break;
        }
    }

return 1;
}
