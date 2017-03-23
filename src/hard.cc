/*--------------------------------------------------------------------*\
|- Hard-function													  -|
\*--------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifdef LINUX
#include <unistd.h>
#else
#include <malloc.h>
#endif

#include "hard.h"

#include "font8x16.h"
#include "font8x8.h"

#ifndef __WC32__
#include <time.h>
#endif

void (*AffChr)(long x,long y,long c);
void (*AffCol)(long x,long y,long c);
long (*Wait)(long x,long y);
int  (*KbHit)(void);
void (*GotoXY)(long x,long y);
void (*WhereXY)(long *x,long *y);
void (*Window)(long left,long top,long right,long bottom,long color);
void (*Clr)(void);
int  (*SetMode)(void);
void (*Cadre)(int x1,int y1,int x2,int y2,int type,int col1,int col2);

#ifdef CURSES
void Curses_AffChr(long x,long y,long c);
void Curses_AffCol(long x,long y,long c);
void Curses_GotoXY(long x,long y);
long Curses_Wait(long x,long y);
int Curses_SetMode(void);
char Curses_KbHit(void);
#endif

#if defined(__WC32__) | defined(DJGPP)
	#include <conio.h>
	#include <mem.h>
	#include <direct.h>
	#include <dos.h>
	#include <bios.h>
	#include <io.h>
	#include <conio.h>
#else
	#define cprintf printf
#endif

#ifdef USEPTC
int ptc_system(int command,char *buffer);  //4
#endif

#ifdef USEVESA
int vesa_system(int command,char *buffer);	  //7
#endif

#ifndef NOANSI
int ansi_system(int command,char *buffer);	   //1
#endif

#ifdef CURSES
int curses_system(int command,char *buffer);   //5
#endif

#ifndef COM
int com_system(int command,char *buffer);	   //2
#endif

#ifndef NODIRECTVIDEO
int video_system(int command,char *buffer);   //3
#endif

#ifdef PCDISPLAY
int Pc_system(int command,char *buffer);   //6
#endif

int cache_system(int command,char *buffer); 	//0

/*
#ifdef __WC32__

#include <i86.h>

void outportb(unsigned short,unsigned char);
#pragma aux outportb parm [dx] [al] = "out dx,al"

void outportw(unsigned short,unsigned short);
#pragma aux outportw parm [dx] [ax] = "out dx,ax"

unsigned char inportb(unsigned short);
#pragma aux inportb parm [dx] = "in al,dx" value[al]

unsigned short inportw(unsigned short);
#pragma aux inportw parm [dx] = "in ax,dx" value[ax]

#define outpw(_valx,_valy)	 outportw(_valx,_valy)
#define outp(_valx,_valy)	 outportb(_valx,_valy)

#define inpw(_valy)   inportw(_valy)
#define inp(_valy)	  inportb(_valy)

#endif
*/

#define PRINTF printf

/*--------------------------------------------------------------------*\
|- variable globale 												  -|
\*--------------------------------------------------------------------*/

int IOver;
int IOerr;

struct config *Cfg;
struct fichier *Fics;

char _RB_screen[256*128*2];

static int _numhelp;

/*--------------------------------------------------------------------*\
|- Variable locale													  -|
\*--------------------------------------------------------------------*/

#ifndef NOMOUSE
static char _MouseOK=0;
static signed long _TmpClik;
static signed long _xm,_ym,_zm,_zmok;
static int _dclik,_mclock;
static char _charm;
#endif

static int _xw,_yw; 			   // position up left in window -------
static int _xw2,_yw2;			   // position bottom right in window --

static char _IntBuffer[256];		   // Buffer interne multi usage ---

static int _KeyBuf[32]; 			   // Buffer pour les touches ------
static int _NbrKey=0;				   // Nbr de touches dans KeyBuf ---

char *scrseg[50];


void InitSeg(void);

int Switch(int x,int y,int *Val,int len);
int MSwitch(int x,int y,int *Val,int len,int i);


/*--------------------------------------------------------------------*\
|- Fonction interne 												  -|
\*--------------------------------------------------------------------*/
void Font8xFile(int height,char *path);

void SetScreenSizeX(int x);
void SetScreenSizeY(int y);

#ifdef __WC32__
int __far Error_handler(unsigned, unsigned, unsigned far *);
#endif

void Buffer_Clr(void)
{
memset(_RB_screen+256*128,0,256*128);
memset(_RB_screen,0,256*128);
}

/*--------------------------------------------------------------------*\
|-	   Fonction qui convertit les caracteres ASCII en RedBug one	  -|
\*--------------------------------------------------------------------*/
char CnvASCII(char table,char car)
{
char _Tab1[]={
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	 16, 17, 18, 19, 80, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 83, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	 79, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 67, 61, 62, 63,
	 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	 96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
	 67, 85, 69, 65, 65, 65, 65, 67, 69, 69, 69, 73, 73, 73, 65, 65,
	 69, 65, 65, 79, 79, 79, 85, 85, 89, 79, 85, 67, 76, 89, 80, 70,
	 65, 73, 79, 85, 78, 78, 65, 79, 67,169,170,171,172, 73,174,175,
	176,177,178,179,180,181,'H',183,184,185,186,187,188,189,190,191,
	'L',193,'T',195,196,197,198,'H',200,201,202,203,204,205,206,207,
	208,209, 78,211,212,213,214,'H',216,217,218,219,220,221,222,223,
	 65, 66,226, 78, 69, 79, 85, 84,232, 79, 78, 68,236, 79, 69, 78,
	240,241,242,243,244,245,246,247, 79, 46, 46, 86, 78, 50,254, 32};


char _Tab2[]={
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
	112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
	199,252,233,226,228,224,229,231,234,235,232,239,238,236,196,197,
	201,230,198,244,246,242,251,249,255,214,220,248,163,216,215,159,
	225,237,243,250,241,209,170,186,191,174,172,189,188,161,171,187,
	176,177,178,179,180,193,194,192,169,185,186,187,188,162,165,191,
	192,193,194,195,196,197,227,195,200,201,202,203,204,205,206,164,
	240,208,202,203,200,213,205,206,207,217,218,219,220,166,204,223,
	211,223,212,210,245,213,181,222,254,218,219,217,253,221,175,180,
	173,177,242,190,182,167,247,184,176,168,250,185,179,178,183,160};


switch(table)
	{
	case 1:
		return _Tab1[car];
	case 2:
		return _Tab2[car];
	}

if (Cfg->font==0) return car;

switch(car)
	{
	case 0:
		return 32;

	case 'Š':
		return 232;
	case '‚':
		return 233;
	case 'ˆ':
		return 234;
	case '‰':
		return 235;

	case '—':
		return 'u';
	case '£':
		return 'u';
	case '–':
		return 'u';
	case '':
		return 'u';

	case '':
		return 'i';
	case '¡':
		return 'i';
	case 'Œ':
		return 'i';
	case '‹':
		return 'i';

	case '•':
		return 'o';
	case '¢':
		return 'o';
	case '“':
		return 'o';
	case '”':
		return 'o';
	case 153:
		return 'O';


	case '…':
		return 224;
	case ' ':
		return 225;
	case 'ƒ':
		return 226;
	case '„':
		return 227;

	case '‡':
		return 231;

	case 0xFA:
		return 7;

	case 0x90:
		return 'E';

	default:
		return car;
	}
}

#ifdef LINUX

unsigned _bios_keybrd(unsigned cmd)  // UTILISE pour voir si shift est pressé
{
return 0;
}

char kbhit(void)
{
return Curses_KbHit();
}


#define cprintf printf

int filelength(int fic)
{
        return 1;
}

int strnicmp(char *s1,char *s2,int c)
{
char ns1[255], ns2[255];
strcpy(ns1,s1);
strcpy(ns2,s2);
strupr(ns1);
strupr(ns2);
return(strncmp(ns1,ns2,c));
}

int stricmp(char *s1, char *s2)
{
char ns1[255], ns2[255];
strcpy(ns1,s1);
strcpy(ns2,s2);
strupr(ns1);
strupr(ns2);
return(strcmp(ns1,ns2));
}

char *strlwr(char *a)
{
}

char *strupr(char *s)
{
int i;
for(i=0;s[i]!=0;i++) if((s[i]>96)&&(s[i]<123)) s[i]-=32;
return(s);
}



#endif


/*--------------------------------------------------------------------*\
|-	Fonction interne d'affichage                                      -|
\*--------------------------------------------------------------------*/
void Norm_Clr(void);
void Norm_Window(long left,long top,long right,long bottom,long color);
int Norm_KbHit(void);

void Norm_Clr(void)
{
char x,y;

for(y=0;y<Cfg->TailleY;y++)
	for (x=0;x<Cfg->TailleX;x++)
	  AffCol(x,y,7);

for(y=0;y<Cfg->TailleY;y++)
	for (x=0;x<Cfg->TailleX;x++)
	  AffChr(x,y,32);

Wait(0,0);
}

void Norm_Window(long left,long top,long right,long bottom,long color)
{
int x,y;

_xw=left;
_yw=top;

_xw2=right;
_yw2=bottom;

for(y=top;y<=bottom;y++)
	for (x=left;x<=right;x++)
		AffCol(x,y,color);


for(y=top;y<=bottom;y++)
	for (x=left;x<=right;x++)
		AffChr(x,y,32);
}

int Norm_KbHit(void)
{
if (_NbrKey==0)
	return kbhit();
return 1;
}


long Cache_Wait(long x,long y);
int Cache_SetMode(void);
int Cache_KbHit(void);
void Cache_AffChr(long x,long y,long c);
void Cache_AffCol(long x,long y,long c);
void Cache_GotoXY(long x,long y);
void Cache_WhereXY(long *x,long *y);



#ifndef NOINT10
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

/*--------------------------------------------------------------------*\
|- Changement de mode texte 										  -|
\*--------------------------------------------------------------------*/

//--- Prototype --------------------------------------------------------

void Mode25(void);
void Mode50(void);
void Mode30(void);
void Mode90(void);
void Mode80(void);


//--- Fonction ---------------------------------------------------------

void Mode25(void)
{
union REGS R;

R.w.ax=3;
int386(0x10,&R,&R);
}

void Mode50(void)
{
union REGS R;

R.w.ax=3;
int386(0x10,&R,&R);

R.w.bx=0;
R.w.ax=0x1112;
int386(0x10,&R,&R);
}

void Mode30(void)
{
union REGS R;
uchar t;

R.w.ax=3;
int386(0x10,&R,&R);

R.w.ax=0x1114;
R.w.bx=0;
int386(0x10,&R,&R);

t=(uchar)inp(0x3CC);
outp(0x3C2,(uchar)(t|192));
outp(0x3D4,0x11);
t=(uchar)((t&112)|12);
outp(0x3D5,t);
outpw(0x3D4,0xB06);
outpw(0x3D4,0x3E07);
outpw(0x3D4,0xEA10);
outpw(0x3D4,0xDF12);
outpw(0x3D4,0xE715);
outpw(0x3D4,0x416);

outp(0x3D4,0x11);
outp(0x3D5,t);
}


void Mode90(void)
{
int x;

outpw(0x3C4,0x100); 								// Synchronous reset
outpw(0x3C4,0x101); 									// 8 pixels/char

x=inp(0x3CC);
x=(x&0xF3)|4;						   // mets les bits 2-3 … 01 = 28MhZ
outp(0x3C2,(char)x);

x=inp(0x3DA);
outp(0x3C0,0x13);								   // Horizontal panning

outp(0x3C0,0);											  // set shift=0

outp(0x3C0,0x20);									   // Restart screen
outp(0x3C0,0x20);

outp(0x3D4,0x11);									 // Register protect

x=inp(0x3D5);
x=x&0X7F;
outp(0x3D5,(char)x);								 // Turn off protect

outpw(0x3D4,0x6B00);								 // Horizontal Total
outpw(0x3D4,0x5901);							 // Horizontal Displayed
outpw(0x3D4,0x5A02);							 // Start Horiz Blanking
outpw(0x3D4,0x8E03);							   // End Horiz Blanking
outpw(0x3D4,0x6004);							  // Start Horiz Retrace
outpw(0x3D4,0x8D05);								// End Horiz Retrace
outpw(0x3D4,0x2D13);								// Memory Allocation

outpw(0x3D4,0x9311);								  // Turn on protect

outpw(0x3C4,0x300); 								// Restart Sequencer
}

void Mode80(void)
{

}

void LoadPal(void)
{
union REGS regs;
int n;

Buf2Pal(Cfg->palette);

regs.w.ax=0x1003;					// Donne la palette n … la couleur n
regs.w.bx=0;
int386(0x10,&regs,&regs);

for (n=0;n<16;n++)
	{
	regs.h.bh=(char)n;
	regs.h.bl=(char)n;
	regs.w.ax=0x1000;
	int386(0x10,&regs,&regs);
	}
}

void SetPal(int x,char r,char g,char b)
{
outp(0x3C8,(char)x);
outp(0x3C9,r);
outp(0x3C9,g);
outp(0x3C9,b);
}

void GetPal(int x,char *r,char *g,char *b)
{
union REGS R;

R.w.ax=0x1015;
R.h.bl=(char)x;

int386(0x10,&R,&R);
(*r)=R.h.dh;
(*g)=R.h.ch;
(*b)=R.h.cl;

/*
outp(0x3C7,x);
(*r)=inp(0x3C9);
(*g)=inp(0x3C9);
(*b)=inp(0x3C9);
*/
}

#else
/*--------------------------------------------------------------------*\
|- Changement de mode texte 										  -|
\*--------------------------------------------------------------------*/

//--- Prototype --------------------------------------------------------

void Mode25(void);
void Mode50(void);
void Mode30(void);
void Mode90(void);
void Mode80(void);


//--- Fonction ---------------------------------------------------------

void Mode25(void)
{
}

void Mode50(void)
{
}

void Mode30(void)
{
}


void Mode90(void)
{
}

void Mode80(void)
{
}

char _curx,_cury;

void GetCur(char *x,char *y)
{
*x=_curx;
*y=_cury;
}

void PutCur(char x,char y)
{
_curx=x;
_cury=y;
}

void LoadPal(void)
{
}

void SetPal(int x,char r,char g,char b)
{
}

void GetPal(int x,char *r,char *g,char *b)
{
}

#endif


long _cx,_cy;

void Norm_GotoXY(long x,long y)
{
_cx=x;
_cy=y;
}

void Norm_WhereXY(long *x,long *y)
{
*x=_cx;
*y=_cy;
}




void PutKey(int key)
{
if (_NbrKey!=32)
	{
	_KeyBuf[_NbrKey]=key;
	_NbrKey++;
	}
}

int GetKey(void)
{
if (_NbrKey!=0)
	{
	_NbrKey--;
	return _KeyBuf[_NbrKey];
	}
return -1;
}



#ifndef NOCOM

/*--------------------------------------------------------------------*\
|-						 Gestion du port s‚rie						  -|
\*--------------------------------------------------------------------*/
#define XON 1
#define XOFF 0
#define MAX_BUFFER 1024

#define INT_OFF() _disable()
#define INT_ON() _enable()

#define SETVECT _dos_setvect
#define GETVECT _dos_getvect

char *modem_buffer;

long modem_pause;
short modem_base;
long modem_port;
long modem_buffer_head;
long modem_buffer_tail;
long modem_overflow;
long modem_irq;
long modem_open=0;
long modem_xon_xoff=0;
long modem_rts_cts;

long modem_buffer_count;

long old_modem_imr;
long old_modem_ier;

void (_interrupt *old_modem_isr)(void);

