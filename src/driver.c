#include <errno.h>

#include <stdlib.h>
#include <malloc.h>
#include <dos.h>
#include <string.h>
#include <direct.h>


#include <fcntl.h>
#include <io.h>

#include "kk.h"

/******************************************************************************
                               FICHIER .ARJ
 *****************************************************************************/

struct ARJTime {
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
int handle;

int n,m;

struct file **Fic;


DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrfic=2;

DFen->taillesel=0;
DFen->nbrsel=0;

Fic=DFen->F;

Fic[0]->name=GetMem(2);
strcpy(Fic[0]->name,".");

Fic[0]->size=0;
Fic[0]->time=0;
Fic[0]->date=33;
Fic[0]->attrib=0x10;
Fic[0]->select=0;

Fic[1]->name=GetMem(3);
strcpy(Fic[1]->name,"..");

Fic[1]->size=0;
Fic[1]->time=0;
Fic[1]->date=33;
Fic[1]->attrib=0x10;
Fic[1]->select=0;

DFen->nbrfic=2;

handle=open(DFen->VolName,O_RDONLY | O_BINARY);

// Okay.

pos=0;
fin=0;

while(!fin)
{
char car;
short lng;

lseek(handle,pos,SEEK_SET);

if (read(handle,&Header,sizeof(struct ARJHeader))!=sizeof(struct ARJHeader))
    {
    fin=1;
    break;
    }

lng=0;
do
    {
    read(handle,&car,1);
    nom[lng]=car;
    lng++;
    }
while(car!=0);

do
    read(handle,&car,1);
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

if ( (n==1) & (Header.FType!=2) )
    {
    Fic[DFen->nbrfic]=malloc(sizeof(struct file));

    Fic[DFen->nbrfic]->name=malloc(strlen(Dest)+1);
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


if ( (!Maskcmp(Nomarch,nom)) & (Header.FType!=2) )
    {
    char cont;

    cont=0;

    strcpy(Dest,Nomarch);
    for (m=strlen(nom);m<strlen(Dest);m++)
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
            Fic[DFen->nbrfic]=malloc(sizeof(struct file));

            Fic[DFen->nbrfic]->name=malloc(strlen(Dest)+1);
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
    pos=tell(handle)+Header.PackSize+4+2;   // Passe le CRC et les Zeros
    else
    pos=tell(handle)+4+2;   // Passe le CRC et les Zeros
}

close(handle);

DFen->init=1;

if (DFen->nbrfic==2)
    {
    strcpy(DFen->path,DFen->VolName);
    return 1;
    }

InfoSupport();
ChangeLine();

return 0;
}

void InstallARJ(void)
{
strcpy(DFen->Tpath,DFen->path);

strcpy(DFen->VolName,DFen->path);
Path2Abs(DFen->VolName,DFen->F[DFen->pcur]->name);

strcpy(DFen->path,DFen->VolName);

DFen->system=2;
}


/******************************************************************************
                               FICHIER .RAR
 *****************************************************************************/

struct RARTime {
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
int handle;

struct file **Fic;

DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrfic=2;

DFen->taillesel=0;
DFen->nbrsel=0;

Fic=DFen->F;

Fic[0]->name=GetMem(2);
strcpy(Fic[0]->name,".");

Fic[0]->size=0;
Fic[0]->time=0;
Fic[0]->date=33;
Fic[0]->attrib=0x10;
Fic[0]->select=0;

Fic[1]->name=GetMem(3);
strcpy(Fic[1]->name,"..");

Fic[1]->size=0;
Fic[1]->time=0;
Fic[1]->date=33;
Fic[1]->attrib=0x10;
Fic[1]->select=0;

DFen->nbrfic=2;

handle=open(DFen->VolName,O_RDONLY | O_BINARY);


// Okay.

pos=tell(handle);
fin=0;

while(!fin)
{
lseek(handle,pos,SEEK_SET);

if ( (read(handle,&Lt,32)!=32) |
	 (Lt.TeteType<0x72) | (Lt.TeteType>0x75) | (Lt.TeteSize<7) )  {
		close(handle);
		handle=-1;
		fin=1;
		break;
		}

if (Lt.TeteType==0x74) {
    int n;

    lseek(handle,pos+32,SEEK_SET);
    read(handle,Nomarch,Lt.NomSize);

    Nomarch[Lt.NomSize]=0;          // contient les fichiers dans l'archive

    if (strlen(DFen->path)==strlen(DFen->VolName))
        strcpy(nom,"");
        else
        {
        strcpy(nom,(DFen->path)+strlen(DFen->VolName)+1);
        strcat(nom,"\\");
        }

    n=find1st(Nomarch,Dest,nom);

    if (n==1) {
        Fic[DFen->nbrfic]=malloc(sizeof(struct file));

        Fic[DFen->nbrfic]->name=malloc(strlen(Dest)+1);
        memcpy(Fic[DFen->nbrfic]->name,Dest,strlen(Dest)+1);

        Fic[DFen->nbrfic]->size=Lt.UnpSize;      // >< Lt.PackSize

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
if (Lt.Flags & 0x8000) pos+=Lt.PackSize;		// LONG_BLOCK
}

DFen->init=1;

if (DFen->nbrfic==2)
    {
    strcpy(DFen->path,DFen->VolName);
    return 1;
    }

SortFic(DFen);
AffFen(DFen);
InfoSupport();
ChangeLine();

return 0;
}

void InstallRAR(void)
{
strcpy(DFen->Tpath,DFen->path);

strcpy(DFen->VolName,DFen->path);
Path2Abs(DFen->VolName,DFen->F[DFen->pcur]->name);

strcpy(DFen->path,DFen->VolName);

DFen->system=1;
}

/******************************************************************************
                               FICHIER .ZIP
 *****************************************************************************/

struct ZIPTime {
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
int handle;

int n,m;

struct file **Fic;


DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrfic=2;

DFen->taillesel=0;
DFen->nbrsel=0;

Fic=DFen->F;

Fic[0]->name=GetMem(2);
strcpy(Fic[0]->name,".");

Fic[0]->size=0;
Fic[0]->time=0;
Fic[0]->date=33;
Fic[0]->attrib=0x10;
Fic[0]->select=0;

Fic[1]->name=GetMem(3);
strcpy(Fic[1]->name,"..");

Fic[1]->size=0;
Fic[1]->time=0;
Fic[1]->date=33;
Fic[1]->attrib=0x10;
Fic[1]->select=0;

DFen->nbrfic=2;

handle=open(DFen->VolName,O_RDONLY | O_BINARY);

// Okay.

pos=0;
fin=0;

while(!fin)
{
lseek(handle,pos,SEEK_SET);

if (read(handle,&Header,sizeof(struct ZIPHeader))!=sizeof(struct ZIPHeader))
    {
    fin=1;
    break;
    }

read(handle,nom,Header.FNameLen);
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

if ( (n==1) & (Header.Signature==0x04034B50) )
    {
    Fic[DFen->nbrfic]=malloc(sizeof(struct file));

    Fic[DFen->nbrfic]->name=malloc(strlen(Dest)+1);
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


if ( (!Maskcmp(Nomarch,nom)) & (Header.Signature==0x04034B50) )
    {
    char cont;

    cont=0;

    strcpy(Dest,Nomarch);
    for (m=strlen(nom);m<strlen(Dest);m++)
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
            Fic[DFen->nbrfic]=malloc(sizeof(struct file));

            Fic[DFen->nbrfic]->name=malloc(strlen(Dest)+1);
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



pos=tell(handle)+Header.PackSize+Header.ExtraField;
}

close(handle);

DFen->init=1;

if (DFen->nbrfic==2)
    {
    strcpy(DFen->path,DFen->VolName);
    return 1;
    }

InfoSupport();
ChangeLine();

return 0;
}

void InstallZIP(void)
{
strcpy(DFen->Tpath,DFen->path);

strcpy(DFen->VolName,DFen->path);
Path2Abs(DFen->VolName,DFen->F[DFen->pcur]->name);

strcpy(DFen->path,DFen->VolName);

DFen->system=3;
}


/******************************************************************************
                               FICHIER .LHA
 *****************************************************************************/

struct LHATime {
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
int handle;

int n,m;

struct file **Fic;


DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrfic=2;

DFen->taillesel=0;
DFen->nbrsel=0;

Fic=DFen->F;

Fic[0]->name=GetMem(2);
strcpy(Fic[0]->name,".");

Fic[0]->size=0;
Fic[0]->time=0;
Fic[0]->date=33;
Fic[0]->attrib=0x10;
Fic[0]->select=0;

Fic[1]->name=GetMem(3);
strcpy(Fic[1]->name,"..");

Fic[1]->size=0;
Fic[1]->time=0;
Fic[1]->date=33;
Fic[1]->attrib=0x10;
Fic[1]->select=0;

DFen->nbrfic=2;

handle=open(DFen->VolName,O_RDONLY | O_BINARY);

// Okay.

pos=0;
fin=0;

while(!fin)
{
lseek(handle,pos,SEEK_SET);

if (read(handle,&Header,sizeof(struct LHAHeader))!=sizeof(struct LHAHeader))
    {
    fin=1;
    break;
    }

read(handle,Nomarch,Header.FNameLen);
Nomarch[Header.FNameLen]=0;

lseek(handle,pos,SEEK_SET);

read(handle,nom,128);



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
    Fic[DFen->nbrfic]=malloc(sizeof(struct file));

    Fic[DFen->nbrfic]->name=malloc(strlen(Dest)+1);
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

    strcpy(Dest,Nomarch);
    for (m=strlen(nom);m<strlen(Dest);m++)
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
            Fic[DFen->nbrfic]=malloc(sizeof(struct file));

            Fic[DFen->nbrfic]->name=malloc(strlen(Dest)+1);
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

pos+=Header.HSize+Header.PackSize+2;     // CRC ?
}

close(handle);

DFen->init=1;

if (DFen->nbrfic==2)
    {
    strcpy(DFen->path,DFen->VolName);
    return 1;
    }

InfoSupport();
ChangeLine();

return 0;
}

void InstallLHA(void)
{
strcpy(DFen->Tpath,DFen->path);

strcpy(DFen->VolName,DFen->path);
Path2Abs(DFen->VolName,DFen->F[DFen->pcur]->name);

strcpy(DFen->path,DFen->VolName);

DFen->system=4;
}
