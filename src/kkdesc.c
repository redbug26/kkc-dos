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

char *VERSION_KKR;

char buf[256];

char KKType;

int NbrFormat;

/*--------------------------------------------------------------------*\
|- prototype                                                          -|
\*--------------------------------------------------------------------*/

int crc32file(char *name,unsigned long *crc);  // Compute CRC-32 of file

int AutoUpdate(void);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void main(short argc,char **argv)
{
int OldX,OldY;
int n;
char *path;

/*--------------------------------------------------------------------*\
|-                    Initialisation de l'ecran                       -|
\*--------------------------------------------------------------------*/

Cfg=GetMem(sizeof(struct config));

OldX=(*(char*)(0x44A));
OldY=(*(char*)(0x484))+1;

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;                  // Initialisation de la taille ecran

InitScreen(0);                     // Initialise toutes les donn‚es HARD

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

if (  (argc!=2) & (argc!=4) )
    {
    puts("KKDESC");
    puts("------");
    puts(" - Create or update a description:");
    puts("     KKDESC file.kkr");
    puts(" - Update a description from a previous");
    puts("     KKDESC old.kkr new.kkr version");
    exit(1);
    }


Cfg=GetMem(sizeof(struct config));
Fics=GetMem(sizeof(struct fichier));

Cfg->_4dos=0;

/*--------------------------------------------------------------------*\
|-                      Initialisation des fichiers                   -|
\*--------------------------------------------------------------------*/

SetDefaultPath(path);

Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\kkdesc.hlp");

DefaultCfg();

Cfg->TailleY=30;

TXTMode();
InitFont();

LoadPal();

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

if (argc==2)
    {
    IdfListe();
    }

if (argc==4)
    {
    strcpy(KKRname,argv[2]);
    for (n=strlen(KKRname);n>0;n--)  {
        if (KKRname[n]=='\\') break;
        if (KKRname[n]=='.') {
            KKRname[n]=0;
            break;
            }
        }
    strcat(KKRname,".kkr");

    VERSION_KKR=argv[3];
    AutoUpdate();
    }

Sauve();


Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

TXTMode();

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
            ColLin(1,y,78,1*16+3);
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
            ColLin(1,y,4,  1*16+3);
            ColLin(5,y,1,  1*16+3);
            ColLin(6,y,32, 1*16+4);
            ColLin(38,y,6, 1*16+3);
            ColLin(44,y,29,1*16+5);
            ColLin(73,y,1, 1*16+3);
            ColLin(74,y,5, 1*16+3);
            }

        PrintAt(1,y," %3s %-32s from %29s %4s ",K[n].ext,K[n].format,
                               K[n].pro,NumExt[n]==1 ? " OK " : "    ");

        if (K[n].other==1) info++;
        }


    if (pres==n)
        ColLin(1,y,78,12*16+1);

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
        InputAt(37,16,Filename,28);

        ChargeEcran();
        break;
    case 0x41:      // F7  (CRC-32)
        strcpy(buf,Filename);
        n=0;
        while(buf[n]!=0)
            if (buf[n]==32) buf[n]=0; else  n++;

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


/*--------------------------------------------------------------------*\
|---- Crc - 32 BIT ANSI X3.66 CRC checksum files ----------------------|
\*--------------------------------------------------------------------*/

static unsigned long int crc_32_tab[] = {   // CRC polynomial 0xedb88320
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


/*--------------------------------------------------------------------*\
|-  Return -1 if error, 0 in other case                               -|
\*--------------------------------------------------------------------*/

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
    oldcrc32 = (crc_32_tab[(oldcrc32^c) & 0xff]^(oldcrc32 >> 8));
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


int AutoUpdate(void)
{
strcat(Titre," ");
strcat(Titre,VERSION_KKR);

strcpy(buf,Filename);
if (strlen(buf)==0)
   return 0;

crc32file(buf,&Checksum);
return 1;
}
