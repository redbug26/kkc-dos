// Editor

#include <ctype.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <conio.h>
#include <stdlib.h>

#include <io.h>
#include <direct.h>

#include <dos.h>
#include <fcntl.h>

#include <time.h>

#include <bios.h>

#include "win.h"
#include "kk.h"

#include "idf.h"


static void Bar(char *bar);

int TxtEdit(char *fichier);


static long taille;
char Edit_buffer[32768];

// static char srcch[80];

int TxtEdit(char *fichier)
{
long tableau[50][80]; // Position du buffer dans l'ecran
char aff,wrap;

long posn;  // position courante dans buffer


long posd;  // Dernier position courante dans buffer affich‚e
int xl,yl;  // Taille de la fenˆtre
int xd,yd;  // Position initiale de la fenˆtre

int x,y;    // Position du curseur par rapport a (xd,yd)
char ins;   // Insertion ou pas

int x2,y2;

char car;

char chaine[256];
short lchaine;

int n,m;

char affichage[81];

int code;
int fin=0;

int warp=0;

char pasfini;

SaveEcran();
PutCur(3,0);

Bar(" Help  ----  ----  Hexa  ----  ---- Search Print Mask  ---- ");

wrap=0;
aff=1;

//-------------------- Calcul de la taille maximum ------------------------//
xl=yl=0;

xd=0;

for (n=0;n<taille;n++)
    {
    switch(Edit_buffer[n])
        {
        case 10:
            xd=0;
            yl++;
            break;
        case 13:
            break;
        case 9:
            lchaine=xd/8;
            lchaine=lchaine*8+8;
            lchaine-=xd;
            xd+=lchaine;
            break;
        default:
            xd++;
            if (xd>xl) xl=xd;
            break;
        }
    }
yl++;


// Limite la fenˆtre … l'‚cran
// ---------------------------
if (yl>=79)   xl=80;            //--> Longueur

if (yl>Cfg->TailleY-1)  yl=Cfg->TailleY-1;

// Maximise la fenˆtre
// -------------------
// xl=80;
// yl=Cfg->TailleY-1;

// Place la fenˆtre au milieu de l'‚cran
// -------------------------------------
xd=(80-xl)/2;           // centre le texte
yd=(Cfg->TailleY-yl)/2; //

if (xd<0) xd=0;
if (yd<0) yd=0;

// Affiche la fenˆtre
// ------------------


if ( (xd>0) & (yd>0) & (xd+xl<80) & (yd+yl<Cfg->TailleY) )
    WinCadre(xd-1,yd-1,xd+xl,yd+yl,3);
    else
    {
    if ( (yd+yl<Cfg->TailleY) & (yd>0) )
        {
        ColLin(0,yd-1,80,10*16+9);
        WinLine(0,yd-1,80,1);
        WinLine(0,yd+yl,80,1);
        ColLin(0,yd+yl,80,10*16+9);
        }
    if ( (xd+xl<80) & (xd>0) )
        {
        ColWin(xd-1,0,xd-1,Cfg->TailleY-2,10*16+9);
        ChrWin(xd-1,0,xd-1,Cfg->TailleY-2,0xB3);
        ChrWin(xd+xl,0,xd+xl,Cfg->TailleY-2,0xB3);
        ColWin(xd+xl,0,xd+xl,Cfg->TailleY-2,10*16+9);
        }
    }


ChrWin(xd,yd,xd+xl-1,yd+yl-1,32);
ColWin(xd,yd,xd+xl-1,yd+yl-1,10*16+9);

//-------------------------------------------------------------------------//
affichage[xl]=0;

x=y=0;      // Position dans fenˆtre
ins=0;      // Insere est OFF

posn=0;
posd=0;


do
{
memset(tableau,-1,sizeof(int)*80*50);

pasfini=0;

x2=xd;
y2=yd;

posd=posn;

do
    {
    lchaine=1;      // Longueur chaine

    chaine[0]=Edit_buffer[posn];

    switch(chaine[0])
        {
        case 10:
            lchaine=xl+1-(x2-xd-warp);          // xl= taille totale de la ligne
                                      // x2-xd-warp= taille actuelle de la ligne
            memset(chaine,32,lchaine);
            chaine[0]=10;
            aff=2;
            break;
        case 9:
            lchaine=(x2-xd)/8;
            lchaine=lchaine*8+8;
            lchaine-=(x2-xd);
            memset(chaine,32,lchaine);
            chaine[0]=9;
            break;
        case 13:
            chaine[0]=0;
            break;

        default:
            chaine[0]=CnvASCII(chaine[0]);
            break;
        }

    if (chaine[0]!=0)
        tableau[y2-yd][x2-warp-xd]=posn;

    for(n=0;n<lchaine;n++)
        {
        car=chaine[n];

        if ((x2-xd-warp)>=xl)
            {
            if (aff==2) // Le monsieur te demande d'afficher la ligne
                {
                for (m=0;m<xl;m++)
                    AffChr(xd+m,y2,affichage[m]);

                affichage[0]=0;
                x2=xd;
                y2++;
                if (y2>=yd+yl) break;
                lchaine=0;
                aff=1;
                }
                else
                aff=0;
            }
            else
            {
            if ((x2-xd-warp)>=0)
                affichage[x2-xd-warp]=car;
            x2++;
            }
        }

    if (aff==2) aff=1;

    if (y2>=yd+yl) break;

    posn++;
    if (posn>=taille)
        {
        pasfini=1;
        lchaine=xl-x2+xd;

        if (yl==Cfg->TailleY-1)
            memset(affichage+x2-xd,'-',lchaine);
            else
            memset(affichage+x2-xd,' ',lchaine);

        affichage[xl]=0;
        PrintAt(xd,y2,"%s",affichage);
        y2++;
        break;
        }
    }
while(1);

while(y2<yd+yl)
    {
    memset(affichage,' ',xl);        // remplit une ligne entiere de espace
    affichage[xl]=0;
    PrintAt(xd,y2,"%s",affichage);
    y2++;
    }

GotoXY(x+xd,y+yd);
if (ins==0)
    PutCur(7,7);
    else
    PutCur(2,7);

posn=posd;

while (!kbhit())
{
// Attend une touche
}

code=Wait(0,0,0);

switch(LO(code))
    {
    case 0:
        switch(HI(code))
            {
            case 0x3E:  // F4
                fin=-2;
                break;
            case 0x4D:  // RIGHT
                x++;
                break;
            case 0x4B:  // LEFT
                x--;
                break;
            case 0x74:  // CTRL RIGHT
                warp+=40;
                break;
            case 0x73:  // CTRL LEFT
                warp-=40;
                break;
            case 0x77:  // CTRL HOME
                warp=0;
                break;
            case 80:    // BAS
                y++;
                break;
            case 72:    // HAUT
                y--;
                break;
            case 0x51:  // PGDN
                y+=20;
                break;
            case 0x52:  // Insere
                ins=ins ? 0 : 1;
                break;
            case 0x49:  // PGUP
                y-=20;
                break;
            case 0x4F:  // END
                x=79;
                while(tableau[y][x]==-1) x--;
                break;
            case 0x47:  // HOME
                x=0;
                break;
            case 0x3C:   // F2
            case 0x3C+8: // F10
                fin=-1;
                break;
            }
        break;
    default:
        
        if (ins==0)
            {
            if (tableau[y][x]!=-1)
                Edit_buffer[tableau[y][x]]=code;

            }
//        AffChr(x,y,code);
        x++;
        break;
    }


while (y<0)
    {
    y++;
    if (posn!=0)
        {
        posn--;
        if (posn!=0)
            {
             do
                {
                posn--;
                if (posn==0) break;
                }
            while(Edit_buffer[posn]!=0x0A);
            if (posn!=0) posn++;
            }
        }
    }

while (y>=yl)
    {
    y--;
    do
        {
        posn++;
        if (posn==taille)
            {
            posn=taille-2;
            break;
            }
        }
    while(Edit_buffer[posn]!=0x0A);
    posn++;
    }

// PrintAt(0,0,"x: %3d, y: %3d, xl: %3d, yl: %3d",x,y,xl,yl);


if (warp<0) warp=0;

}
while(!fin);


ChargeEcran();

return fin;
}

