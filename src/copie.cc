/*--------------------------------------------------------------------*\
|-	 Copy Function													  -|
\*--------------------------------------------------------------------*/
#include <ctype.h>

#include <stdio.h>
#include <string.h>

#include <dos.h>

#include <io.h>
#include <direct.h>
#include <malloc.h> 		//-- uniquement pour le free ---------------

#include <fcntl.h>

#include "rbio.h"

#include "kk.h"
#include "idf.h"

#include "kkt\copie1.h"

int xcop;

int OldNbrSel;	 //--- Nombre de fichier selectionn‚ au d‚part ----------

#define TEMPNAME "~(~~__)_"

/*--------------------------------------------------------------------*\
|- prototype														  -|
\*--------------------------------------------------------------------*/
void DispCopyMess(char *inpath,char *outpath);

int recopy(char *inpath,char *outpath,struct file *F);
int Copytree(char *inpath,char *outpath);
int truecopy(char *inpath,char *outpath);

void ArjCopie(FENETRE *F1,FENETRE *F2);
void LhaCopie(FENETRE *F1,FENETRE *F2);
void RarCopie(FENETRE *F1,FENETRE *F2);
void ZipCopie(FENETRE *F1,FENETRE *F2);
void KkdCopie(FENETRE *F1,FENETRE *F2);

void DFPcopie(FENETRE *F1,FENETRE *F2);
int DFPrecopy(FENETRE *F1,int F1pos,FENETRE *F2);

void DKFcopie(FENETRE *F1,FENETRE *F2);
int DKFrecopy(FENETRE *F1,int F1pos,FENETRE *F2);

void CopieRar(FENETRE *F1,FENETRE *F2);
void CopieKkd(FENETRE *F1,FENETRE *F2);

int RemoveM(char *inpath,char *outpath,struct file *F);
int Movetree(char *inpath,char *outpath);

int FenCopie2(FENETRE *F1,FENETRE *F2);
int FenCopie(FENETRE *F1,FENETRE *F2);
int FenCopieArc(FENETRE *F1,FENETRE *F2);
int FenMove(FENETRE *F1,FENETRE *F2);

int UserInt(void);		//--- Interruption par l'utilisateur -----------

int CountRepSize(FENETRE *F1,int *nbr,int *size);
					//--- Renvoit -1 si le nombre de fichier est nul ---

void UpdateName(char *buffer);

int SelectFile(FENETRE *F1,int i);
int Renome(char *inpath,char *outpath);

int KKDrecopy(FENETRE *F1,int F1pos,FENETRE *F2);
int KKD2File(FENETRE *F1,int F1pos,FENETRE *F2);
int File2KKD(FENETRE *F1,int F1pos,FENETRE *F2);

void ChgFromHeader(char *in,char *out,int chgext,int chgname);

bool FicCopy(RBFile *in,RBFile *out,long size);

/*--------------------------------------------------------------------*\
|- internal variable												  -|
\*--------------------------------------------------------------------*/


static int Nbrfic,Sizefic,Nbrcur,Sizecur;
static int Clock_Dep;
static int FicEcrase;
static char Dir[256];
static char temp[256];
static char bufton[32];
static long SizeMaxRecord;
extern FENETRE *Fenetre[4]; 	// uniquement pour trouver la 3‚me trash

static int chgattr=0;
static int chgext=0;
static int chgname=0;


void UpdateName(char *str)
{
char buf[256];
char c;
int i,j;

if ((KKCfg->_Win95) & (KKCfg->win95ln))
	return;

i=0;
j=0;

while(str[i]!=0)
	{
	c=(char)toupper(str[i]);
	if ( ((c>='0') & (c<='9')) | ((c>='A') & (c<='Z')) )
		{
		buf[j]=c;
		j++;
		}
	i++;
	if ((j==8) | (c=='.')) break;
	}
buf[j]=0;
strcpy(str,buf);
}




/*--------------------------------------------------------------------*\
|- Affiche le nom des fichiers … copier 							  -|
\*--------------------------------------------------------------------*/
void DispCopyMess(char *inpath,char *oupath)
{
char inbuf[256],oubuf[256];

strcpy(inbuf,inpath);
strcpy(oubuf,oupath);

ReduceString(inbuf,59);
ReduceString(oubuf,59);

PrintAt(xcop+2,5,_CopyFrom,inbuf);
PrintAt(xcop+2,6,_CopyTo  ,oubuf);
}

/*--------------------------------------------------------------------*\
|- Convertit un masque en ce qui faut ;)							  -|
\*--------------------------------------------------------------------*/
void MaskCnv(char *path)
{
if (!strcmp(temp,"*.*")) return;

Path2Abs(path,"..");
Path2Abs(path,temp);
}



/*--------------------------------------------------------------------*\
|- Extended copy													  -|
\*--------------------------------------------------------------------*/
void ExtendedCopy(void)
{
char buffer[256];
int n;

MacAlloc(10,80);
MacAlloc(11,80);

if (!(Info->macro))
	{
	strcpy(_sbuf[10],DFen->Fen2->path);
    Path2Abs(_sbuf[10],GetFilename(DFen,-1));

	if (DFen->path[1]==':')
		{
		DriveInfo((char)(DFen->path[0]-'A'),buffer);
        sprintf(_sbuf[11],_CopyLinkStr,buffer);
		}
	else
		{
        strcpy(_sbuf[11],_CopyLink);
		}
	}

NumHelp(108);
n=MWinTraite(copie1_kkt);

if (n==-1)
	{
	MacFree(10);
	MacFree(11);
	return;
	}

strcpy(buffer,DFen->path);
Path2Abs(buffer,_sbuf[10]);

if (n==1)
	{
	FILE *fic;
	int len;

	fic=fopen(buffer,"wb");

	fprintf(fic,"MCRB");
	fprintf(fic,"%30s",_sbuf[11]);
	fputc(1,fic);
	fputc(0,fic);

	strcpy(buffer,DFen->path);
    Path2Abs(buffer,GetFilename(DFen,-1));
	len=strlen(buffer);

	fputc(len+8,fic);

	for(n=0;n<7;n++)
		fputc(0,fic);

	fputc(2,fic);
	fputc(0x0A,fic);
	fputc(len,fic);
	fputc(0,fic);

	fprintf(fic,"%s",buffer);

	fputc(1,fic);
	fputc(0x67,fic);
	fputc(0,fic);
	fputc(0x06,fic);

	fclose(fic);

	}

MacFree(10);
MacFree(11);
}







/*--------------------------------------------------------------------*\
|- Copytree 														  -|
\*--------------------------------------------------------------------*/

int Copytree(char *inpath,char *outpath)
{
#ifdef __WC32__

DIR *dirp;
struct dirent *ff;
char error;


Path2Abs(inpath,"*.*");
dirp=opendir(inpath);
Path2Abs(inpath,"..");

if (KKCfg->longname)
	{
	char buf1[256],buf2[256],buf3[256];
	strcpy(buf1,outpath);
	Path2Abs(buf1,"..");
	Path2Abs(buf1,TEMPNAME);
	mkdir(buf1);
	Short2LongFile(inpath,buf2);
	FileinPath(buf2,buf3);
	strcpy(buf2,outpath);
	Path2Abs(buf2,"..");
	Path2Abs(buf2,buf3);
	InternalRename(buf1,buf2);
	}
	else
	mkdir(outpath);

if (dirp!=NULL)
	while(1)
	{
	ff=readdir(dirp);
	if (ff==NULL) break;

	error=ff->d_attr;

	if (ff->d_name[0]!='.')
		{
		Path2Abs(inpath,ff->d_name);
		Path2Abs(outpath,ff->d_name);
		if ((error&0x10)==0x10)
			Copytree(inpath,outpath);
			else
			truecopy(inpath,outpath);
		Path2Abs(inpath,"..");
		Path2Abs(outpath,"..");
		}
	if (FicEcrase==2) return 0;
	}

closedir(dirp);

return 1;

#else
return 0;
#endif
}


/*--------------------------------------------------------------------*\
|- Copie convenablement 											  -|
\*--------------------------------------------------------------------*/

int recopy(char *inpath,char *outpath,struct file *F)
{
int i;

if ((F->attrib & _A_SUBDIR)==_A_SUBDIR)
	i=Copytree(inpath,outpath);
	else
	i=truecopy(inpath,outpath);

if (FicEcrase==2) return 0;

return i;
}


/*--------------------------------------------------------------------*\
|- Impossible de copier 											  -|
\*--------------------------------------------------------------------*/
void ProtFile(char *path)
{
char Dir[256];

if (FicEcrase==1) return;

strcpy(Dir,KKMsgErr[0]);
strcat(Dir,"\n");
strcat(Dir,path);

switch(WinError(Dir))
    {
    case 0: //ok
        break;
    case 1: //cancel
		FicEcrase=2;	// Cancel
	}
}


