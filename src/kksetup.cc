/*--------------------------------------------------------------------*\
|- KKSETUP: Main configuration program                                -|
\*--------------------------------------------------------------------*/
#include <stdarg.h>


#ifndef LINUX
    #include <dos.h>
    #include <direct.h>
    #include <io.h>
    #include <conio.h>
    #include <mem.h>
    #include <bios.h>
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define KDBVER 2

#include "kk.h"
#include "idf.h"
#include "rbdos.h"

#include "language.h"

#define MAXDIR 250
#define MAXBOX 250

char *RBTitle2="Ketchup Killers Setup V"VERSION" / RedBug";

void KKR_Read(FILE *Fic);
void KKR_Search(char *nom2);
void DispDir(char *dir);
void ConfigSetup(void);

extern struct key K[nbrkey];
extern int IOerr;

char PathOfKK[256];
char ActualPath[256];

char subdir=1;
char updatekkr=0;
char areadir[256];      //--- Pour une recherche personnalis‚e ---------

void ModifKey(void);
int ChooseFunction(FUNCTION prem);

char GVerif=0;  // Verification globale: 0, on interroge
                //                       1, toujours oui
                //                       2, toujours non

char LoadDefCfg;

struct kkconfig *KKCfg;
struct kkfichier *KKFics;

#include "savecol.h"

/*--------------------------------------------------------------------*\
|- Gestion macros (fake in kksetup)                                   -|
\*--------------------------------------------------------------------*/

short FctStack[128];
short NbrFunct;


/*--------------------------------------------------------------------*\
|- Fake MenuCreat pour ficidf                                         -|
\*--------------------------------------------------------------------*/
void MenuCreat(char *titbuf,char *buf,char *path)
{
if ((!titbuf) || (!buf) ||(!path));         //--- For the warnings -----
}

/*--------------------------------------------------------------------*\
|- Fake CommandLine pour rbdos                                        -|
\*--------------------------------------------------------------------*/
int CommandLine(char *string,...)
{
if (!string);                               //--- For the warnings -----

return 0;
}


// Variable globale

FUNCTION todo;


/*--------------------------------------------------------------------*\
|- prototype                                                          -|
\*--------------------------------------------------------------------*/

int crc32file(char *name,unsigned long *crc);  // Compute CRC-32 of file
int FileComp(char *a,char *b);   // Comparaison entre 2 noms de fichiers

void InitMode(void);
void ClearSpace(char *name);    //--- efface les espaces inutiles ------

void Interroge(char *path,struct player *app,char *Verif,char *GVerif);

void Col2Str(char *from,char *to);
void Str2Col(char *from,char *to);

void ChangePalette(void);
void ColorChange(void);
void IColor(void);       // InitColor
int DColor(int col);
void CColor(int m);
void SaveCol(void);

void AffColScreen(int a);

struct player
    {
    char *Filename;
    char *Meneur;
    char *Titre;
    unsigned long Checksum;
    short ext;                                     // Numero d'extension
    short pres;             // 0 si pas trouv‚ sinon numero du directory
    char type;
    char os;
    char info;
    short numbox;
    short boxlen;
    char *box;
    } *app[5000];

char dir[MAXDIR][128];      // 50 directory diff‚rents de 128 caracteres

char *kkbox[MAXBOX];
int boxlen[MAXBOX];

short nbr;              // nombre d'application lu dans les fichiers KKR
int nbrdir;
int nbrbox;

long OldY,OldX,PosX,PosY;
char OldPal[48];
char OldFont[256*16];

char *Screen_Adr=(char*)0xB8000;
char *Screen_Buffer;

FENETRE *Fenetre[NBWIN];

void SSearch(char *nom);
void ApplSearch(void);
void ClearAllSpace(char *name);

void GestionFct(FUNCTION i);
FUNCTION GestionBar(void);

struct RB_info *Info;


void KKInit(void)
{
Info=(struct RB_info*)GetMem(sizeof(struct RB_info));   // Starting time

Info->temps=GetClock();
Info->defform=1;
Info->macro=0;
}



/*--------------------------------------------------------------------*
 -                          Gestion Message                           -
 *--------------------------------------------------------------------*/

int MesgY;

/*--------------------------------------------------------------------*
 -                      Initialise les messages                       -
 *--------------------------------------------------------------------*/
void InitMessage(void)
{
MesgY=3;
}

/*--------------------------------------------------------------------*
 -                        Affiche un message                          -
 *--------------------------------------------------------------------*/
void DispMessage(char *string,...)
{
static char sortie[256];
va_list arglist;
char *suite;
int a;

suite=sortie;

va_start(arglist,string);
vsprintf(sortie,string,arglist);
va_end(arglist);

a=2;
while (*suite!=0)
	{
/*
    AffChr(a,MesgY,'Û');
    while ((inp(0x3DA) & 8)==8);
    while ((inp(0x3DA) & 8)!=8);
    while ((inp(0x3DA) & 1)==1);
    while ((inp(0x3DA) & 1)!=1);
*/
    AffChr(a,MesgY,*suite);
	a++;
	suite++;
    GotoXY(a,MesgY);
    PutCur(7,7);
	}

MesgY++;
if (MesgY>(Cfg->TailleY-3))
    {
    MoveText(1,3,Cfg->TailleX-2,Cfg->TailleY-3,1,2);
    MesgY--;
    ChrLin(1,(Cfg->TailleY-3),Cfg->TailleX-2,32);
    }
}


int sort_function(const void *a,const void *b)
{
struct key *a1,*b1;

a1=(struct key*)a;
b1=(struct key*)b;

// return (a1->numero)-(b1->numero);

if (a1->type!=b1->type) return (a1->type)-(b1->type);

return strcmp(a1->ext,b1->ext);                           // ou format ?
}


/*--------------------------------------------------------------------*\
|- Liste des fichiers reconnus par IDF                                -|
\*--------------------------------------------------------------------*/
void IdfListe(void)
{
int car,y;

int n,m;
int info;

int curr,prem;

SaveScreen();
PutCur(32,0);

Bar(" Help  Info  ----  ----  ----  ----  ----  ----  ----  ---- ");

info=0;



y=3;

PrintAt(2,0,"%-77s","List of the format");

prem=0;
curr=1;

do
{
if (curr<0) curr=0;
if (curr>=nbrkey) curr=nbrkey-1;

if (K[curr].ext[0]=='*') curr++;

if (curr<prem+1) prem=curr-1;
while (curr>=prem+(Cfg->TailleY-6)) prem=curr-(Cfg->TailleY-6)+1;


if (prem<0)
    prem=0;
if (prem>nbrkey-(Cfg->TailleY-6))
    prem=nbrkey-(Cfg->TailleY-6);

y=3;

for (n=prem;n<nbrkey;n++)
    {
    m= (n&1==1) ? 16 : 28;  // y

    if (K[n].ext[0]=='*')
        {
        ChrLin(1,y,Cfg->TailleX-2,196);
        PrintAt(4,y,"%s",K[n].format);

        ColLin(1,y,Cfg->TailleX-2,Cfg->col[m]);
        }
        else
        {
        char ext[32];

        ColLin(1,y,4,  Cfg->col[m]);
        ColLin(5,y,1,  Cfg->col[m]);
        ColLin(6,y,32, Cfg->col[m+1]);
        ColLin(38,y,6, Cfg->col[m]);
        ColLin(44,y,29,Cfg->col[m+1]);
        ColLin(73,y,1, Cfg->col[m]);
        ColLin(74,y,5, Cfg->col[m]);
        if (Cfg->TailleX!=80)
            {
            ColLin(79,y,Cfg->TailleX-80,Cfg->col[m]);
            ChrLin(79,y,Cfg->TailleX-80,32);
            }

        strcpy(ext,K[n].ext);
        ext[3]=0;

        PrintAt(1,y," %3s %-32s from %29s %4s ",ext,
            K[n].format,K[n].pro,((K[n].other)&1)==1 ? "Info" : "----");

        if (((K[n].other)&1)==1) info++;

        if (curr==n)
            ColLin(1,y,Cfg->TailleX-2,Cfg->col[30]);
        }

    y++;

    if ( (y==(Cfg->TailleY-3)) | (n==nbrkey-1) ) break;
    }

car=Wait(0,0);



switch(HI(car))
    {
    case 72:                                                       // UP
        if (curr!=0)
            {
            curr--;
            if (K[curr].ext[0]=='*') curr--;
            }
        break;
    case 80:                                                     // DOWN
        curr++;
        break;
    case 0x49:                                                   // PGUP
        curr-=10;
        break;
    case 0x51:                                                   // PGDN
        curr+=10;
        break;
    case 0x47:                                                   // HOME
        curr=0;
        break;
    case 0X4F:                                                    // END
        curr=nbrkey-1;
        break;
    case 0x3B:
        HelpTopic(1);
        break;
    case 0x3C: //--- F2 ------------------------------------------------
        FicIdfMan(curr,NULL);
        break;
    }
}
while (car!=27);

LoadScreen();
}



/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

unsigned long crc32buf(char *buffer,int length);

/*--------------------------------------------------------------------*\
|---- Crc - 32 BIT ANSI X3.66 CRC checksum files ----------------------|
\*--------------------------------------------------------------------*/

