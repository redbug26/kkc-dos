#include <ctype.h>
#include <io.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <conio.h>
#include <dos.h>

#include <bios.h>   // Gestion clavier

#include "win.h"
#include "kk.h"

static char **tabnom;
static char **tabpath;
static int *tabtime;
static int *tabdate;

static char *nomtemp;
static int nbrmax;
static int nbr;

static char SearchName[70]="*.*";
static char SearchString[42];
static char Drive[28];
static int sw=3;

#define BreakESC  if (kbhit()) touche=_bios_keybrd(0)/256; else if (touche==1) break;

// Recherche la chaine SearchString dans le fichier *name
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

sprintf(chaine,"%-12s %02d/%02d/%2d %02d:%02d %48s",
        tabnom[n],
        (tabdate[n]&31),(tabdate[n]>>5)&15,(tabdate[n]>>9)+80,
        (tabtime[n]>>11)&31,(tabtime[n]>>5)&63,tabpath[n]);
chaine[76]=0;


PrintAt(3,pos,chaine);

}

static touche;

void cherdate(char *nom2)
{
char cont;
struct find_t ff;
char error;
char **TabRec;
int NbrRec;

short touche;

char moi[256],nom[256];

TabRec=malloc(500*sizeof(char*));
TabRec[0]=malloc(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
PrintAt(0,0,"Go in  %-73s",TabRec[NbrRec-1]);

strcpy(nomtemp,TabRec[NbrRec-1]);
Path2Abs(nomtemp,"*.*");

strcpy(nom,TabRec[NbrRec-1]);

// The files

error=_dos_findfirst(nomtemp,63,&ff);

while (error==0) {
    error=ff.attrib;

    if ( (ff.name[0]!='.') & (!WildCmp(ff.name,SearchName)) )
        {
        cont=1;

        if ((error&0x10)==0x10) cont=0; // Not Subdir
        if ((error&0x08)==0x08) cont=0; // Not Subdir

        if ( ((SearchString[0])!=0) & (cont==1) )
            {
            if (strfic(nom,ff.name)!=0) cont=0;
            }

        if (cont)
            {
            int n,pos;

            pos=nbr;

            for (n=0;n<nbr;n++)
                if ( (ff.wr_date>tabdate[n]) | ( (ff.wr_date==tabdate[n]) & (ff.wr_time>tabtime[n]) ) )
                    {
                    pos=n;
                    break;
                    }

            if ( (pos<nbrmax) & ((ff.wr_date>>9)+80<100) )
                {
                if (nbr<nbrmax) nbr++;

                for (n=nbr-1;n>pos;n--)
                    {
                    strcpy(tabnom[n],tabnom[n-1]);
                    strcpy(tabpath[n],tabpath[n-1]);
                    tabtime[n]=tabtime[n-1];
                    tabdate[n]=tabdate[n-1];
                    }

                strcpy(tabnom[pos],ff.name);
                strcpy(tabpath[pos],nom);
                if (strlen(nom)==2) strcat(tabpath[pos],"\\");
                tabtime[pos]=ff.wr_time;
                tabdate[pos]=ff.wr_date;

                for (n=0;n<nbr;n++)
                    {
                    if (n>(Cfg->TailleY)-5) break;
                    SchAffLine(n+2,n);
                    }
                }
            }
        }

    BreakESC

    error=_dos_findnext(&ff);
    }

free(TabRec[NbrRec-1]);
NbrRec--;

// The directories

error=_dos_findfirst(nomtemp,63,&ff);

while (error==0) {
    error=ff.attrib;

    if ( (ff.name[0]!='.') & (sw!=5) )
        {
        if ((error&0x10)==0x10)    // Subdir
            {
            strcpy(moi,nom);
            strcat(moi,"\\");
            strcat(moi,ff.name);

            TabRec[NbrRec]=malloc(strlen(moi)+1);
            memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
            NbrRec++;
            }
        }

    BreakESC

    error=_dos_findnext(&ff);
    }

BreakESC


}
while (NbrRec>0);

free(TabRec);

}

