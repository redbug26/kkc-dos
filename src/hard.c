// Hard-function
#include <stdarg.h>
#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include <malloc.h>

#include <i86.h>
#include <time.h>

#include "hard.h"

struct config *Cfg;
struct fichier *Fics;
struct PourMask **Mask;

char *scrseg=(char*)0xB8000;

void GotoXY(char x,char y)
{
union REGS regs;

regs.h.dl=x;
regs.h.dh=y;
regs.h.bh=0;
regs.h.ah=2;

int386(0x10,&regs,&regs);
}

void WhereXY(char *x,char *y)
{
union REGS regs;

regs.h.bh=0;
regs.h.ah=3;

int386(0x10,&regs,&regs);

*x=regs.h.dl;
*y=regs.h.dh;

}

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
return *(scrseg+(y*80+x)*2);
}

char GetCol(char x,char y)
{
return *(scrseg+(y*80+x)*2+1);
}

void AffChr(char x,char y,char c)
{
*(scrseg+(y*80+x)*2)=c;
}

void AffCol(char x,char y,char c)
{
*(scrseg+(y*80+x)*2+1)=c;
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


void ColWin(int left,int top,int right,int bottom,short color)
{
int i,j;

for (i=left;i<=right;i++)
   for(j=top;j<=bottom;j++)
	  AffCol(i,j,color);
}

void ChrWin(int left,int top,int right,int bottom,short car)
{
int i,j;

for (i=left;i<=right;i++)
   for(j=top;j<=bottom;j++)
      AffChr(i,j,car);
}


void Clr()
{
char i,j;

for (i=0;i<80;i++)
   for(j=0;j<25;j++)
	  {
	  AffChr(i,j,32);
	  AffCol(i,j,7);
	  }
}

void ScrollUp(void)
{
int n;
for (n=0;n<49*160;n++)
    scrseg[n]=scrseg[n+160];
}


char MEcran[8000];

void MoveText(int x1,int y1,int x2,int y2,int x3,int y3)
{
int x,y,n;
for (n=0;n<8000;n++) MEcran[n]=*(scrseg+n);

for (y=y3;y<=y3+(y2-y1);y++)
    for (x=x3;x<=x3+(x2-x1);x++)    {
        *(scrseg+(x+y*80)*2)=MEcran[((x-x3+x1)+(y-y3+y1)*80)*2];
        *(scrseg+(x+y*80)*2+1)=MEcran[((x-x3+x1)+(y-y3+y1)*80)*2+1];
        }

}

char *Ecran[10];
char EcranX[10],EcranY[10];
char EcranD[10],EcranF[10];
signed short WhichEcran=0;

void SaveEcran(void)
{
int n;

if (Ecran[WhichEcran]==NULL)
    Ecran[WhichEcran]=GetMem(8000);

for (n=0;n<8000;n++)
    Ecran[WhichEcran][n]=*(scrseg+n);

WhereXY(&(EcranX[WhichEcran]),&(EcranY[WhichEcran]));

GetCur(&(EcranD[WhichEcran]),&(EcranF[WhichEcran]));

WhichEcran++;
}

void ChargeEcran(void)
{
int n;

WhichEcran--;

if ( (Ecran[WhichEcran]==NULL) | (WhichEcran<0) )
    {
    Clr();
    PrintAt(0,0,"Internal Error: ChargeEcran");
    return;
    }

for (n=0;n<8000;n++)
    *(scrseg+n)=Ecran[WhichEcran][n];

GotoXY(EcranX[WhichEcran],EcranY[WhichEcran]);

PutCur(EcranD[WhichEcran],EcranF[WhichEcran]);

free(Ecran[WhichEcran]);
Ecran[WhichEcran]=NULL;
}




void PrintAt(int x,int y,char *string,...)
{
char sortie[255];
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
//    AffChr(a,y,'Û');
//    Delay(1);
	AffChr(a,y,*suite);
	a++;
	suite++;
	}

}

/************************/
/* Retourne 1 sur ESC	*/
/*          0 ENTER     */
/*          2 TAB       */
/*          3 SHIFT-TAB */
/************************/

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
if (fin>longueur)	 {
	*chaine=0;
	fin=0;
	}

PrintAt(colonne,ligne,chaine);		/* R‚‚crit la chaine … la position d‚sir‚e */

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

