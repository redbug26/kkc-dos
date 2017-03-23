/*--------------------------------------------------------------------*\
|- Gestion des disquettes											  -|
|----------------------------------------------------------------------|
|- By RedBug/Ketchup^Pulpe											  -|
\*--------------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#ifndef LINUX
    #include <dos.h>
    #include <direct.h>
#else
    #include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#endif

#include "kk.h"

void ClearSpace(char *name);

void ReadWiredCfg(struct wiredcfg *wcfg);
void WriteWiredCfg(struct wiredcfg *wcfg);

int CompoCorrCfg(struct wiredcfg *wcfg,char *buffrom,char *bufto);
void GetCompoCfg(struct wiredcfg *wcfg,char *buffer);

// KKT macro
#include "grpinfo.h"
#include "partycfg.h"
#include "compocfg.h"

struct wiredcfg
	{
	char pcompo[256];		// les 'betes fichiers'
	char prelea[256];		// les zips
	char source[256];
	char name[2048];
	char dir[2048];

	char lhandle[33];
	char lgroupe[33];
	char lcompo[9];
	char lnumber[33];
	char ldescri[33];
	char ltitle[33];
	};

void center(char *buffer,int length)
{
int i,n,l;
char *buf;

buf=(char*)GetMem(length+1);

l=strlen(buffer);
n=(length-l)/2;
for(i=0;i<n;i++)
	buf[i]=32;
buf[n]=0;
strcat(buf,buffer);
n=strlen(buf);
while(n<length)
	{
	buf[n]=32;
	n++;
	}
buf[n]=0;
strcpy(buffer,buf);
}

void CompoCfg(struct wiredcfg *wcfg)
{
char buffer[256],buffer2[256];

struct barmenu *bar;
MENU menu;

int cur,retour,nbrbar;

int res,m,n;

bar=(struct barmenu*)GetMem(sizeof(struct barmenu)*50);

nbrbar=0;
m=0;
n=-1;
do
	{
	n++;
	if ((wcfg->name[n]==';') | (wcfg->name[n]==0))

	   {
	   memcpy(buffer,wcfg->name+m,n-m);
	   buffer[n-m]=0;
	   bar[nbrbar].Titre=(char*)GetMem(256);
	   strcpy(bar[nbrbar].Titre,buffer);
	   bar[nbrbar].fct=1;
	   nbrbar++;
	   m=n+1;
	   }
	}
while(wcfg->name[n]!=0);

nbrbar=0;
m=0;
n=-1;
do
	{
	n++;
	if ((wcfg->dir[n]==';') | (wcfg->dir[n]==0))
	   {
	   strcpy(buffer2,bar[nbrbar].Titre);
	   memcpy(buffer,wcfg->dir+m,n-m);
	   buffer[n-m]=0;
	   sprintf(bar[nbrbar].Titre,"%-32s %s",buffer2,buffer);
	   nbrbar++;
	   m=n+1;
	   }
	}
while(wcfg->dir[n]!=0);


cur=0;

do
	{
	menu.x=2;
	menu.y=2;

	menu.attr=0;

	menu.cur=cur;

	retour=PannelMenu(bar,nbrbar,&menu);

	cur=menu.cur;

	if (retour==1)
		{
		bar[nbrbar].fct=1;

		for(n=nbrbar;n>cur;n--)
			bar[n].Titre=bar[n-1].Titre;

		bar[cur].Titre=(char*)GetMem(256);
		sprintf(bar[cur].Titre,"%-32s %s","?","?");

		nbrbar++;
		}

	if ((retour==2) | (retour==1))
		{
		MacAlloc(20,80); // compo name
		MacAlloc(21,80); // directory

		memcpy(_sbuf[20],bar[cur].Titre,32);
		_sbuf[20][32]=0;
		ClearSpace(_sbuf[20]);

		strcpy(_sbuf[21],bar[cur].Titre+33);

		res=MWinTraite(compocfg_kkt);

		if (res!=-1)
			sprintf(bar[cur].Titre,"%-32s %s",_sbuf[20],_sbuf[21]);

		MacFree(20);
		MacFree(21);
		}

	if (retour==-1)
		{
		LibMem(bar[cur].Titre);

		for(n=cur;n<nbrbar-1;n++)
			bar[n].Titre=bar[n+1].Titre;

		nbrbar--;
		}
	}
while(retour!=0);

wcfg->name[0]=0;
wcfg->dir[0]=0;

for(n=0;n<nbrbar;n++)
	{
	memcpy(buffer,bar[n].Titre,32);
	buffer[32]=0;
	ClearSpace(buffer);

	strcpy(buffer2,bar[n].Titre+33);

	strcat(wcfg->name,buffer);
	if (n!=nbrbar-1)
		strcat(wcfg->name,";");

	strcat(wcfg->dir,buffer2);
	if (n!=nbrbar-1)
		strcat(wcfg->dir,";");

	LibMem(bar[n].Titre);
	}
}

int CompoCorrCfg(struct wiredcfg *wcfg,char *buffrom,char *bufto)
{
int trouv=-1;
char buf2[256];

int nbrbar;

int m,n;

nbrbar=0;
m=0;
n=-1;
do
	{
	n++;
	if ((wcfg->dir[n]==';') | (wcfg->dir[n]==0))

		{
		memcpy(buf2,wcfg->dir+m,n-m);
		buf2[n-m]=0;
		if (!strcmp(buf2,buffrom))
			{
			trouv=nbrbar;
			break;
			}
	   nbrbar++;
	   m=n+1;
	   }
	}
while(wcfg->dir[n]!=0);


if (trouv==-1)
	return 0;

nbrbar=0;
m=0;
n=-1;
do
	{
	n++;
	if ((wcfg->name[n]==';') | (wcfg->name[n]==0))
		{
		if (nbrbar==trouv)
			{
			memcpy(bufto,wcfg->name+m,n-m);
			bufto[n-m]=0;
			}
	   nbrbar++;
	   m=n+1;
	   }
	}
while(wcfg->name[n]!=0);

return 1;

}




void GetCompoCfg(struct wiredcfg *wcfg,char *buffer)
{
char buf2[256];

struct barmenu *bar;
MENU menu;

int retour,nbrbar;

int m,n;

bar=(struct barmenu*)GetMem(sizeof(struct barmenu)*50);

nbrbar=0;
m=0;
n=-1;
do
	{
	n++;
	if ((wcfg->name[n]==';') | (wcfg->name[n]==0))

	   {
	   memcpy(buf2,wcfg->name+m,n-m);
	   buf2[n-m]=0;
	   bar[nbrbar].Titre=(char*)GetMem(256);
	   strcpy(bar[nbrbar].Titre,buf2);
	   bar[nbrbar].fct=1;
	   nbrbar++;
	   m=n+1;
	   }
	}
while(wcfg->name[n]!=0);

menu.x=2;
menu.y=2;

menu.attr=8;

menu.cur=0;

retour=PannelMenu(bar,nbrbar,&menu);

if (retour==2)
	{
	nbrbar=0;
	m=0;
	n=-1;
	do
		{
		n++;
		if ((wcfg->dir[n]==';') | (wcfg->dir[n]==0))
			{
			if (nbrbar==menu.cur)
				{
				memcpy(buffer,wcfg->dir+m,n-m);
				buffer[n-m]=0;
				}
		   nbrbar++;
		   m=n+1;
		   }
		}
	while(wcfg->dir[n]!=0);
	}

for(n=0;n<nbrbar;n++)
	LibMem(bar[n].Titre);
}



void WiredCfg(void)
{
struct wiredcfg wcfg;
int res;

ReadWiredCfg(&wcfg);

MacAlloc(10,80); // release
MacAlloc(11,80); // compo
MacAlloc(12,80); // source

strcpy(_sbuf[10],wcfg.prelea);
strcpy(_sbuf[11],wcfg.pcompo);
strcpy(_sbuf[12],wcfg.source);

do
{
res=MWinTraite(partycfg_kkt);

if (res==1)
	{
	CompoCfg(&wcfg);
	}


}
while(res==1);

if (res!=-1)
	{
	strcpy(wcfg.prelea,_sbuf[10]);
	strcpy(wcfg.pcompo,_sbuf[11]);
	strcpy(wcfg.source,_sbuf[12]);
	}

WriteWiredCfg(&wcfg);

MacFree(10);
MacFree(11);
MacFree(12);
}

void ReadWiredCfg(struct wiredcfg *wcfg)
{
FILE *fic;
char filename[256];

strcpy(filename,KKFics->trash);
Path2Abs(filename,"party.cfg");

fic=fopen(filename,"rb");
if (fic!=NULL)
	{
	fread(wcfg,1,sizeof(struct wiredcfg),fic);
	fclose(fic);
	}
else
	{
	strcpy(wcfg->pcompo,"\\compo");
	strcpy(wcfg->prelea,"\\release");
	strcpy(wcfg->source,"a:\\");
	strcpy(wcfg->name,"demo;intro 4k;intro 64k;demo 3dfx;graphics raytrace");
	strcpy(wcfg->dir,"demo;in4k;in64;d3fx;grtc");
	strcpy(wcfg->lhandle,"");
	strcpy(wcfg->lgroupe,"");
	strcpy(wcfg->lcompo,"");
	strcpy(wcfg->lnumber,"");
	strcpy(wcfg->ldescri,"");
	strcpy(wcfg->ltitle,"");

	}
}

void WriteWiredCfg(struct wiredcfg *wcfg)
{
FILE *fic;
char filename[256];

strcpy(filename,KKFics->trash);
Path2Abs(filename,"party.cfg");

fic=fopen(filename,"wb");
if (fic!=NULL)
	{
	fwrite(wcfg,1,sizeof(struct wiredcfg),fic);
	fclose(fic);
	}
else
	{
	WinError("Couldn't save party configuration");
	}
}

void EMakeDir(char *path)
{
char nom[256];
int n;

n=-1;

do
{
n++;
if ((path[n]==0) | (path[n]=='\\'))
	{
	strcpy(nom,path);
	nom[n]=0;
	if (chdir(nom)!=0)
		mkdir(nom,0);
	}
}
while(path[n]!=0);



}

void Wired(void)
{
char componame[256];
FENETRE *CFen;
struct wiredcfg wcfg;
int i,res;
FILE *fic;

CFen=DFen;

ReadWiredCfg(&wcfg);

MacAlloc(10,80); // Handle
MacAlloc(11,80); // Groupe
MacAlloc(12,80); // Compo
MacAlloc(13,80); // Number
MacAlloc(14,80); // Description
MacAlloc(15,80); // Title

strcpy(_sbuf[10],wcfg.lhandle);
strcpy(_sbuf[11],wcfg.lgroupe);
strcpy(_sbuf[12],wcfg.lcompo);
strcpy(_sbuf[13],wcfg.lnumber);
strcpy(_sbuf[14],wcfg.ldescri);
strcpy(_sbuf[15],wcfg.ltitle);

do
	{
	res=MWinTraite(grpinfo_kkt);

	strcpy(wcfg.lhandle,_sbuf[10]);
	strcpy(wcfg.lgroupe,_sbuf[11]);
	strcpy(wcfg.lcompo,_sbuf[12]);
	strcpy(wcfg.lnumber,_sbuf[13]);
	strcpy(wcfg.ldescri,_sbuf[14]);
	strcpy(wcfg.ltitle,_sbuf[15]);

	if (res==1)
		{
		GetCompoCfg(&wcfg,_sbuf[12]);
		}

	if ((CompoCorrCfg(&wcfg,_sbuf[12],componame)==0) & (res==0))
		{
		WinError("This compo doesn't exist");
		CompoCfg(&wcfg);
		res=1;
		}
	}
while(res==1);

if (res==0)
	{
	char sourcep[256];
	char buffer2[256],buffer[256];
	int numero;
	char ok;

	strcpy(sourcep,CFen->path);
	Path2Abs(sourcep,wcfg.source);

	DFen=CFen;
	CommandLine("#cd %s",wcfg.source);
	if (!stricmp(DFen->path,sourcep))
		{

		(KKCfg->noprompt)=(char)((KKCfg->noprompt)|1);

		DFen=CFen->Fen2;

		strcpy(buffer,wcfg.pcompo);
		Path2Abs(buffer,wcfg.lcompo);

		if (!PathExist(buffer))
			EMakeDir(buffer);

		CommandLine("#cd %s",buffer);

		numero=0;

		do
			{
			numero++;
			ok=1;

			sprintf(buffer,"%d",numero);
			for(i=0;i<DFen->nbrfic;i++)
				if (!WildCmp(DFen->F[i]->name,buffer))
					ok=0;

			}
		while(ok==0);

		strcpy(buffer,DFen->path);
		sprintf(wcfg.lnumber,"%d",numero);
		Path2Abs(buffer,wcfg.lnumber);

		if (!PathExist(buffer))
			EMakeDir(buffer);

		CommandLine("#cd %s",buffer);

		do
			{
			DFen=CFen;
			CommandLine("#cd %s",wcfg.source);

			if (stricmp(DFen->path,sourcep)!=0)
				break;

			for(i=0;i<CFen->nbrfic;i++)
				{
				CFen->nopcur=i;
				Copie(CFen,CFen->Fen2->path);
				}
			}
		while(WinMesg("Continue ?","Do you have another disk ?",1)==0);

//-- .old
		strcpy(buffer,CFen->Fen2->path);
		Path2Abs(buffer,"file_id.diz");

		strcpy(buffer2,CFen->Fen2->path);
		Path2Abs(buffer2,"file_id.old");

		rename(buffer,buffer2);

//-- .diz
		strcpy(buffer,CFen->Fen2->path);
		Path2Abs(buffer,"file_id.diz");

		fic=fopen(buffer,"wt");
		fprintf(fic,"                                        ÛÜ\n");
		fprintf(fic,"Ü   Ü ÜÜÜÜÜÜÜÜÜÜÜ  ÜÛÛÜ ÜÜ    ÜÜÜ   ÜÜÛÛÛÛÛÜÜ\n");
		fprintf(fic," ÜÛÛÝ    Ü     Ü ÜßÛÛÛÛÝ  ÜÛÛÜ ß ÜÛÜ  ßßßß\n");
		fprintf(fic,"ßÛÛÛ  ÞÜ ÞÛÛÜ ßß Û ÜÛÛß  Û ßÛÛÝ ßßÛÛÛ  WiREDÞ\n");
		fprintf(fic,"Ý ÛÛÛÜÛÛÛÜÛÛß ÛÛ ÛßÛßÜÛÜ ÛÜßßÜ  Û  ÛÛÛ 1998 Û\n");
		fprintf(fic,"ÛÜ ßÛÛß ßÛß  ÜÛÛ ÛÝ ÜÛÛÛÝÛÜÜÛÛß ÛÜ ÜÛÝ   ÜÜÛÛ\n");
		fprintf(fic,"Ûßß  ß ß   ßßßß  ßß  ßÛß  ßßß    ßßß  ßßßßßßÛ\n");
//		  fprintf(fic,"Û                                           Û\n");

		sprintf(buffer,"² [%d]",numero);
		fprintf(fic,"%-7s %35s ²\n",buffer,componame);

		strcpy(buffer,wcfg.ltitle);
		center(buffer,41);
		fprintf(fic,"² %41s ²\n",buffer);
 // 	  fprintf(fic,"²                                           ²\n",buffer);

		strcpy(buffer,"( ");
		strcat(buffer,wcfg.lhandle);
		strcat(buffer,"/");
		strcat(buffer,wcfg.lgroupe);
		strcat(buffer," )");
		center(buffer,41);
		fprintf(fic,"± %-36s ±\n",buffer);
		fprintf(fic,"ßÛÛßÜß Ü   Ü      Ü   ß  ÜßÜßÜ  Ü   Ü   ßÛÛÛß");
		fclose(fic);


		strcpy(buffer,wcfg.prelea);
		Path2Abs(buffer,wcfg.lcompo);

		if (!PathExist(buffer))
			EMakeDir(buffer);

		sprintf(buffer2,"index.lst");
		Path2Abs(buffer,buffer2);

		fic=fopen(buffer,"rt");
		if (fic==NULL)
			{
			fic=fopen(buffer,"wt");
			fprintf(fic,"%s\n\n",componame);
			}

		fclose(fic);

		fic=fopen(buffer,"at");
		fprintf(fic,"Title: %s\n",wcfg.ltitle);
		fprintf(fic,"Handle: %s\n",wcfg.lhandle);
		fprintf(fic,"Group: %s\n",wcfg.lgroupe);
		fprintf(fic,"Filename: %d.zip\n",numero);
/*		if (wcfg.ldescri[0]!=0)
			fprintf(fic,"Description: %s\n",wcfg.ldescri);
*/
		fprintf(fic,"\n");
		fclose(fic);

		DFen=CFen;
		strcpy(buffer,wcfg.pcompo);
		Path2Abs(buffer,wcfg.lcompo);
		Path2Abs(buffer,wcfg.lnumber);
		CommandLine("#cd %s",buffer);

		strcpy(buffer,wcfg.prelea);
		Path2Abs(buffer,wcfg.lcompo);
	//	  Path2Abs(buffer,wcfg.lnumber);

		if (!PathExist(buffer))
			EMakeDir(buffer);

		Path2Abs(buffer,wcfg.lnumber);

	// Pkzip

			{
			char old;

			KKCfg->scrrest=0;

			old=KKCfg->internshell;
			KKCfg->internshell=1;

			CommandLine("#pkzip -r -p %s >nul",buffer);

			KKCfg->internshell=old;
			}

	// Fin
		DFen=CFen->Fen2;
		strcpy(buffer,wcfg.pcompo);
		Path2Abs(buffer,wcfg.lcompo);
		Path2Abs(buffer,wcfg.lnumber);
		CommandLine("#cd %s",buffer);

		DFen=CFen;
		strcpy(buffer,wcfg.prelea);
		Path2Abs(buffer,wcfg.lcompo);
		CommandLine("#cd %s",buffer);

		(KKCfg->noprompt)=(char)((KKCfg->noprompt)&126);
		}
	}

