/*--------------------------------------------------------------------*\
|- programme de shell                                                 -|
\*--------------------------------------------------------------------*/
#include <stdlib.h>
#include <dos.h>

void main(int argc,char **argv)
{
char far *ShellAdr;
int n,m;
char a[256];
char ch[]="kkmain 6969";

ShellAdr=(char far*)MK_FP(0xB000,0xA000);

n=0;
while (argv[0][n]!=0)
    {
    a[n]=argv[0][n];
    n++;
    }

n--;
while(a[n]!='\\') n--;
n++;

do
{
for (m=n;m<n+12;m++)
    a[m]=ch[m-n];

system(a);

if (ShellAdr[0]!='#')
    break;

for (m=n;m<256;m++)
    a[m]=ShellAdr[m-n+1];

system(a+n);
}
while(1);

}
