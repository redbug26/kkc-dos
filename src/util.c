/*--------------------------------------------------------------------*\
|- Procedures diverses (outils dos)                                   -|
\*--------------------------------------------------------------------*/
#include <ctype.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <bios.h>                                     // Gestion clavier
#include <fcntl.h>
#include <time.h>

#include "kk.h"

#include "util.h"


/*--------------------------------------------------------------------*\
|- Fonction cach‚e IDF                                                -|
\*--------------------------------------------------------------------*/

void ClearSpace(char *name);    //--- efface les espaces inutiles ------


/*--------------------------------------------------------------------*\
|- Prototype local                                                    -|
\*--------------------------------------------------------------------*/
char *GetLine(char *ligne,FILE *fic);


/*--------------------------------------------------------------------*\
|- Buffer global                                                      -|
\*--------------------------------------------------------------------*/

static char buffer[256];

/*--------------------------------------------------------------------*\
|- Fonction                                                           -|
\*--------------------------------------------------------------------*/

int WinAttrib(void)
{
static int sa,sh,sr,ss;

static char Day[3],Month[3],Year[5];
static char Hour[3],Minute[3],Second[3];

static int deux=2,quatre=4;

static char x2=26,x3=12;
static int y2=7,y3=3;


struct Tmt T[34] = {
      { 30, 4,10, "",&sa},
      { 38, 4,10, "",&sa},
      { 46, 4,10, "",&sa},
      { 30, 6,10, "",&sh},
      { 38, 6,10, "",&sh},
      { 46, 6,10, "",&sh},
      { 30, 8,10, "",&sr},
      { 38, 8,10, "",&sr},
      { 46, 8,10, "",&sr},
      { 30,10,10, "",&ss},
      { 38,10,10, "",&ss},
      { 46,10,10, "",&ss},

      { 35,13,11, Day,&deux},
      { 38,13,11, Month,&deux},
      { 41,13,11, Year,&quatre},
      { 35,15,11, Hour,&deux},
      { 38,15,11, Minute,&deux},
      { 41,15,11, Second,&deux},

      { 6,2,0,"Change Attribute        Set    Unset   Change",NULL},
      { 45,1,0,"Not",NULL},

      { 6, 4,0,"Archive",NULL},
      { 6, 6,0,"Hidden ",NULL},
      { 6, 8,0,"Read-only",NULL},
      { 6,10,0,"System",NULL},
      { 6,13,0,"Date",NULL},
      { 6,15,0,"Time",NULL},

      {37,13,0,"/",NULL},
      {40,13,0,"/",NULL},
      {37,15,0,":",NULL},
      {40,15,0,":",NULL},

      { 26,3,9,&x2,&y2},
      { 33,12,9,&x3,&y3},

      {10,17,2,NULL,NULL},                // 1:Ok
      {35,17,3,NULL,NULL},
      };

struct TmtWin F =   {   8,4,69,23, "Change attribute/time"};

int n,m;
unsigned short datep,timep;
unsigned o;


sa=sr=sh=ss=-1;

strcpy(Day,"-");
strcpy(Month,"-");
strcpy(Year,"-");
strcpy(Hour,"-");
strcpy(Minute,"-");
strcpy(Second,"-");


for (n=0;n<DFen->nbrfic;n++)
    {
    if ((DFen->F[n]->select==1) | ((DFen->nbrsel==0) & (n==DFen->pcur)))
        {
        o=(DFen->F[n]->attrib);
        o=(o&0x20)!=0x20;

        if (sa==-1) sa=0+o;
        if (sa!=o) sa=2;

        o=(DFen->F[n]->attrib);
        o=(o&0x2)!=0x2;

        if (sh==-1) sh=3+o;
        if (sh!=3+o) sh=5;

        o=(DFen->F[n]->attrib);
        o=(o&0x1)!=0x1;

        if (sr==-1) sr=6+o;
        if (sr!=6+o) sr=8;

        o=(DFen->F[n]->attrib);
        o=(o&0x4)!=0x4;

        if (ss==-1) ss=9+o;
        if (ss!=9+o) ss=11;

        sprintf(buffer,"%2d",(DFen->F[n]->date)&31);
        if (!strcmp(Day,"-"))        strcpy(Day,buffer);
        if (strcmp(Day,buffer)!=0)   strcpy(Day,"");

        sprintf(buffer,"%2d",((DFen->F[n]->date)>>5)&15);
        if (!strcmp(Month,"-"))        strcpy(Month,buffer);
        if (strcmp(Month,buffer)!=0)   strcpy(Month,"");

        sprintf(buffer,"%4d",((DFen->F[n]->date)>>9)+1980);
        if (!strcmp(Year,"-"))        strcpy(Year,buffer);
        if (strcmp(Year,buffer)!=0)   strcpy(Year,"");

        sprintf(buffer,"%02d",((DFen->F[n]->time)>>11)&31);
        if (!strcmp(Hour,"-"))        strcpy(Hour,buffer);
        if (strcmp(Hour,buffer)!=0)   strcpy(Hour,"");

        sprintf(buffer,"%02d",((DFen->F[n]->time)>>5)&63);
        if (!strcmp(Minute,"-"))        strcpy(Minute,buffer);
        if (strcmp(Minute,buffer)!=0)   strcpy(Minute,"");

        sprintf(buffer,"%02d",((DFen->F[n]->time)&31)*2);
        if (!strcmp(Second,"-"))        strcpy(Second,buffer);
        if (strcmp(Second,buffer)!=0)   strcpy(Second,"");
        }
    }


n=WinTraite(T,34,&F,0);

if (n!=27)  // pas escape
    {
    if (T[n].type!=3)    // pas cancel
        for (n=0;n<DFen->nbrfic;n++)
            {
            if ( (DFen->F[n]->select==1) | ((DFen->nbrsel==0) &
                                                      (n==DFen->pcur)) )
                {
                int hand2;

                strcpy(buffer,DFen->path);
                Path2Abs(buffer,DFen->F[n]->name);

                o=0;
                if (sa!=2)  o=0x20*(1-sa);
                if (sa!=5)  o+=+0x2*(4-sh);
                if (sr!=8)  o+=0x1*(7-sr);
                if (ss!=11) o+=0x4*(10-ss);

                _dos_setfileattr(buffer,o);
                _dos_getfileattr(buffer,&o);
                DFen->F[n]->attrib=o;

                _dos_open(buffer,O_RDONLY,&hand2);
                _dos_getftime(hand2,&datep,&timep);

                if (*Day!=0)
                    {
                    sscanf(Day,"%d",&m);
                    datep=(datep&65504)+m;
                    }
                if (*Month!=0)
                    {
                    sscanf(Month,"%d",&m);
                    datep=(datep&65055)+(m<<5);
                    }
                if (*Year!=0)
                    {
                    sscanf(Year,"%d",&m);
                    datep=(datep&511)+((m-1980)<<9);
                    }
                if (*Hour!=0)
                    {
                    sscanf(Hour,"%d",&m);
                    timep=(timep&2047)+(m<<11);
                    }
                if (*Minute!=0)
                    {
                    sscanf(Minute,"%d",&m);
                    timep=(timep&63519)+(m<<5);
                    }
                if (*Second!=0)
                    {
                    sscanf(Second,"%d",&m);
                    timep=(timep&65504)+(m/2);
                    }

                _dos_setftime(hand2,datep,timep);
                _dos_close(hand2);

                _dos_open(buffer,O_RDONLY,&hand2);
                _dos_getftime(hand2,&datep,&timep);
                _dos_close(hand2);

                DFen->F[n]->date=datep;
                DFen->F[n]->time=timep;
                }

            }
    }

return 1;                                                      // Erreur
}

