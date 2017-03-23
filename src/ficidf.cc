/*--------------------------------------------------------------------*\
|- Association header - player										  -|
\*--------------------------------------------------------------------*/
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "kk.h"
#include "idf.h"

#ifdef LINUX
#else
    #include <dos.h>
    #include <process.h>
    #include <conio.h>
#endif

#include "filesetu.h"

static struct iarapp
	{
	char *Filename; 		 // Max 32 char
	char *Titre;			 // Max 38 char
	char *Meneur;			 // Max 38 char
	signed short ext;
	short NoDir;
	char type;	/* 7 6 5 4 3 2 1 0
				   ³ ³ ³ ³ ³ ³ ³ ÀÄÄÄ  1 Unpacker
				   ³ ³ ³ ³ ³ ³ ÀÄÄÄÄÄ  2 Packer
				   ³ ³ ³ ³ ³ ÀÄÄÄÄÄÄÄ  4 Default Player
				   ³ ³ ³ ³ ÀÄÄÄÄÄÄÄÄÄ  8 Off
				   ÀÄÁÄÁÄÁÄÄÄÄÄÄÄÄÄÄÄ .. 0
				 */
	char os;	/* 7 6 5 4 3 2 1 0
				   ³ ³ ³ ³ ³ ³ ³ ÀÄÄÄ  1 Windows
				   ÀÄÁÄÁÄÁÄÁÄÁÄÁÄÄÄÄÄ .. 0
				 */
	char info;	/* 7 6 5 4 3 2 1 0
				   ³ ³ ³ ³ ³ ³ ³ ÀÄÄÄ  1 CRC is OK
				   ÀÄÁÄÁÄÁÄÁÄÁÄÁÄÄÄÄÄ .. 0
				 */
	char dir[128];
	char *box;
	short numbox;
	int pos;
	} app[50];

static int nbrappl;
static struct barmenu bar[50];
static int max;

static char chaine[132];

static FILE *fic;
static char clef[8];

char rbbox[32768];

static char Filename[256],Titre[256],Meneur[256];

void ReadIarEntry(FILE *fic,struct iarapp *app);
int IarF0Fct(struct barmenu *bar);
int IarF2Fct(struct barmenu *bar);
void FicIdfDev(char *dest,char *chaine,char *name);

void AddOther(char *fichier);

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
fread(&(app->numbox),2,1,fic); //------- Numero de la box --------------
fread(&(app->type),1,1,fic);   //------- Numero type -------------------
fread(&(app->os),1,1,fic);	   //------- Numero operating system -------
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

fwrite(&(app->ext),2,1,fic);	//------- Numero de format -------------
fwrite(&(app->NoDir),2,1,fic);	//------- Numero directory -------------
fwrite(&(app->numbox),2,1,fic); //------- Numero de la box -------------
fwrite(&(app->type),1,1,fic);	//------- Numero type ------------------
fwrite(&(app->os),1,1,fic); 	//------- Numero operating system ------
fwrite(&(app->info),1,1,fic);	//------- Information sur player -------
}

int IarF5Fct(struct barmenu *bar)
{
FILE *fic;

if (bar->Help!=501)
	return 0;

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

if (bar->Help!=501)
	return 0;

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
if (bar->Help!=501)
	{
	ColLin(0,0,Cfg->TailleX,Cfg->col[7]);
	PrintAt(0,0,"%-*s",Cfg->TailleX,bar->Titre);
	return 0;
	}

ColLin( 0,0,40,Cfg->col[7]);
ColLin(40,0,(Cfg->TailleX)-40,Cfg->col[11]);
PrintAt(0,0,"%-40sby %-*s",bar->Titre,
							  Cfg->TailleX-43,app[(bar->fct)-1].Meneur);
return 0;
}

int GesF2Fct(struct barmenu *bar)
{
FILE *fic;

if (bar->Help!=501)
	return 0;

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

if (bar->Help!=501)
	return 0;

(app[(bar->fct)-1].type)^=8;

fic=fopen(KKFics->FicIdfFile,"r+b");
fseek(fic,app[(bar->fct)-1].pos,SEEK_SET);
WriteIarEntry(fic,&(app[(bar->fct)-1]));
fclose(fic);

return 0;
}

