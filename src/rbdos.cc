/*--------------------------------------------------------------------*\
|- Gestion des ressources DOS - source                                -|
|- Gestion des noms longs                                             -|
\*--------------------------------------------------------------------*/
#include <direct.h>
#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <time.h>
#include <bios.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

struct RBREGS
    {
    long edi,esi,ebp;
    long reserved;
    long ebx,edx,ecx,eax;
    short flag,es,ds,fs,gs,ip,cs,sp,ss;
    };

static unsigned short DOSbuf1,DOSsel1;
static unsigned short DOSbuf2,DOSsel2;

static char buf1[256],buf2[256];

static struct RBREGS R;

#include "kk.h"

/*--------------------------------------------------------------------*\
|- Renvoit le nombre d'octets libres sur l'unite drive                -|
|-      1 -> 'a:'                                                     -|
\*--------------------------------------------------------------------*/
long GetDiskFree(char drive)
{
union REGS R;
long l;
unsigned short ax,bx,cx;

R.h.ah=0x36;
R.h.dl=drive;

int386(0x21,&R,&R);

ax=(unsigned short)R.w.ax;
bx=(unsigned short)R.w.bx;
cx=(unsigned short)R.w.cx;

l=((unsigned long)ax)*((unsigned long)bx)*((unsigned long)cx);

return l;
}

/*--------------------------------------------------------------------*\
|-  disk 0 --> 'A:'                                                   -|
\*--------------------------------------------------------------------*/
void GetVolume(char disk,char *volume)
{
struct find_t ff;
int n,m;
int error;

sprintf(volume,"%c:\\*.*",disk+'A');

error=_dos_findfirst(volume,_A_VOLID,&ff);

if (error!=0)
    strcpy(volume,"Unknow");
    else
    strcpy(volume,ff.name);

n=0;
while (volume[n]!=0)
    {
    if (volume[n]=='.')
        for (m=n;m<strlen(volume);m++)
            volume[m]=volume[m+1];
        else
        n++;
    }
}

/*--------------------------------------------------------------------*\
|-                         Gestion 4DOS                               -|
|-  Put KKCfg->_4dos on if 4dos found                                 -|
\*--------------------------------------------------------------------*/
int _4DOSverif(void)
{
union REGS R;

R.w.ax=0xD44D;
R.h.bh=0;

int386(0x2F,&R,&R);

if (R.w.ax==0x44DD)
    {
    return 1;
//  PrintAt(0,0,"Found 4DOS V%d.%d",R.h.bl,R.h.bh);
    }
    else
    {
    return 0;
    }
}

char *_4DOSLhistdir(void)
{
unsigned short seg;
unsigned short *adr;

union REGS R;

R.w.ax=0xD44D;
R.h.bh=0;

int386(0x2F,&R,&R);

if (R.w.ax==0x44DD)
    {
    seg=R.w.cx;
    adr=(unsigned short*)(seg*16+0x290);

    seg=adr[1];
    adr=(unsigned short*)(seg*16+0x4C60);

    return (char*)adr;
    }
return 0;
}

void _4DOSShistdir(char *buf)
{
unsigned short seg;
unsigned short *adr;

char a;

register unsigned char n;

union REGS R;

R.w.ax=0xD44D;
R.h.bh=0;

int386(0x2F,&R,&R);

if (R.w.ax==0x44DD)
    {
    seg=R.w.cx;
    adr=(unsigned short*)(seg*16+0x290);

    seg=adr[1];
    adr=(unsigned short*)(seg*16+0x4C60);

    for (n=0;n<255;n++)
        {
        a=buf[n];
        ((char*)adr)[n]=a;
        }
    }
}


char KeyTurbo(char val)
{
union REGS R;

if (val==0) return 0;

R.w.ax=0x0900;
int386(0x16,&R,&R);

if ((R.h.ah & 4)==4)
    {
    R.w.ax=0x0305;
    R.w.bx=0x0000;
    int386(0x16,&R,&R);                             // Keystroke maximum
    return 1;
    }
    else
    return 0;
}

