/*--------------------------------------------------------------------*\
|- Editor by RedBug/Ketchup^Pulpe                                     -|
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


/*--------------------------------------------------------------------*\
|- Affichage de la barre en dessous de l'ecran                        -|
\*--------------------------------------------------------------------*/

void Bar(char *bar)
{
int TY;
int i,j,n;

TY=Cfg->TailleY-1;

n=0;
for (i=0;i<10;i++)
    {
    PrintAt(n,TY,"F%d",(i+1)%10);
    for(j=0;j<2;j++,n++)
        AffCol(n,TY,1*16+8);
    
    for(j=0;j<6;j++,n++)
        {
        AffCol(n,TY,1*16+2);
        AffChr(n,TY,*(bar+i*6+j));
        }
    if (Cfg->TailleX==90)
        {
        AffCol(n,TY,1*16+2);
        AffChr(n,TY,32);
        n++;
        }
    }
}



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

OldX=(*(char*)(0x44A));
OldY=(*(char*)(0x484))+1;

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;                  // Initialisation de la taille ecran

InitScreen(0);                     // Initialise toutes les donn‚es HARD

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

Mask=GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    Mask[n]=GetMem(sizeof(struct PourMask));

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

DefaultCfg();

Cfg->TailleY=50;

TXTMode();
LoadPal();

InitFont();

Edit(argv[1]);

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

TXTMode();

puts("That's all folk...");
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

struct line
  {
  struct line *from,*next;
  char *chaine;
  };

int TxtEdit(char *fichier);


int TxtEdit(char *fichier)
{
int xm,ym;
char fin=0;
FILE *fic;
char chaine[256];

struct line *prem,*curr,*haut,*ligne;

int n,x,y;

int cx,cy;  //--- Position du curseur ----------------------------------
int car;
char affiche=1;
char ins=1;

fic=fopen(fichier,"rb");

prem=GetMem(sizeof(struct line));
prem->from=NULL;
prem->next=NULL;

curr=prem;

while(fgets(chaine,256,fic)!=NULL)
    {
    n=strlen(chaine);
    if ( (chaine[n-1]==0x0A) | (chaine[n-1]==0x0D) )  chaine[n-1]=0;
    if ( (chaine[n-2]==0x0A) | (chaine[n-2]==0x0D) )  chaine[n-2]=0;

    curr->next=GetMem(sizeof(struct line));
    curr->next->from=curr;
    curr->chaine=GetMem(strlen(chaine)+1);
    strcpy(curr->chaine,chaine);
    curr=curr->next;
    }

fclose(fic);

haut=prem;

SaveScreen();
PutCur(3,0);

Bar(" Help  ----  ----  Hexa  ----  ---- Search Print Mask  ---- ");

xm=Cfg->TailleX;
ym=Cfg->TailleY-1;

cx=0;
cy=0;

/*--------------------------------------------------------------------*\
|- Boucle principale                                                  -|
\*--------------------------------------------------------------------*/

do
{

if (affiche==1)  //--- Affichage de toute la page ----------------------
    {
    curr=haut;

    for(y=0;y<ym;y++)
        {
        if (curr->next==NULL) break;

        n=strlen(curr->chaine);

        for(x=0;x<n;x++)
            AffChr(x,y,curr->chaine[x]);

        for(x=n;x<xm;x++)
            AffChr(x,y,32);

        if (y==cy)
            ligne=curr;

        curr=curr->next;
        }

    strcpy(chaine,ligne->chaine);
    affiche=0;
    }

PrintAt(0,cy,"%-80s",chaine);


GotoXY(cx,cy);
PutCur(7,7);     // 2,7

car=Wait(0,0,0);
switch(LO(car))
    {
    case 0:
        switch(HI(car))
            {
            case 72:                                               // UP
                cy--;
                affiche=1;
                break;
            case 80:                                             // DOWN
                cy++;
                affiche=1;
                break;
            case 0x4D:                                          // RIGHT
                cx++;
                break;
            case 0x4B:                                           // LEFT
                cx--;
                break;
            case 0x4F:                                            // END
                cx=strlen(chaine);
                break;
            case 0x47:                                           // HOME
                cx=0;
                break;
            case 0x53:                                         // DELETE
                n=strlen(chaine);

                if (n!=0)
                    for(x=cx;x<n;x++)
                        chaine[x]=chaine[x+1];
                break;
            }
        break;
    case 8:                                                   // DEL <--
        n=strlen(chaine);

        if (cx!=0)
            {
            cx--;
            for(x=cx;x<n;x++)
                chaine[x]=chaine[x+1];
            }
        break;
    case 13:
        curr=ligne->next;
        ligne->next=GetMem(sizeof(struct line));
        ligne->next->next=curr;
        ligne->next->from=ligne;
        curr->from=ligne->next;

        ligne->next->chaine=GetMem(strlen(chaine)+1-cx);
        strcpy(ligne->next->chaine,chaine+cx);
        chaine[cx]=0;

        affiche=1;
        cy++;
        cx=0;
        break;
    case 27:                                                      // ESC
        affiche=1;
        break;
    default:
        if (ins==1)
            {
            n=strlen(chaine);

            if (cx<n)
                {
                for(x=n+1;x>=cx;x--)
                    chaine[x+1]=chaine[x];
                chaine[cx]=car;
                }
            }
        cx++;
        break;
    }

if (affiche==1)  //--- Sauve la chaine dans la liste des chaines -------
    {
    free(ligne->chaine);
    ligne->chaine=GetMem(strlen(chaine)+1);
    strcpy(ligne->chaine,chaine);
    }

if (cy<0)
    {
    cy=0;
    if (haut->from!=NULL) haut=haut->from;
    affiche=1;
    }
if (cy>=ym)
    {
    cy=ym-1;
    if (curr->next!=NULL) haut=haut->next;
    affiche=1;
    }

}
while(car!=27);

LoadScreen();


/*--------------------------------------------------------------------*\
|-  Sauvegarde du fichier                                             -|
\*--------------------------------------------------------------------*/

fic=fopen("redbug.doc","wt");

curr=prem;

while(curr!=NULL)
    {
    fprintf(fic,"%s\n",curr->chaine);
    prem=curr->next;
    curr=prem;
    }

fclose(fic);

/*--------------------------------------------------------------------*\
|- desalloue la ram                                                   -|
\*--------------------------------------------------------------------*/

curr=prem;

while(curr!=NULL)
    {
    prem=curr->next;
    LibMem(curr);
    curr=prem;
    }

return fin;
}


void Edit(char *name)
{
char *fichier;

SaveScreen();

Bar(" ----  ----  ----  ----  ----  ----  ----  ----  ----  ---- ");

fichier=GetMem(256);
strcpy(fichier,name);



TxtEdit(fichier);



free(fichier);


LoadScreen();
}




