/*--------------------------------------------------------------------*\
|- Viewer:                                                            -|
|          html: illimit‚ (d‚pend de la m‚moire)                      -|
|          ansi: 1000 lignes maximum                                  -|
|           txt: illimit‚                                             -|
|           bin: illimit‚                                             -|
|----------------------------------------------------------------------|
|- By RedBub/Ketchup^Pulpe                                            -|
\*--------------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <dos.h>

#include "idf.h"
#include "view.h"
#include "hard.h"
#include "reddies.h"

#define MAXANSLINE 1000

/*--------------------------------------------------------------------*\
|- Table de conversion                                                -|
|- Les deux premieres tables proviennent de latin.85O de termail      -|
\*--------------------------------------------------------------------*/
char TabDosWin[]={
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

char TabWinDos[]={
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,
49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,
65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,
97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,
113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,
129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,
145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,255,
173,189,156,207,190,221,245,249,184,166,174,170,240,169,238,248,
241,253,252,239,230,244,254,247,251,167,175,172,171,243,168,183,
181,182,199,142,143,146,128,212,144,210,211,222,214,215,216,209,
165,227,224,226,229,153,158,157,235,233,234,154,237,231,225,133,
160,131,198,132,134,145,135,138,130,136,137,141,161,140,139,208,
164,149,162,147,228,148,246,155,151,163,150,129,236,232,152
    };

char TabBBS[]={
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

char TabInt[]={
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,
    31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,
    46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,
    61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,
    76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,
    91,92,93,94,95,96,97,98,99,100,101,102,103,104,
    105,106,107,108,109,110,111,112,113,114,115,116,
    117,118,119,120,121,122,123,124,125,126,127,128,
    117,233,226,227,224,134,231,234,235,232,105,105,
    105,142,143,144,145,39,111,111,111,117,117,152,
    79,154,155,156,157,158,159,225,105,111,117,164,
    165,166,167,168,169,170,171,172,173,174,175,176,
    177,178,179,180,181,182,183,184,185,186,187,188,
    189,190,191,192,193,194,195,196,197,198,199,200,
    201,202,203,204,205,206,207,208,209,210,211,212,
    213,214,215,216,217,218,219,220,221,222,223,224,
    225,226,227,228,229,230,231,232,233,234,235,236,
    237,238,239,240,241,242,243,244,245,246,247,248,
    249,'.',251,252,253,254,255};


/*--------------------------------------------------------------------*\
|- Variable interne                                                   -|
\*--------------------------------------------------------------------*/

static char ReadChar(void);
KKVIEW *ViewCfg;

static char *Keyboard_Flag1=(char*)0x417;

static FILE *fic;               //--- handle du fichier ----------------
static char *view_buffer;       //--- buffer pour la lecture -----------
static long pos;                //--- position de depart ---------------
static long sizebuf;            //--- taille du buffer -----------------
static long posn;               //--- octet courant --------------------

static char srcch[132];         //--- chaine … rechercher --------------

static char raw;                //--- si vaut 1 -> raw screen ansi -----

static char namebuf[40];

static char secpart=0;          //--- Affiche les messages secrets -----

static long fromoct,nbroct;     //--- premier octet - nombre d'octet ---

static long sizefic;            //--- taille reelle du fichier ---------
static long taille;             //--- taille du fichier qu'on utilise --
static long firstoct;           //--- premier octet qu'on utilise vmt --


/*--------------------------------------------------------------------*\
|- Prototype                                                          -|
\*--------------------------------------------------------------------*/

void AltK(void);

void DispAnsiPage(int x1,int y1,int x2,int y2);

void ViewSetup(void);

void (*AnsiAffChr)(long x,long y,long c);
void (*AnsiAffCol)(long x,long y,long c);

void RefreshBar(char *bar);

void ChangeTrad(void);
void ChangeMask(void);
int ChangeLnFeed(void);
void Masque(short x1,short y1,short x2,short y2,char *bufscr);

void SavePosition(char *fichier,int posn);
int LoadPosition(char *fichier);


int HexaView(char *fichier);
int TxtView(char *fichier);
int HtmlView(char *fichier,char *liaison);

int TxtDown(int xl);
int TxtUp(int xl);

void SearchHexa(void);
void SearchTxt(void);
void Decrypt(void);
void Print(char *fichier,int n);

void StartAnsiPage(void);

int ChangeViewer(int i);
static int xarc,debarc;

void BufAffChr(long x,long y,long c);
void BufAffCol(long x,long y,long c);


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void StartWinView(char *titre)
{
int x,y,l;

SaveScreen();

xarc=(Cfg->TailleX-60)/2;
l=(60-strlen(titre))/2;

Cadre(xarc-1,5,xarc+62,11,0,Cfg->col[55],Cfg->col[56]);
Window(xarc,6,xarc+61,10,Cfg->col[16]);

Cadre(xarc+1,6,xarc+60,8,2,Cfg->col[55],Cfg->col[56]);

PrintAt(xarc+l,5,"%s",titre);

PrintAt(xarc+3,9,"Reading File");
PrintAt(xarc+3,10,"%s",namebuf);

x=xarc+43;
y=9;

PrintAt(x,y,"    STOP    ");
Puce(x,y,13,0);

debarc=0;
}

int InterWinView(void)
{
debarc=LongGradue(xarc+2,7,58,debarc,posn,taille);

if (KbHit())
    if (Wait(0,0)==13) return 1;

return 0;
}

void CloseWinView(void)
{
LoadScreen();
}



/*--------------------------------------------------------------------*\
|- Change le mode vid‚o                                               -|
\*--------------------------------------------------------------------*/
void ChangeSize(void)
{
TXTMode();
LoadPal(Cfg->palette);

InitFont();
}

uchar ROR(uchar,uchar);
#pragma aux ROR = \
      "ror al,cl" \
      parm [al] [cl] \
      value [al];


/*--------------------------------------------------------------------*\
|- Lit un buffer entier de sizebuf octets                             -|
\*--------------------------------------------------------------------*/
void ReadBlock(void)
{
int nbr;

fseek(fic,pos+firstoct,SEEK_SET);
nbr=fread(view_buffer,1,sizebuf,fic);
GetPreprocString(view_buffer,nbr);
}


/*--------------------------------------------------------------------*\
|-  Lit l'octet se trouvant en posn                                   -|
\*--------------------------------------------------------------------*/
char ReadChar(void)
{
if (posn>=taille)
    {
    posn=taille-1;
    return 0;
    }

if (posn<0)
    posn=0;

if (posn-pos>=sizebuf)
    {
    while (posn-pos>=sizebuf) pos+=sizebuf;
    ReadBlock();
    }

if (posn-pos<0)
    {
    while (posn-pos<0) pos-=sizebuf;
    ReadBlock();
    }

return view_buffer[posn-pos];
}

char ReadNextChar(void)
{
char car;
long l;

l=posn;
posn++;
car=ReadChar();
posn=l;

return car;
}


/*--------------------------------------------------------------------*\
|- ALT-K                                                              -|
\*--------------------------------------------------------------------*/
void AltK(void)
{
char fichier[256];
FILE *fic;
int car;
int x[2],y[2],z;
int i,j;
char col;

strcpy(fichier,ViewCfg->viewhist);
Path2Abs(fichier,"..\\clpboard.tmp");

x[0]=0;
y[0]=0;
z=0;

while(z!=2)
{
if (z==0)
    x[1]=x[0],y[1]=y[0];

SaveScreen();
col=GetCol(x[z],y[z]);
ColWin(x[0],y[0],x[1],y[1],(col&15)*16+(col/16));
car=Wait(x[z],y[z]);
LoadScreen();

switch(car)
    {
    case 80*256: //--- DOWN --------------------------------------------
        if (y[z]<Cfg->TailleY)
            y[z]++;
        break;
    case 72*256: //--- UP ----------------------------------------------
        if (y[z]>0)
            y[z]--;
        break;
    case 0x4D00: //--- RIGHT -------------------------------------------
        if (x[z]<Cfg->TailleX)
            x[z]++;
        break;
    case 0x4B00: //--- LEFT --------------------------------------------
        if (x[z]>0)
            x[z]--;
        break;
    case 0x4F00: //--- END ---------------------------------------------
        x[z]=Cfg->TailleX-1;
        break;
    case 0x4700: //--- HOME --------------------------------------------
        x[z]=0;
        break;
    case 0x4900: //--- PGUP --------------------------------------------
        y[z]=0;
        break;
    case 0x5100: //--- PGDN --------------------------------------------
        y[z]=Cfg->TailleY-1;
        break;

    case 13:
        z++;
        if (z==1)
            x[1]=x[0],y[1]=y[0];
        break;
    case 27:
        return;
    }
}

fic=fopen(fichier,"rb");
if (fic!=NULL)
    {
    static char CadreLength=60;

    struct Tmt T[5] = {
      {5,4,5, "   Append    ",NULL},
      {22,4,5,"  Overwrite  ",NULL},
      {46,4,3,NULL,NULL},
      { 5,2,0,"File CLPBOARD.TMP already exists in your user directory",NULL},
      { 1,1,6,&CadreLength,NULL}
      };

    struct TmtWin F = {-1,8,64,14, "Text Capture" };

    int n;

    n=WinTraite(T,5,&F,0);

    switch(n)
        {
        case 0:
            fic=fopen(fichier,"ab");
            if (fic==NULL) return;
            break;
        case 1:
            fic=fopen(fichier,"wb");
            if (fic==NULL) return;
            break;
        case -1:
        case 2:
            return;       // Cancel
        }
    }
    else
    {
    fic=fopen(fichier,"wb");
    if (fic==NULL) return;
    }

for(j=y[0];j<=y[1];j++)
    {
    for(i=x[0];i<=x[1];i++)
        fputc(GetChr(i,j),fic);
    fputc(0x0D,fic);
    fputc(0x0A,fic);
    }
fclose(fic);

}





/*--------------------------------------------------------------------*\
|- Load position of viewing in posdeb                                 -|
\*--------------------------------------------------------------------*/
int LoadPosition(char *fichier)
{
long posdeb=0;
FILE *fic;
uchar n;
long s,size;
static char ficname[256];

if (ViewCfg->saveviewpos==1)
    {
    fic=fopen(ViewCfg->viewhist,"rb");
    if (fic!=NULL)
        {
        while(fread(&n,1,1,fic)==1)
            {
            fread(ficname,1,n,fic);
            fread(&size,4,1,fic);
            fread(&s,4,1,fic);
            if ( (!stricmp(ficname,fichier)) & (size==sizefic) )
                {
                posdeb=s;
                break;
                }
            }
        fclose(fic);
        }
    }

if ((posdeb-firstoct)<0)
    return 0;

return posdeb-firstoct;
}

/*--------------------------------------------------------------------*\
|- Save position of viewing                                           -|
\*--------------------------------------------------------------------*/
void SavePosition(char *fichier,int posn)
{
long pos;
FILE *fic;
uchar n;
long s,size,pn;
static char ficname[256];

pn=posn+firstoct;

if (ViewCfg->saveviewpos==1)
    {
    fic=fopen(ViewCfg->viewhist,"r+b");
    if (fic==NULL)
        fic=fopen(ViewCfg->viewhist,"w+b");
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

                fwrite(&sizefic,4,1,fic);
                fwrite(&pn,4,1,fic);
                pn=0;
                break;
                }
            fread(&size,1,4,fic);
            fread(&s,1,4,fic);
            }
        if (pn!=0)
            {
            pos=ftell(fic);
            fseek(fic,pos,SEEK_SET);    // Parce qu'on ‚crit juste aprŠs

            n=(char)(strlen(fichier)+1);
            fwrite(&n,1,1,fic);
            fwrite(fichier,n,1,fic);
            fwrite(&sizefic,4,1,fic);
            fwrite(&pn,4,1,fic);
            }
        fclose(fic);
        }
    }
}


