/*--------------------------------------------------------------------*\
|-   Copy Function                                                    -|
\*--------------------------------------------------------------------*/
#include <ctype.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>


#include <io.h>
#include <direct.h>

#include <fcntl.h>

#include <time.h>

#include "win.h"
#include "kk.h"

int LongGradue(int x,int y,int length,int from,int to,int total);

int recopy(char *inpath,char *outpath,struct file *F);
int Copytree(char *inpath,char *outpath);
int truecopy(char *inpath,char *outpath);

void ArjCopie(struct fenetre *F1,struct fenetre *F2);
void RarCopie(struct fenetre *F1,struct fenetre *F2);
void ZipCopie(struct fenetre *F1,struct fenetre *F2);

int RemoveM(char *inpath,char *outpath,struct file *F);
int Movetree(char *inpath,char *outpath);
int Truemove(char *inpath,char *outpath);

static int FicEcrase;
static char Dir[256];
static char temp[256];

int FenCopie(struct fenetre *F1,struct fenetre *F2);
int FenMove(struct fenetre *F1,struct fenetre *F2);



/*--------------------------------------------------------------------*\
|- Convertit un masque en ce qui faut ;)                              -|
\*--------------------------------------------------------------------*/
void MaskCnv(char *path)
{
if (!strcmp(temp,"*.*")) return;

Path2Abs(path,"..");
Path2Abs(path,temp);
}


/*--------------------------------------------------------------------*\
|- Copytree                                                           -|
\*--------------------------------------------------------------------*/

int Copytree(char *inpath,char *outpath)
{
DIR *dirp;
struct dirent *ff;

char error;

Path2Abs(inpath,"*.*");
Path2Abs(outpath,"*.*");
dirp=opendir(inpath);
Path2Abs(inpath,"..");
Path2Abs(outpath,"..");

mkdir(outpath);

if (dirp!=NULL)
    while(1)
    {
    ff=readdir(dirp);
    if (ff==NULL) break;

    error=ff->d_attr;

    if (ff->d_name[0]!='.')
        {
        Path2Abs(inpath,ff->d_name);
        Path2Abs(outpath,ff->d_name);
        if ((error&0x10)==0x10)
            Copytree(inpath,outpath);
            else
            truecopy(inpath,outpath);
        Path2Abs(inpath,"..");
        Path2Abs(outpath,"..");
        }
    if (FicEcrase==2) return 0;
    }

closedir(dirp);

return 1;
}


/*--------------------------------------------------------------------*\
|- Copie convenablement                                               -|
\*--------------------------------------------------------------------*/

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


/*--------------------------------------------------------------------*\
|- Impossible de copier                                               -|
\*--------------------------------------------------------------------*/
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



/*--------------------------------------------------------------------*\
|-  Fichier existe d‚j…: Renvoie 0 si il faut l'ecraser               -|
\*--------------------------------------------------------------------*/


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


/*--------------------------------------------------------------------*\
|- Vrai copie de FICHIER                                              -|
\*--------------------------------------------------------------------*/
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

MaskCnv(outpath);

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

    // if (Taille<=0)

    Taille=32*1024;

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

    j3=LongGradue(10,12,60,0,TailleRest,size);

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

        j3=LongGradue(10,12,60,j3,TailleRest,size);
        }

    LongGradue(10,12,60,j3,size,size);

    free(buffer);

    fclose(inhand);
    fclose(outhand);
    }

if (ok==1)      // Mise a l'heure
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





return ok;
}


/*--------------------------------------------------------------------*\
|- retourne 0 si pas copie                                            -|
\*--------------------------------------------------------------------*/

