#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <dos.h>
#include <direct.h>
#include <ctype.h>
#include <malloc.h>
#include <bios.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include "kk.h"
#include "win.h"
#include "idf.h"

int *TailleX;


// Avancement de graduation
// Renvoit le prochain
int Gradue(int x,int y,int length,int from,int to,int total)
{
short j1,j2;
int j3;

if (total==0) return 0;

if ( (to>1000) & (total>1000) )
    {
    j3=(to/1000);
    j3=(j3*length*8)/(total/1000);
    }
    else
    j3=(to*length*8)/total;

if (j3>=(length*8)) j3=(length*8)-1;

j1=from;

for (;j1<j3;j1++)
    {
    j2=j1/8;
    if (Cfg->UseFont==0)
    switch(j1%8) {
        case 0:
            AffChr(j2+x,y,'*'); // b
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            AffChr(j2+x,y,'*'); // d
            AffChr(j2+x+1,y,32); // i
            break;
        case 5:
        case 6:
        case 7:
            AffChr(j2+x,y,32); // g
            AffChr(j2+x+1,y,'*'); // l
            break;
        }
    else
    switch(j1%8) {
        case 0:
            AffChr(j2+x,y,156); // b
            break;
        case 1:
            AffChr(j2+x,y,157); // c
            AffChr(j2+x+1,y,32); // c
            break;
        case 2:
            AffChr(j2+x,y,158); // d
            AffChr(j2+x+1,y,163); // i
            break;
        case 3:
            AffChr(j2+x,y,159); // e
            AffChr(j2+x+1,y,164); // j
            break;
        case 4:
            AffChr(j2+x,y,160); // f
            AffChr(j2+x+1,y,165); // k
            break;
        case 5:
            AffChr(j2+x,y,161); // g
            AffChr(j2+x+1,y,166); // l
            break;
        case 6:
            AffChr(j2+x,y,162); // h
            AffChr(j2+x+1,y,167); // k
            break;
        case 7:
            AffChr(j2+x,y,32);  // a
            AffChr(j2+x+1,y,155);
            break;
        }
    }

if (to==total)
    ChrLin(x,y,length+1,32);

if (to==0)
    if (Cfg->UseFont==0)
    AffChr(x,y,'*'); // b
    else
    AffChr(x,y,155);

return j1;
}

// si p vaut 0 mets off
// si p vaut 1 interroge
// retourne -1 si SHIFT TAB, 1 si TAB
int Puce(int x,int y,int lng,char p)
{
int r=0;
char a;

AffChr(x,y,16);
AffChr(x+lng-1,y,17);

AffChr(x+lng,y,220);
ChrLin(x+1,y+1,lng,223);

ColLin(x,y,lng,2*16+5);        // Couleur

if (p==1)
    while (r==0)  {
        a=getch();
        if (a==13) return 0;
        if (a==27) r=1;
        if (a==9) r=2;
        if (a==0) {
            a=getch();
            if (a==0x0F) r=3;
            if (a==0x4D) r=2;
            if (a==0x4B) r=3;
            }
        }

AffChr(x,y,32);
AffChr(x+lng-1,y,32);

AffChr(x+lng,y,220);
ChrLin(x+1,y+1,lng,223);

ColLin(x,y,lng,2*16+3);        // Couleur

return r;
}


// Retourne 27 si escape
// Retourne numero de la liste sinon

int WinTraite(struct Tmt *T,int nbr,struct TmtWin *F)
{
char fin;       // si =0 continue
char direct;    // direction du tab
int i;

SaveEcran();


WinCadre(F->x1,F->y1,F->x2,F->y2,0);
ColWin(F->x1+1,F->y1+1,F->x2-1,F->y2-1,10*16+1);
ChrWin(F->x1+1,F->y1+1,F->x2-1,F->y2-1,32);

PrintAt(F->x1+((F->x2-F->x1)-(strlen(F->name)))/2,F->y1,F->name);

for(i=0;i<nbr;i++)
switch(T[i].type) {
    case 0:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str);
        break;
    case 1:
        ColLin(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier),1*16+5);
        ChrLin(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier),32);
        break;
    case 2:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,"      OK     ");
        Puce(F->x1+T[i].x,F->y1+T[i].y,13,0);
        break;
    case 3:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,"    CANCEL   ");
        Puce(F->x1+T[i].x,F->y1+T[i].y,13,0);
        break;
    case 4:
        WinCadre(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier)+F->x1+T[i].x,F->y1+T[i].y+3,1);
        break;
    case 5:
        PrintAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str);
        Puce(F->x1+T[i].x,F->y1+T[i].y,13,0);
        break;
    case 6:
        WinCadre(F->x1+T[i].x,F->y1+T[i].y,*(T[i].entier)+F->x1+T[i].x,F->y1+T[i].y+2,2);
        break;
    }

