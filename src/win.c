/*--------------------------------------------------------------------*\
|-             Procedure pour gestion des fenˆtres                    -|
\*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
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

void ClearSpace(char *name);    //--- efface les espaces inutiles ------

int NameIDF(char *name)
{
RB_IDF Info;

strcpy(Info.path,name);

Traitefic(&Info);

return Info.numero;
}



/*--------------------------------------------------------------------*\
|- Information with IDF                                               -|
\*--------------------------------------------------------------------*/

int InfoIDF(struct fenetre *Fen)
{
struct file *F;
RB_IDF Info;

F=Fen->F[Fen->pcur];

if ( (F->attrib & _A_SUBDIR)==_A_SUBDIR)
    {
    PrintAt(0,0,"%-40s%-40s","Directory",F->name);
    return 0;
    }

if (Fen->system!=0)
    {
    PrintAt(0,0,"%-40s%-40s","Internal File",F->name);
    return 0;
    }

strcpy(Info.path,DFen->path);
Path2Abs(Info.path,F->name);

strcpy(Info.format,"Invalid Name");
strcpy(Info.fullname,"");

Traitefic(&Info);

Info.fullname[40]=0;
Info.format[40]=0;

PrintAt(0,0,"%-40s%-40s",Info.format,Info.fullname);

return Info.numero;
}

/*--------------------------------------------------------------------*\
|-  Efface la fenˆtre gestion de fichier                              -|
\*--------------------------------------------------------------------*/

void ClearNor(struct fenetre *Fen)
{
char col;

int i;
int x,y;
int yl;

char a;

Fen->oldscur=0;
Fen->oldpcur=-1;

ColWin(Fen->x+1,Fen->y+1,Fen->x+Fen->xl-1,Fen->y+Fen->yl-1,7*16+6);

//---Couleur uniquement pour fentype=1,2 ou 3 (le 4 le fait 2 fois !)---

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

Fen->x2=x-1;
Fen->xl2=Fen->xl;
Fen->x3=x+2;
Fen->y3=y+Fen->yl-1;

switch (Cfg->fentype)
    {
    case 1:
        Cfg->Tfont=179;
        Fen->y2=y-1;
        Fen->yl2=Fen->yl-4;

        PrintAt(x,y,     "ÉÍÍÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÑÍÍÍÍÍ»");
        PrintAt(x,y+1,   "º    Name    ³   Size   ³  Date  ³Time º");
        for (i=2;i<yl-3;i++)
           PrintAt(x,y+i,"º                                      º");
        PrintAt(x,y+yl-3,"ÇÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄ¶");
        PrintAt(x,y+yl-2,"º                                      º");
        PrintAt(x,y+yl-1,"ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼");
        col=7*16+5;

        break;
    case 2:
        if (Cfg->UseFont==1)
            Cfg->Tfont=168;                // Barre Verticale | with 8x?

        Fen->y2=y-2;
        Fen->yl2=Fen->yl-3;

        ColLin(x,y,40,3*16+7);
        PrintAt(x,y,     "     Name    ³   Size   ³  Date  ³Time  ");
        for (i=1;i<yl-3;i++)
           PrintAt(x,y+i,"Û                                      Û");
        PrintAt(x,y+yl-3,"ÛßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßÛ");
        PrintAt(x,y+yl-2,"Û                                      Û");
        PrintAt(x,y+yl-1,"ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß");
        col=3*16+7;
        break;
    case 3:
        Cfg->Tfont=179;
        Fen->y2=y-2;
        Fen->yl2=Fen->yl-3;

        PrintAt(x,y,     "ÚÄ Ä Name Ä ÄÂ Ä Size Ä ÂÄ Date ÄÂTime ¿");
        for (i=1;i<yl-3;i++)
           PrintAt(x,y+i,"³                                      ³");
        PrintAt(x,y+yl-3,"³ÚÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄ¿³");
        PrintAt(x,y+yl-2,"³                                      ³");
        PrintAt(x,y+yl-1,"ÀÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙÙ");
        col=10*16+5;
        break;
    case 4:
        if (Cfg->UseFont==1)
            Cfg->Tfont=168;                // Barre Verticale | with 8x?
        a=Cfg->Tfont;

        Fen->y2=y-1;
        Fen->yl2=Fen->yl-4;

        ChrLin(x+1,y+yl-2,38,32);

        PrintAt(x+1,y+1,
                     "    Name    %c   Size   %c  Date  %cTime ",a,a,a);

        WinLine(x+1,y+yl-3,38,1);
        WinCadre(x,y,x+39,y+yl-1,1);
        col=7*16+5;
        break;
    }

ColLin(x+ 5,Fen->y2+2,4,col);
ColLin(x+17,Fen->y2+2,4,col);
ColLin(x+27,Fen->y2+2,4,col);
ColLin(x+34,Fen->y2+2,4,col);
}


