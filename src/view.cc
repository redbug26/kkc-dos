/*--------------------------------------------------------------------*\
|- Viewer:															  -|
|		   html: illimit‚ (d‚pend de la m‚moire)					  -|
|		   ansi: 1000 lignes maximum								  -|
|			txt: illimit‚											  -|
|			bin: illimit‚											  -|
|----------------------------------------------------------------------|
|- By RedBug/Ketchup^Pulpe											  -|
\*--------------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#ifdef __WC32__
	#include <dos.h>
#endif

#include "kk.h"
#include "idf.h"

// KKT macro
#include "altk0.h"
#include "altk1.h"
#include "viewsave.h"

int ansi_out (char b);

int NextFct(struct barmenu *bar);

void StartWinView(char *name,char *titre);

#define MAXANSLINE 1000

#define VIEWER_AUTO -2
#define VIEWER_BINARY -3
#define VIEWER_ANSI 86
#define VIEWER_HTML 104
#define VIEWER_TXT 91

char kk_raw;						//--- si vaut 1 -> kk_raw screen ansi -----
char secpart=0; 				//--- Affiche les messages secrets -----

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

class KKFile
{
public:
	KKFile(char *name);
	~KKFile();

	char ReadChar(void);
	char ReadNextChar(void);
	int Reload(int i);

	void SavePosition(char *savefile,int posn);
	int LoadPosition(char *savefile);

	long posn;				 //--- octet courant --------------------
	long taille;			 //--- taille du fichier qu'on utilise --

	long fromoct,nbroct;	 //--- premier octet - nombre d'octet ---

	char namebuf[256];		 //--- Titre du fichier r‚duit ----------

	void ReadBlock(void);

	void getname(char*);	 //--- Nom du fichier -------------------
	void setname(char*);	 //--- Nom du fichier -------------------

protected:
	char filename[256];

	int idf;				 //--- IDF Number -----------------------

	FILE *fic;				 //--- handle du fichier ----------------
	char *view_buffer;		 //--- buffer pour la lecture -----------
	long pos;				 //--- position de depart ---------------
	long sizebuf;			 //--- taille du buffer -----------------

	long sizefic;			 //--- taille reelle du fichier ---------
	long firstoct;			 //--- premier octet qu'on utilise vmt --

};

void KKFile::getname(char *name)
{
strcpy(name,filename);
}

void KKFile::setname(char *name)
{

strcpy(filename,name);
}

KKFile::KKFile(char *name)
{
fic=NULL;

fromoct=0;
nbroct=-1;

sizebuf=32768;	   //--- Taille Maximum du buffer de lecture -----------
view_buffer=(char*)GetMem(sizebuf); //--Allocation du buffer de lecture-

strcpy(filename,name);
}

KKFile::~KKFile()
{
if (fic!=NULL)
	fclose(fic);

LibMem(view_buffer);
}

int KKFile::Reload(int i)
{
RB_IDF Info;

char ficbuf[256];
char *numam;

strcpy(ficbuf,filename);
numam=strrchr(ficbuf,'#');
if (numam!=NULL)
	numam[0]=0;

if (fic!=NULL)
	fclose(fic);

fic=fopen(ficbuf,"rb");
if (fic==NULL)
	return -1;

strcpy(namebuf,filename);
ReduceString(namebuf,36);

fseek(fic,0,SEEK_END);
sizefic=ftell(fic);

taille=sizefic;
firstoct=fromoct;

if ((nbroct!=-1) & (nbroct<=taille-firstoct))
	taille=nbroct;
	else
	taille-=firstoct;

if ((taille<=0) | (firstoct<0))
	{
	return -1;
	}

pos=0;
ReadBlock();

posn=0;

if (i==VIEWER_AUTO)
	{
	Info.path[0]=0;
	Info.inbuf=view_buffer;
	if (taille<sizebuf)
		Info.buflen=taille;
		else
		Info.buflen=sizebuf;

	Traitefic(&Info);

	if (Info.numero==VIEWER_AUTO)
		i=VIEWER_BINARY;   //--- Binaire alors -------------------------
		else
		i=Info.numero;
	}

return i;
}


/*--------------------------------------------------------------------*\
|-	Lit l'octet se trouvant en posn                                   -|
\*--------------------------------------------------------------------*/
char KKFile::ReadChar(void)
{
if (posn>=taille)
	{
	posn=taille-1;
	return 0;
	}

if (posn<0)
	posn=0;

if (posn-pos>=sizebuf)
	{
	while (posn-pos>=sizebuf) pos+=sizebuf;
	ReadBlock();
	}

if (posn-pos<0)
	{
	while (posn-pos<0) pos-=sizebuf;
	ReadBlock();
	}

return view_buffer[posn-pos];
}

char KKFile::ReadNextChar(void)
{
char car;
long l;

l=posn;
posn++;
car=ReadChar();
posn=l;

return car;
}



/*--------------------------------------------------------------------*\
|- Lit un buffer entier de sizebuf octets							  -|
\*--------------------------------------------------------------------*/
void KKFile::ReadBlock(void)
{
int nbr;

fseek(fic,pos+firstoct,SEEK_SET);
nbr=fread(view_buffer,1,sizebuf,fic);
GetPreprocString(view_buffer,nbr);
}

/*--------------------------------------------------------------------*\
|- Load position of viewing in posdeb								  -|
\*--------------------------------------------------------------------*/
int KKFile::LoadPosition(char *savefile)
{
long posdeb=0;
FILE *fic;
uchar n;
long s,size;
char ficname[256];

fic=fopen(savefile,"rb");
if (fic!=NULL)
	{
	while(fread(&n,1,1,fic)==1)
		{
		fread(ficname,1,n,fic);
		fread(&size,4,1,fic);
		fread(&s,4,1,fic);
		if ( (!stricmp(ficname,filename)) & (size==sizefic) )
			{
			posdeb=s;
			break;
			}
		}
	fclose(fic);
	}

if ((posdeb-firstoct)<0)
	return 0;

return posdeb-firstoct;
}

/*--------------------------------------------------------------------*\
|- Save position of viewing 										  -|
\*--------------------------------------------------------------------*/
void KKFile::SavePosition(char *savefile,int posn)
{
long pos;
FILE *fic;
uchar n;
long s,size,pn;
char ficname[256];

pn=posn+firstoct;

fic=fopen(savefile,"r+b");

if (fic==NULL)
	fic=fopen(savefile,"w+b");

if (fic!=NULL)
	{
	fseek(fic,0,SEEK_SET);
	while(fread(&n,1,1,fic)==1)
		{
		fread(ficname,n,1,fic);
		if (!stricmp(ficname,filename))
			{
			pos=ftell(fic);
			fseek(fic,pos,SEEK_SET);// Parce qu'on ‚crit juste aprŠs

			fwrite(&sizefic,4,1,fic);
			fwrite(&pn,4,1,fic);
			pn=0;
			break;
			}
		fread(&size,1,4,fic);
		fread(&s,1,4,fic);
		}
	if (pn!=0)
		{
		pos=ftell(fic);
		fseek(fic,pos,SEEK_SET);	// Parce qu'on ‚crit juste aprŠs

		n=(char)(strlen(filename)+1);
		fwrite(&n,1,1,fic);
		fwrite(filename,n,1,fic);
		fwrite(&sizefic,4,1,fic);
		fwrite(&pn,4,1,fic);
		}
	fclose(fic);
	}
}





/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


void ChgViewPreproc(KKFile &Fic);



char GetHtmChr(int x,int y);

char GetHtmChr(int x,int y)
{
if (x<0) return 0;
if (x>=Cfg->TailleX) return 0;
return GetChr(x,y);
}

/*--------------------------------------------------------------------*\
|- Table de conversion												  -|
|- Les deux premieres tables proviennent de latin.85O de termail	  -|
\*--------------------------------------------------------------------*/
char TabDosWin[]={
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
	112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
	199,252,233,226,228,224,229,231,234,235,232,239,238,236,196,197,
	201,230,198,244,246,242,251,249,255,214,220,248,163,216,215,159,
	225,237,243,250,241,209,170,186,191,174,172,189,188,161,171,187,
	176,177,178,179,180,193,194,192,169,185,186,187,188,162,165,191,
	192,193,194,195,196,197,227,195,200,201,202,203,204,205,206,164,
	240,208,202,203,200,213,205,206,207,217,218,219,220,166,204,223,
	211,223,212,210,245,213,181,222,254,218,219,217,253,221,175,180,
	173,177,242,190,182,167,247,184,176,168,250,185,179,178,183,160};

char TabWinDos[]={
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,
49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,
65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,
97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,
113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,
129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,
145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,255,
173,189,156,207,190,221,245,249,184,166,174,170,240,169,238,248,
241,253,252,239,230,244,254,247,251,167,175,172,171,243,168,183,
181,182,199,142,143,146,128,212,144,210,211,222,214,215,216,209,
165,227,224,226,229,153,158,157,235,233,234,154,237,231,225,133,
160,131,198,132,134,145,135,138,130,136,137,141,161,140,139,208,
164,149,162,147,228,148,246,155,151,163,150,129,236,232,152
	};

char TabBBS[]={
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	 16, 17, 18, 19, 80, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 83, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	 79, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 67, 61, 62, 63,
	 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	 96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
	 67, 85, 69, 65, 65, 65, 65, 67, 69, 69, 69, 73, 73, 73, 65, 65,
	 69, 65, 65, 79, 79, 79, 85, 85, 89, 79, 85, 67, 76, 89, 80, 70,
	 65, 73, 79, 85, 78, 78, 65, 79, 67,169,170,171,172, 73,174,175,
	176,177,178,179,180,181,'H',183,184,185,186,187,188,189,190,191,
	'L',193,'T',195,196,197,198,'H',200,201,202,203,204,205,206,207,
	208,209, 78,211,212,213,214,'H',216,217,218,219,220,221,222,223,
	 65, 66,226, 78, 69, 79, 85, 84,232, 79, 78, 68,236, 79, 69, 78,
	240,241,242,243,244,245,246,247, 79, 46, 46, 86, 78, 50,254, 32};

char TabInt[]={
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
	17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,
	46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,
	61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,
	76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,
	91,92,93,94,95,96,97,98,99,100,101,102,103,104,
	105,106,107,108,109,110,111,112,113,114,115,116,
	117,118,119,120,121,122,123,124,125,126,127,128,
	117,233,226,227,224,134,231,234,235,232,105,105,
	105,142,143,144,145,39,111,111,111,117,117,152,
	79,154,155,156,157,158,159,225,105,111,117,164,
	165,166,167,168,169,170,171,172,173,174,175,176,
	177,178,179,180,181,182,183,184,185,186,187,188,
	189,190,191,192,193,194,195,196,197,198,199,200,
	201,202,203,204,205,206,207,208,209,210,211,212,
	213,214,215,216,217,218,219,220,221,222,223,224,
	225,226,227,228,229,230,231,232,233,234,235,236,
	237,238,239,240,241,242,243,244,245,246,247,248,
	249,'.',251,252,253,254,255};


/*--------------------------------------------------------------------*\
|- Variable interne 												  -|
\*--------------------------------------------------------------------*/

KKVIEW *ViewCfg;

/*--------------------------------------------------------------------*\
|- Prototype														  -|
\*--------------------------------------------------------------------*/

void AltK(void);

void DispAnsiPage(int x1,int y1,int x2,int y2);

void ViewSetup(KKFile &Fic);

void (*AnsiAffChr)(long x,long y,long c);
void (*AnsiAffCol)(long x,long y,long c);

void RefreshBar(char *bar);

void ChangeTrad(void);
void ChangeMask(void);
int ChangeLnFeed(void);
void Masque(int x1,int y1,int x2,int y2,char *bufscr,char *bufcol);

int HexaView(KKFile &Fic);
int TxtView(KKFile &Fic);
int AnsiView(KKFile &Fic);
int HtmlView(KKFile &Fic,int &debpos);


int TxtDown(KKFile &Fic,int xl);
int TxtUp(KKFile &Fic,int xl);

void SearchHexa(KKFile &Fic);
void SearchTxt(KKFile &Fic);
void Decrypt(void);
void Print(char *fichier,int n);

void StartAnsiPage(KKFile &Fic);

int ChangeViewer(int i);
static int xarc,debarc;

void BufAffChr(long x,long y,long c);
void BufAffCol(long x,long y,long c);

char PRN_print(int lpt,char a);

void SaveFile(KKFile &Fic,int n);


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void StartWinView(char *name,char *titre)
{
int x,y,l;

SaveScreen();

xarc=(Cfg->TailleX-60)/2;
l=(60-strlen(titre))/2;

Cadre(xarc-1,5,xarc+62,11,0,Cfg->col[55],Cfg->col[56]);
Window(xarc,6,xarc+61,10,Cfg->col[16]);

Cadre(xarc+1,6,xarc+60,8,2,Cfg->col[55],Cfg->col[56]);

PrintAt(xarc+l,5,"%s",titre);

PrintAt(xarc+3,9,"Reading File");
PrintAt(xarc+3,10,"%s",name);

x=xarc+43;
y=9;

PrintAt(x,y,"    STOP    ");
Puce(x,y,13,0);

debarc=0;
}

int InterWinView(int pos,int taille)
{
debarc=LongGradue(xarc+2,7,58,debarc,pos,taille);

if (KbHit())
	if (Wait(0,0)==13) return 1;

return 0;
}

void CloseWinView(void)
{
LoadScreen();
}



/*--------------------------------------------------------------------*\
|- Change le mode vid‚o 											  -|
\*--------------------------------------------------------------------*/
void ChangeSize(void)
{
SetMode();
LoadPal();

#ifndef NOFONT
if (KKCfg->userfont)
	InitFontFile(KKFics->trash);
	else
	InitFont();
#endif
}

uchar ROR(uchar,uchar);
#pragma aux ROR = \
	  "ror al,cl" \
	  parm [al] [cl] \
	  value [al];

#ifdef LINUX
uchar ROR(uchar a,uchar b)
{
return(a+b);
}
#endif



/*--------------------------------------------------------------------*\
|- ALT-K															  -|
\*--------------------------------------------------------------------*/
void AltK(void)
{
int res;
char fichier[256];
FILE *fic;
int car;
int x[2],y[2],z;
int i,j;
char col;
char *buffer;
int lngbuf;

strcpy(fichier,ViewCfg->viewhist);
Path2Abs(fichier,"..\\clpboard.tmp");

x[0]=0;
y[0]=0;
z=0;

while(z!=2)
{
if (z==0)
	x[1]=x[0],y[1]=y[0];

SaveScreen();
col=GetCol(x[z],y[z]);
ColWin(x[0],y[0],x[1],y[1],(col&15)*16+(col/16));
car=Wait(x[z],y[z]);
LoadScreen();

switch(car)
	{
	case KEY_DOWN: //--- DOWN ------------------------------------------
		if (y[z]<Cfg->TailleY)
			y[z]++;
		break;
	case KEY_UP: //--- UP ----------------------------------------------
		if (y[z]>0)
			y[z]--;
		break;
	case KEY_RIGHT: //--- RIGHT ----------------------------------------
		if (x[z]<Cfg->TailleX)
			x[z]++;
		break;
	case KEY_LEFT: //--- LEFT ------------------------------------------
		if (x[z]>0)
			x[z]--;
		break;
	case KEY_END: //--- END --------------------------------------------
		x[z]=Cfg->TailleX-1;
		break;
	case KEY_HOME: //--- HOME ------------------------------------------
		x[z]=0;
		break;
	case KEY_PPAGE: //--- PGUP -----------------------------------------
		y[z]=0;
		break;
	case KEY_NPAGE: //--- PGDN -----------------------------------------
		y[z]=Cfg->TailleY-1;
		break;

	case KEY_ENTER:
		z++;
		if (z==1)
			x[1]=x[0],y[1]=y[0];
		break;
	case 27:
		return;
	}
}

MacAlloc(10,80);

strcpy(_sbuf[10],fichier);

if (ExistClipboard())
	res=MWinTraite(altk1_kkt);
	else
	res=MWinTraite(altk0_kkt);

buffer=(char*)GetMem((Cfg->TailleX+2)*Cfg->TailleY);
lngbuf=0;

for(j=y[0];j<=y[1];j++)
	{
	for(i=x[0];i<=x[1];i++)
		buffer[lngbuf]=GetChr(i,j),lngbuf++;

	buffer[lngbuf]=0x0D,lngbuf++;
	buffer[lngbuf]=0x0A,lngbuf++;
	}

switch(res)
	{
	case 1:
		fic=fopen(_sbuf[10],"wb");
		fwrite(buffer,1,lngbuf,fic);
		fclose(fic);
		break;

	case 2:
		fic=fopen(_sbuf[10],"ab");
		fwrite(buffer,1,lngbuf,fic);
		fclose(fic);
		break;

	case 3:
		PutInClipboard(buffer,lngbuf);
		break;
	}

LibMem(buffer);

MacFree(10);

fclose(fic);
}




/*--------------------------------------------------------------------*\
| Pcboard															   |
\*--------------------------------------------------------------------*/
void ReadPCB(KKFile &Fic)
{
char tag[32];
int ltag;
int x,y;
char car,chr,col;

col=7;
x=0;
y=0;

ltag=0; //--- Longueur du tag

for ((Fic.posn)=0;(Fic.posn)<(Fic.taille);(Fic.posn)++)
	{
	car=Fic.ReadChar();
	if (car==0x1A) break;

	chr=0;

	if (ltag==0)
		switch(car)
			{
			case 10:
				x=0;
				break;
			case 13:
				y++;
				break;
			case '@':
				tag[0]='@';
				ltag=1;
				break;
			default:
				chr=car;
				break;
			}
		else
		{
		tag[ltag]=car;

		ltag++;
		if (ltag==32) ltag=0;

		tag[ltag]=0;

		if (car=='@')
			{
			int n;
			tag[ltag-1]=0;
			if (!strnicmp(tag+1,"POS:",4))
				{
				sscanf(tag+5,"%d",&n);
				x=n;
				}
			ltag=0;
			}

		if ((tag[1]=='X') & (ltag==4))  //--- Code couleur -------------
			{
			int n;
			sscanf(tag+2,"%02X",&n);
			col=(char)n;
			ltag=0;
			}


		}

	if (chr!=0)
		{
		BufAffChr(x,y,chr);
		BufAffCol(x,y,col);
		x++;
		}
	}
}

