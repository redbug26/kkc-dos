/*--------------------------------------------------------------------*\
|- programme de concat‚nation des kkr                                 -|
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

#include "hard.h"

/*--------------------------------------------------------------------*\
|- Pour Statistique;                                                  -|
\*--------------------------------------------------------------------*/
int St_App;
int St_Dir;

struct player {
    char *Filename;
    char *Meneur;
    char *Titre;
    unsigned long Checksum;
    short ext;                                     // Numero d'extension
    short pres;             // 0 si pas trouv‚ sinon numero du directory
    char type;
    char os;
    } *app[5000];

char dir[100][256];         // 50 directory diff‚rents de 128 caracteres

short nbr;              // nombre d'application lu dans les fichiers KKR

void Search(char *nom);

int posy;

char kkos,kktype;

void main(short argc,char **argv)
{
int OldX,OldY;
int n;
char *path;

Cfg=GetMem(sizeof(struct config));

OldX=(*(char*)(0x44A));
OldY=(*(char*)(0x484))+1;

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;                  // Initialisation de la taille ecran

/*--------------------------------------------------------------------*\
|-  Initialisation de l'ecran                                         -|
\*--------------------------------------------------------------------*/

InitScreen(0);                     // Initialise toutes les donn‚es HARD

path=GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
    }


Fics=GetMem(sizeof(struct fichier));

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
strcat(Fics->help,"\\kkc.hlp");

DefaultCfg();

Cfg->TailleY=50;

TXTMode();
InitFont();

LoadPal(Cfg->palette);


WinCadre(0,0,79,49,1);
ChrWin(1,1,78,48,32);
ColWin(1,1,78,48,0*16+1);
ColLin(1,1,78,0*16+5);
WinLine(1,2,78,0);


posy=3;


nbr=0;
KKR_Search("\\");

KKR_Save();


PrintAt(29,48,"Press a key to continue");
ColLin(1,48,78,0*16+2);

Wait(0,0,0);

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

TXTMode();
}

char KKR_Read(FILE *Fic)
{
char Key[4];
char Comment[255],SComment;
char Titre[255],STitre;
char Meneur[255],SMeneur;
char Filename[255],SFilename;
int Checksum;
short format;

char Code;
char fin;

fin=0;

fread(Key,4,1,Fic);
if (!strncmp(Key,"KKRB",4))
    do {
    fread(&Code,1,1,Fic);
    switch(Code)  {
        case 0:                         // Commentaire (sans importance)
            fread(&SComment,1,1,Fic);
            fread(Comment,SComment,1,Fic);
            break;
        case 1:                                            // Code Titre
            fread(&STitre,1,1,Fic);
            Titre[STitre]=0;
            fread(Titre,STitre,1,Fic);

            PrintAt(3,posy,"Loading information about %s",Titre);
            posy++;

            if (posy>=45) {
                MoveText(1,4,78,46,1,3);
                posy--;
                ChrLin(1,46,78,32);
                }
            break;
        case 2:                                      // Code Programmeur
            fread(&SMeneur,1,1,Fic);
            fread(Meneur,SMeneur,1,Fic);
            Meneur[SMeneur]=0;
            break;
        case 3:                                 // Code Nom du programme
            fread(&SFilename,1,1,Fic);
            Filename[SFilename]=0;
            fread(Filename,SFilename,1,Fic);
            break;
        case 4:                                              // Checksum
            fread(&Checksum,4,1,Fic);
            break;
        case 5:                                                // Format
            fread(&format,2,1,Fic);
            app[nbr]=GetMem(sizeof(struct player));

            app[nbr]->Filename=GetMem(SFilename+1);
            strcpy(app[nbr]->Filename,Filename);

            app[nbr]->Meneur=GetMem(SMeneur+1);
            strcpy(app[nbr]->Meneur,Meneur);

            app[nbr]->Titre=GetMem(STitre+1);
            strcpy(app[nbr]->Titre,Titre);

            app[nbr]->Checksum=Checksum;
            app[nbr]->ext=format;
            app[nbr]->pres=0;

            app[nbr]->type=kktype;
            app[nbr]->os=kkos;

            nbr++;
            break;
        case 6:                                        // Fin de fichier
            fin=1;
            break;
        case 7:                                                 // Reset
            Checksum=0;

            strcpy(Titre,"?");
            strcpy(Meneur,"?");
            strcpy(Filename,".");

            SFilename=strlen(Filename);
            STitre=strlen(Titre);
            SMeneur=strlen(Meneur);

            kkos=0;
            kktype=0;
            break;
        case 8:
            fread(&kktype,1,1,Fic);
            break;
        case 9:
            fread(&kkos,1,1,Fic);
            break;

        }
    }
    while(fin==0);
    else return 0;
return 1;
}

