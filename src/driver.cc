/*--------------------------------------------------------------------*\
|- gestion de lecture des fichiers dans les directories 			  -|
\*--------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef __WC32__
	#include <dos.h>
	#include <direct.h>
#endif

#include "kk.h"

#ifndef LCCWIN
	#include <bios.h>
#endif

#ifdef GCC
	#include <unistd.h>
#endif



/*--------------------------------------------------------------------*\
|-	Les fonctions '???litfic()'                                       -|
|-		renvoient 1 en cas d'erreur                                   -|
\*--------------------------------------------------------------------*/

void StartWinArc(void);
int InterWinArc(int debut,int nbr,int n);
void CloseWinArc(void);

static int xarc,debarc;
static FILE *fic;
static int LngCopy;

static clock_k Clock;
static char winarcaff;

void StartWinArc(void)
{
Clock=GetClock();
winarcaff=0;

debarc=0;

fic=fopen(DFen->VolName,"rb");

LngCopy=filelength(fileno(fic));

DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrfic=0;

DFen->taillesel=0;
DFen->nbrsel=0;

CreateFile(DFen,"..",0,0,0x10,0);

if (KKCfg->pntrep==1)
    CreateFile(DFen,".",0,0,0x10,0);
}




int InterWinArc(int debut,int nbr,int n)
{
int x,y;

if (n!=0) LngCopy=n;

if (((KKCfg->noprompt)&1)==0)
	{
	if (winarcaff==0)
		{
		if (GetClock()-Clock>=CLOCKK_PER_SEC)
			{
			winarcaff=1;

			SaveScreen();

			xarc=(Cfg->TailleX-30)/2;

			Window(xarc,6,xarc+31,11,Cfg->col[16]);
			Cadre(xarc-1,5,xarc+32,12,0,Cfg->col[55],Cfg->col[56]);

			Cadre(xarc+1,6,xarc+30,8,1,Cfg->col[55],Cfg->col[56]);

			PrintAt(xarc+8,5,"Reading Archive");

			x=xarc+9;
			y=10;

			PrintAt(x,y,"    STOP    ");
			Puce(x,y,13,0);
			}
		}
	else
		{
		PrintAt(xarc+4,9,"%3d files found  %4d %%",nbr,(debut*100)/LngCopy);

		debarc=LongGradue(xarc+2,7,28,debarc,debut,LngCopy);

		if (KbHit())
			if (Wait(0,0)==13) return 1;
		}
	}

return 0;
}


void CloseWinArc(void)
{
fclose(fic);

if ( (((KKCfg->noprompt)&1)==0) & (winarcaff) )
	{
	LoadScreen();
	}
}





/*--------------------------------------------------------------------*\
|-							  FICHIER .ARJ							  -|
\*--------------------------------------------------------------------*/

struct ARJTime
	{
	unsigned t_tsec : 5;
	unsigned t_min : 6;
	unsigned t_heure : 5;
	unsigned t_day : 5;
	unsigned t_mois : 4;
	unsigned t_year : 7;
	};

struct ARJHeader
	{
	WORD HeadID;
	WORD HdrSize;
	BYTE HeadSize;
	BYTE VerNum;
	BYTE MinVerNum;
	BYTE HostOS;
	BYTE ArjFlag;
	BYTE Method;
	BYTE FType;
	BYTE Reserved;
    struct ARJTime d;
	ULONG PackSize;
	ULONG UnpSize;
	ULONG FileCRC;
	WORD FilePosF;
	WORD FileAcc;
	WORD HostData;
	};

int ARJlitfic(void)
{
struct ARJHeader Header;

char nom[256];

char Nomarch[256];
char Dest[256];
ULONG pos;
char fin;


int n,m;

if (strnicmp(DFen->VolName,DFen->path,strlen(DFen->VolName))!=0)
	{
	DFen->system=0;
	return 1;
	}

StartWinArc();

/*--------------------------------------------------------------------*\
|- --- Okay -----------------------------------------------------------|
\*--------------------------------------------------------------------*/

pos=0;
fin=0;

while(!fin)
{
char car;
short lng;

fseek(fic,pos,SEEK_SET);

if ( (InterWinArc(pos,DFen->nbrfic,0)==1) |
	 (fread(&Header,sizeof(struct ARJHeader),1,fic)!=1) )
	{
	fin=1;
	break;
	}

lng=0;
do
	{
	fread(&car,1,1,fic);
	nom[lng]=car;
	lng++;
	}
while(car!=0);

do
	fread(&car,1,1,fic);
while(car!=0);

strcpy(Nomarch,nom);


if (strlen(DFen->path)==strlen(DFen->VolName))
	strcpy(nom,"");
	else
	{
	strcpy(nom,(DFen->path)+strlen(DFen->VolName)+1);
	strcat(nom,"/");
	}

n=find1st(Nomarch,Dest,nom);

/*--------------------------------------------------------------------*\
|-	---fichier normal---											  -|
\*--------------------------------------------------------------------*/
if ( (n==1) & (Header.FType!=2) )
	{
    CreateFile(DFen,
               Dest,
               (unsigned short)((Header.d.t_tsec)+(Header.d.t_min)*32+(Header.d.t_heure)*2048),
               (unsigned short)((Header.d.t_day)+(Header.d.t_mois)*32+(Header.d.t_year)*512),
               0,
               Header.UnpSize);
	}

/*--------------------------------------------------------------------*\
|-	---On fout un repertoire--- 									  -|
\*--------------------------------------------------------------------*/
if ( (!Maskcmp(Nomarch,nom)) & (Header.FType!=2) )
	{
	char cont;

	cont=0;

	strcpy(Dest,Nomarch+strlen(nom) );
	for (m=0;m<strlen(Dest);m++)
		{
		if ((Dest[m]=='/') | (Dest[m]=='\\'))
			{
			cont=1;
			Dest[m]=0;
			}
		}

	if (cont==1)
		{
		for (n=0;n<DFen->nbrfic;n++)
            if (!stricmp(Dest,GetFilename(DFen,n)))
				cont=0;

		if (cont==1)
            CreateFile(DFen,Dest,0,0,0x10,0);
		}
	}



if (Header.FType!=2)
	pos=ftell(fic)+Header.PackSize+4+2; 	// Passe le CRC et les Zeros
	else
	pos=ftell(fic)+4+2; 					// Passe le CRC et les Zeros
}

CloseWinArc();

DFen->IDFSpeed=0;

if ( ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) | ((KKCfg->pntrep==0)
												  & (DFen->nbrfic==1)) )
	return 1;

