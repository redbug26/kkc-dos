/*--------------------------------------------------------------------*\
|- gestion ligne de commande ...                                      -|
\*--------------------------------------------------------------------*/
#include <time.h>                                      // Heure presente

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <bios.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include <dos.h>

#include "kk.h"
#include "win.h"

/*--------------------------------------------------------------------*\
|- prototype interne                                                  -|
\*--------------------------------------------------------------------*/
int SortTest(const void *P1,const void *P2);
void ClearAllSpace(char *name);


/*--------------------------------------------------------------------*\
|-   Fonction utilis‚e pour le classement des fichiers                -|
\*--------------------------------------------------------------------*/
int SortTest(const void *P1,const void *P2)
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
if (F1->name[1]==':') return 1;
if (F2->name[1]==':') return -1;

// Place les X* Eject en bas
//---------------------------
if (F1->name[0]=='*') return 1;
if (F2->name[0]=='*') return -1;


// Place les .. et . en haut
//---------------------------
if (!strcmp(F1->name,"..")) return -1;
if (!strcmp(F2->name,"..")) return 1;

if (!strcmp(F1->name,".")) return -1;
if (!strcmp(F2->name,".")) return 1;

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

        if ( (KKCfg->extens[c3]==32) | (KKCfg->extens[c3]==0) )
            {
            memcpy(e,KKCfg->extens+c4,4);
            e[3]=0;
            if (!stricmp(e,e1)) c1=c3;
            if (!stricmp(e,e2)) c2=c3;
            c4=c3+1;
            }
        }
    while(KKCfg->extens[c3]!=0);

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

/*--------------------------------------------------------------------*\
|-  Classe les fichiers                                               -|
|- Sauf si il ne faut pas ou si c'est une fenetre cach‚e              -|
\*--------------------------------------------------------------------*/
void SortFic(FENETRE *Fen)
{
if ((DFen->nfen>=2) | ((DFen->order&15)==0)) return;
qsort((void*)Fen->F,Fen->nbrfic,sizeof(struct file *),SortTest);
}


/*--------------------------------------------------------------------*\
|- Erase the all the space in the name                                -|
\*--------------------------------------------------------------------*/
void ClearAllSpace(char *name)
{
char buf[1024];
short i,j;

i=0;    //--- navigation dans name -------------------------------------
j=0;    //--- position dans buf ----------------------------------------

while (name[i]!=0)
    {
    if (name[i]!=32)
        {
        buf[j]=name[i];
        j++;
        }
    i++;
    }
buf[j]=0;

strcpy(name,buf);
}