fin=0;
direct=1;
i=0;

while (fin==0) {

switch(T[i].type) {
    case 0:
    case 4:
        break;
    case 1:
        direct=InputAt(F->x1+T[i].x,F->y1+T[i].y,T[i].str,*(T[i].entier));
        break;
    case 2:
        direct=Puce(F->x1+T[i].x,F->y1+T[i].y,13,1);
        break;
    case 3:
        direct=Puce(F->x1+T[i].x,F->y1+T[i].y,13,1);
        break;
    case 5:
        direct=Puce(F->x1+T[i].x,F->y1+T[i].y,13,1);
        break;
    }

if (direct==0) fin=1;   // ENTER
if (direct==1) fin=2;   // ESC
if (direct==2) i++;
if (direct==3) i--;

if (i==-1) i=nbr-1;
if (i==nbr) i=0;

}

ChargeEcran();

if (fin==1)
    return i;

return 27;  // ESCAPE
}


void ErrWin95(void)
{
int x,y;
char c=0;

while (c!=27) {
	  for (x=0;x<(*TailleX);x++)
          for (y=0;y<Cfg->TailleY;y++)  {
			  AffChr(x,y,rand()&255);
			  AffCol(x,y,26 + (rand()&1));
			  }
	  PrintAt(26,11,"*************************");
	  PrintAt(26,12,"* WINDOWS 95 KEYBOARD ! *");
	  PrintAt(26,13,"*************************");
	  if (kbhit()) c=getch();
	  }

AfficheTout();
}

int InfoIDF(struct fenetre *Fen)
{
struct file *F;
struct info Info;

F=Fen->F[Fen->pcur];

if ( (F->attrib & _A_SUBDIR)==_A_SUBDIR)    {
   PrintAt(0,0,"%-40s%-40s","Directory",F->name);
   return 0;
   }

strcpy(Info.path,DFen->path);
if (Info.path[strlen(Info.path)-1]!='\\') strcat(Info.path,"\\");
strcat(Info.path,F->name);

Traitefic(F->name,&Info);


// sprintf(Info.path,"%s%s",drive,dir);

PrintAt(0,0,"%-40s%-40s",Info.format,Info.fullname);

return Info.numero;
}