return 0;
}

/*--------------------------------------------------------------------*\
|-							   FICHIER .RAR 						  -|
\*--------------------------------------------------------------------*/

struct RARTime
	{
	unsigned t_tsec : 5;
	unsigned t_min : 6;
	unsigned t_heure : 5;
	unsigned t_day : 5;
	unsigned t_mois : 4;
	unsigned t_year : 7;
	};


struct RARHeader
	{
	WORD TeteCRC;
	BYTE TeteType;
	WORD Flags;
	WORD TeteSize;
	ULONG PackSize;
	ULONG UnpSize;
	BYTE HostOS;
	ULONG FileCRC;
    struct RARTime d;
	BYTE UnpVer;
	BYTE Method;
	WORD NomSize;
	ULONG FileAttr;
	};

int RARlitfic(void)
{
char nom[256];

char Nomarch[256];
char Dest[256];
struct RARHeader Lt;
ULONG pos;
char fin;

if (strnicmp(DFen->VolName,DFen->path,strlen(DFen->VolName))!=0)
	{
	DFen->system=0;
	return 1;
	}

StartWinArc();

/*--------------------------------------------------------------------*\
|-	Okay.															  -|
\*--------------------------------------------------------------------*/

pos=ftell(fic);
fin=0;

while(!fin)
{
fseek(fic,pos,SEEK_SET);


if ( (InterWinArc(pos,DFen->nbrfic,0)==1) |
	 (fread(&Lt,32,1,fic)!=1) |
	 (Lt.TeteType<0x72) |
	 (Lt.TeteType>0x75) |
	 (Lt.TeteSize<7) )
		{
		fin=1;
		break;
		}

if (Lt.TeteType==0x74)
	{
	int n;

	fseek(fic,pos+32,SEEK_SET);
	fread(Nomarch,Lt.NomSize,1,fic);

	Nomarch[Lt.NomSize]=0;		 // contient les fichiers dans l'archive

	if (strlen(DFen->path)==strlen(DFen->VolName))
		strcpy(nom,"");
		else
		{
		strcpy(nom,(DFen->path)+strlen(DFen->VolName)+1);
		strcat(nom,"\\");
		}

	n=find1st(Nomarch,Dest,nom);

	if (n==1)
		{
		char cont;

		cont=1;

		for (n=0;n<DFen->nbrfic;n++)
            if (!stricmp(Dest,GetFilename(DFen,n)))
				cont=0;

		if (cont==1)
            CreateFile(DFen,
                       Dest,
                       (unsigned short)((Lt.d.t_tsec)+(Lt.d.t_min)*32+(Lt.d.t_heure)*2048),
                       (unsigned short)((Lt.d.t_day)+(Lt.d.t_mois)*32+(Lt.d.t_year)*512),
                       (char)(Lt.FileAttr),
                       Lt.UnpSize);

		}
	else
		if (!Maskcmp(Nomarch,nom))
		{
		int m;
		char cont;

		cont=0;

		strcpy(Dest,Nomarch+strlen(nom) );
		for (m=0;m<strlen(Dest);m++)
			{
			if ((Dest[m]=='/') | (Dest[m]=='\\'))
				{
				cont=1;
				Dest[m]=0;
				}
			}
		if (cont==1)
			{
			for (n=0;n<DFen->nbrfic;n++)
                if (!stricmp(Dest,GetFilename(DFen,n)))
					cont=0;

			if (cont==1)
                CreateFile(DFen,Dest,0,0,0x10,0);
			}
		}

	}

pos=pos+Lt.TeteSize;
if (Lt.Flags & 0x8000) pos+=Lt.PackSize;		//--- LONG_BLOCK -------
}

CloseWinArc();

DFen->IDFSpeed=0;

if ( ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) | ((KKCfg->pntrep==0) &
													(DFen->nbrfic==1)) )
	return 1;

return 0;
}

/*--------------------------------------------------------------------*\
|-							   FICHIER .ZIP 						  -|
\*--------------------------------------------------------------------*/