void ReadRAW(KKFile &Fic)
{
int x,y;
(Fic.posn)=0;
for(y=0;y<Cfg->TailleY;y++)
	for(x=0;x<Cfg->TailleX;x++)
		{
		if ((Fic.posn)<(Fic.taille))
			{
			AnsiAffChr(x,y,Fic.ReadChar());
			(Fic.posn)++;
			AnsiAffCol(x,y,Fic.ReadChar());
			(Fic.posn)++;
			}
		}
}

void ReadANS(KKFile &Fic)
{
char car;

for ((Fic.posn)=0;(Fic.posn)<(Fic.taille);(Fic.posn)++)
	{
	car=Fic.ReadChar();
	if (car==0x1A) break;
	ansi_out(car);
	}
}

/*--------------------------------------------------------------------*\
|- Sauvegarde du fichiers											  -|
\*--------------------------------------------------------------------*/
void SaveFile(KKFile &Fic,int n)
{
FILE *outfic;
char *buf;
int pos,x,y,z;
int res;

buf=(char*)GetMem(32768);


switch(n)
	{
	case 0:
		MacAlloc(10,256);
		Fic.getname(_sbuf[10]);

		res=MWinTraite(viewsave_kkt);

		if (res==1)
			{
			pos=(Fic.posn);
			outfic=fopen(_sbuf[10],"wb");
			(Fic.posn)=0;
			x=Fic.taille;		//--- Reste … copier
			while(x>0)
				{
				if (x<32768)
					z=x;
				else
					z=32768;

				for(y=0;y<z;y++)
					{
					buf[y]=Fic.ReadChar();
					(Fic.posn)++;
					}

				x-=z;
				fwrite(buf,1,z,outfic);
				}
			fclose(outfic);
			(Fic.posn)=pos;
			}
		MacFree(10);
		break;
	}

LibMem(buf);

}


/*--------------------------------------------------------------------*\
|- Affichage en Hexadecimal 										  -|
\*--------------------------------------------------------------------*/

int HexaView(KKFile &Fic)
{
int x,y;
long cur1,cur2;
long posd;
int car;
int x1;
int cv;

#ifndef NOMOUSE
int p;	// position du curseur dans la grille --------------------------
int n;
#endif

int fin=0; //--- Code de retour ----------------------------------------

char fichier[256];

Fic.getname(fichier);

x1=(Cfg->TailleX-80)/2;

SaveScreen();
PutCur(3,0);

Bar(" Help  Save  ---- ChView ---- PreproSearch ----  ----  Quit ");

Window	(x1+ 1,1,x1+78,(Cfg->TailleY)-3,Cfg->col[16]);
WinCadre(x1+ 0,3,x1+ 9,(Cfg->TailleY)-2,2);
WinCadre(x1+10,3,x1+58,(Cfg->TailleY)-2,2);
WinCadre(x1+59,3,x1+76,(Cfg->TailleY)-2,2);
WinCadre(x1+77,3,x1+79,(Cfg->TailleY)-2,2);
WinCadre(x1+ 0,0,x1+79,2,3);

if (x1!=0)
	{
	Window(0,0,x1-1,(Cfg->TailleY)-2,Cfg->col[16]);
	Window(80+x1,0,(Cfg->TailleX)-1,(Cfg->TailleY)-2,Cfg->col[16]);
	}

ChrCol(x1+34,4,(Cfg->TailleY)-6,179);		// Cfg->TFont

PrintAt(x1+3,1,"View File %s",fichier);

if (ViewCfg->saveviewpos)
	(Fic.posn)=Fic.LoadPosition(ViewCfg->viewhist);

do
{
posd=(Fic.posn);

for (y=0;y<Cfg->TailleY-6;y++)
	{
	PrintAt(x1+1,y+4,"%08X",(Fic.posn));

	for (x=0;x<16;x++)
		{
		if ((Fic.posn)<(Fic.taille))
			{
			char a;
			a=Fic.ReadChar();
			PrintAt(x1+x*3+11,y+4,"%02X",a);
			AffChr(x1+x+60,y+4,a);
			(Fic.posn)++;
			}
			else
			{
			PrintAt(x1+x*3+11,y+4,"  ");
			AffChr(x1+x+60,y+4,32);
			}
		}
	}

if (Fic.taille<1024*1024)
	{
	cur1=(posd)*(Cfg->TailleY-7);
	cur1=cur1/(Fic.taille)+4;

	cur2=((Fic.posn))*(Cfg->TailleY-7);
	cur2=cur2/(Fic.taille)+4;
	}
	else
	{
	cur1=(posd/1024)*(Cfg->TailleY-7);
	cur1=cur1/((Fic.taille)/1024)+4;

	cur2=((Fic.posn)/1024)*(Cfg->TailleY-7);
	cur2=cur2/((Fic.taille)/1024)+4;
	}

ChrCol(x1+78,4,cur1-4,32);
ChrCol(x1+78,cur1,cur2-cur1+1,219);
ChrCol(x1+78,cur2+1,(Cfg->TailleY-3)-cur2,32);

(Fic.posn)=posd;

car=0;

while ((!KbHit()) & (car==0))
	{
#ifndef NOMOUSE
	int x,y,x2,y2,button,button2;

	GetPosMouse(&x,&y,&button);

	if (iskeypressed(SHIFT))
		{
		if ( (y>=4) & (y<Cfg->TailleY-2) )
			{
			p=(x-x1-11)/3;
			if ( (((p*3)==x-x1-11) | ((p*3)==x-x1-12)) & (p<16) )
				p=p+(y-4)*16;
				else
				{
				p=(x-x1-60);
				if ( (p>=0) & (p<16) )
					p=p+(y-4)*16;
					else
					p=-1;
				}
			}
			else
			p=-1;

		if (p!=-1)
			{
			ColLin((p&15)*3+x1+11,(p/16)+4,2,Cfg->col[18]);
			AffCol((p&15)+x1+60,(p/16)+4,Cfg->col[18]);
			}

		if ((button&3)!=0)
			do
				GetPosMouse(&x2,&y2,&button2);
			while((button2&3)!=0);

		if ( ((button&1)==1) & (p!=-1) ) //--- gauche ------------------
			{
			(Fic.fromoct)+=(p+(Fic.posn));
			if ((Fic.nbroct)!=-1)
				(Fic.nbroct)-=(p+(Fic.posn));

			fin=VIEWER_BINARY; //--- reload
			car=1;
			}

		if ( ((button&2)==2) & (p!=-1) ) //--- droite ------------------
			{
			(Fic.nbroct)=(p+(Fic.posn));

			fin=VIEWER_BINARY; //--- reload
			car=1;
			}
		}
		else
		{
		p=-1;

		if ((button&1)==1) //--- gauche --------------------------------
			{
			if ((x==78+x1) & (y>=4) & (y<=Cfg->TailleY-3)) //- Ascens.--
				{
				(Fic.posn)=((Fic.taille)*(y-4))/(Cfg->TailleY-7);
				car=1;
				}
				else
				if (y==Cfg->TailleY-1)
					if (Cfg->TailleX==90)
						car=(0x3B+(x/9))*256;
						else
						car=(0x3B+(x/8))*256;
					else
					if (y>(Cfg->TailleY)/2)
						car=80*256;
						else
						car=72*256;
			}

		if ((button&2)==2) //--- droite --------------------------------
			{
			car=27;
			}
		}

	for(n=0;n<16*(Cfg->TailleY-6);n++)
		{
		if (n!=p)
			{
			ColLin((n&15)*3+x1+11,(n/16)+4,2,Cfg->col[16]);
			AffCol((n&15)+x1+60,(n/16)+4,Cfg->col[16]);
			}
		}
#endif
	}

if (KbHit())
	car=Wait(0,0);

switch(car)
	{
	case 0x2500:  // --- ALT-K -----------------------------------------
		AltK();
		break;
	case KEY_DOWN:
		(Fic.posn)+=16;
		break;
	case KEY_UP:
		(Fic.posn)-=16;
		break;
	case KEY_NPAGE:
		(Fic.posn)+=(Cfg->TailleY-6)*16;
		break;
	case KEY_PPAGE:
		(Fic.posn)-=(Cfg->TailleY-6)*16;
		break;
	case KEY_HOME:
		(Fic.posn)=0;
		break;
	case KEY_END:
		(Fic.posn)=(Fic.taille)-((((Cfg->TailleY)-6)*16));
		break;
	case KEY_F(6):
		ChgViewPreproc(Fic);
		break;
	case KEY_F(7):
		SearchHexa(Fic);
		break;
	case KEY_F(10):
	case KEY_C_UP:
		fin=-1;
		break;
	case KEY_F(1):
		HelpTopic(8);
		fin=VIEWER_BINARY;
		break;
	case KEY_F(4):
		cv=ChangeViewer(1);
		if (cv!=0)
			fin=cv;
		break;
	case KEY_A_F11:
		fin=VIEWER_AUTO;
		break;
	case KEY_A_F12:
		ViewSetup(Fic);
		fin=VIEWER_BINARY;
		break;
	case KEY_F(2):
		SaveFile(Fic,0);
		break;
	case 32:
	case 13:
	case 27:
		fin=-1;
		break;
	default:
		break;
	}

while ((Fic.posn)>=(Fic.taille)-((((Cfg->TailleY)-7)*16))) (Fic.posn)-=16;
while ((Fic.posn)<0) (Fic.posn)+=16;
}
while(fin==0);

if (ViewCfg->saveviewpos)
	Fic.SavePosition(ViewCfg->viewhist,Fic.posn);

if (fin==VIEWER_AUTO)
	{
	char fic[256];

	GetListFile(fic);
	if (fic[0]!=0)
		Fic.setname(fic);
	}

LoadScreen();

return fin;
}

/*--------------------------------------------------------------------*\
|- to print through ansi interpreter:								  -|
|-	 call ansi_out(<string>);										  -|
\*--------------------------------------------------------------------*/

static char curattr = 7;
static int curx = 0,cury = 0;
static int maxx = 80, maxy = 25;		   // size of ansi output window
static int tabspaces = 8;
static int savx,savy,issaved = 0;
static char ansi_terminators[] = "HFABCDnsuJKmp";

#define MAXARGLEN		128

void set_pos(char *argbuf,int arglen)
{
int y,x;
char *p;

if (!*argbuf || !arglen)
	{
	curx=0;
	cury=0;
	}

y=atoi(argbuf) - 1;
p=strchr(argbuf,';');

if ((y>=0) & (p!=NULL))
	{
	x = atoi(p+1) - 1;
	if (x>=0)
		{
		curx=x;
		cury=y;
		}
	}
}

void go_up (char *argbuf)
{
int x;

x = atoi(argbuf);

do
	{
	if (cury==0)
		break;
	cury--;
	x--;
	}
while(x>0);
}

void go_down (char *argbuf)
{
int x;

x = atoi(argbuf);

do
	{
//	  if (cury == maxy - 1) 	   break;
	cury++;
	x--;
	}
while(x>0);
}

void go_left (char *argbuf)
{
int x;

x = atoi(argbuf);

do
	{
	if(curx==0)
		break;
	curx--;
	x--;
	}
while(x>0);
}

void go_right (char *argbuf)
{
int x;

x = atoi(argbuf);

do
	{
	if (curx == maxx - 1)
		break;
	curx++;
	x--;
	}
while(x>0);
}

void save_pos (void)
{
savx = curx;
savy = cury;
issaved = 1;
}

void restore_pos (void)
{
if(issaved)
	{
	curx = savx;
	cury = savy;
	issaved = 0;
	}
}


void set_colors (char *argbuf,int arglen)
{
char *p,*pp;

if (*argbuf && arglen)
	{
	pp = argbuf;
	do
		{
		p = strchr(pp,';');
		if (p && *p)
			{
			*p = 0;
			p++;
			}
		switch (atoi(pp))
			{
			case 0: //--- all attributes off ---------------------------
				curattr = 7;
				break;

			case 1: //--- Bold On --------------------------------------
				  curattr |= 8;
				  break;

			case 2: //--- Dim On ---------------------------------------
				  curattr &= (~8);
				  break;

			case 3: //--- Italic On ------------------------------------
				  break;

			case 4: //--- Underline On ---------------------------------
				  break;

			case 5: //--- Blink On -------------------------------------
				  curattr |= 128;
				  break;

			case 6: //--- Rapid Blink On -------------------------------
				  break;

			case 7: //--- Reverse Video On -----------------------------
				  curattr = 112;
				  break;

			case 8: //--- Invisible On ---------------------------------
				  curattr = 0;
				  break;

			case 30: //--- black fg ------------------------------------
				  curattr &= (~7);
				  break;

			case 31: //--- red fg --------------------------------------
				  curattr &= (~7);
				  curattr |= 4;
				  break;

			case 32: //--- green fg ------------------------------------
				  curattr &= (~7);
				  curattr |= 2;
				  break;

			case 33: //--- yellow fg -----------------------------------
				  curattr &= (~7);
				  curattr |= 6;
				  break;

			case 34: //--- blue fg -------------------------------------
				  curattr &= (~7);
				  curattr |= 1;
				  break;

			case 35: //--- magenta fg ----------------------------------
				  curattr &= (~7);
				  curattr |= 5;
				  break;

			case 36: //--- cyan fg -------------------------------------
				  curattr &= (~7);
				  curattr |= 3;
				  break;

			case 37: //--- white fg ------------------------------------
				  curattr |= 7;
				  break;

			case 40: //--- black bg ------------------------------------
				  curattr &= (~112);
				  break;

			case 41: //--- red bg --------------------------------------
				  curattr &= (~112);
				  curattr |= (4 << 4);
				  break;

			case 42: //--- green bg ------------------------------------
				  curattr &= (~112);
				  curattr |= (2 << 4);
				  break;

			case 43: //--- yellow bg -----------------------------------
				  curattr &= (~112);
				  curattr |= (6 << 4);
				  break;

			case 44: //--- blue bg -------------------------------------
				  curattr &= (~112);
				  curattr |= (1 << 4);
				  break;

			case 45: //--- magenta bg ----------------------------------
				  curattr &= (~112);
				  curattr |= (5 << 4);
				  break;

			case 46: //--- cyan bg -------------------------------------
				  curattr &= (~112);
				  curattr |= (3 << 4);
				  break;

			case 47: //--- white bg ------------------------------------
				  curattr |= 112;
				  break;

			case 48: //--- subscript bg --------------------------------
				  break;

			case 49: //--- superscript bg ------------------------------
				  break;

			default: //--- unsupported ---------------------------------
				  break;
			}
		pp = p;
		}
	while (p);
	}
}



int ansi_out (char b)
{
static int	arglen=0, ansistate=0, x;
static char argbuf[MAXARGLEN] = "";

switch (ansistate)
	{
	case 0:
		switch (b)
			{
			case 27:
				ansistate = 1;
				break;
/*			  case '\r':
				curx = 0;
				break;

			case '\n':
				cury++;
				break;
*/

			case '\n':
				curx = 0;
				cury++;
				break;

			case '\r':
				break;

			case '\t':
				for (x = 0; x < tabspaces; x++)
					{
					AnsiAffChr(curx,cury,' ');
					AnsiAffCol(curx,cury,curattr);
					curx++;
					if (curx > maxx - 1)
						{
						curx = 0;
						cury++;
						}
					}
				break;

			case '\b':
				if (curx)
					curx--;
				break;

			case '\07':                  // The beep -------------------
				// putchar('\07');
				break;

			default:
				AnsiAffChr(curx,cury,b);
				AnsiAffCol(curx,cury,curattr);
				curx++;
				if (curx > maxx - 1)
					{
					curx = 0;
					cury++;
					}
				break;
			}
		break;

	case 1:
		if (b == '[')
			{
			ansistate = 2;
			arglen = 0;
			*argbuf = 0;
			break;
			}
		ansistate = 0;
		break;

	case 2:
		if (strchr(ansi_terminators, (int)b))
			{
			switch ((int)b)
				{
				case 'H': //--- set cursor position --------------------
				case 'F':
					set_pos(argbuf,arglen);
					break;

				case 'A': //--- up -------------------------------------
					go_up(argbuf);
					break;

				case 'B': //--- down -----------------------------------
					go_down(argbuf);
					break;

				case 'C': //--- right ----------------------------------
					go_right(argbuf);
					break;

				case 'D': //--- left -----------------------------------
					go_left(argbuf);
					break;

				case 'n': //--- device statusreport pos ----------------
					break;

				case 's': //--- save pos -------------------------------
					save_pos();
					break;

				case 'u': //--- restore pos ----------------------------
					restore_pos();
					break;

				case 'J': //--- clear screen ---------------------------
				   // ChrWin(0,0,maxx - 1,maxy - 1,32);
				   // ColWin(0,0,maxx - 1,maxy - 1,curattr);
					curx = cury = 0;
					break;

				case 'K': //--- delete to eol --------------------------
				   // ChrWin(curx,cury,maxx-1,cury,32);
				   // ColWin(curx,cury,maxx-1,cury,curattr);
					break;

				case 'm': //--- set video attribs ----------------------
					set_colors(argbuf,arglen);
					break;

				case 'p': //--- keyboard redef -------------------------
					break;

				default:  //--- unsupported ----------------------------
					break;
				}
			ansistate = 0;
			arglen = 0;
			*argbuf = 0;
			}
		else
			{
			if (arglen < MAXARGLEN)
				{
				argbuf[arglen] = b;
				argbuf[arglen + 1] = 0;
				arglen++;
				}
			}
		break;

	default:
		WinError("Error of ANSI code");
		break;
	}

return cury;
}



void RefreshBar(char *bar)
{
switch(ViewCfg->warp)	   //--- F2 --------------------------------------
	{
	case 0: memcpy(bar+6,"Nowrap",6); break;
	case 1: memcpy(bar+6," Wrap ",6); break;
	case 2: memcpy(bar+6,"WoWrap",6); break;
	}

}

/*void AutoTrad(KKFile &fic)
{
int i,j;
char car;

int maxtrad,numtrad,trad;

maxtrad=0;
numtrad=0;

for (i=0;i<3;i++)
	{
	trad=0;
	for (j=0;j<(((Fic.taille)<32768) ? (Fic.taille):32768);j++)
		{
		(Fic.posn)=j;
		car=Fic.ReadChar();
		if (car>0xB0)
		if (CnvASCII((char)i,car)<0xB0)
			trad++;
		}
	if (trad>maxtrad) maxtrad=trad,numtrad=i;
	}
(Fic.posn)=0;
ViewCfg->cnvtable=(char)numtrad;
}*/