void ClearNor(struct fenetre *Fen)
{
int i;
int x,y;
int yl;

char a;

a=Cfg->Tfont[0];

ColWin(Fen->x,Fen->y,Fen->x+Fen->xl,Fen->y+Fen->yl,7*16+6);


// Couleur uniquement pour fentype=1,2 ou 3 (le 4 le fait 2 fois !)
for(x=Fen->x;x<=Fen->x+Fen->xl;x++)
    AffCol(x,Fen->y,10*16+1);
for(y=Fen->y;y<=Fen->y+Fen->yl;y++)
    AffCol(Fen->x,y,10*16+1);

for(x=Fen->x+1;x<=Fen->x+Fen->xl;x++)
    AffCol(x,Fen->y+Fen->yl,10*16+3);
for(y=Fen->y+1;y<Fen->y+Fen->yl;y++)
    AffCol(Fen->x+Fen->xl,y,10*16+3);


x=Fen->x;
y=Fen->y;

yl=(Fen->yl)+(Fen->y);

switch (Cfg->fentype)  {
    case 1:
        Cfg->Tfont[0]=179;
        Fen->x2=Fen->x-1;
        Fen->y2=Fen->y-1;
        Fen->xl2=Fen->xl;
        Fen->yl2=Fen->yl-4;
        Fen->x3=Fen->x+2;
        Fen->y3=Fen->y+Fen->yl-1;

        PrintAt(x,y     ,"ÉÍÍÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÑÍÍÍÍÍ»");
        PrintAt(x,y+1,   "º    Name    ³    Size  ³  Date  ³ Timeº");
        for (i=2;i<yl-3;i++)
           PrintAt(x,y+i,"º            ³          ³        ³     º");
        PrintAt(x,y+yl-3,"ÇÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄ¶");
        PrintAt(x,y+yl-2,"º                                      º");
        PrintAt(x,y+yl-1,"ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼");

        break;
    case 2:
        Cfg->Tfont[0]=179;
        Fen->x2=Fen->x-1;
        Fen->y2=Fen->y-2;
        Fen->xl2=(Fen->xl);
        Fen->yl2=Fen->yl-3;
        Fen->x3=Fen->x+2;
        Fen->y3=Fen->y+Fen->yl-1;

        ColLin(x,y,40,Cfg->bkcol);
        PrintAt(x,y,     "     Name    ³    Size  ³  Date  ³ Time ");
        for (i=1;i<yl-3;i++)
           PrintAt(x,y+i,"Û            ³          ³        ³     Û");
        PrintAt(x,y+yl-3,"ÛßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßÛ");
        PrintAt(x,y+yl-2,"Û                                      Û");
        PrintAt(x,y+yl-1,"ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß");
        break;
    case 3:
        Cfg->Tfont[0]=179;
        Fen->x2=Fen->x-1;
        Fen->y2=Fen->y-2;
        Fen->xl2=Fen->xl;
        Fen->yl2=Fen->yl-3;
        Fen->x3=Fen->x+2;
        Fen->y3=Fen->y+Fen->yl-1;

        PrintAt(x,y     ,"ÚÄúÄúNameúÄúÄÂúÄúSizeúÄúÂÄúDateúÄÂTimeú¿");
        for (i=1;i<yl-3;i++)
           PrintAt(x,y+i,"³            ³          ³        ³     ³");
        PrintAt(x,y+yl-3,"³ÚÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄ¿³");
        PrintAt(x,y+yl-2,"³                                      ³");
        PrintAt(x,y+yl-1,"ÀÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙÙ");
        break;
    case 4:
        Fen->x2=Fen->x-1;
        Fen->y2=Fen->y-1;
        Fen->xl2=Fen->xl;
        Fen->yl2=Fen->yl-4;
        Fen->x3=Fen->x+2;
        Fen->y3=Fen->y+Fen->yl-1;

        ColLin(x+ 5,y+1,4,7*16+5);
        ColLin(x+18,y+1,4,7*16+5);
        ColLin(x+27,y+1,4,7*16+5);
        ColLin(x+34,y+1,4,7*16+5);
        ChrLin(x+1,y+yl-2,38,32);

        PrintAt(x+1,y+1,   "    Name    %c    Size  %c  Date  %cTime ",a,a,a);
        for (i=2;i<yl-3;i++)
           PrintAt(x+1,y+i,   "            %c          %c        %c     ",a,a,a);

        WinLine(x+1,y+yl-3,38,1);
        WinCadre(x,y,x+39,y+yl-1,1);

        break;
    }
}