struct ZIPTime
	{
	unsigned t_tsec : 5;
	unsigned t_min : 6;
	unsigned t_heure : 5;
	unsigned t_day : 5;
	unsigned t_mois : 4;
	unsigned t_year : 7;
	};

struct ZIPHeader
	{
	ULONG Signature;
	WORD Version;
	WORD GPBFlag;
	WORD Compress;
    struct ZIPTime d;
	ULONG CRC32;
	ULONG PackSize;
	ULONG UnpSize;
	WORD FNameLen;
	WORD ExtraField;
	};

int ZIPlitfic(void)
{
struct ZIPHeader Header;

char nom[256];

char Nomarch[256];
char Dest[256];
ULONG pos;
char fin;

int n,m;

if (strnicmp(DFen->VolName,DFen->path,strlen(DFen->VolName))!=0)
	{
	DFen->system=0;
	return 1;
	}

StartWinArc();

/*--------------------------------------------------------------------*\
|-	Okay.															  -|
\*--------------------------------------------------------------------*/

pos=0;
fin=0;

while(fin==0)
{
fseek(fic,pos,SEEK_SET);

if ( (InterWinArc(pos,DFen->nbrfic,0)==1) |
	 (fread(&Header,sizeof(struct ZIPHeader),1,fic)!=1) )
	{
	fin=1;
	break;
	}

if (Header.FNameLen>255)
	{
	fin=2;
	break;
	}

if (fread(nom,Header.FNameLen,1,fic)!=1)
	{
	fin=1;
	break;
	}

nom[Header.FNameLen]=0;

strcpy(Nomarch,nom);


if (strlen(DFen->path)==strlen(DFen->VolName))
	strcpy(nom,"");
	else
	{
	strcpy(nom,(DFen->path)+strlen(DFen->VolName)+1);
	strcat(nom,"/");
	}

n=find1st(Nomarch,Dest,nom);

/*--------------------------------------------------------------------*\
|-	---fichier normal---											  -|
\*--------------------------------------------------------------------*/
if ( (n==1) & (Header.Signature==0x04034B50) )
	{
    CreateFile(DFen,
               Dest,
               (unsigned short)((Header.d.t_tsec)+(Header.d.t_min)*32+(Header.d.t_heure)*2048),
               (unsigned short)((Header.d.t_day)+(Header.d.t_mois)*32+(Header.d.t_year)*512),
               0,
               Header.UnpSize);

    // Fic[DFen->nbrfic]->truesize=Header.PackSize;
    }

/*--------------------------------------------------------------------*\
|-	---On fout un repertoire--- 									  -|
\*--------------------------------------------------------------------*/
if ( (!Maskcmp(Nomarch,nom)) & (Header.Signature==0x04034B50) )
	{
	char cont;

	cont=0;

	strcpy(Dest,Nomarch+strlen(nom) );
	for (m=0;m<strlen(Dest);m++)
		{
		if ((Dest[m]=='/') | (Dest[m]=='\\'))
			{
			cont=1;
			Dest[m]=0;
			}
		}
	if (cont==1)
		{
		for (n=0;n<DFen->nbrfic;n++)
            if (!stricmp(Dest,GetFilename(DFen,n)))
				cont=0;

		if (cont==1)
            CreateFile(DFen,Dest,0,0,0x10,0);
		}
	}

pos=ftell(fic)+Header.PackSize+Header.ExtraField;
}

CloseWinArc();

DFen->IDFSpeed=0;

if ( (fin==2) | ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) |
							  ((KKCfg->pntrep==0) & (DFen->nbrfic==1)) )
	return 1;

return 0;
}

/*--------------------------------------------------------------------*\
|-							 FICHIER .LHA							  -|
\*--------------------------------------------------------------------*/

struct LHATime
	{
	unsigned t_tsec : 5;
	unsigned t_min : 6;
	unsigned t_heure : 5;
	unsigned t_day : 5;
	unsigned t_mois : 4;
	unsigned t_year : 7;
	};

struct LHAHeader
	{
	BYTE HSize;
	BYTE Fill1;
	char Method[5];
	ULONG PackSize;
	ULONG UnpSize;
    struct LHATime d;
	WORD Fill2;
	BYTE FNameLen;
	};