/*--------------------------------------------------------------------*\
|- User interrupt													  -|
\*--------------------------------------------------------------------*/
int UserInt(void)
{
static char CadreLength=70;

struct Tmt T[5] = {
	  {5,4,5, "     Yes     ",NULL}, // Copy
	  {22,4,5,"     No      ",NULL}, // No replace
	  {56,4,3,NULL,NULL},
	  { 5,2,0,"Do you will copy this file ?",NULL},
	  { 1,1,6,&CadreLength,NULL}
	  };

struct TmtWin F = {-1,8,74,14, "User Interrupt" };

int n;

n=WinTraite(T,5,&F,0);

switch(n)	{
	case 0:
		return 0;		// Replace

	case -1:
	case 1:
		return 1;		// Cancel
	case 2:
		FicEcrase=2;
		return 1;		// Cancel ALL
	}
return 0;
}


/*--------------------------------------------------------------------*\
|-	Fichier existe d‚j…: Renvoie 0 si il faut l'ecraser               -|
|-					   sinon retourne 1 							  -|
\*--------------------------------------------------------------------*/

int FileExist(char *path)
{
static char CadreLength=70;
static char Dir[256];

struct Tmt T[7] = {
	  {5,5,5, "     Yes     ",NULL}, // Copy
	  {22,5,5,"     No      ",NULL}, // No replace
	  {39,5,5,"   ALL Yes   ",NULL}, // Copy All
	  {56,5,3,NULL,NULL},
	  { 5,3,0,Dir,NULL},
	  { 5,2,0,"Overwrite file ?",NULL},
	  { 1,1,4,&CadreLength,NULL}
	  };

struct TmtWin F = {-1,8,74,15, "Copy" };

if (FicEcrase==1) return 0;

memcpy(Dir,path,255);

switch(WinTraite(T,7,&F,0))
	{
	case 0:
		return 0;		// Replace

	case -1:
	case 1:
		return 1;		// Cancel
	case 2:
		FicEcrase=1;
		return 0;		// Replace ALL

	case 3:
		FicEcrase=2;
		return 1;		// Cancel ALL
	}
return 0;
}

bool FicCopy(RBFile *in,RBFile *out,long size)
{
bool fin;
long TailleEnreg,TailleRest,Taille;
int j3,j4;
char *buffer;
//int bpt;			// Byte per tick

if (Nbrfic!=0)
	{
/*	  int n;
	PrintAt(0,0,"%d",Tpio);
	n=(GetClock()-Clock_Dep-((Tpio*Nbrcur)/1024));
	if (n!=0)
		bpt=Sizecur/n;
*/

	j4=LongGradue(xcop+3,15,60,0,Sizecur,Sizefic);
	PrintAt(xcop+45,14,"(%4d of %4d)",Nbrcur+1,Nbrfic);
	}

Taille=size;
TailleEnreg=SizeMaxRecord;


buffer=(char*)GetMem(TailleEnreg);

TailleRest=0;

j3=LongGradue(xcop+3,10,60,0,TailleRest,size);

fin=FALSE;

while ((!fin) & (Taille>0))
	{
	if (Taille<SizeMaxRecord) TailleEnreg=Taille;

	IOerr=0;
	TailleEnreg=in->read(buffer,TailleEnreg);
	if (IOerr==3)
		{
		fin=TRUE;
		break;
		}

	IOerr=0;
	out->write(buffer,TailleEnreg);
	if (IOerr==3)
		{
		fin=TRUE;
		break;
		}

	Taille-=TailleEnreg;
	TailleRest+=TailleEnreg;

	PrintAt(xcop+5,9,"Copying %9d of %9d",TailleRest,size);

	j3=LongGradue(xcop+3,10,60,j3,TailleRest,size);

	if (Nbrfic!=0)
		{
		PrintAt(xcop+5,14,"Copying %9d of %9d",Sizecur+TailleRest,Sizefic);
						 // Nbrcur+1  Nbrfic
		j4=LongGradue(xcop+3,15,60,j4,Sizecur+TailleRest,Sizefic);

		if (KKCfg->esttime==1)
			{
			int n,n1,n2;

			if (TailleRest>2048)
				{
/*				  n2=(GetClock()-Clock_Dep-((Tpio*Nbrcur)/1024))*
														 (Sizefic/2048);
*/
				n2=(GetClock()-Clock_Dep)*(Sizefic/2048);

				n=((Sizecur+TailleRest)/2048);
				if (n!=0)
					{
					n2=n2/n;
					n1=n2+Clock_Dep-GetClock();

					n1=n1/CLOCKK_PER_SEC;
					n2=n2/CLOCKK_PER_SEC;

					if ( (n1>=0) & (n2>=0) )
						{
						PrintAt(xcop+13,12,
						"Remaining: %6d sec. Expected: %6d sec.",n1,n2);
						//TitleBox("KKC: Remaining: %d sec",n1);
						}
					}
				}
			}
		}

	if (KbHit())
		{
		Wait(0,0);
		fin=(bool)UserInt();
		}
	}

/*
if (Nbrfic!=0)
	{
	int n;
	n=(bpt*(Nbrcur+1));
	if (n!=0)
		Tpio=(1024*bpt*(GetClock()-Clock_Dep)-(Sizecur+size))/n;

	PrintAt(0,0,"%d",Tpio);
	}
*/

LongGradue(xcop+3,10,60,j3,size,size);

free(buffer);

return fin;
}



void ChgFromHeader(char *in,char *out,int chgext,int chgname)
{
RB_IDF Info;
static char buffer[256],buf2[256];
char *ext;

strcpy(Info.path,in);  //--- Path en shortname -------------------------

if ((chgext) | (chgname))
	{
	if (Info.path[0]==0)
		WinError("IDF 9");
	Traitefic(&Info);
	}

if (KKCfg->longname)
	{
	Short2LongFile(in,buffer);
	FileinPath(buffer,buf2);
	Path2Abs(out,"..");
	Path2Abs(out,buf2);
	}

if ((chgname) & (*(Info.fullname)!=0))
	{
    FileinPath(out,buffer);
    Path2Abs(out,"..");

    if (*(Info.composer)!=0)
        {
        strcpy(buffer,Info.composer);
        strcat(buffer," - ");
        strcat(buffer,Info.fullname);
        }
    else
        strcpy(buffer,Info.fullname);

	UpdateName(buffer);
	Path2Abs(out,buffer);

    if (chgext)
        {
        strcpy(buf2,Info.ext);
        strlwr(buf2);

        strcat(out,".");
        strcat(out,buf2);
        }
    }
else if ((chgext) | (chgname))
	{
	strcpy(buf2,Info.ext);
	strlwr(buf2);

	FileinPath(out,buffer);
	Path2Abs(out,"..");

	ext=strchr(buffer,'.');
	if (ext!=NULL)
		strcpy(ext+1,buf2);
		else
		{
		strcat(buffer,".");
		strcat(buffer,buf2);
		}

	Path2Abs(out,buffer);
	}
}


/*--------------------------------------------------------------------*\
|-							Fonction RENAME 						  -|
\*--------------------------------------------------------------------*/
void WinRename2(FENETRE *F1)
{
static char From[256],Temp[256],FromPath[256];
static char OutPath[2][256];
static int DirLength=70;

static char Length=55;
static int High=1;

struct Tmt T[] = {
	  { 2,2,1, OutPath[0], &DirLength},
	  { 2,4,5, "  Rename    ",NULL},
	  {59,4,5, "    Rename  ",NULL},
	  { 2,6,1, OutPath[1], &DirLength},

	  { 2,8,3,NULL,NULL},

	  {16,7,9,&Length,&High},
	  {17,8,0,"From",NULL},
	  {22,8,0,FromPath,NULL}
	  };

struct TmtWin F = {-1,10,74,20,"Rename"};

int n;

#define LGS 50

if (strlen(F1->path)>LGS)
	{
	memcpy(FromPath,F1->path,3);
	memcpy(FromPath+3,"...",3);
	memcpy(FromPath+6,F1->path+strlen(F1->path)-(LGS-6),LGS-6);
	FromPath[LGS]=0;
	}
else
	strcpy(FromPath,F1->path);

strcpy(From,F1->path);
Path2Abs(From,GetFilename(F1,-1));

strcpy(Temp,From);
ChgFromHeader(From,Temp,1,0);
FileinPath(Temp,OutPath[0]);

strcpy(Temp,From);
ChgFromHeader(From,Temp,1,1);
FileinPath(Temp,OutPath[1]);

n=WinTraite(T,8,&F,0);

if (n!=-1)
	{
	Temp[0]=0;
	switch(n)
		{
		case 1:
			strcpy(Temp,F1->path);
			Path2Abs(Temp,OutPath[0]);
			break;
		case 2:
			strcpy(Temp,F1->path);
			Path2Abs(Temp,OutPath[1]);
			break;
		}
	if (Temp[0]!=0)
		{
		if (InternalRename(From,Temp)!=0)
			WinError("Couldn't rename file");
		}
	}
}

