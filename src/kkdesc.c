/*--------------------------------------------------------------------*\
|- Creation des .KKR                                                  -|
\*--------------------------------------------------------------------*/
#include <dos.h>
#include <direct.h>
#include <io.h>
#include <stdlib.h>
#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>

#include <time.h>

#include "idf.h"
#include "kk.h"


char *RBTitle2="Ketchup Killers Descriptor V"VERSION" / RedBug";

extern struct key K[nbrkey];

short NumExt[nbrkey];

char KKRname[256];
char Filename[256];
char Meneur[256];
char Titre[256];
unsigned long Checksum;

char buf[256];

char KKType;

int NbrFormat;

void main(short argc,char **argv)
{
char OldY;
int n;
char *path;

/*--------------------------------------------------------------------*\
|-                    Initialisation de l'ecran                       -|
\*--------------------------------------------------------------------*/

InitScreen(0);                     // Initialise toutes les donn‚es HARD

OldY=(*(char*)(0x484))+1;

path=GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--)
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }

ColWin(0,0,79,49,7);
ChrWin(0,0,79,49,32);
GotoXY(0,0);

if (argc<=1)
    {
    printf("KKDESC\n");
    printf("-------\n");
    printf("  Parameters:\n");
    printf("   filename:  file.KKR");
    exit(1);
    }


Cfg=GetMem(sizeof(struct config));
Fics=GetMem(sizeof(struct fichier));

Cfg->_4dos=0;

Fics->FicIdfFile=GetMem(256);
strcpy(Fics->FicIdfFile,path);
strcat(Fics->FicIdfFile,"\\idfext.rb");

Fics->CfgFile=GetMem(256);
strcpy(Fics->CfgFile,path);
strcat(Fics->CfgFile,"\\kkrb.cfg");

Fics->path=GetMem(256);
strcpy(Fics->path,path);

Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\kkdesc.hlp");

TXTMode(30);
NoFlash();

Font8x(16);

Cfg->TailleY=30;

SetPal(0, 43, 37, 30);
SetPal(1, 31, 22, 17);
SetPal(2, 0, 0, 0);
SetPal(3, 58, 58, 50);
SetPal(4, 44, 63, 63);
SetPal(5, 63, 63, 21);
SetPal(6,43,37,30);
SetPal(7,  0,  0,  0);
SetPal(10, 43, 37, 30);
SetPal(15, 47, 41, 34);


strcpy(KKRname,argv[1]);
for (n=strlen(KKRname);n>0;n--)  {
    if (KKRname[n]=='\\') break;
    if (KKRname[n]=='.') {
        KKRname[n]=0;
        break;
        }
    }
strcat(KKRname,".kkr");

strcpy(Filename,"*.*");


ColWin(0,0,79,(Cfg->TailleY-2),10*16+1);
ChrWin(0,0,79,(Cfg->TailleY-2),32);

WinCadre(0,2,79,(Cfg->TailleY-2),2);

ColLin(0,(Cfg->TailleY-1),80,1*16+2);
ChrLin(0,(Cfg->TailleY-1),80,32);

FileFormat();

NbrFormat=0;
strcpy(Titre,"Unknown");
strcpy(Meneur,"?");
Load();

IdfListe();

Sauve();

TXTMode(OldY);

puts(RBTitle2);
}





void Sauve(void)
{
int n;
FILE *fic;
char key[4];
char code;

char *a;
char sa;

strcpy(key,"KKRB");


fic=fopen(KKRname,"wb");
fwrite(key,4,1,fic);

code=7;     // reset
fwrite(&code,1,1,fic);

code=2;     // meneur
a=Meneur;

sa=strlen(a);
fwrite(&code,1,1,fic);
fwrite(&sa,1,1,fic);
fwrite(a,sa,1,fic);

code=4;     // checksum
fwrite(&code,1,1,fic);
fwrite(&Checksum,4,1,fic);

code=1;     // titre
a=Titre;

sa=strlen(a);
fwrite(&code,1,1,fic);
fwrite(&sa,1,1,fic);
fwrite(a,sa,1,fic);

for (n=0;n<nbrkey;n++)  {
    if (NumExt[n]==1)
        {
        code=3;     // nom du programme

        sa=strlen(Filename);
        fwrite(&code,1,1,fic);
        fwrite(&sa,1,1,fic);
        fwrite(Filename,sa,1,fic);

        code=5;
        fwrite(&code,1,1,fic);
        fwrite(&(K[n].numero),2,1,fic);
        }
    }

code=6;     // Fin de fichier
fwrite(&code,1,1,fic);

fclose(fic);
}

