#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <direct.h>

#include <bios.h>

#include <fcntl.h>
#include <io.h>

#include "win.h"
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

Fic[0]->name=GetMem(3);
strcpy(Fic[0]->name,"..");

Fic[0]->size=0;
Fic[0]->time=0;
Fic[0]->date=33;
Fic[0]->attrib=0x10;
Fic[0]->select=0;

DFen->nbrfic=1;

if (Cfg->pntrep==1)
    {
    Fic[1]->name=GetMem(2);
    strcpy(Fic[1]->name,".");

    Fic[1]->size=0;
    Fic[1]->time=0;
    Fic[1]->date=33;
    Fic[1]->attrib=0x10;
    Fic[1]->select=0;

    DFen->nbrfic=2;
    }


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

InfoSupport();  // Selection vitesse du support

return 0;
}

void InstallARJ(void)
{
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

Fic[0]->name=GetMem(3);
strcpy(Fic[0]->name,"..");

Fic[0]->size=0;
Fic[0]->time=0;
Fic[0]->date=33;
Fic[0]->attrib=0x10;
Fic[0]->select=0;

DFen->nbrfic=1;

if (Cfg->pntrep==1)
    {
    Fic[1]->name=GetMem(2);
    strcpy(Fic[1]->name,".");

    Fic[1]->size=0;
    Fic[1]->time=0;
    Fic[1]->date=33;
    Fic[1]->attrib=0x10;
    Fic[1]->select=0;

    DFen->nbrfic=2;
    }

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
        Fic[DFen->nbrfic]=GetMem(sizeof(struct file));

        Fic[DFen->nbrfic]->name=GetMem(strlen(Dest)+1);
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

InfoSupport();

return 0;
}

void InstallRAR(void)
{
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

Fic[0]->name=GetMem(3);
strcpy(Fic[0]->name,"..");

Fic[0]->size=0;
Fic[0]->time=0;
Fic[0]->date=33;
Fic[0]->attrib=0x10;
Fic[0]->select=0;

DFen->nbrfic=1;

if (Cfg->pntrep==1)
    {
    Fic[1]->name=GetMem(2);
    strcpy(Fic[1]->name,".");

    Fic[1]->size=0;
    Fic[1]->time=0;
    Fic[1]->date=33;
    Fic[1]->attrib=0x10;
    Fic[1]->select=0;

    DFen->nbrfic=2;
    }

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

return 0;
}

void InstallZIP(void)
{
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

Fic[0]->name=GetMem(3);
strcpy(Fic[0]->name,"..");

Fic[0]->size=0;
Fic[0]->time=0;
Fic[0]->date=33;
Fic[0]->attrib=0x10;
Fic[0]->select=0;

DFen->nbrfic=1;

if (Cfg->pntrep==1)
    {
    Fic[1]->name=GetMem(2);
    strcpy(Fic[1]->name,".");

    Fic[1]->size=0;
    Fic[1]->time=0;
    Fic[1]->date=33;
    Fic[1]->attrib=0x10;
    Fic[1]->select=0;

    DFen->nbrfic=2;
    }

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

return 0;
}

void InstallLHA(void)
{
strcpy(DFen->VolName,DFen->path);
Path2Abs(DFen->VolName,DFen->F[DFen->pcur]->name);

strcpy(DFen->path,DFen->VolName);

DFen->system=4;
}

/******************************************************************************
                               FICHIER .KKD
 *****************************************************************************/

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
int handle;


int n;

int i,j;

unsigned char tai;  // taille des noms

struct file **Fic;

DFen->pcur=0;
DFen->scur=0;

DFen->taillefic=0;
DFen->nbrfic=2;

DFen->taillesel=0;
DFen->nbrsel=0;

Fic=DFen->F;

DFen->nbrfic=0;

if (Cfg->pntrep==1)
    {
    Fic[1]->name=GetMem(2);
    strcpy(Fic[1]->name,".");

    Fic[1]->size=0;
    Fic[1]->time=0;
    Fic[1]->date=33;
    Fic[1]->attrib=0x10;
    Fic[1]->select=0;

    DFen->nbrfic=1;
    }

if (strlen(DFen->path)==strlen(DFen->VolName))
    {
    Fic[1]->name=GetMem(2);
    strcpy(Fic[1]->name,"..");

    Fic[1]->size=0;
    Fic[1]->time=0;
    Fic[1]->date=33;
    Fic[1]->attrib=0x10;
    Fic[1]->select=0;

    (DFen->nbrfic)++;
    strcpy(nom,"");
    }
    else
    {
    strcpy(nom,(DFen->path)+strlen(DFen->VolName)+1);
    strcat(nom,"\\");
    }

handle=open(DFen->VolName,O_RDONLY | O_BINARY);

lseek(handle,3,SEEK_SET);   // Passe la cle

read(handle,&tai,1);        // Passe la version

read(handle,&tai,1);        // Passe le nom du volume
read(handle,Nomarch,tai);   

read(handle,&n,4);          // Passe le nombre d'octets non utilise
read(handle,&n,4);          // Passe le nombre d'octets non utilise avant reconstruction