void KKR_Search(char *nom)
{
struct find_t fic;
char moi[128];
char ok;
FILE *Fic;


PrintAt(1,1,"%-78s",nom);

PrintAt(1,48,"%-78s",nom);

strcpy(moi,nom);
strcat(moi,"*.KKR");

if (_dos_findfirst(moi,63-_A_SUBDIR,&fic)==0)
do
    {
    ok=0;
    if ((fic.attrib&_A_SUBDIR)!=_A_SUBDIR)
        {
        strcpy(moi,nom);
        strcat(moi,fic.name);
        PrintAt(1,47,"%s",fic.name);
        Fic=fopen(moi,"rb");
        if (Fic==NULL)  {
            PrintAt(0,0,"KKR_Read (1)");
            exit(1);
            }
        ok=KKR_Read(Fic);
        fclose(Fic);
        }
    }
while (_dos_findnext(&fic)==0);

strcpy(moi,nom);
strcat(moi,"*.*");

if (_dos_findfirst(moi,_A_SUBDIR,&fic)==0)
do
	{
    if  ( (fic.name[0]!='.') & (((fic.attrib) & _A_SUBDIR)==_A_SUBDIR) )
			{
            strcpy(moi,nom);
            strcat(moi,fic.name);
            strcat(moi,"\\");
            KKR_Search(moi);
			}
	}
while (_dos_findnext(&fic)==0);
}

void KKR_Save(void)
{
int n;
FILE *fic;
char key[4];
char code;

char *a;
char sa;

strcpy(key,"KKRB");


fic=fopen("iar_desc.kkr","wb");
fwrite(key,4,1,fic);


n=0;

do {
code=7;                                                         // reset
fwrite(&code,1,1,fic);

code=2;                                                        // meneur
a=app[n]->Meneur;

sa=strlen(a);
fwrite(&code,1,1,fic);
fwrite(&sa,1,1,fic);
fwrite(a,sa,1,fic);

code=4;                                                      // checksum
fwrite(&code,1,1,fic);
fwrite(&(app[n]->Checksum),4,1,fic);

code=1;                                                         // titre
a=app[n]->Titre;

sa=strlen(a);
fwrite(&code,1,1,fic);
fwrite(&sa,1,1,fic);
fwrite(a,sa,1,fic);

code=3;                                              // Nom de programme
a=app[n]->Filename;

sa=strlen(a);
fwrite(&code,1,1,fic);
fwrite(&sa,1,1,fic);
fwrite(a,sa,1,fic);

code=8;
fwrite(&code,1,1,fic);
fwrite(&(app[n]->type),1,1,fic);

code=9;
fwrite(&code,1,1,fic);
fwrite(&(app[n]->os),1,1,fic);

do  {
    code=5;
    fwrite(&code,1,1,fic);
    fwrite(&(app[n]->ext),2,1,fic);
    n++;
    } while ( (!strcmp(app[n]->Titre,app[n-1]->Titre)) & (n<nbr) );

}
while(n<nbr);

code=6;                                                // Fin de fichier
fwrite(&code,1,1,fic);

fclose(fic);
}


