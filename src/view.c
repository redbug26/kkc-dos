// Viewer

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

int HexaView(char *fichier);
int TextView(char *fichier);
int HtmlView(char *fichier,char *liaison);

int HexaView(char *fichier)
{
FILE *fic;
int x,y,z,k,kl,ks;

long cur1,cur2;

long taille;

char *buffer;

int car;


int fin=0; // Code de retour

fic=fopen(fichier,"rb");

if (fic==NULL)
    {
    PrintAt(0,0,"Error on file '%s'",fichier);
    WinError("Couldn't open file");
    return -1;
    }
taille=filelength(fileno(fic));

if (taille==0) return -1;

buffer=GetMem(32768);

SaveEcran();
PutCur(3,0);


ColWin(1,1,78,(Cfg->TailleY)-3,10*16+1);
ChrWin(1,1,78,(Cfg->TailleY)-3,32);

WinCadre(0,3,9,(Cfg->TailleY)-2,2);

WinCadre(10,3,58,(Cfg->TailleY)-2,2);

WinCadre(59,3,76,(Cfg->TailleY)-2,2);

WinCadre(77,3,79,(Cfg->TailleY)-2,2);

WinCadre(0,0,79,2,3);

ChrCol(34,4,(Cfg->TailleY)-6,Cfg->Tfont[0]);

z=0;

PrintAt(3,1,"View File %s",fichier);

fseek(fic,z,SEEK_SET);

fread(buffer,32768,1,fic);

k=0;

//

do
{
if ((z+k+(((Cfg->TailleY)-6)*16) )>taille)
    kl=taille-z;
    else
    kl=k+(((Cfg->TailleY)-6)*16);

if ( (kl>32768) & (kl<0) )
    {
    kl=kl-k;
    z=z+k;
    k=0;

    fseek(fic,z,SEEK_SET);
    fread(buffer,32768,1,fic);
    }


if (taille<1024*1024)
    {
    cur1=(z+k)*(Cfg->TailleY-7);
    cur1=cur1/taille+4;

    cur2=(z+kl)*(Cfg->TailleY-7);
    cur2=cur2/taille+4;
    }


ChrCol(78,4,cur1-4,32);

ChrCol(78,cur1,cur2-cur1+1,219);

ChrCol(78,cur2+1,(Cfg->TailleY-3)-cur2,32);

ks=k;
     
for (y=0;y<Cfg->TailleY-6;y++)
    {
    PrintAt(1,y+4,"%08X",z+k);

    for (x=0;x<16;x++)
        {
        if (k<kl)
            {
            PrintAt(x*3+11,y+4,"%02X",(unsigned char)(buffer[k]));
            AffChr(x+60,y+4,buffer[k]);
            }
            else
            {
            PrintAt(x*3+11,y+4,"  ");
            AffChr(x+60,y+4,32);
            }

        k++;
        }
    }

k=ks;

car=Wait(0,0,0);

switch(LO(car))   {
    case 0:
        switch(HI(car))   {
            case 80:    // BAS
                k+=16;
                break;
            case 72:    // HAUT
                k-=16;
                break;
            case 0x51:  // PGDN
                k+=480;
                break;
            case 0x49:  // PGUP
                k-=480;
                break;
            case 0x47:  // HOME
                k=-z;
                break;
            case 0x4F:  // END
                k=taille-((((Cfg->TailleY)-6)*16))-z+15;
                k=k/16;
                k=k*16;
                break;
            case 0x43:  // F9
                fin=86;
                break;
            case 0x8D:  // CTRL-UP
                fin=-1;
                break;
            default:
                break;
            }
        break;
    case 27:
        fin=-1;
        break;
    default:
        break;
    }

while (k>=taille-((((Cfg->TailleY)-7)*16))-z) k-=16;

while ((z+k)<0) k+=16;

}
while(fin==0);

ChargeEcran();

free(buffer);

fclose(fic);

return fin;
}

/*
 * to initialize:
 *   call set_screensize(<# lines to reserve>);
 * to print through ansi interpreter:
 *   call ansi_out(<string>);
 */

static char curattr = 7;
static int curx = 0,cury = 0;
static int maxx = 80, maxy = 25;  /* size of ansi output window */
static char useansi = 1;           /* while true, interp ansi seqs */
static int tabspaces = 8;
static int savx,savy,issaved = 0;
static char ansi_terminators[] = "HFABCDnsuJKmp";