i=0;    // DEBUT
j=0;    // FIN

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
read(handle,&tai,1);
read(handle,Nomarch,tai);
Nomarch[tai]=0;

read(handle,&KKD_desc,sizeof(struct kkdesc));

if ( (!stricmp(Nomarch,name)) & ((KKD_desc.attrib&0x10)==0x10) )
    {
    if (KKD_desc.desc==0)
        {
        fin=1;          // Un gros probleme: un directory qui ne mene a rien
        break;
        }
    lseek(handle,KKD_desc.desc,SEEK_SET);

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
        fin=1;          // Directory pas trouv‚
        break;
        }
    lseek(handle,KKD_desc.next,SEEK_SET);
    }
}


if (fin==0)
    {
    do
        {
        read(handle,&tai,1);
        read(handle,Nomarch,tai);
        Nomarch[tai]=0;

        read(handle,&KKD_desc,sizeof(struct kkdesc));


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

        DFen->nbrfic++;

        lseek(handle,KKD_desc.next,SEEK_SET);
        }
    while(KKD_desc.next!=0);
    }

close(handle);
DFen->init=1;

if (fin==1)
    {
    strcpy(DFen->path,DFen->VolName);
    return 1;
    }

InfoSupport();

return 0;
}

void InstallKKD(void)
{
strcpy(DFen->VolName,DFen->path);
Path2Abs(DFen->VolName,DFen->F[DFen->pcur]->name);

strcpy(DFen->path,DFen->VolName);

DFen->system=5;
}

/**************************/

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

void MakeKKD(struct fenetre *Fen,char *ficname)
{
struct find_t ff;
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


SaveEcran();
ColLin(0,0,80,1*16+4);

oldpos=3+1+256+4+4;

Desc[NbrRec-1]=0; // pour la boucle, sera ecraser plus tard

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

// The files

error=_dos_findfirst(nomtemp,63,&ff);

while (error==0)
    {
    if  (strcmp(ff.name,".")!=0)
        {
        error=ff.attrib;

        if ((error&0x10)!=0x10)    // Not a Subdir
            {
            Fichier[Nbfic]->desc=0;

            Fichier[Nbfic]=GetMem(sizeof(struct HeaderKKD));

            Fichier[Nbfic]->nom=GetMem(strlen(ff.name)+1);
            memcpy(Fichier[Nbfic]->nom,ff.name,strlen(ff.name)+1);
            Fichier[Nbfic]->size=ff.size;
            Fichier[Nbfic]->time=ff.wr_time;
            Fichier[Nbfic]->date=ff.wr_date;
            Fichier[Nbfic]->attrib=ff.attrib;

            oldpos+=18+strlen(ff.name);

            Fichier[Nbfic]->next=oldpos;

            Nbfic++;
            }
        }

    error=_dos_findnext(&ff);
    }

free(TabRec[CurRec]);
NbrRec--;

// The directories

error=_dos_findfirst(nomtemp,63,&ff);

while (error==0)
    {
    if  (strcmp(ff.name,".")!=0)
        {
        error=ff.attrib;

        if ((error&0x10)==0x10)    // Not a Subdir
            {
            Fichier[Nbfic]->desc=0; // On le fera plus tard;

            Fichier[Nbfic]=GetMem(sizeof(struct HeaderKKD));

            Fichier[Nbfic]->nom=GetMem(strlen(ff.name)+1);
            memcpy(Fichier[Nbfic]->nom,ff.name,strlen(ff.name)+1);
            Fichier[Nbfic]->size=ff.size;
            Fichier[Nbfic]->time=ff.wr_time;
            Fichier[Nbfic]->date=ff.wr_date;
            Fichier[Nbfic]->attrib=ff.attrib;

            oldpos+=18+strlen(ff.name);

            Fichier[Nbfic]->next=oldpos;

            if (ff.name[0]!='.')
                {
                strcpy(moi,nom);
                Path2Abs(moi,ff.name);

                Desc[NbrRec]=Nbfic;

                TabRec[NbrRec]=GetMem(strlen(moi)+1);
                memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
                NbrRec++;
                }

            Nbfic++;
            }
        }

    error=_dos_findnext(&ff);
    }

Fichier[Nbfic-1]->next=0;
}
while (NbrRec>0);

ChargeEcran();


fprintf(fic,"KKD");
tai=1;
fwrite(&tai,1,1,fic);       // VERSION
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
    fwrite(&tai,1,1,fic);                   // 2
    fwrite(Fichier[n]->nom,tai,1,fic);      // t

    fwrite(&Fichier[n]->desc,4,1,fic);      // desc
    fwrite(&Fichier[n]->next,4,1,fic);      // suiv

    fwrite(&Fichier[n]->size,4,1,fic);      // 4

    fwrite(&Fichier[n]->time,2,1,fic);      // 2
    fwrite(&Fichier[n]->date,2,1,fic);      // 2

    fwrite(&Fichier[n]->attrib,1,1,fic);    // 1

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



