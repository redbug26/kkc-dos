/*--------------------------------------------------------------------*\
|-   Copy Function                                                    -|
\*--------------------------------------------------------------------*/
#include <ctype.h>

#include <stdio.h>
#include <string.h>

#include <dos.h>

#include <io.h>
#include <direct.h>
#include <malloc.h>         //-- uniquement pour le free ---------------

#include <fcntl.h>

#include <time.h>

#include "kk.h"

/*--------------------------------------------------------------------*\
|- prototype                                                          -|
\*--------------------------------------------------------------------*/

int LongGradue(int x,int y,int length,int from,int to,int total);

int recopy(char *inpath,char *outpath,struct file *F);
int Copytree(char *inpath,char *outpath);
int truecopy(char *inpath,char *outpath);

void ArjCopie(FENETRE *F1,FENETRE *F2);
void LhaCopie(FENETRE *F1,FENETRE *F2);
void RarCopie(FENETRE *F1,FENETRE *F2);
void ZipCopie(FENETRE *F1,FENETRE *F2);
void KkdCopie(FENETRE *F1,FENETRE *F2);

void CopieRar(FENETRE *F1,FENETRE *F2);
void CopieKkd(FENETRE *F1,FENETRE *F2);

int RemoveM(char *inpath,char *outpath,struct file *F);
int Movetree(char *inpath,char *outpath);
int Truemove(char *inpath,char *outpath);

int FenCopie(FENETRE *F1,FENETRE *F2);
int FenMove(FENETRE *F1,FENETRE *F2);

int UserInt(void);      //--- Interruption par l'utilisateur -----------

void CountRepSize(FENETRE *F1,FENETRE *FTrash,int *nbr,int *size);

/*--------------------------------------------------------------------*\
|- internal variable                                                  -|
\*--------------------------------------------------------------------*/

static int Nbrfic,Sizefic,Nbrcur,Sizecur;
static int Clock_Dep;
static int FicEcrase;
static char Dir[256];
static char temp[256];
static char bufton[32];
static long SizeMaxRecord;
static char noselect;              // prend le celui qui est highlighted
extern FENETRE *Fenetre[4];     // uniquement pour trouver la 3‚me trash


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

Path2Abs(inpath,"*.*");     Path2Abs(outpath,"*.*");
dirp=opendir(inpath);
Path2Abs(inpath,"..");      Path2Abs(outpath,"..");

mkdir(outpath);

if (Cfg->_Win95==1)
    UpdateLongName(inpath,outpath);

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

struct TmtWin F = { 3,8,76,15,"Copy" };

if (FicEcrase==1) return;

memcpy(Dir,path,255);

switch(WinTraite(T,5,&F))
    {
    case 27:
    case 1:
        FicEcrase=2;    // Cancel
    case 0:
        break;
    }
}


/*--------------------------------------------------------------------*\
|- User interrupt                                                     -|
\*--------------------------------------------------------------------*/
int UserInt(void)
{
static int CadreLength=71;

struct Tmt T[5] = {
      {5,4,5, "     Yes     ",NULL}, // Copy
      {22,4,5,"     No      ",NULL}, // No replace
      {56,4,3,NULL,NULL},
      { 5,2,0,"Do you will copy this file ?",NULL},
      { 1,1,6,NULL,&CadreLength}
      };

struct TmtWin F = { 3,8,76,14, "User Interrupt" };

int n;

n=WinTraite(T,5,&F);

switch(n)   {
    case 0:
        return 0;       // Replace
        break;
    case 27:
    case 1:
        return 1;       // Cancel
    case 2:
        FicEcrase=2;
        return 1;       // Cancel ALL
    }
return 0;
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

struct TmtWin F = { 3,8,76,15, "Copy" };

if (FicEcrase==1) return 0;

memcpy(Dir,path,255);

switch(WinTraite(T,7,&F))
    {
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
    }
return 0;
}

