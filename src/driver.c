/*--------------------------------------------------------------------*\
|- gestion de lecture des fichiers dans les directories               -|
\*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <bios.h>
#include <fcntl.h>
#include <io.h>
#include <ctype.h>
#include <dos.h>

#include "kk.h"

/*--------------------------------------------------------------------*\
|-  Les fonctions '???litfic()'                                       -|
|-      renvoient 1 en cas d'erreur                                   -|
\*--------------------------------------------------------------------*/

void StartWinArc(void);
int InterWinArc(int debut,int nbr,int n);
void CloseWinArc(void);

int xarc,debarc;
FILE *fic;
int LngCopy;

void StartWinArc(void)
{
int x,y;

SaveScreen();

xarc=(Cfg->TailleX-30)/2;

Window(xarc,6,xarc+31,11,10*16+1);
WinCadre(xarc-1,5,xarc+32,12,0);

WinCadre(xarc+1,6,xarc+30,8,1);

PrintAt(xarc+8,5,"Reading Archive");

x=xarc+11;
y=10;

PrintAt(x,y,"    STOP    ");
AffChr(x,y,16);
AffChr(x+11,y,17);

AffChr(x+12,y,220);
ChrLin(x+1,y+1,12,223);

ColLin(x,y,12,2*16+5);                                       // Couleur

debarc=0;

fic=fopen(DFen->VolName,"rb");

LngCopy=filelength(fileno(fic));

DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrfic=2;

DFen->taillesel=0;
DFen->nbrsel=0;

DFen->F[0]=GetMem(sizeof(struct file));
DFen->F[0]->name=GetMem(3);
strcpy(DFen->F[0]->name,"..");

DFen->F[0]->size=0;
DFen->F[0]->time=0;
DFen->F[0]->date=33;
DFen->F[0]->attrib=0x10;
DFen->F[0]->select=0;

DFen->nbrfic=1;

if (KKCfg->pntrep==1)
    {
    DFen->F[1]=GetMem(sizeof(struct file));
    DFen->F[1]->name=GetMem(2);
    strcpy(DFen->F[1]->name,".");

    DFen->F[1]->size=0;
    DFen->F[1]->time=0;
    DFen->F[1]->date=33;
    DFen->F[1]->attrib=0x10;
    DFen->F[1]->select=0;

    DFen->nbrfic=2;
    }
}

int InterWinArc(int debut,int nbr,int n)
{
if (n!=0) LngCopy=n;

PrintAt(xarc+4,9,"%3d files found   %3d %%",nbr,(debut*100)/LngCopy);

debarc=LongGradue(xarc+2,7,28,debarc,debut,LngCopy);

if (KbHit())
    {
    if (Wait(0,0,0)==13) return 1;
    }
return 0;
}


void CloseWinArc(void)
{
fclose(fic);

LoadScreen();
}





/*--------------------------------------------------------------------*\
|-                            FICHIER .ARJ                            -|
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
    struct ARJTime Filetemps;
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

struct file **Fic;

Fic=DFen->F;

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
|-  ---fichier normal---                                              -|
\*--------------------------------------------------------------------*/
if ( (n==1) & (Header.FType!=2) )
    {
    Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

    Fic[DFen->nbrfic]->name=GetMem(strlen(Dest)+1);
    memcpy(Fic[DFen->nbrfic]->name,Dest,strlen(Dest)+1);

    Fic[DFen->nbrfic]->size=Header.UnpSize;

    DFen->taillefic+=Header.UnpSize;

    Fic[DFen->nbrfic]->time=(Header.Filetemps.t_tsec)+
               (Header.Filetemps.t_min)*32+
               (Header.Filetemps.t_heure)*2048;

    Fic[DFen->nbrfic]->date=(Header.Filetemps.t_day)+
               (Header.Filetemps.t_mois)*32+
               (Header.Filetemps.t_year)*512;

    Fic[DFen->nbrfic]->attrib=0;

    Fic[DFen->nbrfic]->select=0;

    DFen->nbrfic++;
    }

