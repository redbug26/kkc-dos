#include <dos.h>
#include <direct.h>
#include <io.h>
#include <stdlib.h>
#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>

#include "hard.h"

#include "idf.h"

#include "help.h"

extern struct key K[nbrkey];

struct player {
    char *Meneur;
    char *Titre;
    short ext;      // Numero d'extension
    short pres;     // 0 si pas trouv‚ sinon numero du directory
    } *app[50];

short NumExt[50];

char KKRname[256];
char Filename[256];
char Meneur[256];
char Titre[256];
unsigned long Checksum;

char buf[256];

int NbrFormat;

void main(short argc,char **argv)
{
int n;
char *path;

path=GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
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

Fics->view=GetMem(256);
strcpy(Fics->view,path);
strcat(Fics->view,"\\view");

Fics->edit=GetMem(256);
strcpy(Fics->edit,path);
strcat(Fics->edit,"\\edit");

Fics->font=GetMem(256);
strcpy(Fics->font,path);
strcat(Fics->font,"\\font.cfg");

Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\kk_desc.hlp");

TXTMode(50);
NoFlash();

Font();

Cfg->TailleY=50;

SetPal(0, 43, 37, 30);
SetPal(1, 31, 22, 17);
SetPal(2, 0, 0, 0);
SetPal(3, 58, 58, 50);
SetPal(4, 44, 63, 63);
SetPal(5, 63, 63, 21);
SetPal(6,43,37,30);
SetPal(7,  0,  0,  0);

SetPal(10, 43, 37, 30);


ColWin(0,0,79,49,7);
ChrWin(0,0,79,49,32);
GotoXY(0,0);

if (argc<=1) {
    printf("KK_DESC\n");
    printf("-------\n");
    printf("  Parameters:\n");
    printf("   filename:  file.KKR");
    exit(1);
    }

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

for(n=0;n<50;n++)  {
    app[n]=malloc(sizeof(struct player));
    app[n]->Meneur=malloc(255);
    strcpy(app[n]->Meneur,"?");
    app[n]->Titre=malloc(255);
    strcpy(app[n]->Titre,"?");
    }


WinCadre(0,0,79,49,0);
ColWin(1,1,78,48,10*16+1);
ChrWin(1,1,78,48,32);

WinLine(1,3,76,0);

// ChrWin(1,3,78,47,176);  // '°'

FileFormat();

NbrFormat=0;
strcpy(Titre,"Unknow");
strcpy(Meneur,"?");
Load();

Insere();

Sauve();

getch();

TXTMode(50);
}




void Insere(void)
{
#define MAXFOR 21

char car1,car2;
int o,n;
char fin;
char aff;

int i,i0,i1;

i0=0;

aff=1;
fin=0;
n=0;
o=0;


do {
i1=NbrFormat;
if ((i1-i0)>=MAXFOR) i1=i0+MAXFOR;

if (aff==1)  {
    WinCadre(0,0,79,49,1);
    ColWin(1,1,78,48,10*16+1);
    ChrWin(1,1,78,48,32);

    WinLine(1,2,78,0);

    WinLine(1,46,78,0);

    PrintAt(2,47,"Arrow: Change format       [F3] Details");
    PrintAt(2,48,"[ESC] Quit Selection       [F7] Compute CRC-32");


    PrintAt(2,1,"%s by %s",Titre,Meneur);
    aff=0;
    }



for(i=i0;i<=i1;i++)  {
    int a;
    a=((i-i0)*2)+3;
    n=NumExt[i];

    ColLin(38,a,40,10*16+3); //  "Info" : "----"

    ColLin(2,a+1,17,10*16+3); // ÀÄÄÄ> Filename:

    ColLin(19,a+1,59,1*16+2);  // filename

    ColLin(6,a,32,10*16+2);   // format

    ColLin(2,a,4,10*16+5);    // EXT

    PrintAt(2,a,"%3s %-32s %4s",K[n].ext,K[n].format,K[n].other==1 ? "Info" : "----");
    PrintAt(2,a+1," ÀÄÄÄ> Filename: \"%s\" %08X",Filename,Checksum);
    }

n=NumExt[o];

ColWin(2,(o*2)+3,77,(o*2)+4,3*16+7);
car1=getch();

switch(car1) {
    case 0:
        car2=getch();
        switch(car2) {
            case 0x4B:    // gauche
                n--;
                if (n==-1)
                    n=nbrkey-1;
                break;
            case 0x4D:    // droite
                n++;
                if (n==nbrkey)
                    n=0;
                break;
            case 80:      // bas
                o++;
                if (o>NbrFormat) {
                    if (o<MAXFOR) {
                        NbrFormat++;
                        NumExt[o]=n;
                        }
                        else
                        o=0;
                    }
                n=NumExt[o];
                break;
            case 72:      // haut
                o--;
                if (o==-1)
                    o=NbrFormat;
                n=NumExt[o];
                break;
            case 0x3B:
                Help();
                break;
            case 0x3D:    // F3
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

                aff=1;
                break;
            case 0x41:      // F7  (CRC-32)
                strcpy(buf,Filename);
                if (strlen(buf)==0)
                    strcpy(buf,"*.*");
                if (Seekfile(10,10,buf)==0)
                    crc32file(buf,&Checksum);
                break;
            case 0x53:      // DEL
                if (NbrFormat!=0) {
                    int k;
                    struct player *ptmp;
                    short Numtmp;

                    for(k=o;k<NbrFormat;k++) {
                        ptmp=app[k];
                        app[k]=app[k+1];
                        app[k+1]=ptmp;

                        Numtmp=NumExt[k];
                        NumExt[k]=NumExt[k+1];
                        NumExt[k+1]=Numtmp;
                        }
                    NbrFormat--;
                    if (o>NbrFormat) o--;
                    aff=1;
                    n=NumExt[o];
                    }
                break;
            default:
                break;
            }
        break;
    case 13:
        break;
    case 27:
        fin=1;
        break;
    default:
        break;
    }
NumExt[o]=n;
}
while(!fin);


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

for (n=0;n<=NbrFormat;n++)  {
    code=3;     // nom du programme

    sa=strlen(a);
    fwrite(&code,1,1,fic);
    fwrite(&sa,1,1,fic);
    fwrite(Filename,sa,1,fic);

    code=5;
    fwrite(&code,1,1,fic);
    fwrite(&(K[NumExt[n]].numero),2,1,fic);
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

        printf("KK_DESC\n");
        printf("-------\n");
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
            Filename[SFilename]=0;
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
            strcpy(app[NbrFormat]->Meneur,Meneur);
            strcpy(app[NbrFormat]->Titre,Titre);

            Checksum;
            app[NbrFormat]->ext=format;
            app[NbrFormat]->pres=0;

            for (n=0;n<nbrkey;n++)
                if (K[n].numero==app[NbrFormat]->ext)
                    NumExt[NbrFormat]=n;

            NbrFormat++;
            break;
        case 6:             // Fin de fichier
            fin=1;
            break;
        case 7:             // Reset
            Checksum=0;

            strcpy(Titre,"?");
            strcpy(Meneur,"?");
            strcpy(Filename,"*.*");

            SFilename=strlen(Filename);
            STitre=strlen(Titre);
            SMeneur=strlen(Meneur);
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



/*****************************************************************************/



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

/*****************************************************************************/

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


/*****************************************************************************/
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
            ColLin(x,o-z+up+1,15,1*16+5);   /* si fic[o] est selectionn‚ */
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
