// Viewer

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


void ChangeMask(void);
void Masque(short x1,short y1,short x2,short y2);

char ReadChar(void);

int HexaView(char *fichier);
int TxtView(char *fichier);
int HtmlView(char *fichier,char *liaison);



static FILE *fic;
static long taille;
static char view_buffer[32768];
static long pos;        // position de depart
static long posl;       // taille du buffer
static long posn;       // octet courant

// Lit l'octet se trouvant en posn
//---------------------------------
char ReadChar(void)
{
// PrintAt(0,0,"Position: %9d",posn);

if (posn>taille)
    {
    posn=taille-1;
    return 0;
    }

if (posn<0)
    posn=0;

if (posn-pos>=posl)
    {
    while (posn-pos>=posl) pos+=32768;

    fseek(fic,pos,SEEK_SET);
    fread(view_buffer,32768,1,fic);
    }

if (posn-pos<0)
    {
    while (posn-pos<0) pos-=32768;

    fseek(fic,pos,SEEK_SET);
    fread(view_buffer,32768,1,fic);
    }

return view_buffer[posn-pos];
}

int HexaView(char *fichier)
{
int x,y;
long cur1,cur2;
long posd;
int car;

int fin=0; // Code de retour

SaveEcran();
PutCur(3,0);

Bar(" Help  ----  ----  Text  ----  ----  ----  ----  ----  ---- ");


ColWin(1,1,78,(Cfg->TailleY)-3,10*16+1);
ChrWin(1,1,78,(Cfg->TailleY)-3,32);

WinCadre(0,3,9,(Cfg->TailleY)-2,2);

WinCadre(10,3,58,(Cfg->TailleY)-2,2);

WinCadre(59,3,76,(Cfg->TailleY)-2,2);

WinCadre(77,3,79,(Cfg->TailleY)-2,2);

WinCadre(0,0,79,2,3);

ChrCol(34,4,(Cfg->TailleY)-6,Cfg->Tfont[0]);

PrintAt(3,1,"View File %s",fichier);

//

do
{
posd=posn;
     
for (y=0;y<Cfg->TailleY-6;y++)
    {
    PrintAt(1,y+4,"%08X",posn);

    for (x=0;x<16;x++)
        {
        if (posn<taille)
            {
            char a;
            a=ReadChar();
            PrintAt(x*3+11,y+4,"%02X",a);
            AffChr(x+60,y+4,a);
            }
            else
            {
            PrintAt(x*3+11,y+4,"  ");
            AffChr(x+60,y+4,32);
            }

        posn++;
        }
    }

if (taille<1024*1024)
    {
    cur1=(posd)*(Cfg->TailleY-7);
    cur1=cur1/taille+4;

    cur2=(posn)*(Cfg->TailleY-7);
    cur2=cur2/taille+4;
    }
    else
    {
    cur1=(posd/1024)*(Cfg->TailleY-7);
    cur1=cur1/(taille/1024)+4;

    cur2=(posn/1024)*(Cfg->TailleY-7);
    cur2=cur2/(taille/1024)+4;
    }


ChrCol(78,4,cur1-4,32);
ChrCol(78,cur1,cur2-cur1+1,219);
ChrCol(78,cur2+1,(Cfg->TailleY-3)-cur2,32);

posn=posd;

car=Wait(0,0,0);

switch(LO(car))   {
    case 0:
        switch(HI(car))   {
            case 80:    // BAS
                posn+=16;
                break;
            case 72:    // HAUT
                posn-=16;
                break;
            case 0x51:  // PGDN
                posn+=480;
                break;
            case 0x49:  // PGUP
                posn-=480;
                break;
            case 0x47:  // HOME
                posn=0;
                break;
            case 0x4F:  // END
                posn=taille-((((Cfg->TailleY)-6)*16))+15;
                break;
            case 0x43:  // F9
                fin=86;
                break;
            case 0x8D:  // CTRL-UP
                fin=-1;
                break;
            case 0x3B:  // F1
                Help();
                break;
            case 0x3E:  // F4
                fin=91;
                break;
            default:
                break;
            }
        break;
    case 32:
    case 13:
    case 27:
        fin=-1;
        break;
    default:
        break;
    }

while (posn>=taille-((((Cfg->TailleY)-7)*16))) posn-=16;
while (posn<0) posn+=16;

}
while(fin==0);

ChargeEcran();

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
    x=1;

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
        }
    while (p);
    }
}



