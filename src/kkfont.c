/*--------------------------------------------------------------------*\
|- programme de gestion des font                                      -|
\*--------------------------------------------------------------------*/
#include <direct.h>
#include <i86.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <bios.h>
#include <io.h>
#include <graph.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include <signal.h>                               // For handling signal

#include <dos.h>                             // Pour redirection des I/O

#include "hard.h"

void ChgFont(char *font);
int Seekfile(int x,int y,char *name);

char savefont[16];

char DelByte(char b,char p)
{
char a[8];
int n;

a[0]=(b&1)==1;
a[1]=(b&2)==2;
a[2]=(b&4)==4;
a[3]=(b&8)==8;
a[4]=(b&16)==16;
a[5]=(b&32)==32;
a[6]=(b&64)==64;
a[7]=(b&128)==128;

for(n=p+1;n<8;n++)
    a[n-1]=a[n];
a[7]=0;

return a[0]+a[1]*2+a[2]*4+a[3]*8+a[4]*16+a[5]*32+a[6]*64+a[7]*128;
}

char InsByte(char b,char p)
{
char a[8];
int n;

a[0]=b&1;
a[1]=b&2;
a[2]=b&4;
a[3]=b&8;
a[4]=b&16;
a[5]=b&32;
a[6]=b&64;
a[7]=b&128;

for(n=p+1;n<8;n++)
    a[n-1]=a[n];
a[7]=0;

return a[0]+a[1]*2+a[2]*4+a[3]*8+a[4]*16+a[5]*32+a[6]*64+a[7]*128;
}



void ChgFont(char *font)
{
char *buf=(char*)0xA0000;
int i,n;

union REGS R;
unsigned char x;


for (n=0;n<256;n++)  {
    outpw( 0x3C4, 0x402);
    outpw( 0x3C4, 0x704);
    outpw( 0x3CE, 0x204);

    outpw( 0x3CE, 5);
    outpw( 0x3CE, 6);

    for (i=0;i<16;i++)
        *(buf+n*32+i)=*(font+n*16+i);

    outpw( 0x3C4, 0x302);
    outpw( 0x3C4, 0x304);

    outpw( 0x3CE, 4);
    outpw( 0x3CE, 0x1005);
    outpw( 0x3CE, 0xE06);
    }

R.w.bx=(8==8) ? 0x0001 : 0x0800;
x=inp(0x3CC) & (255-12);
(void) outp(0x3C2,x);
outpw( 0x3C4, 0x0100);
outpw( 0x3C4, 0x01+ (R.h.bl<<8) );
outpw( 0x3C4, 0x0300);

R.w.ax=0x1000;
R.h.bl=0x13;
int386(0x10,&R,&R);
}

char FileName[256]=".\\*.*";
int i1=0,j1=0;

void Peek(char *font)
{
FILE *fic;
int car,z;

static char pol[4096];

z=strlen(FileName)-1;
while (z>=0)
    {
    if (FileName[z]=='\\') break;
    z--;
    }
FileName[z]=0;
strcat(FileName,"\\*.*");

// erreur=Seekfile(5,5,FileName);
// if (erreur==1) return;

strcpy(FileName,"fonte39.fnt");

fic=fopen(FileName,"rb");
fread(pol,4096,1,fic);
fclose(fic);

ChgFont(pol);

do
{
AffCol(40+i1,2+j1,7*16+10);
car=Wait(0,0);
AffCol(40+i1,2+j1,10*16+7);

switch(HI(car))
    {
    case 0x8D: // CTRL-UP
    case 72:   // UP
        j1--;
        break;
    case 0x91: // CTRL-DOWN
    case 80:   // DOWN
        j1++;
        break;
    case 0x73: // CTRL-LEFT
    case 0x4B: // LEFT
        i1--;
        break;
    case 0x74: // CTRL-RIGHT
    case 0x4D: // RIGHT
        i1++;
        break;
    }

i1=i1&15;
j1=j1&15;
}
while ( (car!=27) & (car!=13) );

if (car==13)
    memcpy(font,pol+(i1+j1*16)*16,16);
}


