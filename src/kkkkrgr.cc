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

void Search(char *nom);

char KKR_Read(FILE *Fic);
void KKR_Search(char *nom);

int posy;

char box[32768],box2[32768];



struct config *OldCfg,*NewCfg;


FILE *iardesc;



void main(int argc,char **argv)
{
char key[4];
char code;
int n;
char *path;

Redinit();
OldCfg=Cfg;

/*--------------------------------------------------------------------*\
|-  Initialisation de l'ecran                                         -|
\*--------------------------------------------------------------------*/
NewCfg=(struct config*)GetMem(sizeof(struct config));

ReadCfg(OldCfg);           //--- Lit la config courrante ---------------

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
strcat(Fics->help,"\\kkc.hlp");
*/

DefaultCfg(NewCfg);

Cfg=NewCfg;

Cfg->TailleY=50;

SetMode();

#ifndef NOFONT
InitFont();
#endif

LoadPal();

WinCadre(0,0,79,49,1);
ChrWin(1,1,78,48,32);
ColWin(1,1,78,48,0*16+1);
ColLin(1,1,78,0*16+5);
WinLine(1,2,78,0);


posy=3;


//--- debut ------------------------------------------------------------

strcpy(key,"KKRB");

iardesc=fopen("iar_desc.kkr","wb");
fwrite(key,4,1,iardesc);

code=7;                                                         // reset
fwrite(&code,1,1,iardesc);

//----------------------------------------------------------------------

KKR_Search("\\");

code=6;                                                // Fin de fichier
fwrite(&code,1,1,iardesc);

fclose(iardesc);


PrintAt(29,48,"Press a key to continue");
ColLin(1,48,78,0*16+2);

Wait(0,0);

Cfg=OldCfg;
SetMode();

if (argc);
}

char KKR_Read(FILE *Fic)
{
char Key[4];
char Comment[255],Titre[255],Meneur[255],Filename[255];
int Checksum;
short format;
char kkos,kktype;
short lenbox,lenbox2;
char sa, code, *a;

char Titre2[256],Meneur2[256],Filename2[256];
int Checksum2;
char kkos2,kktype2;


char Code;
char fin;

int gain;

fin=0;

fread(Key,4,1,Fic);
if (!strncmp(Key,"KKRB",4))
    {
    do
        {
        if (fread(&Code,1,1,Fic)==0)
            {
            WinError("Invalid KKR");
            break;
            }

        switch(Code)
            {
            case 0:                         // Commentaire (sans importance)
                fread(&sa,1,1,Fic);
                fread(Comment,sa,1,Fic);
                break;
            case 1:                                            // Code Titre
                fread(&sa,1,1,Fic);
                Titre[sa]=0;
                fread(Titre,sa,1,Fic);

                PrintAt(3,posy,"Loading information about %s",Titre);
                posy++;

                if (posy>=45) {
                    MoveText(1,4,78,46,1,3);
                    posy--;
                    ChrLin(1,46,78,32);
                    }
                break;
            case 2:                                      // Code Programmeur
                fread(&sa,1,1,Fic);
                fread(Meneur,sa,1,Fic);
                Meneur[sa]=0;
                break;
            case 3:                                 // Code Nom du programme
                fread(&sa,1,1,Fic);
                Filename[sa]=0;
                fread(Filename,sa,1,Fic);
                break;
            case 4:                                              // Checksum
                fread(&Checksum,4,1,Fic);
                break;
            case 5:                                                // Format
                fread(&format,2,1,Fic);

                gain=0;

                if (strcmp(Meneur,Meneur2)!=0)
                    gain-=(1+1+strlen(Meneur));
                else
                    gain+=(1+1+strlen(Meneur));

                if (Checksum!=Checksum2)
                    gain-=5;
                else
                    gain+=5;

                if (strcmp(Titre,Titre2)!=0)
                    gain-=(1+1+strlen(Titre));
                else
                    gain+=(1+1+strlen(Titre));

                if (strcmp(Filename,Filename2)!=0)
                    gain-=(1+1+strlen(Filename));
                else
                    gain+=(1+1+strlen(Filename));

                if (kktype!=kktype2)
                    gain-=2;
                else
                    gain+=2;

                if (kkos!=kkos2)
                    gain-=2;
                else
                    gain+=2;

                if ((memcmp(box,box2,lenbox)!=0) | (lenbox!=lenbox2))
                    gain-=lenbox;
                else
                    gain+=lenbox;

                if (gain<=0)
                    {
                    code=7;                                                         // reset
                    fwrite(&code,1,1,iardesc);

                    Checksum2=0;

                    strcpy(Titre2,"?");
                    strcpy(Meneur2,"?");
                    strcpy(Filename2,".");

                    lenbox2=0;

                    kkos2=0;
                    kktype2=0;
                    }


                if (strcmp(Meneur,Meneur2)!=0)
                    {
                    code=2;

                    a=Meneur;

                    sa=(char)strlen(a);
                    fwrite(&code,1,1,iardesc);
                    fwrite(&sa,1,1,iardesc);
                    fwrite(a,sa,1,iardesc);
                    }

                if (Checksum!=Checksum2)
                    {
                    code=4;
                    fwrite(&code,1,1,iardesc);
                    fwrite(&Checksum,4,1,iardesc);
                    }

                if (strcmp(Titre,Titre2)!=0)
                    {
                    code=1;
                    a=Titre;

                    sa=(char)strlen(a);
                    fwrite(&code,1,1,iardesc);
                    fwrite(&sa,1,1,iardesc);
                    fwrite(a,sa,1,iardesc);
                    }

                if (strcmp(Filename,Filename2)!=0)
                    {
                    code=3;
                    a=Filename;

                    sa=(char)strlen(a);
                    fwrite(&code,1,1,iardesc);
                    fwrite(&sa,1,1,iardesc);
                    fwrite(a,sa,1,iardesc);
                    }

                if (kktype!=kktype2)
                    {
                    code=8;
                    fwrite(&code,1,1,iardesc);
                    fwrite(&kktype,1,1,iardesc);
                    }

                if (kkos!=kkos2)
                    {
                    code=9;
                    fwrite(&code,1,1,iardesc);
                    fwrite(&kkos,1,1,iardesc);
                    }

                if ((memcmp(box,box2,lenbox)!=0) | (lenbox!=lenbox2))
                    {
                    code=10;
                    fwrite(&lenbox,1,2,iardesc);
                    fwrite(box,lenbox,1,iardesc);
                    }


                kkos2=kkos;
                memcpy(box2,box,lenbox);
                lenbox2=lenbox;
                strcpy(Meneur2,Meneur);
                Checksum2=Checksum;
                strcpy(Titre2,Titre);
                strcpy(Filename2,Filename);
                kktype2=kktype;

                code=5;
                fwrite(&code,1,1,iardesc);
                fwrite(&format,2,1,iardesc);

                break;
            case 6:                                        // Fin de fichier
                fin=1;
                break;
            case 7:                                                 // Reset
                Checksum=0;

                strcpy(Titre,"?");
                strcpy(Meneur,"?");
                strcpy(Filename,".");

                lenbox=0;

                kkos=0;
                kktype=0;
                break;
            case 8:
                fread(&kktype,1,1,Fic);
                break;
            case 9:
                fread(&kkos,1,1,Fic);
                break;
            case 10:
                fread(&lenbox,1,1,Fic);
                fread(box,lenbox,1,Fic);
                break;
            default:
                WinError("Invalid KKR didju");
                break;
            }
        }
    while(fin==0);
    }
    else
    {
    return 0;
    }

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
        PrintAt(1,47,"%-78s",fic.name);
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


