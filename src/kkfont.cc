/*--------------------------------------------------------------------*\
|- programme de gestion desfont                                      -|
\*--------------------------------------------------------------------*/
#include <direct.h>
#include <i86.h>
#include <stdio.h>
//#include <time.h>
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

#include "deffont.h"


#define CLI()          _disable()
#define STI()          _enable()

void ChgFont(char *font);
int Seekfile(int x,int y,char *name);

void SelectMaps( unsigned char map0, unsigned char map1);

char savefont[16];

char OldFont[4096*2];



/*--------------------------------------------------------------------*\
|-  0 -> Yes                                                          -|
|-  1 -> No                                                           -|
|- -1 -> Cancel                                                       -|
\*--------------------------------------------------------------------*/
int NewWinMesg(char *title,char *msg,char info)
{
static char Buffer2[70];

int d,n,lng;
static int width;
static char length;
int ok;
char *Mesg[5];



struct Tmt T[8] = {
      { 0,4,2,NULL,NULL},                                          // OK
      { 0,4,3,NULL,NULL},                                      // CANCEL
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

ok=WinTraite(T,nbr,&F,info>>4);

for(n=0;n<d;n++)
    LibMem(Mesg[n]);

return ok;
}









void SaveFont(char *buffer)
{
int i,j;
char *scr=(char*)0xA0000;

int height;

height=16;

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

return (char)(a[0]+a[1]*2+a[2]*4+a[3]*8+a[4]*16+a[5]*32+a[6]*64+a[7]*128);
}

char InsByte(char b,char p)
{
char a[8];
int n;

a[0]=(char)(b&1);
a[1]=(char)(b&2);
a[2]=(char)(b&4);
a[3]=(char)(b&8);
a[4]=(char)(b&16);
a[5]=(char)(b&32);
a[6]=(char)(b&64);
a[7]=(char)(b&128);

for(n=p+1;n<8;n++)
    a[n-1]=a[n];
a[7]=0;

return (char)(a[0]+a[1]*2+a[2]*4+a[3]*8+a[4]*16+a[5]*32+a[6]*64+a[7]*128);
}



void ChgFont(char *buffer)
{
int i,j;
char *scr=(char*)0xA0000;
int height;

height=16;

outpw( 0x3C4, 0x402);
outpw( 0x3C4, 0x704);
outpw( 0x3CE, 0x204);

outpw( 0x3CE, 5);
outpw( 0x3CE, 6);

for (i=0;i<256;i++)
    for (j=0;j<16;j++)
        scr[i*32+j]=buffer[i*height+j];

for (i=0;i<256;i++)
    for (j=0;j<16;j++)
        scr[i*32+j+0x4000]=buffer[(i+256)*height+j];

outpw( 0x3C4, 0x302);
outpw( 0x3C4, 0x304);

outpw( 0x3CE, 4);
outpw( 0x3CE, 0x1005);
outpw( 0x3CE, 0xE06);
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

void Sauve(char *font)
{
FILE *fic;

fic=fopen("font8x16.fnt","wb");
fwrite(font,4096,1,fic);
fclose(fic);
PrintAt(40,22,"Font saved");
}


void Font(void)
{
char fen;
char etatsauve=1;

static char font[4096*2];
int i,j;
FILE *fic;


char tx;

int n;

int cur;
char a;

int car;
int pi,pj;
int n1;

fen=0;


fic=fopen("font8x16.fnt","rb");
if (fic==NULL)
    memcpy(font,deffont,4096);
    else
    {
    for(n1=0;n1<256;n1++)
        fread(font+n1*16,16,1,fic);
    fclose(fic);
    }

fic=fopen("default.fnt","rb");
if (fic==NULL)
    memcpy(font+4096,deffont,4096);
    else
    {
    fread(font+4096,4096,1,fic);
    fclose(fic);
    }
fic=fopen("a.h","wt");
for(i=0;i<256;i++)
    {
    for(j=0;j<16;j++)
        {
        fprintf(fic,"0x%02X,",font[4096+i*16+j]);
        }
    fprintf(fic,"\n");
    }
fclose(fic);



PrintAt(3,20,"ALT-S: Save Font");
PrintAt(3,21,"ALT-R: Retrieve character from file");

PrintAt(3,23,"When you run KKFONT, it loads      ");
PrintAt(3,24,"font 8x16.fnt and save it when     D: default character");
PrintAt(3,25,"you press ALT-S                    F2: save char   F3: load char");

PrintAt(3,27,"ALT to switch, SPACE to set/unset");





cur='a';
tx=16;

pi=0;
pj=0;

for(i=19;i<=31;i++)
    for(j=2;j<=13;j++)
        AffCol(i,j,8);

Window(20,3,23,6,25);
Cadre(20,3,23,6,0,25,31);

Window(21,9,22,10,25);
Cadre(20,8,23,11,1,105,111);

Window(26,4,27,5,25);
Cadre(25,3,28,6,2,105,111);

Window(26,9,27,10,25);
Cadre(25,8,28,11,3,25,31);

AffChr(30, 9,145);
AffChr(30,10,0xAE);
AffChr(30,11,145);

Window(14,13,18,17,25);
PrintAt(14,13,"ÉÍÑÍ»");
PrintAt(14,14,"º ³ º");
PrintAt(14,15,"ÇÄÁÄ¶");
PrintAt(14,16,"º ³ º");
PrintAt(14,17,"ÈÍÍÍ¼");

Window(20,13,24,17,25);
PrintAt(20,13,"ÛÛÛÛÛ");
PrintAt(20,14,"Û   Û");
PrintAt(20,15,"ÛßßßÛ");
PrintAt(20,16,"Û   Û");
PrintAt(20,17,"ßßßßß");

Window(26,13,30,17,25);
PrintAt(26,13,"ÚÄÂ ¿");
PrintAt(26,14,"³ ³ ³");
PrintAt(26,15,"³ÚÁ¿³");
PrintAt(26,16,"³   ³");
PrintAt(26,17,"ÀÀÄÙÙ");

Window(33,14,35,16,25);
Cadre(32,13,36,17,1,25,31);
AffChr(34,14,168);
AffChr(34,15,168);
WinLine(33,16,3,1);



WinCadre(1,1,10,2+tx,2);

WinCadre(39,21,78,23,2);

WinCadre(39,1,56,18,2);
for(i=0;i<16;i++)
    for(j=0;j<16;j++)
        {
        AffChr(40+i,j+2,i+j*16);
        AffCol(40+i,j+2,31);
        }

WinCadre(59,1,76,18,2);
for(i=0;i<16;i++)
    for(j=0;j<16;j++)
        {
        AffChr(60+i,j+2,i+j*16);
        AffCol(60+i,j+2,7); // 7);
        }


SelectMaps(1,0);


do
{
ChgFont(font);

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
        a=(char)(a/2);
        }
    }

AffCol(pi+2,pj+2,2*16+4);

AffCol(60+(cur&15),2+(cur/16),5);
AffCol(40+(cur&15),2+(cur/16),25);
car=Wait(0,0);
AffCol(60+(cur&15),2+(cur/16),7);
AffCol(40+(cur&15),2+(cur/16),31);




switch(HI(car))
    {
    case 0x3C: //--- F2 ------------------------------------------------
        PrintAt(40,22,"Font saved in buffer");
        for(n=0;n<tx;n++)
            savefont[n]=font[cur*tx+n];
        break;

    case 0x3D: //--- F3 ------------------------------------------------
        etatsauve=0;
        PrintAt(40,22,"Font saved from buffer");
        for(n=0;n<tx;n++)
            font[cur*tx+n]=savefont[n];
        break;

   case 0x53: //--- DEL -----------------------------------------------
        etatsauve=0;
/*        for(n=pj;n<tx-1;n++)
            font[cur*tx+n]=font[cur*tx+n+1];
        font[cur*tx+tx-1]=0;
*/
        for(n=0;n<16;n++)
            font[cur*tx+n]=DelByte(font[cur*tx+n],(char)(7-pi));
        break;

    case 0x52: //--- INS -----------------------------------------------
        etatsauve=0;
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
        Sauve(font);

        break;
    case 0xF: // SHIFT-TAB
        fen--;
        break;
    }

switch(car)
    {
    case 'l':
    case 'L':
        if (etatsauve==0)
            if (WinMesg("Save Font","Do you want save the font ?",1)==0)
                Sauve(font);

        fic=fopen("copie.rbt","rb");
        for(n1=0;n1<256;n1++)
            {
            fread(font+n1*16,16,1,fic);
            fseek(fic,16,SEEK_CUR);
            }
        fclose(fic);
        break;
    case 'D':
    case 'd':
        etatsauve=0;
        for(n=0;n<tx;n++)
            font[cur*tx+n]=font[cur*tx+n+4096];
        break;

    case 0x19: //--- Ctrl-y --------------------------------------------

    case 9:
        fen++;
        break;
    case 32:
        etatsauve=0;
        font[cur*tx+pj]^=(char)(1<<(7-pi));
        break;
    }

fen=(char)(fen&1);
cur=cur&255;
pi=pi&7;
pj=pj&(tx-1);

if (car==27)
    {
    switch(NewWinMesg("Save Font","Do you want save the font ?",1))
        {
        case 0:
            Sauve(font);
            break;
        case 1:
            break;
        default:
            car=0;
            break;
        }
    }


}
while(car!=27);



}


