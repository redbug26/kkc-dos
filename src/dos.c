// Dos-function
#include <errno.h>
#include <ctype.h>

#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <direct.h>

#include "kk.h"




// Retourne 1 si erreur

int DOSlitfic(void)
{
struct find_t ff;

struct file **Fic;
unsigned error;
char rech[256];

if (chdir(DFen->path)!=0)
    return 1;


Fic=DFen->F;

strcpy(rech,DFen->path);
if (rech[strlen(rech)-1]!='\\') strcat(rech,"\\");

if ( (!stricmp(rech,"A:\\")) | (!stricmp(rech,"B:\\")) )
    {
    Fic[0]=GetMem(sizeof(struct file));

    Fic[0]->name=GetMem(4); // Pour Reload
    memcpy(Fic[0]->name,rech,4);
    Fic[0]->time=0;
    Fic[0]->date=0;
    Fic[0]->attrib=_A_SUBDIR;
    Fic[0]->select=0;
    Fic[0]->size=0;
    DFen->nbrfic=1;
    }
    else
    {
    DFen->nbrfic=0;
    }

strcat(rech,"*.*");

DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrsel=0;

DFen->taillesel=0;

error=_dos_findfirst(rech,63,&ff);
if (error==1)
    return 1;           // Pas bien si unit‚ est !='A' et 'B'

while (error==0)
    {
    if ( ((Cfg->pntrep==1) |  (strcmp(ff.name,".")!=0)) &
         ((Cfg->hidfil==1) | (((ff.attrib)&_A_HIDDEN)!=_A_HIDDEN)) &
         (((ff.attrib)&_A_VOLID)!=_A_VOLID)
         )
        {
        Fic[DFen->nbrfic]=GetMem(sizeof(struct file));
        Fic[DFen->nbrfic]->name=GetMem(strlen(ff.name)+1);
        strcpy(Fic[DFen->nbrfic]->name,ff.name);
        Fic[DFen->nbrfic]->time=ff.wr_time;
        Fic[DFen->nbrfic]->date=ff.wr_date;
        Fic[DFen->nbrfic]->attrib=ff.attrib;
        Fic[DFen->nbrfic]->select=0;
        Fic[DFen->nbrfic]->size=ff.size;
        DFen->taillefic+=Fic[DFen->nbrfic]->size;
        DFen->nbrfic++;
        }
    error=_dos_findnext(&ff);
	}

DFen->init=1;

ChangeLine();

// Temporise le temps … attendre avant le lecture du header OK ;-}

switch(toupper(DFen->path[0]))
    {
    case 'C':
        DFen->IDFSpeed=2*18;
        break;
    default:
        DFen->IDFSpeed=5*18;
        break;
   }

return 0;
}

void InstallDOS(void)
{
getcwd(DFen->path,255);

DFen->system=0;
}




