/*--------------------------------------------------------------------*\
|- Puissance 4: By RedBug/Ketchup^Pulpe                               -|
\*--------------------------------------------------------------------*/

#include <stdarg.h>
#include <dos.h>
#include <direct.h>
#include <io.h>
#include <stdlib.h>
#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "hard.h"

#define TRUE 1
#define FALSE 0

#define mess_you      "Which Row ?"
#define mess_ordi     "Please Wait"

char mogwai[42]; /* Endroit du pion au tour [] */

int tab[42];
int nfin[7];

void test_dbl(long *prob,int valeur,char colonne,char moi);
char gagne(void);
char avous(char moi,char begin);
char amoi(char moi);
void test(long  *prob,char ex,int valeur,char qui,char colonnes,char n);
void tr_colonnes(long  *prob,int valeur,char qui,char colonnes);
void print (char *);
void affiche(char x,char y);
void testpair(long *prob,char begin,char qui,char moi);
void deux(long *prob,char toi,int valeur);


#ifdef DEBUG
void ecris(long *prob,char n);
#endif


void P4(void)
{
char x,y,n,fin,joueur;

char begin;  //--- Joueur qui a commenc‚ … ce tour ---------------------
char nbr;    //--- Nombre de pion --------------------------------------

char o,m=3;

int cx,cy;

cx=15;
cy=2;

SaveScreen();

WinCadre(cx,cy,cx+44,cy+22,3);
Window(cx+1,cy+1,cx+43,cy+21,1*16+10);

PrintTo(0,0,"Four in a row");

/*--------------------------------------------------------------------*\
|- Initialisation des donnees                                         -|
\*--------------------------------------------------------------------*/

for (x=0;x<7;x++)  //--- initialise le Tableau -------------------------
    {
    PrintTo(x*6+3,20,"%d",x+1);
    nfin[x]=0;
    for (y=0;y<6;y++)
        tab[x+y*7]=0;
    }

nbr=0;             //--- Nombre de pion --------------------------------

fin=FALSE;

begin=(rand()&1)+1;

joueur=begin;

/*--------------------------------------------------------------------*\
|- Boucle principale                                                  -|
\*--------------------------------------------------------------------*/


while (fin==0)
    {
    if (joueur==2)
        {
        print(mess_you);

        n=0;
        do
            {
            o=Wait(0,0,0);
            if ( (o>='1') & (o<='7') )
                {
                m=o-'1';
                n=o;
                }
            if ((o&255)==0)
                {
                o=o/256;
                if (o=='M')
                    {
                    m++;
                    if (m==7) m=6;
                    }
                if (o=='K')
                    {
                    m--;
                    if (m==-1) m=0;
                    }
                if (o=='P') n=m+'1';
                }
            if ( (o==13) | (o==32) ) n=m+'1';
            if (o==27) n=27;
            }
        while( ((n<'1') | (n>'7')) & (n!=27) );

        if (n==27)
            {
            fin=1;
            break;
            }

        n=n-'1';
        }

    if (joueur==1)
        {
        print(mess_ordi);
        n=avous(joueur,begin);
        }

    m=n;
    if (nfin[n]<6)  /* 012345 */
        {
        tab[n+nfin[n]*7]=joueur;

        affiche(n,nfin[n]);
        nfin[n]++;

        joueur++;
        if (joueur==3)  joueur=1;

        mogwai[nbr]=n;
        nbr++;

        fin=gagne();

        if ( (fin==0) & (nbr==42) ) fin=3;
        }
    }

switch (fin)
    {
    case 1:
        ColRLin(18,0,25,11*16+1);
        print("I win");
        break;
    case 2:
        ColRLin(18,0,25,13*16+1);
        print("You win");
        break;
    case 3:
        ColRLin(18,0,25,12*16+1);
        print("Nobody win");
        break;
    }

Wait(0,0,0);

LoadScreen();
}


