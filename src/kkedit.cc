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

//--- kkt ---------------------------------------------------------------
#include "editsave.h"

#include "kkedit.h"

#define LENMAX 2048



#ifdef STANDALONE_KKEDIT

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

#endif

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

struct line
  {
  struct line *from,*next;
  char *chaine;
  };


int StrLength(char *chaine);
void BipBip(void);
void GotoLine(void);
void EditSearch(int todo);


void InitQFctStack(void);
void PutLIFOQFct(short fct);
int GetLIFOQFct(void);


/*--------------------------------------------------------------------*\
|- variable interne                                                   -|
\*--------------------------------------------------------------------*/
int xm,ym;
int x0,y0;  //--- premiere ligne ---------------------------------------

int cx;  //--- Position du curseur -------------------------------------
int cy,clig; //--- Position du curseur y, et num‚ro de ligne -----------
int py,plig; //--- Pas pour position du curseur y, et num‚ro de ligne --

int nblig;   //--- Nombre de ligne total -------------------------------

struct line *ligne;

char affiche=1;

static char *tampon;
static int nbrtampon;

static char savebit;

/*--------------------------------------------------------------------*\
|- Gestion macros                                                     -|
\*--------------------------------------------------------------------*/

short QFctStack[128];
short QNbrFunct;

/*--------------------------------------------------------------------*\
|- Init Function Stack                                                -|
\*--------------------------------------------------------------------*/
void InitQFctStack(void)
{
QNbrFunct=0;
}

/*--------------------------------------------------------------------*\
|- Put a Function LIFO                                                -|
\*--------------------------------------------------------------------*/
void PutLIFOQFct(short fct)
{
if (QNbrFunct>128)
    {
    WinError("Macro too long");
    return;
    }
QFctStack[QNbrFunct]=fct;
QNbrFunct++;
}