/*--------------------------------------------------------------------*\
|-							Fonction RENAME 						  -|
\*--------------------------------------------------------------------*/
void WinRename(FENETRE *F1)
{
static char Dir[70];
static char Name[256],Temp[256];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[5] = {
	  { 2,3,1, Dir, &DirLength},
	  {15,5,2,NULL,NULL},
	  {45,5,3,NULL,NULL},
	  { 5,2,0,"Rename file to",NULL},
	  { 1,1,4,&CadreLength,NULL}
	  };

struct TmtWin F = {-1,10,74,17,"Rename"};

int n;

strcpy(Dir,F1->path);
Path2Abs(Dir,GetFilename(F1,-1));

strcpy(Name,F1->path);
Path2Abs(Name,GetFilename(F1,-1));

n=WinTraite(T,5,&F,0);

strcpy(Temp,F1->path);
Path2Abs(Temp,Dir);


if (n!=-1)
	if (T[n].type!=3)
		if (InternalRename(Name,Temp)!=0)
			WinError("Couldn't rename file");
}



/*--------------------------------------------------------------------*\
|- Vrai copie de FICHIER											  -|
\*--------------------------------------------------------------------*/
int truecopy(char *inpath,char *outpath)
{
bool fin=0;
long size;
static char temppath[256];

char ok;

ok=1;


{
RBFile inhand,outhand;

strcpy(temppath,outpath);
Path2Abs(temppath,"..");
Path2Abs(temppath,TEMPNAME);

ChgFromHeader(inpath,outpath,chgext,chgname);

MaskCnv(outpath);

if (!WildCmp(inpath,outpath))
	{
//	  PrintAt(0,0,"%40s%40s",inpath,outpath);
	WinError("Source & Destination are the same");
	ok=0;
	}

DispCopyMess(inpath,outpath);

if (ok==1)
	{
	inhand.open(inpath,RBREAD);

	if (inhand.isopen)
		{
		long tfree;

		size=inhand.length;

		tfree=GetDiskFree((char)(toupper(outpath[0])-'A'));

		SizeMaxRecord=32*1024;

		if (tfree<size)
			{
            if (WinError(KKMsgErr[3])==1)
				FicEcrase=2;
			ok=0;
			}
		}
	else
		ok=0;
	}

if (ok==1)
	{
	if (IsFileExist(outpath))
		{
		if (((KKCfg->noprompt)&1)==0)
			if (FileExist(outpath)==1)
				{
				ok=0;
				}
			else
				{
				_dos_setfileattr(outpath,0);
				remove(outpath);
				}
		}
	}

IOver=1;
IOerr=0;

if (ok==1)
	{
	outhand.open(temppath,RBWRITE);
	if ( (outhand.isopen!=1) | (IOerr!=0) )
		{
		ProtFile(outpath);
		ok=0;
		}
	IOver=0;
	}

if (ok==1)
	{
	fin=FicCopy(&inhand,&outhand,size);
	}
}

if ( (ok==1) & (fin==0) )	   //--- Mise a l'heure --------------------
	{
	unsigned short d,t;
	int handle;

#ifdef __WC32__
	_dos_open(inpath,O_RDONLY,&handle);
	_dos_getftime(handle,&d,&t);
	_dos_close(handle);

	_dos_open(temppath,O_RDONLY,&handle);
	_dos_setftime(handle,d,t);
	_dos_close(handle);
#endif

	if (chgattr)
		{
		unsigned attrib;

		_dos_getfileattr(inpath,&attrib);
		_dos_setfileattr(temppath,attrib);
		}

	if (InternalRename(temppath,outpath)!=0)
		{
        WinError(KKMsgErr[0]);
		remove(temppath);	// Efface le fichier temporaire si il y lieu
		}
	}

if (Nbrfic!=0)
	{
	Nbrcur++;
	Sizecur+=size;
	}

if (fin==1)
	{
	remove(temppath);
	ok=0;
	}

return ok;
}


/*--------------------------------------------------------------------*\
|- retourne 0 si pas copie											  -|
\*--------------------------------------------------------------------*/

int FenCopie(FENETRE *F1,FENETRE *FTrash)
{
static int DirLength=70;
static char CadreLength=70;
static int n,m,o;
static char buffer[80];
static int hauteur=3;

struct Tmt T[] = {
	  { 2,3,1,Dir,&DirLength},
	  { 5,6,8,"Keep attribut",&chgattr},
	  { 5,7,8,"Change extension",&chgext},
	  { 5,8,8,"Change filename",&chgname},
	  {15,10,2,NULL,NULL},			 // le OK
	  {45,10,3,NULL,NULL},			 // le CANCEL
	  { 5,2,0,buffer,NULL},
	  { 1,1,4,&CadreLength,NULL},
	  { 1,5,9,&CadreLength,&hauteur}
	  };

struct TmtWin F = {-1,10,74,22,"Copy" };

chgattr=0;
chgext=0;
chgname=0;

if (Nbrfic!=0)
	{
	if (Nbrfic==1)
        sprintf(buffer,_CopyOneTo,Long2Str(Sizefic,bufton));
    else
        sprintf(buffer,_CopyMulTo,Nbrfic,Long2Str(Sizefic,bufton));
	}
else
	{
    strcpy(buffer,_CopyFilTo);
	}

memcpy(Dir,FTrash->path,255);

n=0;
if (((KKCfg->noprompt)&1)==0)
	{
	NumHelp(10);
	n=WinTraite(T,9,&F,0);
	}

Path2Abs(Dir,".");

strcpy(temp,F1->path);
Path2Abs(temp,Dir);
strcpy(Dir,temp);


if (n!=-1)	// pas escape
	{
	o=-1;
	for (m=0;m<strlen(Dir);m++)
		if ( (Dir[m]=='\\') | (Dir[m]=='/') ) o++;

/*	  if (o==-1)
		{
		strcpy(temp,F1->path);
		Path2Abs(temp,Dir);
		strcpy(Dir,temp);
		}
*/

	strcpy(temp,"*.*");    //--- Masque de conversion ------------------

	if (T[n].type!=3) // Pas cancel
		{
		if (!strcmp(Dir,F1->path)) return 1;

		if (FTrash->system==0)	// Pour pouvoir copie dans les archives-
			{
			if (chdir(Dir)!=0)
				{
				strcpy(temp,Dir);
				Path2Abs(temp,"..");
				if (chdir(temp)!=0)
					{
					WinError("Unknown path");
					return 0;
					}
				FileinPath(Dir,temp);
				Path2Abs(Dir,"..");
				}
			}

		DFen=FTrash;
		CommandLine("#cd %s",Dir);

		return 1;
		}
	}

return 0;		//--- Erreur -------------------------------------------
}

int FenCopie2(FENETRE *F1,FENETRE *FTrash)
{
static int DirLength=70;
static char CadreLength=70;
static int n;
static char buffer[80];

struct Tmt T[] = {
	  { 2,3,1,Dir,&DirLength},
	  {15,5,2,NULL,NULL},			// le OK
	  {45,5,3,NULL,NULL},			// le CANCEL
	  { 5,2,0,buffer,NULL},
	  { 1,1,4,&CadreLength,NULL}
	  };

struct TmtWin F = {-1,10,74,17,"External Copy" };

if (Nbrfic!=0)
	{
	if (Nbrfic==1)
        sprintf(buffer,_CopyOneTo,Long2Str(Sizefic,bufton));
    else
        sprintf(buffer,_CopyMulTo,Nbrfic,Long2Str(Sizefic,bufton));
	}
	else
	{
	strcpy(buffer,"Copy files to");
	}

memcpy(Dir,FTrash->path,255);

n=0;
if (((KKCfg->noprompt)&1)==0)
	{
	NumHelp(10);
	n=WinTraite(T,5,&F,0);
	}

Path2Abs(Dir,".");

strcpy(temp,F1->path);
Path2Abs(temp,Dir);
strcpy(Dir,temp);

if (n!=-1)	//--- pas escape -------------------------------------------
	{
	if (T[n].type!=3) // Pas cancel
		{
		if (!strcmp(Dir,F1->path)) return 1;

		if (FTrash->system==0)	// Pour pouvoir copie dans les archives
			if (chdir(Dir)!=0)
				{
                WinError(KKMsgErr[4]);
				return 0;
				}

		DFen=FTrash;
		CommandLine("#cd %s",Dir);

		return 1; //--- OK ---------------------------------------------
		}
	}

return 0;		//--- Erreur -------------------------------------------
}