void FenNor(struct fenetre *Fen)
{
int j,n;  // Compteur
int date,time;
char chaine[255];
char temp[16];
char nom[13],ext[4];

short i;

char a;                 // Separator Character

int x1,y1;
int x=2,y=3;

if (Fen->init==1)
    ClearNor(Fen);

if (Fen->scur>Fen->pcur) Fen->pcur=Fen->scur;

while (Fen->pcur<0)
    {
    Fen->scur++;
    Fen->pcur++;
    }

while (Fen->pcur>=Fen->nbrfic)
    {
    Fen->pcur--;
    Fen->scur--;
    }

if (Fen->scur<0)
    Fen->scur=0;

if (Fen->scur>Fen->yl2-1)
    Fen->scur=Fen->yl2-1;

a=Cfg->Tfont[0];

x1=Fen->x2;
y1=Fen->y2;

n=(Fen->pcur)-(Fen->scur);			  // premier

InfoSelect(Fen);

for (i=0;(i<Fen->yl2) & (n<Fen->nbrfic);i++,n++,y++)
    {
// ------------------ Line Activity ------------------------------------
    if ((Fen->actif==1) & (n==(Fen->pcur)) )
        {
        if (Fen->F[n]->select==0)
            ColLin(x+x1,y+y1,38,1*16+6);
            else
            ColLin(x+x1,y+y1,38,1*16+5);
        }
        else
        {
        if (Fen->F[n]->select==0)
            ColLin(x+x1,y+y1,38,7*16+6);
            else
            ColLin(x+x1,y+y1,38,7*16+5);
        }

// ---------------------------------------------------------------------
    date=Fen->F[n]->date;
    time=Fen->F[n]->time;

    strcpy(nom,Fen->F[n]->name);
    if ((Fen->F[n]->attrib& _A_SUBDIR)!=_A_SUBDIR)
        strlwr(nom);


    ext[0]=0;
    j=0;

    if (nom[0]!='.')
        while(nom[j]!=0)
            {
            if (nom[j]=='.')
                {
                memcpy(ext,nom+j+1,4);
                ext[3]=0;
                nom[j]=0;
                break;
                }
            j++;
            }
    nom[8]=0;


    Int2Char((date>>9)+80,temp,2);

    sprintf(chaine,"%-8s %-3s%c%10s%c%02d/%02d/%2s%c%02d:%02d",
                      nom,ext,a,Long2Str(Fen->F[n]->size),a,
                     (date&31),(date>>5)&15,temp,a,
					 (time>>11)&31,(time>>5)&63
					 );

    if ((Fen->F[n]->attrib & _A_SUBDIR)==_A_SUBDIR)
        memcpy(chaine+13,"SUB--DIR",10);

    if ((Fen->F[n]->attrib & _A_VOLID)==_A_VOLID)
        sprintf(chaine,"%-8s%-3s %c##Vol-ID##%c%02d/%02d/%2s%c%02d:%02d",
                     nom,ext,a,a,
                     (date&31),(date>>5)&15,temp,a,
					 (time>>11)&31,(time>>5)&63
					 );

    if (nom[1]==':')
        sprintf(chaine,"%-8s    %c##Reload##%c        %c     ",nom,a,a,a);

    if (nom[0]=='.')
        {
        if (nom[1]!='.')
            memcpy(chaine+13," RELOAD ",10);
            else
        memcpy(chaine+13," UP-DIR ",10);
        }


    PrintAt(x+x1,y+y1,chaine);
    }

Fen->paff=Fen->pcur;
Fen->saff=Fen->scur;
Fen->init=0;
}


/********************************************
             For the FILE_ID.DIZ
 ********************************************/

