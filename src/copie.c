// Copy-function

#include <ctype.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>


#include <io.h>
#include <direct.h>

#include <dos.h>
#include <fcntl.h>

#include "win.h"
#include "kk.h"

int recopy(char *inpath,char *outpath,struct file *F);
int Copytree(char *inpath,char *outpath);
int truecopy(char *inpath,char *outpath);

void ArjCopie(struct fenetre *F1,struct fenetre *F2);
void RarCopie(struct fenetre *F1,struct fenetre *F2);

static int FicEcrase;



// Copytree
//----------

int Copytree(char *inpath,char *outpath)
{
struct find_t ff;
char error;

Path2Abs(inpath,"*.*");
Path2Abs(outpath,"*.*");
error=_dos_findfirst(inpath,63,&ff);
Path2Abs(inpath,"..");
Path2Abs(outpath,"..");

mkdir(outpath);

while(error==0)  {
    error=ff.attrib;

    if (ff.name[0]!='.')
        {
        Path2Abs(inpath,ff.name);
        Path2Abs(outpath,ff.name);
        if ((error&0x10)==0x10)
            Copytree(inpath,outpath);
            else
            truecopy(inpath,outpath);
        Path2Abs(inpath,"..");
        Path2Abs(outpath,"..");
        }
    error=_dos_findnext(&ff);

    if (FicEcrase==2) return 0;
    }

return 1;
}


// Copie convenablement
//----------------------

int recopy(char *inpath,char *outpath,struct file *F)
{
int i;

if ((F->attrib & _A_SUBDIR)==_A_SUBDIR)
    i=Copytree(inpath,outpath);
    else
    i=truecopy(inpath,outpath);

if (FicEcrase==2) return 0;

return i;
}


