/*--------------------------------------------------------------------*\
|-                             Viewer                                 -|
\*--------------------------------------------------------------------*/
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include <dos.h>
#include <fcntl.h>
#include <time.h>
#include <bios.h>

#include "kk.h"
#include "idf.h"

void (*AnsiAffChr)(long x,long y,long c);
void (*AnsiAffCol)(long x,long y,long c);


static char ReadChar(void);

void RefreshBar(char *bar);

void ChangeTrad(void);
void ChangeMask(void);
int ChangeLnFeed(void);
void Masque(short x1,short y1,short x2,short y2);

void SavePosition(char *fichier,int posn);
int LoadPosition(char *fichier);


int HexaView(char *fichier);
int TxtView(char *fichier);
int HtmlView(char *fichier,char *liaison);


int TxtDown(int xl);
int TxtUp(int xl);


static char *Keyboard_Flag1=(char*)0x417;


static char xor;

static FILE *fic;
static long taille;
static char view_buffer[32768];
static long pos;        //--- position de depart -----------------------
static long posl;       //--- taille du buffer -------------------------
static long posn;       //--- octet courant ----------------------------

static char srcch[132];

/*--------------------------------------------------------------------*\
|-  Lit l'octet se trouvant en posn                                   -|
\*--------------------------------------------------------------------*/

char ReadChar(void)
{
if (posn>taille)
    {
    posn=taille-1;
    return 0;
    }

if (posn<0)
    posn=0;

if (posn-pos>=posl)
    {
    while (posn-pos>=posl) pos+=32768;

    fseek(fic,pos,SEEK_SET);
    fread(view_buffer,32768,1,fic);
    }

if (posn-pos<0)
    {
    while (posn-pos<0) pos-=32768;

    fseek(fic,pos,SEEK_SET);
    fread(view_buffer,32768,1,fic);
    }

return xor^view_buffer[posn-pos];
}

char ReadNextChar(void)
{
char car;
long l;

l=posn;
posn++;
car=ReadChar();
posn=l;

return xor^car;
}


/*--------------------------------------------------------------------*\
|- Load position of viewing in posdeb                                 -|
\*--------------------------------------------------------------------*/
int LoadPosition(char *fichier)
{
long posdeb=0;
FILE *fic;
unsigned char n;
long s,size;
static char ficname[256];

if (KKCfg->saveviewpos==1)
    {
    strcpy(ficname,KKFics->trash);
    Path2Abs(ficname,"kkview.rb");
    fic=fopen(ficname,"rb");
    if (fic!=NULL)
        {
        while(fread(&n,1,1,fic)==1)
            {
            fread(ficname,1,n,fic);
            fread(&size,4,1,fic);
            fread(&s,4,1,fic);
            if ( (!stricmp(ficname,fichier)) & (size==taille) )
                {
                posdeb=s;
                break;
                }
            }
        fclose(fic);
        }
    }
return posdeb;
}

void SavePosition(char *fichier,int posn)
{
long pos;
FILE *fic;
unsigned char n;
long s,size;
static char ficname[256];

if (KKCfg->saveviewpos==1)
    {
    strcpy(ficname,KKFics->trash);
    Path2Abs(ficname,"kkview.rb");
    fic=fopen(ficname,"r+b");
    if (fic==NULL)
        fic=fopen(ficname,"w+b");
    if (fic!=NULL)
        {
        fseek(fic,0,SEEK_SET);
        while(fread(&n,1,1,fic)==1)
            {
            fread(ficname,n,1,fic);
            if (!stricmp(ficname,fichier))
                {
                pos=ftell(fic);
                fseek(fic,pos,SEEK_SET);// Parce qu'on ‚crit juste aprŠs

                fwrite(&taille,4,1,fic);
                fwrite(&posn,4,1,fic);
                posn=0;
                break;
                }
            fread(&size,1,4,fic);
            fread(&s,1,4,fic);
            }
        if (posn!=0)
            {
            pos=ftell(fic);
            fseek(fic,pos,SEEK_SET);    // Parce qu'on ‚crit juste aprŠs

            n=strlen(fichier)+1;
            fwrite(&n,1,1,fic);
            fwrite(fichier,n,1,fic);
            fwrite(&taille,4,1,fic);
            fwrite(&posn,4,1,fic);
            }
        fclose(fic);
        }
    }
}

/*--------------------------------------------------------------------*\
|- Affichage en Hexadecimal                                           -|
\*--------------------------------------------------------------------*/

int HexaView(char *fichier)
{
int x,y;
long cur1,cur2;
long posd;
int car;

int fin=0; //--- Code de retour ----------------------------------------

SaveScreen();
PutCur(3,0);

Bar(" Help  ----  ----  Text  ----  ---- Search ----  ----  Quit ");

Window(1,1,Cfg->TailleX-2,(Cfg->TailleY)-3,Cfg->col[16]);
WinCadre(0,3,9,(Cfg->TailleY)-2,2);
WinCadre(10,3,58,(Cfg->TailleY)-2,2);
WinCadre(59,3,76,(Cfg->TailleY)-2,2);
WinCadre(77,3,79,(Cfg->TailleY)-2,2);
WinCadre(0,0,79,2,3);

RemplisVide();

ChrCol(34,4,(Cfg->TailleY)-6,Cfg->Tfont);

PrintAt(3,1,"View File %s",fichier);

posn=LoadPosition(fichier);

do
{
posd=posn;
     
for (y=0;y<Cfg->TailleY-6;y++)
    {
    PrintAt(1,y+4,"%08X",posn);

    for (x=0;x<16;x++)
        {
        if (posn<taille)
            {
            char a;
            a=ReadChar();
            PrintAt(x*3+11,y+4,"%02X",a);
            AffChr(x+60,y+4,a);
            posn++;
            }
            else
            {
            PrintAt(x*3+11,y+4,"  ");
            AffChr(x+60,y+4,32);
            }
        }
    }

if (taille<1024*1024)
    {
    cur1=(posd)*(Cfg->TailleY-7);
    cur1=cur1/taille+4;

    cur2=(posn)*(Cfg->TailleY-7);
    cur2=cur2/taille+4;
    }
    else
    {
    cur1=(posd/1024)*(Cfg->TailleY-7);
    cur1=cur1/(taille/1024)+4;

    cur2=(posn/1024)*(Cfg->TailleY-7);
    cur2=cur2/(taille/1024)+4;
    }

ChrCol(78,4,cur1-4,32);
ChrCol(78,cur1,cur2-cur1+1,219);
ChrCol(78,cur2+1,(Cfg->TailleY-3)-cur2,32);

posn=posd;

car=Wait(0,0);

if (car==0)     //--- Pression bouton souris ---------------------------
    {
    int button;

    button=MouseButton();

    if ((button&1)==1)     //--- gauche --------------------------------
        {
        int x,y;

        x=MousePosX();
        y=MousePosY();


        if ( (x==78) & (y>=4) & (y<=Cfg->TailleY-3) ) //-- Ascensceur --
            {
            posn=(taille*(y-4))/(Cfg->TailleY-7);
            }
            else
            if (y==Cfg->TailleY-1)
                if (Cfg->TailleX==90)
                    car=(0x3B+(x/9))*256;
                    else
                    car=(0x3B+(x/8))*256;
                else
                if (y>(Cfg->TailleY)/2)
                    car=80*256;
                    else
                    car=72*256;
        }

    if ((button&2)==2)     //--- droite ---------------------------
        {
        car=27;
        }
    }

switch(LO(car))   {
    case 0:
        switch(HI(car))   {
            case 80:    // --- BAS -------------------------------------
                posn+=16;
                break;
            case 72:    // --- HAUT ------------------------------------
                posn-=16;
                break;
            case 0x51:  // --- PGDN ------------------------------------
                posn+=480;
                break;
            case 0x49:  // --- PGUP ------------------------------------
                posn-=480;
                break;
            case 0x47:  // --- HOME ------------------------------------
                posn=0;
                break;
            case 0x4F:  // --- END -------------------------------------
                posn=taille-((((Cfg->TailleY)-6)*16))+15;
                break;
            case 0x41:  // --- F7 --------------------------------------
                SearchHexa();
                break;
            case 0x43:  // --- F9 --------------------------------------
                fin=86;
                break;
            case 0x44:  // --- F10 -------------------------------------
            case 0x8D:  // --- CTRL-UP ---------------------------------
                fin=-1;
                break;
            case 0x3B:  // --- F1 --------------------------------------
                HelpTopic("View");
                break;
            case 0x3E:  // --- F4 --------------------------------------
                fin=91;
                break;
            default:
                break;
            }
        break;
    case 32:
    case 13:
    case 27:
        fin=-1;
        break;
    default:
        break;
    }

while (posn>=taille-((((Cfg->TailleY)-7)*16))) posn-=16;
while (posn<0) posn+=16;

}
while(fin==0);

SavePosition(fichier,posn);

LoadScreen();

return fin;
}

/*--------------------------------------------------------------------*\
|- to print through ansi interpreter:                                 -|
|-   call ansi_out(<string>);                                         -|
\*--------------------------------------------------------------------*/

static char curattr = 7;
static int curx = 0,cury = 0;
static int maxx = 80, maxy = 25;           // size of ansi output window
static int tabspaces = 8;
static int savx,savy,issaved = 0;
static char ansi_terminators[] = "HFABCDnsuJKmp";

static int touche;
static clock_t Cl_Start;

#define MAXARGLEN       128

void set_pos(char *argbuf,int arglen,char cmd)
{
int y,x;
char *p;

if (!*argbuf || !arglen)
    {
    curx=0;
    cury=0;
    }

y=atoi(argbuf) - 1;
p=strchr(argbuf,';');

if ((y>=0) & (p!=NULL))
    {
    x = atoi(p+1) - 1;
    if (x>=0)
        {
        curx=x;
        cury=y;
        }
    }
}

void go_up (char *argbuf,int arglen,char cmd)
{
int x;

x = atoi(argbuf);

do
    {
    if (cury==0)
        break;
    cury--;
    x--;
    }
while(x>0);
}

void go_down (char *argbuf,int arglen,char cmd)
{
int x;

x = atoi(argbuf);

do
    {
    if (cury == maxy - 1)
        break;
    cury++;
    x--;
    }
while(x>0);
}

void go_left (char *argbuf,int arglen,char cmd)
{
int x;

x = atoi(argbuf);

do
    {
    if(curx==0)
        break;
    curx--;
    x--;
    }
while(x>0);
}

void go_right (char *argbuf,int arglen,char cmd)
{
int x;

x = atoi(argbuf);

do
    {
    if (curx == maxx - 1)
        break;
    curx++;
    x--;
    }
while(x>0);
}

