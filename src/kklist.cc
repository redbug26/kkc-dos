/*--------------------------------------------------------------------*\
|- TEST1: Example file for using the hard library /RedBug             -|
\*--------------------------------------------------------------------*/

#include <stdarg.h>
#include <dos.h>
#include <direct.h>
#include <io.h>
#include <stdlib.h>
#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "hard.h"
#include "view.h"

KKVIEW V;

char *Screen_Buffer;
char *Screen_Adr=(char*)0xB8000;

long OldY,OldX,PosX,PosY;


/*--------------------------------------------------------------------*\
|- Main function                                                      -|
\*--------------------------------------------------------------------*/
void main(short argc,char **argv)
{
int OldX,OldY;                         // To save the size of the screen
int n;

char *path;

/*--------------------------------------------------------------------*\
|-  Initialisation de l'ecran                                         -|
\*--------------------------------------------------------------------*/

Cfg=GetMem(sizeof(struct config));

Screen_Buffer=(char*)GetMem(9000);                      // maximum 90x50

OldX=(*(char*)(0x44A));
OldY=(*(char*)(0x484))+1;

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;                  // Initialisation de la taille ecran

InitScreen(0);                     // Initialise toutes les donn‚es HARD

WhereXY(&PosX,&PosY);

for (n=0;n<9000;n++)
    Screen_Buffer[n]=Screen_Adr[n];

/*--------------------------------------------------------------------*\
|-  Save the current path                                             -|
\*--------------------------------------------------------------------*/

path=GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
    }

/*--------------------------------------------------------------------*\
|-  Initialisation des variables                                      -|
\*--------------------------------------------------------------------*/

Fics=GetMem(sizeof(struct fichier));

/*--------------------------------------------------------------------*\
|- Path & File initialisation                                         -|
\*--------------------------------------------------------------------*/

SetDefaultPath(path);

Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\test1.hlp");


/*--------------------------------------------------------------------*\
|- Configuration loading                                              -|
\*--------------------------------------------------------------------*/

InitMouse();

DefaultCfg();

Cfg->TailleY=50;

TXTMode();
LoadPal(Cfg->palette);

InitFont();


V.warp=1;
V.cnvtable=0;                                    // Table Ketchup
V.autotrad=0;
V.ajustview=1;
V.saveviewpos=1;
V.wmask=15;                                  // RedBug preference

V.lnfeed=4;                                              // CR/LF
V.userfeed=0xE3;

V.AnsiSpeed=133;

V.Mask=(struct PourMask**)GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    V.Mask[n]=(struct PourMask*)GetMem(sizeof(struct PourMask));

View(&V,argv[1],0);

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

TXTMode();

GotoXY(0,PosY);

for (n=0;n<9000;n++)
    Screen_Adr[n]=Screen_Buffer[n];
}