static int touche;
static clock_t Cl_Start;

int tempo;

#define MAXARGLEN       128

#define NOTHING         0
#define WASESCAPE       1
#define WASBRKT         2

void set_pos (char *argbuf,int arglen,char cmd)
{
      int   y,x;
      char *p;

      if (!*argbuf || !arglen)
      {
            curx = cury = 0;
      }
      y = atoi(argbuf) - 1;
      p = strchr(argbuf,';');
      if (y >= 0 && p)
      {
            x = atoi(p + 1) - 1;
            if(x >= 0)
            {
                  curx = x;
                  cury = y;
            }
      }
}

void go_up (char *argbuf,int arglen,char cmd)
{
      int x;

      x = atoi(argbuf);
      if (!x)
            x = 1;
      for ( ; x ; x--)
      {
            if (!cury)
                  break;
            cury--;
      }
}

void go_down (char *argbuf,int arglen,char cmd)
{
      int x;

      x = atoi(argbuf);
      if (!x)
            x = 1;
      for ( ; x ; x--)
      {
            if (cury == maxy - 1)
                  break;
            cury++;
      }
}

void go_left (char *argbuf,int arglen,char cmd)
{
      int x;

      x = atoi(argbuf);
      if (!x)
            x = 1;
      for ( ; x ; x--)
      {
            if(!curx)
                  break;
            curx--;
      }
}

void go_right (char *argbuf,int arglen,char cmd)
{
      int x;

      x = atoi(argbuf);
      if (!x)
            x = 1;
      for ( ; x ; x--)
      {
            if (curx == maxx - 1)
                  break;
            curx++;
      }
}

void report (char *argbuf,int arglen,char cmd)
{
      /* you figure out how to implement it ... */
}

void save_pos (char *argbuf,int arglen,char cmd)
{
      savx = curx;
      savy = cury;
      issaved = 1;
}

void restore_pos (char *argbuf,int arglen,char cmd)
{
      if(issaved)
      {
            curx = savx;
            cury = savy;
            issaved = 0;
      }
}



void set_colors (char *argbuf,int arglen,char cmd)
{
      char *p,*pp;

      if (*argbuf && arglen)
      {
            pp = argbuf;
            do
            {
                  p = strchr(pp,';');
                  if (p && *p)
                  {
                        *p = 0;
                        p++;
                  }
                  switch (atoi(pp))
                  {
                  case 0: /* all attributes off */
                        curattr = 7;
                        break;

                  case 1: /* bright on */
                        curattr |= 8;
                        break;

                  case 2: /* faint on */
                        curattr &= (~8);
                        break;

                  case 3: /* italic on */
                        break;

                  case 5: /* blink on */
                        curattr |= 128;
                        break;

                  case 6: /* rapid blink on */
                        break;

                  case 7: /* reverse video on */
                        curattr = 112;
                        break;

                  case 8: /* concealed on */
                        curattr = 0;
                        break;

                  case 30: /* black fg */
                        curattr &= (~7);
                        break;

                  case 31: /* red fg */
                        curattr &= (~7);
                        curattr |= 4;
                        break;

                  case 32: /* green fg */
                        curattr &= (~7);
                        curattr |= 2;
                        break;

                  case 33: /* yellow fg */
                        curattr &= (~7);
                        curattr |= 6;
                        break;

                  case 34: /* blue fg */
                        curattr &= (~7);
                        curattr |= 1;
                        break;

                  case 35: /* magenta fg */
                        curattr &= (~7);
                        curattr |= 5;
                        break;

                  case 36: /* cyan fg */
                        curattr &= (~7);
                        curattr |= 3;
                        break;

                  case 37: /* white fg */
                        curattr |= 7;
                        break;

                  case 40: /* black bg */
                        curattr &= (~112);
                        break;

                  case 41: /* red bg */
                        curattr &= (~112);
                        curattr |= (4 << 4);
                        break;

                  case 42: /* green bg */
                        curattr &= (~112);
                        curattr |= (2 << 4);
                        break;

                  case 43: /* yellow bg */
                        curattr &= (~112);
                        curattr |= (6 << 4);
                        break;

                  case 44: /* blue bg */
                        curattr &= (~112);
                        curattr |= (1 << 4);
                        break;

                  case 45: /* magenta bg */
                        curattr &= (~112);
                        curattr |= (5 << 4);
                        break;

                  case 46: /* cyan bg */
                        curattr &= (~112);
                        curattr |= (3 << 4);
                        break;

                  case 47: /* white bg */
                        curattr |= 112;
                        break;

                  case 48: /* subscript bg */
                        break;

                  case 49: /* superscript bg */
                        break;

                  default: /* unsupported */
                        break;
                  }
                  pp = p;
            } while (p);
      }
}