static unsigned long int crc_32_tab[] = {   // CRC polynomial 0xedb88320
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

unsigned long crc32buf(char *buffer,int length)
{
unsigned long oldcrc32;
unsigned long crc32;
unsigned long oldcrc;
int n,c;
long charcnt;

oldcrc32 = 0xFFFFFFFF;
charcnt = 0;

for(n=0;n<length;n++)
    {
    c=buffer[n];
    ++charcnt;
    oldcrc32 = (crc_32_tab[(oldcrc32^c) & 0xff]^(oldcrc32 >> 8));
    }

crc32 = oldcrc32;
oldcrc = oldcrc32 = ~oldcrc32;

return oldcrc;
}


static char *cfgbuffer;
static int cfgpos;

void cfgwopen(void);
int cfgwclose(void);                // return 1: ok, 0: error
void cfgwrite(void *,int n);

int cfgropen(void);                 // return 1: ok, 0: error
void cfgrclose(void);
void cfgread(void *,int n);

void cfgwopen(void)
{
cfgbuffer=(char*)GetMem(32768);
cfgpos=0;
}

int cfgwclose(void)
{
FILE *fic;
unsigned long crc;

fic=fopen(KKFics->CfgFile,"wb");
if (fic==NULL)
    {
    LibMem(cfgbuffer);
    return 0;
    }

crc=crc32buf(cfgbuffer,cfgpos);

fprintf(fic,"KCFG");
fwrite(&crc,4,1,fic);
fwrite(&cfgpos,4,1,fic);
fwrite(cfgbuffer,cfgpos,1,fic);
fclose(fic);

LibMem(cfgbuffer);
return 1;
}

void cfgwrite(void *buf,int n)
{
memcpy((void*)(cfgbuffer+cfgpos),buf,n);
cfgpos+=n;
}

int cfgropen(void)
{
FILE *fic;
char buffer[4];
int crc,length;


fic=fopen(KKFics->CfgFile,"rb");
if (fic==NULL) return 0;

fread(buffer,4,1,fic);
if (memcmp(buffer,"KCFG",4)!=0)
    {
    fclose(fic);
    return 0;
    }
fread(&crc,4,1,fic);
fread(&length,4,1,fic);

if (length<32768)
    {
    cfgbuffer=(char*)GetMem(32768);
    fread(cfgbuffer,length,1,fic);
    }
    else
    {
    fclose(fic);
    return 0;
    }

fclose(fic);

if (crc!=crc32buf(cfgbuffer,length))
    {
    LibMem(cfgbuffer);
    return 0;
    }

cfgpos=0;
return 1;
}

void cfgrclose(void)
{
LibMem(cfgbuffer);
}

void cfgread(void *buf,int n)
{
memcpy(buf,(void*)(cfgbuffer+cfgpos),n);
cfgpos+=n;
}



void SaveCfg(void)
{
int m,n,t;
short taille;
FENETRE *Fen;

KKCfg->FenTyp[0]=0;
KKCfg->FenTyp[1]=0;

cfgwopen();
cfgwrite((void*)Cfg,sizeof(struct config));

cfgwrite((void*)KKCfg,sizeof(struct kkconfig));

for(n=0;n<16;n++)
    {
    cfgwrite(&(KKCfg->V.Mask[n]->Ignore_Case),1);
    cfgwrite(&(KKCfg->V.Mask[n]->Other_Col),1);
    taille=(short)strlen(KKCfg->V.Mask[n]->chaine);
    cfgwrite(&taille,2);
    cfgwrite(KKCfg->V.Mask[n]->chaine,taille);
    taille=(short)strlen(KKCfg->V.Mask[n]->title);
    cfgwrite(&taille,2);
    cfgwrite(KKCfg->V.Mask[n]->title,taille);
    }

for(t=0;t<NBWIN;t++)
{
Fen=Fenetre[t];

cfgwrite(Fen->path,256);
cfgwrite(&(Fen->order),sizeof(short));
cfgwrite(&(Fen->sorting),sizeof(short));

cfgwrite(&(Fen->nbrfic),4);

for (n=0;n<=Fen->nbrfic;n++)
    {
    m=strlen(Fen->F[n]->name);
    cfgwrite(&m,4);
    cfgwrite(Fen->F[n]->name,m);
    }

cfgwrite(&(Fen->scur),sizeof(short));
}

cfgwrite(&NbrFunct,2);
if (NbrFunct!=0)
    cfgwrite(FctStack,2*NbrFunct);

cfgwclose();
}



/*--------------------------------------------------------------------*\
|- Chargement de KKRB.CFG                                             -|
|-  Retourne -1 en cas d'erreur                                       -|
|-            0 si tout va bien                                       -|
\*--------------------------------------------------------------------*/
int LoadCfgSetup(void)
{
int i,t,n,m;
short taille;
char nom[256];
FENETRE *DFen;
struct PourMask **Mask;
char *viewhist;

if (cfgropen()==0)
    return -1;

cfgread((void*)Cfg,sizeof(struct config));

Mask=KKCfg->V.Mask;
viewhist=KKCfg->V.viewhist;
cfgread((void*)KKCfg,sizeof(struct kkconfig));
KKCfg->V.Mask=Mask;
KKCfg->V.viewhist=viewhist;

for(n=0;n<16;n++)
    {
    cfgread(&(KKCfg->V.Mask[n]->Ignore_Case),1);
    cfgread(&(KKCfg->V.Mask[n]->Other_Col),1);
    cfgread(&taille,2);
    cfgread(KKCfg->V.Mask[n]->chaine,taille);
    KKCfg->V.Mask[n]->chaine[taille]=0;
    cfgread(&taille,2);
    cfgread(KKCfg->V.Mask[n]->title,taille);
    KKCfg->V.Mask[n]->title[taille]=0;
    }

for (t=0;t<NBWIN;t++)
    {
    DFen=Fenetre[t];

    DFen->FenTyp=KKCfg->FenTyp[t];

    cfgread(DFen->path,256);
    cfgread(&(DFen->order),sizeof(short));
    cfgread(&(DFen->sorting),sizeof(short));

    cfgread(&(DFen->nbrfic),4);

    for (i=0;i<=DFen->nbrfic;i++)
        {
        DFen->F[i]=(file*)GetMem(sizeof(struct file));

        cfgread(&m,4);
        cfgread(nom,m);
        nom[m]=0;

        DFen->F[i]->name=(char*)GetMem(m+1);
        memcpy(DFen->F[i]->name,nom,m+1);
        }

    cfgread(&(DFen->scur),sizeof(short));
    }

cfgread(&NbrFunct,2);
if (NbrFunct!=0)
    cfgread(FctStack,2*NbrFunct);

cfgrclose();

return 0;
}


void ListDrive(char *lstdrv)
{
char drive[26],etat[26];
int n,nbr;
signed char i;

static int l[26];
static char x1=52;
static int y1=6;
static int l1;

struct Tmt T[36]= {
    {3, 9,10, "Accept all application",&l1},
    {3,10,10,"Accept only player with CRC",&l1},
    {3,11,10,"Ask everytime",&l1},
    {23,11,2,NULL,NULL},                                      // le OK
    {41,11,3,NULL,NULL},                                 // le CANCEL
    {2,1,9,&x1,&y1},
    {3,2,0,"Select path to search:",NULL}
    };

struct TmtWin F = {-1,5,58,18,"Configuration"};

switch(GVerif)
    {
    case 0:        l1=2;        break;
    case 1:        l1=0;        break;
    case 2:        l1=1;        break;
    }

n=7;
nbr=0;
for (i=0;i<26;i++)
    {
    etat[i]=0;
    drive[i]=(char)DriveExist(i);

    if (drive[i])
        {
        if (i>=2)
            l[i]=(DriveReady(i)==1);
            else
            l[i]=0;


        T[n+nbr].x=(i%6)*9+3;
        T[n+nbr].y=i/6+3;
        T[n+nbr].type=8;
        T[n+nbr].str=(char*)GetMem(4);
        sprintf(T[n+nbr].str,"%c:\\",'C'+i-2);
        T[n+nbr].entier=&(l[i]);
        nbr++;
        }
    }

DriveSet(Fics->LastDir);

n=WinTraite(T,7+nbr,&F,0);

if (n==-1)
    {
    for (n=0;n<26;n++)
        lstdrv[n]=0;
    }
    else
    {
    switch(l1)
        {
        case 0:            GVerif=1;            break;
        case 1:            GVerif=2;            break;
        case 2:            GVerif=0;            break;
        }
    for (n=0;n<26;n++)
        lstdrv[n]=(char)(l[n]);
    }

n=7;
for(i=0;i<nbr;i++,n++)
    LibMem(T[n].str);

}



void PutInPath(void)
{
MENU menu;
static struct barmenu dir[MAXDIR];
char *TPath;
static char path[2048];
char erreur;
FILE *fic;

int i,j,k;


k=Cfg->TailleY-6;

DispMessage("Select the directory where you will that KKSETUP"
                                           " copy the batch files for");
DispMessage("the execution of KK and KKDESC. (ESC for cancel)");
DispMessage("");

erreur=1;

TPath=getenv("PATH");
strcpy(path,TPath);
k=strlen(path);

for(i=0;i<k;i++)
    if (path[i]==';')
        path[i]=0;

j=0;
for (i=0;i<50;i++)
    {
    dir[i].Titre=path+j;
    dir[i].Help=0;
    dir[i].fct=i+1;
    dir[i].Titre=strupr(dir[i].Titre);
    if (strlen(dir[i].Titre)==0) break;
    while ( (j!=k) & (path[j]!=0) ) j++;
    j++;
    }

menu.x=2;
menu.y=9;

menu.attr=2+8;

menu.cur=0;

if (i!=0)
    {
    if (PannelMenu(dir,i,&menu)==2)
        {
        strcpy(path,dir[menu.cur].Titre);
        Path2Abs(path,"kk.bat");
        fic=fopen(path,"wt");
        if (fic!=NULL)
            {
            fprintf(fic,"@%s\\kk.exe\n",ActualPath);
            fprintf(fic,"@REM This file was making by KKSETUP\n");
            fclose(fic);

            DispMessage("%s is done",path);
            DispMessage("");

            Path2Abs(path,"..\\kkdesc.bat");
            fic=fopen(path,"wt");
            if (fic!=NULL)
                {
                fprintf(fic,"@%s\\kkdesc.exe %%1 %%2 %%3\n",ActualPath);
                fprintf(fic,"@REM This file was making by KKSETUP\n");
                fclose(fic);
                erreur=0;

                strcpy(PathOfKK,dir[menu.cur].Titre);

                DispMessage("%s is done",path);
                DispMessage("");
                }
            }

        }
    }

if (erreur==1)
    strcpy(PathOfKK,"");

}


/*--------------------------------------------------------------------*\
|- Procedure principale                                               -|
\*--------------------------------------------------------------------*/
int main(int argc,char **argv)
{
char buffer[256],chaine[256];
int OldX,OldY;                         // To save the size of the screen


int n;
FILE *fic;

char *path;

IOerr=1;

/*--------------------------------------------------------------------*\
|- Initialisation de la librairie hard                                -|
\*--------------------------------------------------------------------*/

Redinit();

KKInit();

/*--------------------------------------------------------------------*\
|-  Initialisation de l'ecran                                         -|
\*--------------------------------------------------------------------*/

if (argc>2)
    {
    printf("No argument");
    return 0;
    }

todo=0;

if (!stricmp((argv[1])+1,"ABOUT")) todo=2;
if (!stricmp((argv[1])+1,"COLOR")) todo=12;
if (!stricmp((argv[1])+1,"PALETTE")) todo=13;
if (!stricmp((argv[1])+1,"AREA"))
    {
    subdir=0;
    updatekkr=1;
    todo=4;
    }

InitScreen(0);                     // Initialise toutes les donn~Bes HARD

OldX=GetScreenSizeX(); // A faire apres le SetMode ou le InitScreen
OldY=GetScreenSizeY();

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;                  // Initialisation de la taille ecra

/*--------------------------------------------------------------------*\
|- initialisation des clefs                                           -|
\*--------------------------------------------------------------------*/
qsort((void*)K,nbrkey,sizeof(struct key),sort_function);


/*--------------------------------------------------------------------*\
|-  Gestion des erreurs                                               -|
\*--------------------------------------------------------------------*/

KKCfg=(struct kkconfig*)GetMem(sizeof(struct kkconfig));

LoadErrorHandler();

path=(char*)GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--)
    {
    if (path[n]=='\\')
       {
       path[n]=0;
       break;
       }
    }


/*--------------------------------------------------------------------*\
|-  Initialisation des variables                                      -|
\*--------------------------------------------------------------------*/

Fenetre[0]=(FENETRE*)GetMem(sizeof(FENETRE));
Fenetre[0]->F=(struct file**)GetMem(TOTFIC*sizeof(void *));

Fenetre[1]=(FENETRE*)GetMem(sizeof(FENETRE));
Fenetre[1]->F=(struct file**)GetMem(TOTFIC*sizeof(void *));

Fenetre[2]=(FENETRE*)GetMem(sizeof(FENETRE));
Fenetre[2]->F=(struct file**)GetMem(TOTFIC*sizeof(void *));

KKFics=(struct kkfichier*)GetMem(sizeof(struct kkfichier));

KKCfg->V.Mask=(struct PourMask**)GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    KKCfg->V.Mask[n]=(struct PourMask*)GetMem(sizeof(struct PourMask));


/*--------------------------------------------------------------------*\
|-                      Initialisation des fichiers                   -|
\*--------------------------------------------------------------------*/

SetDefaultPath(path);
SetDefaultKKPath(path);

/*
Fics->help=(char*)GetMem(256);
strcpy(Fics->help,path);
Path2Abs(Fics->help,"kksetup.hlp");
*/

/*--------------------------------------------------------------------*\
|- Chargement de la configuration                                     -|
\*--------------------------------------------------------------------*/

LoadDefCfg=0;

if (LoadCfgSetup()==-1)
    {
    FENETRE *DFen;
    int t;

    DefaultCfg(Cfg);
    DefaultKKCfg();

    KKCfg->firsttime=1;

    LoadDefCfg=1;

    for (t=0;t<NBWIN;t++)
        {
        DFen=Fenetre[t];

        DFen->FenTyp=KKCfg->FenTyp[t];

        strcpy(DFen->path,Fics->path);
        DFen->order=1;
        DFen->sorting=1;

        DFen->nbrsel=0;

        DFen->F[0]=(struct file*)GetMem(sizeof(struct file));

        DFen->F[0]->name=(char*)GetMem(256);
        strcpy(DFen->F[0]->name,"kk.exe");


        DFen->scur=0;
        }
    // LoadConfigFile("main");
    }

Cfg->reinit=0;
InitMode();

#ifndef NOMOUSE
InitMouse();
#endif

Bar(" ----  ----  ----  ----  ----  ----  ----  ----  ----  ---- ");

if (todo!=0)
    {
    int x;

    x=(Cfg->TailleX-14)/2;

    Window(x,9,x+13,9,Cfg->col[28]);
    Cadre(x-1,8,x+14,10,0,Cfg->col[46],Cfg->col[47]);

    PrintAt(x+1,9,"Hi everybody");
    }

/*--------------------------------------------------------------------*\
|- Autres configurations                                              -|
\*--------------------------------------------------------------------*/

// if (LoadDefCfg)  ConfigSetup();

/*--------------------------------------------------------------------*\
|-  Gestion Message                                                   -|
\*--------------------------------------------------------------------*/

if (todo==0)
    {
    strcpy(chaine,KKFics->trash);
#ifndef LINUX
    if (mkdir(chaine)==0)
#else 
    if (mkdir(chaine,0)==0)
#endif
        {
        DispMessage("Creation of the user directory '%s': OK",chaine);
        DispMessage("");
        }
        else
        {
        char path[256];
        getcwd(path,256);

        if (chdir(chaine)==0)
            {
            DispMessage("User directory: '%s'",chaine);
            DispMessage("");
            }
            else
            {
            DispMessage("You must uncompress archive on your disk");
            DispMessage("");
            }
        chdir(path);
        }


/*--------------------------------------------------------------------*\
|-  Insertion de KK dans la path si pas d‚ja pr‚sent !                -|
\*--------------------------------------------------------------------*/

    strcpy(ActualPath,path);

    _searchenv("KK.BAT","PATH",buffer);
    if (strlen(buffer)!=0)
        {
        FILE *fic;
        static char toto[256];
        static char tata[256];

        fic=fopen(buffer,"rt");
        fgets(toto,256,fic);

        sprintf(tata,"@%s",ActualPath);
        Path2Abs(tata,"kk.exe\n");

        DispMessage("Found kk.Bat in %s",buffer);
            DispMessage("");

        if (stricmp(tata,toto)!=0)
            {
            if (WinMesg("Warning","KK.BAT have changed\n"
                                "Do you want overwrite it ?",16+1)==0)
                PutInPath();
            }

        Path2Abs(buffer,"..");
        strcpy(PathOfKK,buffer);
        }
        else
        PutInPath();


    fic=fopen(KKFics->FicIdfFile,"rb");
    if (fic==NULL)
        {
        DispMessage("You don't have already search player :(");
        DispMessage("  -> Go to menu 'Player'");
        DispMessage("  -> Select 'Search Player'");
        DispMessage("");
        }
        else
        fclose(fic);
    }


if (todo!=0)
    {
    GestionFct(todo);
    }
    else
    {
    do
        {
        FUNCTION fct;

        fct=GestionBar();
        if ((fct==7) | (fct==0)) break;
        GestionFct(fct);
        }
    while(1);

    if (strlen(PathOfKK)!=0)
        {
        DispMessage("KK.BAT & KKDESC.BAT are now in PATH (%s)",
                                                              PathOfKK);
        DispMessage("  -> You could run KK from everywhere");
        }
        else
        {
        DispMessage("WARNING: You couldn't run KK from everywhere "
                                                    "(Reload KKSETUP)");
        }

    DispMessage("");
    DispMessage("Press a key to continue");
    Wait(0,0);
    }

SaveCfg();

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

Cfg->reinit=0;
SetMode();

GotoXY(0,PosY);

for (n=0;n<9000;n++)
    Screen_Adr[n]=Screen_Buffer[n];

#ifndef NOFONT
Buf2Font(OldFont);
#endif

Buf2Pal(OldPal);

if (todo==0)
    cputs(RBTitle2);

return 0;
}