/*--------------------------------------------------------------------*/
void interrupt modem_isr(void)
{
int c;

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
		modem_buffer[modem_buffer_head++]=(char)c;
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

long com_carrier(void)
{
long x;

if (!modem_open) return(0);
if ((inp((short)(modem_base+6)) & 0x80)==128) return(1);

for (x=0; x<500; x++)
	{
	if ((inp((short)(modem_base+6)) & 0x80)==128) return(1);
	}
return(0);
}

char com_ch_ready(void)
{
if (!modem_open) return(0);
if (modem_buffer_count!=0) return(1);
return(0);
}

/*--------------------------------------------------------------------*\
|- This will return 0 is there is no character waiting.  Please check -|
|- the port with com_ch_ready(); first so that if they DID send a 0x0 -|
|-	   that you will know it's a true 0, not a no character return!   -|
\*--------------------------------------------------------------------*/
long com_read_ch(void)
{
long ch;

if (!modem_open) return(0);

if (!com_ch_ready()) return(0);

ch=modem_buffer[modem_buffer_tail];
modem_buffer[modem_buffer_tail]=0;
modem_buffer_count--;
if (++modem_buffer_tail>=MAX_BUFFER)
	modem_buffer_tail=0;

return(ch);
}

void com_send_ch(long ch)
{
if (!modem_open) return;

outp((short)(modem_base+4),0x0B);

if (modem_rts_cts)
	{
	while((inp((short)(modem_base+6)) & 0x10)!=0x10) ;	// Wait for Clear to Send
	}
while((inp((short)(modem_base+5)) & 0x20)!=0x20) ;

if (modem_xon_xoff)
	{
	while((modem_pause) && (com_carrier())) ;
	}
outp((short)(modem_base),(char)ch);
}


char com_open(long comport,long speed,long bit,BYTE parity,BYTE stop)
{
long x,  newb=0;
char l, m;
long d;

modem_buffer=(char*)GetMem(MAX_BUFFER);

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

outp((short)(modem_base+1),0x00);			// turn off comm interrupts

if (inp((short)(modem_base+1))!=0)
	{
	INT_ON();
/*	  PrintAt(0,0,"(Toto %X)",modem_base);
	Wait(0,0);*/
	return 0;
	}

/*--------------------------------------------------------------------*\
|-	Set up the Interupt Info										  -|
\*--------------------------------------------------------------------*/
old_modem_ier=inp((short)(modem_base+1));
outp((short)(modem_base+1),0x01);

old_modem_isr=(void (_interrupt *)(void))GETVECT(modem_irq+8);
SETVECT(modem_irq+8,modem_isr);

if (modem_rts_cts)
	{
	outp((short)(modem_base+4),0x0B);
	}
else
	{
	outp((short)(modem_base+4),0x09);
	}

old_modem_imr=inp(0x21);
outp(0x21,(char)(old_modem_imr & ((1 << modem_irq) ^ 0x00FF)));

for (x=1; x<=5; x++)
	inp((short)(modem_base+x));

modem_open=1;

modem_buffer_count=0;
modem_buffer_head=0;
modem_buffer_tail=0;

/*--------------------------------------------------------------------*\
|----- Speed ----------------------------------------------------------|
\*--------------------------------------------------------------------*/

x=inp((short)(modem_base+3));						// Read In Old Stats

if ((x & 0x80)!=0x80) outp((short)(modem_base+3),(char)(x+0x80));

d=(long)(115200/speed);
l=(char)(d & 0xFF);
m=(char)((d >> 8) & 0xFF);

outp((short)(modem_base+0),l);
outp((short)(modem_base+1),m);

outp((short)(modem_base+3),(char)x);			 // Restore the DLAB bit

/*--------------------------------------------------------------------*\
|---- Data-bit --------------------------------------------------------|
\*--------------------------------------------------------------------*/
newb=0;

x=inp((short)(modem_base+3));

newb=(x>>2<<2); 						 // Get rid of the old Data Bits

switch(bit)
	{
	case 5 : newb+=0x00; break;
	case 6 : newb+=0x01; break;
	case 7 : newb+=0x02; break;
	default: newb+=0x03; break;
	}

outp((short)(modem_base+3),(char)(newb));

/*--------------------------------------------------------------------*\
|---- Parity ----------------------------------------------------------|
\*--------------------------------------------------------------------*/
newb=0;

x=inp((short)(modem_base+3));

newb=(x>>6<<6)+(x<<5>>5);						// Get rid of old parity

switch(toupper(parity))
	{
	case 'N':newb+=0x00; break;                                 //  None
	case 'O':newb+=0x08;break;                                  //   Odd
	case 'E':newb+=0x18; break;                                 //  Even
	case 'M':newb+=0x28;break;                                  //  Mark
	case 'S':newb+=0x38;break;                                  // Space
	}

outp((short)(modem_base+3),(char)(newb));

/*--------------------------------------------------------------------*\
|---- Stop bits -------------------------------------------------------|
\*--------------------------------------------------------------------*/
newb=0;

x=inp((short)(modem_base+3));

newb=(x<<6>>6)+(x>>5<<5);					   // Kill the old Stop Bits

if (stop==2) newb+=0x04;		 // Only check for 2, assume 1 otherwise

outp((short)(modem_base+3),(char)(newb));

/*--------------------------------------------------------------------*\
|---- fin de l'initialisation -----------------------------------------|
\*--------------------------------------------------------------------*/

INT_ON();

// PrintAt(0,0,"(Presque %X)",modem_base);
return 1;
}

void com_close(void)
{
if (!modem_open) return;

outp((short)(modem_base+1),(char)(old_modem_ier));
outp(0x21,(char)(old_modem_imr));

SETVECT(modem_irq+8, old_modem_isr);
outp(0x20,0x20);
modem_open=0;

LibMem(modem_buffer);
}
#endif
/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/



void ColLin(long left,long top,long length,long color)
{
long i;

for (i=left;i<left+length;i++)
	  AffCol(i,top,color);
}

void ChrLin(long left,long top,long length,long color)
{
long i;

for (i=left;i<left+length;i++)
	  AffChr(i,top,color);
}

void ChrCol(long left,long top,long length,long color)
{
long i;

for (i=top;i<top+length;i++)
	  AffChr(left,i,color);
}

void ColCol(long left,long top,long length,long color)
{
long i;

for (i=top;i<top+length;i++)
	  AffCol(left,i,color);
}


void ColWin(long left,long top,long right,long bottom,long color)
{
long i,j;

for(j=top;j<=bottom;j++)
	for (i=left;i<=right;i++)
		AffCol(i,j,color);
}

void ChrWin(long left,long top,long right,long bottom,long car)
{
long i,j;

for(j=top;j<=bottom;j++)
	for (i=left;i<=right;i++)
		AffChr(i,j,car);
}


void MoveText(long x1,long y1,long x2,long y2,long x3,long y3)
{
long x,y,z;
char *_MEcran;

_MEcran=(char*)GetMem(Cfg->TailleX*Cfg->TailleY*2);

for (y=0;y<Cfg->TailleY;y++)
	{
	z=(y*Cfg->TailleX)*2;
	for (x=0;x<Cfg->TailleX;x++)
		{
		_MEcran[z]=GetChr(x,y);
		z++;
		_MEcran[z]=GetCol(x,y);
		z++;
		}
	}

for (y=y3;y<=y3+(y2-y1);y++)
	{
	z=(x1+(y-y3+y1)*Cfg->TailleX)*2;
	for (x=x3;x<=x3+(x2-x1);x++)
		{
		AffChr(x,y,_MEcran[z]);
		z++;
		AffCol(x,y,_MEcran[z]);
		z++;
		}
	}

LibMem(_MEcran);
}

/*--------------------------------------------------------------------*\
|-	Routine de sauvegarde de l'ecran                                  -|
\*--------------------------------------------------------------------*/

char *_Ecran[10];						 //--- Copie de l'ecran --------
long _EcranXW[10],_EcranYW[10]; 		 //--- Coordonnes absolues -----
long _EcranXW2[10],_EcranYW2[10];		 //--- Coordonnes absolues -----
signed long _WhichEcran=0;				 //--- Nbr d'‚cran en memoire --


long _EcranX[10],_EcranY[10];			 //--- Position du curseur -----
char _EcranD[10],_EcranF[10];			 //--- Definition du curseur ---
signed long _WhichState=0;				 //--- Nbr d'‚tats en memoire --


void SaveState(void)
{
WhereXY(&(_EcranX[_WhichState]),&(_EcranY[_WhichState]));
GetCur(&(_EcranD[_WhichState]),&(_EcranF[_WhichState]));

_WhichState++;
}

void LoadState(void)
{

_WhichState--;

GotoXY(_EcranX[_WhichState],_EcranY[_WhichState]);
PutCur(_EcranD[_WhichState],_EcranF[_WhichState]);
}


void SaveScreen(void)
{
int x,y,n;

if (_Ecran[_WhichEcran]==NULL)
	_Ecran[_WhichEcran]=(char*)GetMem((Cfg->TailleY)*(Cfg->TailleX)*2);

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

SaveState();

_EcranXW[_WhichEcran]=_xw;
_EcranYW[_WhichEcran]=_yw;

_EcranXW2[_WhichEcran]=_xw2;
_EcranYW2[_WhichEcran]=_yw2;

_WhichEcran++;
}

void LoadScreen(void)
{
int x,y,n;

_WhichEcran--;

#ifdef DEBUG
if ( (_Ecran[_WhichEcran]==NULL) | (_WhichEcran<0) )
	{
	Clr();
	PrintAt(0,0,"Internal Error: LoadScreen");
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

LoadState();

_xw=_EcranXW[_WhichEcran];
_yw=_EcranYW[_WhichEcran];

_xw2=_EcranXW2[_WhichEcran];
_yw2=_EcranYW2[_WhichEcran];

LibMem(_Ecran[_WhichEcran]);
_Ecran[_WhichEcran]=NULL;
}

void LoadScreenPart(int x1,int y1,int x2,int y2)
{
int x,y,n;

_WhichEcran--;

#ifdef DEBUG
if ( (_Ecran[_WhichEcran]==NULL) | (_WhichEcran<0) )
	{
	Clr();
	PrintAt(0,0,"Internal Error: LoadScreen");
	getch();
	return;
	}
#endif

n=0;
for (y=0;y<Cfg->TailleY;y++)
	for (x=0;x<Cfg->TailleX;x++)
		{
		if ((x<x1) | (x>x2) | (y<y1) | (y>y2))
			AffCol(x,y,_Ecran[_WhichEcran][n*2+1]);
		n++;
		}

n=0;
for (y=0;y<Cfg->TailleY;y++)
	for (x=0;x<Cfg->TailleX;x++)
		{
		if ((x<x1) | (x>x2) | (y<y1) | (y>y2))
			AffChr(x,y,_Ecran[_WhichEcran][n*2]);
		n++;
		}

_WhichEcran++;
}

/*--------------------------------------------------------------------*\
|- Fonction relative												  -|
\*--------------------------------------------------------------------*/
void WinRCadre(long x1,long y1,long x2,long y2,long type)
{
WinCadre(x1+_xw,y1+_yw,x2+_xw,y2+_yw,type);
}

void AffRChr(long x,long y,long c)
{
AffChr(x+_xw,y+_yw,c);
}

void AffRCol(long x,long y,long c)
{
AffCol(x+_xw,y+_yw,c);
}

long GetRChr(long x,long y)
{
return GetChr(x+_xw,y+_yw);
}

long GetRCol(long x,long y)
{
return GetCol(x+_xw,y+_yw);
}

void ColRLin(long left,long top,long length,long color)
{
ColLin(left+_xw,top+_yw,length,color);
}

void ChrRLin(long left,long top,long length,long color)
{
ChrLin(left+_xw,top+_yw,length,color);
}

void ChrRCol(long left,long top,long length,long color)
{
ChrCol(left+_xw,top+_yw,length,color);
}

void ColRCol(long left,long top,long length,long color)
{
ColCol(left+_xw,top+_yw,length,color);
}

void ColRWin(long right,long top,long left,long bottom,long color)
{
ColWin(right+_xw,top+_yw,left+_xw,bottom+_yw,color);
}

void ChrRWin(long right,long top,long left,long bottom,long color)
{
ChrWin(right+_xw,top+_yw,left+_xw,bottom+_yw,color);
}

long InputTo(long colonne,long ligne,char *chaine, long longueur)
{
return InputAt(colonne+_xw,ligne+_yw,chaine,longueur);
}

/*--------------------------------------------------------------------*\
|-	Fonction d'impression du texte relatif                            -|
\*--------------------------------------------------------------------*/
void PrintTo(long x,long y,char *string,...)
{
va_list arglist;

char *suite;
long xa,ya;

suite=_IntBuffer;

va_start(arglist,string);
vsprintf(_IntBuffer,string,arglist);
va_end(arglist);

xa=x+_xw;
ya=y+_yw;

while (*suite!=0)
	{
	AffChr(xa,ya,*suite);
	xa++;
	if (xa>_xw2)
		{
		ya++;
		xa=_xw;
		if (ya>_yw2)
			ya=_yw;
		}
	suite++;
	}
}


/*--------------------------------------------------------------------*\
|- Fonction Absolue 												  -|
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-	Fonction d'impression du texte absolu                             -|
\*--------------------------------------------------------------------*/
void PrintAt(long x,long y,char *string,...)
{
va_list arglist;

char *suite;
int a;

suite=_IntBuffer;

va_start(arglist,string);
vsprintf(_IntBuffer,string,arglist);
va_end(arglist);

a=x;
while (*suite!=0)
	{
	AffChr(a,y,CnvASCII(0,*suite));
	a++;
	suite++;
	}
}



/*--------------------------------------------------------------------*\
|-	  Retourne 1 sur ESC											  -|
|-			   0 ENTER												  -|
|-			   2 TAB												  -|
|-			   3 SHIFT-TAB											  -|
\*--------------------------------------------------------------------*/

char InputAt(long colonne,long ligne,char *chaine, long longueur)
{
long car,c2;
char chaine2[255], old[255];
char couleur;
int n, i=0, fin;
int ins=1; //--- insere = 1 par default --------------------------------

char end,retour;

SaveState();

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

do	{

if (ins==0)
	PutCur(7,7);
	else
	PutCur(2,7);

car=Wait(colonne+i,ligne);

#ifndef NOMOUSE
if (car==0)
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
#endif

if ( ((car&255)!=0) & (couleur!=0) & (car!=13) & (car!=27) & (car!=9) )
	{
	ColLin(colonne,ligne,fin,couleur);
	ChrLin(colonne,ligne,fin,32);

	couleur=0;
	fin=0;
	i=0;
	*chaine=0;
	}

switch (car&255)
	{
	case 9:
		retour=2;
		end=1;
		break;
	case 0: 			// v‚rifier si pas de touche de fonction press‚e
		c2=(car/256);
		if (couleur!=0) 		  // Preserve ou pas l'ancienne valeur ?
			{
			if ( (c2==71) | (c2==75) | (c2==77) | (c2==79) )
				{
				ColLin(colonne,ligne,fin,couleur);
				couleur=0;
				}
			if (c2==83)
				{
				ColLin(colonne,ligne,fin,couleur);
				ChrLin(colonne,ligne,fin,32);

				couleur=0;
				fin=0;
				i=0;
				*chaine=0;
				}
			}
		switch (c2)
			{
			case 0x0F:										// SHIFT-TAB
				retour=3;
				end=1;
				break;
			case 71:											 // HOME
				i=0;
				break;
			case 75:											 // LEFT
				if (i>0)
					i--;
				else
					Beep(BEEP_WARNING0);
				break;
			case 77:											// RIGHT
				if (i<fin)
					i++;
				else
					Beep(BEEP_WARNING0);
				break;
			case 79:											  // END
				i=fin;
				break;
			case 13:											// ENTER
				*(chaine+fin)=0;
				break;
			case 72:											   // UP
				retour=3;
				end=1;
				break;
			case 80:											 // DOWN
				retour=2;
				end=1;
				break;
			case 83:											  // del
				if (i!=fin) 		// v‚rifier si pas premiere position
					{
					fin--;
					*(chaine+fin+1)=' ';
					*(chaine+fin+2)='\0';
					strcpy(chaine+i,chaine+i+1);
					PrintAt(colonne+i,ligne,chaine+i);
					}
				else
					Beep(BEEP_WARNING0);
				break;

			case 82:
				ins=(!ins);
				break;

			case 0x3B:	  //---------------------- F1 ------------------
				retour=7;
				end=1;
				break;

			default:
				break;
			}  /* fin du switch */
		break;

		case 8: 							 // v‚rifier si touche [del]
			if (i>0)				// v‚rifier si pas premiere position
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
				Beep(BEEP_WARNING0);
			 break;

		case 13:							 //--- ENTER ---------------
			retour=0;
			end=1;
			break;

		case 27:							  //--- ESCAPE -------------
			if (couleur!=0)
				{
				ColLin(colonne,ligne,fin,couleur);

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
			ChrLin(colonne,ligne,fin,32);

			fin=0;
			i=0;
			*chaine=0;
			break;

		default:					   // v‚rifier si caractŠre correcte
			if ((car>31) && (car<=255))
				{
				if ((i==fin) || (!ins))
					{
					if (i==longueur)
						i--;
					else
					if (i==fin)
						fin++;
					*(chaine+i)=(char)car;
					AffChr(colonne+i,ligne,car);
					i++;
					}						 // fin du if i==fin || !ins
				else
				if (fin<longueur)
					{
					*(chaine+fin)=0;
					strcpy(chaine2,chaine+i);
					strcpy(chaine+i+1,chaine2);
					*(chaine+i)=(char)car;
					PrintAt(colonne+i,ligne,chaine+i);
					fin++;
					i++;
					}
				}									 // fin du if car>31
			else
				Beep(BEEP_WARNING0);
			break;
	}  //--- fin du switch ---------------------------------------------
}
while (!end);

*(chaine+fin)=0;

if (couleur!=0)
	ColLin(colonne,ligne,fin,couleur);

LoadState();
return retour;
}

/*--------------------------------------------------------------------*\
|-							 Screen Saver							  -|
\*--------------------------------------------------------------------*/
void ScreenSaver(void)
{
int xm,ym,zm,xm2,ym2,zm2;
char r,g,b;

#ifndef NOMOUSE
	GetPosMouse(&xm,&ym,&zm);
#endif

GetPal(0,&r,&g,&b);
SetPal(0,0,0,0);

#ifndef NOINT10
inp(0x3DA);
inp(0x3BA);
outp(0x3C0,0);
#endif

#ifndef NOMOUSE
while(!KbHit())
	{
	GetPosMouse(&xm2,&ym2,&zm2);
	if ((xm2!=xm) | (ym2!=ym) | (zm2!=zm))
		break;
	}
#endif

#ifndef NOINT10
inp(0x3DA);
inp(0x3BA);
outp(0x3C0,0x20);
#endif

SetPal(0,r,g,b);

while (KbHit())
	Wait(0,0);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

#ifndef LINUX
void Delay(long ms)
{
clock_k Cl;

Cl=GetClock();
while((GetClock()-Cl)<ms);
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
#else
void Delay(long ms)
{
}

void Pause(int n)
{
}
#endif


// type: 0	   double exterieur
// type: 1	   double interieur
// type: 2	   fin interieur
// type: 3	   fin exterieur


void Norm_Cadre(int x1,int y1,int x2,int y2,int type,int col1,int col2);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/
void Norm_Cadre(int x1,int y1,int x2,int y2,int type,int col1,int col2)
{
switch(Cfg->windesign)
	{
	case 1:
	case 3:
	case 4:
		if ((type==1) | (type==0))
			{
			//--- Relief (surtout pour type==1) --------------------------------
			ColLin(x1,y1,x2-x1+1,col1);
			ColCol(x1,y1+1,y2-y1,col1);

			ColLin(x1+1,y2,x2-x1,col2);
			ColCol(x2,y1+1,y2-y1-1,col2);

			if (Cfg->UseFont==0)
				switch(type)
					{
					case 0:
						AffChr(x1,y1,'Ú');
						AffChr(x2,y1,'¿');
						AffChr(x1,y2,'À');
						AffChr(x2,y2,'Ù');

						ChrLin(x1+1,y1,x2-x1-1,196);
						ChrLin(x1+1,y2,x2-x1-1,196);

						ChrCol(x1,y1+1,y2-y1-1,179);
						ChrCol(x2,y1+1,y2-y1-1,179);
						break;
					case 1:
						AffChr(x1,y1,'É');
						AffChr(x2,y1,'»');
						AffChr(x1,y2,'È');
						AffChr(x2,y2,'¼');

						ChrLin(x1+1,y1,x2-x1-1,205);
						ChrLin(x1+1,y2,x2-x1-1,205);

						ChrCol(x1,y1+1,y2-y1-1,186);
						ChrCol(x2,y1+1,y2-y1-1,186);
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

						ChrLin(x1+1,y1,x2-x1-1,143);
						ChrLin(x1+1,y2,x2-x1-1,148);

						ChrCol(x1,y1+1,y2-y1-1,145);
						ChrCol(x2,y1+1,y2-y1-1,146);
						break;
					case 1:
						AffChr(x1,y1,153);
						AffChr(x2,y1,152);
						AffChr(x1,y2,151);
						AffChr(x2,y2,150);

						ChrLin(x1+1,y1,x2-x1-1,148);
						ChrLin(x1+1,y2,x2-x1-1,143);

						ChrCol(x1,y1+1,y2-y1-1,146);
						ChrCol(x2,y1+1,y2-y1-1,145);
						break;
					}
			}
		else
			{
			//--- Relief (surtout pour type==1) --------------------------------
			ColLin(x1,y1,x2-x1+1,col2);
			ColCol(x1,y1+1,y2-y1,col2);

			ColLin(x1+1,y2,x2-x1,col1);
			ColCol(x2,y1+1,y2-y1-1,col1);

			if (Cfg->UseFont==0)
				switch(type)
					{
					case 2:
					case 3:
						AffChr(x1,y1,'Ú');
						AffChr(x2,y1,'¿');
						AffChr(x1,y2,'À');
						AffChr(x2,y2,'Ù');

						ChrLin(x1+1,y1,x2-x1-1,196);
						ChrLin(x1+1,y2,x2-x1-1,196);

						ChrCol(x1,y1+1,y2-y1-1,179);
						ChrCol(x2,y1+1,y2-y1-1,179);
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

						ChrLin(x1+1,y1,x2-x1-1,134);
						ChrLin(x1+1,y2,x2-x1-1,129);

						ChrCol(x1,y1+1,y2-y1-1,132);
						ChrCol(x2,y1+1,y2-y1-1,131);
						break;
					case 3:
						AffChr(x1,y1,128);
						AffChr(x2,y1,130);
						AffChr(x1,y2,133);
						AffChr(x2,y2,135);

						ChrLin(x1+1,y1,x2-x1-1,129);
						ChrLin(x1+1,y2,x2-x1-1,134);

						ChrCol(x1,y1+1,y2-y1-1,131);
						ChrCol(x2,y1+1,y2-y1-1,132);
						break;
					}
			}
		break;
	case 2:
		ColLin(x1,y1,x2-x1+1,col1);
		ColCol(x1,y1+1,y2-y1,col1);

		ColLin(x1+1,y2,x2-x1,col2);
		ColCol(x2,y1+1,y2-y1-1,col2);

		switch(type)
			{
			case 0:
			case 3:
				AffChr(x1,y1,219);
				AffChr(x2,y1,219);
				AffChr(x1,y2,219);
				AffChr(x2,y2,219);

				ChrLin(x1+1,y1,x2-x1-1,219);
				ChrLin(x1+1,y2,x2-x1-1,219);

				ChrCol(x1,y1+1,y2-y1-1,219);
				ChrCol(x2,y1+1,y2-y1-1,219);
				break;
			case 1:
				AffChr(x1,y1,222);
				AffChr(x2,y1,221);
				AffChr(x1,y2,222);
				AffChr(x2,y2,221);

				ChrLin(x1+1,y1,x2-x1-1,219);
				ChrLin(x1+1,y2,x2-x1-1,219);

				ChrCol(x1,y1+1,y2-y1-1,222);
				ChrCol(x2,y1+1,y2-y1-1,221);
			case 2:
				AffChr(x1,y1,220);
				AffChr(x2,y1,220);
				AffChr(x1,y2,223);
				AffChr(x2,y2,223);

				ChrLin(x1+1,y1,x2-x1-1,220);
				ChrLin(x1+1,y2,x2-x1-1,223);

				ChrCol(x1,y1+1,y2-y1-1,219);
				ChrCol(x2,y1+1,y2-y1-1,219);
				break;
			}
		break;
	}
//PrintAt(x1,y1,"%d",type);
}

/*--------------------------------------------------------------------*\
|-	Make a Window (0: exterieurn, 1: interieur) 					  -|
\*--------------------------------------------------------------------*/
void WinCadre(int x1,int y1,int x2,int y2,int type)
{
int col1,col2;

col1=(type&4)==4 ? Cfg->col[39] : Cfg->col[37];
col2=(type&4)==4 ? Cfg->col[40] : Cfg->col[38];

Cadre(x1,y1,x2,y2,type&3,col1,col2);
}


/*--------------------------------------------------------------------*\
|-	Make a line 													  -|
\*--------------------------------------------------------------------*/

void WinLine(int x1,int y1,int xl,int type)
{
char car;

if (Cfg->UseFont==0)
	{
	car=196;
	}
else
	switch(type)
		{
		case 0:
		case 1:
			car=143;	break;
		case 2:
			car=196;	break;
		}

ChrLin(x1,y1,xl,car);
}

/*--------------------------------------------------------------------*\
|- Gestion des fontes												  -|
\*--------------------------------------------------------------------*/

#ifndef NOFONT
void Buf2Font(char *buffer)
{
int i,j;
char *scr=(char*)0xA0000;

int height;

switch(Cfg->TailleY)
	{
	case 50:
		height=8;
		break;
	case 25:
	case 30:
	default:
		height=16;
		break;
	}

outpw( 0x3C4, 0x402);
outpw( 0x3C4, 0x704);
outpw( 0x3CE, 0x204);

outpw( 0x3CE, 5);
outpw( 0x3CE, 6);

for (i=0;i<256;i++)
	for (j=0;j<16;j++)
		scr[i*32+j]=buffer[i*height+j];

outpw( 0x3C4, 0x302);
outpw( 0x3C4, 0x304);

outpw( 0x3CE, 4);
outpw( 0x3CE, 0x1005);
outpw( 0x3CE, 0xE06);
}

void Font2Buf(char *buffer)
{
int i,j;
char *scr=(char*)0xA0000;

int height;

switch(Cfg->TailleY)
	{
	case 50:
		height=8;
		break;
	case 25:
	case 30:
	default:
		height=16;
		break;
	}

outpw( 0x3C4, 0x402);
outpw( 0x3C4, 0x704);
outpw( 0x3CE, 0x204);

outpw( 0x3CE, 5);
outpw( 0x3CE, 6);

for (i=0;i<256;i++)
	for (j=0;j<16;j++)
		buffer[i*height+j]=scr[i*32+j];

outpw( 0x3C4, 0x302);
outpw( 0x3C4, 0x304);

outpw( 0x3CE, 4);
outpw( 0x3CE, 0x1005);
outpw( 0x3CE, 0xE06);
}
#endif

/*--------------------------------------------------------------------*\
|- Gestion de la palette											  -|
\*--------------------------------------------------------------------*/
void Buf2Pal(char *pal)
{
int n;
for(n=0;n<16;n++)
	SetPal(n,pal[n*3],pal[n*3+1],pal[n*3+2]);
}

void Pal2Buf(char *pal)
{
int n;
for(n=0;n<16;n++)
	GetPal(n,&(pal[n*3]),&(pal[n*3+1]),&(pal[n*3+2]));
}


/*--------------------------------------------------------------------*\
|- Make the font													  -|
\*--------------------------------------------------------------------*/

#ifndef NOFONT
void Font8xFile(int height,char *path)
{
char *pol;

pol=(char*)GetMem(256*height);

strcpy(_IntBuffer,path);
sprintf(_IntBuffer+strlen(_IntBuffer),"\\font8x%d.cfg",height);

Cfg->UseFont=0;

if (Cfg->font)
	{
	FILE *fic;

	fic=fopen(_IntBuffer,"rb");
	if (fic!=NULL)
		{
		Cfg->UseFont=1; 						// utilise les fonts 8x?
		fread(pol,256*height,1,fic);
		fclose(fic);
		}
	}

if (Cfg->UseFont==0)
	{
	if (height==16)
		memcpy(pol,font8x16,256*height);

	if (height==8)
		memcpy(pol,font8x8,256*height);
	}

Buf2Font(pol);

if ((Cfg->TailleX==80) & (Cfg->UseFont))	  // 9 bits normal -> 8 bits
	{
	int x;
	union REGS R;

	x=inp(0x3CC) & (255-12);

	outp(0x3C2,(char)x);
	outpw( 0x3C4, 0x0100);
	outpw( 0x3C4, 0x0101);	// Ou 0x0001
	outpw( 0x3C4, 0x0300);

	R.w.ax=0x1000;			// remet le horizontal PEL panning
	R.h.bl=0x13;
	int386(0x10,&R,&R);
	}

LibMem(pol);
}
#endif

void LibMem(void *mem)
{
free(mem);
}

void *GetMem(int s)
{
void *buf;

if (s==0)
	WinError("Null allocation !");

buf=malloc(s);

if (buf==NULL)
	{
	WinError("Not Enough Memory !");
	exit(1);
	}

memset(buf,0,s);

return buf;
}

void *GetMemSZ(int s)						  // GetMem sans mise … z‚ro
{
void *buf;

if (s==0)
	WinError("Null allocation !");

buf=malloc(s);

if (buf==NULL)
	{
	WinError("Not Enough Memory !");
	exit(1);
	}

return buf;
}


/*--------------------------------------------------------------------*\
|-	 si p vaut 0 mets off											  -|
|-	 si p vaut 2 mets on											  -|
|-	 si p vaut 3 mets default										  -|
|-	 si p vaut 1 interroge											  -|
|-	 retourne -1 si SHIFT TAB, 1 si TAB 							  -|
\*--------------------------------------------------------------------*/
int Puce(int x,int y,int lng,char p)
{
int r=0;

int car;

SaveState();

PutCur(2,7);

if (((Cfg->col[48])&240)==((Cfg->col[19])&240))
	{
	AffChr(x,y,'[');
	AffChr(x+lng-1,y,']');
	}
else
	{
	AffChr(x,y,16);
	AffChr(x+lng-1,y,17);
	AffChr(x+lng,y,220);
	ChrLin(x+1,y+1,lng,223);
	}

ColLin(x,y,lng,Cfg->col[49]);							   // Couleur ON

if (p==1)
	while (r==0)
		{
		car=Wait(x,y);

#ifndef NOMOUSE
		if (car==0)
			{
			int px,py,pz,xm2,ym2;

			px=MousePosX();
			py=MousePosY();
			pz=MouseButton();

			if ( ((pz&1)==1) & (px>=x) & (px<x+lng) & (py==y) )
				{
				ReleaseButton();

				do
					GetPosMouse(&xm2,&ym2,&pz);
				while ((pz&1)==1);

				car=13;
				}
				else
				r=8;
			}
#endif

		switch(car%256)
			{
			case 13:
				LoadState();
				return 0;
			case 27:
				r=1;		  break;
			case 9:
				r=2;		  break;
			case 0:
				switch(car/256)
					{
					case 15:
					case 0x4B:
					case 72:
						r=3;			   break;
					case 0x4D:
					case 80:
						r=2;			   break;
					case 0x3B:
						r=7;			   break;
					}
				break;
			}
		}

if (p!=2)		//--- Mets OFF -----------------------------------------
	{
	if ((((Cfg->col[48])&240)!=((Cfg->col[19])&240)) & (p!=3))
		{
		AffChr(x,y,32);
		AffChr(x+lng-1,y,32);

		AffChr(x+lng,y,220);
		ChrLin(x+1,y+1,lng,223);
		}

	ColLin(x,y,lng,Cfg->col[48]);						  // Couleur OFF
	}

LoadState();
return r;
}

/*--------------------------------------------------------------------*\
|-	 si p vaut 0 mets off											  -|
|-	 si p vaut 1 interroge											  -|
|-	 retourne -1 si SHIFT TAB, 1 si TAB 							  -|
\*--------------------------------------------------------------------*/
int Switch(int x,int y,int *Val,int len)
{
int r=0;

int car;

SaveState();

PutCur(2,7);

while (r==0)
	{
	AffChr(x+1,y,(*Val) ? 'X' : ' ');

	car=Wait(x+1,y);

#ifndef NOMOUSE
	if (car==0)
		{
		int px,py,pz;

		px=MousePosX();
		py=MousePosY();
		pz=MouseButton();

		if ( ((pz&1)==1) & (px>=x) & (px<=x+len+3) & (py==y) )
			car=32;
			else
			r=8;
		}
#endif

	switch(car%256)
		{
		case 13:
			LoadState();
			return 0;
		case 27:
			r=1;			break;
		case 9:
			r=2;			break;
		case 32:
			(*Val)^=1;		break;
		case 0:
			switch(car/256)
				{
				case 15:
				case 72:
					r=3;			break;
				case 80:
					r=2;			break;
				case 0x3B:
					r=7;			break;
				}
			break;
		}
	}

LoadState();
return r;
}

/*--------------------------------------------------------------------*\
|-	 0 si ENTER 													  -|
|-	 1 si ESCAPE													  -|
|-	 2 si -->														  -|
|-	 3 si <--														  -|
|-	 4 si pas bouger												  -|
|-	 5 si HAUT														  -|
|-	 6 si BAT														  -|
|-	 7 si F1														  -|
|-	 8 si Souris													  -|
\*--------------------------------------------------------------------*/
int MSwitch(int x,int y,int *Val,int len,int i)
{
int r=0;

int car;

SaveState();

PutCur(2,7);

while (r==0)
	{
	AffChr(x+1,y,(*Val)==i ? 'X' : ' ');

	car=Wait(x+1,y);

#ifndef NOMOUSE
	if (car==0)
		{
		int px,py,pz;

		px=MousePosX();
		py=MousePosY();
		pz=MouseButton();

		if ( ((pz&1)==1)  & (px>=x) & (px<=x+len+3) & (py==y) )
			car=32;
			else
			r=8;
		}
#endif

	switch(car%256)
		{
		case 13:
			LoadState();
			return 0;
		case 27:
			r=1;			break;
		case 32:
			(*Val)=i;
			r=4;			break;
		case 9: 												  // TAB
			r=5;			break;
		case 0:
			switch(car/256)
				{
				case 0x4B:										 // LEFT
				case 15:									// SHIFT-TAB
					r=6;					break;
				case 72:										  // BAS
					r=3;					break;
				case 80:										 // HAUT
					r=2;					break;
				case 0x4D:										// RIGHT
					r=5;					break;
				case 0x3B:
					r=7;					break;
				}
			break;
		}
	}

LoadState();
return r;
}

/*--------------------------------------------------------------------*\
|-	 Retourne -1 si escape											  -|
|-	 Retourne numero de la liste sinon								  -|
\*--------------------------------------------------------------------*/
int WinTraite(struct Tmt *T,int nbr,struct TmtWin *F,int first)
{
char fin;											   // si =0 continue
long direct;										 // direction du tab
int i,i2,j;
int *adr;
static char chaine[80];
int x1,y1,x2,y2;
int def=-1;

char update=1;

x1=F->x1;
y1=F->y1;

x2=F->x2;
y2=F->y2;

if (x1==-1)
	{
	x1=(Cfg->TailleX-F->x2)/2;
	x2=x1+F->x2-1;
	}

if (y1==-1)
	{
	y1=(Cfg->TailleY-F->y2)/2;
	y2=y1+F->y2-1;
	}

fin=0;

SaveScreen();

Bar(" Help  ----  ----  ----  ----  ----  ----  ----  ----  ---- ");

i=first;

while (fin==0) {

if (update)
{
i2=i;

Cadre(x1,y1,x2,y2,0,Cfg->col[44],Cfg->col[45]);
Window(x1+1,y1+1,x2-1,y2-1,Cfg->col[19]);

PrintAt(x1+((x2-x1+1)-(strlen(F->name)))/2,y1,F->name);

for(i=0;i<nbr;i++)
switch(T[i].type)
	{
	case 0:
		PrintAt(x1+T[i].x,y1+T[i].y,T[i].str);
		break;
	case 1:
		ColLin(x1+T[i].x,y1+T[i].y,*(T[i].entier),Cfg->col[21]);
		ChrLin(x1+T[i].x,y1+T[i].y,*(T[i].entier),32);
		PrintAt(x1+T[i].x,y1+T[i].y,T[i].str);
		break;
	case 2:
		PrintAt(x1+T[i].x,y1+T[i].y,"      OK     ");
		if (def!=-1)
			Puce(x1+T[def].x,y1+T[def].y,13,0);
		Puce(x1+T[i].x,y1+T[i].y,13,3);
		def=i;
		break;
	case 3:
		PrintAt(x1+T[i].x,y1+T[i].y,"    CANCEL   ");
		if (def==-1) def=i;
		Puce(x1+T[i].x,y1+T[i].y,13,(def == i) ? 3 : 0);
		break;
	case 4:
		Cadre(x1+T[i].x,y1+T[i].y,
	   *(T[i].str)+x1+T[i].x+1,y1+T[i].y+3,1,Cfg->col[44],Cfg->col[45]);
		break;
	case 5:
		PrintAt(x1+T[i].x,y1+T[i].y,T[i].str);
		if (def==-1) def=i;
		Puce(x1+T[i].x,y1+T[i].y,13,(def == i) ? 3 : 0);
		break;
	case 6:
		Cadre(x1+T[i].x,y1+T[i].y,
	   *(T[i].str)+x1+T[i].x+1,y1+T[i].y+2,2,Cfg->col[44],Cfg->col[45]);
		break;
	case 7:
		j=strlen(T[i].str)+2;
		ColLin(x1+T[i].x+j,y1+T[i].y,9,Cfg->col[21]);
		PrintAt(x1+T[i].x,y1+T[i].y,"%s: %-9d",T[i].str,*(T[i].entier));
		break;
	case 8:
		PrintAt(x1+T[i].x,y1+T[i].y,"[%c] %s",
								   *(T[i].entier) ? 'X' : ' ',T[i].str);
		break;
	case 9:
		Cadre(x1+T[i].x,y1+T[i].y,*(T[i].str)+x1+T[i].x+1,
				*(T[i].entier)+y1+T[i].y+1,2,Cfg->col[44],Cfg->col[45]);
		break;
	case 10:
		PrintAt(x1+T[i].x,y1+T[i].y,
					"(%c) %s",(*(T[i].entier)==i) ? 'X' : ' ',T[i].str);

		break;
	case 11:
		ChrLin(x1+T[i].x,y1+T[i].y,*(T[i].entier),32);
		PrintAt(x1+T[i].x,y1+T[i].y,T[i].str);
		break;
	}


direct=1;

i=i2;

update=0;
}


for(i2=0;i2<nbr;i2++)	//--- Affichage a ne faire qu'une fois ---------
	switch(T[i2].type)
		{
		case 10:
			PrintAt(x1+T[i2].x,y1+T[i2].y,"(%c) %s",
						   (*(T[i2].entier)==i2) ? 'X' : ' ',T[i2].str);
			break;
		}

if ( (T[i].type!=2) & (T[i].type!=3) & (T[i].type!=5))
	Puce(x1+T[def].x,y1+T[def].y,13,3);
	else
	Puce(x1+T[def].x,y1+T[def].y,13,0);

switch(T[i].type)
	{
	case 0:
	case 4:
	case 9:
		break;
	case 11:
	case 1:
		direct=InputAt(x1+T[i].x,y1+T[i].y,T[i].str,*(T[i].entier));
		break;
	case 2:
	case 3:
	case 5:
		direct=Puce(x1+T[i].x,y1+T[i].y,13,1);
		break;
	case 7:
		sprintf(chaine,"%d",*(T[i].entier));
		direct=InputAt(x1+T[i].x+strlen(T[i].str)+2,
												 y1+T[i].y,chaine,9);
		sscanf(chaine,"%d",T[i].entier);
		break;
	case 8:
		direct=Switch(x1+T[i].x,y1+T[i].y,T[i].entier,strlen(T[i].str));
		break;
	case 10:
		ColLin(x1+T[i].x+4,y1+T[i].y,strlen(T[i].str),Cfg->col[21]);
		direct=MSwitch(x1+T[i].x,y1+T[i].y,T[i].entier,strlen(T[i].str),i);
		ColLin(x1+T[i].x+4,y1+T[i].y,strlen(T[i].str),Cfg->col[19]);
		PrintAt(x1+T[i].x,y1+T[i].y,"(%c) %s",
							  (*(T[i].entier)==i) ? 'X' : ' ',T[i].str);
		break;
	}

switch(direct)
	{
	case 0: 												// SELECTION
		fin=1;	 break;
	case 1: 													// ABORT
		fin=2;	 break;
	case 2: 												// Next Case
		i++;	 break;
	case 3: 											// Previous Case
		i--;	 break;
	case 5: 											 // Type suivant
		adr=T[i].entier;
		while (adr==T[i].entier)
			{
			i++;
			if (i==nbr) i=0;
			}
		break;
	case 6: 										   // Type precedent
		adr=T[i].entier;
		while (adr==T[i].entier)
			{
			i--;
			if (i==-1) i=nbr-1;
			}
		break;
	case 7: 									  // Aide sur la fenˆtre
		HelpTopic(_numhelp);
		break;
#ifndef NOMOUSE
	case 8:
		{
		int px2,py2,pz2,px,py,pz,j,k;
		int xc1,xc2,yc1,lc1;

		ReleaseButton();

		px=MousePosX();
		py=MousePosY();
		pz=MouseButton();

		if ((px>=x1) & (py==y1) & (px<=x2))
			{
			int ax,ay;

			ReleaseButton();

			do
				{
				GetPosMouse(&px2,&py2,&pz2);
				}
			while ((px2==px) & (py2==py) & (pz2==pz));


			ax=x2-x1;
			ay=y2-y1;

			if ( ((px2-px+x1)>=0) & (py2>=0) &
			 ((px2-px+x2)<Cfg->TailleX) & ((py2+ay)<Cfg->TailleY) )
				{
				update=1;
				x1=px2-px+x1;
				y1=py2;
				x2=x1+ax;
				y2=y1+ay;

				LoadScreenPart(x1,y1,x2,y2);
				}
			}
			else
			{
			k=-1;
			for(j=0;j<nbr;j++)
				{
				xc1=x1+T[j].x;
				xc2=x1+T[j].x+(*(T[j].entier));
				yc1=y1+T[j].y;

				switch(T[j].type)
					{
					case 11:
					case 1:
					case 7:
						if ( (py==yc1) & (px>=xc1) & (px<xc2) ) k=j;
						break;
					case 2:
					case 3:
					case 5:
						if ( (px>=xc1) & (px<xc1+13) & (py==yc1) ) k=j;
						break;
					case 8:
					case 10:
						lc1=strlen(T[j].str);
						if ( (px>=xc1) & (px<=xc1+lc1+3) & (py==yc1) ) k=j;
						break;
					}
				if (k!=-1) break;
				}
			if (k!=-1) i=k;
			}
		}
		break;
#endif
	case 4:
	default:											   // Pas normal
		break;
	}

if (i==-1) i=nbr-1;
if (i==nbr) i=0;
}

LoadScreen();

_numhelp=0;

if (fin==1)
	{
	if ((T[i].type==2) | (T[i].type==3) | (T[i].type==5))
		return i;

	return def;
	}

return -1;													   // ESCAPE
}

/*--------------------------------------------------------------------*\
|-	1 -> Cancel 													  -|
|-	0 -> OK 														  -|
\*--------------------------------------------------------------------*/
char WinMesg(const char *title, const char *msg,char info)
{
static char Buffer2[70];

int d,n,lng;
static int width;
static char length;
char ok;
char *Mesg[5];



struct Tmt T[8] = {
	  { 0,4,2,NULL,NULL},										   // OK
	  { 0,4,3,NULL,NULL},									   // CANCEL
	  { 1,1,9,&length,&width},
	  { 2,2,0,NULL,NULL},
	  { 2,3,0,NULL,NULL},
	  { 2,4,0,NULL,NULL},
	  { 2,5,0,NULL,NULL},
	  { 2,6,0,NULL,NULL}
	  };
int nbr=0;
struct TmtWin F = {-1,10,0,16, Buffer2};

lng=0;
d=0;
for(n=0;n<=strlen(msg);n++)
	{
	if ((msg[n]==0) | (msg[n]=='\n'))
		{
		Mesg[nbr]=(char*)GetMem(n-d+1);
		memcpy(Mesg[nbr],msg+d,n-d);
		Mesg[nbr][n-d]=0;
		if (n-d>lng) lng=n-d;
		d=n+1;
		T[nbr+3].str=Mesg[nbr];
		nbr++;
		}
	}
d=nbr;

T[0].y=nbr+3;
T[1].y=nbr+3;
F.y2=nbr+15;

lng=MAX(lng,strlen(title))+3;
if (lng<31) lng=31;


length=(char)(lng-3);
width=nbr;

F.x2=lng+1;

T[0].x=(lng/4)-5;
T[1].x=(3*lng/4)-6;

switch((info&15))
	{
	case 0:
		break;
	case 1:
		T[0].str="     YES     ";
		T[1].str="      NO     ";
		T[0].type=5;
		T[1].type=5;
		break;
	case 2:
		T[1].str="";
		T[1].type=0;
		T[0].x=(T[0].x+T[1].x)/2;
		break;
	}

nbr+=3;

strcpy(Buffer2,title);

if (WinTraite(T,nbr,&F,info>>4)==0)
	ok=0;
	else
	ok=1;

for(n=0;n<d;n++)
	LibMem(Mesg[n]);

return ok;
}

/*--------------------------------------------------------------------*\
|-	 Avancement de graduation										  -|
|-	 Renvoit le prochain											  -|
\*--------------------------------------------------------------------*/
int Gradue(int x,int y,int length,int from,int to,int total)
{
long j1,j2,j3;

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

char IsSlash(char c)
{
if (c=='\\') return 1;
if (c=='/') return 1;
return 0;
}


/*--------------------------------------------------------------------*\
|- Configuration par default										  -|
\*--------------------------------------------------------------------*/
void DefaultCfg(struct config *Cfg2)
{
#ifndef NOFONT

char defpal[48]=RBPALDEF;

char defcol[64]={
	 7*16+6,14*16+6,13*16+8,14*16+2,7*16+5,1*16+8,1*16+3,14*16+7,7*16+4,
   14*16+1,14*16+7,14*16+3,7*16+4,7*16+3,3*16+0,7*16+13,10*16+1,10*16+5,
  14*16+5,10*16+1,10*16+3,14*16+3,7*16+8,7*16+12,10*16+1,10*16+3,1*16+5,
	   10*16+5,14*16+7,14*16+4,7*16+4,0,7*16+11,7*16+4,13*16+11,3*16+14,
 3*16+13,10*16+1,10*16+3,10*16+1,10*16+3,14*16+3,14*16+3,7*16+3,10*16+1,
  10*16+3,14*16+1,14*16+3,14*16+3,7*16+4,4*16+13,5*16+1,10*16+1,10*16+3,
	1*16+10,10*16+1,10*16+3,3*16+1,4*16+1,5*16+1,0*16+11,0*16+7,0*16+13,
																 0*16+2};

//Cfg2->TailleY=30;
Cfg2->font=1;

memcpy(Cfg2->palette,NORTONPAL,48);
memcpy(Cfg2->col,NORTONCOL,64);

#else

//Cfg2->TailleY=25;
Cfg2->font=0;

memcpy(Cfg2->palette,NORTONPAL,48);
memcpy(Cfg2->col,NORTONCOL,64);
#endif

Cfg2->windesign=4;


Cfg2->SaveSpeed=120;

Cfg2->crc=0x69;

Cfg2->debug=0;

Cfg2->speedkey=1;

Cfg2->display=0;

Cfg2->comport=2;
Cfg2->comspeed=19200;
Cfg2->combit=8;
Cfg2->comparity='N';
Cfg2->comstop=1;

Cfg2->mousemode=0;
}



/*--------------------------------------------------------------------*\
|- Configuration par default										  -|
\*--------------------------------------------------------------------*/
void ReadCfg(struct config *Cfg2)
{
char defcol[]="\x1B\x30\x1E\x3E\x1E\x03\x30\x30\x0F\x30\x3F\x3E"
		   "\x0F\x0E\x30\x19\x1B\x13\x30\x3F\x3E\x0F\x15\x12"
		   "\x30\x3F\x0F\x3E\x4F\x4E\x70\x00\x14\x13\xB4\x60"
		   "\x70\x1B\x1B\x1B\x1E\x30\x30\x0F\x3F\x3F\x4F\x4F"
		   "\x3F\x0F\x80\x90\x30\x30\x0E\x1B\x1B\xA0\xB0\xC0"
		   "\x04\x03\x05\x07";

#ifndef NOFONT
//	  Font2Buf(_intfont);
#endif

Cfg2->font=0;

Cfg2->TailleX=GetScreenSizeX();
Cfg2->TailleY=GetScreenSizeY();

Pal2Buf(Cfg2->palette);
memcpy(Cfg2->col,defcol,64);

Cfg2->SaveSpeed=120;

Cfg2->crc=0x69;

Cfg2->debug=0;

Cfg2->speedkey=1;

Cfg2->display=0;

Cfg2->comport=2;
Cfg2->comspeed=19200;
Cfg2->combit=8;
Cfg2->comparity='N';
Cfg2->comstop=1;

Cfg2->mousemode=0;
}






void LoadErrorHandler(void)
{
#ifdef __WC32__
_harderr(Error_handler);
#endif
}

/*--------------------------------------------------------------------*\
|-						Error and Signal Handler					  -|
|*--------------------------------------------------------------------*|
|-	 Return IOerr si IOerr = 1 ou 3 								  -|
|-	 Return 	3 si IOver = 1										  -|
\*--------------------------------------------------------------------*/
#ifdef __WC32__
int __far Error_handler(unsigned deverr,unsigned errcode,
												   unsigned far *devhdr)
{
int i,n,erreur[3];
char car;
int t;

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

t=(Cfg->TailleX-80)/2;

SaveScreen();

Cadre(t+19,9,t+61,16,0,Cfg->col[46],Cfg->col[47]);
Window(t+20,10,t+60,15,Cfg->col[28]);

PrintAt(t+23,10,"Disk Error: %s",((deverr&32768)==32768) ? "No":"Yes");

PrintAt(t+23,11,"Position of error: ");

switch((deverr&1536)/512)
	{
	case 0: PrintAt(42+t,11,"MS-DOS"); break;
	case 1: PrintAt(42+t,11,"FAT"); break;
	case 2: PrintAt(42+t,11,"Directory"); break;
	case 3: PrintAt(42+t,11,"Data-area"); break;
	}

PrintAt(t+23,12,"Type of error: %s %04X",((deverr&256)==256) ?
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

if (erreur[0])
	{
	PrintAt(t+25,14,"Ignore");
	AffCol(t+25,14,Cfg->col[29]);
	Cadre(t+24,13,t+31,15,2,Cfg->col[46],Cfg->col[47]);
	}

if (erreur[1])
	{
	PrintAt(t+38,14,"Retry");
	AffCol(t+38,14,Cfg->col[29]);
	Cadre(t+37,13,t+43,15,2,Cfg->col[46],Cfg->col[47]);
	}

if (erreur[2])
	{
	PrintAt(t+51,14,"Fail");
	AffCol(t+51,14,Cfg->col[29]);
	Cadre(t+50,13,t+55,15,2,Cfg->col[46],Cfg->col[47]);
	}

IOerr=0;
do
{
car=(char)getch();	  //  Wait(0,0);

if ( (car=='I') | (car=='i') & (erreur[0]) ) IOerr=1;
if ( (car=='R') | (car=='r') & (erreur[1]) ) IOerr=2;
if ( (car=='F') | (car=='f') & (erreur[2]) ) IOerr=3;

}
while (IOerr==0);

LoadScreen();

switch(IOerr)
	{
	case 1:
		return _HARDERR_IGNORE;
	case 2:
		return _HARDERR_RETRY;
	}

if ((errcode) || (devhdr));

return _HARDERR_FAIL;
}
#endif



/*--------------------------------------------------------------------*\
|- Initialise les adresses dans la m‚moire vid‚o					  -|
\*--------------------------------------------------------------------*/
void InitSeg(void)
{
int n;

ClearEvents();

#ifndef NODIRECTVIDEO
for(n=0;n<50;n++)
	scrseg[n]=(char*)(0xB8000+n*(Cfg->TailleX)*2);
#endif

#ifndef NOMOUSE
InitMouse();
#endif
}

/*--------------------------------------------------------------------*\
|- command															  -|
|-	 1:    buffer -> Title											  -|
|-		   return -> Display Mode									  -|
|-																	  -|
|-	 2:    init mode												  -|
|-		   return -> Status (1: correct, 0: bad)					  -|
|-																	  -|
|-	 3:    desinit mode 											  -|
|-		   return -> 1												  -|
|-																	  -|
|- other:  return -> -1 (incorrect command) 						  -|
|-																	  -|
\*--------------------------------------------------------------------*/

int (*disp_system)(int,char *);

/*--------------------------------------------------------------------*\
|-	Renvoit le nombre de systeme									  -|
\*--------------------------------------------------------------------*/
int givesyst(int x)
{
int nbrsyst=0;

//printf("Traite Commandline after dos %d FIN)\n\n",x); exit(1);


#ifdef CURSES
if (nbrsyst==x)
	{	disp_system=curses_system; return 1; }
nbrsyst++;
#endif

#ifndef NODIRECTVIDEO
if (nbrsyst==x)
	{	 disp_system=cache_system; return 1; }
nbrsyst++;
#endif

#ifndef NOANSI
if (nbrsyst==x)
	{	disp_system=ansi_system; return 1; }
nbrsyst++;
#endif

#ifndef NOCOM
if (nbrsyst==x)
	{	disp_system=com_system; return 1; }
nbrsyst++;
#endif

#ifndef NODIRECTVIDEO
if (nbrsyst==x)
	{	disp_system=video_system; return 1; }
nbrsyst++;
#endif

#ifdef PCDISPLAY
if (nbrsyst==x)
	{	disp_system=Pc_system; return 1; }
nbrsyst++;
#endif

#ifdef USEVESA
if (nbrsyst==x)
	{	disp_system=vesa_system; return 1; }
nbrsyst++;
#endif

if (x==-1)
	return nbrsyst;
	else
	return -1;

}

void Redinit(void)
{
Cfg=(struct config*)GetMem(sizeof(struct config));
Fics=(struct fichier*)GetMem(sizeof(struct fichier));

givesyst(0);
}



/*--------------------------------------------------------------------*\
|- Initialise l'‚cran                                                 -|
\*--------------------------------------------------------------------*/
int InitScreen(int a)
{
char buffer[80];
int n, ok, old;

// --- Default mode ----------------------------------------------------

ok=0;


if ((Cfg->TailleX==0) | (Cfg->TailleY==0))
	{
	Cfg->TailleX=80;
	Cfg->TailleY=25;
	}

#ifdef LINUX
AffChr=Curses_AffChr;
AffCol=Curses_AffCol;
Wait=Curses_Wait;
SetMode=Curses_SetMode;
#else 
AffChr=Cache_AffChr;
AffCol=Cache_AffCol;
Wait=Cache_Wait;
SetMode=Cache_SetMode;
#endif

KbHit=Norm_KbHit;
GotoXY=Norm_GotoXY;
WhereXY=Norm_WhereXY;
Window=Norm_Window;
Cadre=Norm_Cadre;
Clr=Norm_Clr;


if (a!=-1)
	{
	old=disp_system(1,buffer);

	for(n=0;n<givesyst(-1);n++)
		{
		givesyst(n);
		if (disp_system(1,buffer)==a)
			{
			ok=disp_system(2,NULL);
			break;
			}
		}

	if (!ok)
		{
		for(n=0;n<givesyst(-1);n++)
			{
			givesyst(n);
			if (disp_system(1,buffer)==old)
				{
				disp_system(2,NULL);
				break;
				}
			}
		}
	}

//printf("Traite Commandline after dos (%s) FIN)\n\n",buffer); exit(1);

InitSeg();
return ok;
}

/*--------------------------------------------------------------------*\
|-	   Gestion de la barre de menu									  -|
|-	 Renvoie 0 pour ESC 											  -|
|-	 Sinon numero du titre; 										  -|
|-	 xp: au depart, c'est le numero du titre                          -|
|-		 a l'arrivee ,c'est la position du titre                      -|
\*--------------------------------------------------------------------*/
int BarMenu(struct barmenu *bar,int nbr,int *poscur,int *xp,int yp)
{
char ok;
int c,i,j,n,x;
char let[32];
int car=0;

#ifndef NOMOUSE
int oldxm;
int xm,ym,button;
int xm2,ym2,pz;

GetPosMouse(&oldxm,&ym,&button);
#endif

for (n=0;n<nbr;n++)
	let[n]=(char)(toupper(bar[n].Titre[0]));

ColLin(0,0,Cfg->TailleX,Cfg->col[7]);
ChrLin(0,0,Cfg->TailleX,32);


x=0;
for(n=0;n<nbr;n++)
	x+=strlen(bar[n].Titre);

i=((Cfg->TailleX)-x)/nbr;
x=((Cfg->TailleX)-(nbr-1)*i-x)/2;

c=*poscur;

ok=0;

do
{
if (c<0) c=nbr-1;
if (c>=nbr) c=0;

j=0;
for (n=0;n<nbr;n++)
	{
	if (n==c)
		{
		AffCol(x+j+n*i-1,0,Cfg->col[8]);
		AffCol(x+j+n*i,0,Cfg->col[43]);
		ColLin(x+j+n*i+1,0,strlen(bar[n].Titre),Cfg->col[8]);
		*xp=x+j+n*i;
		}
		else
		{
		AffCol(x+j+n*i-1,0,Cfg->col[7]);
		AffCol(x+j+n*i,0,Cfg->col[42]);
		ColLin(x+j+n*i+1,0,strlen(bar[n].Titre),Cfg->col[7]);
		}

	PrintAt(x+j+n*i,0,"%s",bar[n].Titre);
	j+=strlen(bar[n].Titre);
	}

car=0;

if (ok==1)
	break;

while ( (!KbHit()) & (car==0) )
	{
#ifndef NOMOUSE

	GetPosMouse(&xm,&ym,&button);

	button=MouseButton();

	if (button!=0)
		{
		do
			GetPosMouse(&xm2,&ym2,&pz);
		while (pz!=0);

		if ((xm2!=xm) | (ym2!=ym))
			button=0;

		if ((button&4)==4)
			car=13;

		if ((button&2)==2)
			car=27;

		if ((button&1)==1)
			{
			if (ym!=0)
				{
				car=1;
				ok=1;
				}
				else
				{
				j=0;
				for (n=0;n<nbr;n++)
					{
					if (xm>=x+j+n*i-(i/2))
						c=n;
					j+=strlen(bar[n].Titre);
					}
				car=1;
				ok=1;
				}
			}
		}

	if ((yp==1) & (ym!=0))
		car=27;

	if ((ym==0) & (oldxm!=xm) & (car==0))
		{
		j=0;
		for (n=0;n<nbr;n++)
			{
			if (xm>=x+j+n*i-(i/2))
				c=n;
			j+=strlen(bar[n].Titre);
			}
		car=1;
		}

	oldxm=xm;
#endif
	}

if (car==0)
	car=Wait(0,0);

// car=1 -> touche pour rire

switch(car)
	{
	case KEY_LEFT:		//--- Gauche ---------------------------------------
		c--;
		break;
	case KEY_RIGHT:		//--- Droite ---------------------------------------
		c++;
		break;
    case KEY_DOWN:
		ok=1;
		break;
	}

if (LO(car)!=0)
	for (n=0;n<nbr;n++)
		if (toupper(car)==let[n])
			c=n,ok=1;
}
while ( (car!=13) & (car!=27) );

*poscur=c;

if (car==27)
	return 0;
	else
	return 1;
}


static int (*fonction[12])(struct barmenu *);
static char fctname[61];

void ClearEvents(void)
{
int n;

for(n=0;n<12;n++)
	{
	fonction[n]=NULL;
	if (n<10)
		strcpy(fctname+n*6," ---- ");
	}
}


void NewEvents(register int (*fct)(struct barmenu *),char *titre,int key)
{
if ((key<1) | (key>12))
	WinError("Invalid Key");

fonction[key-1]=fct;
if (key<=10)
	memcpy(fctname+(key-1)*6,titre,6);
}


/*--------------------------------------------------------------------*\
|-	1: [RIGHT]	 -1: [LEFT] 										  -|
|-	0: [ESC]	  2: [ENTER]										  -|
\*--------------------------------------------------------------------*/
int PannelMenu(struct barmenu *bar,int nbr,MENU *menu)
{
#ifndef NOMOUSE
int oldxm,oldym,oldzm;
#endif

int max,n,m,car,fin;
int i,col;
int col1,col2;
char *let;
char bbar[61];
int nbraff,prem;
int xp,yp;
int c;
int ascen=0;

xp=menu->x;
yp=menu->y;
c=menu->cur;

let=(char*)GetMem(nbr);

if (((menu->attr)&2)==2)
	{
	for (n=0;n<nbr;n++)
		let[n]=0;
	}
else
	{
	for (n=0;n<nbr;n++)
		{
		i=0;

        if (bar[n].fct != 0) {

		do
			{
			do
				{
                car = bar[n].Titre[i];

				let[n]=(char)toupper(car);
				i++;
				}
			while ((car<=32) & (car!=0));

			fin=1;
			if (let[n]!=0)
				for (m=0;m<n;m++)
					if (let[m]==let[n]) fin=0;
			}
		while(fin==0);
		}
        }
	}

max=0;

for (n=0;n<nbr;n++) {
    if (bar[n].fct !=0 ) {
	    if (max<strlen(bar[n].Titre))
		    max=strlen(bar[n].Titre);
    }
}

if (((menu->attr)&1)==1)
	nbraff=menu->nbrmax;
	else
	nbraff=nbr;

if (nbraff>Cfg->TailleY-yp-2)
	{
	nbraff=Cfg->TailleY-yp-2;
	ascen=1;  //--- Pour l'ascensceur ----------------------------------
	}

prem=0;

if (((menu->attr)&4)==0)
	SaveScreen();

strcpy(bbar,fctname);

if (xp+max+ascen>=Cfg->TailleX)
	xp=(Cfg->TailleX)-max-ascen-1;

if (xp<1) xp=1;

Cadre(xp-1,yp-1,xp+max+ascen,yp+nbraff,3,Cfg->col[9],Cfg->col[41]);
Window(xp,yp,xp+max-1+ascen,yp+nbraff-1,Cfg->col[10]);

fin=0;

do
{
if (c<0) c=nbr-1;
if (c>nbr-1) c=0;

while ((c-prem)<0) prem--;
while ((c-prem)>=nbraff) prem++;

for (n=0;n<nbraff;n++)
	{
	if (bar[n+prem].fct==0)
		{
		ChrLin(xp,yp+n,max,196);
		ColLin(xp,yp+n,max,Cfg->col[10]);
		}
		else
		{
		PrintAt(xp,yp+n,"%-*s",max,bar[n+prem].Titre);
		col=1;
		if ((n+prem)==c)
			{
			col1=Cfg->col[12];	// 7
			col2=Cfg->col[13];	// 7
			}
			else
			{
			col1=Cfg->col[10];	// 4
			col2=Cfg->col[11];	// 7
			}

		for (i=0;i<strlen(bar[n+prem].Titre);i++)
			{
			if ((col==1) & (toupper(bar[n+prem].Titre[i])==let[n+prem]))
				AffCol(xp+i,yp+n,col2),col=0;
				else
				AffCol(xp+i,yp+n,col1);
			}

		if (i!=max)
			ColLin(xp+i,yp+n,max-i,col1);
		}
	}

//--- Ascensceur -------------------------------------------------------
if (ascen)
	{
	AffChr(xp+max,yp,(prem!=0) ? 30 : 32);
	AffChr(xp+max,yp+nbraff-1,(prem+nbraff<nbr) ? 31 : 32);
	}

memcpy(bbar,(bar[c].Help!=0) ? " Help " : " ---- ",6);

Bar(bbar);

if (fonction[0]!=NULL)
	fonction[0](&(bar[c]));

car=0;

#ifndef NOMOUSE
GetRPosMouse(&oldxm,&oldym,&oldzm);

while ((!KbHit()) & (car==0))
	{
	int xm,ym,zm,zm2;

	GetRPosMouse(&xm,&ym,&zm);

	if ( (ym<0) & (((menu->attr)&8)==0) & (zm!=0) )
		{
		ReleaseButton();
		zm=0;
		car=27; 				// en esperant que la barre est en 0
		}

	if (zm!=0)
		{
		do
			GetRPosMouse(&xm,&ym,&zm2);
		while (zm2!=0);

		if ((zm&2)==2)
			car=27;

		if ((zm&1)==1)
			{
			int xm2,ym2;

			xm2=xm+xp;
			ym2=ym+yp;

			if (ym2==Cfg->TailleY-1)  //- Fx key -----------------------
				{
				int n;
				if (Cfg->TailleX==90)
					{
					n=xm2/9;
					ColLin(n*9+2,Cfg->TailleY-1,6,Cfg->col[6]);
					car=((0x3B+(xm2/9))*256);
					}
					else
					{
					n=xm2/8;
					ColLin(n*8+2,Cfg->TailleY-1,6,Cfg->col[6]);
					car=((0x3B+(xm2/8))*256);
					}
				}
				else
			if (xm<0)
				car=0x4B00;
				else
			if (xm>(max-1))
				car=0x4D00;
				else
			if ( (ym>=0) & (ym<nbr) )
				{
				if (bar[ym].fct!=0)
					{
					if (c==ym)
						car=13;
						else
						{
						car=13;
						c=ym;
						}
					}
				}
			}
		}
		else
		{
		if ( (oldym!=ym) & (ym>=0) & (ym<nbr) & (xm>=0) & (xm<=(max-1)) )
			if (bar[ym].fct!=0)
				{
				c=ym;
				car=1;

				oldxm=xm;
				oldym=ym;
				}
		}
	}
#endif

if (car==0)
	car=Wait(0,0);

do
	{
	switch(car)
		{
        case 0:
            break;
		case KEY_UP:	   //--- UP ----------------------------------------
			c--;
			break;
		case KEY_LEFT:	   //--- LEFT --------------------------------------
			if (((menu->attr)&8)==0)
				fin=-1, car=27;
			break;
		case KEY_RIGHT:	   //--- RIGHT -------------------------------------
			if (((menu->attr)&8)==0)
				fin=1, car=27;
			break;
		case KEY_PPAGE :	   //--- PGUP --------------------------------------
			m=c;
			for(n=0;n<nbraff;n++)
				{
				m--;
				if (m>=0)
					if (bar[m].fct!=0)
						c=m;
				}
			break;

		case KEY_NPAGE:	   //--- PGDN --------------------------------------
			m=c;
			for(n=0;n<nbraff;n++)
				{
				m++;
				if (m<nbr)
					if (bar[m].fct!=0)
						c=m;
				}
			break;

		case KEY_DOWN:	   //--- DOWN --------------------------------------
			c++;
			break;
		case KEY_HOME:	   //--- HOME --------------------------------------
			for (n=0;n<nbr;n++)
				if (bar[n].fct!=0)
					{
					c=n;
					break;
					}
			break;
        case KEY_END:	   //--- END ---------------------------------------
			for (n=0;n<nbr;n++)
				if (bar[n].fct!=0)
					c=n;
			break;
		case KEY_F(1):	   //--- F1 ----------------------------------------
			if (bar[c].Help!=0)
				HelpTopic(bar[c].Help);
			break;
        case KEY_F(2):	   //--- F2 ----------------------------------------
			if (fonction[1]!=NULL)
				fonction[1](&(bar[c]));
            break;
        case KEY_F(3):     //--- F3 ----------------------------------------
            if (fonction[2]!=NULL)
                fonction[2](&(bar[c]));
            break;
        case KEY_F(4):     //--- F4 ----------------------------------------
            if (fonction[3]!=NULL)
                fonction[3](&(bar[c]));
            break;
        case KEY_F(5):     //--- F5 ----------------------------------------
            if (fonction[4]!=NULL)
                fonction[4](&(bar[c]));
            break;
        case KEY_F(6):     //--- F6 ----------------------------------------
            if (fonction[5]!=NULL)
                fonction[5](&(bar[c]));
            break;
        case KEY_F(7):     //--- F7 ----------------------------------------
            if (fonction[6]!=NULL)
                fonction[6](&(bar[c]));
            break;
        case KEY_F(8):     //--- F8 ----------------------------------------
            if (fonction[7]!=NULL)
                fonction[7](&(bar[c]));
            break;  
        case KEY_F(9):     //--- F9 ----------------------------------------
            if (fonction[8]!=NULL)
                fonction[8](&(bar[c]));
            break;
        case KEY_F(10):     //--- F10 ----------------------------------------
            if (fonction[9]!=NULL)
                fonction[9](&(bar[c]));
            break;
		}

	if (LO(car)!=0)
		for (n=0;n<nbr;n++)
			if (toupper(car)==let[n])
				c=n,car=13;
	}
while (bar[c].fct==0);
}
while ( (car!=13) & (car!=27) );

if (((menu->attr)&4)==0)
	LoadScreen();

menu->cur=c;

LibMem(let);

//PrintAt(1,1,"(%d,%d)",fin,car);

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
|- prototype														  -|
\*--------------------------------------------------------------------*/

static void (*helphandler)(void);
static void (*helptopichandler)(char *topic);

void LoadHelpHandler(register void (*fct)(void))
{
helphandler=fct;
}

void LoadHelpTopicHandler(register void (*fct)(char *))
{
helptopichandler=fct;
}

void NumHelp(int n)
{
_numhelp=n;
}

void Help(void)
{
if (helphandler!=NULL)
	helphandler();
}

void HelpTopic(int n)
{
if (helptopichandler!=NULL)
	{
	char topic[32];
	sprintf(topic,"lnk%d",n);
	helptopichandler(topic);
	}
}


/*--------------------------------------------------------------------*\
|- Initialisation des fichiers selon la path						  -|
\*--------------------------------------------------------------------*/
void SetDefaultPath(char *path)
{
strcpy(_IntBuffer,path);

if ( (path[strlen(path)-1]!='\\') &
	 (path[strlen(path)-1]!='/') )
		_IntBuffer[strlen(path)]=DEFSLASH,
		_IntBuffer[strlen(path)+1]=0;

Fics->LastDir=(char*)GetMem(256);
getcwd(Fics->LastDir,256);

Fics->path=(char*)GetMem(256);
strcpy(Fics->path,_IntBuffer);
}

/*--------------------------------------------------------------------*\
|- Gestion souris													  -|
\*--------------------------------------------------------------------*/

#ifndef NOMOUSE

int MouseRPosX(void)
{
return _xm-_xw;
}

int MouseRPosY(void)
{
return _ym-_yw;
}

void GetRPosMouse(int *x,int *y,int *button)
{
int x1,y1;
GetPosMouse(&x1,&y1,button);

(*x)=x1-_xw;
(*y)=y1-_yw;
}


/*--------------------------------------------------------------------*\
|-	Absolu															  -|
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

void ReleaseButton(void)
{
_zmok=1;
}

void InitMouse(void)
{
union REGS R;

R.w.ax=0x0000;
int386(0x33,&R,&R);
if (R.w.ax==0) return;

R.w.ax=0x07;
R.w.cx=0;
R.w.dx=(short)(((Cfg->TailleX)-1)*8);
int386(0x33,&R,&R);

R.w.ax=0x08;
R.w.cx=0;
R.w.dx=(short)(((Cfg->TailleY)-1)*8);
int386(0x33,&R,&R);

_TmpClik=3;

_MouseOK=1;

_charm=0;

_dclik=0;

ReleaseButton();
}




void GetPosMouse(int *x,int *y,int *button)
{
if (_MouseOK==0)
	{
	(*x)=(*y)=(*button)=0;
	return;
	}

if (Cfg->mousemode==0)
	{
	union REGS R;

	R.w.ax=0x03;
	int386(0x33,&R,&R);

	(*x)=(R.w.cx)/8;
	(*y)=(R.w.dx)/8;

	_zm=R.w.bx;

	if ( ((*x)!=_xm) | ((*y)!=_ym) | (_charm==0) )
		{
		*(scrseg[_ym]+(_xm<<1)+1)=GetCol(_xm,_ym);

		_charm=GetCol((*x),(*y));

		_xm=(*x);
		_ym=(*y);
		}

	if (_charm!=0)
		*(scrseg[_ym]+(_xm<<1)+1)=(char)((_charm&15)*16 + (_charm/16));

	if ((_zm&1)==1)
		{
		_mclock=GetClock();
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
			if (GetClock()!=_mclock)
				{
				_dclik--;
				_mclock=GetClock();
				}
			}
		}

	if (_zm==0) _zmok=1;				// On debloque si touche relache

	if (_zmok==0) _zm=0;	 // Touche est relache si pas encore relache

		if ((_zm&3)==3)
			{
			_NbrKey=0;
			Cfg->mousemode=1;
			_zm=0;
			_zmok=0;

			*(scrseg[_ym]+(_xm<<1)+1)=GetCol(_xm,_ym);
			}

	(*button)=_zm;
	}
else
	{
	signed short xp,yp;
	union REGS R;

	R.w.ax=0x0B;
	int386(0x33,&R,&R);

	(*x)=0;
	(*y)=0;
	(*button)=0;

	xp=(signed short)(R.w.cx);
	yp=(signed short)(R.w.dx);

	if ((xp>0) & (yp==0))
		PutKey(0x4D00);
	if ((xp<0) & (yp==0))
		PutKey(0x4B00);

	if ((yp>0) & (xp==0))
		PutKey(0x5000);
	if ((yp<0) & (xp==0))
		PutKey(0x4800);


	R.w.ax=0x05; //--- Etat des boutons --------------------------------
	int386(0x33,&R,&R);

	_zm=R.w.ax;

	if (_zm==0)
		 _zmok=1;				 //--- On debloque si touche relache ---

	if (_zmok==0) _zm=0;	 // Touche est relache si pas encore relache

	if (_zm!=0)
		_zmok=0;

	if ((_zm&3)==3)
		{
		_NbrKey=0;
		Cfg->mousemode=0;
		_zm=0;
		_zmok=0;
		}

	if ((_zm&1)==1)
		PutKey(13);
	if ((_zm&2)==2)
		PutKey(27);

	_zm=0;
	}
}
#endif

#ifndef NOFONT
void InitFont(void)
{
InitFontFile(Fics->path);
}

void InitFontFile(char *path)
{
switch (Cfg->TailleY)
	{
	case 50:
		Font8xFile(8,path);
		break;
	case 25:
	case 30:
		Font8xFile(16,path);
		break;
	}
}
#endif

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
		AffCol(n,TY,Cfg->col[5]);

	for(j=0;j<6;j++,n++)
		{
		AffCol(n,TY,Cfg->col[6]);
		AffChr(n,TY,*(bar+i*6+j));
		}
	if (Cfg->TailleX==90)
		{
		AffCol(n,TY,Cfg->col[6]);
		AffChr(n,TY,32);
		n++;
		}
	}
}

#ifndef NOMOUSE
static char statusbar;


int GetMouseFctBar(int status)
{
int xm,ym,zm;
int n0,n1;
char col0,col1;

GetPosMouse(&xm,&ym,&zm);

n0=-1;
n1=-1;

col0=(char)(((Cfg->col[6])/16)+(Cfg->col[6]&15)*16);
col1=Cfg->col[6];

switch(status)
	{
	case 0:
		statusbar =(Cfg->TailleX==90) ? (char)(xm/9) : (char)(xm/8);
		n0=statusbar;
		break;
	case 1:
		n0 =(Cfg->TailleX==90) ? xm/9 : xm/8;

		if ((n0!=statusbar) | (ym!=Cfg->TailleY-1))
			{
			n0=-1;
			n1=statusbar;
			}

		break;
	case 2:
		n1=statusbar;
		statusbar =(Cfg->TailleX==90) ? (char)(xm/9) : (char)(xm/8);
		break;
	}

if ((n0!=-1) & (ym==Cfg->TailleY-1))
	ColLin(n0*((Cfg->TailleX==90) ? 9 : 8)+2,Cfg->TailleY-1,6,col0);

if (n1!=-1)
	ColLin(n1*((Cfg->TailleX==90) ? 9 : 8)+2,Cfg->TailleY-1,6,col1);

if ((n1==statusbar) & (ym==Cfg->TailleY-1))
	return ((0x3B+(xm/((Cfg->TailleX==90) ? 9 : 8)))*256);
	else
	return 0;
}
#endif

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

#ifndef NOINT
void GetFreeMem(char *buffer);
#pragma aux GetFreeMem = \
	"mov ax,0500h" \
	"int 31h" \
	modify [edi eax] \
	parm [edi];

int FreeMem(void)
{
int tail[12];

GetFreeMem((char*)tail);						   // inconsistent ?

return tail[0];
}
#else
int FreeMem(void)
{
return 0;
}
#endif

void SnapShot(void)
{
char head1[]={0x6d,0x68,0x77,0x61,0x6e,0x68,0x0,0x04};
char head2[]={1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
char palette[768];
char font[256*16];
FILE *fic;
int x,y,x1,y1,z;
char car,col;
int expos[]={128,64,32,16,8,4,2,1};

int n;

switch(Cfg->TailleY)
	{
	case 50:
		z=8;
		break;
	case 25:
	case 30:
		z=16;
		break;
	default:
		WinError("Unknown Size");
		break;
	}

x1=(Cfg->TailleX)*8;
y1=(Cfg->TailleY)*z;

x=(x1&255)*256+x1/256;
y=(y1&255)*256+y1/256;

fic=fopen("snap.raw","wb");

fwrite(head1,1,8,fic);
fwrite(&x,1,2,fic);
fwrite(&y,1,2,fic);
fwrite(head2,1,20,fic);

memset(palette,0,768);
Pal2Buf(palette);
for(n=0;n<768;n++)
	palette[n]*=4;

fwrite(palette,1,768,fic);

#ifndef NOFONT
Font2Buf(font);
#endif

for(y=0;y<Cfg->TailleY;y++)
for(y1=0;y1<z;y1++)
	for(x=0;x<Cfg->TailleX;x++)
	for(x1=0;x1<8;x1++)
		{
		car=GetChr(x,y);
		col=GetCol(x,y);
		fputc(((font[car*z+y1])&expos[x1])!=expos[x1] ? col/16 : col&15,fic);
		}

fclose(fic);
}


#ifndef NODIRECTVIDEO
void Buf2Scr(char *buffer)
{
int n;
char *Screen_Adr=(char*)0xB8000;

for (n=0;n<MaxZ;n++)
	Screen_Adr[n]=buffer[n];
}

void Scr2Buf(char *buffer)
{
int n;
char *Screen_Adr=(char*)0xB8000;

for (n=0;n<MaxZ;n++)
	buffer[n]=Screen_Adr[n];
}


int GetScreenSizeX(void)
{
return (*(char*)(0x44A));
}

int GetScreenSizeY(void)
{
return (*(char*)(0x484))+1;
}

void SetScreenSizeX(int x)
{
(*(char*)(0x44A))=(char)x;
}

void SetScreenSizeY(int y)
{
(*(char*)(0x484))=(char)(y-1);
}
#else

// Mode par defaut

void Buf2Scr(char *buffer)
{
}

void Scr2Buf(char *buffer)
{
}

static int _screensizex=80,_screensizey=25;

int GetScreenSizeX(void)
{
return _screensizex;
}

int GetScreenSizeY(void)
{
return  _screensizey;
}

void SetScreenSizeX(int x)
{
_screensizex=x;
}

void SetScreenSizeY(int y)
{
_screensizey=y;
}
#endif



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



/*--------------------------------------------------------------------*\
|- Time handler 													  -|
\*--------------------------------------------------------------------*/

#ifdef __WC32__
clock_k GetClock(void)
{
long *cl=(long*)(0x46C);

return *cl;
}
#else
clock_k GetClock(void)
{
return clock();
}
#endif


/*--------------------------------------------------------------------*\
|- Classe KKWin 													  -|
\*--------------------------------------------------------------------*/

KKWin::KKWin()
{
Defaults();

SaveScreen();
}

void KKWin::Test(char *chaine)
{
PrintAt(0,0,chaine);
Wait(0,0);
}

KKWin::KKWin(char *buf)
{
Defaults();

SaveScreen();

title=(char*)GetMem(strlen(buf));
strcpy(title,buf);
}

KKWin::KKWin(char *buf,int x1,int y1,int x2,int y2,int typ,int col)
{
SaveScreen();

title=(char*)GetMem(strlen(buf));
strcpy(title,buf);

WinCadre(x1,y1,x2,y2,typ);
Window(x1+1,y1+1,x2-1,y2-1,col);

left=x1;
top=y1;
right=x2;
bottom=y2;
color=col;
type=typ;
}

void Beep(int type)
{
char x,y;

switch(type)
	{
	case BEEP_WARNING0:
	case BEEP_WARNING1:
	case BEEP_WARNING2:
	case BEEP_ERROR:
		for(y=0;y<Cfg->TailleY;y++)
			for (x=0;x<Cfg->TailleX;x++)
			  AffCol(x,y,(GetCol(x,y)&15)*16+(GetCol(x,y)&240)/16);

		Delay(1);

		for(y=0;y<Cfg->TailleY;y++)
			for (x=0;x<Cfg->TailleX;x++)
			  AffCol(x,y,(GetCol(x,y)&15)*16+(GetCol(x,y)&240)/16);
		break;
	}
}

KKWin::~KKWin()
{
if (title!=NULL)
	LibMem(title);

LoadScreen();
}

int KKWin::Wait(int x,int y)
{
return ::Wait(x,y);
}

void KKWin::Defaults(void)
{
title=NULL;

left=0;
top=0;
right=0;
bottom=0;
color=0;
type=-1;
}


/*--------------------------------------------------------------------*\
|- Cache															  -|
\*--------------------------------------------------------------------*/
#ifndef NODIRECTVIDEO

int cache_system(int command,char *buffer)
{
switch (command)
	{
	case 1: 	//--- Info ---------------------------------------------
		strcpy(buffer,"Normal Mode");
		return 0;

	case 2:
		AffChr=Cache_AffChr;
		AffCol=Cache_AffCol;
		SetMode=Cache_SetMode;

		#ifndef NOINT10
			GotoXY=Cache_GotoXY;
			WhereXY=Cache_WhereXY;
		#endif

		#ifdef __WC32__
			KbHit=Cache_KbHit;
		#endif

		return 1;

	default:
		return -1;
	}
}

int Cache_SetMode(void)
{
int TX,TY;
long lig;
char ok;

TX=GetScreenSizeX();
TY=GetScreenSizeY();

if (Cfg->TailleX==0) Cfg->TailleX=80;
if (Cfg->TailleY==0) Cfg->TailleY=25;

Cfg->UseFont=0;

if ((Cfg->reinit) | ((Cfg->TailleY)!=TY) | ((Cfg->TailleX)!=TX))
	ok=1;
	else
	ok=0;

if (Cfg->font==0)
	ok=1;

if (ok)
	{
	lig=Cfg->TailleY;
	switch (lig)
		{
		case 30:
			Mode30();
			break;
		case 50:
			Mode50();
			break;
		case 25:
		default:
			Cfg->TailleY=25;
			Mode25();
			break;
		}

	SetScreenSizeY(Cfg->TailleY);

	switch(Cfg->TailleX)
		{
		case 90:
			Mode90();
			break;
		case 80:
		default:
			Mode80();
			break;
		}
	SetScreenSizeX(Cfg->TailleX);
	// Clr();

	memset(_RB_screen,0,256*128*2); //--- Clr --------------------------
	}

Cfg->reinit=1;

InitSeg();

return 1;
}




void Cache_AffChr(long x,long y,long c)
{
if (*(_RB_screen+((y<<8)+x))!=(char)c)
	{
	*(scrseg[y]+(x<<1))=(char)c;
	*(_RB_screen+((y<<8)+x))=(char)c;
	}
}

void Cache_AffCol(long x,long y,long c)
{
if (*(_RB_screen+((y<<8)+x)+256*128)!=(char)c)
	{
	*(scrseg[y]+(x<<1)+1)=(char)c;
	*(_RB_screen+((y<<8)+x)+256*128)=(char)c;
	}
}

long Cache_Wait(long x,long y)
{
int a,b;
clock_k Cl;

#ifndef NOMOUSE
int xm=0,ym=0;
#endif
int zm=0;

if ((x!=0) | (y!=0))
	GotoXY(x,y);

if (_NbrKey!=0)
	return GetKey();

Cl=GetClock();

a=0;
b=0;

while ( (!kbhit()) & (b==0) & (zm==0) )
	{
#ifndef NOMOUSE
	GetPosMouse(&xm,&ym,&zm);
#endif

	if ( ((GetClock()-Cl)>Cfg->SaveSpeed*CLOCKK_PER_SEC)
		& (Cfg->SaveSpeed!=0) )
		{
		ScreenSaver();
		Cl=GetClock()+(clock_k)(10*CLOCKK_PER_SEC);
		}
	}

#ifndef NOMOUSE
if (zm!=0) _zmok=0;
#endif

if ((b==0) & (zm==0))
	{
	a=getch();

/*
	if (a=='ü')
		SnapShot();
*/

	if (a==0)
		return getch()*256+a;

	return a;
	}

return b;
}



#ifdef __WC32__
int Cache_KbHit(void)
{
short int deb,fin;

deb=*(short int*)(0x41A);
fin=*(short int*)(0x41C);

if (_NbrKey==0)
	return (deb!=fin);

return 1;
}
#endif

#ifndef NOINT10
void Cache_GotoXY(long x,long y)
{
union REGS regs;

regs.h.dl=(char)x;
regs.h.dh=(char)y;
regs.h.bh=0;
regs.h.ah=2;

int386(0x10,&regs,&regs);
}

void Cache_WhereXY(long *x,long *y)
{
union REGS regs;

regs.h.bh=0;
regs.h.ah=3;

int386(0x10,&regs,&regs);

*x=regs.h.dl;
*y=regs.h.dh;
}
#endif

#endif




/*--------------------------------------------------------------------*\
|- ansi 															  -|
\*--------------------------------------------------------------------*/
#ifndef NOANSI

void Ansi_AffChr(long x,long y,long c);
void Ansi_AffCol(long x,long y,long c);

void Ansi_Clr(void);
void Ansi_Window(long left,long top,long right,long bottom,long color);
void Ansi_GotoXY(long x,long y);
void Ansi_WhereXY(long *x,long *y);


int ansi_system(int command,char *buffer)
{
char buf[31];
int nr;

switch (command)
	{
	case 1: 	//--- Info ---------------------------------------------
		strcpy(buffer,"Ansi Mode");
		return 1;

	case 2:
		nr=0;
		PRINTF("\x1b[6n\r      \r");      // ask for ansi device report
		while ((0 !=kbhit()) && (nr<30))//read whatever input is present
			buf[nr++] = (char)getch();

		buf[nr]=0;								// zero terminate string
		if (strncmp(buf,"\x1b[",2)!=0) //check precense of device report
			return 0;

		Cfg->TailleX=80;
		Cfg->TailleY=24;

		AffChr=Ansi_AffChr;
		AffCol=Ansi_AffCol;
		GotoXY=Ansi_GotoXY;
		WhereXY=Ansi_WhereXY;
		Clr=Ansi_Clr;
		Window=Ansi_Window;
		return 1;

	case 3:
		PRINTF("\x1b[0m\n\n\x1b[2J");
		return 1;

	default:
		return -1;
	}
}

void Ansi_GenCol(long x,long y);
void Ansi_GenChr(long x,long y,long c);

int _Ansi_col1,_Ansi_col2;				 // Couleur que l'on doit mettre
int _Ansi_tcol; 				// Couleur que l'on a demand‚ par affcol
int _Ansi_x=0,_Ansi_y=0;						  // precedente position

char _Ansi_cnv[]={0,4,2,6,1,5,3,7};


void Ansi_AffCol(long x,long y,long c)
{
if (*(_RB_screen+((y<<8)+x)+256*128)!=(char)c)
	{
	*(_RB_screen+((y<<8)+x)+256*128)=(char)c;

	Ansi_GenCol(x,y);
	Ansi_GenChr(x,y,GetChr(x,y));
	}
}

void Ansi_AffChr(long x,long y,long c)
{
Ansi_GenCol(x,y);

if (*(_RB_screen+((y<<8)+x))!=(char)c)
		Ansi_GenChr(x,y,c);
}

void Ansi_GenCol(long x,long y)
{
if (*(_RB_screen+((y<<8)+x)+256*128)!=_Ansi_tcol)
	{
	_Ansi_tcol=*(_RB_screen+((y<<8)+x)+256*128);

	_Ansi_col1=(_Ansi_cnv[(_Ansi_tcol/16)&7])+40;
	_Ansi_col2=(_Ansi_cnv[_Ansi_tcol&7])+30;

	PRINTF("\x1b[%d;%d;%dm",(_Ansi_tcol&8)==8,_Ansi_col1,_Ansi_col2);
	fflush(stdout);
	}
}

void Ansi_GenChr(long x,long y,long c)
{
*(_RB_screen+((y<<8)+x))=(char)c;

switch(c)
	{
	case 0:
	case 8:
	case 10:
	case 13:	c=32;	break;
	case 16:	c='>';  break;
	case 17:	c='<';  break;
	case 127:	c='^';  break;
	case 7: 	c='.';  break;
	}

_Ansi_x++;

//	  if (_Ansi_x==80) _Ansi_x=0, _Ansi_y=y+1;

if ( (x!=_Ansi_x) | (y!=_Ansi_y) )
   {
   PRINTF("\x1b[%d;%dH",y+1,x+1);
   _Ansi_x=x,  _Ansi_y=y;
   }

PRINTF("%c",c);
fflush(stdout);
}

void Ansi_GotoXY(long x,long y)
{
PRINTF("\x1b[%d;%dH",y+1,x+1);
fflush(stdout);
_Ansi_x=x;
_Ansi_y=y;
}

void Ansi_WhereXY(long *x,long *y)
{
*x=_Ansi_x;
*y=_Ansi_y;
}

void Ansi_Clr(void)
{
memset(_RB_screen+256*128,7,256*128);
memset(_RB_screen,32,256*128);

PRINTF("\x1b[0m\n\n\x1b[2J");
}

void Ansi_Window(long left,long top,long right,long bottom,long color)
{
int i,j;

_xw=left;
_yw=top;

_xw2=right;
_yw2=bottom;


for(j=top;j<=bottom;j++)
	for (i=left;i<=right;i++)
		{
		*(_RB_screen+((j<<8)+i)+256*128)=(char)color;
		*(_RB_screen+((j<<8)+i))=0; 			// Pour le remettre apres
		}

for(j=top;j<=bottom;j++)
	for (i=left;i<=right;i++)
		AffChr(i,j,32);
}

#endif

/*--------------------------------------------------------------------*\
|- doorway															  -|
\*--------------------------------------------------------------------*/

#ifndef NOCOM

static char _ComBuffer[256];		   // Buffer interne multi usage ---

/*--------------------------------------------------------------------*\
|-						 Affichage par COM	Ansi					  -|
\*--------------------------------------------------------------------*/
void Com_AffChr(long x,long y,long c);
void Com_AffCol(long x,long y,long c);
void Com_GotoXY(long x,long y);
long Com_Wait(long x,long y);
int Com_KbHit(void);
void Com_Clr(void);
void Com_Window(long left,long top,long right,long bottom,long color);

int com_system(int command,char *buffer)
{
switch (command)
	{
	case 1: 	//--- Info ---------------------------------------------
		strcpy(buffer,"Doorway Mode");
		return 2;

	case 2:
		com_open(Cfg->comport,Cfg->comspeed,Cfg->combit,
				 Cfg->comparity,Cfg->comstop);

		AffChr=Com_AffChr;
		AffCol=Com_AffCol;
		Wait=Com_Wait;
		KbHit=Com_KbHit;
		GotoXY=Com_GotoXY;
		Clr=Com_Clr;
		Window=Com_Window;
		return 1;

	case 3:
		com_close();
		return 1;

	default:
		return -1;
	}
}

void Com_GenCol(long x,long y);
void Com_GenChr(long x,long y,long c);

int _Com_col1,_Com_col2;				 // Couleur que l'on doit mettre
int _Com_tcol;					// Couleur que l'on a demand‚ par affcol
int _Com_x=0,_Com_y=0;							  // precedente position

char _Com_cnv[]={0,4,2,6,1,5,3,7};

void Com_AffCol(long x,long y,long c)
{
if (y>=Cfg->TailleY-1) return;

if (*(_RB_screen+((y<<8)+x)+256*128)!=(char)c)
	{
	*(_RB_screen+((y<<8)+x)+256*128)=(char)c;

	*(scrseg[y]+(x<<1)+1)=(char)c;		// --------- Echo console ------

	Com_GenCol(x,y);
	Com_GenChr(x,y,GetChr(x,y));
	}

}

void Com_AffChr(long x,long y,long c)
{
if (y>=Cfg->TailleY-1) return;

if (*(_RB_screen+((y<<8)+x))!=(char)c)
	{
	Com_GenCol(x,y);

	*(scrseg[y]+(x<<1))=(char)c;		// --------- Echo console ------

	Com_GenChr(x,y,c);
	}
}

void Com_GenCol(long x,long y)
{
int n;

if (*(_RB_screen+((y<<8)+x)+256*128)!=_Com_tcol)
	{
	_Com_tcol=*(_RB_screen+((y<<8)+x)+256*128);

	_Com_col1=(_Com_cnv[(_Com_tcol/16)&7])+40;
	_Com_col2=(_Com_cnv[_Com_tcol&7])+30;

	sprintf(_ComBuffer,
				 "\x1b[%d;%d;%dm",(_Com_tcol&8)==8,_Com_col1,_Com_col2);
	for (n=0;n<strlen(_ComBuffer);n++)
		com_send_ch(_ComBuffer[n]);
	}
}

void Com_GenChr(long x,long y,long c)
{
*(_RB_screen+((y<<8)+x))=(char)c;

switch(c)
	{
	case 0:
	case 8:
	case 10:
	case 13:	c=32;	break;
	case 16:	c='>';  break;
	case 17:	c='<';  break;
	case 127:	c='^';  break;
	case 7: 	c='.';  break;
	}

_Com_x++;

if ( (x!=_Com_x) | (y!=_Com_y) )
	Com_GotoXY(x,y);

com_send_ch(c);
}

long Com_Wait(long x,long y)
{
char buf[32];
char n;
char cont;

cont=1;
n=0;

if ((x!=0) | (y!=0))
	GotoXY(x,y);

while(cont==1)
	{
	while (1)
		{
		if (com_ch_ready())
			{
			buf[n]=(char)com_read_ch();
			break;
			}
		if (kbhit())
			{
			buf[n]=(char)getch();
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

void Com_GotoXY(long x,long y)
{
int n;

sprintf(_ComBuffer,"\x1b[%d;%dH",y+1,x+1);
for (n=0;n<strlen(_ComBuffer);n++)
	com_send_ch(_ComBuffer[n]);

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
int n;

memset(_RB_screen+256*128,7,256*128);
memset(_RB_screen,32,256*128);

sprintf(_ComBuffer,"\x1b[0m\n\n\x1b[2J");
for (n=0;n<strlen(_ComBuffer);n++)
	com_send_ch(_ComBuffer[n]);
}

void Com_Window(long left,long top,long right,long bottom,long color)
{
int i,j;

_xw=left;
_yw=top;

_xw2=right;
_yw2=bottom;

for(j=top;j<=bottom;j++)
	for (i=left;i<=right;i++)
		{
		*(_RB_screen+((j<<8)+i)+256*128)=(char)color;
		*(_RB_screen+((j<<8)+i))=0; 			// Pour le remettre apres

		*(scrseg[j]+(i<<1)+1)=(char)color;	  // ------- Echo console --
		}

for(j=top;j<=bottom;j++)
	for (i=left;i<=right;i++)
		AffChr(i,j,32);
}
#endif

/*--------------------------------------------------------------------*\
|-		  Display the characters on the graphics video screen		  -|
\*--------------------------------------------------------------------*/
#ifndef NODIRECTVIDEO

void Video_AffChr(long x,long y,long c);
void Video_AffCol(long x,long y,long c);
int Video_SetMode(void);


int video_system(int command,char *buffer)
{
switch (command)
	{
	case 1: 	//--- Info ---------------------------------------------
		strcpy(buffer,"Graphics Mode (320x200)");
		return 3;

	case 2:
		AffChr=Video_AffChr;
		AffCol=Video_AffCol;
		SetMode=Video_SetMode;
		return 1;

	default:
		return -1;
	}
}

static char videofont[4096];

void Video_Aff(long x,long y)
{
int car,col;
char *screen=(char*)(0xA0000);
int i,j,d,e;

if ((x>=Cfg->TailleX) | (y>=Cfg->TailleY))
	return;

car=*(_RB_screen+((y<<8)+x));
col=*(_RB_screen+((y<<8)+x)+256*128);

d=x*4+y*8*320;

for(j=0;j<8;j++)
	{
	d+=3;
	e=videofont[car*8+j];

	for(i=0;i<4;i++)
		{
		screen[d]=(e&1) ? (char)(col&15) : (char)(col/16);
		e=e/4;
		d--;
		}
	d+=321;
	}
}


void Video_AffChr(long x,long y,long c)
{
if (*(_RB_screen+((y<<8)+x))!=(char)c)
	{
	*(_RB_screen+((y<<8)+x))=(char)c;
	Video_Aff(x,y);
	}
}

void Video_AffCol(long x,long y,long c)
{
if (*(_RB_screen+((y<<8)+x)+256*128)!=(char)c)
	{
	*(_RB_screen+((y<<8)+x)+256*128)=(char)c;
	Video_Aff(x,y);
	}
}

int Video_SetMode(void)
{
FILE *fic;
union REGS R;
char path[256];

R.w.ax=0x13;
int386(0x10,&R,&R);

memset(_RB_screen,0,256*128*2); //--- Clr --------------------------

strcpy(path,Fics->path);
strcat(path,"\\font8x8.cfg");

fic=fopen(path,"rb");
if (fic!=NULL)
	{
	Cfg->TailleX=80;
	Cfg->TailleY=24;
	Cfg->UseFont=1; 						// utilise les fonts 8x?
	fread(videofont,256*8,1,fic);
	fclose(fic);
	}
else
	{
	memcpy(videofont,font8x8,256*8);
	Cfg->UseFont=0; 						// utilise les fonts 8x?
	Cfg->TailleX=80;
	Cfg->TailleY=24;
	}

InitSeg();

return 1;
}
#endif


/*--------------------------------------------------------------------*\
|- video / DJGPP													  -|
\*--------------------------------------------------------------------*/
#ifdef PCDISPLAY

#include <pc.h>

void Pc_AffChr(long x,long y,long c);
void Pc_AffCol(long x,long y,long c);


int Pc_system(int command,char *buffer)
{
switch (command)
	{
	case 1: 	//--- Info ---------------------------------------------
		strcpy(buffer,"Graphics Mode (320x200)");
		return 6;

	case 2:
		AffChr=Pc_AffChr;
		AffCol=Pc_AffCol;
		return 1;

	default:
		return -1;
	}
}

void Pc_AffCol(long x,long y,long c)
{
char ch,attr;

*(_RB_screen+((y<<8)+x)+256*128)=(char)c;

attr=*(_RB_screen+((y<<8)+x)+256*128);
ch=*(_RB_screen+((y<<8)+x));

ScreenPutChar(ch,attr,x,y);
}

void Pc_AffChr(long x,long y,long c)
{
char ch,attr;

*(_RB_screen+((y<<8)+x))=(char)c;

attr=*(_RB_screen+((y<<8)+x)+256*128);
ch=*(_RB_screen+((y<<8)+x));

ScreenPutChar(ch,attr,x,y);
}
#endif


/*--------------------------------------------------------------------*\
|- PTC																  -|
\*--------------------------------------------------------------------*/

#ifdef USEPTC

#include "ptc.h"

void Ptc_AffChr(long x,long y,long c);
void Ptc_AffCol(long x,long y,long c);
int Ptc_SetMode(void);


/*--------------------------------------------------------------------*\
|-		  Display the characters on the graphics video screen		  -|
\*--------------------------------------------------------------------*/

static char ptc_videofont[4096];

PTC ptc;
Surface surface;

int ptc_system(int command,char *buffer)
{
switch (command)
	{
	case 1: 	//--- Info ---------------------------------------------
		strcpy(buffer,"Graphic Mode (with PTC)");
		return 4;
		break;
	case 2:
		AffChr=Ptc_AffChr;
		AffCol=Ptc_AffCol;
		SetMode=Ptc_SetMode;
		break;
	}
return 4;
}


void Ptc_Aff(long x,long y)
{
int car,col;
char *buffer;
int i,j,e;
int pitch;

if ((x>=Cfg->TailleX) | (y>=Cfg->TailleY))
	return;

buffer=(char*)(surface.Lock());

car=*(_RB_screen+((y<<8)+x));
col=*(_RB_screen+((y<<8)+x)+256*128);

pitch=surface.GetPitch();

for(j=0;j<8;j++)
	{
	e=videofont[car*8+j];

	for(i=0;i<8;i++)
		{
		ushort *pixel=(ushort*)(buffer+pitch*(y+j)+(x*8+8-i)*2);
		*pixel=RGB16(
					  (e&1) ? 190 : 0,
					  (e&1) ? 190 : 0,
					  (e&1) ? 190 : 0 );
		e=e/2;
		}
	}

surface.Unlock();	// unlock surface

surface.Update();	// update to display

}


void Ptc_AffChr(long x,long y,long c)
{
if (*(_RB_screen+((y<<8)+x))!=(char)c)
	{
	*(_RB_screen+((y<<8)+x))=(char)c;
	Ptc_Aff(x,y);
	}
}

void Ptc_AffCol(long x,long y,long c)
{
if (*(_RB_screen+((y<<8)+x)+256*128)!=(char)c)
	{
	*(_RB_screen+((y<<8)+x)+256*128)=(char)c;
	Ptc_Aff(x,y);
	}
}


int Ptc_SetMode(void)
{
FILE *fic;
char path[256];

if (!ptc.Init(640,480,VIRTUAL16))
	{
	ptc.Error("could not initialize");
	return 0;
	}

ptc.SetTitle("fire");

// create main drawing surface
surface.Init(ptc,640,480,RGB565);

memset(_RB_screen,0,256*128*2); //--- Clr --------------------------

strcpy(path,Fics->path);
strcat(path,"\\font8x8.cfg");

fic=fopen(path,"rb");
if (fic!=NULL)
	{
	Cfg->TailleX=80;
	Cfg->TailleY=50;
	Cfg->UseFont=1; 						// utilise les fonts 8x?
	fread(ptc_videofont,256*8,1,fic);
	fclose(fic);
	}
else
	{
	memcpy(ptc_videofont,font8x8,256*8);
	Cfg->UseFont=0; 						// utilise les fonts 8x?
	Cfg->TailleX=80;
	Cfg->TailleY=50;
	}

InitSeg();

return 1;
}
#endif





/*--------------------------------------------------------------------*\
|- curses															  -|
\*--------------------------------------------------------------------*/
#ifdef CURSES

#include <curses.h>


void Curses_Cadre(int x1,int y1,int x2,int y2,int type,int col1,int col2)
{

// Cfg->windesign
//  type
        
ColLin(x1,y1,x2-x1+1,col1);
ColCol(x1,y1+1,y2-y1,col1);

ColLin(x1+1,y2,x2-x1,col2);
ColCol(x2,y1+1,y2-y1-1,col2);

AffChr(x1,y1,'(');
AffChr(x2,y1,')');
AffChr(x1,y2,'(');
AffChr(x2,y2,')');

ChrLin(x1+1,y1,x2-x1-1,'-');
ChrLin(x1+1,y2,x2-x1-1,'-');

ChrCol(x1,y1+1,y2-y1-1,'|');
ChrCol(x2,y1+1,y2-y1-1,'|');
}


int Curses_SetMode(void)
{
int TX,TY;
long lig;
char ok;

memset(_RB_screen,0,256*128*2); //--- Clr --------------------------

Cfg->reinit=1;

Cfg->UseFont=0;                         // utilise les fonts 8x?

Cfg->TailleX = _screensizex;
Cfg->TailleY = _screensizey;

InitSeg();

return 1;
}

char Curses_KbHit(void)
{
int ckey, retour;

//return 0;

if ( (ckey = getch()) != ERR) {
    retour = 1;
    } else {
    retour = 0;
    }

if (retour == 1) {
//        printf("Traite Commandline after dos (%d) FIN)\n\n",ckey); exit(1);
    PutKey(ckey);
    }

return retour;
}
       
       

long Curses_Wait(long x,long y)
{
int a,b;
clock_k Cl;

if ((x!=0) | (y!=0))
	GotoXY(x,y);

if (_NbrKey!=0)
	return GetKey();

Cl=GetClock();

a=0;
b=0;

if (b==0)
	{
	a=getch();
	if (a==0)
		return getch()*256+a;

	return a;
	}

return b;
}


int curses_system(int command,char *buffer)
{
WINDOW *w;
        

switch (command)
	{
	case 1: 	//--- Info ---------------------------------------------
		strcpy(buffer,"Curses Mode");
		return 5;

	case 2:
		w=initscr();
		cbreak();
		noecho();
        nonl();
        nodelay(w, TRUE);

		intrflush(stdscr,FALSE);
        start_color();

        _screensizex = getmaxx(w);
        _screensizey = getmaxy(w);


        {
                int n,m;

                for(n=0;n<8;n++)
                        for(m=0;m<8;m++)
                                init_pair(n+m*8,n,m);
        }

        curs_set(1);
		keypad(stdscr,TRUE);

		AffChr=Curses_AffChr;
		AffCol=Curses_AffCol;
		GotoXY=Curses_GotoXY;
        Cadre=Curses_Cadre;
		return 1;

	case 3:
		endwin();
		return 1;

	default:
		return -1;
	}
}

void Curses_AffCol(long x,long y,long attr)
{
char ch;

*(_RB_screen+((y<<8)+x)+256*128)=(char)attr;

ch=*(_RB_screen+((y<<8)+x));

attrset(COLOR_PAIR( (attr & 7) + ((attr & 240) >>2) ));
mvaddch(y,x,ch);
}

void Curses_AffChr(long x,long y,long ch)
{
char attr;

*(_RB_screen+((y<<8)+x))=(char)ch;

attr=  *(_RB_screen+((y<<8)+x)+256*128);


attrset(COLOR_PAIR( (attr & 7) + ((attr & 240) >>2) ));
mvaddch(y,x,ch);
}


void Curses_GotoXY(long x,long y)
{
move(y,x);
refresh();
}

#endif


/*--------------------------------------------------------------------*\
|-		  Display the characters on the graphics video screen		  -|
\*--------------------------------------------------------------------*/
#ifdef USEVESA

void vesa_AffChr(long x,long y,long c);
void vesa_AffCol(long x,long y,long c);
int vesa_SetMode(void);
void vesa_Cadre(int x1,int y1,int x2,int y2,int type,int col1,int col2);


int vesa_system(int command,char *buffer)
{
switch (command)
	{
	case 1: 	//--- Info ---------------------------------------------
		strcpy(buffer,"Graphics Mode (Vesa mode)");
		return 7;

	case 2:
//		  AffChr=Video_AffChr;
//		  AffCol=Video_AffCol;
//		  SetMode=vesa_SetMode;

		AffChr=vesa_AffChr;
		AffCol=vesa_AffCol;
		SetMode=vesa_SetMode;
		Cadre=vesa_Cadre;
		return 1;

	default:
		return -1;
	}
}

static char vesa_videofont[4096];

void vesa_Aff(long x,long y);

void vesa_Aff(long x,long y)
{
int car,col;
char *screen=(char*)(0xA0000);
int i,j,d,e;

for(i=0;i<250;i++)
	screen[i]=i;

if ((x>=Cfg->TailleX) | (y>=Cfg->TailleY))
	return;

if ((x>=40) | (y>=25))
	return;

car=*(_RB_screen+((y<<8)+x));
col=*(_RB_screen+((y<<8)+x)+256*128);

d=(x+y*320)*8;

for(j=0;j<8;j++)
	{
	d+=7;
	e=vesa_videofont[car*8+j];

	for(i=0;i<8;i++)
		{
		screen[d]=(e&1) ? (char)(col&15) : (char)(col/16);
		e=e/2;
		d--;
		}
	d+=321;
	}
}

void vesa_Cadre(int x1,int y1,int x2,int y2,int type,int col1,int col2)
{
int x,y;

for(x=x1;x<=x2;x++)
	{
	*(_RB_screen+((y1<<8)+x))=0;
	*(_RB_screen+((y2<<8)+x))=0;
	}

for(y=y1;y<=y2;y++)
	{
	*(_RB_screen+((y<<8)+x1))=0;
	*(_RB_screen+((y<<8)+x2))=0;
	}

char *screen=(char*)(0xA0000);

if (x1>=40) x1=39;
if (x2>=40) x2=39;
if (y1>=25) y1=24;
if (y2>=25) y2=24;

for(x=(x1+1)*8;x<x2*8;x++)
	{
	screen[x+(y1*8+7)*320]=(char)col1;
	screen[x+(y2*8)*320]=(char)col2;
	}

for(y=(y1+1)*8;y<y2*8;y++)
	{
	screen[(x1*8+7)+y*320]=(char)col1;
	screen[(x2*8)+y*320]=(char)col2;
	}

}


void vesa_AffChr(long x,long y,long c)
{
if (*(_RB_screen+((y<<8)+x))!=(char)c)
	{
	*(_RB_screen+((y<<8)+x))=(char)c;
	vesa_Aff(x,y);
	}
}

void vesa_AffCol(long x,long y,long c)
{
if (*(_RB_screen+((y<<8)+x)+256*128)!=(char)c)
	{
	*(_RB_screen+((y<<8)+x)+256*128)=(char)c;
	vesa_Aff(x,y);
	}
}

int vesa_SetMode(void)
{
FILE *fic;
union REGS R;
char path[256];

R.w.ax=0x13;
int386(0x10,&R,&R);

memset(_RB_screen,0,256*128*2); //--- Clr --------------------------

strcpy(path,Fics->path);
strcat(path,"\\font8x8.cfg");

fic=fopen(path,"rb");
if (fic!=NULL)
	{
	Cfg->TailleX=80;
	Cfg->TailleY=24;
	Cfg->UseFont=1; 						// utilise les fonts 8x?
	fread(vesa_videofont,256*8,1,fic);
	fclose(fic);
	}
else
	{
	memcpy(vesa_videofont,font8x8,256*8);
	Cfg->UseFont=0; 						// utilise les fonts 8x?
	Cfg->TailleX=80;
	Cfg->TailleY=24;
	}

InitSeg();

return 1;
}
#endif