int ansi_out (char *buf)
{
int  arglen = 0, ansistate = NOTHING, x;
char *b = buf, argbuf[MAXARGLEN] = "";



if (!useansi)                       /* is ANSI interp on? */
    {
    ansistate = NOTHING;
    arglen = 0;
    *argbuf = 0;
    }

while (*b)
    {
    tempo++;

    if (touche!=1)
        {
        if (kbhit()) touche=Wait(0,0,0);
        if ( (touche==27) | (touche==0x8D00) ) touche=1; else touche=0;
        }
        else
        break;

    if (tempo>=66)  // Mode 14400 baud
        {
        tempo=0;
        
        while ((clock()-Cl_Start)<1);
        Cl_Start=clock();
        }

    switch (ansistate)
            {
            case NOTHING:
                  switch (*b)
                  {
                  case '\x1b':
                        if (useansi)
                        {
                              ansistate = WASESCAPE;
                              break;
                        }

                  case '\r':
                        curx = 0;
                        break;

                  case '\n':
                        cury++;
                        if (cury > maxy - 1)
                        {
                        ScrollUp();
                        ChrWin(0,maxy-1,maxx-1,maxy,32);
                        ColWin(0,maxy-1,maxx-1,maxy,curattr);
                        cury--;
                        }
                        break;

                  case '\t':     /* so _you_ figure out what to do... */
                        for (x = 0; x < tabspaces; x++)
                        {
                        AffChr(curx,cury,' ');
                        AffCol(curx,cury,curattr);
                              curx++;
                              if (curx > maxx - 1)
                              {
                                    curx = 0;
                                    cury++;
                                    if (cury > maxy - 1)
                                    {
                                    ScrollUp();
                                    ChrWin(0,maxy-1,maxx-1,maxy,32);
                                    ColWin(0,maxy-1,maxx-1,maxy,curattr);
                                    cury--;
                                    }
                              }
                        }
                        break;

                  case '\b':
                        if (curx)
                        {
                              curx--;
                        }
                        break;

                  case '\07':     /* usually a console bell */
                        putchar('\07');
                        break;

                  default:
                        AffChr(curx,cury,*b);
                        AffCol(curx,cury,curattr);
                        curx++;
                        if (curx > maxx - 1)
                        {
                              curx = 0;
                              cury++;
                              if (cury > maxy - 1)
                              {
                              ScrollUp();
                              ChrWin(0,maxy-1,maxx-1,maxy,32);
                              ColWin(0,maxy-1,maxx-1,maxy,curattr);
                              cury--;
                              }

                        }
                        break;
                  }
                  break;

            case WASESCAPE:
                  if (*b == '[')
                  {
                        ansistate = WASBRKT;
                        arglen = 0;
                        *argbuf = 0;
                        break;
                  }
                  ansistate = NOTHING;
                  break;

            case WASBRKT:
                  if (strchr(ansi_terminators, (int)*b))
                  {
                        switch ((int)*b)
                        {
                        case 'H':   /* set cursor position */
                        case 'F':
                              set_pos(argbuf,arglen,*b);
                              break;

                        case 'A':   /* up */
                              go_up(argbuf,arglen,*b);
                              break;

                        case 'B':   /* down */
                              go_down(argbuf,arglen,*b);
                              break;

                        case 'C':   /* right */
                              go_right(argbuf,arglen,*b);
                              break;

                        case 'D':   /* left */
                              go_left(argbuf,arglen,*b);
                              break;

                        case 'n':   /* report pos */
                              report(argbuf,arglen,*b);
                              break;

                        case 's':   /* save pos */
                              save_pos(argbuf,arglen,*b);
                              break;

                        case 'u':   /* restore pos */
                              restore_pos(argbuf,arglen,*b);
                              break;

                        case 'J':   /* clear screen */
                            ChrWin(0,0,maxx - 1,maxy - 1,32);
                            ColWin(0,0,maxx - 1,maxy - 1,curattr);
                            curx = cury = 0;
                            break;

                        case 'K':   /* delete to eol */
                            ChrWin(curx,cury,maxx-1,cury,32);
                            ColWin(curx,cury,maxx-1,cury,curattr);
                            break;

                        case 'm':   /* set video attribs */
                              set_colors(argbuf,arglen,*b);
                              break;

                        case 'p':   /* keyboard redef -- disallowed */
                              break;

                        default:    /* unsupported */
                              break;
                        }
                        ansistate = NOTHING;
                        arglen = 0;
                        *argbuf = 0;
                  }
                  else
                  {
                        if (arglen < MAXARGLEN)
                        {
                              argbuf[arglen] = *b;
                              argbuf[arglen + 1] = 0;
                              arglen++;
                        }
                  }
                  break;

            default:
                WinError("Error in ANSI state machine");
                break;
            }
            b++;
      }

return ((int)b - (int)buf);
}