int FicCopy(FILE *infic,FILE *outfic,long size)
{
char fin;
long TailleEnreg,TailleRest,Taille;
int j3,j4;
char *buffer;

if (Nbrfic!=0)
    {
    j4=LongGradue(10,15,60,0,Sizecur,Sizefic);
    PrintAt(52,14,"(%4d of %4d)",Nbrcur+1,Nbrfic);
    }

Taille=size;
TailleEnreg=SizeMaxRecord;

buffer=GetMem(TailleEnreg);

TailleRest=0;

j3=LongGradue(10,10,60,0,TailleRest,size);

fin=0;

while ((fin==0) & (Taille>0))
    {
    if (Taille<SizeMaxRecord) TailleEnreg=Taille;

    IOerr=0;
    TailleEnreg=fread(buffer,1,TailleEnreg,infic);
    if (IOerr==3)
        {
        fin=1;
        break;
        }

    IOerr=0;
    fwrite(buffer,1,TailleEnreg,outfic);
    if (IOerr==3)
        {
        fin=1;
        break;
        }

    Taille-=TailleEnreg;
    TailleRest+=TailleEnreg;
    
    PrintAt(12,9,"Copying %9d of %9d",TailleRest,size);

    j3=LongGradue(10,10,60,j3,TailleRest,size);

    if (Nbrfic!=0)
        {
        PrintAt(12,14,"Copying %9d of %9d",Sizecur+TailleRest,Sizefic);
        j4=LongGradue(10,15,60,j4,Sizecur+TailleRest,Sizefic);

        if (Cfg->esttime==1)
            {
            int n1,n2;

            if (TailleRest>2048)
                {
                n2=(clock()-Clock_Dep)*(Sizefic/2048);
                n2=n2/((Sizecur+TailleRest)/2048);
                n1=n2+Clock_Dep-clock();

                n1=n1/CLOCKS_PER_SEC;
                n2=n2/CLOCKS_PER_SEC;

                PrintAt(20,12,
                        "Remaining: %6d sec. Expected: %6d sec.",n1,n2);
                }
            }
        }

    if (KbHit())
        {
        Wait(0,0,0);
        fin=UserInt();
        }
    }

LongGradue(10,10,60,j3,size,size);

free(buffer);

return fin;
}



/*--------------------------------------------------------------------*\
|- Vrai copie de FICHIER                                              -|
\*--------------------------------------------------------------------*/
int truecopy(char *inpath,char *outpath)
{
char fin=0;
long size;

FILE *inhand,*outhand;

char ok;

ok=1;

MaskCnv(outpath);

PrintAt(9,5,"From %59s",inpath);
PrintAt(9,6,"To   %59s",outpath);

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

    SizeMaxRecord=(d.bytes_per_sector);

    SizeMaxRecord=32*1024;

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
        if (((Cfg->noprompt)&1)==0)
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
    fin=FicCopy(inhand,outhand,size);

    fclose(inhand);
    fclose(outhand);
    }

if ( (ok==1) & (fin==0) )      //--- Mise a l'heure --------------------
    {
    unsigned short d,t;
    int handle;

    _dos_open(inpath,O_RDONLY,&handle);
    _dos_getftime(handle,&d,&t);
    _dos_close(handle);

    _dos_open(outpath,O_RDONLY,&handle);
    _dos_setftime(handle,d,t);
    _dos_close(handle);

    if (Cfg->_Win95==1)
        UpdateLongName(inpath,outpath);
    }

if (Nbrfic!=0)
    {
    Nbrcur++;
    Sizecur+=size;
    }

if (fin==1)
    {
    remove(outpath);
    ok=0;
    }

return ok;
}


/*--------------------------------------------------------------------*\
|- retourne 0 si pas copie                                            -|
\*--------------------------------------------------------------------*/