int LHAlitfic(void)
{
struct LHAHeader Header;

char nom[256];

char Nomarch[256];
char Dest[256];
ULONG pos;
char fin;

int n,m;

if (strnicmp(DFen->VolName,DFen->path,strlen(DFen->VolName))!=0)
	{
	DFen->system=0;
	return 1;
	}

StartWinArc();

/*--------------------------------------------------------------------*\
|-	Okay.															  -|
\*--------------------------------------------------------------------*/

pos=0;
fin=0;

while(!fin)
{
fseek(fic,pos,SEEK_SET);

if ( (InterWinArc(pos,DFen->nbrfic,0)==1) |
	 (fread(&Header,sizeof(struct LHAHeader),1,fic)!=1) )
	{
	fin=1;
	break;
	}

fread(Nomarch,Header.FNameLen,1,fic);
Nomarch[Header.FNameLen]=0;

fseek(fic,pos,SEEK_SET);

fread(nom,128,1,fic);


if (strlen(DFen->path)==strlen(DFen->VolName))
	strcpy(nom,"");
	else
	{
	strcpy(nom,(DFen->path)+strlen(DFen->VolName)+1);
	strcat(nom,"/");
	}

n=find1st(Nomarch,Dest,nom);

if (n==1)
    CreateFile(DFen,
               Dest,
               (unsigned short)((Header.d.t_tsec)+(Header.d.t_min)*32+(Header.d.t_heure)*2048),
               (unsigned short)((Header.d.t_day)+(Header.d.t_mois)*32+(Header.d.t_year)*512),
               0,
               Header.UnpSize);

if (!Maskcmp(Nomarch,nom))
	{
	char cont;

	cont=0;

	strcpy(Dest,Nomarch+strlen(nom) );
	for (m=0;m<strlen(Dest);m++)
		{
		if ( (Dest[m]=='/') | (Dest[m]=='\\') )
			{
			cont=1;
			Dest[m]=0;
			}
		}
	if (cont==1)
		{
		for (n=0;n<DFen->nbrfic;n++)
            if (!stricmp(Dest,GetFilename(DFen,n)))
				cont=0;

		if (cont==1)
            CreateFile(DFen,Dest,0,0,0x10,0);
		}
	}

pos+=Header.HSize+Header.PackSize+2;	 //--- CRC ? -------------------
}

CloseWinArc();

DFen->IDFSpeed=0;

if ( ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) | ((KKCfg->pntrep==0) &
													(DFen->nbrfic==1)) )
	return 1;

return 0;
}


/*--------------------------------------------------------------------*\
|-							 FICHIER .KKD							  -|
\*--------------------------------------------------------------------*/

struct kkdesc
	{
	long desc;
	long next;
	long size;
	short time;
	short date;
	char attrib;
	};


int KKDlitfic(void)
{
struct kkdesc KKD_desc;

char nom[256];
static char name[256];

char Nomarch[256];
char fin;

int n,i,j;

unsigned char tai;									  // taille des noms

if (strnicmp(DFen->VolName,DFen->path,strlen(DFen->VolName))!=0)
	{
	DFen->system=0;
	return 1;
	}

StartWinArc();

if (strlen(DFen->path)==strlen(DFen->VolName))
	{
	strcpy(nom,"");
	}
	else
	{
	strcpy(nom,(DFen->path)+strlen(DFen->VolName)+1);
	strcat(nom,"\\");
	}



fseek(fic,3+1+1+255+4+4,SEEK_SET);						 // Passe la cle

//fread(&tai,1,1,fic);								 // Passe la version
//fread(&tai,1,1,fic);						   // Passe le nom du volume
//fread(Nomarch,tai,1,fic);

//fread(&n,4,1,fic);			 // Passe le nombre d'octets non utilise
//fread(&n,4,1,fic);// Passe le nbr de bytes non utilise avant reconstr.


i=0;	//--- DEBUT ----------------------------------------------------
j=0;	//--- FIN ------------------------------------------------------

i=j;
strcpy(name,nom+j);
for(n=j;n<strlen(nom);n++)
	{
	if (nom[n]=='\\')
		{
		name[n-j]=0;
		j=n+1;
		break;
		}
	}

fin=0;

while(i!=j)
{
fread(&tai,1,1,fic);
fread(Nomarch,tai,1,fic);
Nomarch[tai]=0;

fread(&KKD_desc,sizeof(struct kkdesc),1,fic);

if ( (!stricmp(Nomarch,name)) & ((KKD_desc.attrib&0x10)==0x10) )
	{
	if (KKD_desc.desc==0)
		{
		fin=1;		// Un gros probleme: un directory qui ne mene a rien
		break;
		}
	fseek(fic,KKD_desc.desc,SEEK_SET);

	i=j;
	strcpy(name,nom+j);
	for(n=j;n<strlen(nom);n++)
		{
		if (nom[n]=='\\')
			{
			name[n-j]=0;
			j=n+1;
			break;
			}
		}
	}
	else
	{
	if (KKD_desc.next==0)
		{
		fin=1;									 // Directory pas trouv‚
		break;
		}
	fseek(fic,KKD_desc.next,SEEK_SET);
	}
}


if (fin==0)
	{
	do
		{
		fread(&tai,1,1,fic);
		fread(Nomarch,tai,1,fic);
		fread(&KKD_desc,sizeof(struct kkdesc),1,fic);

		if (Nomarch[0]!='.')
			{
			Nomarch[tai]=0;

            CreateFile(DFen,
                       Nomarch,
                       KKD_desc.time,
                       KKD_desc.date,
                       KKD_desc.attrib,
                       KKD_desc.size);
/*
            if (KKD_desc.desc==0)
				Fic[DFen->nbrfic]->desc=0;
				else
				Fic[DFen->nbrfic]->desc=1;
*/
			}

		fseek(fic,KKD_desc.next,SEEK_SET);
		}
	while(KKD_desc.next!=0);
	}

CloseWinArc();

DFen->IDFSpeed=0;

return fin;
}

/*--------------------------------------------------------------------*\
|-							 FICHIER .DFP							  -|
\*--------------------------------------------------------------------*/

