#include <direct.h>
#include <i86.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <bios.h>
#include <io.h>
#include <graph.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include <signal.h> // For handling signal

#include <dos.h>    //  Pour redirection des I/O

#include "hard.h"

void main(short argc,char **argv)
{
short n;
int TailleY;
char OldY;

char *path;

OldY=(*(char*)(0x484))+1;

path=GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
    }


Cfg=GetMem(sizeof(struct config));
Fics=GetMem(sizeof(struct fichier));

Mask=GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    Mask[n]=GetMem(sizeof(struct PourMask));

Fics->FicIdfFile=GetMem(256);
strcpy(Fics->FicIdfFile,path);
strcat(Fics->FicIdfFile,"\\idfext.rb");

Fics->CfgFile=GetMem(256);
strcpy(Fics->CfgFile,path);
strcat(Fics->CfgFile,"\\kkrb.cfg");

Fics->view=GetMem(256);
strcpy(Fics->view,path);
strcat(Fics->view,"\\view");

Fics->edit=GetMem(256);
strcpy(Fics->edit,path);
strcat(Fics->edit,"\\edit");

Fics->path=GetMem(256);
strcpy(Fics->path,path);

Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\kksetup.hlp");

Fics->LastDir=GetMem(256);
strcpy(Fics->LastDir,path);


TailleY=50;


TXTMode(TailleY);
NoFlash();

switch (TailleY)  {
   case 50:
        Font8x8();
        break;
   case 25:
   case 30:
        Font8x16();
        break;
   }
LoadPal();


WinCadre(0,0,79,TailleY-1,1);
ChrWin(1,1,78,TailleY-2,32);
ColWin(1,1,78,TailleY-2,10*16+1);


Wait(0,0,0);

TXTMode(OldY);
}