int AnsiView(char *fichier)
{
char *buffer;
FILE *fic;
int n;

fic=fopen(fichier,"rb");

if (fic==NULL)
    {
    PrintAt(0,0,"Error on file '%s'",fichier);
    WinError("Couldn't open file");
    return -1;
    }

buffer=GetMem(32768);

SaveEcran();

maxx=80;
maxy=Cfg->TailleY;

ChrWin(0,0,79,(Cfg->TailleY)-2,32);
ColWin(0,0,79,(Cfg->TailleY)-2,0*16+7);

TXTMode(Cfg->TailleY);

touche=0;

curx=0;
cury=0;

Cl_Start=clock();

do
    {
    n=fread(buffer,1,16384,fic);
    memset(buffer+n,0,32768-n);
    ansi_out(buffer);
    }
while(n==16384);


getch();

SetTaille();


ChargeEcran();
free(buffer);
fclose(fic);

return (-1);
}


int TextView(char *fichier)
{
FILE *fic;

long n,taille;

char *buffer;

int xl,yl;
int x,y;

char car;


fic=fopen(fichier,"rb");

if (fic==NULL)
    {
    PrintAt(0,0,"Error on file '%s'",fichier);
    WinError("Couldn't open file");
    return -1;
    }
taille=filelength(fileno(fic));

if (taille==0) return -1;

if (taille>4000) return 0;

buffer=GetMem(32768);

fread(buffer,32768,1,fic);

fclose(fic);

x=0;
y=0;
xl=0;

for (n=0;n<taille;n++)
    switch(buffer[n])  {
        case 13:
            x=0;
            y++;
            break;
        case 10:
            break;
        default:
            x++;
            if (x>xl) xl=x;
            break;
        }

if (x==0)
    yl=y-1;
    else
    yl=y;
xl--;

if ( (xl>=78) | (yl>=(Cfg->TailleY-2)) )
    {
    free(buffer);
    return 0;
    }

SaveEcran();
PutCur(3,0);

x=(80-xl)/2;
y=(Cfg->TailleY-yl)/2;

WinCadre(x-1,y-1,x+xl+1,y+yl+1,3);

ChrWin(x,y,x+xl,y+yl,32);
ColWin(x,y,x+xl,y+yl,10*16+1);

xl=x;
yl=y;

for (n=0;n<taille;n++)
    {
    car=buffer[n];

    switch(car)  {
        case 13:
            car=0;
            xl=x;
            yl++;
            break;
        case 10:
            car=0;
            break;
        case 'Š':   car=232; break;
        case '‚':   car=233; break;
        case 'ˆ':   car=234; break;
        case '‰':   car=235; break;

        case '…':   car=224; break;
        case ' ':   car=225; break;
        case 'ƒ':   car=226; break;
        case '„':   car=227; break;

        default:
            break;
        }

    if (car!=0)
        {
        AffChr(xl,yl,car);
        xl++;
        }
    }

Wait(0,0,0);

ChargeEcran();

free(buffer);

return -1;
}


struct Href {
    char *link;
    int x1,y1;
    int x2,y2;
    struct Href *next;
    };