/*--------------------------------------------------------------------*\
| Pcboard                                                              |
\*--------------------------------------------------------------------*/
void ReadPCB(void)
{
char tag[32];
int ltag;
int x,y;
char car,chr,col;

col=7;
x=0;
y=0;

ltag=0; //--- Longueur du tag

for (posn=0;posn<taille;posn++)
    {
    car=ReadChar();
    if (car==0x1A) break;

    chr=0;

    if (ltag==0)
        switch(car)
            {
            case 10:
                x=0;
                break;
            case 13:
                y++;
                break;
            case '@':
                tag[0]='@';
                ltag=1;
                break;
            default:
                chr=car;
                break;
            }
        else
        {
        tag[ltag]=car;

        ltag++;
        if (ltag==32) ltag=0;

        tag[ltag]=0;

        if (car=='@')
            {
            int n;
            tag[ltag-1]=0;
            if (!strnicmp(tag+1,"POS:",4))
                {
                sscanf(tag+5,"%d",&n);
                x=n;
                }
            ltag=0;
            }

        if ((tag[1]=='X') & (ltag==4))  //--- Code couleur -------------
            {
            int n;
            sscanf(tag+2,"%02X",&n);
            col=n;
            ltag=0;
            }


        }

    if (chr!=0)
        {
        BufAffChr(x,y,chr);
        BufAffCol(x,y,col);
        x++;
        }
    }
}

void ReadRAW(void)
{
int x,y;
posn=0;
for(y=0;y<Cfg->TailleY;y++)
    for(x=0;x<Cfg->TailleX;x++)
        {
        if (posn<taille)
            {
            AnsiAffChr(x,y,ReadChar());
            posn++;
            AnsiAffCol(x,y,ReadChar());
            posn++;
            }
        }
}