int FenCopie(struct fenetre *F1,struct fenetre *FTrash)
{
static int DirLength=70;
static int CadreLength=71;
static int n,m,o;

struct Tmt T[5] = {
      { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},           // le OK
      {45,5,3,NULL,NULL},           // le CANCEL
      { 5,2,0,"Copy files to",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {3,10,76,17,"Copy"};

memcpy(Dir,FTrash->path,255);

n=0;
if (((Cfg->noprompt)&1)==0)
    n=WinTraite(T,5,&F);

Path2Abs(Dir,".");

if (n!=27)  // pas escape
    {
    o=-1;
    for (m=0;m<strlen(Dir);m++)
        if ( (Dir[m]=='\\') | (Dir[m]=='/') ) o++;

    if (o==-1)
        {
        strcpy(temp,F1->path);
        Path2Abs(temp,Dir);
        strcpy(Dir,temp);
        }

    strcpy(temp,"*.*");

    if (T[n].type!=3) // Pas cancel
        {
        if (!strcmp(Dir,F1->path)) return 1;

        if (chdir(Dir)!=0)
            {
            strcpy(temp,Dir);
            Path2Abs(temp,"..");
            if (chdir(temp)!=0)
                {
                WinError("Unknown path");
                return 0;
                }
            FileinPath(Dir,temp);
            Path2Abs(Dir,"..");
            }

        DFen=FTrash;
        CommandLine("#cd %s",Dir);

        return 1;
        }
    }

return 0;       // Erreur
}

/*--------------------------------------------------------------------*\
|- retourne 0 si pas move                                             -|
\*--------------------------------------------------------------------*/
int FenMove(struct fenetre *F1,struct fenetre *FTrash)
{
static int DirLength=70;
static int CadreLength=71;
static char Dir[256];
static int n,m,o;

struct Tmt T[5] = {
      { 5,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Move files to",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {3,10,76,17,"Move"};

memcpy(Dir,FTrash->path,255);

n=0;
if (((Cfg->noprompt)&1)==0)
    n=WinTraite(T,5,&F);

Path2Abs(Dir,".");

if (n!=27)  // pas escape
    {
    o=-1;
    for (m=0;m<strlen(Dir);m++)
        if ( (Dir[m]=='\\') | (Dir[m]=='/') ) o++;

    if (o==-1)
        {
        strcpy(temp,F1->path);
        Path2Abs(temp,Dir);
        strcpy(Dir,temp);
        }

    strcpy(temp,"*.*");

    if (T[n].type!=3) // Pas cancel
        {
        if (!strcmp(Dir,F1->path)) return 1;

        if (chdir(Dir)!=0)
            {
            strcpy(temp,Dir);
            Path2Abs(temp,"..");
            if (chdir(temp)!=0)
                {
                WinError("Unknown path");
                return 0;
                }
            FileinPath(Dir,temp);
            Path2Abs(Dir,"..");
            }

        DFen=FTrash;
        CommandLine("#cd %s",Dir);

        return 1;
        }
    }

return 0;       // Erreur
}

void CopieRar(struct fenetre *F1,struct fenetre *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];
char player[256];

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

if (PlayerIdf(player,30)==0)
    {
    CommandLine("#%s a -ep1 -std %s @%s %s",player,F2->VolName,
                                                        Fics->temp,nom);
    }
}

void RarCopie(struct fenetre *F1,struct fenetre *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];
char player[256];

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

if (PlayerIdf(player,34)==0)
    {
    CommandLine("#%s %c -std -y %s @%s %s",player,option,
                                       F1->VolName,Fics->temp,F2->path);
    }
}

void ZipCopie(struct fenetre *F1,struct fenetre *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];
char player[256];

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

if (PlayerIdf(player,35)==0)
    {
    CommandLine("#%s -%c -o %s @%s %s",player,option,F1->VolName,
                                                   Fics->temp,F2->path);
    }                    // Overwrite newer file
}

void ArjCopie(struct fenetre *F1,struct fenetre *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];
char player[256];

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
if (PlayerIdf(player,30)==0)
    {
    CommandLine("#%s %c -P -y %s %s !%s",player,option,F1->VolName,
                                                   F2->path,Fics->temp);
    }
}










