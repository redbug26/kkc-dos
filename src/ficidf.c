/*--------------------------------------------------------------------*\
|- Association header - player                                        -|
\*--------------------------------------------------------------------*/
#include <dos.h>
#include <process.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#include <time.h>

#include "kk.h"
#include "win.h"

struct {
    char *Filename;
    char *Titre;
    char *Meneur;
    short ext;
    short NoDir;
    char type;  // 0: Rien de particulier
                // 1: Decompacteur
                // 2: Compacteur
    char dir[128];
    } app[50];


int nbr;	// nombre d'application
int nbrdir; //
int prem,max;


int d,fin;

char di;
int pos=0;

char a;
char chaine[132];
char col[132];

FILE *fic;
char key[8];


/*--------------------------------------------------------------------*\
|-                                                                    -|
|- Code de retour:                                                    -|
|- 0: perfect                                                         -|
|- 1: error                                                           -|
|- 2: no player for this file                                         -|
|- 3: Arret ESCape                                                    -|
|-                                                                    -|
|- kefaire: 0 lancer application + fichier                            -|
|-          1 lancer application toute seule                          -|
|-                                                                    -|
\*--------------------------------------------------------------------*/

int FicIdf(char *name,int numero,int kefaire)
{
int j,i,n,m;

int nbrappl;

a=numero;

SaveEcran();

fic=fopen(Fics->FicIdfFile,"rb");


if (fic==NULL) {
	PUTSERR("IDFEXT.RB missing");
	return 1;
	}

fread(key,1,8,fic);
if (memcmp(key,"RedBLEXU",8)) {
	PUTSERR("File IDFEXT.RB is bad");
	return 1;
	}
di=fgetc(fic);

fread(&nbr,1,2,fic);

nbrappl=0;

for (j=0;j<nbr;j++)
	{
    char n;
    char *a;

    fread(&n,1,1,fic);
    app[nbrappl].Filename=malloc(n+1);
    a=app[nbrappl].Filename;
    a[n]=0;
    fread(a,n,1,fic);

    fread(&n,1,1,fic);
    app[nbrappl].Titre=malloc(n+1);
    a=app[nbrappl].Titre;
    a[n]=0;
    fread(a,n,1,fic);

    fread(&n,1,1,fic);
    app[nbrappl].Meneur=malloc(n+1);
    a=app[nbrappl].Meneur;
    a[n]=0;
    fread(a,n,1,fic);

    fread(&(app[nbrappl].ext),2,1,fic);              // Numero de format

    fread(&(app[nbrappl].NoDir),2,1,fic);            // Numero directory

    fread(&(app[nbrappl].type),1,1,fic);             // Numero directory

    if (app[nbrappl].ext==numero)  nbrappl++;
	}

fread(&nbrdir,1,2,fic);

for(n=0;n<nbrdir;n++)
    {
	fread(col,1,128,fic);
    for (i=0;i<nbrappl;i++)
        {
        if (n==app[i].NoDir-1)
            strcpy(app[i].dir,col);
		}
	}

fclose(fic);

if (nbrappl==0)
    {
	PUTSERR("No player for this file !");
	return 2;
	}

if (nbrappl!=1)
    {
    m=((Cfg->TailleY)-nbrappl)/2;

    if (m<2) m=2;
    max=m+nbrappl;
    if (max>Cfg->TailleY-3) max=Cfg->TailleY-3;


    WinCadre(12,m-1,67,max,0);
    ColWin(13,m,66,max-1,10*16+1);
    ChrWin(13,m,66,max-1,32);

    PrintAt(37,m-1," Who? ");
    
    pos=m;
	nbr=nbrappl;

    d=14;
    fin=65;

    prem=0;

	do	{
        while (pos<m) pos=m;
        while (pos>m+nbrappl-1) pos=m+nbrappl-1;

        while (pos-prem<m) prem--;
        while (pos-prem>max-1) prem++;

        PrintAt(0,0,"%39s by %-37s",app[pos-m].Titre,app[pos-m].Meneur);

        for(n=m;n<max;n++)
            PrintAt(15,n,"%-49s",app[n-m+prem].Titre);

		for (n=d;n<=fin;n++)
            {
            col[n]=GetCol(n,pos-prem);
            AffCol(n,pos-prem,7*16+4);
            }

		a=getch();
		for (n=d;n<=fin;n++)
            AffCol(n,pos-prem,col[n]);


        if (a==0)
            {
			a=getch();
            switch(a)
                {
                case 72:
                    pos--;
                    break;
                case 80:
                    pos++;
                    break;
                case 0x47:
                    pos=m;
                    break;
                case 0x4F:
                    pos=m+nbr-1;
                    break;
                case 0x51:
                    pos+=5;
                    break;
                case 0x49:
                    pos-=5;
                    break;
                case 0x86:
                    WinMesg("Info. on dir",app[pos-m].dir);       // F12
                    break;
                }
			}
        }
    while ( (a!=27) & (a!=13) & (a!=0x8D) & (a!=0x4B) & (a!=0x4D) );
	ChargeEcran();

	if (a==27) return 3;
	n=pos-m;
	}
	else
    {
	n=0;
    a=13;
    }

strcpy(chaine,app[n].dir);
strcat(chaine,app[n].Filename);

if ((kefaire==1) | (a!=13))
    CommandLine("#%s",chaine);
    else
    CommandLine("#%s %s",chaine,name);
    

return 0;
}

/*--------------------------------------------------------------------*\
|- Code de retour:                                                    -|
|- 0: perfect                                                         -|
|- 1: error                                                           -|
|- 2: no player for this file                                         -|
\*--------------------------------------------------------------------*/

int PlayerIdf(char *name,int numero)
{
int j,i,n;

int nbrappl;

a=numero;

fic=fopen(Fics->FicIdfFile,"rb");

if (fic==NULL)
    {
	PUTSERR("IDFEXT.RB missing");
	return 1;
	}

fread(key,1,8,fic);
if (memcmp(key,"RedBLEXU",8))
    {
	PUTSERR("File IDFEXT.RB is bad");
	return 1;
	}
di=fgetc(fic);

fread(&nbr,1,2,fic);

nbrappl=0;

for (j=0;j<nbr;j++)
	{
    char n;
    char *a;

    fread(&n,1,1,fic);
    app[nbrappl].Filename=malloc(n+1);
    a=app[nbrappl].Filename;
    a[n]=0;
    fread(a,n,1,fic);

    fread(&n,1,1,fic);
    app[nbrappl].Titre=malloc(n+1);
    a=app[nbrappl].Titre;
    a[n]=0;
    fread(a,n,1,fic);

    fread(&n,1,1,fic);
    app[nbrappl].Meneur=malloc(n+1);
    a=app[nbrappl].Meneur;
    a[n]=0;
    fread(a,n,1,fic);

    fread(&(app[nbrappl].ext),2,1,fic);              // Numero de format
    fread(&(app[nbrappl].NoDir),2,1,fic);            // Numero directory

    fread(&(app[nbrappl].type),1,1,fic);             // Numero directory

    if (app[nbrappl].ext==numero)  nbrappl++;
	}

fread(&nbrdir,1,2,fic);

for(n=0;n<nbrdir;n++)
    {
	fread(col,1,128,fic);
    for (i=0;i<nbrappl;i++)
        {
        if (n==app[i].NoDir-1)
            strcpy(app[i].dir,col);
		}
	}

fclose(fic);

if (nbrappl==0)
    {
	PUTSERR("No player for this file !");
	return 2;
	}

strcpy(name,app[0].dir);
strcat(name,app[0].Filename);

return 0;
}



