// Hard-function
#include <stdarg.h>
#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include <malloc.h>

#include <i86.h>
#include <time.h>

#include <dos.h>

#include <ctype.h>
#include <direct.h>
#include <bios.h>

#include <time.h>

#include "hard.h"

int IOver;
int IOerr;


struct RB_info *Info;
struct config *Cfg;
struct PourMask **Mask;
struct fichier *Fics;


char _RB_screen[256*128*2];

void (*AffChr)(char x,char y,char c);
void (*AffCol)(char x,char y,char c);
int (*Wait)(int x,int y,char c);
int (*KbHit)(void);
void (*GotoXY)(char x,char y);
void (*WhereXY)(char *x,char *y);
void (*Clr)(void);
void (*Window)(int left,int top,int right,int bottom,short color);


/*--------------------------------------------------------------------*\
 ------------------------- Fonction interne ---------------------------
\*--------------------------------------------------------------------*/
void Norm_Clr(void);

void Norm_Clr(void)
{
char i,j;

for(j=0;j<Cfg->TailleY;j++)
    for (i=0;i<80;i++)
      AffCol(i,j,7);

for(j=0;j<Cfg->TailleY;j++)
    for (i=0;i<80;i++)
      AffChr(i,j,32);
}

void Norm_Window(int left,int top,int right,int bottom,short color)
{
int i,j;

for(j=top;j<=bottom;j++)
    for (i=left;i<=right;i++)
        AffCol(i,j,color);


for(j=top;j<=bottom;j++)
    for (i=left;i<=right;i++)
        AffChr(i,j,32);
}



/*--------------------------------------------------------------------*\
 -           Affiche des caractŠres directement … l'‚cran             -
\*--------------------------------------------------------------------*/
void Cache_AffChr(char x,char y,char c);
void Cache_AffCol(char x,char y,char c);

char *scrseg=(char*)0xB8000;

void Cache_AffChr(char x,char y,char c)
{
if (*(_RB_screen+(y*256+x))!=c)
    {
    *(scrseg+((y*80+x)<<1))=c;
    *(_RB_screen+(y*256+x))=c;
    }
}

void Cache_AffCol(char x,char y,char c)
{
if (*(_RB_screen+(y*256+x)+256*128)!=c)
    {
    *(scrseg+((y*80+x)<<1)+1)=c;
    *(_RB_screen+(y*256+x)+256*128)=c;
    }
}

int Cache_Wait(int x,int y,char c)
{
char a;
int b;
clock_t Cl;

if ((x!=0) | (y!=0))
    GotoXY(x,y);

Cl=clock();

a=0;
b=0;

while ( (!kbhit()) & (b==0) )
    {
    if ( ((clock()-Cl)>Cfg->SaveSpeed) & (Cfg->SaveSpeed!=0) )
        b=ScreenSaver();
    }

if (b==0)
    {
    a=getch();
    if (a==0)
        return getch()*256+a;
    return a;
    }
return b;
}

void Cache_GotoXY(char x,char y)
{
union REGS regs;

regs.h.dl=x;
regs.h.dh=y;
regs.h.bh=0;
regs.h.ah=2;

int386(0x10,&regs,&regs);
}

void Cache_WhereXY(char *x,char *y)
{
union REGS regs;

regs.h.bh=0;
regs.h.ah=3;

int386(0x10,&regs,&regs);

*x=regs.h.dl;
*y=regs.h.dh;
}






/*--------------------------------------------------------------------*
 -                      Affichage par code Ansi                       -
 *--------------------------------------------------------------------*/
void Ansi_AffChr(char x,char y,char c);
void Ansi_AffCol(char x,char y,char c);
void Ansi_GenCol(char x,char y);
void Ansi_GenChr(char x,char y,char c);
void Ansi_Clr(void);
void Ansi_Window(int left,int top,int right,int bottom,short color);

int _Ansi_col1,_Ansi_col2;               // Couleur que l'on doit mettre
int _Ansi_tcol;                 // Couleur que l'on a demand‚ par affcol
int _Ansi_x=0,_Ansi_y=0;                          // precedente position

char _Ansi_cnv[]={0,5,0,2,3,7,6,4,3,1,6,1,2,0,0,0};


void Ansi_AffCol(char x,char y,char c)
{
if (*(_RB_screen+(y*256+x)+256*128)!=c)
    {
    *(_RB_screen+(y*256+x)+256*128)=c;

    Ansi_GenCol(x,y);
    Ansi_GenChr(x,y,GetChr(x,y));
    }

}

void Ansi_AffChr(char x,char y,char c)
{
Ansi_GenCol(x,y);

if (*(_RB_screen+(y*256+x))!=c)
        Ansi_GenChr(x,y,c);
}

void Ansi_GenCol(char x,char y)
{
if (*(_RB_screen+(y*256+x)+256*128)!=_Ansi_tcol)
    {
    _Ansi_tcol=*(_RB_screen+(y*256+x)+256*128);

    _Ansi_col1=_Ansi_cnv[_Ansi_tcol/16]+40;
    _Ansi_col2=_Ansi_cnv[_Ansi_tcol&15]+30;

    cprintf("\x1b[%d;%dm",_Ansi_col1,_Ansi_col2);
    }
}

void Ansi_GenChr(char x,char y,char c)
{
if (y<Cfg->TailleY-2)
    {
    *(_RB_screen+(y*256+x))=c;

    switch(c)
        {
        case 0:
        case 8:
        case 10:
        case 13:    c=32;   break;
        case 16:    c='>';  break;
        case 17:    c='<';  break;
        case 127:   c='^';  break;
        case 7:     c='.';  break;
        }

    _Ansi_x++;
//    if (_Ansi_x==80) _Ansi_x=0, _Ansi_y=y+1;

    if ( (x!=_Ansi_x) | (y!=_Ansi_y) )
        cprintf("\x1b[%d;%dH",y+1,x+1), _Ansi_x=x,  _Ansi_y=y;

    cprintf("%c",c);
    }
}

void Ansi_GotoXY(char x,char y)
{
cprintf("\x1b[%d;%dH",y+1,x+1);
_Ansi_x=x;
_Ansi_y=y;
}

void Ansi_Clr(void)
{
memset(_RB_screen+256*128,7,256*128);
memset(_RB_screen,32,256*128);

cprintf("\x1b[0m\n\n\x1b[2J");
}

void Ansi_Window(int left,int top,int right,int bottom,short color)
{
int i,j;

for(j=top;j<=bottom;j++)
    for (i=left;i<=right;i++)
        {
        *(_RB_screen+(j*256+i)+256*128)=color;
        *(_RB_screen+(j*256+i))=0;             // Pour le remettre apres
        }

for(j=top;j<=bottom;j++)
    for (i=left;i<=right;i++)
        AffChr(i,j,32);
}



/*--------------------------------------------------------------------*
 -                      Affichage par COM  Ansi                       -
 *--------------------------------------------------------------------*/
void Com_AffChr(char x,char y,char c);
void Com_AffCol(char x,char y,char c);
void Com_GenCol(char x,char y);
void Com_GenChr(char x,char y,char c);
int Com_KbHit(void);
void Com_Clr(void);
void Com_Window(int left,int top,int right,int bottom,short color);

int _Com_col1,_Com_col2;                 // Couleur que l'on doit mettre
int _Com_tcol;                  // Couleur que l'on a demand‚ par affcol
int _Com_x=0,_Com_y=0;                            // precedente position

char _Com_cnv[]={0,5,0,2,3,7,6,4,3,1,6,1,2,0,0,0};

long modem_buffer_count;

void Com_AffCol(char x,char y,char c)
{
if (y>=Cfg->TailleY-1) return;

if (*(_RB_screen+(y*256+x)+256*128)!=c)
    {
    *(_RB_screen+(y*256+x)+256*128)=c;

    *(scrseg+((y*80+x)<<1)+1)=c;            // --------- Echo console --

    Com_GenCol(x,y);
    Com_GenChr(x,y,GetChr(x,y));
    }

}

void Com_AffChr(char x,char y,char c)
{
if (y>=Cfg->TailleY-1) return;

if (*(_RB_screen+(y*256+x))!=c)
    {
    Com_GenCol(x,y);

    *(scrseg+((y*80+x)<<1))=c;              // --------- Echo console --

    Com_GenChr(x,y,c);
    }
}

void Com_GenCol(char x,char y)
{
char buffer[32];
int n;

if (*(_RB_screen+(y*256+x)+256*128)!=_Com_tcol)
    {
    _Com_tcol=*(_RB_screen+(y*256+x)+256*128);

    _Com_col1=_Com_cnv[_Com_tcol/16]+40;
    _Com_col2=_Com_cnv[_Com_tcol&15]+30;

    sprintf(buffer,"\x1b[%d;%dm",_Com_col1,_Com_col2);
    for (n=0;n<strlen(buffer);n++)
        com_send_ch(buffer[n]);
    }
}