int Load(void)
{
char Key[4];
char Comment[255],SComment;
char STitre;
char SMeneur;
char SFilename;
short format;
int n;

char NFic;  // nombre de player que contient ce KKR

char Code;
char fin;

FILE *Fic;

fin=0;

NFic=0;

Fic=fopen(KKRname,"rb");
if (Fic==NULL) return 0;


fread(Key,4,1,Fic);
if (!strncmp(Key,"KKRB",4))
    {
    do {
    if (NFic>1)
        {
        ColWin(0,0,79,49,7);
        ChrWin(0,0,79,49,32);
        GotoXY(0,0);

        printf("KKDESC\n");
        printf("------\n");
        printf("  Parameters:\n");
        printf("   filename:  file.KKR");
        printf("\n\nYou couldn't modify THIS file !!!\n");
        exit(1);
        }
    fread(&Code,1,1,Fic);
    switch(Code)  {
        case 0:             // Commentaire (sans importance)
            fread(&SComment,1,1,Fic);
            fread(Comment,SComment,1,Fic);
            break;
        case 1:             // Code Titre
            fread(&STitre,1,1,Fic);
            Titre[STitre]=0;
            fread(Titre,STitre,1,Fic);
            break;
        case 2:             // Code Programmeur
            fread(&SMeneur,1,1,Fic);
            fread(Meneur,SMeneur,1,Fic);
            Meneur[SMeneur]=0;
            break;
        case 3:             // Code Nom du programme
            fread(&SFilename,1,1,Fic);
            Comment[SFilename]=0;
            fread(Comment,SFilename,1,Fic);
            if (strcmp(Comment,Filename)!=0)
                NFic++;
            strcpy(Filename,Comment);
            break;
        case 4:             // Checksum
            fread(&Checksum,4,1,Fic);
            break;
        case 5:             // Format
            fread(&format,2,1,Fic);

            for (n=0;n<nbrkey;n++)
                if (K[n].numero==format)
                    NumExt[n]=1;

            NbrFormat++;
            break;
        case 6:             // Fin de fichier
            fin=1;
            break;
        case 7:             // Reset
            Checksum=0;

            for (n=0;n<nbrkey;n++)
                NumExt[n]=0;

            strcpy(Titre,"?");
            strcpy(Meneur,"?");
            strcpy(Filename,"*.*");

            SFilename=strlen(Filename);
            STitre=strlen(Titre);
            SMeneur=strlen(Meneur);

            KKType=0;
            break;
        }
    }
    while(fin==0);
    Code=1;

    NbrFormat--;
    }
    else
    {
    Code=0;
    }


fclose(Fic);

return Code;
}



/**********************************************************************/

int sort_function(const void *a,const void *b)
{
struct key *a1,*b1;

a1=(struct key*)a;
b1=(struct key*)b;

// return (a1->numero)-(b1->numero);

if (a1->type!=b1->type) return (a1->type)-(b1->type);

return strcmp(a1->ext,b1->ext);     // ou format ?
}


void FileFormat(void)
{
qsort((void*)K,nbrkey,sizeof(struct key),sort_function);
}

/**********************************************************************/

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

void Desc_SortFic(struct find_t **fic,int n)
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


/**********************************************************************/
// Retourne 1 si erreur

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


olddir=GetMem(256);
curdir=GetMem(256);
buffer=GetMem(256);

SaveEcran();

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
        z=_dos_findfirst("*.*",0x10,&bfic);
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
        z=_dos_findfirst("*.*",0x10,&bfic);
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


        Desc_SortFic(fic,nbr);
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

ChargeEcran();

free(olddir);
free(curdir);
free(buffer);

if (a==27)
    return 1;
    else
    return 0;

}

/**********************************************************************/
int sortidf(const void *a,const void *b)
{
struct key *a1,*b1;

a1=(struct key*)a;
b1=(struct key*)b;

// return (a1->numero)-(b1->numero);

if (a1->type!=b1->type) return (a1->type)-(b1->type);

return strcmp(a1->ext,b1->ext);		// ou format ?
}




