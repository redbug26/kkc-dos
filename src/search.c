/*--------------------------------------------------------------------*\
|- Outil de recherche de fichiers                                     -|
\*--------------------------------------------------------------------*/
#include <ctype.h>
#include <io.h>

#include <stdio.h>
#include <string.h>

#include <conio.h>

#include <time.h>

#include "kk.h"

static char **tabnom;
static char **tabpath;
static int *tabtime;
static int *tabdate;
static int *tabsize;

static int nbrmax;
static int nbr;

static char SearchName[70]="*.*";
static char SearchString[42];
static char Drive[28];
static int sw=3;

#define BreakESC  if (KbHit()) touche=Wait(0,0,0); \
                                             else if (touche==27) break;

/*--------------------------------------------------------------------*\
|-  Recherche la chaine SearchString dans le fichier *name            -|
\*--------------------------------------------------------------------*/
int strfic(char *path,char *name)
{
long lng,n,m;
char fin=1;

static char fichier[256];
FILE *fic;

strcpy(fichier,path);
Path2Abs(fichier,name);

fic=fopen(fichier,"rb");
if (fic==NULL)
    {
    WinError(path);

    WinError(fichier);
    return 1;
    }

lng=filelength(fileno(fic));

m=0;
for (n=0;n<lng;n++)
    {
    if (fgetc(fic)==SearchString[m])
        {
        m++;
        if (SearchString[m]==0) fin=0;
        }
        else
        m=0;
    if (fin==0) break;
    }

fclose(fic);
return fin;
}

void SchAffLine(int pos,int n)
{
char chaine[256];

sprintf(chaine,"%-13s %9d %02d/%02d/%2d %02d:%02d %*s",
        tabnom[n],tabsize[n],
        (tabdate[n]&31),(tabdate[n]>>5)&15,(tabdate[n]>>9)+80,
        (tabtime[n]>>11)&31,(tabtime[n]>>5)&63,
        Cfg->TailleX-42,tabpath[n]);

chaine[13]=Cfg->Tfont;
chaine[23]=Cfg->Tfont;
chaine[32]=Cfg->Tfont;
chaine[38]=Cfg->Tfont;

chaine[Cfg->TailleX-3]=0;

PrintAt(1,pos," %s",chaine);
}

static int touche;

void cherdate(char *nom2)
{
char cont;
struct file *ff;
char error;
static char **TabRec;
int NbrRec;
int n,m;
static char moi[256],nom[256];

TabRec=GetMem(500*sizeof(char*));

TabRec[0]=GetMem(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
m=strlen(TabRec[NbrRec-1]);
m= (m>=72) ? m-72 : 0;
PrintAt(0,0,"Go in  %-73s",TabRec[NbrRec-1]+m);

CommandLine("#cd %s",TabRec[NbrRec-1]);

strcpy(nom,TabRec[NbrRec-1]);

/*--------------------------------------------------------------------*\
|-  The files                                                         -|
\*--------------------------------------------------------------------*/

for (m=0;m<DFen->nbrfic;m++)
    {
    ff=DFen->F[m];

    error=ff->attrib;

    if ( (ff->name[0]!='.') & (!WildCmp(ff->name,SearchName)) )
        {
        cont=1;

        if (IsDir(ff)) cont=0; //--- Not Subdir ------------------------
        if ((error&0x08)==0x08) cont=0; //--- Not Subdir ---------------

        if ( ((SearchString[0])!=0) & (cont==1) & (DFen->system==0) )
            {
            if (strfic(nom,ff->name)!=0) cont=0;
            }

        if (cont)
            {
            int n,pos;

            pos=nbr;

            for (n=0;n<nbr;n++)
                if ( (ff->date>tabdate[n]) |
                    ( (ff->date==tabdate[n]) & (ff->time>tabtime[n]) ) )
                    {
                    pos=n;
                    break;
                    }

            if ( (pos<nbrmax) & ((ff->date>>9)+80<100) )
                {
                if (nbr<nbrmax) nbr++;

                for (n=nbr-1;n>pos;n--)
                    {
                    strcpy(tabnom[n],tabnom[n-1]);
                    strcpy(tabpath[n],tabpath[n-1]);
                    tabtime[n]=tabtime[n-1];
                    tabdate[n]=tabdate[n-1];
                    tabsize[n]=tabsize[n-1];
                    }

                strcpy(tabnom[pos],ff->name);
                strcpy(tabpath[pos],nom);
                if (strlen(nom)==2) strcat(tabpath[pos],"\\");
                tabtime[pos]=ff->time;
                tabdate[pos]=ff->date;
                tabsize[pos]=ff->size;

                for (n=0;n<nbr;n++)
                    {
                    if (n>(Cfg->TailleY)-5) break;
                    SchAffLine(n+2,n);
                    }
                if (nbr<Cfg->TailleY-5)
                    WinLine(1,nbr+2,Cfg->TailleX-2,1);
                }
            }
        }

    BreakESC
    }

NbrRec--;
LibMem(TabRec[NbrRec]);


/*--------------------------------------------------------------------*\
|-  The directories                                                   -|
\*--------------------------------------------------------------------*/

for (m=0;m<DFen->nbrfic;m++)
    {
    ff=DFen->F[m];

    error=ff->attrib;

/*--------------------------------------------------------------------*\
|- Subdir                                                             -|
\*--------------------------------------------------------------------*/

    if ( (ff->name[0]!='.')  & (sw!=5) )
        {
        if ( (IsDir(ff))
           | ((!stricmp(getext(ff->name),"KKD")) & (Cfg->enterkkd==1)
                                                 & (DFen->system==0)) )
            {
            strcpy(moi,nom);
            Path2Abs(moi,ff->name);

            TabRec[NbrRec]=GetMem(strlen(moi)+1);
            memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
            NbrRec++;
            }
        }

    BreakESC
    }

BreakESC

}
while (NbrRec>0);

for(n=0;n<NbrRec;n++)             // Dans le cas o— on appuie sur ESCAPE
    LibMem(TabRec[n]);

LibMem(TabRec);

}

