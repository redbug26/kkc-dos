/*--------------------------------------------------------------------*\
|- procedure d'effacement                                             -|
\*--------------------------------------------------------------------*/
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <direct.h>

#include <time.h>

#include "win.h"
#include "kk.h"
#include "gestion.h"

int Erase(char *path,struct file *F);
int Deltree(char *path);

/*--------------------------------------------------------------------*\
|-  Delete File                                                       -|
\*--------------------------------------------------------------------*/

int Del(char *s)
{
_dos_setfileattr(s,_A_NORMAL);

IOerr=0;

return remove(s);
}


/*--------------------------------------------------------------------*\
|-  Deltree                                                           -|
\*--------------------------------------------------------------------*/

int Deltree(char *path)
{
DIR *dirp;
struct dirent *ff;

char error;

Path2Abs(path,"*.*");
dirp=opendir(path);
Path2Abs(path,"..");

if (dirp!=NULL)
    while(1)
    {
    ff=readdir(dirp);
    if (ff==NULL) break;

    error=ff->d_attr;

    if (ff->d_name[0]!='.')
        {
        Path2Abs(path,ff->d_name);
        if ((error&0x10)==0x10)
            Deltree(path);
            else
            Del(path);
        Path2Abs(path,"..");
        }
    }

closedir(dirp);

IOerr=0;

return rmdir(path);
}



/*--------------------------------------------------------------------*\
|-  Efface convenablement                                             -|
\*--------------------------------------------------------------------*/

int Erase(char *path,struct file *F)
{
int i;

if ((F->attrib & _A_SUBDIR)==_A_SUBDIR)
    i=Deltree(path);
    else
    i=Del(path);


return i;
}


// retourne 0 si pas copie
//-------------------------

int FenDelete(char *dest)
{
static int CadreLength=71;
static int Dir[256];

struct Tmt T[6] = {
      {5,5,5,"   Delete    ",NULL}, // Delete
      {30,5,5," Delete All  ",NULL}, // Delete All
      {55,5,3,NULL,NULL},
      { 5,3,0,Dir,NULL},
      { 5,2,0,"Delete file",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,10,76,17,
    "Delete"};

int n;

memcpy(Dir,dest,255);

n=WinTraite(T,6,&F);

return n;

}

int DelErr(char *dest)
{
static int CadreLength=71;
static int Dir[256];

struct Tmt T[5] = {
      {15,5,2,NULL,NULL}, // Delete
      {45,5,3,NULL,NULL},
      { 5,3,0,Dir,NULL},
      { 5,2,0,"Couldn't delete file",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = { 3,10,76,17, "Error" };

int n;

memcpy(Dir,dest,255);

n=WinTraite(T,5,&F);

return n;

}


void Delete(struct fenetre *F1)                       // Delete Multiple
{
int i;

char fin;
char test;
char car;

char inpath[128];

struct file *F;

if ( (F1->nbrsel==0) & (F1->F[F1->pcur]->name[0]!='.') )
    {
    F1->F[F1->pcur]->select=1;
    F1->nbrsel++;
    F1->taillesel+=F1->F[F1->pcur]->size;
    }

test=1;
fin=0;

for (i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];

    if ((F->select)==1)
        {
        strcpy(inpath,F1->path);
        Path2Abs(inpath,F->name);

        fin=0;

        if (test==1)
            car=FenDelete(inpath);
            else
            car=0;

        switch(car)
            {
            case 27:    // Touche ESCape
            case 3:
                fin=1;
                break;
            case 1:     // Delete ALL
                test=0;
            case 0:     // Delete
                switch(F1->system)
                    {
                    case 0:
                        if (Erase(inpath,(F1->F[i]))==0) {
                            F1->F[i]->select=0;
                            F1->nbrsel--;
                            (F1->taillesel)-=F1->F[i]->size;
                            }
                        else
                        if (DelErr(inpath)!=0)
                            fin=1;
                        break;
                    default:
                        YouMad("Delete");
                        break;
                }
                break;
            }
        }
    if (fin==1) break;
    }
}

