/*--------------------------------------------------------------------*\
|-                                                                    -|
|- Toutes les fonctions independantes (logiquement) Ö l'O.S. ainsi qu'-|
|-                                                aux fichiers hard.c -|
|-                                                                    -|
|- By RedBug/Ketchup Killers                                          -|
|-                                                                    -|
\*--------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <io.h> //--- pour filelength ----------------------------------

#include "reddies.h"
#include "hard.h"

void TPath2Abs(char *p,char *Ficname);


/*--------------------------------------------------------------------*\
|-     Fonction qui convertit les caracteres ASCII en RedBug one      -|
\*--------------------------------------------------------------------*/
char CnvASCII(char table,char car)
{
char _Tab1[]={
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 80, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
     32, 33, 34, 35, 83, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
     79, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 67, 61, 62, 63,
     64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
     80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
     96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
     80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
     67, 85, 69, 65, 65, 65, 65, 67, 69, 69, 69, 73, 73, 73, 65, 65,
     69, 65, 65, 79, 79, 79, 85, 85, 89, 79, 85, 67, 76, 89, 80, 70,
     65, 73, 79, 85, 78, 78, 65, 79, 67,169,170,171,172, 73,174,175,
    176,177,178,179,180,181,'H',183,184,185,186,187,188,189,190,191,
    'L',193,'T',195,196,197,198,'H',200,201,202,203,204,205,206,207,
    208,209, 78,211,212,213,214,'H',216,217,218,219,220,221,222,223,
     65, 66,226, 78, 69, 79, 85, 84,232, 79, 78, 68,236, 79, 69, 78,
    240,241,242,243,244,245,246,247, 79, 46, 46, 86, 78, 50,254, 32};
    

char _Tab2[]={
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
     32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
     48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
     64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
     80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
     96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
    112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
    199,252,233,226,228,224,229,231,234,235,232,239,238,236,196,197,
    201,230,198,244,246,242,251,249,255,214,220,248,163,216,215,159,
    225,237,243,250,241,209,170,186,191,174,172,189,188,161,171,187,
    176,177,178,179,180,193,194,192,169,185,186,187,188,162,165,191,
    192,193,194,195,196,197,227,195,200,201,202,203,204,205,206,164,
    240,208,202,203,200,213,205,206,207,217,218,219,220,166,204,223,
    211,223,212,210,245,213,181,222,254,218,219,217,253,221,175,180,
    173,177,242,190,182,167,247,184,176,168,250,185,179,178,183,160};


switch(table)
    {
    case 1:
        return _Tab1[car];
    case 2:
        return _Tab2[car];
    }

if (Cfg->font==0) return car;

switch(car)
    {
    case 0:
        return 32;

    case 'ä':
        return 232;
    case 'Ç':
        return 233;
    case 'à':
        return 234;
    case 'â':
        return 235;

    case 'ó':
        return 'u';
    case '£':
        return 'u';
    case 'ñ':
        return 'u';
    case 'Å':
        return 'u';

    case 'ç':
        return 'i';
    case '°':
        return 'i';
    case 'å':
        return 'i';
    case 'ã':
        return 'i';

    case 'ï':
        return 'o';
    case '¢':
        return 'o';
    case 'ì':
        return 'o';
    case 'î':
        return 'o';


    case 'Ö':
        return 224;
    case '†':
        return 225;
    case 'É':
        return 226;
    case 'Ñ':
        return 227;

    case 'á':
        return 231;

    case 0xFA:
        return 7;
    default:
        return car;
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
short p1,p2;
int ok,ok2;
char c1,c2,c3;
int n;

n=0;

ok2=1;      //--- Pas trouvÇ -------------------------------------------

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
            p2+=2;
        p1++;
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

strcpy(Ficname,relatif);

for(n=0;n<strlen(p);n++)
    if (p[n]=='/') p[n]=DEFSLASH;

for(n=0;n<strlen(Ficname);n++)
    if (Ficname[n]=='/') Ficname[n]=DEFSLASH;

m=0;
l=strlen(Ficname);
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
}

void TPath2Abs(char *p,char *Ficname)
{
int n;
static char old[256];     // Path avant changement

memcpy(old,p,256);

if (p[strlen(p)-1]==DEFSLASH) p[strlen(p)-1]=0;

if ( (!strncmp(Ficname,"..",2)) & (p[0]!=0) ) {
    for (n=strlen(p);n>0;n--)
        if (p[n]==DEFSLASH) {
            p[n]=0;
            break;
            }
    if (p[strlen(p)-1]==':') strcat(p,"\\");
    return;
    }

if (Ficname[0]!='.') {
    if  ( (Ficname[1]==':') & (Ficname[2]==DEFSLASH) )  {
        strcpy(p,Ficname);
        if (p[strlen(p)-1]==':') strcat(p,"\\");
        return;
        }

    if (Ficname[0]==DEFSLASH)  {
        strcpy(p+2,Ficname);
        if (p[strlen(p)-1]==':') strcat(p,"\\");
        return;
        }

    if (p[strlen(p)-1]!=DEFSLASH) strcat(p,"\\");
    strcat(p,Ficname);
   }

if (p[strlen(p)-1]==':') strcat(p,"\\");
}

/*--------------------------------------------------------------------*\
|- Recherche l'extension ext dans la chaine src                       -|
|- Renvoit 1 si trouve                                                -|
|- Ex: FoundExt(ext,"EXE COM BAT BTM")                                -|
\*--------------------------------------------------------------------*/

char FoundExt(char *ext,char *src)
{
char e[4];
short c1,c3,c4;

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

ltoa(n,s,10);

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
short i,j,n;

ltoa(entier,chaine2,10);

if ((n=strlen(chaine2))<10)
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
short j1;
int j3;

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