/*--------------------------------------------------------------------*\
|-  ---On fout un repertoire---                                       -|
\*--------------------------------------------------------------------*/
if ( (!Maskcmp(Nomarch,nom)) & (Header.FType!=2) )
    {
    char cont;

    cont=0;

    strcpy(Dest,Nomarch+strlen(nom) );
    for (m=0;m<strlen(Dest);m++)
        {
        if (Dest[m]=='/')
            {
            cont=1;
            Dest[m]=0;
            }
        }
    if (cont==1)
        {
        for (n=0;n<DFen->nbrfic;n++)
            if (!stricmp(Dest,Fic[n]->name))
                cont=0;

        if (cont==1)
            {
            Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

            Fic[DFen->nbrfic]->name=GetMem(strlen(Dest)+1);
            memcpy(Fic[DFen->nbrfic]->name,Dest,strlen(Dest)+1);

            Fic[DFen->nbrfic]->size=0;

            Fic[DFen->nbrfic]->time=33;

            Fic[DFen->nbrfic]->date=33;

            Fic[DFen->nbrfic]->attrib=0x10;

            Fic[DFen->nbrfic]->select=0;


            DFen->nbrfic++;

            }
        }
    }



if (Header.FType!=2)
    pos=ftell(fic)+Header.PackSize+4+2;     // Passe le CRC et les Zeros
    else
    pos=ftell(fic)+4+2;                     // Passe le CRC et les Zeros
}

CloseWinArc();

if ( ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) | ((KKCfg->pntrep==0)
                                                  & (DFen->nbrfic==1)) )
    {
    strcpy(DFen->path,DFen->VolName);
    Path2Abs(DFen->path,"..");
    return 1;
    }

return 0;
}

/*--------------------------------------------------------------------*\
|-                             FICHIER .RAR                           -|
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
    struct RARTime Filetemps;
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

struct file **Fic;

Fic=DFen->F;

StartWinArc();


/*--------------------------------------------------------------------*\
|-  Okay.                                                             -|
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

    Nomarch[Lt.NomSize]=0;       // contient les fichiers dans l'archive

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
        Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

        Fic[DFen->nbrfic]->name=GetMem(strlen(Dest)+1);
        memcpy(Fic[DFen->nbrfic]->name,Dest,strlen(Dest)+1);

        Fic[DFen->nbrfic]->size=Lt.UnpSize;            // >< Lt.PackSize

        DFen->taillefic+=Lt.UnpSize;

        Fic[DFen->nbrfic]->time=(Lt.Filetemps.t_tsec)+
			   (Lt.Filetemps.t_min)*32+
			   (Lt.Filetemps.t_heure)*2048;

        Fic[DFen->nbrfic]->date=(Lt.Filetemps.t_day)+
			   (Lt.Filetemps.t_mois)*32+
			   (Lt.Filetemps.t_year)*512;

        Fic[DFen->nbrfic]->attrib=Lt.FileAttr;

        Fic[DFen->nbrfic]->select=0;

        DFen->nbrfic++;
        }

	  }

pos=pos+Lt.TeteSize;
if (Lt.Flags & 0x8000) pos+=Lt.PackSize;        //--- LONG_BLOCK -------
}

CloseWinArc();

if ( ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) | ((KKCfg->pntrep==0) &
                                                    (DFen->nbrfic==1)) )
    {
    strcpy(DFen->path,DFen->VolName);
    Path2Abs(DFen->path,"..");
    return 1;
    }

return 0;
}

/*--------------------------------------------------------------------*\
|-                             FICHIER .ZIP                           -|
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
    struct ZIPTime Filetemps;
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

struct file **Fic;

Fic=DFen->F;

StartWinArc();

/*--------------------------------------------------------------------*\
|-  Okay.                                                             -|
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
|-  ---fichier normal---                                              -|
\*--------------------------------------------------------------------*/
if ( (n==1) & (Header.Signature==0x04034B50) )
    {
    Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

    Fic[DFen->nbrfic]->name=GetMem(strlen(Dest)+1);
    memcpy(Fic[DFen->nbrfic]->name,Dest,strlen(Dest)+1);

    Fic[DFen->nbrfic]->size=Header.UnpSize;

    DFen->taillefic+=Header.UnpSize;

    Fic[DFen->nbrfic]->time=(Header.Filetemps.t_tsec)+
               (Header.Filetemps.t_min)*32+
               (Header.Filetemps.t_heure)*2048;

    Fic[DFen->nbrfic]->date=(Header.Filetemps.t_day)+
               (Header.Filetemps.t_mois)*32+
               (Header.Filetemps.t_year)*512;

    Fic[DFen->nbrfic]->attrib=0;

    Fic[DFen->nbrfic]->select=0;

    DFen->nbrfic++;
    }

