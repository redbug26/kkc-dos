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

int HexView(char *fichier);

int HexView(char *fichier)
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
        touche=_bios_keybrd(0x1);
        touche=touche/256;
        }
        else
        break;

    if (tempo>=66)
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


void View(struct fenetre *F)
{
char *fichier;
int i;

fichier=GetMem(256);
strcpy(fichier,F->path);
Path2Abs(fichier,F->F[F->pcur]->name);

i=InfoIDF(F);

while(i!=-1)
    {
    switch(i) {
        case 86: //Ansi
            i=AnsiView(fichier);
            break;
        default:
            i=HexView(fichier);
            break;
        }
    }

free(fichier);
}
