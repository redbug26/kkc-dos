// Viewer

#include <stdio.h>
#include <string.h>
#include <malloc.h>


#include <io.h>
#include <direct.h>

#include <dos.h>
#include <fcntl.h>

#include "win.h"
#include "kk.h"


void View(struct fenetre *F)
{
FILE *fic;
int x,y,z,k,kl,ks;

long cur1,cur2;

long taille;

char *buffer;

int car;

char *fichier;

fichier=GetMem(256);
strcpy(fichier,F->path);
Path2Abs(fichier,F->F[F->pcur]->name);
fic=fopen(fichier,"rb");



if (fic==NULL)
    {
    PrintAt(0,0,"Error on file '%s'",fichier);
    WinError("Couldn't open file");
    free(fichier);
    return;
    }
taille=filelength(fileno(fic));

if (taille==0) return;

buffer=GetMem(32768);

SaveEcran();


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

if (kl>32768)
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
            default:
                break;
            }
        break;
    default:
        break;
    }

while (k>=taille-((((Cfg->TailleY)-7)*16))-z) k-=16;

while ((z+k)<0) k+=16;

}
while(LO(car)!=27);

ChargeEcran();

free(buffer);

free(fichier);
}
