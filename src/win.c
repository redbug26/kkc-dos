/*--------------------------------------------------------------------*\
|-             Procedure pour gestion des fenˆtres                    -|
\*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>


#include <dos.h>

#include "kk.h"

#include "idf.h"

static char winbuffer[256];      //--- Buffer interne ------------------

extern FENETRE *Fenetre[4];     // uniquement pour trouver la 3‚me trash

void AffUpperPath(FENETRE *Fen,int y);

void InfoSelect(FENETRE *Fen);
void ExtSetup(void);
void SerialSetup(void);
void MasqueSetup(void);
void SplitMasque(char *chaine,char *buf1,char *buf2);
void JointMasque(char *chaine,char *buf1,char *buf2);

void CtrlMenu(void);
void ShiftMenu(void);
void AltMenu(void);

static char _intbuffer[256];

/*--------------------------------------------------------------------*\
|- Fonction cach‚e IDF                                                -|
\*--------------------------------------------------------------------*/

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

int InfoIDF(FENETRE *Fen)
{
static char buffer[256];
struct file *F;
RB_IDF Info;

F=Fen->F[Fen->pcur];

if (KKCfg->_Win95==1)
    InfoLongFile(Fen,buffer);
    else
    strcpy(buffer,F->name);

if (F->name[0]=='*')
    {
    PrintAt(0,0,"%-40s%-*s","Internal function",(Cfg->TailleX)-40,"");
    return -1;
    }

if ( (F->attrib & _A_SUBDIR)==_A_SUBDIR)
    {
    PrintAt(0,0,"%-40s%-*s","Directory",(Cfg->TailleX)-40,buffer);
    return -1;
    }

if (Fen->system!=0)
    {
    PrintAt(0,0,"%-40s%-*s","Internal File",(Cfg->TailleX)-40,buffer);
    return -1;
    }

strcpy(Info.path,DFen->path);
Path2Abs(Info.path,F->name);

strcpy(Info.format,"Invalid Name");
strcpy(Info.fullname,"");

Traitefic(&Info);

Info.fullname[(Cfg->TailleX)-40]=0;
Info.format[40]=0;

if (!strcmp(Info.fullname,F->name))
    strcpy(Info.fullname,buffer);

PrintAt(0,0,"%-40s%-*s",Info.format,(Cfg->TailleX)-40,Info.fullname);

return Info.numero;
}