#ifdef DOOR
void ServerMode(void)
{
union REGS regs;
char buf[32];
int n;
char ok=0;

SaveScreen();

WinCadre(19,4,61,11,0);
Window(20,5,60,10,10*16+4);

PrintAt(35,5,"Server Mode");

com_open(Cfg->comport,Cfg->comspeed,Cfg->combit,
                                           Cfg->comparity,Cfg->comstop);
sprintf(buf,"ATS0=0");
for(n=0;n<strlen(buf);n++)
    com_send_ch(buf[n]);
com_close();


do
    {
    regs.w.dx=Cfg->comport-1;
    regs.w.ax=0x300;

    int386(0x14,&regs,&regs);

    PrintAt(21,7,"Modem Status: (0x%4X) ",regs.w.ax);
    }
while( (!kbhit()) & ((regs.h.al&64)==0) );

if ((regs.h.al&64)==64)
    {
    PrintAt(44,7,"RING");

    com_open(Cfg->comport,Cfg->comspeed,Cfg->combit,
                                           Cfg->comparity,Cfg->comstop);

    while ( (com_read_ch()!=27) & (!kbhit()) )
        {
        clock_t cl;

        sprintf(buf,"\x1b[6n*");
        for(n=0;n<strlen(buf);n++)   com_send_ch(buf[n]);

        cl=clock();
        while((clock()-cl)<60)
            PrintAt(21,8,"Terminal Ready ? : %s",
                                           com_ch_ready() ? "Yes":"No");
        }

    if (com_ch_ready())
        {
        PrintAt(21,10,"Erase port buffer");

        while(com_ch_ready()==1)
            com_read_ch();
        ok=1;
        }

    com_close();

    if (ok==1)
        {
        LoadScreen();
        Cfg->font=0;
        ChangeTaille(25);                          // Rafraichit l'ecran
        DFen->yl=(Cfg->TailleY)-4;
        DFen->Fen2->yl=(Cfg->TailleY)-4;
        ChangeLine();

        DesinitScreen();
        Cfg->display=2;
        InitScreen(Cfg->display);

        sprintf(buf,"\x1b[=255h");
        for(n=0;n<strlen(buf);n++)   com_send_ch(buf[n]);

        UseCfg();

        com_send_ch(27);
        com_send_ch(27);
        }
    }

if (ok==0)
    LoadScreen();

while (kbhit()) Wait(0,0,0);
}
#endif