char gagne(void)
{
int x,y;

for (x=0;x<=3;x++)
    for(y=0;y<=5;y++)
        if ( (tab[x+y*7]==tab[(x+1)+y*7]) &
             (tab[(x+1)+y*7]==tab[(x+2)+y*7]) &
             (tab[(x+2)+y*7]==tab[(x+3)+y*7]) &
                                   (tab[x+y*7]!=0) ) return(tab[x+y*7]);

for (y=0;y<=2;y++)
    for(x=0;x<=6;x++)
        if ( (tab[x+y*7]==tab[x+(y+1)*7]) &
             (tab[x+(y+1)*7]==tab[(x)+(y+2)*7]) &
             (tab[(x)+(y+2)*7]==tab[(x)+(y+3)*7]) &
                                   (tab[x+y*7]!=0) ) return(tab[x+y*7]);

for (x=0;x<=3;x++)
    for(y=0;y<=2;y++)
        if ( (tab[x+y*7]==tab[(x+1)+(y+1)*7]) &
             (tab[(x+1)+(y+1)*7]==tab[(x+2)+(y+2)*7]) &
             (tab[(x+2)+(y+2)*7]==tab[(x+3)+(y+3)*7]) &
                                   (tab[x+y*7]!=0) ) return(tab[x+y*7]);

for (x=3;x<=6;x++)
    for(y=0;y<=2;y++)
        if ( (tab[x+y*7]==tab[(x-1)+(y+1)*7]) &
             (tab[(x-1)+(y+1)*7]==tab[(x-2)+(y+2)*7]) &
             (tab[(x-2)+(y+2)*7]==tab[(x-3)+(y+3)*7]) &
                         (tab[x+y*7]!=0) ) return(tab[x+y*7]);return(0);
}



void affiche(char x,char y)
{
char col[]={0,11*16+11,13*16+13};

for (x=0;x<7;x++)
    for (y=0;y<6;y++)
        {
        if (tab[x+y*7]!=0)
            {
            WinRCadre(x*6+1,(5-y)*3+2, x*6+5,(5-y)*3+4,1);
            ColRLin  (x*6+2,(5-y)*3+3, 3,col[tab[x+y*7]]);
            }
            else
            ColRWin (x*6+1,(5-y)*3+2, x*6+5,(5-y)*3+4,10*16+10);
        }
}

char avous(char moi,char begin)
{
long prob[7];

long ch[7];

char n,m,z,toi;
long temp;
int vt3,vm3,vt4,vm4;

if (moi==1) toi=2;
if (moi==2) toi=1;

for(n=0;n<=6;n++)
    prob[n]=0;

test_dbl(prob,450,4,moi);   // Teste si il y a un test_dble quelque part
test_dbl(prob,30,3,moi);    // Teste si il y a un test_dble de 3 "     "


/*--------------------------------------------------------------------*\
|-  Teste si il est possible de faire un quatre, un trois ou un deux  -|
|-    en mettant la piece correspondante dans la bonne colonne        -|
|-    Si on peut empecher l'autre de faire cela, ca marche aussi !    -|
\*--------------------------------------------------------------------*/

tr_colonnes( prob ,200000,moi,4);
tr_colonnes( prob ,9000,toi,4);
testpair(prob,begin,moi,moi);

tr_colonnes( prob ,32,moi,3); // Marjo
testpair(prob,begin,toi,moi);
tr_colonnes( prob ,35,toi,3); // Marjo

tr_colonnes( prob ,10,moi,2);
tr_colonnes( prob ,12,toi,2);


/*--------------------------------------------------------------------*\
|-  Teste l'influence de mon pion pour le prochain coups              -|
\*--------------------------------------------------------------------*/

for (n=0;n<7;n++)
    {
    if (nfin[n]<5)      // 01234: Si il n'y a pas 6 pions … la colonne n
        {
        if ( (nfin[n] % 2)==0)
            prob[n]=prob[n]+15;
            else
            prob[n]=prob[n]-10;     /* OK */

        tab[n+nfin[n]*7]=moi;
        vt3=-25;
        vt4=-15000;
        test(prob,nfin[n]+1,vt3,toi,3,n);
        test(prob,nfin[n]+1,vt4,toi,4,n);

        tab[n+nfin[n]*7]=toi;
        vm3=-20;
        vm4=-100;
        test(prob,nfin[n]+1,vm3,moi,3,n);
        test(prob,nfin[n]+1,vm4,moi,4,n);

        tab[n+nfin[n]*7]=0;
        }
    }

for (n=0;n<7;n++)
   {
    // Teste si il est possible de faire un double si on met un pion ici
   if (nfin[n]<6) /* 012345 */
    {
    tab[n+nfin[n]*7]=moi;
    temp=0;
    for (z=0;z<7;z++)       ch[z]=0;
    test_dbl(ch,100,4,moi);
    for (z=0;z<7;z++)       temp+=ch[z];
    if (temp!=0)        prob[n]+=275;
        /* Je peux faire un double aprŠs si je mets mon pion en n */

    tab[n+nfin[n]*7]=toi;
    temp=0;
    for (z=0;z<7;z++)   ch[z]=0;

    test_dbl(ch,100,4,toi);

    for (z=0;z<7;z++)   temp+=ch[z];
    if (temp!=0) prob[n]+=250;
        /* Il peut faire un double aprŠs si il met son pion en n */

    tab[n+nfin[n]*7]=0;
    }
   }

for (n=0;n<7;n++)
    {
    if (nfin[n]<5)  /* 01234: Si il n'y a pas 6 pions … la colonne n */
        {
        tab[n+nfin[n]*7]=moi;

/*      tab[n+(nfin[n]+1)*7]=moi;
        temp=0;
        for (z=0;z<7;z++)       ch[z]=0;
        test_dbl(ch,100,4,moi);
        for (z=0;z<7;z++)       temp+=ch[z];
        if (temp!=0)        prob[n]+=25;*/
        /* Je peux faire un double aprŠs si je mets mon pion en n */

        tab[n+(nfin[n]+1)*7]=toi;
        temp=0;
        for (z=0;z<7;z++)   ch[z]=0;

        test_dbl(ch,100,4,toi);

        for (z=0;z<7;z++)   temp+=ch[z];
        if (temp!=0) prob[n]-=150;
            /* Il peut faire un double aprŠs si il met son pion en n */

        tab[n+(nfin[n]+1)*7]=0;


        tab[n+nfin[n]*7]=0;
        }
    }


deux(prob,toi,200);
deux(prob,moi,200);

for(n=0;n<7;n++)
   for(m=0;m<7;m++)
      {
      if (prob[n]==prob[m]) prob[m]=prob[m]+(rand()&1);
      prob[n]=prob[n]+(rand()&1);
      }


m=0;
temp=-320000L;

for(n=0;n<7;n++)
    if ( (prob[n]>=temp) & (nfin[n]!=6) )
        {
        m=n;
        temp=prob[m];
        }

return(m);
}

