// Dos-function
#include <errno.h>
#include <ctype.h>

#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <direct.h>

#include <time.h>

#include "kk.h"




// Retourne 1 si erreur

int DOSlitfic(void)
{
struct find_t ff;

struct file **Fic;
unsigned error;
char rech[256];
union REGS R;

char drive;

drive=toupper(DFen->path[0])-'A';

/*

R.w.ax=0x5601;
R.w.dx=0xFFFF;
R.h.bh=drive;
R.h.bl=0;
int386(0x2F,&R,&R);

if (R.h.al==0xFF) PrintAt(0,0,"Redirected drive (with interlnk)"),getch();

// PrintAt(0,0,"%04X %04X %04X %04X",R.w.ax,R.w.bx,R.w.cx,R.w.dx);
// pour C renvoit 5601 0200 0168 FFFF
// pour D renvoit 5601 0300 0168 FFFF
// pour E renvoit 56FF 0401 0001 0630

R.w.ax=0x150B;
R.w.cx=drive;
int386(0x2F,&R,&R);

if ( (R.w.bx==0xADAD) & (R.w.ax!=0) ) PrintAt(0,0,"CD-ROM drive"),getch();

// PrintAt(0,0,"%04X %04X %04X %04X",R.w.ax,R.w.bx,R.w.cx,R.w.dx);
// pour C renvoit 0000 ADAD 0002 FFFF
// pour D renvoit 0000 ADAD 0003 FFFF
// pour H renvoit 5AD4 ADAD 0007 FFFF

*/

if (chdir(DFen->path)!=0)
    return 1;

Fic=DFen->F;

strcpy(rech,DFen->path);
if (rech[strlen(rech)-1]!='\\') strcat(rech,"\\");

DFen->nbrfic=0;

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
        Fic[DFen->nbrfic]=GetMemSZ(sizeof(struct file));
        Fic[DFen->nbrfic]->name=GetMemSZ(strlen(ff.name)+1);
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

// DFen->init=1;

rech[3]=0;

if ( ( (!stricmp(rech,"A:\\")) | (!stricmp(rech,"B:\\")) )
     | (DFen->nbrfic==0) )
    {
    Fic[DFen->nbrfic]=GetMemSZ(sizeof(struct file));

    Fic[DFen->nbrfic]->name=GetMemSZ(4); // Pour Reload
    memcpy(Fic[DFen->nbrfic]->name,rech,4);
    Fic[DFen->nbrfic]->time=0;
    Fic[DFen->nbrfic]->date=0;
    Fic[DFen->nbrfic]->attrib=_A_SUBDIR;
    Fic[DFen->nbrfic]->select=0;
    Fic[DFen->nbrfic]->size=0;
    DFen->nbrfic++;
    }

ChangeLine();

// Temporise le temps … attendre avant le lecture du header OK ;-}

switch(drive)
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