void Makediz(struct info *Info,char *Buf)
{
struct dosdate_t Date;
char ligne[256];

_dos_getdate(&Date);


sprintf(ligne,"                                    \r\n");
strcpy(Buf,ligne);
sprintf(ligne,"      Ketchup Killers  (C) %4d     \r\n",Date.year);
strcat(Buf,ligne);
sprintf(ligne,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\r\n");
strcat(Buf,ligne);

if (*Info->fullname!=0)  {
        ClearSpace(Info->fullname);
        Info->fullname[30]=0;

        if (strlen(Info->fullname)<=26)
                sprintf(ligne," Title: %27s \r\n",Info->fullname); //34
                else
                sprintf(ligne," T.:%31s \r\n",Info->fullname); //34
        strcat(Buf,ligne); }

if (*Info->format!=0) {
        sprintf(ligne," Type : %27s \r\n",Info->format); //34
        strcat(Buf,ligne); }

if (*Info->info!=0) {
        sprintf(ligne," %-14s : %17s \r\n",Info->Tinfo,Info->info); //34
        strcat(Buf,ligne); }

if (*Info->composer!=0) {
        sprintf(ligne," Composer: %24s \r\n",Info->composer); //34
        strcat(Buf,ligne); }

if (*Info->message[0]!=0) {
        short m;
        sprintf(ligne,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\r\n");
        strcat(Buf,ligne);
        for (m=0;m<9;m++)
                {
                if (*Info->message[m]!=0)
                        {
                        sprintf(ligne,"%-36s\r\n",Info->message[m]); //36
                        strcat(Buf,ligne);
                        }
                }
        }

if (Info->taille!=0) {
        sprintf(ligne," True Size: %23ld \r\n",Info->taille);
        strcat(Buf,ligne); }
sprintf(ligne,"                                    \r\n");
strcat(Buf,ligne);

return;
}

void FenDIZ(struct fenetre *Fen)
{
char *Buf;
struct info Info;
struct fenetre *Fen2;
struct file *F;
int x,y,i;

Fen2=Fen->Fen2;
F=Fen2->F[Fen2->pcur];
if ( (F->attrib & _A_SUBDIR)==_A_SUBDIR) return;

WinCadre(Fen->x,Fen->y,Fen->x+Fen->xl,Fen->y+Fen->yl,1);

strcpy(Info.path,Fen2->path);
if (Info.path[strlen(Info.path)-1]!='\\') strcat(Info.path,"\\");
strcat(Info.path,F->name);

Traitefic(F->name,&Info);

Buf=malloc(4000);

Makediz(&Info,Buf);

ChrWin(Fen->x+1,Fen->y+1,Fen->x+Fen->xl-1,Fen->y+4,' ');
ColWin(Fen->x+1,Fen->y+1,Fen->x+Fen->xl-1,Fen->y+4,170);

x=Fen->x+2;
y=Fen->y+6;

i=0;
while (Buf[i]!=0)  {
	switch (Buf[i]) {
           case 10: x=Fen->x+2; break;
		   case 13: y++; break;
		   default: AffChr(x,y,Buf[i]); x++; break;
		   }
	i++;
	}

WinCadre(Fen->x+1,Fen->y+5,Fen->x+Fen->xl-1,y,2);
ColWin(Fen->x+2,Fen->y+6,Fen->x+Fen->xl-2,y-1,10*16+1);


ChrWin(Fen->x+1,y+1,Fen->x+Fen->xl-1,Fen->y+Fen->yl-1,' ');
ColWin(Fen->x+1,y+1,Fen->x+Fen->xl-1,Fen->y+Fen->yl-1,170);

free (Buf);

Fen->init=0;
}

/**********************************************
            Display about select
 **********************************************/


void InfoSelect(struct fenetre *Fen)
{
if (Fen->nbrsel==0)  {
   if (Fen->nbrfic==1)
      PrintAt(Fen->x3,Fen->y3,"%-10s bytes in one file         ",Long2Str(Fen->taillefic));
	  else
      PrintAt(Fen->x3,Fen->y3,"%-10s bytes in %3d files        ",Long2Str(Fen->taillefic),Fen->nbrfic);
   }
   else {
   if (Fen->nbrsel==1)
      PrintAt(Fen->x3,Fen->y3,"%-10s b. in one selected file   ",Long2Str(Fen->taillesel));
	  else
      PrintAt(Fen->x3,Fen->y3,"%-10s b. in %3d selected files  ",Long2Str(Fen->taillesel),Fen->nbrsel);
   }

}


//-----------------------------------------------------------------------------
// The KEY's menu
//-----------------------------------------------------------------------------
void CtrlMenu(void)
{
if (DFen->nbrfic==1)
    PrintAt(DFen->x3,DFen->y3,"%-10s bytes in one file         ",Long2Str(DFen->taillefic));
    else
    PrintAt(DFen->x3,DFen->y3,"%-10s bytes in %3d files        ",Long2Str(DFen->taillefic),DFen->nbrfic);
}

void ShiftMenu(void)
{
char chaine[37];
memcpy(chaine,DFen->path,37);
chaine[36]=0;
PrintAt(DFen->x3,DFen->y3,"%-36s",chaine);
}

void SetTaille(void)
{
TXTMode(Cfg->TailleY);
NoFlash();


switch (Cfg->TailleY)  {
   case 50:
        Font();
        break;
   case 25:
        Cfg->UseFont=0;
        Cfg->Tfont[0]=179;
        break;
   }
DFen->yl=(Cfg->TailleY)-4;
DFen->Fen2->yl=(Cfg->TailleY)-4;

AfficheTout();

LoadPal();

   /*
SetPal(0, 43, 37, 30);
SetPal(1, 31, 22, 17);
SetPal(2, 63, 63, 63);
SetPal(3, 58, 58, 50);
SetPal(4, 44, 00, 21);
SetPal(5, 63, 63, 21);

SetPal(7,  0,  0,  0); */

}



void AltMenu(void)
{
PrintAt(DFen->x3,DFen->y3,"[ Hello World !!!                  ]");
}

void MenuBar(char c)
{
static char bar[4][60]=
   { " Help  ----  View  Edit  Copy  ----  MDir Delete ----  Quit ",      // NORMAL
     " ----  ----  View  ----  ----  ----  ----  ----  ----  ---- ",      // SHIFT
     "On-OffOn-Off Name  .Ext  Date  Size Unsort ----  ----  ---- ",      // CONTROL
     " Drv1  Drv2  FDiz  ----  ----  ---- Search Type  Line  ---- "       // ALT
   };
char i,j,n;
static char d=-1;
int TY;

TY=Cfg->TailleY;

if (d==c) return;
d=c;


if (DFen->FenTyp==0)    // Fenetre Normale
switch(c) {
    case 0:
        InfoSelect(DFen);
        break;
    case 1:
        ShiftMenu();
        break;
    case 2:
        CtrlMenu();
        break;
    case 3:
        AltMenu();
        break;
    }

n=0;
for (i=0;i<10;i++)	{
	PrintAt(n,TY-1,"F%d",(i+1)%10);
    for(j=0;j<2;j++,n++) AffCol(n,TY-1,1*16+8);
	for(j=0;j<6;j++,n++) {
       AffCol(n,TY-1,1*16+2);
	   AffChr(n,TY-1,*(bar[c]+i*6+j));
	   }
	}

}

//-----------------------------------------------------------------------------


void Int2Char(int n,char *s,char length)
{
if ((length>=3) & (n==1)) {
   strcpy(s,"One");
   return;
   }

ltoa(n,s,10);

if (strlen(s)<=length) return;

strcpy(s,s+strlen(s)-length);
}

char *Long2Str(long entier)
{
static char chaine[20];
char chaine2[20];
short i,j,n;

ltoa(entier,chaine2,10);
if ((n=strlen(chaine2))<9)
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

void AfficheTout(void)
{
PrintAt(0,0,"%-40s%40s","Ketchup Killers Commander","RedBug");
ColLin( 0,0,40,1*16+5);
ColLin(40,0,40,1*16+3);
ColLin(0,(Cfg->TailleY)-2,80,7);

CommandLine("##INIT 0 %d 80\n",(Cfg->TailleY)-2);

DFen->init=1;
DFen->Fen2->init=1;

ChangeLine();

MenuBar(3);

AffFen(DFen);
AffFen(DFen->Fen2);
}

//--------------
// ASCII Table -
//--------------
void ASCIItable(void)
{
int n,x,y;

SaveEcran();

for (n=0;n<256;n++) {
    x=(n/16)*5;
    y=(n%16)+6;

    PrintAt(x,y,"%2X   ",n);
    AffChr(x+3,y,n);
    }

WinCadre(0,5,79,22,0);
ColWin(1,6,78,21,10*16+5);

getch();

ChargeEcran();
}



// 1 -> Cancel
// 0 -> OK
int WinError(char *erreur)
{
int x,l;
static char Buffer[70];
static int CadreLength=71;

struct Tmt T[4] = {
      {15,4,2,NULL,NULL},
      {45,4,3,NULL,NULL},
      { 1,1,6,NULL,&CadreLength},
      { 2,2,0,Buffer,NULL}
      };

struct TmtWin F = {
    3,10,76,16,
    "Error"};

l=strlen(erreur);

x=(80-l)/2;     // 1-> 39, 2->39
if (x>25) x=25;

l=(40-x)*2;

CadreLength=l+1;

F.x1=x-2;
F.x2=x+l+1;

l=l+3;

T[0].x=(l/4)-5;
T[1].x=(3*l/4)-6;

strcpy(Buffer,erreur);

if (WinTraite(T,4,&F)==0)
    return 0;
    else
    return 1;

}

void YouMad(char *s)
{
int x,l;
static char Buffer[70];
static int CadreLength=71;

struct Tmt T[5] = {
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 2,2,0,"You are MAD!",NULL},
      { 1,1,4,NULL,&CadreLength},
      { 2,3,0,Buffer,NULL}
      };

struct TmtWin F = {
    3,10,76,17,
    "Error!"};

l=strlen(s);

x=(80-l)/2;     // 1-> 39, 2->39
if (x>25) x=25;

l=(40-x)*2;

CadreLength=l+1;

F.x1=x-2;
F.x2=x+l+1;

l=l+3;

T[0].x=(l/4)-5;
T[1].x=(3*l/4)-6;

strcpy(Buffer,s);

WinTraite(T,5,&F);

}