void save_pos (char *argbuf,int arglen,char cmd)
{
savx = curx;
savy = cury;
issaved = 1;
}

void restore_pos (char *argbuf,int arglen,char cmd)
{
if(issaved)
    {
    curx = savx;
    cury = savy;
    issaved = 0;
    }
}


void set_colors (char *argbuf,int arglen,char cmd)
{
char *p,*pp;

if (*argbuf && arglen)
    {
    pp = argbuf;
    do
        {
        p = strchr(pp,';');
        if (p && *p)
            {
            *p = 0;
            p++;
            }
        switch (atoi(pp))
            {
            case 0: //--- all attributes off ---------------------------
                curattr = 7;
                break;

            case 1: //--- Bold On --------------------------------------
                  curattr |= 8;
                  break;

            case 2: //--- Dim On ---------------------------------------
                  curattr &= (~8);
                  break;

            case 3: //--- Italic On ------------------------------------
                  break;

            case 4: //--- Underline On ---------------------------------
                  break;

            case 5: //--- Blink On -------------------------------------
                  curattr |= 128;
                  break;

            case 6: //--- Rapid Blink On -------------------------------
                  break;

            case 7: //--- Reverse Video On -----------------------------
                  curattr = 112;
                  break;

            case 8: //--- Invisible On ---------------------------------
                  curattr = 0;
                  break;

            case 30: //--- black fg ------------------------------------
                  curattr &= (~7);
                  break;

            case 31: //--- red fg --------------------------------------
                  curattr &= (~7);
                  curattr |= 4;
                  break;

            case 32: //--- green fg ------------------------------------
                  curattr &= (~7);
                  curattr |= 2;
                  break;

            case 33: //--- yellow fg -----------------------------------
                  curattr &= (~7);
                  curattr |= 6;
                  break;

            case 34: //--- blue fg -------------------------------------
                  curattr &= (~7);
                  curattr |= 1;
                  break;

            case 35: //--- magenta fg ----------------------------------
                  curattr &= (~7);
                  curattr |= 5;
                  break;

            case 36: //--- cyan fg -------------------------------------
                  curattr &= (~7);
                  curattr |= 3;
                  break;

            case 37: //--- white fg ------------------------------------
                  curattr |= 7;
                  break;

            case 40: //--- black bg ------------------------------------
                  curattr &= (~112);
                  break;

            case 41: //--- red bg --------------------------------------
                  curattr &= (~112);
                  curattr |= (4 << 4);
                  break;

            case 42: //--- green bg ------------------------------------
                  curattr &= (~112);
                  curattr |= (2 << 4);
                  break;

            case 43: //--- yellow bg -----------------------------------
                  curattr &= (~112);
                  curattr |= (6 << 4);
                  break;

            case 44: //--- blue bg -------------------------------------
                  curattr &= (~112);
                  curattr |= (1 << 4);
                  break;

            case 45: //--- magenta bg ----------------------------------
                  curattr &= (~112);
                  curattr |= (5 << 4);
                  break;

            case 46: //--- cyan bg -------------------------------------
                  curattr &= (~112);
                  curattr |= (3 << 4);
                  break;

            case 47: //--- white bg ------------------------------------
                  curattr |= 112;
                  break;

            case 48: //--- subscript bg --------------------------------
                  break;

            case 49: //--- superscript bg ------------------------------
                  break;

            default: //--- unsupported ---------------------------------
                  break;
            }
        pp = p;
        }
    while (p);
    }
}



int ansi_out (char b)
{
static int  arglen=0, ansistate=0, x;
static char argbuf[MAXARGLEN] = "";

switch (ansistate)
    {
    case 0:
        switch (b)
            {
            case 27:
                ansistate = 1;
                break;
            case '\r':
                curx = 0;
                break;

            case '\n':
                cury++;
                break;

            case '\t':
                for (x = 0; x < tabspaces; x++)
                    {
                    AnsiAffChr(curx,cury,' ');
                    AnsiAffCol(curx,cury,curattr);
                    curx++;
                    if (curx > maxx - 1)
                        {
                        curx = 0;
                        cury++;
                        }
                    }
                break;

            case '\b':
                if (curx)
                    curx--;
                break;

            case '\07':                  // The beep -------------------
                putchar('\07');
                break;

            default:
                AnsiAffChr(curx,cury,b);
                AnsiAffCol(curx,cury,curattr);
                curx++;
                if (curx > maxx - 1)
                    {
                    curx = 0;
                    cury++;
                    }
                break;
            }
        break;

    case 1:
        if (b == '[')
            {
            ansistate = 2;
            arglen = 0;
            *argbuf = 0;
            break;
            }
        ansistate = 0;
        break;

    case 2:
        if (strchr(ansi_terminators, (int)b))
            {
            switch ((int)b)
                {
                case 'H': //--- set cursor position --------------------
                case 'F':
                    set_pos(argbuf,arglen,b);
                    break;

                case 'A': //--- up -------------------------------------
                    go_up(argbuf,arglen,b);
                    break;

                case 'B': //--- down -----------------------------------
                    go_down(argbuf,arglen,b);
                    break;

                case 'C': //--- right ----------------------------------
                    go_right(argbuf,arglen,b);
                    break;

                case 'D': //--- left -----------------------------------
                    go_left(argbuf,arglen,b);
                    break;

                case 'n': //--- device statusreport pos ----------------
                    break;

                case 's': //--- save pos -------------------------------
                    save_pos(argbuf,arglen,b);
                    break;

                case 'u': //--- restore pos ----------------------------
                    restore_pos(argbuf,arglen,b);
                    break;

                case 'J': //--- clear screen ---------------------------
                   // ChrWin(0,0,maxx - 1,maxy - 1,32);
                   // ColWin(0,0,maxx - 1,maxy - 1,curattr);
                    curx = cury = 0;
                    break;

                case 'K': //--- delete to eol --------------------------
                   // ChrWin(curx,cury,maxx-1,cury,32);
                   // ColWin(curx,cury,maxx-1,cury,curattr);
                    break;

                case 'm': //--- set video attribs ----------------------
                    set_colors(argbuf,arglen,b);
                    break;

                case 'p': //--- keyboard redef -------------------------
                    break;

                default:  //--- unsupported ----------------------------
                    break;
                }
            ansistate = 0;
            arglen = 0;
            *argbuf = 0;
            }
        else
            {
            if (arglen < MAXARGLEN)
                {
                argbuf[arglen] = b;
                argbuf[arglen + 1] = 0;
                arglen++;
                }
            }
        break;

    default:
        WinError("Error of ANSI code");
        break;
    }

return cury;
}



int AnsiView(char *fichier)
{
char *buffer;
FILE *fic;
int n;

fic=fopen(fichier,"rb");

if (fic==NULL)
    {
    //PrintAt(0,0,"Error on file '%s'",fichier);
    WinError("Couldn't open file");
    return -1;
    }

buffer=GetMem(32768);

SaveScreen();

maxx=80;
maxy=Cfg->TailleY;

Window(0,0,Cfg->TailleX-1,(Cfg->TailleY)-2,0*16+7);

TXTMode();

touche=0;

curx=0;
cury=0;

Cl_Start=clock();

do
    {
    n=fread(buffer,1,16384,fic);
    memset(buffer+n,0,32768-n);
//    ansi_out(buffer);
    }
while(n==16384);

Wait(0,0);

ChangeTaille(Cfg->TailleY);

LoadScreen();
free(buffer);
fclose(fic);

return (-1);
}


void RefreshBar(char *bar)
{
switch(KKCfg->warp)      //--- F2 --------------------------------------
    {
    case 0: memcpy(bar+6,"Nowrap",6); break;
    case 1: memcpy(bar+6," Wrap ",6); break;
    case 2: memcpy(bar+6,"WoWrap",6); break;
    }

}

void AutoTrad(void)
{
int i,j;
char car;

int maxtrad,numtrad,trad;

maxtrad=0;
numtrad=0;

for (i=0;i<3;i++)
    {
    trad=0;
    for (j=0;j<((taille<32768) ? taille:32768);j++)
        {
        posn=j;
        car=ReadChar();
        if (car>0xB0)
        if (CnvASCII(i,car)<0xB0)
            trad++;
        }
    if (trad>maxtrad) maxtrad=trad,numtrad=i;
    }
posn=0;
KKCfg->cnvtable=numtrad;
}


/*--------------------------------------------------------------------*\
|- Deplace le texte d'une ligne vers le bas                           -|
|- Renvoit 0 si il faut arreter                                       -|
\*--------------------------------------------------------------------*/
int TxtDown(int xl)
{
int car,m;

m=0;
do
    {
    car=ReadChar();
    posn++;

    if ( ((car==10) & (KKCfg->lnfeed==0)) |
         ((car==13) & (KKCfg->lnfeed==1)) |
         ((car==10) & (KKCfg->lnfeed==2)) |
         ((car==10) & (KKCfg->lnfeed==4)) |
         ((car==KKCfg->userfeed) & (KKCfg->lnfeed==3)))
        {
        return 1;
        }
//
    if (posn==taille)
        {
        posn=taille-1;
        return 0;
        }
//
    if ((car!=10) & (car!=13)) m++;
    if ((m>xl) & (KKCfg->warp!=0))
        {
        posn--;
        return 1;
        }
    }
while(1);
}


/*--------------------------------------------------------------------*\
|- Deplace le texte d'une ligne vers le haut                          -|
|- Renvoit 0 si il faut arreter                                       -|
\*--------------------------------------------------------------------*/
int TxtUp(int xl)
{
int car,m;

if (posn==0) return 0;

m=0;
posn--;
car=ReadChar();
if ((car!=10) & (car!=13)) m++;

if (posn==0) return 0;
do
    {
    posn--;
    if (posn==0)
        {
        if (posn!=0) posn++;
        return 0;
        }
    car=ReadChar();
    if ( ((car==10) & (KKCfg->lnfeed==0)) |
         ((car==13) & (KKCfg->lnfeed==1)) |
         ((car==10) & (KKCfg->lnfeed==2)) |
         ((car==10) & (KKCfg->lnfeed==4)) |
         ((car==KKCfg->userfeed) & (KKCfg->lnfeed==3)))
        {
        if (posn!=0) posn++;
        return 1;
        }
    if ((car!=10) & (car!=13)) m++;
    if ((m>=xl) & (KKCfg->warp!=0)) return 1;
    }        // m>xl-1
while(1);

}

