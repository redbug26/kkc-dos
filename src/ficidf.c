#include <dos.h>
#include <process.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "kk.h"
#include "win.h"

struct {
    char *Filename;
    char *Titre;
    char *Meneur;
    short ext;
    short NoDir;
    char dir[128];
    } app[20];


int nbr;	// nombre d'application
int nbrdir; //


int d,fin;

char di;
int pos=0;

char a;
char chaine[132];
char col[132];

FILE *fic;
char key[8];



// Code de retour:
// 0: perfect
// 1: error
// 2: no player for this file
// 3: Arret ESCape


int FicIdf(char *name,int numero)
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

    fread(&(app[nbrappl].ext),2,1,fic);    // Numero de format
    fread(&(app[nbrappl].NoDir),2,1,fic);   // Numero directory

    if (app[nbrappl].ext==numero)  nbrappl++;
	}

fread(&nbrdir,1,2,fic);

for(n=0;n<nbrdir;n++)	{
	fread(col,1,128,fic);
	for (i=0;i<nbrappl;i++) {
        if (n==app[i].NoDir-1)
            strcpy(app[i].dir,col);
		}
	}

fclose(fic);

if (nbrappl==0) {
	PUTSERR("No player for this file !");
	return 2;
	}

if (nbrappl!=1) {
	m=(25-nbrappl)/2;

    WinCadre(12,m-1,67,m+nbrappl,0);
    ColWin(13,m,66,m+nbrappl-1,10*16+1);
    ChrWin(13,m,66,m+nbrappl-1,32);

    PrintAt(37,m-1," Who? ");
    
    for(n=m;n<m+nbrappl;n++)
        PrintAt(15,n,app[n-m].Titre);

	pos=m;
	nbr=nbrappl;

    d=14;
    fin=65;

	do	{
        PrintAt(0,0,"%39s by %-37s",app[pos-m].Titre,app[pos-m].Meneur);
		while (pos<m) pos+=nbr;
		while (pos>m+nbr-1) pos-=nbr;

		for (n=d;n<=fin;n++)
			col[n]=GetCol(n,pos);

		for (n=d;n<=fin;n++)
            AffCol(n,pos,7*16+4);

		a=getch();
		for (n=d;n<=fin;n++)
			AffCol(n,pos,col[n]);


		if (a==0)	{
			a=getch();
			if (a==72)		pos--;
			if (a==80)		pos++;
			if (a==0x47)	pos=m;
			if (a==0x4F)	pos=m+nbr-1;
			if (a==0x51)	pos+=5;
			if (a==0x49)	pos-=5;
			}

		} while ( (a!=27) & (a!=13) );
	ChargeEcran();

	if (a==27) return 3;
	n=pos-m;
	}
	else
	n=0;


chaine[0]=di+'A';
chaine[1]=':';
chaine[2]=0;
strcpy(chaine+2,app[n].dir);
strcat(chaine,app[n].Filename);
CommandLine("#%s %s",chaine,name);

return 0;
}