void FenNor(struct fenetre *Fen)
{
int j,n;                                                     // Compteur
int date,time;
char ch1[13],ch2[11],ch3[9],ch4[6];
char temp[16],temp2[20];
char nom[13],ext[4];

char dispall;

char col;

short i;

char a;                                           // Separator Character

int x1,y1;


if (Fen->init==1)
    ClearNor(Fen);

a=Cfg->Tfont;

if (Fen->scur>Fen->pcur)
    Fen->scur=Fen->pcur;

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

x1=2+Fen->x2;
y1=3+Fen->y2;

n=(Fen->pcur)-(Fen->scur);                                    // premier

InfoSelect(Fen);

dispall=((Fen->oldpcur-Fen->oldscur)!=(Fen->pcur-Fen->scur));

for (i=0;(i<Fen->yl2) & (n<Fen->nbrfic);i++,n++,y1++)
    {
    if ( (n==Fen->oldpcur) | (n==Fen->pcur) | (dispall) )
        {
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
        nom[8]=0;                // Dans le cas o— on aurait un nom long


        Int2Char((date>>9)+80,temp,2);

        sprintf(ch1,"%-8s %-3s",nom,ext);
        sprintf(ch2,"%10s",Long2Str(Fen->F[n]->size,temp2));
        sprintf(ch3,"%02d/%02d/%2s",(date&31),(date>>5)&15,temp);
        sprintf(ch4,"%02d:%02d",(time>>11)&31,(time>>5)&63);

        if ((Fen->F[n]->attrib & _A_SUBDIR)==_A_SUBDIR)
            memcpy(ch2,"SUB--DIR",10);

        if ((Fen->F[n]->attrib & _A_VOLID)==_A_VOLID)
            memcpy(ch2,"##Vol-ID##",10);

        if (nom[1]==':')
            {
            memcpy(ch2,"##Reload##",10);
            strcpy(ch3,"        ");
            strcpy(ch4,"     ");
            }

        if (Fen->F[n]->select!=0)
            memcpy(ch4,"<SEL>",5);

        if ((Fen->F[n]->attrib & _A_HIDDEN)==_A_HIDDEN)
            ch1[8]=176;  // 176,177,178

        if (nom[0]=='.')
            {
            if (nom[1]!='.')
                memcpy(ch2," RELOAD ",10);
                else
            memcpy(ch2," UP-DIR ",10);
            }

/*--------------------------------------------------------------------*\
|------------------- Line Activity ------------------------------------|
\*--------------------------------------------------------------------*/
        if ((Fen->actif==1) & (n==(Fen->pcur)) )
            {
            if (Fen->F[n]->select==0)
                col=1*16+6;
                else
                col=1*16+5;
            }
            else
            {
            if (Fen->F[n]->select==0)
                {
                col=7*16+6;

                if (Cfg->dispcolor==1)
                    {
                    if (FoundExt(ext,Cfg->ExtExe))         // Executable
                        col=7*16+13;
                    else
                    if (FoundExt(ext,Cfg->ExtArc))            // Archive
                        col=7*16+8;
                    else
                    if (FoundExt(ext,Cfg->ExtSnd))                // Son
                        col=7*16+12;
                    else
                    if (FoundExt(ext,Cfg->ExtBmp))              // Image
                        col=7*16+11;
                    else
                    if (FoundExt(ext,Cfg->ExtTxt))              // Texte
                         col=7*16+4;
                    else
                    if (FoundExt(ext,Cfg->ExtUsr))       // User defined
                         col=13*16+11;

                    }
                }
                else
                col=7*16+5;
            }


// ---------------------------------------------------------------------

        ColLin(x1,y1,12,col);      PrintAt(x1,y1,"%s",ch1);
        AffCol(x1+12,y1,7*16+6);   AffChr(x1+12,y1,a);
        ColLin(x1+13,y1,10,col);   PrintAt(x1+13,y1,"%s",ch2);
        AffCol(x1+23,y1,7*16+6);   AffChr(x1+23,y1,a);
        ColLin(x1+24,y1,8,col);    PrintAt(x1+24,y1,"%s",ch3);
        AffCol(x1+32,y1,7*16+6);   AffChr(x1+32,y1,a);
        ColLin(x1+33,y1,5,col);    PrintAt(x1+33,y1,"%s",ch4);
        }
    }

/*--------------------------------------------------------------------*\
|------------------- Remplis le reste de la fenˆtre -------------------|
\*--------------------------------------------------------------------*/

if (dispall)
    for (;(i<Fen->yl2);i++,y1++)
        {
       PrintAt(x1,y1,"            %c          %c        %c     ",a,a,a);
        ColLin(x1,y1,38,7*16+6);
        }

Fen->oldscur=Fen->scur;
Fen->oldpcur=Fen->pcur;

if (Fen->FenTyp==3)
    FenInfo(Fen);       //  Affiche les infos sur les fichiers si type=3

Fen->init=0;
}


/*--------------------------------------------------------------------*\
|-            For the FILE_ID.DIZ                                     -|
\*--------------------------------------------------------------------*/