/*--------------------------------------------------------------------*\
|- retourne 0 si pas move											  -|
\*--------------------------------------------------------------------*/
int FenMove(FENETRE *F1,FENETRE *FTrash)
{
static int DirLength=70;
static char CadreLength=70;
static char Dir[256];
static int n,m,o;

struct Tmt T[5] = {
	  { 2,3,1,Dir,&DirLength},
	  {15,5,2,NULL,NULL},
	  {45,5,3,NULL,NULL},
	  { 5,2,0,"Move files to",NULL},
	  { 1,1,4,&CadreLength,NULL}
	  };

struct TmtWin F = {-1,10,74,17,"Move"};

chgext=0;
chgattr=0;

memcpy(Dir,FTrash->path,255);

n=0;
if (((KKCfg->noprompt)&1)==0)
	{
	NumHelp(11);
	n=WinTraite(T,5,&F,0);
	}

Path2Abs(Dir,".");

if (n!=-1)	// pas escape
	{
	o=-1;
	for (m=0;m<strlen(Dir);m++)
		if ( (Dir[m]=='\\') | (Dir[m]=='/') ) o++;

	if (o==-1)
		{
		strcpy(temp,F1->path);
		Path2Abs(temp,Dir);
		strcpy(Dir,temp);
		}

	strcpy(temp,"*.*");

	if (T[n].type!=3) // Pas cancel
		{
		if (!strcmp(Dir,F1->path)) return 1;

		if (chdir(Dir)!=0)
			{
			strcpy(temp,Dir);
			Path2Abs(temp,"..");
			if (chdir(temp)!=0)
				{
                WinError(KKMsgErr[4]);
				return 0;
				}
			FileinPath(Dir,temp);
			Path2Abs(Dir,"..");
			}

		DFen=FTrash;
		CommandLine("#cd %s",Dir);

		return 1;
		}
	}

return 0;		// Erreur
}

/*--------------------------------------------------------------------*\
|- Copy files to .rar (experimental)								  -|
\*--------------------------------------------------------------------*/
void CopieRar(FENETRE *F1,FENETRE *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];
char player[256];

fic=fopen(KKFics->temp,"wt");

strcpy(nom,F1->path);

if (nom[strlen(nom)-1]!='\\')  strcat(nom,"\\");

for(i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

	if (SelectFile(F1,i))
		{
		FicSelect(F1,i,0);

		if (IsDir(F))
			fprintf(fic,"%s%s\\*.*\n",nom,F->name);
			else
			fprintf(fic,"%s%s\n",nom,F->name);
		}
	}
fclose(fic);

strcpy(nom,F2->path+strlen(F2->VolName));

if (PlayerIdf(player,30)==0)
	{
	KKCfg->scrrest=0;
	CommandLine("#%s a -c- -ep1 -std %s @%s %s >nul",player,F2->VolName,
													  KKFics->temp,nom);
	}
}

/*--------------------------------------------------------------------*\
|- Copy file of .rar												  -|
\*--------------------------------------------------------------------*/
void RarCopie(FENETRE *F1,FENETRE *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];
char player[256];

char option;


option='E';

fic=fopen(KKFics->temp,"wt");

if (strlen(F1->path)==strlen(F1->VolName))
	strcpy(nom,"");
	else
	{
	strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
	strcat(nom,"\\");

	}

for(i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

	if (SelectFile(F1,i))
		{
		FicSelect(F1,i,0);

		if (IsDir(F))
			{
			fprintf(fic,"%s%s\\*.*\n",nom,F->name);
			option='X';
			}
			else
			fprintf(fic,"%s%s\n",nom,F->name);
		}
	}
fclose(fic);

if (PlayerIdf(player,34)==0)
	{
	KKCfg->scrrest=0;
	CommandLine("#%s %c -c- -std -y %s @%s %s >nul",player,option,
									 F1->VolName,KKFics->temp,F2->path);
	}
}


/*--------------------------------------------------------------------*\
|- copy file of .zip												  -|
\*--------------------------------------------------------------------*/
void ZipCopie(FENETRE *F1,FENETRE *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];
char player[256];

char option;


option='e';

fic=fopen(KKFics->temp,"wt");

if (strlen(F1->path)==strlen(F1->VolName))
	strcpy(nom,"");
	else
	{
	strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
	strcat(nom,"\\");
	}

for(i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

	if (SelectFile(F1,i))
		{
		FicSelect(F1,i,0);

		if (IsDir(F))
			{
			fprintf(fic,"%s%s\\*.*\n",nom,F->name);
			option='d';
			}
		else
			fprintf(fic,"%s%s\n",nom,F->name);
		}
	}
fclose(fic);

// Execution of uncompressor

if (PlayerIdf(player,35)==0)
	{
	char old;

	KKCfg->scrrest=0;

	old=KKCfg->internshell;
	KKCfg->internshell=1;

	CommandLine("#%s -%c -o %s @%s %s >nul",player,option,F1->VolName,
												 KKFics->temp,F2->path);

	KKCfg->internshell=old;
	}					 // Overwrite newer file
}

/*--------------------------------------------------------------------*\
|- copy files of .arj												  -|
\*--------------------------------------------------------------------*/
void ArjCopie(FENETRE *F1,FENETRE *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];
char player[256];

char option;


option='E';

fic=fopen(KKFics->temp,"wt");

if (strlen(F1->path)==strlen(F1->VolName))
	strcpy(nom,"");
	else
	{
	strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
	strcat(nom,"\\");

	}

for(i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

	if (SelectFile(F1,i))
		{
		FicSelect(F1,i,0);

		if (IsDir(F))
			{
			fprintf(fic,"%s%s\\*.*\n",nom,F->name);
			option='X';
			}
			else
			fprintf(fic,"%s%s\n",nom,F->name);
		}
	}
fclose(fic);
if (PlayerIdf(player,30)==0)
	{
	KKCfg->scrrest=0;
	CommandLine("#%s %c -P -y %s %s !%s >nul",player,option,F1->VolName,
												 F2->path,KKFics->temp);
	}
}


/*--------------------------------------------------------------------*\
|- copy files of .lha												  -|
\*--------------------------------------------------------------------*/
void LhaCopie(FENETRE *F1,FENETRE *F2)
{
FILE *fic;
int i;
struct file *F;
char nom[256];
char player[256];

char option;


option='E';

fic=fopen(KKFics->temp,"wt");

if (strlen(F1->path)==strlen(F1->VolName))
	strcpy(nom,"");
	else
	{
	strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
	strcat(nom,"\\");

	}

for(i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

	if (SelectFile(F1,i))
		{
		FicSelect(F1,i,0);

		if (IsDir(F))
			{
			fprintf(fic,"%s%s\\*.*\n",nom,F->name);
			option='X';
			}
			else
			fprintf(fic,"%s%s\n",nom,F->name);
		}
	}
fclose(fic);
if (PlayerIdf(player,32)==0)
	{
	KKCfg->scrrest=0;
	CommandLine("#%s %c -a %s -w%s @%s >nul",player,option,F1->VolName,
						F2->path,KKFics->temp);
	}
}






/*--------------------------------------------------------------------*\
|- Movetree 														  -|
\*--------------------------------------------------------------------*/
int Movetree(char *inpath,char *outpath)
{
#ifdef __WC32__

int error;
DIR *dirp;
struct dirent *ff;

int i;

Path2Abs(inpath,"*.*");
Path2Abs(outpath,"*.*");
dirp=opendir(inpath);
Path2Abs(inpath,"..");
Path2Abs(outpath,"..");

mkdir(outpath);

if (dirp!=NULL)
	while(1)
	{
	ff=readdir(dirp);
	if (ff==NULL) break;

	error=ff->d_attr;

	if (ff->d_name[0]!='.')
		{
		Path2Abs(inpath,ff->d_name);
		Path2Abs(outpath,ff->d_name);
		if ((error&0x10)==0x10)
			{
			i=Movetree(inpath,outpath);
			rmdir(inpath);
			}
			else
			{
			if (Renome(inpath,outpath)!=0)
				{
				i=truecopy(inpath,outpath);
				if (i==1)
					remove(inpath);
				}
				else
				{
				i=1;
				}
			}
		Path2Abs(inpath,"..");
		Path2Abs(outpath,"..");
		}
	if (FicEcrase==2) return 0;
	}

closedir(dirp);

return 1;
#else
return 0;
#endif
}

int Renome(char *inpath,char *outpath)
{
int i;

ChgFromHeader(inpath,outpath,chgext,chgname);
MaskCnv(outpath);

DispCopyMess(inpath,outpath);

i=InternalRename(inpath,outpath);

return i;
}


/*--------------------------------------------------------------------*\
|- Move convenablement												  -|
\*--------------------------------------------------------------------*/
int RemoveM(char *inpath,char *outpath,struct file *F)
{
int i;

if ((F->attrib & _A_SUBDIR)==_A_SUBDIR)
	{
	i=Movetree(inpath,outpath);
	rmdir(inpath);
	}
	else
	{
	if (Renome(inpath,outpath)!=0)
		{
		i=truecopy(inpath,outpath);
		if (i==1)
			remove(inpath);
		}
		else
		{
		i=1;
		}
	}

if (FicEcrase==2) return 0;

return i;
}


/*--------------------------------------------------------------------*\
|- Impossible de mover												  -|
\*--------------------------------------------------------------------*/
void ProtFileM(char *path)
{
static char CadreLength=70;
static char Dir[256];

struct Tmt T[5] = {
	  {15,5,2,NULL,NULL}, // Move All
	  {45,5,3,NULL,NULL},
	  { 5,3,0,Dir,NULL},
	  { 5,2,0,"Couldn't create file",NULL},
	  { 1,1,4,&CadreLength,NULL}
	  };

struct TmtWin F = {-1,8,74,15,"Move" };

if (FicEcrase==1) return;

memcpy(Dir,path,255);

switch(WinTraite(T,5,&F,0))
	{
	case -1:
	case 1:
		FicEcrase=2;	// Cancel
	case 0:
		break;
	}
}