/*--------------------------------------------------------------------*\
|----------------------------------------------------------------------|
|- Gestion du menu F2                                                 -|
|----------------------------------------------------------------------|
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|- Insertion d'un batch dans une section                              -|
\*--------------------------------------------------------------------*/
char MenuInsert(char *section,char *titre,char *buffer)
{
static char buf1[256],buf2[256];
char tmp_name[15];
FILE *infic,*outfic;
char ok=0;
int n;

infic=fopen(KKFics->menu,"rt");
if (infic==NULL)
    {
    outfic=fopen(KKFics->menu,"wt");
    if (outfic==NULL) return 0;
    fprintf(outfic,"%s\n%s = %s\n",section,titre,buffer);
    fclose(outfic);
    return 1;
    }

tmpnam(tmp_name);
outfic=fopen(tmp_name,"wt");
if (outfic==NULL)
    {
    fclose(infic);
    return 0;
    }

while (fgets(buf2,256,infic)!=NULL)
    {
    memcpy(buf1,buf2,256);
    ClearSpace(buf1);
    fprintf(outfic,"%s",buf2);

    if (!strnicmp(buf1,section,strlen(section)))
        {
        fprintf(outfic,"%s = %s\n",titre,buffer);
        ok=1;
        }
    }

if (ok==0)
    fprintf(outfic,"%s\n%s = %s\n",section,titre,buffer);


fclose(outfic);
fclose(infic);

strcpy(buf1,KKFics->menu);

for (n=0;n<strlen(buf1);n++)
    {
    if (buf1[n]=='.')
        {
        buf1[n]=0;
        break;
        }
    }
strcat(buf1,".bak");

unlink(buf1);
rename(KKFics->menu,buf1);
rename(tmp_name,KKFics->menu);

return 1;
}



char *GetLine(char *ligne,FILE *fic)
{
char *res;
int n;

do
{
res=fgets(ligne,256,fic);
if (res==NULL) break;

n=strlen(res)-1;

while ( ( (res[n]==0x0D) | (res[n]==0x0A) ) & (n>=0) ) res[n]=0,n--;

ClearSpace(res);

if ( (res[0]!=';') & (res[0]!=0) ) break;
}
while(1);

return res;
}

/*--------------------------------------------------------------------*\
|-  Result:    1 --> fin                                              -|
\*--------------------------------------------------------------------*/

