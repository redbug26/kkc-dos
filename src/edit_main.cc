/*--------------------------------------------------------------------*\
|- Editor by RedBug/Ketchup^Pulpe                                     -|
\*--------------------------------------------------------------------*/

// define STANDALONE_KKEDIT to compile kkedit.cc without kkmain

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "kk.h"

#ifndef LINUX
    #include <dos.h>
    #include <direct.h>
    #include <io.h>
    #include <conio.h>
    #include <mem.h>
#else
    #include <unistd.h>
#endif

#include "kkedit.h"

#define LENMAX 2048



void Edit(KKEDIT *E,char *name);


/*--------------------------------------------------------------------*\
|- Main function                                                      -|
\*--------------------------------------------------------------------*/
int main(int argc,char **argv)
{
int OldX,OldY;                         // To save the size of the screen
int n;
KKEDIT E;

char *path;

if (argc != 2) {
    return 0;
}


/*--------------------------------------------------------------------*\
|-  Initialisation de l'ecran                                         -|
\*--------------------------------------------------------------------*/

Redinit();

InitScreen(0);                     // Initialise toutes les donn‚es HARD

OldX=GetScreenSizeX(); // A faire apres le SetMode ou le InitScreen
OldY=GetScreenSizeY();

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;                  // Initialisation de la taille ecran

//printf("FIN2(%d,%d)\n\n", Cfg->TailleX, Cfg->TailleY); exit(1);

/*--------------------------------------------------------------------*\
|-  Save the current path                                             -|
\*--------------------------------------------------------------------*/

path=(char*)GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
    }

/*--------------------------------------------------------------------*\
|- Path & File initialisation                                         -|
\*--------------------------------------------------------------------*/

SetDefaultPath(path);


/*
Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\test1.hlp");
*/


/*--------------------------------------------------------------------*\
|- Configuration loading                                              -|
\*--------------------------------------------------------------------*/

DefaultCfg(Cfg);

#ifndef LINUX
TXTMode();
LoadPal(Cfg->palette);
#endif

#ifndef NOFONT
InitFont();
#endif

E.zoom=1;

Edit(&E,argv[1]);

DesinitScreen();

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

#ifndef LINUX
TXTMode();
#endif

puts("That's all folk...");
}

void Edit(KKEDIT *E,char *name)
{
char *fichier;


SaveScreen();

Bar(" ----  ----  ----  ----  ----  ----  ----  ----  ----  ---- ");

//printf("ok\n\n"); exit(1);

fichier=(char*)GetMem(256);
strcpy(fichier,name);


TxtEdit(E,fichier);


free(fichier);


LoadScreen();
}

