
/*--------------------------------------------------------------------*\
|- Hard-function                                                      -|
\*--------------------------------------------------------------------*/
#include <stdarg.h>
#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include <malloc.h>
#include <i86.h>
#include <direct.h>

#include <dos.h>

#include <ctype.h>

#include <bios.h>

#include <time.h>

#include <string.h>

#include <io.h>
#include <conio.h>

#include "hard.h"

/*--------------------------------------------------------------------*\
|- variable globale                                                   -|
\*--------------------------------------------------------------------*/

int IOver;
int IOerr;

struct RB_info *Info;
struct config *Cfg;
struct PourMask **Mask;
struct fichier *Fics;

/*--------------------------------------------------------------------*\
|- Variable locale                                                    -|
\*--------------------------------------------------------------------*/

static char _MouseOK=0;

static int _MPosX,_MPosY;
static int _PasX,_PasY,_TmpClik;
static int _xm,_ym,_zm,_zmok;
static int _dclik,_mclock;
static char _charm;

char _IntBuffer[256];

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
|- Fonction interne                                                   -|
\*--------------------------------------------------------------------*/
void MakeFont(char *font,char *adr);

/*--------------------------------------------------------------------*\
|-  Fonction interne d'affichage                                      -|
\*--------------------------------------------------------------------*/
void Norm_Clr(void);
void Norm_Window(int left,int top,int right,int bottom,short color);

void Norm_Clr(void)
{
char i,j;

for(j=0;j<Cfg->TailleY;j++)
    for (i=0;i<Cfg->TailleX;i++)
      AffCol(i,j,7);

for(j=0;j<Cfg->TailleY;j++)
    for (i=0;i<Cfg->TailleX;i++)
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
|-            Affiche des caractŠres directement … l'‚cran            -|
\*--------------------------------------------------------------------*/
void Cache_AffChr(char x,char y,char c);
void Cache_AffCol(char x,char y,char c);

char *scrseg[50];

void Cache_AffChr(char x,char y,char c)
{
if (*(_RB_screen+(y*256+x))!=c)
    {
    *(scrseg[y]+(x<<1))=c;
    *(_RB_screen+(y*256+x))=c;
    }
}

void Cache_AffCol(char x,char y,char c)
{
if (*(_RB_screen+(y*256+x)+256*128)!=c)
    {
    *(scrseg[y]+(x<<1)+1)=c;
    *(_RB_screen+(y*256+x)+256*128)=c;
    }
}

int Cache_Wait(int x,int y,char c)
{
char a;
int b;
clock_t Cl;

int xm=0,ym=0,zm=0;

if ((x!=0) | (y!=0))
    GotoXY(x,y);

Cl=clock();

a=0;
b=0;

while ( (!kbhit()) & (b==0) & (zm==0) )
    {
    GetPosMouse(&xm,&ym,&zm);

    if ( ((clock()-Cl)>Cfg->SaveSpeed) & (Cfg->SaveSpeed!=0) )
        b=ScreenSaver();
    }

if (zm!=0) _zmok=0;

if ((b==0) & (zm==0))
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



/*--------------------------------------------------------------------*\
|-                       Affichage par code Ansi                      -|
\*--------------------------------------------------------------------*/
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



/*--------------------------------------------------------------------*\
|-                       Affichage par COM  Ansi                      -|
\*--------------------------------------------------------------------*/
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

    *(scrseg[y]+(x<<1)+1)=c;            // --------- Echo console --

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

    *(scrseg[y]+(x<<1))=c;              // --------- Echo console --

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

        *(scrseg[j]+(i<<1)+1)=color;    // --------- Echo console --
        }

for(j=top;j<=bottom;j++)
    for (i=left;i<=right;i++)
        AffChr(i,j,32);
}


/*--------------------------------------------------------------------*\
|-                       Gestion du port s‚rie                        -|
\*--------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------*\
|- This will return 0 is there is no character waiting.  Please check -|
|- the port with com_ch_ready(); first so that if they DID send a 0x0 -|
|-     that you will know it's a true 0, not a no character return!   -|
\*--------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------*\
|-  Set up the Interupt Info                                          -|
\*--------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------*\
|----- Speed ----------------------------------------------------------|
\*--------------------------------------------------------------------*/

x=inp(modem_base+3);                                // Read In Old Stats

if ((x & 0x80)!=0x80) outp(modem_base+3,x+0x80);          // Set DLab On

d=(short)(115200/speed);
l=d & 0xFF;
m=(d >> 8) & 0xFF;

outp(modem_base+0,l);
outp(modem_base+1,m);

outp(modem_base+3,x);                            // Restore the DLAB bit

/*--------------------------------------------------------------------*\
|---- Data-bit --------------------------------------------------------|
\*--------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------*\
|---- Parity ----------------------------------------------------------|
\*--------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------*\
|---- Stop bits -------------------------------------------------------|
\*--------------------------------------------------------------------*/
newb=0;

x=inp(modem_base+3);

newb=(x<<6>>6)+(x>>5<<5);                      // Kill the old Stop Bits

if (stop==2) newb+=0x04;         // Only check for 2, assume 1 otherwise

outp(modem_base+3,newb);

/*--------------------------------------------------------------------*\
|---- fin de l'initialisation -----------------------------------------|
\*--------------------------------------------------------------------*/

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

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


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