/*--------------------------------------------------------------------*\
|- Recherche d'application                                            -|
\*--------------------------------------------------------------------*/

int St_App;
int St_Dir;
char KeyPres[nbrkey];
int KeyPosX[nbrkey],KeyPosY[nbrkey];



//--- todo=1 -> affichage seulement ------------------------------------

int ExtAdd(int n)
{
do
{
n++;
if (n>=nbrkey) n=0;
}
while(K[n].numero<0);
return n;
}

int ExtSub(int n)
{
n--;
if (n==0) n=nbrkey-1;
return n;
}



/*--------------------------------------------------------------------*\
|- charge et initialise iardesc                                       -|
\*--------------------------------------------------------------------*/
void LoadKeyPres(void)
{
FILE *fic;
char sn;
short ext;
short lenbox;
char lendir;

//--- Mise … z‚ro des clefs --------------------------------------------
memset(KeyPres,0,nbrkey);

fic=fopen(KKFics->FicIdfFile,"rb");
if (fic!=NULL)
    {
    fseek(fic,8+1,SEEK_SET);

    while ( (sn=(char)(fgetc(fic)))!=3)
        {
        switch(sn)
            {
            case 1:
                fseek(fic,32+38+38,SEEK_CUR);
                if (fread(&ext,2,1,fic)==0) //--- EOF impromptu --------
                    break;
                fseek(fic,2+1+1+1,SEEK_CUR);
                KeyPres[ext]++;
                break;
            case 2:
                fread(&lendir,1,1,fic);
                fseek(fic,lendir,SEEK_CUR);
                break;
            case 4:
                fread(&lenbox,2,1,fic);
                fseek(fic,lenbox,SEEK_CUR);
                break;
            }
        }

   fclose(fic);
   }
}


void TabExt(char todo)
{
int car,x,y,n,j;
int mx;
char ext[32];

if (todo==0)
    {
    SaveScreen();
    Window(0,0,Cfg->TailleX-1,Cfg->TailleY-2,Cfg->col[16]);
    PrintAt(1,1,"Quick Extension Table");
    }

y=3;
mx=0;

for(n=0;n<nbrkey;n++)
    {
    int j;

    if ( (K[n].ext[0]=='*') | (x>Cfg->TailleX-8) )
        x=2,y++;

    if (x>mx) mx=x;

    if (K[n].ext[0]!='*')
        {
        j=K[n].numero;

        KeyPosX[j]=x+4;
        KeyPosY[j]=y;

        x+=7;
        }
    }

x=(Cfg->TailleX-(mx+6))/2-1;

if (todo==0)
    Cadre(1+x,3,mx+6+x,y+1,2,Cfg->col[55],Cfg->col[56]);

Window(2+x,4,mx+5+x,y,Cfg->col[28]);

for(n=0;n<nbrkey;n++)
    {
    if (K[n].numero>=0)
        {
        j=K[n].numero;

        KeyPosX[j]+=x;

        strcpy(ext,K[n].ext);
        ext[3]=0;

        PrintAt(KeyPosX[j]-4,KeyPosY[j],"%3s:%2d",ext,KeyPres[j]);

        if (KeyPres[j]==0)
            ColLin(KeyPosX[j],KeyPosY[j],2,Cfg->col[28]);
            else
            ColLin(KeyPosX[j],KeyPosY[j],2,Cfg->col[29]);
        }
    }



Window(1,y+2,Cfg->TailleX-2,Cfg->TailleY-3,Cfg->col[16]);


if (todo!=0) return;

n=1;

Bar(" ----  Info  ----  ----  ----  ----  ----  ----  ----  ---- ");

do
{
j=K[n].numero;

ColLin(KeyPosX[j]-4,KeyPosY[j],3,Cfg->col[30]);
car=Wait(0,0);
ColLin(KeyPosX[j]-4,KeyPosY[j],3,Cfg->col[28]);

#ifndef NOMOUSE
if (car==0)
    {
    int i,px,py;

    px=MousePosX();
    py=MousePosY();

    for(i=0;i<nbrkey;i++)
        {
        if (K[i].numero>=0)
            {
            if ( (px>=KeyPosX[K[i].numero]-4) &
                 (px<=KeyPosX[K[i].numero]+1) &
                 (py==KeyPosY[K[i].numero]) )
                 {
                 n=i;
                 }
            }
        }

    GetMouseFctBar(0);      //--- Allume -------------------------------

    car=GetMouseFctBar(2);  //--- Eteint -------------------------------
    }
#endif


switch(car)
    {
    case 9:
        n=ExtAdd(n);
        break;

    case 0x3C00:        //--- F2 ---------------------------------------
        FicIdfMan(n,NULL);   //--- n:numero de l'ext -------------------
        break;

    case 0x5000:        //--- DOWN -------------------------------------
        x=KeyPosX[K[n].numero];
        do
            n=ExtAdd(n);
        while(KeyPosX[K[n].numero]!=x);
        break;

    case 0x4800:        //--- UP ---------------------------------------
        x=KeyPosX[K[n].numero];
        do
            n=ExtSub(n);
        while(KeyPosX[K[n].numero]!=x);
        break;

    case 0x4D00:       //--- RIGHT -------------------------------------
        y=KeyPosY[K[n].numero];
        do
            n=ExtAdd(n);
        while(KeyPosY[K[n].numero]!=y);
        break;

    case 0x4B00:       //--- LEFT --------------------------------------
        y=KeyPosY[K[n].numero];
        do
            n=ExtSub(n);
        while(KeyPosY[K[n].numero]!=y);
        break;
    }
}
while(car!=27);

LoadScreen();
}


void LoadApplication(void)
{
char a[42];
FILE *fic;
char key[8];
char version;
char type;

char lendir;
short lenbox;

//--- Mise … z‚ro des clefs --------------------------------------------
memset(KeyPres,0,nbrkey);

nbr=0;
nbrdir=0;
nbrbox=0;

if (!updatekkr)
    return;

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
    fclose(fic);
    return;
	}

fread(&version,1,1,fic);
if (version!=KDBVER)
    {
    PUTSERR("Bad version of IDFEXT.RB");
    fclose(fic);
    return;
    }


do
	{
    fread(&type,1,1,fic);

    switch(type)
        {
        case 1:
            app[nbr]=(struct player*)GetMem(sizeof(struct player));

            fread(a,32,1,fic);            a[32]=0;
            app[nbr]->Filename=(char*)GetMem(strlen(a)+1);
            strcpy(app[nbr]->Filename,a);

            fread(a,38,1,fic);            a[38]=0;
            app[nbr]->Titre=(char*)GetMem(strlen(a)+1);
            strcpy(app[nbr]->Titre,a);

            fread(a,38,1,fic);            a[38]=0;
            app[nbr]->Meneur=(char*)GetMem(strlen(a)+1);
            strcpy(app[nbr]->Meneur,a);

            fread(&(app[nbr]->ext),2,1,fic);   //-Numero de format --------
            fread(&(app[nbr]->pres),2,1,fic);  //-Numero directory --------
            fread(&(app[nbr]->numbox),2,1,fic);//-Numero de la box --------
            fread(&(app[nbr]->type),1,1,fic);  //-Numero type -------------
            fread(&(app[nbr]->os),1,1,fic);    //-Numero operating system -
            fread(&(app[nbr]->info),1,1,fic);  //-Information sur player --

            if (((app[nbr]->info)&1)==1)
                app[nbr]->Checksum=1;

            KeyPres[app[nbr]->ext]++;

            nbr++;

//            if (nbrappl==50) break;

            break;
        case 2:
            fread(&lendir,1,1,fic);
            fread(dir[nbrdir],lendir,1,fic);
            dir[nbrdir][lendir]=0;
            nbrdir++;
            break;
        case 3:
            break;
        case 4:
            fread(&lenbox,2,1,fic);
            kkbox[nbrbox]=(char*)GetMem(lenbox);
            boxlen[nbrbox]=lenbox;

            fread(kkbox[nbrbox],boxlen[nbrbox],1,fic);
            nbrbox++;
            break;

        default:
            WinError("Error with FicIdf");
            type=3;
            break;
        }
    }
while(type!=3);

fclose(fic);
}

void ApplSearch(void)
{
char lstdrv[26];
short n;
char ch[256];
FILE *fic;

char sn;

//----------------------------------------------------------------------

GVerif=0;
St_App=0;
St_Dir=0;

//----------------------------------------------------------------------

SaveScreen();

if (todo!=0)
    Window(0,0,Cfg->TailleX-1,Cfg->TailleY-1,Cfg->col[16]);


PutCur(32,0);

Window(1,3,Cfg->TailleX-2,Cfg->TailleY-3,Cfg->col[16]);

LoadApplication();

if (todo==0)
    {
    for (n=2;n<26;n++)
        lstdrv[n]=1;
    lstdrv[0]=0;
    lstdrv[1]=0;

    ListDrive(lstdrv);

    for (n=0;n<26;n++)
        {
        sprintf(ch,"%c:\\",n+'A');
        if (lstdrv[n]==1)
            if (DriveReady((char)n)==1)
                KKR_Search(ch);
        }
    }
    else
    {
    getcwd(ch,256);
    KKR_Search(ch);
    }

strcpy(ch,Fics->path);
Path2Abs(ch,"iar_desc.kkr");

fic=fopen(ch,"rb");
if (fic==NULL)
    WinError("iar_desc.kkr not found");
    else
    {
    KKR_Read(fic);
    fclose(fic);
    }

//--- pr‚paration de l'‚cran -------------------------------------------

Window(2,2,Cfg->TailleX-2,4,Cfg->col[16]);

PrintAt(2,3,"Scanning directory for %d records",nbr+1);

TabExt(1);

//----------------------------------------------------------------------

if (todo==0)
    {
    for (n=0;n<26;n++)
        {
        sprintf(ch,"%c:\\*.*",n+'A');
        if (lstdrv[n]==1)
            if (DriveReady((char)n)==1)
                SSearch(ch);
        }
    }
    else
    {
    getcwd(ch,256);
    Path2Abs(ch,"*.*");
    SSearch(ch);
    }


if (nbr>0)
    {
    fic=fopen(KKFics->FicIdfFile,"wb");
    if (fic!=NULL)
        {
        char iarver;

        fwrite("RedBLEXU",1,8,fic);

        iarver=KDBVER;
        fwrite(&iarver,1,1,fic);

        for(n=0;n<nbr;n++)
            if (app[n]->pres!=0)
                {
                static char a[256];

                sn=1;
                fwrite(&sn,1,1,fic);

                strcpy(a,app[n]->Filename);
                fwrite(a,32,1,fic);

                strcpy(a,app[n]->Titre);
                fwrite(a,38,1,fic);

                strcpy(a,app[n]->Meneur);
                fwrite(a,38,1,fic);

                fwrite(&(app[n]->ext),2,1,fic);      // Numero de format
                fwrite(&(app[n]->pres),2,1,fic);     // Numero directory
                fwrite(&(app[n]->numbox),2,1,fic);   // Numero de la box

                fwrite(&(app[n]->type),1,1,fic);          // Numero type

                fwrite(&(app[n]->os),1,1,fic);// Numero operating system

                if (app[n]->info==0)
                    {
                    if (app[n]->Checksum!=0)
                        app[n]->info=1;
                    }

                fwrite(&(app[n]->info),1,1,fic);  // Information fichier
                }

        for(n=0;n<nbrdir;n++)
            {
            char lendir;

            sn=2;
            fwrite(&sn,1,1,fic);
            lendir=(char)(strlen(dir[n]));
            fwrite(&lendir,1,1,fic);
            fwrite(dir[n],1,lendir,fic);
            }

        for(n=0;n<nbrbox;n++)
            {
            short lenbox;

            sn=4;
            fwrite(&sn,1,1,fic);
            lenbox=(short)(boxlen[n]);
            fwrite(&lenbox,2,1,fic);
            fwrite(kkbox[n],1,lenbox,fic);
            }

        sn=3;
        fwrite(&sn,1,1,fic);

        fclose(fic);
        }

    TabExt(0);

    LoadScreen();

    DispMessage("Statistics");
    DispMessage("I have found %3d applications",St_App);
    DispMessage("          in %3d directories",St_Dir);
    DispMessage("");
    }
    else
    {
    LoadScreen();
    }
}

int posy=3;