if ( ((caractere&255)!=0) & (couleur!=0) & (caractere!=13) & (caractere!=27) & (caractere!=9) )
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
    case 0:  /* v‚rifier si pas de touche de fonction press‚e */
		c2=(caractere/256);
		if (couleur!=0) 	/* Preserve ou pas l'ancienne valeur  ? */
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
          case 0x0F:    // SHIFT-TAB
            retour=3;
            end=1;
            break;
		  case 71: i=0; 						 break;  /* Home */
		  case 75: if (i>0) i--; else Beep();	 break;  /* Left */
		  case 77: if (i<fin) i++; else Beep();  break;  /* Right */
		  case 79: i=fin;						 break;  /* End */
          case 13: *(chaine+fin)=0;              break;  /* Enter */

		  case 83:	/* del */
			if (i!=fin)  /* v‚rifier si pas premiere position */
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

		  case 82:	ins=(!ins);  break;

		  default:
				  break;
		  }  /* fin du switch */
		  break;

	  case 8:  /* v‚rifier si touche [del] */
		if (i>0)  /* v‚rifier si pas premiere position */
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

	  case 13:	/* v‚rifier si touche [enter] */
            retour=0;
            end=1;
            break;

	  case 27:	/* v‚rifier si touche [esc] */
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
		{	  /* v‚rifier si caractŠre correcte */
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
				}  /* fin du if i==fin || !ins */
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
			}  /* fin du if caractere>31 */
		  else
			Beep();
		  }  /* fin du default */
      }  /* fin du switch */
}
while (!end);


*(chaine+fin)=0;

if (couleur!=0)
	for (n=0;n<fin;n++)
        AffCol(colonne+n,ligne,couleur);

GotoXY(0,0);

return retour;
}  /* fin fonction lire_chaine */

int ScreenSaver(void)
{
char a;
int b;

inp(0x3DA);
inp(0x3BA);
outp(0x3C0,0);

a=getch();
if (a==0) b=getch()*256+a; else b=a;

inp(0x3DA);
inp(0x3BA);
outp(0x3C0,0x20);

return b;
}


int Wait(int x,int y,char c)
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

void Pause(int n)
{
int m;

for (m=0;m<n;m++)  {
    while ((inp(0x3DA) & 8)!=8);
    while ((inp(0x3DA) & 8)==8);
    }
}


void Beep(void)
{
}

/*****************************************************************************/

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

        for(x=x1+1;x<x2;x++) {
            AffChr(x,y1,196);
            AffChr(x,y2,196);
            }
        
        for(y=y1+1;y<y2;y++) {
            AffChr(x1,y,179);
            AffChr(x2,y,179);
            }
        break;
    case 1:
        AffChr(x1,y1,'É');
        AffChr(x2,y1,'»');
        AffChr(x1,y2,'È');
        AffChr(x2,y2,'¼');

        for(x=x1+1;x<x2;x++) {
            AffChr(x,y1,'Í');
            AffChr(x,y2,'Í');
            }

        for(y=y1+1;y<y2;y++) {
            AffChr(x1,y,'º');
            AffChr(x2,y,'º');
            }
        break;
    }
    else
    switch(type)   {
    case 0:
        AffChr(x1,y1,142);
        AffChr(x2,y1,144);
        AffChr(x1,y2,147);
        AffChr(x2,y2,149);

        for(x=x1+1;x<x2;x++) {
            AffChr(x,y1,143);
            AffChr(x,y2,148);
            }
        
        for(y=y1+1;y<y2;y++) {
            AffChr(x1,y,145);
            AffChr(x2,y,146);
            }
        break;
    case 1:
        AffChr(x1,y1,153);
        AffChr(x2,y1,152);
        AffChr(x1,y2,151);
        AffChr(x2,y2,150);

        for(x=x1+1;x<x2;x++) {
            AffChr(x,y1,148);
            AffChr(x,y2,143);
            }
        
        for(y=y1+1;y<y2;y++) {
            AffChr(x1,y,146);
            AffChr(x2,y,145);
            }
        break;
    }