int HtmlView(char *fichier,char *liaison)
{
FILE *fic;
char titre[256];
short lentit;

char chaine[256];

char mot[128];  // phrase a ecrire
char motc[128]; // couleur de cette phrase
short smot;     // Taille du mot

long n,taille,lu;

char *buffer;

long debut;     // position du <
long code;      // position du &

char aff;       // vaut 1 si il faut ecrire
char prev;      // vaut 1 si la derniere commande est enter

int yp;    // position virtuelle sur ecran
int xl,yl,ye;
int x,y;

char ahref; // nombre de ref en cours

unsigned char col;

unsigned char tabcol[32];
short nbrcol;

char car;

int i,j,k;        // Compteur

char bold,ital,unde;
char pre;
char nlist;      // nombre de liste
char listn[16];  // position dans liste
char listt[16];  // type de liste

short xpage=78;

struct Href *prem,*suiv,*rete;

int ix,iy;


char *ColTxt;
char *ChrTxt;

struct info Info;


ColTxt=GetMem(320000);
memset(ColTxt,10*16+1,320000);
ChrTxt=GetMem(320000);
memset(ChrTxt,32,320000);


fic=fopen(fichier,"rb");

if (fic==NULL)
    {
    PrintAt(0,0,"Error on file '%s'",fichier);
    WinError("Couldn't open file");
    return -1;
    }
taille=filelength(fileno(fic));

if (taille==0) return -1;

buffer=GetMem(32768);



SaveEcran();
PutCur(3,0);

x=1;
y=1;

xl=78;
yl=(Cfg->TailleY)-2;

WinCadre(x-1,y-1,xl+1,yl,2);

ChrWin(x,y,xl,yl-1,32);
ColWin(x,y,xl,yl-1,10*16+1);

ColLin(0,yl+1,80,2*16+5);



//---------------------------------------------------//

prem=(struct Href*)GetMem(sizeof(struct Href));
suiv=prem;


suiv->next=NULL;

ye=0;   // debut


bold=0;
ital=0;
unde=0;

nbrcol=0;
tabcol[0]=10*16+1;

debut=0;
code=0;

prev=1;

yp=0;

aff=0;

ahref=0;

smot=0;

pre=0;

lu=0;

nlist=0;

lentit=0;

for (n=0;n<taille;n++)   {

if (n>=lu) fread(buffer,32768,1,fic),lu+=32768;

car=buffer[n+32768-lu];

if ( (debut==0) & (code==0) )
switch(car)  {
    case '<':
        debut=1;
        car=0;
        break;

    case '&':
        code=1;
        car=0;
        break;

    case 9:
        k=smot/8;
        k=k*8+8;
        k-=smot;

        memset(titre,32,k);
        lentit=k;
        break;

    default:
        break;
    }

if (debut!=0)
switch(car)  {
    case '>':
        if (debut<128)
            {
            titre[debut-1]=0;

            if (!stricmp(titre,"TITLE"))  nbrcol++,tabcol[nbrcol]=3*16+14,aff=1;
            if (!strnicmp(titre,"H1",2))  nbrcol++,tabcol[nbrcol]=3*16+13,aff=1;
            if (!strnicmp(titre,"H2",2))  nbrcol++,tabcol[nbrcol]=4*16+13,aff=1;
            if (!strnicmp(titre,"H3",2))  nbrcol++,tabcol[nbrcol]=5*16+13,aff=1;
            if (!strnicmp(titre,"H4",2))  nbrcol++,tabcol[nbrcol]=3*16+1,aff=1;
            if (!strnicmp(titre,"H5",2))  nbrcol++,tabcol[nbrcol]=4*16+1,aff=1;
            if (!strnicmp(titre,"H6",2))  nbrcol++,tabcol[nbrcol]=5*16+1,aff=1;

            if (!stricmp(titre,"STRONG")) bold++;    // GRAS ON
            if (!stricmp(titre,"B"))      bold++;    // GRAS ON
            if (!stricmp(titre,"EM"))     ital++;    // ITALIQUE ON
            if (!stricmp(titre,"I"))     ital++;    // ITALIQUE ON
            if (!stricmp(titre,"U"))      unde++;    // UNDERLINE ON

            if (!strnicmp(titre,"A HREF",6))
                {
                ahref++;
                nbrcol++;
                tabcol[nbrcol]=2*16+12;
                suiv->link=GetMem(strlen(titre)+1);
                memcpy(suiv->link,titre,strlen(titre)+1);

                suiv->x1=smot;
                suiv->y1=yp;
                }

            if (!strnicmp(titre,"A NAME",6))
                {
                titre[strlen(titre)-1]=0;

                if (!stricmp(titre+8,liaison))
                    ye=yp;
                }


            if (!stricmp(titre,"/A"))
                {
                if (ahref!=0)
                    {
                    nbrcol--;
                    ahref--;
                    suiv->next=(struct Href*)GetMem(sizeof(struct Href));

                    if ( (suiv->x1==smot) & (suiv->y1==yp) )
                        suiv->x2=smot;          // REFERENCE DE 1 DE LARGE
                        else
                        suiv->x2=smot-1;

                    suiv->y2=yp;

                    suiv=suiv->next;
                    suiv->next=NULL;
                    }
                }


            if (!stricmp(titre,"/STRONG")) bold--;    // GRAS OFF
            if (!stricmp(titre,"/B"))      bold--;    // GRAS OFF
            if (!stricmp(titre,"/EM"))     ital--;    // ITALIQUE OFF
            if (!stricmp(titre,"/I"))     ital--;    // ITALIQUE OFF
            if (!stricmp(titre,"/U"))      unde--;    // UNDERLINE OFF
                
            if (!stricmp(titre,"/TITLE")) nbrcol--,aff=2;
            if (!stricmp(titre,"/H1")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H2")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H3")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H4")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H5")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H6")) nbrcol--,aff=1;
            if (!stricmp(titre,"LI"))
                {
                if (nlist!=0)
                    {
                    switch (listt[nlist-1])  {
                        case 1:
                            sprintf(titre,"%c%*s%c ",1,nlist*2,"",7);
                            break;
                        case 2:
                            sprintf(titre,"%c%*s%02d) ",1,nlist*2,"",listn[nlist-1]);
                            break;
                        }
                    listn[nlist-1]++;
                    lentit=strlen(titre);
                    }
                    else
                    {
                    sprintf(titre,"%c%*s%c ",1,nlist*2,"",7);
                    lentit=strlen(titre);
                    }
                }
//            if (!stricmp(titre,"LI"))
            if (!stricmp(titre,"BR"))  aff=1;        // C‚sure forc‚e
            if (!stricmp(titre,"P")) aff=1;         // fin de paragraphe
            if (!stricmp(titre,"OL"))  listt[nlist]=2,listn[nlist]=1,nlist++;
            if (!stricmp(titre,"/OL")) listt[nlist]=0,listn[nlist]=0,nlist--;
            if (!stricmp(titre,"UL"))  listt[nlist]=1,listn[nlist]=1,nlist++;
            if (!stricmp(titre,"/UL")) listt[nlist]=0,listn[nlist]=0,nlist--;

            if (!stricmp(titre,"DL")) aff=1;
            if (!stricmp(titre,"DT")) aff=1;
            if (!stricmp(titre,"DD")) aff=1;
            if (!stricmp(titre,"/DL")) aff=1;
            if (!stricmp(titre,"/DT")) aff=1;
            if (!stricmp(titre,"/DD")) aff=1;

            if (!stricmp(titre,"TR")) aff=1;

            if (!stricmp(titre,"I"))  aff=0;    // Chasse fixe
            if (!stricmp(titre,"/I")) aff=0;

            if (!stricmp(titre,"PRE")) pre++;
            if (!stricmp(titre,"/PRE")) pre--;

            if (!stricmp(titre,"HR"))
                {
                titre[0]=1;
                for(i=1;i<xpage+1;i++)
                    titre[i]=0xC4;
                lentit=xpage+1;
                }
            }

        debut=0;
        car=0;
        break;
    case 0:
        break;
    default:
        titre[debut-1]=car;
        lentit=0;
        debut++;
        car=0;
        break;
    }

if (code!=0)
    switch(car)  {
    case 32:
        titre[code-1]=0;
        sprintf(chaine,"&%s ",titre);
        memcpy(titre,chaine,255);
        lentit=strlen(titre);
        code=0;
        break;
    case ';':
        car=0;
        if (code<128)
            {
            titre[code-1]=0;
            if (!stricmp(titre,"AMP")) car='&';
            if (!stricmp(titre,"EGRAVE")) car=232;
            if (!stricmp(titre,"EACUTE")) car=233;
            if (!stricmp(titre,"ECIRC")) car=234;

            if (!stricmp(titre,"AGRAVE")) car=224;
            if (!stricmp(titre,"AACUTE")) car=225;
            if (!stricmp(titre,"ACIRC")) car=226;
            if (!stricmp(titre,"CCEDIL")) car=231;
            if (!stricmp(titre,"NBSP")) car=32;

            if (!stricmp(titre,"IUML")) car='i';        // En attendant

                
            if (!stricmp(titre,"LT")) car='<';
            if (!stricmp(titre,"GT")) car='>';
            }

        code=0;
        break;
    case 0:
        break;
    default:
        titre[code-1]=car;
        lentit=0;
        code++;
        car=0;
        break;
    }

if (debut>=256)
    {
    lentit=debut-1;
    debut=0;
    }

if (code>=256)
    {
    lentit=code-1;
    code=0;
    }

if (lentit==0)
    {
    lentit=1;
    chaine[0]=car;
    }
    else
    {
    memcpy(chaine,titre,lentit);
    }

for (k=0;k<lentit;k++)
    {
    car=chaine[k];

    if ( (debut==0) & (code==0) )
    switch(car)  {
        case 10:
            if (pre==0)
                car=13;
                else
                aff=1,car=0;
            break;
        case 13:
            car=0;
            break;
        case 1:
            aff=1,car=0;
            break;
        case 'Š':
            car=232; break;
        case '‚':
            car=233; break;
        case 'ˆ':
            car=234; break;
        case '‰':
            car=235; break;

        case '‡':
            car=231; break;

        case '…':
            car=224; break;
        case ' ':
            car=225; break;
        case 'ƒ':
            car=226; break;
        case '„':
            car=227; break;

        default:
            break;
        }

    if (nbrcol<0)
        {
        nbrcol=0;
        }

    col=tabcol[nbrcol];

    if (bold!=0) col=(col&240)+11;
    if (ital!=0) col=(col&240)+12;
    if (unde!=0) col=(col&240)+13;



    if (car!=0)
        {
        if ( (pre==0) & (car==13) & (smot==0) ) car=0;
        if ( (pre==0) & (car==13) ) car=32;

//        if ( (pre==0) & (smot!=0) & (mot[smot-1]==32) & (car==32) ) car=0;

        if (car!=0)
            {
            mot[smot]=car;
            motc[smot]=col;
            smot++;
            }
        }

    if (smot>xpage)
        {
        j=smot-1;
        while ( (j>=0) & (mot[j]!=32) ) j--;

        if (j<=0) j=smot-1;

        for(i=0;i<j;i++)
           {
           ChrTxt[i+yp*xpage]=mot[i];
           ColTxt[i+yp*xpage]=motc[i];
           }

        for(i=j+1;i<smot;i++)   // +1 car on passe l'espace
            {
            mot[i-j-1]=mot[i];
            motc[i-j-1]=motc[i];
            }

        smot-=(j+1);

        yp++;
        }

    while (aff!=0)
        {
        switch (aff)
            {
            case 1:
                j=0;
                break;
            case 2:
                j=(xpage-smot)/2;
                break;
            }

        for(i=0;i<smot;i++)
           {
           ChrTxt[(i+j)+yp*xpage]=mot[i];
           ColTxt[(i+j)+yp*xpage]=motc[i];
           }

        smot=0;

        yp++;

        break;
        }
    aff=0;

    if (yp>3998) break;

    }

lentit=0;

}