void Font(void)
{
char fen;

static char font[4096];
int i,j;
FILE *fic;


char tx;

int n;

int cur;
char a;

int car;
int pi,pj;

fen=0;


fic=fopen("font8x16.cfg","rb");
fread(font,4096,1,fic);
fclose(fic);

PrintAt(3,20,"ALT-S: Save Font");
PrintAt(3,21,"ALT-R: Retrieve character from file");

for(i=0;i<16;i++)
    for(j=0;j<16;j++)
        AffChr(40+i,j+2,i+j*16);
WinCadre(39,1,56,18,2);

cur='a';
tx=16;

pi=0;
pj=0;

WinCadre(75,3,77,5,1);


WinCadre(1,1,10,2+tx,2);

WinCadre(39,21,78,23,2);

do
{
for(j=0;j<tx;j++)
    {
    a=font[cur*tx+j];

    for(i=7;i>=0;i--)
        {
        if (a&1==1)
            {
            AffChr(i+2,j+2,15);
            AffCol(i+2,j+2,1*16+10);
            }
        else
            {
            AffChr(i+2,j+2,135);
            AffCol(i+2,j+2,10*16+1);
            }
        a=a/2;
        }
    }

AffCol(pi+2,pj+2,2*16+4);

AffCol(40+(cur&15),2+(cur/16),7*16+10);
car=Wait(0,0);
AffCol(40+(cur&15),2+(cur/16),10*16+7);


switch(HI(car))
    {
    case 0x3C: //--- F2 ------------------------------------------------
        PrintAt(40,22,"Font saved in buffer");
        for(n=0;n<tx;n++)
            savefont[n]=font[cur*tx+n];
        break;

    case 0x3D: //--- F3 ------------------------------------------------
        PrintAt(40,22,"Font saved from buffer");
        for(n=0;n<tx;n++)
            font[cur*tx+n]=savefont[n];
        break;

   case 0x53: //--- DEL -----------------------------------------------
/*        for(n=pj;n<tx-1;n++)
            font[cur*tx+n]=font[cur*tx+n+1];
        font[cur*tx+tx-1]=0;
*/
        for(n=0;n<16;n++)
            font[cur*tx+n]=DelByte(font[cur*tx+n],7-pi);
        break;

    case 0x52: //--- INS -----------------------------------------------
        for(n=tx-1;n>pj;n--)
            font[cur*tx+n]=font[cur*tx+n-1];
        font[cur*tx+pj]=0;
        break;

    case 0x8D: // CTRL-UP
        cur-=tx;
        break;
    case 0x91: // CTRL-DOWN
        cur+=tx;
        break;
    case 0x73: // CTRL-LEFT
        cur--;
        break;
    case 0x74: // CTRL-RIGHT
        cur++;
        break;
    case 72:   // UP
        if (fen==0)  pj--; else cur-=tx;
        break;
    case 80:   // DOWN
        if (fen==0) pj++; else cur+=tx;
        break;
    case 0x4B:
        if (fen==0) pi--; else cur--;
        break;
    case 0x4D:
        if (fen==0) pi++; else cur++;
        break;
    case 0x13:  // ALT-R
        Peek(font+cur*tx);
        break;
    case 0x1F:  // ALT-S
        fic=fopen("font8x16.cfg","wb");
        fwrite(font,4096,1,fic);
        fclose(fic);
        PrintAt(40,22,"Font saved");
        break;
    case 0xF: // SHIFT-TAB
        fen--;
        break;
    }

switch(car)
    {
    case 0x19: //--- Ctrl-y --------------------------------------------


    case 9:
        fen++;
        break;
    case 32:
        font[cur*tx+pj]^=(1<<(7-pi));
        break;
    }

fen=fen&1;
cur=cur&255;
pi=pi&7;
pj=pj&(tx-1);


ChgFont(font);
}
while(car!=27);

}


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

InitScreen(0);                     // Initialise toutes les donn‚es HARD

path=GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
    }

Fics=(struct fichier*)GetMem(sizeof(struct fichier));

SetDefaultPath(path);

Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\kksetup.hlp");

DefaultCfg();

Cfg->TailleY=30;

TXTMode();