/*--------------------------------------------------------------------*\
|-	Fichier existe d‚j…: Renvoie 0 si il faut l'ecraser               -|
\*--------------------------------------------------------------------*/
int FileExistM(char *path)
{
static char CadreLength=70;
static char Dir[256];

struct Tmt T[7] = {
	  {5,5,5, "     Yes     ",NULL},                             // Move
	  {22,5,5,"     No      ",NULL},                       // No replace
	  {39,5,5,"   ALL Yes   ",NULL},                         // Move All
	  {56,5,3,NULL,NULL},
	  { 5,3,0,Dir,NULL},
	  { 5,2,0,"Overwrite file ?",NULL},
	  { 1,1,4,&CadreLength,NULL}
	  };

struct TmtWin F = {-1,8,74,15,"Move" };

if (FicEcrase==1) return 0;

memcpy(Dir,path,255);

switch(WinTraite(T,7,&F,0))
	{
	case 0:
		return 0;											  // Replace
	case -1:
	case 1:
		return 1;											   // Cancel
	case 2:
		FicEcrase=1;
		return 0;										  // Replace ALL
	case 3:
		FicEcrase=2;
		return 1;										   // Cancel ALL
	}
return 0;
}


/*--------------------------------------------------------------------*\
|- Fonction de MOVE principale										  -|
\*--------------------------------------------------------------------*/
void Move(FENETRE *F1,char *path)
{
int i;
int j1,j2;							 // postion du compteur (read,write)
char inpath[128],outpath[128];
struct file *F;
FENETRE *FTrash;

if (F1->FenTyp!=0) return;

FTrash=AllocWin();

OldNbrSel=F1->nbrsel;

strcpy(FTrash->path,path);

FicEcrase=0;

if (FenMove(F1,FTrash)==0)
	{
	FreeWin(FTrash);
	return;
	}

if (!strcmp(F1->path,FTrash->path))
	{
	FreeWin(FTrash);
	return;
	}

switch(F1->system)
	{
	case 0:
		break;
	default:
		YouMad("Move");
		FreeWin(FTrash);
		return;
	}

switch(FTrash->system)
	{
	case 0:
		break;
	default:
		YouMad("Move");
		FreeWin(FTrash);
		return;
	}

SaveScreen();


xcop=(Cfg->TailleX-66)/2;

Cadre(xcop,4,xcop+65,12,0,Cfg->col[55],Cfg->col[56]);
Window(xcop+1,5,xcop+64,11,Cfg->col[16]);

PrintAt(xcop+2,7,"Current");
Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);

Nbrfic=0;

j1=0;
j2=0;

for (i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

	if (SelectFile(F1,i))
		{
		strcpy(inpath,F1->path);
		Path2Abs(inpath,F->name);

		strcpy(outpath,FTrash->path);
		Path2Abs(outpath,F->name);

        if (RemoveM(inpath,outpath,GetFile(F1,i))==1)
			FicSelect(F1,i,0);
		}
	if (FicEcrase==2) break;
	}

// Fenˆtre 1 ne change pas


LoadScreen();
FreeWin(FTrash);

//TitleBox("KKC: Move finished");
}

int FenCopieArc(FENETRE *F1,FENETRE *FTrash)
{
char *ext,name[256];
char buf[256];
int m;

static int DirLength=70;
static char CadreLength=70;
static char buffer[80];

struct Tmt T[] = {
	  { 2,3,1,Dir,&DirLength},
	  {15,5,2,NULL,NULL},			// le OK
	  {45,5,3,NULL,NULL},			// le CANCEL
	  { 5,2,0,buffer,NULL},
	  { 1,1,4,&CadreLength,NULL}
	  };

struct TmtWin F = {-1,10,74,17,"Extract files" };

DFen=F1;

for(m=0;m<F1->nbrfic;m++)
	if (SelectFile(F1,m))
		{
        strcpy(name,GetFilename(F1,m));
		strcpy(buf,DFen->path);
		Path2Abs(buf,name);
		CommandLine("#CD %s",name);

		if (stricmp(buf,DFen->path)!=0)
			return 0;
		break;		//--- Car ca n'arrive qu'une fois ------------------
		}

sprintf(buffer,"Extract files of '%s' to",name);

for(m=0;m<F1->nbrfic;m++)
	FicSelect(F1,m,1);							  //--- Select ---------

memcpy(Dir,FTrash->path,255);

ext=strchr(name,'.');
if (ext!=NULL)
	{
	ext[0]=0;
	Path2Abs(Dir,name);
	}


m=0;
if (((KKCfg->noprompt)&1)==0)
	{
	NumHelp(504);
	m=WinTraite(T,5,&F,2);
	}

Path2Abs(Dir,".");

strcpy(temp,F1->path);
Path2Abs(temp,Dir);
strcpy(Dir,temp);

if (m!=-1)	//--- pas escape -------------------------------------------
	{
	if (T[m].type!=3) // Pas cancel
		{
		if (!strcmp(Dir,F1->path)) return 1;

		DFen=FTrash;
		MakeDir(Dir);
		CommandLine("#cd %s",Dir);

		return 1; //--- OK ---------------------------------------------
		}
	}

DFen=F1;
CommandLine("#CD ..");

return 0;		//--- Erreur -------------------------------------------
}



/*--------------------------------------------------------------------*\
|- Fonction de COPY principale										  -|
\*--------------------------------------------------------------------*/
void Copie(FENETRE *F1,char *path)
{
int i;
int j1,j2;							// postion du compteur (read,write)
char inpath[128],outpath[128];
struct file *F;
int dialogue;
FENETRE *FTrash;

if (F1->FenTyp!=0) return;

FTrash=AllocWin();

OldNbrSel=F1->nbrsel;

FicEcrase=0;

Nbrcur=0;
Sizecur=0;
if (CountRepSize(F1,&Nbrfic,&Sizefic)==-1)
	{
	FreeWin(FTrash);
	return;
	}

Clock_Dep=GetClock();

DFen=FTrash;
CommandLine("#cd %s",path);

dialogue=0;

if ((F1->system==0) & (F1->nbrsel<=1) & (((KKCfg->noprompt)&1)==0))
	{
	static char buf[256];
	int m;

	DFen=F1;

	strcpy(buf,F1->path);
	for(m=0;m<F1->nbrfic;m++)
		if (SelectFile(F1,m))
            Path2Abs(buf,GetFilename(F1,m));

	switch (i=NameIDF(buf))
		{
		case 30:													  // ARJ
		case 34:													  // RAR
		case 35:													  // ZIP
		case 32:													  // LHA
		case 102:													  // KKD
		case 139:													  // DFP
		case 195:													  // DKF
			dialogue=1;
			break;
		}
	}

if (F1->system!=0)
	dialogue=2;

switch(dialogue)
	{
	case 0:
		if (FenCopie(F1,FTrash)==0)
			{
			FreeWin(FTrash);
			return;
			}
		break;
	case 1:
		if (FenCopieArc(F1,FTrash)==0)
			if (FenCopie(F1,FTrash)==0)
			{
			FreeWin(FTrash);
			return;
			}
		break;
	case 2:
		if (FenCopie2(F1,FTrash)==0)
			{
			FreeWin(FTrash);
			return;
			}
		break;
	}


switch(F1->system)
	{
	case 1:
		RarCopie(F1,FTrash);
		FreeWin(FTrash);
		return;
	case 2:
		ArjCopie(F1,FTrash);
		FreeWin(FTrash);
		return;
	case 3:
		ZipCopie(F1,FTrash);
		FreeWin(FTrash);
		return;
	case 4:
		LhaCopie(F1,FTrash);
		FreeWin(FTrash);
		return;
	case 5:
		KkdCopie(F1,FTrash);
		FreeWin(FTrash);
		return;
	case 6:
		DFPcopie(F1,FTrash);
		FreeWin(FTrash);
		return;
	case 9:
		DKFcopie(F1,FTrash);
		FreeWin(FTrash);
		return;

	case 0:
		break;
	default:
		YouMad("Copie");
		FreeWin(FTrash);
		return;
	}

switch(FTrash->system)
	{
	case 0:
		break;
	case 1:
		CopieRar(F1,FTrash);
		FreeWin(FTrash);
		return;
	case 5:
		CopieKkd(F1,FTrash);
		FreeWin(FTrash);
		return;
	default:
		YouMad("Copie");
		FreeWin(FTrash);
		return;
	}

SaveScreen();

xcop=(Cfg->TailleX-66)/2;

if (Nbrfic!=0)
	{
	Cadre(xcop,4,xcop+65,17,0,Cfg->col[55],Cfg->col[56]);
	Window(xcop+1,5,xcop+64,16,Cfg->col[16]);

	PrintAt(xcop+2,7,"Current");
	Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);

	PrintAt(xcop+2,12,"Total");
	Cadre(xcop+2,13,xcop+63,16,2,Cfg->col[55],Cfg->col[56]);
	}
	else
	{
	Cadre(xcop,4,xcop+65,12,0,Cfg->col[55],Cfg->col[56]);
	Window(xcop+1,5,xcop+64,11,Cfg->col[16]);

	PrintAt(xcop+2,7,"Current");
	Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);
	}