/*--------------------------------------------------------------------*\
|-  ---On fout un repertoire---                                       -|
\*--------------------------------------------------------------------*/
if ( (!Maskcmp(Nomarch,nom)) & (Header.Signature==0x04034B50) )
    {
    char cont;

    cont=0;

    strcpy(Dest,Nomarch+strlen(nom) );
    for (m=0;m<strlen(Dest);m++)
        {
        if (Dest[m]=='/')
            {
            cont=1;
            Dest[m]=0;
            }
        }
    if (cont==1)
        {
        for (n=0;n<DFen->nbrfic;n++)
            if (!stricmp(Dest,Fic[n]->name))
                cont=0;

        if (cont==1)
            {
            Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

            Fic[DFen->nbrfic]->name=GetMem(strlen(Dest)+1);
            memcpy(Fic[DFen->nbrfic]->name,Dest,strlen(Dest)+1);

            Fic[DFen->nbrfic]->size=0;
            Fic[DFen->nbrfic]->time=33;
            Fic[DFen->nbrfic]->date=33;
            Fic[DFen->nbrfic]->attrib=0x10;
            Fic[DFen->nbrfic]->select=0;

            DFen->nbrfic++;
            }
        }
    }

pos=ftell(fic)+Header.PackSize+Header.ExtraField;
}

CloseWinArc();

if ( (fin==2) | ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) |
                              ((KKCfg->pntrep==0) & (DFen->nbrfic==1)) )
    {
    strcpy(DFen->path,DFen->VolName);
    Path2Abs(DFen->path,"..");
    return 1;
    }

return 0;
}

/*--------------------------------------------------------------------*\
|-                           FICHIER .LHA                             -|
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
    struct LHATime Filetemps;
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

struct file **Fic;

Fic=DFen->F;

StartWinArc();

/*--------------------------------------------------------------------*\
|-  Okay.                                                             -|
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
    {
    Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

    Fic[DFen->nbrfic]->name=GetMem(strlen(Dest)+1);
    memcpy(Fic[DFen->nbrfic]->name,Dest,strlen(Dest)+1);

    Fic[DFen->nbrfic]->size=Header.UnpSize;

    DFen->taillefic+=Header.UnpSize;

    Fic[DFen->nbrfic]->time=(Header.Filetemps.t_tsec)+
               (Header.Filetemps.t_min)*32+
               (Header.Filetemps.t_heure)*2048;

    Fic[DFen->nbrfic]->date=(Header.Filetemps.t_day)+
               (Header.Filetemps.t_mois)*32+
               (Header.Filetemps.t_year)*512;

    Fic[DFen->nbrfic]->attrib=0;

    Fic[DFen->nbrfic]->select=0;

    DFen->nbrfic++;
    }


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
            if (!stricmp(Dest,Fic[n]->name))
                cont=0;

        if (cont==1)
            {
            Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

            Fic[DFen->nbrfic]->name=GetMem(strlen(Dest)+1);
            memcpy(Fic[DFen->nbrfic]->name,Dest,strlen(Dest)+1);

            Fic[DFen->nbrfic]->size=0;
            Fic[DFen->nbrfic]->time=33;
            Fic[DFen->nbrfic]->date=33;
            Fic[DFen->nbrfic]->attrib=0x10;
            Fic[DFen->nbrfic]->select=0;
            DFen->nbrfic++;
            }
        }
    }

pos+=Header.HSize+Header.PackSize+2;     //--- CRC ? -------------------
}

CloseWinArc();

if ( ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) | ((KKCfg->pntrep==0) &
                                                    (DFen->nbrfic==1)) )
    {
    strcpy(DFen->path,DFen->VolName);
    Path2Abs(DFen->path,"..");
    return 1;
    }

return 0;
}


/*--------------------------------------------------------------------*\
|-                           FICHIER .KKD                             -|
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

unsigned char tai;                                    // taille des noms

struct file **Fic;

Fic=DFen->F;

StartWinArc();

if (strlen(DFen->path)==strlen(DFen->VolName))
    {
/*
    Fic[DFen->nbrfic]=GetMem(sizeof(struct file));
    Fic[DFen->nbrfic]->name=GetMem(2);
    strcpy(Fic[DFen->nbrfic]->name,"..");

    Fic[DFen->nbrfic]->size=0;
    Fic[DFen->nbrfic]->time=0;
    Fic[DFen->nbrfic]->date=33;
    Fic[DFen->nbrfic]->attrib=0x10;
    Fic[DFen->nbrfic]->select=0;

    Fic[DFen->nbrfic]->desc=0;

    (DFen->nbrfic)++;
*/
    strcpy(nom,"");
    }
    else
    {
    strcpy(nom,(DFen->path)+strlen(DFen->VolName)+1);
    strcat(nom,"\\");
    }