int DFPlitfic(void)
{
char oldnoprompt;
char key[13];

//ULONG pos;
//char fin;

int m,n,nbr,lng,deb;

if (strnicmp(DFen->VolName,DFen->path,strlen(DFen->VolName))!=0)
	{
	DFen->system=0;
	return 1;
	}

oldnoprompt=KKCfg->noprompt;
KKCfg->noprompt=1;

StartWinArc();

/*--------------------------------------------------------------------*\
|-	Okay.															  -|
\*--------------------------------------------------------------------*/

//pos=0;
//fin=0;

fseek(fic,5,SEEK_SET);

fread(&nbr,1,4,fic);

for(n=0;n<nbr;n++)
	{
	if (InterWinArc(n,DFen->nbrfic,nbr)==1) break;

	fread(key,1,12,fic);
	fread(&deb,1,4,fic);
	fread(&lng,1,4,fic);

	for(m=0;m<12;m++)
		if (key[m]==32) key[m]=0;

	key[12]=0;

    CreateFile(DFen, key, 0, 0, 0, lng);
    }

CloseWinArc();

DFen->IDFSpeed=0;

KKCfg->noprompt=oldnoprompt;

if ( ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) | ((KKCfg->pntrep==0) &
													(DFen->nbrfic==1)) )
	return 1;

return 0;
}

/*--------------------------------------------------------------------*\
|-							 FICHIER .DKF							  -|
\*--------------------------------------------------------------------*/

int DKFlitfic(void)
{
char oldnoprompt;
char name[17];

if (strnicmp(DFen->VolName,DFen->path,strlen(DFen->VolName))!=0)
	{
	DFen->system=0;
	return 1;
	}

oldnoprompt=KKCfg->noprompt;
KKCfg->noprompt=1;

StartWinArc();

/*--------------------------------------------------------------------*\
|-	Okay.															  -|
\*--------------------------------------------------------------------*/

fseek(fic,0x80,SEEK_SET);

while(1)
	{
	ushort SizeH,SizeL,PosH,PosL,LongName;
	int size,pos;


	fread(name,1,12,fic);
	if (name[0]==0)
		break;
	name[12]=0;

	fread(&LongName,1,2,fic);
	if (LongName!=0) name[0]='!';

	fread(&SizeL,1,2,fic);
	fread(&SizeH,1,2,fic);
	size=SizeH+SizeL*65536;

	fseek(fic,1,SEEK_CUR);

	fread(&PosH,1,2,fic);
	fread(&PosL,1,2,fic);
	pos=PosH+PosL*65536;

    CreateFile(DFen, name, 0, 0, 0, size);

	fseek(fic,16,SEEK_CUR);
	}

CloseWinArc();

DFen->IDFSpeed=0;

KKCfg->noprompt=oldnoprompt;

if ( ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) | ((KKCfg->pntrep==0) &
													(DFen->nbrfic==1)) )
	return 1;

return 0;
}




/*--------------------------------------------------------------------*\
|-							 FICHIER .RAW							  -|
\*--------------------------------------------------------------------*/