// Impossible de copier
void ProtFile(char *path)
{
static int CadreLength=71;
static int Dir[256];

struct Tmt T[5] = {
      {15,5,2,NULL,NULL}, // Copy All
      {45,5,3,NULL,NULL},
      { 5,3,0,Dir,NULL},
      { 5,2,0,"Couldn't create file",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,8,76,15,
    "Copy"};

int n;

if (FicEcrase==1) return;

memcpy(Dir,path,255);

n=WinTraite(T,5,&F);

switch(n)   {
    case 27:
    case 1:
        FicEcrase=2;    // Cancel
    case 0:
        break;
    default:
        PrintAt(0,0,"(%d)",n);
    }
}



// Fichier existe d‚j…: Renvoie 0 si il faut l'ecraser
int FileExist(char *path)
{
static int CadreLength=71;
static int Dir[256];

struct Tmt T[7] = {
      {5,5,5, "     Yes     ",NULL}, // Copy
      {22,5,5,"     No      ",NULL}, // No replace
      {39,5,5,"   ALL Yes   ",NULL}, // Copy All
      {56,5,3,NULL,NULL},
      { 5,3,0,Dir,NULL},
      { 5,2,0,"Overwrite file ?",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,8,76,15,
    "Copy"};

int n;

if (FicEcrase==1) return 0;

memcpy(Dir,path,255);

n=WinTraite(T,7,&F);

switch(n)   {
    case 0:
        return 0;       // Replace
        break;
    case 27:
    case 1:
        return 1;       // Cancel
    case 2:
        FicEcrase=1;
        return 0;       // Replace ALL
        break;
    case 3:
        FicEcrase=2;
        return 1;       // Cancel ALL
    default:
        PrintAt(0,0,"(%d)",n);
    }
return 0;
}


// Vrai copie de FICHIER
//-----------------------
int truecopy(char *inpath,char *outpath)
{
long size;
long Taille,TailleEnreg;

FILE *inhand,*outhand;

char *buffer;

char ok;

int j3;
long TailleRest;

ok=1;

PrintAt(9,5,"In   %59s",inpath);
PrintAt(9,6,"From %59s",outpath);

inhand=fopen(inpath,"rb");
if (inhand==NULL)
    ok=0;
    else
    {
    struct diskfree_t d;
    long tfree;

    size=filelength(fileno(inhand));

    _dos_getdiskfree(toupper(outpath[0])-'A'+1,&d);

    tfree=(d.avail_clusters)*(d.sectors_per_cluster);
    tfree=tfree*(d.bytes_per_sector);

    Taille=(d.bytes_per_sector);

    if (Taille<=0) Taille=32*1024;

    if (tfree<size)
        {
        fclose(inhand);
        if (WinError("No more place on drive")==1)
            FicEcrase=2;
        ok=0;
        }
    }



if (ok==1)
    {
    outhand=fopen(outpath,"rb");
    if (outhand!=NULL)
        {
        fclose(outhand);
        if (FileExist(outpath)==1)
            ok=0;
        }
    }

IOver=1;
IOerr=0;

if (ok==1)
    {
    outhand=fopen(outpath,"wb");
    if ( (outhand==NULL) | (IOerr!=0) )
        {
        ProtFile(outpath);
        fclose(inhand);
        ok=0;
        }
    IOver=0;
    }

if (ok==1)
    {
    if (Taille>size)
        TailleEnreg=size;
        else
        TailleEnreg=Taille;

    Taille=size;

    buffer=malloc(TailleEnreg);

    TailleRest=0;

    j3=Gradue(10,12,60,0,TailleRest,size);

    while (Taille>0)
        {
        IOerr=0;
        TailleEnreg=fread(buffer,1,TailleEnreg,inhand);
        if (IOerr==3) { ok=1; break; }
        IOerr=0;
        fwrite(buffer,1,TailleEnreg,outhand);
        if (IOerr==3) { ok=1; break; }
        Taille-=TailleEnreg;
        TailleRest+=TailleEnreg;
        PrintAt(12,9,"Copying %9d of %9d",TailleRest,size);

        j3=Gradue(10,12,60,j3,TailleRest,size);
        }

    Gradue(10,12,60,j3,size,size);

    fclose(inhand);
    fclose(outhand);
    }

if (ok==1)
    {
    unsigned short d,t;
    int hand1,hand2;

    _dos_open(inpath,O_RDONLY,&hand1);
    _dos_open(outpath,O_RDONLY,&hand2);

    _dos_getftime(hand1,&d,&t);
    _dos_setftime(hand2,d,t);

    _dos_close(hand1);
    _dos_close(hand2);
    }



free(buffer);

return ok;
}


// retourne 0 si pas copie
//-------------------------

int FenCopie(char *dest)
{

static int CadreLength=71;
static int Dir[256];

struct Tmt T[5] = {
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,3,0,Dir,NULL},
      { 5,2,0,"Copy files to",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,10,76,17,
    "Copy"};

int n;

memcpy(Dir,dest,255);

n=WinTraite(T,5,&F);

if (n==0)
    return 1;
    else
    return 0;

}

void CopieRar(struct fenetre *F1,struct fenetre *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];

fic=fopen(Fics->temp,"wt");

strcpy(nom,F1->path);

if (nom[strlen(nom)-1]!='\\')  strcat(nom,"\\");

for(i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];

// inserer path from

    if ((F->select)==1)
        {
        F->select=0;
        F1->nbrsel--;
        F1->taillesel-=F->size;

        if (((F->attrib)&0x10)==0x10)
            fprintf(fic,"%s%s\\*.*\n",nom,F->name);
            else
            fprintf(fic,"%s%s\n",nom,F->name);
        }
    }
fclose(fic);

strcpy(nom,F2->path+strlen(F2->VolName));

CommandLine("#RAR a -ep1 -std %s @%s %s",F2->VolName,Fics->temp,nom);
}

void RarCopie(struct fenetre *F1,struct fenetre *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];

char option;


option='E';

fic=fopen(Fics->temp,"wt");

if (strlen(F1->path)==strlen(F1->VolName))
    strcpy(nom,"");
    else
    {
    strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
    strcat(nom,"\\");

    }

for(i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];

    if ((F->select)==1)
        {
        F->select=0;
        F1->nbrsel--;
        F1->taillesel-=F->size;

        if (((F->attrib)&0x10)==0x10)
            {
            fprintf(fic,"%s%s\\*.*\n",nom,F->name);
            option='X';
            }
            else
            fprintf(fic,"%s%s\n",nom,F->name);
        }
    }