void tr_colonnes(long *prob,int valeur,char qui,char colonnes)
{
int n;
int y;
char ex,oui;

for (n=0;n<7;n++)
    {
    oui=FALSE;
    for (y=0;y<6;y++)
        if (tab[n+y*7]==0)
            {
            oui=TRUE;
            ex=y;
            y=6;
            }
    if (oui==TRUE)
        test(prob,ex,valeur,qui,colonnes,n);
   }
}

void test(long *prob,char ex,int valeur,char qui,char colonnes,char n)
{
char nombre;
char x,y;
char temp;
int t1,t2;
char v;

if ( ( (n>6) | (ex>5) ) & (tab[n+ex*7]==0) )
    {
    v=v;
    }

if (tab[n+ex*7]==0)
  {
  tab[n+ex*7]=qui;
  for(x=(n>3)*(n-3);x<=n-(n>3)*(n-3);x++)
        {
       nombre=0;
       for (y=0;y<=3;y++)
        {
        temp=tab[x+y+ex*7];
        if (temp==qui) nombre++;    else if ( temp!=0) nombre=5;
        }
       if ( nombre==colonnes ) prob[n]=prob[n]+valeur;
       }

  for (x=(ex>3)*(ex-3);x<=ex-(ex>2)*(ex-2);x++)
       {
       nombre=0;
       for (y=0;y<=3;y++)
        {
        temp=tab[n+(x+y)*7];
        if (temp==qui ) nombre++;    else if ( temp!=0) nombre=5;
        }
       if ( nombre==colonnes ) prob[n]=prob[n]+valeur;
       }

    t1=min(ex,n);  if (t1>=4) t1=3;
    if ( (ex<=2) & (n<=3) ) t2=0;
        else
        t2=max(ex-2,n-3);

    for (x=-t1;x<=-t2;x++)
        {
        nombre=0;
        for (y=0;y<=3;y++)
            {
            temp=tab[n+x+y+(ex+x+y)*7];
            if (temp==qui )
                nombre++;
                else if ( temp!=0) nombre=5;
            }
        if ( nombre==colonnes ) prob[n]=prob[n]+valeur;
            }

    v=5-ex;
    t1=min(n,v);  if (t1>=4) t1=3;
    if ( (v<=2) & (n<=3) ) t2=0;
        else
        t2=max(v-2,n-3);

    for (x=-t1;x<=-t2;x++)
       {
         nombre=0;
         for (y=0;y<=3;y++)
        {
        temp=tab[n+x+y+(5-(v+x+y))*7];
        if (temp==qui ) nombre++;    else if ( temp!=0) nombre=5;
        }
        if ( nombre==colonnes ) prob[n]=prob[n]+valeur;
       }
  tab[n+ex*7]=0;
  }
}