/*--------------------------------------------------------------------*\
|- Movetree                                                           -|
\*--------------------------------------------------------------------*/
int Movetree(char *inpath,char *outpath)
{
int error;
DIR *dirp;
struct dirent *ff;

int i;

Path2Abs(inpath,"*.*");
Path2Abs(outpath,"*.*");
dirp=opendir(inpath);
Path2Abs(inpath,"..");
Path2Abs(outpath,"..");

mkdir(outpath);

if (dirp!=NULL)
    while(1)
    {
    ff=readdir(dirp);
    if (ff==NULL) break;

    error=ff->d_attr;

    if (ff->d_name[0]!='.')
        {
        Path2Abs(inpath,ff->d_name);
        Path2Abs(outpath,ff->d_name);
        if ((error&0x10)==0x10)
            {
            i=Movetree(inpath,outpath);
            rmdir(inpath);
            }
            else
            {
            if (Renome(inpath,outpath)!=0)
                {
                i=Truemove(inpath,outpath);
                if (i==1)
                    remove(inpath);
                }
                else
                {
                i=1;
                }
            }
        Path2Abs(inpath,"..");
        Path2Abs(outpath,"..");
        }
    if (FicEcrase==2) return 0;
    }

closedir(dirp);

return 1;
}

int Renome(char *inpath,char *outpath)
{
int i;

MaskCnv(outpath);

PrintAt(9,5,"In   %59s",inpath);
PrintAt(9,6,"From %59s",outpath);

i=rename(inpath,outpath);

return i;
}


/*--------------------------------------------------------------------*\
|- Move convenablement                                                -|
\*--------------------------------------------------------------------*/
int RemoveM(char *inpath,char *outpath,struct file *F)
{
int i;

if ((F->attrib & _A_SUBDIR)==_A_SUBDIR)
    {
    i=Movetree(inpath,outpath);
    rmdir(inpath);
    }
    else
    {
    if (Renome(inpath,outpath)!=0)
        {
        i=Truemove(inpath,outpath);
        if (i==1)
            remove(inpath);
        }
        else
        {
        i=1;
        }
    }

if (FicEcrase==2) return 0;

return i;
}


