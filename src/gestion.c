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


/*-----------------------------------------------------*
 -  Fonction utilis‚e pour le classement des fichiers  -
 *-----------------------------------------------------*/
int SortTest(void *P1,void *P2)
{
char *e1,*e2,e[4];
short c1,c2,c3,c4;

struct file **FF1,**FF2;
struct file *F1,*F2;

int c;

char a,b;

FF1=(struct file **)P1;
FF2=(struct file **)P2;

F1=*FF1;
F2=*FF2;

// Place les X:Reload en haut
//----------------------------
if (F1->name[1]==':') return -1;
if (F2->name[1]==':') return 1;


// Place les .. et . en haut
//---------------------------
if (!strcmp(F1->name,"..")) return -1;
if (!strcmp(F2->name,"..")) return 1;

if (!strcmp(F1->name,".")) return -1;
if (!strcmp(F2->name,".")) return 1;

// Ne trie pas si il ne le faut pas
//----------------------------------
if (DFen->order==0)
    return -1;

// Affiche les repertoires tout en haut
//--------------------------------------
a=((F1->attrib & RB_SUBDIR)==RB_SUBDIR);
b=((F2->attrib & RB_SUBDIR)==RB_SUBDIR);

if ( a ^ b )
    if (b==0)
        return -1;
        else
        return 1;


// Place les extensions speciales toute en haut
//----------------------------------------------
e1=getext(F1->name);
e2=getext(F2->name);

if ((DFen->order&16)==16)
    {
    c1=c2=c3=c4=0;

    do
        {
        c3++;

        if ( (Cfg->extens[c3]==32) | (Cfg->extens[c3]==0) )
            {
            memcpy(e,Cfg->extens+c4,4);
            e[3]=0;
            if (!stricmp(e,e1)) c1=c3;
            if (!stricmp(e,e2)) c2=c3;
            c4=c3+1;
            }
        }
    while(Cfg->extens[c3]!=0);

    if (c1==0) c1=1000;
    if (c2==0) c2=1000;

    if ((c1<c2) & (c1!=1000)) return -1;
    if ((c2<c1) & (c2!=1000)) return 1;
    }

// Trie les autres fichiers
//--------------------------
switch((DFen->order)&15)
    {
    case 1: 
        c=stricmp(F1->name,F2->name);           //--- name ---
        break;
    case 2:
        c=stricmp(e1,e2);                       //--- ext ----
        if (c==0)
            c=stricmp(F1->name,F2->name);
        break;
    case 3: 
        c=(F1->date)-(F2->date);                //--- date ---
        if (c==0) c=(F1->time)-(F2->time);
        break;
    case 4: 
        c=(F1->size)-(F2->size);                //--- size ---
        break;
    }

return c;

}

/*---------------------*
 - Classe les fichiers -
 *---------------------*/
void SortFic(struct fenetre *Fen)
{
qsort(Fen->F,Fen->nbrfic,sizeof(struct file *),SortTest);
}



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


/*----------------------*
 - Change de repertoire -
 *----------------------*/
void ChangeDir(char *Ficname)
{
static char nom[256];
static char old[256];     // Path avant changement
int n;
int err;
char *p;

// PrintAt(0,0,"%-80s","Wait Please");

p=DFen->path;
memcpy(old,p,256);

for(n=0;n<strlen(Ficname);n++)
    if (Ficname[n]=='/') Ficname[n]='\\';

FileinPath(DFen->path,nom);

Path2Abs(DFen->path,Ficname);

if ( (p[strlen(p)-1]=='\\') & (p[strlen(p)-2]!=':') )
     p[strlen(p)-1]=0;

err=0;

do
{
if (DFen->system!=0)
    if (strnicmp(DFen->VolName,DFen->path,strlen(DFen->VolName))!=0)
        DFen->system=0;

if (DFen->system==0)
    {
    static char tnom[256],nom2[256];
    int i;

    if (chdir(DFen->path)!=0)
        {
        strcpy(nom2,DFen->path);
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
            WinError("Invalid Path");
            memcpy(DFen->path,"C:\\",4);
            }
            else
            {
            static char buf[256];

            strcpy(buf,DFen->path);
            Path2Abs(buf,tnom);

            switch (n=NameIDF(buf))
                {
                case 30:    // ARJ
                case 34:    // RAR
                case 35:    // ZIP
                case 32:    // LHA
                case 102:   // KKD
                    break;
                default:
                    n=0;
                }

            if (n!=0)
                {
                strcpy(DFen->VolName,buf);

                strcpy(DFen->path,DFen->VolName);

                switch (n)
                    {
                    case 34:    // RAR
                        DFen->system=1;
                        break;
                    case 30:    // ARJ
                        DFen->system=2;
                        break;
                    case 35:    // ZIP
                        DFen->system=3;
                        break;
                    case 32:    // LHA
                        DFen->system=4;
                        break;
                    case 102:   // KKD
                        DFen->system=5;
                        break;
                    }

                if (strlen(nom2)!=strlen(DFen->VolName))
                    Path2Abs(DFen->path,nom2+strlen(DFen->VolName)+1);
                }
                else
                {
                WinMesg("Invalid Path",nom2);
//                strcpy(DFen->path,GetLastHistDir());
                }
            }
        }
    }

switch(DFen->system)
    {
    case 0:
        err=DOSlitfic();
        if (err==0)
            {
            if (strcmp(Ficname,"..")!=0)
                PutInHistDir();
            }
        break;
    case 1:
        err=RARlitfic();
        break;
    case 2:
        err=ARJlitfic();
        break;
    case 3:
        err=ZIPlitfic();
        break;
    case 4:
        err=LHAlitfic();
        break;
    case 5:
        err=KKDlitfic();
        break;
    default:
        sprintf(nom,"ChangeDir on system: %d",DFen->system);
        YouMad(nom);
        break;
    }
}
while(err==1);