void print(char *n)
{
PrintTo(18,0,"%25s",n);
}


void test_dbl(long *prob,int valeur,char colonne,char moi)
{
long chance[7];
int tableau[42];
int x,y,z;

for (x=0;x<7;x++)
   for (y=0;y<6;y++)
       {
       tableau[x+y*7]=0;
       if (tab[x+y*7]==0)
            {
            for (z=0;z<7;z++)   chance[z]=0;

            test(chance,y,2,moi,colonne,x);
        /* met un pion … moi dans tab(x,y) et incremente chance[x] de 2
            si je fais 4 lignes en suivant */

            for (z=0;z<7;z++)
                {
                if (chance[z]!=0)
                    tableau[x+y*7]++;
                }
            }
       }

for (x=0;x<7;x++)
   for (y=0;y<5;y++)
     if ( ((tableau[x+y*7])!=0) & ((tableau[x+(y+1)*7])!=0) )
        prob[x]=prob[x]+valeur;
}




void deux(long *prob,char toi,int valeur)
{
int n1,n2;
long tempon[7];

memset(tempon,0,28);

for(n1=0;n1<7;n1++)
    if(nfin[n1]!=6)
        {
        tab[n1+nfin[n1]*7]=toi;
        if (gagne()==toi)
            {
            tab[n1+nfin[n1]*7]=0;
            return;
            }
        tab[n1+nfin[n1]*7]=0;
        }

for (n1=0;n1<6;n1++)
    for(n2=n1+1;n2<7;n2++)
        if ( (n1!=n2) & (nfin[n1]!=6) & (nfin[n2]!=6) )
            {
            tab[n2+nfin[n2]*7]=toi;
            tab[n1+nfin[n1]*7]=toi;
            if (gagne()==toi)
                {
                tempon[n1]+=1;
                tempon[n2]+=1;
                }
            tab[n1+nfin[n1]*7]=0;
            tab[n2+nfin[n2]*7]=0;
            }
n2=0;
for (n1=0;n1<7;n1++)
    if (tempon[n1]>=2) n2++;

if (n2!=0)
    for (n1=0;n1<7;n1++)
        if (tempon[n1]!=0) prob[n1]+=valeur;


}


#define vo 25
#define vn 20

