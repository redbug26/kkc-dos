#include <dos.h>
#include <direct.h>
#include <io.h>
#include <stdlib.h>
#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>

#include "hard.h"

// Pour Statistique;
int St_App;
int St_Dir;

struct player {
    char *Filename;
    char *Meneur;
    char *Titre;
    unsigned long Checksum;
    short ext;      // Numero d'extension
    short pres;     // 0 si pas trouv‚ sinon numero du directory
    } *app[5000];

char dir[50][128]; // 50 directory diff‚rents de 128 caracteres

short nbr;    // nombre d'application lu dans les fichiers KKR
short nbrdir; //



void Search(char *nom);

int posy;

void main(short argc,char **argv)
{
FILE *fic;
char chaine[255];
short n,t;

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
strcat(Fics->help,"\\kkc.hlp");

TXTMode(50);
NoFlash();

Font();

SetPal(0, 43, 37, 30);
SetPal(1, 31, 22, 17);
SetPal(2, 0, 0, 0);
SetPal(3, 58, 58, 50);
SetPal(4, 44, 63, 63);
SetPal(5, 63, 63, 21);
SetPal(6,43,37,30);
SetPal(7,  0,  0,  0);

SetPal(10, 43, 37, 30);

WinCadre(0,0,79,49,1);
ChrWin(1,1,78,48,32);
ColWin(1,1,78,48,0*16+1);
ColLin(1,1,78,0*16+5);
WinLine(1,2,78,0);




posy=3;

strcpy(chaine,"c:\\dos\\kk.bat");
fic=fopen(chaine,"wt");

strcpy(chaine,*argv);
for (n=strlen(chaine);n>0;n--) {
    if (chaine[n]=='\\') {
        chaine[n]=0;
        break;
        }
    }

fprintf(fic,"@%s\\kk.exe\n",chaine);
fprintf(fic,"@REM This file was making by KK_SETUP\n");

fclose(fic);

strcpy(chaine,"c:\\dos\\kk_desc.bat");
fic=fopen(chaine,"wt");

strcpy(chaine,*argv);
for (n=strlen(chaine);n>0;n--) {
    if (chaine[n]=='\\') {
        chaine[n]=0;
        break;
        }
    }

fprintf(fic,"@%s\\kk_desc.exe %1\n",chaine);
fprintf(fic,"@REM This file was making by KK_SETUP\n");

fclose(fic);

strcat(chaine,"\\trash");
mkdir(chaine);

nbr=0;
KKR_Search("\\");


nbrdir=0;

Search("\\*.*");

fic=fopen("idfext.rb","wb");
if (fic!=NULL) {
	fwrite("RedBLEXU",1,8,fic);

    fputc(_getdrive()-1,fic);

	t=0;

	for(n=0;n<nbr;n++)
        if (app[n]->pres!=0) t++;

	fwrite(&t,1,2,fic);

	for(n=0;n<nbr;n++)
        if (app[n]->pres!=0)  {
            char sn;
            char *a;

            a=app[n]->Filename;
            sn=strlen(a);
            fwrite(&sn,1,1,fic);
            fwrite(a,sn,1,fic);

            a=app[n]->Titre;
            sn=strlen(a);
            fwrite(&sn,1,1,fic);
            fwrite(a,sn,1,fic);

            a=app[n]->Meneur;
            sn=strlen(a);
            fwrite(&sn,1,1,fic);
            fwrite(a,sn,1,fic);

            fwrite(&(app[n]->ext),2,1,fic);    // Numero de format
            fwrite(&(app[n]->pres),2,1,fic);   // Numero directory
            }

	fwrite(&nbrdir,1,2,fic);

	for(n=0;n<nbrdir;n++)
		fwrite(dir[n],1,128,fic);

	fclose(fic);
	}



PrintAt(29,48,"Press a key to continue");
ColLin(1,48,78,0*16+2);

getch();
ColWin(1,1,78,48,0*16+1);
ChrWin(1,1,78,48,32);  // '±'

PrintAt(10,10,"Stat.");
PrintAt(10,12,"I have founded %3d applications",St_App);
PrintAt(10,13,"            in %3d directories",St_Dir);

PrintAt(10,15,"KK.BAT is now in PATH -> You could run KK from everywhere");
PrintAt(10,17,"KK_DESC.BAT is now in path -> IDEM");
PrintAt(10,16,"%s is done",chaine);

PrintAt(29,48,"Press a key to continue");
ColLin(1,48,78,0*16+2);

getch();
TXTMode(50);
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
        case 0:             // Commentaire (sans importance)
            fread(&SComment,1,1,Fic);
            fread(Comment,SComment,1,Fic);
            break;
        case 1:             // Code Titre
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
        case 2:             // Code Programmeur
            fread(&SMeneur,1,1,Fic);
            fread(Meneur,SMeneur,1,Fic);
            Meneur[SMeneur]=0;
            break;
        case 3:             // Code Nom du programme
            fread(&SFilename,1,1,Fic);
            Filename[SFilename]=0;
            fread(Filename,SFilename,1,Fic);
            break;
        case 4:             // Checksum
            fread(&Checksum,4,1,Fic);
            break;
        case 5:             // Format
            fread(&format,2,1,Fic);
            app[nbr]=malloc(sizeof(struct player));

            app[nbr]->Filename=malloc(SFilename+1);
            strcpy(app[nbr]->Filename,Filename);

            app[nbr]->Meneur=malloc(SMeneur+1);
            strcpy(app[nbr]->Meneur,Meneur);

            app[nbr]->Titre=malloc(STitre+1);
            strcpy(app[nbr]->Titre,Titre);

            app[nbr]->Checksum=Checksum;
            app[nbr]->ext=format;
            app[nbr]->pres=0;

            nbr++;
            break;
        case 6:             // Fin de fichier
            fin=1;
            break;
        case 7:             // Reset
            Checksum=0;

            strcpy(Titre,"?");
            strcpy(Meneur,"?");
            strcpy(Filename,".");

            SFilename=strlen(Filename);
            STitre=strlen(Titre);
            SMeneur=strlen(Meneur);
            break;
        }
    }
    while(fin==0);
    else return 0;
