//     corriger  sizesec

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
#include <bios.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include "kk.h"


#ifdef __WC32__

struct RBREGS
    {
    long edi,esi,ebp;
    long reserved;
    long ebx,edx,ecx,eax;
    short flag,es,ds,fs,gs,ip,cs,sp,ss;
    };

static unsigned short DOSbuf1,DOSsel1;
static unsigned short DOSbuf2,DOSsel2;

static struct RBREGS R;

int RAWFile(char *in,char *to);

int iskeypressed(int key)
{
char *Keyboard_Flag1=(char*)0x417;
char car;

car=*Keyboard_Flag1;

switch(key)
    {
    case SHIFT:
        return ( ((car&1)==1) | ((car&2)==2) );
    case CTRL:
        return ((car&8)==8);
    case ALT:
        return ((car&4)==4);
    }
return 0;
}


void CutName(char *str);

void CutName(char *str)
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
|- Renvoit le nombre d'octets libres sur l'unite drive                -|
|-      0 -> 'a:'                                                     -|
\*--------------------------------------------------------------------*/
long GetDiskFree(char drive)
{
union REGS R;
long l;
unsigned short ax,bx,cx;

R.h.ah=0x36;
R.h.dl=(char)(drive+1);

int386(0x21,&R,&R);

ax=(unsigned short)R.w.ax;
bx=(unsigned short)R.w.bx;
cx=(unsigned short)R.w.cx;

l=((unsigned long)ax)*((unsigned long)bx)*((unsigned long)cx);

return l;
}