code=0;

// yp= limitte end

suiv=prem;

do
{
if (ye>yp-yl+y) ye=yp-yl+y;
if (ye<0) ye=0;

aff=0;

for (i=y;i<yl;i++)
    for(j=x;j<=xl;j++)
        {
        AffChr(j,i,ChrTxt[(j-x)+(i-y+ye)*xpage]);
        AffCol(j,i,ColTxt[(j-x)+(i-y+ye)*xpage]);
        }

if ( (suiv->next!=NULL) & (suiv!=NULL) )
    {
    ix=suiv->x1;
    iy=suiv->y1-ye;

    if ((iy>-y) & (iy<yl-y))
        while(1)
            {
            AffCol(ix+y,iy+y,11*16+12);
            if ( (ix==suiv->x2) & (iy==(suiv->y2-ye)) ) break;
            ix++;
            if (ix==xpage) ix=0,iy++;
            }

    PrintAt(0,yl+1,"%-80s",suiv->link);
    }



code=Wait(0,0,0);

switch(LO(code))   {
    case 0:
        switch(HI(code))   {
            case 80:    // BAS
                ye++;
                break;
            case 72:    // HAUT
                ye--;
                break;
            case 0x51:  // PGDN
                ye+=20;
                break;
            case 0x49:  // PGUP
                ye-=20;
                break;
            case 0x47:  // HOME
                ye=0;
                break;
            case 0x4F:  // END
                ye=yp-(yl-y);
                break;
            case 0xF:   // SHIFT-TAB
                rete=suiv;
                suiv=prem;
                while ( (suiv->next!=rete) & (suiv->next!=NULL) )
                    suiv=suiv->next;

                if (suiv->next==NULL)
                    {
                    rete=suiv;
                    suiv=prem;
                    while ( (suiv->next!=rete) & (suiv->next!=NULL) )
                        suiv=suiv->next;
                    }


                if ((suiv->next)!=NULL)
                    {
                    iy=suiv->y1-ye;

                    if ((iy<=(-y)) | (iy>=yl-y))
                        ye=suiv->y1-y;
                    }

                break;

            default:
                break;
            }
        break;
    case 9:
        car=0;
        while (suiv->next!=NULL)
            {
            if (((suiv->y1-ye)<=(-y)) | ((suiv->y1-ye)>=yl-y))
                {
                car++;
                suiv=suiv->next;
                }
                else
                {
                if (car==0)  suiv=suiv->next;
                break;
                }
            }

        if (suiv->next==NULL)
            suiv=prem;

        if (suiv->next!=NULL)
            {
            iy=suiv->y1-ye;

            if ((iy<=(-y)) | (iy>=yl-y))
                ye=suiv->y1-y;
            }
        break;
    case 13:
        strcpy(titre,suiv->link+8);
        titre[strlen(titre)-1]=0;

        if (!strnicmp(titre,"MAILTO:",7))
            {
            code=0;
            strcpy(chaine,"You couldn't mail to: ");
            strcat(chaine,titre+7);
            WinError(chaine);
            break;
            }
        if (!strnicmp(titre,"HTTP:",5))
            {
            code=0;
            strcpy(chaine,"You couldn't HTTP to: ");
            strcat(chaine,titre+7);
            WinError(chaine);
            break;
            }
        if (!strnicmp(titre,"FTP:",4))
            {
            code=0;
            strcpy(chaine,"You couldn't FTP to: ");
            strcat(chaine,titre+6);
            WinError(chaine);
            break;
            }
                

        if (titre[0]=='#')
            {
            strcpy(liaison,titre+1);
            Info.numero=104;
            }
            else
            {

            Path2Abs(fichier,"..");
            Path2Abs(fichier,titre);

            strcpy(Info.path,fichier);

            FileinPath(fichier,titre);


            Traitefic(titre,&Info);
            }

        break;
    default:
        break;
    }
}
while ((code!=27) & (code!=0x8D00) & (code!=13) );


while(prem->next!=NULL)
    {
    free(prem->link);
    suiv=prem->next;
    free(prem);
    prem=suiv;
    }
free(prem);

ChargeEcran();

fclose(fic);

free(buffer);

free(ColTxt);
free(ChrTxt);

if (code==13)
    return Info.numero;
    else
    return -1;
}



void View(struct fenetre *F)
{
char *fichier,*liaison;
int i;

fichier=GetMem(256);
strcpy(fichier,F->path);
Path2Abs(fichier,F->F[F->pcur]->name);

liaison=GetMem(256);
strcpy(liaison,"");

i=InfoIDF(F);

while(i!=-1)
    {
    switch(i) {
        case 86: //Ansi
            i=AnsiView(fichier);
            break;
        case 91: //View text
            i=TextView(fichier);
            break;
        case 104: // HTML
            i=HtmlView(fichier,liaison);
            break;
        case 37: // GIF
        case 38: // JPG
            FicIdf(fichier,i);
            i=-1;
            break;
        default:
            i=HexaView(fichier);
            break;
        }
    }

free(liaison);
free(fichier);
}
