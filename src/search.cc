/*--------------------------------------------------------------------*\
|- Outil de recherche de fichiers                                     -|
\*--------------------------------------------------------------------*/
#include <ctype.h>
#include <io.h>

#include <stdio.h>
#include <string.h>

#include <conio.h>

//#include <time.h>

#include "kk.h"

#include "kkt\search.h"

extern FENETRE *Fenetre[4];     // uniquement pour trouver la 3‚me trash

static char **tabnom;
static char **tabpath;
static int *tabtime;
static int *tabdate;
static int *tabsize;

static int nbrmax;
static int nbr;

char SearchOld[80]="*.*";

#define BreakESC  if (KbHit()) touche=Wait(0,0); \
                                             else if (touche==27) break;

static int oldIOver;

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
    if (fgetc(fic)==_sbuf[12][m])
        {
        m++;
        if (_sbuf[12][m]==0) fin=0;
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

chaine[13]=179; // Cfg->Tfont;
chaine[23]=179; // Cfg->Tfont;
chaine[32]=179; // Cfg->Tfont;
chaine[38]=179; // Cfg->Tfont;

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

char ok;


TabRec=(char**)GetMem(500*sizeof(char*));

TabRec[0]=(char*)GetMem(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
m=strlen(TabRec[NbrRec-1]);
m= (m>=72) ? m-72 : 0;
PrintAt(0,0,"Go in  %-73s",TabRec[NbrRec-1]+m);

CommandLine("#cd %s",TabRec[NbrRec-1]);

strcpy(nom,TabRec[NbrRec-1]);

ok=(!stricmp(nom,DFen->path));

/*--------------------------------------------------------------------*\
|-  The files                                                         -|
\*--------------------------------------------------------------------*/

if (ok)
for (m=0;m<DFen->nbrfic;m++)
    {
    ff=GetFile(DFen,m);

    error=ff->attrib;

    if ( (ff->name[0]!='.') & (!WildCmp(ff->name,_sbuf[10])) )
        {
        cont=1;

        if (IsDir(ff)) cont=0; //--- Not Subdir ------------------------
        if ((error&0x08)==0x08) cont=0; //--- Not Subdir ---------------

        if ( ((_sbuf[12][0])!=0) & (cont==1) & (DFen->system==0) )
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

if (ok)
for (m=0;m<DFen->nbrfic;m++)
    {
    ff=GetFile(DFen,m);

    error=ff->attrib;

/*--------------------------------------------------------------------*\
|- Subdir                                                             -|
\*--------------------------------------------------------------------*/

    if ( (ff->name[0]!='.')  & (_cbuf[10]!=3) )
        {
        if ( (IsDir(ff))
           | ((!stricmp(getext(ff->name),"KKD")) & (KKCfg->enterkkd==1)
                                                 & (DFen->system==0)) )
            {
            strcpy(moi,nom);
            Path2Abs(moi,ff->name);

            TabRec[NbrRec]=(char*)GetMem(strlen(moi)+1);
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
if (!(Info->macro))
    {
    strcpy(_sbuf[10],SearchOld);
    strcpy(_sbuf[11],"");
    strcpy(_sbuf[12],"");

    _cbuf[10]=0;
    _cbuf[11]=KKCfg->enterkkd;
    }

NumHelp(5);
if (MWinTraite(search_kkt)==-1) //--- escape ou cancel -------------
    return 1;

strcpy(SearchOld,_sbuf[10]);
KKCfg->enterkkd=_cbuf[11];
return 0;
}


void Search(FENETRE *Fen)
{
int n;
static char nom[256];

int a;
int pos;

int m;          //--- position de la premiere ligne --------------------
int d,fin;      //--- debut et fin de la ligne -------------------------

int prem;       //--- premier visible ----------------------------------

int i,k;

FENETRE *SFen,*TempFen;

MacAlloc(10,80);
MacAlloc(11,80);
MacAlloc(12,80);

if (WinSearch()==1)
    {
    MacFree(10);
    MacFree(11);
    MacFree(12);

    return;
    }

oldIOver=IOver;
IOver=1;

SFen=AllocWin();
TempFen=AllocWin();

nbr=0;
nbrmax=200;

SaveScreen();
PutCur(32,0);

ColLin(0,0,Cfg->TailleX,1*16+4);
ChrLin(0,0,Cfg->TailleX,32);


/*--------------------------------------------------------------------*\
|-  Allocate Memory                                                   -|
\*--------------------------------------------------------------------*/

tabnom=(char**)GetMem(sizeof(char *)*nbrmax);
for (n=0;n<nbrmax;n++)
    tabnom[n]=(char*)GetMem(255);

tabpath=(char**)GetMem(sizeof(char *)*nbrmax);
for (n=0;n<nbrmax;n++)
    tabpath[n]=(char*)GetMem(255);

tabtime=(int*)GetMem(sizeof(int)*nbrmax);
tabdate=(int*)GetMem(sizeof(int)*nbrmax);
tabsize=(int*)GetMem(sizeof(int)*nbrmax);

/*--------------------------------------------------------------------*\
|-  Setup of all                                                      -|
\*--------------------------------------------------------------------*/

Bar(" Help  ----  View  ----  ----  ----  ---- Delete ----  ---- ");

(KKCfg->noprompt)=(char)((KKCfg->noprompt)|1);

Cadre(0,1,Cfg->TailleX-1,(Cfg->TailleY)-2,1,Cfg->col[55],Cfg->col[56]);
Window(1,2,Cfg->TailleX-2,(Cfg->TailleY)-3,Cfg->col[16]);

touche=0;

DFen=TempFen;

switch(_cbuf[10])
    {
    case 0: //--- Current drive ----------------------------------------
        sprintf(nom,"%c:\\",Fen->path[0]);
        CommandLine("#cd %s",nom);
        cherdate(nom);
        break;
    case 1: //--- Current dir & subdir ---------------------------------
        strcpy(nom,Fen->path);
        cherdate(nom);
        break;
    case 2: //--- Selected dir & subdir --------------------------------
        for(n=0;n<Fen->nbrfic;n++)
            {
            struct file *F;
            F=GetFile(Fen,n);

            if ( (F->select) | ((Fen->nbrsel==0) & (Fen->pcur==n)))
                {
                strcpy(nom,Fen->path);
                Path2Abs(nom,F->name);
                cherdate(nom);
                }
            }
        break;
    case 3: //--- Current dir ------------------------------------------
        strcpy(nom,Fen->path);
        cherdate(nom);
        break;
    case 4: //--- all drive --------------------------------------------
        for(n=0;n<26;n++)
            {
            if (DriveReady((char)n)==1)
                {
                sprintf(nom,"%c:\\",n+'A');
                CommandLine("#cd %s",nom);
                cherdate(nom);
                }
            }
        break;
    case 5: //--- user defined drive -----------------------------------
        for(n=0;n<strlen(_sbuf[11]);n++)
            {
            m=toupper(_sbuf[11][n])-'A';
            if (DriveReady((char)m)==1)
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
    LoadScreen();
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
            AffCol(n,pos+m-prem,Cfg->col[18]);

        a=Wait(0,0);

        for (n=d;n<=fin;n++)
            AffCol(n,pos+m-prem,Cfg->col[16]);

#ifndef NOMOUSE
        if (a==0)     //--- Pression bouton souris ---------------------
            {
            int button;

            button=MouseButton();

            if ((button&1)==1)     //--- gauche ------------------------
                {
                int x,y;

                x=MousePosX();
                y=MousePosY();


                if (y==Cfg->TailleY-1)
                    if (Cfg->TailleX==90)
                        a=(0x3B+(x/9))*256;
                        else
                        a=(0x3B+(x/8))*256;
                    else
                    {
                    pos=y-m+prem;
                    ReleaseButton();
                    }
                }

            if ((button&2)==2)     //--- droite ---------------------------
                {
                a=27;
                }
            }
#endif


        switch(HI(a))
            {
            case 72:
                pos--;
                break;
            case 80:
                pos++;
                break;
            case 0x47:
                pos=0;
                break;
            case 0x4F:
                pos=nbr-1;
                break;
            case 0x51:
                pos+=5;
                break;
            case 0x49:
                pos-=5;
                break;
            case 0x3D:      //--- F3 -----------------------------------
            case 0x8D:      //--- CTRL-UP ------------------------------
                if (strcmp(tabpath[pos],"*")!=0)
                    {
                    DFen=SFen;
                    CommandLine("#CD %s",tabpath[pos]);
                    k=-1;
                    for (i=0;i<DFen->nbrfic;i++)
                        if (!WildCmp(tabnom[pos],GetFilename(DFen,i)))
                            {
                            DFen->pcur=i;
                            DFen->scur=i;
                            k=i;
                            break;
                            }

                    if (k==-1)
                        WinError("Error");
                        else
                        {
                        AccessFile(k);

                        GestionFct(8);
                        }
                    }
                break;
            case 0x42:      //--- F8 -----------------------------------
                KKCfg->noprompt=(char)((KKCfg->noprompt)&254);   // bit 0 = 0
                for (n=0;n<nbr;n++)
                    {
                    if (strcmp(tabpath[n],"*")!=0)
                        {
                        DFen=SFen;
                        CommandLine("#CD %s",tabpath[n]);
                        k=-1;
                        for (i=0;i<DFen->nbrfic;i++)
                            if (!WildCmp(tabnom[n],GetFilename(DFen,i)))
                                {
                                DFen->pcur=i;
                                DFen->scur=i;
                                k=i;
                                break;
                                }
                        if (k==-1)
                            WinError("Error");
                            else
                            {
                            k=Delete(SFen);
                            if (k==1) break;
                            if (k==4)
                                KKCfg->noprompt=(char)((KKCfg->noprompt)|1);
                            if ( (k==4) | (k==3) )
                                {
                                strcpy(tabnom[n],"[DELETED]");
                                strcpy(tabpath[n],"*");
                                }
                            }
                        }
                    }
                break;
            }
        }
    while ( (a!=27) & (a!=13) );

    LoadScreen();

    if ( (a==13) & (strcmp(tabpath[pos],"*")!=0) )
        {
        DFen=Fen;
        CommandLine("#CD %s",tabpath[pos]);

        for (n=0;n<DFen->nbrfic;n++)
            if (!WildCmp(tabnom[pos],GetFilename(DFen,n)))
                {
                DFen->pcur=n;
                DFen->scur=n;
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

FreeWin(SFen);
FreeWin(TempFen);

IOver=oldIOver;

MacFree(10);
MacFree(11);
MacFree(12);
}