void KKR_Read(FILE *Fic)
{
char Key[4];
char KKType,KKos;
char Comment[255],SComment;
char Titre[255],STitre;
char Meneur[255],SMeneur;
char Filename[255],SFilename;
int Checksum;
short format;
int n;
char pres;
short lenbox;
char *box;

char Code;
char fin;

box=NULL;
lenbox=0;

fin=0;

fread(Key,4,1,Fic);
if (!strncmp(Key,"KKRB",4))
    do {
    fread(&Code,1,1,Fic);
    switch(Code)
        {
        case 0:                         // Commentaire (sans importance)
            fread(&SComment,1,1,Fic);
            fread(Comment,SComment,1,Fic);
            break;
        case 1:                                            // Code Titre
            fread(&STitre,1,1,Fic);
            Titre[STitre]=0;
            fread(Titre,STitre,1,Fic);

            PrintAt(2,2,"Loading information about %-*s",Cfg->TailleX-30,
                                                                 Titre);
/*            posy++;

            if (posy>(Cfg->TailleY-3))
                {
                MoveText(1,4,78,(Cfg->TailleY-3),1,3);
                posy--;
                ChrLin(1,(Cfg->TailleY-3),78,32);
                }*/

            posy=6;
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

            pres=0;
            for(n=0;n<nbr;n++)
                if (format==app[n]->ext)
                    if ( (!stricmp(Filename,app[n]->Filename)) &
                     (!stricmp(Titre,app[n]->Titre)))
                    {
                    if (Checksum==app[n]->Checksum)
                        pres=1;
                    }

            if (!pres)
                {
                PrintAt(2,3,"%4d records read",nbr+1);

                app[nbr]=(struct player*)GetMem(sizeof(struct player));

                app[nbr]->Filename=(char*)GetMem(SFilename+1);
                strcpy(app[nbr]->Filename,Filename);

                app[nbr]->Meneur=(char*)GetMem(SMeneur+1);
                strcpy(app[nbr]->Meneur,Meneur);

                app[nbr]->Titre=(char*)GetMem(STitre+1);
                strcpy(app[nbr]->Titre,Titre);

                app[nbr]->Checksum=Checksum;
                app[nbr]->ext=format;
                app[nbr]->pres=0;

                app[nbr]->type=KKType;

                app[nbr]->os=KKos;

                if (lenbox!=0)
                    {
                    app[nbr]->box=(char*)GetMem(lenbox);

                    app[nbr]->boxlen=lenbox;
                    memcpy(app[nbr]->box,box,lenbox);
                    }

                nbr++;
                }
            break;
        case 6:                                        // Fin de fichier
            fin=1;
            break;
        case 7:                                                 // Reset
            Checksum=0;

            strcpy(Titre,"?");
            strcpy(Meneur,"?");
            strcpy(Filename,".");

            SFilename=(char)strlen(Filename);
            STitre=(char)strlen(Titre);
            SMeneur=(char)strlen(Meneur);

            if (lenbox!=0)
                {
                free(box);
                lenbox=0;
                }

            KKType=0;
            KKos=0;
            break;
        case 8:                                     // type du programme
            fread(&KKType,1,1,Fic);
            break;
        case 9:                                      // Operating system
            fread(&KKos,1,1,Fic);
            break;
        case 10:
            fread(&lenbox,2,1,Fic);
            if (lenbox!=0)
                {
                box=(char*)GetMem(lenbox);
                fread(box,lenbox,1,Fic);
                }

            break;
        }
    }
    while(fin==0);

}


void KKR_Search(char *nom2)
{
struct find_t fic;
char moi[256],nom[256];
FILE *Fic;

char **TabRec;              // Tableau qui remplace les appels recursifs
int NbrRec;                          // Nombre d'element dans le tableau

TabRec=(char**)GetMem(500*sizeof(char*));
TabRec[0]=(char*)GetMem(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
strcpy(nom,TabRec[NbrRec-1]);

DispDir(nom);

strcpy(moi,nom);
Path2Abs(moi,"*.KKR");

if (_dos_findfirst(moi,63-RB_SUBDIR,&fic)==0)
do
    {
    if ((fic.attrib&RB_SUBDIR)!=RB_SUBDIR)
        {
        strcpy(moi,nom);
        Path2Abs(moi,fic.name);
        Fic=fopen(moi,"rb");
        if (Fic==NULL)
            {
            PrintAt(0,0,"KKR_search (1)");
            exit(1);
            }
        KKR_Read(Fic);
        fclose(Fic);
        }
    }
while (_dos_findnext(&fic)==0);




free(TabRec[NbrRec-1]);
NbrRec--;

strcpy(moi,nom);
Path2Abs(moi,"*.*");

if ( (_dos_findfirst(moi,63,&fic)==0) & (subdir==1))
do
    {
    if  ( (fic.name[0]!='.') & (((fic.attrib)&RB_SUBDIR) == RB_SUBDIR))
            {
            strcpy(moi,nom);
            Path2Abs(moi,fic.name);

            TabRec[NbrRec]=(char*)GetMem(strlen(moi)+1);
            memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
            NbrRec++;
            }
    }
while (_dos_findnext(&fic)==0);
}
while(NbrRec>0);

free(TabRec);
}


void DispDir(char *dir)
{
ColLin(0,Cfg->TailleY-1,Cfg->TailleX,Cfg->col[30]);
PrintAt(0,Cfg->TailleY-1,"%-*s",Cfg->TailleX,dir);
}

void SSearch(char *nom2)
{
struct find_t fic;
char moi[256],nom[256];
short n;
int o;
char ok;
char bill;
signed long wok;

unsigned long KKcrc;
unsigned long K1crc;                 // crc calcul‚ une fois pour toutes

unsigned long C;

char **TabRec;              // Tableau qui remplace les appels recursifs
int NbrRec;                          // Nombre d'element dans le tableau

char *StrVerif,Verif;


TabRec=(char**)GetMem(500*sizeof(char*));
TabRec[0]=(char*)GetMem(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
o=nbrdir+1;

DispDir(nom);
St_Dir++;

strcpy(nom,TabRec[NbrRec-1]);
DispDir(nom);

if (_dos_findfirst(nom,63-RB_SUBDIR,&fic)==0)
do
    {
    ok=0;
    wok=-1;

    if ((fic.attrib&RB_SUBDIR)!=RB_SUBDIR)
        {
        C=0;
        KKcrc=0;                               // CRC du fichier courant
        K1crc=0;

        for(n=0;n<nbr;n++)
            if ( (!FileComp(fic.name,app[n]->Filename)) &
                                                 (app[n]->Checksum!=0) )
                {
                if (KKcrc==0)
                    {
                    if (K1crc==0)
                        {
                        int n;

                        strcpy(moi,nom);
                        moi[strlen(moi)-3]=0;
                        Path2Abs(moi,fic.name);
                        n=0;
                        while(moi[n]!=0)
                            if (moi[n]==32) moi[n]=0; else n++;

                        crc32file(moi,&KKcrc);
                        K1crc=KKcrc;
                        }
                        else
                        KKcrc=K1crc;

                    if (KKcrc!=app[n]->Checksum) KKcrc=0;
                    }
                }

        Verif=0;
        StrVerif=app[0]->Titre;//pour pas mettre NULL, ca veut rien dire

        for(n=0;n<nbr;n++)
            {
            bill=0;
            if (!FileComp(fic.name,app[n]->Filename))
                {
                if ( (KKcrc==0) & (app[n]->Checksum==0) )
                    {
                    if ((Verif==0)| (strcmp(StrVerif,app[n]->Titre)!=0))
                        {
                        strcpy(moi,nom);
                        moi[strlen(moi)-3]=0;
                        Path2Abs(moi,fic.name);

                        if (GVerif==0)
                            Interroge(moi,app[n],&Verif,&GVerif);
                            else
                            Verif=GVerif;

                        StrVerif=app[n]->Titre;
                        }

                    if (Verif==1)
                        bill=1;
                    }

                if ((KKcrc==app[n]->Checksum) & (KKcrc!=0))
                    bill=1;

                if (bill==1) //- l'appl. n est presente dans le dir. o -
                    {
                    if (app[n]->pres==0)
                        {
                        app[n]->pres=(short)o;

                        strcpy(moi,nom);
                        moi[strlen(moi)-3]=0;
                        ok=1;
                        wok=n;

                        //--- Editeur par default ------------------------------
                        if (app[n]->ext==91)
                            if (KKCfg->editeur[0]==0)
                                if (strlen(app[n]->Filename)<63)
                                    {
                                    strcpy(KKCfg->editeur,moi);
                                    Path2Abs(KKCfg->editeur,app[n]->Filename);
                                    }

                        KeyPres[app[n]->ext]++;
                        PrintAt(KeyPosX[app[n]->ext],KeyPosY[app[n]->ext],"%2d",KeyPres[app[n]->ext]);
                        ColLin(KeyPosX[app[n]->ext],KeyPosY[app[n]->ext],2,Cfg->col[29]);

                        if (app[n]->boxlen!=0)
                            {
                            box[nbrbox]=(char*)GetMem(app[n]->boxlen);
                            boxlen[nbrbox]=app[n]->boxlen;
                            memcpy(box[nbrbox],app[n]->box,app[n]->boxlen);
                            nbrbox++;
                            app[n]->numbox=(short)nbrbox;
                            }
                        }
                    }
                }
            }
        }

    if (ok==1)
        {
        char buf[256];

        St_App++;

        nbrdir=o;
        strcpy(dir[o-1],moi);

        if (app[wok]->Checksum==0)
            sprintf(buf,"? Found %s in %s",app[wok]->Titre,dir[o-1]);
            else
            sprintf(buf,"Found %s in %s",app[wok]->Titre,dir[o-1]);

        PrintAt(2,2,"%-*s",Cfg->TailleX-4,buf);
        }
   }
while (_dos_findnext(&fic)==0);

free(TabRec[NbrRec-1]);
NbrRec--;

if ((_dos_findfirst(nom,63,&fic)==0) & (subdir==1))
do
	{
    if ( (fic.name[0]!='.') & (((fic.attrib) & RB_SUBDIR)==RB_SUBDIR) )
			{
			strcpy(moi,nom);
			moi[strlen(moi)-3]=0;
            Path2Abs(moi,fic.name);
            Path2Abs(moi,"*.*");

            TabRec[NbrRec]=(char*)GetMem(strlen(moi)+1);
            memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
            NbrRec++;
			}
	}
while (_dos_findnext(&fic)==0);

}
while(NbrRec>0);


free(TabRec);
}


char TestCar(char c)
{
if (c==32) return 1;
if (c=='=') return 1;
return 0;
}

void ClearAllSpace(char *name)
{
char c,buf[128];
short i,j;

i=0;                                             // navigation dans name
j=0;                                                // position dans buf

while ( (TestCar(name[i])) & (name[i]!=0) ) i++;

if (name[i]!=0)
    while ((c=name[i])!=0)
        {
        buf[j]=name[i];
        j++;
        i++;
        if (TestCar(name[i]))
            {
            c=name[i];
            while ( (TestCar(name[i])) & (name[i]!=0) )
                {
                if (name[i]=='=') c='=';
                i++;
                }
            buf[j]=c;
            j++;
            }
        }
buf[j]=0;

strcpy(name,buf);
}

int Traite(char *from,char *to)
{
int i;

i=0;

while( (from[i]!=0) & (from[i]!='=') )
    {
    to[i]=from[i];
    i++;
    }
to[i]=0;

if (from[i]!=0)
    {
    sscanf(from+i+1,"%d",&i);
    return i;
    }
return 0;
}



void SaveConfigFile(void)
{
FILE *fic;
char filename[256];
char title[32];
int size,pos;
char cbuf;
int lbuf;

strcpy(filename,KKFics->trash);
Path2Abs(filename,"savecfg.kkp");

fic=fopen(filename,"wb");
if (fic!=NULL)
    {
    strcpy(title,"Saved config from setup");

    size=16384;
    pos=0;

    fprintf(fic,"MCRB");
    fwrite(title,1,32,fic);
    fwrite(&size,1,4,fic);
    fwrite(&pos,1,4,fic);

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x30,0);  // copie 48 bytes dans sbuf[0xFF]
    fwrite(Cfg->palette,1,48,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x0E);      // pal=sbuf[0xFF]

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(Cfg->col,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x0F);      // col=sbuf[0xFF]

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Cfg->windesign;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,3);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Cfg->font;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,5);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Cfg->display;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,6);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->Esc2Close;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,7);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->pathdown;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,8);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->dispath;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,9);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->sizewin;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,10);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->isidf;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,11);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->isbar;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,12);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->dispcolor;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,13);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->userfont;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,16);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->pntrep;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,17);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->hidfil;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,18);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->logfile;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,19);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->autoreload;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,20);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->verifhist;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,21);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->palafter;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,22);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Cfg->debug;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,23);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->currentdir;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,24);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Cfg->speedkey;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,25);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->insdown;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,26);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->seldir;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,27);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Cfg->comport;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,28);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->enterkkd;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,29);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->cnvhist;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,34);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->esttime;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,35);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->confexit;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,36);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->savekey;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,37);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->KeyAfterShell;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,38);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->addselect;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,39);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Cfg->combit;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,41);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Cfg->comparity;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,42);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Cfg->comstop;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,43);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Cfg->mousemode;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,44);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->lift;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,45);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=Info->defform;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,47);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->internshell;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,48);

    fprintf(fic,"%c%c",0x05,0xFF);      // 5-> lbuf
    lbuf=Cfg->TailleY;
    fwrite(&lbuf,1,4,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,2);

    fprintf(fic,"%c%c",0x05,0xFF);      // 5-> lbuf
    lbuf=Cfg->SaveSpeed;
    fwrite(&lbuf,1,4,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,4);

    fprintf(fic,"%c%c",0x05,0xFF);      // 5-> lbuf
    lbuf=Cfg->TailleX;
    fwrite(&lbuf,1,4,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,1);

    fprintf(fic,"%c%c",0x05,0xFF);      // 5-> lbuf
    lbuf=Cfg->comspeed;
    fwrite(&lbuf,1,4,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,33);

    fprintf(fic,"%c%c",0x05,0xFF);      // 5-> lbuf
    lbuf=KKCfg->mtrash;
    fwrite(&lbuf,1,4,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,40);

//--- Les fontes maintenant --------------------------------------------

    if (KKCfg->userfont)
        {
        FILE *infic;
        short int lng;
        char buffer[256];

        strcpy(buffer,KKFics->trash);
        switch (Cfg->TailleY)
            {
            case 50:
                Path2Abs(buffer,"font8x8.cfg");
                lng=2048;
                break;
            case 25:
            case 30:
                Path2Abs(buffer,"font8x16.cfg");
                lng=4096;
                break;
            }

        infic=fopen(buffer,"rb");
        if (infic!=NULL)
            {
            int n;

            fprintf(fic,"%c%c",2,255);
            fwrite(&lng,2,1,fic);
            for(n=0;n<lng;n++)
                fputc(fgetc(infic),fic);
            fclose(infic);

            switch (Cfg->TailleY)
                {
                case 50:
                    fprintf(fic,"%c%c%c",7,2,255); //-- font8x16.cfg --
                    break;
                case 25:
                case 30:
                    fprintf(fic,"%c%c%c",7,1,255); //-- font8x8.cfg ---
                    break;
                }
            fclose(infic);
            }
        }