int WinSearch(void)
{
static int l1;

static int DirLength=68;
static int DriveLen=26;
static int StrLen=40;
static int CadreLength=69;

static char x1=26,x2=40,x3=40;
static int y1=6,y2=2,y3=2;

char SearchOld[80];


struct Tmt T[18] = {
      { 3,3,1,SearchName,&DirLength},    // 0
      { 6,2,0,"Filename",NULL},
      { 2,1,4,NULL,&CadreLength},
      { 3,6,10, "Current drive",&sw},
      { 3,7,10, "Current dir & subdir",&sw},
      { 3,8,10, "Only current directory",&sw},
      { 3,9,10, "All drive",&sw},
      { 3,10,10,"user defined drive",&sw},
      { 3,11,1,Drive,&DriveLen},
      { 2,5,9,&x1,&y1},
      { 30,5,9,&x2,&y2},
      { 30,9,9,&x3,&y3},
      { 32,6,0,"Search String:",NULL},
      { 31,7,1,SearchString,&StrLen},
      { 31,10,8,"Search in KKD",&l1},
      { 32,11,0,"Ketchup Killers        Search Function",NULL},

      {15,13,2,NULL,NULL},                // 1:Ok
      {45,13,3,NULL,NULL},
      };

struct TmtWin F = {
    -1,4,73,19,
    "Search file(s)"};

int n;

strcpy(SearchOld,SearchName);

l1=Cfg->enterkkd;

n=WinTraite(T,18,&F);

if (n!=27)  // pas escape
    {
    if (T[n].type!=3)
        {
        Cfg->enterkkd=l1;
        return 0;  // pas cancel
        }
    }

strcpy(SearchName,SearchOld);


return 1;       //--- Erreur -------------------------------------------
}