void Com_GenChr(char x,char y,char c)
{
*(_RB_screen+(y*256+x))=c;

switch(c)
    {
    case 0:
    case 8:
    case 10:
    case 13:    c=32;   break;
    case 16:    c='>';  break;
    case 17:    c='<';  break;
    case 127:   c='^';  break;
    case 7:     c='.';  break;
    }

_Com_x++;

if ( (x!=_Com_x) | (y!=_Com_y) )
    Com_GotoXY(x,y);

com_send_ch(c);
}

int Com_Wait(int x,int y,char c)
{
char buf[32];
char n;
char cont;

cont=1;
n=0;

while(cont==1)
    {
    while (1)
        {
        if (com_ch_ready())
            {
            buf[n]=com_read_ch();
            break;
            }
        if (kbhit())
            {
            buf[n]=getch();
            break;
            }
        }

    if ( (buf[0]!=27)  & (n==0) ) cont=0;
    if ( (buf[1]!='[') & (n==1) ) cont=0;

    if (buf[n]==0) cont=1;
    n++;
    if (n==3) break;
    }

// cprintf("**");
// for (m=0;m<n;m++)
//    cprintf("%02X",buf[m]);


buf[n]=0;

if (buf[0]==0) return buf[1]*256;

if (n==3)
    {
    switch (buf[2])
        {
        case 'A': return 72*256;                                 // HAUT
        case 'B': return 80*256;                                  // BAS
        case 'C': return 77*256;                               // DROITE
        case 'D': return 75*256;                               // GAUCHE
        case 'H': return 0x47*256;                               // HOME
        case 'K': return 0x4F*256;                                // END
        }
    }

return buf[0];
}

void Com_GotoXY(char x,char y)
{
char buffer[32];
int n;

sprintf(buffer,"\x1b[%d;%dH",y+1,x+1);
for (n=0;n<strlen(buffer);n++)
    com_send_ch(buffer[n]);

_Com_x=x;
_Com_y=y;
}

int Com_KbHit(void)
{
if (kbhit()) return 1;
if (com_ch_ready()) return 1;

return 0;
}

void Com_Clr(void)
{
char buffer[32];
int n;

memset(_RB_screen+256*128,7,256*128);
memset(_RB_screen,32,256*128);

sprintf(buffer,"\x1b[0m\n\n\x1b[2J");
for (n=0;n<strlen(buffer);n++)
    com_send_ch(buffer[n]);
}

void Com_Window(int left,int top,int right,int bottom,short color)
{
int i,j;

for(j=top;j<=bottom;j++)
    for (i=left;i<=right;i++)
        {
        *(_RB_screen+(j*256+i)+256*128)=color;
        *(_RB_screen+(j*256+i))=0;             // Pour le remettre apres

        *(scrseg+((j*80+i)<<1)+1)=color;    // --------- Echo console --
        }

for(j=top;j<=bottom;j++)
    for (i=left;i<=right;i++)
        AffChr(i,j,32);
}


/*--------------------------------------------------------------------*
 -                      Gestion du port s‚rie                         -
 *--------------------------------------------------------------------*/
#define XON 1
#define XOFF 0
#define MAX_BUFFER 1024

#define INT_OFF() _disable()
#define INT_ON() _enable()

#define SETVECT _dos_setvect
#define GETVECT _dos_getvect

char modem_buffer[MAX_BUFFER];


short modem_pause;
short modem_base;
short modem_port;
short modem_buffer_head;
short modem_buffer_tail;
short modem_overflow;
short modem_irq;
short modem_open=0;
short modem_xon_xoff=0;
short modem_rts_cts;

char old_modem_imr;
char old_modem_ier;

void (_interrupt *old_modem_isr)(void);

/*--------------------------------------------------------------------*/
void interrupt modem_isr(void)
{
unsigned char c;

INT_ON();

if (modem_buffer_count<1024)
    {
    c=inp(modem_base);
    if ( ((c==XON) | (c==XOFF)) & (modem_xon_xoff) )
        {
        switch(c)
            {
            case XON :modem_pause=0; break;
            case XOFF:modem_pause=1; break;
            }
        }
    else
        {
        modem_pause=0;
        modem_buffer[modem_buffer_head++]=c;
        if (modem_buffer_head>=MAX_BUFFER)
            modem_buffer_head=0;
        modem_buffer_count++;
        }
    modem_overflow=0;
    }
else
    {
    modem_overflow=1;
    }

INT_OFF();
outp(0x20,0x20);
}

short com_carrier(void)
{
short x;

if (!modem_open) return(0);
if ((inp(modem_base+6) & 0x80)==128) return(1);

for (x=0; x<500; x++)
    {
    if ((inp(modem_base+6) & 0x80)==128) return(1);
    }
return(0);
}

short com_ch_ready(void)
{
if (!modem_open) return(0);
if (modem_buffer_count!=0) return(1);
return(0);
}

/*--------------------------------------------------------------------*
 - This will return 0 is there is no character waiting.  Please check -
 - the port with com_ch_ready(); first so that if they DID send a 0x00-
 -     that you will know it's a true 0, not a no character return!   -
 *--------------------------------------------------------------------*/
unsigned char com_read_ch(void)
{
unsigned char ch;

if (!modem_open) return(0);

if (!com_ch_ready()) return(0);

ch=modem_buffer[modem_buffer_tail];
modem_buffer[modem_buffer_tail]=0;
modem_buffer_count--;
if (++modem_buffer_tail>=MAX_BUFFER)
    modem_buffer_tail=0;

return(ch);
}

void com_send_ch(unsigned char ch)
{
if (!modem_open) return;

outp(modem_base+4,0x0B);

if (modem_rts_cts)
    {
    while((inp(modem_base+6) & 0x10)!=0x10) ;  // Wait for Clear to Send
    }
while((inp(modem_base+5) & 0x20)!=0x20) ;

if (modem_xon_xoff)
    {
    while((modem_pause) && (com_carrier())) ;
    }
outp(modem_base,ch);
}


short com_open(short comport,long speed,short bit,BYTE parity,BYTE stop)
{
short x,  newb=0;
char l, m;
short d;

INT_OFF();

if (modem_open)
    com_close();

modem_port=comport;

switch(modem_port)
    {
    case 2: modem_base=0x2F8; modem_irq=3; break;
    case 3: modem_base=0x3E8; modem_irq=4; break;
    case 4: modem_base=0x2E8; modem_irq=3; break;
    case 1:
    default:modem_base=0x3F8; modem_irq=4; break;
    }

outp(modem_base+1,0x00);                     // turn off comm interrupts

if (inp(modem_base+1)!=0)
    {
    INT_ON();
    return(0);
    }

/* Set up the Interupt Info */
old_modem_ier=inp(modem_base+1);
outp(modem_base+1,0x01);

old_modem_isr=(void (_interrupt *)(void))GETVECT(modem_irq+8);
SETVECT(modem_irq+8,modem_isr);

if (modem_rts_cts)
    {
    outp(modem_base+4,0x0B);
    }
else
    {
    outp(modem_base+4,0x09);
    }

old_modem_imr=inp(0x21);
outp(0x21,old_modem_imr & ((1 << modem_irq) ^ 0x00FF));

for (x=1; x<=5; x++)
    inp(modem_base+x);

modem_open=1;

modem_buffer_count=0;
modem_buffer_head=0;
modem_buffer_tail=0;

//--- Speed ------------------------------------------------------------

x=inp(modem_base+3);                                // Read In Old Stats

if ((x & 0x80)!=0x80) outp(modem_base+3,x+0x80);          // Set DLab On

d=(short)(115200/speed);
l=d & 0xFF;
m=(d >> 8) & 0xFF;

outp(modem_base+0,l);
outp(modem_base+1,m);

outp(modem_base+3,x);                            // Restore the DLAB bit

//--- Data-bit ---------------------------------------------------------
newb=0;

x=inp(modem_base+3);

newb=(x>>2<<2);                          // Get rid of the old Data Bits

switch(bit)
    {
    case 5 : newb+=0x00; break;
    case 6 : newb+=0x01; break;
    case 7 : newb+=0x02; break;
    default: newb+=0x03; break;
    }

outp(modem_base+3,newb);

//--- Parity -----------------------------------------------------------
newb=0;

x=inp(modem_base+3);

newb=(x>>6<<6)+(x<<5>>5);                       // Get rid of old parity

switch(toupper(parity))
    {
    case 'N':newb+=0x00; break;                                 //  None
    case 'O':newb+=0x08;break;                                  //   Odd
    case 'E':newb+=0x18; break;                                 //  Even
    case 'M':newb+=0x28;break;                                  //  Mark
    case 'S':newb+=0x38;break;                                  // Space
    }

outp(modem_base+3,newb);

//--- Stop bits --------------------------------------------------------
newb=0;

x=inp(modem_base+3);

newb=(x<<6>>6)+(x>>5<<5);                      // Kill the old Stop Bits

if (stop==2) newb+=0x04;         // Only check for 2, assume 1 otherwise

outp(modem_base+3,newb);

//--- fin de l'initialisation ------------------------------------------

INT_ON();
return(1);
}