//--- Sauvegarde de l'editeur, du viewer et du screensaver -------------

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(KKCfg->editeur,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,30);

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(KKCfg->vieweur,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,31);

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(KKCfg->ssaver,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,32);

//--- Les Extensions Highlighted ---------------------------------------

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->Enable_Txt;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x32);

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(KKCfg->ExtTxt,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x31);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->Enable_Bmp;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x34);

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(KKCfg->ExtBmp,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x33);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->Enable_Snd;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x36);

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(KKCfg->ExtSnd,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x35);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->Enable_Arc;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x38);

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(KKCfg->ExtArc,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x37);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->Enable_Exe;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x3A);

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(KKCfg->ExtExe,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x39);

    fprintf(fic,"%c%c",0x04,0xFF);      // 4-> cbuf
    cbuf=KKCfg->Enable_Usr;
    fwrite(&cbuf,1,1,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x3C);

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(KKCfg->ExtUsr,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x3B);

//----------------------------------------------------------------------

//----------------------------------------------------------------------

    fprintf(fic,"%c%c%c%c",0x01,0x43,0x00,0x06);

    fclose(fic);

    DispMessage("Saving current configuration in SAVECFG.KKP: OK");
    DispMessage("");
    }
    else
    {
    DispMessage("Saving current configuration in SAVECFG.KKP: Error !!");
    DispMessage("");
    }

}


/*--------------------------------------------------------------------*\
|-  Gestion de la barre de menu du haut                               -|
\*--------------------------------------------------------------------*/

FUNCTION GestionBar(void)
{
MENU menu;
int retour,nbmenu;
int u,s,x;

struct barmenu bar[20];

short fin;

static int cpos[20],poscur;

SaveScreen();
PutCur(32,0);

do
{
bar[0].Titre="Player";
bar[1].Titre="Information";
bar[2].Titre="Tools";
bar[3].Titre="Help";

bar[0].Help=0;
bar[1].Help=0;
bar[2].Help=0;
bar[3].Help=0;

u=BarMenu(bar,4,&poscur,&x,0);
                               // Renvoit t: position du machin surligne
            // Renvoit v: 0 si rien, autre si position dans sous fenetre
if (u==0)
    {
    fin=0;
    break;
    }

switch (poscur)
 {
 case 0:
    bar[0].Titre="Search player";   bar[0].fct=4; bar[0].Help=0;
    nbmenu=1;
    break;
 case 1:
    bar[0].Titre="Show all format "; bar[0].fct=3; bar[0].Help=0;
    bar[1].Titre="Quick format    "; bar[1].fct=14; bar[1].Help=0;
    nbmenu=2;
    break;
 case 2:
    bar[0].Titre="Config. Default "; bar[0].fct=10; bar[0].Help=0;
    bar[1].Titre="File Setup      "; bar[1].fct=11; bar[1].Help=0;
    bar[2].Titre="Key setting     "; bar[2].fct=8;  bar[2].Help=0;
    bar[3].Titre=NULL;               bar[3].fct=0;  bar[3].Help=0;
    bar[4].Titre="Color Definition"; bar[4].fct=12; bar[4].Help=0;
    bar[5].Titre="Palette         "; bar[5].fct=13; bar[5].Help=0;
    bar[6].Titre=NULL;               bar[6].fct=0;  bar[6].Help=0;
    bar[7].Titre="Write Profile   "; bar[7].fct=9;  bar[7].Help=0;
    nbmenu=8;
    break;
 case 3:
    bar[0].Titre="Help "; bar[0].fct=1; bar[0].Help=0;
    bar[1].Titre="About"; bar[1].fct=2; bar[1].Help=0;
    bar[2].Titre=NULL;    bar[2].fct=0; bar[2].Help=0;
    bar[3].Titre="Exit "; bar[3].fct=7; bar[3].Help=0;
    nbmenu=4;
    break;
    }

s=2;
menu.x=x;
menu.y=s;
menu.cur=cpos[poscur];
menu.attr=0;

retour=PannelMenu(bar,nbmenu,&menu);      // (x,y)=(t,s)

cpos[poscur]=menu.cur;

if (retour==2)
    {
    fin=(short)(bar[cpos[poscur]].fct);
    break;
    }
    else
    {
    poscur+=retour;
    fin=0;
    }

}
while(1);

LoadScreen();

return (FUNCTION)fin;
}

/*--------------------------------------------------------------------*\
|- 0: Quit                                                            -|
|- 1: Help                                                            -|
|- 2: About                                                           -|
|- 3: list all the format                                             -|
|- 4: search application                                              -|
|- 5: load ------------                                               -|
|- 6: Putinpath                                                       -|
|- 7: Exit                                                            -|
|- 8: Modification des touches                                        -|
|- 9: Sauve le fichier des configurations                             -|
|-10: Appelle le menu setup configuration                             -|
|-11: Configuration de l'editeur                                      -|
|-12: Setup couleur                                                   -|
|-13: Setup Palette                                                   -|
|-14: Quick Tab                                                       -|
\*--------------------------------------------------------------------*/

void GestionFct(FUNCTION i)
{
switch(i)
    {
    case 0:
        break;  // Quit
    case 1:
        SaveScreen();
        PutCur(32,0);
        ChrWin(0,0,Cfg->TailleX-1,Cfg->TailleY-1,32);
        Help();
        LoadScreen();
        break;
    case 2:
        WinMesg("About",RBTitle,0);
        break;
    case 3:
        IdfListe();
        break;
    case 4:
        ApplSearch();
        break;
    case 5:
        break;
    case 6:
        PutInPath();
        break;
    case 7:
        break;
    case 8:
        ModifKey();
        break;
    case 9:
        SaveConfigFile();
        break;
    case 10:
        ConfigSetup();
        break;
    case 11:
        FileSetup();
        break;
    case 12:
        ColorChange();
        break;
    case 13:
        ChangePalette();
        break;
    case 14:
        LoadKeyPres();
        TabExt(0);
        break;
    }
}



/*--------------------------------------------------------------------*\
|-  Return -1 if error, 0 in other case                               -|
\*--------------------------------------------------------------------*/

int crc32file(char *name,unsigned long *crc)
{
FILE *fin;
unsigned long oldcrc32;
unsigned long crc32;
unsigned long oldcrc;
int c;
long charcnt;

oldcrc32 = 0xFFFFFFFF;
charcnt = 0;

if ((fin=fopen(name, "rb"))==NULL)
    {
    perror(name);
    return -1;
    }

while ((c=getc(fin))!=EOF)
    {
    ++charcnt;
    oldcrc32 = (crc_32_tab[(oldcrc32^c) & 0xff]^(oldcrc32 >> 8));
    }

if (ferror(fin))
    {
    perror(name);
    charcnt = -1;
    }
fclose(fin);

crc32 = oldcrc32;
oldcrc = oldcrc32 = ~oldcrc32;

*crc=oldcrc;

return 0;
}

/*--------------------------------------------------------------------*\
|- Charge un configuration par default                                -|
|- ------------------------------------------------------------------ -|
|- 0: Ketchup Improvement                                             -|
|- 1: Classic Configuration                                           -|
\*--------------------------------------------------------------------*/

void DefaultConfig(int a)
{
switch(a)
    {
    case 0:                                  //--- Configuration normale
        KKCfg->enterkkd=1;                      //--- Entre dans les KKD
        KKCfg->V.ajustview=1;    //--- Ajuste l'affichage pour le viewer
        KKCfg->cnvhist=1;                  //--- Conversion de l'history
        KKCfg->dispcolor=1;                  //--- Souligne les couleurs
        KKCfg->hidfil=1;               //--- Affiche les fichiers caches
        break;
    case 1:
        KKCfg->enterkkd=0;                //--- N'entre pas dans les KKD
        KKCfg->V.ajustview=0;//- N'ajuste pas l'affichage pour le viewer
        KKCfg->cnvhist=0;           //--- Pas de conversion de l'history
        KKCfg->dispcolor=0;           //--- Ne souligne pas les couleurs
        KKCfg->hidfil=0;         //--- N'affiche pas les fichiers caches
        break;
    }
}

/*--------------------------------------------------------------------*\
|- Confiration par default                                            -|
\*--------------------------------------------------------------------*/
void ConfigSetup(void)
{
static int sy;

static char x1=32;
static int y1=2;

struct Tmt T[] = {
      {5, 2,10,"Ketchup Improvement ",&sy},
      {5, 3,10,"Classic Configuration ",&sy},

      {3,1,9,&x1,&y1},

      {3,5,2,NULL,NULL},                                      // le OK
      {24,5,3,NULL,NULL}                                   // le CANCEL
      };

struct TmtWin F = {-1,5,40,12,"Configuration"};

int n;

sy=0;

n=WinTraite(T,5,&F,0);

if (n==-1) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

DefaultConfig(sy);
}

/*--------------------------------------------------------------------*\
|- Comparaison entre 2 noms de fichiers                               -|
\*--------------------------------------------------------------------*/
int FileComp(char *a,char *b)
{
int n;
char an,bn;

n=0;

while(1)
    {
    an=(char)toupper(a[n]);
    bn=(char)toupper(b[n]);

    if ( ((an==0) | (an==32)) & ((bn==0) | (bn==32)) )
        return 0;

    if (an!=bn)
        return 1;

    n++;
    }
}


void Str2Pal(char *from,char *to)
{
int n;
char cf,ct;

for(n=0;n<48;n++)
    {
    ct=0;
    cf=from[n];

    if ((cf>='A') & (cf<='Z')) ct=(char)(cf-'A');
    if ((cf>='a') & (cf<='z')) ct=(char)(cf-'a'+26);
    if ((cf>='0') & (cf<='9')) ct=(char)(cf-'0'+52);
    if (cf=='+') ct=62;
    if (cf=='-') ct=63;

    to[n]=ct;
    }
}

void Pal2Str(char *from,char *to)
{
int n;
char cf,ct;

for(n=0;n<48;n++)
    {
    ct=0;
    cf=from[n];

    if (cf<26) ct=(char)(cf+'A');
    if ((cf>=26) & (cf<52)) ct=(char)(cf+'a'-26);
    if ((cf>=52) & (cf<62)) ct=(char)(cf+'0'-52);
    if (cf==62) ct='+';
    if (cf==63) ct='-';

    to[n]=ct;
    }
to[48]=0;
}

void Interroge(char *path,struct player *app,char *Verif,char *GVerif)
{
static char CadreLength=70;
static char Msg1[256],Msg2[256],Msg3[256],Msg4[256];
static int CadreWidth=4;

struct Tmt T[] = {
      { 5,7,5,"     Yes     ",NULL},
      {22,7,5,"     No      ",NULL},
      {39,7,5,"  Always Yes ",NULL},
      {56,7,5,"  Always No  ",NULL},
      { 5,2,0,Msg1,NULL},
      { 5,3,0,Msg2,NULL},
      { 5,4,0,Msg3,NULL},
      { 5,5,0,Msg4,NULL},
      { 1,1,9,&CadreLength,&CadreWidth}
      };

struct TmtWin F = {-1,3,74,12, "Security prompt" };

strcpy(Msg1,"Do you think that");
strcpy(Msg2,path);
sprintf(Msg3,"is %s of",app->Titre);
sprintf(Msg4,"%s",app->Meneur);

switch(WinTraite(T,9,&F,0))
    {
    case 0:
        (*Verif)=1;     // Yes
        break;
    case -1:
    case 1:
        (*Verif)=2;     // Cancel
        break;
    case 2:
        (*Verif)=1;
        (*GVerif)=1;    // Replace ALL
        break;
    case 3:
        (*Verif)=2;
        (*GVerif)=2;    // Cancel ALL
        break;
    }
}

void Col2Str(char *from,char *to)
{
int n;

for(n=0;n<64;n++)
    sprintf(to+n*2,"%02X",from[n]);
}

void Str2Col(char *from,char *to)
{
int n,m;

for(n=0;n<64;n++)
    {
    sscanf(from+n*2,"%02X",&m);
    to[n]=(char)m;
    }
}

void InitMode(void)
{
int x,y;

if (todo==0)
    {
    SetMode();

#ifndef NOFONT
    if (KKCfg->userfont)
        InitFontFile(KKFics->trash);
        else
        InitFont();
#endif

    LoadPal();

    Cadre(0,1,Cfg->TailleX-1,(Cfg->TailleY-2),1,Cfg->col[55],Cfg->col[56]);
    Window(1,2,Cfg->TailleX-2,(Cfg->TailleY-3),Cfg->col[16]);

    ColLin(0,0,Cfg->TailleX,Cfg->col[17]);
    ChrLin(0,0,Cfg->TailleX,32);

    PrintAt((Cfg->TailleX-38)/2,0,"Setup of Ketchup Killers Commander");
    }
    else
    {
    SetMode();

#ifndef NOFONT
    InitFont();
#endif

    LoadPal();

    for(x=0;x<OldX;x++)
        for(y=0;y<OldY;y++)
            {
            AffCol(x,y,Screen_Buffer[(x+y*OldX)*2+1]);
            AffChr(x,y,Screen_Buffer[(x+y*OldX)*2]);
            }
    }

InitMessage();
}