int GesF0Fct(struct barmenu *bar)
{
char buffer[256];

int n=0;

if (bar->Help!=501)
	{
	ColLin(0,0,Cfg->TailleX,Cfg->col[7]);
	PrintAt(0,0,"%-*s",Cfg->TailleX,bar->Titre);
	return 0;
	}

ColLin( 0,0,40,Cfg->col[7]);
ColLin(40,0,(Cfg->TailleX)-40,Cfg->col[11]);

while(K[n].numero!=app[(bar->fct)-1].ext)
	n++;

Cadre(0,0,(Cfg->TailleX)-1,5,3,Cfg->col[46],Cfg->col[47]);
Window(1,1,(Cfg->TailleX)-2,4,Cfg->col[28]);


PrintAt(1,1,"Title: %-38s",app[(bar->fct)-1].Titre);
PrintAt(1,2,"Coder: %-38s",app[(bar->fct)-1].Meneur);
PrintAt(1,3,"Filename: %-32s",app[(bar->fct)-1].Filename);
PrintAt(50,3,"%-3s",(((app[(bar->fct)-1].type)&8)==8) ? "Off" : "On" );

if (((app[(bar->fct)-1].type)&4)==4)
	PrintAt(50,1,"Default for .%s",K[n].ext);

strcpy(buffer,app[(bar->fct)-1].dir);
buffer[72]=0;

PrintAt(1,4,"Path: %s",buffer);

if (((app[(bar->fct)-1].os)&1)==1)
	PrintAt(50,2,"Windows only");


return 0;
}

//--- Rajoute les fichiers qui ont la meme extension (kksetup.ini) -----

void AddOther(char *fichier)
{
char ficext[256];
char buffer[256],*ext;
char exten[256];
FILE *fic;
char *Filename,*Titre;
int n,m;


FileinPath(fichier,buffer);
ext=strchr(buffer,'.');
if (ext==NULL) return;

ext++;

strcpy(ficext,KKFics->trash);
Path2Abs(ficext,"kkc.ext");

fic=fopen(ficext,"rt");
if (fic==NULL) return;

while(fgets(ficext,256,fic)!=NULL)
{
m=0;
for(n=0;n<strlen(ficext);n++)
	{
	if ((ficext[n]!=32) & (ficext[n]!=':'))
		{
		exten[m]=ficext[n];
		m++;
		}
		else
		if (m!=0)
			break;
	}
exten[m]=0;

if (!stricmp(exten,ext))
	{
	ficext[strlen(ficext)-1]=0;

	Filename=strchr(ficext,':')+1;
	while ((Filename[0]!=0)&(Filename[0]==32)) Filename++;

	if (strlen(Filename)>0)
		{
		Titre=ficext;

		app[nbrappl].Filename=(char*)GetMem(strlen(Filename)+1);
		strcpy(app[nbrappl].Filename,Filename);

		app[nbrappl].Meneur=(char*)GetMem(2);
		strcpy(app[nbrappl].Meneur,"?");

		app[nbrappl].Titre=(char*)GetMem(strlen(Titre)+1);
		strcpy(app[nbrappl].Titre,Titre);

		bar[nbrappl].Titre=(char*)GetMem(strlen(Filename)+2);
		sprintf(bar[nbrappl].Titre," %s",Filename);

		if (strlen(bar[nbrappl].Titre)>max)
			max=strlen(bar[nbrappl].Titre);

		bar[nbrappl].fct=nbrappl+1;
		bar[nbrappl].Help=0;

		nbrappl++;
		}
	}
if (nbrappl==50) break;
}

fclose(fic);
}


/*--------------------------------------------------------------------*\
|-																	  -|
|- Code de retour:													  -|
|- 0: perfect														  -|
|- 1: error 														  -|
|- 2: no player for this file										  -|
|- 3: Arret ESCape													  -|
|- 4: Management demand‚											  -|
|-																	  -|
|- kefaire: 0 mettre l'application + fichier dans dest                -|
|-			1 mettre l'application toute seule dans dest              -|
|-			2 mettre l'application dans le buffer *name               -|
|-			3 gestion de ficidf 									  -|
|-																	  -|
\*--------------------------------------------------------------------*/