/*--------------------------------------------------------------------*\
|- Deplace le texte d'une ligne vers le bas                           -|
|- Renvoit 0 si il faut arreter 									  -|
\*--------------------------------------------------------------------*/
int TxtDown(KKFile &Fic,int xl)
{
int car,m;

m=0;
do
	{
	car=Fic.ReadChar();
	(Fic.posn)++;

	if ( ((car==10) & (ViewCfg->lnfeed==0)) |
		 ((car==13) & (ViewCfg->lnfeed==1)) |
		 ((car==10) & (ViewCfg->lnfeed==2)) |
		 ((car==10) & (ViewCfg->lnfeed==4)) |
		 ((car==ViewCfg->userfeed) & (ViewCfg->lnfeed==3)))
		{
		return 1;
		}
//
	if ((Fic.posn)==(Fic.taille))
		{
		(Fic.posn)=(Fic.taille)-1;
		return 0;
		}
//
	if ((car!=10) & (car!=13)) m++;
	if ((m>xl) & (ViewCfg->warp!=0))
		{
		(Fic.posn)--;
		return 1;
		}
	}
while(1);
}


/*--------------------------------------------------------------------*\
|- Deplace le texte d'une ligne vers le haut                          -|
|- Renvoit 0 si il faut arreter 									  -|
\*--------------------------------------------------------------------*/
int TxtUp(KKFile &Fic,int xl)
{
int car,m;

if ((Fic.posn)==0) return 0;

m=0;
(Fic.posn)--;
car=Fic.ReadChar();
if ((car!=10) & (car!=13)) m++;

if ((Fic.posn)==0) return 0;
do
	{
	(Fic.posn)--;
	if ((Fic.posn)==0)
		{
		if ((Fic.posn)!=0) (Fic.posn)++;
		return 0;
		}
	car=Fic.ReadChar();
	if ( ((car==10) & (ViewCfg->lnfeed==0)) |
		 ((car==13) & (ViewCfg->lnfeed==1)) |
		 ((car==10) & (ViewCfg->lnfeed==2)) |
		 ((car==10) & (ViewCfg->lnfeed==4)) |
		 ((car==ViewCfg->userfeed) & (ViewCfg->lnfeed==3)))
		{
		if ((Fic.posn)!=0) (Fic.posn)++;
		return 1;
		}
	if ((car!=10) & (car!=13)) m++;
	if ((m>=xl) & (ViewCfg->warp!=0)) return 1;
	}		 // m>xl-1
while(1);

}

/*--------------------------------------------------------------------*\
|- Affichage de texte												  -|
\*--------------------------------------------------------------------*/
int TxtView(KKFile &Fic)
{
#ifndef NOMOUSE
int xm,ym;
#endif

int zm;

int aff,wrap;
int cv;

long posd;

#ifndef NOMOUSE
long cur1,cur2;
#endif

int xl,yl;
int x,y;
int x2,y2;
int lbufchr;

int y3;

char highchr;

char bufchr[256];
char bufcol[256];

char autowarp=0; //--- met a 1 si il faut absolument faire un nowarp ---

int m;

char linechr[101];
char linecol[101];

int w1,w2;

int code;
int fin=0;

int warp=0;

int shift=0; //--- Vaut -1 si reaffichage de l'ecran -------------------
			 //---		 0 si pas de shift -----------------------------
			 //---		 1 si touche shiftee ---------------------------

char pasfini;

char *bufscr;
char *bufscrcol;
char bar[81];

int xl2;
int tpos;

char fichier[256];
Fic.getname(fichier);

bufscr=(char*)GetMem((Cfg->TailleX)*(Cfg->TailleY));
bufscrcol=(char*)GetMem((Cfg->TailleX)*(Cfg->TailleY));

SaveScreen();
PutCur(3,0);

wrap=0;
aff=1;

/*--------------------------------------------------------------------*\
|-			   Recherche de la meilleur traduction					  -|
\*--------------------------------------------------------------------*/
//if (ViewCfg->autotrad)	AutoTrad();



/*--------------------------------------------------------------------*\
|-			   Calcul de la taille maximum							  -|
\*--------------------------------------------------------------------*/
if (ViewCfg->ajustview)
	{
	int xm=0,ym=0;
	int n;

	char vb,vb2;
	long posold;

	x=0;
	posold=(Fic.posn);

	vb2=Fic.ReadChar();

	for (n=0;n<(((Fic.taille)<16000) ? (Fic.taille):16000);n++)
		{
		vb=vb2;

		(Fic.posn)=n+1;
		vb2=Fic.ReadChar();

		if ( ((vb==13) & (vb2==10) & (ViewCfg->lnfeed==0)) |
			 ((vb==13) & (ViewCfg->lnfeed==1)) |
			 ((vb==10) & (ViewCfg->lnfeed==2)) |
			 ((vb==10) & (ViewCfg->lnfeed==4)) |
			 ((vb==ViewCfg->userfeed) & (ViewCfg->lnfeed==3)) )
			 {
			 x=0;
			 ym++;
			 if (ViewCfg->lnfeed==0) n++;
			 }
		else
		switch(vb)
			{
			case 9:
				lbufchr=x/8;
				lbufchr=lbufchr*8+8;
				lbufchr-=x;
				x+=lbufchr;
				break;
			case 32:
				x++;
				break;
			case 13:
				if (ViewCfg->lnfeed==4) break;
			default:
				x++;
				if (x>xm) xm=x;
				break;
			}
		if (x>Cfg->TailleX) ym++,x=0;
		}

	(Fic.posn)=posold;
	ym++;

	if (xm>=((Cfg->TailleX)-1))
		xl=Cfg->TailleX;
		else
		autowarp=1, xl=xm;						  //--> Longueur fenˆtre

	if (ym>Cfg->TailleY-1)
		yl=Cfg->TailleY-1;
		else
		yl=ym;
	}
	else
	{
	xl=Cfg->TailleX;
	yl=Cfg->TailleY-1;
	}


/*--------------------------------------------------------------------*\
|- Affichage de la fenetre											  -|
\*--------------------------------------------------------------------*/

x=(Cfg->TailleX-xl)/2;								  // centre le texte
y=(Cfg->TailleY-yl)/2;

if (x<0) x=0;
if (y<0) y=0;

if ( (x>0) & (y>0) & (x+xl<Cfg->TailleX) & (y+yl<Cfg->TailleY) )
	Cadre(x-1,y-1,x+xl,y+yl,3,Cfg->col[55],Cfg->col[56]);
	else
	{
	if ( (y+yl<Cfg->TailleY) & (y>0) )
		{
		ColLin(0,y-1,Cfg->TailleX,Cfg->col[16]);
		WinLine(0,y-1,Cfg->TailleX,1);
		WinLine(0,y+yl,Cfg->TailleX,1);
		ColLin(0,y+yl,Cfg->TailleX,Cfg->col[16]);
		}
	if ( (x+xl<Cfg->TailleX) & (x>0) )
		{
		ColWin(x-1,0,x-1,Cfg->TailleY-2,Cfg->col[16]);
		ChrWin(x-1,0,x-1,Cfg->TailleY-2,0xB3);
		ColWin(x+xl,0,x+xl,Cfg->TailleY-2,Cfg->col[16]);
		ChrWin(x+xl,0,x+xl,Cfg->TailleY-2,0xB3);
		}
	}

ColWin(x,y,x+xl-1,y+yl-1,Cfg->col[16]);
ChrWin(x,y,x+xl-1,y+yl-1,32);

/*--------------------------------------------------------------------*\
|- Affichage de la bar												  -|
\*--------------------------------------------------------------------*/

strcpy
   (bar," Help NowrapLnFeedChView Zoom PreproSearch Print Mask  Quit ");

RefreshBar(bar);

Bar(bar);

/*--------------------------------------------------------------------*\
|- Garde la position pr‚c‚dente 									  -|
\*--------------------------------------------------------------------*/

if (ViewCfg->saveviewpos==1)
	(Fic.posn)=Fic.LoadPosition(ViewCfg->viewhist);

/*--------------------------------------------------------------------*/

highchr=0;

linechr[xl]=0;
linecol[xl]=0;

do
{
//memset(bufscrcol,0,(Cfg->TailleX)*(Cfg->TailleY));

pasfini=0;

posd=(Fic.posn);

x2=x;
y2=y;

aff=0;

if (xl==Cfg->TailleX)		//--- Jusqu'ou on ecrit --------------------
	xl2=xl-shift;
	else
	xl2=xl;

/*--------------------------------------------------------------------*\
|- Affichage du texte												  -|
\*--------------------------------------------------------------------*/
do
	{
	char chr2;

	tpos=x2-x-warp;

	lbufchr=1;										  // Longueur bufchr

	bufcol[0]=0;
	bufchr[0]=Fic.ReadChar();

	chr2=Fic.ReadNextChar();

	if ( ((bufchr[0]==13) & (chr2==10) & (ViewCfg->lnfeed==0)) |
		 ((bufchr[0]==13) & (ViewCfg->lnfeed==1)) |
		 ((bufchr[0]==10) & (ViewCfg->lnfeed==2)) |
		 ((bufchr[0]==10) & (ViewCfg->lnfeed==4)) |
		 ((bufchr[0]==ViewCfg->userfeed) & (ViewCfg->lnfeed==3)) )
		{
		if (ViewCfg->lnfeed==0) (Fic.posn)++;
		w1=tpos;
		aff=1;
		}
	else
		{
		switch(bufchr[0])
			{
			case 9:
				lbufchr=(x2-x)/8;
				lbufchr=lbufchr*8+8;
				lbufchr-=(x2-x);
				if (lbufchr>0)
					{
					memset(bufchr,32,lbufchr);
					memset(bufcol,0,lbufchr);
					}
				break;
			case 13:
				if (ViewCfg->lnfeed==4)
					{
					lbufchr=0;
					bufchr[0]=0;
					bufcol[0]=0;
					}
				break;
			default:
				break;
			}
		}

	for(m=0;m<lbufchr;m++)
		{
		char car;

		car=bufchr[m];

		if ( (tpos<xl+10) & (tpos>=0) )
			{
			if (car==8)
				{
				if ( (tpos>0) & (x2>0) )
					{
					highchr=linechr[tpos-1];

					tpos-=2;
					x2-=2;
					}
				}
			else
				{
				if (highchr!=0)
					{
					if (car==highchr)
						{
						linecol[tpos]=(char)(((Cfg->col[16])&240)+((Cfg->col[60])&15));
						linechr[tpos]=highchr;
						}

					if (car=='_')
						{
						linecol[tpos]=(char)(((Cfg->col[16])&240)+((Cfg->col[62])&15));
						linechr[tpos]=highchr;
						}

					if (highchr=='_')
						{
						linecol[tpos]=(char)(((Cfg->col[16])&240)+((Cfg->col[62])&15));
						linechr[tpos]=car;
						}

					highchr=0;
					}
				else
					{
					linecol[tpos]=0;
					linechr[tpos]=car;
					}
				}
			}

		tpos++;
		x2++;
		}

	tpos=x2-x-warp-1;

	if ( (tpos>=xl2) & (aff==0) )	// Si le prochain d‚passe la fenˆtre
		{
		if (tpos>=xl2)
			{
			if ( (ViewCfg->warp==1) & (autowarp==0) )
				{
				aff=2;

				w1=xl2; 							// Premier … retenir
				w2=tpos;							// Dernier … retenir
				}

			if ( (ViewCfg->warp==2) & (autowarp==0) )
				{
				int n;

				aff=2;
				n=xl2;
				while ( (n>0) & (linechr[n]!=32) ) n--;

				if (n==0) n=xl2-1;	  // On coupe pas trop !

				w1=n+1;
				w2=tpos;
				}
			}
		}

	if (aff!=0) 		   // Le monsieur te demande d'afficher la ligne
		{
		if ((shift!=1) | (y2!=0))
			{
			y3=y2*(Cfg->TailleX);

			if (w1<0)
				{
				memset(bufscr+x+y3,32,xl2);
				memset(bufscrcol+x+y3,0,xl2);
				}
			else if (w1>=xl2)
				{
				memcpy(bufscr+x+y3,linechr,xl2);
				memcpy(bufscrcol+x+y3,linecol,xl2);
				}
			else
				{
				memcpy(bufscr+x+y3,linechr,w1);
				memset(bufscr+x+y3+w1,32,xl2-w1);

				memcpy(bufscrcol+x+y3,linecol,w1);
				memset(bufscrcol+x+y3+w1,0,xl2-w1);
				}
			}

		x2=x;
		y2++;

		if (aff==2)
			{
			memcpy(linechr,linechr+w1,w2-w1+1);
			memcpy(linecol,linecol+w1,w2-w1+1);

			x2+=(w2-w1+1);
			}

		aff=0;
		}

	if (y2>=y+yl) break;

	(Fic.posn)++;

	if ((Fic.posn)>=(Fic.taille))
		{
		int n=0;

		pasfini=1;
		lbufchr=xl-x2+x;

		if (lbufchr>0)
			{
			if (yl==Cfg->TailleY-1)
				memset(linechr+x2-x,'-',lbufchr);
				else
				memset(linechr+x2-x,' ',lbufchr);
			memset(linecol+x2-x,0,lbufchr);
			}

		linechr[xl]=0;
		linecol[xl]=0;

		y3=y2*(Cfg->TailleX);

		for(n=0;n<strlen(linechr);n++)
			{
			bufscr[x+n+y3]=linechr[n];
			bufscrcol[x+n+y3]=linecol[n];
			}

		y2++;
		break;
		}
	}
while(1);


while(y2<y+yl)
	{
	int n=0;

	memset(linechr,' ',xl);     // remplit une ligne entiere de espace
	memset(linecol,0,xl);

	linechr[xl]=0;
	linecol[xl]=0;

	y3=y2*(Cfg->TailleX);
	for(n=0;n<strlen(linechr);n++)
		{
		bufscr[x+n+y3]=linechr[n];
		bufscrcol[x+n+y3]=linecol[n];
		}
	y2++;
	}

if (shift==0)
	Masque(x,y,x+xl-1,y+yl-1,bufscr,bufscrcol);
	else
	Masque(x,y+(y==0),x+xl-1-(xl==Cfg->TailleX),y+yl-1,bufscr,bufscrcol);


/*--------------------------------------------------------------------*\
|- Gestion des touches												  -|
\*--------------------------------------------------------------------*/

zm=0;

while ( (!KbHit()) & (zm==0) )
{
#ifndef NOMOUSE
GetPosMouse(&xm,&ym,&zm);


if (iskeypressed(SHIFT))
	{
	int prc;
	char temp[132];

	if (shift==0)
		SaveScreen();

	if ((Fic.posn)>(Fic.taille))
		(Fic.posn)=(Fic.taille);

	if ((Fic.taille)<1024*1024)
		{
		cur1=(posd)*(Cfg->TailleY-2);
		cur1=cur1/(Fic.taille)+1;

		cur2=((Fic.posn)-1)*(Cfg->TailleY-2);
		cur2=cur2/(Fic.taille)+1;

		prc=((Fic.posn)*100)/(Fic.taille);
		}
		else
		{
		cur1=(posd/1024)*(Cfg->TailleY-2);
		cur1=cur1/((Fic.taille)/1024)+1;

		cur2=(((Fic.posn)-1)/1024)*(Cfg->TailleY-2);
		cur2=cur2/((Fic.taille)/1024)+1;

		prc=((Fic.posn)/(Fic.taille))*100;
		}

	ColLin(0,0,Cfg->TailleX,Cfg->col[7]);

	strncpy(temp,fichier,78);

	temp[45]=0;

	PrintAt(0,0,"View: %-*s Col%3d %9d bytes %3d%% ",Cfg->TailleX-35,
											  temp,warp,Fic.taille,prc);

	ColCol(Cfg->TailleX-1,1,Cfg->TailleY-2,Cfg->col[7]);
	ChrCol(Cfg->TailleX-1,1,cur1-1,32);
	ChrCol(Cfg->TailleX-1,cur1,cur2-cur1+1,219);
	ChrCol(Cfg->TailleX-1,cur2+1,Cfg->TailleY-1-cur2,32);

	if (shift!=1)
		{
		shift=2;
		break;
		}
	shift=1;
	}
	else
	if (shift==1)
		{
		shift=-1;
		break;
		}
#endif
}

(Fic.posn)=posd;

if ( (shift!=-1) & (shift!=2) )
{
code=Wait(0,0);

#ifndef NOMOUSE

if (code==0)	 //--- Pression bouton souris --------------------------
	{
	int button;

	button=MouseButton();

	if ((button&1)==1)	   //--- gauche --------------------------------
		{
		int x,y;

		x=MousePosX();
		y=MousePosY();


		if ( (x==Cfg->TailleX-1) & (y>=1) & (y<=Cfg->TailleY-1) )
													  //-- Ascensceur --
			{
			(Fic.posn)=((Fic.taille)*(y-1))/(Cfg->TailleY-2);
			}
			else
			if (y==Cfg->TailleY-1)
				if (Cfg->TailleX==90)
					code=(0x3B+(x/9))*256;
					else
					code=(0x3B+(x/8))*256;
				else
				{
				if (y>(Cfg->TailleY)/2)
					code=80*256;
					else
					code=72*256;
				ReleaseButton();
				}
		}

	if ((button&2)==2)	   //--- droite --------------------------------
		{
		code=27;
		}
	}
#endif

switch(code)
	{
	case 0x2500:		//--- ALT-K ------------------------------------
		AltK();
		break;
	case 0x54:			//--- SHIFT-F1 ---------------------------------
		Decrypt();
		break;
	case KEY_F(1):		//--- F1 ---------------------------------------
		HelpTopic(8);
		fin=VIEWER_TXT;
		break;
	case KEY_F(2):		//--- F2 ---------------------------------------
		if (autowarp==1) break;
		ViewCfg->warp++;
		if (ViewCfg->warp==3) ViewCfg->warp=0;
		RefreshBar(bar);
		Bar(bar);
		break;
	case KEY_F(3):		//--- F3 ---------------------------------------
		fin=ChangeLnFeed();
		break;
	case KEY_F(4):		//--- F4 ---------------------------------------
		cv=ChangeViewer(0);
		if (cv!=0)
			fin=cv;
		break;
	case KEY_F(5):		//--- F5 ---------------------------------------
		ViewCfg->ajustview^=1;
		fin=VIEWER_TXT;
		break;
	case KEY_F(6):		//--- F6 ---------------------------------------
		ChgViewPreproc(Fic);
		break;
	case KEY_F(7):		//--- F7 ---------------------------------------
		SearchTxt(Fic);
		break;
	case KEY_F(8):		//--- F8 ---------------------------------------
		Print(fichier,1);
		break;
	case KEY_RIGHT: 	//--- RIGHT ------------------------------------
		warp+=8;
		break;
	case KEY_LEFT:		//--- LEFT -------------------------------------
		warp-=8;
		break;
	case KEY_C_RIGHT:	//--- CTRL RIGHT -------------------------------
		warp+=40;
		break;
	case KEY_C_LEFT:	//--- CTRL LEFT --------------------------------
		warp-=40;
		break;
	case KEY_C_HOME:	//--- CTRL HOME --------------------------------
		warp=0;
		break;
	case KEY_F(9):		//--- F9 ---------------------------------------
		ChangeMask();
		ColWin(x,y,x+xl-1,y+yl-1,Cfg->col[16]);
		break;
	case KEY_DOWN:		//--- DOWN -------------------------------------
		if (pasfini==1) break;
		TxtDown(Fic,xl);
		break;
	case KEY_UP:		//--- UP ---------------------------------------
		TxtUp(Fic,xl);
		break;
	case KEY_NPAGE: 	//--- PGDN -------------------------------------
		if (pasfini==1) break;
		for (m=0;m<yl;m++)
			if (!TxtDown(Fic,xl)) break;
		break;
	case KEY_PPAGE: 	//--- PGUP -------------------------------------
		for (m=0;m<yl;m++)
			if (!TxtUp(Fic,xl)) break;
		break;
	case KEY_END:		//--- END --------------------------------------
		(Fic.posn)=(Fic.taille);
		for (m=0;m<yl;m++)
			TxtUp(Fic,xl);
		break;
	case KEY_HOME:		//--- HOME -------------------------------------
		(Fic.posn)=0;
		break;
	case KEY_F(10): 	//--- F10 --------------------------------------
	case KEY_C_UP:		//--- CTRL-UP ----------------------------------
		fin=-1;
		break;


	case KEY_A_F11: 	// --- ALT-F11 ---------------------------------
		fin=VIEWER_AUTO;
		break;

	case KEY_A_F12: 	// --- ALT-F12 ---------------------------------
		ViewSetup(Fic);
		fin=VIEWER_TXT;
		break;
	case 32:
	case 13:
	case 27:
		fin=-1;
		break;
	case 6: 												//--- CTRL-F
		SaveScreen();
		Cfg->font^=1;
		ChangeSize();				//--- Rafraichit l'ecran -----------
		LoadScreen();
		break;
	}

if ( (ViewCfg->warp!=0) & (autowarp==1) ) warp=0;

if (warp<0) warp=0;
}
else
{
if (shift==-1)
	{
	shift=0;
	LoadScreen();
	}
if (shift==2)
	{
	shift=1;
	}
}
}
while(!fin);


if (ViewCfg->saveviewpos)
	Fic.SavePosition(ViewCfg->viewhist,Fic.posn);

if (fin==VIEWER_AUTO)
	{
	char fic[256];

	GetListFile(fic);
	if (fic[0]!=0)
		Fic.setname(fic);
	}

LoadScreen();

LibMem(bufscr);
LibMem(bufscrcol);

return fin;
}