void com_close(void)
{
if (!modem_open) return;

outp(modem_base+1,old_modem_ier);
outp(0x21, old_modem_imr);

SETVECT(modem_irq+8, old_modem_isr);
outp(0x20,0x20);
modem_open=0;
}

/*--------------------------------------------------------------------*
 *--------------------------------------------------------------------*/





void GetCur(char *x,char *y)
{
union REGS regs;

regs.h.bh=0;
regs.h.ah=3;

int386(0x10,&regs,&regs);

*x=regs.h.ch;
*y=regs.h.cl;
}

void PutCur(char x,char y)
{
union REGS regs;

regs.h.ah=1;
regs.h.ch=x;
regs.h.cl=y;

int386(0x10,&regs,&regs);
}

char GetChr(char x,char y)
{
return *(_RB_screen+(y*256+x));
}

char GetCol(char x,char y)
{
return *(_RB_screen+(y*256+x)+256*128);
}




void ColLin(int left,int top,int length,char color)
{
int i;

for (i=left;i<left+length;i++)
      AffCol(i,top,color);
}

void ChrLin(int left,int top,int length,char color)
{
int i;

for (i=left;i<left+length;i++)
      AffChr(i,top,color);
}

void ChrCol(int left,int top,int length,char color)
{
int i;

for (i=top;i<top+length;i++)
      AffChr(left,i,color);
}

void ColCol(int left,int top,int length,char color)
{
int i;

for (i=top;i<top+length;i++)
      AffCol(left,i,color);
}


void ColWin(int left,int top,int right,int bottom,short color)
{
int i,j;

for(j=top;j<=bottom;j++)
    for (i=left;i<=right;i++)
        AffCol(i,j,color);
}

void ChrWin(int left,int top,int right,int bottom,short car)
{
int i,j;

for(j=top;j<=bottom;j++)
    for (i=left;i<=right;i++)
        AffChr(i,j,car);
}



void ScrollUp(void)
{
int x,y;
for (y=0;y<49;y++)
    for (x=0;x<160;x++)
        {
        AffChr(x,y,GetChr(x,y+1));
        AffCol(x,y,GetCol(x,y+1));
        }
}


static char _MEcran[8000];

void MoveText(int x1,int y1,int x2,int y2,int x3,int y3)
{
int x,y;

for (x=0;x<80;x++)
    for (y=0;y<49;y++)
        {
        _MEcran[(x+y*80)*2]=GetChr(x,y);
        _MEcran[(x+y*80)*2+1]=GetCol(x,y);
        }

for (y=y3;y<=y3+(y2-y1);y++)
    for (x=x3;x<=x3+(x2-x1);x++)
        {
        AffChr(x,y,_MEcran[((x-x3+x1)+(y-y3+y1)*80)*2]);
        AffCol(x,y,_MEcran[((x-x3+x1)+(y-y3+y1)*80)*2+1]);
        }

}


/*----------------------------------*
 - Routine de sauvegarde de l'ecran -
 *----------------------------------*/

char *_Ecran[10];
char _EcranX[10],_EcranY[10];
char _EcranD[10],_EcranF[10];
signed short _WhichEcran=0;

void SaveEcran(void)
{
int x,y;

if (_Ecran[_WhichEcran]==NULL)
    _Ecran[_WhichEcran]=GetMem(8000);

for (y=0;y<50;y++)
    for (x=0;x<80;x++)
        _Ecran[_WhichEcran][(x+y*80)*2+1]=GetCol(x,y);

for (y=0;y<50;y++)
    for (x=0;x<80;x++)
        _Ecran[_WhichEcran][(x+y*80)*2]=GetChr(x,y);

WhereXY(&(_EcranX[_WhichEcran]),&(_EcranY[_WhichEcran]));
GetCur(&(_EcranD[_WhichEcran]),&(_EcranF[_WhichEcran]));

_WhichEcran++;
}

void ChargeEcran(void)
{
int x,y;

_WhichEcran--;

#ifdef DEBUG
if ( (_Ecran[_WhichEcran]==NULL) | (_WhichEcran<0) )
    {
    Clr();
    PrintAt(0,0,"Internal Error: ChargeEcran");
    getch();
    return;
    }
#endif

for (y=0;y<50;y++)
    for (x=0;x<80;x++)
        AffCol(x,y,_Ecran[_WhichEcran][(x+y*80)*2+1]);

for (y=0;y<50;y++)
    for (x=0;x<80;x++)
        AffChr(x,y,_Ecran[_WhichEcran][(x+y*80)*2]);

GotoXY(_EcranX[_WhichEcran],_EcranY[_WhichEcran]);
PutCur(_EcranD[_WhichEcran],_EcranF[_WhichEcran]);

free(_Ecran[_WhichEcran]);
_Ecran[_WhichEcran]=NULL;
}



/*--------------------------------*
 - Fonction d'impression du texte -
 *--------------------------------*/
void PrintAt(int x,int y,char *string,...)
{
static char sortie[256];
va_list arglist;

char *suite;
int a;

suite=sortie;

va_start(arglist,string);
vsprintf(sortie,string,arglist);
va_end(arglist);

a=x;
while (*suite!=0)
    {
//    AffChr(a,y,'Û'), Delay(1000);
    AffChr(a,y,*suite);
    a++;
    suite++;
    }

}

void Delay(long ms)
{
clock_t Cl;

Cl=clock();
while((clock()-Cl)<ms);
}

/*----------------------*
 - Retourne 1 sur ESC   -
 ---------* 0 ENTER     -
          - 2 TAB       -
          - 3 SHIFT-TAB -
          *-------------*/