return;
}
else
{
// Relief (surtout pour type==1)
for(x=x1;x<=x2;x++)
    AffCol(x,y1,10*16+3);
for(y=y1;y<=y2;y++)
    AffCol(x1,y,10*16+3);

for(x=x1+1;x<=x2;x++)
    AffCol(x,y2,10*16+1);
for(y=y1+1;y<y2;y++)
    AffCol(x2,y,10*16+1);


if (Cfg->UseFont==0)
    switch(type)   {
    case 2:
    case 3:
        AffChr(x1,y1,'Ú');
        AffChr(x2,y1,'¿');
        AffChr(x1,y2,'À');
        AffChr(x2,y2,'Ù');

        for(x=x1+1;x<x2;x++) {
            AffChr(x,y1,196);
            AffChr(x,y2,196);
            }
        
        for(y=y1+1;y<y2;y++) {
            AffChr(x1,y,179);
            AffChr(x2,y,179);
            }
        break;
    }
    else
    switch(type)   {
    case 2:
        AffChr(x1,y1,139);
        AffChr(x2,y1,138);
        AffChr(x1,y2,137);
        AffChr(x2,y2,136);

        for(x=x1+1;x<x2;x++) {
            AffChr(x,y1,134);
            AffChr(x,y2,129);
            }
        
        for(y=y1+1;y<y2;y++) {
            AffChr(x1,y,132);
            AffChr(x2,y,131);
            }
        break;
    case 3:
        AffChr(x1,y1,128);
        AffChr(x2,y1,130);
        AffChr(x1,y2,133);
        AffChr(x2,y2,135);

        for(x=x1+1;x<x2;x++) {
            AffChr(x,y1,129);
            AffChr(x,y2,134);
            }
        
        for(y=y1+1;y<y2;y++) {
            AffChr(x1,y,131);
            AffChr(x2,y,132);
            }
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
        for(x=x1;x<x1+xl;x++)
            AffChr(x,y1,196);
        break;
    case 1:
        for(x=x1;x<x1+xl;x++)
            AffChr(x,y1,196);
        break;
    }
    else
    switch(type) {
    case 0:
        for(x=x1;x<x1+xl;x++)
            AffChr(x,y1,143);
        break;
    case 1:
        for(x=x1;x<x1+xl;x++)
            AffChr(x,y1,143);
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



Cfg->Tfont[0]=179;      // Barre Verticale | with 8x8

strcpy(chaine,Fics->path);
strcat(chaine,"\\font8x8.cfg");

Cfg->UseFont=0;
if (Cfg->font==0) return;

fic=fopen(chaine,"rb");
if (fic==NULL) return;

Cfg->UseFont=1;         // utilise les fonts 8x8
Cfg->Tfont[0]=168;      // Barre Verticale | with 8x8

pol=malloc(2048);

fread(pol,2048,1,fic);

fclose(fic);

for (n=0;n<8;n++)
    Cfg->Tfont[n+1]=128+n;
for (n=0;n<8;n++)
    Cfg->Tfont[n+9]=142+n;

for (n=0;n<256;n++)  {
    MakeFont(pol+n*8,buf+n*32);
    }


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

Cfg->Tfont[0]=179;      // Barre Verticale | with 8x8

strcpy(chaine,Fics->path);
strcat(chaine,"\\font8x16.cfg");

Cfg->UseFont=0;
if (Cfg->font==0) return;

fic=fopen(chaine,"rb");
if (fic==NULL) return;

Cfg->UseFont=1;         // utilise les fonts 8x8
Cfg->Tfont[0]=168;      // Barre Verticale | with 8x8

pol=malloc(4096);

fread(pol,4096,1,fic);

fclose(fic);

for (n=0;n<8;n++)
    Cfg->Tfont[n+1]=128+n;
for (n=0;n<8;n++)
    Cfg->Tfont[n+9]=142+n;

for (n=0;n<256;n++)  {
    MakeFont(pol+n*16,buf+n*32);
    }


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

if (buf==NULL) {
//    ErrWin95();
    exit(1);
   }

memset(buf,0,s);

return buf;
}


// Crc - 32 BIT ANSI X3.66 CRC checksum files

static unsigned long int crc_32_tab[] = { /* CRC polynomial 0xedb88320 */
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

#define UPDC32(octet,crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

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
    oldcrc32 = UPDC32(c, oldcrc32);
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

// si p vaut 0 mets off
// si p vaut 1 interroge
// retourne -1 si SHIFT TAB, 1 si TAB
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
        car=Wait(0,0,0);

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
                    case 0x0F:
                        r=3;
                        break;
                    case 0x4D:
                        r=2;
                        break;
                    case 0x4B:
                        r=3;
                        break;
                    }
                break;
            }
        }


AffChr(x,y,32);
AffChr(x+lng-1,y,32);

AffChr(x+lng,y,220);
ChrLin(x+1,y+1,lng,223);

ColLin(x,y,lng,2*16+3);        // Couleur

return r;
}


// Retourne 27 si escape
// Retourne numero de la liste sinon

int WinTraite(struct Tmt *T,int nbr,struct TmtWin *F)
{
char fin;       // si =0 continue
char direct;    // direction du tab
int i;

SaveEcran();


WinCadre(F->x1,F->y1,F->x2,F->y2,0);
ColWin(F->x1+1,F->y1+1,F->x2-1,F->y2-1,10*16+1);
ChrWin(F->x1+1,F->y1+1,F->x2-1,F->y2-1,32);

PrintAt(F->x1+((F->x2-F->x1)-(strlen(F->name)))/2,F->y1,F->name);

for(i=0;i<nbr;i++)
switch(T[i].type) {
    case 0:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str);
        break;
    case 1:
        ColLin(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier),1*16+5);
        ChrLin(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier),32);
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
        WinCadre(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier)+F->x1+T[i].x,F->y1+T[i].y+3,1);
        break;
    case 5:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str);
        Puce(F->x1+T[i].x,F->y1+T[i].y,13,0);
        break;
    case 6:
        WinCadre(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier)+F->x1+T[i].x,F->y1+T[i].y+2,2);
        break;
    }

