/*--------------------------------------------------------------------*\
|- Gestion des ressources DOS - source								  -|
|- Gestion des noms longs											  -|
|----------------------------------------------------------------------|
|-	  Version DJGPP 												  -|
\*--------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include "kk.h"

#ifdef GCC
#include <unistd.h>
#endif


void DriveInfo(char drive,char *volume)
{
strcpy(volume,"Unknown");
}

void DrivePath(char drive,char *path)
{
sprintf(path,"%c:\\",drive+'A');
}

int DriveReady(char drive)
{
return 1;
}

int DriveExist(char drive)
{
return 1;
}

void DriveSet(char *path)
{
chdir(path);
}

void EjectDrive(char lect)
{
}

void Short2LongFile(char *from,char *chaine)
{
strcpy(chaine,from);
}

long GetDiskFree(char drive)
{
return 0;
}

long GetDiskTotal(char drive)
{
return 0;
}

int TypeDisk(int drive,char *info)
{
strcpy(info,"Unknown type");
return 0;
}

char ExistClipboard(void)
{
return 0;
}

void PutInClipboard(char *buffer,int lng)
{
}

bool InternalRename(char *from,char *to)
{
return rename(from,to);
}

char IsFileExist(char *name)
{
FILE *fic;

fic=fopen(name,"rb");
if (fic==NULL) return 0;

fclose(fic);
return 1;
}

int iskeypressed(int key)
{
/*
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
*/
return 0;
}