/*--------------------------------------------------------------------*\
|-   Change de repertoire                                             -|
\*--------------------------------------------------------------------*/
void ChangeDir(char *Ficname)
{
static char nom[256];
static char old[256];     // Path avant changement
int n;
int err;
char *p;

p=DFen->path;
memcpy(old,p,256);

ClearAllSpace(Ficname);

// Conversion suivant le type de dir du systeme (pour comptabilite UNIX)
for(n=0;n<strlen(Ficname);n++)  
    if (Ficname[n]=='/') Ficname[n]=DEFSLASH;

FileinPath(DFen->path,nom);

Path2Abs(DFen->path,Ficname);

if ( (p[strlen(p)-1]==DEFSLASH) & (p[strlen(p)-2]!=':') )
     p[strlen(p)-1]=0;

err=0;

/*--------------------------------------------------------------------*\
|- Libere la m‚moire utilis‚e par les fichiers                        -|
\*--------------------------------------------------------------------*/

for(n=0;n<DFen->nbrfic;n++)
    {
    if (DFen->F[n]->info!=NULL)
        LibMem(DFen->F[n]->info);
    LibMem(DFen->F[n]->name);
    LibMem(DFen->F[n]);
    DFen->nbrfic=0;
    }

do
{
switch(DFen->system)
    {
    case 0:
        err=DOSlitfic();
        if (err==0)
            {
            if ( (strcmp(Ficname,"..")!=0) & (DFen->Fen2!=DFen) )
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
    case 6:
        err=DFPlitfic();
        break;
    case 7:
        err=Hostlitfic();
        break;
    case 8:
        err=RAWlitfic();
        break;
    default:
        sprintf(nom,"ChangeDir on system: %d",DFen->system);
        YouMad(nom);
        break;
    }
}
while(err==1);

if (KKCfg->addselect)
    {
     DFen->F[DFen->nbrfic]=(struct file*)GetMem(sizeof(struct file));

     DFen->F[DFen->nbrfic]->name=(char*)GetMem(4);
     strcpy(DFen->F[DFen->nbrfic]->name,"*92");
     DFen->F[DFen->nbrfic]->time=0;
     DFen->F[DFen->nbrfic]->date=0;
     DFen->F[DFen->nbrfic]->attrib=_A_SUBDIR;
     DFen->F[DFen->nbrfic]->select=0;
     DFen->F[DFen->nbrfic]->size=0;
     DFen->nbrfic++;
     }

StrUpr(DFen->path);

SortFic(DFen);  

if (err==0)
    {
    if (!strcmp(Ficname,".."))
        {
        for (n=0;n<DFen->nbrfic;n++)
            if (!WildCmp(DFen->F[n]->name,nom))
                {
                DFen->pcur=n;
                DFen->scur=(DFen->yl)/2;              // Centrage du nom
                }
        }
    }
    else
    memcpy(p,old,256);

if (KKCfg->dispath) // C'est laid mais il faut r‚actualiser les fenˆtres
    DFen->init=1;
}

/*--------------------------------------------------------------------*\
|-                      Gestion history dir                           -|
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-  Give Last Directory in History                                    -|
\*--------------------------------------------------------------------*/
char *GetLastHistDir(void)
{
char *dir,*dir2;
int j;

j=0;

while(1)
    {
    dir=&(KKCfg->HistDir[j]);
    if (strlen(dir)==0) break;

    dir2=dir;

    while ( (j!=256) & (KKCfg->HistDir[j]!=0) ) j++;
    j++;
    }
return dir2;
}

/*--------------------------------------------------------------------*\
|-  Verify history                                                    -|
\*--------------------------------------------------------------------*/
void VerifHistDir(void)
{
char *dir;
int j;

j=0;

ColLin(0,0,Cfg->TailleX,Cfg->col[6]);

while(1) {
    dir=&(KKCfg->HistDir[j]);
    if ( (dir[0]==0) | (j>=256) ) break;

    dir=StrUpr(dir);

    IOver=1;
    IOerr=0;

    PrintAt(0,0,"Verify history dir. %-60s",dir);
    if (chdir(dir)!=0)
        RemoveHistDir(j,j+strlen(dir)+1);
        else
        {
        while ( (j!=256) & (KKCfg->HistDir[j]!=0) ) j++;
        j++;
        }

    IOver=0;
    }
if (j<256)
    memset(&(KKCfg->HistDir[j]),0,256-j);

}

/*--------------------------------------------------------------------*\
|-  Retire directory de a0 jusque a1                                  -|
\*--------------------------------------------------------------------*/
void RemoveHistDir(int a0,int a1)
{
int i;

for (i=a0;i<256;i++)
    {
    if (i<256-(a1-a0))
        KKCfg->HistDir[i]=KKCfg->HistDir[a1-a0+i];
        else
        KKCfg->HistDir[i]=0;
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
        if (KKCfg->HistDir[j]==0) break;
        while ( (j!=256) & (KKCfg->HistDir[j]!=0) ) j++;
        j++;
        }

    //--- Retire a qui empeche le nouveau de se placer -----------------

    if ((j+strlen(DFen->path))>=256)
        a=1;

    for (k=0;k<i;k++)
        {
        if (!strnicmp(DFen->path,
                     &(KKCfg->HistDir[TabDir[k]]),strlen(DFen->path)+1))
            {
            a=k+1;
            break;
            }
        }

    //--- Retire celui qui se trouve en a-1 ----------------------------

    if (a!=0)
        RemoveHistDir(TabDir[a-1],TabDir[a]);
    }
while (a!=0);


memcpy(&(KKCfg->HistDir[TabDir[i]]),DFen->path,strlen(DFen->path)+1);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|- Creation des repertoires                                           -|
\*--------------------------------------------------------------------*/
void MakeDir(char *Ficname)
{
Path2Abs(DFen->path,Ficname);

switch(DFen->system)
    {
    case 0:
        if (mkdir(DFen->path)!=0)
            {
            WinError("This directory couldn't be created");
            strcpy(DFen->path,GetLastHistDir());
            }
        break;
    default:
        YouMad("Makedir");
        break;
        }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void Line2PrcLine(char *s,char *chaine)
{
int n,m;
static char path[256],path2[256];

strcpy(path,DFen->path);
strcat(path,"\\");
strcpy(path2,DFen->Fen2->path);
strcat(path2,"\\");

chaine[0]=0;    m=n=0;

while(s[n]!=0)
    {
    if (!strnicmp(s+n,path,2))
        {
        strcat(chaine+m,"!:");
        m+=2;
        n+=2;
        }
    else
    if (!strnicmp(s+n,path2+2,strlen(path2)-2))
        {
        strcat(chaine+m,"$\\");
        m+=2;
        n+=strlen(path2)-2;
        }
    else
    if (!strnicmp(s+n,path+2,strlen(path)-2))
        {
        strcat(chaine+m,"!\\");
        m+=2;
        n+=strlen(path)-2;
        }
    else
    if (!strnicmp(s+n,path2+2,strlen(path2)-3))
        {
        strcat(chaine+m,"$/");
        m+=2;
        n+=strlen(path2)-3;
        }
    else
    if (!strnicmp(s+n,path+2,strlen(path)-3))
        {
        strcat(chaine+m,"!/");
        m+=2;
        n+=strlen(path)-3;
        }
    else
    if (!strnicmp(s+n,DFen->F[DFen->pcur]->name,
                                 strlen(DFen->F[DFen->pcur]->name)))
        {
        strcat(chaine+m,"!.!");
        m+=3;
        n+=strlen(DFen->F[DFen->pcur]->name);
        }
    else
    if (!strnicmp(s+n,path2,2))
        {
        strcat(chaine+m,"$:");
        m+=2;
        n+=2;
        }
    else
    if (!strnicmp(s+n,DFen->Fen2->F[DFen->Fen2->pcur]->name,
                     strlen(DFen->Fen2->F[DFen->Fen2->pcur]->name)))
        {
        strcat(chaine+m,"$.$");
        m+=3;
        n+=strlen(DFen->Fen2->F[DFen->Fen2->pcur]->name);
        }
    else
       {
       chaine[m]=s[n];
       m++;
       n++;
       }
    chaine[m]=0;
    }
}

/*--------------------------------------------------------------------*\
|- Gestion history                                                    -|
\*--------------------------------------------------------------------*/
void Line2History(char *s)
{
char chaine[256];

if (KKCfg->cnvhist==1)
    {
    Line2PrcLine(s,chaine);
    PutInHistCom(chaine);
    }
    else
    PutInHistCom(s);
}

void History2Line(char *s,char *chaine)
{
int n,m;

chaine[0]=0;
m=n=0;

while(s[n]!=0)
{
if (!strnicmp(s+n,"!:",2))
    {
    memcpy(chaine+m,DFen->path,2);
    m+=2;
    n+=2;
    }
else
if (!strnicmp(s+n,"!\\",2))
    {
    memcpy(chaine+m,DFen->path+2,strlen(DFen->path)-2);
    m+=strlen(DFen->path)-1;
    chaine[m-1]='\\';
    n+=2;
    }
else
if (!strnicmp(s+n,"!/",2))
    {
    memcpy(chaine+m,DFen->path+2,strlen(DFen->path)-2);
    m+=strlen(DFen->path)-2;
    n+=2;
    }
else
if (!strnicmp(s+n,"!.!",3))
    {
    memcpy(chaine+m,DFen->F[DFen->pcur]->name,
                                     strlen(DFen->F[DFen->pcur]->name));
    m+=strlen(DFen->F[DFen->pcur]->name);
    n+=3;
    }
else
if (!strnicmp(s+n,"$:",2))
    {
    memcpy(chaine+m,DFen->Fen2->path,2);
    m+=2;
    n+=2;
    }
else
if (!strnicmp(s+n,"$\\",2))
    {
    memcpy(chaine+m,DFen->Fen2->path+2,strlen(DFen->Fen2->path)-2);
    m+=strlen(DFen->Fen2->path)-1;
    chaine[m-1]='\\';
    n+=2;
    }
else
if (!strnicmp(s+n,"$/",2))
    {
    memcpy(chaine+m,DFen->Fen2->path+2,strlen(DFen->Fen2->path)-2);
    m+=strlen(DFen->Fen2->path)-2;
    n+=2;
    }
else
if (!strnicmp(s+n,"$.$",3))
    {
    memcpy(chaine+m,DFen->Fen2->F[DFen->Fen2->pcur]->name,
                         strlen(DFen->Fen2->F[DFen->Fen2->pcur]->name));
    m+=strlen(DFen->Fen2->F[DFen->Fen2->pcur]->name);
    n+=3;
    }
else
   {
   chaine[m]=s[n];
   m++;
   n++;
   }
chaine[m]=0;
}

}

/*--------------------------------------------------------------------*\
|-  Give Last Directory in History of command                         -|
\*--------------------------------------------------------------------*/
char *GetNextHistCom(char *chaine)
{
int j,k;
char *dir;

dir=KKCfg->HistCom;
k=0;

for(j=0;j<511;j++)
    {
    if (KKCfg->HistCom[j]==0)
        {
        if ((k>KKCfg->posinhist)&(!strnicmp(chaine,dir,strlen(chaine)))&
            (KKCfg->HistCom[j+1]!=0) )
            {
            KKCfg->posinhist=k;
            return dir;
            }
        k=j+1;
        dir=KKCfg->HistCom+k;
        }
    }

dir=KKCfg->HistCom+KKCfg->posinhist;

if (!strnicmp(chaine,dir,strlen(chaine)))
    return dir;
    else
    return chaine;
}

char *GetPrevHistCom(char *chaine)
{
int j,k,l;
char *dir,*dir2;

dir2=NULL;
dir=KKCfg->HistCom;
k=0;

l=KKCfg->posinhist;

for(j=0;j<511;j++)
    {
    if (KKCfg->HistCom[j]==0)
        {
        if ((k<KKCfg->posinhist)&(!strnicmp(chaine,dir,strlen(chaine)))&
               (KKCfg->HistCom[k]!=0))
            {
            l=k;
            dir2=dir;
            }
        k=j+1;
        dir=KKCfg->HistCom+k;
        }
    }

KKCfg->posinhist=l;

if (dir2==NULL)
    {
    dir=KKCfg->HistCom+KKCfg->posinhist;

    if (!strnicmp(chaine,dir,strlen(chaine)))
        return dir;
        else
        return chaine;
    }
    else
    return dir2;
}

/*--------------------------------------------------------------------*\
|-  Verify history of command                                         -|
\*--------------------------------------------------------------------*/
void VerifHistCom(void)
{
char *dir;
int j;

j=0;

while(1) {
    dir=&(KKCfg->HistCom[j]);
    if ( (dir[0]==0) | (j>=512) ) break;

    dir=StrUpr(dir);

    IOver=1;
    IOerr=0;

    while ( (j!=512) & (KKCfg->HistCom[j]!=0) ) j++;
    j++;

    IOver=0;
    }
if (j<512)
    memset(&(KKCfg->HistCom[j]),0,512-j);
}

/*--------------------------------------------------------------------*\
|-  Retire commande  de a0 jusque a1                                  -|
\*--------------------------------------------------------------------*/
void RemoveHistCom(int a0,int a1)
{
int i;

for (i=a0;i<512;i++)
    {
    if (i<512-(a1-a0))
        KKCfg->HistCom[i]=KKCfg->HistCom[a1-a0+i];
        else
        KKCfg->HistCom[i]=0;
    }
}


void PutInHistCom(char *chaine)
{
int i,j,k;
static int TabCom[100];

int a;

do
    {
    a=0;

    j=0;
    for (i=0;i<100;i++)
        {
        TabCom[i]=j;
        if (KKCfg->HistCom[j]==0) break;
        while ( (j!=512) & (KKCfg->HistCom[j]!=0) ) j++;
        j++;
        }

    //--- Retire a qui empeche le nouveau de se placer -----------------

    if ((j+strlen(chaine))>=512)
        a=1;

    for (k=0;k<i;k++)
        {
        if (!strnicmp(chaine,
                         &(KKCfg->HistCom[TabCom[k]]),strlen(chaine)+1))
            {
            a=k+1;
            break;
            }
        }

    //--- Retire celui qui se trouve en a-1 ----------------------------

    if (a!=0)
        RemoveHistCom(TabCom[a-1],TabCom[a]);
    }
while (a!=0);

memcpy(&(KKCfg->HistCom[TabCom[i]]),chaine,strlen(chaine)+1);
}


/*--------------------------------------------------------------------*\
|-   Gestion ligne de commande                                        -|
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-  Commandes globales                                                -|
\*--------------------------------------------------------------------*/
char CLstr[256];                                     // commande interne
static int px;                                                // Chaipus

/*--------------------------------------------------------------------*\
|-  Execution d'une commande                                          -|
\*--------------------------------------------------------------------*/
int Run(char *chaine)
{
FILE *fic;
time_t t;

if ((chaine[0]!='#') & (strcmp(chaine,"cd .")!=0) & (KKCfg->logfile==1))
    {
    t=time(NULL);
    fic=fopen(KKFics->log,"at");
    fprintf(fic,"%-50s @ %s",chaine,ctime(&t));
    fclose(fic);
    }

if (!stricmp(chaine,"CD -"))
    {
    ChangeDir(GetLastHistDir());
    DFen->ChangeLine=1;                                // Affichage Path
    return 1;
    }
if (!strnicmp(chaine,"CD ",3))
    {
    ChangeDir(chaine+3);
    DFen->ChangeLine=1;                                // Affichage Path
    return 1;
    }
if ( (!strnicmp(chaine,"CD..",4)) | (!strnicmp(chaine,"CD\\",3)) |
                                           (!strnicmp(chaine,"CD/",3)) )
    {
    ChangeDir(chaine+2);
    DFen->ChangeLine=1;                                // Affichage Path
    return 1;
    }
if (!strnicmp(chaine,"MD ",3))
    {
    MakeDir(chaine+3);
    DFen->ChangeLine=1;                                // Affichage Path
    return 1;
    }

/*if (!strnicmp(chaine,"#P4",3))
    {
    P4();  return 1;
    }*/

if (!strnicmp(chaine,"#MEM",4))
    {
    PrintAt(0,0,"Memory: %20d octets",FreeMem());
    return 1;
    }

if (!strnicmp(chaine,"#FCT",4))
    {
    int fct;
    sscanf(chaine+4,"%d",&fct);
    GestionFct(fct);
    return 1;
    }

return 0;
}


/*--------------------------------------------------------------------*\
|-                      Execute command                               -|
\*--------------------------------------------------------------------*/

void ExecCom(void)
{
static char Dir[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[] =
    { { 2,3,1,Dir,&DirLength},
      { 7,5,2,NULL,NULL},
      {30,5,5," Last result ",NULL},
      {53,5,3,NULL,NULL},
      { 5,2,0,"Enter the command:",NULL},
      { 1,1,4,&CadreLength,NULL} };

struct TmtWin F = {-1,10,74,17, "Execute command" };

int n;

strcpy(Dir,CLstr);
Dir[69]=0;

n=WinTraite(T,6,&F,0);

if (n!=-1)
    {
    if (T[n].type==5)                          // Le bouton personnalis‚
        GestionFct(74);

    if (T[n].type!=3)
        if (strlen(Dir)!=0)
            {
            KKCfg->scrrest=0;
            CommandLine("#%s >%s",Dir,KKFics->temp);
            }
    }
}




/*--------------------------------------------------------------------*\
|-  Affichage de la ligne de commande                                 -|
\*--------------------------------------------------------------------*/
void AffCmdLine(void)
{
int x1,m,n,v;

if (DFen->ChangeLine==0)
    return;

if (DFen->nfen>=2) return;

if (KKCfg->cmdline==0) return;

x1=strlen(DFen->path)+1;
m=strlen(CLstr);

v=Cfg->TailleX-2;


if ( ((x1+m)>75) & (x1>40) )
    {
    n= (m>=v) ? m-v : 0;
    PrintAt(0,KKCfg->cmdline,">%-*s",v,CLstr+n);
    GotoXY(m-n+1,KKCfg->cmdline);
    }
    else
    {
    n= (m>=(v+1-x1)) ? m-(v+1-x1) : 0;
    PrintAt(0,KKCfg->cmdline,"%s>%-*s",DFen->path,v+1-x1,CLstr+n);
    GotoXY(x1+m-n,KKCfg->cmdline);
    }
}

/*--------------------------------------------------------------------*\
|-  affiche le message en command line                                -|
|-     si la premiere lettre est '#', alors on n'affiche pas.         -|
|*--------------------------------------------------------------------*|
|-  Commandes                                                         -|
|-  #MEM                                                              -|
|-                m‚moire restante                                    -|
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-  Valeur de car                                                     -|
|-   0: Commande normale                                              -|
|-   1: Structure fenˆtre                                             -|
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-  Code de retour                                                    -|
|-   - 1: Execution du commande externe OK                            -|
|-   - 0: Pas d'execution                                             -|
\*--------------------------------------------------------------------*/

int CommandLine(char *string,...)
{
char sortie[256];
char *suite;
va_list parameter;

int n;
char a;                                                   // caracter lu

char *chaine;                     // pointeur sur chaine pour traitement
char affich;                               // vaut 1 si on doit afficher
int pos;                                         // position dans chaine

char inter[256];                                     // commande interne

char FctType;                                        // =1 suite[0]=='#'
                                                 // =2 commande de ligne

int traite=0;                      // vaut 1 si on doit traiter la ligne

suite=sortie;

va_start(parameter,string);
vsprintf(sortie,string,parameter);
va_end(parameter);

if (suite[0]=='#')
    {
    pos=0;                             // position dans commande interne
    chaine=inter;                                    // commande interne
    affich=0;                                         // ne pas afficher
    FctType=1;
    n=1;                               // commencer … la deuxieme lettre
    }
    else
    {
    pos=px;                                     // position dans command
    chaine=CLstr;                                    // commande externe
    affich=1;                                                // afficher
    FctType=2;
    n=0;                               // commencer … la premiere lettre
    }

traite=0;

if (suite[0]==0) return 0;

if ( (suite[0]=='\n') & (px==0)) return 0;

if ( (suite[0]==32) & (px==0)) return 0;

if ( (suite[0]=='\r') & (px==0)) return 0;

do {
    a=suite[n];

    switch (a)
        {
		case '\r':
            while (pos!=0)  pos--;
            break;
        case 8:        //--- delete ------------------------------------
			if (pos>0) pos--;
			break;
        case 0X7F:     //--- CTRL-DEL ----------------------------------
            while (pos!=0)
                {
				pos--;
                if ((chaine[pos]==32) | (chaine[pos]=='.')) break;
				chaine[pos]=0;
				}
			break;
        case '\n':     //--- passage … la ligne ------------------------
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
    DFen->ChangeLine=1;

/*--------------------------------------------------------------------*\
|-Traite les commandes normales (mˆme si traite==0) ! mettre … 1 aprŠs-|
\*--------------------------------------------------------------------*/

if ( (traite==1) & (affich==1) )
    Line2History(chaine);

if (FctType==1) traite=1;


if ( (chaine[1]==':') & (chaine[0]!=0) & (chaine[2]==0) & (traite==1) )
    {
    char path[MAXPATH];
    char drive;

    drive=toupper(chaine[0]);
    if ((drive>='A') & (drive<='Z'))
        {
        drive-='A';

        if (DriveReady(drive))
            {
            DrivePath(drive,path);
            if (path[strlen(path)-1]==':')
                strcat(path,"\\");

            ChangeDir(path);
            DFen->ChangeLine=1;                        // Affichage Path
            }
            else
            WinError("Invalid Drive");

        traite=1;
        goto Ligne_Traite;
        }
    }

if (traite==1)
    {
    if (Run(chaine)==0)
        Shell(">%s",chaine);

    goto Ligne_Traite;
    }

Ligne_Traite:


if (traite==1)
    {
    switch (FctType)
        {
        case 1:
            break;                    // Commande Interne (Rien … faire)
        case 2:
            CLstr[0]=0;
            px=0;
            DFen->ChangeLine=1;
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
            break;                    // Commande Interne (Rien … faire)
        case 2:
            px=pos;
            break;
        default:
            break;
        }
    }

return 1;
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|- Function:     int IsDir(struct file *F)                            -|
|-                                                                    -|
|- Description:  Test if a file is a directory                        -|
|-                                                                    -|
|- Input:        F: The file                                          -|
|-                                                                    -|
|- Return:       0: It isn't a directory                              -|
|-               1: It's a directory                                  -|
\*--------------------------------------------------------------------*/
int IsDir(struct file *F)
{
if ( (F->attrib & _A_SUBDIR)!=_A_SUBDIR) return 0;
if (F->name[1]==':') return 0;
if (F->name[1]=='*') return 0;
if (F->name[0]=='*') return 0;
return 1;
}

/*--------------------------------------------------------------------*\
|- Function:     int FicSelect(FENETRE *Fen,int n,char q)             -|
|-                                                                    -|
|- Description:  Selection, Deselect or inverse the select. of a file -|
|-                                                                    -|
|- Input:        Fen: Working pannel                                  -|
|-               n: number of the file                                -|
|-               q: 0 -> deselect                                     -|
|-                  1 -> select                                       -|
|-                  2 -> inverse selection                            -|
|-                                                                    -|
|- Return:       0: It's Okay...                                      -|
|-               1: Error, you couldn't do that                       -|
\*--------------------------------------------------------------------*/
int FicSelect(FENETRE *Fen,int n,char q)
{
if (Fen->F[n]->name[0]=='.') return 1;
if (Fen->F[n]->name[1]==':') return 1;
if (Fen->F[n]->name[0]=='*') return 1;
if (Fen->F[n]->name[1]=='*') return 1;
if ((Fen->F[n]->attrib & _A_VOLID)==_A_VOLID) return 1;

switch(q)
    {
    case 0: //--- Deselect ---------------------------------------------
        if (Fen->F[n]->select==1)
            {
            Fen->F[n]->select=0;
            Fen->taillesel-=Fen->F[n]->size;
            Fen->nbrsel--;
            }
        break;
    case 1: //--- Select -----------------------------------------------
        if (Fen->F[n]->select==0)
            {
            Fen->F[n]->select=1;
            Fen->taillesel+=Fen->F[n]->size;
            Fen->nbrsel++;
            }
        break;
    case 2: //--- inverse selection ------------------------------------
        if (Fen->F[n]->select==0)
            {
            Fen->F[n]->select=1;
            Fen->taillesel+=Fen->F[n]->size;
            Fen->nbrsel++;
            }
            else
            {
            Fen->F[n]->select=0;
            Fen->taillesel-=Fen->F[n]->size;
            Fen->nbrsel--;
            }
        break;
    }

return 0;
}