int WinSearch(void)
{
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
      { 3,6,10, "Current drive     ",&sw},
      { 3,7,10, "Cur.dir & subdir  ",&sw},
      { 3,8,10,"Only current dir  ",&sw},
      { 3,9,10,"All drive         ",&sw},
      { 3,10,10,"user defined drive",&sw},
      { 3,11,1,Drive,&DriveLen},
      { 2,5,9,&x1,&y1},
      { 30,5,9,&x2,&y2},
      { 30,9,9,&x3,&y3},
      { 32,6,0,"Search String:",NULL},
      { 31,7,1,SearchString,&StrLen},

      { 32,10,0,"Ketchup Killers",NULL},
      { 55,11,0,"Search Function",NULL},

      {15,13,2,NULL,NULL},                // 1:Ok
      {45,13,3,NULL,NULL},
      };

struct TmtWin F = {
    3,4,76,19,
    "Search file(s)"};

int n;

strcpy(SearchOld,SearchName);



n=WinTraite(T,18,&F);

if (n!=27)  // pas escape
    {
    if (T[n].type!=3) return 0;  // pas cancel
    }

strcpy(SearchName,SearchOld);
return 1;       // Erreur
}


void Search(void)
{
int n;
char *nom;

char a;
int pos;

int m;          // position de la premiere ligne
int d,fin;      // debut et fin de la ligne

int prem;       // premier visible

if (WinSearch()==1) return;

nbr=0;
nbrmax=200;

SaveEcran();
PutCur(32,0);

ColLin(0,0,80,1*16+4);

// Allocate Memory

nom=GetMem(255);
nomtemp=GetMem(255);

tabnom=GetMem(sizeof(char *)*nbrmax);
for (n=0;n<nbrmax;n++)
    tabnom[n]=GetMem(255);

tabpath=GetMem(sizeof(char *)*nbrmax);
for (n=0;n<nbrmax;n++)
    tabpath[n]=GetMem(255);

tabtime=GetMem(sizeof(int *)*nbrmax);
tabdate=GetMem(sizeof(int *)*nbrmax);

// Setup of all

WinCadre(0,1,79,(Cfg->TailleY)-2,1);
ColWin(1,2,78,(Cfg->TailleY)-3,10*16+1);
ChrWin(1,2,78,(Cfg->TailleY)-3,32);

touche=0;

switch(sw)
    {
    case 3: // Current drive
        sprintf(nom,"%c:",DFen->path[0]);
        cherdate(nom);
        break;
    case 4: // Current dir & subdir
        strcpy(nom,DFen->path);
        cherdate(nom);
        break;
    case 5: // Current dir
        strcpy(nom,DFen->path);
        cherdate(nom);
        break;
    case 6: // all drive
        for(n=0;n<26;n++)
            {
            if (VerifyDisk(n+1)==0)
                {
                sprintf(nom,"%c:",n+'A');
                cherdate(nom);
                }
            }
        break;
    case 7: // user defined drive
        for(n=0;n<strlen(Drive);n++)
            {
            m=toupper(Drive[n])-'A';
            if (VerifyDisk(m+1)==0)
                {
                sprintf(nom,"%c:",m+'A');
                cherdate(nom);
                }
            }
        break;
    }

if (nbr==0)
    {
    WinError("No file found");
    ChargeEcran();
    }
    else
    {

m=2;

d=1;
fin=78;

prem=0;
pos=0;

do
    {
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

    for (n=d;n<=fin;n++)
        AffCol(n,pos+m-prem,7*16+5);

    a=getch();

    for (n=d;n<=fin;n++)
        AffCol(n,pos+m-prem,10*16+1);


    if (a==0)   {
        a=getch();
        if (a==72)      pos--;
        if (a==80)      pos++;
        if (a==0x47)    pos=0;
        if (a==0x4F)    pos=nbr-1;
        if (a==0x51)    pos+=5;
        if (a==0x49)    pos-=5;
        }
    }
while ( (a!=27) & (a!=13) );


ChargeEcran();

if (a==13)
    CommandLine("#CD %s",tabpath[pos]);

for (n=0;n<DFen->nbrfic;n++)
    if (!stricmp(tabnom[pos],DFen->F[n]->name)) {
        DFen->pcur=n;
        DFen->scur=n;
        }
    }

// Free Memory

free(tabdate);
free(tabtime);

for (n=0;n<nbrmax;n++)
    free(tabpath[n]);
free(tabpath);

for (n=0;n<nbrmax;n++)
    free(tabnom[n]);
free(tabnom);

free(nomtemp);
free(nom);
}