char InputAt(char colonne,char ligne,char *chaine, int longueur)
{
unsigned int caractere;
unsigned char c2;
char chaine2[255],old[255];
char couleur;
int n, i=0 , fin;
int ins=1; // insere = 1 par default

char end,retour;

end=0;
retour=0;

memcpy(old,chaine,255);

fin=strlen(chaine);
if (fin>longueur)    {
    *chaine=0;
    fin=0;
    }

PrintAt(colonne,ligne,chaine); //R‚‚crit la chaine … la position d‚sir‚e

couleur=GetCol(colonne,ligne);
i=0;
for (n=0;n<fin;n++)
    AffCol(colonne+n,ligne,((couleur*16)&127)+(couleur&112)/16);

if (fin==0)
    for (n=0;n<longueur;n++)
        AffChr(colonne+n,ligne,' ');

do  {

if (ins==0)
    PutCur(7,7);
    else
    PutCur(2,7);

caractere=Wait(colonne+i,ligne,ins);

if ( ((caractere&255)!=0) & (couleur!=0) & (caractere!=13)
                                    & (caractere!=27) & (caractere!=9) )
    {
    for (n=0;n<fin;n++)
        {
        AffCol(colonne+n,ligne,couleur);
        AffChr(colonne+n,ligne,' ');
        }
    couleur=0;
    fin=0;
    i=0;
    *chaine=0;
    }

switch (caractere&255)
    {
    case 9:
        retour=2;
        end=1;
        break;
    case 0:             // v‚rifier si pas de touche de fonction press‚e
        c2=(caractere/256);
        if (couleur!=0)           // Preserve ou pas l'ancienne valeur ?
            {
            if ( (c2==71) | (c2==75) | (c2==77) | (c2==79) )
                {
                for (n=0;n<fin;n++) AffCol(colonne+n,ligne,couleur);
                couleur=0;
                }
            if (c2==83)
                {
                for (n=0;n<fin;n++)
                    {
                    AffCol(colonne+n,ligne,couleur);
                    AffChr(colonne+n,ligne,' ');
                    }
                couleur=0;
                fin=0;
                i=0;
                *chaine=0;
                }
            }
        switch (c2)
          {
          case 0x0F:                                        // SHIFT-TAB
            retour=3;
            end=1;
            break;
          case 71: i=0;                          break;          // Home
          case 75: if (i>0) i--; else Beep();    break;          // Left
          case 77: if (i<fin) i++; else Beep();  break;         // Right
          case 79: i=fin;                        break;           // End
          case 13: *(chaine+fin)=0;              break;         // Enter

          case 72: retour=3; end=1; break;
          case 80: retour=2; end=1; break;

          case 83:                                                // del
            if (i!=fin)             // v‚rifier si pas premiere position
                {
                fin--;
                *(chaine+fin+1)=' ';
                *(chaine+fin+2)='\0';
                strcpy(chaine+i,chaine+i+1);
                PrintAt(colonne+i,ligne,chaine+i);
                }
            else
                Beep();
            break;

          case 82:  ins=(!ins);  break;

          default:
                  break;
          }  /* fin du switch */
          break;

      case 8:                                // v‚rifier si touche [del]
        if (i>0)                    // v‚rifier si pas premiere position
            {
            i--;
            fin--;
            if (i!=fin)
            {
            *(chaine+fin+1)=' ';
            *(chaine+fin+2)='\0';
            strcpy(chaine+i,chaine+i+1);
            PrintAt(colonne+i,ligne,chaine+i);
            }
            else
              AffChr(colonne+i,ligne,' ');
            }
        else
            Beep();
        break;

      case 13:                             // v‚rifier si touche [enter]
            retour=0;
            end=1;
            break;

      case 27:                               // v‚rifier si touche [esc]
        if (couleur!=0)
                {
                for (n=0;n<fin;n++)
                    AffCol(colonne+n,ligne,couleur);
                retour=1;
                end=1;
                break;
                }
        if (*chaine==0)
            {
            strcpy(chaine,old);
            PrintAt(colonne,ligne,chaine);
            retour=1;
            end=1;
            }

        for (i=0;i<fin;i++)
                AffChr(colonne+i,ligne,' ');
        fin=0;
        i=0;
        *chaine=0;
        break;

      default:
        {                              // v‚rifier si caractŠre correcte
        if ((caractere>31) && (caractere<=255))
            {
            if ((i==fin) || (!ins))
                {
                if (i==longueur)  i--;
                                else
                if (i==fin) fin++;
                *(chaine+i)=caractere;
                AffChr(colonne+i,ligne,caractere);
                i++;
                }                            // fin du if i==fin || !ins
            else
            if (fin<longueur)
            {
            *(chaine+fin)=0;
            strcpy(chaine2,chaine+i);
            strcpy(chaine+i+1,chaine2);
            *(chaine+i)=caractere;
            PrintAt(colonne+i,ligne,chaine+i);
            fin++;
            i++;
            }
            }                                  // fin du if caractere>31
          else
            Beep();
          }  //--- fin du default --------------------------------------
      }  //--- fin du switch -------------------------------------------
}
while (!end);


*(chaine+fin)=0;

if (couleur!=0)
    for (n=0;n<fin;n++)
        AffCol(colonne+n,ligne,couleur);

GotoXY(0,0);

return retour;
}

/*--------------------------------------------------------------------*\
 -                          Screen Saver                              -
\*--------------------------------------------------------------------*/
int ScreenSaver(void)
{
char a;
int b;

inp(0x3DA);
inp(0x3BA);
outp(0x3C0,0);

a=getch();
if (a==0)
    b=getch()*256+a;
    else
    b=a;

inp(0x3DA);
inp(0x3BA);
outp(0x3C0,0x20);

return b;
}


void Pause(int n)
{
int m;

for (m=0;m<n;m++)
    {
    while ((inp(0x3DA) & 8)!=8);
    while ((inp(0x3DA) & 8)==8);
    }
}


void Beep(void)
{
}

/*--------------------------------------------------------------------*/

// Make a Window (0: exterieurn, 1: interieur)
// -------------------------------------------
void WinCadre(int x1,int y1,int x2,int y2,int type)
{
int x,y;


if ((type==1) | (type==0))
{

// Relief (surtout pour type==1)
for(x=x1;x<=x2;x++)
    AffCol(x,y1,10*16+1);
for(y=y1;y<=y2;y++)
    AffCol(x1,y,10*16+1);

for(x=x1+1;x<=x2;x++)
    AffCol(x,y2,10*16+3);
for(y=y1+1;y<y2;y++)
    AffCol(x2,y,10*16+3);


if (Cfg->UseFont==0)
    switch(type)   {
    case 0:
        AffChr(x1,y1,'Ú');
        AffChr(x2,y1,'¿');
        AffChr(x1,y2,'À');
        AffChr(x2,y2,'Ù');

        for(x=x1+1;x<x2;x++)    AffChr(x,y1,196);
        for(x=x1+1;x<x2;x++)    AffChr(x,y2,196);

        for(y=y1+1;y<y2;y++)    AffChr(x1,y,179);
        for(y=y1+1;y<y2;y++)    AffChr(x2,y,179);
        break;
    case 1:
        AffChr(x1,y1,'É');
        AffChr(x2,y1,'»');
        AffChr(x1,y2,'È');
        AffChr(x2,y2,'¼');

        for(x=x1+1;x<x2;x++)    AffChr(x,y1,'Í');
        for(x=x1+1;x<x2;x++)    AffChr(x,y2,'Í');

        for(y=y1+1;y<y2;y++)    AffChr(x1,y,'º');
        for(y=y1+1;y<y2;y++)    AffChr(x2,y,'º');
        break;
    }
    else
    switch(type)   {
    case 0:
        AffChr(x1,y1,142);
        AffChr(x2,y1,144);
        AffChr(x1,y2,147);
        AffChr(x2,y2,149);

        for(x=x1+1;x<x2;x++)    AffChr(x,y1,143);
        for(x=x1+1;x<x2;x++)    AffChr(x,y2,148);

        for(y=y1+1;y<y2;y++)    AffChr(x1,y,145);
        for(y=y1+1;y<y2;y++)    AffChr(x2,y,146);
        break;
    case 1:
        AffChr(x1,y1,153);
        AffChr(x2,y1,152);
        AffChr(x1,y2,151);
        AffChr(x2,y2,150);

        for(x=x1+1;x<x2;x++)    AffChr(x,y1,148);
        for(x=x1+1;x<x2;x++)    AffChr(x,y2,143);

        for(y=y1+1;y<y2;y++)    AffChr(x1,y,146);
        for(y=y1+1;y<y2;y++)    AffChr(x2,y,145);
        break;
    }
return;
}
else
{
// Relief (surtout pour type==1)
for(x=x1;x<=x2;x++)    AffCol(x,y1,10*16+3);
for(y=y1;y<=y2;y++)    AffCol(x1,y,10*16+3);

for(x=x1+1;x<=x2;x++)    AffCol(x,y2,10*16+1);
for(y=y1+1;y<y2;y++)     AffCol(x2,y,10*16+1);


if (Cfg->UseFont==0)
    switch(type)   {
    case 2:
    case 3:
        AffChr(x1,y1,'Ú');
        AffChr(x2,y1,'¿');
        AffChr(x1,y2,'À');
        AffChr(x2,y2,'Ù');

        for(x=x1+1;x<x2;x++)    AffChr(x,y1,196);
        for(x=x1+1;x<x2;x++)    AffChr(x,y2,196);

        for(y=y1+1;y<y2;y++)    AffChr(x1,y,179);
        for(y=y1+1;y<y2;y++)    AffChr(x2,y,179);
        break;
    }
    else
    switch(type)   {
    case 2:
        AffChr(x1,y1,139);
        AffChr(x2,y1,138);
        AffChr(x1,y2,137);
        AffChr(x2,y2,136);

        for(x=x1+1;x<x2;x++)    AffChr(x,y1,134);
        for(x=x1+1;x<x2;x++)    AffChr(x,y2,129);

        for(y=y1+1;y<y2;y++)    AffChr(x1,y,132);
        for(y=y1+1;y<y2;y++)    AffChr(x2,y,131);
        break;
    case 3:
        AffChr(x1,y1,128);
        AffChr(x2,y1,130);
        AffChr(x1,y2,133);
        AffChr(x2,y2,135);

        for(x=x1+1;x<x2;x++)    AffChr(x,y1,129);
        for(x=x1+1;x<x2;x++)    AffChr(x,y2,134);

        for(y=y1+1;y<y2;y++)    AffChr(x1,y,131);
        for(y=y1+1;y<y2;y++)    AffChr(x2,y,132);
        break;
    }
return;
}


}


// Make A line
// -----------