void main(int argc,char **argv)
{
int OldX,OldY;                         // To save the size of the screen
int n;

char *path;

Redinit();

SaveFont(OldFont);

/*--------------------------------------------------------------------*\
|-  Initialisation de l'ecran                                         -|
\*--------------------------------------------------------------------*/

OldX=(*(char*)(0x44A));
OldY=(*(char*)(0x484))+1;

InitScreen(0);                     // Initialise toutes les donn‚es HARD

path=(char*)GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
    }

Fics=(struct fichier*)GetMem(sizeof(struct fichier));

SetDefaultPath(path);

/*
Fics->help=(char*)GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\kksetup.hlp");
*/

DefaultCfg(Cfg);

Cfg->TailleY=30;

SetMode();

#ifndef NOFONT
InitFont();
#endif

LoadPal();

WinCadre(0,0,79,Cfg->TailleY-1,1);
ChrWin(1,1,78,Cfg->TailleY-2,32);
ColWin(1,1,78,Cfg->TailleY-2,10*16+7);

Font();


Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

SetMode();

if (argc);;
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


olddir=(char*)GetMem(256);
curdir=(char*)GetMem(256);
buffer=(char*)GetMem(256);

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
        fic=(struct find_t **)GetMem(nbr*sizeof(void *));
        for (z=0;z<nbr;z++)
            fic[z]=(struct find_t*)GetMem(sizeof(struct find_t));

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


void SelectMaps( unsigned char map0, unsigned char map1)
{
union REGS   Regs;             // Registres pour g‚rer les interruptions

Regs.w.ax = 0x1103;              // Registre de s‚lection des caractŠres
Regs.h.bl = (char)(( map0 & 3 ) + ( ( map0 & 4 ) << 2 ) +
              ( ( map1 & 3 ) << 2 ) + ( ( map1 & 4 ) << 3 ));
int386( 0x10, &Regs, &Regs );        // D‚clenche l'interruption du BIOS
}