void ColorChange(void)
{
MENU menu;
int retour;
static struct barmenu bar[19];

bar[0].Titre="Pannel"; bar[0].Help=0;
bar[0].fct=1;

bar[1].Titre="KeyBar"; bar[1].Help=0;
bar[1].fct=2;

bar[2].Titre="Window 1"; bar[2].Help=0;
bar[2].fct=3;

bar[3].Titre="Window 2"; bar[3].Help=0;
bar[3].fct=4;

bar[4].Titre="Help"; bar[4].Help=0;
bar[4].fct=5;

bar[5].Titre="Input Box"; bar[5].Help=0;
bar[5].fct=6;

bar[6].Titre="PullDown Bar"; bar[6].Help=0;
bar[6].fct=7;

bar[7].Titre="PullDown Menu"; bar[7].Help=0;
bar[7].fct=8;

bar[8].Titre="HTML Viewer"; bar[8].Help=0;
bar[8].fct=9;

SaveScreen();

IColor();


menu.x=20;
menu.y=4;
menu.cur=0;
menu.attr=8;

do
    {
    retour=PannelMenu(bar,9,&menu);

    if (retour==2)
        CColor(bar[menu.cur].fct-1);
    }
while (retour==2);

LoadScreen();
}

void IColor(void)       // InitColor
{
int x,y;
int Hexa[]={'0','1','2','3','4','5','6','7','8','9',
                                               'A','B','C','D','E','F'};

Cadre(0,0,18,18,0,Cfg->col[55],Cfg->col[56]);
AffChr(1,1,3);
for(x=0;x<16;x++)
    {
    AffCol(x+2,1,Cfg->col[16]);
    AffCol(1,x+2,Cfg->col[16]);
    AffChr(x+2,1,Hexa[x]);
    AffChr(1,x+2,Hexa[x]);
    for(y=0;y<16;y++)
        {
        AffCol(x+2,y+2,y*16+x);
        AffChr(x+2,y+2,'+');
        }
    }
}


int DColor(int col)
{
int x,y,car;

x=col&15;
y=col/16;

AffCol(x+2,1,Cfg->col[18]);
AffCol(1,y+2,Cfg->col[18]);
AffChr(x+2,y+2,'*');
car=Wait(0,0);
AffCol(x+2,1,Cfg->col[16]);
AffCol(1,y+2,Cfg->col[16]);
AffChr(x+2,y+2,'+');

switch(HI(car))
    {
    case 72:    y--;    break;
    case 80:    y++;    break;
    case 0x4B:  x--;    break;
    case 0x4D:  x++;    break;
    }
x=x&15;
y=y&15;

PrintAt(0,0,"%3d",y*16+x);

if (car==27)
    return -1;
    else
    return y*16+x;
}