/*--------------------------------------------------------------------*\
|- Affichage de texte                                                 -|
\*--------------------------------------------------------------------*/
int TxtView(char *fichier)
{
int xm,ym,zm;

int aff,wrap;

long posd;
long cur1,cur2;

int xl,yl;
int x,y;
int x2,y2;
int lchaine;

char car;

char chaine[256];

char autowarp=0;        // met a 1 si il faut absolument faire un nowarp

int m;

char affichage[101];

int w1,w2;

int code;
int fin=0;

int warp=0;

int shift=0; // Vaut -1 si reaffichage de l'ecran
             //       0 si pas de shift
             //       1 si touche shiftee

char pasfini;

static char bar[81];

int xl2;
int tpos;

SaveScreen();
PutCur(3,0);

wrap=0;
aff=1;

/*--------------------------------------------------------------------*\
|------------- Recherche de la meilleur traduction --------------------|
\*--------------------------------------------------------------------*/
if (KKCfg->autotrad)
    AutoTrad();

/*--------------------------------------------------------------------*\
|------------- Calcul de la taille maximum ----------------------------|
\*--------------------------------------------------------------------*/
if (KKCfg->ajustview)
    {
    int xm=0,ym=0;
    int n;

    char vb;
    long posold;

    x=0;
    posold=posn;

    for (n=0;n<((taille<32768) ? taille:32768);n++)
        {
        posn=n;
        vb=ReadChar();

        if ( ((vb==13) & (ReadNextChar()==10) & (KKCfg->lnfeed==0)) |
             ((vb==13) & (KKCfg->lnfeed==1)) |
             ((vb==10) & (KKCfg->lnfeed==2)) |
             ((vb==10) & (KKCfg->lnfeed==4)) |
             ((vb==KKCfg->userfeed) & (KKCfg->lnfeed==3)) )
             {
             x=0;
             ym++;
             if (KKCfg->lnfeed==0) n++;
             }
        else
        switch(view_buffer[n])
            {
            case 9:
                lchaine=x/8;
                lchaine=lchaine*8+8;
                lchaine-=x;
                x+=lchaine;
                break;
            case 32:
                x++;
                break;
            case 13:
                if (KKCfg->lnfeed==4) break;
            default:
                x++;
                if (x>xm) xm=x;
                break;
            }
        if (x>Cfg->TailleX) ym++,x=0;
        }

    posn=posold;
    ym++;

    if (xm>=((Cfg->TailleX)-1))
        xl=Cfg->TailleX;
        else
        autowarp=1, xl=xm;                        //--> Longueur fenˆtre

    if (ym>Cfg->TailleY-1)
        yl=Cfg->TailleY-1;
        else
        yl=ym;
    }
    else
    {
    xl=Cfg->TailleX;
    yl=Cfg->TailleY-1;
    }


/*--------------------------------------------------------------------*\
|- Affichage de la fenetre                                            -|
\*--------------------------------------------------------------------*/

x=(Cfg->TailleX-xl)/2;                                // centre le texte
y=(Cfg->TailleY-yl)/2;

if (x<0) x=0;
if (y<0) y=0;

if ( (x>0) & (y>0) & (x+xl<Cfg->TailleX) & (y+yl<Cfg->TailleY) )
    Cadre(x-1,y-1,x+xl,y+yl,3,Cfg->col[55],Cfg->col[56]);
    else
    {
    if ( (y+yl<Cfg->TailleY) & (y>0) )
        {
        ColLin(0,y-1,Cfg->TailleX,Cfg->col[16]);
        WinLine(0,y-1,Cfg->TailleX,1);
        WinLine(0,y+yl,Cfg->TailleX,1);
        ColLin(0,y+yl,Cfg->TailleX,Cfg->col[16]);
        }
    if ( (x+xl<Cfg->TailleX) & (x>0) )
        {
        ColWin(x-1,0,x-1,Cfg->TailleY-2,Cfg->col[16]);
        ChrWin(x-1,0,x-1,Cfg->TailleY-2,0xB3);
        ColWin(x+xl,0,x+xl,Cfg->TailleY-2,Cfg->col[16]);
        ChrWin(x+xl,0,x+xl,Cfg->TailleY-2,0xB3);
        }
    }

ColWin(x,y,x+xl-1,y+yl-1,Cfg->col[16]);
ChrWin(x,y,x+xl-1,y+yl-1,32);

/*--------------------------------------------------------------------*\
|- Affichage de la bar                                                -|
\*--------------------------------------------------------------------*/

strcpy
   (bar," Help NowrapLnFeed Hexa  Zoom  Trad Search Print Mask  Quit ");



RefreshBar(bar);

Bar(bar);

/*--------------------------------------------------------------------*\
|- Garde la position pr‚c‚dente                                       -|
\*--------------------------------------------------------------------*/

posn=LoadPosition(fichier);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

affichage[xl]=0;

do
{
pasfini=0;

posd=posn;

x2=x;
y2=y;

aff=0;

if (xl==Cfg->TailleX)       //--- Jusqu'ou on ecrit --------------------
    xl2=xl-shift;
    else
    xl2=xl;

/*--------------------------------------------------------------------*\
|- Affichage du texte                                                 -|
\*--------------------------------------------------------------------*/
do
    {
    tpos=x2-x-warp;

    lchaine=1;                                        // Longueur chaine

    chaine[0]=ReadChar();

    if ( ((chaine[0]==13) & (ReadNextChar()==10) & (KKCfg->lnfeed==0)) |
         ((chaine[0]==13) & (KKCfg->lnfeed==1)) |
         ((chaine[0]==10) & (KKCfg->lnfeed==2)) |
         ((chaine[0]==10) & (KKCfg->lnfeed==4)) |
         ((chaine[0]==KKCfg->userfeed) & (KKCfg->lnfeed==3)) )
        {
        if (KKCfg->lnfeed==0) posn++;
        w1=tpos;
        aff=1;
        }
        else
        switch(chaine[0])
            {
            case 9:
                lchaine=(x2-x)/8;
                lchaine=lchaine*8+8;
                lchaine-=(x2-x);
                if (lchaine>0)
                    memset(chaine,32,lchaine);
                break;
            case 13:
                if (KKCfg->lnfeed==4)
                    {
                    lchaine=0;
                    chaine[0]=0;
                    }
                break;
            default:
                chaine[0]=CnvASCII(KKCfg->cnvtable,chaine[0]);
                break;
            }

    for(m=0;m<lchaine;m++)
        {
        car=chaine[m];

        if ( (tpos<xl+10) & (tpos>=0) )
            affichage[tpos]=car;

        tpos++;
        x2++;
        }

    tpos=x2-x-warp-1;

    if ( (tpos>=xl2) & (aff==0) )
                                    // Si le prochain d‚passe la fenˆtre
        {
        if (tpos>=xl2)
            {
            if ( (KKCfg->warp==1) & (autowarp==0) )
                {
                aff=2;

                w1=xl2;                             // Premier … retenir
                w2=tpos;                            // Dernier … retenir
                }
            if ( (KKCfg->warp==2) & (autowarp==0) )
                {
                int n;
                aff=2;
                n=xl2;
                while ( (n>0) & (affichage[n]!=32) ) n--;

                w1=n+1;
                w2=tpos;
                }
            }
        }

    if (aff!=0)            // Le monsieur te demande d'afficher la ligne
        {
        if ( (shift!=1) | (y2!=0) )
            {
            if (w1<0)
                {
                for (m=0;m<xl2;m++)
                    AffChr(x+m,y2,32);
                }
                else
            if (w1>=xl2)
                {
                for (m=0;m<xl2;m++)
                    AffChr(x+m,y2,affichage[m]);
                }
                else
                {
                for (m=0;m<w1;m++)
                    AffChr(x+m,y2,affichage[m]);
                for (m=w1;m<xl2;m++)
                     AffChr(x+m,y2,32);
                }
            }

        x2=x;
        y2++;

        if (aff==2)
            {
            memcpy(affichage,affichage+w1,w2-w1+1);
            x2+=(w2-w1+1);
            }

        aff=0;
        }

    if (y2>=y+yl) break;

    posn++;
    if (posn>=taille)
        {
        pasfini=1;
        lchaine=xl-x2+x;

        if (lchaine>0)
            {
            if (yl==Cfg->TailleY-1)
                memset(affichage+x2-x,'-',lchaine);
                else
                memset(affichage+x2-x,' ',lchaine);
            }

        affichage[xl]=0;
        PrintAt(x,y2,"%s",affichage);
        y2++;
        break;
        }
    }
while(1);



while(y2<y+yl)
    {
    memset(affichage,' ',xl);     // remplit une ligne entiere de espace
    affichage[xl]=0;
    PrintAt(x,y2,"%s",affichage);
    y2++;
    }

if (shift==0)
    Masque(x,y,x+xl-1,y+yl-1);
    else                                     // Ca marche mais ca pas b“
    {
    if ( (xl==Cfg->TailleX) & (y==0) )
        Masque(x,y+1,x+xl-2,y+yl-1);
    if ( (xl==Cfg->TailleX) & (y!=0) )
        Masque(x,y,x+xl-2,y+yl-1);
    if ( (xl!=Cfg->TailleX) & (y==0) )
        Masque(x,y+1,x+xl-1,y+yl-1);
    if ( (xl!=Cfg->TailleX) & (y!=0) )
        Masque(x,y,x+xl-1,y+yl-1);
    }


/*--------------------------------------------------------------------*\
|- Gestion des touches                                                -|
\*--------------------------------------------------------------------*/

zm=0;

while ( (!KbHit()) & (zm==0) )
{
GetPosMouse(&xm,&ym,&zm);

car=*Keyboard_Flag1;

if ( ((car&1)==1) | ((car&2)==2) )
    {
    int prc;
    char temp[132];

    if (shift==0)
        SaveScreen();

    if (posn>taille) posn=taille;

    if (taille<1024*1024)
        {
        cur1=(posd)*(Cfg->TailleY-2);
        cur1=cur1/taille+1;

        cur2=(posn-1)*(Cfg->TailleY-2);
        cur2=cur2/taille+1;

        prc=(posn*100)/taille;
        }
        else
        {
        cur1=(posd/1024)*(Cfg->TailleY-2);
        cur1=cur1/(taille/1024)+1;

        cur2=((posn-1)/1024)*(Cfg->TailleY-2);
        cur2=cur2/(taille/1024)+1;

        prc=(posn/taille)*100;
        }

    ColLin(0,0,Cfg->TailleX,Cfg->col[7]);

    strncpy(temp,fichier,78);

    temp[45]=0;

    PrintAt(0,0,
            "View: %-*s Col%3d %9d bytes %3d%% ",Cfg->TailleX-35,
                                                  temp,warp,taille,prc);

    ColCol(Cfg->TailleX-1,1,Cfg->TailleY-2,Cfg->col[7]);
    ChrCol(Cfg->TailleX-1,1,cur1-1,32);
    ChrCol(Cfg->TailleX-1,cur1,cur2-cur1+1,219);
    ChrCol(Cfg->TailleX-1,cur2+1,Cfg->TailleY-1-cur2,32);

    if (shift!=1)
        {
        shift=2;
        break;
        }
    shift=1;
    }
    else
    if (shift==1)
        {
        shift=-1;
        break;
        }

}

posn=posd;

if ( (shift!=-1) & (shift!=2) )
{
code=Wait(0,0);

if (code==0)     //--- Pression bouton souris --------------------------
    {
    int button;

    button=MouseButton();

    if ((button&1)==1)     //--- gauche --------------------------------
        {
        int x,y;

        x=MousePosX();
        y=MousePosY();


        if ( (x==Cfg->TailleX-1) & (y>=1) & (y<=Cfg->TailleY-1) )
                                                      //-- Ascensceur --
            {
            posn=(taille*(y-1))/(Cfg->TailleY-2);
            }
            else
            if (y==Cfg->TailleY-1)
                if (Cfg->TailleX==90)
                    code=(0x3B+(x/9))*256;
                    else
                    code=(0x3B+(x/8))*256;
                else
                {
                if (y>(Cfg->TailleY)/2)
                    code=80*256;
                    else
                    code=72*256;
                ReleaseButton();
                }
        }

    if ((button&2)==2)     //--- droite --------------------------------
        {
        code=27;
        }
    }

switch(LO(code))
    {
    case 0:
       switch(HI(code))   {
            case 0x54:
                Decrypt();
                break;
            case 0x3B:  //--- F1 ---------------------------------------
                HelpTopic("View");
                break;
            case 0x3C:  //--- F2 ---------------------------------------
                if (autowarp==1) break;
                KKCfg->warp++;
                if (KKCfg->warp==3) KKCfg->warp=0;
                RefreshBar(bar);
                Bar(bar);
                break;
            case 0x3D:  //--- F3 ---------------------------------------
                fin=ChangeLnFeed();
                break;
            case 0x3E:  //--- F4 ---------------------------------------
                fin=-2;     //--- N'importe quoi -----------------------
                break;
            case 0x3F:  //--- F5 ---------------------------------------
                KKCfg->ajustview^=1;
                fin=91;
                break;
            case 0x40:  //--- F6 ---------------------------------------
                ChangeTrad();

                if (KKCfg->autotrad)
                    AutoTrad();
                break;
            case 0x41:  //--- F7 ---------------------------------------
                SearchTxt();
                break;
            case 0x42:  //--- F8 ---------------------------------------
                Print(fichier,1);
                break;
            case 0x4D:  //--- RIGHT ------------------------------------
                warp+=8;
                break;
            case 0x4B:  //--- LEFT -------------------------------------
                warp-=8;
                break;
            case 0x74:  //--- CTRL RIGHT -------------------------------
                warp+=40;
                break;
            case 0x73:  //--- CTRL LEFT --------------------------------
                warp-=40;
                break;
            case 0x77:  //--- CTRL HOME --------------------------------
                warp=0;
                break;
            case 0x43:  //--- F9 ---------------------------------------
                ChangeMask();
                ColWin(x,y,x+xl-1,y+yl-1,Cfg->col[16]);
                break;
            case 80:    //--- DOWN -------------------------------------
                if (pasfini==1) break;
                TxtDown(xl);
                break;
            case 72:    //--- UP ---------------------------------------
                TxtUp(xl);
                break;
            case 0x51:    //--- PGDN -----------------------------------
                if (pasfini==1) break;
                for (m=0;m<yl;m++)
                    if (!TxtDown(xl)) break;
                break;
            case 0x49:    //--- PGUP -----------------------------------
                for (m=0;m<yl;m++)
                    if (!TxtUp(xl)) break;
                break;
            case 0x4F:    //--- END ------------------------------------
                posn=taille;
                for (m=0;m<yl;m++)
                    TxtUp(xl);
                break;
            case 0x47:  //--- HOME -------------------------------------
                posn=0;
                break;
            case 0x44:  //--- F10 --------------------------------------
            case 0x8D:  //--- CTRL-UP ----------------------------------
                fin=-1;
                break;
            }
        break;
    case 32:
    case 13:
    case 27:
        fin=-1;
        break;
    case 6:                                                    // CTRL-F
        SaveScreen();
        GestionFct(32);
        LoadScreen();
        break;
    }

if ( (KKCfg->warp!=0) & (autowarp==1) ) warp=0;

if (warp<0) warp=0;
}
else
{
if (shift==-1)
    {
    shift=0;
    LoadScreen();
    }
if (shift==2)
    {
    shift=1;
    }
}
}
while(!fin);

SavePosition(fichier,posn);
LoadScreen();

return fin;
}