int RAWlitfic(void)
{
short sizesec;

char oldnoprompt;

char nom[256],*buf;

char path[256],*p;

short sf;
char key[32];
long sec;

//ULONG pos;
//char fin;

int n;

if (strnicmp(DFen->VolName,DFen->path,strlen(DFen->VolName))!=0)
	{
	DFen->system=0;
	return 1;
	}

strcpy(path,DFen->path+strlen(DFen->VolName));

oldnoprompt=KKCfg->noprompt;
KKCfg->noprompt=1;

StartWinArc();

fseek(fic,0x0B,SEEK_SET);
fread(&sizesec,1,2,fic);

/*--------------------------------------------------------------------*\
|-	Okay.															  -|
\*--------------------------------------------------------------------*/
//pos=0;
//fin=0;

fseek(fic,0x16,SEEK_SET);
fread(&sf,1,2,fic);

sec=((sf*2)+1)*sizesec;

if (path[0]!=0)
	strcpy(path,path+1);

while(path[0]!=0)
	{
	n=0;
	while ((path[n]!=0) & (path[n]!='\\') & (path[n]!='/') )
		n++;

	if (path[n]==0)
		p=path+n;
		else
		{
		path[n]=0;
		p=path+n+1;
		}

	//--- On cherche repertoire par repertoire -------------------------

	do
		{
		fseek(fic,sec,SEEK_SET);
		fread(key,32,1,fic);

		if (key[0]==0)
			{
			CloseWinArc();
			DFen->IDFSpeed=0;
			KKCfg->noprompt=oldnoprompt;
			return 1;
			}

		memcpy(nom,key,8);
		nom[8]=32;
		buf=strchr(nom,' ');
		buf[0]='.';
		memcpy(buf+1,key+8,3);
		buf[4]=32;
		buf=strchr(nom,' ');
		buf[0]=0;

		if (nom[strlen(nom)-1]=='.')
			nom[strlen(nom)-1]=0;

		sec+=32;
		}
	while(stricmp(nom,path)!=0);

	memcpy((short*)(&sf),key+0x1A,2);
	sec=sf-2;

	fseek(fic,0x0D,SEEK_SET);
	sf=(short)fgetc(fic);
	sec=sec*sf;

	fseek(fic,0x16,SEEK_SET);
	fread(&sf,2,1,fic);
	sec=sec+sf*2;

	fseek(fic,0x11,SEEK_SET);
	fread(&sf,2,1,fic);
	sec=sec+sf/16;

	sec=(sec+1)*sizesec;

	strcpy(path,p);
	}

while(1)
	{
//	  if (InterWinArc(DFen->nbrfic,DFen->nbrfic,nbr)==1) break;

	fseek(fic,sec,SEEK_SET);
	fread(key,32,1,fic);

	if (key[0]==0)
		break;

	if ((key[0]!=0xE5) & (key[11]==0x0F))
		{
		int nbr;

		nbr=key[0]&0x3F;

		sec+=32;

		nom[nbr*13]=0;

		for(n=nbr-1;n>=0;n--)
			{
			nom[n*13+ 0]=key[0x01];
			nom[n*13+ 1]=key[0x03];
			nom[n*13+ 2]=key[0x05];
			nom[n*13+ 3]=key[0x07];
			nom[n*13+ 4]=key[0x09];
			nom[n*13+ 5]=key[0x0E];
			nom[n*13+ 6]=key[0x10];
			nom[n*13+ 7]=key[0x12];
			nom[n*13+ 8]=key[0x14];
			nom[n*13+ 9]=key[0x16];
			nom[n*13+10]=key[0x18];
			nom[n*13+11]=key[0x1C];
			nom[n*13+12]=key[0x1E];

			fseek(fic,sec,SEEK_SET);
			fread(key,32,1,fic);

			sec+=32;
			}
		}
		else
		{
		memcpy(nom,key,8);
		nom[8]=32;
		buf=strchr(nom,' ');
		buf[0]='.';
		memcpy(buf+1,key+8,3);
		buf[4]=32;
		buf=strchr(nom,' ');
		buf[0]=0;

		sec+=32;
		}

	if (nom[strlen(nom)-1]=='.')
		nom[strlen(nom)-1]=0;

	if ((nom[0]!='.') & (nom[0]!=0xE5) & ((key[11]&0x8)!=0x8) )
		{
        short time,date;
        int size;

        memcpy(&time,key+22,2);
        memcpy(&date,key+24,2);
        memcpy(&size,key+28,4);

        CreateFile(DFen, nom, time, date, key[11], size);
		}
	}

CloseWinArc();

DFen->IDFSpeed=0;

KKCfg->noprompt=oldnoprompt;

if ( ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) | ((KKCfg->pntrep==0) &
													(DFen->nbrfic==1)) )
	return 1;

return 0;
}


/*--------------------------------------------------------------------*\
|-	Dos-function													  -|
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|-	Retourne 1 si erreur											  -|
\*--------------------------------------------------------------------*/

int DOSlitfic(void)
{
int n;

char rech[256];

static char tnom[256],nom2[256];
static char buf[256];
int i;

DIR *dirp;

#ifndef __WC32__

struct stat s;
int attrib;
struct dirent *ff;

#else

struct dirent *ff;

#endif

if (chdir(DFen->path)!=0)	//--- Invalid drive ------------------------
	{
	if (DFen->path[0]=='*')
		{
		DFen->system=7;
		return 1;
		}

//--
	strcpy(nom2,DFen->path);
	strcpy(tnom,"");
	do
		{
//			strcpy(nom2,DFen->path);  // Marjorie (pq j'avais mis ca ?)
		n=0;
		for (i=0;i<strlen(DFen->path)-1;i++)
			if (DFen->path[i]==DEFSLASH) n=i;

		if (n==0) break;

		strcpy(tnom,DFen->path+n+1);
		DFen->path[n]=0;

		strcpy(buf,DFen->path);
		if (buf[strlen(buf)-1]==':')
			strcat(buf,"\\");

		if (chdir(buf)==0) break;
		}
	while(1);

	if (n==0)
		{
		if (IOver==0)
			WinError("Drive Not Ready");
		memcpy(DFen->path,"C:\\",4);
		}
		else
		{
		if (EnterArcFile(DFen,tnom)==-1)
			{
			if (strlen(nom2)!=strlen(DFen->VolName))
				Path2Abs(DFen->path,nom2+strlen(DFen->VolName)+1);
			}
			else
			{
			if (IOver==0)
				WinMesg("Invalid Path",nom2,0);
			strcpy(DFen->path,nom2);

//				  strcpy(DFen->path,GetLastHistDir());
			}
		}
//-

	return 1;
	}

DFen->nbrfic=0;
DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrsel=0;

DFen->taillesel=0;

#ifdef __WC32__
strcpy(rech,DFen->path);
Path2Abs(rech,"*.*");

dirp=opendir(rech);

if (dirp!=NULL)
	while(1)
	{
	ff=readdir(dirp);
	if (ff==NULL) break;

	if ( ((KKCfg->pntrep==1) |	(strcmp(ff->d_name,".")!=0)) &
		 ((KKCfg->hidfil==1) | (((ff->d_attr)&_A_HIDDEN)!=_A_HIDDEN)) &
		 (((ff->d_attr)&_A_VOLID)!=_A_VOLID)
		 )
        CreateFile(DFen, ff->d_name, ff->d_time, ff->d_date, ff->d_attr, ff->d_size);
	}
closedir(dirp);
#else
strcpy(rech,DFen->path);

dirp=opendir(rech);

if (dirp!=NULL)
	while(1)
	{
	ff=readdir(dirp);
	if (ff==NULL) break;

	stat(ff->d_name,&s);

	attrib=S_ISDIR(s.st_mode)*_A_SUBDIR;

	if ( (KKCfg->pntrep==1) | (strcmp(ff->d_name,".")!=0) )
        CreateFile(DFen, ff->d_name, 0, 0, attrib, s.st_size);
	}
closedir(dirp);
#endif



rech[3]=0;

if ( ( (!stricmp(rech,"A:\\")) | (!stricmp(rech,"B:\\")) )
	 | (DFen->nbrfic==0) )
    CreateFile(DFen, "*27", 0, 0, _A_VOLID, 0);           // Pour Reload

/*--------------------------------------------------------------------*\
|- Test si c'est un CD-ROM                                            -|
\*--------------------------------------------------------------------*/

#ifdef __WC32__
	{
	union REGS R;

	R.w.ax=0x150B;
	R.w.cx=(short)(toupper(DFen->path[0])-'A');
	int386(0x2F,&R,&R);

	if ( (R.w.bx==0xADAD) & (R.w.ax!=0) )
        CreateFile(DFen, "*86", 0, 0, _A_VOLID, 0);        // Pour Open

	}
#endif

DFen->ChangeLine=1;

/*--------------------------------------------------------------------*\
|-	Temporise le temps … attendre avant le lecture du header OK ;-}   -|
\*--------------------------------------------------------------------*/


switch(TypeDisk( (toupper(DFen->path[0])-'A'),buf))
	{
	case 0: 	//--- C: -----------------------------------------------
		DFen->IDFSpeed=(long)(CLOCKK_PER_SEC/4);
		break;
	case 1: 	//--- Cdrom --------------------------------------------
		DFen->IDFSpeed=(long)(CLOCKK_PER_SEC);
		break;
	case 2: 	//--- Slow support -------------------------------------
		DFen->IDFSpeed=(long)(CLOCKK_PER_SEC*30);
		break;
	default:	//--- Never arrive there -------------------------------
		break;
   }

return 0;
}