InitFont();

LoadPal(Cfg->palette);


WinCadre(0,0,79,Cfg->TailleY-1,1);
ChrWin(1,1,78,Cfg->TailleY-2,32);
ColWin(1,1,78,Cfg->TailleY-2,10*16+7);

Font();


Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

TXTMode();
}


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

int SortName(struct find_t *F1,struct find_t *F2)
{
if (stricmp(F1->name,F2->name)>0)
    return 1;
    else
    return 0;
}

int SortExt(struct find_t *F1,struct find_t *F2)
{
char e1[4],e2[4];

char a,b,c;
int j;

a=((F1->attrib & 0x10)==0x10);
b=((F2->attrib & 0x10)==0x10);
if ( a ^ b ) return b;

e1[0]=0;
j=0;

if (F1->name[0]!='.')
    while(F1->name[j]!=0)
        {
        if (F1->name[j]=='.')
            {
            strcpy(e1,F1->name+j+1);
            break;
            }
        j++;
        }

e2[0]=0;
j=0;

if (F2->name[0]!='.')
    while(F2->name[j]!=0)
        {
        if (F2->name[j]=='.')
            {
            strcpy(e2,F2->name+j+1);
            break;
            }
        j++;
        }

if (stricmp(e1,e2)>0)
    c=1;
    else
    c=0;

return c;
}

void SortFic(struct find_t **fic,int n)
{
int i,j;
struct find_t *F1,*F2;

for (i=0;i<n-1;i++)
    for (j=i;j<n;j++) {
        F1=fic[i];
        F2=fic[j];

        if (SortName(F1,F2)>0)    {
             fic[i]=F2;
             fic[j]=F1;
			 }
		}

for (i=0;i<n-1;i++)
    for (j=i;j<n;j++) {
        F1=fic[i];
        F2=fic[j];

        if (SortExt(F1,F2)>0)    {
             fic[i]=F2;
             fic[j]=F1;
			 }
        }
}

/*--------------------------------------------------------------------*\
|-  Retourne 1 si erreur                                              -|
\*--------------------------------------------------------------------*/

