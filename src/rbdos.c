/*--------------------------------------------------------------------*\
|- Gestion des ressources DOS - source                                -|
\*--------------------------------------------------------------------*/
#include <direct.h>
#include <dos.h>

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "rbdos.h"

#include "hard.h"


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