void WinLine(int x1,int y1,int xl,int type)
{
int x;

if (Cfg->UseFont==0)
    switch(type) {
    case 0:
        for(x=x1;x<x1+xl;x++)       AffChr(x,y1,196);
        break;
    case 1:
        for(x=x1;x<x1+xl;x++)       AffChr(x,y1,196);
        break;
    }
    else
    switch(type) {
    case 0:
        for(x=x1;x<x1+xl;x++)       AffChr(x,y1,143);
        break;
    case 1:
        for(x=x1;x<x1+xl;x++)       AffChr(x,y1,143);
        break;
    }
}


void MakeFont(char *font,char *adr);

void MakeFont(char *font,char *adr)
{
int n;

outpw( 0x3C4, 0x402);
outpw( 0x3C4, 0x704);
outpw( 0x3CE, 0x204);

outpw( 0x3CE, 5);
outpw( 0x3CE, 6);

for (n=0;n<16;n++)
    adr[n]=font[n];

outpw( 0x3C4, 0x302);
outpw( 0x3C4, 0x304);

outpw( 0x3CE, 4);
outpw( 0x3CE, 0x1005);
outpw( 0x3CE, 0xE06);

}

/*

#pragma aux MakeFont = \
    "cli" \
    "mov dx,3C4h" \
    "mov ax,402h" \
    "out dx,ax" \
    "mov ax,704h" \
    "out dx,ax" \
    "mov dx,3CEh" \
    "mov ax,204h" \
    "out dx,ax" \
    "mov ax,5" \
    "out dx,ax" \
    "mov ax,6" \
    "out dx,ax" \
    "mov cx,16" \
    "rep movsb" \
    "mov dx,3C4h" \
    "mov ax,302h" \
    "out dx,ax" \
    "mov ax,304h" \
    "out dx,ax" \
    "mov dx,3CEh" \
    "mov ax,4" \
    "out dx,ax" \
    "mov ax,1005h" \
    "out dx,ax" \
    "mov ax,0E06h" \
    "out dx,ax" \
    "sti" \
    parm [esi] [edi];
*/

void Font8x8(void)
{
FILE *fic;
char *pol;
char *buf=(char*)0xA0000;
int n;

union REGS R;
unsigned char x;

char chaine[256];



Cfg->Tfont=179;                            // Barre Verticale | with 8x8

strcpy(chaine,Fics->path);
strcat(chaine,"\\font8x8.cfg");

Cfg->UseFont=0;
if (Cfg->font==0) return;

fic=fopen(chaine,"rb");
if (fic==NULL) return;

Cfg->UseFont=1;                                 // utilise les fonts 8x8
Cfg->Tfont=168;                            // Barre Verticale | with 8x8

pol=malloc(2048);

fread(pol,2048,1,fic);

fclose(fic);

for (n=0;n<256;n++)
    MakeFont(pol+n*8,buf+n*32);

R.w.bx=(8==8) ? 0x0001 : 0x0800;
x=inp(0x3CC) & (255-12);
(void) outp(0x3C2,x);
// disable();
outpw( 0x3C4, 0x0100);
outpw( 0x3C4, 0x01+ (R.h.bl<<8) );
outpw( 0x3C4, 0x0300);
// enable();

R.w.ax=0x1000;
R.h.bl=0x13;
int386(0x10,&R,&R);
}

void Font8x16(void)
{
FILE *fic;
char *pol;
char *buf=(char*)0xA0000;
int n;

union REGS R;
unsigned char x;

char chaine[256];

Cfg->Tfont=179;                            // Barre Verticale | with 8x8

strcpy(chaine,Fics->path);
strcat(chaine,"\\font8x16.cfg");

Cfg->UseFont=0;
if (Cfg->font==0) return;

fic=fopen(chaine,"rb");
if (fic==NULL) return;

Cfg->UseFont=1;                                 // utilise les fonts 8x8
Cfg->Tfont=168;                            // Barre Verticale | with 8x8

pol=malloc(4096);

fread(pol,4096,1,fic);

fclose(fic);

/*
for (n=0;n<8;n++)
    Cfg->Tfont[n+1]=128+n;
for (n=0;n<8;n++)
    Cfg->Tfont[n+9]=142+n;
*/

for (n=0;n<256;n++)
    MakeFont(pol+n*16,buf+n*32);


R.w.bx=(8==8) ? 0x0001 : 0x0800;
x=inp(0x3CC) & (255-12);
(void) outp(0x3C2,x);
// disable();
outpw( 0x3C4, 0x0100);
outpw( 0x3C4, 0x01+ (R.h.bl<<8) );
outpw( 0x3C4, 0x0300);
// enable();

R.w.ax=0x1000;
R.h.bl=0x13;
int386(0x10,&R,&R);
}


void Mode25(void);
#pragma aux Mode25 = \
    "mov ax,3" \
    "int 10h";

void Mode50(void);
#pragma aux Mode50 = \
    "mov ax,3" \
    "int 10h" \
    "mov bx,0" \
    "mov ax,1112h" \
    "int 10h";

void Mode30(void);
#pragma aux Mode30 = \
    "mov ax,3" \
    "int 10h" \
    "mov ax,1114h" \
    "xor bl,bl" \
    "int 10h" \
    "mov dx,3cch" \
    "in al,dx" \
    "mov dl,0c2h" \
    "or al,192" \
    "out dx,al" \
    "mov dx,3d4h" \
    "mov al,11h" \
    "out dx,al" \
    "inc dx" \
    "and al,112" \
    "or al,12" \
    "mov bl,al" \
    "out dx,al" \
    "dec dx" \
    "mov ax,0B06h" \
    "out dx,ax" \
    "mov ax,3E07h" \
    "out dx,ax" \
    "mov ax,0EA10h" \
    "out dx,ax" \
    "mov ax,0DF12h" \
    "out dx,ax" \
    "mov ax,0E715h" \
    "out dx,ax" \
    "mov ax,0416h" \
    "out dx,ax" \
    "mov al,11h" \
    "out dx,al" \
    "inc dx" \
    "mov al,bl" \
    "out dx,al" \
    "mov ebx,484h" \
    "mov al,29" \
    "mov [ebx],al";


void TXTMode(char lig)
{
Clr();

Cfg->UseFont=0;
switch (lig)
    {
    case 25:
        Mode25();
        break;
    case 30:
        Mode30();
        break;
    case 50:
        Mode50();
        break;
    }
}

void NoFlash(void)
{
union REGS regs;
int n;

regs.w.ax=0x1003;
regs.w.bx=0;
int386(0x10,&regs,&regs);

for (n=0;n<16;n++)
    {
    regs.h.bh=n;
    regs.h.bl=n;
    regs.w.ax=0x1000;
    int386(0x10,&regs,&regs);
    }
}


void LoadPal(void)
{
int n;

for(n=0;n<16;n++)
    SetPal(n,Cfg->palette[n*3],Cfg->palette[n*3+1],Cfg->palette[n*3+2]);
}

void SetPal(char x,char r,char g,char b)
{
outp(0x3C8,x);
outp(0x3C9,r);
outp(0x3C9,g);
outp(0x3C9,b);
}

void *GetMem(int s)
{
void *buf;

buf=malloc(s);

if (buf==NULL)
    {
    exit(1);
    }

memset(buf,0,s);

return buf;
}

void *GetMemSZ(int s)                         // GetMem sans mise … z‚ro
{
void *buf;

buf=malloc(s);

if (buf==NULL)
    {
    exit(1);
    }

return buf;
}


//--- Crc - 32 BIT ANSI X3.66 CRC checksum files -----------------------

