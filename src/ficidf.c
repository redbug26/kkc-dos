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
#include "idf.h"

extern struct key K[nbrkey];

static struct iarapp
    {
    char *Filename;          // Max 32 char
    char *Titre;             // Max 38 char
    char *Meneur;            // Max 38 char
    signed short ext;
    short NoDir;
    char type;  /* 7 6 5 4 3 2 1 0
                   ³ ³ ³ ³ ³ ³ ³ ÀÄÄÄ  1 Unpacker
                   ³ ³ ³ ³ ³ ³ ÀÄÄÄÄÄ  2 Packer
                   ³ ³ ³ ³ ³ ÀÄÄÄÄÄÄÄ  4 Default Player
                   ³ ³ ³ ³ ÀÄÄÄÄÄÄÄÄÄ  8 Off
                   ÀÄÁÄÁÄÁÄÄÄÄÄÄÄÄÄÄÄ .. 0
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

static char chaine[132];

static FILE *fic;
static char key[8];

static char Filename[256],Titre[256],Meneur[256];

void ReadIarEntry(FILE *fic,struct iarapp *app);
int IarF0Fct(struct barmenu *bar);
int IarF2Fct(struct barmenu *bar);
void FicIdfDev(char *dest,char *chaine,char *name);

void ReadIarEntry(FILE *fic,struct iarapp *app)
{
fread(Filename,32,1,fic);
Filename[32]=0;

fread(Titre,38,1,fic);
Titre[38]=0;

fread(Meneur,38,1,fic);
Meneur[38]=0;

fread(&(app->ext),2,1,fic);    //------- Numero de format --------------
fread(&(app->NoDir),2,1,fic);  //------- Numero directory --------------
fread(&(app->type),1,1,fic);   //------- Numero type -------------------
fread(&(app->os),1,1,fic);     //------- Numero operating system -------
fread(&(app->info),1,1,fic);   //------- Information sur player --------
}

void WriteIarEntry(FILE *fic,struct iarapp *app)
{
strcpy(Filename,app->Filename);
strcpy(Titre,app->Titre);
strcpy(Meneur,app->Meneur);

fwrite(Filename,32,1,fic);
fwrite(Titre,38,1,fic);
fwrite(Meneur,38,1,fic);

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
PrintAt(0,0,"%-40sby %-*s",bar->Titre,
                              Cfg->TailleX-43,app[(bar->fct)-1].Meneur);
return 0;
}

int GesF2Fct(struct barmenu *bar)
{
FILE *fic;

(app[(bar->fct)-1].type)^=4;

fic=fopen(KKFics->FicIdfFile,"r+b");
fseek(fic,app[(bar->fct)-1].pos,SEEK_SET);
WriteIarEntry(fic,&(app[(bar->fct)-1]));
fclose(fic);

return 0;
}

int GesF3Fct(struct barmenu *bar)
{
FILE *fic;

(app[(bar->fct)-1].type)^=8;

fic=fopen(KKFics->FicIdfFile,"r+b");
fseek(fic,app[(bar->fct)-1].pos,SEEK_SET);
WriteIarEntry(fic,&(app[(bar->fct)-1]));
fclose(fic);

return 0;
}

int GesF0Fct(struct barmenu *bar)
{
int n=0;

while(K[n].numero!=app[(bar->fct)-1].ext)
    n++;

Cadre(0,0,(Cfg->TailleX)-1,5,3,Cfg->col[46],Cfg->col[47]);
Window(1,1,(Cfg->TailleX)-2,4,Cfg->col[28]);


PrintAt(1,1,"Title: %-38s",app[(bar->fct)-1].Titre);
PrintAt(1,2,"Coder: %-38s",app[(bar->fct)-1].Meneur);
PrintAt(1,3,"Filename: %-32s",app[(bar->fct)-1].Filename);

if (((app[(bar->fct)-1].type)&4)==4)
    PrintAt(50,1,"(Default for .%s)",K[n].ext);

if (((app[(bar->fct)-1].type)&8)==8)
    PrintAt(1,4,"Off");
    else
    PrintAt(1,4,"On");

if (((app[(bar->fct)-1].os)&1)==1)
    PrintAt(50,3,"(Windows only)");


return 0;
}


/*--------------------------------------------------------------------*\
|-                                                                    -|
|- Code de retour:                                                    -|
|- 0: perfect                                                         -|
|- 1: error                                                           -|
|- 2: no player for this file                                         -|
|- 3: Arret ESCape                                                    -|
|- 4: Management demand‚                                              -|
|-                                                                    -|
|- kefaire: 0 mettre l'application + fichier dans dest                -|
|-          1 mettre l'application toute seule dans dest              -|
|-          2 mettre l'application dans le buffer *name               -|
|-          3 gestion de ficidf                                       -|
|-                                                                    -|
\*--------------------------------------------------------------------*/

int FicIdf(char *dest,char *name,int numero,int kefaire)
{
char version;
char type;

int j,i,n;
MENU menu;
static struct barmenu bar[50];
int res=2;

int nbrappl,nbrdir;

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

fread(&version,1,1,fic);

nbrappl=0;
nbrdir=0;

do
	{
    fread(&type,1,1,fic);

    switch(type)
        {
        case 1:
            app[nbrappl].pos=ftell(fic);
            ReadIarEntry(fic,&(app[nbrappl]));

            if ( (app[nbrappl].ext==numero) &
                 ( (((app[nbrappl].type)&8)==0) | (kefaire==3) ) &
                 (  (app[nbrappl].os==0) |
                    ((app[nbrappl].os==1) & (KKCfg->_Win95==1))|
                    (kefaire==3) )  )
                {
                app[nbrappl].Filename=(char*)GetMem(strlen(Filename)+1);
                strcpy(app[nbrappl].Filename,Filename);

                app[nbrappl].Meneur=(char*)GetMem(strlen(Meneur)+1);
                strcpy(app[nbrappl].Meneur,Meneur);

                app[nbrappl].Titre=(char*)GetMem(strlen(Titre)+1);
                strcpy(app[nbrappl].Titre,Titre);

                bar[nbrappl].Titre=(char*)GetMem(41);
                sprintf(bar[nbrappl].Titre," %-39s",Titre);

                bar[nbrappl].fct=nbrappl+1;
                bar[nbrappl].Help="FicIdf";

                nbrappl++;
                }
            if (nbrappl==50) break;
            break;
        case 2:
            fread(chaine,128,1,fic);
            chaine[128]=0;
            for (i=0;i<nbrappl;i++)
                {
                if (nbrdir==app[i].NoDir-1)
                    strcpy(app[i].dir,chaine);
                }
            nbrdir++;
            break;
        case 3:
            break;
        default:
            WinError("Error with FicIdf");
            type=3;
            break;
        }
    }
while(type!=3);

fclose(fic);

if (nbrappl==0)
    {
	PUTSERR("No player for this file !");
    res=0;
	}

n=0;

if (kefaire!=3)
    {
    for(i=0;i<nbrappl;i++)
        if (((app[i].type)&4)==4)
            {
            n=i;
            nbrappl=1;
            break;
            }
    }

if ( ((nbrappl!=1) | (kefaire==3)) & (res!=0) )
    {
    switch(kefaire)
        {
        case 0:
        case 1:
        case 2:
            NewEvents(IarF0Fct,"IarDes",1);
            NewEvents(IarF2Fct,"CrMenu",2);
            NewEvents(IarF5Fct," Deflt",5);
            menu.y=((Cfg->TailleY)-nbrappl)/2;
            if (menu.y<2) menu.y=2;
            menu.attr=0;
            break;
        case 3:
            NewEvents(GesF0Fct,"------",1);
            NewEvents(GesF2Fct," Deflt",2);
            NewEvents(GesF3Fct,"On/Off",3);
            menu.y=6;
            menu.attr=8;
            break;
        }

    menu.x=(Cfg->TailleX-40)/2;

    menu.cur=0;

    res=PannelMenu(bar,nbrappl,&menu);

    n=menu.cur;

    ClearEvents();
	}

if (res!=0)
    {
    strcpy(chaine,app[n].dir);
    strcat(chaine,app[n].Filename);

    if (res!=2)     // Appui sur la fleche droite
        kefaire=1;

    if (res==1)
        kefaire=3;

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
        case 3:
            strcpy(dest,app[n].Titre);
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

if (res==1)
    return 4;

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
char version;
char type;

int nbrappl,nbrdir;

char fin=0;

int j,i,n;

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

fread(&version,1,1,fic);

nbrappl=0;
nbrdir=0;

do
	{
    fread(&type,1,1,fic);

    switch(type)
        {
        case 1:
            app[nbrappl].pos=ftell(fic);
            ReadIarEntry(fic,&(app[nbrappl]));

            if ( (app[nbrappl].ext==numero) &
                 (((app[nbrappl].type)&8)==0) &
                 (  (app[nbrappl].os==0) |
                    ((app[nbrappl].os==1) & (KKCfg->_Win95==1)) )  )
                {
                app[nbrappl].Filename=(char*)GetMem(strlen(Filename)+1);
                strcpy(app[nbrappl].Filename,Filename);

                app[nbrappl].Meneur=(char*)GetMem(strlen(Meneur)+1);
                strcpy(app[nbrappl].Meneur,Meneur);

                app[nbrappl].Titre=(char*)GetMem(strlen(Titre)+1);
                strcpy(app[nbrappl].Titre,Titre);

                nbrappl++;
                }
            if (nbrappl==50) break;
            break;
        case 2:
            fread(chaine,128,1,fic);
            chaine[128]=0;
            for (i=0;i<nbrappl;i++)
                {
                if (nbrdir==app[i].NoDir-1)
                    strcpy(app[i].dir,chaine);
                }
            nbrdir++;
            break;
        case 3:
            break;
        default:
            WinError("Error with FicIdf");
            type=3;
            break;
        }
    }
while(type!=3);

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

void FicIdfMan(int curr,char *ch2)
{
FILE *fic;
MENU menu;
struct barmenu *bar;
static char buffer[256],buf2[256];
char key[8],version;
int i,j,n;
int nbrappl,nbrdir;
char fin,type;

fin=0;


do
{
if (curr>0)
    {
    n=FicIdf(buffer,buf2,K[curr].numero,3);
    if (n==2)
        {
        WinError("No player for this file");
        return;
        }

    if (n!=0)
        return;
    }
    else
    if (ch2!=NULL)
        strcpy(buffer,ch2);

bar=(struct barmenu*)GetMem(sizeof(struct barmenu)*nbrkey);

fic=fopen(KKFics->FicIdfFile,"rb");

if (fic==NULL)
    {
    PUTSERR("IDFEXT.RB missing");
    return;
    }

fread(key,1,8,fic);
if (memcmp(key,"RedBLEXU",8))
    {
    PUTSERR("File IDFEXT.RB is bad");
    return;
    }

fread(&version,1,1,fic);

nbrappl=0;
nbrdir=0;

do
    {
    fread(&type,1,1,fic);

    switch(type)
        {
        case 1:
            app[nbrappl].pos=ftell(fic);
            ReadIarEntry(fic,&(app[nbrappl]));

            if (!stricmp(Titre,buffer))
                {
                app[nbrappl].Filename=(char*)GetMem(strlen(Filename)+1);
                strcpy(app[nbrappl].Filename,Filename);

                app[nbrappl].Meneur=(char*)GetMem(strlen(Meneur)+1);
                strcpy(app[nbrappl].Meneur,Meneur);

                app[nbrappl].Titre=(char*)GetMem(strlen(Titre)+1);
                strcpy(app[nbrappl].Titre,Titre);

                bar[nbrappl].Titre=(char*)GetMem(41);

                n=0;
                while(K[n].numero!=app[nbrappl].ext)
                    n++;

                sprintf(bar[nbrappl].Titre," %-35s %3s",K[n].format,
                                                              K[n].ext);

                bar[nbrappl].fct=nbrappl+1;
                bar[nbrappl].Help="FicIdf";

                nbrappl++;
                }
            if (nbrappl==50) break;
            break;
        case 2:
            fread(chaine,128,1,fic);
            chaine[128]=0;
            for (i=0;i<nbrappl;i++)
                {
                if (nbrdir==app[i].NoDir-1)
                    strcpy(app[i].dir,chaine);
                }
            nbrdir++;
            break;
        case 3:
            break;
        default:
            WinError("Error with FicIdf");
            type=3;
            break;
        }
    }
while(type!=3);

fclose(fic);

NewEvents(GesF0Fct,"------",1);
NewEvents(GesF2Fct," Deflt",2);
NewEvents(GesF3Fct,"On/Off",3);
menu.y=6;

menu.x=(Cfg->TailleX-40)/2;

menu.attr=8;  // 2;

menu.cur=0;

if (nbrappl>0)
    if  (PannelMenu(bar,nbrappl,&menu)!=2) fin=1;

ClearEvents();

for (j=0;j<nbrappl;j++)
    {
    LibMem(bar[j].Titre);
    LibMem(app[j].Titre);
    LibMem(app[j].Meneur);
    LibMem(app[j].Filename);
    }
LibMem(bar);

curr=0;
while(K[curr].numero!=app[menu.cur].ext)
    curr++;
}
while(!fin);

}

