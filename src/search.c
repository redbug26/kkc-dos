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
strcat(nomtemp,"\\*.*");

strcpy(nom,TabRec[NbrRec-1]);

// The files

error=_dos_findfirst(nomtemp,63,&ff);

while (error==0) {
    error=ff.attrib;

    if (ff.name[0]!='.')
        {
        if ((error&0x10)!=0x10)    // Not Subdir
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


    if (touche!=1)
        {
        touche=_bios_keybrd(0x1);
        touche=touche/256;
        }
        else
        break;

    error=_dos_findnext(&ff);
    }

free(TabRec[NbrRec-1]);
NbrRec--;

// The directories

error=_dos_findfirst(nomtemp,63,&ff);

while (error==0) {
    error=ff.attrib;

    if (ff.name[0]!='.')
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

    if (touche!=1)
        {
        touche=_bios_keybrd(0x1);
        touche=touche/256;
        }
        else
        break;

    error=_dos_findnext(&ff);
    }

if (touche!=1)
    {
    touche=_bios_keybrd(0x1);
    touche=touche/256;
    }
    else
    break;



}
while (NbrRec>0);

free(TabRec);

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

nbr=0;
nbrmax=200;

SaveEcran();


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
ColWin(1,2,78,(Cfg->TailleY)-3,0*16+1);
ChrWin(1,2,78,(Cfg->TailleY)-3,32);

sprintf(nom,"%c:",DFen->path[0]);



touche=0;

cherdate(nom);

if ((_bios_keybrd(0x11)/256)==1)
    getch();

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
        AffCol(n,pos+m-prem,0*16+1);


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