int Seekfile(int x,int y,char *name)
{
int up,down,top;  /*  cadre haut,bas,ligne de commande */
int a;
struct find_t **fic,bfic;
int nbr,z,o,pos;
char fini=0;

char *olddir;
char *curdir;
char *buffer;
char b1[20],b2[10],drive[10];

static char dir[128];


olddir=GetMem(256);
curdir=GetMem(256);
buffer=GetMem(256);

SaveScreen();

getcwd(olddir,256);

fic=NULL;

down=Cfg->TailleY-3;
up=y+1;
top=y-1;

WinCadre(x-1,top,x+15,down,1);
ColWin(x,top+1,x+14,down-1,10*16+1);
ChrWin(x,top+1,x+14,down-1,32);
WinLine(x,y+1,15,0);

ColLin(x+1,y,13,10*16+2);
AffChr(x,y,0x10);
AffChr(x+14,y,0x11);


while(!fini)
	{
	if (fic==NULL)
		{
        strcpy(dir,name);
        z=strlen(dir)-1;
        while (z>=0)
            {
            if (dir[z]=='\\')
                break;
            z--;
            }
        dir[z]=0;
        strcat(dir,"\\*.*");

        _splitpath(name,drive,buffer,b1,b2);

        if (strlen(drive)==0)
            {
            getcwd(buffer,256);
            if (buffer[strlen(buffer)-1]!='\\')
                strcat(buffer,"\\");
            strcat(buffer,"*.*");
            _splitpath(buffer,drive,buffer,NULL,NULL);
            }

        _makepath(name,drive,buffer,b1,b2);



		/* Compte le nombre de fichier */
		nbr=0;
        z=_dos_findfirst(dir,0x10,&bfic);
		while (z==0)
			{
            if ( ((bfic.attrib)&0x10)==0x10)  nbr++;
            z=_dos_findnext(&bfic);
			}
        z=_dos_findfirst(name,63-0x10,&bfic);
        while (z==0)
            {
            nbr++;
            z=_dos_findnext(&bfic);
            }

		/* Alloue de la m‚moire pour les fichiers */
        fic=GetMem(nbr*sizeof(void *));
        for (z=0;z<nbr;z++)
            fic[z]=GetMem(sizeof(struct find_t));

		/* Place les fichiers en m‚moire */
		o=0;

        z=_dos_findfirst(name,63-0x10,&bfic);
		while (z==0)
			{
            memcpy(fic[o],&bfic,sizeof(struct find_t));
			o++;
            z=_dos_findnext(&bfic);
			}
        z=_dos_findfirst(dir,0x10,&bfic);
		while (z==0)
	    {
            if ( ((bfic.attrib)&0x10)==0x10)
                {
                memcpy(fic[o],&bfic,sizeof(struct find_t));
				o++;
                }
            z=_dos_findnext(&bfic);
            }
        z=0;    /* Premier affich‚ dans la colonne */
        pos=0;  /* fic[pos] est selectionn‚ */


        SortFic(fic,nbr);
        }

    /* Affiche les fichiers a l'ecran */
	for (o=z;o<down-up-1+z;o++)
        {
        if (o<nbr)
			{
            char buffer[25];
            short n;
            char ext[4];
            strcpy(buffer,fic[o]->name);
            
            strcpy(ext,"");
            if (buffer[0]!='.')
                {
                for(n=0;n<strlen(buffer);n++)
                    if (buffer[n]=='.')
                        {
                        strcpy(ext,buffer+n+1);
                        buffer[n]=0;
                        }
                }

            if ( ((fic[o]->attrib)&0x10)==0x10)
                PrintAt(x,o-z+up+1,"[%-9s %-3s]",buffer,ext);
                else
                {
                strlwr(buffer);
                strlwr(ext);
                PrintAt(x,o-z+up+1," %-9s %-3s ",buffer,ext);
                }
            }
            else
            PrintAt(x,o-z+up+1,"               ");
        if (o==pos)
            {
            PrintAt(x+1,y,"%13s",fic[o]->name);
            ColLin(x,o-z+up+1,15,1*16+5);   // si fic[o] est selectionn‚
            }
            else
            ColLin(x,o-z+up+1,15,0*16+5);
        }
    a=getch();

    switch (a) {
		case 0:
            a=getch();
            switch(a) {
                case 'H':
                    if (pos>0) pos--;
                    break;
                case 'P':
                    if (pos<nbr-1) pos++;
                    break;
                case 0x49: // PGUP
                    if (pos>5) pos-=5;
                    break;
                case 0x51: // PGDN
                    if (pos<nbr-6) pos+=5;
                    break;
                case 0x47: // HOME
                    pos=0;
                    break;
                case 0x4F: // END
                    pos=nbr-1;
                    break;

			    }
			break;
        case 13:
            if ( ((fic[pos]->attrib)&0x10)==0x10)
                    {
                    chdir(fic[pos]->name);

                    _splitpath(name,buffer,buffer,b1,b2);
                    getcwd(buffer,128);
                    _makepath(name,NULL,buffer,b1,b2);

                    for (z=0;z<nbr;z++)
                        free(fic[z]);
                    free(fic);

                    fic=NULL;
                    }
                    else
                    {
                    strcpy(name,fic[pos]->name);
                    fini=1;
                    }
            break;
		case 27:
            fini=1;
            break;
		default:
            name[0]=0;
            InputAt(x+1,y,name,13);

            for (z=0;z<nbr;z++)
                free(fic[z]);
            free(fic);
            fic=NULL;
            break;
        } /* Fin switch a */

    while (pos>down-up+z-2) z++;
    while (pos<z) z--;
    } /* Fin du while(!fini); */

for (z=0;z<nbr;z++)
    free(fic[z]);
free(fic);


if (a!=27)
	{
    _splitpath(name,buffer,buffer,b1,b2);
    getcwd(buffer,128);
    _makepath(curdir,NULL,buffer,b1,b2);
	}
strcpy(name,curdir);

chdir(olddir);

LoadScreen();

free(olddir);
free(curdir);
free(buffer);

if (a==27)
    return 1;
    else
    return 0;

}