int FicIdf(char *dest,char *name,int numero,int kefaire)
{
char version;
char type;
short lenbox;
char lendir;

int j,i,n;
MENU menu;

int res=2;

int nbrdir;
int nbrbox;

max=40; //--- Longueur maximum dans cadre ------------------------------

fic=fopen(KKFics->FicIdfFile,"rb");

if (fic==NULL)
	{
	PUTSERR("IDFEXT.RB missing");
	return 1;
	}

fread(clef,1,8,fic);
if (memcmp(clef,"RedBLEXU",8))
	{
	PUTSERR("File IDFEXT.RB is bad");
	return 1;
	}

fread(&version,1,1,fic);

if (version!=2)
	{
	PUTSERR("Version of IDFEXT.RB is incorrect");
	return 1;
	}


nbrappl=0;
nbrdir=0;
nbrbox=0;

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
				 (	(app[nbrappl].os==0) |
					((app[nbrappl].os==1) & (KKCfg->_Win95==1))|
					(kefaire==3) )	)
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
				bar[nbrappl].Help=501;

				nbrappl++;
				}
			if (nbrappl==50) break;
			break;
		case 2:
			fread(&lendir,1,1,fic);
			fread(chaine,lendir,1,fic);
			chaine[lendir]=0;
			for (i=0;i<nbrappl;i++)
				{
				if (nbrdir==app[i].NoDir-1)
					strcpy(app[i].dir,chaine);
				}
			nbrdir++;
			break;
		case 3:
			break;
		case 4:
			fread(&lenbox,2,1,fic);
			fread(rbbox,lenbox,1,fic);

			for (i=0;i<nbrappl;i++)
				{
				if (nbrbox==app[i].numbox-1)
					{
					app[i].box=(char*)GetMem(lenbox);
					memcpy(app[i].box,rbbox,lenbox);
					}
				}
			nbrbox++;
			break;

		default:
			PrintAt(0,0,"Error %d",type);
			WinError("Error with FicIdf");
			type=3;
			break;
		}
	}
while(type!=3);

fclose(fic);

if (name[0]!=0)
	AddOther(name);

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

	menu.x=(Cfg->TailleX-max)/2;

	menu.cur=0;

	res=PannelMenu(bar,nbrappl,&menu);

	n=menu.cur;

	ClearEvents();
	}