int Hostlitfic(void)
{
FILE *infic;

char attr,lng;
int Time,Date,Size;

char nom[256];

KKCfg->scrrest=0;

if (DFen->path[0]!='*')
	{
	DFen->system=0;
	return 1;
	}

if (DFen->path[2]!='+')
	{
	DFen->path[2]='+';
	CommandLine("#%s %s %s","remdir",DFen->path+3,KKFics->temp);
	}
DFen->path[2]='-';

DFen->nbrfic=0;
DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrsel=0;

DFen->taillesel=0;

infic=fopen(KKFics->temp,"rb");
if (infic==NULL)
	{
	DFen->system=0;
	return 1;
	}

while(fread(&lng,1,1,infic)==1)
	{
	if (lng==0) break;

	fread(nom,lng,1,infic);
	fread(&attr,1,1,infic);
	fread(&Time,4,1,infic);
	fread(&Date,4,1,infic);
	fread(&Size,4,1,infic);

	if ( ((KKCfg->pntrep==1) |	(strcmp(nom,".")!=0)) &
		 ((KKCfg->hidfil==1) | ((attr&_A_HIDDEN)!=_A_HIDDEN)) &
		 ((attr&_A_VOLID)!=_A_VOLID)
		 )
		{
        nom[lng]=0;

        CreateFile(DFen,
                   nom,
                   (unsigned short)Time,
                   (unsigned short)Date,
                   attr,
                   Size);
		}
	}
fclose(infic);



if (DFen->nbrfic==0)
    CreateFile(DFen, "*27", 0, 0, _A_VOLID, 0);        // Pour Reload --

DFen->ChangeLine=1;
return 0;
}



/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

struct HeaderKKD
	{
	char *nom;
	long next;
	long desc;
	long size;
	short time;
	short date;
	char attrib;
	};