void MoveText(int x1,int y1,int x2,int y2,int x3,int y3)
{
static char _MEcran[8000];
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

/*--------------------------------------------------------------------*\
|-  Routine de sauvegarde de l'ecran                                  -|
\*--------------------------------------------------------------------*/

char *_Ecran[10];
char _EcranX[10],_EcranY[10];
char _EcranD[10],_EcranF[10];
signed short _WhichEcran=0;

void SaveEcran(void)
{
int x,y,n;

if (_Ecran[_WhichEcran]==NULL)
    _Ecran[_WhichEcran]=GetMem((Cfg->TailleY)*(Cfg->TailleX)*2);


n=0;
for (y=0;y<Cfg->TailleY;y++)
    for (x=0;x<Cfg->TailleX;x++)
        {
        _Ecran[_WhichEcran][n*2+1]=GetCol(x,y);
        n++;
        }

n=0;
for (y=0;y<Cfg->TailleY;y++)
    for (x=0;x<Cfg->TailleX;x++)
        {
        _Ecran[_WhichEcran][n*2]=GetChr(x,y);
        n++;
        }

WhereXY(&(_EcranX[_WhichEcran]),&(_EcranY[_WhichEcran]));
GetCur(&(_EcranD[_WhichEcran]),&(_EcranF[_WhichEcran]));

_WhichEcran++;
}

void ChargeEcran(void)
{
int x,y,n;

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


n=0;
for (y=0;y<Cfg->TailleY;y++)
    for (x=0;x<Cfg->TailleX;x++)
        {
        AffCol(x,y,_Ecran[_WhichEcran][n*2+1]);
        n++;
        }

n=0;
for (y=0;y<Cfg->TailleY;y++)
    for (x=0;x<Cfg->TailleX;x++)
        {
        AffChr(x,y,_Ecran[_WhichEcran][n*2]);
        n++;
        }

GotoXY(_EcranX[_WhichEcran],_EcranY[_WhichEcran]);
PutCur(_EcranD[_WhichEcran],_EcranF[_WhichEcran]);

LibMem(_Ecran[_WhichEcran]);
_Ecran[_WhichEcran]=NULL;
}



/*--------------------------------------------------------------------*\
|-  Fonction d'impression du texte                                    -|
\*--------------------------------------------------------------------*/
void PrintAt(int x,int y,char *string,...)
{
char sortie[256];      // Pas de raison que ca soit un static ici (ok ?)
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

/*--------------------------------------------------------------------*\
|-    Retourne 1 sur ESC                                              -|
|-             0 ENTER                                                -|
|-             2 TAB                                                  -|
|-             3 SHIFT-TAB                                            -|
\*--------------------------------------------------------------------*/

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
if (fin>longueur)
    {
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

if (caractere==0)
    {
    int px,py;

    px=MousePosX();
    py=MousePosY();

    if ( (py!=ligne) | (px<colonne) | (px>=colonne+longueur) )
        {
        retour=8;
        end=1;
        }
    }

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

          case 0x3B:    //---------------------- F1 --------------------
                retour=7;
                end=1;
                break;

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
|-                           Screen Saver                             -|
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

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-  Make a Window (0: exterieurn, 1: interieur)                       -|
\*--------------------------------------------------------------------*/
void WinCadre(int x1,int y1,int x2,int y2,int type)
{
int x,y;


if ((type==1) | (type==0))
{
    //--- Relief (surtout pour type==1) --------------------------------
    for(x=x1;x<=x2;x++)
        AffCol(x,y1,10*16+1);
    for(y=y1;y<=y2;y++)
        AffCol(x1,y,10*16+1);

    for(x=x1+1;x<=x2;x++)
        AffCol(x,y2,10*16+3);
    for(y=y1+1;y<y2;y++)
        AffCol(x2,y,10*16+3);


    if (Cfg->UseFont==0)
        switch(type)
            {
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
        switch(type)
            {
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
    //--- Relief (surtout pour type==1) --------------------------------
    for(x=x1;x<=x2;x++)    AffCol(x,y1,10*16+3);
    for(y=y1;y<=y2;y++)    AffCol(x1,y,10*16+3);

    for(x=x1+1;x<=x2;x++)    AffCol(x,y2,10*16+1);
    for(y=y1+1;y<y2;y++)     AffCol(x2,y,10*16+1);


    if (Cfg->UseFont==0)
        switch(type)
            {
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
        switch(type)
            {
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


/*--------------------------------------------------------------------*\
|-  Make A line                                                       -|
\*--------------------------------------------------------------------*/

void WinLine(int x1,int y1,int xl,int type)
{
register int x;
char car;

if (Cfg->UseFont==0)
    switch(type)
        {
        case 0:
        case 1:
        case 2:
            car=196;    break;
        }
else
    switch(type)
        {
        case 0:
        case 1:
            car=143;    break;
        case 2:
            car=196;    break;
        }

for(x=x1;x<x1+xl;x++)   AffChr(x,y1,car);
}



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

void Font8x(int height)
{
FILE *fic;
char *pol;
char *buf=(char*)0xA0000;
int n;

char chaine[256];

Cfg->Tfont=179;                            // Barre Verticale | with 8x?

strcpy(chaine,Fics->path);
sprintf(chaine+strlen(chaine),"\\font8x%d.cfg",height);

Cfg->UseFont=0;
if (Cfg->font==0) return;

fic=fopen(chaine,"rb");
if (fic==NULL) return;

Cfg->UseFont=1;                                 // utilise les fonts 8x?
Cfg->Tfont=168;                            // Barre Verticale | with 8x?

pol=GetMem(256*height);

fread(pol,256*height,1,fic);

fclose(fic);

for (n=0;n<256;n++)
    MakeFont(pol+n*height,buf+n*32);

if (Cfg->TailleX==80)                         // 9 bits normal -> 8 bits
    {
    unsigned char x;
    union REGS R;

    R.w.bx=(8==8) ? 0x0001 : 0x0800;

    x=inp(0x3CC) & (255-12);

    outp(0x3C2,x);
    // disable();
    outpw( 0x3C4, 0x0100);
    outpw( 0x3C4, 0x01+ (R.h.bl<<8) );
    outpw( 0x3C4, 0x0300);
    // enable();

    R.w.ax=0x1000;
    R.h.bl=0x13;
    int386(0x10,&R,&R);
    }


LibMem(pol);
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
    "out dx,al";

void Mode90(void);

/*
#pragma aux Mode90 = \
    "mov dx,03C4h" \
    "mov ax,0100h" \
    "out dx,ax" \
    "inc ax" \
    "out dx,ax" \
    "mov dx,03CCh" \
    "in al,dx" \
    "and al,0F3h" \
    "or al,4" \
    "mov dx,03C2h" \
    "out dx,al" \
    "mov dx,03DAh" \
    "in al,dx" \
    "mov dx,03C0h" \
    "mov al,13h" \
    "out dx,al" \
    "xor al,al" \
    "out dx,al" \
    "mov al,20h" \
    "out dx,al" \
    "out dx,al" \
    "mov dx,03D4h" \
    "mov al,11h" \
    "out dx,al" \
    "inc dx" \
    "in al,dx" \
    "and al,7Fh" \
    "out dx,al" \
    "dec dx" \
    "mov ax,06B00h" \
    "out dx,ax" \
    "mov ax,05901h" \
    "out dx,ax" \
    "mov ax,05A02h" \
    "out dx,ax" \
    "mov ax,08E03h" \
    "out dx,ax" \
    "mov ax,06004h" \
    "out dx,ax" \
    "mov ax,08D05h" \
    "out dx,ax" \
    "mov ax,02D13h" \
    "out dx,ax" \
    "mov ah,al" \
    "mov al,11h" \
    "or ah,80h" \
    "out dx,ax" \
    "mov dx,03C4h" \
    "mov ax,0300h" \
    "out dx,ax";
*/

void Mode90(void)
{
unsigned char x;

outpw(0x3C4,0x100);                                 // Synchronous reset
outpw(0x3C4,0x101);                                     // 8 pixels/char

x=inp(0x3CC);
x=(x&0xF3)|4;                          // mets les bits 2-3 … 01 = 28MhZ
outp(0x3C2,x);

x=inp(0x3DA);
outp(0x3C0,0x13);                                  // Horizontal panning

outp(0x3C0,0);                                            // set shift=0

outp(0x3C0,0x20);                                      // Restart screen
outp(0x3C0,0x20);

outp(0x3D4,0x11);                                    // Register protect

x=inp(0x3D5);
x=x&0X7F;
outp(0x3D5,x);                                       // Turn off protect

outpw(0x3D4,0x6B00);                                 // Horizontal Total
outpw(0x3D4,0x5901);                             // Horizontal Displayed
outpw(0x3D4,0x5A02);                             // Start Horiz Blanking
outpw(0x3D4,0x8E03);                               // End Horiz Blanking
outpw(0x3D4,0x6004);                              // Start Horiz Retrace
outpw(0x3D4,0x8D05);                                // End Horiz Retrace
outpw(0x3D4,0x2D13);                                // Memory Allocation

outpw(0x3D4,0x9311);                                  // Turn on protect

outpw(0x3C4,0x300);                                 // Restart Sequencer
}

void Mode80(void)
{

}

void TXTMode(char lig)
{
char *TX=(char*)0x44A;
char *TY=(char*)0x484;

Clr();

if (Cfg->TailleX==0) Cfg->TailleX=80;
if (Cfg->TailleY==0) Cfg->TailleY=25;

if (lig==0)
    lig=Cfg->TailleY;

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

switch(Cfg->TailleX)
    {
    case 80:
        Mode80();
        break;
    case 90:
        Mode90();
        break;
    }

(*TX)=Cfg->TailleX;
(*TY)=Cfg->TailleY-1;

InitSeg();
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

void LibMem(char *mem)
{
free(mem);
}

void *GetMem(int s)
{
void *buf;


buf=malloc(s);

if (buf==NULL)
    exit(1);

memset(buf,0,s);

return buf;
}

void *GetMemSZ(int s)                         // GetMem sans mise … z‚ro
{
void *buf;

buf=malloc(s);

if (buf==NULL)
    exit(1);

return buf;
}


/*--------------------------------------------------------------------*\
|-   si p vaut 0 mets off                                             -|
|-   si p vaut 1 interroge                                            -|
|-   retourne -1 si SHIFT TAB, 1 si TAB                               -|
\*--------------------------------------------------------------------*/
int Puce(int x,int y,int lng,char p)
{
int r=0;

int car;

AffChr(x,y,16);
AffChr(x+lng-1,y,17);

AffChr(x+lng,y,220);
ChrLin(x+1,y+1,lng,223);

ColLin(x,y,lng,2*16+5);                                       // Couleur

if (p==1)
    while (r==0)
        {
        car=Wait(x,y,0);

        if (car==0)
            {
            int px,py,pz;

            px=MousePosX();
            py=MousePosY();
            pz=MouseButton();

            if ( ((pz&4)==4) & (px>=x) & (px<x+lng) & (py==y) )
                car=13;
                else
                r=8;
            }

        switch(car%256)
            {
            case 13:
                return 0;
            case 27:
                r=1;          break;
            case 9:
                r=2;          break;
            case 0:
                switch(car/256)
                    {
                    case 15:
                    case 0x4B:
                    case 72:
                        r=3;               break;
                    case 0x4D:
                    case 80:
                        r=2;               break;
                    case 0x3B:
                        r=7;               break;
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
|-   si p vaut 0 mets off                                             -|
|-   si p vaut 1 interroge                                            -|
|-   retourne -1 si SHIFT TAB, 1 si TAB                               -|
\*--------------------------------------------------------------------*/
int Switch(int x,int y,int *Val)
{
int r=0;

int car;

while (r==0)
    {
    AffChr(x+1,y,(*Val) ? 'X' : ' ');

    car=Wait(x+1,y,0);

    if (car==0)
        {
        int px,py,pz;

        px=MousePosX();
        py=MousePosY();
        pz=MouseButton();

        if ( ((pz&4)==4) & (px==x+1) & (py==y) )
            car=32;
            else
            r=8;
        }

    switch(car%256)
        {
        case 13:
            return 0;
        case 27:
            r=1;            break;
        case 9:
            r=2;            break;
        case 32:
            (*Val)^=1;      break;
        case 0:
            switch(car/256)
                {
                case 15:
                case 72:
                    r=3;            break;
                case 80:
                    r=2;            break;
                case 0x3B:
                    r=7;            break;
                }
            break;
        }
    }


return r;
}

/*--------------------------------------------------------------------*\
|-   0 si ENTER                                                       -|
|-   1 si ESCAPE                                                      -|
|-   2 si -->                                                         -|
|-   3 si <--                                                         -|
|-   4 si pas bouger                                                  -|
|-   5 si HAUT                                                        -|
|-   6 si BAT                                                         -|
|-   7 si F1                                                          -|
|-   8 si Souris                                                      -|
\*--------------------------------------------------------------------*/
int MSwitch(int x,int y,int *Val,int i)
{
int r=0;

int car;

while (r==0)
    {
    AffChr(x+1,y,(*Val)==i ? 'X' : ' ');

    car=Wait(x+1,y,0);

    if (car==0)
        {
        int px,py,pz;

        px=MousePosX();
        py=MousePosY();
        pz=MouseButton();

        if ( ((pz&4)==4) & (px==x+1) & (py==y) )
            car=32;
            else
            r=8;
        }

    switch(car%256)
        {
        case 13:
            return 0;
        case 27:
            r=1;            break;
        case 32:
            (*Val)=i;
            r=4;            break;
        case 9:                                                   // TAB
            r=5;            break;
        case 0:
            switch(car/256)
                {
                case 0x4B:                                       // LEFT
                case 15:                                    // SHIFT-TAB
                    r=6;                    break;
                case 72:                                          // BAS
                    r=3;                    break;
                case 80:                                         // HAUT
                    r=2;                    break;
                case 0x4D:                                      // RIGHT
                    r=5;                    break;
                case 0x3B:
                    r=7;                    break;
                }
            break;
        }
    }


return r;
}

/*--------------------------------------------------------------------*\
|-   Retourne 27 si escape                                            -|
|-   Retourne numero de la liste sinon                                -|
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

for(i2=0;i2<nbr;i2++)   //--- Affichage a ne faire qu'une fois ---------
    switch(T[i2].type)
        {
        case 10:
            PrintAt(F->x1+T[i2].x,F->y1+T[i2].y,"(%c) %s",
                           (*(T[i2].entier)==i2) ? 'X' : ' ',T[i2].str);
            break;
        }

switch(T[i].type)
    {
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
        ColLin(F->x1+T[i].x+4,F->y1+T[i].y,strlen(T[i].str),7*16+4);
        direct=MSwitch(F->x1+T[i].x,F->y1+T[i].y,T[i].entier,i);
        ColLin(F->x1+T[i].x+4,F->y1+T[i].y,strlen(T[i].str),10*16+1);
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,"(%c) %s",
                              (*(T[i].entier)==i) ? 'X' : ' ',T[i].str);
        break;
    }

switch(direct)
    {
    case 0:                                                 // SELECTION
        fin=1;   break;
    case 1:                                                     // ABORT
        fin=2;   break;
    case 2:                                                 // Next Case
        i++;     break;
    case 3:                                             // Previous Case
        i--;     break;
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
    case 7:                                       // Aide sur la fenˆtre
        HelpTopic(F->name);      break;
    case 8:
        {
        int px,py,j,k;
        int x1,x2,y1;

        px=MousePosX();
        py=MousePosY();

        k=-1;
        for(j=0;j<nbr;j++)
            {
            x1=F->x1+T[j].x;
            x2=F->x1+T[j].x+(*(T[j].entier));
            y1=F->y1+T[j].y;

            switch(T[j].type)
                {
                case 11:
                case 1:
                case 7:
                    if ( (py==y1) & (px>=x1) & (px<x2) ) k=j;
                    break;
                case 2:
                case 3:
                case 5:
                    if ( (px>=x1) & (px<x1+13) & (py==y1) ) k=j;
                    break;
                case 8:
                case 10:
                    if ( (px==x1+1) & (py==y1) ) k=j;
                    break;
                }
            if (k!=-1) break;
            }
        if (k!=-1) i=k;
        }
        break;
    case 4:
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
|-  1 -> Cancel                                                       -|
|-  0 -> OK                                                           -|
\*--------------------------------------------------------------------*/
int WinError(char *erreur)
{
return WinMesg("Error",erreur);
}

/*--------------------------------------------------------------------*\
|-  1 -> Cancel                                                       -|
|-  0 -> OK                                                           -|
\*--------------------------------------------------------------------*/
int WinMesg(char *mesg,char *erreur)
{
int x,l;
static char Buffer[70],Buffer2[70];
static int CadreLength=71;

struct Tmt T[4] = {
      {15,4,2,NULL,NULL},
      {45,4,3,NULL,NULL},
      { 1,1,6,NULL,&CadreLength},
      { 2,2,0,Buffer,NULL}
      };

struct TmtWin F = { 3,10,76,16, Buffer2};

l=strlen(erreur);

x=((Cfg->TailleX)-l)/2;                                 // 1-> 39, 2->39
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
|-   Avancement de graduation                                         -|
|-   Renvoit le prochain                                              -|
\*--------------------------------------------------------------------*/
int Gradue(int x,int y,int length,int from,int to,int total)
{
short j1,j2;
int j3;

if (total==0) return 0;

if ( (to>1024) & (total>1024) )
    {
    j3=(to/1024);
    j3=(j3*length*8)/(total/1024);
    }
    else
    j3=(to*length*8)/total;

if (j3>=(length*8)) j3=(length*8)-1;

for (j1=from;j1<j3;j1++)
    {
    j2=j1/8;
    if (Cfg->UseFont==0)
        switch(j1%8)
            {
            case 0:
                AffChr(j2+x,y,'*');
                break;
            case 1:
            case 2:
            case 3:
            case 4:
                AffChr(j2+x,y,'*');
                AffChr(j2+x+1,y,32);
                break;
            case 5:
            case 6:
            case 7:
                AffChr(j2+x,y,32);
                AffChr(j2+x+1,y,'*');
                break;
            }
    else
        switch(j1%8)
            {
            case 0:
                AffChr(j2+x,y,156);
                break;
            case 1:
                AffChr(j2+x,y,157);
                AffChr(j2+x+1,y,32);
                break;
            case 2:
                AffChr(j2+x,y,158);
                AffChr(j2+x+1,y,163);
                break;
            case 3:
                AffChr(j2+x,y,159);
                AffChr(j2+x+1,y,164);
                break;
            case 4:
                AffChr(j2+x,y,160);
                AffChr(j2+x+1,y,165);
                break;
            case 5:
                AffChr(j2+x,y,161);
                AffChr(j2+x+1,y,166);
                break;
            case 6:
                AffChr(j2+x,y,162);
                AffChr(j2+x+1,y,167);
                break;
            case 7:
                AffChr(j2+x,y,32);
                AffChr(j2+x+1,y,155);
                break;
            }
    }

if (to==total)
    ChrLin(x,y,length+1,32);

if (to==0)
    if (Cfg->UseFont==0)
    AffChr(x,y,'*');
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
strcpy(Mask[0]->chaine,      "asm break case cdecl char const continue "
                         "default do double else enum extern far float "
                             "for goto huge if int interrupt long near "
                           "pascal register short signed sizeof static "
                            "struct switch typedef union unsigned void "
                                                    "volatile while @");
Mask[0]->Ignore_Case=0;
Mask[0]->Other_Col=1;

strcpy(Mask[1]->title,"Pascal Style");
strcpy(Mask[1]->chaine,    "absolute and array begin case const div do "
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
strcpy(Mask[15]->chaine,      "ketchup killers redbug access dark angel "
                   "marjorie katana ecstasy cray magic fred cobra z @");
Mask[15]->Ignore_Case=1;
Mask[15]->Other_Col=1;

strcpy(Cfg->extens,"RAR ARJ ZIP LHA DIZ EXE COM BAT BTM");

Cfg->wmask=15;                                      // RedBug preference

Cfg->TailleY=30;
Cfg->font=1;
Cfg->AnsiSpeed=133;
Cfg->SaveSpeed=7200;

Cfg->fentype=4;

Cfg->mtrash=100000;

Cfg->currentdir=1;
Cfg->overflow1=0;
Cfg->overflow2=0;

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

Cfg->hidfil=1;

Cfg->comport=2;
Cfg->comspeed=19200;
Cfg->combit=8;
Cfg->comparity='N';
Cfg->comstop=1;

Cfg->enterkkd=1;

Cfg->warp=1;

Cfg->cnvhist=1;
Cfg->esttime=1;

Cfg->ajustview=1;

Cfg->editeur[0]=0;
Cfg->vieweur[0]=0;

Cfg->Esc2Close=0;

strcpy(Cfg->ExtTxt,"ASM BAS C CPP DIZ DOC H HLP HTM INI LOG NFO PAS TXT");
Cfg->Enable_Txt=1;
strcpy(Cfg->ExtBmp,"BMP GIF ICO JPG LBM PCX PIC PKM PNG RAW TGA TIF WMF WPG");
Cfg->Enable_Bmp=1;
strcpy(Cfg->ExtSnd,"IT IFF MID MOD MTM S3M VOC WAV XM RTM");
Cfg->Enable_Snd=1;
strcpy(Cfg->ExtArc,"ARJ LHA RAR ZIP KKD");
Cfg->Enable_Arc=1;
strcpy(Cfg->ExtExe,"BAT BTM COM EXE PRG");
Cfg->Enable_Exe=1;
strcpy(Cfg->ExtUsr,"XYZ");
Cfg->Enable_Usr=1;

strcpy(Cfg->HistDir,"C:\\");

strcpy(Cfg->HistCom,"!.!");
}

/*--------------------------------------------------------------------*\
|-                      Error and Signal Handler                      -|
|*--------------------------------------------------------------------*|
|-   Return IOerr si IOerr = 1 ou 3                                   -|
|-   Return     3 si IOver = 1                                        -|
\*--------------------------------------------------------------------*/

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

switch((deverr&1536)/512)
    {
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

if (erreur[0]) PrintAt(25,14,"Ignore"),AffCol(25,14,10*16+5),
                                                WinCadre(24,13,31,15,2);
if (erreur[1]) PrintAt(38,14,"Retry"),AffCol(38,14,10*16+5),
                                                WinCadre(37,13,43,15,2);
if (erreur[2]) PrintAt(51,14,"Fail"),AffCol(51,14,10*16+5),
                                                WinCadre(50,13,55,15,2);

IOerr=0;
do
{
car=getch();

if ( (car=='I') | (car=='i') & (erreur[0]) ) IOerr=1;
if ( (car=='R') | (car=='r') & (erreur[1]) ) IOerr=2;
if ( (car=='F') | (car=='f') & (erreur[2]) ) IOerr=3;

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

/*--------------------------------------------------------------------*\
|-  Retourne 0 si tout va bene                                        -|
\*--------------------------------------------------------------------*/
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

void InitSeg(void)
{
int n;

for(n=0;n<50;n++)
    scrseg[n]=(char*)(0xB8000+n*(Cfg->TailleX)*2);

}

int InitScreen(int a)
{
char buf[31];
int nr;

// --- Console ---------------------------------------------------------

if ((Cfg->TailleX==0) | (Cfg->TailleY==0))
    {
    Cfg->TailleX=80;
    Cfg->TailleY=25;
    }

InitSeg();

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

/*--------------------------------------------------------------------*\
|-     Gestion de la barre de menu                                    -|
|-   Renvoie 0 pour ESC                                               -|
|-   Sinon numero du titre;                                           -|
|-   xp: au depart, c'est le numero du titre                          -|
|-       a l'arrivee ,c'est la position du titre                      -|
\*--------------------------------------------------------------------*/
int BarMenu(struct barmenu *bar,int nbr,int *poscur,int *xp,int *yp)
{
int c,i,j,n,x;
char let[32];
int car=0;

for (n=0;n<nbr;n++)
    let[n]=toupper(bar[n].titre[0]);

ColLin(0,0,Cfg->TailleX,1*16+7);
ChrLin(0,0,Cfg->TailleX,32);


x=0;
for(n=0;n<nbr;n++)
    x+=strlen(bar[n].titre);

i=((Cfg->TailleX)-x)/nbr;
x=((Cfg->TailleX)-(nbr-1)*i-x)/2;

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

if (car==0)
    {
    int xm,ym,button;

    xm=MousePosX();
    ym=MousePosY();

    button=MouseButton();

    if ((button&4)==4) car=13;

    if ((button&2)==2) car=27;

    if ((button&1)==1)
        {
        if (ym!=0)
            car=13;
            else
            {
            j=0;
            for (n=0;n<nbr;n++)
                {
                if (xm>=x+j+n*i)
                    c=n;
                j+=strlen(bar[n].titre);
                }
            }
        }


    }


switch(HI(car))
    {
    case 0x4B:      //--- Gauche ---------------------------------------
        c--;
        break;
    case 0x4D:      //--- Droite ---------------------------------------
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


/*--------------------------------------------------------------------*\
|-  1: [RIGHT]   -1: [LEFT]                                           -|
|-  0: [ESC]      2: [ENTER]                                          -|
\*--------------------------------------------------------------------*/
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
if ((*c)<0)   (*c)=0;
if ((*c)>=nbr) (*c)=nbr-1;

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

if (car==0)
    {
    int xm,ym,button;

    ym=MousePosY();
    xm=MousePosX();

    button=MouseButton();

    if ((button&4)==4) car=13;

    if ((button&2)==2) car=27;

    if ((button&1)==1)
        {
        if ((*c)>ym-(*yp))
            car=0x4800;
        if ((*c)<ym-(*yp))
            car=0x5000;

        if (xm<(*xp))
            car=0x4B00;
        if (xm>(*xp+max-1))
            car=0x4D00;
        }

    _zmok=1;                                     // On relache le bouton
    }

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


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|- Gestion de l'aide                                                  -|
\*--------------------------------------------------------------------*/



/*--------------------------------------------------------------------*\
|- prototype                                                          -|
\*--------------------------------------------------------------------*/

void MainTopic(void);
void SubTopic(long z);
void Page(long z);
void Hlp2Chaine(long pos,char *chaine);


/*--------------------------------------------------------------------*\
|- variable interne                                                   -|
\*--------------------------------------------------------------------*/

static long NdxMainTopic[16];
static short NbrMain;
static long NdxSubTopic[16][512];
static short NbrSub[16];

static long lng;
char *hlp;

/*--------------------------------------------------------------------*\
|- Lit la ligne suivant jusque … la fin                               -|
\*--------------------------------------------------------------------*/
void Hlp2Chaine(long pos,char *chaine)
{
long n;

n=0;

while ( (hlp[pos+n]!=0x0A) & (hlp[pos+n]!=0x0D) )

    {
    chaine[n]=hlp[pos+n];
    n++;
    }
chaine[n]=0;
}

/*--------------------------------------------------------------------*\
|- Appelle l'aide en g‚n‚ral                                          -|
\*--------------------------------------------------------------------*/
void Help(void)
{
FILE *fic;

long n;

short i,j;

NbrMain=0;

for(i=0;i<16;i++)
    {
    NdxMainTopic[i]=0;
    NbrSub[i]=0;
    for(j=0;j<512;j++)
        NdxSubTopic[i][j]=0;
    }

fic=fopen(Fics->help,"rb");
lng=filelength(fileno(fic));
hlp=GetMem(lng);
fread(hlp,1,lng,fic);
fclose(fic);

/*--------------------------------------------------------------------*\
|-  Creation de l'index                                               -|
\*--------------------------------------------------------------------*/

n=0;
while(n<lng)
    {
    switch(hlp[n])
        {
        case '@':
            NdxMainTopic[NbrMain]=n+1;
            NbrMain++;
            break;
        case ':':
            NdxSubTopic[NbrMain-1][NbrSub[NbrMain-1]]=n+1;
            NbrSub[NbrMain-1]++;
            break;
        }
    while (hlp[n]!=0x0A)
        n++;
    n++;
    }

/*--------------------------------------------------------------------*\
|-  Tri de l'index                                                    -|
\*--------------------------------------------------------------------*/

/* A faire */

MainTopic();

LibMem(hlp);
}

/*--------------------------------------------------------------------*\
|- Aide sur un topic en particulier                                   -|
\*--------------------------------------------------------------------*/
void HelpTopic(char *topic)
{
FILE *fic;
char buffer[80];

long n;

fic=fopen(Fics->help,"rb");
lng=filelength(fileno(fic));
hlp=GetMem(lng);
fread(hlp,1,lng,fic);
fclose(fic);

n=0;
while(n<lng)
    {
    switch(hlp[n])
        {
        case '#':
            Hlp2Chaine(n+1,buffer);
            if (!strcmp(buffer,topic))
                Page(n);
            break;
        }
    while (hlp[n]!=0x0A) n++;
    n++;
    }

LibMem(hlp);
}

/*--------------------------------------------------------------------*\
|- Affichage du menu principal                                        -|
\*--------------------------------------------------------------------*/
void MainTopic(void)
{
short x1,y1,x2,y2,max,n,pos;

char chaine[256];
char car,car2;
int c;



max=10;                                        // Lenght of "Main Topic"
for (n=0;n<NbrMain;n++)
    {
    Hlp2Chaine(NdxMainTopic[n],chaine);
    if (max<strlen(chaine))
        max=strlen(chaine);
    }


x1=5;
y1=3;

x2=x1+max+3;
y2=y1+(NbrMain+1)*3;

SaveEcran();

WinCadre(x1,y1,x2,y2,0);

Window(x1+1,y1+1,x2-1,y2-1,10*16+1);

PrintAt(x1+2,y1,"Main Topic");
ColLin(x1+2,y1,10,10*16+2);

pos=0;

do
    {
    for (n=0;n<NbrMain;n++)
        {
        Hlp2Chaine(NdxMainTopic[n],chaine);
        if (pos==n)
            ColLin(x1+2,y1+3+n*3,strlen(chaine),1*16+5);
            else
            ColLin(x1+2,y1+3+n*3,strlen(chaine),10*16+1);

        PrintAt(x1+2,y1+3+n*3,chaine);
        }

    c=Wait(0,0,0);

    if (c==0)     //--- Pression bouton souris ---------------------------
        {
        int button;

        button=MouseButton();

        if ((button&1)==1)     //--- gauche --------------------------------
            {
            int y;

            y=MousePosY();

            if (y>(y1+3+pos*3))
                c=80*256;
            if (y<(y1+3+pos*3))
                c=72*256;
            }
        if ((button&2)==2)     //--- droite ---------------------------
            c=27;

        if ((button&4)==4)
            c=13;
        }

    car=LO(c);
    car2=HI(c);

    switch(car2)
        {
        case 0x47:
            pos=0;
            break;
        case 0x4F:
            pos=NbrMain-1;
            break;
        case 72:
            pos--;
            if (pos==-1) pos=0;
            break;
        case 80:
            pos++;
            if (pos==NbrMain) pos--;
            break;
        }
    switch(car) {
        case 13:
            Hlp2Chaine(NdxMainTopic[pos],chaine);
            ColLin(x1+2,y1+3+pos*3,strlen(chaine),10*16+1);
            SubTopic(pos);
            break;
        }

    }
while ( (c!=27) & (c!=0x8D00) );

ChargeEcran();

}

/*--------------------------------------------------------------------*\
|- Affichage du menu secondaire                                       -|
\*--------------------------------------------------------------------*/
void SubTopic(long z)
{
char chaine[256];
char car,car2;
int c,x1,x2,y1,y2,max;
short n,dernier,pos,prem;

Hlp2Chaine(NdxMainTopic[z],chaine);
max=strlen(chaine);

for (n=0;n<NbrSub[z];n++)
    {
    Hlp2Chaine(NdxSubTopic[z][n],chaine);
    if (max<strlen(chaine))
        max=strlen(chaine);
    }


x1=(82-max)/2;
y1=((Cfg->TailleY-1)-2*NbrSub[z])/2;

x2=x1+max+3;
y2=y1+(NbrSub[z]+1)*2;

if (y1<0) y1=2;
if (y2>Cfg->TailleY) y2=Cfg->TailleY-3;

SaveEcran();

WinCadre(x1,y1,x2,y2,0);

Window(x1+1,y1+1,x2-1,y2-1,10*16+1);

Hlp2Chaine(NdxMainTopic[z],chaine);
PrintAt(x1+2,y1,"%s",chaine);
ColLin(x1+2,y1,strlen(chaine),10*16+2);

pos=0;
prem=0;

do
    {
    for (n=prem;n<NbrSub[z];n++)
        {
        Hlp2Chaine(NdxSubTopic[z][n],chaine);
        if (pos==n)
            ColLin(x1+2,y1+3+(n-prem)*2,max,1*16+5);
            else
            ColLin(x1+2,y1+3+(n-prem)*2,max,10*16+1);

        PrintAt(x1+2,y1+3+(n-prem)*2,"%-*s",max,chaine);

        if (y1+3+(n-prem+1)*2>=y2)
            {
            dernier=n;
            break;
            }
        }

    c=Wait(0,0,0);

    if (c==0)     //--- Pression bouton souris ---------------------------
        {
        int button;

        button=MouseButton();

        if ((button&1)==1)     //--- gauche --------------------------------
            {
            int y;

            y=MousePosY();

            if (y>(y1+3+(pos-prem)*2))
                c=80*256;
            if (y<(y1+3+(pos-prem)*2))
                c=72*256;
            }

        if ((button&2)==2)     //--- droite ---------------------------
            c=27;

        if ((button&4)==4)
            c=13;
        }

    car=LO(c);
    car2=HI(c);

    switch(car2)
        {
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
            if (pos==NbrSub[z]) pos--;
            if (pos>dernier) prem++;
            break;
        }
    switch(car) {
        case 13:
            Page(NdxSubTopic[z][pos]);
            break;
        }

    }
while ( (c!=27) & (c!=0x8D00) );

ChargeEcran();

}

#define SPACE ' '


/*--------------------------------------------------------------------*\
|- Affichage d'une page d'aide                                        -|
\*--------------------------------------------------------------------*/
void Page(long z)
{
char car,car2;
unsigned int c;

long n;

int nbrkey;

short x,y;

char type;                   //--- 1: Centre & highlighted -------------
                             //--- 2: Highlighted ----------------------
                             //--- 3: Marqueur pour topic aide ---------

char chaine[256];

long avant,apres,pres;

SaveEcran();
PutCur(32,0);

WinCadre(0,0,(Cfg->TailleX)-1,(Cfg->TailleY)-2,1);
Window(1,1,78,(Cfg->TailleY)-3,10*16+1);

pres=z;

nbrkey=0;

do
{
n=pres;
avant=pres;
apres=pres;

y=0;

while(hlp[n]!=0x0A) n++;
n++;

while(1)
    {
    switch(hlp[n])                                  // Premier caractere
        {
        case '^':
            type=1;
            break;
        case '%':
            type=2;
            break;
        case '#':
            type=3; //--- marqueur -------------------------------------
            break;
        case 9:
        case 10:
        case 13:
        case 32:
            type=0;
            break;

        default:
            type=69;
            break;
        }

    if (type==69)                       // Autre type -> fin d'affichage
        break;

    n++;
    if (n>lng) break;         // Depassement de ligne -> fin d'affichage

    if (type!=3)
        {
        y++;
        x=1;

        if (hlp[n-1]==9)
            while(x!=8)
                {
                AffChr(x,y,SPACE);
                x++;
                }
        if (type==1)
            {
            Hlp2Chaine(n,chaine);
            while (x!=(78-strlen(chaine))/2+1)
                {
                AffChr(x,y,SPACE);
                x++;
                }
            }

        while(hlp[n]!=0x0A)
            {
            if (n>lng) break;           // Autre type -> fin d'affichage
            switch(hlp[n])
                {
                case 0x09:
                    do
                       {
                       AffChr(x,y,SPACE);
                       x++;
                       }
                    while ((x&7)!=0);
                    break;
                case 0x0D:
                    break;
                default:
                    AffChr(x,y,hlp[n]);
                    x++;
                    break;
                    }
            n++;
            }

        ChrLin(x,y,(Cfg->TailleX)-1-x,SPACE);                 // Efface jusqu'a la fin

        if (type!=0)                              // Couleur de la ligne
            ColLin(1,y,78,10*16+5);
            else
            ColLin(1,y,78,10*16+1);

        n++;

        if (y==Cfg->TailleY-3)
            {
            while(hlp[apres]!=0x0A) apres++;
            apres++;
            break;               // On arrive en bas --> fin d'affichage
            }
        }
        else
        {
        while(hlp[n]!=0x0A) n++;
        n++;
        }
    }

if (kbhit()!=0) nbrkey=0;

if (nbrkey==0)
    {
    c=Wait(0,0,0);

    if (c==0)     //--- Pression bouton souris ---------------------------
        {
        int button;

        button=MouseButton();

        if ((button&1)==1)     //--- gauche ----------------------------
            {
            int y;

            y=MousePosY();

            if (y>(Cfg->TailleY)/2)
                c=80*256;
                else
                c=72*256;
            }
        if ((button&2)==2)     //--- droite ----------------------------
            c=27;
        }

    car=LO(c);
    car2=HI(c);
    }
    else
    {
    nbrkey--;
    c=0;
    }

if (pres!=z)
    {
    avant-=2;
    while(hlp[avant]!=0x0A)
        avant--;

    avant++;
    if (avant<z)
        avant=z;
    }

switch(car2)
    {
    case 80:    // BAS
        pres=apres;
        break;
    case 72:    // HAUT
        pres=avant;
        break;
    case 0x51:  // PAGE DOWN
        pres=apres;
        nbrkey=20;
        car2=80;
        break;
    case 0x49:  // PAGE UP
        pres=avant;
        nbrkey=20;
        car2=72;
        break;
    case 0x47:  // HOME
        if (pres!=avant)
            {
            pres=avant;
            nbrkey=1;
            }
            else
            nbrkey=0;
        break;
    case 0x4F:  // END
        if (pres!=apres)
            {
            pres=apres;
            nbrkey=1;
            }
            else
            nbrkey=0;
        break;
    }
}
while ( (c!=27) & (c!=0x8D00) );

ChargeEcran();
}

/*--------------------------------------------------------------------*\
|- Initialisation des fichiers selon la path                          -|
\*--------------------------------------------------------------------*/
void SetDefaultPath(char *path)
{
strcpy(_IntBuffer,path);

if ( (path[strlen(path)-1]!='\\') &
     (path[strlen(path)-1]!='/') )
        _IntBuffer[strlen(path)]=DEFSLASH,
        _IntBuffer[strlen(path)+1]=0;

Fics->LastDir=GetMem(256);
getcwd(Fics->LastDir,256);

Fics->FicIdfFile=GetMem(256);
strcpy(Fics->FicIdfFile,_IntBuffer);
strcat(Fics->FicIdfFile,"idfext.rb");

Fics->CfgFile=GetMem(256);
strcpy(Fics->CfgFile,_IntBuffer);
strcat(Fics->CfgFile,"kkrb.cfg");

Fics->path=GetMem(256);
strcpy(Fics->path,_IntBuffer);

Fics->help=GetMem(256);
strcpy(Fics->help,_IntBuffer);
strcat(Fics->help,"kkc.hlp");

Fics->temp=GetMem(256);
strcpy(Fics->temp,_IntBuffer);
strcat(Fics->temp,"temp.tmp");

Fics->trash=GetMem(256);
strcpy(Fics->trash,_IntBuffer);
strcat(Fics->trash,"trash");                         // repertoire trash

Fics->log=GetMem(256);
strcpy(Fics->log,_IntBuffer);
strcat(Fics->log,"trash\\logfile");                     // logfile trash
}

/*--------------------------------------------------------------------*\
|- Gestion souris                                                     -|
\*--------------------------------------------------------------------*/


int MousePosX(void)
{
return _xm;
}

int MousePosY(void)
{
return _ym;
}

int MouseButton(void)
{
return _zm;
}

void InitMouse(void)
{
union REGS R;

R.w.ax=0x0000;
int386(0x33,&R,&R);
if (R.w.ax==0) return;

R.w.ax=0x0001;
int386(0x33,&R,&R);

_PasX=4;
_PasY=4;
_TmpClik=3;

_MouseOK=1;
_MPosX=40*_PasX;
_MPosY=(Cfg->TailleY)/2*_PasY;

_charm=0;

_dclik=0;

_zmok=1;        // Bouton relach‚
}


void GetPosMouse(int *x,int *y,int *button)
{
union REGS R;
signed short t;

if (_MouseOK==0)
    {
    (*x)=(*y)=(*button)=0;
    return;
    }

R.w.ax=0x000B;
int386(0x33,&R,&R);

t=R.w.cx;
_MPosX+=t;
t=R.w.dx;
_MPosY+=t;

if (_MPosX<0) _MPosX=0;
if (_MPosX>=((Cfg->TailleX)*_PasX)) _MPosX=(Cfg->TailleX-1)*_PasX;

if (_MPosY<0) _MPosY=0;
if (_MPosY>=((Cfg->TailleY)*_PasY)) _MPosY=(Cfg->TailleY-1)*_PasY;

(*x)=_MPosX/_PasX;
(*y)=_MPosY/_PasY;

R.w.ax=0x0005;
int386(0x33,&R,&R);

_zm=R.w.ax;

if ( ((*x)!=_xm) | ((*y)!=_ym) | (_charm==0) )
    {
    *(scrseg[_ym]+(_xm<<1)+1)=GetCol(_xm,_ym);

    _charm=GetCol((*x),(*y));

    _xm=(*x);
    _ym=(*y);
    }

if (_charm!=0)
    *(scrseg[_ym]+(_xm<<1)+1)=(_charm&15)*16 + (_charm/16);

if ((_zm&1)==1)
    {
    _mclock=clock();
    if ( (_dclik!=0) & (_dclik!=_TmpClik) )
        {
        _zm=4;
        }
    _dclik=_TmpClik;
    }
    else
    {
    if (_dclik!=0)
        {
        if (clock()!=_mclock)
            {
            _dclik--;
            _mclock=clock();
            }
        }
    }


if (_zm==0) _zmok=1;                    // On debloque si touche relache

if (_zmok==0) _zm=0;         // Touche est relache si pas encore relache


(*button)=_zm;

}

#ifdef DEBUG
void Debug(char *string,...)
{
char sortie[256];
va_list arglist;
FILE *fic;

char *suite;

suite=sortie;

va_start(arglist,string);
vsprintf(sortie,string,arglist);
va_end(arglist);

fic=fopen("c:\\debug","at");
fprintf(fic,"%s",suite);
fclose(fic);
}
#endif