strupr(DFen->path);

if (DFen->nfen!=2)
    SortFic(DFen);  // On trie pas la fenetre cach‚e

if (err==0)
    {
    if (!strcmp(Ficname,".."))
        {
        for (n=0;n<DFen->nbrfic;n++)
            if (!stricmp(DFen->F[n]->name,nom))
                {
                DFen->pcur=n;
                DFen->scur=(DFen->yl)/2;   // Centrage du nom
                }
        }
    }
    else
    memcpy(p,old,256);

// PrintAt(0,0,"%-40s%40s","Ketchup Killers Commander","RedBug");
}


/*--------------------------------------------------------------*
 -                     Gestion history                          -
 *--------------------------------------------------------------*/

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
static int TabDir[100];

int a;

do
    {
    a=0;

    j=0;
    for (i=0;i<100;i++)
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


/*-----------------------------*
 -  Gestion ligne de commande  -
 *-----------------------------*/

// Commandes globales
// ------------------
static int x0,py,xmax;       // position en X,Y ,X initial, Taille de X max.
static char str[256];        // commande interne
static int px;               // Chaipus
static char flag;            // direction flag pour plein de choses

// Execution d'une commande
// ------------------------
int Run(char *chaine)
{
FILE *fic;
time_t t;

if ( (chaine[0]!='#') & (strcmp(chaine,"cd .")!=0) & (Cfg->logfile==1) )
    {
    t=time(NULL);
    fic=fopen(Fics->log,"at");
    fprintf(fic,"%-50s @ %s",chaine,ctime(&t));
    fclose(fic);
    }

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
int x1,m,n;

if (DFen->nfen==2) return;      // Fenetre[2]

x1=strlen(DFen->path)+1;
m=strlen(str);

if ( ((x1+m)>75) & (x1>40) )
    {
    n= (m>=(78-x0)) ? m-(78-x0) : 0;
    GotoXY(x0+m-n+1,py);
    PrintAt(x0,py,">%-*s",78-x0,str+n);
    }
    else
    {
    n= (m>=(79-(x0+x1))) ? m-(79-(x0+x1)) : 0;
    GotoXY(x1+x0+m-n,py);
    PrintAt(x0,py,"%s>%-*s",DFen->path,79-x0-x1,str+n);
    }

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

if ( (suite[0]==32) & (px==0)) return 0;

do {
   a=suite[n];

   switch (a) {
		case '\r':
             while (pos!=0)  pos--;
			 break;
		case 8: 			// delete
			if (pos>0) pos--;
			break;
        case 0X7F:  // CTRL-DEL
			while (pos!=0) {
				pos--;
                if ((chaine[pos]==32) | (chaine[pos]=='.')) break;
				chaine[pos]=0;
				}
			break;
		case '\n':          // passage … la ligne
			traite=1;
			break;
		default:
			chaine[pos]=a;
			pos++;
			break;
		}
   chaine[pos]=0;
   n++;
   } while (suite[n]!=0);


if (affich==1)
    ChangeLine();

// Traite les commandes normales (mˆme si traite==0) mais mettre … 1 aprŠs

if (FctType==1) traite=1;

if ( (chaine[1]==':') & (chaine[0]!=0) & (chaine[2]==0) & (traite==1) )
    {
    unsigned nbrdrive;
    char error;
    struct diskfree_t d;

    error=0;

    IOver=1;
    IOerr=0;

    _dos_setdrive(toupper(chaine[0])-'A'+1,&nbrdrive);
    getcwd(DFen->path,256);
    if (chdir(DFen->path)!=0)
        DFen->path[3]=0;

    InstallDOS();

    _dos_getdrive(&nbrdrive);

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

if (traite==1)
    {
    if (Run(chaine)==0)
        {
        Shell(">%s",chaine);
        }
    goto Ligne_Traite;
    }

Ligne_Traite:


if (traite==1)
    {
    switch (FctType)
        {
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
    switch (FctType)
        {
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