if (res!=0)
	{
	strcpy(chaine,app[n].dir);
	strcat(chaine,app[n].Filename);

	if (res!=2) 	// Appui sur la fleche droite
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
			for(j=0;j<strlen(dest);j++)
				if (dest[j]==32)
					dest[j]=0;
			break;
		case 2:
			strcpy(name,chaine);
			break;
		case 3:
			strcpy(dest,app[n].Titre);
			break;
		}

	if (app[n].numbox!=0)
		{
		_cbuf[10]=2;
		_cbuf[11]=1;

		_cbuf[12]=1;
		_cbuf[13]=0;
		_cbuf[14]=1;

		if (MWinTraite(app[n].box)!=-1)
			{
			PrintAt(0,0,"(10:%d)",_cbuf[10]);
			PrintAt(0,1,"(11:%d)",_cbuf[11]);
			PrintAt(0,2,"(%s)",_sbuf[10]);
			Wait(0,0);

			strcat(dest," ");
			strcat(dest,_sbuf[10]);
			MacFree(10);
			}
		else
			{
			res=0;		//--- Escape -----------------------------------
			}
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
|- Code de retour:													  -|
|- 0: perfect														  -|
|- 1: error 														  -|
|- 2: no player for this file										  -|
\*--------------------------------------------------------------------*/

int PlayerIdf(char *name,int numero)
{
char version;
char type;
short lenbox;
char lendir;

int nbrdir;
int nbrbox;

char fin=0;

int j,i,n;

fic=fopen(KKFics->FicIdfFile,"rb");

if (fic==NULL)
	{
	PUTSERR("IDFEXT.RB missing");
	return 1;
	}

fread(clef,1,8,fic);
if (memcmp(clef,"RedBLEXU",8))
	{
	PUTSERR("File IDFEXT.RB is bad");
	return 1;
	}

fread(&version,1,1,fic);

nbrappl=0;
nbrdir=0;
nbrbox=0;

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
				 (	(app[nbrappl].os==0) |
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
			fread(&lendir,1,1,fic);
			fread(chaine,lendir,1,fic);
			chaine[lendir]=0;
			for (i=0;i<nbrappl;i++)
				{
				if (nbrdir==app[i].NoDir-1)
					strcpy(app[i].dir,chaine);
				}
			nbrdir++;
			break;
		case 3:
			break;
		case 4:
			fread(&lenbox,2,1,fic);
			fread(rbbox,lenbox,1,fic);

			for (i=0;i<nbrappl;i++)
				{
				if (nbrbox==app[i].numbox-1)
					{
					app[i].box=(char*)GetMem(lenbox);
					memcpy(app[i].box,rbbox,lenbox);
					}
				}
			nbrbox++;
			break;

		default:
			PrintAt(0,0,"Error %d",type);
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

	if (app[i].numbox!=0)
		{
		PrintAt(0,0,"(%d)",app[i].numbox);
		WinError("toto2");
		}
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
char clef[8],version;
int i,j,n;
int nbrdir;
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

fread(clef,1,8,fic);
if (memcmp(clef,"RedBLEXU",8))
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
				bar[nbrappl].Help=501;

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
	if (PannelMenu(bar,nbrappl,&menu)!=2) fin=1;

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


/*--------------------------------------------------------------------*\
|- Configuration KKMAIN 											  -|
\*--------------------------------------------------------------------*/

static char buffer[256];

struct _keydef defkeydef[]=
	{ { 0x0001 , 28 },
	  { 0x0002 , 87 },
	  { 0x0004 , 29 },
	  { 0x0005 , 70 },
	  { 0x0006 , 32 },
	  { 0x0009 , 89 },
	  { 0x000A , 30 },
	  { 0x000C , 63 },
	  { 0x000D , 84 },
	  { 0x0012 , 27 },
	  { 0x0015 , 68 },
	  { 0x0019 , 38 },
	  { 'ý'    , 19 },
	  { '_'    , 65 },
	  { 0x8500 , 74 },
	  { 0x1C00 , 58 },
	  { KEY_UP , 37 },
	  { 0x5200 , 16 },
	  { KEY_DOWN , 36 }, // BAS
	  { KEY_LEFT , 56 }, // GAUCHE
	  { KEY_RIGHT , 57 }, // DROITE
	  { KEY_PPAGE , 52 },
	  { KEY_NPAGE , 53 },
	  { KEY_HOME , 54 }, // HOME
	  { KEY_END , 55 }, // END
	  { KEY_F(1),  1 }, // F1
	  { KEY_F(2), 79 }, // F2
	  { KEY_F(3) ,  7 }, // F3
	  { KEY_F(4) ,  9 },
	  { KEY_F(5) , 10 },
	  { KEY_F(6) , 11 },
	  { KEY_F(7) , 12 },
	  { KEY_F(8) , 13 },
	  { 0x5400 ,107 },
	  { 0x5500 , 39 },
	  { 0x5600 ,110 },
	  { 0x8D00 ,  8 },
	  { 0x5700 , 59 },
	  { 0x5800 ,108 },
	  { 0x5900 , 44 },
	  { 0x5C00 , 81 },
	  { 0x5E00 , 14 },
	  { 0x5F00 , 15 },
	  { 0x6000 , 22 },
	  { 0x6100 , 23 },
	  { 0x6200 , 24 },
	  { 0x6300 , 25 },
	  { 0x6400 , 26 },
	  { 0x6500 , 33 },
	  { 0x6800 , 49 },
	  { 0x6900 , 50 },
	  { 0x6A00 , 21 },
	  { 0x6B00 , 71 },
	  { 0x6C00 ,109 },
	  { 0x6D00 , 69 },
	  { 0x6E00 ,  5 },
	  { 0x6F00 , 48 },
	  { 0x7000 , 47 },
	  { 0x7100 , 60 },
	  { 0x7300 , 41 },
	  { 0x7400 , 46 },
	  { 0x7600 , 40 },
	  { 0x8400 , 41 },
	  { 0x8600 , 42 },
	  { 0x8800 ,  6 },
	  { 0x8A00 , 17 },
	  { 0x9100 , 64 },
	  { 0xA100 , 51 },
	  { 0x8B00 , 99 },
	  { 0x8C00 , 31 },
	  { KEY_F(9) , 87 }, // F9
	  { KEY_F(10) , 88 }, // F10 QUIT
	  { 0x5D00 , 97 },
	  { 0x000F ,105 },
	  { 0x001A , 67 },
	  { 0x0014 ,111 },

	  { 0x6600 ,112 },		// New Compo
	  { 0x6700 ,113 },		// Compo setup

	  { 0x0000 ,  0 } };


void DefaultKKCfg(void)
{
short Nm[]={72,68,42,40,92,63};

memcpy(KKCfg->keydef,defkeydef,sizeof(defkeydef));

#ifndef NOFONT
KKCfg->dispath=0;
#else
KKCfg->dispath=1;
#endif

KKCfg->fullnamesup=0;	//--- N'affiche pas les fullnames

KKCfg->ncmode=0;

KKCfg->displong=0;

KKCfg->cmdrun=0;
KKCfg->cmdshell=0;

KKCfg->internshell=0;

KKCfg->longname=1;
KKCfg->transtbl=0;	//0 -> 3
KKCfg->win95ln=1;
KKCfg->daccessln=0; //0 ou 1

KKCfg->userfont=0;

KKCfg->isbar=1;
KKCfg->isidf=1;

KKCfg->sizewin=0;

KKCfg->addselect=0;
KKCfg->lift=1;

KKCfg->KeyAfterShell=0;

KKCfg->savekey=0;			//--- Vaut 0 par defaut --------------------

memcpy(KKCfg->Qmenu,"ChgDrive"
					"  Swap  "
					"Go Trash"
					"QuickDir"
					" Select "
					"  Info  ",48);

memcpy(KKCfg->Nmenu,Nm,8*sizeof(short));

KKCfg->scrrest=1;

KKCfg->confexit=1;



KKCfg->pathdown=0;

KKCfg->V.Traduc[0]=10;
KKCfg->V.Traduc[1]=0;

strcpy(KKCfg->V.Mask[0]->title,"C Style");
strcpy(KKCfg->V.Mask[0]->chaine,
							 "asm break case cdecl char const continue "
						 "default do double else enum extern far float "
							 "for goto huge if int interrupt long near "
						   "pascal register short signed sizeof static "
							"struct switch typedef union unsigned void "
													"volatile while @");
KKCfg->V.Mask[0]->Ignore_Case=0;
KKCfg->V.Mask[0]->Other_Col=1;

strcpy(KKCfg->V.Mask[1]->title,"Pascal Style");
strcpy(KKCfg->V.Mask[1]->chaine,
						   "absolute and array begin case const div do "
							"downto else end external file for forward "
							"function goto if implementation in inline "
						  "interface interrupt label mod nil not of or "
							   "packed procedure program record repeat "
						   "set shl shr string then to type unit until "
										   "uses var while with xor @");
KKCfg->V.Mask[1]->Ignore_Case=1;
KKCfg->V.Mask[1]->Other_Col=1;

strcpy(KKCfg->V.Mask[2]->title,"Assembler Style");
strcpy(KKCfg->V.Mask[2]->chaine,
					   "aaa aad aam aas adc add and arpl bound bsf bsr "
	  "bswap bt btc btr bts call cbw cdq clc cld cli clts cmc cmp cmps "
	  "cmpxchg cwd cwde daa das dec div enter esc hlt idiv imul in inc "
	 "ins int into invd invlpg iret iretd jcxz jecxz jmp ja jae jb jbe "
	  "jc jcxz je jg jge jl jle jna jnae jnb jnbe jnc jne jng jnge jnl "
  "jnle jno jnp jns jnz jo jp jpe jpo js jz lahf lar lds lea leave les "
		"lfs lgdt lidt lgs lldt lmsw lock lods loop loope loopz loopnz "
  "loopne lsl lss ltr mov movs movsx movsz mul neg nop not or out outs "
  "pop popa popad push pusha pushad pushf pushfd rcl rcr rep repe repz "
   "repne repnz ret retf rol ror sahf sal shl sar sbb scas setae setnb "
	   "setb setnae setbe setna sete setz setne setnz setl setng setge "
	   "setnl setle setng setg setnle sets setns setc setnc seto setno "
	 "setp setpe setnp setpo sgdt sidt shl shr shld shrd sldt smsw stc "
	  "std sti stos str sub test verr verw wait fwait wbinvd xchg xlat "
							   "db dw dd endp ends assume xlatb xor @");

KKCfg->V.Mask[1]->Ignore_Case=1;
KKCfg->V.Mask[1]->Other_Col=1;

strcpy(KKCfg->V.Mask[15]->title,"Ketchup^Pulpe Style");
strcpy(KKCfg->V.Mask[15]->chaine,
				  " blackward ketchup killers redbug access darköangel "
					  "off topy kennet typeöone pulpe tyby djamm vatin "
				   "marjorie katana ecstasy cray magicöfred cobra z @");
KKCfg->V.Mask[15]->Ignore_Case=1;
KKCfg->V.Mask[15]->Other_Col=1;

strcpy(KKCfg->extens,"RAR ARJ ZIP LHA DIZ EXE COM BAT BTM KKP");

KKCfg->mtrash=100000;

KKCfg->currentdir=1;
KKCfg->alcddir=0;
KKCfg->overflow1=0;
KKCfg->overflow2=0;

KKCfg->autoreload=1;
KKCfg->verifhist=0;

KKCfg->noprompt=0;

KKCfg->crc=0x69;

KKCfg->key=0;

KKCfg->dispcolor=1;

KKCfg->insdown=1;
KKCfg->seldir=1;

KKCfg->hidfil=1;

KKCfg->enterkkd=0;

KKCfg->palafter=1;

KKCfg->cnvhist=1;
KKCfg->esttime=1;

KKCfg->editeur[0]=0;
KKCfg->vieweur[0]=0;
KKCfg->ssaver[0]=0;

KKCfg->Esc2Close=0;

strcpy(KKCfg->ExtTxt,
				 "ASM BAS C CPP DIZ DOC H HLP HTM INI LOG NFO PAS TXT CKK");
KKCfg->Enable_Txt=1;
strcpy(KKCfg->ExtBmp,
		 "FIF BMP GIF ICO JPG LBM PCX PIC PKM PNG RAW TGA TIF WMF WPG");
KKCfg->Enable_Bmp=1;
strcpy(KKCfg->ExtSnd,"IT IFF MID MOD MTM S3M VOC WAV XM RTM MXM");
KKCfg->Enable_Snd=1;
strcpy(KKCfg->ExtArc,"ARJ LHA RAR ZIP KKD DFP");
KKCfg->Enable_Arc=1;
strcpy(KKCfg->ExtExe,"BAT BTM COM EXE PRG KKP");
KKCfg->Enable_Exe=1;
strcpy(KKCfg->ExtUsr,"XYZ");
KKCfg->Enable_Usr=1;

strcpy(KKCfg->HistDir,"C:\\");

strcpy(KKCfg->HistCom,"!.!");

KKCfg->V.warp=1;
KKCfg->V.cnvtable=0;									// Table Ketchup
KKCfg->V.autotrad=0;
KKCfg->V.ajustview=1;
KKCfg->V.saveviewpos=1;
KKCfg->V.wmask=15;									// RedBug preference

KKCfg->V.lnfeed=4;												// CR/LF
KKCfg->V.maskaccel=0;
KKCfg->V.userfeed=0xE3;

KKCfg->V.AnsiSpeed=133;

NbrFunct=0;
}

void FileSetup(void)
{
int n;

MacAlloc(10,80);
MacAlloc(11,80);
MacAlloc(12,80);

if (!(Info->macro))
	{
	strcpy(_sbuf[10],KKCfg->vieweur);
	strcpy(_sbuf[11],KKCfg->editeur);
	strcpy(_sbuf[12],KKCfg->ssaver);
	}

do
{
n=MWinTraite(filesetu_kkt);

if (n==-1)
	{
	MacFree(10);
	MacFree(11);
	MacFree(12);
	return; 											// ESCape
	}

if (n==1)	//--- Editor -----------------------------------------------
	{
	static char dest[256];
	static int i;

	i=FicIdf(dest,buffer,91,2);

	switch(i)
		{
		case 0:
			strcpy(_sbuf[11],buffer);
			break;
		case 1:
			WinError("Run Main Setup before");
			break;	//--- error ----------------------------------------
		case 2:
			WinError("No editor found");
			break;	//--- no player ------------------------------------
		case 3:
			break; //--- Escape ----------------------------------------
		}
	}

if (n==2)	//--- Screen saver -----------------------------------------
	{
	static char dest[256];
	static int i;

	i=FicIdf(dest,buffer,143,2);

	switch(i)
		{
		case 0:
			strcpy(_sbuf[12],buffer);
			break;
		case 1:
			WinError("Run Main Setup before");
			break;	//--- error ----------------------------------------
		case 2:
			WinError("No screen saver found");
			break;	//--- no player ------------------------------------
		case 3:
			break; //--- Escape ----------------------------------------
		}
	}
}
while (n!=0);

strcpy(KKCfg->vieweur,_sbuf[10]);
strcpy(KKCfg->editeur,_sbuf[11]);
strcpy(KKCfg->ssaver,_sbuf[12]);

MacFree(10);
MacFree(11);
MacFree(12);
}

void SetDefaultKKPath(char *path)
{
char *Trash;

Trash=getenv("KKUSER");
if (Trash==NULL) Trash="trash";

KKFics->trash=(char*)GetMem(256);
strcpy(KKFics->trash,path);
Path2Abs(KKFics->trash,Trash);						 // repertoire trash

KKFics->FicIdfFile=(char*)GetMem(256);
strcpy(KKFics->FicIdfFile,KKFics->trash);
Path2Abs(KKFics->FicIdfFile,"idfext.rb");

KKFics->CfgFile=(char*)GetMem(256);
strcpy(KKFics->CfgFile,KKFics->trash);
Path2Abs(KKFics->CfgFile,"kkrb.cfg");

KKFics->temp=(char*)GetMem(256);
strcpy(KKFics->temp,KKFics->trash);
Path2Abs(KKFics->temp,"kktemp.tmp");

KKFics->log=(char*)GetMem(256);
strcpy(KKFics->log,KKFics->trash);
Path2Abs(KKFics->log,"logfile");                        // logfile trash

KKFics->menu=(char*)GetMem(256);
strcpy(KKFics->menu,KKFics->trash);
Path2Abs(KKFics->menu,"kkc.mnu");

KKFics->ficscreen=(char*)GetMem(256);
strcpy(KKFics->ficscreen,KKFics->trash);
Path2Abs(KKFics->ficscreen,"kkc.scr");

KKCfg->V.viewhist=(char*)GetMem(256);
strcpy(KKCfg->V.viewhist,KKFics->trash);
Path2Abs(KKCfg->V.viewhist,"kkview.rb");
}


/*--------------------------------------------------------------------*\
|- Remplis le vide si on est en mode 90 colonnes					  -|
\*--------------------------------------------------------------------*/
void RemplisVide(void)
{
if (Cfg->TailleX==80) return;

Cadre(80,0,Cfg->TailleX-1,2,3,Cfg->col[55],Cfg->col[56]);
PrintAt(81,1,"%*s",Cfg->TailleX-82,"KetchupK");

Cadre(80,3,Cfg->TailleX-1,Cfg->TailleY-2,2,Cfg->col[55],Cfg->col[56]);
Window(81,4,Cfg->TailleX-2,Cfg->TailleY-3,Cfg->col[16]);

AffChr(Cfg->TailleX-2,Cfg->TailleY-3,3);
}

void Console(void)
{
FILE *fic;
int n;

SaveScreen();

strcpy(buffer,KKFics->trash);
Path2Abs(buffer,"console.log");

fic=fopen(buffer,"wt");

for(n=1;n<14;n++)
	{
	if (n>1)
		Window(1,1,Cfg->TailleX-2,n-1,Cfg->col[10]);
	Cadre(0,0,Cfg->TailleX-1,n,3,Cfg->col[9],Cfg->col[41]);
	Pause(1);
	}

Wait(0,0);
LoadScreen();
}