fseek(fic,3,SEEK_SET);                                   // Passe la cle

fread(&tai,1,1,fic);                                 // Passe la version

fread(&tai,1,1,fic);                           // Passe le nom du volume
fread(Nomarch,tai,1,fic);

fread(&n,4,1,fic);               // Passe le nombre d'octets non utilise
fread(&n,4,1,fic);  // Passe le nbr de bytes non utilise avant reconstr.


i=0;    //--- DEBUT ----------------------------------------------------
j=0;    //--- FIN ------------------------------------------------------

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
        fin=1;      // Un gros probleme: un directory qui ne mene a rien
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
        fin=1;                                   // Directory pas trouv‚
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

            Fic[DFen->nbrfic]=GetMem(sizeof(struct file));
            Fic[DFen->nbrfic]->name=GetMem(tai+1);

            strcpy(Fic[DFen->nbrfic]->name,Nomarch);
            Fic[DFen->nbrfic]->name[tai]=0;

            Fic[DFen->nbrfic]->size=KKD_desc.size;

            DFen->taillefic+=Fic[DFen->nbrfic]->size;

            Fic[DFen->nbrfic]->time=KKD_desc.time;
            Fic[DFen->nbrfic]->date=KKD_desc.date;

            Fic[DFen->nbrfic]->attrib=KKD_desc.attrib;

            Fic[DFen->nbrfic]->select=0;

            if (KKD_desc.desc==0)
                Fic[DFen->nbrfic]->desc=0;
                else
                Fic[DFen->nbrfic]->desc=1;

            DFen->nbrfic++;
            }

        fseek(fic,KKD_desc.next,SEEK_SET);
        }
    while(KKD_desc.next!=0);
    }
    else
    {
    strcpy(DFen->path,DFen->VolName);
    Path2Abs(DFen->path,"..");
    }

CloseWinArc();

return fin;
}

/*--------------------------------------------------------------------*\
|-                           FICHIER .DFP                             -|
\*--------------------------------------------------------------------*/

