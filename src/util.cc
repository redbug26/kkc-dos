/*--------------------------------------------------------------------*\
|- Procedures diverses (outils dos)                                   -|
\*--------------------------------------------------------------------*/
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
//#include <time.h>

#include "kk.h"

#ifdef LINUX
    #include <unistd.h>
#else
    #include <io.h>
    #include <conio.h>
    #include <dos.h>
    #include <bios.h>                                     // Gestion clavier
#endif


/*--------------------------------------------------------------------*\
|- Fonction cach‚e IDF                                                -|
\*--------------------------------------------------------------------*/

void ClearSpace(char *name);    //--- efface les espaces inutiles ------


/*--------------------------------------------------------------------*\
|- Prototype local                                                    -|
\*--------------------------------------------------------------------*/
char *GetLine(char *ligne,FILE *fic);

int PcTeamGroup(char *groupe);
int MenuGroup(char *groupe);
void TeamAffLine(char *titre,char *ligne);
int TeamSelect(struct barmenu *bar);

int MenuPcTeamExist(void);
int UserMenuExist(void);

void PcTeam(void);

/*--------------------------------------------------------------------*\
|- Buffer global                                                      -|
\*--------------------------------------------------------------------*/

static char buffer[256];

/*--------------------------------------------------------------------*\
|- Fonction                                                           -|
\*--------------------------------------------------------------------*/