/*--------------------------------------------------------------------*\
|-	Recherche une chaine en mode texte								  -|
\*--------------------------------------------------------------------*/
static char srcch[132]; 		//--- chaine … rechercher --------------

void SearchTxt(KKFile &Fic)
{
static char Dir[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[5] =
	{ { 2,3,1,Dir,&DirLength},
	  {15,5,2,NULL,NULL},
	  {45,5,3,NULL,NULL},
	  { 5,2,0,"Search for:",NULL},
	  { 1,1,4,&CadreLength,NULL} };

struct TmtWin F =
	{ -1,10,74,17, "Search Text" };

int a,n;
char c1,c2;

a=(Fic.posn);

do
	{
	(Fic.posn)++;

	if ((Fic.posn)==(Fic.taille))
		{
		(Fic.posn)=(Fic.taille)-2;
		break;
		}
	}
	while(Fic.ReadChar()!=10);

(Fic.posn)++;

n=WinTraite(T,5,&F,0);

if ( (n!=0) & (n!=1) ) return;

strcpy(srcch,Dir);

n=0;



do
{
if (Dir[n]==0) break;

c1=Dir[n];
if ( (c1>='a') & (c1<='z') ) c1+='A'-'a';

c2=Fic.ReadChar();;
if ( (c2>='a') & (c2<='z') ) c2+='A'-'a';

if (c1==c2)
	n++;
	else
	{
	if (n!=0)
		{
		(Fic.posn)-=(n-1);
		n=0;
		}
	}

(Fic.posn)++;
}
while ((Fic.posn)<(Fic.taille));

if (Dir[n]!=0)
	{
	(Fic.posn)=a;
	WinError("Don't find text");
	return;
	}



if ((Fic.posn)==0) return;
(Fic.posn)--;
if ((Fic.posn)==0) return;
do
	{
	(Fic.posn)--;
	if ((Fic.posn)==0) return;
	}
while(Fic.ReadChar()!=10);
if ((Fic.posn)!=0) (Fic.posn)++;
}

/*--------------------------------------------------------------------*\
|- Change le type de viewer 										  -|
\*--------------------------------------------------------------------*/
int ChangeViewer(int i)
{
MENU menu;
int nbr;
static struct barmenu bar[5];
int retour,x,y;

nbr=4;

x=((Cfg->TailleX)-10)/2;
y=((Cfg->TailleY)-2*(nbr-2))/2;
if (y<2) y=2;

bar[0].Titre="Texte";
bar[0].Help=0;
bar[0].fct=VIEWER_TXT;

bar[1].Titre="Binary";
bar[1].Help=0;
bar[1].fct=VIEWER_BINARY;

bar[2].Titre="Ansi";
bar[2].Help=0;
bar[2].fct=VIEWER_ANSI;

bar[3].Titre="HTML";
bar[3].Help=0;
bar[3].fct=VIEWER_HTML;

menu.cur=i; // +1
//if (menu.cur>nbr) menu.cur=0;

menu.x=x;
menu.y=y;
menu.attr=8;

NewEvents(NextFct, " Next ",4);

retour=PannelMenu(bar,nbr,&menu);

ClearEvents();

if (retour==2)
	return bar[menu.cur].fct;

return 0;
}

/*--------------------------------------------------------------------*\
|-	Recherche une chaine en mode hexa								  -|
\*--------------------------------------------------------------------*/

void SearchHexa(KKFile &Fic)
{
static char Text[70],Hexa[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[8] =
	{ { 2,3,1,Hexa,&DirLength},
	  { 2,7,1,Text,&DirLength},
	  {15,9,2,NULL,NULL},
	  {45,9,3,NULL,NULL},
	  { 5,6,0,"Search for text:",NULL},
	  { 5,2,0,"Search for hexa:",NULL},
	  { 1,1,4,&CadreLength,NULL},
	  { 1,5,4,&CadreLength,NULL} };

struct TmtWin F =
	{-1,7,74,18, "Search Text/Hexa" };

int a,n,lng;
char c1,c2;
int testhexa=0;

a=(Fic.posn);

if (strlen(Hexa)!=0)
	(Fic.posn)++;

n=WinTraite(T,8,&F,0);

if (n!=2) return;

if (strlen(Hexa)!=0)
	{
	int len=0,te,t,l;

	l=strlen(Hexa);

	for(te=0;te<l;te+=3)
		{
		Hexa[te+2]=0;
		sscanf(Hexa+te,"%02X",&t);
		Hexa[te+2]=32;
		srcch[len]=(char)t;
		len++;
		}

	srcch[len]=0;
	lng=len;
	testhexa=1;
	}
	else
	{
	strcpy(srcch,Text);
	lng=strlen(Text);
	}


n=0;

(Fic.posn)++;

do
{
if (n>=lng) break;

c1=srcch[n];
if ( (c1>='a') & (c1<='z') & (testhexa==0) ) c1+='A'-'a';

c2=Fic.ReadChar();;
if ( (c2>='a') & (c2<='z') & (testhexa==0) ) c2+='A'-'a';

if (c1==c2)
	n++;
	else
	{
	if (n!=0)
		{
		(Fic.posn)-=(n-1);
		n=0;
		}
	}

(Fic.posn)++;
}
while ((Fic.posn)<(Fic.taille));

if (srcch[n]!=0)
	{
	(Fic.posn)=a;
	WinError("Don't find text");
	return;
	}

(Fic.posn)=(Fic.posn)-lng;
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|- Viewer HTML														  -|
\*--------------------------------------------------------------------*/


struct Href
	{
	char *link;
	signed int x1,y1;
	signed int x2,y2;
	struct Href *from,*next;
	};

struct HLine
	{
	char *Chr;
	char *Col;
	struct HLine *from,*next;
	};

struct HTMtd
	{
	char align; //---0: left, 1:right, 2:center -------------------------
	};


/*--------------------------------------------------------------------*\
|- Allocation d'une ligne HTML                                        -|
\*--------------------------------------------------------------------*/
struct HLine *HtmAlloc(void)
{
struct HLine *H;
int x;

x=Cfg->TailleX;

H=(struct HLine*)GetMem(sizeof(struct HLine));

H->Chr=(char*)GetMem(x);
memset(H->Chr,32,x);
H->Col=(char*)GetMem(x);
memset(H->Col,Cfg->col[16],x);

H->next=NULL;
H->from=NULL;



return H;
}

/*--------------------------------------------------------------------*\
|- Ligne HTML pr‚cedente											  -|
\*--------------------------------------------------------------------*/
void HTMLfrom(struct HLine **D,struct HLine **P,int *ye)
{
if ((*P)->from!=NULL)
	{
	(*ye)--;
	(*P)=(*P)->from;
	(*D)=(*D)->from;
	}
}

/*--------------------------------------------------------------------*\
|- Ligne HTML suivante												  -|
\*--------------------------------------------------------------------*/
void HTMLnext(struct HLine **D,struct HLine **P,int *ye)
{
if ((*D)->next!=NULL)
	{
	(*ye)++;
	(*P)=(*P)->next;
	(*D)=(*D)->next;
	}
}


/*--------------------------------------------------------------------*\
|- Lecture du tag TD, P, Hx (changement de paragraphe)				  -|
\*--------------------------------------------------------------------*/
void ReadTagTD(char *titre,struct HTMtd *HTM)
{
char buffer[256];
char *align;

// HTM->align=0;

strncpy(buffer,titre,255);
buffer[255]=0;
strupr(buffer);

align=strstr(buffer," ALIGN"); //--- Pour le cas ou 'VALIGN' -----------
if (align!=NULL)
	{
	align=strchr(align,'=');
	if (align!=NULL)
		{
		strcpy(buffer,align+1);
		if (!strnicmp(buffer,"center",6)) HTM->align=2;
		if (!strnicmp(buffer,"right",5)) HTM->align=1;
		if (!strnicmp(buffer,"left",4)) HTM->align=0;
		}
	}
}

char ReadAlign(char *titre)
{
char buffer[256];
char *align;

char al;

strncpy(buffer,titre,255);
buffer[255]=0;
strupr(buffer);

al=0;

align=strstr(buffer," ALIGN"); //--- Pour le cas ou 'VALIGN' -----------
if (align!=NULL)
	{
	align=strchr(align,'=');
	if (align!=NULL)
		{
		strcpy(buffer,align+1);
		if (!strnicmp(buffer,"center",6)) al=2;
		if (!strnicmp(buffer,"right",5)) al=1;
		if (!strnicmp(buffer,"left",4)) al=0;
		}
	}

return al;
}

/*--------------------------------------------------------------------*\
|- Gestion du filtre pour l'bufchr texte                           -|
\*--------------------------------------------------------------------*/
void Masque(int x1,int y1,int x2,int y2,char *bufscr,char *bufcol)
{
char ok=1;
char *chaine;
char chain2[132];
int m1,m2;

uchar c,c2,oldc;
int x,y,l;
int oldx,oldy,oldl;
int xt[80],yt[80];

int y3;

char cont,trouve=0;

struct PourMask *CMask;

CMask=ViewCfg->Mask[(ViewCfg->wmask)&15];

chaine=CMask->chaine;

if ((((ViewCfg->wmask)&128)==128) | (*chaine==0))
	{
	int x,y,y3;

	for(y=y1;y<=y2;y++)
		{
		y3=y*Cfg->TailleX;
		for(x=x1;x<=x2;x++)
			AffChr(x,y,bufscr[x+y3]);
		}
	return;
	}

x=x1;
y=y1;
y3=y*(Cfg->TailleX);

l=0;

while ((y<=y2) | (ok==0) )
	{
	if (ok)
		c=bufscr[x+y3];
		else
		c=32;

	if ( ((c>='a') & (c<='z')) | ((c>='A') & (c<='Z')) | (c=='_') |
			((c>=0xE0) & (c<=0xEB)) |			 ((c>='0') & (c<='9')) )
		{
		chain2[l]=c;
		xt[l]=x;
		yt[l]=y;
		l++;
		if (l==80) l=0;
		}
	else	//--- Separator --------------------------------------------
		{
		if (l!=0)
			{
			cont=(trouve==2);	//--- Maybe a 'ö' name -----------------
			trouve=0;
			m1=0;
			m2=0;

			while ( (chaine[m2]!='@') & (trouve==0) )
				{
				if ((chaine[m2]==32) | ((chaine[m2]==246) & (cont==0)))
					{
					if (m2-m1==l)
						{
						if (CMask->Ignore_Case==0)
							{
							if (!strncmp(chaine+m1,chain2,l))
								trouve=1;
							}
							else
							{
							if (!strnicmp(chaine+m1,chain2,l))
								trouve=1;
							}
						if ( (trouve==1) & (chaine[m2]==246) )
							{
							xt[l]=xt[l-1];
							yt[l]=yt[l-1];
							trouve=2;
							chain2[l]=246;
							oldl=l;
							oldx=x; // x
							oldy=y; // y
							oldc=c;
							l++;
							}

						if (m1!=0)
							{
							if (chaine[m1-1]==246)
								trouve=0;
							}
						}
					m1=m2+1;
					}
				m2++;
				}

			if (trouve!=2)
				{
				if (trouve==1)
					c2=Cfg->col[17];	 // trouve -> bright
					else
					c2=Cfg->col[16];

				if ( (trouve==0) & (cont==1) )
					{
					x=oldx;
					y=oldy;
					l=oldl;
					c=oldc;
					y3=y*(Cfg->TailleX);
					}

				while (l!=0)
					{
					l--;
					if ((c2==Cfg->col[16]) & (bufcol[xt[l]+(yt[l])*(Cfg->TailleX)]!=0))
						AffCol(xt[l],yt[l],bufcol[xt[l]+(yt[l])*(Cfg->TailleX)]);
					else
						AffCol(xt[l],yt[l],c2);

					AffChr(xt[l],yt[l],bufscr[xt[l]+(yt[l])*(Cfg->TailleX)]);
					}
				l=0;
				}
			}

		if (ok) //--- Display of the separator -------------------------
			{
			if (bufcol[x+y*Cfg->TailleX]!=0)
				AffCol(x,y,bufcol[x+y*Cfg->TailleX]);
			else
				AffCol(x,y,Cfg->col[16]);		// Ou autre chose
			AffChr(x,y,c);
			}
		}

	if (ok)
		{
		x++;
		if (x>x2)
			{
			x--;
			ok=0;
			}
		}
	else
		{
		x=x1;
		y++;
		y3=y*(Cfg->TailleX);
		ok=1;

		if (ViewCfg->maskaccel)
			{
			if (KbHit()) break;
			}
		}
	}

/*--------------------------------------------------------------------*\
|- Affichage de la chaine qui a ‚t‚ trouv‚ (par F7) 				  -|
\*--------------------------------------------------------------------*/

if (*srcch!=0)
	{
	y=y1;
	for (x=x1;x<=x2;x++)
		chain2[x-x1]=bufscr[x+y1*(Cfg->TailleX)];
	chain2[x2-x1+1]=0;

	for (x=0;x<=(x2-x1)-strlen(srcch)+1;x++)
		if (!strnicmp(chain2+x,srcch,strlen(srcch)))
			for(l=0;l<strlen(srcch);l++)
				{
				AffCol(x1+l+x,y1,Cfg->col[18]); 	//Reverse
				AffChr(x1+l+x,y1,bufscr[(x1+l+x)+y1*(Cfg->TailleX)]);
				}
	}

}


/*--------------------------------------------------------------------*\
|- Changement du filtre pour l'bufchr du texte                     -|
\*--------------------------------------------------------------------*/
void ChangeMask(void)
{
MENU menu;
int nbr;
char bars1[25];
static struct barmenu bar[19];
int retour,x,y,n,i,max;

char fin;

max=0;

nbr=2;

for(i=0;i<16;i++)
	if (strlen(ViewCfg->Mask[i]->title)>0)
		{
		bar[nbr].fct=i+10;

		bar[nbr].Titre=ViewCfg->Mask[i]->title;
		bar[nbr].Help=0;

		if ( ((ViewCfg->wmask)&15) ==i) n=nbr;

		if (strlen(ViewCfg->Mask[i]->title)>max) max=strlen(ViewCfg->Mask[i]->title);

		nbr++;
		}

x=((Cfg->TailleX)-max)/2;
y=((Cfg->TailleY)-2*(nbr-2))/2;
if (y<2) y=2;

do
{
sprintf(bars1,"Mask %3s",(ViewCfg->wmask&128)==128 ? "OFF" : "ON");
bar[0].Titre=bars1;
bar[0].Help=0;
bar[0].fct=2;

bar[1].fct=0;

fin=0;

SaveScreen();

do
	{
	menu.x=x;
	menu.y=y;
	menu.cur=n;
	menu.attr=4+8;

	retour=PannelMenu(bar,nbr,&menu);

	n=menu.cur;
	}
while ( (retour==1) | (retour==-1) );

LoadScreen();

if (retour==2)
	{
	switch (bar[n].fct)
		{
		case 1:
			ViewCfg->wmask^=64;
			break;
		case 2:
			ViewCfg->wmask^=128;
			break;
		case 3:
			break;
		default:
			ViewCfg->wmask=(char)(((ViewCfg->wmask)&240)|(bar[n].fct-10));
			fin=1;
		}
	}
}
while ( (!fin) & (retour!=0) );
}

int NextFct(struct barmenu *bar)
{
if (bar==NULL);

PutKey(KEY_ENTER);
PutKey(KEY_DOWN);

return 0;
}

/*--------------------------------------------------------------------*\
|- Changement du filtre pour l'bufchr du texte                     -|
\*--------------------------------------------------------------------*/
void ChangeTrad(void)
{
int nbr;
static struct barmenu bar[5];
char bars0[25],bars1[25],bars2[25],bars4[25];
int retour,x,y,n,max;
MENU menu;

char fin;

max=0;

nbr=5;

x=((Cfg->TailleX)-max)/2;
y=((Cfg->TailleY)-2*(nbr-2))/2;
if (y<2) y=2;

sprintf(bars0,"Normal %c",ViewCfg->cnvtable==0 ? 15 : 32);
bar[0].Titre=bars0;
bar[0].Help=0;
bar[0].fct=1;

sprintf(bars1,"BBS    %c",ViewCfg->cnvtable==1 ? 15 : 32);
bar[1].Titre=bars1;
bar[1].Help=0;
bar[1].fct=2;

sprintf(bars2,"Latin  %c",ViewCfg->cnvtable==2 ? 15 : 32);
bar[2].Titre=bars2;
bar[2].Help=0;
bar[2].fct=3;

bar[3].fct=0;

sprintf(bars4,"%s",ViewCfg->autotrad ? "Auto" : "No Auto");
bar[4].Titre=bars4;
bar[4].Help=0;
bar[4].fct=10;

n=0;
if (ViewCfg->autotrad) n=4;
	else
	while (ViewCfg->cnvtable!=(bar[n].fct)-1)
		n++;

fin=0;

menu.x=x;
menu.y=y;
menu.cur=n;
menu.attr=8;

retour=PannelMenu(bar,nbr,&menu);

n=menu.cur;

if (retour==2)
	{
	switch (bar[n].fct)
		{
		case 1:
		case 2:
		case 3:
			ViewCfg->cnvtable=(char)(bar[n].fct-1);
			ViewCfg->autotrad=0;
			break;
		case 10:
			ViewCfg->autotrad^=1;
			break;
		default:
			break;
		}
	}
}



/*--------------------------------------------------------------------*\
|- Changement du Line Feed pour le passage … la ligne				  -|
\*--------------------------------------------------------------------*/
int ChangeLnFeed(void)
{
MENU menu;
int nbr;
static struct barmenu bar[5];
char bars4[25];
int retour,x,y,n;

nbr=5;

x=((Cfg->TailleX)-10)/2;
y=((Cfg->TailleY)-2*(nbr-2))/2;
if (y<2) y=2;

bar[0].Titre="DOS (CR/LF)";
bar[0].Help=0;
bar[0].fct=1;						// lnfeed=0

bar[1].Titre="Unix (LF)";
bar[1].Help=0;
bar[1].fct=3;						// lnfeed=2

bar[2].Titre="Mac (CR)";
bar[2].Help=0;
bar[2].fct=2;						// lnfeed=1

bar[3].Titre="Mixed (CR-LF)";
bar[3].Help=0;
bar[3].fct=5;						// lnfeed=4

bar[4].fct=4;

n=0;

while (ViewCfg->lnfeed!=(bar[n].fct)-1)
	n++;

SaveScreen();

do
	{
	sprintf(bars4,"User Line Feed: $%02X",ViewCfg->userfeed);
	bar[4].Titre=bars4;
	bar[4].Help=0;

	menu.x=x;
	menu.y=y;
	menu.cur=n;
	menu.attr=4;

	NewEvents(NextFct, " Next ",3);

	retour=PannelMenu(bar,nbr,&menu);

	ClearEvents();

	n=menu.cur;

	if ((bar[n].fct==4) & (retour!=2))
		ViewCfg->userfeed+=retour;
	}
while ( (retour==1) | (retour==-1) );

LoadScreen();

if (retour==2)
	{
	ViewCfg->lnfeed=(char)((bar[n].fct)-1);
	}

if (retour==0)
	return 0;
	else
	return VIEWER_TXT;	//--- Viewer TeXTe -----------------------------

}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|- Gestion impression												  -|
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|-	Retourne 1 si tout va bien										  -|
\*--------------------------------------------------------------------*/

#ifndef LINUX
char PRN_print(int lpt,char a)
{
union REGS regs;
char code,cont;
char result=1;

do
	{
	regs.h.ah=0;
	regs.h.al=a;
	regs.w.dx=(short)lpt;

	int386(0x17,&regs,&regs);

	code=regs.h.ah;

	cont=0;

	if ( ((code&128)==128) & ((code&16)==16) )
		cont=1;

	if ((code&1)==1)
		if (WinMesg("Time-Out","Do you want to continue ?",1)==1)
			result=0,cont=0;
			else
			cont=1;
	if ( ((code&8)==8) & (result==1) )
		if (WinMesg("I/O Error","Do you want to continue ?",1)==1)
			result=0,cont=0;
			else
			cont=1;
	if ( ((code&32)==32) & (result==1) )
		if (WinMesg("No more paper","Do you want to continue ?",1)==1)
			result=0,cont=0;
			else
			cont=1;
	if ((code&64)==64)
		PrintAt(0,0,"ACK Error"),cont=0;
	}
while(cont);

return result;
}
#else
char PRN_print(int lpt,char a)
{
return 1;
}
#endif



void Print(char *fichier,int n)
{
static int sw,lf=28,l1,pp;

FILE *fic;
int lpt;
char a;
int m;
int ok;

static char x1=22,x2=22,x3=22;
static int y1=4,y2=7,y3=1;

struct Tmt T[] = {
	  { 5, 2,10,"LPT1",&sw},
	  { 5, 3,10,"LPT2",&sw},
	  { 5, 4,10,"LPT3",&sw},
	  { 5, 5,8,"IBM Graphic Code",&l1},

	  { 5, 8, 7,"Line Feed",&lf},

	  {32, 2,10,"10    cpi",&pp},
	  {32, 3,10,"12    cpi",&pp},
	  {32, 4,10,"15    cpi",&pp},
	  {32, 5,10,"17.1  cpi",&pp},
	  {32, 6,10,"20    cpi",&pp},
	  {32, 7,10,"24    cpi",&pp},
	  {32, 8,10,"30    cpi",&pp},

	  {3,1,9,&x1,&y1},
	  {28,1,9,&x2,&y2},
	  {3,7,9,&x3,&y3},

	  { 8,10,2,NULL,NULL},										  // 1:Ok
	  {33,10,3,NULL,NULL}
	  };

struct TmtWin F = {-1,4,55,16, "Print file"};

char StrInit[]={27,51,0,27,91,0};

ok=1;		 // Tout va bien

sw=0;
l1=1;
pp=8;

m=WinTraite(T,17,&F,0);

if (m==-1)	//--- escape -----------------------------------------------
	return;
	else
	if (T[m].type==3) return;  //--- cancel ----------------------------

fic=fopen(fichier,"rb");
if (fic==NULL)
	{
	WinError("Couldn't open file");
	return;
	}

lpt=sw;

if (l1==1)
	{
	StrInit[2]=(char)(lf);
	StrInit[5]=(char)(pp-5);

	for(m=0;m<6;m++)
		if ((ok=PRN_print(lpt,StrInit[m]))==0) break;
	}


if ( (n==1) & (ok==1) )  //--- Fichier TEXTE ---------------------------
	{
	do
		{
		if (fread(&a,1,1,fic)==0) break;
		if (PRN_print(lpt,a)==0) break;
		}
	while(1);
	}

if (ok==1)
	WinMesg("Print","The file is printed",0);
}


/*--------------------------------------------------------------------*\
|- Gestion Ansi 													  -|
\*--------------------------------------------------------------------*/
char *AnsiBuffer;
int Ansi1,Ansi2;

void BufAffChr(long x,long y,long c)
{
if (y<MAXANSLINE)
	AnsiBuffer[y*160+x]=(char)c;
}

void BufAffCol(long x,long y,long c)
{
if (y<MAXANSLINE)
	AnsiBuffer[y*160+x+80]=(char)c;
}


void StartAnsiPage(KKFile &Fic)
{
AnsiAffChr=BufAffChr;
AnsiAffCol=BufAffCol;

AnsiBuffer=(char*)GetMem(160*MAXANSLINE);

Ansi1=0;

maxx=80;
maxy=Cfg->TailleY;

curx=0;
cury=0;

if (kk_raw==0)
	ReadANS(Fic);
	else
	ReadRAW(Fic);

Ansi2=cury+1-Cfg->TailleY+2;
if (Ansi2<0) Ansi2=0;
}

void DispAnsiPage(int x1,int y1,int x2,int y2)
{
int x,y;
static char cc=0;

char col;

cc++;
if (cc==16) cc=0;

for(y=y1;y<y2;y++)
	{
	for(x=x1;x<x2;x++)
		{
		col=AnsiBuffer[(y+Ansi1)*160+80+x];

		AffChr(x,y,AnsiBuffer[(y+Ansi1)*160+x]);

		if (((col&15)==(col/16)) & (secpart))
			AffCol(x,y,(col&240)+cc);
			else
			AffCol(x,y,col);
		}
	if (KbHit()) break;
	}
}

void CloseAllPage(void)
{
LibMem(AnsiBuffer);
}


/*--------------------------------------------------------------------*\
|- Affichage ansi													  -|
\*--------------------------------------------------------------------*/
int AnsiView(KKFile &Fic)
{
clock_k cl,cl2;

#ifndef NOMOUSE
int xm;
#endif
int ym,zm;

int aff,wrap;

int cv;

char autodown=0;

int code;
int fin=0;

int warp=0;

int shift=0; //--- Vaut -1 si rebufchr de l'ecran -------------------
			 //---		 0 si pas de shift -----------------------------
			 //---		 1 si touche shiftee ---------------------------


static char bar[81];

static char oldpal[48],oldcol[64];
static int tx,ty,font;

char fichier[256];
Fic.getname(fichier);

SaveScreen();
PutCur(3,0);

memcpy(oldpal,Cfg->palette,48);
memcpy(oldcol,Cfg->col,64);
tx=Cfg->TailleX;
ty=Cfg->TailleY;
font=Cfg->font;

memcpy(Cfg->palette,NORTONPAL,48);
memcpy(Cfg->col,NORTONCOL,64);

Cfg->TailleX=80;
Cfg->font=0;

SetMode();

StartAnsiPage(Fic);

wrap=0;
aff=1;


/*--------------------------------------------------------------------*\
|- Affichage de la bar												  -|
\*--------------------------------------------------------------------*/

strcpy
   (bar," Help Spart  ---- ChView ---- Prepro ----  ----  ----  Quit ");


secpart=0;

Bar(bar);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

cl=GetClock();

do
{


/*--------------------------------------------------------------------*\
|- Gestion des touches												  -|
\*--------------------------------------------------------------------*/

zm=0;
code=0;

while ( (!KbHit()) & (zm==0) & (code==0) )
{
#ifndef NOMOUSE
GetPosMouse(&xm,&ym,&zm);
#else
ym=0;
#endif

if ((iskeypressed(SHIFT)) | (ym==Cfg->TailleY-1))
	{
	int prc;
	char temp[132];

	if (Ansi2!=0)
		prc=(Ansi1*100)/Ansi2;
		else
		prc=100;

	ColLin(0,0,Cfg->TailleX,3*16+0);

	strncpy(temp,fichier,78);

	temp[45]=0;

	PrintAt(0,0,
			"View: %-*s Col%3d %9d bytes %3d%% ",Cfg->TailleX-35,
												  temp,warp,(Fic.taille),prc);

	Bar(bar);

	if (shift!=1)
		{
		shift=2;
		break;
		}
	shift=1;
	DispAnsiPage(0,1,Cfg->TailleX,(Cfg->TailleY)-1); // Display page ---
	}
	else
	{
	if (shift==1)
		{
		shift=-1;
		break;
		}
	DispAnsiPage(0,0,Cfg->TailleX,Cfg->TailleY);	 // Display page ---
	}

if (autodown)
	{
	while ((GetClock()-cl)<5);

	cl2=GetClock()-cl;

	code=80*256;
	cl=GetClock();
	}
}

if ( (shift!=-1) & (shift!=2) )
{
if (KbHit())
	{
	autodown=0;
	code=Wait(0,0);
	}

#ifndef NOMOUSE
if (code==0)	 //--- Pression bouton souris --------------------------
	{
	int x,y,button,button2;

	GetPosMouse(&x,&y,&button);


	if (((button&3)!=0) & (y==Cfg->TailleY-1))
		{
		do
			GetPosMouse(&x,&y,&button2);
		while((button2&3)!=0);
		}

	if ((button&1)==1)	   //--- gauche --------------------------------
		{
		int x,y;

		x=MousePosX();
		y=MousePosY();


		if ( (x==Cfg->TailleX-1) & (y>=1) & (y<=Cfg->TailleY-1) )
													  //-- Ascensceur --
			{
			(Fic.posn)=((Fic.taille)*(y-1))/(Cfg->TailleY-2);
			}
			else
			if (y==Cfg->TailleY-1)
				if (Cfg->TailleX==90)
					code=(0x3B+(x/9))*256;
					else
					code=(0x3B+(x/8))*256;
				else
				{
				if (y>(Cfg->TailleY)/2)
					code=80*256;
					else
					code=72*256;
				ReleaseButton();
				}
		}

	if ((button&2)==2)	   //--- droite --------------------------------
		{
		code=27;
		}
	}
#endif

switch(LO(code))
	{
	case 0:
		switch(HI(code))
			{
			case 0x25:	// --- ALT-K -----------------------------------
				AltK();
				break;
			case 0x54:	  //--- SHIFT-F1 -------------------------------
			case 0x3B:	  //--- F1 -------------------------------------
				HelpTopic(8);
				fin=VIEWER_ANSI;
				break;
			case 0x57:	  //--- SHIFT-F4 -------------------------------
			case 0x3E:	  //--- F4 -------------------------------------
				cv=ChangeViewer(2);
				if (cv!=0)
					fin=cv;
				break;

			case 0x55:	  //--- SHIFT-F2 -------------------------------
			case 0x3C:	  //--- F2 -------------------------------------
				secpart^=1;
				bar[11]= (secpart) ? SELCHAR : ' ';
				break;

			case 0x5A:	  //--- SHIFT-F6 -------------------------------
			case 0x40:	  //--- F6 -------------------------------------
				ChgViewPreproc(Fic);
				fin=VIEWER_ANSI;
				break;

			case 0x8B:	// --- ALT-F11 ---------------------------------
				fin=VIEWER_AUTO;
				break;

			case 0x8C:	// --- ALT-F12 ---------------------------------
				ViewSetup(Fic);
				fin=VIEWER_ANSI;
				break;
			case 80:	  //--- DOWN -----------------------------------
				Ansi1++;
				break;
			case 72:	  //--- UP -------------------------------------
				if (Ansi1!=0)
					Ansi1--;
				break;
			case 0x51:	  //--- PGDN -----------------------------------
				Ansi1+=Cfg->TailleY;
				break;
			case 0x49:	  //--- PGUP -----------------------------------
				Ansi1-=Cfg->TailleY;
				if (Ansi1<0) Ansi1=0;
				break;
			case 0x4F:	  //--- END ------------------------------------
				Ansi1=Ansi2;
				break;
			case 0x47:	  //--- HOME -----------------------------------
				Ansi1=0;
				break;
			case 0x5D:	  //--- SHIFT-F10 ------------------------------
			case 0x44:	  //--- F10 ------------------------------------
			case 0x8D:	  //--- CTRL-UP --------------------------------
				fin=-1;
				break;
			}
		break;
	case 32:
	case 13:
	case 27:
		fin=-1;
		break;
	}

if ((Ansi1+Cfg->TailleY)>MAXANSLINE)
	Ansi1=MAXANSLINE-Cfg->TailleY;

}
else
{
if (shift==-1)
	{
	shift=0;
//	  LoadScreen();
	}
if (shift==2)
	{
	shift=1;
	}
}
}
while(!fin);

CloseAllPage();

memcpy(Cfg->palette,oldpal,48);
memcpy(Cfg->col,oldcol,64);

Cfg->TailleY=ty;
Cfg->TailleX=tx;
Cfg->font=(char)font;

ChangeSize();

if (fin==VIEWER_AUTO)
	{
	char fic[256];

	GetListFile(fic);
	if (fic[0]!=0)
		Fic.setname(fic);
	}

LoadScreen();

return fin;
}

void Decrypt(void)
{
unsigned short tab[256];
short ord[256];
short i,j,k,n;
char col[]=" etanoris-hdlc";
short val;

char *buffer;
int tbuf;

int kbest,xval;

kbest=0;
xval=0;

buffer=(char*)GetMem(Cfg->TailleX*Cfg->TailleY);
tbuf=0;

for(j=0;j<Cfg->TailleY;j++)
	for(i=0;i<Cfg->TailleX;i++)
		{
		buffer[tbuf]=GetChr(i,j);
		tbuf++;
		}


for(k=0;k<256;k++)
{
val=0;

for(i=0;i<256;i++)
	tab[i]=0;

for(i=0;i<256;i++)
	ord[i]=i;

for (i=0;i<tbuf;i++)
	tab[(buffer[i])^k]++;

for (i=0;i<256;i++)
	for(j=i;j<256;j++)
		if (tab[ord[i]]<tab[ord[j]])
			{
			n=ord[i];
			ord[i]=ord[j];
			ord[j]=n;
			}

for (i=0;i<15;i++)
	for (n=0;n<15;n++)
		if (ord[i]==col[n])
			{
			val+=100+((50*abs(i-n))/15);
			break;
			}

if (val>xval)
	{
	xval=val;
	kbest=k;
	}
}

sprintf(buffer,"Try XOR %d",kbest);
WinError(buffer);

LibMem(buffer);

}

/*--------------------------------------------------------------------*\
|- Fonction principale du viewer									  -|
\*--------------------------------------------------------------------*/

void View(KKVIEW *V,char *file,int type)
{
int n,o;
int i;
int debpos;
extern struct key K[nbrkey];
char file2[256];

debpos=0;

kk_raw=0;

ViewCfg=V;

SaveScreen();

Bar(" ----  ----  ---- ChView ----  ----  ----  ----  ----  ---- ");

strcpy(file2,file);

//printf("Traite Commandline after dos (%s-%d) FIN)\n\n",file2,type); exit(1);


if (file2[0]==0)
	GetListFile(file2);

KKFile Fic(file2);

switch(type)
	{
	case 0:
		i=VIEWER_AUTO;
		break;
	case 1:
		i=VIEWER_ANSI;
		break;
	case 2:
		i=VIEWER_HTML;
		break;
	case 3:
		i=-1;
		break;
	case 4:
		i=VIEWER_TXT;
		break;
	case 5:
		i=VIEWER_ANSI;
		kk_raw=1;
		break;
	default:
		i=-1;
		WinError("You couldn't arrive here ! Weird...");
		break;
	}

while(i!=-1)
	{
	i=Fic.Reload(i);

	switch(i)
		{
		case VIEWER_ANSI:  // Ansi
			i=AnsiView(Fic);
			break;
		case VIEWER_HTML: // HTML
			i=HtmlView(Fic,debpos);
			break;
		case -1:
			break;
		default:
			o=-1;
			for(n=0;n<nbrkey;n++)
				if (K[n].numero==i) o=n;

			if (o==-1)
				{
				i=HexaView(Fic);
				}
				else
				{
				if ((K[o].other&4)==4)
					i=TxtView(Fic);
					else
					i=HexaView(Fic);
				}
			break;
		}
	}

LoadScreen();
}

void ViewSetup(KKFile &Fic)
{

int res;

//--- Internal Variable ---

static char KKTmpX0=29;
static int KKTmpY0=4;
static int KKTmp9=5;
static char KKTmpX12=16;
static int KKTmpY12=6;

//--- User Field ---

static int l1,l2;

static int KKField1;
static int KKField2;
static int KKField5;
static char KKField9[6];

struct TmtWin F = {-1,9,51,18,"Viewer"};

struct Tmt T[] = {
	{ 1, 1, 9,&KKTmpX0,&KKTmpY0},
	{ 3, 2, 8,"Auto adjust viewer size",&KKField1},
	{ 3, 3, 8,"Save position in viewer",&KKField2},
	{ 3, 4, 7,"First byte:   ",&l1},
	{ 3, 5, 7,"Size of file: ",&l2},
	{ 2, 7, 2,NULL,NULL},
	{17, 7, 3,NULL,NULL},
	{34, 3,10,"DOS (CR/LF)",&KKField5},
	{34, 4,10,"Unix (CR)",&KKField5},
	{34, 5,10,"LF",&KKField5},
	{34, 6,10,"User",&KKField5},
	{43, 6, 1,KKField9,&KKTmp9},
	{34, 7,10,"Mixed CR-LF",&KKField5},
	{33, 2, 0,"Line Feed:",NULL},
	{32, 1, 9,&KKTmpX12,&KKTmpY12}
   };

l1=Fic.fromoct;
l2=Fic.nbroct;

KKField1=ViewCfg->ajustview;
KKField2=ViewCfg->saveviewpos;
KKField5=5;
sprintf(KKField9,"%d",ViewCfg->userfeed);

res=WinTraite(T,15,&F,3);

if (res==-1)
	return;

if (T[res].type==3)
	return;

Fic.fromoct=l1;
Fic.nbroct=l2;

ViewCfg->ajustview=(char)KKField1;
ViewCfg->saveviewpos=(char)KKField2;
ViewCfg->lnfeed=(char)(KKField5-5);
sscanf(KKField9,"%d",&l1);
ViewCfg->userfeed=(char)l1;
}

#define MAXFILE 500

void GetListFile(char *fichier)
{
FILE *fic;
uchar n;
long s,size;
static char ficname[256];

static struct barmenu *bar;
int nbrbar=0;
MENU menu;

char err;

bar=(struct barmenu*)GetMem(sizeof(struct barmenu)*MAXFILE);

if (ViewCfg->saveviewpos)
	{
	fic=fopen(ViewCfg->viewhist,"rb");
	if (fic!=NULL)
		{
		while(fread(&n,1,1,fic)==1)
			{
			fread(ficname,1,n,fic);
			fread(&size,4,1,fic);
			fread(&s,4,1,fic);

/*
			if ( (!stricmp(ficname,fichier)) & (size==(Fic.taille)) )
				{
				posdeb=s;
				break;
				}
*/

			bar[nbrbar].Titre=(char*)GetMem(strlen(ficname)+1);
			strcpy(bar[nbrbar].Titre,ficname);
			bar[nbrbar].Help=0;
			bar[nbrbar].fct=nbrbar+1;
			nbrbar++;
			if (nbrbar==MAXFILE) break;
			}
		fclose(fic);
		}
	}

menu.x=2;
menu.y=2;

menu.attr=2+8;

menu.cur=nbrbar-1;

err=0;

if (nbrbar==0)
	err=1;

if (nbrbar>1)
	if (PannelMenu(bar,nbrbar,&menu)!=2)
		err=1;

if (!err)
	{
	Path2Abs(fichier,"..");
	Path2Abs(fichier,bar[menu.cur].Titre);
	}
	else
	fichier[0]=0;
}

/*--------------------------------------------------------------------*\
|- Gestion du preprocesseur pour la traduction						  -|
\*--------------------------------------------------------------------*/

#define NBPREPROC 11
#define NBMAXPROC 16

char GetPreprocValue(char car)
{
char str[1];
str[0]=car;
GetPreprocString(str,1);
return str[0];
}

void GetPreprocString(char *car,int lng)
{
int n=0;
int x;

while ( ((ViewCfg->Traduc[n])!=0) & (n<32))
	{
	switch(ViewCfg->Traduc[n])
		{
		case 1: //--- XOR ----------------------------------------------
			for(x=0;x<lng;x++)
				(car[x])^=(ViewCfg->Traduc[n+1]);
			break;
		case 2: //--- AND ----------------------------------------------
			for(x=0;x<lng;x++)
				(car[x])&=(ViewCfg->Traduc[n+1]);
			break;
		case 3: //--- OR -----------------------------------------------
			for(x=0;x<lng;x++)
				(car[x])|=(ViewCfg->Traduc[n+1]);
			break;
		case 4: //--- NEG ----------------------------------------------
			for(x=0;x<lng;x++)
				(car[x])=(char)(-(car[x]));
			break;
		case 5: //--- ROR ----------------------------------------------
			for(x=0;x<lng;x++)
				car[x]=ROR(car[x],ViewCfg->Traduc[n+1]);
			break;
		case 6: //--- ADD ----------------------------------------------
			for(x=0;x<lng;x++)
				(car[x])+=ViewCfg->Traduc[n+1];
			break;
		case 7: //--- Latin --------------------------------------------
			for(x=0;x<lng;x++)
				car[x]=TabDosWin[car[x]];
			break;
		case 8: //--- BBS ----------------------------------------------
			for(x=0;x<lng;x++)
				car[x]=TabBBS[car[x]];
			break;
		case 9: //--- Elite --------------------------------------------
			for(x=0;x<lng;x++)
				{
				car[x]=(char)(toupper(car[x]));
				switch(car[x])
					{
					case 'A':
					case 'E':
					case 'I':
					case 'O':
					case 'U':
					case 'Y':
						(car[x])+='a'-'A';
						break;
					}
				}
			break;
		case 10: //--- DOS->KKC font -----------------------------------
			if (Cfg->font==0)
				break;
			for(x=0;x<lng;x++)
				car[x]=TabInt[car[x]];
			break;
		case 11: //--- Win->dos -----------------------------------------
			for(x=0;x<lng;x++)
				car[x]=TabWinDos[car[x]];
			break;
		}
	n+=2;
	}
return;
}

/*--------------------------------------------------------------------*\
|-	Taille: 40 maximum												  -|
\*--------------------------------------------------------------------*/

void GetNamePreproc(char n,char opt,char *chaine)
{
switch(n)
	{
	case 1:
		sprintf(chaine,"XOR %3d (0x%02X)",opt,opt);
		break;
	case 2:
		sprintf(chaine,"AND %3d (0x%02X)",opt,opt);
		break;
	case 3:
		sprintf(chaine,"OR  %3d (0x%02X)",opt,opt);
		break;
	case 4:
		sprintf(chaine,"NEG");
		break;
	case 5:
		sprintf(chaine,"ROR %3d (0x%02X)",opt,opt);
		break;
	case 6:
		sprintf(chaine,"ADD %3d (0x%02X)",opt,opt);
		break;
	case 7:
		sprintf(chaine,"Latin");
		break;
	case 8:
		sprintf(chaine,"BBS");
		break;
	case 9:
		sprintf(chaine,"Elite");
		break;
	case 10:
		sprintf(chaine,"Internal Filter");
		break;
	case 11:
		sprintf(chaine,"Windows -> Dos");
		break;
	}
}

int Add1Fct(struct barmenu *bar)
{
int n;

if (bar->fct<NBMAXPROC*2)
	{
	n=bar->fct;
	(ViewCfg->Traduc[n])++;
	GetNamePreproc(ViewCfg->Traduc[n-1],ViewCfg->Traduc[n],bar->Titre);
	}
return 0;
}

int Add10Fct(struct barmenu *bar)
{
int n;

if (bar->fct<NBMAXPROC*2)
	{
	n=bar->fct;
	(ViewCfg->Traduc[n])+=0x10;
	GetNamePreproc(ViewCfg->Traduc[n-1],ViewCfg->Traduc[n],bar->Titre);
	}
return 0;
}

int Sub1Fct(struct barmenu *bar)
{
int n;

if (bar->fct<NBMAXPROC*2)
	{
	n=bar->fct;
	(ViewCfg->Traduc[n])--;
	GetNamePreproc(ViewCfg->Traduc[n-1],ViewCfg->Traduc[n],bar->Titre);
	}
return 0;
}

int Sub10Fct(struct barmenu *bar)
{
int n;

if (bar->fct<NBMAXPROC*2)
	{
	n=bar->fct;
	(ViewCfg->Traduc[n])-=0x10;
	GetNamePreproc(ViewCfg->Traduc[n-1],ViewCfg->Traduc[n],bar->Titre);
	}
return 0;
}


void ChgViewPreproc(KKFile &Fic)
{
KKVIEW *V;
int nbr;
static struct barmenu bar[NBPREPROC+NBMAXPROC+1];
int retour,max,x,y,m,n;
MENU menu;

V=ViewCfg;

for(n=0;n<NBPREPROC+NBMAXPROC+1;n++)
	bar[n].Titre=(char*)GetMem(40);

menu.attr=2;	//--- No shortcut --------------------------------------
menu.cur=0;

NewEvents(Sub1Fct, "Sub 01",5);
NewEvents(Add1Fct, "Add 01",6);

NewEvents(Sub10Fct,"Sub 10",7);
NewEvents(Add10Fct,"Add 10",8);


do
{
n=0;
nbr=0;

while (((V->Traduc[n])!=0) & (nbr<NBMAXPROC))
	{
	GetNamePreproc(V->Traduc[n],V->Traduc[n+1],bar[nbr].Titre);
	bar[nbr].Help=0;
	bar[nbr].fct=n+1;	//--- Emplacement de l'option ------------------
	n+=2;
	nbr++;
	}

bar[nbr].fct=0; //-- Separateur ----------------------------------------
nbr++;

for(x=0;x<NBPREPROC;x++)
	{
	bar[nbr].fct=NBMAXPROC*2+x+1;
	GetNamePreproc((char)(x+1),0,bar[nbr].Titre);
	nbr++;
	}

/*-------- Redimension de la fenˆtre ---------*/
max=0;
for(n=0;n<nbr;n++)
	if (strlen(bar[n].Titre)>max)
		max=strlen(bar[n].Titre);

x=((Cfg->TailleX)-max)/2;
y=((Cfg->TailleY)-nbr)/2;
if (y<2) y=2;

menu.x=x;
menu.y=y;

if (bar[menu.cur].fct==0)
	menu.cur++;

retour=PannelMenu(bar,nbr,&menu);

m=menu.cur;

if (retour==-1) //--- En haut ------------------------------------------
	{
	if (bar[m].fct>NBMAXPROC*2) //--- Nouveau --------------------------
		{
		n=0;
		while (((V->Traduc[n])!=0) & (n<NBMAXPROC*2))
			n+=2;
		if (n<NBMAXPROC*2)
			{
			V->Traduc[n]=(char)((bar[m].fct)-NBMAXPROC*2);
			V->Traduc[n+1]=0;
			if (n<30)
				V->Traduc[n+2]=0;
			menu.cur=n/2;
			}
		}
		else
	if (bar[m].fct!=0) //--- Deplacement -------------------------------
		{
		char t1,t2;
		int pos;

		pos=bar[m].fct-1;
		if (pos!=0)
			{
			t1=V->Traduc[pos];
			t2=V->Traduc[pos+1];

			V->Traduc[pos]=V->Traduc[pos-2];
			V->Traduc[pos+1]=V->Traduc[pos-1];

			V->Traduc[pos-2]=t1;
			V->Traduc[pos-1]=t2;
			menu.cur--;
			}
		}
	}

if (retour==1) //--- En bas --------------------------------------------
	{
	if (bar[m].fct<NBMAXPROC*2)
		{
		char t1,t2;
		int pos;

		pos=bar[m].fct-1;

		if (pos<30)
			{
			t1=V->Traduc[pos+2];
			t2=V->Traduc[pos+3];

			V->Traduc[pos+2]=V->Traduc[pos];
			V->Traduc[pos+3]=V->Traduc[pos+1];
			}
			else
			{
			t1=0;
			t2=0;
			}

		V->Traduc[pos]=t1;
		V->Traduc[pos+1]=t2;

		menu.cur++;
		}
	}
}
while ((retour!=2) & (retour!=0));

ClearEvents();

for(n=0;n<NBPREPROC+NBMAXPROC+1;n++)
	LibMem(bar[n].Titre);

Fic.ReadBlock();
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

class HtmLine
{
public:
	HtmLine(int xl,int yl);
	~HtmLine();

	void AddChar(char car);

	void AddCol(char a);
	void SubCol(void);

	void AddAli(char a);
	void SubAli(void);

	void AddLine(void);

	void AddAttr(int attr,int value);

	void AddLink(char *link);
	void EndLink(void);

	void Aff(void);

	struct Href *prem;	 //--- Le premier link -------------------------
	struct Href *dern;	 //--- Le dernier link -------------------------

	struct HLine *FLine; //--- La toute premiere ligne -----------------

	int yp; //--- Numero de la ligne courante --------------------------

	void AddList(char x,char y);
	void SubList(void);


	int pre;

	int nlist;	   //--- nombre de liste -------------------------------
	char listn[16]; //--- position dans liste --------------------------
	char listt[16]; //--- type de liste --------------------------------

	struct HLine *CLine; //--- La ligne courrante ----------------------

protected:

	int nbrcol;
	char tabcol[16];

	int nbrali;
	int tabali[16]; 	//-- 2 centre

	int xpage,ypage;

	int ahref;

	int bold,ital,unde;

	struct Href *suiv,*rete;

	char motc[256],mot[256];
	int smot;	//--- Position dans motc et mot ------------------------

	char derspace;
};

HtmLine::HtmLine(int xl,int yl)
{
xpage=xl;
ypage=yl;

ital=0;
bold=0;
unde=0;
pre=0;

nbrcol=0;
tabcol[0]=Cfg->col[16];

nbrali=0;
tabali[0]=0;

ahref=0;

smot=0;

nlist=0;

yp=0;

prem=(struct Href*)GetMem(sizeof(struct Href));
suiv=prem;

suiv->next=NULL;
suiv->from=NULL;

FLine=HtmAlloc();
CLine=FLine;

derspace=0;
}

HtmLine::~HtmLine()
{
ital=0;
bold=0;
unde=0;
}

void HtmLine::AddList(char x,char y)
{
listt[nlist]=x;
listn[nlist]=y;
nlist++;
}

void HtmLine::SubList(void)
{
listt[nlist]=0;
listn[nlist]=0;
nlist--;
if (nlist<0)
	nlist=0;
}


void HtmLine::AddAttr(int attr,int value)
{
switch(attr)
	{
	case 1: // ital
		ital+=value;
		if (ital<0) ital=0;
		break;
	case 2: // bold
		bold+=value;
		if (bold<0) bold=0;
		break;
	case 3: // unde
		unde+=value;
		if (unde<0) unde=0;
		break;
	case 4: // pre
		pre+=value;
		if (pre<0) pre=0;
		break;
	}
}

void HtmLine::AddLine(void)
{
struct HLine *TLine; //--- Ligne temporaire ----------------------------

if (smot!=0)		//--- Evite les lignes vides -----------------------
	Aff();

smot=0; 	//--- Normally, it's useless

TLine=HtmAlloc();

CLine->next=TLine;
TLine->from=CLine;
TLine->next=NULL;
CLine=TLine;

yp++;
derspace=1; //--- le dernier caractere est un space ------------
}


void HtmLine::Aff(void)
{
struct HLine *TLine; //--- Ligne temporaire ----------------------------
int i,j;

if (ahref!=0)
	{
	EndLink();
	}

if (smot==0)		//--- Evite les lignes vides -----------------------
	return;

j=0;

if (tabali[nbrali]==2)	  //--- centrage du texte ----------------
	j=(xpage-smot)/2;

for(i=0;i<smot;i++)
   {
   CLine->Chr[i+j]=mot[i];
   CLine->Col[i+j]=motc[i];
   }

//GetPreprocString(CLine->Chr,smot);

rete=prem;
while(rete->next!=NULL)
	{
	if (rete->y1==yp) rete->x1+=j;
	if (rete->y2==yp) rete->x2+=j;

	rete=rete->next;
	}

smot=0;

TLine=HtmAlloc();

CLine->next=TLine;
TLine->from=CLine;
TLine->next=NULL;
CLine=TLine;

yp++;
derspace=1; //--- le dernier caractere est un space ------------

/*if (nlist!=0)
	{
	int k;
	k=nlist*2+2;
	for(i=0;i<k;i++)
		{
		mot[i]=32;
		motc[i]=Cfg->col[16];
		}
	smot=k;
	}
*/
}

void HtmLine::AddChar(char car)
{
int i,j;
char col;
struct HLine *TLine; //--- Ligne temporaire ----------------------------

switch(car)
	{
	case 32:
		if ((smot==0) & (pre==0))
			car=0;
		break;
	case 1:
		car=32;
		break;
	case 0:
		break;
	case 10:
		if (pre==0)
			{
			car=13;
			}
		else
			{
			Aff();
			car=0;
			}
		break;
	case 13:
		car=0;
		break;
	default:
		break;
	}

col=tabcol[nbrcol];

if (bold>1) bold=1;
if (ital>1) ital=1;
if (unde>1) unde=1;

if ( (bold<0) | (ital<0) | (unde<0) )
	bold=0,ital=0,unde=0;

if (bold!=0)
	col=(char)((col&240)+((Cfg->col[60])&15));

if (ital!=0)
	col=(char)((col&240)+((Cfg->col[61])&15));

if (unde!=0)
	col=(char)((col&240)+((Cfg->col[62])&15));

if ((car!=0) & (car!=1))
	{
	if ( (pre==0) & (car==13) & (smot==0) )
		car=0;

	if ( (pre==0) & (car==13) )
		car=32;

	if (car!=0)
		{
		mot[smot]=car;
		motc[smot]=col;
		smot++;
		}
	}

if (smot>xpage) //--- le paragraphe d‚passe une ligne ------------------
	{
	char cesure;

	cesure=1;

	j=smot-1;
	while ( (j>=0) & (mot[j]!=32) ) j--;

	if (j<=0)
		{
		j=smot-1;
		cesure=0;
		}

	for(i=0;i<j;i++)
	   {
	   CLine->Chr[i]=mot[i];
	   CLine->Col[i]=motc[i];
	   }

	for(i=j+cesure;i<smot;i++)		  // +1 car on passe l'espace---
		{
		mot[i-j-cesure]=mot[i];
		motc[i-j-cesure]=motc[i];
		}

	smot-=(j+cesure);

	TLine=HtmAlloc();

	CLine->next=TLine;
	TLine->from=CLine;
	TLine->next=NULL;
	CLine=TLine;

	yp++;

	if (nlist!=0)
		{
		int k;
		k=nlist*2+2;
		for(i=smot+k-1;i>=k;i--)
			{
			mot[i]=mot[i-k];
			motc[i]=motc[i-k];
			}
		for(i=0;i<k;i++)
			{
			mot[i]=32;
			motc[i]=Cfg->col[16];
			}
		smot+=k;
		}
	}

//--- Quelqu'un a demand‚ un passage … la ligne --------------------


//	  if (yp>3998)		  break;


}

void HtmLine::AddLink(char *link)
{
ahref++;

suiv->link=(char*)GetMem(strlen(link)+1);
memcpy(suiv->link,link,strlen(link)+1);

suiv->x1=smot;
suiv->y1=yp;
}

void HtmLine::AddCol(char a)
{
nbrcol++;
if (nbrcol==16) nbrcol=15;
tabcol[nbrcol]=a;
}

void HtmLine::SubCol(void)
{
nbrcol--;
if (nbrcol<0)
	{
	nbrcol=0;
	tabcol[0]=Cfg->col[16];
	}
}

void HtmLine::AddAli(char a)
{
nbrali++;
if (nbrali==16) nbrali=15;
tabali[nbrali]=a;
}

void HtmLine::SubAli(void)
{
nbrali--;
if (nbrali<0)
	{
	nbrali=0;
	tabali[0]=0;
	}
}


void HtmLine::EndLink(void)
{
ahref--;
if (ahref<0)
	{
	ahref=0;
	}
else
	{
	SubCol();

	dern=suiv;

	suiv->next=(struct Href*)GetMem(sizeof(struct Href));

	suiv->x2=smot;	  //--- REFERENCE DE 1 DE LARGE --------------------
	suiv->y2=yp;

	suiv->next->from=suiv;

	suiv=suiv->next;
	suiv->next=NULL;

	}
}


char ReadAlign(char *titre);



int HtmlView(KKFile &Fic,int &debpos)
{
char liaison[256];
char *numam;

char finview;

char split;
int ysplit;
struct HLine *SplitLine;


struct Href *rete,*suiv;

int cv;

char titre[256],*titre2;
int lentit;
long posd,taille2;

char chaine[256];

char bracket;

int smot;		//--- Taille du mot ------------------------------------

long lu;

long debut; 	//--- position du < ------------------------------------
long code;		//--- position du & ------------------------------------

char prev;		//--- vaut 1 si la derniere commande est enter ---------

//int yp;		  //--- position virtuelle sur ecran ---------------------
int xl,yl,ye;
int ye0;		//--- Premiere ligne au lancement du viewer ------------
int x,y;

char car;

char psuiv; 	//--- vaut 1 si on passe le suivant --------------------

int i,j,k,l;	//--- Compteur -----------------------------------------

int fin=0;

int xpage,ypage;

#ifndef NOMOUSE
int xm,ym;
#endif
int zm;   //--- Gestion souris -----------------------------------------

int ix,iy;

int shift=0;	//--- Vaut -1 si rebufchr de l'ecran ----------------
				//---		0 si pas de shift --------------------------
				//---		1 si touche shiftee ------------------------

int debnext;

int derspace;	//--- Vaut 1 si le dernier caractere ‚tait un space ----

char tag[32];

RB_IDF Info;

SaveScreen();

StartWinView(Fic.namebuf,"HTML Viewer");

Fic.SavePosition(ViewCfg->viewhist,1); //- Pour l'historique aprŠs -

PutCur(3,0);

x=1;
y=1;

xl=Cfg->TailleX-2;

yl=Cfg->TailleY-2;


//----------------------------------------------------------------------
//--- Ajout pour l'aide

if (ViewCfg->ajustview)
	xpage=80-2-14;
else
	xpage=(short)(Cfg->TailleX-2);


x=(Cfg->TailleX-xpage)/2;
xl=x+xpage-1;

ypage=Cfg->TailleY-2;

y=(Cfg->TailleY-ypage)/2;
yl=y+ypage-1;

(Fic.posn)=debpos;

Fic.getname(liaison);

numam=strrchr(liaison,'#');
if (numam!=NULL)
	strcpy(liaison,numam+1);
else
	liaison[0]=0;

if ((liaison[0]!=0) & (debpos==0))
	{
	char aname[256];
	sprintf(aname,"<A NAME=\"%s\">",liaison);

//	  PrintAt(0,0,"(%s)",liaison); Wait(0,0);

	while((Fic.posn)<(Fic.taille))
		{
		char car;
		char buffer[256];

		car=Fic.ReadChar();

		if(car=='<')
			{
			int n,m;
			char car;

			n=(Fic.posn);
			m=0;

			do
				{
				car=Fic.ReadChar();
				buffer[m]=car;
				m++;
				(Fic.posn)++;
				if (m>=255) break;
				}
			while(car!='>');
			buffer[m]=0;

			if (!stricmp(aname,buffer))
				break;

			(Fic.posn)=n;
			}
		(Fic.posn)++;
		}
	}

if ((Fic.posn)>=(Fic.taille))
	return -1;


//----------------------------------------------------------------------

HtmLine Line(xpage,ypage);


//----------------------------------------------------------------------

ye=0;  //--- Initialisation de la premiere ligne -----------------------
ye0=0; //--- On commence par afficher la premiere ligne ----------------

debut=0;
code=0;

prev=1;

//yp=0;

smot=0;

lu=0;

lentit=0;

psuiv=0;

finview=0;	//--- Stop the view of the file ----------------------------

derspace=1; //--- le dernier caractere est un space --------------))))))

split=0;

while((Fic.posn)<(Fic.taille))
{
do
	{
	if ((Fic.posn)>=(Fic.taille))
		{
		Line.Aff();
		break;
		}
	car=Fic.ReadChar();
	(Fic.posn)++;
	if ((Line.pre)!=0) break;

	if (car==13)
		{
		if (InterWinView(Fic.posn,Fic.taille))
			(Fic.posn)=(Fic.taille);
		car=32;
		}

	if ((car==10) | (car==9)) car=32;
	if (car!=32) break;
	}
while(derspace);

if (car!=32)
	derspace=0;
	else
	derspace=1;

if ( (debut==0) & (code==0) )
switch(car)
	{
	case '<':
		bracket=1;
		debut=1;
		car=0;
		break;

	case '&':
		memset(titre,0,128);
		code=1;
		car=0;
		break;

	case 9:
		k=smot/8;
		k=k*8+8;
		k-=smot;

		memset(titre,32,k);
		lentit=k;
		break;

	case ';':
		if (psuiv==1)
			{
			psuiv=0;
			car=0;
			}

	default:
		break;
	}

if (debut!=0)		//--- Code <...> -----------------------------------
switch(car)
	{
	case '>':
		bracket--;
		if (bracket!=0)
			{
			lentit=0;
			car=0;
			break;
			}

		if (debut<256)
			{
			int n;

			titre[debut-1]=0;

			memcpy(tag,titre,32);
			tag[31]=0;
			for(n=0;n<32;n++)
				{
				char c;
				c=(char)toupper(tag[n]);

				if ( ((c<'A')|(c>'Z')) & (c!='/') & ((c<'0')|(c>'9')) )
					tag[n]=0;
					else
					tag[n]=c;
				}

			if (!strcmp(tag,"TITLE"))
				{
				Line.AddAli(2);
				Line.AddCol(Cfg->col[35]);
				Line.Aff();
				}
			if (!strcmp(tag,"/TITLE"))
				{
				Line.SubAli();
				Line.SubCol();
				Line.Aff();
				}

			if (!strcmp(tag,"H"))
				{
				Line.AddAli(ReadAlign(titre));
				}

			if (!strcmp(tag,"/H"))
				{
				Line.SubAli();
				}

			if (!strcmp(tag,"H1"))
				{
				Line.AddCol(Cfg->col[36]);
				Line.Aff();
				}
			if (!strcmp(tag,"H2"))
				{
				Line.AddCol(Cfg->col[50]);
				Line.Aff();
				}
			if (!strcmp(tag,"H3"))
				{
				Line.AddCol(Cfg->col[51]);
				Line.Aff();
				}
			if (!strcmp(tag,"H4"))
				{
				Line.AddCol(Cfg->col[57]);
				Line.Aff();
				}
			if (!strcmp(tag,"H5"))
				{
				Line.AddCol(Cfg->col[58]);
				Line.Aff();
				}
			if (!strcmp(tag,"H6"))
				{
				Line.AddCol(Cfg->col[59]);
				Line.Aff();
				}

			if (!strcmp(tag,"STRONG"))
				Line.AddAttr(2,1);

			if (!strcmp(tag,"B"))
				Line.AddAttr(2,1);

			if (!strcmp(tag,"EM"))
				Line.AddAttr(1,1);

			if (!strcmp(tag,"I"))
				Line.AddAttr(1,1);

			if (!strcmp(tag,"U"))
				Line.AddAttr(3,1);

			if (!strcmp(tag,"KK"))
				{
				if (!strnicmp(titre+3,"END",3))
					finview=1;

				if (!strnicmp(titre+3,"SPLIT",5))
					{
					Line.Aff();
					split=1;
					ysplit=Line.yp;
					SplitLine=Line.CLine;
					}
				}

			if (!strcmp(tag,"A"))
				{
				if (!strnicmp(titre+2,"HREF",4))
					{
					Line.AddCol(Cfg->col[17]);
					Line.AddLink(titre);
					}

				if (!strnicmp(titre+2,"NAME",4))
					{
					titre[strlen(titre)-1]=0;

					if (!stricmp(titre+8,liaison))
						ye0=Line.yp;
					}
				}

			if (!strcmp(tag,"/A"))
				Line.EndLink();

			if (!strcmp(tag,"IMG"))
				{
				char *alt,*src;
				char buffer[256];

				strncpy(buffer,titre,255);
				strupr(buffer);

				titre[0]=0;

				alt=strstr(buffer,"ALT");
				if (alt!=NULL)
					{
					alt=strchr(alt,'\"');
					if (alt!=NULL)
						{
						strncpy(titre,alt+1,255);
						alt=strchr(titre,'\"');
						if (alt!=NULL)
							alt[0]=0;
						}
					}

				src=strstr(buffer,"SRC");
				if ((src!=NULL) & (alt==NULL))
					{
					src=strchr(src,'\"');
					if (src!=NULL)
						{
						strncpy(titre,src+1,255);
						src=strchr(titre,'\"');
						if (src!=NULL)
							src[0]=0;
						src=titre;

						if (!strcmp(src,"INTERNAL-GOPHER-IMAGE"))
							strcpy(titre,"\03"); else
						if (!strcmp(src,"INTERNAL-GOPHER-UNKNOWN"))
							strcpy(titre,"?"); else
						if (!strcmp(src,"INTERNAL-GOPHER-TEXT"))
							strcpy(titre,"#"); else
						if (!strcmp(src,"INTERNAL-GOPHER-SOUND"))
							strcpy(titre,"\0E");
							else
							titre[0]=0;
						}
					}

				if (titre[0]==0)
					strcpy(titre,"[IMAGE]");

				lentit=strlen(titre);
				}

			if (!strcmp(tag,"/STRONG"))
				Line.AddAttr(2,-1);

			if (!strcmp(tag,"/B"))
				Line.AddAttr(2,-1);

			if (!strcmp(tag,"/EM"))
				Line.AddAttr(1,-1);

			if (!strcmp(tag,"/I"))
				Line.AddAttr(1,-1);

			if (!strcmp(tag,"/U"))
				Line.AddAttr(3,-1);

			if (!strcmp(tag,"/H1"))
				{
				Line.SubCol();
				Line.Aff();
				}
			if (!strcmp(tag,"/H2"))
				{
				Line.SubCol();
				Line.Aff();
				}
			if (!strcmp(tag,"/H3"))
				{
				Line.SubCol();
				Line.Aff();
				}
			if (!strcmp(tag,"/H4"))
				{
				Line.SubCol();
				Line.Aff();
				}
			if (!strcmp(tag,"/H5"))
				{
				Line.SubCol();
				Line.Aff();
				}
			if (!strcmp(tag,"/H6"))
				{
				Line.SubCol();
				Line.Aff();
				}

			if (!strcmp(tag,"LI"))
				{
				if (Line.nlist!=0)
					{
					switch (Line.listt[Line.nlist-1])
						{
						case 1:
							Line.Aff();
							for(k=0;k<Line.nlist*2;k++)
								Line.AddChar(1);
							sprintf(titre,"%c ",7);
							break;
						case 2:
							Line.Aff();
							for(k=0;k<Line.nlist*2;k++)
								Line.AddChar(1);
							sprintf(titre,"%02d) ",Line.listn[Line.nlist-1]);
							break;
						}
					Line.listn[Line.nlist-1]++;
					lentit=strlen(titre);
					}
					else
					{
					Line.Aff();
					for(k=0;k<Line.nlist*2;k++)
						Line.AddChar(1);
					sprintf(titre,"%c ",7);
					lentit=strlen(titre);
					}
				}
			if (!strcmp(tag,"BR"))
				Line.Aff(); 	 //--- C‚sure forc‚e -------------------

			if (!strcmp(tag,"P"))  //--- Debut de paragraphe -----------
				{
				Line.Aff();
				Line.AddAli(ReadAlign(titre));
				Line.AddLine();
				}
			if (!strcmp(tag,"CENTER"))  //--- Paragraphe centr‚ --------
				{
				Line.Aff();
				Line.AddAli(2); 			//--- On reprend l'ancien --
				}

			if (!strcmp(tag,"/CENTER"))  //--- Paragraphe centr‚ ----
				{
				Line.Aff();
				Line.SubAli();
				}

			if (!strcmp(tag,"/P"))
				{
				Line.Aff();
				Line.SubAli();
				Line.AddLine();
				}

			if (!strcmp(tag,"OL"))
				{
				Line.AddList(2,1);
				Line.Aff();
				}

			if ((!strcmp(tag,"/OL")) & (Line.nlist!=0))
				{
				Line.SubList();
				Line.Aff();
				}

			if (!strcmp(tag,"UL"))
				{
				Line.AddList(1,1);
				}

			if ((!strcmp(tag,"/UL")) & (Line.nlist!=0))
				{
				Line.SubList();
				Line.Aff();
				}

			if (!strcmp(tag,"MENU"))
				{
				Line.AddList(1,1);
				}
			if ((!strcmp(tag,"/MENU")) & (Line.nlist!=0))
				{
				Line.SubList();
				Line.Aff();
				}

			if (!strcmp(tag,"DL"))
				Line.Aff();
			if (!strcmp(tag,"DT"))
				Line.Aff();
			if (!strcmp(tag,"DD"))
				Line.Aff();
			if (!strcmp(tag,"/DL"))
				Line.Aff();
			if (!strcmp(tag,"/DT"))
				Line.Aff();
			if (!strcmp(tag,"/DD"))
				Line.Aff();

			if (!strcmp(tag,"TR"))
				{
				Line.AddAli(ReadAlign(titre));
				Line.Aff();
				}
			if (!strcmp(tag,"TD"))
				{
				Line.AddAli(ReadAlign(titre));
				Line.Aff();
				}
			if (!strcmp(tag,"TH"))
				{
				Line.AddAli(ReadAlign(titre));
				Line.Aff();
				}

			if (!strcmp(tag,"/TR"))
				Line.SubAli();

			if (!strcmp(tag,"/TD"))
				Line.SubAli();

			if (!strcmp(tag,"/TH"))
				Line.SubAli();

/*			  if (!strcmp(tag,"I"))
				Line.Aff(); */

/*			  if (!strcmp(tag,"/I"))  //--- Chasse fixe ------------------
				Line.Aff();*/

			if (!strcmp(tag,"PRE"))
				Line.AddAttr(4,1);

			if (!strcmp(tag,"/PRE"))
				Line.AddAttr(4,-1);

			if (!strcmp(tag,"HR"))
				{
				Line.Aff();
				for(i=0;i<xpage;i++)
					Line.AddChar(0xc4);
				}
			}
		debut=0;
		car=0;
		break;
	case 0:
		break;
	case '<':
		bracket++;
		lentit=0;
		car=0;

/*
		titre[debut-1]=0;
		sprintf(chaine,"<%s%c",titre,car);
		memcpy(titre,chaine,255);
		lentit=strlen(titre);
		debut=0;
*/

		break;
	default:
		titre[debut-1]=car;
		lentit=0;
		debut++;
		car=0;
		break;
	}

if (code!=0)	// code &...;
	{
	if (psuiv!=0)
		{
		psuiv--;
		car=0;
		}

	switch(car)  {
		case 0:
			break;
		case 32:
			titre[code-1]=0;
			sprintf(chaine,"&%s ",titre);
			memcpy(titre,chaine,255);
			lentit=strlen(titre);
			code=0;
			break;
		case ';':
			car=0;
			if (titre[0]=='#')
				{
				int n;
				sscanf(titre+1,"%d",&n);

				car=(char)n;

				switch(n)
					{
					case 146:  car=39; break;
					case 171:  car=34; break;
					case 187:  car=34; break;

					case 177:  lentit=3;  strcpy(titre,"+/-"); break;
					case 133:  lentit=3;  strcpy(titre,"..."); break;
					case 153:  lentit=2;  strcpy(titre,"TM"); break;
					case 167:  lentit=3;  strcpy(titre,"(s)"); break;
					case 169:  lentit=3;  strcpy(titre,"(c)"); break;
					case 174:  lentit=3;  strcpy(titre,"(R)"); break;
					case 176:
					case 186:  car='ø'; break;
					}

				code=0;

/*				  if (lentit!=0)
					GetPreprocString(titre,lentit);
					else
					car=GetPreprocValue(car);
*/
				}
			code=0;

			break;
		default:
			titre[code-1]=car;

			car=0;
			lentit=0;
			code++;

			if (code>128) code=0;

			if (!strnicmp(titre,"EGRAVE",6)) psuiv=1,car='Š';
			if (!strnicmp(titre,"EACUTE",6)) psuiv=1,car='‚';

			if (!strnicmp(titre,"AGRAVE",6)) psuiv=1,car='…';
			if (!strnicmp(titre,"AACUTE",6)) psuiv=1,car=' ';

			if (!strnicmp(titre,"IGRAVE",6)) psuiv=1,car='i';
			if (!strnicmp(titre,"IACUTE",6)) psuiv=1,car='i';

			if (!strnicmp(titre,"UGRAVE",6)) psuiv=1,car='u';
			if (!strnicmp(titre,"UACUTE",6)) psuiv=1,car='u';

			if (!strnicmp(titre,"CCEDIL",6)) psuiv=1,car='‡';

			if (!strnicmp(titre,"ECIRC",5)) psuiv=1,car='ˆ';
			if (!strnicmp(titre,"ACIRC",5)) psuiv=1,car='ƒ';
			if (!strnicmp(titre,"ICIRC",5)) psuiv=1,car='Œ';
			if (!strnicmp(titre,"UCIRC",5)) psuiv=1,car='–';
			if (!strnicmp(titre,"OCIRC",5)) psuiv=1,car='“';

			if (!strnicmp(titre,"QUOT",4)) psuiv=1,car=34;

			if (!strnicmp(titre,"NBSP",4)) psuiv=1,car=32;
			if (!strnicmp(titre,"IUML",4)) psuiv=1,car='i';
												   // En attendant mieux
			if (!strnicmp(titre,"COPY",4))
				 {
				 psuiv=1;
				 lentit=3;
				 strcpy(titre,"(C)");
				 }

			if (!strnicmp(titre,"AMP",3)) psuiv=1,car='&';

			if (!strnicmp(titre,"LT",2)) psuiv=1,car='<';
			if (!strnicmp(titre,"GT",2)) psuiv=1,car='>';

			if (psuiv==1)	//--- La valeur est d‚cod‚ correctement ----
				{
				code=0;
/*				  if (lentit!=0)
					GetPreprocString(titre,lentit);
					else
					car=GetPreprocValue(car);*/
				}
			break;
		}
	}



/*--------------------------------------------------------------------*\
|- Gestion de tous les petits machins								  -|
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

if (debut>=256)
	{
	debut=255;

//		  lentit=debut-1;		 debut=0;
	}

if (code>=256)
	{
	lentit=code-1;
	code=0;
	}

if (lentit==0)
	{
	lentit=1;
	chaine[0]=car;
	}
	else
	{
	memcpy(chaine,titre,lentit);
	}

for (k=0;k<lentit;k++)
	{
	Line.AddChar(chaine[k]);
	} //--- fin du for (k=0;k<lentit;k++) ------------------------------

lentit=0;

// if (yp>3998) break;

if (finview)
	break;

}
debnext=(Fic.posn);


CloseWinView();

/*--------------------------------------------------------------------*\
|- Affichage de la page 											  -|
\*--------------------------------------------------------------------*/

WinCadre(x-1,y-1,xl+1,yl,3);
Window(x,y,xl,yl-1,Cfg->col[16]);

ColLin(0,yl+1,Cfg->TailleX,Cfg->col[6]);
ChrLin(0,yl+1,Cfg->TailleX,32);

//----------------------------------------------------------------------

struct HLine *PLine;
struct HLine *FLine;
struct HLine *DLine;


FLine=Line.FLine;

PLine=FLine;
DLine=FLine;

for(i=0;i<yl-y-split;i++) //--- Pr‚pare la derniere ligne … l'‚cran --
	{
	if (DLine->next!=NULL)
		DLine=DLine->next;
	}

for(i=0;i<ye0;i++)		//--- On ne commence pas … la premiere ligne ---
	HTMLnext(&DLine,&PLine,&ye);

code=0;

// yp= limitte end

suiv=Line.prem;


do
{
struct HLine *TLine;

TLine=PLine;

for (i=y;i<yl-(split*2);i++)
	{
	if ((split==1) & (TLine==SplitLine))
		break;

	if (TLine!=NULL)
		{
		for(j=x;j<=xl;j++)
			AffCol(j,i,TLine->Col[j-x]);

		for(j=x;j<=xl;j++)
			AffChr(j,i,TLine->Chr[j-x]);
		TLine=TLine->next;
		}
	}

if (split==1)
	{
	for(j=x;j<=xl;j++)
		{
		AffCol(j,yl-2,Cfg->col[16]);
		AffChr(j,yl-2,196);
		}

	TLine=SplitLine;

	for(j=x;j<=xl;j++)
		AffCol(j,yl-1,TLine->Col[j-x]);

	for(j=x;j<=xl;j++)
		AffChr(j,yl-1,TLine->Chr[j-x]);
	}


//--- Place le link en haut --------------------------------------------
rete=suiv;

do
	{
	if ( (((suiv->y1)>(ye-y)) & ((suiv->y1)<ye+yl-y-(split*2))) |
		 ((split==1) & (suiv->y1>=ysplit)) )  break;

	suiv=suiv->next;

	if (suiv==NULL)
		suiv=Line.prem;

	if (suiv->next==NULL)
		suiv=Line.prem;
	}
while(rete!=suiv);


if ( (suiv->next!=NULL) & (suiv!=NULL) )
	{
	int ye2;

	if ((suiv->y1>=ysplit) & (split==1))
		ye2=(suiv->y1)-yl+1+y;
		else
		ye2=ye;

	ix=suiv->x1;
	iy=suiv->y1-ye2;	// yl-1-y;

	if ((iy>-y) & (iy<yl-y))
		while(1)
			{
			if ( (ix<xpage) & (iy>=0) & (iy<yl-y) )
				{
				if ( (GetHtmChr(ix+x,iy+y)!=32) |
					 ((GetHtmChr(ix+x-1,iy+y)!=32) &
					  (GetHtmChr(ix+x+1,iy+y)!=32)) )

					AffCol(ix+x,iy+y,Cfg->col[18]);
				}

			if (iy>(suiv->y2-ye2))
				break;

			ix++;
			if (ix>=xpage) ix=0,iy++;

			if ( (ix==suiv->x2) & (iy==(suiv->y2-ye2)) )
				break;
			}

	PrintAt(0,yl+1,"%-*s",Cfg->TailleX,suiv->link);
	}


posd=ye;
(Fic.posn)=(ye+yl-y);
taille2=Line.yp;


zm=0;

while ( (!KbHit()) & (zm==0) )
	{
#ifndef NOMOUSE
	GetPosMouse(&xm,&ym,&zm);

	if (iskeypressed(SHIFT))
		{
		long cur1,cur2;
		int prc;
		char temp[132];

		if (shift==0)
			SaveScreen();

		if ((Fic.posn)>taille2)
			(Fic.posn)=taille2;

		if (taille2<1024*1024)
			{
			cur1=(posd)*(Cfg->TailleY-2);
			cur1=cur1/taille2+1;

			cur2=((Fic.posn)-1)*(Cfg->TailleY-2);
			cur2=cur2/taille2+1;

			prc=((Fic.posn)*100)/taille2;
			}
		else
			{
			cur1=(posd/1024)*(Cfg->TailleY-2);
			cur1=cur1/(taille2/1024)+1;

			cur2=(((Fic.posn)-1)/1024)*(Cfg->TailleY-2);
			cur2=cur2/(taille2/1024)+1;

			prc=((Fic.posn)/taille2)*100;
			}

		ColLin(0,0,Cfg->TailleX,Cfg->col[7]);

		Fic.getname(temp);
		ReduceString(temp,52);

		PrintAt(0,0,"View: %-52s %9d bytes %3d%% ",temp,(Fic.taille),prc);

		ColCol(Cfg->TailleX-1,1,Cfg->TailleY-2,Cfg->col[7]);
		ChrCol(Cfg->TailleX-1,1,cur1-1,32);
		ChrCol(Cfg->TailleX-1,cur1,cur2-cur1+1,219);
		ChrCol(Cfg->TailleX-1,cur2+1,Cfg->TailleY-1-cur2,32);

		Bar(
		" Help  ----  ---- ChView ---- Prepro ----  ----  ----  Quit ");

		shift=1;
		}
	else
		{
		if (shift==1)
			{
			shift=-1;
			break;
			}
		}
#endif
	}

if (shift!=-1)
	{
	code=Wait(0,0);

#ifndef NOMOUSE
if (code==0)	 //--- Pression bouton souris --------------------------
	{
	int button;

	button=MouseButton();

	if ((button&1)==1)	   //--- gauche --------------------------------
		{
		int x,y;

		x=MousePosX();
		y=MousePosY();

/*		  if ( (x==Cfg->TailleX-1) & (y>=1) & (y<=Cfg->TailleY-1) )
													  //-- Ascensceur --
			{
			(Fic.posn)=((Fic.taille)*(y-1))/(Cfg->TailleY-2);
			}
			else
*/
			if (y==Cfg->TailleY-1)
				if (Cfg->TailleX==90)
					code=(0x3B+(x/9))*256;
					else
					code=(0x3B+(x/8))*256;
				else
				{
				if (y>(Cfg->TailleY)/2)
					code=80*256;
					else
					code=72*256;
				ReleaseButton();
				}
		}

	if ((button&2)==2)	   //--- droite --------------------------------
		{
		code=27;
		}
	}
#endif

	switch(code)
		{
		case 27:  //--- ESCape -----------------------------------------
			fin=-1;
			break;

		case 0x2500:  // --- ALT-K -------------------------------
			AltK();
			break;

		case 0x5400:  //--- SHIFT-F1 -----------------------------
		case 0x3B00:  //--- F1 -----------------------------------
			HelpTopic(8);
			fin=VIEWER_HTML;
			break;

		case 0x5D00:  //--- SHIFT-F10 ----------------------------
		case 0x4400:  //--- F10 ----------------------------------
		case 0x8D00:  //--- CTRL-UP ------------------------------
			fin=-1;
			break;

		case 0x5700:  //--- SHIFT-F4 -----------------------------
		case 0x3E00:  //--- F4 -----------------------------------
			cv=ChangeViewer(3);
			if (cv!=0)
				fin=cv;
			break;

		case 0x5A00:	//--- SHIFT-F6 ---------------------------
		case 0x4000:	//--- F6 ---------------------------------
			ChgViewPreproc(Fic);
			fin=VIEWER_HTML;
			break;

		case 0x8B00:  // --- ALT-F11 -----------------------------
			fin=VIEWER_AUTO;
			break;

		case 0x8C00:  // --- ALT-F12 -----------------------------
			ViewSetup(Fic);
			fin=VIEWER_HTML;
			break;

		case KEY_DOWN:	  //--- BAS ----------------------------------
			HTMLnext(&DLine,&PLine,&ye);
			break;

		case KEY_UP:	//--- HAUT ---------------------------------
			HTMLfrom(&DLine,&PLine,&ye);
			break;

		case KEY_NPAGE:  //--- PGDN ---------------------------------
			for(l=y;l<yl;l++)
				HTMLnext(&DLine,&PLine,&ye);
			break;

		case KEY_PPAGE:  //--- PGUP ---------------------------------
			for(l=y;l<yl;l++)
				HTMLfrom(&DLine,&PLine,&ye);
			break;

		case KEY_HOME:	//--- HOME ---------------------------------
			while(PLine->from!=NULL)
				HTMLfrom(&DLine,&PLine,&ye);
			break;

		case KEY_END:  //--- END ----------------------------------
			while(DLine->next!=NULL)
				HTMLnext(&DLine,&PLine,&ye);
			break;

		case KEY_LEFT:
		case 0x0F00:   //--- SHIFT-TAB ----------------------------
			if (suiv->next==NULL)
				break;

			rete=suiv;

			do
				{
				suiv=suiv->from;

				if (suiv==NULL)
					suiv=Line.dern;

				if ( (((suiv->y1)>(ye-y)) & ((suiv->y1)<ye+yl-y-(split*2))) |
					 ((split==1) & (suiv->y1>=ysplit)) )
					{
					break;
					}
				}
			while(suiv!=rete);

			iy=suiv->y1-ye;

			if ( (split==0) & ((iy<=(-y)) | (iy>=yl-y-(split*2))) )
				{
				j=abs(ye-(suiv->y1-y));
				for(i=0;i<j;i++)
					{
					if (ye<suiv->y1-y)
						HTMLnext(&DLine,&PLine,&ye);

					if (ye>suiv->y1-y)
						HTMLfrom(&DLine,&PLine,&ye);
					}
				}
			break;

		case KEY_RIGHT:
		case 9:
			if (suiv->next==NULL)
				break;

			rete=suiv;

			do
				{
				suiv=suiv->next;

				if (suiv->next==NULL)
					suiv=Line.prem;

				if ( (((suiv->y1)>(ye-y)) & ((suiv->y1)<ye+yl-y-(split*2))) |
					 ((split==1) & (suiv->y1>=ysplit)) )
					{
					break;
					}
				}
			while(suiv!=rete);

			iy=suiv->y1;

			if ( (split==0) & ((iy<=(ye-y)) | (iy>=ye+yl-y-(split*2))) )
				{
				j=abs(ye-(suiv->y1-y));
				for(i=0;i<j;i++)
					{
					if (ye<suiv->y1-y)
						HTMLnext(&DLine,&PLine,&ye);

					if (ye>suiv->y1-y)
						HTMLfrom(&DLine,&PLine,&ye);
					}
				}
			break;
		case 13:
			strcpy(titre,suiv->link);
			titre2=strchr(titre,'\"');

			if (titre2==NULL)
				break;

			titre2++;
			titre2[strlen(titre2)-1]=0;

			if (!strnicmp(titre2,"MAILTO:",7))
				{
				code=0;
				strcpy(chaine,"You couldn't mail to: ");
				strcat(chaine,titre2+7);
				ReduceString(chaine,Cfg->TailleX-8);
				WinError(chaine);
				break;
				}
			if (!strnicmp(titre2,"HTTP:",5))
				{
				code=0;
				strcpy(chaine,"You couldn't HTTP to: ");
				strcat(chaine,titre2+5);
				ReduceString(chaine,Cfg->TailleX-8);
				WinError(chaine);
				break;
				}
			if (!strnicmp(titre2,"FTP:",4))
				{
				code=0;
				strcpy(chaine,"You couldn't FTP to: ");
				strcat(chaine,titre2+4);
				ReduceString(chaine,Cfg->TailleX-8);
				WinError(chaine);
				break;
				}

			if (titre2[0]=='#')     //--- Internal Link ----------------
				{
				char ficbuf[256];
				char *numam;

				Fic.getname(ficbuf);
				numam=strrchr(ficbuf,'#');
				if (numam!=NULL)
					numam[0]=0;

				strcat(ficbuf,titre2);

				Fic.setname(ficbuf);

				Info.numero=VIEWER_HTML;
				fin=VIEWER_HTML;
				}
			else
				{
				char buffer[256];

				Fic.getname(buffer);
				Path2Abs(buffer,"..");
				Path2Abs(buffer,titre2);
				Fic.setname(buffer);

				fin=-4;
				}
			break;
		case 'n':
			debpos=debnext;
			fin=VIEWER_HTML;
		default:
			break;
		}
	}
else
	{
	shift=0;
	LoadScreen();
	}

}
while (fin==0);

rete=Line.prem;
while(rete->next!=NULL)
	{
	LibMem(rete->link);
	suiv=rete->next;
	LibMem(rete);
	rete=suiv;
	}
LibMem(rete);

while(FLine->next!=NULL)
	{
	struct HLine *TLine; //--- Ligne temporaire ------------------------

	TLine=FLine->next;
	LibMem(FLine->Chr);
	LibMem(FLine->Col);
	LibMem(FLine);
	FLine=TLine;
	}
LibMem(FLine);
LibMem(FLine->Chr);
LibMem(FLine->Col);

Fic.SavePosition(ViewCfg->viewhist,1); //- Pour l'historique aprŠs -

if (fin==VIEWER_AUTO)
	{
	char fic[256];

	GetListFile(fic);
	if (fic[0]!=0)
		Fic.setname(fic);
	}

LoadScreen();

if (fin==-4)
	fin=VIEWER_AUTO;

return fin;
}


// ftp,http 	  rundll32.exe url.dll,FileProtocolHandler %1
// mail 		  rundll32.exe mailnews.dll,Mail_RunDLL
// Eg: rundll32.exe mailnews.dll,Mail_RunDLL mailto:redbug26@yahoo.com

// control pannel rundll32.exe shell32.dll,Control_RunDLL

// sound clip	  rundll32.exe amovie.ocx,RunDll /play /close %1