void IdfListe(void)
{
int car,y;

int n;
int info;

int prem;
int pres;

SaveEcran();

info=0;

y=3;

PrintAt(32,0,"Describe player");
ColLin(32,0,20,10*16+3);

PrintAt(2,(Cfg->TailleY-1),
                          "F1: Help   F3: Information   F7: Compute CRC"
                                    "   ESC: Quit & Save Modification");

prem=0;
pres=0;

do
{
if (Checksum==0)
    PrintAt(68,0,"            ");
    else
    PrintAt(68,0,"CRC:%08X",Checksum);

PrintAt(0,1,"%-38s by %38s",Titre,Meneur);

y=3;

for (n=prem;n<nbrkey;n++)
    {
    if (K[n].ext[0]=='*')
        {
        ChrLin(1,y,78,196);
        PrintAt(4,y,"%s",K[n].format);

        if (y&1==1)
            ColLin(1,y,78,10*16+3);
            else
            ColLin(1,y,78,15*16+3);
        }
        else
        {
        if (y&1==1)
            {
            ColLin(1,y,4,  10*16+3);
            ColLin(5,y,1,  10*16+3);
            ColLin(6,y,32, 10*16+4);
            ColLin(38,y,6, 10*16+3);
            ColLin(44,y,29,10*16+5);
            ColLin(73,y,1, 10*16+3);
            ColLin(74,y,5, 10*16+3);
            }
            else
            {
            ColLin(1,y,4,  15*16+3);
            ColLin(5,y,1,  15*16+3);
            ColLin(6,y,32, 15*16+4);
            ColLin(38,y,6, 15*16+3);
            ColLin(44,y,29,15*16+5);
            ColLin(73,y,1, 15*16+3);
            ColLin(74,y,5, 15*16+3);
            }

        PrintAt(1,y," %3s %-32s from %29s %4s ",K[n].ext,K[n].format,
                               K[n].pro,NumExt[n]==1 ? " OK " : "    ");

        if (K[n].other==1) info++;
        }


    if (pres==n)
        ColLin(1,y,78,2*16+1);

    y++;

    if ( (y==(Cfg->TailleY-2)) | (n==nbrkey-1) ) break;
    }


car=Wait(0,0,0);

switch(HI(car))
    {
    case 72:        // UP
        pres--;
        break;
    case 80:        // DOWN
        pres++;
        break;
    case 0x49:      // PGUP
        pres-=10;
        break;
    case 0x51:      // PGDN
        pres+=10;
        break;
    case 0x47:  // HOME
        pres=0;
        break;
    case 0X4F: // END
        pres=nbrkey-1;
        break;
    case 0x3B:
        Help();
        break;
    case 0x3D:    // F3
        SaveEcran();

        WinCadre(9,10,70,17,0);
        ChrWin(10,11,69,16,32);
        ColWin(10,11,69,16,10*16+1);
            // 2         3
            // 012345678901234567
        PrintAt(11,12,"Player Title:");
        PrintAt(11,14,"Created by ");
        PrintAt(11,16,"Filename with parameter:");

        ColLin(11,12,13,10*16+3);
        ColLin(11,14,11,10*16+3);
        ColLin(11,16,23,10*16+3);

        PrintAt(26,12,Titre);
        PrintAt(23,14,Meneur);
        PrintAt(37,16,Filename);

        InputAt(26,12,Titre,38);
        InputAt(23,14,Meneur,38);
        InputAt(37,16,Filename,18);

        ChargeEcran();
        break;
    case 0x41:      // F7  (CRC-32)
        strcpy(buf,Filename);
        if (strlen(buf)==0)
            strcpy(buf,"*.*");
        if (Seekfile(10,10,buf)==0)
            crc32file(buf,&Checksum);
        break;
    }

if (car==32)
    {
    if (K[pres].ext[0]!='*')
        NumExt[pres] = (NumExt[pres]==1) ? 0 : 1;
    }

if (pres<0) pres=0;
if (pres>=nbrkey) pres=nbrkey-1;

while (pres>prem+Cfg->TailleY-6) prem++;
while (pres<prem) prem--;

if (prem<0) prem=0;
if (prem>nbrkey-Cfg->TailleY+6) prem=nbrkey-Cfg->TailleY+6;


}
while (car!=27);

ChargeEcran();
}