/*--------------------------------------------------------------------*\
|- Type de support                                                    -|
\*--------------------------------------------------------------------*/
int TypeDisk(int drive,char *info)
{
union REGS R;
int value=0;

info[0]=0;

R.w.ax=0x5601;
R.w.dx=0xFFFF;
R.h.bh=drive;
R.h.bl=0;
int386(0x2F,&R,&R);

if (R.h.al==0xFF)
    value=1,
    strcpy(info,"Redirected drive (with interlnk)");

// PrintAt(0,0,"%04X %04X %04X %04X",R.w.ax,R.w.bx,R.w.cx,R.w.dx);
// pour C renvoit 5601 0200 0168 FFFF
// pour D renvoit 5601 0300 0168 FFFF
// pour E renvoit 56FF 0401 0001 0630

R.w.ax=0x150B;
R.w.cx=drive;
int386(0x2F,&R,&R);

if ( (R.w.bx==0xADAD) & (R.w.ax!=0) )
    value=2,
    strcpy(info,"CD-ROM drive");

// PrintAt(0,0,"%04X %04X %04X %04X",R.w.ax,R.w.bx,R.w.cx,R.w.dx);
// pour C renvoit 0000 ADAD 0002 FFFF
// pour D renvoit 0000 ADAD 0003 FFFF
// pour H renvoit 5AD4 ADAD 0007 FFFF

return value;
}


/*--------------------------------------------------------------------*\
|-  Recherche des infos sur Windows                                   -|
\*--------------------------------------------------------------------*/

short windows(short *HVersion, short *NVersion )
{
union  REGS  regs;              // Registres pour l'appel d'interruption
struct SREGS sregs;               // Segment pour l'appel d'interruption

*HVersion = 0;                        // Initialise le num‚ro de version
*NVersion = 0;

           //-- Identifie Windows x.y en mode Etendu -------------------

regs.w.ax = 0x1600;                    // Test d'installation de Windows
segread( &sregs );                      // Lire les registres de segment
int386x(0x2F, &regs, &regs, &sregs );

switch ( regs.h.al )
    {
    case 0x01:
    case 0xFF:
        *HVersion = 2;                                   // Hauptversion
        *NVersion = 0;                    // Version secondaire inconnue
        return 1;                            // Windows /386 Version 2.x

    case 0x00:
    case 0x80:
        regs.w.ax = 0x4680;                    // Modes R‚el et Standard
        int386x( 0x2F, &regs, &regs, &sregs );
        if( regs.h.al == 0x80 )
            return 0;                       // Windows ne fonctionne pas
            else
            {         //-- Windows en mode R‚el ou Standard ------------
            regs.w.ax = 0x1605;               // Simuler l'initialiation
            regs.w.bx = regs.w.si = regs.w.cx =  sregs.es = sregs.ds =0;
            regs.w.dx = 0x0001;
            int386x( 0x2F, &regs, &regs, &sregs );
            if( regs.w.cx == 0x0000 )       //-- Windows en mode R‚el --
                {
                regs.w.ax = 0x1606;
                int386x(0x2F, &regs, &regs, &sregs );
                return 0x81;
                }
            else
                return 0x82;
            }

   //-- Windows en mode Etendu, ax contient le num‚ro de version -------

    default:
        *HVersion = regs.h.al;         // Afficher la version de Windows
        *NVersion = regs.h.ah;
        return 0x83;                           // Windows en mode Etendu
    }
}


/*--------------------------------------------------------------------*\
|- drive: 0:A                                                         -|
\*--------------------------------------------------------------------*/
#define DEFSLASH '\\'

void DriveInfo(char drive,char *volume)
{
GetVolume(drive,volume);
}

void DrivePath(char drive,char *path)
{
unsigned nbrdrive,ii;

ii=drive+1;

_dos_setdrive(ii,&nbrdrive);

getcwd(path,256);

if (path[strlen(path)-1]!=DEFSLASH)
    strcat(path,"\\");
}

int DriveReady(char drive)
{
if (VerifyDisk(drive+1)==0)
    return 1;
    else
    return 0;
}

/*--------------------------------------------------------------------*\
|-  Procedure en Assembleur                                           -|
\*--------------------------------------------------------------------*/

