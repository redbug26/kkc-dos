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

static struct iarapp
    {
    char *Filename;          // Max 28 char
    char *Titre;             // Max 38 char
    char *Meneur;            // Max 38 char
    signed short ext;
    short NoDir;
    char type;  /* 7 6 5 4 3 2 1 0
                   ³ ³ ³ ³ ³ ³ ³ ÀÄÄÄ  1 Unpacker
                   ³ ³ ³ ³ ³ ³ ÀÄÄÄÄÄ  2 Packer
                   ³ ³ ³ ³ ³ ÀÄÄÄÄÄÄÄ  4 Default Player
                   ÀÄÁÄÁÄÁÄÁÄÄÄÄÄÄÄÄÄ .. 0
                 */
    char os;    /* 7 6 5 4 3 2 1 0
                   ³ ³ ³ ³ ³ ³ ³ ÀÄÄÄ  1 Windows
                   ÀÄÁÄÁÄÁÄÁÄÁÄÁÄÄÄÄÄ .. 0
                 */
    char info;  /* 7 6 5 4 3 2 1 0
                   ³ ³ ³ ³ ³ ³ ³ ÀÄÄÄ  1 CRC is OK
                   ÀÄÁÄÁÄÁÄÁÄÁÄÁÄÄÄÄÄ .. 0
                 */
    char dir[128];
    int pos;
    } app[50];


static int nbr;        // nombre d'application
static int nbrdir;     //

static char chaine[132];

static FILE *fic;
static char key[8];

static char Filename[256],Titre[256],Meneur[256];

void ReadIarEntry(FILE *fic,struct iarapp *app);
int IarF0Fct(struct barmenu *bar);
int IarF2Fct(struct barmenu *bar);

void ReadIarEntry(FILE *fic,struct iarapp *app)
{
char n;

fread(&n,1,1,fic);
Filename[n]=0;
fread(Filename,n,1,fic);

fread(&n,1,1,fic);
Titre[n]=0;
fread(Titre,n,1,fic);
if (n>=38) Titre[38]=0;

fread(&n,1,1,fic);
Meneur[n]=0;
fread(Meneur,n,1,fic);

fread(&(app->ext),2,1,fic);    //------- Numero de format --------------
fread(&(app->NoDir),2,1,fic);  //------- Numero directory --------------
fread(&(app->type),1,1,fic);   //------- Numero type -------------------
fread(&(app->os),1,1,fic);     //------- Numero operating system -------
fread(&(app->info),1,1,fic);   //------- Information sur player --------
}

void WriteIarEntry(FILE *fic,struct iarapp *app)
{
char n;

strcpy(Filename,app->Filename);
n=strlen(Filename);
fwrite(&n,1,1,fic);
fwrite(Filename,n,1,fic);

strcpy(Titre,app->Titre);
n=strlen(Titre);
fwrite(&n,1,1,fic);
fwrite(Titre,n,1,fic);

strcpy(Meneur,app->Meneur);
n=strlen(Meneur);
fwrite(&n,1,1,fic);
fwrite(Meneur,n,1,fic);

fwrite(&(app->ext),2,1,fic);    //------- Numero de format -------------
fwrite(&(app->NoDir),2,1,fic);  //------- Numero directory -------------
fwrite(&(app->type),1,1,fic);   //------- Numero type ------------------
fwrite(&(app->os),1,1,fic);     //------- Numero operating system ------
fwrite(&(app->info),1,1,fic);   //------- Information sur player -------
}

int IarF5Fct(struct barmenu *bar)
{
FILE *fic;

(app[(bar->fct)-1].type)|=4;

fic=fopen(KKFics->FicIdfFile,"r+b");
fseek(fic,app[(bar->fct)-1].pos,SEEK_SET);
WriteIarEntry(fic,&(app[(bar->fct)-1]));
fclose(fic);

ungetch(13);

return 0;
}

