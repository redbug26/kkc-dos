/*-----------------------------------------------------------------------*
 -                                                                       -
 -  Toutes les fonctions independantes (logiquement) Ö l'O.S. ainsi qu'  -
 -                                                   aux fichiers hard.c -
 -                                                                       -
 - By RedBug/Ketchup Killers                                             -
 -                                                                       -
 *-----------------------------------------------------------------------*/
#include <string.h>
#include <ctype.h>

void TPath2Abs(char *p,char *Ficname);


/*-----------------------------------------------------------*
 - Fonction qui convertit les caracteres ASCII en RedBug one -
 *-----------------------------------------------------------*/
char CnvASCII(char car)
{
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

/*------------------------------*
 - Renvoit l'extension d'un nom -
 *------------------------------*/
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

/*--------------------------------------*
 - Comparaison entre un nom et un autre -
 *--------------------------------------*/
int Maskcmp(char *src,char *mask)
{
int n;

for (n=0;n<strlen(mask);n++)
    {
    if ( (src[n]=='/') & (mask[n]=='/') ) continue;
    if ( (src[n]=='\\') & (mask[n]=='/') ) continue;
    if ( (src[n]=='\\') & (mask[n]=='\\') ) continue;
    if ( (src[n]=='/') & (mask[n]=='\\') ) continue;
    if (toupper(src[n])==toupper(mask[n])) continue;

    return 1;
    }
return 0;
}


/*-----------------------------------------------------------*
 - Envoie le nom du fichier src dans dest si le mask est bon -
 - Renvoit 1 dans ce cas, sinon renvoie 0                    -
 *-----------------------------------------------------------*/
int find1st(char *src,char *dest,char *mask)
{
int i;

if (!Maskcmp(src,mask))
    {
    if ( (src[strlen(mask)]=='\\') | (src[strlen(mask)]=='/') )
        memcpy(dest,src+strlen(mask)+1,strlen(src+strlen(mask)));
        else
        memcpy(dest,src+strlen(mask),strlen(src+strlen(mask))+1);
    for (i=0;i<strlen(dest);i++)
        {
        if (dest[i]=='\\') return 0;
        if (dest[i]=='/') return 0;
        }

    if (!strcmp(dest,"")) return 0;
    return 1;
    }

return 0;
}


/*----------------------------*
 - Comparaison avec WildCards -
 *----------------------------*/
int WildCmp(char *a,char *b)
{
short p1=0,p2=0;
int ok;
char c1,c2,c3;

ok=0;

while (1)
    {
    c1=a[p1];
    c2=b[p2];
    c3=b[p2+1];

    if ( (c1>='a') & (c1<='z') ) c1-=32;
    if ( (c2>='a') & (c2<='z') ) c2-=32;
    if ( (c3>='a') & (c3<='z') ) c3-=32;

    if ( ((c1==0) & (c2==0)) |
         ((c1==0) & (c2=='*') & (c3==0)) |
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

return ok;
}

/*---------------------------------*
 - Give the name include in a path -
 - E.G. c:\kkcom\kkc.c --> "kkc.c" -
 -      c:\            --> ""      -
 -      c:\kkcom       --> "kkcom" -
 -           p         --> Ficname -
 *---------------------------------*/
char *FileinPath(char *p,char *Ficname)
{
int n;
char *s;

for (n=0;n<strlen(p);n++)
    if ( (p[n]=='\\') | (p[n]=='/') )
        s=p+n+1;

strcpy(Ficname,s);

return s;
}

/*---------------------------------------------*
 - Make absolue path from old path and ficname -
 *---------------------------------------------*/
void Path2Abs(char *p,const char *relatif)
{
static char Ficname[256];
int l,m,n;
char car;

strcpy(Ficname,relatif);

for(n=0;n<strlen(p);n++)
    if (p[n]=='/') p[n]='\\';

for(n=0;n<strlen(Ficname);n++)
    if (Ficname[n]=='/') Ficname[n]='\\';

m=0;
l=strlen(Ficname);
for(n=0;n<l;n++)
    if (Ficname[n]=='\\')
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

if (p[strlen(p)-1]=='\\') p[strlen(p)-1]=0;

if ( (!strncmp(Ficname,"..",2)) & (p[0]!=0) ) {
    for (n=strlen(p);n>0;n--)
        if (p[n]=='\\') {
            p[n]=0;
            break;
            }
    if (p[strlen(p)-1]==':') strcat(p,"\\");
    return;
    }

if (Ficname[0]!='.') {
    if  ( (Ficname[1]==':') & (Ficname[2]=='\\') )  {
        strcpy(p,Ficname);
        if (p[strlen(p)-1]==':') strcat(p,"\\");
        return;
        }

    if (Ficname[0]=='\\')  {
        strcpy(p+2,Ficname);
        if (p[strlen(p)-1]==':') strcat(p,"\\");
        return;
        }

    if (p[strlen(p)-1]!='\\') strcat(p,"\\");
    strcat(p,Ficname);
   }

if (p[strlen(p)-1]==':') strcat(p,"\\");
}