int DFPlitfic(void)
{
char key[13];

ULONG pos;
char fin;

int n,nbr,lng,deb;

struct file **Fic;

Fic=DFen->F;

StartWinArc();

/*--------------------------------------------------------------------*\
|-  Okay.                                                             -|
\*--------------------------------------------------------------------*/

pos=0;
fin=0;

fseek(fic,5,SEEK_SET);

fread(&nbr,1,4,fic);

for(n=0;n<nbr;n++)
    {
    if (InterWinArc(n,DFen->nbrfic,nbr)==1) break;

    fread(key,1,12,fic);
    fread(&deb,1,4,fic);
    fread(&lng,1,4,fic);

    key[12]=0;

    Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

    Fic[DFen->nbrfic]->name=GetMem(strlen(key)+1);
    memcpy(Fic[DFen->nbrfic]->name,key,strlen(key)+1);

    Fic[DFen->nbrfic]->size=lng;

    DFen->taillefic+=lng;

    Fic[DFen->nbrfic]->time=0;
    Fic[DFen->nbrfic]->date=0;

    Fic[DFen->nbrfic]->attrib=0;

    Fic[DFen->nbrfic]->select=0;

    DFen->nbrfic++;
    }

CloseWinArc();

if ( ((KKCfg->pntrep==1) & (DFen->nbrfic==2)) | ((KKCfg->pntrep==0) &
                                                    (DFen->nbrfic==1)) )
    {
    strcpy(DFen->path,DFen->VolName);
    Path2Abs(DFen->path,"..");
    return 1;
    }

return 0;
}


/*--------------------------------------------------------------------*\
|-  Dos-function                                                      -|
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|-  Retourne 1 si erreur                                              -|
\*--------------------------------------------------------------------*/