/*--------------------------------------------------------------------*\
|-  Recherche une chaine en mode texte                                -|
\*--------------------------------------------------------------------*/

void SearchTxt(void)
{
static char Dir[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[5] =
    { { 2,3,1,Dir,&DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Search for:",NULL},
      { 1,1,4,&CadreLength,NULL} };

struct TmtWin F =
    { -1,10,74,17, "Search Text" };

int a,n;
char c1,c2;

a=posn;

do
    {
    posn++;
    if (posn==taille)
        {
        posn=taille-2;
        break;
        }
    }
    while(ReadChar()!=10);

posn++;

n=WinTraite(T,5,&F,0);

if ( (n!=0) & (n!=1) ) return;

strcpy(srcch,Dir);

n=0;



do
{
if (Dir[n]==0) break;

c1=Dir[n];
if ( (c1>='a') & (c1<='z') ) c1+='A'-'a';

c2=ReadChar();;
if ( (c2>='a') & (c2<='z') ) c2+='A'-'a';

if (c1==c2)
    n++;
    else
    {
    if (n!=0)
        {
        posn-=(n-1);
        n=0;
        }
    }

posn++;
}
while (posn<taille);

if (Dir[n]!=0)
    {
    posn=a;
    WinError("Don't find text");
    return;
    }



if (posn==0) return;
posn--;
if (posn==0) return;
do
    {
    posn--;
    if (posn==0) return;
    }
while(ReadChar()!=10);
if (posn!=0) posn++;
}

/*--------------------------------------------------------------------*\
|-  Recherche une chaine en mode hexa                                 -|
\*--------------------------------------------------------------------*/

void SearchHexa(void)
{
static char Text[70],Hexa[70];
static int DirLength=70;
static char CadreLength=70;

struct Tmt T[8] =
    { { 2,3,1,Hexa,&DirLength},
      { 2,7,1,Text,&DirLength},
      {15,9,2,NULL,NULL},
      {45,9,3,NULL,NULL},
      { 5,6,0,"Search for text:",NULL},
      { 5,2,0,"Search for hexa:",NULL},
      { 1,1,4,&CadreLength,NULL},
      { 1,5,4,&CadreLength,NULL} };

struct TmtWin F =
    {-1,7,74,18, "Search Text/Hexa" };

int a,n,lng;
char c1,c2;
int testhexa=0;

a=posn;

if (strlen(Hexa)!=0)
    posn++;

n=WinTraite(T,8,&F,0);

if ( (n!=0) & (n!=1) ) return;

if (strlen(Hexa)!=0)
    {
    int len=0,te,t,l;

    l=strlen(Hexa);

    for(te=0;te<l;te+=3)
        {
        Hexa[te+2]=0;
        sscanf(Hexa+te,"%02X",&t);
        Hexa[te+2]=32;
        srcch[len]=t;
        len++;
        }
    srcch[len]=0;
    lng=len;
    testhexa=1;
    }
    else
    {
    strcpy(srcch,Text);
    lng=strlen(Text);
    }

n=0;

do
{
if (n>=lng) break;

c1=srcch[n];
if ( (c1>='a') & (c1<='z') & (testhexa==0) ) c1+='A'-'a';

c2=ReadChar();;
if ( (c2>='a') & (c2<='z') & (testhexa==0) ) c2+='A'-'a';

if (c1==c2)
    n++;
    else
    {
    if (n!=0)
        {
        posn-=(n-1);
        n=0;
        }
    }

posn++;
}
while (posn<taille);

if (srcch[n]!=0)
    {
    posn=a;
    WinError("Don't find text");
    return;
    }

posn=posn-lng;
}

struct Href {
    char *link;
    signed int x1,y1;
    signed int x2,y2;
    struct Href *next;
    };



/*--------------------------------------------------------------------*\
|- Affichage d'une page HTML                                          -|
\*--------------------------------------------------------------------*/

int HtmlView(char *fichier,char *liaison)
{
FILE *fic;
char titre[256];
short lentit;
long posd,taille2;

char chaine[256];

char mot[128];  // phrase a ecrire
char motc[128]; // couleur de cette phrase
short smot;     // Taille du mot

long n,taille,lu;

char *buffer;

long debut;     // position du <
long code;      // position du &

char aff;       // vaut 1 si il faut ecrire
char prev;      // vaut 1 si la derniere commande est enter

int yp;    // position virtuelle sur ecran
int xl,yl,ye;
int x,y;

char ahref; // nombre de ref en cours

unsigned char col;

unsigned char tabcol[32];
short nbrcol;

char car;

char psuiv;     // vaut 1 si on passe le suivant

int i,j,k;        // Compteur

signed char bold,ital,unde;
char pre;
char nlist;      // nombre de liste
char listn[16];  // position dans liste
char listt[16];  // type de liste

short xpage=Cfg->TailleX-2;

struct Href *prem,*suiv,*rete;

int ix,iy;

int shift=0; // Vaut -1 si reaffichage de l'ecran
             //       0 si pas de shift
             //       1 si touche shiftee

char *ColTxt;
char *ChrTxt;

RB_IDF Info;


ColTxt=GetMem(40000*Cfg->TailleX);
memset(ColTxt,Cfg->col[16],40000*Cfg->TailleX);
ChrTxt=GetMem(40000*Cfg->TailleX);
memset(ChrTxt,32,40000*Cfg->TailleX);


fic=fopen(fichier,"rb");

if (fic==NULL)
    {
    PrintAt(0,0,"Error on file '%s'",fichier);
    WinError("Couldn't open file");
    return -1;
    }
taille=filelength(fileno(fic));

if (taille==0) return -1;

buffer=GetMem(32768);


SaveScreen();
PutCur(3,0);

x=1;
y=1;

xl=Cfg->TailleX-2;
yl=Cfg->TailleY-2;

WinCadre(x-1,y-1,xl+1,yl,2);

Window(x,y,xl,yl-1,Cfg->col[16]);

ColLin(0,yl+1,Cfg->TailleX,Cfg->col[6]);
ChrLin(0,yl+1,Cfg->TailleX,32);


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

prem=(struct Href*)GetMem(sizeof(struct Href));
suiv=prem;


suiv->next=NULL;

ye=0;   // debut


bold=0;
ital=0;
unde=0;

nbrcol=0;
tabcol[0]=Cfg->col[16];

debut=0;
code=0;

prev=1;

yp=0;

aff=0;

ahref=0;

smot=0;

pre=0;

lu=0;

nlist=0;

lentit=0;

psuiv=0;

for (n=0;n<taille;n++)   {

if (n>=lu) fread(buffer,32768,1,fic),lu+=32768;

car=buffer[n+32768-lu];

if ( (debut==0) & (code==0) )
switch(car)  {
    case '<':
        debut=1;
        car=0;
        break;

    case '&':
        memset(titre,0,128);
        code=1;
        car=0;
        break;

    case 9:
        k=smot/8;
        k=k*8+8;
        k-=smot;

        memset(titre,32,k);
        lentit=k;
        break;

    case ';':
        if (psuiv==1)
            {
            psuiv=0;
            car=0;
            }

    default:
        break;
    }

if (debut!=0)       // Code <...>
switch(car)  {
    case '>':
        if (debut<128)
            {
            titre[debut-1]=0;

            if (!stricmp(titre,"TITLE"))
                             nbrcol++,tabcol[nbrcol]=Cfg->col[35],aff=1;
            if (!strnicmp(titre,"H1",2))
                                   nbrcol++,tabcol[nbrcol]=Cfg->col[36];
            if (!strnicmp(titre,"H2",2))
                                   nbrcol++,tabcol[nbrcol]=Cfg->col[50];
            if (!strnicmp(titre,"H3",2))
                                   nbrcol++,tabcol[nbrcol]=Cfg->col[51];
            if (!strnicmp(titre,"H4",2))
                                   nbrcol++,tabcol[nbrcol]=Cfg->col[57];
            if (!strnicmp(titre,"H5",2))
                                   nbrcol++,tabcol[nbrcol]=Cfg->col[58];
            if (!strnicmp(titre,"H6",2))
                                   nbrcol++,tabcol[nbrcol]=Cfg->col[59];

            if (!stricmp(titre,"STRONG")) bold++;             // GRAS ON
            if (!stricmp(titre,"B"))      bold++;             // GRAS ON
            if (!stricmp(titre,"EM"))     ital++;         // ITALIQUE ON
            if (!stricmp(titre,"I"))     ital++;          // ITALIQUE ON
            if (!stricmp(titre,"U"))      unde++;        // UNDERLINE ON

            if (!strnicmp(titre,"A HREF",6))
                {
                ahref++;
                nbrcol++;
                tabcol[nbrcol]=Cfg->col[17];
                suiv->link=GetMem(strlen(titre)+1);
                memcpy(suiv->link,titre,strlen(titre)+1);

                suiv->x1=smot;
                suiv->y1=yp;
                }

            if (!strnicmp(titre,"A NAME",6))
                {
                titre[strlen(titre)-1]=0;

                if (!stricmp(titre+8,liaison))
                    ye=yp;
                }


            if (!stricmp(titre,"/A"))
                {
                if (ahref!=0)
                    {
                    nbrcol--;
                    ahref--;
                    suiv->next=(struct Href*)
                                            GetMem(sizeof(struct Href));

                    suiv->x2=smot;            // REFERENCE DE 1 DE LARGE
                    suiv->y2=yp;

                    suiv=suiv->next;
                    suiv->next=NULL;
                    }
                }


            if (!stricmp(titre,"/STRONG")) bold--;           // GRAS OFF
            if (!stricmp(titre,"/B"))      bold--;           // GRAS OFF
            if (!stricmp(titre,"/EM"))     ital--;       // ITALIQUE OFF
            if (!stricmp(titre,"/I"))     ital--;        // ITALIQUE OFF
            if (!stricmp(titre,"/U"))      unde--;      // UNDERLINE OFF
                
            if (!stricmp(titre,"/TITLE")) nbrcol--,aff=2;
            if (!stricmp(titre,"/H1")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H2")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H3")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H4")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H5")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H6")) nbrcol--,aff=1;
            if (!stricmp(titre,"LI"))
                {
                if (nlist!=0)
                    {
                    switch (listt[nlist-1])  {
                        case 1:
                            sprintf(titre,"%c%*s%c ",1,nlist*2,"",7);
                            break;
                        case 2:
                            sprintf(titre,"%c%*s%02d) ",1,nlist*2,
                                                     "",listn[nlist-1]);
                            break;
                        }
                    listn[nlist-1]++;
                    lentit=strlen(titre);
                    }
                    else
                    {
                    sprintf(titre,"%c%*s%c ",1,nlist*2,"",7);
                    lentit=strlen(titre);
                    }
                }
//            if (!stricmp(titre,"LI"))
            if (!stricmp(titre,"BR"))  aff=1;           // C‚sure forc‚e
            if (!stricmp(titre,"P")) aff=1;         // fin de paragraphe
            if (!stricmp(titre,"/P")) aff=1;        // fin de paragraphe
            if (!stricmp(titre,"OL"))
                                  listt[nlist]=2,listn[nlist]=1,nlist++;
            if (!stricmp(titre,"/OL"))
                            listt[nlist]=0,listn[nlist]=0,nlist--,aff=1;
            if (!stricmp(titre,"UL"))
                                  listt[nlist]=1,listn[nlist]=1,nlist++;
            if (!stricmp(titre,"/UL"))
                            listt[nlist]=0,listn[nlist]=0,nlist--,aff=1;

            if (!stricmp(titre,"DL")) aff=1;
            if (!stricmp(titre,"DT")) aff=1;
            if (!stricmp(titre,"DD")) aff=1;
            if (!stricmp(titre,"/DL")) aff=1;
            if (!stricmp(titre,"/DT")) aff=1;
            if (!stricmp(titre,"/DD")) aff=1;

            if (!stricmp(titre,"TR")) aff=1;
            if (!strnicmp(titre,"TD",2)) aff=1;

            if (!stricmp(titre,"I"))  aff=0;              // Chasse fixe
            if (!stricmp(titre,"/I")) aff=0;

            if (!stricmp(titre,"PRE")) pre++;
            if (!stricmp(titre,"/PRE")) pre--;

            if (!stricmp(titre,"MENU")) pre++;
            if (!stricmp(titre,"/MENU")) pre--;

            if (!stricmp(titre,"HR"))
                {
                titre[0]=1;
                for(i=1;i<xpage+1;i++)
                    titre[i]=0xC4;
                lentit=xpage+1;
                }
            }

        debut=0;
        car=0;
        break;
    case 0:
        break;
    case '<':
        titre[debut-1]=0;
        sprintf(chaine,"<%s%c",titre,car);
        memcpy(titre,chaine,255);
        lentit=strlen(titre);
        debut=0;
        break;
    default:
        titre[debut-1]=car;
        lentit=0;
        debut++;
        car=0;
        break;
    }

if (code!=0)    // code &...;
    {
    if (psuiv!=0)
        {
        psuiv--;
        car=0;
        }

    switch(car)  {
        case 0:
            break;
        case 32:
            titre[code-1]=0;
            sprintf(chaine,"&%s ",titre);
            memcpy(titre,chaine,255);
            lentit=strlen(titre);
            code=0;
            break;
        case ';':
            code=0;
            car=0;
            break;
        default:
            titre[code-1]=car;

            car=0;
            lentit=0;
            code++;

            if (code>128) code=0;

            if (!strnicmp(titre,"EGRAVE",6)) psuiv=1,car='Š';
            if (!strnicmp(titre,"EACUTE",6)) psuiv=1,car='‚';

            if (!strnicmp(titre,"AGRAVE",6)) psuiv=1,car='…';
            if (!strnicmp(titre,"AACUTE",6)) psuiv=1,car=' ';

            if (!strnicmp(titre,"IGRAVE",6)) psuiv=1,car='i';
            if (!strnicmp(titre,"IACUTE",6)) psuiv=1,car='i';

            if (!strnicmp(titre,"CCEDIL",6)) psuiv=1,car='‡';

            if (!strnicmp(titre,"ECIRC",5)) psuiv=1,car='ˆ';
            if (!strnicmp(titre,"ACIRC",5)) psuiv=1,car='ƒ';
            if (!strnicmp(titre,"ICIRC",5)) psuiv=1,car='i';

            if (!strnicmp(titre,"QUOT",4)) psuiv=1,car=34;

            if (!strnicmp(titre,"NBSP",4)) psuiv=1,car=32;
            if (!strnicmp(titre,"IUML",4)) psuiv=1,car='i';
                                                   // En attendant mieux
            if (!strnicmp(titre,"COPY",4))
                 {
                 psuiv=1;
                 lentit=3;
                 strcpy(titre,"(C)");
                 }

            if (!strnicmp(titre,"AMP",3)) psuiv=1,car='&';

            if (!strnicmp(titre,"#146",4)) psuiv=1,car=39;  /* ' */
            if (!strnicmp(titre,"#171",4)) psuiv=1,car=34;  /* << */
            if (!strnicmp(titre,"#187",4)) psuiv=1,car=34;  /* >> */

            if (!strnicmp(titre,"LT",2)) psuiv=1,car='<';
            if (!strnicmp(titre,"GT",2)) psuiv=1,car='>';

            if (psuiv==1)
                code=0;
            break;
        }
    }

if (debut>=256)
    {
    lentit=debut-1;
    debut=0;
    }

if (code>=256)
    {
    lentit=code-1;
    code=0;
    }

if (lentit==0)
    {
    lentit=1;
    chaine[0]=car;
    }
    else
    {
    memcpy(chaine,titre,lentit);
    }

for (k=0;k<lentit;k++)
    {
    car=chaine[k];

    if ( (debut==0) & (code==0) )
        {
        switch(car)
            {
            case 0:
                break;
            case 10:
                if (pre==0)
                    car=13;
                    else
                    aff=1,car=0;
                break;
            case 13:
                car=0;
                break;
            case 1:
                aff=1,car=0;
                break;
            default:
                car=CnvASCII(KKCfg->cnvtable,car);
                break;
            }
        }

    if (nbrcol<0)
        nbrcol=0;

    col=tabcol[nbrcol];

    if ( (bold<0) | (ital<0) | (unde<0) )
        bold=0,ital=0,unde=0;

    if (bold!=0)
        col=(col&240)+Cfg->col[60];

    if (ital!=0)
        col=(col&240)+Cfg->col[61];

    if (unde!=0)
        col=(col&240)+Cfg->col[62];

    if (car!=0)
        {
        if ( (pre==0) & (car==13) & (smot==0) )
            car=0;

        if ( (pre==0) & (car==13) )
            car=32;

        if (car!=0)
            {
            mot[smot]=car;
            motc[smot]=col;
            smot++;
            }
        }

    if (smot>xpage)
        {
        j=smot-1;
        while ( (j>=0) & (mot[j]!=32) ) j--;

        if (j<=0) j=smot-1;


        for(i=0;i<j;i++)
           {
           ChrTxt[i+yp*xpage]=mot[i];
           ColTxt[i+yp*xpage]=motc[i];
           }

        if (j==xpage) j--;    // Car on passe espace quand il y a espace

        for(i=j+1;i<smot;i++)                // +1 car on passe l'espace
            {
            mot[i-j-1]=mot[i];
            motc[i-j-1]=motc[i];
            }

        smot-=(j+1);

        yp++;
        }

    while (aff!=0)
        {
        switch (aff)
            {
            case 1:
                j=0;
                break;
            case 2:
                j=(xpage-smot)/2;
                break;
            }

        for(i=0;i<smot;i++)
           {
           ChrTxt[(i+j)+yp*xpage]=mot[i];
           ColTxt[(i+j)+yp*xpage]=motc[i];
           }

        smot=0;

        yp++;

        break;
        }
    aff=0;

    if (yp>3998)
        break;

    } //--- fin du for (k=0;k<lentit;k++) ------------------------------


lentit=0;

if (yp>3998) break;

}

code=0;

// yp= limitte end

suiv=prem;

do
{
if (ye>yp-yl+y) ye=yp-yl+y;
if (ye<0) ye=0;

aff=0;

for (i=y;i<yl;i++)
    for(j=x;j<=xl;j++)
        AffCol(j,i,ColTxt[(j-x)+(i-y+ye)*xpage]);

for (i=y;i<yl;i++)
    for(j=x;j<=xl;j++)
        AffChr(j,i,ChrTxt[(j-x)+(i-y+ye)*xpage]);

if ( (suiv->next!=NULL) & (suiv!=NULL) )
    {
    ix=suiv->x1;
    iy=suiv->y1-ye;

    if ((iy>-y) & (iy<yl-y))
        while(1)
            {
            if (ix<xpage)
                AffCol(ix+x,iy+y,Cfg->col[18]);

            if (iy>(suiv->y2-ye)) break;

            ix++;
            if (ix>=xpage) ix=0,iy++;

            if ( (ix==suiv->x2) & (iy==(suiv->y2-ye)) ) break;

            
            }

    PrintAt(0,yl+1,"%-*s",Cfg->TailleX,suiv->link);
    }

posd=ye;
posn=(ye+yl-y);
taille2=yp;

while (!KbHit())
{
car=*Keyboard_Flag1;

if ( ((car&1)==1) | ((car&2)==2) )
    {
    long cur1,cur2;
    int prc;
    char temp[132];

    if (shift==0)
        SaveScreen();

    if (posn>taille2) posn=taille2;

    if (taille2<1024*1024)
        {
        cur1=(posd)*(Cfg->TailleY-2);
        cur1=cur1/taille2+1;

        cur2=(posn-1)*(Cfg->TailleY-2);
        cur2=cur2/taille2+1;

        prc=(posn*100)/taille2;
        }
        else
        {
        cur1=(posd/1024)*(Cfg->TailleY-2);
        cur1=cur1/(taille2/1024)+1;

        cur2=((posn-1)/1024)*(Cfg->TailleY-2);
        cur2=cur2/(taille2/1024)+1;

        prc=(posn/taille2)*100;
        }

    ColLin(0,0,Cfg->TailleX,Cfg->col[7]);

    strncpy(temp,fichier,78);

    temp[45]=0;

    PrintAt(0,0,"View: %-52s %9d bytes %3d%% ",temp,taille,prc);

    ColCol(Cfg->TailleX-1,1,Cfg->TailleY-2,Cfg->col[7]);
    ChrCol(Cfg->TailleX-1,1,cur1-1,32);
    ChrCol(Cfg->TailleX-1,cur1,cur2-cur1+1,219);
    ChrCol(Cfg->TailleX-1,cur2+1,Cfg->TailleY-1-cur2,32);

    shift=1;
    }
    else
    if (shift==1)
        {
        shift=-1;
        break;
        }

}

if (shift!=-1)
{
code=Wait(0,0);

switch(LO(code))   {
    case 0:
        switch(HI(code))   {
            case 80:    // BAS
                ye++;
                break;
            case 72:    // HAUT
                ye--;
                break;
            case 0x51:  // PGDN
                ye+=20;
                break;
            case 0x49:  // PGUP
                ye-=20;
                break;
            case 0x47:  // HOME
                ye=0;
                break;
            case 0x4F:  // END
                ye=yp-(yl-y);
                break;
            case 0xF:   // SHIFT-TAB
                rete=suiv;
                suiv=prem;
                while ( (suiv->next!=rete) & (suiv->next!=NULL) )
                    suiv=suiv->next;

                if (suiv->next==NULL)
                    {
                    rete=suiv;
                    suiv=prem;
                    while ( (suiv->next!=rete) & (suiv->next!=NULL) )
                        suiv=suiv->next;
                    }


                if ((suiv->next)!=NULL)
                    {
                    iy=suiv->y1-ye;

                    if ((iy<=(-y)) | (iy>=yl-y))
                        ye=suiv->y1-y;
                    }

                break;

            default:
                break;
            }
        break;
    case 9:
        car=0;
        while (suiv->next!=NULL)
            {
            if (((suiv->y1-ye)<=(-y)) | ((suiv->y1-ye)>=yl-y))
                {
                car++;
                suiv=suiv->next;
                }
                else
                {
                if (car==0)  suiv=suiv->next;
                break;
                }
            }

        if (suiv->next==NULL)
            suiv=prem;

        if (suiv->next!=NULL)
            {
            iy=suiv->y1-ye;

            if ((iy<=(-y)) | (iy>=yl-y))
                ye=suiv->y1-y;
            }
        break;
    case 13:
        strcpy(titre,suiv->link+8);
        titre[strlen(titre)-1]=0;

        if (!strnicmp(titre,"MAILTO:",7))
            {
            code=0;
            strcpy(chaine,"You couldn't mail to: ");
            strcat(chaine,titre+7);
            WinError(chaine);
            break;
            }
        if (!strnicmp(titre,"HTTP:",5))
            {
            code=0;
            strcpy(chaine,"You couldn't HTTP to: ");
            strcat(chaine,titre+7);
            WinError(chaine);
            break;
            }
        if (!strnicmp(titre,"FTP:",4))
            {
            code=0;
            strcpy(chaine,"You couldn't FTP to: ");
            strcat(chaine,titre+6);
            WinError(chaine);
            break;
            }
                

        if (titre[0]=='#')
            {
            strcpy(liaison,titre+1);
            Info.numero=104;
            }
            else
            {
            Path2Abs(fichier,"..");
            Path2Abs(fichier,titre);

            strcpy(Info.path,fichier);

            Traitefic(&Info);
            }

        break;
    default:
        break;
    }
}
else
{
shift=0;
LoadScreen();
}

}
while ((code!=27) & (code!=0x8D00) & (code!=13) );


while(prem->next!=NULL)
    {
    free(prem->link);
    suiv=prem->next;
    free(prem);
    prem=suiv;
    }
free(prem);

LoadScreen();

fclose(fic);

free(buffer);

free(ColTxt);
free(ChrTxt);

if (code==13)
    return Info.numero;
    else
    return -1;
}





/*--------------------------------------------------------------------*\
|- Gestion du filtre pour l'affichage texte                           -|
\*--------------------------------------------------------------------*/
void Masque(short x1,short y1,short x2,short y2)
{
char ok=1;
char *chaine;
char chain2[132];
short m1,m2;

unsigned char c,c2;
short x,y,l;
short oldx,oldy,oldl;
short xt[80],yt[80];

char cont,trouve=0;

struct PourMask *CMask;

if (((KKCfg->wmask)&128)==128) return;

CMask=Mask[(KKCfg->wmask)&15];

chaine=CMask->chaine;

x=x1;
y=y1;

l=0;

while ((y<=y2) | (ok==0) )
    {
    if (ok)
        c=GetChr(x,y);
        else
        c=32;

/*--------------------------------------------------------------------*\
|- Filtre Font de m... -> dos                                         -|
\*--------------------------------------------------------------------*/

    if ( ((c>='a') & (c<='z')) | ((c>='A') & (c<='Z')) | (c=='_') |
            ((c>=0xE0) & (c<=0xEB)) |            ((c>='0') & (c<='9')) )
        {
        chain2[l]=c;
        xt[l]=x;
        yt[l]=y;
        l++;
        if (l==80) l=0;
        }
        else
        {
        if (l!=0)
            {
            if (trouve==2)
                cont=1;
                else
                cont=0;

            trouve=0;
            m1=0;
            m2=0;

            while ( (chaine[m2]!='@') & (trouve==0) )
                {
                if ((chaine[m2]==32) | ((chaine[m2]==246) & (cont==0)))
                    {
                    if (m2-m1==l)
                        {
                        if (CMask->Ignore_Case==0)
                            {
                            if (!strncmp(chaine+m1,chain2,l))
                                trouve=1;
                            }
                            else
                            {
                            if (!strnicmp(chaine+m1,chain2,l))
                                trouve=1;
                            }
                        if ( (trouve==1) & (chaine[m2]==246) )
                            {
                            xt[l]=xt[l-1];
                            yt[l]=yt[l-1];
                            trouve=2;
                            chain2[l]=246;
                            oldl=l;
                            oldx=x;
                            oldy=y;
                            l++;
                            }
                        if (m1!=0)
                            {
                            if (chaine[m1-1]==246)
                                trouve=0;
                            }
                        }
                    m1=m2+1;
                    }
                m2++;
                }

            if (trouve!=2)
                {
                if (trouve==1)
                    c2=Cfg->col[17];     // trouve -> bright
                    else
                    c2=Cfg->col[16];

                if ( (trouve==0) & (cont==1) )
                    {
                    x=oldx;
                    y=oldy;
                    l=oldl;
                    }

                while (l!=0)
                    {
                    l--;
                    AffCol(xt[l],yt[l],c2);
                    }
                l=0;
                }
            }
        if (ok)
            AffCol(x,y,Cfg->col[16]);       // Ou autre chose
        }

/*--------------------------------------------------------------------*\
|- Filtre eLiTe                                                       -|
\*--------------------------------------------------------------------*/

    if (((KKCfg->wmask)&64)==64)
        {
        c=toupper(c);
        switch(c)
            {
            case 'A':
            case 'E':
            case 'I':
            case 'O':
            case 'U':
            case 'Y':
                c+='a'-'A';
                break;
            }
        AffChr(x,y,c);
        }

    if (ok==1)
        {
        x++;
        if (x>x2)
            {
            x--;
            ok=0;
            }
        }
        else
        {
        x=x1;
        y++;
        ok=1;
        }
    }

/*--------------------------------------------------------------------*\
|- Affichage de la chaine qui a ‚t‚ trouv‚ (par F7)                   -|
\*--------------------------------------------------------------------*/

if (*srcch!=0)
    {
    y=y1;
    for (x=x1;x<=x2;x++)
        chain2[x-x1]=GetChr(x,y1);
    chain2[x2-x1+1]=0;

    for (x=0;x<=(x2-x1)-strlen(srcch)+1;x++)
        if (!strnicmp(chain2+x,srcch,strlen(srcch)))
            for(l=0;l<strlen(srcch);l++)
                AffCol(x1+l+x,y1,Cfg->col[18]);     //Reverse
    }

}


/*--------------------------------------------------------------------*\
|- Changement du filtre pour l'affichage du texte                     -|
\*--------------------------------------------------------------------*/
void ChangeMask(void)
{
MENU menu;
int nbr;
char bars0[25],bars1[25],bars2[25];
static struct barmenu bar[19];
int retour,x,y,n,i,max;

char fin;

max=0;

nbr=4;

for(i=0;i<16;i++)
    if (strlen(Mask[i]->title)>0)
        {
        bar[nbr].fct=i+10;

        bar[nbr].Titre=Mask[i]->title;
        bar[nbr].Help=NULL;

        if ( ((KKCfg->wmask)&15) ==i) n=nbr;

        if (strlen(Mask[i]->title)>max) max=strlen(Mask[i]->title);

        nbr++;
        }

x=((Cfg->TailleX)-max)/2;
y=((Cfg->TailleY)-2*(nbr-2))/2;
if (y<2) y=2;

do
{
sprintf(bars0,"Elite %3s",(KKCfg->wmask&64)==64 ? "ON" : "OFF");
bar[0].Titre=bars0;
bar[0].Help=NULL;
bar[0].fct=1;

sprintf(bars1,"Mask %3s",(KKCfg->wmask&128)==128 ? "OFF" : "ON");
bar[1].Titre=bars1;
bar[1].Help=NULL;
bar[1].fct=2;

bar[2].fct=3;

bar[3].fct=0;


fin=0;

do
    {
    sprintf(bars2,"Xor %02X",xor);
    bar[2].Titre=bars2;
    bar[2].Help=NULL;

    menu.x=x;
    menu.y=y;
    menu.cur=n;
    menu.attr=0;

    retour=PannelMenu(bar,nbr,&menu);

    n=menu.cur;

    if ((bar[n].fct==3) & (retour!=2))
        xor+=retour;
    }
while ( (retour==1) | (retour==-1) );

if (retour==2)
    {
    switch (bar[n].fct)
        {
        case 1:
            KKCfg->wmask^=64;
            break;
        case 2:
            KKCfg->wmask^=128;
            break;
        case 3:
            break;
        default:
            KKCfg->wmask=((KKCfg->wmask)&240)|(bar[n].fct-10);
            fin=1;
        }
    }
}
while ( (!fin) & (retour!=0) );
}

/*--------------------------------------------------------------------*\
|- Changement du filtre pour l'affichage du texte                     -|
\*--------------------------------------------------------------------*/
void ChangeTrad(void)
{
int nbr;
static struct barmenu bar[5];
char bars0[25],bars1[25],bars2[25],bars4[25];
int retour,x,y,n,max;
MENU menu;

char fin;

max=0;

nbr=5;

x=((Cfg->TailleX)-max)/2;
y=((Cfg->TailleY)-2*(nbr-2))/2;
if (y<2) y=2;

n=0;
if (KKCfg->autotrad) n=4;
    else
    while (KKCfg->cnvtable!=(bar[n].fct)-1)
        n++;

sprintf(bars0,"Normal %c",KKCfg->cnvtable==0 ? 15 : 32);
bar[0].Titre=bars0;
bar[0].Help=NULL;
bar[0].fct=1;

sprintf(bars1,"BBS    %c",KKCfg->cnvtable==1 ? 15 : 32);
bar[1].Titre=bars1;
bar[1].Help=NULL;
bar[1].fct=2;

sprintf(bars2,"Latin  %c",KKCfg->cnvtable==2 ? 15 : 32);
bar[2].Titre=bars2;
bar[2].Help=NULL;
bar[2].fct=3;

bar[3].fct=0;

sprintf(bars4,"%s",KKCfg->autotrad ? "Auto" : "No Auto");
bar[4].Titre=bars4;
bar[4].Help=NULL;
bar[4].fct=10;

fin=0;

    menu.x=x;
    menu.y=y;
    menu.cur=n;
    menu.attr=8;

    retour=PannelMenu(bar,nbr,&menu);

    n=menu.cur;

if (retour==2)
    {
    switch (bar[n].fct)
        {
        case 1:
        case 2:
        case 3:
            KKCfg->cnvtable=bar[n].fct-1;
            KKCfg->autotrad=0;
            break;
        case 10:
            KKCfg->autotrad^=1;
            break;
        default:
            break;
        }
    }
}

/*--------------------------------------------------------------------*\
|- Changement du Line Feed pour le passage … la ligne                 -|
\*--------------------------------------------------------------------*/
int ChangeLnFeed(void)
{
MENU menu;
int nbr;
static struct barmenu bar[5];
char bars4[25];
int retour,x,y,n;

nbr=5;

x=((Cfg->TailleX)-10)/2;
y=((Cfg->TailleY)-2*(nbr-2))/2;
if (y<2) y=2;

bar[0].Titre="DOS (CR/LF)";
bar[0].Help=NULL;
bar[0].fct=1;

bar[1].Titre="Unix (LF)";
bar[1].Help=NULL;
bar[1].fct=3;

bar[2].Titre="CR";
bar[2].Help=NULL;
bar[2].fct=2;

bar[3].Titre="Mixed (CR-LF)";
bar[3].Help=NULL;
bar[3].fct=5;

bar[4].fct=4;

n=0;

while (KKCfg->lnfeed!=(bar[n].fct)-1)
    n++;

do
    {
    sprintf(bars4,"User Line Feed: $%02X",KKCfg->userfeed);
    bar[4].Titre=bars4;
    bar[4].Help=NULL;

    menu.x=x;
    menu.y=y;
    menu.cur=n;
    menu.attr=0;

    retour=PannelMenu(bar,nbr,&menu);

    n=menu.cur;

    if ((bar[n].fct==4) & (retour!=2))
        KKCfg->userfeed+=retour;
    }
while ( (retour==1) | (retour==-1) );

if (retour==2)
    {
    KKCfg->lnfeed=(bar[n].fct)-1;
    }

if (retour==0)
    return 0;
    else
    return 91;  //--- Viewer TeXTe -------------------------------------

}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|- Gestion impression                                                 -|
\*--------------------------------------------------------------------*/

char PRN_init(short lpt,char a)
{
union REGS regs;

regs.h.ah=1;
regs.w.dx=lpt;

int386(0x17,&regs,&regs);

return regs.h.ah;
}

/*--------------------------------------------------------------------*\
|-  Retourne 1 si tout va bien                                        -|
\*--------------------------------------------------------------------*/

char PRN_print(short lpt,char a)
{
union REGS regs;
char code,cont;
char result=1;

// PrintAt(0,0,"Printing...");

do
    {
    regs.h.ah=0;
    regs.h.al=a;
    regs.w.dx=lpt;

    int386(0x17,&regs,&regs);

    code=regs.h.ah;

    cont=0;

    if ( ((code&128)==128) & ((code&16)==16) )
        cont=1;

    if ((code&1)==1)
        if (WinMesg("Time-Out","Do you want to continue ?",1)==1)
            result=0,cont=0;
            else
            cont=1;
    if ( ((code&8)==8) & (result==1) )
        if (WinMesg("I/O Error","Do you want to continue ?",1)==1)
            result=0,cont=0;
            else
            cont=1;
    if ( ((code&32)==32) & (result==1) )
        if (WinMesg("No more paper","Do you want to continue ?",1)==1)
            result=0,cont=0;
            else
            cont=1;
    if ((code&64)==64)
        PrintAt(0,0,"ACK Error"),cont=0;
    }
while(cont);

return result;
}



void Print(char *fichier,int n)
{
static int sw,lf=28,l1,pp;

FILE *fic;
short lpt;
char a;
int m;
int ok;

static char x1=22,x2=22,x3=22;
static int y1=4,y2=7,y3=1;

struct Tmt T[] = {
      { 5, 2,10,"LPT1",&sw},
      { 5, 3,10,"LPT2",&sw},
      { 5, 4,10,"LPT3",&sw},
      { 5, 5,8,"IBM Graphic Code",&l1},

      { 5, 8, 7,"Line Feed",&lf},

      {32, 2,10,"10    cpi",&pp},
      {32, 3,10,"12    cpi",&pp},
      {32, 4,10,"15    cpi",&pp},
      {32, 5,10,"17.1  cpi",&pp},
      {32, 6,10,"20    cpi",&pp},
      {32, 7,10,"24    cpi",&pp},
      {32, 8,10,"30    cpi",&pp},

      {3,1,9,&x1,&y1},
      {28,1,9,&x2,&y2},
      {3,7,9,&x3,&y3},

      { 8,10,2,NULL,NULL},                                        // 1:Ok
      {33,10,3,NULL,NULL}
      };

struct TmtWin F = {-1,4,55,16, "Print file"};

char StrInit[]={27,51,0,27,91,0};

ok=1;        // Tout va bien

sw=0;
l1=1;
pp=8;

m=WinTraite(T,17,&F,0);

if (m==27)  //--- escape -----------------------------------------------
    return;
    else
    if (T[m].type==3) return;  //--- cancel ----------------------------

fic=fopen(fichier,"rb");
if (fic==NULL)
    {
    WinError("Couldn't open file");
    return;
    }

lpt=sw;

if (l1==1)
    {
    StrInit[2]=lf;
    StrInit[5]=pp-5;

    for(m=0;m<6;m++)
        if ((ok=PRN_print(lpt,StrInit[m]))==0) break;
    }


if ( (n==1) & (ok==1) )  //--- Fichier TEXTE ---------------------------
    {
    do
        {
        if (fread(&a,1,1,fic)==0) break;
        if (PRN_print(lpt,a)==0) break;
        }
    while(1);
    }

if (ok==1)
    WinMesg("Print","The file is printed",0);
}


/*--------------------------------------------------------------------*\
|- Gestion Ansi                                                       -|
\*--------------------------------------------------------------------*/
char *AnsiBuffer;
int Ansi1,Ansi2;

void BufAffChr(long x,long y,long c)
{
AnsiBuffer[y*160+x]=c;
}

void BufAffCol(long x,long y,long c)
{
AnsiBuffer[y*160+x+80]=c;
}


void StartAllPage(void)
{
char car;

AnsiAffChr=BufAffChr;
AnsiAffCol=BufAffCol;

AnsiBuffer=GetMem(160*1000);

Ansi1=0;

maxx=80;
maxy=Cfg->TailleY;

curx=0;
cury=0;

for (posn=0;posn<taille;posn++)
    {
    car=ReadChar();
    if (car==0x1A) break;
    ansi_out(car);
    }

Ansi2=cury+1-Cfg->TailleY+2;
if (Ansi2<0) Ansi2=0;
}

void DispAnsiPage(void)
{
int x,y;

for(y=0;y<Cfg->TailleY;y++)
    for(x=0;x<Cfg->TailleX;x++)
        {
        AffChr(x,y,AnsiBuffer[(y+Ansi1)*160+x]);
        AffCol(x,y,AnsiBuffer[(y+Ansi1)*160+80+x]);
        }
}

void CloseAllPage(void)
{
LibMem(AnsiBuffer);
}


/*--------------------------------------------------------------------*\
|- Affichage ansi                                                     -|
\*--------------------------------------------------------------------*/
int Ansi2View(char *fichier)
{
clock_t cl,cl2;

int xm,ym,zm;

int aff,wrap;


char autodown=0;
char car;


int code;
int fin=0;

int warp=0;

int shift=0; // Vaut -1 si reaffichage de l'ecran
             //       0 si pas de shift
             //       1 si touche shiftee


static char bar[81];


int oldx,oldy;


SaveScreen();
PutCur(3,0);

oldx=Cfg->TailleX;
oldy=Cfg->TailleY;

Cfg->TailleX=80;
//Cfg->TailleY=25;

TXTMode();

StartAllPage();

wrap=0;
aff=1;


/*--------------------------------------------------------------------*\
|- Affichage de la bar                                                -|
\*--------------------------------------------------------------------*/

strcpy
   (bar," Help  ----  ----  Text  ----  ----  ----  ----  ----  Quit ");



Bar(bar);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

cl=clock();

do
{

/*--------------------------------------------------------------------*\
|- Affichage d'une page                                               -|
\*--------------------------------------------------------------------*/
DispAnsiPage();

/*--------------------------------------------------------------------*\
|- Gestion des touches                                                -|
\*--------------------------------------------------------------------*/

zm=0;
code=0;

while ( (!KbHit()) & (zm==0) & (code==0) )
{
GetPosMouse(&xm,&ym,&zm);

car=*Keyboard_Flag1;

if ( ((car&1)==1) | ((car&2)==2) )
    {
    int prc;
    char temp[132];

    if (shift==0)
        SaveScreen();

    if (Ansi2!=0)
        prc=(Ansi1*100)/Ansi2;
        else
        prc=100;

    ColLin(0,0,Cfg->TailleX,Cfg->col[7]);

    strncpy(temp,fichier,78);

    temp[45]=0;

    PrintAt(0,0,
            "View: %-*s Col%3d %9d bytes %3d%% ",Cfg->TailleX-35,
                                                  temp,warp,taille,prc);

    Bar(bar);

    if (shift!=1)
        {
        shift=2;
        break;
        }
    shift=1;
    }
    else
    if (shift==1)
        {
        shift=-1;
        break;
        }

if (autodown)
    {
    while ((clock()-cl)<5);

    cl2=clock()-cl;
        
    code=80*256;
    cl=clock();
    }
}

if ( (shift!=-1) & (shift!=2) )
{
if (KbHit())
    {
    autodown=0;
    code=Wait(0,0);
    }

if (code==0)     //--- Pression bouton souris --------------------------
    {
    int button;

    button=MouseButton();

    if ((button&1)==1)     //--- gauche --------------------------------
        {
        int x,y;

        x=MousePosX();
        y=MousePosY();


        if ( (x==Cfg->TailleX-1) & (y>=1) & (y<=Cfg->TailleY-1) )
                                                      //-- Ascensceur --
            {
            posn=(taille*(y-1))/(Cfg->TailleY-2);
            }
            else
            if (y==Cfg->TailleY-1)
                if (Cfg->TailleX==90)
                    code=(0x3B+(x/9))*256;
                    else
                    code=(0x3B+(x/8))*256;
                else
                {
                if (y>(Cfg->TailleY)/2)
                    code=80*256;
                    else
                    code=72*256;
                ReleaseButton();
                }
        }

    if ((button&2)==2)     //--- droite --------------------------------
        {
        code=27;
        }
    }

switch(LO(code))
    {
    case 0:
       switch(HI(code))   {
            case 0x3B:  // F1
                SaveScreen();
                Cfg->TailleX=oldx;
                Cfg->TailleY=oldy;

                ChangeTaille(Cfg->TailleY);

                HelpTopic("View");
                Cfg->TailleX=80;
//                Cfg->TailleY=oldy;

                TXTMode();
                LoadScreen();
                break;
            case 0x3E:  // --- F4 --------------------------------------
                fin=91;

            case 80:    // DOWN
                Ansi1++;
                break;
            case 72:    // UP
                if (Ansi1!=0)
                    Ansi1--;
                break;
            case 0x51:    // PGDN
                Ansi1+=10;
                break;
            case 0x49:    // PGUP
                Ansi1-=10;
                if (Ansi1<0) Ansi1=0;
                break;
            case 0x4F:    // END
                Ansi1=Ansi2;
                break;
            case 0x47: // HOME
                Ansi1=0;
                break;
            case 0x44:  // --- F10 -------------------------------------
            case 0x8D: // CTRL-UP
                fin=-1;
                break;
            }
        break;
    case 32:
    case 13:
    case 27:
        fin=-1;
        break;
    }

}
else
{
if (shift==-1)
    {
    shift=0;
    LoadScreen();
    }
if (shift==2)
    {
    shift=1;
    }
}
}
while(!fin);

CloseAllPage();

Cfg->TailleX=oldx;
Cfg->TailleY=oldy;

ChangeTaille(Cfg->TailleY);

LoadScreen();

return fin;
}

void Decrypt(void)
{
unsigned short tab[256];
short ord[256];
short i,j,k,n;
char col[]=" etanoris-hdlc";
short val;

char *buffer;
int tbuf;

int kbest,xval;

kbest=0;
xval=0;

buffer=GetMem(Cfg->TailleX*Cfg->TailleY);
tbuf=0;

for(j=0;j<Cfg->TailleY;j++)
    for(i=0;i<Cfg->TailleX;i++)
        {
        buffer[tbuf]=GetChr(i,j);
        tbuf++;
        }


for(k=0;k<256;k++)
{
val=0;

for(i=0;i<256;i++)
    tab[i]=0;

for(i=0;i<256;i++)
    ord[i]=i;

for (i=0;i<tbuf;i++)
    tab[(buffer[i])^k]++;

for (i=0;i<256;i++)
    for(j=i;j<256;j++)
        if (tab[ord[i]]<tab[ord[j]])
            {
            n=ord[i];
            ord[i]=ord[j];
            ord[j]=n;
            }

for (i=0;i<15;i++)
    for (n=0;n<15;n++)
        if (ord[i]==col[n])
            {
            val+=100+((50*abs(i-n))/15);
            break;
            }

if (val>xval)
    {
    xval=val;
    kbest=k;
    }
}

xor=kbest;

sprintf(buffer,"Try XOR %d",kbest);
WinError(buffer);

LibMem(buffer);

}


/*--------------------------------------------------------------------*\
|- Fonction principale du viewer                                      -|
\*--------------------------------------------------------------------*/

void View(FENETRE *F)
{
static char buffer[256];
char *fichier,*liaison;
int n,o;
short i;
extern struct key K[nbrkey];

xor=0;

if (F->FenTyp!=0) return;

SaveScreen();

Bar(" ----  ----  ----  ----  ----  ----  ----  ----  ----  ---- ");

fichier=GetMem(256);
strcpy(fichier,F->path);
Path2Abs(fichier,F->F[F->pcur]->name);

liaison=GetMem(256);
strcpy(liaison,"");

i=InfoIDF(F);

fic=fopen(fichier,"rb");

if (fic==NULL)
    {
    //PrintAt(0,0,"Error on file '%s'",fichier);
    WinError("Couldn't open file");
    i=-1;
    }
    else
    {
    taille=filelength(fileno(fic));

    if (taille==0) i=-1;

    fread(view_buffer,32768,1,fic);

    pos=0;
    posl=32768;

    posn=0;
    }

while(i!=-1)
    {
    switch(i)
        {
        case 86:  // Ansi
            i=Ansi2View(fichier);
            break;
        case 104: // HTML
            i=HtmlView(fichier,liaison);
            break;
        case 37:  // GIF
        case 38:  // JPG
            FicIdf(buffer,fichier,i,0);
            CommandLine(buffer);
            i=-1;
            break;
        default:
            o=-1;
            for(n=0;n<nbrkey;n++)
                if (K[n].numero==i) o=n;

            if ((K[o].other&4)==4)
                i=TxtView(fichier);
                else
                i=HexaView(fichier);
            break;
        }
    }

if (fic!=NULL)
    fclose(fic);

free(liaison);
free(fichier);

LoadScreen();
}


/*--------------------------------------------------------------------*\
|- Viewer de vrai file (avec une path complete)                       -|
\*--------------------------------------------------------------------*/
void ViewFile(char *file)
{
FENETRE *SFen,*OldFen;
static char buf[256];
int i,k;

OldFen=DFen;

SFen=GetMem(sizeof(FENETRE));
SFen->F=GetMem(TOTFIC*sizeof(void *));

SFen->x=40;
SFen->nfen=7;
SFen->FenTyp=0;
SFen->Fen2=SFen;
SFen->y=1;
SFen->yl=(Cfg->TailleY)-4;
SFen->xl=39;
SFen->order=17;
SFen->pcur=0;
SFen->scur=0;

DFen=SFen;

strcpy(buf,file);
Path2Abs(buf,"..");

IOver=1;
IOerr=0;

CommandLine("#CD %s\n",buf);

IOver=0;

FileinPath(file,buf);

k=-1;
for (i=0;i<DFen->nbrfic;i++)
    if (!WildCmp(buf,DFen->F[i]->name))
        {
        k=i;
        break;
        }

if (k!=-1)
    {
    DFen->pcur=k;
    View(DFen);
    }

DFen=OldFen;

LibMem(SFen->F);
LibMem(SFen);
}