void ReadANS(void)
{
char car;

for (posn=0;posn<taille;posn++)
    {
    car=ReadChar();
    if (car==0x1A) break;
    ansi_out(car);
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
int x1;
int cv;

int p;  // position du curseur dans la grille --------------------------

int n;

int fin=0; //--- Code de retour ----------------------------------------

x1=(Cfg->TailleX-80)/2;

SaveScreen();
PutCur(3,0);

Bar(" Help  ----  ---- ChView ---- PreproSearch ----  ----  Quit ");

Window  (x1+ 1,1,x1+78,(Cfg->TailleY)-3,Cfg->col[16]);
WinCadre(x1+ 0,3,x1+ 9,(Cfg->TailleY)-2,2);
WinCadre(x1+10,3,x1+58,(Cfg->TailleY)-2,2);
WinCadre(x1+59,3,x1+76,(Cfg->TailleY)-2,2);
WinCadre(x1+77,3,x1+79,(Cfg->TailleY)-2,2);
WinCadre(x1+ 0,0,x1+79,2,3);

if (x1!=0)
    {
    Window(0,0,x1-1,(Cfg->TailleY)-2,Cfg->col[16]);
    Window(80+x1,0,(Cfg->TailleX)-1,(Cfg->TailleY)-2,Cfg->col[16]);
    }

ChrCol(x1+34,4,(Cfg->TailleY)-6,Cfg->Tfont);

PrintAt(x1+3,1,"View File %s",fichier);

posn=LoadPosition(fichier);

do
{
posd=posn;
     
for (y=0;y<Cfg->TailleY-6;y++)
    {
    PrintAt(x1+1,y+4,"%08X",posn);

    for (x=0;x<16;x++)
        {
        if (posn<taille)
            {
            char a;
            a=ReadChar();
            PrintAt(x1+x*3+11,y+4,"%02X",a);
            AffChr(x1+x+60,y+4,a);
            posn++;
            }
            else
            {
            PrintAt(x1+x*3+11,y+4,"  ");
            AffChr(x1+x+60,y+4,32);
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

ChrCol(x1+78,4,cur1-4,32);
ChrCol(x1+78,cur1,cur2-cur1+1,219);
ChrCol(x1+78,cur2+1,(Cfg->TailleY-3)-cur2,32);

posn=posd;

car=0;


while ((!KbHit()) & (car==0))
    {
    int x,y,x2,y2,button,button2;

    GetPosMouse(&x,&y,&button);

    if (((*Keyboard_Flag1)&3)!=0)
        {
        if ( (y>=4) & (y<Cfg->TailleY-2) )
            {
            p=(x-x1-11)/3;
            if ( (((p*3)==x-x1-11) | ((p*3)==x-x1-12)) & (p<16) )
                p=p+(y-4)*16;
                else
                {
                p=(x-x1-60);
                if ( (p>=0) & (p<16) )
                    p=p+(y-4)*16;
                    else
                    p=-1;
                }
            }
            else
            p=-1;

        if (p!=-1)
            {
            ColLin((p&15)*3+x1+11,(p/16)+4,2,Cfg->col[18]);
            AffCol((p&15)+x1+60,(p/16)+4,Cfg->col[18]);
            }

        if ((button&3)!=0)
            do
                GetPosMouse(&x2,&y2,&button2);
            while((button2&3)!=0);

        if ( ((button&1)==1) & (p!=-1) ) //--- gauche ------------------
            {
            fromoct+=(p+posn);
            if (nbroct!=-1)
                nbroct-=(p+posn);

            fin=-3; //--- reload
            car=1;
            }

        if ( ((button&2)==2) & (p!=-1) ) //--- droite ------------------
            {
            nbroct=(p+posn);

            fin=-3; //--- reload
            car=1;
            }
        }
        else
        {
        p=-1;

        if ((button&1)==1) //--- gauche --------------------------------
            {
            if ((x==78+x1) & (y>=4) & (y<=Cfg->TailleY-3)) //- Ascens.--
                {
                posn=(taille*(y-4))/(Cfg->TailleY-7);
                car=1;
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

        if ((button&2)==2) //--- droite --------------------------------
            {
            car=27;
            }
        }

    for(n=0;n<16*(Cfg->TailleY-6);n++)
        {
        if (n!=p)
            {
            ColLin((n&15)*3+x1+11,(n/16)+4,2,Cfg->col[16]);
            AffCol((n&15)+x1+60,(n/16)+4,Cfg->col[16]);
            }
        }

    }

if (KbHit())
    car=Wait(0,0);

switch(LO(car))   {
    case 0:
        switch(HI(car))
            {
            case 0x25:  // --- ALT-K -----------------------------------
                AltK();
                break;
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
                posn=taille-((((Cfg->TailleY)-6)*16));
                break;
            case 0x40:  //--- F6 ---------------------------------------
                ChgViewPreproc();
                break;
            case 0x41:  // --- F7 --------------------------------------
                SearchHexa();
                break;
            case 0x44:  // --- F10 -------------------------------------
            case 0x8D:  // --- CTRL-UP ---------------------------------
                fin=-1;
                break;
            case 0x3B:  // --- F1 --------------------------------------
                HelpTopic("View");
                break;
            case 0x3E:  // --- F4 --------------------------------------
                cv=ChangeViewer(1);
                if (cv!=0)
                    fin=cv;
                break;

            case 0x8B:  // --- ALT-F11 ---------------------------------
                fin=-2;
                break;

            case 0x8C:  // --- ALT-F12 ---------------------------------
                ViewSetup();
                fin=-3;
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

if (fin==-2)
    GetListFile(fichier);

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

#define MAXARGLEN       128

void set_pos(char *argbuf,int arglen)
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

void go_up (char *argbuf)
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

void go_down (char *argbuf)
{
int x;

x = atoi(argbuf);

do
    {
//    if (cury == maxy - 1)        break;
    cury++;
    x--;
    }
while(x>0);
}

void go_left (char *argbuf)
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

void go_right (char *argbuf)
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

void save_pos (void)
{
savx = curx;
savy = cury;
issaved = 1;
}

void restore_pos (void)
{
if(issaved)
    {
    curx = savx;
    cury = savy;
    issaved = 0;
    }
}


void set_colors (char *argbuf,int arglen)
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
/*            case '\r':
                curx = 0;
                break;

            case '\n':
                cury++;
                break;
*/

            case '\n':
                curx = 0;
                cury++;
                break;

            case '\r':
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
                // putchar('\07');
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
                    set_pos(argbuf,arglen);
                    break;

                case 'A': //--- up -------------------------------------
                    go_up(argbuf);
                    break;

                case 'B': //--- down -----------------------------------
                    go_down(argbuf);
                    break;

                case 'C': //--- right ----------------------------------
                    go_right(argbuf);
                    break;         

                case 'D': //--- left -----------------------------------
                    go_left(argbuf);
                    break;

                case 'n': //--- device statusreport pos ----------------
                    break;

                case 's': //--- save pos -------------------------------
                    save_pos();
                    break;

                case 'u': //--- restore pos ----------------------------
                    restore_pos();
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
                    set_colors(argbuf,arglen);
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



void RefreshBar(char *bar)
{
switch(ViewCfg->warp)      //--- F2 --------------------------------------
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
ViewCfg->cnvtable=numtrad;
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

    if ( ((car==10) & (ViewCfg->lnfeed==0)) |
         ((car==13) & (ViewCfg->lnfeed==1)) |
         ((car==10) & (ViewCfg->lnfeed==2)) |
         ((car==10) & (ViewCfg->lnfeed==4)) |
         ((car==ViewCfg->userfeed) & (ViewCfg->lnfeed==3)))
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
    if ((m>xl) & (ViewCfg->warp!=0))
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
    if ( ((car==10) & (ViewCfg->lnfeed==0)) |
         ((car==13) & (ViewCfg->lnfeed==1)) |
         ((car==10) & (ViewCfg->lnfeed==2)) |
         ((car==10) & (ViewCfg->lnfeed==4)) |
         ((car==ViewCfg->userfeed) & (ViewCfg->lnfeed==3)))
        {
        if (posn!=0) posn++;
        return 1;
        }
    if ((car!=10) & (car!=13)) m++;
    if ((m>=xl) & (ViewCfg->warp!=0)) return 1;
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
int cv;

long posd;
long cur1,cur2;

int xl,yl;
int x,y;
int x2,y2;
int lchaine;

int y3;

char car;

char chaine[256];

char autowarp=0; //--- met a 1 si il faut absolument faire un nowarp ---

int m;

char affichage[101];

int w1,w2;

int code;
int fin=0;

int warp=0;

int shift=0; //--- Vaut -1 si reaffichage de l'ecran -------------------
             //---       0 si pas de shift -----------------------------
             //---       1 si touche shiftee ---------------------------

char pasfini;

char *bufscr;

static char bar[81];

int xl2;
int tpos;

bufscr=GetMem((Cfg->TailleX)*(Cfg->TailleY));

SaveScreen();
PutCur(3,0);

wrap=0;
aff=1;

/*--------------------------------------------------------------------*\
|-             Recherche de la meilleur traduction                    -|
\*--------------------------------------------------------------------*/
//if (ViewCfg->autotrad)    AutoTrad();



/*--------------------------------------------------------------------*\
|-             Calcul de la taille maximum                            -|
\*--------------------------------------------------------------------*/
if (ViewCfg->ajustview)
    {
    int xm=0,ym=0;
    int n;

    char vb,vb2;
    long posold;

    x=0;
    posold=posn;

    vb2=ReadChar();

    for (n=0;n<((taille<16000) ? taille:16000);n++)
        {
        vb=vb2;

        posn=n+1;
        vb2=ReadChar();

        if ( ((vb==13) & (vb2==10) & (ViewCfg->lnfeed==0)) |
             ((vb==13) & (ViewCfg->lnfeed==1)) |
             ((vb==10) & (ViewCfg->lnfeed==2)) |
             ((vb==10) & (ViewCfg->lnfeed==4)) |
             ((vb==ViewCfg->userfeed) & (ViewCfg->lnfeed==3)) )
             {
             x=0;
             ym++;
             if (ViewCfg->lnfeed==0) n++;
             }
        else
        switch(vb)
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
                if (ViewCfg->lnfeed==4) break;
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
   (bar," Help NowrapLnFeedChView Zoom PreproSearch Print Mask  Quit ");



RefreshBar(bar);

Bar(bar);

/*--------------------------------------------------------------------*\
|- Garde la position pr‚c‚dente                                       -|
\*--------------------------------------------------------------------*/

posn=LoadPosition(fichier);

/*--------------------------------------------------------------------*/

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

    if ( ((chaine[0]==13) & (ReadNextChar()==10) & (ViewCfg->lnfeed==0)) |
         ((chaine[0]==13) & (ViewCfg->lnfeed==1)) |
         ((chaine[0]==10) & (ViewCfg->lnfeed==2)) |
         ((chaine[0]==10) & (ViewCfg->lnfeed==4)) |
         ((chaine[0]==ViewCfg->userfeed) & (ViewCfg->lnfeed==3)) )
        {
        if (ViewCfg->lnfeed==0) posn++;
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
                if (ViewCfg->lnfeed==4)
                    {
                    lchaine=0;
                    chaine[0]=0;
                    }
                break;
            default:
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

    if ( (tpos>=xl2) & (aff==0) )   // Si le prochain d‚passe la fenˆtre
        {
        if (tpos>=xl2)
            {
            if ( (ViewCfg->warp==1) & (autowarp==0) )
                {
                aff=2;

                w1=xl2;                             // Premier … retenir
                w2=tpos;                            // Dernier … retenir
                }
            if ( (ViewCfg->warp==2) & (autowarp==0) )
                {
                int n;
                aff=2;
                n=xl2;
                while ( (n>0) & (affichage[n]!=32) ) n--;

                if (n==0) n=xl2-1;    // On coupe pas trop !

                w1=n+1;
                w2=tpos;
                }
            }
        }

    if (aff!=0)            // Le monsieur te demande d'afficher la ligne
        {
        if ((shift!=1) | (y2!=0))
            {
            y3=y2*(Cfg->TailleX);

            if (w1<0)
                memset(bufscr+x+y3,32,xl2);
                else
            if (w1>=xl2)
                memcpy(bufscr+x+y3,affichage,xl2);
                else
                {
                memcpy(bufscr+x+y3,affichage,w1);
                memset(bufscr+x+y3+w1,32,xl2-w1);
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
        {
        int n=0;
        y3=y2*(Cfg->TailleX);

        for(n=0;n<strlen(affichage);n++)
            bufscr[x+n+y3]=affichage[n];
        }
        y2++;
        break;
        }
    }
while(1);


while(y2<y+yl)
    {
    memset(affichage,' ',xl);     // remplit une ligne entiere de espace
    affichage[xl]=0;
        {
        int n=0;

        y3=y2*(Cfg->TailleX);
        for(n=0;n<strlen(affichage);n++)
            bufscr[x+n+y3]=affichage[n];
        }
    y2++;
    }

if (shift==0)
    Masque(x,y,x+xl-1,y+yl-1,bufscr);
    else
    Masque(x,y+(y==0),x+xl-1-(xl==Cfg->TailleX),y+yl-1,bufscr);


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

    PrintAt(0,0,"View: %-*s Col%3d %9d bytes %3d%% ",Cfg->TailleX-35,
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
        switch(HI(code))
            {
            case 0x25:  //--- ALT-K ------------------------------------
                AltK();
                break;
            case 0x54:
                Decrypt();
                break;
            case 0x3B:  //--- F1 ---------------------------------------
                HelpTopic("View");
                break;
            case 0x3C:  //--- F2 ---------------------------------------
                if (autowarp==1) break;
                ViewCfg->warp++;
                if (ViewCfg->warp==3) ViewCfg->warp=0;
                RefreshBar(bar);
                Bar(bar);
                break;
            case 0x3D:  //--- F3 ---------------------------------------
                fin=ChangeLnFeed();
                break;
            case 0x3E:  //--- F4 ---------------------------------------
                cv=ChangeViewer(0);
                if (cv!=0)
                    fin=cv;
                break;
            case 0x3F:  //--- F5 ---------------------------------------
                ViewCfg->ajustview^=1;
                fin=91;
                break;
            case 0x40:  //--- F6 ---------------------------------------
                ChgViewPreproc();

                /*
                ChangeTrad();

                if (ViewCfg->autotrad)
                    AutoTrad();
                */
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


            case 0x8B:  // --- ALT-F11 ---------------------------------
                fin=-2;
                break;

            case 0x8C:  // --- ALT-F12 ---------------------------------
                ViewSetup();
                fin=91;
                break;
            }
        break;
    case 32:
    case 13:
    case 27:
        fin=-1;
        break;
    case 6:                                                 //--- CTRL-F
        SaveScreen();
        Cfg->font^=1;
        ChangeSize();               //--- Rafraichit l'ecran -----------
        LoadScreen();
        break;
    }

if ( (ViewCfg->warp!=0) & (autowarp==1) ) warp=0;

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

if (fin==-2)
    GetListFile(fichier);

LoadScreen();

LibMem(bufscr);

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
|- Change le type de viewer                                           -|
\*--------------------------------------------------------------------*/
int ChangeViewer(int i)
{
MENU menu;
int nbr;
static struct barmenu bar[5];
int retour,x,y;

nbr=4;

x=((Cfg->TailleX)-10)/2;
y=((Cfg->TailleY)-2*(nbr-2))/2;
if (y<2) y=2;

bar[0].Titre="Texte";
bar[0].Help=NULL;
bar[0].fct=91;

bar[1].Titre="Binary";
bar[1].Help=NULL;
bar[1].fct=-3;

bar[2].Titre="Ansi";
bar[2].Help=NULL;
bar[2].fct=86;

bar[3].Titre="HTML";
bar[3].Help=NULL;
bar[3].fct=104;

menu.cur=i+1;
if (menu.cur>nbr) menu.cur=0;

menu.x=x;
menu.y=y;
menu.attr=8;

retour=PannelMenu(bar,nbr,&menu);

if (retour==2)
    return bar[menu.cur].fct;

return 0;
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

if (n!=2) return;

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

posn++;

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

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|- Viewer HTML                                                        -|
\*--------------------------------------------------------------------*/


struct Href
    {
    char *link;
    signed int x1,y1;
    signed int x2,y2;
    struct Href *next;
    };

struct HLine
    {
    char *Chr;
    char *Col;
    struct HLine *from,*next;
    };

struct HTMtd
    {
    int align; //---0: left, 1:right, 2:center -------------------------
    };


/*--------------------------------------------------------------------*\
|- Allocation d'une ligne HTML                                        -|
\*--------------------------------------------------------------------*/
struct HLine *HtmAlloc(void)
{
struct HLine *H;
int x;

x=Cfg->TailleX;

H=(struct HLine*)GetMem(sizeof(struct HLine));

H->Chr=(char*)GetMem(x);
memset(H->Chr,32,x);
H->Col=(char*)GetMem(x);
memset(H->Col,Cfg->col[16],x);

H->next=NULL;
H->from=NULL;

if (InterWinView())
    posn=taille;

return H;
}

/*--------------------------------------------------------------------*\
|- Ligne HTML pr‚cedente                                              -|
\*--------------------------------------------------------------------*/
void HTMLfrom(struct HLine **D,struct HLine **P,int *ye)
{
if ((*P)->from!=NULL)
    {
    (*ye)--;
    (*P)=(*P)->from;
    (*D)=(*D)->from;
    }
}

/*--------------------------------------------------------------------*\
|- Ligne HTML suivante                                                -|
\*--------------------------------------------------------------------*/
void HTMLnext(struct HLine **D,struct HLine **P,int *ye)
{
if ((*D)->next!=NULL)
    {
    (*ye)++;
    (*P)=(*P)->next;
    (*D)=(*D)->next;
    }
}


/*--------------------------------------------------------------------*\
|- Lecture du tag TD, P, Hx (changement de paragraphe)                -|
\*--------------------------------------------------------------------*/
void ReadTagTD(char *titre,struct HTMtd *HTM)
{
char buffer[256];
char *align;

HTM->align=0;

strncpy(buffer,titre,255);
buffer[255]=0;
strupr(buffer);

align=strstr(buffer," ALIGN"); //--- Pour le cas ou 'VALIGN' -----------
if (align!=NULL)
    {
    align=strchr(align,'=');
    if (align!=NULL)
        {
        strcpy(buffer,align+1);
        if (!strnicmp(buffer,"center",6)) HTM->align=2;
        if (!strnicmp(buffer,"right",5)) HTM->align=1;
        if (!strnicmp(buffer,"left",4)) HTM->align=0;
        }
    }
}


/*--------------------------------------------------------------------*\
|- Affichage d'une page HTML                                          -|
\*--------------------------------------------------------------------*/

int HtmlView(char *fichier,char *liaison)
{
struct HTMtd HTMpar,HTMpar2;
char finref;    //--- Vaut 1 si on doit stopper une reference ----------

int cv;

char titre[256],*titre2;
short lentit;
long posd,taille2;

char chaine[256];

char mot[128];  //--- phrase a ecrire ----------------------------------
char motc[128]; //--- couleur de cette phrase --------------------------
short smot;     //--- Taille du mot ------------------------------------

long lu;

long debut;     //--- position du < ------------------------------------
long code;      //--- position du & ------------------------------------

char aff;       //--- vaut 1 si il faut ecrire -------------------------
char prev;      //--- vaut 1 si la derniere commande est enter ---------

int yp;         //--- position virtuelle sur ecran ---------------------
int xl,yl,ye;
int ye0;        //--- Premiere ligne au lancement du viewer ------------
int x,y;

char ahref;     //--- nombre de ref en cours ---------------------------

uchar col;

uchar tabcol[32];
short nbrcol;

char car;

char psuiv;     //--- vaut 1 si on passe le suivant --------------------

int i,j,k,l;    //--- Compteur -----------------------------------------

signed char bold;
signed char ital;
signed char unde;

char pre;       //--- vaut 0 si on ignore spaces, tab, CR et LF --------
char nlist;     //--- nombre de liste ----------------------------------
char listn[16]; //--- position dans liste ------------------------------
char listt[16]; //--- type de liste ------------------------------------

int fin=0;

short xpage=Cfg->TailleX-2;

struct Href *prem,*suiv,*rete;

int xm,ym,zm;   //--- Gestion souris -----------------------------------

struct HLine *FLine; //--- La toute premiere ligne ---------------------
struct HLine *CLine; //--- La ligne courrante --------------------------
struct HLine *TLine; //--- Ligne temporaire ----------------------------
struct HLine *PLine; //--- Premiere ligne en haut de l'‚cran -----------
struct HLine *DLine; //--- Derniere ligne en bas  de l'‚cran -----------

int ix,iy;

int shift=0;    //--- Vaut -1 si reaffichage de l'ecran ----------------
                //---       0 si pas de shift --------------------------
                //---       1 si touche shiftee ------------------------

int derspace;   //--- Vaut 1 si le dernier caractere ‚tait un space ----

RB_IDF Info;

SaveScreen();

StartWinView("HTML Viewer");

FLine=HtmAlloc();

CLine=FLine;

SavePosition(fichier,1);    //--- Pour l'historique aprŠs --------------

PutCur(3,0);

x=1;
y=1;

xl=Cfg->TailleX-2;
yl=Cfg->TailleY-2;



/*--------------------------------------------------------------------*/

prem=(struct Href*)GetMem(sizeof(struct Href));
suiv=prem;


suiv->next=NULL;

ye=0;  //--- Initialisation de la premiere ligne -----------------------
ye0=0; //--- On commence par afficher la premiere ligne ----------------

HTMpar2.align=0;


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

posn=0;

derspace=1; //--- le dernier caractere est un space --------------))))))

while(posn<taille)
{
HTMpar.align=HTMpar2.align;

do
    {
    if (posn>=taille)
        {
        aff=1;
        break;
        }
    car=ReadChar();
    posn++;
    if (pre!=0) break;

    if ((car==10) | (car==13) | (car==9)) car=32;
    if (car!=32) break;
    }
while(derspace);

finref=0;

if (car!=32)
    derspace=0;
    else
    derspace=1;

if ( (debut==0) & (code==0) )
switch(car)
    {
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

if (debut!=0)       //--- Code <...> -----------------------------------
switch(car)
    {
    case '>':
        if (debut<128)
            {
            titre[debut-1]=0;

            if (!stricmp(titre,"TITLE"))
                {
                HTMpar2.align=2;
                nbrcol++,tabcol[nbrcol]=Cfg->col[35],aff=1;
                }
            if (!stricmp(titre,"/TITLE"))
                {
                HTMpar2.align=0;
                nbrcol--,aff=1;
                }

            if (!strnicmp(titre,"H",1))
                ReadTagTD(titre,&HTMpar2);

            if (!strnicmp(titre,"/H",2))
                HTMpar2.align=0;


            if (!strnicmp(titre,"H1",2))
                             nbrcol++,tabcol[nbrcol]=Cfg->col[36],aff=1;
            if (!strnicmp(titre,"H2",2))
                             nbrcol++,tabcol[nbrcol]=Cfg->col[50],aff=1;
            if (!strnicmp(titre,"H3",2))
                             nbrcol++,tabcol[nbrcol]=Cfg->col[51],aff=1;
            if (!strnicmp(titre,"H4",2))
                             nbrcol++,tabcol[nbrcol]=Cfg->col[57],aff=1;
            if (!strnicmp(titre,"H5",2))
                             nbrcol++,tabcol[nbrcol]=Cfg->col[58],aff=1;
            if (!strnicmp(titre,"H6",2))
                             nbrcol++,tabcol[nbrcol]=Cfg->col[59],aff=1;

            if (!stricmp(titre,"STRONG"))
                bold++;                                //--- GRAS ON ---
            if (!stricmp(titre,"B"))
                bold++;                                //--- GRAS ON ---
            if (!stricmp(titre,"EM"))
                ital++;                            //--- ITALIQUE ON ---
            if (!stricmp(titre,"I"))
                ital++;                            //--- ITALIQUE ON ---
            if (!stricmp(titre,"U"))
                unde++;                           //--- UNDERLINE ON ---

            if (!strnicmp(titre,"A HREF",6))
                {
                ahref++;
                nbrcol++;
                tabcol[nbrcol]=Cfg->col[17];
                suiv->link=(char*)GetMem(strlen(titre)+1);
                memcpy(suiv->link,titre,strlen(titre)+1);

                suiv->x1=smot;
                suiv->y1=yp;
                }

            if (!strnicmp(titre,"A NAME",6))
                {
                titre[strlen(titre)-1]=0;

                if (!stricmp(titre+8,liaison))
                    ye0=yp;
                }


            if (!stricmp(titre,"/A"))
                {
                finref=1;
                }

            if (!stricmp(titre,"DEBUG"))
                {
                WinError("Debug");
                }

            if (!strnicmp(titre,"IMG",3))
                {
                char *alt,*src;
                char buffer[256];

                strncpy(buffer,titre,255);
                strupr(buffer);

                alt=strstr(buffer,"ALT");
                if (alt!=NULL)
                    {
                    alt=strchr(alt,'\"');
                    if (alt!=NULL)
                        {
                        strncpy(titre,alt+1,255);
                        alt=strchr(titre,'\"');
                        if (alt!=NULL)
                            alt[0]=0;
                            else
                            titre[0]=0;
                        }
                    }

                src=strstr(buffer,"SRC");
                if ((src!=NULL) & (alt==NULL))
                    {
                    src=strchr(src,'\"');
                    if (src!=NULL)
                        {
                        strncpy(titre,src+1,255);
                        src=strchr(titre,'\"');
                        if (src!=NULL)
                            src[0]=0;
                        src=titre;

                        if (!strcmp(src,"INTERNAL-GOPHER-IMAGE"))
                            strcpy(titre,"\03"); else
                        if (!strcmp(src,"INTERNAL-GOPHER-UNKNOWN"))
                            strcpy(titre,"?"); else
                        if (!strcmp(src,"INTERNAL-GOPHER-TEXT"))
                            strcpy(titre,"#"); else
                        if (!strcmp(src,"INTERNAL-GOPHER-SOUND"))
                            strcpy(titre,"\0E");
                            else
                            titre[0]=0;
                        }
                    }

                if (titre[0]==0)
                    strcpy(titre,"[IMAGE]");

                lentit=strlen(titre);
                }

            if (!stricmp(titre,"/STRONG")) bold--;           // GRAS OFF
            if (!stricmp(titre,"/B"))      bold--;           // GRAS OFF
            if (!stricmp(titre,"/EM"))     ital--;       // ITALIQUE OFF
            if (!stricmp(titre,"/I"))     ital--;        // ITALIQUE OFF
            if (!stricmp(titre,"/U"))      unde--;      // UNDERLINE OFF
                

            if (!stricmp(titre,"/H1")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H2")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H3")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H4")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H5")) nbrcol--,aff=1;
            if (!stricmp(titre,"/H6")) nbrcol--,aff=1;
            if (nbrcol<0) nbrcol=0;
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

            if (!strnicmp(titre,"P",1))  //--- Debut de paragraphe -----
                {
                ReadTagTD(titre,&HTMpar2);
                aff=1;       
                }
            if (!stricmp(titre,"CENTER"))  //--- Paragraphe centr‚ -----
                HTMpar2.align=2;

            if (!stricmp(titre,"/CENTER"))  //--- Paragraphe centr‚ ----
                HTMpar2.align=0;

            if (!stricmp(titre,"/P"))
                {
                HTMpar2.align=0;
                aff=1;                     //--- fin de paragraphe -----
                }

            if (!stricmp(titre,"OL"))
                                  listt[nlist]=2,listn[nlist]=1,nlist++;
            if ((!stricmp(titre,"/OL")) & (nlist!=0))
                            listt[nlist]=0,listn[nlist]=0,nlist--,aff=1;
            if (!stricmp(titre,"UL"))
                                  listt[nlist]=1,listn[nlist]=1,nlist++;
            if ((!stricmp(titre,"/UL")) & (nlist!=0))
                            listt[nlist]=0,listn[nlist]=0,nlist--,aff=1;

            if (!stricmp(titre,"MENU"))
                                  listt[nlist]=1,listn[nlist]=1,nlist++;
            if ((!stricmp(titre,"/MENU")) & (nlist!=0))
                            listt[nlist]=0,listn[nlist]=0,nlist--,aff=1;

            if (!stricmp(titre,"DL")) aff=1;
            if (!stricmp(titre,"DT")) aff=1;
            if (!stricmp(titre,"DD")) aff=1;
            if (!stricmp(titre,"/DL")) aff=1;
            if (!stricmp(titre,"/DT")) aff=1;
            if (!stricmp(titre,"/DD")) aff=1;

            if (!strnicmp(titre,"TR",2))
                {
                ReadTagTD(titre,&HTMpar2);
                aff=1;
                }
            if (!strnicmp(titre,"TD",2))
                {
                ReadTagTD(titre,&HTMpar2);
                aff=1;
                }
            if (!strnicmp(titre,"TH",2))
                {
                ReadTagTD(titre,&HTMpar2);
                aff=1;
                }

            if (!strnicmp(titre,"/TR",3))
                HTMpar2.align=0;
            if (!strnicmp(titre,"/TD",3))
                HTMpar2.align=0;
            if (!strnicmp(titre,"/TH",3))
                HTMpar2.align=0;

            if (!stricmp(titre,"I"))  aff=0;              // Chasse fixe
            if (!stricmp(titre,"/I")) aff=0;

            if (!stricmp(titre,"PRE")) pre++;
            if ((!stricmp(titre,"/PRE")) & (pre!=0) )
                pre--;

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

            if (!strnicmp(titre,"UGRAVE",6)) psuiv=1,car='u';
            if (!strnicmp(titre,"UACUTE",6)) psuiv=1,car='u';

            if (!strnicmp(titre,"CCEDIL",6)) psuiv=1,car='‡';

            if (!strnicmp(titre,"ECIRC",5)) psuiv=1,car='ˆ';
            if (!strnicmp(titre,"ACIRC",5)) psuiv=1,car='ƒ';
            if (!strnicmp(titre,"ICIRC",5)) psuiv=1,car='Œ';
            if (!strnicmp(titre,"UCIRC",5)) psuiv=1,car='–';
            if (!strnicmp(titre,"OCIRC",5)) psuiv=1,car='“';

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

            if (psuiv==1)   //--- La valeur est d‚cod‚ correctement ----
                {
                code=0;
                if (lentit!=0)
                    GetPreprocString(titre,lentit);
                    else
                    car=GetPreprocValue(car);
                }
            break;
        }
    }


/*--------------------------------------------------------------------*\
|- Gestion de tous les petits machins                                 -|
\*--------------------------------------------------------------------*/

if (aff==1)
    finref=1;


if (finref)
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

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

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
                aff=1;
                break;
            default:
                break;
            }
        }

    if (nbrcol<0)
        nbrcol=0;

    col=tabcol[nbrcol];

    if (bold>1) bold=1;
    if (ital>1) ital=1;
    if (unde>1) unde=1;

    if ( (bold<0) | (ital<0) | (unde<0) )
        bold=0,ital=0,unde=0;

    if (bold!=0)
        col=(col&240)+Cfg->col[60];

    if (ital!=0)
        col=(col&240)+Cfg->col[61];

    if (unde!=0)
        col=(col&240)+Cfg->col[62];

    if ((car!=0) & (car!=1))
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

    if (smot>xpage) //--- le paragraphe d‚passe une ligne --------------
        {
        char cesure;

        cesure=1;

        j=smot-1;
        while ( (j>=0) & (mot[j]!=32) ) j--;

        if (j<=0)
            {
            j=smot-1;
            cesure=0;
            }

        for(i=0;i<j;i++)
           {
           CLine->Chr[i]=mot[i];
           CLine->Col[i]=motc[i];
           }

        for(i=j+cesure;i<smot;i++)        // +1 car on passe l'espace---
            {
            mot[i-j-cesure]=mot[i];
            motc[i-j-cesure]=motc[i];
            }

        smot-=(j+cesure);

        TLine=HtmAlloc();

        CLine->next=TLine;
        TLine->from=CLine;
        TLine->next=NULL;
        CLine=TLine;

        yp++;
        if (nlist!=0)
            {
            int k;
            k=nlist*2+2;
            for(i=smot+k-1;i>=k;i--)
                {
                mot[i]=mot[i-k];
                motc[i]=motc[i-k];
                }
            for(i=0;i<k;i++)
                {
                mot[i]=32;
                motc[i]=Cfg->col[16];
                }
            smot+=k;
            }
        }

    while (aff!=0) //--- Quelqu'un a demand‚ un passage … la ligne -----
        {
        j=0;

        if (HTMpar.align==2)    //--- centrage du texte ----------------
            j=(xpage-smot)/2;

        for(i=0;i<smot;i++)
           {
           CLine->Chr[i+j]=mot[i];
           CLine->Col[i+j]=motc[i];
           }

        GetPreprocString(CLine->Chr,smot);


        rete=prem;
        while(rete->next!=NULL)
            {
            if (rete->y1==yp) rete->x1+=j;
            if (rete->y2==yp) rete->x2+=j;

            rete=rete->next;
            }

        smot=0;

        TLine=HtmAlloc();

        CLine->next=TLine;
        TLine->from=CLine;
        TLine->next=NULL;
        CLine=TLine;

        yp++;
        derspace=1; //--- le dernier caractere est un space ------------

        if ((nlist!=0) & (car!=1))
            {
            int k;
            k=nlist*2+2;
            for(i=0;i<k;i++)
                {
                mot[i]=32;
                motc[i]=Cfg->col[16];
                }
            smot=k;
            }
        break;
        }
    aff=0;

//    if (yp>3998)        break;

    } //--- fin du for (k=0;k<lentit;k++) ------------------------------


lentit=0;

// if (yp>3998) break;
}

CloseWinView();

WinCadre(x-1,y-1,xl+1,yl,2);

Window(x,y,xl,yl-1,Cfg->col[16]);

ColLin(0,yl+1,Cfg->TailleX,Cfg->col[6]);
ChrLin(0,yl+1,Cfg->TailleX,32);

PLine=FLine;
DLine=FLine;
for(i=0;i<yl-y;i++)
    {
    if (DLine->next!=NULL)
        DLine=DLine->next;
    }

for(i=0;i<ye0;i++)
    HTMLnext(&DLine,&PLine,&ye);

code=0;

// yp= limitte end

suiv=prem;

do
{
aff=0;

TLine=PLine;

for (i=y;i<yl;i++)
    {
    if (TLine!=NULL)
        {
        for(j=x;j<=xl;j++)
            AffCol(j,i,TLine->Col[j-x]);
        TLine=TLine->next;
        }
    }

TLine=PLine;

for (i=y;i<yl;i++)
    {
    if (TLine!=NULL)
        {
        for(j=x;j<=xl;j++)
            AffChr(j,i,TLine->Chr[j-x]);
        TLine=TLine->next;
        }
    }

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

zm=0;

while ( (!KbHit()) & (zm==0) )
    {
    GetPosMouse(&xm,&ym,&zm);

    car=*Keyboard_Flag1;

    if ((car&3)!=0)
        {
        long cur1,cur2;
        int prc;
        char temp[132];

        if (shift==0)
            SaveScreen();

        if (posn>taille2)
            posn=taille2;

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

        Bar(
        " Help  ----  ---- ChView ---- Prepro ----  ----  ----  Quit ");

        shift=1;
        }
    else
        {
        if (shift==1)
            {
            shift=-1;
            break;
            }
        }
    }

if (shift!=-1)
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

/*        if ( (x==Cfg->TailleX-1) & (y>=1) & (y<=Cfg->TailleY-1) )
                                                      //-- Ascensceur --
            {
            posn=(taille*(y-1))/(Cfg->TailleY-2);
            }
            else
*/
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
        case 27:  //--- ESCape -----------------------------------------
            fin=-1;
            break;
        case 0:
            switch(HI(code))
                {
                case 0x25:  // --- ALT-K -------------------------------
                    AltK();
                    break;
                case 0x54:  //--- SHIFT-F1 -----------------------------
                case 0x3B:  //--- F1 -----------------------------------
                    HelpTopic("View");
                    break;
                case 0x5D:  //--- SHIFT-F10 ----------------------------
                case 0x44:  //--- F10 ----------------------------------
                case 0x8D:  //--- CTRL-UP ------------------------------
                    fin=-1;
                    break;
                case 0x57:  //--- SHIFT-F4 -----------------------------
                case 0x3E:  //--- F4 -----------------------------------
                    cv=ChangeViewer(3);
                    if (cv!=0)
                        fin=cv;
                    break;

                case 0x5A:    //--- SHIFT-F6 ---------------------------
                case 0x40:    //--- F6 ---------------------------------
                    ChgViewPreproc();
                    fin=104;
                    break;


                case 0x8B:  // --- ALT-F11 -----------------------------
                    fin=-2;
                    break;

                case 0x8C:  // --- ALT-F12 -----------------------------
                    ViewSetup();
                    fin=104;
                    break;
                case 80:    //--- BAS ----------------------------------
                    HTMLnext(&DLine,&PLine,&ye);
                    break;
                case 72:    //--- HAUT ---------------------------------
                    HTMLfrom(&DLine,&PLine,&ye);
                    break;
                case 0x51:  //--- PGDN ---------------------------------
                    for(l=0;l<20;l++)
                        HTMLnext(&DLine,&PLine,&ye);
                    break;
                case 0x49:  //--- PGUP ---------------------------------
                    for(l=0;l<20;l++)
                        HTMLfrom(&DLine,&PLine,&ye);
                    break;
                case 0x47:  //--- HOME ---------------------------------
                    while(PLine->from!=NULL)
                        HTMLfrom(&DLine,&PLine,&ye);
                    break;
                case 0x4F:  //--- END ----------------------------------
                    while(DLine->next!=NULL)
                        HTMLnext(&DLine,&PLine,&ye);
                    break;
                case 0xF:   //--- SHIFT-TAB ----------------------------
                    rete=suiv;
                    suiv=prem;
                    while ( (suiv->next!=rete) & (suiv->next!=NULL) )
                        suiv=suiv->next;

                    if (suiv->next==NULL)
                        {
                        rete=suiv;
                        suiv=prem;
                        while ((suiv->next!=rete) & (suiv->next!=NULL))
                            suiv=suiv->next;
                        }

                    if ((suiv->next)!=NULL)
                        {
                        iy=suiv->y1-ye;

                        if ((iy<=(-y)) | (iy>=yl-y))
                            {
                            j=abs(ye-(suiv->y1-y));
                            for(i=0;i<j;i++)
                                {
                                if (ye<suiv->y1-y)
                                    HTMLnext(&DLine,&PLine,&ye);

                                if (ye>suiv->y1-y)
                                    HTMLfrom(&DLine,&PLine,&ye);
                                }
                            }
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
                    if (car==0)
                        suiv=suiv->next;
                    break;
                    }
                }

            if (suiv->next==NULL)
                suiv=prem;

            if (suiv->next!=NULL)
                {
                iy=suiv->y1-ye;

                if ((iy<=(-y)) | (iy>=yl-y))
                    {
                    j=abs(ye-(suiv->y1-y));
                    for(i=0;i<j;i++)
                        {
                        if (ye<suiv->y1-y)
                            HTMLnext(&DLine,&PLine,&ye);

                        if (ye>suiv->y1-y)
                            HTMLfrom(&DLine,&PLine,&ye);
                        }
                    }
                }
            break;
        case 13:
            strcpy(titre,suiv->link);
            titre2=strchr(titre,'\"');

            if (titre2==NULL)
                break;

            titre2++;
            titre2[strlen(titre2)-1]=0;

            if (!strnicmp(titre2,"MAILTO:",7))
                {
                code=0;
                strcpy(chaine,"You couldn't mail to: ");
                strcat(chaine,titre2+7);
                WinError(chaine);
                break;
                }
            if (!strnicmp(titre2,"HTTP:",5))
                {
                code=0;
                strcpy(chaine,"You couldn't HTTP to: ");
                strcat(chaine,titre2+5);
                WinError(chaine);
                break;
                }
            if (!strnicmp(titre2,"FTP:",4))
                {
                code=0;
                strcpy(chaine,"You couldn't FTP to: ");
                strcat(chaine,titre2+4);
                WinError(chaine);
                break;
                }

            if (titre2[0]=='#')     //--- Internal Link ----------------
                {
                strcpy(liaison,titre2+1);
                Info.numero=104;
                fin=104;
                }
                else
                {
                Path2Abs(fichier,"..");
                Path2Abs(fichier,titre2);
                fin=-4;
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
while (fin==0);


while(prem->next!=NULL)
    {
    LibMem(prem->link);
    suiv=prem->next;
    LibMem(prem);
    prem=suiv;
    }
LibMem(prem);

while(FLine->next!=NULL)
    {
    CLine=FLine->next;
    LibMem(FLine->Chr);
    LibMem(FLine->Col);
    LibMem(FLine);
    FLine=CLine;
    }
LibMem(FLine);
LibMem(FLine->Chr);
LibMem(FLine->Col);

SavePosition(fichier,1);    //--- Pour l'historique aprŠs --------------

if (fin==-2)
    {
    char oldfic[256];
    strcpy(oldfic,fichier);
    GetListFile(fichier);
    if (fichier[0]==0)
        strcpy(fichier,oldfic);
    }



LoadScreen();

if (fin==-4)
    fin=-2;

return fin;
}





/*--------------------------------------------------------------------*\
|- Gestion du filtre pour l'affichage texte                           -|
\*--------------------------------------------------------------------*/
void Masque(short x1,short y1,short x2,short y2,char *bufscr)
{
char ok=1;
char *chaine;
char chain2[132];
short m1,m2;

uchar c,c2;
short x,y,l;
short oldx,oldy,oldl;
short xt[80],yt[80];

int y3;

char cont,trouve=0;

struct PourMask *CMask;

CMask=ViewCfg->Mask[(ViewCfg->wmask)&15];

chaine=CMask->chaine;


if ((((ViewCfg->wmask)&128)==128) | (*chaine==0))
    {
    int x,y,y3;

    for(y=y1;y<=y2;y++)
        {
        y3=y*Cfg->TailleX;
        for(x=x1;x<=x2;x++)
            AffChr(x,y,bufscr[x+y3]);
        }
    return;
    }

x=x1;
y=y1;
y3=y*(Cfg->TailleX);

l=0;

while ((y<=y2) | (ok==0) )
    {
    if (ok)
        c=bufscr[x+y3];
        else
        c=32;

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
            cont=(trouve==2);
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
                    y3=y*(Cfg->TailleX);
                    }

                while (l!=0)
                    {
                    l--;
                    AffCol(xt[l],yt[l],c2);
                    AffChr(xt[l],yt[l],bufscr[xt[l]+(yt[l])*(Cfg->TailleX)]);
                    }
                l=0;
                }
            }
        if (ok)
            {
            AffCol(x,y,Cfg->col[16]);       // Ou autre chose
            AffChr(x,y,c);
            }
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
        y3=y*(Cfg->TailleX);
        ok=1;
        if (KbHit()) break;
        }
    }

/*--------------------------------------------------------------------*\
|- Affichage de la chaine qui a ‚t‚ trouv‚ (par F7)                   -|
\*--------------------------------------------------------------------*/

if (*srcch!=0)
    {
    y=y1;
    for (x=x1;x<=x2;x++)
        chain2[x-x1]=bufscr[x+y1*(Cfg->TailleX)];
    chain2[x2-x1+1]=0;

    for (x=0;x<=(x2-x1)-strlen(srcch)+1;x++)
        if (!strnicmp(chain2+x,srcch,strlen(srcch)))
            for(l=0;l<strlen(srcch);l++)
                {
                AffCol(x1+l+x,y1,Cfg->col[18]);     //Reverse
                AffChr(x1+l+x,y1,bufscr[(x1+l+x)+y1*(Cfg->TailleX)]);
                }
    }

}


/*--------------------------------------------------------------------*\
|- Changement du filtre pour l'affichage du texte                     -|
\*--------------------------------------------------------------------*/
void ChangeMask(void)
{
MENU menu;
int nbr;
char bars1[25];
static struct barmenu bar[19];
int retour,x,y,n,i,max;

char fin;

max=0;

nbr=2;

for(i=0;i<16;i++)
    if (strlen(ViewCfg->Mask[i]->title)>0)
        {
        bar[nbr].fct=i+10;

        bar[nbr].Titre=ViewCfg->Mask[i]->title;
        bar[nbr].Help=NULL;

        if ( ((ViewCfg->wmask)&15) ==i) n=nbr;

        if (strlen(ViewCfg->Mask[i]->title)>max) max=strlen(ViewCfg->Mask[i]->title);

        nbr++;
        }

x=((Cfg->TailleX)-max)/2;
y=((Cfg->TailleY)-2*(nbr-2))/2;
if (y<2) y=2;

do
{
sprintf(bars1,"Mask %3s",(ViewCfg->wmask&128)==128 ? "OFF" : "ON");
bar[0].Titre=bars1;
bar[0].Help=NULL;
bar[0].fct=2;

bar[1].fct=0;

fin=0;

SaveScreen();

do
    {
    menu.x=x;
    menu.y=y;
    menu.cur=n;
    menu.attr=4+8;

    retour=PannelMenu(bar,nbr,&menu);

    n=menu.cur;
    }
while ( (retour==1) | (retour==-1) );

LoadScreen();

if (retour==2)
    {
    switch (bar[n].fct)
        {
        case 1:
            ViewCfg->wmask^=64;
            break;
        case 2:
            ViewCfg->wmask^=128;
            break;
        case 3:
            break;
        default:
            ViewCfg->wmask=((ViewCfg->wmask)&240)|(bar[n].fct-10);
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

sprintf(bars0,"Normal %c",ViewCfg->cnvtable==0 ? 15 : 32);
bar[0].Titre=bars0;
bar[0].Help=NULL;
bar[0].fct=1;

sprintf(bars1,"BBS    %c",ViewCfg->cnvtable==1 ? 15 : 32);
bar[1].Titre=bars1;
bar[1].Help=NULL;
bar[1].fct=2;

sprintf(bars2,"Latin  %c",ViewCfg->cnvtable==2 ? 15 : 32);
bar[2].Titre=bars2;
bar[2].Help=NULL;
bar[2].fct=3;

bar[3].fct=0;

sprintf(bars4,"%s",ViewCfg->autotrad ? "Auto" : "No Auto");
bar[4].Titre=bars4;
bar[4].Help=NULL;
bar[4].fct=10;

n=0;
if (ViewCfg->autotrad) n=4;
    else
    while (ViewCfg->cnvtable!=(bar[n].fct)-1)
        n++;

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
            ViewCfg->cnvtable=bar[n].fct-1;
            ViewCfg->autotrad=0;
            break;
        case 10:
            ViewCfg->autotrad^=1;
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

bar[2].Titre="Mac (CR)";
bar[2].Help=NULL;
bar[2].fct=2;

bar[3].Titre="Mixed (CR-LF)";
bar[3].Help=NULL;
bar[3].fct=5;

bar[4].fct=4;

n=0;

while (ViewCfg->lnfeed!=(bar[n].fct)-1)
    n++;

SaveScreen();

do
    {
    sprintf(bars4,"User Line Feed: $%02X",ViewCfg->userfeed);
    bar[4].Titre=bars4;
    bar[4].Help=NULL;

    menu.x=x;
    menu.y=y;
    menu.cur=n;
    menu.attr=4;

    retour=PannelMenu(bar,nbr,&menu);

    n=menu.cur;

    if ((bar[n].fct==4) & (retour!=2))
        ViewCfg->userfeed+=retour;
    }
while ( (retour==1) | (retour==-1) );

LoadScreen();

if (retour==2)
    {
    ViewCfg->lnfeed=(bar[n].fct)-1;
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


/*--------------------------------------------------------------------*\
|-  Retourne 1 si tout va bien                                        -|
\*--------------------------------------------------------------------*/

char PRN_print(short lpt,char a)
{
union REGS regs;
char code,cont;
char result=1;

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

if (m==-1)  //--- escape -----------------------------------------------
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
if (y<MAXANSLINE)
    AnsiBuffer[y*160+x]=(char)c;
}

void BufAffCol(long x,long y,long c)
{
if (y<MAXANSLINE)
    AnsiBuffer[y*160+x+80]=(char)c;
}


void StartAnsiPage(void)
{
AnsiAffChr=BufAffChr;
AnsiAffCol=BufAffCol;

AnsiBuffer=(char*)GetMem(160*MAXANSLINE);

Ansi1=0;

maxx=80;
maxy=Cfg->TailleY;

curx=0;
cury=0;

if (raw==0)
    ReadANS();
    else
    ReadRAW();

Ansi2=cury+1-Cfg->TailleY+2;
if (Ansi2<0) Ansi2=0;
}

void DispAnsiPage(int x1,int y1,int x2,int y2)
{
int x,y;
static char cc=0;

char col;

cc++;
if (cc==16) cc=0;

for(y=y1;y<y2;y++)
    {
    for(x=x1;x<x2;x++)
        {
        col=AnsiBuffer[(y+Ansi1)*160+80+x];

        AffChr(x,y,AnsiBuffer[(y+Ansi1)*160+x]);

        if (((col&15)==(col/16)) & (secpart))
            AffCol(x,y,(col&240)+cc);
            else
            AffCol(x,y,col);
        }
    if (KbHit()) break;
    }
}

void CloseAllPage(void)
{
LibMem(AnsiBuffer);
}


/*--------------------------------------------------------------------*\
|- Affichage ansi                                                     -|
\*--------------------------------------------------------------------*/
int AnsiView(char *fichier)
{
clock_t cl,cl2;

int xm,ym,zm;

int aff,wrap;

int cv;

char autodown=0;
char car;


int code;
int fin=0;

int warp=0;

int shift=0; //--- Vaut -1 si reaffichage de l'ecran -------------------
             //---       0 si pas de shift -----------------------------
             //---       1 si touche shiftee ---------------------------


static char bar[81];

static char oldpal[48],oldcol[64];
static int tx,ty,font;

char pal[]="\x00\x00\x00\x00\x00\x2A\x00\x2A\x00\x00\x2A\x2A"
                   "\x2A\x00\x00\x2A\x00\x2A\x2A\x15\x00\x2A\x2A\x2A"
                   "\x15\x15\x15\x15\x15\x3F\x15\x3F\x15\x15\x3F\x3F"
                   "\x3F\x15\x15\x3F\x15\x3F\x3F\x3F\x15\x3F\x3F\x3F";
char col[]="\x1B\x30\x1E\x3E\x1E\x03\x30\x30\x0F\x30\x3F\x3E"
                   "\x0F\x0E\x30\x19\x1B\x13\x30\x3F\x3E\x0F\x15\x12"
                   "\x30\x3F\x0F\x3E\x4F\x4E\x70\x00\x14\x13\xB4\x60"
                   "\x70\x1B\x1B\x1B\x1E\x30\x30\x0F\x3F\x3F\x4F\x4F"
                   "\x3F\x0F\x80\x90\x30\x30\x0E\x1B\x1B\xA0\xB0\xC0"
                   "\x04\x03\x05\x07";

SaveScreen();
PutCur(3,0);

memcpy(oldpal,Cfg->palette,48);
memcpy(oldcol,Cfg->col,64);
tx=Cfg->TailleX;
ty=Cfg->TailleY;
font=Cfg->font;

memcpy(Cfg->palette,pal,48);
memcpy(Cfg->col,col,64);

Cfg->TailleX=80;
Cfg->font=0;

TXTMode();

StartAnsiPage();

wrap=0;
aff=1;


/*--------------------------------------------------------------------*\
|- Affichage de la bar                                                -|
\*--------------------------------------------------------------------*/

strcpy
   (bar," Help Spart  ---- ChView ---- Prepro ----  ----  ----  Quit ");


secpart=0;

Bar(bar);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

cl=clock();

do
{


/*--------------------------------------------------------------------*\
|- Gestion des touches                                                -|
\*--------------------------------------------------------------------*/

zm=0;
code=0;

while ( (!KbHit()) & (zm==0) & (code==0) )
{
GetPosMouse(&xm,&ym,&zm);

car=*Keyboard_Flag1;

if (((car&3)!=0) | (ym==Cfg->TailleY-1))
    {
    int prc;
    char temp[132];

    if (Ansi2!=0)
        prc=(Ansi1*100)/Ansi2;
        else
        prc=100;

    ColLin(0,0,Cfg->TailleX,3*16+0);

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
    DispAnsiPage(0,1,Cfg->TailleX,(Cfg->TailleY)-1); // Display page ---
    }
    else
    {
    if (shift==1)
        {
        shift=-1;
        break;
        }
    DispAnsiPage(0,0,Cfg->TailleX,Cfg->TailleY);     // Display page ---
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
    int x,y,button,button2;

    GetPosMouse(&x,&y,&button);


    if (((button&3)!=0) & (y==Cfg->TailleY-1))
        {
        do
            GetPosMouse(&x,&y,&button2);
        while((button2&3)!=0);
        }

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
        switch(HI(code))
            {
            case 0x25:  // --- ALT-K -----------------------------------
                AltK();
                break;
            case 0x54:    //--- SHIFT-F1 -------------------------------
            case 0x3B:    //--- F1 -------------------------------------
                HelpTopic("View");
                break;
            case 0x57:    //--- SHIFT-F4 -------------------------------
            case 0x3E:    //--- F4 -------------------------------------
                cv=ChangeViewer(2);
                if (cv!=0)
                    fin=cv;
                break;

            case 0x55:    //--- SHIFT-F2 -------------------------------
            case 0x3C:    //--- F2 -------------------------------------
                secpart^=1;
                bar[11]= (secpart) ? SELCHAR : ' ';
                break;

            case 0x5A:    //--- SHIFT-F6 -------------------------------
            case 0x40:    //--- F6 -------------------------------------
                ChgViewPreproc();
                fin=86;
                break;

            case 0x8B:  // --- ALT-F11 ---------------------------------
                fin=-2;
                break;

            case 0x8C:  // --- ALT-F12 ---------------------------------
                ViewSetup();
                fin=86;
                break;
            case 80:      //--- DOWN -----------------------------------
                Ansi1++;
                break;
            case 72:      //--- UP -------------------------------------
                if (Ansi1!=0)
                    Ansi1--;
                break;
            case 0x51:    //--- PGDN -----------------------------------
                Ansi1+=Cfg->TailleY;
                break;
            case 0x49:    //--- PGUP -----------------------------------
                Ansi1-=Cfg->TailleY;
                if (Ansi1<0) Ansi1=0;
                break;
            case 0x4F:    //--- END ------------------------------------
                Ansi1=Ansi2;
                break;
            case 0x47:    //--- HOME -----------------------------------
                Ansi1=0;
                break;
            case 0x5D:    //--- SHIFT-F10 ------------------------------
            case 0x44:    //--- F10 ------------------------------------
            case 0x8D:    //--- CTRL-UP --------------------------------
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

if ((Ansi1+Cfg->TailleY)>MAXANSLINE)
    Ansi1=MAXANSLINE-Cfg->TailleY;

}
else
{
if (shift==-1)
    {
    shift=0;
//    LoadScreen();
    }
if (shift==2)
    {
    shift=1;
    }
}
}
while(!fin);

CloseAllPage();

memcpy(Cfg->palette,oldpal,48);
memcpy(Cfg->col,oldcol,64);

Cfg->TailleY=ty;
Cfg->TailleX=tx;
Cfg->font=font;

ChangeSize();

if (fin==-2)
    GetListFile(fichier);

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

buffer=(char*)GetMem(Cfg->TailleX*Cfg->TailleY);
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

sprintf(buffer,"Try XOR %d",kbest);
WinError(buffer);

LibMem(buffer);

}

int LoadFile(char *fichier,int i)
{
RB_IDF Info;

if (fic!=NULL)
    fclose(fic);

fic=fopen(fichier,"rb");
if (fic==NULL)
    return -1;

strupr(fichier);

if (strlen(fichier)>36)
    {
    memcpy(namebuf,fichier,3);
    memcpy(namebuf+3,"...",3);
    memcpy(namebuf+6,fichier+strlen(fichier)-30,30);
    namebuf[36]=0;
    }
else
    strcpy(namebuf,fichier);

fseek(fic,0,SEEK_END);
sizefic=ftell(fic);

taille=sizefic;
firstoct=fromoct;

if ((nbroct!=-1) & (nbroct<=taille-firstoct))
    taille=nbroct;
    else
    taille-=firstoct;

if ((taille<=0) | (firstoct<0))
    return -1;

pos=0;
ReadBlock();

posn=0;

if (i==-2)
    {
    Info.path[0]=0;
    Info.inbuf=view_buffer;
    if (taille<sizebuf)
        Info.buflen=taille;
        else
        Info.buflen=sizebuf;

//    strcpy(Info.path,fichier);

    Traitefic(&Info);

    if (Info.numero==-2)
        i=-3;       //--- Binaire alors --------------------------------
        else
        i=Info.numero;
    }

return i;
}


/*--------------------------------------------------------------------*\
|- Fonction principale du viewer                                      -|
\*--------------------------------------------------------------------*/

void View(KKVIEW *V,char *file,int type)
{
static char fichier[256];
char *liaison;
int n,o;
int i;
extern struct key K[nbrkey];

fic=NULL;

raw=0;

fromoct=0;
nbroct=-1;

sizebuf=32768;     //--- Taille Maximum du buffer de lecture -----------
view_buffer=(char*)GetMem(sizebuf); //--Allocation du buffer de lecture-

ViewCfg=V;

SaveScreen();

Bar(" ----  ----  ---- ChView ----  ----  ----  ----  ----  ---- ");

strcpy(fichier,file);
if (fichier[0]==0)
    GetListFile(fichier);

liaison=(char*)GetMem(256);
strcpy(liaison,"");


switch(type)
    {
    case 0:
        i=-2;
        break;
    case 1:
        i=86;
        break;
    case 2:
        i=104;
        break;
    case 3:
        i=-1;
        break;
    case 4:
        i=91;
        break;
    case 5:
        i=86;
        raw=1;
        break;
    default:
        i=-1;
        WinError("You couldn't arrive here ! Weird...");
        break;
    }

while(i!=-1)
    {
    i=LoadFile(fichier,i);

    switch(i)
        {
        case 86:  // Ansi
            i=AnsiView(fichier);
            break;
        case 104: // HTML
            i=HtmlView(fichier,liaison);
            break;
        case 37:  // GIF
        case 38:  // JPG
            {
//            static char buffer[256];
//            FicIdf(buffer,fichier,i,0);
//            CommandLine(buffer);
            i=-1;
            break;
            }
        case -1:
            break;
        default:
            o=-1;
            for(n=0;n<nbrkey;n++)
                if (K[n].numero==i) o=n;

            if (o==-1)
                {
                i=HexaView(fichier);
                }
                else
                {
                if ((K[o].other&4)==4)
                    i=TxtView(fichier);
                    else
                    i=HexaView(fichier);
                }
            break;
        }
    }

if (fic!=NULL)
    fclose(fic);

LibMem(liaison);
LibMem(view_buffer);

LoadScreen();
}

// marjo

void ViewSetup(void)
{

int res;

//--- Internal Variable ---

static char KKTmpX0=29;
static int KKTmpY0=4;
static int KKTmp9=5;
static char KKTmpX12=16;
static int KKTmpY12=6;

//--- User Field ---

static int l1,l2;

static int KKField1;
static int KKField2;
static int KKField5;
static char KKField9[6];

struct TmtWin F = {-1,9,51,18,"Viewer"};

struct Tmt T[] = {
    { 1, 1, 9,&KKTmpX0,&KKTmpY0},
    { 3, 2, 8,"Auto adjust viewer size",&KKField1},
    { 3, 3, 8,"Save position in viewer",&KKField2},
    { 3, 4, 7,"First byte:   ",&l1},
    { 3, 5, 7,"Size of file: ",&l2},
    { 2, 7, 2,NULL,NULL},
    {17, 7, 3,NULL,NULL},
    {34, 3,10,"DOS (CR/LF)",&KKField5},
    {34, 4,10,"Unix (CR)",&KKField5},
    {34, 5,10,"LF",&KKField5},
    {34, 6,10,"User",&KKField5},
    {43, 6, 1,KKField9,&KKTmp9},
    {34, 7,10,"Mixed CR-LF",&KKField5},
    {33, 2, 0,"Line Feed:",NULL},
    {32, 1, 9,&KKTmpX12,&KKTmpY12}
   };

l1=fromoct;
l2=nbroct;

KKField1=ViewCfg->ajustview;
KKField2=ViewCfg->saveviewpos;
KKField5=5;
sprintf(KKField9,"%d",ViewCfg->userfeed);

res=WinTraite(T,15,&F,3);

if (res==-1)
    return;

if (T[res].type==3)
    return;

fromoct=l1;
nbroct=l2;

ViewCfg->ajustview=KKField1;
ViewCfg->saveviewpos=KKField2;
ViewCfg->lnfeed=(KKField5-5);
sscanf(KKField9,"%d",&l1);
ViewCfg->userfeed=l1;
}

#define MAXFILE 500

void GetListFile(char *fichier)
{
FILE *fic;
uchar n;
long s,size;
static char ficname[256];

static struct barmenu *bar;
int nbrbar=0;
MENU menu;

char err;

bar=(struct barmenu*)GetMem(sizeof(struct barmenu)*MAXFILE);

if (ViewCfg->saveviewpos==1)
    {
    fic=fopen(ViewCfg->viewhist,"rb");
    if (fic!=NULL)
        {
        while(fread(&n,1,1,fic)==1)
            {
            fread(ficname,1,n,fic);
            fread(&size,4,1,fic);
            fread(&s,4,1,fic);

/*
            if ( (!stricmp(ficname,fichier)) & (size==taille) )
                {
                posdeb=s;
                break;
                }
*/

            bar[nbrbar].Titre=(char*)GetMem(strlen(ficname)+1);
            strcpy(bar[nbrbar].Titre,ficname);
            bar[nbrbar].Help=NULL;
            bar[nbrbar].fct=nbrbar+1;
            nbrbar++;
            if (nbrbar==MAXFILE) break;
            }
        fclose(fic);
        }
    }

menu.x=2;
menu.y=2;

menu.attr=2+8;

menu.cur=nbrbar-1;

err=0;

if (nbrbar==0)
    err=1;

if (nbrbar>1)
    if (PannelMenu(bar,nbrbar,&menu)!=2)
        err=1;

if (!err)
    {
    Path2Abs(fichier,"..");
    Path2Abs(fichier,bar[menu.cur].Titre);
    }
    else
    fichier[0]=0;
}

/*--------------------------------------------------------------------*\
|- Gestion du preprocesseur pour la traduction                        -|
\*--------------------------------------------------------------------*/

#define NBPREPROC 11
#define NBMAXPROC 16

char GetPreprocValue(char car)
{
char str[1];
str[0]=car;
GetPreprocString(str,1);
return str[0];
}

void GetPreprocString(char *car,int lng)
{
int n=0;
int x;

while ( ((ViewCfg->Traduc[n])!=0) & (n<32))
    {
    switch(ViewCfg->Traduc[n])
        {
        case 1: //--- XOR ----------------------------------------------
            for(x=0;x<lng;x++)
                (car[x])^=(ViewCfg->Traduc[n+1]);
            break;
        case 2: //--- AND ----------------------------------------------
            for(x=0;x<lng;x++)
                (car[x])&=(ViewCfg->Traduc[n+1]);
            break;
        case 3: //--- OR -----------------------------------------------
            for(x=0;x<lng;x++)
                (car[x])|=(ViewCfg->Traduc[n+1]);
            break;
        case 4: //--- NEG ----------------------------------------------
            for(x=0;x<lng;x++)
                (car[x])=-(car[x]);
            break;
        case 5: //--- ROR ----------------------------------------------
            for(x=0;x<lng;x++)
                car[x]=ROR(car[x],ViewCfg->Traduc[n+1]);
            break;
        case 6: //--- ADD ----------------------------------------------
            for(x=0;x<lng;x++)
                (car[x])+=ViewCfg->Traduc[n+1];
            break;
        case 7: //--- Latin --------------------------------------------
            for(x=0;x<lng;x++)
                car[x]=TabDosWin[car[x]];
            break;
        case 8: //--- BBS ----------------------------------------------
            for(x=0;x<lng;x++)
                car[x]=TabBBS[car[x]];
            break;
        case 9: //--- Elite --------------------------------------------
            for(x=0;x<lng;x++)
                {
                car[x]=toupper(car[x]);
                switch(car[x])
                    {
                    case 'A':
                    case 'E':
                    case 'I':
                    case 'O':
                    case 'U':
                    case 'Y':
                        (car[x])+='a'-'A';
                        break;
                    }
                }
            break;
        case 10: //--- DOS->KKC font -----------------------------------
            if (Cfg->font==0)
                break;
            for(x=0;x<lng;x++)
                car[x]=TabInt[car[x]];
            break;
        case 11: //--- Win->dos -----------------------------------------
            for(x=0;x<lng;x++)
                car[x]=TabWinDos[car[x]];
            break;
        }
    n+=2;
    }
return;
}

/*--------------------------------------------------------------------*\
|-  Taille: 40 maximum                                                -|
\*--------------------------------------------------------------------*/

void GetNamePreproc(char n,char opt,char *chaine)
{
switch(n)
    {
    case 1:
        sprintf(chaine,"XOR %3d (0x%02X)",opt,opt);
        break;
    case 2:
        sprintf(chaine,"AND %3d (0x%02X)",opt,opt);
        break;
    case 3:
        sprintf(chaine,"OR  %3d (0x%02X)",opt,opt);
        break;
    case 4:
        sprintf(chaine,"NEG");
        break;
    case 5:
        sprintf(chaine,"ROR %3d (0x%02X)",opt,opt);
        break;
    case 6:
        sprintf(chaine,"ADD %3d (0x%02X)",opt,opt);
        break;
    case 7:
        sprintf(chaine,"Latin");
        break;
    case 8:
        sprintf(chaine,"BBS");
        break;
    case 9:
        sprintf(chaine,"Elite");
        break;
    case 10:
        sprintf(chaine,"Internal Filter");
        break;
    case 11:
        sprintf(chaine,"Windows -> Dos");
        break;
    }
}

int Add1Fct(struct barmenu *bar)
{
int n;

if (bar->fct<NBMAXPROC*2)
    {
    n=bar->fct;
    (ViewCfg->Traduc[n])++;
    GetNamePreproc(ViewCfg->Traduc[n-1],ViewCfg->Traduc[n],bar->Titre);
    }
return 0;
}

int Add10Fct(struct barmenu *bar)
{
int n;

if (bar->fct<NBMAXPROC*2)
    {
    n=bar->fct;
    (ViewCfg->Traduc[n])+=0x10;
    GetNamePreproc(ViewCfg->Traduc[n-1],ViewCfg->Traduc[n],bar->Titre);
    }
return 0;
}

int Sub1Fct(struct barmenu *bar)
{
int n;

if (bar->fct<NBMAXPROC*2)
    {
    n=bar->fct;
    (ViewCfg->Traduc[n])--;
    GetNamePreproc(ViewCfg->Traduc[n-1],ViewCfg->Traduc[n],bar->Titre);
    }
return 0;
}

int Sub10Fct(struct barmenu *bar)
{
int n;

if (bar->fct<NBMAXPROC*2)
    {
    n=bar->fct;
    (ViewCfg->Traduc[n])-=0x10;
    GetNamePreproc(ViewCfg->Traduc[n-1],ViewCfg->Traduc[n],bar->Titre);
    }
return 0;
}


void ChgViewPreproc()
{
KKVIEW *V;
int nbr;
static struct barmenu bar[NBPREPROC+NBMAXPROC+1];
int retour,max,x,y,m,n;
MENU menu;

V=ViewCfg;

for(n=0;n<NBPREPROC+NBMAXPROC+1;n++)
    bar[n].Titre=GetMem(40);

menu.attr=2;    //--- No shortcut --------------------------------------
menu.cur=0;

NewEvents(Sub1Fct, "Sub 01",5);
NewEvents(Add1Fct, "Add 01",6);

NewEvents(Sub10Fct,"Sub 10",7);
NewEvents(Add10Fct,"Add 10",8);


do
{
n=0;
nbr=0;

while (((V->Traduc[n])!=0) & (nbr<NBMAXPROC))
    {
    GetNamePreproc(V->Traduc[n],V->Traduc[n+1],bar[nbr].Titre);
    bar[nbr].Help=NULL;
    bar[nbr].fct=n+1;   //--- Emplacement de l'option ------------------
    n+=2;
    nbr++;
    }

bar[nbr].fct=0; //-- Separateur ----------------------------------------
nbr++;

for(x=0;x<NBPREPROC;x++)
    {
    bar[nbr].fct=NBMAXPROC*2+x+1;
    GetNamePreproc(x+1,0,bar[nbr].Titre);
    nbr++;
    }

/*-------- Redimension de la fenˆtre ---------*/
max=0;
for(n=0;n<nbr;n++)
    if (strlen(bar[n].Titre)>max)
        max=strlen(bar[n].Titre);

x=((Cfg->TailleX)-max)/2;
y=((Cfg->TailleY)-nbr)/2;
if (y<2) y=2;

menu.x=x;
menu.y=y;

if (bar[menu.cur].fct==0)
    menu.cur++;

retour=PannelMenu(bar,nbr,&menu);

m=menu.cur;

if (retour==-1) //--- En haut ------------------------------------------
    {
    if (bar[m].fct>NBMAXPROC*2) //--- Nouveau --------------------------
        {
        n=0;
        while (((V->Traduc[n])!=0) & (n<NBMAXPROC*2))
            n+=2;
        if (n<NBMAXPROC*2)
            {
            V->Traduc[n]=(bar[m].fct)-NBMAXPROC*2;
            V->Traduc[n+1]=0;
            if (n<30)
                V->Traduc[n+2]=0;
            menu.cur=n/2;
            }
        }
        else
    if (bar[m].fct!=0) //--- Deplacement -------------------------------
        {
        char t1,t2;
        int pos;

        pos=bar[m].fct-1;
        if (pos!=0)
            {
            t1=V->Traduc[pos];
            t2=V->Traduc[pos+1];

            V->Traduc[pos]=V->Traduc[pos-2];
            V->Traduc[pos+1]=V->Traduc[pos-1];

            V->Traduc[pos-2]=t1;
            V->Traduc[pos-1]=t2;
            menu.cur--;
            }
        }
    }

if (retour==1) //--- En bas --------------------------------------------
    {
    if (bar[m].fct<NBMAXPROC*2)
        {
        char t1,t2;
        int pos;

        pos=bar[m].fct-1;

        if (pos<30)
            {
            t1=V->Traduc[pos+2];
            t2=V->Traduc[pos+3];

            V->Traduc[pos+2]=V->Traduc[pos];
            V->Traduc[pos+3]=V->Traduc[pos+1];
            }
            else
            {
            t1=0;
            t2=0;
            }

        V->Traduc[pos]=t1;
        V->Traduc[pos+1]=t2;

        menu.cur++;
        }
    }
}
while ((retour!=2) & (retour!=0));

ClearEvents();

for(n=0;n<NBPREPROC+NBMAXPROC+1;n++)
    LibMem(bar[n].Titre);

ReadBlock();
}
