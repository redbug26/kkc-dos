/*--------------------------------------------------------------------*\
|-                                                                    -|
|- Toutes les fonctions independantes (logiquement) … l'O.S. ainsi qu'-|
|-                                                aux fichiers hard.c -|
|-                                                                    -|
|- By RedBug/Ketchup Killers                                          -|
|-                                                                    -|
\*--------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#ifndef LINUX
     #include <io.h> //--- pour filelength ----------------------------------
#endif

#include "reddies.h"
#include "hard.h"

void TPath2Abs(char *p,char *Ficname);

/*--------------------------------------------------------------------*\
|- Reduce String                                                      -|
\*--------------------------------------------------------------------*/
void ReduceString(char *str,int length)
{
char namebuf[256];

if (strlen(str)>length)
    {
    memcpy(namebuf,str,3);
    memcpy(namebuf+3,"...",3);
    memcpy(namebuf+6,str+strlen(str)-(length-6),length-6);
    namebuf[length]=0;
    strcpy(str,namebuf);
    }
}


/*--------------------------------------------------------------------*\
|-                  Renvoit l'extension d'un nom                      -|
\*--------------------------------------------------------------------*/
char *getext(const char *nom)
{
char *e,*ext;

e=strchr(nom,'.');

if (e==NULL)
    ext=strchr(nom,0);
    else
    ext=e+1;

return ext;
}

/*--------------------------------------------------------------------*\
|-                Comparaison entre un nom et un autre                -|
\*--------------------------------------------------------------------*/
int Maskcmp(char *src,char *mask)
{
int n;

for (n=0;n<strlen(mask);n++)
    {
    if ( (src[n]=='/') & (mask[n]=='/') ) continue;
    if ( (src[n]==DEFSLASH) & (mask[n]=='/') ) continue;
    if ( (src[n]==DEFSLASH) & (mask[n]==DEFSLASH) ) continue;
    if ( (src[n]=='/') & (mask[n]==DEFSLASH) ) continue;
    if (toupper(src[n])==toupper(mask[n])) continue;

    return 1;
    }
return 0;
}


/*--------------------------------------------------------------------*\
|-     Envoie le nom du fichier src dans dest si le mask est bon      -|
|-     Renvoit 1 dans ce cas, sinon renvoie 0                         -|
\*--------------------------------------------------------------------*/
int find1st(char *src,char *dest,char *mask)
{
int i;

if (!Maskcmp(src,mask))
    {
    if ( (src[strlen(mask)]==DEFSLASH) | (src[strlen(mask)]=='/') )
        memcpy(dest,src+strlen(mask)+1,strlen(src+strlen(mask)));
        else
        memcpy(dest,src+strlen(mask),strlen(src+strlen(mask))+1);
    for (i=0;i<strlen(dest);i++)
        {
        if (dest[i]==DEFSLASH) return 0;
        if (dest[i]=='/') return 0;
        }

    if (!strcmp(dest,"")) return 0;
    return 1;
    }

return 0;
}


/*--------------------------------------------------------------------*\
|-                 Comparaison avec WildCards                         -|
|-    EX: "fichier.kkr","*.kkr" renvoit 0                             -|
\*--------------------------------------------------------------------*/
int WildCmp(char *a,char *b)
{
int p1,p2;
int ok,ok2;
char c1,c2,c3;
int n;

int astp1=-1,astp2;

n=0;

ok2=1;      //--- Pas trouv‚ -------------------------------------------

while(b[n]!=0)
{
n+=(b[n]==';');

ok=0;

p1=0;
p2=n+(b[n]=='-');


while (1)
    {
    c1=a[p1];
    c2=b[p2];
    c3=b[p2+1];

    if ( (c1>='a') & (c1<='z') ) c1-=32;
    if ( (c2>='a') & (c2<='z') ) c2-=32;
    if ( (c3>='a') & (c3<='z') ) c3-=32;

    if ( ((c1==0) & ((c2==0) | (c2==';')) ) |
         ((c1==0) & (c2=='*') & ((c3==0) | (c3==';')) ) |
         ((c1==0) & (c2=='*') & (c3=='.') & (b[p2+2]=='*')) |
         ((c1==0) & (c2=='.') & (c3=='*'))
         )
        break;

    if (c1==0)
        {
        ok=1;
        break;
        }

    if ( (c1==c2) | (c2=='?') )
        {
        p1++;
        p2++;
        continue;
        }

    if  (c2=='*')
        {
        if (c1==c3)
            {
            astp1=p1;
            astp2=p2;
            p2+=2;
            }

        p1++;

        continue;
        }

    if (astp1!=-1)
        {
        p1=astp1+1;
        p2=astp2;

        astp1=-1;

        continue;
        }

    ok=1;
    break;
    }

if (ok==0)
    ok2=(b[n]=='-');

while ((b[n]!=';') & (b[n]!=0)) n++;
}

return ok2;
}

/*--------------------------------------------------------------------*\
|- Give the name include in a path                                    -|
|- E.G. c:\kkcom\kkc.c --> "kkc.c"                                    -|
|-      c:\            --> ""                                         -|
|-      c:\kkcom       --> "kkcom"                                    -|
|-           p         --> Ficname                                    -|
\*--------------------------------------------------------------------*/
char *FileinPath(char *p,char *Ficname)
{
int n;
char *s;

s=p;
for (n=0;n<strlen(p);n++)
    if ( (p[n]==DEFSLASH) | (p[n]=='/') )
        s=p+n+1;

strcpy(Ficname,s);

return s;
}