if (res==2)
	{
//	  char sourcep[256];
	char buffer2[256],buffer[256];
	int numero;
	char ok;

	DFen=CFen->Fen2;

	numero=0;

	strcpy(buffer,wcfg.prelea);
	Path2Abs(buffer,wcfg.lcompo);

	if (!PathExist(buffer))
		EMakeDir(buffer);

	CommandLine("#cd %s",buffer);

	do
		{
		numero++;
		ok=1;

		sprintf(buffer,"%d",numero);
		for(i=0;i<DFen->nbrfic;i++)
			if (!WildCmp(DFen->F[i]->name,buffer))
				ok=0;

		}
	while(ok==0);


	strcpy(buffer,wcfg.prelea);
	Path2Abs(buffer,wcfg.lcompo);

	if (!PathExist(buffer))
		EMakeDir(buffer);

	sprintf(buffer2,"index.lst");
	Path2Abs(buffer,buffer2);

	fic=fopen(buffer,"rt");
	if (fic==NULL)
		{
		fic=fopen(buffer,"wt");
		fprintf(fic,"%s\n\n",componame);
		}

	fclose(fic);

	fic=fopen(buffer,"at");
	fprintf(fic,"Title: %s\n",wcfg.ltitle);
	fprintf(fic,"Handle: %s\n",wcfg.lhandle);
	fprintf(fic,"Group: %s\n",wcfg.lgroupe);
	fprintf(fic,"Filename: none\n");
/*		if (wcfg.ldescri[0]!=0)
			fprintf(fic,"Description: %s\n",wcfg.ldescri);
*/
	fprintf(fic,"\n");
	fclose(fic);


// Fin
	DFen=CFen->Fen2;
	strcpy(buffer,wcfg.prelea);
	Path2Abs(buffer,wcfg.lcompo);
	CommandLine("#cd %s",buffer);

	DFen=CFen;
	CommandLine("#cd %s",buffer);
	}




WriteWiredCfg(&wcfg);
}