static unsigned long int crc_32_tab[] = {   // CRC polynomial 0xedb88320
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


// Return -1 if error, 0 in other case

int crc32file(char *name,unsigned long *crc)
{
FILE *fin;
unsigned long oldcrc32;
unsigned long crc32;
unsigned long oldcrc;
int c;
long charcnt;

oldcrc32 = 0xFFFFFFFF;
charcnt = 0;

if ((fin=fopen(name, "rb"))==NULL)
    {
    perror(name);
    return -1;
    }

while ((c=getc(fin))!=EOF)
    {
    ++charcnt;
    oldcrc32 = (crc_32_tab[(oldcrc32^c) & 0xff]^(oldcrc32 >> 8));
    }

if (ferror(fin))
    {
    perror(name);
    charcnt = -1;
    }
fclose(fin);

crc32 = oldcrc32;
oldcrc = oldcrc32 = ~oldcrc32;

*crc=oldcrc;

return 0;
}

/*--------------------------------------------------------------------*\
 - si p vaut 0 mets off                                               -
 - si p vaut 1 interroge                                              -
 - retourne -1 si SHIFT TAB, 1 si TAB                                 -
\*--------------------------------------------------------------------*/
int Puce(int x,int y,int lng,char p)
{
int r=0;

int car;

AffChr(x,y,16);
AffChr(x+lng-1,y,17);

AffChr(x+lng,y,220);
ChrLin(x+1,y+1,lng,223);

ColLin(x,y,lng,2*16+5);        // Couleur

if (p==1)
    while (r==0)  {
        car=Wait(x,y,0);

        switch(car%256)
            {
            case 13:
                return 0;
            case 27:
                r=1;
                break;
            case 9:
                r=2;
                break;
            case 0:
                switch(car/256)
                    {
                    case 15:
                    case 0x4B:
                    case 72:
                        r=3;
                        break;
                    case 0x4D:
                    case 80:
                        r=2;
                        break;
                    }
                break;
            }
        }


AffChr(x,y,32);
AffChr(x+lng-1,y,32);

AffChr(x+lng,y,220);
ChrLin(x+1,y+1,lng,223);

ColLin(x,y,lng,2*16+3);                                       // Couleur

return r;
}

/*--------------------------------------------------------------------*\
 - si p vaut 0 mets off                                               -
 - si p vaut 1 interroge                                              -
 - retourne -1 si SHIFT TAB, 1 si TAB                                 -
\*--------------------------------------------------------------------*/
int Switch(int x,int y,int *Val)
{
int r=0;

int car;

while (r==0)
    {
    AffChr(x+1,y,(*Val) ? 'X' : ' ');

    car=Wait(x+1,y,0);

    switch(car%256)
        {
        case 13:
            return 0;
        case 27:
            r=1;
            break;
        case 9:
            r=2;
            break;
        case 32:
            (*Val)^=1;
            break;
        case 0:
            switch(car/256)
                {
                case 15:
                case 72:
                    r=3;
                    break;
                case 80:
                    r=2;
                    break;
                }
            break;
        }
    }


return r;
}

/*--------------------------------------------------------------------*\
 - 0 si ENTER                                                         -
 - 1 si ESCAPE                                                        -
 - 2 si -->                                                           -
 - 3 si <--                                                           -
 - 4 si pas bouger                                                    -
 - 5 si HAUT                                                          -
 - 6 si BAT                                                           -
\*--------------------------------------------------------------------*/

int MSwitch(int x,int y,int *Val,int i)
{
int r=0;

int car;

while (r==0)
    {
    AffChr(x+1,y,(*Val)==i ? 'X' : ' ');

    car=Wait(x+1,y,0);

    switch(car%256)
        {
        case 13:
            return 0;
        case 27:
            r=1;
            break;
        case 32:
            (*Val)=i;
            r=4;
            break;
        case 9: // TAB
            r=2;
            break;
        case 0:
            switch(car/256)
                {
                case 0x4B:                                       // LEFT
                case 15:                                    // SHIFT-TAB
                    r=3;
                    break;
                case 72:                                          // BAS
                    r=6;
                    break;
                case 80:                                         // HAUT
                    r=5;
                    break;
                case 0x4D:                                      // RIGHT
                    r=2;
                    break;
                }
            break;
        }
    }


return r;
}

/*--------------------------------------------------------------------*\
 - Retourne 27 si escape                                              -
 - Retourne numero de la liste sinon                                  -
\*--------------------------------------------------------------------*/
int WinTraite(struct Tmt *T,int nbr,struct TmtWin *F)
{
char fin;                                              // si =0 continue
char direct;                                         // direction du tab
int i,i2,j;
int *adr;
static char chaine[80];

SaveEcran();

WinCadre(F->x1,F->y1,F->x2,F->y2,0);
Window(F->x1+1,F->y1+1,F->x2-1,F->y2-1,10*16+1);

PrintAt(F->x1+((F->x2-F->x1)-(strlen(F->name)))/2,F->y1,F->name);

for(i=0;i<nbr;i++)
switch(T[i].type) {
    case 0:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str);
        break;
    case 1:
        ColLin(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier),1*16+5);
        ChrLin(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier),32);
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str);
        break;
    case 2:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,"      OK     ");
        Puce(F->x1+T[i].x,F->y1+T[i].y,13,0);
        break;
    case 3:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,"    CANCEL   ");
        Puce(F->x1+T[i].x,F->y1+T[i].y,13,0);
        break;
    case 4:
        WinCadre(F->x1+T[i].x,F->y1+T[i].y,
                          *(T[i].entier)+F->x1+T[i].x,F->y1+T[i].y+3,1);
        break;
    case 5:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str);
        Puce(F->x1+T[i].x,F->y1+T[i].y,13,0);
        break;
    case 6:
        WinCadre(F->x1+T[i].x,F->y1+T[i].y,
                          *(T[i].entier)+F->x1+T[i].x,F->y1+T[i].y+2,2);
        break;
    case 7:
        j=strlen(T[i].str)+2;
        ColLin(F->x1+T[i].x+j,F->y1+T[i].y,9,1*16+5);
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,"%s: %-9d",T[i].str,
                                                        *(T[i].entier));
        break;
    case 8:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,"[%c] %s",
                                   *(T[i].entier) ? 'X' : ' ',T[i].str);
        break;
    case 9:
        WinCadre(F->x1+T[i].x,F->y1+T[i].y,
            *(T[i].str)+F->x1+T[i].x+1,*(T[i].entier)+F->y1+T[i].y+1,2);
        break;
    case 10:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,
                    "(%c) %s",(*(T[i].entier)==i) ? 'X' : ' ',T[i].str);
        break;
    case 11:
        ChrLin(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier),32);
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str);
        break;
    }

fin=0;
direct=1;
i=0;

while (fin==0) {

for(i2=0;i2<nbr;i2++)   // Affichage a ne faire qu'une fois
    switch(T[i2].type)
        {
        case 10:
            PrintAt(F->x1+T[i2].x,F->y1+T[i2].y,"(%c) %s",
                           (*(T[i2].entier)==i2) ? 'X' : ' ',T[i2].str);
            break;
        }

switch(T[i].type) {
    case 0:
    case 4:
    case 9:
        break;
    case 11:
    case 1:
        direct=InputAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str,
                                                        *(T[i].entier));
        break;
    case 2:
        direct=Puce(F->x1+T[i].x,F->y1+T[i].y,13,1);
        break;
    case 3:
        direct=Puce(F->x1+T[i].x,F->y1+T[i].y,13,1);
        break;
    case 5:
        direct=Puce(F->x1+T[i].x,F->y1+T[i].y,13,1);
        break;
    case 7:
        sprintf(chaine,"%d",*(T[i].entier));
        direct=InputAt(F->x1+T[i].x+strlen(T[i].str)+2,
                                                 F->y1+T[i].y,chaine,9);
        sscanf(chaine,"%d",T[i].entier);
        break;
    case 8:
        direct=Switch(F->x1+T[i].x,F->y1+T[i].y,T[i].entier);
        break;
    case 10:
        direct=MSwitch(F->x1+T[i].x,F->y1+T[i].y,T[i].entier,i);
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,"(%c) %s",
                              (*(T[i].entier)==i) ? 'X' : ' ',T[i].str);
        break;
    }

switch(direct)
    {
    case 0:
        fin=1;                                              // SELECTION
        break;
    case 1:
        fin=2;                                                  // ABORT
        break;
    case 2:                                                 // Next Case
        i++;
        break;
    case 3:                                             // Previous Case
        i--;
        break;
    case 4:                                              // Rien du tout
        break;
    case 5:                                              // Type suivant
        adr=T[i].entier;
        while (adr==T[i].entier)
            {
            i++;
            if (i==nbr) i=0;
            }
        break;
    case 6:                                            // Type precedent
        adr=T[i].entier;
        while (adr==T[i].entier)
            {
            i--;
            if (i==-1) i=nbr-1;
            }
        break;

    default:                                               // Pas normal
        break;
    }

if (i==-1) i=nbr-1;
if (i==nbr) i=0;

}

ChargeEcran();

if (fin==1)
    return i;

return 27;                                                     // ESCAPE
}

/*--------------------------------------------------------------------*\
 - 1 -> Cancel                                                        -
 - 0 -> OK                                                            -
\*--------------------------------------------------------------------*/
int WinError(char *erreur)
{
int x,l;
static char Buffer[70];
static int CadreLength=71;

struct Tmt T[4] = {
      {15,4,2,NULL,NULL},
      {45,4,3,NULL,NULL},
      { 1,1,6,NULL,&CadreLength},
      { 2,2,0,Buffer,NULL}
      };

struct TmtWin F = {
    3,10,76,16,
    "Error"};

l=strlen(erreur);

x=(80-l)/2;                                             // 1-> 39, 2->39
if (x>25) x=25;

l=(40-x)*2;

CadreLength=l+1;

F.x1=x-2;
F.x2=x+l+1;

l=l+3;

T[0].x=(l/4)-5;
T[1].x=(3*l/4)-6;

strcpy(Buffer,erreur);

if (WinTraite(T,4,&F)==0)
    return 0;
    else
    return 1;
}