j1=0;
j2=0;

for (i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

    if (SelectFile(F1,i))
		{
		strcpy(inpath,F1->path);
		Path2Abs(inpath,F->name);

		strcpy(outpath,FTrash->path);
		Path2Abs(outpath,F->name);

        if (recopy(inpath,outpath,GetFile(F1,i))==1)
			FicSelect(F1,i,0);
		}
	if (FicEcrase==2) break;
	}

LoadScreen();

FreeWin(FTrash);

//TitleBox("KKC: Copy finished");
}

/*--------------------------------------------------------------------*\
|- Renvoit 1 si on doit copier le fichier							  -|
\*--------------------------------------------------------------------*/
int SelectFile(FENETRE *F1,int i)
{
struct file *F;

F=GetFile(F1,i);

if (F->name[0]=='.') return 0;
if (F->name[1]==':') return 0;
if (F->name[1]=='*') return 0;
if (F->name[0]=='*') return 0;

if (((KKCfg->noprompt)&1)==0)
	{
    if ( (F->select==1) | ((OldNbrSel==0) & (F1->pcur==i)) )
		return 1;
    else
		return 0;
	}
else
	{
	return (i==F1->nopcur);
	}
}


/*--------------------------------------------------------------------*\
|- Calcul de la taille d'un repertoire                                -|
\*--------------------------------------------------------------------*/
int CountRepSize(FENETRE *F1,int *nbr,int *size)
{
char cont;
struct file *ff;
char error;
char **TabRec;
int NbrRec;
int x,m;
char nom2[256];

char moi[256],nom[256];
FENETRE *FTrash;

int i;

int ok=1;

FTrash=AllocWin();

SaveScreen();

x=(Cfg->TailleX-30)/2;

Window(x,6,x+31,10,Cfg->col[16]);
Cadre(x-1,5,x+32,11,0,Cfg->col[55],Cfg->col[56]);

PrintAt(x+10,6,"Please Wait");
PrintAt(x+4,7,"Computing size of files");
PrintAt(x+1,8,"(Continue by pressing any key)");

*size=0;
*nbr=0;

for (i=0;(i<F1->nbrfic) & (!KbHit());i++)
{

if (SelectFile(F1,i))
	{
    struct file *F;

    F=GetFile(F1,i);

    if ((F->attrib & _A_SUBDIR)==_A_SUBDIR)
		{
		strcpy(nom2,F1->path);
        Path2Abs(nom2,F->name);

		DFen=FTrash;

		TabRec=(char**)GetMem(500*sizeof(char*));
		TabRec[0]=(char*)GetMem(strlen(nom2)+1);
		memcpy(TabRec[0],nom2,strlen(nom2)+1);
		NbrRec=1;

		do
		{
		m=strlen(TabRec[NbrRec-1]);

		m= (m>=72) ? m-72 : 0;

		CommandLine("#cd %s",TabRec[NbrRec-1]);

		strcpy(nom,TabRec[NbrRec-1]);

/*--------------------------------------------------------------------*\
|-	The files														  -|
\*--------------------------------------------------------------------*/

		for (m=0;(m<DFen->nbrfic) & (!KbHit());m++)
			{
            ff=GetFile(DFen,m);

			error=ff->attrib;

			if (ff->name[0]!='.')
				{
				cont=1;

				if (IsDir(ff)) cont=0; // Not Subdir
				if ((error&0x08)==0x08) cont=0; // Not Subdir

				if (cont)
					{
					(*nbr)++;
					(*size)+=ff->size;
					PrintAt(x,10,"%11s bytes in %5d files",
										   Long2Str(*size,bufton),*nbr);
					}
				}
			}

		free(TabRec[NbrRec-1]);
		NbrRec--;

/*--------------------------------------------------------------------*\
|-	The directories 												  -|
\*--------------------------------------------------------------------*/

		for (m=0;m<DFen->nbrfic;m++)
			{
            ff=GetFile(DFen,m);

			error=ff->attrib;

/*--------------------------------------------------------------------*\
|- Subdir															  -|
\*--------------------------------------------------------------------*/

			if (ff->name[0]!='.')
				{
				if ( (IsDir(ff))  & (DFen->system==0) )
					{
					strcpy(moi,nom);
					Path2Abs(moi,ff->name);

					TabRec[NbrRec]=(char*)GetMem(strlen(moi)+1);
					memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
					NbrRec++;
					}
				}
			}
		}
		while ( (NbrRec>0) & (!KbHit()) );

		free(TabRec);
		}
else
		{
		(*nbr)++;
        (*size)+=F->size;
		PrintAt(x,10,"%11s bytes in %5d files",
										   Long2Str(*size,bufton),*nbr);
		}
	} //--- END IF SELECT ----------------------------------------------

} //--- END FOR --------------------------------------------------------

if (*nbr==0) ok=-1;

if (KbHit())
	{
	int car;

	car=Wait(0,0);

	if (car!=27)
		PutKey(car);

	*size=0;
	*nbr=0;
	ok=1;
	}

LoadScreen();

FreeWin(FTrash);
return ok;
}



/*--------------------------------------------------------------------*\
|- Calcul de la taille d'un repertoire                                -|
\*--------------------------------------------------------------------*/
int RepSize(char *path)
{
char cont;
struct file *ff;
char error;
char **TabRec;
int NbrRec;
int x,m;
char nom2[256];

char moi[256],nom[256];
FENETRE *FTrash;

int size;
int nbr;

FTrash=AllocWin();

SaveScreen();

x=(Cfg->TailleX-30)/2;

Window(x,6,x+31,10,Cfg->col[16]);
Cadre(x-1,5,x+32,11,0,Cfg->col[55],Cfg->col[56]);

PrintAt(x+10,6,"Please Wait");
PrintAt(x+4,7,"Computing size of files");
PrintAt(x+1,8,"(Continue by pressing any key)");

size=0;
nbr=0;

strcpy(nom2,path);

DFen=FTrash;

TabRec=(char**)GetMem(500*sizeof(char*));
TabRec[0]=(char*)GetMem(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
m=strlen(TabRec[NbrRec-1]);

m= (m>=72) ? m-72 : 0;

CommandLine("#cd %s",TabRec[NbrRec-1]);

strcpy(nom,TabRec[NbrRec-1]);

/*--------------------------------------------------------------------*\
|-	The files														  -|
\*--------------------------------------------------------------------*/

for (m=0;(m<DFen->nbrfic) & (!KbHit());m++)
	{
    ff=GetFile(DFen,m);

	error=ff->attrib;

	if (ff->name[0]!='.')
		{
		cont=1;

		if (IsDir(ff)) cont=0; // Not Subdir
		if ((error&0x08)==0x08) cont=0; // Not Subdir

		if (cont)
			{
			nbr++;
			size+=ff->size;
			PrintAt(x,10,"%11s bytes in %5d files",
								   Long2Str(size,bufton),nbr);
			}
		}
	}

free(TabRec[NbrRec-1]);
NbrRec--;

/*--------------------------------------------------------------------*\
|-	The directories 												  -|
\*--------------------------------------------------------------------*/

for (m=0;m<DFen->nbrfic;m++)
	{
    ff=GetFile(DFen,m);

	error=ff->attrib;

/*--------------------------------------------------------------------*\
|- Subdir															  -|
\*--------------------------------------------------------------------*/

	if (ff->name[0]!='.')
		{
		if ( (IsDir(ff))  & (DFen->system==0) )
			{
			strcpy(moi,nom);
			Path2Abs(moi,ff->name);

			TabRec[NbrRec]=(char*)GetMem(strlen(moi)+1);
			memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
			NbrRec++;
			}
		}
	}
}
while ( (NbrRec>0) & (!KbHit()) );

free(TabRec);

if (KbHit())
	{
	int car;

	car=Wait(0,0);

	if (car!=27)
		PutKey(car);

	size=-1;
	nbr=0;
	}

LoadScreen();

FreeWin(FTrash);

return size;
}



/*--------------------------------------------------------------------*\
|- Copie des DKF													  -|
\*--------------------------------------------------------------------*/
void DKFcopie(FENETRE *F1,FENETRE *FTrash)
{
int i;

int j1,j2;							 // postion du compteur (read,write)

struct file *F;

SaveScreen();

xcop=(Cfg->TailleX-66)/2;

if (Nbrfic!=0)
	{
	Cadre(xcop,4,xcop+65,17,0,Cfg->col[55],Cfg->col[56]);
	Window(xcop+1,5,xcop+64,16,Cfg->col[16]);

	PrintAt(xcop+2,7,"Current");
	Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);

	PrintAt(xcop+2,12,"Total");
	Cadre(xcop+2,13,xcop+63,16,2,Cfg->col[55],Cfg->col[56]);
	}
	else
	{
	Cadre(xcop,4,xcop+65,12,0,Cfg->col[55],Cfg->col[56]);
	Window(xcop+1,5,xcop+64,11,Cfg->col[16]);

	PrintAt(xcop+2,7,"Current");
	Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);
	}