int MenuGroup(char *groupe)
{
static char filename[256];
FILE *outfic;
FILE *fic;
char ligne[256],*buf;
char ok;
int nbr;
static struct barmenu bar[20];
int retour,x,y,n;

fic=fopen(KKFics->menu,"rt");
if (fic==NULL) return 1;

//--- Recherche le menu "groupe" ---------------------------------------

ok=0;
while(GetLine(ligne,fic)!=NULL)
    {
    if (!strnicmp(ligne,groupe,strlen(groupe)))
        {
        ok=1;
        break;
        }
    }
if (ok==0)
    {
    fclose(fic);
    return 1;
    }

nbr=0;
do
    {
    bar[nbr].fct=ftell(fic);

    if (GetLine(ligne,fic)==NULL) break;

    if (ligne[0]=='[')
        break;

    buf=strchr(ligne,'=');
    if (buf!=NULL)
        {
        if (strchr(ligne,'[')!=NULL)
            strcpy(bar[nbr].titre," ");
            else
            strcpy(bar[nbr].titre,"");

        *buf=0;
        ClearSpace(ligne);
        if (strlen(ligne)>20) ligne[20]=0;
        strcat(bar[nbr].titre,ligne);
        nbr++;
        if (nbr==20) break;
        }
    }
while(1);

if (nbr==0)
    {
    fclose(fic);
    return 1;
    }

x=4;
y=4;
n=0;

do
    {
    retour=PannelMenu(bar,nbr,&n,&x,&y);
    }
while ((retour==1) | (retour==-1));

if (retour==2)
    {
    fseek(fic,bar[n].fct,SEEK_SET);
    GetLine(ligne,fic);
    buf=strchr(ligne,'[');

    if (buf!=NULL)
        {
        strcpy(groupe,buf);
        groupe[strlen(groupe)-1]=0;
        fclose(fic);
        return 0;
        }

    strcpy(filename,Fics->trash);
    Path2Abs(filename,"z.bat");

    outfic=fopen(filename,"wt");
    fprintf(outfic,
          "@REM *-------------------------------------------------*\n");
    fprintf(outfic,
          "@REM * Batch file created by Ketchup Killers Commander *\n");
    fprintf(outfic,
          "@REM * according to your user menu                     *\n");
    fprintf(outfic,
          "@REM *-------------------------------------------------*\n");

    buf=strchr(ligne,'=')+1;        // Il y a tjs un '=' sur cette ligne
    ClearSpace(buf);
    History2Line(buf,buffer);
    fprintf(outfic,"@%s\n",buffer);

    while(GetLine(ligne,fic)!=NULL)
        {
        if ( (ligne[0]=='[') | (strchr(ligne,'=')!=NULL) )
            break;

        History2Line(ligne,buffer);
        fprintf(outfic,"@%s\n",buffer);
        }

    fclose(outfic);
    fclose(fic);

    CommandLine("#%s",filename);
    return 1;
    }


fclose(fic);
return 1;
}

/*--------------------------------------------------------------------*\
|- Menu                                                               -|
\*--------------------------------------------------------------------*/
void Menu(void)
{
char res[256];
int r;

strcpy(res,"[Main]");

do
    {
    r=MenuGroup(res);
    }
while(r==0);

}

/*--------------------------------------------------------------------*\
|- Creation d'un nouveau titre                                        -|
\*--------------------------------------------------------------------*/
void MenuCreat(char *titbuf,char *buf,char *path)
{
static char title[34],section[34],line[80];
static int lng=32;
static int l1;
static int x1=42,y1=2;

int n;
char buffer[256],buf2[256],enter[64];

struct Tmt T[] = {
      { 6, 7,2,NULL,NULL},                                      // le OK
      {27, 7,3,NULL,NULL},                                  // le CANCEL

      {12, 2,1,title, &lng},
      { 2, 2,0,"Title:   ",NULL},
      {12, 3,1,section, &lng},
      { 2, 3,0,"Section: ",NULL},

      { 1, 1,9,&x1,&y1},

      { 1, 5,8,"Change to current directory",&l1}

    };

struct TmtWin F = {-1,5,46,14,"New User Menu"};


strncpy(title,titbuf,32);
strcpy(section,"[Main]");
Line2PrcLine(buf,line);

l1=0;

n=WinTraite(T,8,&F,0);

if (n==27) return;
if (T[n].type==3) return;

strcpy(buffer,"");

memset(enter,32,80);
enter[strlen(title)+3]=0;

if (l1==1)
    {
    sprintf(buf2,"%c:\n",path[0]);
    strcat(buffer,buf2);

    strcat(buffer,enter);
    sprintf(buf2,"cd %s\n",path+2);
    strcat(buffer,buf2);

    strcat(buffer,enter);
    }

sprintf(buf2,"%s\n",line);
strcat(buffer,line);


MenuInsert(section,title,buffer);
}