char GetDriveState(int i);
#pragma aux GetDriveState = \
    "mov ah,19h" \
    "int 21h" \
    "mov ch,al" \
    "mov ah,0Eh" \
    "int 21h" \
    "mov ah,19h" \
    "int 21h" \
    "sub al,dl" \
    "mov cl,al" \
    "mov dl,ch" \
    "mov ah,0Eh" \
    "int 21h" \
    modify [eax ebx ecx edx] \
    parm [edx] \
    value [cl];

int DriveExist(char drive)
{
if (GetDriveState(drive)==0)
    return 1;
    else
    return 0;
}

void DriveSet(char *path)
{
unsigned ndrv;

_dos_setdrive(toupper(path[0])-'A'+1,&ndrv);
chdir(path);
}

 
/*--------------------------------------------------------------------*\
|- Gestion des interruptions en mode reel                             -|
\*--------------------------------------------------------------------*/
void DOS_Int(short i,struct RBREGS *RR)
{
union REGS R;
struct SREGS S;

segread(&S);
R.w.ax=0x300;
R.h.bl=i;
R.h.bh=0;
R.w.cx=0;     // Number of word to copy from the protected mode stack to
                                                  // the real mode stack
R.x.edi=(unsigned long)RR;

int386x(0x31,&R,&R,&S);
}

/*--------------------------------------------------------------------*\
|- Allocation des buffer DOSbuf1 et DOSbuf2                           -|
\*--------------------------------------------------------------------*/
void DOS_Alloc(void)
{
union REGS R;

R.w.ax=0x100;
R.w.bx=40;                            // Nombre de paragraphes … allouer

int386(0x31,&R,&R);

//if (carry flag!=0)

DOSbuf1=R.w.ax;
DOSsel1=R.w.dx;

R.w.ax=0x100;
R.w.bx=40;                            // Nombre de paragraphes … allouer

int386(0x31,&R,&R);

// if (carry flag!=0)

DOSbuf2=R.w.ax;
DOSsel2=R.w.dx;
}

/*--------------------------------------------------------------------*\
|- Liberation des buffers DOSbuf1 et DOSbuf2                          -|
\*--------------------------------------------------------------------*/
void DOS_Free(void)
{
union REGS R;

R.w.ax=0x101;
R.w.dx=DOSsel1;

int386(0x31,&R,&R);

R.w.ax=0x101;
R.w.dx=DOSsel2;

int386(0x31,&R,&R);
}


/*--------------------------------------------------------------------*\
|- Donne le nom long du fichier courant                               -|
\*--------------------------------------------------------------------*/
void InfoLongFile(FENETRE *Fen,char *chaine)
{
char *old,*nouv;
long l;

DOS_Alloc();

l=DOSbuf1*16;
old=(char*)l;

l=DOSbuf2*16;
nouv=(char*)l;

strcpy(old,Fen->path);
Path2Abs(old,Fen->F[Fen->pcur]->name);

strcpy(nouv,"");

R.eax=0x7160;
R.ecx=2;
R.ds=DOSbuf1;
R.esi=0;
R.es=DOSbuf2;
R.edi=0;

DOS_Int(0x21,&R);

if (R.eax!=0x7100)
    {
    FileinPath(nouv,chaine);
    }
    else
    {
    strcpy(chaine,Fen->F[Fen->pcur]->name);
    }

DOS_Free();
}

void Short2LongFile(char *from,char *chaine)
{
char *old,*nouv;
long l;

DOS_Alloc();

l=DOSbuf1*16;
old=(char*)l;

l=DOSbuf2*16;
nouv=(char*)l;

strcpy(old,from);

strcpy(nouv,"");

R.eax=0x7160;
R.ecx=2;
R.ds=DOSbuf1;
R.esi=0;
R.es=DOSbuf2;
R.edi=0;

DOS_Int(0x21,&R);

if (R.eax==0x7100)
    strcpy(chaine,from);
    else
    strcpy(chaine,nouv);

DOS_Free();
}