fin=0;
direct=1;
i=0;

while (fin==0) {

switch(T[i].type) {
    case 0:
    case 4:
        break;
    case 1:
        direct=InputAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str,*(T[i].entier));
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
    }

if (direct==0) fin=1;   // ENTER
if (direct==1) fin=2;   // ESC
if (direct==2) i++;
if (direct==3) i--;

if (i==-1) i=nbr-1;
if (i==nbr) i=0;

}

ChargeEcran();

if (fin==1)
    return i;

return 27;  // ESCAPE
}

// 1 -> Cancel
// 0 -> OK
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

x=(80-l)/2;     // 1-> 39, 2->39
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

// 1 -> Cancel
// 0 -> OK
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

x=(80-l)/2;     // 1-> 39, 2->39
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

// Avancement de graduation
// Renvoit le prochain
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
char defcol[48]={43,37,30, 31,22,17,  0, 0, 0, 58,58,50,
                 44,63,63, 63,63,21, 43,37,30,  0, 0, 0,
                 63,63, 0, 63,63,63, 43,37,30, 63, 0, 0,
                  0,63, 0,  0, 0,63,  0, 0, 0,  0, 0, 0};

Cfg->KeyAfterShell=0;

memcpy(Cfg->palette,defcol,48);

strcpy(Mask[0]->title,"C Style");
strcpy(Mask[0]->chaine,"asm break case cdecl char const continue default do double else enum extern far float for goto huge if int interrupt long near pascal register short signed sizeof static struct switch typedef union unsigned void volatile while @");
Mask[0]->Ignore_Case=0;
Mask[0]->Other_Col=1;

strcpy(Mask[1]->title,"Pascal Style");
strcpy(Mask[1]->chaine,"absolute and array begin case const div do downto else end external file for forward function goto if implementation in inline interface interrupt label mod nil not of or packed procedure program record repeat ");
strcat(Mask[1]->chaine,"set shl shr string then to type unit until uses var while with xor @");
Mask[1]->Ignore_Case=1;
Mask[1]->Other_Col=1;

strcpy(Mask[2]->title,"Assembler Style");
strcpy(Mask[2]->chaine,"aaa aad aam aas adc add and arpl bound bsf bsr bswap bt btc btr bts call cbw cdq clc cld cli clts cmc cmp cmps cmpxchg cwd cwde ");
strcat(Mask[2]->chaine,"daa das dec div enter esc hlt idiv imul in inc ins int into invd invlpg iret iretd jcxz jecxz jmp ");
strcat(Mask[2]->chaine,"ja jae jb jbe jc jcxz je jg jge jl jle jna jnae jnb jnbe jnc jne jng jnge jnl jnle jno jnp jns jnz jo jp jpe jpo js jz ");
strcat(Mask[2]->chaine,"lahf lar lds lea leave les lfs lgdt lidt lgs lldt lmsw lock lods loop loope loopz loopnz loopne lsl lss ");
strcat(Mask[2]->chaine,"ltr mov movs movsx movsz mul neg nop not or out outs pop popa popad push pusha pushad pushf pushfd ");
strcat(Mask[2]->chaine,"rcl rcr rep repe repz repne repnz ret retf rol ror sahf sal shl sar sbb scas ");
strcat(Mask[2]->chaine,"setae setnb setb setnae setbe setna sete setz setne setnz setl setng setge setnl setle setng setg setnle ");
strcat(Mask[2]->chaine,"sets setns setc setnc seto setno setp setpe setnp setpo sgdt ");
strcat(Mask[2]->chaine,"sidt shl shr shld shrd sldt smsw stc std sti stos str sub test verr verw wait fwait wbinvd xchg xlat xlatb xor @");
strcat(Mask[2]->chaine,"db dw dd endp ends assume");
Mask[1]->Ignore_Case=1;
Mask[1]->Other_Col=1;

strcpy(Mask[15]->title,"User Defined Style");
strcpy(Mask[15]->chaine,"ketchup killers redbug access darkangel katana ecstasy cray magic fred cobra z @");
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

Cfg->overflow=0;

Cfg->crc=0x69;
}