int IarF2Fct(struct barmenu *bar)
{
int n;
char ok;

ok=1;
strcpy(Filename,app[(bar->fct)-1].Filename);
for(n=0;n<strlen(Filename);n++)
    if (Filename[n]=='!') ok=0;

if (ok==1)
    strcat(Filename," !:!\\!.!");

MenuCreat(bar->Titre+1,Filename,app[(bar->fct)-1].dir);
return 0;
}

int IarF0Fct(struct barmenu *bar)
{
PrintAt(0,0,"%40sby %-*s",bar->Titre,
                              Cfg->TailleX-43,app[(bar->fct)-1].Meneur);
return 0;
}

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
|-          2 mettre l'application dans le buffer *name               -|
|-                                                                    -|
\*--------------------------------------------------------------------*/

int FicIdf(char *dest,char *name,int numero,int kefaire)
{
int j,i,n;
MENU menu;
static struct barmenu bar[50];
int res=2;

int car,nbrappl;

car=numero;

fic=fopen(KKFics->FicIdfFile,"rb");

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
fgetc(fic);

fread(&nbr,1,2,fic);

nbrappl=0;

for (j=0;j<nbr;j++)
	{
    ReadIarEntry(fic,&(app[nbrappl]));
    app[nbrappl].pos=ftell(fic);

    if ( (app[nbrappl].ext==numero) &
          (  (app[nbrappl].os==0) |
            ((app[nbrappl].os==1) & (KKCfg->_Win95==1)) )
         )
        {
        app[nbrappl].Filename=GetMem(strlen(Filename)+1);
        strcpy(app[nbrappl].Filename,Filename);

        app[nbrappl].Meneur=GetMem(strlen(Meneur)+1);
        strcpy(app[nbrappl].Meneur,Meneur);

        app[nbrappl].Titre=GetMem(strlen(Titre)+1);
        strcpy(app[nbrappl].Titre,Titre);

        bar[nbrappl].Titre=GetMem(41);
        sprintf(bar[nbrappl].Titre," %-39s",Titre);

        bar[nbrappl].fct=nbrappl+1;
        bar[nbrappl].Help="FicIdf";

        nbrappl++;
        }
    if (nbrappl==50) break;
	}

fread(&nbrdir,1,2,fic);

for(n=0;n<nbrdir;n++)
    {
    fread(chaine,1,128,fic);
    chaine[127]=0;
    for (i=0;i<nbrappl;i++)
        {
        if (n==app[i].NoDir-1)
            strcpy(app[i].dir,chaine);
		}
    }

fclose(fic);

if (nbrappl==0)
    {
	PUTSERR("No player for this file !");
    res=0;
	}

n=0;

for(i=0;i<nbrappl;i++)
    if (((app[i].type)&4)==4)
        {
        n=i;
        nbrappl=1;
        break;
        }

if ( (nbrappl!=1) & (res!=0) )
    {
    NewEvents(IarF0Fct,"IarDes",1);
    NewEvents(IarF2Fct,"CrMenu",2);
    NewEvents(IarF5Fct," Deflt",5);

    menu.y=((Cfg->TailleY)-nbrappl)/2;

    if (menu.y<2) menu.y=2;

    menu.x=(Cfg->TailleX-40)/2;

    menu.attr=0;  // 2;

    menu.cur=0;

    res=PannelMenu(bar,nbrappl,&menu);

    n=menu.cur;

    ClearEvents();
	}

if (res!=0)
    {
    strcpy(chaine,app[n].dir);
    strcat(chaine,app[n].Filename);

    if (res!=2) kefaire=1;

    strcpy(dest,"");

    switch(kefaire)
        {
        case 0:
            FicIdfDev(dest,chaine,name);
            break;
        case 1:
            sprintf(dest,"#%s",chaine);
            break;
        case 2:
            strcpy(name,chaine);
            break;
        }
    }

for (j=0;j<nbrappl;j++)
	{
    LibMem(bar[j].Titre);
    LibMem(app[j].Titre);
    LibMem(app[j].Meneur);
    LibMem(app[j].Filename);
    }

if (nbrappl!=0)
    {
    if (res==0)
        return 3;
        else
        return 0;
    }

return 2;
}