/*--------------------------------------------------------------------*\
|- Get a Function LIFO                                                -|
\*--------------------------------------------------------------------*/
int GetLIFOQFct(void)
{
if (QNbrFunct==0) return 0;
QNbrFunct--;
return QFctStack[QNbrFunct];
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

int StrLength(char *chaine)
{
int l,n;

n=0;
l=0;

while(chaine[n]!=0)
    {
    if (chaine[n]!=32) l=n+1;
    n++;
    }
return l;
}


void SaveFile(struct line *prem,char *fichier)
{
char oldname[256];
struct line *curr;
FILE *fic;
char *ext;

strcpy(oldname,fichier);

ext=strchr(oldname,'.');
if (ext==NULL)
    ext=strchr(oldname,0);
    else
    ext=ext+1;

strcpy(ext,"bak");

unlink(oldname);

rename(fichier,oldname);

/*--------------------------------------------------------------------*\
|-  Sauvegarde du fichier                                             -|
\*--------------------------------------------------------------------*/

fic=fopen(fichier,"wt");

curr=prem;

if (curr!=NULL)
    {
    while(curr->next!=NULL)
        {
        fprintf(fic,"%s\n",curr->chaine);
        prem=curr->next;
        curr=prem;
        }
    }

fclose(fic);

savebit=0;
}

void GotoLine(void)
{
char chaine[32];
int n;

SaveScreen();

ColLin(0,0,Cfg->TailleX,Cfg->col[7]);
ChrLin(0,0,Cfg->TailleX,32);

chaine[0]=0;

PrintAt(0,0,"Goto Line: ");

if (InputAt(11,0,chaine,15)==0)
    {
    sscanf(chaine,"%d",&n);
    plig=n-clig-1;
    }

LoadScreen();
return;
}



void EditSearch(int todo)
{
static char chaine[64]="",option[8]="I";

char icase=0;
int n;

struct line *curr;
int posx,lng,lngcur;
int pl; //--- pas de ligne ---------------------------------------------

SaveScreen();

if (todo==0)
    {
    ColLin(0,0,Cfg->TailleX,Cfg->col[7]);
    ChrLin(0,0,Cfg->TailleX,32);

    PrintAt(0,0,"Search for: ");
    if (InputAt(12,0,chaine,60)==0)
        {
        if (chaine[0]!=0)
            {
            ChrLin(0,0,Cfg->TailleX,32);
            PrintAt(0,0,"Options [I] (Ignore-case): ");

            if (InputAt(27,0,option,6)==0)
                todo=1;
            }
        }
    }

if (todo)
    {
    for(n=0;n<strlen(option);n++)
        switch(option[n])
            {
            case 'i':
            case 'I':
                icase=1;
                break;
            }

    curr=ligne;
    posx=cx+1;
    lng=strlen(chaine);
    lngcur=strlen(curr->chaine);
    pl=0;

    do
        {
        if ( ((icase) & (!strnicmp(curr->chaine+posx,chaine,lng)) ) |
             ((!icase) & (!strncmp(curr->chaine+posx,chaine,lng)) ) )
            {
            plig=pl;
            cx=posx;
            break;
            }

        posx++;

        if (posx+lng>lngcur)
            {
            curr=curr->next;
            if (curr!=NULL)
                {
                lngcur=strlen(curr->chaine);
                posx=0;
                pl++;
                }
            }
        }
    while(curr!=NULL);

    if (curr==NULL)
        WinError("String not found");
    }

LoadScreen();
}


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/
void UpdateLine(char *chaine,struct line *ligne)
{
chaine[StrLength(chaine)]=0;

free(ligne->chaine);
ligne->chaine=(char*)GetMem(strlen(chaine)+1);
strcpy(ligne->chaine,chaine);
}


/*--------------------------------------------------------------------*\
|- return:    0: OK                                                   -|
|-            1: Error                                                -|
\*--------------------------------------------------------------------*/

int TxtEdit(KKEDIT *E,char *fichier)
{
char fin=0;
FILE *fic;
char *chaine2,*chaine;
char fonction=0;

struct line *prem,*dern,*curr,*haut,*l0,*l1;

int blcx0,blcx1;
int bllig0,bllig1;
char col,colt;

int x0_old,y0_old,xm_old,ym_old;

int blcx2,bllig2;

int m,n,o,x,y;
int xp;
int car;
char ins=1;

savebit=0;

chaine2=(char*)GetMem(LENMAX);
chaine=(char*)GetMem(LENMAX);


l0=(line*)GetMem(sizeof(struct line));
l0->from=NULL;

prem=(line*)GetMem(sizeof(struct line));
prem->from=l0;
prem->from->next=prem;

curr=prem;

nblig=0;


fic=fopen(fichier,"rb");
if (fic!=NULL)
    {
    while(fgets(chaine,LENMAX,fic)!=NULL)
        {
        n=strlen(chaine);
        if ( (chaine[n-1]==0x0A) | (chaine[n-1]==0x0D) )  chaine[n-1]=0;
        if ( (chaine[n-2]==0x0A) | (chaine[n-2]==0x0D) )  chaine[n-2]=0;

        curr->next=(line*)GetMem(sizeof(struct line));
        curr->next->from=curr;

        n=StrLength(chaine);

        curr->chaine=(char*)GetMem(n+1);
        memcpy(curr->chaine,chaine,n);
        curr->chaine[n]=0;

        nblig++;

        curr=curr->next;
        }
    fclose(fic);
    }
    else
    {
    curr->next=(line*)GetMem(sizeof(struct line));
    curr->next->from=curr;

    curr->chaine=(char*)GetMem(1);
    curr->chaine[0]=0;

    nblig++;

    curr=curr->next;
    }

l1=(line*)GetMem(sizeof(struct line));

curr->from->next=l1;

l1->from=curr->from;
l1->next=NULL;

free(curr);

haut=l0;

blcx0=0;    bllig0=0;
blcx1=0;    bllig1=0;

InitQFctStack();

SaveScreen();

Bar(" Help  Save  ----  ----  Zoom  ----  ----  ----  ----  ---- ");

ColLin(0,0,Cfg->TailleX,Cfg->col[7]);
ChrLin(0,0,Cfg->TailleX,32);

#ifndef STANDALONE_KKEDIT
x0_old=DFen->x+1;
y0_old=DFen->y+1;

xm_old=DFen->xl-DFen->x-1;
ym_old=DFen->yl-DFen->y-1;
#else 
x0_old = 0;
y0_old = 0;
xm_old = 1;
ym_old = 1;
#endif

if ((E->zoom)==1)
    {
    xm=Cfg->TailleX;        //--- Taille en X
    ym=Cfg->TailleY-2;      //--- Taille en Y
    x0=0;                   //--- Debut X
    y0=1;                   //--- Debut Y
    }
else
    {
    xm=xm_old;              //--- Taille en X
    ym=ym_old;              //--- Taille en Y
    x0=x0_old;              //--- Debut X
    y0=y0_old;              //--- Debut Y
    }

xp=0;

Window(x0,y0,x0+xm-1,y0+ym-1,Cfg->col[16]);

if (ins==0)
    PutCur(7,7);
    else
    PutCur(2,7);


cx=0;

cy=0;
clig=0;

py=0;
plig=0;

tampon=NULL;
nbrtampon=0;

/*--------------------------------------------------------------------*\
|- Boucle principale                                                  -|
\*--------------------------------------------------------------------*/

// haut: la ligne avant la premier affich‚e

do
{
if (affiche==1)  //--- Affichage de toute la page ----------------------
    {
    curr=haut->next;
    ligne=NULL;

    for(y=0;y<ym;y++)
        {
        n=0;

        if (curr!=l1)
            {
            if (curr->chaine==NULL)
                WinError("erreur");

            n=strlen(curr->chaine)-xp;

            if (n<0)
                n=0;

            if (n>xm)
                n=xm;

            for(x=0;x<n;x++)
                AffChr(x+x0,y+y0,curr->chaine[x+xp]);

            if (y==cy)
                ligne=curr;

            dern=curr;
            curr=curr->next;
            }
            else
            {
            n=xm;

            for(x=0;x<n;x++)
                AffChr(x+x0,y+y0,177);

            if (ligne==NULL)
                {
                ligne=dern;
                clig-=(cy-(y-1));
                cy=y-1;
                }
            }

        for(x=n;x<xm;x++)
            AffChr(x+x0,y+y0,32);
        }

    memset(chaine,32,LENMAX-1);
    chaine[LENMAX-1]=0;

    memcpy(chaine,ligne->chaine,strlen(ligne->chaine));
    affiche=0;
    }

if (tampon!=NULL)
    {
    nbrtampon--;
    car=tampon[nbrtampon];

    if (nbrtampon==0)
        {
        LibMem(tampon);
        tampon=NULL;

        bllig0=bllig2;
        blcx0=blcx2;

        affiche=1;
        }
    }
    else
    {
    PrintAt(0,0,"L%4d C%3d %c %s",clig,cx,savebit ? '*' : ' ',fichier);

    GotoXY(cx+x0-xp,cy+y0);
    // putcur


    n=clig-cy;

    if ( (bllig0<n) & (bllig1>n) )
        col=Cfg->col[18];   // Block
        else
        col=Cfg->col[16];   // Normal

    curr=haut->next;

    for(y=0;y<ym;y++,n++)
        {
        if (curr!=NULL)
            {
            if ( ((n!=bllig0) & (n!=bllig1)) | (bllig1<bllig0) )
                {
                if ((y==cy) & (col!=Cfg->col[18]))
                    for(x=0;x<xm;x++)
                        AffCol(x+x0,y+y0,Cfg->col[17]); // Selection
                    else
                    for(x=0;x<xm;x++)
                        AffCol(x+x0,y+y0,col);
                }
                else
                {
                if (y==cy)
                    colt=Cfg->col[17];
                    else
                    colt=Cfg->col[16];

                if ((n==bllig0) & (n!=bllig1))
                    {
                    for(x=0;x<blcx0;x++)
                        AffCol(x+x0,y+y0,colt);
                    for(x=blcx0;x<xm;x++)
                        AffCol(x+x0,y+y0,Cfg->col[18]);
                    }
                if ((n!=bllig0) & (n==bllig1))
                    {
                    for(x=0;x<blcx1;x++)
                        AffCol(x+x0,y+y0,Cfg->col[18]);
                    for(x=blcx1;x<xm;x++)
                        AffCol(x+x0,y+y0,colt);
                    }
                if ((n==bllig0) & (n==bllig1))
                    {
                    for(x=0;x<blcx0;x++)
                        AffCol(x+x0,y+y0,colt);
                    for(x=blcx0;x<blcx1;x++)
                        AffCol(x+x0,y+y0,Cfg->col[18]);
                    for(x=blcx1;x<xm;x++)
                        AffCol(x+x0,y+y0,colt);
                    }
                if (n==bllig0) col=Cfg->col[18];
                if (n==bllig1) col=Cfg->col[16];
                }

            curr=curr->next;
            }
        }

    for(n=0;n<xm;n++)
        AffChr(n+x0,cy+y0,chaine[n+xp]);

    car=GetLIFOQFct();

    if (car==0)
        car=Wait(cx+x0-xp,cy+y0);
    }


switch(car)
    {
    case KEY_F(1):  //--- HELP -----------------------------------------
        HelpTopic(45);
        break;

    case KEY_F(2):  //--- SAVE -----------------------------------------
        UpdateLine(chaine,ligne);
        SaveFile(l0->next,fichier);
        break;

    case KEY_F(3):
        {
                int i,j;
                for(i=0;i<16;i++)
                        for(j=0;j<16;j++)
                        {
                                ColLin(i*5+5,j+5,3,i+j*16);
                                PrintAt(i*5+6,j+5,"%3d", i+j*16);
                        }
                while(Wait(cx+x0-xp,cy+y0)==ERR);

                             {int n;
                                                           for (n=0;n<40;n++) {
                                                                                                 attrset(COLOR_PAIR(n % 8));
                                                                                                                               mvaddch(0,n,65);
                                                                                                                                                     }
                                                                         }
                              getch();
        }

    case KEY_F(5):  //--- ZOOM -----------------------------------------
        LoadScreen();
        SaveScreen();
        if ((E->zoom)==0)
            {
            xm=Cfg->TailleX;        //--- Taille en X
            ym=Cfg->TailleY-2;      //--- Taille en Y
            x0=0;                   //--- Debut X
            y0=1;                   //--- Debut Y
            E->zoom=1;
            }
        else
            {
            xm=xm_old;              //--- Taille en X
            ym=ym_old;              //--- Taille en Y
            x0=x0_old;              //--- Debut X
            y0=y0_old;              //--- Debut Y
            E->zoom=0;
            }

        Window(x0,y0,x0+xm-1,y0+ym-1,Cfg->col[16]);
        affiche=1;
        break;

    case KEY_PPAGE: //--- PGUP -----------------------------------------
        plig-=ym;
        break;

    case KEY_NPAGE: //--- PGDN -----------------------------------------
        plig+=ym;
        break;

    case 0x8400:    //--- CTRL-PGUP ------------------------------------
        py=-cy;
        plig=-clig;
        break;

    case 0x7600:    //--- CTRL-PGDN ------------------------------------
        plig=nblig-clig;
        break;

    case 0x7700:    //--- CTRL-HOME ------------------------------------
        py=-cy;
        plig=-cy;
        break;

    case 0x7500:    //--- CTRL-END -------------------------------------
        py=ym-cy-1;
        plig=ym-cy-1;
        break;

    case KEY_UP:    //--- UP -------------------------------------------
        py--;
        plig--;
        break;

    case KEY_DOWN:  //--- DOWN -----------------------------------------
        py++;
        plig++;
        break;

    case KEY_RIGHT: //--- RIGHT ----------------------------------------
        cx++;
        break;

    case KEY_LEFT:  //--- LEFT -----------------------------------------
        if (cx!=0)
            cx--;
        break;

    case KEY_END:   //--- END ------------------------------------------
        cx=StrLength(chaine);
        break;

    case KEY_HOME:  //--- HOME -----------------------------------------
        cx=0;
        break;

    case KEY_DC:    //--- DELETE ---------------------------------------
        savebit=1;
        n=StrLength(chaine);

        if (n!=0)
            {
            for(x=cx;x<n;x++)
                chaine[x]=chaine[x+1];
            }
            else
            {


            }
        break;

    case KEY_BACKSPACE: //--- DEL <-- ----------------------------------
        savebit=1;
        if (cx!=0)
            {
            if ( (bllig0==clig) & (blcx0>=cx) ) blcx0--;
            if ( (bllig1==clig) & (blcx1>=cx) ) blcx1--;

            cx--;

            if (ins)
                {
                for(x=cx;x<LENMAX-1;x++)
                    chaine[x]=chaine[x+1];

                chaine[LENMAX-2]=32;
                }
                else
                {
                chaine[cx]=32;
                }

            }
            else    //--- Delete line ----------------------------------
            {
            if (ligne->from==l0)
                {
                BipBip();
                break;
                }

            if ((StrLength(ligne->from->chaine)+StrLength(chaine))>LENMAX)
                {
                BipBip();
                break;
                }

            free(ligne->chaine);
            ligne->from->next=ligne->next;
            ligne->next->from=ligne->from;

            curr=ligne->from;
            free(ligne);
            ligne=curr;

            strcpy(chaine2,chaine);
            chaine2[StrLength(chaine2)]=0;
            cx=strlen(chaine2);
            strcpy(chaine,ligne->chaine);
            cx=strlen(chaine);
            chaine[StrLength(chaine)]=0;
            strcat(chaine,chaine2);

            if ((clig==bllig0) & (cx<=blcx0))
                blcx0+=strlen(ligne->chaine);

            if ((clig==bllig1) & (cx<=blcx1))
                blcx1+=strlen(ligne->chaine);

            if (clig<=bllig0) bllig0--;
            if (clig<=bllig1) bllig1--;

            cx=StrLength(ligne->chaine);

            py--;
            plig--;

            affiche=1;
            }
        break;

    case 0x09:  //--- TAB ----------------------------------------------
        savebit=1;
        n=4-(cx%4);
        while(n!=0)
            PutLIFOQFct(32),n--;
        break;

    case 0x0A:  //--- CTRL-J ou CTRL-ENTER -----------------------------
        GotoLine();
        break;

    case 0x0B:  //--- CTRL-K -------------------------------------------
        car=Wait(0,0);
        switch(car)
            {
            case 'b':
            case 'B':
            case 0x02:
                blcx0=cx;
                bllig0=clig;
                affiche=1;
                break;

            case 'k':
            case 'K':
            case 0x0B:
                blcx1=cx;
                bllig1=clig;
                affiche=1;
                break;

            case 'c':
            case 'C':
            case 0x03:
                savebit=1;
                fonction=1;
                affiche=1;
                break;
            }
        break;

    case 0x0C:  //--- CTRL-L -------------------------------------------
        EditSearch(1);
        break;

    case 0x11:  //--- CTRL-Q -------------------------------------------
        car=Wait(0,0);
        switch(car)
            {
            case 'f':
            case 'F':
            case 0x06:
                EditSearch(0);
                break;
            }
        break;

    case 0x0D:  //--- ENTER, CTRL-M ------------------------------------
        savebit=1;
        curr=ligne->next;
        ligne->next=(line*)GetMem(sizeof(struct line));
        ligne->next->next=curr;
        ligne->next->from=ligne;
        curr->from=ligne->next;

        n=cx;
        m=0;
        while(n>=0)
            {
            if (chaine[n]!=32)
                m=n;
            n--;
            }

        if (tampon!=NULL)
            m=0;

        ligne->next->chaine=(char*)GetMem(strlen(chaine)+1-cx+m);
        strcpy(ligne->next->chaine+m,chaine+cx);
        memset(ligne->next->chaine,32,m);
        chaine[cx]=0;

        affiche=1;

        if ((clig==bllig0) & (cx<=blcx0))
            {
            blcx0+=m-strlen(chaine);
            bllig0++;
            }
            else
            if (clig<bllig0) bllig0++;

        if ((clig==bllig1) & (cx<=blcx1))
            {
            blcx1+=m-strlen(chaine);
            bllig1++;
            }
            else
            if (clig<bllig1) bllig1++;

        py++;
        plig++;

        cx=m;
        break;

    case 0x19:  //--- CTRL-Y -------------------------------------------
        savebit=1;
        if ((ligne->next==NULL) | (ligne->from==NULL))
            {
            BipBip();
            break;
            }

        free(ligne->chaine);
        ligne->from->next=ligne->next;
        ligne->next->from=ligne->from;

        curr=ligne->from;
        free(ligne);
        ligne=curr;

        memset(chaine,32,LENMAX-1);
        chaine[LENMAX-1]=0;

        memcpy(chaine,ligne->chaine,strlen(ligne->chaine));
        affiche=1;

        if (clig==bllig0)
            blcx0=0;

        if (clig==bllig1)
            blcx1=0;

        if (clig<=bllig0) bllig0--;
        if (clig<=bllig1) bllig1--;

        affiche=1;
        break;

    case 0x1B:  //--- ESC ----------------------------------------------
    case 0x2D00: //--- ALT X -------------------------------------------
#ifndef STANDALONE_KKEDIT
        if (savebit)
            {
            switch(MWinTraite(editsave_kkt))
                {
                case 1:         //--- Yes
                    UpdateLine(chaine,ligne);
                    SaveFile(l0->next,fichier);
                    break;
                case 2:         //--- No
                    break;
                default:        //--- Cancel
                    car=0;
                    break;
                }
            }
#else
        UpdateLine(chaine,ligne);
        SaveFile(l0->next,fichier);
#endif

        affiche=1;
        break;

    case 0x5200: //--- Insert ------------------------------------------
        ins=(ins==1) ? 0:1;
        if (ins==0)
            PutCur(7,7);
        else
            PutCur(2,7);
        break;

    case ERR: // Nokey
        break;

    default:
        if (LO(car)==0)
            {
            PrintAt(75,0,"%04X",car);
            break;
            }
        savebit=1;
        if (ins==1)
            {
            if ((clig==bllig0) & (cx<=blcx0))
                blcx0++;

            if ((clig==bllig1) & (cx<=blcx1))
                blcx1++;

            n=StrLength(chaine);

            if (cx<n)
                {
                for(x=n+1;x>=cx;x--)
                    chaine[x+1]=chaine[x];
                }
            }

        if (car==13)
            WinError("treize");

        chaine[cx]=(char)car;

        cx++;
        break;
    }



if ((cx-xm)>=xp)
    {
    xp=cx-xm+1;
    affiche=1;
    }

if (cx<xp)
    {
    xp=cx;
    affiche=1;
    }



if ( (plig!=0) | (py!=0) )
    {
    if (cy+py<0) py=-cy;
    if (cy+py>=ym) py=-cy+ym-1;

    //--- on se place sur cy = 0 ---------------------------------------

    n=plig-py;

    clig+=plig;
    cy+=py;

    dern=ligne;

    while(n!=0)
        {
        if (n<0)
            {
            if (haut!=l0)
                haut=haut->from;
                else
                {
                if (cy!=0)
                    cy--;
                    else
                    clig++;
                }
            n++;
            }
        if (n>0)
            {
            if (dern->next!=l1)
                {
                haut=haut->next;
                dern=dern->next;
                }
                else
                {
                if (cy<ym-1)
                    cy++;
                    else
                clig--;
                }

            n--;
            }
        }

    plig=0;
    py=0;

    affiche=1;
    }

if (affiche==1)  //--- Sauve la chaine dans la liste des chaines -------
    {
    chaine[StrLength(chaine)]=0;

    free(ligne->chaine);
    ligne->chaine=(char*)GetMem(strlen(chaine)+1);
    strcpy(ligne->chaine,chaine);
    }

switch(fonction)
    {
    case 1:
        //--- Number of byte in selection ------------------------------

        curr=l0->next;
        n=0;
        while(n!=bllig0)
            curr=curr->next,n++;

        nbrtampon=-(blcx0+1);

        while(n<=bllig1)
            {
            nbrtampon+=strlen(curr->chaine)+1;
            curr=curr->next;
            n++;
            }
        nbrtampon-=(strlen(curr->from->chaine)-blcx1);

        //--- Allocation of the buffer ---------------------------------
        tampon=(char*)GetMem(nbrtampon);

        m=nbrtampon-1;
        curr=l0->next;
        n=0;
        while(n!=bllig0)
            curr=curr->next,n++;

        if (n!=bllig1)
            {
            for(o=blcx0;o<strlen(curr->chaine);o++,m--)
                tampon[m]=curr->chaine[o];
            tampon[m]=13,m--;
            curr=curr->next;
            n++;

            while(n<bllig1)
                {
                for(o=0;o<strlen(curr->chaine);o++,m--)
                    tampon[m]=curr->chaine[o];
                tampon[m]=13,m--;
                curr=curr->next;
                n++;
                }

            for(o=0;o<blcx1;o++,m--)
                tampon[m]=curr->chaine[o];
            }
            else
            {
            for(o=blcx0;o<blcx1;o++,m--)
                tampon[m]=curr->chaine[o];
            }

        bllig1=clig;
        blcx1=cx;

        bllig2=clig;
        blcx2=cx;
        break;

    }
fonction=0;



}
while ((car!=27) & (car!=0x2D00));  //--- Ni Escape, ni ALT-X -----------

LoadScreen();


/*--------------------------------------------------------------------*\
|- desalloue la ram                                                   -|
\*--------------------------------------------------------------------*/

curr=l0;

while(curr!=NULL)
    {
    prem=curr->next;
    LibMem(curr);
    curr=prem;
    }

LibMem(chaine2);
LibMem(chaine);

return fin;
}





void BipBip(void)
{
int x,y;
char c;

for(y=0;y<Cfg->TailleY;y++)
    for(x=0;x<Cfg->TailleX;x++)
        {
        c=GetCol(x,y);
        AffCol(x,y,(c&15)*16+c/16);
        }
Delay(1);

for(y=0;y<Cfg->TailleY;y++)
    for(x=0;x<Cfg->TailleX;x++)
        {
        c=GetCol(x,y);
        AffCol(x,y,(c&15)*16+c/16);
        }

QNbrFunct=0;
}