/*--------------------------------------------------------------------*\
 - 1 -> Cancel                                                        -
 - 0 -> OK                                                            -
\*--------------------------------------------------------------------*/
int WinMesg(char *mesg,char *erreur)
{
int x,l;
static char Buffer[70];
static char Buffer2[70];
static int CadreLength=71;

struct Tmt T[4] = {
      {15,4,2,NULL,NULL},
      {45,4,3,NULL,NULL},
      { 1,1,6,NULL,&CadreLength},
      { 2,2,0,Buffer,NULL}
      };

struct TmtWin F = {
    3,10,76,16,
    Buffer2};

l=strlen(erreur);

x=(80-l)/2;                                             // 1-> 39, 2->39
if (x>25) x=25;

l=(40-x)*2;

CadreLength=l+1;

F.x1=x-2;
F.x2=x+l+1;

l=l+3;

T[0].x=(l/4)-5;
T[1].x=(3*l/4)-6;

strcpy(Buffer,erreur);
strcpy(Buffer2,mesg);

if (WinTraite(T,4,&F)==0)
    return 0;
    else
    return 1;

}

/*--------------------------------------------------------------------*\
 - Avancement de graduation                                           -
 - Renvoit le prochain                                                -
\*--------------------------------------------------------------------*/
int Gradue(int x,int y,int length,int from,int to,int total)
{
short j1,j2;
int j3;

if (total==0) return 0;

if ( (to>1000) & (total>1000) )
    {
    j3=(to/1000);
    j3=(j3*length*8)/(total/1000);
    }
    else
    j3=(to*length*8)/total;

if (j3>=(length*8)) j3=(length*8)-1;

j1=from;

for (;j1<j3;j1++)
    {
    j2=j1/8;
    if (Cfg->UseFont==0)
    switch(j1%8) {
        case 0:
            AffChr(j2+x,y,'*'); // b
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            AffChr(j2+x,y,'*'); // d
            AffChr(j2+x+1,y,32); // i
            break;
        case 5:
        case 6:
        case 7:
            AffChr(j2+x,y,32); // g
            AffChr(j2+x+1,y,'*'); // l
            break;
        }
    else
    switch(j1%8) {
        case 0:
            AffChr(j2+x,y,156); // b
            break;
        case 1:
            AffChr(j2+x,y,157); // c
            AffChr(j2+x+1,y,32); // c
            break;
        case 2:
            AffChr(j2+x,y,158); // d
            AffChr(j2+x+1,y,163); // i
            break;
        case 3:
            AffChr(j2+x,y,159); // e
            AffChr(j2+x+1,y,164); // j
            break;
        case 4:
            AffChr(j2+x,y,160); // f
            AffChr(j2+x+1,y,165); // k
            break;
        case 5:
            AffChr(j2+x,y,161); // g
            AffChr(j2+x+1,y,166); // l
            break;
        case 6:
            AffChr(j2+x,y,162); // h
            AffChr(j2+x+1,y,167); // k
            break;
        case 7:
            AffChr(j2+x,y,32);  // a
            AffChr(j2+x+1,y,155);
            break;
        }
    }

if (to==total)
    ChrLin(x,y,length+1,32);

if (to==0)
    if (Cfg->UseFont==0)
    AffChr(x,y,'*'); // b
    else
    AffChr(x,y,155);

return j1;
}

void DefaultCfg(void)
{
char defcol[48]=RBPALDEF;

Cfg->KeyAfterShell=0;

memcpy(Cfg->palette,defcol,48);

strcpy(Mask[0]->title,"C Style");
strcpy(Mask[0]->chaine,"asm break case cdecl char const continue "
                       "default do double else enum extern far float "
                       "for goto huge if int interrupt long near "
                       "pascal register short signed sizeof static "
                       "struct switch typedef union unsigned void "
                       "volatile while @");
Mask[0]->Ignore_Case=0;
Mask[0]->Other_Col=1;

strcpy(Mask[1]->title,"Pascal Style");
strcpy(Mask[1]->chaine,"absolute and array begin case const div do "
                       "downto else end external file for forward "
                       "function goto if implementation in inline "
                       "interface interrupt label mod nil not of or "
                       "packed procedure program record repeat "
                       "set shl shr string then to type unit until "
                       "uses var while with xor @");
Mask[1]->Ignore_Case=1;
Mask[1]->Other_Col=1;

strcpy(Mask[2]->title,"Assembler Style");
strcpy(Mask[2]->chaine,"aaa aad aam aas adc add and arpl bound bsf bsr "
  "bswap bt btc btr bts call cbw cdq clc cld cli clts cmc cmp cmps "
  "cmpxchg cwd cwde daa das dec div enter esc hlt idiv imul in inc "
  "ins int into invd invlpg iret iretd jcxz jecxz jmp ja jae jb jbe "
  "jc jcxz je jg jge jl jle jna jnae jnb jnbe jnc jne jng jnge jnl "
  "jnle jno jnp jns jnz jo jp jpe jpo js jz lahf lar lds lea leave les "
  "lfs lgdt lidt lgs lldt lmsw lock lods loop loope loopz loopnz "
  "loopne lsl lss ltr mov movs movsx movsz mul neg nop not or out outs "
  "pop popa popad push pusha pushad pushf pushfd rcl rcr rep repe repz "
  "repne repnz ret retf rol ror sahf sal shl sar sbb scas setae setnb "
  "setb setnae setbe setna sete setz setne setnz setl setng setge "
  "setnl setle setng setg setnle sets setns setc setnc seto setno "
  "setp setpe setnp setpo sgdt sidt shl shr shld shrd sldt smsw stc "
  "std sti stos str sub test verr verw wait fwait wbinvd xchg xlat "
  "db dw dd endp ends assume xlatb xor @");

Mask[1]->Ignore_Case=1;
Mask[1]->Other_Col=1;

strcpy(Mask[15]->title,"User Defined Style");
strcpy(Mask[15]->chaine,"ketchup killers redbug access darkangel "
                      "marjorie katana ecstasy cray magic fred cobra z @");
Mask[15]->Ignore_Case=1;
Mask[15]->Other_Col=1;

strcpy(Cfg->extens,"RAR ARJ ZIP LHA DIZ EXE COM BAT BTM");

Cfg->wmask=15;      // RedBug preference

Cfg->TailleY=30;
Cfg->font=1;
Cfg->AnsiSpeed=133;
Cfg->SaveSpeed=7200;



Cfg->fentype=4;

Cfg->mtrash=100000;

Cfg->currentdir=1;
Cfg->overflow=0;

Cfg->autoreload=1;
Cfg->verifhist=1;
Cfg->palafter=0;

Cfg->noprompt=0;

Cfg->crc=0x69;

Cfg->debug=0;

Cfg->key=0;

Cfg->dispcolor=1;
Cfg->speedkey=1;

Cfg->insdown=1;
Cfg->seldir=1;

Cfg->display=0;

Cfg->comport=2;
Cfg->comspeed=19200;
Cfg->combit=8;
Cfg->comparity='N';
Cfg->comstop=1;

Cfg->enterkkd=1;

Cfg->warp=1;

Cfg->editeur[0]=0;
Cfg->vieweur[0]=0;

strcpy(Cfg->HistDir,"C:\\");
}

/*--------------------------------------------------------------------*
 -                       Error and Signal Handler                     -
 ----------------------------------------------------------------------
 - Return IOerr si IOerr = 1 ou 3                                     -
 - Return     3 si IOver = 1                                          -
 *--------------------------------------------------------------------*/