void Bar(char *bar)
{
int TY;
int i,j,n;

TY=Cfg->TailleY;

n=0;
for (i=0;i<10;i++)
    {
    PrintAt(n,TY-1,"F%d",(i+1)%10);
    for(j=0;j<2;j++,n++) AffCol(n,TY-1,1*16+8);
    for(j=0;j<6;j++,n++)
        {
        AffCol(n,TY-1,1*16+2);
        AffChr(n,TY-1,*(bar+i*6+j));
        }
    }
}


void Edit(struct fenetre *F)
{
FILE *fic;
char *fichier,*liaison;
short i;

SaveEcran();

Bar(" ----  ----  ----  ----  ----  ----  ----  ----  ----  ---- ");

fichier=GetMem(256);
strcpy(fichier,F->path);
Path2Abs(fichier,F->F[F->pcur]->name);

liaison=GetMem(256);
strcpy(liaison,"");

i=InfoIDF(F);

fic=fopen(fichier,"rb");

if (fic==NULL)
    {
    PrintAt(0,0,"Error on file '%s'",fichier);
    WinError("Couldn't open file");
    i=-1;
    }
    else
    {
    taille=filelength(fileno(fic));

    if (taille==0) i=-1;

    fread(Edit_buffer,taille,1,fic);

    }

while(i!=-1)
    {
    switch(i) {
        case 91: //Texte
            i=TxtEdit(fichier);
            break;
        default:
            i=-1;
            break;
        }
    }

if (fic!=NULL)
    fclose(fic);

free(liaison);
free(fichier);

ChargeEcran();
}