int FenCopie(FENETRE *F1,FENETRE *FTrash)
{
static int DirLength=70;
static int CadreLength=71;
static int n,m,o;
static char buffer[80];

struct Tmt T[5] = {
      { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},           // le OK
      {45,5,3,NULL,NULL},           // le CANCEL
      { 5,2,0,buffer,NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = { 3,10,76,17,"Copy" };

if (Nbrfic!=0)
    {
    if (Nbrfic==1)
        sprintf(buffer,"Copy one file (%11s bytes) to",
                                              Long2Str(Sizefic,bufton));
        else
        sprintf(buffer,"Copy %d files (%11s bytes) to",Nbrfic,
                                              Long2Str(Sizefic,bufton));
    }
    else
    {
    strcpy(buffer,"Copy files to");
    }

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

        if (FTrash->system==0)
            {
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
int FenMove(FENETRE *F1,FENETRE *FTrash)
{
static int DirLength=70;
static int CadreLength=71;
static char Dir[256];
static int n,m,o;

struct Tmt T[5] = {
      { 2,3,1,Dir,&DirLength},
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

/*--------------------------------------------------------------------*\
|- Copy files to .rar (experimental)                                  -|
\*--------------------------------------------------------------------*/
void CopieRar(FENETRE *F1,FENETRE *F2)
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

    if (SelectFile(F1,i))
        {
        if (F1->F[i]->select==1)
            {
            F->select=0;
            F1->nbrsel--;
            F1->taillesel-=F->size;
            }

        if (IsDir(F))
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

/*--------------------------------------------------------------------*\
|- Copy file of .rar                                                  -|
\*--------------------------------------------------------------------*/
void RarCopie(FENETRE *F1,FENETRE *F2)
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

    if (SelectFile(F1,i))
        {
        if (F1->F[i]->select==1)
            {
            F->select=0;
            F1->nbrsel--;
            F1->taillesel-=F->size;
            }

        if (IsDir(F))
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


/*--------------------------------------------------------------------*\
|- copy file of .zip                                                  -|
\*--------------------------------------------------------------------*/
void ZipCopie(FENETRE *F1,FENETRE *F2)
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

    if (SelectFile(F1,i))
        {
        if (F1->F[i]->select==1)
            {
            F->select=0;
            F1->nbrsel--;
            F1->taillesel-=F->size;
            }

        if (IsDir(F))
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

/*--------------------------------------------------------------------*\
|- copy files of .arj                                                 -|
\*--------------------------------------------------------------------*/
void ArjCopie(FENETRE *F1,FENETRE *F2)
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

    if (SelectFile(F1,i))
        {
        if (F1->F[i]->select==1)
            {
            F->select=0;
            F1->nbrsel--;
            F1->taillesel-=F->size;
            }

        if (IsDir(F))
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
|- copy files of .lha                                                 -|
\*--------------------------------------------------------------------*/
void LhaCopie(FENETRE *F1,FENETRE *F2)
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

    if (SelectFile(F1,i))
        {
        if (F1->F[i]->select==1)
            {
            F->select=0;
            F1->nbrsel--;
            F1->taillesel-=F->size;
            }

        if (IsDir(F))
            {
            fprintf(fic,"%s%s\\*.*\n",nom,F->name);
            option='X';
            }
            else
            fprintf(fic,"%s%s\n",nom,F->name);
        }
    }
fclose(fic);
if (PlayerIdf(player,32)==0)
    {
    CommandLine("#%s %c -a %s -w%s @%s",player,option,F1->VolName,
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

PrintAt(9,5,"From %59s",inpath);
PrintAt(9,6,"To   %59s",outpath);

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

struct TmtWin F = { 3,8,76,15,"Move" };

if (FicEcrase==1) return;

memcpy(Dir,path,255);

switch(WinTraite(T,5,&F))
    {
    case 27:
    case 1:
        FicEcrase=2;    // Cancel
    case 0:
        break;
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

struct TmtWin F = { 3,8,76,15,"Move" };

if (FicEcrase==1) return 0;

memcpy(Dir,path,255);

switch(WinTraite(T,7,&F))
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

PrintAt(9,5,"From %59s",inpath);
PrintAt(9,6,"To   %59s",outpath);

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

    buffer=GetMem(TailleEnreg);

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
    int handle;

    _dos_open(inpath,O_RDONLY,&handle);
    _dos_getftime(handle,&d,&t);
    _dos_close(handle);

    _dos_open(outpath,O_RDONLY,&handle);
    _dos_setftime(handle,d,t);
    _dos_close(handle);
    }


return ok;
}




/*--------------------------------------------------------------------*\
|- Fonction de MOVE principale                                        -|
\*--------------------------------------------------------------------*/
void Move(FENETRE *F1,FENETRE *FTrash,char *path)
{
int i;
int j1,j2;                           // postion du compteur (read,write)
char inpath[128],outpath[128];
struct file *F;

noselect=(F1->nbrsel==0);

strcpy(FTrash->path,path);

FicEcrase=0;

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
    if (SelectFile(F1,i))
        {
        strcpy(inpath,F1->path);
        Path2Abs(inpath,F->name);

        strcpy(outpath,FTrash->path);
        Path2Abs(outpath,F->name);

        if (RemoveM(inpath,outpath,(F1->F[i]))==1)
            {
            if (F1->F[i]->select==1)
                {
                F1->F[i]->select=0;
                F1->nbrsel--;
                (F1->taillesel)-=F1->F[i]->size;
                }
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
void Copie(FENETRE *F1,FENETRE *FTrash,char *path)
{
int i;
int j1,j2;                          // postion du compteur (read,write)
char inpath[128],outpath[128];
struct file *F;

noselect=(F1->nbrsel==0);

FicEcrase=0;

Nbrcur=0;
Sizecur=0;
CountRepSize(F1,FTrash,&Nbrfic,&Sizefic);
Clock_Dep=clock();

DFen=FTrash;
CommandLine("#cd %s",path);

if (FenCopie(F1,FTrash)==0) return;


if (!strcmp(F1->path,FTrash->path))
    return;

switch(F1->system)
    {
    case 1:
        RarCopie(F1,FTrash);
        return;
    case 2:
        ArjCopie(F1,FTrash);
        return;
    case 3:
        ZipCopie(F1,FTrash);
        return;
    case 4:
        LhaCopie(F1,FTrash);
        return;
    case 5:
        KkdCopie(F1,FTrash);
        return;
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
        return;
    case 5:
        CopieKkd(F1,FTrash);
        return;
    default:
        YouMad("Copie");
        return;
        break;
    }


SaveEcran();

if (Nbrfic!=0)
    {
    WinCadre(7,4,73,17,0);
    Window(8,5,72,16,10*16+1);

    PrintAt(9,7,"Current");
    WinCadre(9,8,71,11,2);

    PrintAt(9,12,"Total");
    WinCadre(9,13,71,16,2);
    }
    else
    {
    WinCadre(7,4,73,12,0);
    Window(8,5,72,11,10*16+1);

    PrintAt(9,7,"Current");
    WinCadre(9,8,71,11,2);
    }

j1=0;
j2=0;

for (i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];
    if (SelectFile(F1,i))
        {
        strcpy(inpath,F1->path);
        Path2Abs(inpath,F->name);

        strcpy(outpath,FTrash->path);
        Path2Abs(outpath,F->name);

        if (recopy(inpath,outpath,(F1->F[i]))==1)
            {
            if (F1->F[i]->select==1)
                {
                F1->F[i]->select=0;
                F1->nbrsel--;
                (F1->taillesel)-=F1->F[i]->size;
                }
            }
        }
    if (FicEcrase==2) break;
    }

ChargeEcran();
}

/*--------------------------------------------------------------------*\
|- Renvoit 1 si on doit copier le fichier                             -|
\*--------------------------------------------------------------------*/
int SelectFile(FENETRE *F1,int i)
{
if (((Cfg->noprompt)&1)==0)
    {
    if ( (F1->F[i]->select==1) | ((noselect) & (F1->pcur==i)) )
        return 1;
        else
        return 0;
    }
    else
    {
    return (i==F1->nopcur);
    }
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

if ( (to>1024) & (total>1024) )
    {
    j3=(to/1024);
    j3=(j3*length)/(total/1024);
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

/*--------------------------------------------------------------------*\
|- Calcul de la taille d'un repertoire                                -|
\*--------------------------------------------------------------------*/
void CountRepSize(FENETRE *F1,FENETRE *FTrash,int *nbr, int *size)
{
char cont;
struct file *ff;
char error;
char **TabRec;
int NbrRec;
int m;
char nom2[256];

char moi[256],nom[256];

int i;

SaveEcran();

Window(25,6,56,10,10*16+1);
WinCadre(24,5,57,11,0);

PrintAt(35,6,"Please Wait");
PrintAt(29,7,"Computing size of files");
PrintAt(26,8,"(Continue by pressing any key)");

*size=0;
*nbr=0;

for (i=0;(i<F1->nbrfic) & (!KbHit());i++)
{

if (SelectFile(F1,i))
    {
    if ((F1->F[i]->attrib & _A_SUBDIR)==_A_SUBDIR)
        {
        strcpy(nom2,F1->path);
        Path2Abs(nom2,F1->F[i]->name);

        DFen=FTrash;

        TabRec=GetMem(500*sizeof(char*));
        TabRec[0]=GetMem(strlen(nom2)+1);
        memcpy(TabRec[0],nom2,strlen(nom2)+1);
        NbrRec=1;

        do
        {
        m=strlen(TabRec[NbrRec-1]);

        m= (m>=72) ? m-72 : 0;

        CommandLine("#cd %s",TabRec[NbrRec-1]);

        strcpy(nom,TabRec[NbrRec-1]);

/*--------------------------------------------------------------------*\
|-  The files                                                         -|
\*--------------------------------------------------------------------*/

        for (m=0;(m<DFen->nbrfic) & (!KbHit());m++)
            {
            ff=DFen->F[m];

            error=ff->attrib;

            if (ff->name[0]!='.')
                {
                cont=1;

                if (IsDir(ff)) cont=0; // Not Subdir
                if ((error&0x08)==0x08) cont=0; // Not Subdir

                if (cont)
                    {
                    (*nbr)++;
                    (*size)+=ff->size;
                    PrintAt(25,10,"%11s bytes in %5d files",
                                           Long2Str(*size,bufton),*nbr);
                    }
                }
            }

        free(TabRec[NbrRec-1]);
        NbrRec--;

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

            if (ff->name[0]!='.')
                {
                if ( (IsDir(ff))  & (DFen->system==0) )
                    {
                    strcpy(moi,nom);
                    Path2Abs(moi,ff->name);

                    TabRec[NbrRec]=GetMem(strlen(moi)+1);
                    memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
                    NbrRec++;
                    }
                }
            }
        }
        while ( (NbrRec>0) & (!KbHit()) );

        free(TabRec);
        }
        else
        {
        (*nbr)++;
        (*size)+=F1->F[i]->size;
        PrintAt(25,10,"%11s bytes in %5d files",
                                           Long2Str(*size,bufton),*nbr);
        }
    } //--- END IF SELECT ----------------------------------------------

} //--- END FOR --------------------------------------------------------


if (KbHit())
    {
    while (KbHit()) Wait(0,0,0);
    *size=0;
    *nbr=0;
    }

ChargeEcran();
}

/*--------------------------------------------------------------------*\
|- Copie des KKD                                                      -|
\*--------------------------------------------------------------------*/


void KkdCopie(FENETRE *F1,FENETRE *FTrash)
{
int i;

int j1,j2;                           // postion du compteur (read,write)

struct file *F;

SaveEcran();

if (Nbrfic!=0)
    {
    WinCadre(7,4,73,17,0);
    Window(8,5,72,16,10*16+1);

    PrintAt(9,7,"Current");
    WinCadre(9,8,71,11,2);

    PrintAt(9,12,"Total");
    WinCadre(9,13,71,16,2);
    }
    else
    {
    WinCadre(7,4,73,12,0);
    Window(8,5,72,11,10*16+1);

    PrintAt(9,7,"Current");
    WinCadre(9,8,71,11,2);
    }

j1=0;
j2=0;

for (i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];
    if (SelectFile(F1,i))
        {
        if (KKDrecopy(F1,i,FTrash)==1)
            {
            if (F1->F[i]->select==1)
                {
                F1->F[i]->select=0;
                F1->nbrsel--;
                (F1->taillesel)-=F1->F[i]->size;
                }
            }
        }
    if (FicEcrase==2) break;
    }

ChargeEcran();
}


int KKDrecopy(FENETRE *F1,int F1pos,FENETRE *F2)
{
static char nom[256];
char inpath[256];
char outpath[256];

int i;

if ( (F1->F[F1pos]->desc==0) | (IsDir(F1->F[F1pos])) )
    {
    if (strlen(F1->path)==strlen(F1->VolName))
        strcpy(nom,"\\");
        else
        {
        strcpy(nom,"\\");
        strcat(nom,(F1->path)+strlen(F1->VolName)+1);
        }

    DFen=Fenetre[3];

    if (F1->KKDdrive==0)
        F1->KKDdrive=ChangeToKKD()+1;
        else
        CommandLine("#cd %c:\\",F1->KKDdrive+'A'-1);

    CommandLine("#cd %s",nom);

    strcpy(nom,DFen->path);
    Path2Abs(nom,F1->F[F1pos]->name);

    for (i=0;i<DFen->nbrfic;i++)
        if (!stricmp(F1->F[F1pos]->name,DFen->F[i]->name))
            {
            DFen->pcur=i;
            DFen->scur=i;
            break;
            }

    strcpy(inpath,DFen->path);
    Path2Abs(inpath,F1->F[F1pos]->name);

    strcpy(outpath,F2->path);
    Path2Abs(outpath,F1->F[F1pos]->name);

    return recopy(inpath,outpath,DFen->F[DFen->pcur]);
    }

KKD2File(F1,F1pos,F2);

return 1;
}

/*--------------------------------------------------------------------*\
|-                           FICHIER .KKD                             -|
\*--------------------------------------------------------------------*/

void CopieKkd(FENETRE *F1,FENETRE *FTrash)
{
int i;

int j1,j2;                           // postion du compteur (read,write)

struct file *F;

SaveEcran();

if (Nbrfic!=0)
    {
    WinCadre(7,4,73,17,0);
    Window(8,5,72,16,10*16+1);

    PrintAt(9,7,"Current");
    WinCadre(9,8,71,11,2);

    PrintAt(9,12,"Total");
    WinCadre(9,13,71,16,2);
    }
    else
    {
    WinCadre(7,4,73,12,0);
    Window(8,5,72,11,10*16+1);

    PrintAt(9,7,"Current");
    WinCadre(9,8,71,11,2);
    }

j1=0;
j2=0;

for (i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];
    if (SelectFile(F1,i))
        {
        if (File2KKD(F1,i,FTrash)==1)
            {
            if (F1->F[i]->select==1)
                {
                F1->F[i]->select=0;
                F1->nbrsel--;
                (F1->taillesel)-=F1->F[i]->size;
                }
            }
        }
    if (FicEcrase==2) break;
    }

ChargeEcran();
}

struct kkdesc
    {
    long desc;
    long next;
    long size;
    unsigned short time;
    unsigned short date;
    char attrib;
    };

int File2KKD(FENETRE *F1,int F1pos,FENETRE *F2)
{
char foundfile;
char copieok;

struct kkdesc KKD_desc;

int z;
int lastpos;

char buffer[256];
char nom[256];
static char name[256];

char Nomarch[256];
char fin;
FILE *fic,*infic;

int n,i,j;

unsigned char tai;                                    // taille des noms

copieok=0;
foundfile=0;

if (strlen(F2->path)==strlen(F2->VolName))
    {
    strcpy(nom,"");
    }
    else
    {
    strcpy(nom,(F2->path)+strlen(F2->VolName)+1);
    strcat(nom,"\\");
    }

PrintAt(9,6,"To   %59s",F2->VolName);

fic=fopen(F2->VolName,"r+b");

fseek(fic,3,SEEK_SET);                                   // Passe la cle

fread(&tai,1,1,fic);                                 // Passe la version

fread(&tai,1,1,fic);                           // Passe le nom du volume
fread(Nomarch,tai,1,fic);

fread(&n,4,1,fic);               // Passe le nombre d'octets non utilise
fread(&n,4,1,fic);  // Passe le nbr de bytes non utilise avant reconstr.


i=0;    //--- DEBUT ----------------------------------------------------
j=0;    //--- FIN ------------------------------------------------------

i=j;
strcpy(name,nom+j);
for(n=j;n<strlen(nom);n++)
    {
    if (nom[n]=='\\')
        {
        name[n-j]=0;
        j=n+1;
        break;
        }
    }

fin=0;

while(i!=j)
{
fread(&tai,1,1,fic);
fread(Nomarch,tai,1,fic);
Nomarch[tai]=0;

fread(&KKD_desc,sizeof(struct kkdesc),1,fic);

if ( (!stricmp(Nomarch,name)) & ((KKD_desc.attrib&0x10)==0x10) )
    {
    if (KKD_desc.desc==0)
        {
        fin=1;      // Un gros probleme: un directory qui ne mene a rien
        break;
        }
    fseek(fic,KKD_desc.desc,SEEK_SET);

    i=j;
    strcpy(name,nom+j);
    for(n=j;n<strlen(nom);n++)
        {
        if (nom[n]=='\\')
            {
            name[n-j]=0;
            j=n+1;
            break;
            }
        }
    }
    else
    {
    if (KKD_desc.next==0)
        {
        fin=1;                                   // Directory pas trouv‚
        break;
        }
    fseek(fic,KKD_desc.next,SEEK_SET);
    }
}

if (fin==0)
    {
    do
        {
        lastpos=ftell(fic);

        fread(&tai,1,1,fic);
        fread(Nomarch,tai,1,fic);
        Nomarch[tai]=0;

        fread(&KKD_desc,sizeof(struct kkdesc),1,fic);

        if (!stricmp(Nomarch,F1->F[F1pos]->name))
            {
            foundfile=1;
            fseek(fic,-sizeof(struct kkdesc),SEEK_CUR);

            KKD_desc.desc=filelength(fileno(fic));

            fwrite(&KKD_desc,sizeof(struct kkdesc),1,fic);

            fseek(fic,0,SEEK_END);

            z=0;
            fwrite(&z,1,4,fic);            //--- Information sur fichier

            strcpy(buffer,F1->path);
            Path2Abs(buffer,F1->F[F1pos]->name);

            PrintAt(9,5,"From %59s",buffer);
            infic=fopen(buffer,"rb");

            SizeMaxRecord=32*1024;

            if (FicCopy(infic,fic,filelength(fileno(infic)))==1)
                copieok=0;
                else
                copieok=1;

            fclose(infic);
            break;
            }

        fseek(fic,KKD_desc.next,SEEK_SET);
        }
    while(KKD_desc.next!=0);
    }

if (foundfile==0)
    {
    fseek(fic,lastpos,SEEK_SET);

    fread(&tai,1,1,fic);
    fread(Nomarch,tai,1,fic);
    Nomarch[tai]=0;

    fread(&KKD_desc,sizeof(struct kkdesc),1,fic);
    fseek(fic,-sizeof(struct kkdesc),SEEK_CUR);

    KKD_desc.next=filelength(fileno(fic));
    fwrite(&KKD_desc,sizeof(struct kkdesc),1,fic);

    //--- Cree le nouveau fichier … la fin -----------------------------
    fseek(fic,0,SEEK_END);

    strcpy(Nomarch,F1->F[F1pos]->name);
    tai=strlen(Nomarch);
    fwrite(&tai,1,1,fic);
    fwrite(Nomarch,tai,1,fic);

    if (IsDir(F1->F[F1pos]))
        KKD_desc.desc=ftell(fic)+sizeof(struct kkdesc);
        else
        KKD_desc.desc=0;

    KKD_desc.next=0;
    KKD_desc.size=F1->F[F1pos]->size;
    KKD_desc.time=F1->F[F1pos]->time;
    KKD_desc.date=F1->F[F1pos]->date;
    KKD_desc.attrib=F1->F[F1pos]->attrib;

    fwrite(&KKD_desc,sizeof(struct kkdesc),1,fic);

    //--- Cree le repertoire .. si necessaire --------------------------
    if (IsDir(F1->F[F1pos]))
        {
        strcpy(Nomarch,"..");
        tai=2;
        fwrite(&tai,1,1,fic);
        fwrite(Nomarch,tai,1,fic);

        KKD_desc.desc=0;
        KKD_desc.next=0;
        KKD_desc.size=F1->F[F1pos]->size;
        KKD_desc.time=F1->F[F1pos]->time;
        KKD_desc.date=F1->F[F1pos]->date;
        KKD_desc.attrib=F1->F[F1pos]->attrib;

        fwrite(&KKD_desc,sizeof(struct kkdesc),1,fic);
        }
    }

fclose(fic);

if (Nbrfic!=0)
    {
    Nbrcur++;
    Sizecur+=F1->F[F1pos]->size;
    }

return copieok;
}

int KKD2File(FENETRE *F1,int F1pos,FENETRE *F2)
{
char copieok;

struct kkdesc KKD_desc;

int z;

char buffer[256];
char nom[256];
static char name[256];

char Nomarch[256];
char fin;
FILE *fic,*outfic;

int n,i,j;

unsigned char tai;                                    // taille des noms

int handle;

copieok=0;

if (strlen(F1->path)==strlen(F1->VolName))
    {
    strcpy(nom,"");
    }
    else
    {
    strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
    strcat(nom,"\\");
    }

PrintAt(9,5,"From %59s",F1->VolName);

fic=fopen(F1->VolName,"rb");

fseek(fic,3,SEEK_SET);                                   // Passe la cle

fread(&tai,1,1,fic);                                 // Passe la version

fread(&tai,1,1,fic);                           // Passe le nom du volume
fread(Nomarch,tai,1,fic);

fread(&n,4,1,fic);               // Passe le nombre d'octets non utilise
fread(&n,4,1,fic);  // Passe le nbr de bytes non utilise avant reconstr.


i=0;    //--- DEBUT ----------------------------------------------------
j=0;    //--- FIN ------------------------------------------------------

i=j;
strcpy(name,nom+j);
for(n=j;n<strlen(nom);n++)
    {
    if (nom[n]=='\\')
        {
        name[n-j]=0;
        j=n+1;
        break;
        }
    }

fin=0;

while(i!=j)
{
fread(&tai,1,1,fic);
fread(Nomarch,tai,1,fic);
Nomarch[tai]=0;

fread(&KKD_desc,sizeof(struct kkdesc),1,fic);

if ( (!stricmp(Nomarch,name)) & ((KKD_desc.attrib&0x10)==0x10) )
    {
    if (KKD_desc.desc==0)
        {
        fin=1;      // Un gros probleme: un directory qui ne mene a rien
        break;
        }
    fseek(fic,KKD_desc.desc,SEEK_SET);

    i=j;
    strcpy(name,nom+j);
    for(n=j;n<strlen(nom);n++)
        {
        if (nom[n]=='\\')
            {
            name[n-j]=0;
            j=n+1;
            break;
            }
        }
    }
    else
    {
    if (KKD_desc.next==0)
        {
        fin=1;                                   // Directory pas trouv‚
        break;
        }
    fseek(fic,KKD_desc.next,SEEK_SET);
    }
}

if (fin==0)
    {
    do
        {
        fread(&tai,1,1,fic);
        fread(Nomarch,tai,1,fic);
        Nomarch[tai]=0;

        fread(&KKD_desc,sizeof(struct kkdesc),1,fic);

        if (!stricmp(Nomarch,F1->F[F1pos]->name))
            {
            fseek(fic,KKD_desc.desc,SEEK_SET);

            z=0;
            fread(&z,1,4,fic);             //--- Information sur fichier

            strcpy(buffer,F2->path);
            Path2Abs(buffer,F1->F[F1pos]->name);

            outfic=fopen(buffer,"wb");

            PrintAt(9,6,"To   %59s",buffer);

            SizeMaxRecord=32*1024;

            if (FicCopy(fic,outfic,KKD_desc.size)==1)
                copieok=0;
                else
                copieok=1;

            fclose(outfic);

            _dos_open(buffer,O_RDONLY,&handle);
            _dos_setftime(handle,KKD_desc.date,KKD_desc.time);
            _dos_close(handle);

            break;
            }

        fseek(fic,KKD_desc.next,SEEK_SET);
        }
    while(KKD_desc.next!=0);
    }

fclose(fic);

if (Nbrfic!=0)
    {
    Nbrcur++;
    Sizecur+=F1->F[F1pos]->size;
    }

return copieok;
}