int __far Error_handler(unsigned deverr,unsigned errcode,
                                                   unsigned far *devhdr)
{
int i,n,erreur[3];
char car;

switch(IOerr)
    {
    case 1:
        return _HARDERR_IGNORE;
    case 3:
        return _HARDERR_FAIL;
    }

IOerr=1;

if (IOver==1)
    return _HARDERR_FAIL;

SaveEcran();

WinCadre(19,9,61,16,0);
Window(20,10,60,15,10*16+4);

PrintAt(23,10,"Disk Error: %s",((deverr&32768)==32768) ? "No":"Yes");

PrintAt(23,11,"Position of error: ");
switch((deverr&1536)/512)  {
    case 0: PrintAt(42,11,"MS-DOS"); break;
    case 1: PrintAt(42,11,"FAT"); break;
    case 2: PrintAt(42,11,"Directory"); break;
    case 3: PrintAt(42,11,"Data-area"); break;
    }

PrintAt(23,12,"Type of error: %s %04X",((deverr&256)==256) ?
                                                 "Write":"Read",deverr);

i=8192;
n=0;

for(n=0;n<3;n++)
    {
    if ((deverr&i)==i)
        erreur[n]=1;
        else
        erreur[n]=0;
    i=i/2;
    }

if (erreur[0]==1) PrintAt(25,14,"Ignore"),AffCol(25,14,10*16+5),
                                                WinCadre(24,13,31,15,2);
if (erreur[1]==1) PrintAt(38,14,"Retry"),AffCol(38,14,10*16+5),
                                                WinCadre(37,13,43,15,2);
if (erreur[2]==1) PrintAt(51,14,"Fail"),AffCol(51,14,10*16+5),
                                                WinCadre(50,13,55,15,2);

IOerr=0;
do
{
car=getch();

if ( (car=='I') | (car=='i') & (erreur[0]==1) ) IOerr=1;
if ( (car=='R') | (car=='r') & (erreur[1]==1) ) IOerr=2;
if ( (car=='F') | (car=='f') & (erreur[2]==1) ) IOerr=3;

}
while (IOerr==0);

ChargeEcran();

switch(IOerr)
    {
    case 1:
        return _HARDERR_IGNORE;
    case 2:
        return _HARDERR_RETRY;
    }
return _HARDERR_FAIL;
}

// Retourne 0 si tout va bene
int VerifyDisk(char c)  // 1='A'
{
unsigned nbrdrive,cdrv,n;
struct diskfree_t d;

if ((c<1) | (c>26)) return 1;

n=_bios_equiplist();

if ( ((n&192)==0) & (c==2) ) return 1;            // Seulement un disque
if ( ((n&1)==0) & (c==1) ) return 1;                    // Pas de disque


_dos_getdrive(&cdrv);

IOerr=0;
IOver=1;

_dos_setdrive(c,&nbrdrive);
// getcwd(path,256);

if (_dos_getdiskfree(c,&d)!=0)
    IOerr=1;

_dos_setdrive(cdrv,&nbrdrive);

return IOerr;
}

int InitScreen(int a)
{
char buf[31];
int nr;

// --- Console ---------------------------------------------------------

AffChr=Cache_AffChr;
AffCol=Cache_AffCol;
Wait=Cache_Wait;
KbHit=kbhit;
GotoXY=Cache_GotoXY;
WhereXY=Cache_WhereXY;

Clr=Norm_Clr;
Window=Norm_Window;

while(1)
    switch (a)
    {
    case 0:
        return 1;
    case 1:
        Clr();
        PrintAt(0,0,"Try Ansi Mode");

        nr=0;
        cprintf("\x1b[6n\r      \r");      // ask for ansi device report
        while ((0 !=kbhit()) && (nr<30))//read whatever input is present
            buf[nr++] = getch();

        buf[nr]=0;                              // zero terminate string
        if (strncmp(buf,"\x1b[",2)!=0) //check precense of device report
            return 0;

        AffChr=Ansi_AffChr;
        AffCol=Ansi_AffCol;
        Wait=Cache_Wait;
        KbHit=kbhit;
        GotoXY=Ansi_GotoXY;
        WhereXY=Cache_WhereXY;
        Clr=Ansi_Clr;
        Window=Ansi_Window;
        return 1;
    case 2:
        Clr();
        PrintAt(0,0,"Try Doorway Mode");

        com_open(Cfg->comport,Cfg->comspeed,Cfg->combit,
                 Cfg->comparity,Cfg->comstop);

        AffChr=Com_AffChr;
        AffCol=Com_AffCol;
        Wait=Com_Wait;
        KbHit=Com_KbHit;
        GotoXY=Com_GotoXY;
        WhereXY=Cache_WhereXY;
        Clr=Com_Clr;
        Window=Com_Window;
        return 1;
    default:
        return 0;
    }
}

void DesinitScreen(void)
{
switch (Cfg->display)
    {
    case 0:
        break;
    case 1:
        cprintf("\x1b[0m\n\n\x1b[2J");
        break;
    case 2:
        com_close();
        break;
    }
}

/*-------------------------------------------------------------*
 -   Gestion de la barre de menu                               -
 - Renvoie 0 pour ESC                                          -
 - Sinon numero du titre;                                      -
 - xp: au depart, c'est le numero du titre                     -
 -     a l'arrivee ,c'est la position du titre                 -
 *-------------------------------------------------------------*/

int BarMenu(struct barmenu *bar,int nbr,int *poscur,int *xp,int *yp)
{
int c,i,j,n,x;
char let[32];
int car=0;

for (n=0;n<nbr;n++)
    let[n]=toupper(bar[n].titre[0]);

ColLin(0,0,80,1*16+7);
ChrLin(0,0,80,32);


x=0;
for(n=0;n<nbr;n++)
    x+=strlen(bar[n].titre);

i=(80-x)/nbr;
x=(80-(nbr-1)*i-x)/2;

c=*poscur;

do
{
if (c<0) c=nbr-1;
if (c>=nbr) c=0;

j=0;
for (n=0;n<nbr;n++)
    {
    if (n==c)
        {
        AffCol(x+j+n*i-1,0,7*16+4);
        AffCol(x+j+n*i,0,7*16+5);
        ColLin(x+j+n*i+1,0,strlen(bar[n].titre),7*16+4);
        *xp=x+j+n*i;
        }
        else
        {
        AffCol(x+j+n*i-1,0,1*16+7);
        AffCol(x+j+n*i,0,1*16+5);
        ColLin(x+j+n*i+1,0,strlen(bar[n].titre),1*16+7);
        }

    PrintAt(x+j+n*i,0,"%s",bar[n].titre);
    j+=strlen(bar[n].titre);
    }

if (*yp==0)
    break;

car=Wait(0,0,0);

switch(HI(car))
    {
    case 0x4B:
        c--;
        break;
    case 0x4D:
        c++;
        break;
    case 80:
        *yp=1;
        car=13;
        break;
    }
if (LO(car)!=0)
    for (n=0;n<nbr;n++)
        if (toupper(car)==let[n])
            c=n;
}
while ( (car!=13) & (car!=27) );

*poscur=c;

if (car==27)
    return 0;
    else
    return 1;
}

// 1: [RIGHT]   -1: [LEFT]
// 0: [ESC]      2: [ENTER]
int PannelMenu(struct barmenu *bar,int nbr,int *c,int *xp,int *yp)
{
int max,n,m,car,fin;
int i,col;
char couleur;
char let[32];

for (n=0;n<nbr;n++)
    {
    i=0;

    do
        {
        let[n]=toupper(bar[n].titre[i]);
        fin=1;
        for (m=0;m<n;m++)
            if (let[m]==let[n]) fin=0,i++;
        }
    while(fin==0);

    }

max=0;

for (n=0;n<nbr;n++)
    if (max<strlen(bar[n].titre))
        max=strlen(bar[n].titre);

SaveEcran();

if ((*xp)<1) (*xp)=1;

WinCadre(*xp-1,*yp-1,*xp+max,*yp+nbr,3);
Window(*xp,*yp,*xp+max-1,*yp+nbr-1,10*16+4);

fin=0;

do
{
if ((*c)<0)   (*c)=nbr-1;
if ((*c)>=nbr) (*c)=0;

for (n=0;n<nbr;n++)
    {
    if (bar[n].fct==0)
        {
        ChrLin(*xp,(*yp)+n,max,196);
        ColLin(*xp,(*yp)+n,max,10*16+1);
        }
        else
        {
        PrintAt(*xp,(*yp)+n,"%s",bar[n].titre);
        col=1;
        if (n==*c)
            couleur=7*16+1;
            else
            couleur=10*16+4;

        for (i=0;i<strlen(bar[n].titre);i++)
            {
            if ( (col==1) & (toupper(bar[n].titre[i])==let[n]) )
                AffCol((*xp)+i,(*yp)+n,(couleur&240)+5),col=0;
                else
                AffCol((*xp)+i,(*yp)+n,couleur);
            }
        }
    }

car=Wait(0,0,0);

do
{
switch(HI(car))
    {
    case 0x48:  (*c)--; break;
    case 0x4B:  fin=-1; car=27;  break;
    case 0x4D:  fin=1;  car=27;  break;
    case 0x50:  (*c)++; break;
    }

if (LO(car)!=0)
    for (n=0;n<nbr;n++)
        if (toupper(car)==let[n])
            (*c)=n;
}
while (bar[*c].fct==0);

}
while ( (car!=13) & (car!=27) );

ChargeEcran();

if (car==27)
    return fin;
    else
    return 2;
}