void Search(FENETRE *TempFen,FENETRE *Fen)
{
int n;
static char nom[256];

int a;
int pos;

int m;          //--- position de la premiere ligne --------------------
int d,fin;      //--- debut et fin de la ligne -------------------------

int prem;       //--- premier visible ----------------------------------

if (WinSearch()==1) return;

nbr=0;
nbrmax=200;

SaveEcran();
PutCur(32,0);

ColLin(0,0,Cfg->TailleX,1*16+4);

/*--------------------------------------------------------------------*\
|-  Allocate Memory                                                   -|
\*--------------------------------------------------------------------*/

tabnom=GetMem(sizeof(char *)*nbrmax);
for (n=0;n<nbrmax;n++)
    tabnom[n]=GetMem(255);

tabpath=GetMem(sizeof(char *)*nbrmax);
for (n=0;n<nbrmax;n++)
    tabpath[n]=GetMem(255);

tabtime=GetMem(sizeof(int)*nbrmax);
tabdate=GetMem(sizeof(int)*nbrmax);
tabsize=GetMem(sizeof(int)*nbrmax);

/*--------------------------------------------------------------------*\
|-  Setup of all                                                      -|
\*--------------------------------------------------------------------*/

WinCadre(0,1,Cfg->TailleX-1,(Cfg->TailleY)-2,1);
Window(1,2,Cfg->TailleX-2,(Cfg->TailleY)-3,10*16+1);

touche=0;

DFen=TempFen;

switch(sw)
    {
    case 3: //--- Current drive ----------------------------------------
        sprintf(nom,"%c:\\",Fen->path[0]);
        CommandLine("#cd %s",nom);
        cherdate(nom);
        break;
    case 4: //--- Current dir & subdir ---------------------------------
        strcpy(nom,Fen->path);
        cherdate(nom);
        break;
    case 5: //--- Current dir ------------------------------------------
        strcpy(nom,Fen->path);
        cherdate(nom);
        break;
    case 6: //--- all drive --------------------------------------------
        for(n=0;n<26;n++)
            {
            if (VerifyDisk(n+1)==0)
                {
                sprintf(nom,"%c:\\",n+'A');
                CommandLine("#cd %s",nom);
                cherdate(nom);
                }
            }
        break;
    case 7: //--- user defined drive -----------------------------------
        for(n=0;n<strlen(Drive);n++)
            {
            m=toupper(Drive[n])-'A';
            if (VerifyDisk(m+1)==0)
                {
                sprintf(nom,"%c:\\",m+'A');
                CommandLine("#cd %s",nom);
                cherdate(nom);
                }
            }
        break;
    }


DFen=Fen;

if (nbr==0)
    {
    WinError("No file found");
    ChargeEcran();
    }
    else
    {
    m=2;

    d=1;
    fin=Cfg->TailleX-2;

    prem=0;
    pos=0;

    do
        {
        char chaine[128];

        if (pos<0) pos=0;
        if (pos>nbr-1) pos=nbr-1;

        while (pos-prem<0)
            prem--;
        while (pos-prem>(Cfg->TailleY)-5)
            prem++;

        for (n=prem;n<nbr;n++)
            {
            if ((n-prem+2)>(Cfg->TailleY)-3) break;
            SchAffLine(n-prem+2,n);
            }

        strcpy(chaine,tabpath[pos]);
        chaine[Cfg->TailleX]=0;
        PrintAt(0,0,"%-*s",Cfg->TailleX,chaine);

        for (n=d;n<=fin;n++)
            AffCol(n,pos+m-prem,7*16+5);

        a=Wait(0,0,0);

        for (n=d;n<=fin;n++)
            AffCol(n,pos+m-prem,10*16+1);

        if (a==72*256)      pos--;
        if (a==80*256)      pos++;
        if (a==0x4700)    pos=0;
        if (a==0x4F00)    pos=nbr-1;
        if (a==0x5100)    pos+=5;
        if (a==0x4900)    pos-=5;
        }
    while ( (a!=27) & (a!=13) );

    ChargeEcran();

    if (a==13)
        {
        CommandLine("#CD %s",tabpath[pos]);

        for (n=0;n<Fen->nbrfic;n++)
            if (!stricmp(tabnom[pos],Fen->F[n]->name))
                {
                Fen->pcur=n;
                Fen->scur=n;
                }
        }
    }

/*--------------------------------------------------------------------*\
|-  Free Memory                                                       -|
\*--------------------------------------------------------------------*/


LibMem(tabsize);
LibMem(tabdate);
LibMem(tabtime);

for (n=0;n<nbrmax;n++)
    LibMem(tabpath[n]);
LibMem(tabpath);

for (n=0;n<nbrmax;n++)
    LibMem(tabnom[n]);
LibMem(tabnom);
}