/*--------------------------------------------------------------------*\
|- Renomme un fichier                                                 -|
\*--------------------------------------------------------------------*/
char Win95Rename(char *from,char *to)
{
char erreur;
char *old,*nouv;
long l;

DOS_Alloc();

l=DOSbuf1*16;
old=(char*)l;

l=DOSbuf2*16;
nouv=(char*)l;

strcpy(old,from);
strcpy(nouv,to);

R.eax=0x7156;
R.ds=DOSbuf1;
R.edx=0;
R.es=DOSbuf2;
R.edi=0;

DOS_Int(0x21,&R);
erreur=((R.flag)&1)==1;  //--- R.flag=12990 si tout va bien ------------

DOS_Free();

return erreur;   //--- Pas d'erreur ------------------------------------
}


/*--------------------------------------------------------------------*\
|- Prend le vrai nom de from et le donne … to                         -|
|- ! inpath est toujours un nom court                                 -|
\*--------------------------------------------------------------------*/
void UpdateLongName(char *from,char *to)
{
char *old,*nouv;
long l;


FileinPath(from,buf1);
FileinPath(to,buf2);

DOS_Alloc();

l=DOSbuf1*16;
old=(char*)l;

l=DOSbuf2*16;
nouv=(char*)l;

strcpy(old,from); // Path2abs avant
strcpy(nouv,"");

R.eax=0x7160;
R.ecx=2;
R.ds=DOSbuf1;
R.esi=0;
R.es=DOSbuf2;
R.edi=0;

DOS_Int(0x21,&R);


if (R.eax!=0x7100)
    {
    if (WildCmp(buf1,buf2)==0)
        {
        strcpy(old,to);

        FileinPath(nouv,buf1);
        strcpy(buf2,to);
        Path2Abs(buf2,"..");
        Path2Abs(buf2,buf1);

        strcpy(nouv,buf2);

        R.eax=0x7156;
        R.ds=DOSbuf1;
        R.edx=0;
        R.es=DOSbuf2;
        R.edi=0;

        DOS_Int(0x21,&R);

        strcpy(old,buf2);
        strcpy(nouv,"");

        R.eax=0x7160;
        R.ecx=1;
        R.ds=DOSbuf1;
        R.esi=0;
        R.es=DOSbuf2;
        R.edi=0;

        DOS_Int(0x21,&R);

        FileinPath(nouv,buf1);
        Path2Abs(to,"..");
        Path2Abs(to,buf1);
        }
    else
        {
        FileinPath(to,buf1);
        buf1[8]=0;

        strcpy(old,to);
        Path2Abs(old,"..");
        Path2Abs(old,buf1);

        strcpy(nouv,to);

        R.eax=0x7156;
        R.ds=DOSbuf1;
        R.edx=0;
        R.es=DOSbuf2;
        R.edi=0;

        DOS_Int(0x21,&R);
        }
    }

DOS_Free();
}

char Verif95(void)
{
char a;
char *old,*nouv;
long l;

DOS_Alloc();

l=DOSbuf1*16;
old=(char*)l;

l=DOSbuf2*16;
nouv=(char*)l;

strcpy(old,Fics->path);

strcpy(nouv,"");

R.eax=0x7160;
R.ecx=2;
R.ds=DOSbuf1;
R.esi=0;
R.es=DOSbuf2;
R.edi=0;

DOS_Int(0x21,&R);

if (R.eax!=0x7100)
    a=1;
    else
    a=0;

// marjo
/*
R.eax=0x5E00;
R.ds=DOSbuf1;
R.edx=0;

DOS_Int(0x21,&R);

{
FILE *fic;
fic=fopen("ee","wb");
fwrite(old,1,512,fic);
fclose(fic);
}
*/

DOS_Free();

return a;
}


/*--------------------------------------------------------------------*\
|- Ejection du CD                                                     -|
\*--------------------------------------------------------------------*/
void EjectCD(FENETRE *Fen)
{
EjectDrive(toupper(Fen->path[0])-'A');

IOver=1;
CommandLine("#CD .");
}