void Makediz(RB_IDF *Info,char *Buf)
{
struct dosdate_t Date;
char ligne[256];

_dos_getdate(&Date);

strcpy(Buf,"                                    \r\n");
sprintf(ligne,"      Ketchup Killers  (C) %4d     \r\n",Date.year);
strcat(Buf,ligne);
strcat(Buf,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\r\n");

if (*Info->fullname!=0)
    {
    ClearSpace(Info->fullname);
    Info->fullname[30]=0;

    if (strlen(Info->fullname)<=26)
        sprintf(ligne," Title: %27s \r\n",Info->fullname); //34
    else
        sprintf(ligne," T.:%31s \r\n",Info->fullname); //34
    strcat(Buf,ligne);
    }

if (*Info->format!=0)
    {
    sprintf(ligne," Type : %27s \r\n",Info->format); //34
    strcat(Buf,ligne);
    }

if (*Info->info!=0)
    {
    sprintf(ligne," %-14s : %17s \r\n",Info->Tinfo,Info->info); //34
    strcat(Buf,ligne);
    }

if (*Info->composer!=0)
    {
    sprintf(ligne," Composer: %24s \r\n",Info->composer); //34
    strcat(Buf,ligne);
    }

if (*Info->message[0]!=0)
    {
    short m;
    sprintf(ligne,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\r\n");
    strcat(Buf,ligne);
    for (m=0;m<9;m++)
        {
        if (*Info->message[m]!=0)
            {
            sprintf(ligne,"%-36s\r\n",Info->message[m]);
            strcat(Buf,ligne);
            }
        }
    }

if (Info->taille!=0)
    {
    sprintf(ligne," True Size: %23ld \r\n",Info->taille);
    strcat(Buf,ligne);
    }

strcat(Buf,"                                    \r\n");

return;
}

void FenDIZ(struct fenetre *Fen)
{
char *Buf;
RB_IDF Info;
struct fenetre *Fen2;
struct file *F;
int x,y,i;

Fen2=Fen->Fen2;
F=Fen2->F[Fen2->pcur];

if ( (F->attrib & _A_SUBDIR)==_A_SUBDIR) return;
if (Fen2->system!=0) return;

WinCadre(Fen->x,Fen->y,Fen->x+Fen->xl,Fen->y+Fen->yl,1);

strcpy(Info.path,Fen2->path);
Path2Abs(Info.path,F->name);

Traitefic(&Info);

Buf=malloc(4000);

Makediz(&Info,Buf);

Window(Fen->x+1,Fen->y+1,Fen->x+Fen->xl-1,Fen->y+4,170);


x=Fen->x+2;
y=Fen->y+6;

i=0;
while (Buf[i]!=0)
    {
    switch (Buf[i])
        {
        case 10: x=Fen->x+2; break;
        case 13: y++; break;
        default: AffChr(x,y,Buf[i]); x++; break;
        }
	i++;
	}

WinCadre(Fen->x+1,Fen->y+5,Fen->x+Fen->xl-1,y,2);
ColWin(Fen->x+2,Fen->y+6,Fen->x+Fen->xl-2,y-1,10*16+1);

Window(Fen->x+1,y+1,Fen->x+Fen->xl-1,Fen->y+Fen->yl-1,170);

free (Buf);

Fen->init=0;
}


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void ClearInfo(struct fenetre *Fen)
{
int i;
struct file *F;
RB_IDF Info;
struct fenetre *Fen2;

SaveEcran();

Fen->oldscur=0;
Fen->oldpcur=-1;

Fen2=Fen->Fen2;

WinCadre(19,9,61,11,0);
Window(20,10,60,10,10*16+4);


PrintAt(23,10,"Wait Please");

for (i=0;i<Fen->nbrfic;i++)
    {
    F=Fen->F[i];

    if (F->info==NULL)
        F->info=GetMem(41);

    if ( (F->attrib & _A_SUBDIR)==_A_SUBDIR)
        {
        sprintf(F->info,"%cDirectory",0);
        }
        else
        {
        strcpy(Info.path,Fen->path);
        Path2Abs(Info.path,F->name);

        Traitefic(&Info);
        strcpy(F->info+1,Info.format);
        F->info[0]=Info.Btype;
        }
    }

ChargeEcran();

WinCadre(Fen2->x,Fen2->y,Fen2->x+Fen2->xl,Fen2->y+Fen2->yl,1);

Window(Fen2->x+1,Fen2->y+1,Fen2->x+Fen2->xl-1,Fen2->y+Fen2->yl-1,118);
                                                               // 7*16+6

ChrLin(Fen2->x+1,Fen2->y+Fen2->yl-2,Fen2->xl-1,196);
PrintAt(Fen2->x+1,Fen2->y+Fen2->yl-1,"Use this at your own risk ;)");
}


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|-           Display about select                                     -|
\*--------------------------------------------------------------------*/


void InfoSelect(struct fenetre *Fen)
{
char temp[20];

if (Fen->nbrsel==0)
    {
    if (Fen->nbrfic==1)
        PrintAt(Fen->x3,Fen->y3,"%-10s bytes in one file         ",
            Long2Str(Fen->taillefic,temp));
    else
        PrintAt(Fen->x3,Fen->y3,"%-10s bytes in %3d files        ",
            Long2Str(Fen->taillefic,temp),Fen->nbrfic);
    }
else
    {
    if (Fen->nbrsel==1)
        PrintAt(Fen->x3,Fen->y3,"%-10s b. in one selected file   ",
            Long2Str(Fen->taillesel,temp));
    else
        PrintAt(Fen->x3,Fen->y3,"%-10s b. in %3d selected files  ",
            Long2Str(Fen->taillesel,temp),Fen->nbrsel);
    }
}


/*--------------------------------------------------------------------*\
|-  The KEY's menu                                                    -|
\*--------------------------------------------------------------------*/

void CtrlMenu(void)
{
char temp[20];

if (DFen->y3==0) return;
if (DFen->nbrfic==1)
    PrintAt(DFen->x3,DFen->y3,"%-10s bytes in one file         ",
        Long2Str(DFen->taillefic,temp));
else
    PrintAt(DFen->x3,DFen->y3,"%-10s bytes in %3d files        ",
        Long2Str(DFen->taillefic,temp),DFen->nbrfic);
}

void ShiftMenu(void)
{
char chaine[37];

if (DFen->y3==0) return;
memcpy(chaine,DFen->path,37);
chaine[36]=0;
PrintAt(DFen->x3,DFen->y3,"%-36s",chaine);
}


void AltMenu(void)
{
time_t clock;
static char buffer[80];

if (DFen->y3==0) return;

clock=time(NULL);
strftime(buffer,80,"%H:%M:%S",localtime(&clock));

PrintAt(DFen->x3,DFen->y3,"%-36s",buffer);
}

void MenuBar(char c)
{
static char bar[4][60]=
 {" Help  ----  View  Edit  Copy  Move  MDir Delete Menu  Quit ",  //NOR
  " ---- Attrib View  Edit  Host Rename ----  ----  ----  ---- ",//SHIFT
  "On-OffOn-Off Name  .Ext  Date  Size Unsort Spec  ----  ---- ", //CTRL
  " Drv1  Drv2  FDiz  ----  ----  ---- Search Type  Line  Disp "}; //ALT

char i,j,n;
static signed char d=-1;
int TY;

TY=Cfg->TailleY;

if (d==c) return;
d=c;


if (DFen->FenTyp==0)                                  // Fenetre Normale
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
        d=-1;
        break;
    }

n=0;
for (i=0;i<10;i++)
    {
	PrintAt(n,TY-1,"F%d",(i+1)%10);
    for(j=0;j<2;j++,n++)
        AffCol(n,TY-1,1*16+8);
    for(j=0;j<6;j++,n++)
        {
        AffCol(n,TY-1,1*16+2);
        AffChr(n,TY-1,*(bar[c]+i*6+j));
        }
	}

}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-  ASCII Table                                                       -|
\*--------------------------------------------------------------------*/

void ASCIItable(void)
{
int n,x,y;

SaveEcran();

for (n=0;n<256;n++)
    {
    x=(n/16)*5;
    y=(n%16)+6;

    PrintAt(x,y,"%2X   ",n);
    AffChr(x+3,y,n);
    }

WinCadre(0,5,79,22,0);
ColWin(1,6,78,21,10*16+5);

Wait(0,0,0);

ChargeEcran();
}


/*--------------------------------------------------------------------*\
|-  Erreur (l'utilisateur est fou !)                                  -|
\*--------------------------------------------------------------------*/

void YouMad(char *s)
{
int x,l;
static char Buffer[70];
static int CadreLength=71;

struct Tmt T[5] =
    { {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 2,2,0,"You are MAD!",NULL},
      { 1,1,4,NULL,&CadreLength},
      { 2,3,0,Buffer,NULL}  };

struct TmtWin F = { 3,10,76,17,"Error!"};

l=strlen(s);

x=(80-l)/2;                                             // 1-> 39, 2->39
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


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-  Change la taille de l'ecran                                       -|
\*--------------------------------------------------------------------*/

void ChangeTaille(int i)
{
if (i==0)
    switch(Cfg->TailleY)
        {
        case 25:  Cfg->TailleY=30;  break;
        case 30:  Cfg->TailleY=50;  break;
        default:  Cfg->TailleY=25;  break;
        }
    else
    Cfg->TailleY=i;

TXTMode(Cfg->TailleY);
NoFlash();

switch (Cfg->TailleY)
    {
    case 50:
        Font8x(8);
        break;
    case 25:
    case 30:
        Font8x(16);
        break;
   }

AfficheTout();

LoadPal();
}


/*--------------------------------------------------------------------*\
|-                              Setup                                 -|
\*--------------------------------------------------------------------*/

void Setup(void)
{
static int l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14;

static char x1=32,x2=32,x3=32;
static int y1=8,y2=3,y3=15;

struct Tmt T[23] = {
      {6,17,2,NULL,NULL},                                       // le OK
      {21,17,3,NULL,NULL},                                  // le CANCEL

      {5,3,7, "Size Trash   ",&l1},
      {5,4,7, "Ansi Speed   ",&l2},
      {5,5,7, "Screen Saver ",&l7},

      {5,8,8, "Debug",&l3},
      {5,9,8, "Point SubDir",&l4},
      {5,10,8,"LogFile",&l5},
      {5,11,8,"Use Font",&l6},
      {5,12,8,"Display Hidden File",&l8},
      {5,13,8,"Auto Reload Directory",&l9},
      {5,14,8,"Verify History Directory",&l10},
      {5,15,8,"Quick Palette",&l11},

      {39,3,8,"Highlight groups",&l12},
      {39,4,8,"Insert move down",&l13},
      {39,5,8,"Select directories",&l14},

      {3,7,9,&x1,&y1},
      {3,2,9,&x2,&y2},
      {37,2,9,&x3,&y3},

      {40, 7,5," Serial Port ",NULL},        // la gestion du port serie
      {55, 7,5," Mask Setup  ",NULL},          // la gestion des masques
      {40, 9,5," File Setup  ",NULL},          // la gestion des masques
      {55, 9,5," Ext. Setup  ",NULL}        // la gestion des extensions
      };

struct TmtWin F = {3,3,76,22,"Setup"};

int n;

l1=Cfg->mtrash;
l2=Cfg->AnsiSpeed;
l3=Cfg->debug;
l4=Cfg->pntrep;
l5=Cfg->logfile;
l6=Cfg->font;
l7=Cfg->SaveSpeed;
l8=Cfg->hidfil;
l9=Cfg->autoreload;
l10=Cfg->verifhist;
l11=Cfg->palafter;
l12=Cfg->dispcolor;
l13=Cfg->insdown;
l14=Cfg->seldir;
// l15=Cfg->currentdir;

do
{
n=WinTraite(T,23,&F);

if (n==27) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

if (T[n].type==5)
    {
    if (n==19)  SerialSetup();
    if (n==20)  MasqueSetup();
    if (n==21)  FileSetup();
    if (n==22)  ExtSetup();
    }
}
while(T[n].type==5);


Cfg->mtrash=l1;
Cfg->AnsiSpeed=l2;
Cfg->debug=l3;
Cfg->pntrep=l4;
Cfg->logfile=l5;
Cfg->font=l6;
Cfg->SaveSpeed=l7;
Cfg->hidfil=l8;
Cfg->autoreload=l9;
Cfg->verifhist=l10;
Cfg->palafter=l11;
Cfg->dispcolor=l12;
Cfg->insdown=l13;
Cfg->seldir=l14;
// Cfg->currentdir=l15;

SaveCfg();

DesinitScreen();
while(!InitScreen(Cfg->display));
    {
    Cfg->display++;
    if (Cfg->display>16) Cfg->display=0;
    }

DFen=DFen->Fen2;
CommandLine("#cd .");
DFen=DFen->Fen2;
CommandLine("#cd .");

LoadCfg();

UseCfg();

}

void FileSetup(void)
{
static char Edit[64],View[64];
static int DirLength=63;


struct Tmt T[6] = {
      { 8,3,1,Edit,&DirLength},
      { 8,5,1,View,&DirLength},
      { 1,3,0, "Editor:",NULL},
      { 1,5,0, "Viewer:",NULL},

      {3,10,2,NULL,NULL},                                       // le OK
      {18,10,3,NULL,NULL}                                   // le CANCEL
      };

struct TmtWin F = {3,5,76,18,"File Setup"};

int n;

strcpy(Edit,Cfg->editeur);
strcpy(View,Cfg->vieweur);

n=WinTraite(T,6,&F);

if (n==27) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

strcpy(Cfg->editeur,Edit);
strcpy(Cfg->vieweur,View);
}

void ExtSetup(void)
{
static char Txt[64],Bmp[64],Snd[64],Arc[64],Exe[64],Usr[64];
static int DirLength=63;


struct Tmt T[14] = {
      { 9, 1,1,Txt,&DirLength},
      { 9, 3,1,Bmp,&DirLength},
      { 9, 5,1,Snd,&DirLength},
      { 9, 7,1,Arc,&DirLength},
      { 9, 9,1,Exe,&DirLength},
      { 9,11,1,Usr,&DirLength},
      { 1, 1,0, "Text:",NULL},
      { 1, 3,0, "Bitmap:",NULL},
      { 1, 5,0, "Sound :",NULL},
      { 1, 7,0, "Arc.:",NULL},
      { 1, 9,0, "Exec:",NULL},
      { 1,11,0, "User:",NULL},


      {13,13,2,NULL,NULL},                                      // le OK
      {50,13,3,NULL,NULL}                                   // le CANCEL
      };

struct TmtWin F = {3,5,76,20,"Extension Setup"};

int n;

strcpy(Txt,Cfg->ExtTxt);
strcpy(Bmp,Cfg->ExtBmp);
strcpy(Snd,Cfg->ExtSnd);
strcpy(Arc,Cfg->ExtArc);
strcpy(Exe,Cfg->ExtExe);
strcpy(Usr,Cfg->ExtUsr);

n=WinTraite(T,14,&F);

if (n==27) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

strcpy(Cfg->ExtTxt,Txt);
strcpy(Cfg->ExtBmp,Bmp);
strcpy(Cfg->ExtSnd,Snd);
strcpy(Cfg->ExtArc,Arc);
strcpy(Cfg->ExtExe,Exe);
strcpy(Cfg->ExtUsr,Usr);
}


void SerialSetup(void)
{
static char Dir[3];
static int DirLength=2;

static int l1,l2,l3,l5;

struct Tmt T[8] = {
      { 5,3,7, "Port COM ",&l1},
      { 5,4,7, "Speed    ",&l2},
      { 5,5,7, "Data_bit ",&l3},
      { 5,6,0, "Parity   ",NULL},
      {16,6,1,Dir,&DirLength},
      { 5,7,7, "Stop_bit ",&l5},

      {3,10,2,NULL,NULL},                                       // le OK
      {18,10,3,NULL,NULL}                                   // le CANCEL
      };

struct TmtWin F = {23,5,56,18,"Serial Setup"};

int n;

l1=Cfg->comport;
l2=Cfg->comspeed;
l3=Cfg->combit;
sprintf(Dir,"%c",Cfg->comparity);
l5=Cfg->comstop;

n=WinTraite(T,8,&F);

if (n==27) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

Cfg->comport=l1;
Cfg->comspeed=l2;
Cfg->combit=l3;
sscanf(Dir,"%c",&(Cfg->comparity));
Cfg->comstop=l5;
}

void MasqueSetup(void)
{
static char buffer[8][80];

static int DirLength=78;

static int l1,l2,l3,l4;

struct Tmt T[18] = {
      {2,14,2,NULL,NULL},                                       // le OK
      {16,14,3,NULL,NULL},                                  // le CANCEL

      { 1, 1,0, "User config 1",NULL},
      {40, 1,8, "Ignore Case",&l1},
      { 1, 2,1,buffer[0],&DirLength},
      { 1, 3,1,buffer[1],&DirLength},

      { 1, 4,0, "User config 2",NULL},
      {40, 4,8, "Ignore Case",&l2},
      { 1, 5,1,buffer[2],&DirLength},
      { 1, 6,1,buffer[3],&DirLength},

      { 1, 7,0, "User config 3",NULL},
      {40, 7,8, "Ignore Case",&l3},
      { 1, 8,1,buffer[4],&DirLength},
      { 1, 9,1,buffer[5],&DirLength},

      { 1,10,0, "User config 4",NULL},
      {40,10,8, "Ignore Case",&l4},
      { 1,11,1,buffer[6],&DirLength},
      { 1,12,1,buffer[7],&DirLength}
      };

struct TmtWin F = {0,5,79,22,"Serial Setup"};

int n;

l1=Mask[11]->Ignore_Case;
l2=Mask[12]->Ignore_Case;
l3=Mask[13]->Ignore_Case;
l4=Mask[14]->Ignore_Case;

SplitMasque(Mask[11]->chaine,buffer[0],buffer[1]);
SplitMasque(Mask[12]->chaine,buffer[2],buffer[3]);
SplitMasque(Mask[13]->chaine,buffer[4],buffer[5]);
SplitMasque(Mask[14]->chaine,buffer[6],buffer[7]);

n=WinTraite(T,18,&F);

if (n==27) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

Mask[11]->Ignore_Case=l1;
Mask[12]->Ignore_Case=l2;
Mask[13]->Ignore_Case=l3;
Mask[14]->Ignore_Case=l4;

JointMasque(Mask[11]->chaine,buffer[0],buffer[1]);
if (strlen(Mask[11]->chaine)>2)
    strcpy(Mask[11]->title,"User config 1");
    else
    strcpy(Mask[11]->title,"");
JointMasque(Mask[12]->chaine,buffer[2],buffer[3]);
if (strlen(Mask[12]->chaine)>2)
    strcpy(Mask[12]->title,"User config 2");
    else
    strcpy(Mask[12]->title,"");
JointMasque(Mask[13]->chaine,buffer[4],buffer[5]);
if (strlen(Mask[13]->chaine)>2)
    strcpy(Mask[13]->title,"User config 3");
    else
    strcpy(Mask[13]->title,"");
JointMasque(Mask[14]->chaine,buffer[6],buffer[7]);
if (strlen(Mask[14]->chaine)>2)
    strcpy(Mask[14]->title,"User config 4");
    else
    strcpy(Mask[14]->title,"");

}

void SplitMasque(char *chaine,char *buf1,char *buf2)
{
int n,m;

m=strlen(chaine);
if (m<3)
    {
    strcpy(chaine," @");
    strcpy(buf1,"");
    strcpy(buf2,"");
    return;
    }

if (m<=80)
    {
    memcpy(buf1,chaine,m-2);
    buf1[m-2]=0;
    strcpy(buf2,"");
    return;
    }
n=78;
while ( (n!=0) & (chaine[n]!=32) )
    n--;

memcpy(buf1,chaine,n);
buf1[n]=0;
memcpy(buf2,chaine+n+1,m-n-3);
buf2[m-n-3]=0;
}

void JointMasque(char *chaine,char *buf1,char *buf2)
{
int n,m;

strcpy(chaine,buf1);
strcat(chaine," ");
strcat(chaine,buf2);
strcat(chaine," @");

n=0;

while(chaine[n]!=0)
    {
    if ( (chaine[n]==32) & ((chaine[n+1]==32) | (n==0) |
                                                     (chaine[n+1]==0)) )
        {
        for (m=n+1;m<strlen(chaine)+1;m++)
            chaine[m-1]=chaine[m];
        }
        else
        n++;
    }
}


/*--------------------------------------------------------------------*\
|-    Utilise les donnees dans la structure configuration avec les    -|
|-    verifications necessaires                                       -|
\*--------------------------------------------------------------------*/

void UseCfg(void)
{
ChangeTaille(Cfg->TailleY);                        // Rafraichit l'ecran

ChangeLine();

if (Cfg->speedkey==1)
    {
    union REGS R;

    R.w.ax=0x0900;
    int386(0x16,&R,&R);

    if ((R.h.ah & 4)==4)
        {
        R.w.ax=0x0305;
        R.w.bx=0x0000;
        int386(0x16,&R,&R);                         // Keystroke maximum
        }
        else
        {
        Cfg->speedkey=0;
        }
    }
}


/*--------------------------------------------------------------------*\
|-  Information on disk                                               -|
\*--------------------------------------------------------------------*/

void FenDisk(struct fenetre *Fen)
{
char *Buf,temp[32],disk[4],volume[32];
char InfoVol[32];

struct fenetre *Fen2;
struct file *F;
int n,m,x,y;
//struct find_t ff;
int error;

static char chaine[80];
short WindowsActif,HVer,NVer;
union REGS R;

static long oldfree;
static char oldpath[256];

struct diskfree_t d;
long tfree,ttotal;

char drive;

drive=toupper(DFen->path[0])-'A';

Fen2=Fen->Fen2;
F=Fen2->F[Fen2->pcur];

if (Fen->init==1)
    {
    oldfree=0;
    oldpath[0]=0;
    }

_dos_getdiskfree(toupper(Fen2->path[0])-'A'+1,&d);
tfree=(d.avail_clusters)*(d.sectors_per_cluster);
tfree=tfree*(d.bytes_per_sector);

if ( (tfree!=oldfree) | (strcmp(oldpath,Fen2->path)!=0) )
    Fen->init=1;

if (Fen->init==0)
    {
    PrintAt(Fen->x+38,Fen->y+Fen->yl-1,"?");
    return;
    }

x=Fen->x+1;
y=Fen->y+1;


Buf=Fen2->path;
if (strlen(Buf)>37) Buf+=(strlen(Buf)-37);

ttotal=(d.total_clusters)*(d.sectors_per_cluster);
ttotal=ttotal*(d.bytes_per_sector)/1024;

R.w.ax=0x0900;
int386(0x16,&R,&R);


WindowsActif = windows( &HVer, &NVer );

switch ( WindowsActif )
    {
    case 0:
        sprintf(chaine,"Windows not present");
        break;
    case 0x81:
        sprintf(chaine,"Windows in Real Mode");
        break;
    case 0x82:
        sprintf(chaine,"Windows in Standard Mode");
        break;
    case 0x01:
        sprintf(chaine,"Windows/386 V 2.x");
        break;
    case 0x83:
        sprintf(chaine,"Windows V %d.%d Exended Mode",HVer,NVer);
        break;
    }

memcpy(disk,Fen2->path,3);
disk[3]=0;
drive=toupper(disk[0])-'A';

strcpy(volume,disk);
strcat(volume,"*.*");

/*
error=_dos_findfirst(volume,_A_VOLID,&ff);

if (error!=0)
*/
    strcpy(volume,"Unknow");
//    else
//    strcpy(volume,ff.name);

n=0;
while (volume[n]!=0)
    {
    if (volume[n]=='.')
        for (m=n;m<strlen(volume);m++)
            volume[m]=volume[m+1];
        else
        n++;
    }

InfoVol[0]=0;

R.w.ax=0x5601;
R.w.dx=0xFFFF;
R.h.bh=drive;
R.h.bl=0;
int386(0x2F,&R,&R);

if (R.h.al==0xFF)
    strcpy(InfoVol,"Redirected drive (with interlnk)");

// PrintAt(0,0,"%04X %04X %04X %04X",R.w.ax,R.w.bx,R.w.cx,R.w.dx);
// pour C renvoit 5601 0200 0168 FFFF
// pour D renvoit 5601 0300 0168 FFFF
// pour E renvoit 56FF 0401 0001 0630

R.w.ax=0x150B;
R.w.cx=drive;
int386(0x2F,&R,&R);

if ( (R.w.bx==0xADAD) & (R.w.ax!=0) )
    strcpy(InfoVol,"CD-ROM drive");

// PrintAt(0,0,"%04X %04X %04X %04X",R.w.ax,R.w.bx,R.w.cx,R.w.dx);
// pour C renvoit 0000 ADAD 0002 FFFF
// pour D renvoit 0000 ADAD 0003 FFFF
// pour H renvoit 5AD4 ADAD 0007 FFFF

WinCadre(Fen->x,Fen->y,Fen->x+Fen->xl,Fen->y+Fen->yl,1);
Window(Fen->x+1,Fen->y+1,Fen->x+Fen->xl-1,Fen->y+Fen->yl-1,10*16+1);

PrintAt(x+1,y,"%s",RBTitle);
ColLin(x,y,38,10*16+5);
ChrLin(x,y+1,38,196);

PrintAt(x+1,y+2,"Current directory");
ColLin(x+1,y+2,37,10*16+3);
PrintAt(x+1,y+3,"%s",Buf);
ChrLin(x,y+4,38,196);

PrintAt(x+1,y+5,"Current disk");
ColLin(x+1,y+5,37,10*16+3);

PrintAt(x+1,y+6,"%s [%s] %s",disk,volume,InfoVol);

PrintAt(x+1,y+7,"Free space: %12s bytes",Long2Str(tfree,temp));
PrintAt(x+1,y+8,"Capacity:   %8s kilobytes",Long2Str(ttotal,temp));

ChrLin(x,y+9,38,196);
PrintAt(x+1,y+10,"System Information");
ColLin(x+1,y+10,37,10*16+3);

PrintAt(x+1,y+11,"þ %s",chaine);            // Information about windows
PrintAt(x+1,y+12,"þ Initialisation: %d clocks ",Info->temps);

PrintAt(Fen->x+38,Fen->y+Fen->yl-1,"");

oldfree=tfree;
strcpy(oldpath,Fen2->path);

Fen->init=0;

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
|- S E C R E T                                                P A R T -|
\*--------------------------------------------------------------------*/



/*--------------------------------------------------------------------*\
|-                     Test la vitesse d'affichage                    -|
\*--------------------------------------------------------------------*/

void SpeedTest(void)
{
static char vit1[64],vit2[64];

int x,y;
clock_t Cl;
long Cl1,Cl2;
char c;
int n,m;
double fvit1,fvit2;

struct Tmt T[6] =  {
      {6,17,2,NULL,NULL},                                       // le OK
      {21,17,3,NULL,NULL},                                  // le CANCEL

      {5,3,0,"No-Random Char",NULL},
      {5,4,0,"Random Char",NULL},
      {25,3,0,vit1,NULL},
      {25,4,0,vit2,NULL}
      };

struct TmtWin F = {3,3,76,22,"Speed Test"};

SaveEcran();

c=(rand()*80)/RAND_MAX+32;
m=0;
n=0;

Cl=clock();
while(clock()==Cl);                           //  met l'horloge au debut

while((clock()-Cl)<270)
    {
    x=(rand()*80)/RAND_MAX;
    y=(rand()*Cfg->TailleY)/RAND_MAX;

    AffChr(x,y,c);
    if (m>10000) m=0,c=(rand()*80)/RAND_MAX+32;
    n++;
    m++;
    }
Cl1=n;

c=(rand()*80)/RAND_MAX+32;
n=0;

Cl=clock();
while(clock()==Cl);                           //  met l'horloge au debut

while((clock()-Cl)<270)
    {
    x=(rand()*80)/RAND_MAX;
    y=(rand()*Cfg->TailleY)/RAND_MAX;

    AffChr(x,y,c);
    c++;
    n++;
    }
Cl2=n;

ChargeEcran();

SaveEcran();
         
fvit1=(Cl1*CLOCKS_PER_SEC)/270;
fvit2=(Cl2*CLOCKS_PER_SEC)/270;

sprintf(vit1,"%f characters/seconds",fvit1);
sprintf(vit2,"%f characters/seconds",fvit2);

n=WinTraite(T,6,&F);

ChargeEcran();
}


/*--------------------------------------------------------------------*\
|-  Fenˆtre avec les infos sur fichiers d'apres header                -|
\*--------------------------------------------------------------------*/

void FenInfo(struct fenetre *Fen)
{
int n;                                                       // Compteur
struct fenetre *Fen2;

short i;

char a;                                           // Separator Character

int x1,y1;
int x=2,y=3;

Fen2=Fen->Fen2;

if (Fen->init==1)
    ClearInfo(Fen);

a=Cfg->Tfont;

x1=Fen2->x2;
y1=Fen2->y2;

n=(Fen->pcur)-(Fen->scur);                                    // premier

for (i=0;(i<Fen2->yl2) & (n<Fen->nbrfic);i++,n++,y++)
    {
// ------------------ Line Activity ------------------------------------
    if (n==(Fen->pcur))
        ColLin(x+x1,y+y1,38,1*16+5);
        else
        {
        if (Fen->F[n]->info!=NULL)
            switch(Fen->F[n]->info[0])
                {
                case 1:  ColLin(x+x1,y+y1,38,4*16+11);   break;
                case 2:  ColLin(x+x1,y+y1,38,4*16+1);    break;
                case 3:  ColLin(x+x1,y+y1,38,4*16+13);   break;
                case 4:  ColLin(x+x1,y+y1,38,3*16+11);   break;
                case 5:  ColLin(x+x1,y+y1,38,3*16+1);    break;
                case 6:  ColLin(x+x1,y+y1,38,3*16+13);   break;
                default: ColLin(x+x1,y+y1,38,7*16+5);    break;
                }
            else
                ColLin(x+x1,y+y1,38,7*16+5);
        }

    if (Fen->F[n]->info!=NULL)
        PrintAt(x+x1,y+y1,"%-38s",Fen->F[n]->info+1);
    }
}


/*
void PacNoe(void)
{
char pac [] = {
    ' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x10,' ',0x11,' ',0x11,
    ' ',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x10,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    '°',0x10,'°',0x10,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    '°',0x10,'°',0x10,'°',0x11,'°',0x11,' ',0x11,' ',0x11,' ',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x10,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,'°',0x10,'°',0x10,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x10,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x10,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x10,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x10,
    '°',0x11,'°',0x11,'°',0x11,' ',0x11,' ',0x11,' ',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,'°',0x10,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,' ',0x11,' ',0x11,' ',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,'°',0x10,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,
    ' ',0x07,'°',0x10,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    ' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,' ',0x11,' ',0x11,' ',0x11,'°',0x11,'°',0x11,'°',0x7F,
    '²',0x7F,'°',0x11,' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x7F,'²',0x7F,'°',0x11,' ',0x07,' ',0x07,' ',0x07,
    '°',0x10,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x7F,'²',0x7F,'°',0x11,' ',0x07,
    ' ',0x07,' ',0x07,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    ' ',0x11,' ',0x11,' ',0x11,'°',0x11,'°',0x11,'²',0x7F,'°',0x00,
    '°',0x11,' ',0x07,' ',0x07,' ',0x07,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '²',0x7F,'°',0x00,'°',0x11,' ',0x07,' ',0x07,' ',0x07,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'²',0x7F,'°',0x00,'°',0x11,' ',0x07,' ',0x07,
    ' ',0x07,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,' ',0x11,
    ' ',0x11,' ',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    ' ',0x07,' ',0x07,' ',0x07,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,' ',0x11,' ',0x11,
    ' ',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,' ',0x07,
    ' ',0x07,' ',0x07,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,' ',0x07,' ',0x07,' ',0x07,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,'°',0x10,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'²',0x7F,'°',0x78,
    ' ',0x07,'°',0x11,'°',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,
    ' ',0x07,'°',0x10,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'²',0x7F,
    '°',0x78,'°',0x11,'°',0x00,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    ' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x11,'°',0x11,'°',0x11,
    '²',0x7F,'°',0x78,'°',0x11,'°',0x11,'°',0x00,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x78,'²',0x7F,'°',0x11,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x78,'²',0x7F,
    '°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,'°',0x11,'°',0x11,'°',0x11,'°',0x78,
    '²',0x7F,'°',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    ' ',0x07,'°',0x04,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    '°',0x10,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x04,'°',0x40,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,'°',0x10,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,' ',0x07,'°',0x04,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    '°',0x10,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,'°',0x10,'°',0x11,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x10,'°',0x10,'°',0x10,
    '°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x10,'°',0x10,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x10,
    '°',0x10,'°',0x10,'°',0x11,'°',0x11,'°',0x11,'°',0x11,'°',0x10,
    '°',0x10,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,'°',0x10,'°',0x10,'°',0x10,'°',0x11,'°',0x11,'°',0x11,
    '°',0x11,'°',0x10,'°',0x10,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,'²',0x7F,'°',0x78,'°',0x10,'°',0x10,
    '°',0x10,'°',0x10,'°',0x10,'°',0x70,'±',0x0F,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    '°',0x10,'°',0x10,'°',0x10,'°',0x10,'°',0x10,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    '±',0x0F,'°',0x70,'°',0x10,'°',0x10,'°',0x10,'°',0x10,'°',0x10,
    '°',0x78,'²',0x7F,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,'°',0x78,'²',0x7F,'°',0x00,'°',0x00,'°',0x00,
    '°',0x00,'°',0x00,'±',0x0F,'°',0x70,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x7F,
    '²',0x7F,'²',0x7F,'±',0x0F,'±',0x0F,'°',0x70,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,'°',0x70,
    '±',0x0F,'°',0x00,'°',0x00,'°',0x00,'°',0x00,'°',0x00,'²',0x7F,
    '°',0x78,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07,
    ' ',0x07,' ',0x07,' ',0x07,' ',0x07,' ',0x07};

int x,y,z,n,deb,fin,nl;
int chr,col,h,l;

char buffer[80*16*2];

for (x=0;x<80;x++)
    for(y=0;y<16;y++)
        {
        buffer[x*2+y*160]=GetChr(x,y);
        buffer[x*2+y*160+1]=GetCol(x,y);
        }

n=0;
for (z=-16;z<80;z++,n++)
    {
    if (n==3) n=0;
    deb=0;
    fin=16;

    if (z>80-16) fin=80-z;
    if (z<0) deb=-z;

    for(x=deb;x<fin;x++)
        for(y=0;y<16;y++)
            {
            nl=x*2+y*96;      // 15*3*2

            nl+=n*32;

            chr=pac[nl];
            col=pac[nl+1];

            h=col/16;
            l=col&15;

            switch(h)
                {
                case 0:  col=0      ; break;
                case 1:  col=13*16  ; break;
                case 4:  col=11*16  ; break;
                case 7:  col=7*16   ; break;
                }
            switch(l)
                {
                case 0:  col+=0    ; break;
                case 1:  col+=13  ; break;
                case 4:  col+=11  ; break;
                case 7:  col+=7   ; break;
                }

            if ((chr==32) & (col==7)) col=0;

            if (col!=0)
                {
                AffCol(x+z,y,col);
                AffChr(x+z,y,chr);
                }
                else
                {
                AffChr(x+z,y,buffer[(x+z)*2+y*160]);
                AffCol(x+z,y,buffer[(x+z)*2+y*160+1]);
                }
            }

    while ((inp(0x3DA) & 8)==8);
    while ((inp(0x3DA) & 8)!=8);
    while ((inp(0x3DA) & 1)==1);
    while ((inp(0x3DA) & 1)!=1);

    while ((inp(0x3DA) & 8)==8);
    while ((inp(0x3DA) & 8)!=8);
    while ((inp(0x3DA) & 1)==1);
    while ((inp(0x3DA) & 1)!=1);

    if (z>=0)
        for (y=0;y<16;y++)
            {
            AffChr(z,y,buffer[z*2+y*160]);
            AffCol(z,y,buffer[z*2+y*160+1]);
            }
    }
}
*/
