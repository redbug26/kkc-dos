// Help-functions

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <io.h>
#include <conio.h>

#include <time.h>

#include "win.h"
#include "kk.h"

void MainTopic(void);
void SubTopic(long z);
void Page(long z);

void Hlp2Chaine(long pos,char *chaine);

long NdxMainTopic[16];
short NbrMain;
long NdxSubTopic[16][512];
short NbrSub[16];

long lng;
char *hlp;

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

// Creation de l'index
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

// Tri de l'index

/* A faire */

// MainTopic

MainTopic();

free(hlp);
}

void MainTopic(void)
{
short x1,y1,x2,y2,max;

char chaine[256];
char car,car2;
int c;

short n;

short pos;



max=10;     // Lenght of "Main Topic"
for (n=0;n<NbrMain;n++)
    {
    Hlp2Chaine(NdxMainTopic[n],chaine);
    if (max<strlen(chaine))
        max=strlen(chaine);
    }


x1=5; // (82-max)/2;
y1=3; // ((Cfg->TailleY)-3*NbrMain)/2;

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
    car=LO(c);
    car2=HI(c);

    switch(car2)    {
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

void SubTopic(long z)
{
char chaine[256];
char car,car2;
int c;

int x1,x2,y1,y2,max;

short n,dernier;


short pos,prem;

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


void Page(long z)
{
char car,car2;
unsigned int c;

long n;

int nbrkey;

short x,y;

char type;  // 1: Centre & highlighted
            // 2: Highlighted

char chaine[256];

long avant,apres,pres;

SaveEcran();
PutCur(32,0);

WinCadre(0,1,79,(Cfg->TailleY)-2,1);
Window(1,2,78,(Cfg->TailleY)-3,10*16+1);

pres=z;

nbrkey=0;

do
{
n=pres;
avant=pres;
apres=pres;

y=1;

while(hlp[n]!=0x0A) n++;
n++;

while(1)
    {
    switch(hlp[n])      // Premier caractere
        {
        case '^':
            type=1;
            break;
        case '%':
            type=2;
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

    if (type==69)       // Autre type -> fin d'affichage
        break;

    n++;
    if (n>lng) break;   // Depassement de ligne -> fin d'affichage

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
        if (n>lng) break;       // Autre type -> fin d'affichage
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

    ChrLin(x,y,79-x,SPACE);             // Efface jusqu'a la fin

    if (type!=0)                        // Couleur de la ligne
        ColLin(1,y,78,10*16+5);
        else
        ColLin(1,y,78,10*16+1);

    n++;

    if (y==Cfg->TailleY-3)
        {
        while(hlp[apres]!=0x0A) apres++;
        apres++;
        break;  // On arrive en bas --> fin d'affichage
        }
    }

if (kbhit()!=0) nbrkey=0;

if (nbrkey==0)
    {
    c=Wait(0,0,0);
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