void testpair(long *prob,char begin,char qui,char moi)
{
char nombre;
char x,y;
char py,px;
char temp;
int t1,t2;
char v;
int n;
int ry;
char ex,oui;

for (n=0;n<7;n++)
    {
    oui=0;
    for (ry=0;ry<6;ry++)
        if (tab[n+ry*7]==0)
            {
            oui=1;
            ex=ry;
            ry=6;
            }

    if (oui==1)
        {
        if ( ( (n>6) | (ex>5) ) & (tab[n+ex*7]==0) )
            exit(1);

        if (tab[n+ex*7]==0)
            {
            tab[n+ex*7]=qui;
            for(x=(n>3)*(n-3);x<=n-(n>3)*(n-3);x++)
                {
                nombre=0;
                for (y=0;y<=3;y++)
                    {
                    temp=tab[x+y+ex*7];
                    switch(temp) {
                        case 0: py=y; px=x; break;
                        default:
                            if (temp==qui)
                                nombre++;
                                else
                                nombre=5;
                            break;
                        }
                    }
                if (nombre==3)
                   {
                   prob[n]+=vo;
                   if ( (begin==moi) & ((nfin[px]%2)==1) & (moi!=qui) ) prob[n]+=vn;
                   if ( (begin==moi) & ((nfin[px]%2)!=1) & (moi==qui) ) prob[n]+=vn;
                   if ( (begin!=moi) & ((nfin[px]%2)==1) & (moi==qui) ) prob[n]+=vn;
                   if ( (begin!=moi) & ((nfin[px]%2)!=1) & (moi!=qui) ) prob[n]+=vn;
                   }
                }

            for (x=(ex>3)*(ex-3);x<=ex-(ex>2)*(ex-2);x++)
                {
                nombre=0;
                for (y=0;y<=3;y++)
                   {
                   temp=tab[n+(x+y)*7];
                   switch(temp)
                        {
                        case 0:
                            py=y;
                            px=x;
                            break;
                        default:
                           if (temp==qui)
                               nombre++;
                               else
                               nombre=5;
                           break;
                       }
                   }
                if (nombre==3)
                   {
                   prob[n]+=vo;
                   if ( (begin==moi) & ((nfin[px]%2)==1) & (moi!=qui) ) prob[n]+=vn;
                   if ( (begin==moi) & ((nfin[px]%2)!=1) & (moi==qui) ) prob[n]+=vn;
                   if ( (begin!=moi) & ((nfin[px]%2)==1) & (moi==qui) ) prob[n]+=vn;
                   if ( (begin!=moi) & ((nfin[px]%2)!=1) & (moi!=qui) ) prob[n]+=vn;
                   }
               }

           t1=min(ex,n);  if (t1>=4) t1=3;
           if ( (ex<=2) & (n<=3) ) t2=0;
               else
               t2=max(ex-2,n-3);

           for (x=-t1;x<=-t2;x++)
               {
               nombre=0;
               for (y=0;y<=3;y++)
                   {
                   temp=tab[n+x+y+(ex+x+y)*7];
                   switch(temp) {
                       case 0: py=y; px=x; break;
                       default:
                           if (temp==qui)
                               nombre++;
                               else
                               nombre=5;
                           break;
                       }
                   }
               if (nombre==3)
                   {
                   prob[n]+=vo;
                   if ( (begin==moi) & ((nfin[px]%2)==1) & (moi!=qui) ) prob[n]+=vn;
                   if ( (begin==moi) & ((nfin[px]%2)!=1) & (moi==qui) ) prob[n]+=vn;
                   if ( (begin!=moi) & ((nfin[px]%2)==1) & (moi==qui) ) prob[n]+=vn;
                   if ( (begin!=moi) & ((nfin[px]%2)!=1) & (moi!=qui) ) prob[n]+=vn;
                   }
               }

           v=5-ex;
           t1=min(n,v);
           if (t1>=4) t1=3;
           if ((v<=2) & (n<=3))
                t2=0;
               else
               t2=max(v-2,n-3);

           for (x=-t1;x<=-t2;x++)
               {
               nombre=0;
               for (y=0;y<=3;y++)
                   {
                   temp=tab[n+x+y+(5-(v+x+y))*7];
                   switch(temp) {
                       case 0: py=y; px=x; break;
                       default:
                           if (temp==qui)
                               nombre++;
                               else
                               nombre=5;
                           break;
                       }
                   }
               if (nombre==3)
                   {
                   prob[n]+=vo;
                   if ( (begin==moi) & ((nfin[px]%2)==1) & (moi!=qui) )
                     prob[n]+=vn;
                   if ( (begin==moi) & ((nfin[px]%2)!=1) & (moi==qui) )
                        prob[n]+=vn;
                   if ( (begin!=moi) & ((nfin[px]%2)==1) & (moi==qui) ) prob[n]+=vn;
                   if ( (begin!=moi) & ((nfin[px]%2)!=1) & (moi!=qui) ) prob[n]+=vn;
                   }
               }
           tab[n+ex*7]=0;
           } /* fin de la conditio if tab */
        } /* fin de la condition if oui==true */
    } /* fin du for n=0;6 */
}


#ifdef DEBUG

void ecris(long *prob,char n)
{
int x;

for (x=0;x<7;x++)
    {
    int z;
    z=prob[x];
    if (z>9999)
        z=9999;
    if (z<-999)
        PrintAt(x*6+17,n,"-----",z);
    else
        PrintAt(x*6+17,n,"%-5d",z);
    }
PrintAt(70,n,"%d",n);
}

#endif