int DOSlitfic(void)
{
DIR *dirp;
struct dirent *ff;

struct file **Fic;
char rech[256];

if (chdir(DFen->path)!=0)
    return 1;

Fic=DFen->F;

strcpy(rech,DFen->path);
Path2Abs(rech,"*.*");

DFen->nbrfic=0;
DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrsel=0;

DFen->taillesel=0;

dirp=opendir(rech);

if (dirp!=NULL)
    while(1)
    {
    ff=readdir(dirp);
    if (ff==NULL) break;

    if ( ((KKCfg->pntrep==1) |  (strcmp(ff->d_name,".")!=0)) &
         ((KKCfg->hidfil==1) | (((ff->d_attr)&_A_HIDDEN)!=_A_HIDDEN)) &
         (((ff->d_attr)&_A_VOLID)!=_A_VOLID)
         )
        {
        Fic[DFen->nbrfic]=GetMem(sizeof(struct file));
        Fic[DFen->nbrfic]->name=GetMem(strlen(ff->d_name)+1);
        strcpy(Fic[DFen->nbrfic]->name,ff->d_name);
        Fic[DFen->nbrfic]->time=ff->d_time;
        Fic[DFen->nbrfic]->date=ff->d_date;
        Fic[DFen->nbrfic]->attrib=ff->d_attr;
        Fic[DFen->nbrfic]->select=0;
        Fic[DFen->nbrfic]->size=ff->d_size;
        DFen->taillefic+=Fic[DFen->nbrfic]->size;
        DFen->nbrfic++;
        }
    }

closedir(dirp);


rech[3]=0;

if ( ( (!stricmp(rech,"A:\\")) | (!stricmp(rech,"B:\\")) )
     | (DFen->nbrfic==0) )
    {
    Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

    Fic[DFen->nbrfic]->name=GetMem(4);                  // Pour Reload
    memcpy(Fic[DFen->nbrfic]->name,rech,4);
    Fic[DFen->nbrfic]->time=0;
    Fic[DFen->nbrfic]->date=0;
    Fic[DFen->nbrfic]->attrib=_A_SUBDIR;
    Fic[DFen->nbrfic]->select=0;
    Fic[DFen->nbrfic]->size=0;
    DFen->nbrfic++;
    }

/*--------------------------------------------------------------------*\
|- Test si c'est un CD-ROM                                            -|
\*--------------------------------------------------------------------*/

if (1)
    {
    union REGS R;

    R.w.ax=0x150B;
    R.w.cx=toupper(DFen->path[0])-'A';
    int386(0x2F,&R,&R);

    if ( (R.w.bx==0xADAD) & (R.w.ax!=0) )
        {
        Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

        Fic[DFen->nbrfic]->name=GetMem(4);                // Pour Reload
        memcpy(Fic[DFen->nbrfic]->name,"****",4);
        Fic[DFen->nbrfic]->time=0;
        Fic[DFen->nbrfic]->date=0;
        Fic[DFen->nbrfic]->attrib=_A_SUBDIR;
        Fic[DFen->nbrfic]->select=0;
        Fic[DFen->nbrfic]->size=0;
        DFen->nbrfic++;
        }
    }


ChangeLine();

/*--------------------------------------------------------------------*\
|-  Temporise le temps … attendre avant le lecture du header OK ;-}   -|
\*--------------------------------------------------------------------*/

switch(toupper(DFen->path[0])-'A')
    {
    case 2:     //--- C: -----------------------------------------------
        DFen->IDFSpeed=2*18;
        break;
    default:
        DFen->IDFSpeed=5*18;
        break;
   }

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


Fichier=GetMem(50000*sizeof(void*));
Desc=GetMem(50000*sizeof(long));

Repert=GetMem(5000*sizeof(void*));


TabRec=GetMem(500*sizeof(char*));
TabRec[0]=GetMem(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

Nbdir=0;
Nbfic=0;


SaveScreen();
ColLin(0,0,80,1*16+4);

oldpos=3+1+256+4+4;

Desc[NbrRec-1]=0; //--- pour la boucle, sera ecraser plus tard ---------

do
{
CurRec=NbrRec-1;

if (Fichier[Desc[CurRec]]!=NULL)
    Fichier[Desc[CurRec]]->desc=oldpos;

Repert[Nbdir]=GetMem(strlen(TabRec[CurRec])-1);
memcpy(Repert[Nbdir],(TabRec[CurRec])+2,strlen(TabRec[CurRec])-1);

Nbdir++;

PrintAt(0,0,"Go in  %-73s",TabRec[CurRec]);

strcpy(nomtemp,TabRec[CurRec]);
Path2Abs(nomtemp,"*.*");

strcpy(nom,TabRec[CurRec]);

/*--------------------------------------------------------------------*\
|-  The files                                                         -|
\*--------------------------------------------------------------------*/

dirp=opendir(nomtemp);

if (dirp!=NULL)
    while(1)
    {
    ff=readdir(dirp);
    if (ff==NULL) break;

    if  (strcmp(ff->d_name,".")!=0)
        {
        error=ff->d_attr;

        if ((error&0x10)!=0x10)    //--- Not a Subdir ------------------
            {
            Fichier[Nbfic]->desc=0;

            Fichier[Nbfic]=GetMem(sizeof(struct HeaderKKD));

            Fichier[Nbfic]->nom=GetMem(strlen(ff->d_name)+1);
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
|-  The directories                                                   -|
\*--------------------------------------------------------------------*/

dirp=opendir(nomtemp);

if (dirp!=NULL)
    while(1)
    {
    ff=readdir(dirp);
    if (ff==NULL) break;

    if  (strcmp(ff->d_name,".")!=0)
        {
        error=ff->d_attr;

        if ((error&0x10)==0x10)    //--- Not a Subdir ------------------
            {
            Fichier[Nbfic]->desc=0; //--- On le fera plus tard; --------

            Fichier[Nbfic]=GetMem(sizeof(struct HeaderKKD));

            Fichier[Nbfic]->nom=GetMem(strlen(ff->d_name)+1);
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

                TabRec[NbrRec]=GetMem(strlen(moi)+1);
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
fwrite(&tai,1,1,fic);       //--- VERSION ------------------------------
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
    unsigned char tai;

    tai=strlen(Fichier[n]->nom);
    fwrite(&tai,1,1,fic);                   //--- 2 --------------------
    fwrite(Fichier[n]->nom,tai,1,fic);      //--- t --------------------
    fwrite(&Fichier[n]->desc,4,1,fic);      //--- desc -----------------
    fwrite(&Fichier[n]->next,4,1,fic);      //--- suiv -----------------
    fwrite(&Fichier[n]->size,4,1,fic);      //--- 4 --------------------
    fwrite(&Fichier[n]->time,2,1,fic);      //--- 2 --------------------
    fwrite(&Fichier[n]->date,2,1,fic);      //--- 2 --------------------
    fwrite(&Fichier[n]->attrib,1,1,fic);    //--- 1 --------------------

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
}