j1=0;
j2=0;

for (i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

    if (SelectFile(F1,i))
		{
		if (DKFrecopy(F1,i,FTrash)==1)
			FicSelect(F1,i,0);
		}
	if (FicEcrase==2) break;
	}

LoadScreen();
}

int DKFrecopy(FENETRE *F1,int F1pos,FENETRE *F2)
{
char key[17],comp[17];

char copieok;

char buffer[256];
char nom[256];

RBFile fic;

copieok=0;

if (strlen(F1->path)==strlen(F1->VolName))
	{
	strcpy(nom,"");
	}
	else
	{
	strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
	strcat(nom,"\\");
	}

PrintAt(xcop+2,5,"From%59s",F1->VolName);

strcpy(comp,GetFilename(F1,F1pos));

fic.open(F1->VolName,RBREAD);

fic.seek(0x80,SEEK_SET);

while(1)
	{
	int deb,lng;
	ushort deb1,deb2;
	ushort lng1,lng2;

	fic.read(key,14);
	if (key[0]==0)
		break;
	key[14]=0;

	fic.read(&lng2,2);
	fic.read(&lng1,2);
	lng=lng1+lng2*65536;

	fic.seek(1,SEEK_CUR);

	fic.read(&deb2,2);
	fic.read(&deb1,2);
	deb=deb1+deb2*65536;

	if (!stricmp(key,comp))
		{
		int pos;
		pos=fic.tell();

		fic.seek(deb,SEEK_SET);

		strcpy(buffer,F2->path);
		Path2Abs(buffer,comp);

		RBFile *outfic=new RBFile(buffer,RBWRITE);

		PrintAt(xcop+2,6,"To  %59s",buffer);

		SizeMaxRecord=32*1024;

		if (FicCopy(&fic,outfic,lng)==1)
			copieok=0;
			else
			copieok=1;

		delete outfic;

//		  _dos_open(buffer,O_RDONLY,&handle);
//		  _dos_setftime(handle,F1->F[F1pos]->date,F1->F[F1pos]->time);
//		  _dos_close(handle);

		fic.seek(pos,SEEK_SET);
		}

	fic.seek(16,SEEK_CUR);
	}

if (Nbrfic!=0)
	{
    struct file *F;

    F=GetFile(F1,F1pos);

	Nbrcur++;
    Sizecur+=F->size;
	}

return copieok;
}







/*--------------------------------------------------------------------*\
|- Copie des DFP													  -|
\*--------------------------------------------------------------------*/
void DFPcopie(FENETRE *F1,FENETRE *FTrash)
{
int i;

int j1,j2;							 // postion du compteur (read,write)

struct file *F;

SaveScreen();

xcop=(Cfg->TailleX-66)/2;

if (Nbrfic!=0)
	{
	Cadre(xcop,4,xcop+65,17,0,Cfg->col[55],Cfg->col[56]);
	Window(xcop+1,5,xcop+64,16,Cfg->col[16]);

	PrintAt(xcop+2,7,"Current");
	Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);

	PrintAt(xcop+2,12,"Total");
	Cadre(xcop+2,13,xcop+63,16,2,Cfg->col[55],Cfg->col[56]);
	}
	else
	{
	Cadre(xcop,4,xcop+65,12,0,Cfg->col[55],Cfg->col[56]);
	Window(xcop+1,5,xcop+64,11,Cfg->col[16]);

	PrintAt(xcop+2,7,"Current");
	Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);
	}

j1=0;
j2=0;

for (i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

    if (SelectFile(F1,i))
		{
		if (DFPrecopy(F1,i,FTrash)==1)
			FicSelect(F1,i,0);
		}
	if (FicEcrase==2) break;
	}

LoadScreen();
}

int DFPrecopy(FENETRE *F1,int F1pos,FENETRE *F2)
{
char key[14],comp[14];
int deb,lng,nbr;

char copieok;

char buffer[256];
char nom[256];

RBFile fic;

int m,n;

copieok=0;

if (strlen(F1->path)==strlen(F1->VolName))
	{
	strcpy(nom,"");
	}
	else
	{
	strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
	strcat(nom,"\\");
	}

PrintAt(xcop+2,5,"From%59s",F1->VolName);

strcpy(comp,GetFilename(F1,F1pos));
for(m=0;m<12;m++)
	if (comp[m]==32) comp[m]=0;

fic.open(F1->VolName,RBREAD);

fic.seek(5,SEEK_SET);

fic.read(&nbr,4);

for(n=0;n<nbr;n++)
	{
	fic.read(key,12);
	key[12]=0;

	fic.read(&deb,4);
	fic.read(&lng,4);

	for(m=0;m<12;m++)
		if (key[m]==32) key[m]=0;

	if (!stricmp(key,comp))
		{
		fic.seek(deb,SEEK_SET);

		strcpy(buffer,F2->path);
		Path2Abs(buffer,comp);

		RBFile *outfic=new RBFile(buffer,RBWRITE);

		PrintAt(xcop+2,6,"To  %59s",buffer);

		SizeMaxRecord=32*1024;

		if (FicCopy(&fic,outfic,lng)==1)
			copieok=0;
			else
			copieok=1;

		delete outfic;

//		  _dos_open(buffer,O_RDONLY,&handle);
//		  _dos_setftime(handle,F1->F[F1pos]->date,F1->F[F1pos]->time);
//		  _dos_close(handle);
		}
	}

if (Nbrfic!=0)
	{
    struct file *F;

    F=GetFile(F1,F1pos);

	Nbrcur++;
    Sizecur+=F->size;
	}

return copieok;
}

/*--------------------------------------------------------------------*\
|- Copie des KKD													  -|
\*--------------------------------------------------------------------*/
void KkdCopie(FENETRE *F1,FENETRE *FTrash)
{
int i;

int j1,j2;							 // postion du compteur (read,write)

struct file *F;

SaveScreen();

xcop=(Cfg->TailleX-66)/2;

if (Nbrfic!=0)
	{
	Cadre(xcop,4,xcop+65,17,0,Cfg->col[55],Cfg->col[56]);
	Window(xcop+1,5,xcop+64,16,Cfg->col[16]);

	PrintAt(xcop+2,7,"Current");
	Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);

	PrintAt(xcop+2,12,"Total");
	Cadre(xcop+2,13,xcop+63,16,2,Cfg->col[55],Cfg->col[56]);
	}
	else
	{
	Cadre(xcop,4,xcop+65,12,0,Cfg->col[55],Cfg->col[56]);
	Window(xcop+1,5,xcop+64,11,Cfg->col[16]);

	PrintAt(xcop+2,7,"Current");
	Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);
	}

j1=0;
j2=0;

for (i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

	if (SelectFile(F1,i))
		{
		if (KKDrecopy(F1,i,FTrash)==1)
			FicSelect(F1,i,0);
		}
	if (FicEcrase==2) break;
	}

LoadScreen();
}


int KKDrecopy(FENETRE *F1,int F1pos,FENETRE *F2)
{
struct file *F;
int res;

F=GetFile(F1,F1pos);

if ( (F->desc==0) | (IsDir(F)) )
	{
	static char nom[256];
	char inpath[256];
	char outpath[256];
	int i;

	FENETRE *FTrash,*OldFen;

	FTrash=AllocWin();
	OldFen=DFen;

	if (strlen(F1->path)==strlen(F1->VolName))
		strcpy(nom,"\\");
		else
		{
		strcpy(nom,"\\");
		strcat(nom,(F1->path)+strlen(F1->VolName)+1);
		}

	DFen=FTrash;

	if (F1->KKDdrive==0)
		F1->KKDdrive=(char)(ChangeToKKD()+1);
		else
		CommandLine("#cd %c:\\",F1->KKDdrive+'A'-1);

	CommandLine("#cd %s",nom);

	strcpy(nom,DFen->path);
    Path2Abs(nom,F->name);

	for (i=0;i<DFen->nbrfic;i++)
        if (!stricmp(F->name,GetFilename(DFen,i)))
			{
			DFen->pcur=i;
			DFen->scur=i;
			break;
			}

	strcpy(inpath,DFen->path);
    Path2Abs(inpath,GetFilename(F1,F1pos));

	strcpy(outpath,F2->path);
    Path2Abs(outpath,GetFilename(F1,F1pos));

    res=recopy(inpath,outpath,GetFile(DFen,-1));

	FreeWin(FTrash);
	DFen=OldFen;
	}
	else
	{
	KKD2File(F1,F1pos,F2);
	res=1;
	}

return res;
}

/*--------------------------------------------------------------------*\
|-							 FICHIER .KKD							  -|
\*--------------------------------------------------------------------*/

