/*--------------------------------------------------------------------*\
|-  Dos-function                                                      -|
\*--------------------------------------------------------------------*/
#include <errno.h>
#include <ctype.h>

#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <direct.h>

#include <time.h>

#include "kk.h"


/*--------------------------------------------------------------------*\
|-  Retourne 1 si erreur                                              -|
\*--------------------------------------------------------------------*/

int DOSlitfic(void)
{
DIR *dirp;
struct dirent *ff;

struct file **Fic;
char rech[256];

if (chdir(DFen->path)!=0)
    return 1;

Fic=DFen->F;

strcpy(rech,DFen->path);
Path2Abs(rech,"*.*");

DFen->nbrfic=0;
DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrsel=0;

DFen->taillesel=0;

dirp=opendir(rech);

if (dirp!=NULL)
    while(1)
    {
    ff=readdir(dirp);
    if (ff==NULL) break;

    if ( ((Cfg->pntrep==1) |  (strcmp(ff->d_name,".")!=0)) &
         ((Cfg->hidfil==1) | (((ff->d_attr)&_A_HIDDEN)!=_A_HIDDEN)) &
         (((ff->d_attr)&_A_VOLID)!=_A_VOLID)
         )
        {
        Fic[DFen->nbrfic]=GetMemSZ(sizeof(struct file));
        Fic[DFen->nbrfic]->name=GetMemSZ(strlen(ff->d_name)+1);
        strcpy(Fic[DFen->nbrfic]->name,ff->d_name);
        Fic[DFen->nbrfic]->time=ff->d_time;
        Fic[DFen->nbrfic]->date=ff->d_date;
        Fic[DFen->nbrfic]->attrib=ff->d_attr;
        Fic[DFen->nbrfic]->select=0;
        Fic[DFen->nbrfic]->size=ff->d_size;
        DFen->taillefic+=Fic[DFen->nbrfic]->size;
        DFen->nbrfic++;
        }
    }

closedir(dirp);


rech[3]=0;

if ( ( (!stricmp(rech,"A:\\")) | (!stricmp(rech,"B:\\")) )
     | (DFen->nbrfic==0) )
    {
    Fic[DFen->nbrfic]=GetMemSZ(sizeof(struct file));

    Fic[DFen->nbrfic]->name=GetMemSZ(4);                  // Pour Reload
    memcpy(Fic[DFen->nbrfic]->name,rech,4);
    Fic[DFen->nbrfic]->time=0;
    Fic[DFen->nbrfic]->date=0;
    Fic[DFen->nbrfic]->attrib=_A_SUBDIR;
    Fic[DFen->nbrfic]->select=0;
    Fic[DFen->nbrfic]->size=0;
    DFen->nbrfic++;
    }

ChangeLine();

/*--------------------------------------------------------------------*\
|-  Temporise le temps … attendre avant le lecture du header OK ;-}   -|
\*--------------------------------------------------------------------*/

switch(toupper(DFen->path[0])-'A')
    {
    case 2:     // C
        DFen->IDFSpeed=2*18;
        break;
    default:
        DFen->IDFSpeed=5*18;
        break;
   }

return 0;
}