/*--------------------------------------------------------------------*\
|-           Make absolue path from old path and ficname              -|
\*--------------------------------------------------------------------*/
void Path2Abs(char *p,const char *relatif)
{
static char Ficname[256];
int l,m,n;
char car;

if (relatif[0]==0) return;

strcpy(Ficname,relatif);

for(n=0;n<strlen(p);n++)
    if (p[n]=='/') p[n]=DEFSLASH;

for(n=0;n<strlen(Ficname);n++)
    if (Ficname[n]=='/') Ficname[n]=DEFSLASH;

m=0;
l=strlen(Ficname);

if ((Ficname[l-1]==DEFSLASH) & (l!=1)) //--- retire le dernier slash ---
    {
    if (Ficname[l-2]!=':')  //--- drive --------------------------------
        {
        l--;
        Ficname[l]=0;
        }
    }

for(n=0;n<l;n++)
    if (Ficname[n]==DEFSLASH)
        {
        car=Ficname[n+1];

        Ficname[n+1]=0;

        TPath2Abs(p,Ficname+m);

        Ficname[n+1]=car;
        Ficname[n]=0;

        m=n+1;
        }

TPath2Abs(p,Ficname+m);


if (p[0]==0) {
        p[0] = DEFSLASH;
        p[1] = 0;
        }
}

void TPath2Abs(char *p,char *Ficname)
{
int n;
static char old[256];     //--- Path avant changement ------------------
signed int deuxpoint;
char defslash[2];

defslash[0] = DEFSLASH;
defslash[1] = 0;


if (Ficname[0]==0) return;

memcpy(old,p,256);

if (p[strlen(p)-1]==DEFSLASH) p[strlen(p)-1]=0;

if ( (!strncmp(Ficname,"..",2)) & (p[0]!=0) )
    {
    for (n=strlen(p);n>0;n--)
        if (p[n]==DEFSLASH)
            {
            p[n]=0;
            break;
            }
    if (p[strlen(p)-1]==':') strcat(p,defslash);
    return;
    }

if (Ficname[0]!='.')
    {
    deuxpoint=-1;

    for(n=0;n<strlen(Ficname);n++)
        if (Ficname[n]==':') deuxpoint=n;

    if (deuxpoint!=-1)
        {
        if (Ficname[deuxpoint+1]==DEFSLASH)
            {
            strcpy(p,Ficname);
            if (p[strlen(p)-1]==':') strcat(p,defslash);
            return;
            }
        }

    if (Ficname[0]==DEFSLASH)
        {
        strcpy(p+2,Ficname);
        if (p[strlen(p)-1]==':') strcat(p,defslash);
        return;
        }

    if (p[strlen(p)-1]!=DEFSLASH) strcat(p,defslash);
    strcat(p,Ficname);
    }

if (p[strlen(p)-1]==':') strcat(p,defslash);
}

/*--------------------------------------------------------------------*\
|- Recherche l'extension ext dans la chaine src                       -|
|- Renvoit 1 si trouve                                                -|
|- Ex: FoundExt(ext,"EXE COM BAT BTM")                                -|
\*--------------------------------------------------------------------*/

char FoundExt(char *ext,char *src)
{
char e[4];
int c1,c3,c4;

c1=c3=c4=0;

if (*ext==0) return 0;

do
    {
    c3++;

    if ( (src[c3]==32) | (src[c3]==0) )
        {
        memcpy(e,src+c4,4);
        e[c3-c4]=0;
        if (!stricmp(e,ext)) return 1;
        c4=c3+1;
        }
    }
while(src[c3]!=0);
return 0;
}

/*--------------------------------------------------------------------*\
|-         Convertit un entier en chaine de longueur length           -|
\*--------------------------------------------------------------------*/

char *Int2Char(int n,char *s,char length)
{
if ((length>=3) & (n==1))
    {
    strcpy(s,"One");
    return s;
    }

sprintf(s,"%d",n); //ltoa(n,s,10);

if (strlen(s)<=length) return s;

strcpy(s,s+strlen(s)-length);

return s;
}

/*--------------------------------------------------------------------*\
|-            Convertit un long en string de 10 positions             -|
\*--------------------------------------------------------------------*/

char *Long2Str(long entier,char *chaine)
{
char chaine2[20];
int i,j,n;

sprintf(chaine2,"%d",entier); // ltoa(entier,chaine2,10);

n=strlen(chaine2);

if (n<10)
    {
    chaine[0]=chaine2[0];
    i=j=1;
    n--;

    while(n!=0)
        {
        if ((n==6) | (n==3))
            {
            chaine[i]='.';
            i++;
            }
        chaine[i]=chaine2[j];
        i++;
        j++;
        n--;
        }
    chaine[i]=0;
    }
    else
    {
    strcpy(chaine,chaine2);
    }

return chaine;
}


/*--------------------------------------------------------------------*\
|- Avancement de graduation                                           -|
|- Renvoit le prochain                                                -|
\*--------------------------------------------------------------------*/
int LongGradue(int x,int y,int length,int from,int to,int total)
{
int j1,j3;

if (total==0) return 0;

if ( (to>1024) & (total>1024) )
    {
    j3=(to/1024);
    j3=(j3*length)/(total/1024);
    }
    else
    j3=(to*length)/total;

if (j3>=(length)) j3=length;

j1=from;

for (;j1<j3;j1++)
    AffChr(j1+x,y,0xB2);

if (to==0)
    ChrLin(x,y,length,32);

return j1;
}

char *StrUpr(char *chaine)
{
char *car;

car=chaine;
while((*car)!=0)
    {
    if (((*car)>='a') & ((*car)<='z')) (*car)-=32;
    car++;
    }

return chaine;

}


long flength(int handle)
{
return filelength(handle);
}