/*--------------------------------------------------------------------*\
|- Affiche la ligne de commande en haut de la fenetre                 -|
\*--------------------------------------------------------------------*/
void AffUpperPath(FENETRE *Fen,int y)
{
int x;
char buffer[40];

if (Fen->FenTyp!=0) return;

if (strlen(Fen->path)>36)
    {
    memcpy(buffer,Fen->path,3);
    memcpy(buffer+3,"...",3);
    memcpy(buffer+6,Fen->path+strlen(Fen->path)-30,30);
    buffer[36]=0;
    }
    else
    {
    strcpy(buffer,Fen->path);
    }

x=Fen->x+1+(36-strlen(buffer))/2;

if (y==Fen->y)
    {
    if (KKCfg->FenAct==Fen->nfen)
        ColLin(x,y,strlen(buffer)+2,Cfg->col[1]);
        else
        ColLin(x,y,strlen(buffer)+2,Cfg->col[0]);
    PrintAt(x,y," %s ",buffer);
    }
    else
    {
    ChrLin(Fen->x+2,y,x-Fen->x-2,32);
    PrintAt(x,y," %s ",buffer);
    ChrLin(x+strlen(buffer)+2,y,Fen->x+36-x-strlen(buffer),32);
    }
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
    int n;
    sprintf(ligne," %s :",Info->Tinfo);
    n=strlen(ligne);
    sprintf(ligne+n,"%*s ",35-n,Info->info); //34
    ligne[36]=0;
    strcat(ligne,"\r\n");
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


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void MenuBar(char c)
{
static char bar[][61]=
 {" Help  User  View  Edit  Copy  Move  MDir Delete Menu  Quit ",  //NOR
  " ---- Attrib View  Edit  Host Rename ----  ----   Row  ---- ",//SHIFT
  "On-OffOn-Off Name  .Ext  Date  Size Unsort Spec  ----  ---- ", //CTRL
  " Drv1  Drv2  FDiz FileID Path  Hist Search Type  Line  Disp "}; //ALT
char Tbar[60];
int x;

static signed char d=-1;

if ((Cfg->TailleX!=80) & ( (DFen->FenTyp!=2) | (DFen->Fen2->FenTyp!=2)))
    {
    time_t clock;
    static char buffer[10];

    clock=time(NULL);
    strftime(buffer,9,"%H:%M:%S",localtime(&clock));

    PrintAt(41,Fenetre[0]->yl-1,"%-8s",buffer);
    }

if (d==c) return;
d=c;

if (DFen->FenTyp==0)                                  // Fenetre Normale
switch(c) {
    case 0:
        InfoSelect(DFen);
        InfoSelect(DFen->Fen2);
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
    default:
        c=4;
        break;
    }

if ((c!=4) & (KKCfg->isbar))
    {
    memcpy(Tbar,bar[c],60);

    if (c==2)
        {
        x=(DFen->order)&15;
        if (x==0)
            Tbar[41]=0x0F;
            else
            Tbar[11+6*x]=0x0F;

        if ((DFen->order&16)==16)
            Tbar[47]=0x0F;
        }
    Bar(Tbar);
    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-  ASCII Table                                                       -|
\*--------------------------------------------------------------------*/

void ASCIItable(void)
{
int n,x,y,m;

SaveScreen();

m=((Cfg->TailleX)-80)/2;

for (n=0;n<256;n++)
    {
    x=(n/16)*5+m;
    y=(n%16)+6;

    PrintAt(x,y,"%2X   ",n);
    AffChr(x+3,y,n);
    }

Cadre(m,5,m+79,22,0,Cfg->col[55],Cfg->col[56]);
ColWin(m+1,6,m+78,21,Cfg->col[16]);

Wait(0,0);

LoadScreen();
}


/*--------------------------------------------------------------------*\
|-  Erreur (l'utilisateur est fou !)                                  -|
\*--------------------------------------------------------------------*/

void YouMad(char *s)
{
int x,l;
static char Buffer[70];
static char CadreLength=70;

struct Tmt T[5] =
    { {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 2,2,0,"You are MAD!",NULL},
      { 1,1,4,&CadreLength,NULL},
      { 2,3,0,Buffer,NULL}  };

struct TmtWin F = {-1,10,74,17,"Error!"};

l=strlen(s);

x=((Cfg->TailleX)-l)/2;                                 // 1-> 39, 2->39
if (x>25) x=25;

l=(Cfg->TailleX)-2*x;

CadreLength=l;

F.x1=x-2;
F.x2=x+l+1;

l=l+3;

T[0].x=(l/4)-5;
T[1].x=(3*l/4)-6;

strcpy(Buffer,s);

WinTraite(T,5,&F,0);
}


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/




/*--------------------------------------------------------------------*\
|-                              Setup                                 -|
\*--------------------------------------------------------------------*/

void Setup(void)
{
static int l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,l15,l16,l17;
static int l18,l19;

static char x1=32,x2=32,x3=32;
static int y1=9,y2=3,y3=16;

struct Tmt T[] = {
      {40,17,2,NULL,NULL},                                      // le OK
      {55,17,3,NULL,NULL},                                  // le CANCEL

      {5, 2,8,"Convert History",&l7},
      {5, 3,8,"Confirmation before exiting",&l3},
      {5, 4,8,"Point SubDir",&l4},
      {5, 5,8,"LogFile",&l5},
      {5, 6,8,"ESC to close windows",&l6},
      {5, 7,8,"Display Hidden File",&l8},
      {5, 8,8,"Auto Reload Directory",&l9},
      {5, 9,8,"Verify History Directory",&l10},
      {5,10,8,"Quick Palette",&l11},
      {5,11,8,"Highlight groups",&l12},
      {5,12,8,"Insert move down",&l13},
      {5,13,8,"Select directories",&l14},
      {5,14,8,"Estimated copying time",&l15},
      {5,15,8,"Auto adjust viewer size",&l16},
      {5,16,8,"Save position in viewer",&l18},
      {5,17,8,"Load startup directory",&l17},

      {39,2,7, "Size Trash   ",&l1},
      {39,3,8, "Key After Shell",&l2},
      {39,4,8,"Add selection function",&l19},

      {3,1,9,&x3,&y3},
      {37,6,9,&x1,&y1},
      {37,1,9,&x2,&y2},

      {40, 8,5," Serial Port ",&l1},         // la gestion du port serie
      {55, 8,5," Mask Setup  ",&l2},           // la gestion des masques
      {40,10,5," File Setup  ",&l3},           // la gestion des masques
      {55,10,5," Ext. Setup  ",&l4}         // la gestion des extensions
      };

struct TmtWin F = {-1,3,74,22,"Setup"};

int n;

l1=KKCfg->mtrash;
l2=KKCfg->KeyAfterShell;
l3=KKCfg->confexit;
l4=KKCfg->pntrep;
l5=KKCfg->logfile;
l6=KKCfg->Esc2Close;
l7=KKCfg->cnvhist;
l8=KKCfg->hidfil;
l9=KKCfg->autoreload;
l10=KKCfg->verifhist;
l11=KKCfg->palafter;
l12=KKCfg->dispcolor;
l13=KKCfg->insdown;
l14=KKCfg->seldir;
l15=KKCfg->esttime;
l16=KKCfg->V.ajustview;
l17=KKCfg->currentdir;
l18=KKCfg->V.saveviewpos;
l19=KKCfg->addselect;

do
{
n=WinTraite(T,28,&F,0);

if (n==-1) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

if (T[n].type==5)
    {
    if (T[n].entier==&l1)  SerialSetup();
    if (T[n].entier==&l2)  MasqueSetup();
    if (T[n].entier==&l3)  FileSetup();
    if (T[n].entier==&l4)  ExtSetup();
    }
}
while(T[n].type==5);


KKCfg->mtrash=l1;
KKCfg->KeyAfterShell=l2;
KKCfg->confexit=l3;
KKCfg->pntrep=l4;
KKCfg->logfile=l5;
KKCfg->Esc2Close=l6;
KKCfg->cnvhist=l7;
KKCfg->hidfil=l8;
KKCfg->autoreload=l9;
KKCfg->verifhist=l10;
KKCfg->palafter=l11;
KKCfg->dispcolor=l12;
KKCfg->insdown=l13;
KKCfg->seldir=l14;
KKCfg->esttime=l15;
KKCfg->V.ajustview=l16;
KKCfg->currentdir=l17;
KKCfg->V.saveviewpos=l18;
KKCfg->addselect=l19;

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



void ExtSetup(void)
{
static int l1,l2,l3,l4,l5,l6;
static char Txt[64],Bmp[64],Snd[64],Arc[64],Exe[64],Usr[64];
static int DirLength=63;


struct Tmt T[16] = {
      { 1, 1,8,"Text:         ",&l1},
      { 9, 2,1,Txt,&DirLength},
      { 1, 3,8,"Bitmap:       ",&l2},
      { 9, 4,1,Bmp,&DirLength},
      { 1, 5,8,"Sound:        ",&l3},
      { 9, 6,1,Snd,&DirLength},
      { 1, 7,8,"Archive:      ",&l4},
      { 9, 8,1,Arc,&DirLength},
      { 1, 9,8,"Executable:   ",&l5},
      { 9,10,1,Exe,&DirLength},
      { 1,11,8,"User Defined: ",&l6},
      { 9,12,1,Usr,&DirLength},

      {13,14,2,NULL,NULL},                                      // le OK
      {50,14,3,NULL,NULL}                                   // le CANCEL
      };

struct TmtWin F = {-1,5,74,21,"Extension Setup"};

int n;

l1=KKCfg->Enable_Txt;
l2=KKCfg->Enable_Bmp;
l3=KKCfg->Enable_Snd;
l4=KKCfg->Enable_Arc;
l5=KKCfg->Enable_Exe;
l6=KKCfg->Enable_Usr;

strcpy(Txt,KKCfg->ExtTxt);
strcpy(Bmp,KKCfg->ExtBmp);
strcpy(Snd,KKCfg->ExtSnd);
strcpy(Arc,KKCfg->ExtArc);
strcpy(Exe,KKCfg->ExtExe);
strcpy(Usr,KKCfg->ExtUsr);

n=WinTraite(T,14,&F,0);

if (n==-1) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

strcpy(KKCfg->ExtTxt,Txt);
strcpy(KKCfg->ExtBmp,Bmp);
strcpy(KKCfg->ExtSnd,Snd);
strcpy(KKCfg->ExtArc,Arc);
strcpy(KKCfg->ExtExe,Exe);
strcpy(KKCfg->ExtUsr,Usr);

KKCfg->Enable_Txt=l1;
KKCfg->Enable_Bmp=l2;
KKCfg->Enable_Snd=l3;
KKCfg->Enable_Arc=l4;
KKCfg->Enable_Exe=l5;
KKCfg->Enable_Usr=l6;
}

void ScreenSetup(void)
{
static int sw,sy;

static int l1,l2,l3,l4,l5,l6,l7;

static char x1=32,x2=32,x3=32,x4=32;
static int y1=3,y2=3,y3=4,y4=4;

struct Tmt T[] = {
      { 5, 2,10,"25 lines",&sw},
      { 5, 3,10,"30 lines",&sw},
      { 5, 4,10,"50 lines",&sw},
      { 5, 7, 8,"Use Font",&l1},
      { 5, 8, 8,"Use User Font",&l7},
      { 5, 9, 7,"Screen Saver ",&l2},

      {39, 2,10,"Norton like ",&sy},
      {39, 3,10,"7-bit mode  ",&sy},
      {39, 4,10,"Thin mode   ",&sy},
      {39, 5,10,"Ketchup Mode",&sy},

      {39, 8, 8,"Display lower path",&l4},
      {39, 9, 8,"Display IDF bar",&l5},
      {39,10, 8,"Display function bar",&l6},
      {39,11, 7,"Size of pannel",&l3},

      {3,1,9,&x1,&y1},
      {3,6,9,&x2,&y2},
      {37,1,9,&x3,&y3},
      {37,7,9,&x4,&y4},

      {11,1,0," Number of lines ",NULL},
      {46,1,0," Window Design ",NULL},

      {13,13,2,NULL,NULL},                                      // le OK
      {50,13,3,NULL,NULL}                                   // le CANCEL
      };

struct TmtWin F = {-1,5,74,20,"Window & Color"};

int n;

switch(Cfg->TailleY)
    {
    case 25:  sw=0; break;
    case 50:  sw=2; break;
    case 30:  
    default:  sw=1; break;
    }
sy=KKCfg->fentype+4;
l1=Cfg->font;
l2=Cfg->SaveSpeed;
l3=KKCfg->sizewin;
l4=KKCfg->pathdown;
l5=KKCfg->isidf;
l6=KKCfg->isbar;
l7=KKCfg->userfont;

n=WinTraite(T,22,&F,0);

if (n==-1) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

switch(sw)
    {
    case 0:  Cfg->TailleY=25; break;
    case 1:  Cfg->TailleY=30; break;
    case 2:  Cfg->TailleY=50; break;
    }
KKCfg->fentype=sy-4;
Cfg->font=l1;
Cfg->SaveSpeed=l2;
if ((l3>=6) | (l3==0))
    KKCfg->sizewin=l3;

KKCfg->pathdown=l4;
KKCfg->isidf=l5;
KKCfg->isbar=l6;
KKCfg->userfont=l7;

GestionFct(67);                                    // Rafraichit l'ecran
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

struct TmtWin F = {-1,5,34,18,"Serial Setup"};

int n;

l1=Cfg->comport;
l2=Cfg->comspeed;
l3=Cfg->combit;
sprintf(Dir,"%c",Cfg->comparity);
l5=Cfg->comstop;

n=WinTraite(T,8,&F,0);

if (n==-1) return;                                             // ESCape
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

struct TmtWin F = {-1,5,80,22,"Mask Setup"};

int n;

l1=KKCfg->V.Mask[11]->Ignore_Case;
l2=KKCfg->V.Mask[12]->Ignore_Case;
l3=KKCfg->V.Mask[13]->Ignore_Case;
l4=KKCfg->V.Mask[14]->Ignore_Case;

SplitMasque(KKCfg->V.Mask[11]->chaine,buffer[0],buffer[1]);
SplitMasque(KKCfg->V.Mask[12]->chaine,buffer[2],buffer[3]);
SplitMasque(KKCfg->V.Mask[13]->chaine,buffer[4],buffer[5]);
SplitMasque(KKCfg->V.Mask[14]->chaine,buffer[6],buffer[7]);

n=WinTraite(T,18,&F,0);

if (n==-1) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

KKCfg->V.Mask[11]->Ignore_Case=l1;
KKCfg->V.Mask[12]->Ignore_Case=l2;
KKCfg->V.Mask[13]->Ignore_Case=l3;
KKCfg->V.Mask[14]->Ignore_Case=l4;

JointMasque(KKCfg->V.Mask[11]->chaine,buffer[0],buffer[1]);
if (strlen(KKCfg->V.Mask[11]->chaine)>2)
    strcpy(KKCfg->V.Mask[11]->title,"User config 1");
    else
    strcpy(KKCfg->V.Mask[11]->title,"");
JointMasque(KKCfg->V.Mask[12]->chaine,buffer[2],buffer[3]);
if (strlen(KKCfg->V.Mask[12]->chaine)>2)
    strcpy(KKCfg->V.Mask[12]->title,"User config 2");
    else
    strcpy(KKCfg->V.Mask[12]->title,"");
JointMasque(KKCfg->V.Mask[13]->chaine,buffer[4],buffer[5]);
if (strlen(KKCfg->V.Mask[13]->chaine)>2)
    strcpy(KKCfg->V.Mask[13]->title,"User config 3");
    else
    strcpy(KKCfg->V.Mask[13]->title,"");
JointMasque(KKCfg->V.Mask[14]->chaine,buffer[6],buffer[7]);
if (strlen(KKCfg->V.Mask[14]->chaine)>2)
    strcpy(KKCfg->V.Mask[14]->title,"User config 4");
    else
    strcpy(KKCfg->V.Mask[14]->title,"");

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
GestionFct(67);                                    // Rafraichit l'ecran

if (Cfg->speedkey==1)
    Cfg->speedkey=KeyTurbo(1);
}

/*--------------------------------------------------------------------*\
|-  Information on FILE_ID.DIZ                                        -|
\*--------------------------------------------------------------------*/
void ViewFileID(FENETRE *Fen)
{
char path[256],*name;

strcpy(path,Fen->Fen2->path);
if (Fen->Fen2->F[Fen->Fen2->pcur]->name[0]!='.')
    Path2Abs(path,Fen->Fen2->F[Fen->Fen2->pcur]->name);  // Ajout gedeon
Path2Abs(path,"FILE_ID.DIZ");

name=AccessAbsFile(path);  // Ajout GEDEON -----------------------------

if (name!=NULL)
    View(&(KKCfg->V),name,4);

DFen->init=1;
}





/*--------------------------------------------------------------------*\
|-                     Test la vitesse d'affichage                    -|
\*--------------------------------------------------------------------*/

void SpeedTest(void)
{
static char vit1[64],vit2[64];

int x,y;
clock_t Cl;
long Cl1;
char c;
int n,m;
double fvit1;

struct Tmt T[6] =  {
      {6,17,2,NULL,NULL},                                       // le OK
      {21,17,3,NULL,NULL},                                  // le CANCEL

      {5,3,0,"No-Random Char",NULL},
      {5,4,0,"Random Char",NULL},
      {25,3,0,vit1,NULL},
      {25,4,0,vit2,NULL}
      };

struct TmtWin F = {-1,3,74,22,"Speed Test"};

SaveScreen();

c=(rand()*80)/RAND_MAX+32;
m=0;
n=0;

Cl=clock();

for(n=0;n<500000;n++)
    {
    x=(rand()*(Cfg->TailleX))/RAND_MAX;
    y=(rand()*Cfg->TailleY)/RAND_MAX;

    if (m>10000) m=0,c=(rand()*80)/RAND_MAX+32;
    m++;

    AffChr(x,y,c);
    }
Cl1=clock()-Cl;

LoadScreen();
         
fvit1=(500000./Cl1)*CLOCKS_PER_SEC;

sprintf(vit1,"%f characters/seconds",fvit1);
sprintf(vit2,"? characters/seconds");

n=WinTraite(T,6,&F,0);

}




/*--------------------------------------------------------------------*\
|- Information about files                                            -|
\*--------------------------------------------------------------------*/
int SearchInfo(FENETRE *Fen)
{
int i;
struct file *F;
RB_IDF Info;
FENETRE *Fen2;
static char Buffer[256];
int nouv=0;               // Renvoit le nombre de nouvelles informations

Fen2=Fen->Fen2;

for (i=0;i<Fen2->nbrfic;i++)
    {
    F=Fen2->F[i];

    if (F->info==NULL)
        {
        nouv++;

        F->info=(char*)GetMem(82);

        if ( (F->attrib & _A_SUBDIR)==_A_SUBDIR)
            sprintf(F->info,"%cDirectory",0);
            else
        if (Fen2->system!=0)
            sprintf(F->info,"%cInternal File",0);
            else
            {
            strcpy(Info.path,Fen2->path);
            Path2Abs(Info.path,F->name);

            Traitefic(&Info);
            sprintf(Buffer,"%s - %s - %s",Info.format,Info.fullname,
                                                       Info.message[0]);
            ClearSpace(Buffer);
            Buffer[80]=0;
            sprintf(F->info+1,"%-80s",Buffer);

            F->info[0]=Info.Btype;
            }
        }
    if (KbHit()) break;
    }

return nouv;
}



/*--------------------------------------------------------------------*\
|-  Fenetre TREE                                                      -|
\*--------------------------------------------------------------------*/

void FenTree(FENETRE *Fen)
{
int x,y,n,i,j,t,pos;
FENETRE *OldFen;
char path[256];

static char oldpath[256];
static char prem[256];
static char tab;
static char op[50];

x=Fen->x+1;
y=Fen->y+1;

if ( (!stricmp(oldpath,Fen->Fen2->path)) & (Fen->init==0) )
    {
    PrintAt(Fen->x+38,Fen->y+Fen->yl-1,"?");
    }
    else
    {
    WinCadre(Fen->x,Fen->y,Fen->x+Fen->xl,Fen->y+Fen->yl,1);
    Window(Fen->x+1,Fen->y+1,Fen->x+Fen->xl-1,Fen->y+Fen->yl-1,164);

    strcpy(oldpath,Fen->Fen2->path);

    PrintAt(Fen->x+38,Fen->y+Fen->yl-1,"");
    for(i=0;i<50;i++) op[i]=0;

    strcpy(prem,Fen->Fen2->path);
    prem[3]=0;
    tab=1;
    pos=0;
    op[12]=1;

    Fen->init=0;
    }

strcpy(path,prem);

OldFen=DFen;
DFen=Fenetre[2];

t=tab;
j=0;
CommandLine("#cd %s",path);

for(i=0;i<Fen->yl-Fen->y-1;i++)
    {
    while ( (j<DFen->nbrfic) & ((DFen->F[j]->attrib&16)!=16) ) j++;

    if (j>=DFen->nbrfic) break;

    if (op[i]==1)
        {
        Path2Abs(path,DFen->F[j]->name);
        CommandLine("#cd %s",path);
        j=0;
        t++;
        }
        else
        {
        for (n=0;n<t;n++)
            PrintAt(Fen->x+1+n*2,Fen->y+2+i,"ÃÄ");

        PrintAt(Fen->x+1+t*2,Fen->y+2+i,"%s",DFen->F[j]->name);
        j++;
        }
    }

for(;i<Fen->yl-Fen->y-1;i++)
    PrintAt(Fen->x+1,Fen->y+2+i,"%-38s","|-|");

DFen=OldFen;
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

TXTMode();
LoadPal(Cfg->palette);

if (KKCfg->userfont)
    InitFontFile(KKFics->trash);
    else
    InitFont();

CalcSizeWin(Fenetre[0]);
CalcSizeWin(Fenetre[1]);

AfficheTout();
}

/*--------------------------------------------------------------------*\
|-  Efface la fenˆtre gestion de fichier                              -|
\*--------------------------------------------------------------------*/

void ClearNor(FENETRE *Fen)
{
char col;
int sb;

int i;
int x,y;
int xl,yl;

char a;

Fen->oldscur=0;
Fen->oldpcur=-1;

sb=1+(KKCfg->pathdown);

ColWin(Fen->x+1,Fen->y+1,Fen->xl-1,Fen->yl-1,Cfg->col[0]);

switch(KKCfg->fentype) //---Couleur uniquement pour fentype=1,2 ou 3 ---
    {
    case 1:
    case 2:
    case 3:
        for(x=Fen->x;x<Fen->xl;x++)
            AffCol(x,Fen->y,Cfg->col[37]);
        for(y=Fen->y;y<=Fen->yl;y++)
            AffCol(Fen->x,y,Cfg->col[37]);

        for(x=Fen->x+1;x<=Fen->xl;x++)
            AffCol(x,Fen->yl,Cfg->col[38]);
        for(y=Fen->y;y<Fen->yl;y++)
            AffCol(Fen->xl,y,Cfg->col[38]);
        break;
    }

x=Fen->x;
y=Fen->y;

xl=Fen->xl;
yl=Fen->yl;

switch (KKCfg->fentype)
    {
    case 1:
        Cfg->Tfont=179;

        PrintAt(x,y,      "ÉÍÍÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÑÍÍÍÍÍ»");
        PrintAt(x,y+1,    "º    Name    ³   Size   ³  Date  ³Time º");
        for (i=y+2;i<yl-sb-1;i++)
             PrintAt(x,i, "º                                      º");
        PrintAt(x,yl-sb-1,"ÇÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄ¶");
        for (i=yl-sb;i<yl;i++)
             PrintAt(x,i, "º                                      º");
        PrintAt(x,yl,     "ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼");
        col=Cfg->col[4];

        break;
    case 2:
        if (Cfg->UseFont==1)
            Cfg->Tfont=168;                // Barre Verticale | with 8x?

        PrintAt(x,y,      "ÛÛÛÛÛNameÛÛÛÛÛÛÛÛSizeÛÛÛÛÛÛDateÛÛÛTimeÛÛ");
        for (i=y+1;i<yl-sb-1;i++)
            PrintAt(x,i,  "Û                                      Û");
        PrintAt(x,yl-sb-1,"ÛßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßÛ");
        for (i=yl-sb;i<yl;i++)
            PrintAt(x,i,  "Û                                      Û");
        PrintAt(x,yl,     "ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß");
        col=(Cfg->col[4]&15)+(Cfg->col[37]&15)*16;
        break;
    case 3:
        Cfg->Tfont=179;

        PrintAt(x,y,      "ÚÄ Ä Name Ä ÄÂ Ä Size Ä ÂÄ Date ÄÂTime ¿");
        for (i=y+1;i<yl-sb-1;i++)
           PrintAt(x,i,"³                                      ³");
        PrintAt(x,yl-sb-1,"³ÚÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄ¿³");
        for (i=yl-sb;i<yl;i++)
            PrintAt(x,i,  "³                                      ³");
        PrintAt(x,yl,     "ÀÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙÙ");
        col=(Cfg->col[4]&15)+(Cfg->col[37]&240);
        break;
    case 4:
        if (Cfg->UseFont==1)
            Cfg->Tfont=168;                // Barre Verticale | with 8x?
        a=Cfg->Tfont;

        Window(x+1,yl-sb,xl-1,yl-1,Cfg->col[0]);

        PrintAt(x+1,Fen->y2,
                     "    Name    %c   Size   %c  Date  %cTime ",a,a,a);
        WinLine(x+1,yl-sb-1,38,1);
        Cadre(x,y,xl,yl,1,Cfg->col[37],Cfg->col[38]);
        col=Cfg->col[4];
        break;
    }

ColLin(x+ 5,Fen->y2,4,col);
ColLin(x+17,Fen->y2,4,col);
ColLin(x+27,Fen->y2,4,col);
ColLin(x+34,Fen->y2,4,col);
}

void FenNor(FENETRE *Fen)
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

if (Fen->scur>=Fen->yl2)
    Fen->scur=Fen->yl2-1;

x1=Fen->x+1;
y1=Fen->y2+1;

n=(Fen->pcur)-(Fen->scur);                                    // premier

InfoSelect(Fen);

if (KKCfg->pathdown)
    AffUpperPath(Fen,Fen->yl-2);

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
            strcpy(ch2,"SUB--DIR");

        if ((Fen->F[n]->attrib & _A_VOLID)==_A_VOLID)
            strcpy(ch2,"##Vol-ID##");

        if (nom[0]=='*')
            {
            int d;
            sscanf(nom+1,"%d",&d);

            switch(d)
                {
                case 86: //--- Eject CD --------------------------------
                    strcpy(ch1,"CD-ROM Drive");
                    strcpy(ch2," RELOAD ");
                    break;
                case 27: //--- Reload Disk -----------------------------
                    strcpy(ch1,"Drive       ");
                    strcpy(ch2," RELOAD ");
                    break;
                case 92:
                    strcpy(ch1,"(Un)select  ");
                    strcpy(ch2,"Function");
                    break;
                default:
                    strcpy(ch1,"Unknow      ");
                    strcpy(ch2,"Function");
                    break;
                }

//            strcpy(ch3,"        ");
//            strcpy(ch4,"     ");
            }

        if (Fen->F[n]->select!=0)
            memcpy(ch4,"<SEL>",5);

        if ((Fen->F[n]->attrib & _A_HIDDEN)==_A_HIDDEN)
            ch1[8]=176;  // 176,177,178

        if ( (Fen->system==5) &
             (Fen->F[n]->desc==1) &
             ((Fen->F[n]->attrib & _A_SUBDIR)!=_A_SUBDIR) )
            ch1[8]=0x18;

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
        if ( (KKCfg->FenAct==Fen->nfen) & (n==Fen->pcur) )
            {
            if (Fen->F[n]->select==0)
                col=Cfg->col[1];
                else
                col=Cfg->col[3];
            }
            else
            {
            if (Fen->F[n]->select==0)
                {
                col=Cfg->col[0];

                if (KKCfg->dispcolor==1)
                    {
                    if ((FoundExt(ext,KKCfg->ExtExe)) &
                                                    (KKCfg->Enable_Exe))
                        col=Cfg->col[15];                  // Executable
                    else
                    if ((FoundExt(ext,KKCfg->ExtArc)) &
                                                    (KKCfg->Enable_Arc))
                        col=Cfg->col[22];                     // Archive
                    else
                    if ((FoundExt(ext,KKCfg->ExtSnd)) &
                                                    (KKCfg->Enable_Snd))
                        col=Cfg->col[23];                         // Son
                    else
                    if ((FoundExt(ext,KKCfg->ExtBmp)) &
                                                    (KKCfg->Enable_Bmp))
                        col=Cfg->col[32];                       // Image
                    else
                    if ((FoundExt(ext,KKCfg->ExtTxt)) &
                                                    (KKCfg->Enable_Txt))
                        col=Cfg->col[33];                       // Texte
                    else
                    if ((FoundExt(ext,KKCfg->ExtUsr)) &
                                                    (KKCfg->Enable_Usr))
                        col=Cfg->col[34];                // User defined
                    }
                }
                else
                col=Cfg->col[2];
            }


// ---------------------------------------------------------------------

        ColLin(x1,y1,12,col);      PrintAt(x1,y1,"%s",ch1);
        AffCol(x1+12,y1,Cfg->col[0]);   AffChr(x1+12,y1,a);
        ColLin(x1+13,y1,10,col);   PrintAt(x1+13,y1,"%s",ch2);
        AffCol(x1+23,y1,Cfg->col[0]);   AffChr(x1+23,y1,a);
        ColLin(x1+24,y1,8,col);    PrintAt(x1+24,y1,"%s",ch3);
        AffCol(x1+32,y1,Cfg->col[0]);   AffChr(x1+32,y1,a);
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
       ColLin(x1,y1,38,Cfg->col[0]);
       }

if ( (dispall) & (KKCfg->dispath) )
    {
    AffUpperPath(Fen,Fen->y);
    }

Fen->oldscur=Fen->scur;
Fen->oldpcur=Fen->pcur;

Fen->init=0;
}

void FenDIZ(FENETRE *Fen)
{
char *Buf;
RB_IDF Info;
FENETRE *Fen2;
struct file *F;
int x,y,i;

Fen2=Fen->Fen2;
F=Fen2->F[Fen2->pcur];

if ( ( (F->attrib & _A_SUBDIR)==_A_SUBDIR) & (F->name[0]=='.') )
    return;

if (F->name[0]=='*')
    return;

if (Fen2->system!=0) return;

Cadre(Fen->x,Fen->y,Fen->xl,Fen->yl,1,Cfg->col[37],Cfg->col[38]);

strcpy(Info.path,Fen2->path);
Path2Abs(Info.path,F->name);

if ( (F->attrib & _A_SUBDIR)==_A_SUBDIR)
    {
    FENETRE *OldDFen;
    int nbr,size;
    int sel;
    char temp[32];

    memset(&Info,0,sizeof(RB_IDF));

    if (KKCfg->_Win95==1)
        {
        InfoLongFile(Fen2,winbuffer);
        strncpy(Info.fullname,winbuffer,80);
        Info.fullname[79]=0;
        }
        else
        strcpy(Info.fullname,F->name);

    OldDFen=DFen;

    sel=F->select;
    F->select=1;

    CountRepSize(Fen2,Fenetre[2],&nbr,&size);

    F->select=sel;

    DFen=OldDFen;

    strcpy(Info.format,"Directory");

    Long2Str(size,temp);

    sprintf(Info.message[0]," %-12s bytes in %d files",temp,nbr);
    }
    else
    {
    Traitefic(&Info);
    }

Buf=(char*)GetMem(4000);

Makediz(&Info,Buf);

Window(Fen->x+1,Fen->y+1,Fen->xl-1,Fen->y+4,Cfg->col[16]);

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

Cadre(Fen->x+1,Fen->y+5,Fen->xl-1,y,2
                                            ,Cfg->col[37],Cfg->col[38]);
ColWin(Fen->x+2,Fen->y+6,Fen->xl-2,y-1,Cfg->col[39]);

Window(Fen->x+1,y+1,Fen->xl-1,Fen->yl-1,Cfg->col[39]);

LibMem(Buf);

Fen->init=0;
}


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void ClearInfo(FENETRE *Fen)
{
Cadre(Fen->x,Fen->y,Fen->xl,Fen->yl,1,Cfg->col[37],Cfg->col[38]);

Window(Fen->x+1,Fen->y+1,Fen->xl-1,Fen->yl-1,Cfg->col[39]);

ChrLin(Fen->x+1,Fen->yl-2,Fen->xl-Fen->x-1,196);
PrintAt(Fen->x+1,Fen->yl-1,"        Press CTRL-Y to close");
}

void DispInfo(FENETRE *Fen)
{
int n;                                                       // Compteur
FENETRE *Fen2;
static char buffer[40];

short i;

int x,y;

Fen2=Fen->Fen2;

if (Fen2->InfoPos>40) Fen2->InfoPos=40;

x=Fen->x+1;
y=Fen2->y2+1;

n=(Fen2->pcur)-(Fen2->scur);                                  // premier

if (n<0) n=0;

for (i=0;(i<Fen2->yl2) & (n<Fen2->nbrfic);i++,n++,y++)
    {
// ------------------ Line Activity ------------------------------------
    if (n==(Fen2->pcur))
        ColLin(x,y,38,Cfg->col[1]);
        else
        {
        if (Fen2->F[n]->info!=NULL)
            switch(Fen2->F[n]->info[0])
                {
                case 1:  ColLin(x,y,38,Cfg->col[23]);   break;
                case 2:  ColLin(x,y,38,Cfg->col[23]);   break;
                case 3:  ColLin(x,y,38,Cfg->col[22]);   break;
                case 4:  ColLin(x,y,38,Cfg->col[32]);   break;
                case 5:  ColLin(x,y,38,Cfg->col[32]);   break;
                case 6:  ColLin(x,y,38,Cfg->col[0]);    break;
                default: ColLin(x,y,38,Cfg->col[0]);    break;
                }
            else
                ColLin(x,y,38,Cfg->col[4]);
        }

    if (Fen2->F[n]->info!=NULL)
        {
        memcpy(buffer,Fen2->F[n]->info+1+Fen2->InfoPos,38);
        buffer[38]=0;
        PrintAt(x,y,"%-38s",buffer);
        }
        else
        PrintAt(x,y,"%-38s","?");
    }

for (;(i<Fen2->yl2);i++,y++)
    {
    PrintAt(x,y,"%-38s","");
    ColLin(x,y,38,Cfg->col[4]);
    }
}


/*--------------------------------------------------------------------*\
|-  Fenˆtre avec les infos sur fichiers d'apres header                -|
\*--------------------------------------------------------------------*/

void FenInfo(FENETRE *Fen)
{
if (Fen->init==1)
    {
    ClearInfo(Fen);
    Fen->init=0;
    Fen->Fen2->InfoPos=0;
    }

DispInfo(Fen);

if (SearchInfo(Fen)!=0)
    DispInfo(Fen);
}



/*--------------------------------------------------------------------*\
|-           Display about select                                     -|
\*--------------------------------------------------------------------*/


void InfoSelect(FENETRE *Fen)
{
char temp[20];

if (Fen->FenTyp!=0) return;

if (Fen->nbrsel==0)
    {
    if (Fen->nbrfic==1)
        PrintAt(Fen->x+2,Fen->yl-1,"%-10s bytes in one file         ",
            Long2Str(Fen->taillefic,temp));
    else
        PrintAt(Fen->x+2,Fen->yl-1,"%-10s bytes in %3d files        ",
            Long2Str(Fen->taillefic,temp),Fen->nbrfic);
    }
else
    {
    if (Fen->nbrsel==1)
        PrintAt(Fen->x+2,Fen->yl-1,"%-10s b. in one selected file   ",
            Long2Str(Fen->taillesel,temp));
    else
        PrintAt(Fen->x+2,Fen->yl-1,"%-10s b. in %3d selected files  ",
            Long2Str(Fen->taillesel,temp),Fen->nbrsel);
    }
}


/*--------------------------------------------------------------------*\
|-  The KEY's menu                                                    -|
\*--------------------------------------------------------------------*/

void CtrlMenu(void)
{
char temp[20];

if (DFen->yl==0) return;
if (DFen->nbrfic==1)
    PrintAt(DFen->x+2,DFen->yl-1,"%-10s bytes in one file         ",
        Long2Str(DFen->taillefic,temp));
else
    PrintAt(DFen->x+2,DFen->yl-1,"%-10s bytes in %3d files        ",
        Long2Str(DFen->taillefic,temp),DFen->nbrfic);
}

void ShiftMenu(void)
{
if (DFen->yl==0) return;

if ((KKCfg->pathdown) | (KKCfg->dispath))
    return;

AffUpperPath(DFen,DFen->yl-1);
AffUpperPath(DFen->Fen2,DFen->yl-1);
}


void AltMenu(void)
{
static char buffer[40];

if (DFen->yl==0) return;

if ((KKCfg->_Win95==1) & (DFen->system==0))
    InfoLongFile(DFen,_intbuffer);
    else
    strcpy(_intbuffer,DFen->F[DFen->pcur]->name);

if (strlen(_intbuffer)>36)
    {
    memcpy(buffer,_intbuffer,3);
    memcpy(buffer+3,"...",3);
    memcpy(buffer+6,_intbuffer+strlen(_intbuffer)-30,30);
    buffer[36]=0;
    }
    else
    {
    strcpy(buffer,_intbuffer);
    }

PrintAt(DFen->x+2,DFen->yl-1,"%-36s",buffer);
}


/*--------------------------------------------------------------------*\
|-  Information on disk                                               -|
\*--------------------------------------------------------------------*/

void FenDisk(FENETRE *Fen)
{
char *Buf,temp[32],disk[4],volume[32];
char InfoVol[32];

FENETRE *Fen2;
struct file *F;
int x,y;

static char chaine[80];
short WindowsActif,HVer,NVer;

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

tfree=GetDiskFree(toupper(Fen2->path[0])-'A'+1)/1024;

if ( (tfree!=oldfree) | (strcmp(oldpath,Fen2->path)!=0) )
    Fen->init=1;

if (Fen->init==0)
    {
    PrintAt(Fen->xl-1,Fen->yl-1,"?");
    return;
    }

x=Fen->x+1;
y=Fen->y+1;


Buf=Fen2->path;
if (strlen(Buf)>37) Buf+=(strlen(Buf)-37);

ttotal=(d.total_clusters)*(d.sectors_per_cluster);
ttotal=ttotal*(d.bytes_per_sector)/1024;

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
        sprintf(chaine,"Windows V %d.%d Extended Mode",HVer,NVer);
        break;
    }

memcpy(disk,Fen2->path,3);
disk[3]=0;
drive=toupper(disk[0])-'A';

GetVolume(drive,volume);

TypeDisk(drive,InfoVol);

Cadre(Fen->x,Fen->y,Fen->xl,Fen->yl,1,Cfg->col[37],Cfg->col[38]);
Window(Fen->x+1,Fen->y+1,Fen->xl-1,Fen->yl-1,Cfg->col[39]);

PrintAt(x+1,y,"%s",RBTitle);
ColLin(x,y,38,Cfg->col[40]);
ChrLin(x,y+1,38,196);

PrintAt(x+1,y+2,"Current directory");
ColLin(x+1,y+2,37,Cfg->col[40]);
PrintAt(x+1,y+3,"%s",Buf);
ChrLin(x,y+4,38,196);


PrintAt(x+1,y+5,"Current disk");
ColLin(x+1,y+5,37,Cfg->col[40]);

PrintAt(x+1,y+6,"%s [%s] %s",disk,volume,InfoVol);

PrintAt(x+1,y+7,"Free space: %8s kilobytes",Long2Str(tfree,temp));
PrintAt(x+1,y+8,"Capacity:   %8s kilobytes",Long2Str(ttotal,temp));

ChrLin(x,y+9 ,38,196);
PrintAt(x+1,y+10,"System Information");
ColLin(x+1,y+10,37,Cfg->col[40]);

PrintAt(x+1,y+11,"ş %s",chaine);            // Information about windows
PrintAt(x+1,y+12,"ş Initialisation: %d clocks ",Info->temps);
PrintAt(x+1,y+13,"ş Long filenames support: %s ",
                                            KKCfg->_Win95 ?"Yes": "No");

PrintAt(Fen->xl-1,Fen->yl-1,"");

oldfree=tfree;
strcpy(oldpath,Fen2->path);

Fen->init=0;
}

/*--------------------------------------------------------------------*\
|-  Information on FILE_ID.DIZ                                        -|
\*--------------------------------------------------------------------*/
void FenFileID(FENETRE *Fen)
{
int x,y,n,l;
char c;
char path[256],*name;
FILE *fic;
char *ext;
static char oldpath[256],oldname[256];

x=Fen->x+1;
y=Fen->y+1;

Cadre(Fen->x,Fen->y,Fen->xl,Fen->yl,1,Cfg->col[37],Cfg->col[38]);
Window(Fen->x+1,Fen->y+1,Fen->xl-1,Fen->yl-1,Cfg->col[39]);

ext=getext(Fen->Fen2->F[Fen->Fen2->pcur]->name);

strcpy(path,Fen->Fen2->path);
if ( (Fen->Fen2->F[Fen->Fen2->pcur]->name[0]!='.') &
    ( (((Fen->Fen2->F[Fen->Fen2->pcur]->attrib)&_A_SUBDIR)==_A_SUBDIR)|
      (!stricmp(ext,"ARJ")) |
      (!stricmp(ext,"ZIP")) |
      (!stricmp(ext,"RAR")) |
      (!stricmp(ext,"LHA")) ))
    Path2Abs(path,Fen->Fen2->F[Fen->Fen2->pcur]->name);  // Ajout gedeon
/*    else
    oldpath[0]=0;*/  // PQ ?
Path2Abs(path,"FILE_ID.DIZ");

if (strcmp(path,oldpath)!=0)
    {
    strcpy(oldpath,path);

    name=AccessAbsFile(path);  // Ajout GEDEON ------------------------
    if (name==NULL)
        strcpy(oldname,"");
        else
        strcpy(oldname,name);

    PrintAt(Fen->xl-1,Fen->yl-1,"");
    }
    else
    PrintAt(Fen->xl-1,Fen->yl-1,"?");

// Affichage du file_id.diz dans la fenetre ----------------------------

if (oldname[0]!=0)
    {
    fic=fopen(oldname,"rb");
    if (fic==NULL)
        {
        PrintAt(x+1,y+1,"FILE_ID.DIZ doesn't exist !");
        return;
        }

    l=flength(fileno(fic));

    if (l>32768) l=32768;

    for(n=0;n<l;n++)
        {
        c=fgetc(fic);

        switch(c)
            {
            case 10:
                x=Fen->x+1;
                y++;
                break;
            case 7:
            case 13:
                break;
            default:
                if (x<Fen->xl)
                    {
                    AffChr(x,y,c);
                    x++;
                    }
                break;
            }
        if (y>=Fen->yl) break;
        }
    fclose(fic);
    }

Fen->init=0;
}

void CalcSizeWin(FENETRE *Fen)
{
int sb;

sb=1+(KKCfg->pathdown);

if (KKCfg->isbar)
    KKCfg->cmdline=Cfg->TailleY-2;
    else
    KKCfg->cmdline=Cfg->TailleY-1;


if (KKCfg->isidf)
    Fen->y=1;
    else
    Fen->y=0;

if (KKCfg->sizewin==0)
    Fen->yl=(KKCfg->cmdline)-1;
    else
    Fen->yl=Fen->y+KKCfg->sizewin-1;

if (Fen->yl>(KKCfg->cmdline)-1)
    Fen->yl=(KKCfg->cmdline)-1;

switch (KKCfg->fentype)
    {
    case 1:
    case 4:
        Fen->y2=Fen->y+1;
        Fen->yl2=Fen->yl-Fen->y-sb-3;
        break;
    case 2:
    case 3:
        Fen->y2=Fen->y;
        Fen->yl2=Fen->yl-Fen->y-sb-2;
    }
}