/*--------------------------------------------------------------------*\
|- Impossible de mover                                                -|
\*--------------------------------------------------------------------*/
void ProtFileM(char *path)
{
static int CadreLength=71;
static int Dir[256];

struct Tmt T[5] = {
      {15,5,2,NULL,NULL}, // Move All
      {45,5,3,NULL,NULL},
      { 5,3,0,Dir,NULL},
      { 5,2,0,"Couldn't create file",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,8,76,15,
    "Move"};

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



/*--------------------------------------------------------------------*\
|-  Fichier existe d‚j…: Renvoie 0 si il faut l'ecraser               -|
\*--------------------------------------------------------------------*/
int FileExistM(char *path)
{
static int CadreLength=71;
static int Dir[256];

struct Tmt T[7] = {
      {5,5,5, "     Yes     ",NULL},                             // Move
      {22,5,5,"     No      ",NULL},                       // No replace
      {39,5,5,"   ALL Yes   ",NULL},                         // Move All
      {56,5,3,NULL,NULL},
      { 5,3,0,Dir,NULL},
      { 5,2,0,"Overwrite file ?",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,8,76,15,
    "Move"};

int n;

if (FicEcrase==1) return 0;

memcpy(Dir,path,255);

n=WinTraite(T,7,&F);

switch(n)
    {
    case 0:
        return 0;                                             // Replace
        break;
    case 27:
    case 1:
        return 1;                                              // Cancel
    case 2:
        FicEcrase=1;
        return 0;                                         // Replace ALL
        break;
    case 3:
        FicEcrase=2;
        return 1;                                          // Cancel ALL
    default:
        PrintAt(0,0,"(%d)",n);
    }
return 0;
}


/*--------------------------------------------------------------------*\
|- Vrai move de FICHIER                                               -|
\*--------------------------------------------------------------------*/
int Truemove(char *inpath,char *outpath)
{
long size;
long Taille,TailleEnreg;

FILE *inhand,*outhand;

char *buffer;

char ok;

int j3;
long TailleRest;

ok=1;

MaskCnv(outpath);

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
        if (FileExistM(outpath)==1)
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
        ProtFileM(outpath);
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

    j3=LongGradue(10,12,60,0,TailleRest,size);

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
        PrintAt(12,9,"Moving %9d of %9d",TailleRest,size);

        j3=LongGradue(10,12,60,j3,TailleRest,size);
        }

    LongGradue(10,12,60,j3,size,size);

    free(buffer);

    fclose(inhand);
    fclose(outhand);
    }

if (ok==1)      // Mise a l'heure
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


return ok;
}




/*--------------------------------------------------------------------*\
|- Fonction de MOVE principale                                        -|
\*--------------------------------------------------------------------*/
void Move(struct fenetre *F1,struct fenetre *FTrash,char *path)
{
int i;
int TailleTotale;
int TailleRest;

int j1,j2;                           // postion du compteur (read,write)

char inpath[128],outpath[128];

struct file *F;

strcpy(FTrash->path,path);

FicEcrase=0;

if ( (F1->nbrsel==0) & (F1->F[F1->pcur]->name[0]!='.') )
    {
    F1->F[F1->pcur]->select=1;
    F1->nbrsel++;
    F1->taillesel+=F1->F[F1->pcur]->size;
    }

if (FenMove(F1,FTrash)==0) return;

if (!strcmp(F1->path,FTrash->path))
    return;

switch(F1->system)
    {
    case 0:
        break;
    default:
        YouMad("Move");
        return;
        break;
    }

switch(FTrash->system)
    {
    case 0:
        break;
    default:
        YouMad("Move");
        return;
        break;
    }



TailleTotale=F1->taillesel;
TailleRest=0;


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
        Path2Abs(inpath,F->name);

        strcpy(outpath,FTrash->path);
        Path2Abs(outpath,F->name);

        if (RemoveM(inpath,outpath,(F1->F[i]))==1)
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
}

/*--------------------------------------------------------------------*\
|- Fonction de COPY principale                                        -|
\*--------------------------------------------------------------------*/
void Copie(struct fenetre *F1,struct fenetre *FTrash,char *path)
{
int i;
int TailleTotale;
int TailleRest;

int j1,j2;                          // postion du compteur (read,write)

char inpath[128],outpath[128];

struct file *F;

FicEcrase=0;

strcpy(FTrash->path,path);

if ( (F1->nbrsel==0) & (F1->F[F1->pcur]->name[0]!='.') )
    {
    F1->F[F1->pcur]->select=1;
    F1->nbrsel++;
    F1->taillesel+=F1->F[F1->pcur]->size;
    }

if (FenCopie(F1,FTrash)==0) return;

if (!strcmp(F1->path,FTrash->path))
    return;

switch(F1->system)
    {
    case 1:
        RarCopie(F1,FTrash);
        break;
    case 2:
        ArjCopie(F1,FTrash);
        break;
    case 3:
        ZipCopie(F1,FTrash);
        break;
    case 0:
        break;
    default:
        YouMad("Copie");
        return;
        break;
    }

switch(FTrash->system)
    {
    case 0:
        break;
    case 1:
        CopieRar(F1,FTrash);
        break;
    default:
        YouMad("Copie");
        return;
        break;
    }


TailleTotale=F1->taillesel;
TailleRest=0;

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
    if (F->select==1)
        {
        strcpy(inpath,F1->path);
        Path2Abs(inpath,F->name);

        strcpy(outpath,FTrash->path);
        Path2Abs(outpath,F->name);

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
}

/*--------------------------------------------------------------------*\
|- Avancement de graduation                                           -|
|- Renvoit le prochain                                                -|
\*--------------------------------------------------------------------*/
int LongGradue(int x,int y,int length,int from,int to,int total)
{
short j1;
int j3;

if (total==0) return 0;

if ( (to>1000) & (total>1000) )
    {
    j3=(to/1000);
    j3=(j3*length)/(total/1000);
    }
    else
    j3=(to*length)/total;

if (j3>=(length)) j3=length+1;

j1=from;

for (;j1<j3;j1++)
    AffChr(j1+x,y,0xB2);

if (to==0)
    ChrLin(x,y,length+1,32);

return j1;
}