fclose(fic);

CommandLine("#RAR %c -std %s @%s %s",option,F1->VolName,Fics->temp,F2->path);
}

void ZipCopie(struct fenetre *F1,struct fenetre *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];

char option;


option='e';

fic=fopen(Fics->temp,"wt");

if (strlen(F1->path)==strlen(F1->VolName))
    strcpy(nom,"");
    else
    {
    strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
    strcat(nom,"\\");

    }

for(i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];

    if ((F->select)==1)
        {
        F->select=0;
        F1->nbrsel--;
        F1->taillesel-=F->size;

        if (((F->attrib)&0x10)==0x10)
            {
            fprintf(fic,"%s%s\\*.*\n",nom,F->name);
            option='d';
            }
        else
            fprintf(fic,"%s%s\n",nom,F->name);
        }
    }
fclose(fic);

// Execution of uncompressor

CommandLine("#PKUNZIP -%c %s @%s %s",option,F1->VolName,Fics->temp,F2->path);
}

void ArjCopie(struct fenetre *F1,struct fenetre *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];

char option;


option='E';

fic=fopen(Fics->temp,"wt");

if (strlen(F1->path)==strlen(F1->VolName))
    strcpy(nom,"");
    else
    {
    strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
    strcat(nom,"\\");

    }

for(i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];

    if ((F->select)==1)
        {
        F->select=0;
        F1->nbrsel--;
        F1->taillesel-=F->size;

        if (((F->attrib)&0x10)==0x10)
            {
            fprintf(fic,"%s%s\\*.*\n",nom,F->name);
            option='X';
            }
            else
            fprintf(fic,"%s%s\n",nom,F->name);
        }
    }
fclose(fic);

CommandLine("#ARJ %c -P %s %s !%s",option,F1->VolName,F2->path,Fics->temp);
}


void Copie(struct fenetre *F1,struct fenetre *F2)   // Copie Multiple
{
int i;
int TailleTotale;
int TailleRest;

int j1,j2;                          // postion du compteur (read,write)

char inpath[128],outpath[128];

struct file *F;

FicEcrase=0;

if ( (F1->nbrsel==0) & (F1->F[F1->pcur]->name[0]!='.') ) {
    F1->F[F1->pcur]->select=1;
    F1->nbrsel++;
    F1->taillesel+=F1->F[F1->pcur]->size;
    }

switch(F1->system)  {
    case 1:
        RarCopie(F1,F2);
        break;
    case 2:
        ArjCopie(F1,F2);
        break;
    case 3:
        ZipCopie(F1,F2);
        break;
    case 0:
        break;
    default:
        YouMad("Copie");
        return;
        break;
        }

switch(F2->system)  {
    case 0:
        break;
    case 1:
        CopieRar(F1,F2);
        break;
    default:
        YouMad("Copie");
        return;
        break;
        }

if ( (!strcmp(F1->path,F2->path)) & (F1->drive==F2->drive))
    return;

TailleTotale=F1->taillesel;
TailleRest=0;

if (FenCopie(F2->path)==0) return;

F=F1->F[F1->pcur];

SaveEcran();

WinCadre(7,4,73,15,0);
ColWin(8,5,72,14,10*16+1);
ChrWin(8,5,72,14,32);

WinCadre(9,8,71,10,1);
WinCadre(9,11,71,13,2);


j1=0;
j2=0;

for (i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];
    if ((F->select)==1)
        {
        strcpy(inpath,F1->path);
        if (inpath[strlen(inpath)-1]!='\\') strcat(inpath,"\\");
        strcat(inpath,F->name);

        strcpy(outpath,F2->path);
        if (outpath[strlen(outpath)-1]!='\\') strcat(outpath,"\\");
        strcat(outpath,F->name);

        if (recopy(inpath,outpath,(F1->F[i]))==1)
            {
            F1->F[i]->select=0;
            F1->nbrsel--;
            (F1->taillesel)-=F1->F[i]->size;
            }
        }
    if (FicEcrase==2) break;
    }

// Fenˆtre 1 ne change pas


ChargeEcran();

DFen=F2;
CommandLine("#cd .");
DFen=F1;
}