void MakeKKD(FENETRE *Fen,char *ficname)
{
#ifdef __WC32__

DIR *dirp;
struct dirent *ff;

char error;
char **TabRec;
long NbrRec,CurRec;

unsigned char tai;

long n;
long Nbfic;
long Nbdir;
FILE *fic;

struct HeaderKKD **Fichier;
int *Desc;

char **Repert;

char moi[256],nom[256],nomtemp[256];

char nom2[256];

int oldpos;


fic=fopen(ficname,"wb");
if (fic==NULL)
	{
	WinError("Protection Error");
	return;
	}

strcpy(nom2,Fen->path);


Fichier=(struct HeaderKKD**)GetMem(50000*sizeof(void*));
Desc=(int*)GetMem(50000*sizeof(long));

Repert=(char**)GetMem(5000*sizeof(void*));


TabRec=(char**)GetMem(500*sizeof(char*));
TabRec[0]=(char*)GetMem(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

Nbdir=0;
Nbfic=0;


SaveScreen();
ColLin(0,0,80,Cfg->col[6]);

oldpos=3+1+256+4+4;

Desc[NbrRec-1]=0; //--- pour la boucle, sera ecraser plus tard ---------

do
{
CurRec=NbrRec-1;

if (Fichier[Desc[CurRec]]!=NULL)
	Fichier[Desc[CurRec]]->desc=oldpos;

Repert[Nbdir]=(char*)GetMem(strlen(TabRec[CurRec])-1);
memcpy(Repert[Nbdir],(TabRec[CurRec])+2,strlen(TabRec[CurRec])-1);

Nbdir++;

PrintAt(0,0,"Go in  %-73s",TabRec[CurRec]);

strcpy(nomtemp,TabRec[CurRec]);
Path2Abs(nomtemp,"*.*");

strcpy(nom,TabRec[CurRec]);

/*--------------------------------------------------------------------*\
|-	The files														  -|
\*--------------------------------------------------------------------*/

dirp=opendir(nomtemp);

if (dirp!=NULL)
	while(1)
	{
	ff=readdir(dirp);
	if (ff==NULL) break;

	if	(strcmp(ff->d_name,".")!=0)
		{
		error=ff->d_attr;

		if ((error&0x10)!=0x10)    //--- Not a Subdir ------------------
			{
            Fichier[Nbfic]->desc=0;

/*            CreateFic(DFen,
                      ff->d_name,
                      ff->d_time,
                      ff->d_date,
                      ff->d_attr,
                      ff->d_size);*/

			Fichier[Nbfic]=(struct HeaderKKD*)
									   GetMem(sizeof(struct HeaderKKD));
			Fichier[Nbfic]->nom=(char*)GetMem(strlen(ff->d_name)+1);
			memcpy(Fichier[Nbfic]->nom,ff->d_name,strlen(ff->d_name)+1);
			Fichier[Nbfic]->size=ff->d_size;
			Fichier[Nbfic]->time=ff->d_time;
			Fichier[Nbfic]->date=ff->d_date;
			Fichier[Nbfic]->attrib=ff->d_attr;

			oldpos+=18+strlen(ff->d_name);

			Fichier[Nbfic]->next=oldpos;

			Nbfic++;
			}
		}
	}

closedir(dirp);

free(TabRec[CurRec]);
NbrRec--;

/*--------------------------------------------------------------------*\
|-	The directories 												  -|
\*--------------------------------------------------------------------*/

dirp=opendir(nomtemp);

if (dirp!=NULL)
	while(1)
	{
	ff=readdir(dirp);
	if (ff==NULL) break;

	if	(strcmp(ff->d_name,".")!=0)
		{
		error=ff->d_attr;

		if ((error&0x10)==0x10)    //--- Not a Subdir ------------------
			{
			Fichier[Nbfic]->desc=0; //--- On le fera plus tard; --------

			Fichier[Nbfic]=(struct HeaderKKD*)
									   GetMem(sizeof(struct HeaderKKD));

			Fichier[Nbfic]->nom=(char*)GetMem(strlen(ff->d_name)+1);
			memcpy(Fichier[Nbfic]->nom,ff->d_name,strlen(ff->d_name)+1);
			Fichier[Nbfic]->size=ff->d_size;
			Fichier[Nbfic]->time=ff->d_time;
			Fichier[Nbfic]->date=ff->d_date;

            Fichier[Nbfic]->attrib=ff->d_attr;

			oldpos+=18+strlen(ff->d_name);

			Fichier[Nbfic]->next=oldpos;

			if (ff->d_name[0]!='.')
				{
				strcpy(moi,nom);
				Path2Abs(moi,ff->d_name);

				Desc[NbrRec]=Nbfic;

				TabRec[NbrRec]=(char*)GetMem(strlen(moi)+1);
				memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
				NbrRec++;
				}

			Nbfic++;
			}
		}
	}

closedir(dirp);

Fichier[Nbfic-1]->next=0;
}
while (NbrRec>0);

LoadScreen();


fprintf(fic,"KKD");
tai=1;
fwrite(&tai,1,1,fic);		//--- VERSION ------------------------------
tai=255;
fwrite(&tai,1,1,fic);
fwrite(Fen->path,1,255,fic);

n=0;
fwrite(&n,4,1,fic);
fwrite(&n,4,1,fic);

for(n=0;n<Nbdir;n++)
	free(Repert[n]);

for(n=0;n<Nbfic;n++)
	{
	uchar tai;

	tai=(uchar)strlen(Fichier[n]->nom);
	fwrite(&tai,1,1,fic);					//--- 2 --------------------
	fwrite(Fichier[n]->nom,tai,1,fic);		//--- t --------------------
	fwrite(&Fichier[n]->desc,4,1,fic);		//--- desc -----------------
	fwrite(&Fichier[n]->next,4,1,fic);		//--- suiv -----------------
	fwrite(&Fichier[n]->size,4,1,fic);		//--- 4 --------------------
	fwrite(&Fichier[n]->time,2,1,fic);		//--- 2 --------------------
	fwrite(&Fichier[n]->date,2,1,fic);		//--- 2 --------------------
	fwrite(&Fichier[n]->attrib,1,1,fic);	//--- 1 --------------------

	free(Fichier[n]->nom);
	free(Fichier[n]);
	}


free(Desc);
free(Fichier);
free(Repert);
free(TabRec);

fclose(fic);

DFen=Fen->Fen2;
CommandLine("#cd .");
DFen=Fen->Fen2;

#endif
}