int ansi_out (char *buf)
{
int  arglen = 0, ansistate = NOTHING, x;
char *b = buf, argbuf[MAXARGLEN] = "";

// Gestion des SHIFT, CTRL ...
char *Keyboard_Flag1=(char*)0x417;
char car;


while (*b)
    {
    tempo++;

    do
        {
        car=*Keyboard_Flag1;
        }
    while ((car&1)==1);

    if (touche!=1)
        {
        if (kbhit()) touche=Wait(0,0,0);
        if ( (touche==27) | (touche==0x8D00) ) touche=1; else touche=0;
        }
        else
        break;

    if ( (tempo>=Cfg->AnsiSpeed) & ((car&2)!=2) )
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
                case 27:
                    ansistate = WASESCAPE;
                    break;
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
                WinError("Error of ANSI code");
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


Wait(0,0,0);

SetTaille();

ChargeEcran();
free(buffer);
fclose(fic);

return (-1);
}



int TxtView(char *fichier)
{
long xm,ym;
char aff,wrap;

long posd;

int xl,yl;
int x,y;
int x2,y2;

char car;

char chaine[256];
short lchaine;

register int n;

char affichage[81];

int code;
int fin=0;

char pasfini;

SaveEcran();
PutCur(3,0);

Bar(" Help  ----  ----  Hexa  ----  ---- Search Print Mask  ---- ");

wrap=0;
aff=1;

//-------------------- Calcul de la taille maximum ------------------------//
xm=0;
ym=0;

x=0;

if (taille<32768)
    {
    for (n=0;n<taille;n++)
        {
        switch(view_buffer[n])
            {
            case 10:
                x=0;
                ym++;
                break;
            case 13:
                break;
            case 9:
                lchaine=x/8;
                lchaine=lchaine*8+8;
                lchaine-=x;
                x+=lchaine;
                break;
            default:
                x++;
                if (x>xm) xm=x;
                break;
            }
        }
    }
    else
    {
    xm=80;
    ym=50;
    }
ym++;

if (xm>=79)
    xl=80;
    else
    xl=xm;            //--> Longueur

if (ym>Cfg->TailleY-1)
    yl=Cfg->TailleY-1;
    else
    yl=ym;

x=(80-xl)/2;           // centre le texte
y=(Cfg->TailleY-yl)/2; //

if (x<0) x=0;
if (y<0) y=0;

if ( (x>0) & (y>0) & (x+xl<80) & (y+yl<Cfg->TailleY) )
    WinCadre(x-1,y-1,x+xl,y+yl,3);
    else
    {
    if ( (y+yl<Cfg->TailleY) & (y>0) )
        {
        ColLin(0,y-1,80,10*16+1);
        WinLine(0,y-1,80,1);
        WinLine(0,y+yl,80,1);
        ColLin(0,y+yl,80,10*16+1);
        }
    if ( (x+xl<80) & (x>0) )
        {
        ColWin(x-1,0,x-1,Cfg->TailleY-2,10*16+1);
        ChrWin(x-1,0,x-1,Cfg->TailleY-2,0xB3);
        ChrWin(x+xl,0,x+xl,Cfg->TailleY-2,0xB3);
        ColWin(x+xl,0,x+xl,Cfg->TailleY-2,10*16+1);
        }
    }


ChrWin(x,y,x+xl-1,y+yl-1,32);
ColWin(x,y,x+xl-1,y+yl-1,10*16+1);

//-------------------------------------------------------------------------//

affichage[xl]=0;



do
{

pasfini=0;

posd=posn;

x2=x;
y2=y;

do
    {
    lchaine=1;

    chaine[0]=ReadChar();


    switch(chaine[0])  {
        case 0:
            chaine[0]=32;
            break;
        case 10:
            lchaine=xl-x2+x+1;
            memset(chaine,32,lchaine);
            aff=2;
            break;
        case 9:
            lchaine=(x2-x)/8;
            lchaine=lchaine*8+8;
            lchaine-=(x2-x);
            memset(chaine,32,lchaine);
            break;
        case 13:
            chaine[0]=0;
            break;

        default:
            chaine[0]=CnvASCII(chaine[0]);
            break;
        }

    for(n=0;n<lchaine;n++)
        {
        car=chaine[n];

        if (x2>=x+xl)
            {
            if (aff==2)
                {
                PrintAt(x,y2,"%s",affichage);
                affichage[0]=0;
                x2=x;
                y2++;
                if (y2>=y+yl) break;
                lchaine=0;
                car=0;
                aff=1;
                }
                else
                aff=0;
            }

        if ( (car!=0) & (aff!=0) )
            {
            affichage[x2-x]=car;
            x2++;
            }
        }

    if (aff==2) aff=1;

    if (y2>=y+yl)
        break;
    posn++;
    if (posn>=taille)
        {
        pasfini=1;
        lchaine=xl-x2+x;
        memset(affichage+x2-x,32,lchaine);
        PrintAt(x,y2,"%s",affichage);
        y2++;
        break;
        }
    }
while(1);

while(y2<y+yl)
    {
    memset(affichage,32,xl);
    PrintAt(x,y2,"%s",affichage);
    affichage[0]=0;
    y2++;
    }


Masque(x,y,x+xl-1,y+yl-1);

code=Wait(0,0,0);

posn=posd;

switch(LO(code))
    {
    case 0:
       switch(HI(code))   {
            case 0x3B:  // F1
                Help();
                break;
            case 0x3E:  // F4
                fin=-2;
                break;
            case 0x41:  // F7
                SearchTxt();
                break;
            case 0x42:  // F8
                Print(fichier,1);
                break;
            case 0x43:  // F9
                ChangeMask();
                ColWin(x,y,x+xl-1,y+yl-1,10*16+1);
                break;
            case 80:    // BAS
                if (pasfini==1) break;
                do
                    {
                    posn++;
                    if (posn==taille)
                        {
                        posn=taille-2;
                        break;
                        }
                    }
                while(ReadChar()!=0x0A);
                posn++;

                break;
            case 72:    // HAUT
                if (posn==0) break;
                posn--;
                if (posn==0) break;
                do
                    {
                    posn--;
                    if (posn==0)
                        break;
                    }
                while(ReadChar()!=0x0A);
                if (posn!=0) posn++;

                break;
            case 0x51:    // PGDN
                for (n=0;n<yl;n++)
                {
                if (pasfini==1) break;
                do
                    {
                    posn++;
                    if (posn==taille)
                        {
                        posn=taille-2;
                        break;
                        }
                    }
                while(ReadChar()!=0x0A);
                posn++;
                }
                break;
            case 0x49:    // PGUP
                for (n=0;n<yl;n++)
                {
                if (posn==0) break;
                posn--;
                if (posn==0) break;
                do
                    {
                    posn--;
                    if (posn==0)
                        break;
                    }
                while(ReadChar()!=0x0A);
                if (posn!=0) posn++;
                }
                break;
            case 0x4F:    // END
                posn=taille;
                for (n=0;n<yl;n++)
                {
                if (posn==0) break;
                posn--;
                if (posn==0) break;
                do
                    {
                    posn--;
                    if (posn==0)
                        break;
                    }
                while(ReadChar()!=0x0A);
                if (posn!=0) posn++;
                }
                break;
            case 0x47: // HOME
                posn=0;
                break;
            case 0x8D: // CTRL-UP
                fin=-1;
                break;
            }
        break;
    case 32:
    case 13:
    case 27:
        fin=-1;
        break;
    }

}
while(!fin);


ChargeEcran();

return fin;
}

// Recherche une chaine
void SearchTxt(void)
{
static char Dir[70];
static int DirLength=70;
static int CadreLength=71;

struct Tmt T[5] = {
      { 2,3,1,
        Dir,
        &DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Change to which directory",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,10,76,17,
    "Search Text"};

int n;
int a;

char c1,c2;

a=posn;

do
    {
    posn++;
    if (posn==taille)
        {
        posn=taille-2;
        break;
        }
    }
    while(ReadChar()!=0x0A);

posn++;

n=WinTraite(T,5,&F);

if ( (n!=0) & (n!=1) ) return;

n=0;



do
{
if (Dir[n]==0) break;

c1=Dir[n];
if ( (c1>='a') & (c1<='z') ) c1+='A'-'a';

c2=ReadChar();;
if ( (c2>='a') & (c2<='z') ) c2+='A'-'a';

if (c1==c2)
    n++;
    else
    {
    if (n!=0)
        {
        posn-=(n-1);
        n=0;
        }
    }

posn++;
}
while (posn<taille);

if (Dir[n]!=0)
    {
    posn=a;
    WinError("Don't find text");
    return;
    }

if (posn==0) return;
posn--;
if (posn==0) return;
do
    {
    posn--;
    if (posn==0) return;
    }
while(ReadChar()!=0x0A);
if (posn!=0) posn++;
}


struct Href {
    char *link;
    signed int x1,y1;
    signed int x2,y2;
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

char psuiv;     // vaut 1 si on passe le suivant

int i,j,k;        // Compteur

signed char bold,ital,unde;
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

psuiv=0;

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
        memset(titre,0,128);
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

    case ';':
        if (psuiv==1)
            {
            psuiv=0;
            car=0;
            }

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
            if (!strnicmp(titre,"H1",2))  nbrcol++,tabcol[nbrcol]=3*16+13;
            if (!strnicmp(titre,"H2",2))  nbrcol++,tabcol[nbrcol]=4*16+13;
            if (!strnicmp(titre,"H3",2))  nbrcol++,tabcol[nbrcol]=5*16+13;
            if (!strnicmp(titre,"H4",2))  nbrcol++,tabcol[nbrcol]=3*16+1;
            if (!strnicmp(titre,"H5",2))  nbrcol++,tabcol[nbrcol]=4*16+1;
            if (!strnicmp(titre,"H6",2))  nbrcol++,tabcol[nbrcol]=5*16+1;

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

                    suiv->x2=smot;          // REFERENCE DE 1 DE LARGE
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
            if (!stricmp(titre,"/P")) aff=1;         // fin de paragraphe
            if (!stricmp(titre,"OL"))  listt[nlist]=2,listn[nlist]=1,nlist++;
            if (!stricmp(titre,"/OL")) listt[nlist]=0,listn[nlist]=0,nlist--,aff=1;
            if (!stricmp(titre,"UL"))  listt[nlist]=1,listn[nlist]=1,nlist++;
            if (!stricmp(titre,"/UL")) listt[nlist]=0,listn[nlist]=0,nlist--,aff=1;

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
    {
    if (psuiv!=0)
        {
        psuiv--;
        car=0;
        }

    switch(car)  {
        case 0:
            break;
        case 32:
            titre[code-1]=0;
            sprintf(chaine,"&%s ",titre);
            memcpy(titre,chaine,255);
            lentit=strlen(titre);
            code=0;
            break;
        case ';':
            code=0;
            car=0;
            break;
        default:
            titre[code-1]=car;

            car=0;
            lentit=0;
            code++;

            if (code>128) code=0;

            if (!strnicmp(titre,"EGRAVE",6)) psuiv=1,car='Š';
            if (!strnicmp(titre,"EACUTE",6)) psuiv=1,car='‚';

            if (!strnicmp(titre,"AGRAVE",6)) psuiv=1,car='…';
            if (!strnicmp(titre,"AACUTE",6)) psuiv=1,car=' ';

            if (!strnicmp(titre,"IGRAVE",6)) psuiv=1,car='i';
            if (!strnicmp(titre,"IACUTE",6)) psuiv=1,car='i';

            if (!strnicmp(titre,"CCEDIL",6)) psuiv=1,car='‡';

            if (!strnicmp(titre,"ECIRC",5)) psuiv=1,car='ˆ';
            if (!strnicmp(titre,"ACIRC",5)) psuiv=1,car='ƒ';
            if (!strnicmp(titre,"ICIRC",5)) psuiv=1,car='i';

            if (!strnicmp(titre,"QUOT",4)) psuiv=1,car=34;

            if (!strnicmp(titre,"NBSP",4)) psuiv=1,car=32;
            if (!strnicmp(titre,"IUML",4)) psuiv=1,car='i';        // En attendant mieux
            if (!strnicmp(titre,"COPY",4))
                 {
                 psuiv=1;
                 lentit=3;
                 strcpy(titre,"(C)");
                 }

            if (!strnicmp(titre,"AMP",3)) psuiv=1,car='&';

            if (!strnicmp(titre,"LT",2)) psuiv=1,car='<';
            if (!strnicmp(titre,"GT",2)) psuiv=1,car='>';

            if (psuiv==1)
                code=0;
            break;
        }
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
        default:
            car=CnvASCII(car);
            break;
        }

    if (nbrcol<0)
        {
        nbrcol=0;
        }

    col=tabcol[nbrcol];

    if ( (bold<0) | (ital<0) | (unde<0) ) bold=0,ital=0,unde=0;

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

        if (j==xpage) j--;      // Car on passe espace quand il y a espace

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

if (yp>3998) break;

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
            if (ix<xpage)
                AffCol(ix+x,iy+y,11*16+12);

            if (iy>(suiv->y2-ye)) break;

            ix++;
            if (ix>=xpage) ix=0,iy++;

            if ( (ix==suiv->x2) & (iy==(suiv->y2-ye)) ) break;

            
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
    for(j=0;j<6;j++,n++) {
       AffCol(n,TY-1,1*16+2);
       AffChr(n,TY-1,*(bar+i*6+j));
       }
    }
}




void View(struct fenetre *F)
{
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

    fread(view_buffer,32768,1,fic);

    pos=0;
    posl=32768;

    posn=0;
    }

while(i!=-1)
    {
    switch(i) {
        case 86: //Ansi
            i=AnsiView(fichier);
            break;
        case 91: //Texte
            i=TxtView(fichier);
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

if (fic!=NULL)
    fclose(fic);

free(liaison);
free(fichier);

ChargeEcran();
}

void Masque(short x1,short y1,short x2,short y2)
{
char *chaine;
char chain2[80];
short l,m1,m2;

unsigned char c,c2;
short x,y;
short xt[80],yt[80];

char trouve;

struct PourMask *CMask;

if (((Cfg->wmask)&128)==128) return;

CMask=Mask[(Cfg->wmask)&15];

chaine=CMask->chaine;

x=x1;
y=y1;

l=0;

while(y<=y2)
    {
    AffCol(x,y,10*16+9);
    c=GetChr(x,y);

    if ( ((c>='a') & (c<='z')) | ((c>='A') & (c<='Z')) | ((c>='0') & (c<='9')) | (c=='_') )
        {
        chain2[l]=c;
        xt[l]=x;
        yt[l]=y;
        l++;
        if (l==80) l=0;
        }
        else
        {
        if (l!=0)
            {
            trouve=0;
            m1=0;
            m2=0;

            while(chaine[m2]!='@')
                {
                if (chaine[m2]==32)
                    {
                    if (m2-m1==l)
                        {
                        if (CMask->Ignore_Case==0)
                            {
                            if (!strncmp(chaine+m1,chain2,l))
                                trouve=1;
                            }
                            else
                            {
                            if (!strnicmp(chaine+m1,chain2,l))
                                trouve=1;
                            }
                        }
                    m1=m2+1;
                    }
                m2++;
                }

            if (trouve==1)
                c2=10*16+5;
                else
                c2=10*16+4;

            while (l!=0)
                {
                l--;
                AffCol(xt[l],yt[l],c2);
                }
            l=0;
            }
        }

    if (((Cfg->wmask)&64)==64)
        {
        c=toupper(c);
        switch(c)
            {
            case 'A':
            case 'E':
            case 'I':
            case 'O':
            case 'U':
            case 'Y':
                c+='a'-'A';
                break;
            }
        AffChr(x,y,c);
        }

    x++;
    if (x>x2) x=x1,y++;

    }
}



void ChangeMask(void)
{
int i,n;
char s[16];
int x1,y1,x2,y2,max;
int pos,prem;
int dernier;
int c;
char car,car2;

SaveEcran();

n=0;
max=0;
pos=0;

for(i=0;i<16;i++)
    if (strlen(Mask[i]->title)>0)
        {
        if ( ((Cfg->wmask)&15) ==i) pos=n;
        s[n]=i;
        n++;
        if (strlen(Mask[i]->title)>max) max=strlen(Mask[i]->title);
        }

x1=(82-max)/2;
y1=((Cfg->TailleY)-2*n)/2;

x2=x1+max+3;
y2=y1+(n+1)*2;

if (y1<0) y1=2;
if (y2>Cfg->TailleY) y2=Cfg->TailleY-3;

WinCadre(x1,y1,x2,y2,0);

ColWin(x1+1,y1+1,x2-1,y2-1,10*16+1);
ChrWin(x1+1,y1+1,x2-1,y2-1,32);

PrintAt(x1+1,y1+1,"Select Mask:");
ColWin(x1+1,y1+1,x2-1,y1+1,10*16+5);

prem=0;

ChrLin(0,(Cfg->TailleY)-1,80,32);
ColLin(0,(Cfg->TailleY)-1,80,1*16+8);


do
    {
    PrintAt( 1,(Cfg->TailleY)-1,"F8: Look %3s",(Cfg->wmask&64)==64 ? "ON" : "OFF");
    PrintAt(17,(Cfg->TailleY)-1,"F9: Mask %3s",(Cfg->wmask&128)==128 ? "OFF" : "ON");
    PrintAt(40,(Cfg->TailleY)-1,"%-40s",Mask[s[pos]]->title);

    for (i=prem;i<n;i++)
        {
        if (pos==i)
            ColLin(x1+2,y1+3+(i-prem)*2,max,1*16+5);
            else
            ColLin(x1+2,y1+3+(i-prem)*2,max,10*16+1);

        PrintAt(x1+2,y1+3+(i-prem)*2,"%-*s",max,Mask[s[i]]->title);

        if (y1+3+(i-prem+1)*2>=y2)
            {
            dernier=i;
            break;
            }
        }

    c=Wait(0,0,0);
    car=LO(c);
    car2=HI(c);

    switch(car2)    {
        case 0x47:
            pos=0;
            break;
        case 0x4F:
            pos=dernier;
            break;
        case 72:
            pos--;
            if (pos==-1) pos=0;
            if (pos<prem) prem--;
            break;
        case 80:
            pos++;
            if (pos==n) pos--;
            if (pos>dernier) prem++;
            break;
        case 0x42: // F8
            Cfg->wmask^=64;
            break;
        case 0x43: // F9
            Cfg->wmask^=128;
            break;
        }
    switch(car) {
        case 13:
            Cfg->wmask=((Cfg->wmask)&240)|s[pos];
            break;
        }

    }
while ( (car!=27) & (car!=13) );


ChargeEcran();
}

char PRN_init(short lpt,char a)
{
union REGS regs;

regs.h.ah=1;
regs.w.dx=lpt;

int386(0x17,&regs,&regs);

return regs.h.ah;
}

// Retourne 1 si tout va bien

char PRN_print(short lpt,char a)
{
union REGS regs;
char code,cont;

PrintAt(0,0,"Printing...");

do
    {
    regs.h.ah=0;
    regs.h.al=a;
    regs.w.dx=lpt;

    int386(0x17,&regs,&regs);

    code=regs.h.ah;

    cont=0;

    if ( ((code&128)==128) & ((code&16)==16) ) cont=1;
    if ((code&1)==1)   WinError("Time-Out");
    if ((code&8)==8)   WinError("I/O Error");
    if ((code&32)==32) WinError("No more paper");
    if ((code&64)==64) PrintAt(0,0,"ACK Error"),cont=0;
    }
while(cont);

return 1;
}



void Print(char *fichier,int n)
{
FILE *fic;
short lpt;
char a;
char Font[]={27,91,3,27,51,28};
int m;

lpt=0;


fic=fopen(fichier,"rb");
if (fic==NULL)
    { WinError("Couldn't open file"); return; }

for (m=0;m<6;m++)
    PRN_print(lpt,Font[m]);
    

if (n==1)   // Fichier TEXTE
    {
    do
        {
        if (fread(&a,1,1,fic)==0) break;
        if (PRN_print(lpt,a)==0) break;
        }
    while(1);
    }

WinMesg("Print","The file is printed");
}