/*--------------------------------------------------------------------*\
|- Code de retour:                                                    -|
|- 0: perfect                                                         -|
|- 1: error                                                           -|
|- 2: no player for this file                                         -|
\*--------------------------------------------------------------------*/

int PlayerIdf(char *name,int numero)
{
char fin=0;

int j,i,n;

int nbrappl;

fic=fopen(KKFics->FicIdfFile,"rb");

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
fgetc(fic);

fread(&nbr,1,2,fic);

nbrappl=0;

for (j=0;j<nbr;j++)
	{
    ReadIarEntry(fic,&(app[nbrappl]));
    app[nbrappl].pos=ftell(fic);

    if ( (app[nbrappl].ext==numero) &
          (  (app[nbrappl].os==0) |
            ((app[nbrappl].os==1) & (KKCfg->_Win95==1)) ) )
        {
        app[nbrappl].Filename=GetMem(strlen(Filename)+1);
        strcpy(app[nbrappl].Filename,Filename);

        app[nbrappl].Titre=GetMem(strlen(Titre)+1);
        strcpy(app[nbrappl].Titre,Titre);

        app[nbrappl].Meneur=GetMem(strlen(Meneur)+1);
        strcpy(app[nbrappl].Meneur,Meneur);

        nbrappl++;
        }
	}

fread(&nbrdir,1,2,fic);

for(n=0;n<nbrdir;n++)
    {
    fread(chaine,1,128,fic);
    for (i=0;i<nbrappl;i++)
        {
        if (n==app[i].NoDir-1)
            strcpy(app[i].dir,chaine);
		}
	}

fclose(fic);

if (nbrappl==0)
    {
	PUTSERR("No player for this file !");
    fin=2;
	}
    else
    {
    i=0;
    for(n=0;n<nbrappl;n++)
        if (app[n].info==1)
            i=n;

    strcpy(name,app[i].dir);
    strcat(name,app[i].Filename);
    }

for (j=0;j<nbrappl;j++)
    {
    free(app[j].Filename);
    free(app[j].Titre);
    free(app[j].Meneur);
    }

return fin;
}


void FicIdfDev(char *dest,char *chaine,char *name)
{
char ok=1;
static char buf1[256],buf2[256];
char *drive,*rep,*file,*ext;
int m,n;

for(n=0;n<strlen(chaine);n++)
    {
    if (chaine[n]=='!') ok=0;
    }

if (ok==1)
    {
    FileinPath(name,buf1);
    sprintf(dest,"#%s %s",chaine,buf1);
    return;
    }


strcpy(buf1,name);

rep=file=ext=NULL;

drive=buf1;

for(n=0;n<strlen(name);n++)
    {
    if (buf1[n]==':')
        rep=buf1+n+1,buf1[n]=0;
    if (buf1[n]=='\\') m=n;
    }

file=buf1+m+1;
buf1[m]=0;

for(n=m;n<strlen(name);n++)
    {
    if (buf1[n]=='.')
        ext=buf1+n+1,buf1[n]=0;
    }

strcat(dest,name);

m=0;
ok=0;

for(n=0;n<strlen(chaine);n++)
    {
    if (chaine[n]!='!')
        {
        buf2[m]=chaine[n];
        m++;
        }
        else
        {
        switch(chaine[n+1])
            {
            case ':':
                strcpy(buf2+m,drive);
                m+=strlen(drive);
                break;
            case '\\':
                strcpy(buf2+m,rep);
                m+=strlen(rep);
                break;
            case '.':
                strcpy(buf2+m,file);
                m+=strlen(file);
                ok=1;
                break;
            default:
                if (ok==1)
                    {
                    strcpy(buf2+m,ext);
                    m+=strlen(ext);
                    }
                    else
                    {
                    strcpy(buf2+m,file);
                    m+=strlen(file);
                    ok=1;
                    }
                break;
            }
        }
    }
buf2[m]=0;

sprintf(dest,"#%s",buf2);

return;
}