long GetDiskTotal(char drive)
{
struct diskfree_t d;
long ttotal;

if (_dos_getdiskfree(drive+1,&d)!=0)
    return 0;

ttotal=(d.total_clusters)*(d.sectors_per_cluster);
ttotal=ttotal*(d.bytes_per_sector);

return ttotal;
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
R.h.bh=(char)drive;
R.h.bl=0;
int386(0x2F,&R,&R);

if (R.h.al==0xFF)
    value=2,
    strcpy(info,"Redirected drive (with interlnk)");

// PrintAt(0,0,"%04X %04X %04X %04X",R.w.ax,R.w.bx,R.w.cx,R.w.dx);
// pour C renvoit 5601 0200 0168 FFFF
// pour D renvoit 5601 0300 0168 FFFF
// pour E renvoit 56FF 0401 0001 0630

R.w.ax=0x150B;
R.w.cx=(short)drive;
int386(0x2F,&R,&R);

if ( (R.w.bx==0xADAD) & (R.w.ax!=0) )
    value=1,
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

void DriveInfo(char disk,char *volume)
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

void DrivePath(char drive,char *path)
{
unsigned nbrdrive,ii;

ii=drive+1;

_dos_setdrive(ii,&nbrdrive);

getcwd(path,256);
}




int DriveReady(char drive)
{
unsigned nbrdrive,cdrv,n;
struct diskfree_t d;

if ((drive+1<1) | (drive+1>26)) return 0;

n=_bios_equiplist();

if ( ((n&192)==0) & (drive==1) ) return 0;            // Seulement un disque
if ( ((n&1)==0) & (drive==0) ) return 0;                    // Pas de disque

_dos_getdrive(&cdrv);

IOerr=0;
IOver=1;

_dos_setdrive(drive+1,&nbrdrive);

if (GetDiskTotal(drive)==0)
    return 0;

if (_dos_getdiskfree(drive+1,&d)!=0)
    IOerr=1;

_dos_setdrive(cdrv,&nbrdrive);

if (IOerr==0)
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
R.h.bl=(char)i;
R.h.bh=0;
R.w.cx=0;     // Number of word to copy from the protected mode stack to
                                                  // the real mode stack
R.x.edi=(unsigned long)RR;

RR->ss=0;
RR->sp=0;

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
R.w.bx=4000;        // Nombre de paragraphes … allouer(un bloc=16 octets)

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

void Short2LongFile(char *from,char *chaine)
{
char ok=0;

if ((KKCfg->_Win95) & (KKCfg->win95ln))
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

    if (R.eax!=0x7100)
        {
        strcpy(chaine,nouv);
        ok=1;
        }

    DOS_Free();
    }

if (((KKCfg->transtbl)&1)==1)
    {
    char transtbl[256],comp[256];
    FILE *fic;

    strcpy(transtbl,from);
    Path2Abs(transtbl,"..");
    Path2Abs(transtbl,"trans.tbl");

    fic=fopen(transtbl,"rt");
    if (fic!=NULL)
        {
        FileinPath(from,comp);

        while(fgets(transtbl,256,fic)!=NULL)
            {
            if (!strnicmp(transtbl+2,comp,strlen(comp)))
                {
                if ( (transtbl[2+strlen(comp)]==' ') |
                     (transtbl[2+strlen(comp)]==';') |
                     ( (transtbl[2+strlen(comp)]=='.') &
                       ( (transtbl[3+strlen(comp)]==' ') |
                         (transtbl[3+strlen(comp)]==';')
                       )
                     )
                   )
                    {
                    int n;
                    char car;

                    n=strlen(comp);

                    do
                        {
                        car=(char)toupper(transtbl[n]);
                        if (car==9) break;
                        n++;
                        }
                    while(1);

                    strcpy(comp,transtbl+n+1);
                    if (comp[strlen(comp)-1]==0x0A)
                        comp[strlen(comp)-1]=0;
                    strcpy(chaine,from);
                    Path2Abs(chaine,"..");
                    Path2Abs(chaine,comp);
                    ok=1;
                    break;
                    }
                }
            }
        fclose(fic);
        }
    }

if ((KKCfg->daccessln) & (!ok) & (!KKCfg->_Win95))
    {
    if (!RAWFile(from,chaine))
        ok=1;
    }

if (!ok)
    strcpy(chaine,from);
}

/*--------------------------------------------------------------------*\
|- Renomme un fichier                                                 -|
\*--------------------------------------------------------------------*/
bool InternalRename(char *from,char *to)
{
char ok=0;
bool erreur;

if ((KKCfg->_Win95) & (KKCfg->win95ln))
    {
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

    erreur=((R.flag)&1)==1;  //--- R.flag=12990 si tout va bien -------

    if (!erreur)
        {
        strcpy(old,to);
        strcpy(nouv,from);

        R.eax=0x7160;
        R.ecx=1;
        R.ds=DOSbuf1;
        R.edx=0;
        R.es=DOSbuf2;
        R.edi=0;

        DOS_Int(0x21,&R);

        if ( ((R.flag)&1)==0)
            {
            char buf1[256];

            FileinPath(nouv,buf1);
            Path2Abs(from,"..");
            Path2Abs(from,buf1);
            }
        }

    DOS_Free();

    ok=1;
    }

if (((KKCfg->transtbl)&2)==2)
    {
    char transtbl[256],trans2[256],comp[256],buffer[256];
    FILE *infic,*outfic;

    if (!ok)
        {
        FileinPath(to,transtbl);
        CutName(transtbl);
        strcpy(comp,to);
        Path2Abs(comp,"..");
        Path2Abs(comp,transtbl);

        rename(from,comp);
        strcpy(from,comp);
        }

    strcpy(transtbl,from);
    Path2Abs(transtbl,"..");
    strcpy(trans2,transtbl);

    Path2Abs(transtbl,"trans.tbl");
    Path2Abs(trans2,"trans.new");

    unlink(trans2);
    rename(transtbl,trans2);

    outfic=fopen(transtbl,"wt");
    if (outfic!=NULL)
        {
        FileinPath(from,comp);
        FileinPath(to,transtbl);
        strupr(comp);

        infic=fopen(trans2,"rt");
        if (infic!=NULL)
            {
            while (fgets(buffer,256,infic)!=NULL)
                {
                if (strnicmp(buffer+2,comp,strlen(comp))!=0)
                    fprintf(outfic,"%s",buffer);
                }
            fclose(infic);
            }

        fprintf(outfic,"F %-34s%c%s\n",comp,9,transtbl);
        fclose(outfic);

        ok=1;
        erreur=0;
        }

    unlink(trans2);
    }


if (!ok)
    erreur=(bool)rename(from,to);

return erreur;   //--- Pas d'erreur ------------------------------------
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
EjectDrive((char)(toupper(Fen->path[0])-'A'));

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
buf[16]=(char)(DOSbuf1&255);
buf[17]=(char)(DOSbuf1/256);

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
buf[16]=(char)(DOSbuf1&255);
buf[17]=(char)(DOSbuf1/256);

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

if ((KKCfg->_Win95==1) & (KKCfg->win95ln))
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


/*--------------------------------------------------------------------*\
|- Set Name                                                           -|
\*--------------------------------------------------------------------*/
void TitleBox(char *string,...)
{
if (KKCfg->_Win95==1)
    {
    char *buf;
    long l;
    char chaine[256];

    va_list arglist;

    va_start(arglist,string);
    vsprintf(chaine,string,arglist);
    va_end(arglist);

    DOS_Alloc();

    l=DOSbuf1*16;
    buf=(char*)l;

    strcpy(buf,chaine);

    R.eax=0X168E;
    R.edx=0;
    R.es=DOSbuf1;
    R.edi=0;

    DOS_Int(0x2F,&R);

    DOS_Free();
    }
}

void PutInClipboard(char *buffer,int lng)
{
char *buf;
long l;

DOS_Alloc();

l=DOSbuf1*16;
buf=(char*)l;

R.eax=0X1701;       //--- Open Clipboard -------------------------------
DOS_Int(0x2F,&R);

memcpy(buf,buffer,lng);

R.eax=0X1703;
R.edx=1;
R.es=DOSbuf1;
R.ebx=0;
R.esi=0;
R.ecx=lng;

DOS_Int(0x2F,&R);

R.eax=0X1708;       //--- Close Clipboard ------------------------------
DOS_Int(0x2F,&R);

DOS_Free();
}


char ExistClipboard(void)
{
union REGS R;

R.w.ax=0x1700;
int386(0x2F,&R,&R);

return (R.w.ax!=0x1700);
}




/*--------------------------------------------------------------------*\
|- Lecture des noms longs … partir des disques                        -|
\*--------------------------------------------------------------------*/
typedef struct _diskfile
{
char *buffer;
int seek;
int debbuf;
int sizebuf;
int sizesec;
int drive;
} DFILE;

DFILE *dopen(int drive);
void dseek(DFILE *dfic,int seek,int from);
int dgetc(DFILE *dfic);
int dread(void *buffer,int nbr,int n,DFILE *fic);
void dclose(DFILE *fic);
void ReadSecteur(DFILE *dfic,int secteur,char *buf);



DFILE *dopen(int drive)
{
DFILE *d;
short size;

d=(DFILE *)GetMem(sizeof(DFILE));

d->buffer=(char*)GetMem(64000);
d->seek=0;
d->debbuf=0;
d->drive=drive;

d->sizesec=64000;

ReadSecteur(d,0,d->buffer);

memcpy(&size,d->buffer+0x0B,2);

if (size==0)
    {
    dclose(d);
    return NULL;
    }

// PrintAt(0,0,"%d",size);  Wait(0,0);

d->sizesec=size;
d->sizebuf=d->sizesec;

return d;
}

void dseek(DFILE *dfic,int seek,int from)
{
if (from==0)
    dfic->seek=seek;
}

int dgetc(DFILE *dfic)
{
char a;

dread(&a,1,1,dfic);

return a;
}


int dread(void *buffer,int nbr,int nbr2,DFILE *fic)
{
int taille,m,n,pos;

taille=nbr*nbr2;

pos=0;

do
    {
    if ( (fic->seek>=(fic->debbuf+fic->sizebuf)) |
         (fic->seek<fic->debbuf) )
        {
        ReadSecteur(fic,(fic->seek)/(fic->sizesec),fic->buffer);
        fic->debbuf=(fic->seek)/(fic->sizesec);
        fic->debbuf=(fic->debbuf)*(fic->sizesec);
        }

    m=(fic->seek)-(fic->debbuf);

    n=(fic->sizebuf)-m;

    if (taille<=n) n=taille;

    memcpy((char*)buffer+pos,fic->buffer+m,n); //m+n=sizebuf

    fic->seek+=n;
    pos+=n;

    taille-=n;
    }
while(taille!=0);

return pos;
}

void dclose(DFILE *fic)
{
LibMem(fic->buffer);
LibMem(fic);
}


//--- drive: 0->a

void ReadSecteur(DFILE *dfic,int secteur,char *buf)
{
char *nouv;
long l;
union REGS RR;
struct SREGS S;            //--- Segment pour l'appel d'interruption ---

DOS_Alloc();

l=DOSbuf2*16;
nouv=(char*)l;

memset(nouv,0,4800);

RR.w.ax=0x200;
RR.h.bl=0x25;
int386(0x31,&RR,&RR);

R.eax=dfic->drive;

/*
R.ecx=1;    //--- Nombre de secteurs … lire ----------------------------
R.edx=secteur;  //--- Num‚ro du premier secteur … lire -----------------
*/

//--
R.ecx=0xFFFF;

memcpy(nouv,&secteur,4);
nouv[4]=1;

nouv[6]=10;
nouv[7]=0;
nouv[8]=(char)(DOSbuf2&255);
nouv[9]=(char)(DOSbuf2/256);
//-

R.ds=DOSbuf2;
R.ebx=0;

R.cs=RR.w.cx;
R.ip=RR.w.dx;

R.ss=0;
R.sp=0;

RR.w.ax=0x301;
RR.h.bh=0;
RR.w.cx=0;     // Number of word to copy from the protected mode stack to
                                                   // the real mode stack
RR.x.edi=(unsigned long)(&R);

segread(&S);
int386x(0x31,&RR,&RR,&S);


/*
{
int x,y;

Clr();

PrintAt(0,0,"Secteur %d",secteur);

for (y=0;y<Cfg->TailleY-6;y++)
    {
    PrintAt(1,y+4,"%08X",y*16);

    for (x=0;x<16;x++)
        {
        char a;
        a=nouv[x+y*16+10];
        PrintAt(x*3+11,y+4,"%02X",a);
        AffChr(x+60,y+4,a);
        }
    }
Wait(0,0);
}
*/

memcpy(buf,nouv+10,dfic->sizesec);

DOS_Free();
}




int RAWFile(char *in,char *to)
{
short sizesec;

char court[16],filerec[16];

char nom[256],*buf;

char path[256],*p;

short sf;
char key[32];
long sec;
DFILE *dfic;

ULONG pos;
char fin;

int n;

strcpy(nom,in);
Path2Abs(nom,"..");

strcpy(path,nom+3);
FileinPath(in,filerec);

strcpy(to,"");

dfic=dopen(tolower(in[0])-'a');
if (dfic==NULL) return 1;

dseek(dfic,0x0B,SEEK_SET);
dread(&sizesec,1,2,dfic);

//  Okay.
pos=0;
fin=0;

dseek(dfic,0x16,SEEK_SET);
dread(&sf,1,2,dfic);

//PrintAt(0,0,"Sizesec: %d",sizesec);    Wait(0,0);

sec=((sf*2)+1)*sizesec;

if (path[strlen(path)-1]=='\\')
    path[strlen(path)-1]=0;

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

//    PrintAt(0,0,"cherche (%s,%s) … partir de %d",p,path,sec);  Wait(0,0);

    do
        {
        dseek(dfic,sec,SEEK_SET);
        dread(key,32,1,dfic);

/*
{
int x,y;

Clr();

for (y=0;y<2;y++)
    {
    PrintAt(1,y+4,"%08X",y*16);

    for (x=0;x<16;x++)
        {
        char a;
        a=key[x+y*16];
        PrintAt(x*3+11,y+4,"%02X",a);
        AffChr(x+60,y+4,a);
        }
    }
Wait(0,0);
}
*/

        if (key[0]==0)
            {
//           PrintAt(0,0,"fin",nom,path);          Wait(0,0);
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

//       PrintAt(0,0,"--(%s,%s)",nom,path);        Wait(0,0);
        }
    while(stricmp(nom,path)!=0);

    memcpy((short*)(&sf),key+0x1A,2);
    sec=sf-2;

    dseek(dfic,0x0D,SEEK_SET);
    sf=(short)dgetc(dfic);
    sec=sec*sf;

    dseek(dfic,0x16,SEEK_SET);
    dread(&sf,2,1,dfic);
    sec=sec+sf*2;

    dseek(dfic,0x11,SEEK_SET);
    dread(&sf,2,1,dfic);
    sec=sec+sf/16;

    sec=(sec+1)*sizesec;

    strcpy(path,p);
    }

while(1)
    {
    dseek(dfic,sec,SEEK_SET);
    dread(key,32,1,dfic);

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

            dseek(dfic,sec,SEEK_SET);
            dread(key,32,1,dfic);

            sec+=32;
            }

        memcpy(court,key,8);
        court[8]=32;
        buf=strchr(court,' ');
        buf[0]='.';
        memcpy(buf+1,key+8,3);
        buf[4]=32;
        buf=strchr(court,' ');
        buf[0]=0;

        if (court[strlen(court)-1]=='.')
            court[strlen(court)-1]=0;

//        PrintAt(0,0,"(%s,%s)",court,filerec);        Wait(0,0);

        if (!stricmp(court,filerec))
            {
            strcpy(to,nom);
            break;
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

        if (nom[strlen(nom)-1]=='.')
            nom[strlen(nom)-1]=0;

        sec+=32;
        }
    }

dclose(dfic);

if (to[0]==0)
    return 1;
    else
    return 0;
}

#else

Popo        // Erreur !!

#endif