return 1;
}




void KKR_Search(char *nom2)
{
struct find_t fic;
char moi[256],nom[256];
char ok;
FILE *Fic;

char **TabRec;  // Tableau qui remplace les appels recursifs
int NbrRec;     // Nombre d'element dans le tableau

TabRec=malloc(500*sizeof(char*));
TabRec[0]=malloc(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
strcpy(nom,TabRec[NbrRec-1]);

PrintAt(1,1,"%-78s",nom);

strcpy(moi,nom);
strcat(moi,"*.KKR");

if (_dos_findfirst(moi,63-_A_SUBDIR,&fic)==0)
do
    {
    ok=0;
    if ((fic.attrib&_A_SUBDIR)!=_A_SUBDIR)  {
        strcpy(moi,nom);
        strcat(moi,fic.name);
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

free(TabRec[NbrRec-1]);
NbrRec--;

strcpy(moi,nom);
strcat(moi,"*.*");

if (_dos_findfirst(moi,_A_SUBDIR,&fic)==0)
do
    {
    if  ( (fic.name[0]!='.') & (((fic.attrib) & _A_SUBDIR) == _A_SUBDIR) )
            {
            strcpy(moi,nom);
            strcat(moi,fic.name);
            strcat(moi,"\\");

            TabRec[NbrRec]=malloc(strlen(moi)+1);
            memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
            NbrRec++;
            }
    }
while (_dos_findnext(&fic)==0);
}
while(NbrRec>0);


free(TabRec);

}


// int Allform[1024];


void Search(char *nom2)
{
struct find_t fic;
char moi[256],nom[256];
short n;
short o;
char ok;
signed long wok;

unsigned long C;

char **TabRec;  // Tableau qui remplace les appels recursifs
int NbrRec;     // Nombre d'element dans le tableau

TabRec=malloc(500*sizeof(char*));
TabRec[0]=malloc(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
o=nbrdir+1;

PrintAt(1,1,"%-78s",nom);
St_Dir++;

strcpy(nom,TabRec[NbrRec-1]);

PrintAt(1,1,"%-78s",nom);

if (_dos_findfirst(nom,63-_A_SUBDIR,&fic)==0)
do
    {
    ok=0;
    wok=-1;

    if ((fic.attrib&_A_SUBDIR)!=_A_SUBDIR)
        {
        C=0;
/*        for (n=0;n<1024;n++)
            Allform[n]=0; */

        for(n=0;n<nbr;n++)
            if (!stricmp(fic.name,app[n]->Filename))
                {
                if ( (app[n]->Checksum!=0) & (C==0) )
                    {
                    strcpy(moi,nom);
                    moi[strlen(moi)-3]=0;
                    strcat(moi,fic.name);
                    crc32file(moi,&C);
                    }
                if ((app[n]->Checksum==C) | (app[n]->Checksum==0))
                    {
                    char cont;

                    cont=1;

                    if (wok==0)
                        {
                        if (app[wok]->Checksum!=0)
                            {
                            cont=0;
                            }
                        }

                    if (cont==1)
                        {
                        strcpy(moi,nom);
                        moi[strlen(moi)-3]=0;
                        ok=1;
                        wok=n;

                        app[wok]->pres=o;
                        }
                    }
                }
        }

    if (ok==1)
        {
        St_App++;

        nbrdir=o;
        strcpy(dir[o-1],moi);

        PrintAt(3,posy,"Found %s in %s",app[wok]->Titre,dir[o-1]);
        posy++;

        if (posy>=45)
            {
            MoveText(1,4,78,46,1,3);
            posy--;
            ChrLin(1,46,78,32);
            }
        }
   }
while (_dos_findnext(&fic)==0);

free(TabRec[NbrRec-1]);
NbrRec--;

if (_dos_findfirst(nom,_A_SUBDIR,&fic)==0)
do
	{
    if  ( (fic.name[0]!='.') & (((fic.attrib) & _A_SUBDIR) == _A_SUBDIR) )
			{
			strcpy(moi,nom);
			moi[strlen(moi)-3]=0;
            strcat(moi,fic.name);
			strcat(moi,"\\*.*");

            TabRec[NbrRec]=malloc(strlen(moi)+1);
            memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
            NbrRec++;
			}
	}
while (_dos_findnext(&fic)==0);

}
while(NbrRec>0);


free(TabRec);
}