void CColor(int m)
{
int retour,n;
static struct barmenu bar[16];
MENU menu;
int r;

SaveScreen();

menu.x=20;
menu.y=4;
menu.attr=8;

switch(m)
    {
    case 0:      // Pannel
        bar[0].Titre="Border1"; bar[0].Help=0;
        bar[0].fct=38;

        bar[1].Titre="Border2"; bar[1].Help=0;
        bar[1].fct=39;

        bar[2].Titre="Normal"; bar[2].Help=0;
        bar[2].fct=1;

        bar[3].Titre="Bright"; bar[3].Help=0;
        bar[3].fct=3;

        bar[4].Titre="Reverse"; bar[4].Help=0;
        bar[4].fct=2;

        bar[5].Titre="Underline"; bar[5].Help=0;
        bar[5].fct=5;

        bar[6].Titre="Bright Reverse"; bar[6].Help=0;
        bar[6].fct=4;

        bar[7].Titre="Underline"; bar[7].Help=0;
        bar[7].fct=40;

        bar[8].Titre="Bright Reverse"; bar[8].Help=0;
        bar[8].fct=41;

        bar[9].Titre=""; bar[9].Help=0;
        bar[9].fct=0;

        bar[10].Titre="EXEcutable"; bar[10].Help=0;
        bar[10].fct=16;
        bar[11].Titre="ARChive"; bar[11].Help=0;
        bar[11].fct=23;
        bar[12].Titre="SouNDfile"; bar[12].Help=0;
        bar[12].fct=24;
        bar[13].Titre="BitMaP"; bar[13].Help=0;
        bar[13].fct=33;
        bar[14].Titre="TeXT"; bar[14].Help=0;
        bar[14].fct=34;
        bar[15].Titre="USeR defined"; bar[15].Help=0;
        bar[15].fct=35;

        menu.cur=0;

        Window(59,2,77,18,0);
        AffColScreen(m);

        do
            {
            retour=PannelMenu(bar,16,&menu);

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[menu.cur].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[menu.cur].fct)-1]=(char)r;
                    }
                while(r!=-1);
                }
            }
        while (retour==2);
        break;



    case 1:     // KeyBar
        bar[0].Titre="Normal"; bar[0].Help=0;
        bar[0].fct=7;

        bar[1].Titre="Bright"; bar[1].Help=0;
        bar[1].fct=6;

        bar[2].Titre="Command Line"; bar[2].Help=0;
        bar[2].fct=64;

        n=0;

        Window(60,3,77,6,0);
        AffColScreen(m);

        do
            {
            menu.cur=n;

            retour=PannelMenu(bar,3,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=(char)r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 2:     // Window 1
        bar[0].Titre="Border1"; bar[0].Help=0;
        bar[0].fct=56;

        bar[1].Titre="Border2"; bar[1].Help=0;
        bar[1].fct=57;

        bar[2].Titre="Normal"; bar[2].Help=0;
        bar[2].fct=17;

        bar[3].Titre="Bright"; bar[3].Help=0;
        bar[3].fct=18;

        bar[4].Titre="Reverse"; bar[4].Help=0;
        bar[4].fct=19;

        n=0;

        Window(59,2,77,8,0);
        AffColScreen(m);

        do
            {
            menu.cur=n;

            retour=PannelMenu(bar,5,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=(char)r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 3:    // Window 2
        bar[0].Titre="Border1"; bar[0].Help=0;
        bar[0].fct=47;

        bar[1].Titre="Border2"; bar[1].Help=0;
        bar[1].fct=48;

        bar[2].Titre="Normal"; bar[2].Help=0;
        bar[2].fct=29;

        bar[3].Titre="Bright"; bar[3].Help=0;
        bar[3].fct=30;

        bar[4].Titre="Reverse"; bar[4].Help=0;
        bar[4].fct=31;

        n=0;

        Window(59,2,77,8,0);
        AffColScreen(m);


        do
            {
            menu.cur=n;

            retour=PannelMenu(bar,5,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=(char)r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );

        Cfg->col[50]=Cfg->col[47];
        Cfg->col[51]=Cfg->col[30];
        break;

    case 4:     // Help
        bar[0].Titre="Border1"; bar[0].Help=0;
        bar[0].fct=53;

        bar[1].Titre="Border2"; bar[1].Help=0;
        bar[1].fct=54;

        bar[2].Titre="Normal"; bar[2].Help=0;
        bar[2].fct=25;

        bar[3].Titre="Bright"; bar[3].Help=0;
        bar[3].fct=26;

        bar[4].Titre="Reverse"; bar[4].Help=0;
        bar[4].fct=27;

        bar[5].Titre="Underline"; bar[5].Help=0;
        bar[5].fct=28;

        bar[6].Titre="Bright Reverse"; bar[6].Help=0;
        bar[6].fct=55;

        n=0;

        Window(59,2,77,10,0);
        AffColScreen(m);

        do
            {
            menu.cur=n;

            retour=PannelMenu(bar,7,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=(char)r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 5:     // Input Box
        bar[0].Titre="Border1"; bar[0].Help=0;
        bar[0].fct=45;

        bar[1].Titre="Border2"; bar[1].Help=0;
        bar[1].fct=46;

        bar[2].Titre="Normal"; bar[2].Help=0;
        bar[2].fct=20;

        bar[3].Titre="Bright"; bar[3].Help=0;
        bar[3].fct=21;

        bar[4].Titre="Reverse"; bar[4].Help=0;
        bar[4].fct=22;

        bar[5].Titre="Button Off"; bar[5].Help=0;
        bar[5].fct=49;

        bar[6].Titre="Button On"; bar[6].Help=0;
        bar[6].fct=50;

        n=0;

        Window(59,2,77,10,0);
        AffColScreen(m);

        do
            {
            menu.cur=n;

            retour=PannelMenu(bar,7,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=(char)r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 6:     // PullDown Bar
        bar[0].Titre="Normal"; bar[0].Help=0;
        bar[0].fct=8;

        bar[1].Titre="Bright"; bar[1].Help=0;
        bar[1].fct=43;

        bar[2].Titre="Reverse"; bar[2].Help=0;
        bar[2].fct=9;

        bar[3].Titre="Bright Reverse"; bar[3].Help=0;
        bar[3].fct=44;

        n=0;

        Window(60,3,75,5,0);
        AffColScreen(m);

        do
            {
            menu.cur=n;

            retour=PannelMenu(bar,4,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=(char)r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 7:     // PullDown Menu
        bar[0].Titre="Border1"; bar[0].Help=0;
        bar[0].fct=10;

        bar[1].Titre="Border2"; bar[1].Help=0;
        bar[1].fct=42;

        bar[2].Titre="Normal"; bar[2].Help=0;
        bar[2].fct=11;

        bar[3].Titre="Bright"; bar[3].Help=0;
        bar[3].fct=12;

        bar[4].Titre="Reverse"; bar[4].Help=0;
        bar[4].fct=13;

        bar[5].Titre="Bright Reverse"; bar[5].Help=0;
        bar[5].fct=14;

        n=0;

        Window(59,2,77,7,0);
        AffColScreen(m);

        do
            {
            menu.cur=n;

            retour=PannelMenu(bar,6,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=(char)r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 8:     // Viewer HTML
        bar[0].Titre="Title"; bar[0].Help=0;
        bar[0].fct=36;

        bar[1].Titre="H1"; bar[1].Help=0;
        bar[1].fct=37;

        bar[2].Titre="H2"; bar[2].Help=0;
        bar[2].fct=51;

        bar[3].Titre="H3"; bar[3].Help=0;
        bar[3].fct=52;

        bar[4].Titre="H4"; bar[4].Help=0;
        bar[4].fct=58;

        bar[5].Titre="H5"; bar[5].Help=0;
        bar[5].fct=59;

        bar[6].Titre="H6"; bar[6].Help=0;
        bar[6].fct=60;

        bar[7].Titre="Bold"; bar[7].Help=0;
        bar[7].fct=61;

        bar[8].Titre="Italic"; bar[8].Help=0;
        bar[8].fct=62;

        bar[9].Titre="Underline"; bar[9].Help=0;
        bar[9].fct=63;

        n=0;

        Window(59,2,77,15,0);
        AffColScreen(m);

        do
            {
            menu.cur=n;

            retour=PannelMenu(bar,10,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=(char)r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;
    }
LoadScreen();
}

void AffColScreen(int a)
{
switch(a)
    {
    case 0:
        Cadre(60,3,76,17,1,Cfg->col[37],Cfg->col[38]);
        ColLin(61,4,15,Cfg->col[0]);
        ColLin(61,5,15,Cfg->col[2]);
        ColLin(61,6,15,Cfg->col[1]);
        ColLin(61,7,15,Cfg->col[4]);
        ColLin(61,8,15,Cfg->col[3]);
        ColLin(61,9,15,Cfg->col[39]);
        ColLin(61,10,15,Cfg->col[40]);

        ColLin(61,11,15,Cfg->col[15]);
        ColLin(61,12,15,Cfg->col[22]);
        ColLin(61,13,15,Cfg->col[23]);
        ColLin(61,14,15,Cfg->col[32]);
        ColLin(61,15,15,Cfg->col[33]);
        ColLin(61,16,15,Cfg->col[34]);

        PrintAt(61,4,"normal  exe");
        PrintAt(61,5,"select  com");
        PrintAt(61,6,"current ini");
        PrintAt(61,7,"c:\\path");
        PrintAt(61,8,"cursel  rb");
        PrintAt(61,9, "normal");
        PrintAt(61,10,"bright");

        PrintAt(61,11,"exe com bat");
        PrintAt(61,12,"rar arj zip");
        PrintAt(61,13,"mod s3m wav");
        PrintAt(61,14,"bmp lbm png");
        PrintAt(61,15,"txt diz doc");
        PrintAt(61,16,"xyz");
        break;
    case 1:
        PrintAt(61,4,"C:\\KKC         ");
        ColLin(61,4,15,Cfg->col[63]);
        ColLin(61,5,2,Cfg->col[5]);
        ColLin(63,5,6,Cfg->col[6]);
        ColLin(69,5,2,Cfg->col[5]);
        ColLin(71,5,6,Cfg->col[6]);
        PrintAt(61,5,"F1 Help F2 Menu");
        break;

    case 2:
        Cadre(60,3,76,7,0,Cfg->col[55],Cfg->col[56]);
        ColLin(61,4,15,Cfg->col[16]);
        ColLin(61,5,15,Cfg->col[17]);
        ColLin(61,6,15,Cfg->col[18]);

        PrintAt(61,4,"Normal");
        PrintAt(61,5,"Bright");
        PrintAt(61,6,"Reverse");
        break;

    case 3:
        Cadre(60,3,76,7,0,Cfg->col[46],Cfg->col[47]);
        ColLin(61,4,15,Cfg->col[28]);
        ColLin(61,5,15,Cfg->col[29]);
        ColLin(61,6,15,Cfg->col[30]);

        PrintAt(61,4,"Normal");
        PrintAt(61,5,"Bright");
        PrintAt(61,6,"Reverse");
        break;

    case 4:
        Cadre(60,3,76,9,0,Cfg->col[52],Cfg->col[53]);
        ColLin(61,4,15,Cfg->col[24]);
        ColLin(61,5,15,Cfg->col[25]);
        ColLin(61,6,15,Cfg->col[26]);
        ColLin(61,7,15,Cfg->col[27]);
        ColLin(61,8,15,Cfg->col[54]);


        PrintAt(61,4,"Normal");
        PrintAt(61,5,"Bright");
        PrintAt(61,6,"Reverse");
        PrintAt(61,7,"Underline");
        PrintAt(61,8,"Bright Reverse");
        break;

    case 5:
        Cadre(60,3,76,9,0,Cfg->col[44],Cfg->col[45]);
        Window(61,4,75,8,Cfg->col[19]);
        ColLin(61,5,15,Cfg->col[20]);
        ColLin(61,6,15,Cfg->col[21]);

        PrintAt(61,4,"Normal");
        PrintAt(61,5,"Bright");
        PrintAt(61,6,"Reverse");

        Puce(62,7,4,2);
        Puce(69,7,5,0);
        PrintAt(63,7,"On");
        PrintAt(70,7,"Off");
        break;

    case 6:
        ColLin(61,4,1,Cfg->col[42]);
        ColLin(62,4,4,Cfg->col[7]);
        ColLin(66,4,1,Cfg->col[43]);
        ColLin(67,4,7,Cfg->col[8]);
        PrintAt(61,4,"Menu Selected");
        break;

    case 7:
        Cadre(60,3,76,6,0,Cfg->col[9],Cfg->col[41]);
        ColLin(61,4,1,Cfg->col[11]);
        ColLin(62,4,14,Cfg->col[10]);
        ColLin(61,5,1,Cfg->col[13]);
        ColLin(62,5,14,Cfg->col[12]);

        PrintAt(61,4,"Menu");
        PrintAt(61,5,"Selected");
        break;

    case 8:
        Cadre(60,3,76,14,0,Cfg->col[37],Cfg->col[38]);
        ColLin(61,4,15,Cfg->col[35]);
        ColLin(61,5,15,Cfg->col[36]);
        ColLin(61,6,15,Cfg->col[50]);
        ColLin(61,7,15,Cfg->col[51]);
        ColLin(61,8,15,Cfg->col[57]);
        ColLin(61,9,15,Cfg->col[58]);
        ColLin(61,10,15,Cfg->col[59]);
        ColLin(61,11,15,Cfg->col[60]);
        ColLin(61,12,15,Cfg->col[61]);
        ColLin(61,13,15,Cfg->col[62]);

        PrintAt(61,4,"Title");
        PrintAt(61,5,"H1");
        PrintAt(61,6,"H2");
        PrintAt(61,7,"H3");
        PrintAt(61,8,"H4");
        PrintAt(61,9,"H5");
        PrintAt(61,10,"H6");
        PrintAt(61,11,"Bold");
        PrintAt(61,12,"Italic");
        PrintAt(61,13,"Underline");
        break;
    }
}

void ChangePalette(void)
{
int col;

int x,y,i;
char rec;

int n,m,ntot;
int nt,mt;
int car;

int ex,ey;
int x1,y1;

int dx,dy;

char *titre="Palette configuration";

#define NBRS 0

SaveScreen();
PutCur(32,0);

Bar(" ----  Save  ----  ----  ----  ----  ----  ----  ----  ---- ");

dx=(Cfg->TailleX-80)/2;
dy=(Cfg->TailleY-25)/2;

if ((dx!=0) | (dy!=0))
    {
    Cadre(0,0,Cfg->TailleX-1,Cfg->TailleY-2,2,Cfg->col[55],Cfg->col[56]);
    Window(1,1,Cfg->TailleX-2,Cfg->TailleY-3,Cfg->col[16]);
    }

Cadre(dx,dy,dx+79,dy+23,2,Cfg->col[55],Cfg->col[56]);
Window(dx+1,dy+1,dx+78,dy+22,Cfg->col[16]);

x1=2;
y1=2;
ey=4;
ex=19;

ntot=15+NBRS;

PrintAt(dx+30,dy+0,titre);

n=0;
m=0;

i=0;

rec=1;

do
{
if (rec==1)
    {
    rec=0;
    for (nt=0;nt<16;nt++)
        {
        x=dx+(nt/4)*11+x1;
        y=dy+(nt%4)*5+y1;     //5

        Cadre(x-1,y-1,x+9,y+3,1,Cfg->col[55],Cfg->col[56]);
        Window(x,y,x+8,y+2,Cfg->col[16]);

        ColWin(x,y,x+8,y+2,nt*16+(15-nt));

        for(mt=0;mt<3;mt++)
            {
            Gradue(x,y+mt,8,0,Cfg->palette[nt*3+mt],64);

            if (Cfg->palette[nt*3+mt]<32)
                PrintAt(x+7,y+mt,"%02d",Cfg->palette[nt*3+mt]);
                else
                PrintAt(x,y+mt,"%02d",Cfg->palette[nt*3+mt]);
            }
        }
    }

if (n<16)
    {
    int somme,num,n1,n2;


    for (n1=0;n1<16;n1++)
        {
        x=dx+(n1/4)*11+x1;
        y=dy+(n1%4)*5+y1;     //5

        somme=0;
        num=0;

        for(n2=0;n2<16;n2++)
            {
            i=abs((Cfg->palette[n1*3+0])-(Cfg->palette[n2*3+0]))+
              abs((Cfg->palette[n1*3+1])-(Cfg->palette[n2*3+1]))+
              abs((Cfg->palette[n1*3+2])-(Cfg->palette[n2*3+2]));

            if (i>somme)
                {
                somme=i;
                num=n2;
                }
            }

        ColWin(x,y,x+8,y+2,n1*16+num);
        }

    i=Cfg->palette[n*3+m];
    if (i!=0) i--;

    x=dx+(n/ey)*11+x1;
    y=dy+(n%ey)*5+y1;

    if (Cfg->palette[n*3+m]<32)
        {
        PrintAt(x,y+m,"  ");
        PrintAt(x+7,y+m,"%02d",Cfg->palette[n*3+m]);
        }
        else
        {
        PrintAt(x,y+m,"%02d",Cfg->palette[n*3+m]);
        PrintAt(x+7,y+m,"  ");
        }

    Gradue(x,y+m,8,i,Cfg->palette[n*3+m],64);

    col=GetCol(x,y+m);

    ColWin(x,y+m,x+8,y+m,(col&15)*16+(col&240)/16);
    car=Wait(0,0);
    ColWin(x,y+m,x+8,y+m,col);

    switch(car)
        {
        case 0x4700:                                             // HOME
            ChrWin(x,y+m,x+8,y+m,32);
            Cfg->palette[n*3+m]=0;
            SetPal(n,Cfg->palette[n*3],Cfg->palette[n*3+1],
                                                   Cfg->palette[n*3+2]);
            break;
        case 0X4F00:                                              // END
            ChrWin(x,y+m,x+8,y+m,32);
            Cfg->palette[n*3+m]=63;
            SetPal(n,Cfg->palette[n*3],Cfg->palette[n*3+1],
                                                   Cfg->palette[n*3+2]);
            break;
        case 80*256:                                              // bas
            m++;
            break;
        case 72*256:                                             // haut
            m--;
            break;
        case 0x4B00:                                           // gauche
            if (Cfg->palette[n*3+m]!=0)
                Cfg->palette[n*3+m]--;
            SetPal(n,Cfg->palette[n*3],Cfg->palette[n*3+1],
                                                   Cfg->palette[n*3+2]);
            break;
        case 0x4D00:                                           // droite
            if (Cfg->palette[n*3+m]!=63)
                Cfg->palette[n*3+m]++;
            SetPal(n,Cfg->palette[n*3],Cfg->palette[n*3+1],
                                                   Cfg->palette[n*3+2]);
            break;
        case 0x0F00:                                        // SHIFT-TAB
            n--;
            break;
        case KEY_F(2):
            SaveCol();
            break;
        case 9:
            n++;
            break;
        }

    if (m==3) n++,m=0;
    if (m<0)  n--,m=2;
    if (n<0)  n=ntot;
    }

if (n<0)        n=ntot;
if (n>ntot)     n=0;

}
while(car!=27);


LoadScreen();
}


void SaveCol(void)
{
FILE *fic;
char title[32];
int size,pos;
int n;

MacAlloc(10,80);
strcpy(_sbuf[10],KKFics->trash);
Path2Abs(_sbuf[10],"temp.kkp");

n=MWinTraite(savecol_kkt);

if (n==-1)
    {
    MacFree(10);
    return;
    }

fic=fopen(_sbuf[10],"wb");
if (fic!=NULL)
    {
    strcpy(title,"Saved color from setup");

    size=0xE9;
    pos=0;

    fprintf(fic,"MCRB");
    fwrite(title,1,32,fic);
    fwrite(&size,1,4,fic);
    fwrite(&pos,1,4,fic);

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x30,0);  // copie 48 bytes dans sbuf[0xFF]
    fwrite(Cfg->palette,1,48,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x0E);      // pal=sbuf[0xFF]

    fprintf(fic,"%c%c%c%c",0x02,0xFF,0x40,0);  // copie 64 bytes dans sbuf[0xFF]
    fwrite(Cfg->col,1,64,fic);
    fprintf(fic,"%c%c%c",0x09,0xFF,0x0F);      // col=sbuf[0xFF]

    fprintf(fic,"%c%c%c%c",0x01,0x43,0x00,0x06);

    fclose(fic);
    }

MacFree(10);
}


/*
struct
   {
   int x,y;
   char *title;
   int length;
   int button;
   } But[]= {
     { 6,6,"     Save    ",13,2 },
     { 6,8,"     Swap    ",13,1 } };
#define NBBUT 2


void ChangePalette(void)
{
int x0,y0;
int n,m;
int car;
int cur;

int todo;
int swap;
int button;

char disppal;

int R,G,B;



button=1;
todo=0;

cur=0;

x0=0;
y0=0;

SaveScreen();
PutCur(32,0);

Cadre(x0,y0,x0+79,y0+24,2,Cfg->col[55],Cfg->col[56]);
Window(x0+1,y0+1,x0+78,y0+23,Cfg->col[16]);

Cadre(x0+1,y0+6,x0+5,y0+23,2,Cfg->col[55],Cfg->col[56]);

for(n=0;n<16;n++)
    AffCol(x0+3,y0+7+n,n*16+((n+3)&15));

PrintAt(x0+6,y0+18,"Color Number:");

Cadre(x0+6,y0+19,x0+71,y0+23,2,Cfg->col[55],Cfg->col[56]);

for(n=0;n<NBBUT;n++)
    {
    PrintAt(x0+But[n].x,y0+But[n].y,But[n].title);
    Puce(x0+But[n].x,y0+But[n].y,13,3);
    }


do
    {



    PrintAt(x0+19,y0+18,"%2d",cur);

    for(n=0;n<64;n++)
        for(m=0;m<3;m++)
            if (n!=Cfg->palette[cur*3+m])
                AffChr(x0+7+n,y0+20+m,32);
                else
                AffChr(x0+7+n,y0+20+m,'#');

    for(n=0;n<16;n++)
        if (n!=cur)
            {
            AffChr(x0+2,y0+7+n,32);
            AffChr(x0+4,y0+7+n,32);
            }
            else
            {
            AffChr(x0+2,y0+7+n,16);
            AffChr(x0+4,y0+7+n,17);
            }

    car=0;

    while ( (!KbHit()) & (car==0) )
        {
#ifndef NOMOUSE
        int xm,ym,zm;

        GetPosMouse(&xm,&ym,&zm);

        if ((zm&1)==1)
            {
            if ((xm>x0+1) & (xm<x0+5) & (ym>y0+6) & (ym<y0+23))
                {
                cur=ym-y0-7;
                car=13;
                }

            for(n=0;n<NBBUT;n++)
                if ((xm>=x0+But[n].x) & (xm<x0+But[n].x+But[n].length) & (ym==y0+But[n].y))
                    {
                    button=But[n].button;
                    car=13;
                    }
            }
#endif
        }

    if (car==0)
        car=Wait(0,0);

    switch(car)
        {
        case 0x4B00: // <-
            break;
        case 0x4D00: // ->
            break;

        case 0x5000:
            cur++;
            if (cur==16) cur=0;
            break;
        case 0x4800:
            cur--;
            if (cur==-1) cur=15;
            break;

        case 0x000D:
            if (todo!=0)
                {
                switch(todo)
                    {
                    case 1:
                        R=Cfg->palette[swap*3+0];
                        G=Cfg->palette[swap*3+1];
                        B=Cfg->palette[swap*3+2];

                        Cfg->palette[swap*3+0]=Cfg->palette[cur*3+0];
                        Cfg->palette[swap*3+1]=Cfg->palette[cur*3+1];
                        Cfg->palette[swap*3+2]=Cfg->palette[cur*3+2];

                        Cfg->palette[cur*3+0]=R;
                        Cfg->palette[cur*3+1]=G;
                        Cfg->palette[cur*3+2]=B;

                        for(n=0;n<64;n++)
                            {
                            R=Cfg->col[n];

                            if ((R&15)==cur) R=(R&240)+swap;
                            else
                            if ((R&15)==swap) R=(R&240)+cur;

                            if ((R/16)==cur) R=(R&15)+swap*16;
                            else
                            if ((R/16)==swap) R=(R&15)+cur*16;

                            Cfg->col[n]=R;
                            }

                        disppal=1;
                        break;
                    }
                todo=0;
                }
                else
                {
                PrintAt(0,0,"Button %d",button);
                Wait(0,0);

                switch(button)
                    {
                    case 1:
                        swap=cur;
                        todo=1;
                        break;
                    case 2:
                        SaveCol();
                        break;
                    }
                }

            break;
        }

    if (disppal)
        Buf2Pal(Cfg->palette);

    disppal=0;
    }
while(car!=27);

LoadScreen();
}
*/

int ChooseFunction(FUNCTION prem)
{
int retour,i;
BARMENU *bar;
MENU menu;
int enter;

int nbrk;

bar=(BARMENU*)GetMem(sizeof(BARMENU)*NBRFUNCT);

menu.cur=0;

nbrk=0;
for(i=0;i<NBRFUNCT;i++)
    {
    if (InfoFct[i].active)
        {
        if (InfoFct[i].nbr==prem)
            menu.cur=nbrk;

        bar[nbrk].fct=InfoFct[i].nbr;

        bar[nbrk].Titre=(char*)GetMem(strlen(InfoFct[i].info)+1);
        strcpy(bar[nbrk].Titre,InfoFct[i].info);
        nbrk++;
        }
    }

menu.x=2;
menu.y=2;
menu.attr=2+8;

retour=PannelMenu(bar,nbrk,&menu);

if (retour!=0)
    enter=bar[menu.cur].fct;
    else
    enter=0;

for(i=0;i<nbrk;i++)
    LibMem(bar[i].Titre);

LibMem(bar);

return enter;
}

void GetKey(int key,char *buffer)
{
sprintf(buffer,"Unknown");

if ((key<=256) & (key>26))
    sprintf(buffer,"<%c>",key);

if (key<=26)
    sprintf(buffer,"<CTRL>+<%c>",key+'A'-1);

switch(key)
    {
    case 27:  strcpy(buffer,"<ESC>");  break;

    case KEY_F(1):   strcpy(buffer,"<F1>");  break;
    case KEY_F(2):   strcpy(buffer,"<F2>");  break;
    case KEY_F(3):   strcpy(buffer,"<F3>");  break;
    case KEY_F(4):   strcpy(buffer,"<F4>");  break;
    case KEY_F(5):   strcpy(buffer,"<F5>");  break;
    case KEY_F(6):   strcpy(buffer,"<F6>");  break;
    case KEY_F(7):   strcpy(buffer,"<F7>");  break;
    case KEY_F(8):   strcpy(buffer,"<F8>");  break;
    case KEY_F(9):   strcpy(buffer,"<F9>");  break;
    case KEY_F(10):  strcpy(buffer,"<F10>"); break;

    case KEY_F(1) + 0x1900:   strcpy(buffer,"<SHIFT>+<F1>");  break;
    case KEY_F(2) + 0x1900:   strcpy(buffer,"<SHIFT>+<F2>");  break;
    case KEY_F(3) + 0x1900:   strcpy(buffer,"<SHIFT>+<F3>");  break;
    case KEY_F(4) + 0x1900:   strcpy(buffer,"<SHIFT>+<F4>");  break;
    case KEY_F(5) + 0x1900:   strcpy(buffer,"<SHIFT>+<F5>");  break;
    case KEY_F(6) + 0x1900:   strcpy(buffer,"<SHIFT>+<F6>");  break;
    case KEY_F(7) + 0x1900:   strcpy(buffer,"<SHIFT>+<F7>");  break;
    case KEY_F(8) + 0x1900:   strcpy(buffer,"<SHIFT>+<F8>");  break;
    case KEY_F(9) + 0x1900:   strcpy(buffer,"<SHIFT>+<F9>");  break;
    case KEY_F(10) + 0x1900:  strcpy(buffer,"<SHIFT>+<F10>"); break;

    case KEY_F(1) + 0x2300:   strcpy(buffer,"<CTRL>+<F1>");  break;
    case KEY_F(2) + 0x2300:   strcpy(buffer,"<CTRL>+<F2>");  break;
    case KEY_F(3) + 0x2300:   strcpy(buffer,"<CTRL>+<F3>");  break;
    case KEY_F(4) + 0x2300:   strcpy(buffer,"<CTRL>+<F4>");  break;
    case KEY_F(5) + 0x2300:   strcpy(buffer,"<CTRL>+<F5>");  break;
    case KEY_F(6) + 0x2300:   strcpy(buffer,"<CTRL>+<F6>");  break;
    case KEY_F(7) + 0x2300:   strcpy(buffer,"<CTRL>+<F7>");  break;
    case KEY_F(8) + 0x2300:   strcpy(buffer,"<CTRL>+<F8>");  break;
    case KEY_F(9) + 0x2300:   strcpy(buffer,"<CTRL>+<F9>");  break;
    case KEY_F(10) + 0x2300:  strcpy(buffer,"<CTRL>+<F10>"); break;

    case KEY_F(1) + 0x2D00:   strcpy(buffer,"<ALT>+<F1>");  break;
    case KEY_F(2) + 0x2D00:   strcpy(buffer,"<ALT>+<F2>");  break;
    case KEY_F(3) + 0x2D00:   strcpy(buffer,"<ALT>+<F3>");  break;
    case KEY_F(4) + 0x2D00:   strcpy(buffer,"<ALT>+<F4>");  break;
    case KEY_F(5) + 0x2D00:   strcpy(buffer,"<ALT>+<F5>");  break;
    case KEY_F(6) + 0x2D00:   strcpy(buffer,"<ALT>+<F6>");  break;
    case KEY_F(7) + 0x2D00:   strcpy(buffer,"<ALT>+<F7>");  break;
    case KEY_F(8) + 0x2D00:   strcpy(buffer,"<ALT>+<F8>");  break;
    case KEY_F(9) + 0x2D00:   strcpy(buffer,"<ALT>+<F9>");  break;
    case KEY_F(10) + 0x2D00:  strcpy(buffer,"<ALT>+<F10>"); break;

    case KEY_F(11) :  strcpy(buffer,"<F11>");  break;
    case KEY_F(12) :  strcpy(buffer,"<F12>");  break;
    case KEY_F(11) + 0x0200:   strcpy(buffer,"<SHIFT>+<F11>");  break;
    case KEY_F(12) + 0x0200:   strcpy(buffer,"<SHIFT>+<F12>");  break;
    case KEY_F(11) + 0x0400:   strcpy(buffer,"<CTRL>+<F11>");  break;
    case KEY_F(12) + 0x0400:   strcpy(buffer,"<CTRL>+<F12>");  break;
    case KEY_F(11) + 0x0600:   strcpy(buffer,"<ALT>+<F11>");  break;
    case KEY_F(12) + 0x0600:   strcpy(buffer,"<ALT>+<F12>");  break;

    case KEY_PPAGE : strcpy(buffer,"<PGUP>");   break;
    case KEY_NPAGE : strcpy(buffer,"<PGDN>");   break;
    case KEY_UP    : strcpy(buffer,"<UP>");     break;
    case KEY_DOWN  : strcpy(buffer,"<DOWN>");   break;
    case KEY_RIGHT : strcpy(buffer,"<RIGHT>");  break;
    case KEY_LEFT  : strcpy(buffer,"<LEFT>");   break;
    case KEY_END   : strcpy(buffer,"<END>");    break;
    case KEY_HOME  : strcpy(buffer,"<HOME>");   break;

//    case KEY_INS: strcpy(buffer,"<INS>"); break;

//    case KEY_A_ENTER: strcpy(buffer,"<ALT>+<ENTER>"); break;

//    case KEY_S_PPAGE : strcpy(buffer,"<SHIFT>+<PGUP>");   break;
//    case KEY_S_NPAGE : strcpy(buffer,"<SHIFT>+<PGDN>");   break;
//    case KEY_S_UP    : strcpy(buffer,"<SHIFT>+<UP>");     break;
//    case KEY_S_DOWN  : strcpy(buffer,"<SHIFT>+<DOWN>");   break;
//    case KEY_S_RIGHT : strcpy(buffer,"<SHIFT>+<RIGHT>");  break;
//    case KEY_S_LEFT  : strcpy(buffer,"<SHIFT>+<LEFT>");   break;
//    case KEY_S_END   : strcpy(buffer,"<SHIFT>+<END>");    break;
//    case KEY_S_HOME  : strcpy(buffer,"<SHIFT>+<HOME>");   break;

//    case KEY_C_PPAGE : strcpy(buffer,"<CTRL>+<PGUP>");   break;
//    case KEY_C_NPAGE : strcpy(buffer,"<CTRL>+<PGDN>");   break;
    case KEY_C_UP    : strcpy(buffer,"<CTRL>+<UP>");     break;
//    case KEY_C_DOWN  : strcpy(buffer,"<CTRL>+<DOWN>");   break;
    case KEY_C_RIGHT : strcpy(buffer,"<CTRL>+<RIGHT>");  break;
    case KEY_C_LEFT  : strcpy(buffer,"<CTRL>+<LEFT>");   break;
//    case KEY_C_END   : strcpy(buffer,"<CTRL>+<END>");    break;
//    case KEY_C_HOME  : strcpy(buffer,"<CTRL>+<HOME>");   break;

//    case KEY_A_PPAGE : strcpy(buffer,"<ALT>+<PGUP>");   break;
//    case KEY_A_NPAGE : strcpy(buffer,"<ALT>+<PGDN>");   break;
//    case KEY_A_UP    : strcpy(buffer,"<ALT>+<UP>");     break;
//    case KEY_A_DOWN  : strcpy(buffer,"<ALT>+<DOWN>");   break;
//    case KEY_A_RIGHT : strcpy(buffer,"<ALT>+<RIGHT>");  break;
//    case KEY_A_LEFT  : strcpy(buffer,"<ALT>+<LEFT>");   break;
//    case KEY_A_END   : strcpy(buffer,"<ALT>+<END>");    break;
//    case KEY_A_HOME  : strcpy(buffer,"<ALT>+<HOME>");   break;

    }
}

int sort_kfct(const void *a,const void *b)
{
struct _keydef *a1,*b1;

a1=(struct _keydef*)a;
b1=(struct _keydef*)b;

return (a1->scan)-(b1->scan);
}


void ModifKey(void)
{
char keybuf[32];
FUNCTION fct;

int car,y;

int n,m;
int info;

int curr,prem;

int nbrkeydef;

for(nbrkeydef=0;KKCfg->keydef[nbrkeydef].scan!=0;nbrkeydef++);
qsort((void*)(KKCfg->keydef),nbrkeydef,sizeof(struct _keydef),sort_kfct);

SaveScreen();
PutCur(32,0);

Bar(" Help   New   Del  ----  ----  ----  ----  ----  ----  ---- ");

info=0;

y=3;

PrintAt(2,0,"%-77s","List of the format");

prem=0;
curr=0;

do
{
if (curr<0) curr=0;
if (curr>=nbrkeydef) curr=nbrkeydef-1;

if (curr<prem+1) prem=curr-1;
while (curr>=prem+(Cfg->TailleY-6)) prem=curr-(Cfg->TailleY-6)+1;

if (prem<0)
    prem=0;
if (prem>nbrkeydef-(Cfg->TailleY-6))
    prem=nbrkeydef-(Cfg->TailleY-6);

y=3;

for (n=prem;n<nbrkeydef;n++)
    {
    int fct;

    fct=(KKCfg->keydef[n].fct)-1;

    m= (n&1==1) ? 16 : 28;  // y

    ColLin(1,y,Cfg->TailleX-2,Cfg->col[m]);

    GetKey(KKCfg->keydef[n].scan,keybuf);


    PrintAt(1,y," %3d %-40s %04X %-27s",
                           InfoFct[fct].nbr,
                           InfoFct[fct].info,
                           KKCfg->keydef[n].scan,
                           keybuf
                           );

    if (curr==n)
        ColLin(1,y,Cfg->TailleX-2,Cfg->col[30]);

    y++;

    if ( (y==(Cfg->TailleY-3)) | (n==nbrkeydef-1) ) break;
    }

car=Wait(0,0);


switch(car)
    {
    case 72*256:                                                   // UP
        curr--;
        break;
    case 80*256:                                                 // DOWN
        curr++;
        break;
    case 0x4900:                                                 // PGUP
        curr-=10;
        break;
    case 0x5100:                                                 // PGDN
        curr+=10;
        break;
    case 0x4700:                                                 // HOME
        curr=0;
        break;
    case 0X4F00:                                                  // END
        curr=nbrkeydef-1;
        break;
    case 0x3B00:
        HelpTopic(1);
        break;
    case 0x3C00: //--- F2 ----------------------------------------------
        for(n=nbrkeydef-1;n>=curr;n--)  //--- Insere -------------------
            memcpy(&(KKCfg->keydef[n+1]),&(KKCfg->keydef[n]),sizeof(struct _keydef));
        nbrkeydef++;
        KKCfg->keydef[curr].fct=1;
        KKCfg->keydef[curr].scan=KEY_F(1);
        break;
    case 0x3D00: //--- F3 ----------------------------------------------
        for(n=curr;n<nbrkeydef-1;n++)  //--- Delete --------------------
            memcpy(&(KKCfg->keydef[n]),&(KKCfg->keydef[n+1]),sizeof(struct _keydef));
        nbrkeydef--;

        break;
    case 13:
        fct=(FUNCTION)ChooseFunction(KKCfg->keydef[curr].fct);
        if (fct!=0)
            {
            int i,j;

            KKCfg->keydef[curr].fct=fct;
            GetKey(KKCfg->keydef[curr].scan,keybuf);
            PrintAt(52,3+curr-prem,"? %s",keybuf);
            ColLin(52,3+curr-prem,27,Cfg->col[(curr&1==1) ? 17 : 29]);

            KKCfg->keydef[curr].scan=(ushort)Wait(52,3+curr-prem);

            i=KKCfg->keydef[curr].scan;

            for(nbrkeydef=0;KKCfg->keydef[nbrkeydef].scan!=0;nbrkeydef++);
            qsort((void*)(KKCfg->keydef),nbrkeydef,sizeof(struct _keydef),sort_kfct);

            for(j=0;j<nbrkeydef;j++)
                if (KKCfg->keydef[j].scan==i)
                    curr=j;
            }
        break;
    }
}
while (car!=27);

LoadScreen();
}

// Helptopic

// 1: idflist