/*--------------------------------------------------------------------*\
|- Set Name                                                           -|
\*--------------------------------------------------------------------*/
/*
void SetWindowsTitle(void)
{
char *buf;
long l;

DOS_Alloc();

l=DOSbuf1*16;
buf=(char*)l;

strcpy(buf,"KKC");

R.eax=0X168E;
R.edx=1;
R.es=DOSbuf1;
R.edi=0;

DOS_Int(0x2F,&R);

strcpy(buf,"RedBug for King");

R.eax=0X168E;
R.edx=0;
R.es=DOSbuf1;
R.edi=0;

DOS_Int(0x2F,&R);

DOS_Free();
}
*/


/*--------------------------------------------------------------------*\
|- Ejection DRIVE  (lect: 0 -> 'a')                                   -|
\*--------------------------------------------------------------------*/
void EjectDrive(char lect)
{
char *buf;
int i;
long l;
union REGS RR;

RR.w.ax=0x150B;
RR.w.cx=lect;
int386(0x2F,&RR,&RR);
if ( (RR.w.bx!=0xADAD) | (RR.w.ax==0) )
    return;

DOS_Alloc();

l=DOSbuf1*16;
buf=(char*)l;

buf[0]=13;
buf[1]=lect;
buf[2]=0;
for(i=3;i<32;i++)
    buf[i]=0;

R.eax=0X1510;
R.ebx=0;
R.ecx=lect;
R.es=DOSbuf1;


buf[0]=13;
buf[1]=lect;
buf[2]=12;
for(i=3;i<32;i++)
    buf[i]=0;

buf[14]=26;     // BYTE 26
buf[15]=0;
buf[16]=DOSbuf1&255;
buf[17]=DOSbuf1/256;

buf[18]=0;      // Taille du bloc
buf[19]=1;      // Taille du bloc

buf[26]=0;   // Open CD ------------------------------------------------

R.eax=0X1510;
R.ebx=0;
R.ecx=lect;
R.es=DOSbuf1;

DOS_Int(0x2F,&R);

WinMesg("Information Message","Put a CD-ROM in the tray",0);

buf[0]=13;
buf[1]=lect;
buf[2]=12;
for(i=3;i<32;i++)
    buf[i]=0;

buf[14]=26;     // BYTE 26
buf[15]=0;
buf[16]=DOSbuf1&255;
buf[17]=DOSbuf1/256;

buf[18]=1;  // Taille du bloc
buf[19]=0;  // Taille du bloc

buf[26]=5;  // Close CD ------------------------------------------------

R.eax=0X1510;
R.ebx=0;
R.ecx=lect;
R.es=DOSbuf1;

DOS_Int(0x2F,&R);

DOS_Free();

Delay(100);
}


char IsFileExist(char *name)
{
FILE *fic;

if (KKCfg->_Win95==1)
    {
    char oldname[256];
    char *old,*nouv;
    long l;

    DOS_Alloc();

    l=DOSbuf1*16;
    old=(char*)l;

    l=DOSbuf2*16;
    nouv=(char*)l;

    strcpy(old,name);
    strcpy(nouv,"");

    R.eax=0x7160;
    R.ecx=2;
    R.ds=DOSbuf1;
    R.esi=0;
    R.es=DOSbuf2;
    R.edi=0;

    DOS_Int(0x21,&R);

    if (R.eax!=0x7100)
        strcpy(oldname,nouv);

    DOS_Free();

    if (oldname[0]==0)
        return 0;       //--- N'existe pas -----------------------------
        else
        return 1;       //--- Existe -----------------------------------

    }

fic=fopen(name,"rb");
if (fic==NULL) return 0;

fclose(fic);
return 1;
}


/*
void ClipBoardInfo(void)
{
char erreur;
char *old,*nouv;
long l;

DOS_Alloc();

l=DOSbuf1*16;
old=(char*)l;

l=DOSbuf2*16;
nouv=(char*)l;

R.eax=0x1700;
DOS_Int(0x2F,&R);   //--- On test si le clipboard peut s'ouvrir

R.eax=0x1701;
DOS_Int(0x2F,&R);   //--- On ouvre le clipboard


PrintAt(0,0,"(%08X)",R.eax);
PrintAt(0,1,"(%08X)",R.ebx);
PrintAt(0,2,"(%08X)",R.ecx);
PrintAt(0,3,"(%08X)",R.edx);
WinError("pause");

DOS_Free();
}
*/
