#include <io.h>
#include <string.h>
#include <stdio.h>

#include "rbio.h"

RBFile::RBFile(char *name,int flag)
{
isopen=0;

open(name,flag);
}

RBFile::RBFile(void)
{
isopen=0;
}

void RBFile::open(char *name,int flag)
{
char buf[8];

if (isopen)
	close();

strcpy(filename,name);

buf[0]=0;

if ((flag&RBREAD)==RBREAD)
	strcat(buf,"r");

if ((flag&RBWRITE)==RBWRITE)
	{
	if ((flag&RBREAD)==RBREAD)
		strcat(buf,"+");
	else
		strcat(buf,"w");
	}

if ((flag&RBTEXT)==RBTEXT)
	strcat(buf,"t");
	else
	strcat(buf,"b");

fic=fopen(filename,buf);
isopen=(fic!=NULL);

if (isopen)
	{
	length=filelength(fileno(fic));

	pos=0;
	}
}

void RBFile::close(void)
{
if (isopen)
	{
	fclose(fic);
	isopen=0;
	}
}




RBFile::~RBFile()
{
close();
}

int RBFile::read(void *buffer,int l)
{
int n;

if (!isopen)  return 0;

n=fread(buffer,1,l,fic);

pos+=n;

return n;
}

int RBFile::write(void *buffer,int l)
{
int n;

if (!isopen)  return 0;

n=fwrite(buffer,1,l,fic);

pos+=n;

return n;
}


void RBFile::seek(int n,int from)
{
if (!isopen)  return;

fseek(fic,n,from);

pos=ftell(fic);
}

int RBFile::tell(void)
{
if (!isopen)  return 0;

return pos;
}