void CopieKkd(FENETRE *F1,FENETRE *FTrash)
{
int i;

int j1,j2;							 // postion du compteur (read,write)

struct file *F;

SaveScreen();

xcop=(Cfg->TailleX-66)/2;

if (Nbrfic!=0)
	{
	Cadre(xcop,4,xcop+65,17,0,Cfg->col[55],Cfg->col[56]);
	Window(xcop+1,5,xcop+64,16,Cfg->col[16]);

	PrintAt(xcop+2,7,"Current");
	Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);

	PrintAt(xcop+2,12,"Total");
	Cadre(xcop+2,13,xcop+63,16,2,Cfg->col[55],Cfg->col[56]);
	}
	else
	{
	Cadre(xcop,4,xcop+65,12,0,Cfg->col[55],Cfg->col[56]);
	Window(xcop+1,5,xcop+64,11,Cfg->col[16]);

	PrintAt(xcop+2,7,"Current");
	Cadre(xcop+2,8,xcop+63,11,2,Cfg->col[55],Cfg->col[56]);
	}


j1=0;
j2=0;

for (i=0;i<F1->nbrfic;i++)
	{
    F=GetFile(F1,i);

    if (SelectFile(F1,i))
		{
		if (File2KKD(F1,i,FTrash)==1)
			FicSelect(F1,i,0);
		}
	if (FicEcrase==2) break;
	}

LoadScreen();
}

struct kkdesc
	{
	long desc;
	long next;
	long size;
	unsigned short time;
	unsigned short date;
	char attrib;
	};

int File2KKD(FENETRE *F1,int F1pos,FENETRE *F2)
{
char foundfile;
char copieok;

struct kkdesc KKD_desc;

int z;
int lastpos;

char buffer[256];
char nom[256];
static char name[256];

char Nomarch[256];
char fin;
RBFile fic;

int n,i,j;

unsigned char tai;									  // taille des noms

struct file *F;
F=GetFile(F1,F1pos);

copieok=0;
foundfile=0;

if (strlen(F2->path)==strlen(F2->VolName))
	{
	strcpy(nom,"");
	}
	else
	{
	strcpy(nom,(F2->path)+strlen(F2->VolName)+1);
	strcat(nom,"\\");
	}

PrintAt(xcop+2,6,"To  %59s",F2->VolName);

fic.open(F2->VolName,RBREAD | RBWRITE);

fic.seek(3,SEEK_SET);									// Passe la cle

fic.read(&tai,1);								  // Passe la version

fic.read(&tai,1);							// Passe le nom du volume
fic.read(Nomarch,tai);

fic.read(&n,4); 			  // Passe le nombre d'octets non utilise
fic.read(&n,4);  // Passe le nbr de bytes non utilise avant reconstr.


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
fic.read(&tai,1);
fic.read(Nomarch,tai);
Nomarch[tai]=0;

fic.read(&KKD_desc,sizeof(struct kkdesc));

if ( (!stricmp(Nomarch,name)) & ((KKD_desc.attrib&0x10)==0x10) )
	{
	if (KKD_desc.desc==0)
		{
		fin=1;		// Un gros probleme: un directory qui ne mene a rien
		break;
		}
	fic.seek(KKD_desc.desc,SEEK_SET);

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
	fic.seek(KKD_desc.next,SEEK_SET);
	}
}

if (fin==0)
	{
	do
		{
		lastpos=fic.tell();

		fic.read(&tai,1);
		fic.read(Nomarch,tai);
		Nomarch[tai]=0;

		fic.read(&KKD_desc,sizeof(struct kkdesc));

        if (!stricmp(Nomarch,GetFilename(F1,F1pos)))
			{
			foundfile=1;
			fic.seek(-sizeof(struct kkdesc),SEEK_CUR);

			KKD_desc.desc=fic.length;

			fic.write(&KKD_desc,sizeof(struct kkdesc));

			fic.seek(0,SEEK_END);

			z=0;
			fic.write(&z,4);			//--- Information sur fichier

			strcpy(buffer,F1->path);
            Path2Abs(buffer,GetFilename(F1,F1pos));

			PrintAt(xcop+2,5,"From%59s",buffer);

			RBFile *infic=new RBFile(buffer,RBREAD);

			SizeMaxRecord=32*1024;

			if (FicCopy(infic,&fic,infic->length)==1)
				copieok=0;
				else
				copieok=1;

			delete infic;

			break;
			}

		fic.seek(KKD_desc.next,SEEK_SET);
		}
	while(KKD_desc.next!=0);
	}

if (foundfile==0)
	{
    fic.seek(lastpos,SEEK_SET);

	fic.read(&tai,1);
	fic.read(Nomarch,tai);
	Nomarch[tai]=0;

	fic.read(&KKD_desc,sizeof(struct kkdesc));
	fic.seek(-sizeof(struct kkdesc),SEEK_CUR);

	KKD_desc.next=fic.length;
	fic.write(&KKD_desc,sizeof(struct kkdesc));

	//--- Cree le nouveau fichier … la fin -----------------------------
	fic.seek(0,SEEK_END);

    strcpy(Nomarch,GetFilename(F1,F1pos));
	tai=(char)strlen(Nomarch);
	fic.write(&tai,1);
	fic.write(Nomarch,tai);

    if (IsDir(GetFile(F1,F1pos)))
		KKD_desc.desc=fic.tell()+sizeof(struct kkdesc);
		else
		KKD_desc.desc=0;

	KKD_desc.next=0;
    KKD_desc.size=F->size;
    KKD_desc.time=F->time;
    KKD_desc.date=F->date;
    KKD_desc.attrib=F->attrib;

	fic.write(&KKD_desc,sizeof(struct kkdesc));

	//--- Cree le repertoire .. si necessaire --------------------------

    if (IsDir(F))
		{
		strcpy(Nomarch,"..");
		tai=2;
		fic.write(&tai,1);
		fic.write(Nomarch,tai);

		KKD_desc.desc=0;
		KKD_desc.next=0;
        KKD_desc.size=F->size;
        KKD_desc.time=F->time;
        KKD_desc.date=F->date;
        KKD_desc.attrib=F->attrib;

		fic.write(&KKD_desc,sizeof(struct kkdesc));
		}
	}

if (Nbrfic!=0)
	{
	Nbrcur++;
    Sizecur+=F->size;
	}

return copieok;
}

int KKD2File(FENETRE *F1,int F1pos,FENETRE *F2)
{
char copieok;

struct kkdesc KKD_desc;

int z;

char buffer[256];
char nom[256];
static char name[256];

char Nomarch[256];
char fin;
RBFile fic;

int n,i,j;

unsigned char tai;									  // taille des noms

int handle;

struct file *F;
F=GetFile(F1,F1pos);

copieok=0;

if (strlen(F1->path)==strlen(F1->VolName))
	{
	strcpy(nom,"");
	}
	else
	{
	strcpy(nom,(F1->path)+strlen(F1->VolName)+1);
	strcat(nom,"\\");
	}

PrintAt(xcop+2,5,"From%59s",F1->VolName);

fic.open(F1->VolName,RBREAD);

fic.seek(3,SEEK_SET);									// Passe la cle

fic.read(&tai,1);								  // Passe la version

fic.read(&tai,1);							// Passe le nom du volume
fic.read(Nomarch,tai);

fic.read(&n,4); 			  // Passe le nombre d'octets non utilise
fic.read(&n,4);  // Passe le nbr de bytes non utilise avant reconstr.


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
fic.read(&tai,1);
fic.read(Nomarch,tai);
Nomarch[tai]=0;

fic.read(&KKD_desc,sizeof(struct kkdesc));

if ( (!stricmp(Nomarch,name)) & ((KKD_desc.attrib&0x10)==0x10) )
	{
	if (KKD_desc.desc==0)
		{
		fin=1;		// Un gros probleme: un directory qui ne mene a rien
		break;
		}
	fic.seek(KKD_desc.desc,SEEK_SET);

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
	fic.seek(KKD_desc.next,SEEK_SET);
	}
}

if (fin==0)
	{
	do
		{
		fic.read(&tai,1);
		fic.read(Nomarch,tai);
		Nomarch[tai]=0;

		fic.read(&KKD_desc,sizeof(struct kkdesc));

        if (!stricmp(Nomarch,F->name))
			{
			fic.seek(KKD_desc.desc,SEEK_SET);

			z=0;
			fic.read(&z,4); 			//--- Information sur fichier

			strcpy(buffer,F2->path);
            Path2Abs(buffer,F->name);

			RBFile *outfic=new RBFile(buffer,RBWRITE);

			PrintAt(xcop+2,6,"To  %59s",buffer);

			SizeMaxRecord=32*1024;

			if (FicCopy(&fic,outfic,KKD_desc.size)==1)
				copieok=0;
				else
				copieok=1;

			delete outfic;

			_dos_open(buffer,O_RDONLY,&handle);
			_dos_setftime(handle,KKD_desc.date,KKD_desc.time);
			_dos_close(handle);

			break;
			}

		fic.seek(KKD_desc.next,SEEK_SET);
		}
	while(KKD_desc.next!=0);
	}

if (Nbrfic!=0)
	{
	Nbrcur++;
    Sizecur+=F->size;
	}

return copieok;
}