int WinAttrib(void)
{
#ifdef __WC32__

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


NumHelp(39);
n=WinTraite(T,34,&F,0);

if (n!=-1)  // pas escape
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
                DFen->F[n]->attrib=(char)o;

                _dos_open(buffer,O_RDONLY,&hand2);
                _dos_getftime(hand2,&datep,&timep);

                if (*Day!=0)
                    {
                    sscanf(Day,"%d",&m);
                    datep=(ushort)((datep&65504)+m);
                    }
                if (*Month!=0)
                    {
                    sscanf(Month,"%d",&m);
                    datep=(ushort)((datep&65055)+(m<<5));
                    }
                if (*Year!=0)
                    {
                    sscanf(Year,"%d",&m);
                    datep=(ushort)((datep&511)+((m-1980)<<9));
                    }
                if (*Hour!=0)
                    {
                    sscanf(Hour,"%d",&m);
                    timep=(ushort)((timep&2047)+(m<<11));
                    }
                if (*Minute!=0)
                    {
                    sscanf(Minute,"%d",&m);
                    timep=(ushort)((timep&63519)+(m<<5));
                    }
                if (*Second!=0)
                    {
                    sscanf(Second,"%d",&m);
                    timep=(ushort)((timep&65504)+(m/2));
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

#endif

return 1;                                                      // Erreur
}

#ifndef NOCOM
void ServerMode(void)
{
union REGS regs;
char buf[32];
int n;
char ok=0;

SaveScreen();

Cadre(19,4,61,11,0,Cfg->col[55],Cfg->col[56]);
Window(20,5,60,10,Cfg->col[16]);

PrintAt(35,5,"Server Mode");

com_open(Cfg->comport,Cfg->comspeed,Cfg->combit,
                                           Cfg->comparity,Cfg->comstop);
sprintf(buf,"ATS0=0");
for(n=0;n<strlen(buf);n++)
    com_send_ch(buf[n]);
com_close();


do
    {
    regs.w.dx=(short)(Cfg->comport-1);
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
        clock_k cl;

        sprintf(buf,"\x1b[6n*");
        for(n=0;n<strlen(buf);n++)   com_send_ch(buf[n]);

        cl=GetClock();
        while((GetClock()-cl)<60)
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
        DFen->ChangeLine=1;

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

while (kbhit()) Wait(0,0);
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

outfic=fopen(KKFics->temp,"wt");
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
rename(KKFics->temp,KKFics->menu);

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

int UserMenuExist(void)
{
FILE *fic;

fic=fopen(KKFics->menu,"rt");
if (fic==NULL) return 0;
fclose(fic);

return 1;
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
char titre[20][25];
MENU menu;

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
            strcpy(titre[nbr]," ");
            else
            strcpy(titre[nbr],"");

        *buf=0;
        ClearSpace(ligne);
        if (strlen(ligne)>20) ligne[20]=0;
        strcat(titre[nbr],ligne);
        bar[nbr].Titre=titre[nbr];
        bar[nbr].Help=0;
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

menu.attr=8;

menu.x=4;
menu.y=4;
menu.cur=0;

if (PannelMenu(bar,nbr,&menu)==2)
    {
    fseek(fic,bar[menu.cur].fct,SEEK_SET);
    GetLine(ligne,fic);
    buf=strchr(ligne,'[');

    if (buf!=NULL)
        {
        strcpy(groupe,buf);
        groupe[strlen(groupe)-1]=0;
        fclose(fic);
        return 0;
        }

    strcpy(filename,KKFics->trash);
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
static struct barmenu bar[6];
MENU menu;
int nbr=0;

if (MenuPcTeamExist())
    {
    bar[nbr].Titre="CD team Menu";
    bar[nbr].Help=500;
    bar[nbr].fct=2;
    nbr++;
    }

if (UserMenuExist())
    {
    bar[nbr].Titre="User Menu";
    bar[nbr].Help=79;
    bar[nbr].fct=1;
    nbr++;
    }

menu.attr=8;

menu.x=4;
menu.y=4;
menu.cur=0;

if (nbr==0) return;

if (nbr>1)
    if (PannelMenu(bar,nbr,&menu)!=2)
        return;

switch(bar[menu.cur].fct)
    {
    case 1:
        strcpy(res,"[Main]");
        do
            r=MenuGroup(res);
        while(r==0);
        break;
    case 2:
        PcTeam();
        break;
    }
}

/*--------------------------------------------------------------------*\
|- Creation d'un nouveau titre                                        -|
\*--------------------------------------------------------------------*/
void MenuCreat(char *titbuf,char *buf,char *path)
{
static char title[34],section[34],line[80];
static int lng=32;
static int l1;
static char x1=42;
static int y1=2;

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

if (!strncmp(line,"!.!",3))
    {
    int n;

    strcpy(buffer,line);
    strcpy(line,buf);
    for(n=0;n<strlen(line);n++)
        if (line[n]==32) break;
    strcpy(line+n+1,buffer+4);
    }


l1=0;

NumHelp(503);
n=WinTraite(T,8,&F,0);

if (n==-1) return;
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

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/
static FILE *teamfic;

/*--------------------------------------------------------------------*\
|-                                                                    -|
\*--------------------------------------------------------------------*/
char *TeamGetLine(char *res,FILE *fic)
{
int n;
char *a;
a=fgets(res,1024,fic);

if (a!=NULL)
    {
    n=strlen(res)-1;
    while ( ( (res[n]==0x0D) | (res[n]==0x0A) ) & (n>=0) ) res[n]=0,n--;
    }

return a;
}

void TeamAffLine(char *titre,char *ligne)
{
int n,x,y,mx,x1,y1;

SaveScreen();

mx=0;
x=0;
y=0;

for(n=0;n<strlen(ligne);n++)
    {
    char aff=1;

    switch(ligne[n])
        {
        case ' ': if (x>30) aff=0,x=0,y++; break;
        }
    if ((aff) & (ligne[n]>=32))
        x++;
    if (x>mx) mx=x;
    }

x1=(Cfg->TailleX-mx)/2;

y1=(Cfg->TailleY-y)/2;

if (x<strlen(titre)) x=strlen(titre);

Cadre(x1-1,y1-1,x1+mx,y1+y+2,3,Cfg->col[46],Cfg->col[47]);
Window(x1,y1,x1+mx-1,y1+y+1,Cfg->col[28]);

PrintAt(x1,y1,"%s",titre);
ColLin(x1,y1,x,Cfg->col[29]);

x=0;
y=1;

for(n=0;n<strlen(ligne);n++)
    {
    char aff=1;

    switch(ligne[n])
        {
        case ' ': if (x>30) aff=0,x=0,y++; break;
        }
    if ((aff) & (ligne[n]>=32))
        {
        PrintAt(x1+x,y1+y,"%c",ligne[n]);
        x++;
        }
    }

while(!KbHit());

LoadScreen();
}



int TeamFct(struct barmenu *bar)
{
static char ligne[1024];
int n,x,y,mx,x1,y1;

SaveScreen();

fseek(teamfic,bar->fct,SEEK_SET);
TeamGetLine(ligne,teamfic);
TeamGetLine(ligne,teamfic);

mx=0;
x=0;
y=0;


for(n=0;n<strlen(ligne);n++)
    {
    char aff=1;

    switch(ligne[n])
        {
        case '$': aff=0,x=0,y++; break;
        case '*': ligne[n]=','; break;
        case ' ': if (x>30) aff=0,x=0,y++; break;
        }
    if ((aff) & (ligne[n]>=32))
        x++;
    if (x>mx) mx=x;
    }

x1=Cfg->TailleX-mx-1;

y1=4;

if (x<strlen(bar->Titre)) x=strlen(bar->Titre);

Cadre(x1-1,y1-1,x1+mx,y1+y+2,3,Cfg->col[46],Cfg->col[47]);
Window(x1,y1,x1+mx-1,y1+y+1,Cfg->col[28]);

PrintAt(x1,y1,"%s",bar->Titre);
ColLin(x1,y1,x,Cfg->col[29]);

x=0;
y=1;

for(n=0;n<strlen(ligne);n++)
    {
    char aff=1;

    switch(ligne[n])
        {
        case '$': aff=0,x=0,y++; break;
        case '*': ligne[n]=','; break;
        case ' ': if (x>30) aff=0,x=0,y++; break;
        }
    if ((aff) & (ligne[n]>=32))
        {
        PrintAt(x1+x,y1+y,"%c",ligne[n]);
        x++;
        }
    }

while(!KbHit());

LoadScreen();

return 0;
}

/* kf1: 1-> path+fichier
        2-> info … afficher

   kf2: 0-> run
        1-> installation

   kf3: 0-> ?
        1-> dos
        2-> win95
        3-> win3x
*/

int TeamSelect(struct barmenu *bar)
{
int retour=0;
static char ligne[1024],buf[256];
int i,kf1,kf2,kf3,len,ok;

fseek(teamfic,bar->fct,SEEK_SET);
TeamGetLine(ligne,teamfic);

kf1=0;
if (!strnicmp(ligne,"!AVI! !CDROM!",13)) kf1=1,kf2=0,kf3=0,len=13;
if (!strnicmp(ligne,"!CDQUIT!",8))       kf1=1,kf2=0,kf3=0,len=8;
if (!strnicmp(ligne,"!CDROM!",7))        kf1=1,kf2=0,kf3=0,len=7;
if (!strnicmp(ligne,"!DOS! !CDROM!",13)) kf1=1,kf2=0,kf3=1,len=13;
if (!strnicmp(ligne,"!DOSe!",6))         kf1=2,kf2=0,kf3=1,len=6;
if (!strnicmp(ligne,"!DOSi!",6))         kf1=2,kf2=0,kf3=1,len=6;
if (!strnicmp(ligne,"!HLPe!",6))         kf1=1,kf2=0,kf3=0,len=6;
if (!strnicmp(ligne,"!NOT!",5))          kf1=2,kf2=0,kf3=0,len=5;
if (!strnicmp(ligne,"!NOTw!",6))         kf1=2,kf2=0,kf3=0,len=6;
if (!strnicmp(ligne,"!Q95i!",6))         kf1=1,kf2=1,kf3=2,len=6;
if (!strnicmp(ligne,"!QINi!",6))         kf1=1,kf2=1,kf3=3,len=6;
if (!strnicmp(ligne,"!QTW! !CDROM!",13)) kf1=1,kf2=0,kf3=0,len=13;
if (!strnicmp(ligne,"!TXTe!",6))         kf1=1,kf2=0,kf3=0,len=6;
if (!strnicmp(ligne,"!W3xi!",6))         kf1=1,kf2=1,kf3=3,len=6;
if (!strnicmp(ligne,"!W95e!",6))         kf1=1,kf2=0,kf3=2,len=6;
if (!strnicmp(ligne,"!W95i!",6))         kf1=1,kf2=1,kf3=2,len=6;
if (!strnicmp(ligne,"!WINe!",6))         kf1=1,kf2=0,kf3=3,len=6;
if (!strnicmp(ligne,"!WINi!",6))         kf1=1,kf2=1,kf3=3,len=6;
if (!strnicmp(ligne,"!WRIe!",6))         kf1=1,kf2=0,kf3=0,len=6;

if (kf1==1)
    {
    strcpy(buf,DFen->path);
    Path2Abs(buf,ligne+len);
    Path2Abs(buf,"..");
    CommandLine("#CD %s",buf);
    FileinPath(ligne+len,buf);
    ok=-1;
    len=strlen(buf);

    for(i=0;i<DFen->nbrfic;i++)
        {
        buf[len]=0;
        if (!WildCmp(DFen->F[i]->name,buf))
            {   ok=i;  break; }
        strcpy(buf+len,".exe");
        if (!WildCmp(DFen->F[i]->name,buf))
            {   ok=i;  break; }
        strcpy(buf+len,".com");
        if (!WildCmp(DFen->F[i]->name,buf))
            {   ok=i;  break; }
        strcpy(buf+len,".bat");
        if (!WildCmp(DFen->F[i]->name,buf))
            {   ok=i;  break; }
        }
    if (ok!=-1)
        {
        DFen->pcur=i;
        DFen->scur=(DFen->yl)/2;              // Centrage du nom
        if ((kf3>=2) & (KKCfg->_Win95!=1))
            WinError("This program cannot be run in DOS mode");
            else
            {
            retour=1;
            strcpy(KKCfg->FileName,DFen->F[DFen->pcur]->name);
            PutLIFOFct(84);
            }
        }

    }

if (kf1==2)
    {
    TeamAffLine(bar->Titre,ligne+len+1);
    Wait(0,0);
    }

if (kf1==0)
    PrintAt(0,0,"%s",ligne);


return retour;
}


/*--------------------------------------------------------------------*\
|-  Result:    1 --> fin                                              -|
\*--------------------------------------------------------------------*/

int PcTeamGroup(char *groupe)
{
static char ligne[1024];
int retour=0;
int nbr;
static struct barmenu bar[100];
MENU menu;

teamfic=fopen(groupe,"rt");
if (teamfic==NULL) return 1;

//--- Recherche le menu "groupe" ---------------------------------------

nbr=0;
do
    {
    if (TeamGetLine(ligne,teamfic)==NULL) break;
    if (ligne[0]!='@') break;

    bar[nbr].Titre=(char*)GetMem(strlen(ligne));
    memcpy(bar[nbr].Titre,ligne+1,strlen(ligne)+1); // On passe le '@' -

    bar[nbr].Help=500;
    bar[nbr].fct=ftell(teamfic);

    TeamGetLine(ligne,teamfic);
    TeamGetLine(ligne,teamfic);

    nbr++;
    if (nbr==100) break;
    }
while(1);

if (nbr==0)
    {
    fclose(teamfic);
    return 1;
    }

menu.attr=2+8;

menu.x=4;
menu.y=4;
menu.cur=0;

do
    {
    NewEvents(TeamFct,"PcTeam",1);

    if (PannelMenu(bar,nbr,&menu)!=2)
        break;

    if (TeamSelect(&(bar[menu.cur])))
        {
        retour=1;
        break;
        }
    }
while(1);

ClearEvents();

fclose(teamfic);

return retour;
}

/*--------------------------------------------------------------------*\
|- Menu                                                               -|
\*--------------------------------------------------------------------*/
int MenuPcTeamExist(void)
{
char res[256];

strcpy(res,DFen->path);
Path2Abs(res,"\\pc__team\\text_00.txt");
teamfic=fopen(res,"rb");
if (teamfic==NULL) return 0;
fclose(teamfic);
return 1;
}

void PcTeam(void)
{
int drive;
MENU menu;
static struct barmenu bar[6];
char res[256],name[12],volume[32];
static int n=0;

strcpy(res,DFen->path);
Path2Abs(res,"\\pc__team\\text_00.txt");
teamfic=fopen(res,"rb");
if (teamfic==NULL) return;
fclose(teamfic);

drive=toupper(DFen->path[0])-'A';
DriveInfo((char)drive,volume);

bar[0].Titre="Demos ludiques"; bar[0].Help=500;
bar[1].Titre="Demos utilitaires"; bar[1].Help=500;
bar[2].Titre="Jeux sharewares"; bar[2].Help=500;
bar[3].Titre="Utils shareware"; bar[3].Help=500;
bar[4].Titre="Demos, images et sons"; bar[4].Help=500;
bar[5].Titre="Divers"; bar[5].Help=500;

if (!strnicmp(volume,"LUDICD",6))
    {
    bar[0].Titre="Jeux commerciaux";
    bar[1].Titre="Ludo-educatifs";
    bar[2].Titre="Shareware";
    bar[3].Titre="Trucs et astuces";
    bar[4].Titre="Add-on et scenarios";
    bar[5].Titre="Divers";
    }

if (!strnicmp(volume,"CDPRO",5))
    {
    bar[0].Titre="Outils commerciaux";
    bar[1].Titre="Shareware";
    bar[2].Titre="Creations";
    bar[3].Titre="Toolbox";
    bar[4].Titre="Hardware";
    bar[5].Titre="Divers";
    }

bar[0].fct=1;
bar[1].fct=2;
bar[2].fct=3;
bar[3].fct=4;
bar[4].fct=5;
bar[5].fct=6;

menu.x=4;
menu.y=4;
menu.attr=8;

menu.cur=n;

do
{
if (PannelMenu(bar,6,&menu)!=2)
    break;

strcpy(res,DFen->path);
Path2Abs(res,"\\pc__team");
sprintf(name,"text_0%d.txt",menu.cur);
Path2Abs(res,name);

if (PcTeamGroup(res))
    break;
}
while(1);

n=menu.cur;
}


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

#define MAXLCD 100

void ExecLCD(FENETRE *Fen,char *dir)
{
MENU menu;
static struct barmenu *bar;
int nbrbar=0;

FILE *fic;
static char rep[256],rep2[256],name[13];
char a[3],err;
short int nbr;
int m,n,l;

err=0;

bar=(struct barmenu*)GetMem(sizeof(struct barmenu)*MAXLCD);

strcpy(rep,Fen->path);
Path2Abs(rep,"\\treeinfo.ncd");

strcpy(rep2,Fen->path);
Path2Abs(rep2,dir);

do
{
fic=fopen(rep,"rb");
if (fic==NULL)
    if (MakeNCD()) return;

}while(fic==NULL);

fseek(fic,5,SEEK_SET);

fread(&nbr,1,2,fic);

fseek(fic,2,SEEK_CUR);

for(n=0;n<nbr;n++)
    {
    fread(name,1,13,fic);
    fread(a,1,3,fic);

    l=0;
    for(m=0;m<strlen(rep);m++)
        {
        if (rep[m]=='\\')
            {
            l++;
            if (l==a[0]) rep[m]=0;
            }
        }
    Path2Abs(rep,name);

    if ((!WildCmp(name,dir)) | (!WildCmp(rep,rep2)))
        {
        bar[nbrbar].Titre=(char*)GetMem(strlen(rep)+1);
        strcpy(bar[nbrbar].Titre,rep);
        bar[nbrbar].Help=0;
        bar[nbrbar].fct=nbrbar+1;
        nbrbar++;
        if (nbrbar==MAXLCD) break;
        }
    }
fclose(fic);

menu.x=2;
menu.y=2;

menu.attr=2+8;

menu.cur=0;

if (nbrbar==0)
    err=1;

if (nbrbar>1)
    if (PannelMenu(bar,nbrbar,&menu)!=2)
        err=1;

if (!err)
    CommandLine("#cd %s",bar[menu.cur].Titre);


for(n=0;n<nbrbar;n++)
    LibMem(bar[n].Titre);
LibMem(bar);
}


// Renvoit 0 si tout va bien

int MakeNCD(void)
{
char prem;
struct file *ff;
static char **TabRec,*ARec;
int NbrRec;
int n,m;
static char rep[256],moi[256],nom[256];
char name[13];
FENETRE *SFen,*OldFen;
static char volname[256];
FILE *fic;
int pos;

short int nbr,crc;

strcpy(volname,DFen->path);
Path2Abs(volname,"\\");

strcpy(rep,DFen->path);
Path2Abs(rep,"\\treeinfo.ncd");

fic=fopen(rep,"wb");
if (fic==NULL) return 1;

crc=0;
nbr=0;

fwrite("PNCI\0",5,1,fic);
fwrite(&nbr,2,1,fic);
fwrite(&nbr,2,1,fic);



OldFen=DFen;

SFen=(FENETRE*)GetMem(sizeof(FENETRE));
SFen->F=(struct file**)GetMem(TOTFIC*sizeof(void *));

SFen->x=40;
SFen->nfen=7;
SFen->FenTyp=0;
SFen->Fen2=SFen;
SFen->y=1;
SFen->yl=(Cfg->TailleY)-4;
SFen->xl=39;
SFen->order=0;
SFen->pcur=0;
SFen->scur=0;

DFen=SFen;

TabRec=(char**)GetMem(500*sizeof(char*));
ARec=(char*)GetMem(500*3);

TabRec[0]=(char*)GetMem(strlen(volname)+1);
memcpy(TabRec[0],volname,strlen(volname)+1);
NbrRec=1;

do
{
NbrRec--;

strcpy(nom,TabRec[0]);
LibMem(TabRec[0]);

CommandLine("#cd %s",nom);

nbr++;

ARec[0]=0;
for(n=0;n<strlen(nom);n++)
    if (nom[n]=='\\') ARec[0]++;

memset(name,0,13);
FileinPath(nom,name);

if (strlen(name)==0)
    {
    ARec[0]=0;
    ARec[1]=1;
    ARec[2]=0;

    memset(name,0,13);

    strcpy(name,"\\");
    }

fwrite(name,13,1,fic);
fwrite(ARec,3,1,fic);

for(n=0;n<13;n++)
    crc+=name[n];
for(n=0;n<3;n++)
    crc+=ARec[n];

for(n=0;n<NbrRec;n++)
    {
    TabRec[n]=TabRec[n+1];
    ARec[n*3]=ARec[(n+1)*3];
    ARec[n*3+1]=ARec[(n+1)*3+1];
    ARec[n*3+2]=ARec[(n+1)*3+2];
    }

prem=1;
pos=0;
for (m=0;m<DFen->nbrfic;m++)
    {
    ff=DFen->F[m];

    if (ff->name[0]!='.')
        {
        if (IsDir(ff))
            {
            int l;

            for(n=NbrRec;n>pos;n--)
                {
                TabRec[n]=TabRec[n-1];
                ARec[n*3]=ARec[(n-1)*3];
                ARec[n*3+1]=ARec[(n-1)*3+1];
                ARec[n*3+2]=ARec[(n-1)*3+2];
                }

            strcpy(moi,nom);
            Path2Abs(moi,ff->name);

            l=strlen(moi)+1;

            TabRec[pos]=(char*)GetMem(l);
            memcpy(TabRec[pos],moi,l);

            ARec[pos*3+1]=prem;
            ARec[pos*3+2]=1;

            prem=0;

            NbrRec++;
            pos++;
            }
        }
    }
if (pos!=0)
    ARec[(pos-1)*3+2]=0;
}
while (NbrRec!=0);

LibMem(ARec);
LibMem(TabRec);

LibMem(SFen->F);
LibMem(SFen);

DFen=OldFen;

// le crc est la somme de tous les names[13] et des a[3]
fwrite(&crc,2,1,fic);

fseek(fic,5,SEEK_SET);
fwrite(&nbr,2,1,fic);

nbr=(ushort)(((nbr&0xFF00)/256)+0x12A+(nbr&0xFF));
fwrite(&nbr,2,1,fic);

fclose(fic);

return 0;
}

