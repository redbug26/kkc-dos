#include <direct.h>
#include <i86.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <bios.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include <signal.h> // For handling signal

#include <dos.h>    //  Pour redirection des I/O

#include "idf.h"
#include "win.h"
#include "kk.h"

/*---------------------*
 * Declaration interne *
 *---------------------*/

void Fin(void);

void SaveSel(struct fenetre *F1);
void LoadSel(struct fenetre *F1,int n);

/*---------------------------*
 * Declaration des variables *
 *---------------------------*/

sig_atomic_t signal_count;

char Select_Chaine[16]="*.*";

char *Screen_Buffer;
char *Screen_Adr=(char*)0xB8000;

char *Keyboard_Flag1=(char*)0x417;

char *ShellAdr=(char*)0xBA000;

struct fenetre *Fenetre[2];

struct fenetre *DFen;


char *RBTitle="Ketchup Killers Commander V0.11 / RedBug";

int IOver;
int IOerr;


/*-------------------------*
 * Procedure en Assembleur *
 *-------------------------*/

char GetDriveReady(char i);
#pragma aux GetDriveReady = \
	"mov ah,19h" \
	"int 21h" \
	"mov ch,al" \
	"mov ah,0Eh" \
	"int 21h" \
	"mov ah,19h" \
	"int 21h" \
	"sub al,dl" \
	"mov cl,al" \
	"mov dl,ch" \
	"mov ah,0Eh" \
	"int 21h" \
	parm [dl] \
	value [cl];


/*------------------*
 * Other Procedures *
 *------------------*/

//---------------- Error and Signal Handler -----------------------------

int __far Error_handler(unsigned deverr,unsigned errcode,unsigned far *devhdr)
{
IOerr=1;

if (IOver==1)
    return _HARDERR_FAIL;


SaveEcran();

WinCadre(29,9,48,14,0);
ColWin(30,10,47,13,0*16+1);
ChrWin(30,10,47,13,32);

PrintAt(31,10,"   ERROR (%04X,%04X)",errcode,deverr);
PrintAt(31,12,"Press a key when");
PrintAt(31,13,"  it is correct");
Wait(0,0,0);

ChargeEcran();
return _HARDERR_FAIL;
}

// c=0 --> change from selection
//   1 --> default set of color
//   2 --> norton set
void ChangePalette(char c)
{
int x,y,i;
char rec;

int n,m,ntot;
int nt,mt;
int car;

int ex,ey;
int x1,y1;

char *titre="Palette configuration";

#define NBRS 3

char defcol[NBRS][48]={ {43,37,30, 31,22,17,  0, 0, 0, 58,58,50,
                      44,63,63, 63,63,21, 43,37,30,  0, 0, 0,
                      63,63, 0, 63,63,63, 43,37,30,  0, 0, 0,
                       0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0},
                     { 0, 0, 0, 43,43,43,  0, 0, 0, 63,63,63,
                      63,63,63, 63,63,21, 50,58,55,  0, 0,43,
                      63,63, 0, 63,63,63,  0, 0,43,  0, 0, 0,
                       0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0},
                     {25,36,29, 36,18,15,  0, 0, 0, 49,39,45,
                      44,63,63, 42,37,63, 45,39,35,  0, 0, 0,
                       0,63,63, 63,63,63, 25,36,29,  0, 0, 0,
                       0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0} };

char *Style[NBRS]={"Default Style","Norton Style","Cyan Style"};
int posx[NBRS],posy[NBRS];

if (c>0)
    {
    memcpy(Cfg->palette,defcol[c-1],48);
    LoadPal();
    return;
    }

SaveEcran();

WinCadre(0,0,79,(Cfg->TailleY)-2,2);
ColWin(1,1,78,(Cfg->TailleY)-3,10*16+1);
ChrWin(1,1,78,(Cfg->TailleY)-3,32);

if (Cfg->TailleY==50)
    {
    x1=9;
    y1=7;
    ey=7;
    ex=24;
    for (n=0;n<NBRS;n++)
        {
        posx[n]=56;
        posy[n]=n*3+24;
        }
    }
    else
    {
    x1=4;
    y1=2;
    ey=4;
    ex=19;
    for (n=0;n<NBRS;n++)
        {
        posx[n]=n*19+4;
        posy[n]=(Cfg->TailleY)-4;
        }
    }



ntot=15+NBRS;

for(n=0;n<ntot-15;n++)
    {
    WinCadre(posx[n]-2,posy[n]-1,posx[n]+15,posy[n]+1,2);
    ColLin(posx[n],posy[n],strlen(Style[n]),10*16+1);
    PrintAt(posx[n],posy[n],Style[n]);
    }

if (Cfg->TailleY==50)
    {
    WinCadre(2,2,77,4,2);
    WinCadre(2,5,77,44,2);
    PrintAt(30,3,titre);

    WinCadre(2,45,77,47,2);

    WinCadre(52,16,73,40,2);
    PrintAt(5,46,RBTitle);

    WinCadre(53,17,72,21,2);
    PrintAt(54,19,"Predefined palette");
    }
    else
    {
    PrintAt(30,0,titre);
    }


n=0;
m=0;


i=0;

rec=1;

do
{
if (rec==1)
    {
    rec=0;
    for (nt=0;nt<16;nt++)
        {
        x=(nt/ey)*ex+x1;
        y=(nt%ey)*5+y1;

        WinCadre(x-1,y-1,x+9,y+3,1);
        ColWin(x,y,x+8,y+2,1*16+5);
        ChrWin(x,y,x+8,y+2,32);

        WinCadre(x+10,y-1,x+14,y+3,1);
        ColWin(x+11,y,x+13,y+2,nt*16+nt);
        ChrWin(x+11,y,x+13,y+2,220);

        for(mt=0;mt<3;mt++)
            {
            Gradue(x,y+mt,8,0,Cfg->palette[nt*3+mt],64);
            ColWin(x,y+mt,x+8,y+mt,1*16+2);
            PrintAt(x-3,y+mt,"%02d",Cfg->palette[nt*3+mt]);
            }
        }
    }

if (n<16)
    {
    i=Cfg->palette[n*3+m];
    if (i!=0) i--;

    x=(n/ey)*ex+x1;
    y=(n%ey)*5+y1;

    Gradue(x,y+m,8,i,Cfg->palette[n*3+m],64);
    PrintAt(x-3,y+m,"%02d",Cfg->palette[n*3+m]);

    ColWin(x,y+m,x+8,y+m,1*16+5);

    car=Wait(0,0,0);

    ColWin(x,y+m,x+8,y+m,1*16+2);

    switch(HI(car))
        {
        case 0x47:  // HOME
            ChrWin(x,y+m,x+8,y+m,32);
            Cfg->palette[n*3+m]=0;
            break;
        case 0X4F: // END
            ChrWin(x,y+m,x+8,y+m,32);
            Cfg->palette[n*3+m]=63;
            break;
        case 80:    // bas
            m++;
            break;
        case 72:    // haut
            m--;
            break;
        case 0x4B:  // gauche
            if (Cfg->palette[n*3+m]!=0)
                Cfg->palette[n*3+m]--;
            break;
        case 0x4D:  // droite
            if (Cfg->palette[n*3+m]!=63)
                Cfg->palette[n*3+m]++;
            break;
        case 0xF:   // SHIFT-TAB
            n--;
            break;
        }

    switch(LO(car))
        {
        case 9:
            n++;
            break;
        }

    if (m==3) n++,m=0;
    if (m<0)  n--,m=2;
    if (n<0)  n=ntot;

    LoadPal();
    }
    else
    {
    ColLin(posx[n-16],posy[n-16],strlen(Style[n-16]),10*16+5);

    car=Wait(0,0,0);

    ColLin(posx[n-16],posy[n-16],strlen(Style[n-16]),10*16+1);

    switch(HI(car))
        {
        case 80:    // bas
            n++;
            break;
        case 72:    // haut
            n--;
            break;
        case 0xF:   // SHIFT-TAB
            n--;
            break;
        }

    switch(LO(car))
        {
        case 9:
            n++;
            break;
        case 13:
            memcpy(Cfg->palette,defcol[n-16],48);
            LoadPal();
            rec=1;
            break;
        }

    if (n<0)        n=ntot;
    if (n>ntot)     n=0;

    }
}
while(car!=27);


ChargeEcran();

}



void Signal_Handler(int sig_no)
{
signal_count++;

WinCadre(29,9,48,14,0);
ColWin(30,10,47,13,0*16+1);
ChrWin(30,10,47,13,32);

PrintAt(31,10,"Signal: %d",sig_no);
PrintAt(31,12,"Press a key when");
PrintAt(31,13,"  it is correct");
Wait(0,0,0);

exit(1);
}

//-----------------------------------------------------------------------

void ChangeTaille(int i)
{
int n;

if (i==0)
    switch(Cfg->TailleY) {
        case 25:
            for (n=0;n<2000;n++)  {
                Screen_Buffer[4000+n*2]=Screen_Buffer[n*2];
                Screen_Buffer[4001+n*2]=Screen_Buffer[1+n*2];
                Screen_Buffer[n*2]=32;
                Screen_Buffer[n*2+1]=7;
                }
            Cfg->TailleY=50;
            break;
        default:
            for (n=0;n<2000;n++)  {
                Screen_Buffer[n*2]=Screen_Buffer[4000+n*2];
                Screen_Buffer[1+n*2]=Screen_Buffer[4001+n*2];
                Screen_Buffer[4000+n*2]=32;
                Screen_Buffer[4000+n*2+1]=7;
                }
            Cfg->TailleY=25;
            break;
    }
    else
    {
    if (Cfg->TailleY==25)
            for (n=0;n<2000;n++)
                {
                Screen_Buffer[4000+n*2]=32;
                Screen_Buffer[4001+n*2]=7;
                }
    Cfg->TailleY=i;
    }

SetTaille();
}



// Type 0: incremente de 1
// autre type: type donn‚
void ChangeType(int n)
{
if (n==0)
   Cfg->fentype++;
   else
   Cfg->fentype=n;

if (Cfg->fentype>4) Cfg->fentype=1;

Cfg->colnor=1*16+9;
Cfg->inscol=1*16+14;
Cfg->bkcol=3*16+7;

}


// Renvoit 1 si il faut inverser

int SortTest(struct file *F1,struct file *F2)
{
char e1[4],e2[4];

char a,b,c;
int j;

if (!strcmp(F1->name,"..")) return 0;
if (!strcmp(F2->name,"..")) return 1;

if (!strcmp(F1->name,".")) return 0;
if (!strcmp(F2->name,".")) return 1;


switch(DFen->order)  {
    case 1: // name
        a=((F1->attrib & RB_SUBDIR)==RB_SUBDIR);
        b=((F2->attrib & RB_SUBDIR)==RB_SUBDIR);
        if ( a ^ b )
            c=b;
            else
            {
            if (stricmp(F1->name,F2->name)>0)
                c=1;
                else
                c=0;
            }
        break;
    case 2: // ext
        a=((F1->attrib & RB_SUBDIR)==RB_SUBDIR);
        b=((F2->attrib & RB_SUBDIR)==RB_SUBDIR);
        if ( a ^ b )
            c=b;
            else
            {
            e1[0]=0;
            j=0;

            if (F1->name[0]!='.')
                while(F1->name[j]!=0)
                {
                if (F1->name[j]=='.')
                    {
                    strcpy(e1,F1->name+j+1);
                    break;
                    }
                j++;
                }
            e2[0]=0;
            j=0;

            if (F2->name[0]!='.')
                while(F2->name[j]!=0)
                {
                if (F2->name[j]=='.')
                    {
                    strcpy(e2,F2->name+j+1);
                    break;
                    }
                j++;
                }

            if (stricmp(e1,e2)>0)
                c=1;
                else
                c=0;
            }
        break;
    case 3: // date
        a=((F1->attrib & RB_SUBDIR)==RB_SUBDIR);
        b=((F2->attrib & RB_SUBDIR)==RB_SUBDIR);
        if ( a ^ b )
            c=b;
            else
            {
            if ( (F1->date>F2->date) | ( (F1->date==F2->date) & (F1->time>F2->time) ) )
                c=1;
                else
                c=0;
            }
        break;
    case 4: // size
        a=((F1->attrib & RB_SUBDIR)==RB_SUBDIR);
        b=((F2->attrib & RB_SUBDIR)==RB_SUBDIR);
        if ( a ^ b )
            c=b;
            else
            {
            if (F1->size>F2->size)
                c=1;
                else
                c=0;
            }
        break;
    }

return c;
}

void SortFic(struct fenetre *Fen)
{
int i,j;
struct file *F1,*F2;

if (DFen->order==0)
    return;


for (i=0;i<Fen->nbrfic-1;i++)
	for (j=i;j<Fen->nbrfic;j++) {
		F1=Fen->F[i];
		F2=Fen->F[j];

		if (SortTest(F1,F2)>0)	  {
			 Fen->F[i]=F2;
			 Fen->F[j]=F1;
			 }
		}
}

//--------------
// Select File -
//--------------

void SelectPlus(void)
{
int i;


SaveEcran();

WinCadre(29,6,51,11,0);
ColWin(30,7,50,10,0*16+1);
ChrWin(30,7,50,10,32);

WinCadre(30,8,50,10,1);

PrintAt(30,7,"Selection of files");

InputAt(31,9,Select_Chaine,12);

ChargeEcran();

for (i=0;i<DFen->nbrfic;i++)
    if (!WildCmp(DFen->F[i]->name,Select_Chaine))
        FicSelect(i,1);
}

//---------------
// Select Moins -
//---------------

void SelectMoins(void)
{
int i;

SaveEcran();

WinCadre(29,6,51,11,0);
ColWin(30,7,50,10,0*16+1);
ChrWin(30,7,50,10,32);

WinCadre(30,8,50,10,1);

PrintAt(30,7,"Deselection of files");

InputAt(31,9,Select_Chaine,12);

ChargeEcran();

for (i=0;i<DFen->nbrfic;i++)
    if (!WildCmp(DFen->F[i]->name,Select_Chaine))
        FicSelect(i,0);
}

//--------------
// History Dir -
//--------------

void HistDir(void)
{
int Mlen;

int i,j;
static char **dir;

dir=GetMem(25*sizeof(char *));

j=0;
Mlen=0;
for (i=0;i<25;i++)  {
    dir[i]=&(Cfg->HistDir[j]);
    dir[i]=strupr(dir[i]);
    if (strlen(dir[i])>Mlen) Mlen=strlen(dir[i]);
    if (strlen(dir[i])==0) break;
    while ( (j!=256) & (Cfg->HistDir[j]!=0) ) j++;
    j++;
    }

if (i!=0)
    {
    int x=2,y=2;
    int pos=i-1;
    int car;

    SaveEcran();

    WinCadre(x-2,y-1,x+Mlen+1,y+i,0);
    ColWin(x-1,y,x+Mlen,y+i-1,0*16+1);
    ChrWin(x-1,y,x+Mlen,y+i-1,32);

    for (j=0;j<i;j++)
        PrintAt(x,y+j,dir[j]);

    do {

    ColLin(x-1,y+pos,Mlen+2,7*16+5);

    car=Wait(0,0,0);

    ColLin(x-1,y+pos,Mlen+2,0*16+1);

    switch(HI(car))  {
        case 72:        // UP
            pos--;
            break;
        case 80:        // DOWN
            pos++;
            break;
        case 0x47:      // HOME
            pos=0;
            break;
        case 0x4F:      // END
            pos=i-1;
            break;
        case 0x49:      // PGUP
            pos-=5;
            break;
        case 0x51:      // PGDN
            pos+=5;
            break;
        }
    if (pos==-1) pos=i-1;
    if (pos==i) pos=0;

    }
    while ( (car!=13) & (car!=27) );

    ChargeEcran();

    if (car==13)
        CommandLine("#cd %s",dir[pos]);
    }


free(dir);
}


//-----------------------
// Win Change directory -
//-----------------------
void WinCD(void)
{
static char Dir[70];
static int DirLength=70;
static int CadreLength=71;

struct Tmt T[5] = {
      { 2,3,1,
        Dir,
        &DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Change to which directory",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,10,76,17,
    "Change Directory"};

int n;

n=WinTraite(T,5,&F);

if ( (n==0) | (n==1) ) {
    CommandLine("#cd %s",Dir);
    }
}

//-------------------------------------
// Create directory in current window -
//-------------------------------------
void CreateDirectory(void)
{
static char Dir[70];
static int DirLength=70;
static int CadreLength=71;

struct Tmt T[5] = {
      { 2,3,1,
        Dir,
        &DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the directory to be created",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,10,76,17,
    "Create Directory"};

int n;

n=WinTraite(T,5,&F);

if ( (n==0) | (n==1) ) {
    CommandLine("#md %s",Dir);
    CommandLine("#cd .");
    }
}

//------------------
// Create KKD disk -
//------------------
void CreateKKD(void)
{
static char Name[256];
static char Dir[70];
static int DirLength=70;
static int CadreLength=71;

struct Tmt T[5] = {
      { 2,3,1,
        Dir,
        &DirLength},
      {15,5,2,NULL,NULL},
      {45,5,3,NULL,NULL},
      { 5,2,0,"Name the KKD file to be created",NULL},
      { 1,1,4,NULL,&CadreLength}
      };

struct TmtWin F = {
    3,10,76,17,
    "Create KKD File"};

int n;

n=WinTraite(T,5,&F);

strcpy(Name,DFen->Fen2->path);

if ( (n==0) | (n==1) ) {
    Path2Abs(Name,Dir);
    MakeKKD(DFen,Name);
    }
}


//---------------------------------
// Change drive of current window -
//---------------------------------
void ChangeDrive(void)
{
char drive[26];
short m,n,x,l,nbr;
signed char i;

int car;


nbr=0;

for (i=0;i<26;i++)
    {
    drive[i]=GetDriveReady(i);
    if (drive[i]==0)
        nbr++;
    }

l=36/nbr;
if (l>3) l=3;
if (nbr<5) l=6;
if (nbr<2) l=9;
x=(40-(l*nbr))/2+DFen->x;


SaveEcran();

WinCadre(x-2,6,x+l*nbr+1,11,0);
ColWin(x-1,7,x+l*nbr,10,0*16+1);
ChrWin(x-1,7,x+l*nbr,10,32);

WinCadre(x-1,8,x+l*nbr,10,1);

PrintAt(x,7,"Choose a drive");

m=x+l/2;
for (n=0;n<26;n++)
    {
    if (drive[n]==0)
        {
        drive[n]=m;
        AffChr(m,9,n+'A');
        m+=l;
        }
        else
        drive[n]=0;
    }


i=-1;
car=DROITE*256;
do	{
    do {
        if (HI(car)==GAUCHE) i--;
        if (HI(car)==DROITE) i++;
        if (i==26) i=0;
        if (i<0) i=25;
        } while (drive[i]==0);

    if (HI(car)==0) {
        car=(toupper(car)-'A');
        if ( (car>=0) & (car<26) )
            if (drive[car]!=0)
                {
                i=car;
                car=13;
                break;
                }
	   }

    AffCol(drive[i],9,1*16+5);

    car=Wait(0,0,0);
    AffCol(drive[i],9,0*16+1);

} while ( (LO(car)!=27) & (LO(car)!=13));

ChargeEcran();

if (car==13) {
   CommandLine("\r%c:",i+'A');
   }
}

void QuickSearch(char *c,char *c2)
{
char fin;
char chaine[20];
int i,lng=0;
int x,y;
char car,car2;

int c1;

x=DFen->Fen2->x+3;
y=DFen->Fen2->y+3;

SaveEcran();

WinCadre(x-1,y-1,x+24,y+1,0);
ColLin(x,y,24,0*16+1);
ChrLin(x,y,24,32);

do
    {
    c1=Wait(0,0,0);

    car=LO(c1);
    car2=HI(c1);

    car=tolower(car);

    fin=1;

    if ( ((car>='a') & (car<='z')) | (car=='.') | ((car>='0') & (car<='9')) )
        {
        chaine[lng]=car;
        lng++;
        fin=0;
        }

    if (lng!=0)
        {
        lng--;

        for (i=0;i<DFen->nbrfic;i++)
            if (!strnicmp(chaine,DFen->F[i]->name,lng+1))
                {
                DFen->pcur=i;
                DFen->scur=i;
                lng++;
                break;
                }
        chaine[lng]=0;
        }
    PrintAt(x,y,"%24s",chaine);

    AffFen(DFen);
            
    } while(!fin);

ChargeEcran();

*c=car;
*c2=car2;
}


/*****************************************************************************/


/**********************************
 -  Access fichier suivant system -
 **********************************/

char *AccessFile(void)
{
static char nom[256];
static char tnom[256];

char ChangePos=0;

int i;

strcpy(tnom,DFen->F[DFen->pcur]->name);

switch (DFen->system)   {
    case 0: // DOS
        strcpy(nom,DFen->path);
        Path2Abs(nom,tnom);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        DFen=DFen->Fen2;

        CommandLine("#cd %s",Fics->trash);

        Cfg->FenAct= (Cfg->FenAct==1) ? 0:1;
        DFen=Fenetre[Cfg->FenAct];
        ChangeLine();      // Affichage Path

        DFen=DFen->Fen2;

        for (i=0;i<DFen->nbrfic;i++)
            if (!strncmp(tnom,DFen->F[i]->name,strlen(tnom)))  {
                DFen->pcur=i;
                DFen->scur=i;

                DFen->F[DFen->pcur]->select=1;
                DFen->nbrsel++;
                DFen->taillesel+=DFen->F[DFen->pcur]->size;
                break;
                }

        DFen=DFen->Fen2;

        Copie(DFen->Fen2,DFen);         // Quitte aprŠs

        break;
    case 5: // KKD
        if (strlen(DFen->path)==strlen(DFen->VolName))
            strcpy(nom,"\\");
            else
            {
            strcpy(nom,"\\");
            strcat(nom,(DFen->path)+strlen(DFen->VolName)+1);
            }

        DFen=DFen->Fen2;

        ChangeDrive();
        CommandLine("#cd %s",nom);

        Cfg->FenAct= (Cfg->FenAct==1) ? 0:1;
        DFen=Fenetre[Cfg->FenAct];
        ChangeLine();      // Affichage Path

        ChangePos=1;
        break;
    default:
        break;
    }

if (ChangePos)
    {
    strcpy(nom,DFen->path);
    Path2Abs(nom,tnom);
    for (i=0;i<DFen->nbrfic;i++)
        if (!strncmp(tnom,DFen->F[i]->name,strlen(tnom)))  {
            DFen->pcur=i;
            DFen->scur=i;
            break;
            }
    }
return nom;
}


/***************************************************************************
 - Retourne 0 si OK                                                        -
 - Sinon retourne numero de IDF                                            -
 ***************************************************************************/
int EnterArchive(void)
{
int i;

switch (i=InfoIDF(DFen))
    {
    case 34:       // RAR
        InstallRAR();
        break;
    case 30:       // ARJ
        InstallARJ();
        break;
    case 35:       // ZIP
        InstallZIP();
        break;
    case 32:       // LHA
        InstallLHA();
        break;
    case 102:       // KKD
        InstallKKD();
        break;
    default:
        return i;
        break;
    }

CommandLine("#cd .");
ChangeLine();
return 0;               // OK
}

void SaveSel(struct fenetre *F1)
{
struct file *F;
FILE *fic;
int i;

fic=fopen(Fics->temp,"wt");

fprintf(fic,"%s\n",F1->F[F1->pcur]->name);

fprintf(fic,"%d\n",F1->scur);   // position a l'ecran



for(i=0;i<F1->nbrfic;i++)
    {
    F=F1->F[i];

    if ((F->select)==1)
        fprintf(fic,"%s\n",F->name);
    }
fclose(fic);
}

void LoadSel(struct fenetre *F1,int n)
{
char nom[256];
struct file *F;
FILE *fic;
int i,j;


fic=fopen(Fics->temp,"rt");
if (fic==NULL) return;

fgets(nom,256,fic);
nom[strlen(nom)-1]=0;

fscanf(fic,"%d",&(F1->scur));

j=1;
for (i=0;i<F1->nbrfic;i++)
    if (!strncmp(nom,F1->F[i]->name,strlen(nom)))  {
        F1->pcur=i;
        j=0;
        break;
        }
if (j==1)
    F1->pcur=F1->scur;

while(!feof(fic))
    {
    fgets(nom,256,fic);
    nom[strlen(nom)-1]=0;

    for(i=0;i<F1->nbrfic;i++)
        {
        F=F1->F[i];

        if (!stricmp(F->name,nom))
            {
            switch(n)   {
                case 0:
                    F->select=1;
                    break;
                case 1:
                    if (F->select==0)
                        F->select=1;
                        else
                        F->select=0;
                    break;
                }
            }
        }
    }
fclose(fic);
}




/*************************************************************************
 -                         Programme Principal                           -
 *************************************************************************/
void Gestion(void)
{
struct fenetre *FenOld;

clock_t Cl_Start;
char car,car2;

short car3,c;

int i;  // Compteur

do {
    DFen=Fenetre[Cfg->FenAct];

    switch(DFen->FenTyp)  {
        case 1:
        case 2:
            Cfg->FenAct= (Cfg->FenAct==1) ? 0:1;
            DFen=Fenetre[Cfg->FenAct];
            ChangeLine();      // Affichage Path
            break;
        }

    DFen->actif=1;
    DFen->Fen2->actif=0;

    AffFen(DFen);
    AffFen(DFen->Fen2);

    Cl_Start=clock();

    if (Cfg->key==0)
    {

    while (!kbhit())
        {
        if ( ((clock()-Cl_Start)>DFen->IDFSpeed)  & (Cl_Start!=0))
            {
            Cl_Start=0;
            InfoIDF(DFen);         // information idf sur fichier selectionn‚

            if (DFen->FenTyp==1) FenDIZ(DFen);
            if (DFen->Fen2->FenTyp==1) FenDIZ(DFen->Fen2);
            }
        car=*Keyboard_Flag1;

        if ((car&1)==1)  MenuBar(1);
            else
        if ((car&2)==2)  MenuBar(1);
            else
        if ((car&4)==4)  MenuBar(2);
            else
        if ((car&8)==8)  MenuBar(3);
            else
            MenuBar(0);
        }

//    if ((car&3)==3)  break;  // SHIFT L & R

    car3=_bios_keybrd(0x11)/256;

    c=Wait(0,0,0);

    Cfg->key=c;

    strcpy(Cfg->FileName,DFen->F[DFen->pcur]->name);

    switch(LO(c))   {
        case 13:
            Cfg->key=0;
            if (CommandLine("\n")!=0)
                {
                c=0;
                break;
                }

            if ((DFen->F[DFen->pcur]->attrib & RB_SUBDIR)==RB_SUBDIR)
                {
                CommandLine("#cd %s",DFen->F[DFen->pcur]->name);
                ChangeLine();
                c=0;
                break;
                }
            Cfg->key=c;
            AccessFile();
            break;
        }

    switch(HI(c))   {
        case 0x3D:  // F3
        case 0x3E:  // F4
        case 0x56:      // SHIFT-F3
            AccessFile();
            break;
        }
    }
    else
    {
    c=Cfg->key;
    car3=0;

    for (i=0;i<DFen->nbrfic;i++)
        if (!strncmp(Cfg->FileName,DFen->F[i]->name,strlen(Cfg->FileName)))  {
            DFen->pcur=i;
            DFen->scur=i;
            break;
            }
    }

    Cfg->key=0;

    car=LO(c);
    car2=HI(c);

    if (car2==0x94)
        QuickSearch(&car,&car2);


//-Switch car3 (BIOS_KEYBOARD)-------------------------------------------------
    switch (car3)  {
        case 0x37:  // '*' --> Inverse selection
            for (i=0;i<DFen->nbrfic;i++)
                FicSelect(i,2);
            car=car2=0;
            break;
        case 0x4E:  // '+' --> Selectionne
            SelectPlus();
            car=car2=0;
            break;
        case 0x4A:  // '-' --> Deselectionne
            SelectMoins();
            car=car2=0;
            break;
        default:
//          PrintAt(0,0,"%04X",car3);
            break;
        }

//-Switch car------------------------------------------------------------------
    switch (car)  {
        case 1:     // CTRL-A
            ASCIItable();
            break;

        case 4:     // CTRL-D
            WinCD();
            break;
        case 9:     // TAB
            Cfg->FenAct= (Cfg->FenAct==1) ? 0:1;
            DFen=Fenetre[Cfg->FenAct];
            ChangeLine();      // Affichage Path
            break;
        case 13:    // ENTER


            switch(i=EnterArchive())
                {
                case 0:
                    // OK
                    break;
                case 57: // Executable
                    CommandLine("%s\n",DFen->F[DFen->pcur]->name);
                    break;
                default:
                    switch(FicIdf(DFen->F[DFen->pcur]->name,i)) {
                        case 0:
                            CommandLine("\n");
                            break;
                        case 1:  // Pas de fichier IDFEXT.RB
                            CommandLine("@ ERROR WITH FICIDF @");
                            break;
                        case 2:
                             CommandLine(DFen->F[DFen->pcur]->name);
                             break;
                        }
                }
            break;

        case 0x0A:  // CTRL-ENTER
            CommandLine("%s",DFen->F[DFen->pcur]->name);
            break;
        case 27:    // ESCAPE
            CommandLine("\r");
            break;
        case 'ý':
            for (i=0;i<DFen->nbrfic;i++)
                {
                if (DFen->F[i]->select==0)
                    {
                    if ( (!WildCmp(DFen->F[i]->name,"*.bak")) |
                         (!WildCmp(DFen->F[i]->name,"*.old")) |
                         (!WildCmp(DFen->F[i]->name,"*.tmp")) |
                         (!WildCmp(DFen->F[i]->name,"*.map")) |
                         (!WildCmp(DFen->F[i]->name,"*.lst")) |
                         (!WildCmp(DFen->F[i]->name,"*.obj")) |
                         (!WildCmp(DFen->F[i]->name,"*.$")) )
                        {
                        DFen->F[i]->select=1;
                        DFen->taillesel+=DFen->F[i]->size;
                        DFen->nbrsel++;
                        }
                    }
                }
            break;

//-Switch car2-----------------------------------------------------------------
        case 0:
        switch(car2)  {       // Switch (car2)
        

        case 72:         // HAUT
            DFen->scur--;
            DFen->pcur--;
            break;

        case 0x52:       // Insert
            if (!strcmp(DFen->F[DFen->pcur]->name,".")) {
                    int i;

                    for (i=0;i<DFen->nbrfic;i++)
                        {
                        if ((DFen->F[i]->select==0) & (DFen->F[i]->name[0]!='.'))
                            {
                            DFen->F[i]->select=1;
                            DFen->taillesel+=DFen->F[i]->size;
                            DFen->nbrsel++;
                            }
                        }
                    break;
                    }

               if (DFen->F[DFen->pcur]->name[0]!='.') {
                  if (DFen->F[DFen->pcur]->select==1) {
                     DFen->F[DFen->pcur]->select=0;
                     DFen->nbrsel--;
                     DFen->taillesel-=DFen->F[DFen->pcur]->size;
					 }
					 else {
                     DFen->F[DFen->pcur]->select=1;
                     DFen->nbrsel++;
                     DFen->taillesel+=DFen->F[DFen->pcur]->size;
					 }
				  } 	   // pas de break car ---/
        case 80:         // BAS
            DFen->scur++;
            DFen->pcur++;
            break;
        case 0x4B:       // GAUCHE
            Cfg->FenAct=0;
            DFen=Fenetre[Cfg->FenAct];
            ChangeLine();      // Affichage Path
            break;
        case 0x4D:       // DROITE
            Cfg->FenAct=1;
            DFen=Fenetre[Cfg->FenAct];
            ChangeLine();      // Affichage Path
            break;
        case 0x49:       // PAGE UP
            DFen->scur-=10;
            DFen->pcur-=10;
            break;
        case 0x51:       // PAGE DOWN
            DFen->scur+=10;
            DFen->pcur+=10;
            break;
        case 0x47:       // HOME
            DFen->scur=0;
            DFen->pcur=0;
            break;
        case 0x4F:       // END
            DFen->scur=DFen->nbrfic;
            DFen->pcur=DFen->nbrfic;
            break;
        case 0x3B:       // F1
            Help();
            break;
        case 0x3D:       // F3
            CommandLine("#%s %s",Fics->view,DFen->F[DFen->pcur]->name);
            break;
        case 0x3E:       // F4
            CommandLine("#%s %s",Fics->edit,DFen->F[DFen->pcur]->name);
            break;
        case 0x3F:       // F5
            Copie(DFen,DFen->Fen2);

            DFen=DFen->Fen2;
            CommandLine("#cd .");
            DFen=DFen->Fen2;
            break;
        case 0x40:       // F6
            SaveSel(DFen);      // sauvegarde position et selection
            Copie(DFen,DFen->Fen2);

            DFen=DFen->Fen2;
            CommandLine("#cd .");
            DFen=DFen->Fen2;

            LoadSel(DFen,1);     // selection des anciens uniquements

            Delete(DFen);
            CommandLine("#cd .");

            LoadSel(DFen,0);
            break;
        case 0x41:       // F7
            CreateDirectory();
            break;
        case 0x42:       // F8
            SaveSel(DFen);
            Delete(DFen);
            CommandLine("#cd .");
            LoadSel(DFen,0);
            break;
        case 0x56:      // SHIFT-F3
            switch(DFen->system)    {
                case 0:
                    View(DFen);
                    break;
                }
            break;
        case 0x5E:       // CTRL-F1
               DFen=Fenetre[0];

               if (DFen->FenTyp==2)  {
                  DFen->init=1;
                  DFen->FenTyp=0;
				  }
				  else
                  {
                  DFen->FenTyp=2;
                  Cfg->FenAct=1;
                  DFen=Fenetre[Cfg->FenAct];
                  ChangeLine();      // Affichage Path
                  }
               break;
        case 0x5F:       // CTRL-F2
               DFen=Fenetre[1];

               if (DFen->FenTyp==2)  {
                  DFen->init=1;
                  DFen->FenTyp=0;
				  }
				  else
                  {
                  DFen->FenTyp=2;
                  Cfg->FenAct=0;
                  DFen=Fenetre[Cfg->FenAct];
                  ChangeLine();      // Affichage Path
                  }
               break;
        case 0x60:       // CTRL-F3
            DFen->order=1;
            SortFic(DFen);
            break;
        case 0x61:       // CTRL-F4
            DFen->order=2;
            SortFic(DFen);
            break;
        case 0x62:       // CTRL-F5
            DFen->order=3;
            SortFic(DFen);
            break;
        case 0x63:       // CTRL-F6
            DFen->order=4;
            SortFic(DFen);
            break;
        case 0x64:       // CTRL-F7
            DFen->order=0;
            SortFic(DFen);
            break;
        case 0x68:       // ALT-F1
               FenOld=DFen;
               DFen=Fenetre[0];

			   ChangeDrive();
			   DFen=FenOld;

               ChangeLine();      // Affichage Path
			   break;
        case 0x69:       // ALT-F2
               FenOld=DFen;
               DFen=Fenetre[1];

			   ChangeDrive();
			   DFen=FenOld;

               ChangeLine();      // Affichage Path
			   break;
        case 0x6A:       // ALT-F3
            DFen=DFen->Fen2;

            if (DFen->FenTyp==1)  {
               DFen->init=1;
               DFen->FenTyp=0;
               }
               else
               DFen->FenTyp=1;

            DFen=DFen->Fen2;
            break;
        case 0x6E:       // ALT-F7
            Search();
            break;
        case 0x6F:       // ALT-F8
            ChangeType(0);
            AfficheTout();
            break;
        case 0x70:       // ALT-F9
            ChangeTaille(0);
            ChangeLine();
            break;
        case 0x76:       // CTRL PGDN
            HistDir();
            break;
        case 0x84:       // CTRL PGUP
            CommandLine("#CD ..");
            break;
        case 0x86:       // F12
            CommandLine("#CD %s",Fics->trash);
            break;
        case 0x88:       // SHIFT-F12
            CreateKKD();
            break;
        case 0x8A:       // CTRL-F12
            ChangePalette(0);
            break;
        case 0xB6:  //
        case 0xB7:  //  Windows 95 keyboard
        case 0xB8:  //
			   ErrWin95();
			   break;

        default:
               PrintAt(78,0,"%02X",car2);
			   break;
        }    // switch (car2);
        break;
//-Retour Switch car-----------------------------------------------------------
        default:  // default du switch car
        CommandLine("%c",car);
			   break;
   }  // switch(car);
   
   }
while(car2!=0x44);      // F10
}


/*******************************************************
 -                      DOS Shell                      -
 *******************************************************/

void Shell(char *string,...)
{
char sortie[255];
va_list arglist;
char *suite;

TXTMode(Cfg->TailleY);

suite=sortie;

va_start(arglist,string);
vsprintf(sortie,string,arglist);
va_end(arglist);

if (Cfg->KeyAfterShell==0)
    suite[0]='#';

strcpy(ShellAdr,suite);

Fin();
}



void PlaceDrive(void)
{
unsigned ndrv;

chdir(DFen->Fen2->path);
_dos_setdrive(toupper(DFen->path[0])-'A'+1,&ndrv);
chdir(DFen->path);

}


/*******************
 - Fin d'execution -
 *******************/

void Fin(void)
{
int n;

SaveCfg();

PlaceDrive();

for (n=0;n<8000;n++)
    Screen_Adr[n]=Screen_Buffer[n];

GotoXY(0,(Cfg->TailleY)-1);

if (Cfg->_4dos==1)  {
    _4DOSShistdir();
    }

exit(1);
}

void AfficheTout(void)
{
PrintAt(0,0,"%-40s%40s","Ketchup Killers Commander","RedBug");
ColLin( 0,0,40,1*16+5);
ColLin(40,0,40,1*16+3);
ColLin(0,(Cfg->TailleY)-2,80,7);

DFen=Fenetre[Cfg->FenAct];

CommandLine("##INIT 0 %d 80\n",(Cfg->TailleY)-2);

DFen->init=1;
DFen->Fen2->init=1;

ChangeLine();

MenuBar(3);

AffFen(DFen);
AffFen(DFen->Fen2);
}

void AffFen(struct fenetre *Fen)
{
int x,y,n;

switch (Fen->FenTyp) {
       case 0:
            FenNor(Fen);
            break;
       case 1:      // FenDIZ --> A lieu au moment de IDF
            break;
       case 2:
            for(y=Fen->y;y<=Fen->y+Fen->yl;y++)  {        // NON !
                n=y*160;
                n+=(Fen->x)*2;
                for(x=0;x<(Fen->xl+1)*2;x++)   {
                    Screen_Adr[n]=Screen_Buffer[n];
                    n++;
                    }
                }
            break;
       }
}


void InfoSupport()
{
switch(DFen->drive) {
   case 1:
   case 2:
		DFen->IDFSpeed=10*18;
		break;
   default:
		DFen->IDFSpeed=2*18;
		break;
   }

}

void SaveCfg(void)
{
int m,n;

int t;

FILE *fic;
struct fenetre *Fen;

Cfg->FenTyp[0]=Fenetre[0]->FenTyp;
Cfg->FenTyp[1]=Fenetre[1]->FenTyp;

fic=fopen(Fics->CfgFile,"wb");
fwrite((void*)Cfg,sizeof(struct config),1,fic);


for(t=0;t<2;t++)
{
Fen=Fenetre[t];

fwrite(Fen->path,256,1,fic);
fwrite(&(Fen->order),sizeof(ENTIER),1,fic);
fwrite(&(Fen->sorting),sizeof(ENTIER),1,fic);


fwrite(&(Fen->nbrsel),4,1,fic);

for (n=0;n<Fen->nbrfic;n++)  {
    if (Fen->F[n]->select==1) {
        m=strlen(Fen->F[n]->name);
        fwrite(&m,4,1,fic);
        fwrite(Fen->F[n]->name,1,m,fic);
        }
    }

m=strlen(Fen->F[Fen->pcur]->name);
fwrite(&m,4,1,fic);
fwrite(Fen->F[Fen->pcur]->name,1,m,fic);

fwrite(&(Fen->scur),sizeof(ENTIER),1,fic);
}

fclose(fic);
}


// Retourne -1 en cas d'erreur
//           0 si tout va bien
int LoadCfg(void)
{
int m,n,i,nbr;
int t;
FILE *fic;
char nom[256],tnom[256];


fic=fopen(Fics->CfgFile,"rb");
if (fic==NULL) return -1;

fread((void*)Cfg,sizeof(struct config),1,fic);


for (t=0;t<2;t++)
    {
    DFen=Fenetre[t];

    DFen->FenTyp=Cfg->FenTyp[t];
    
    fread(DFen->path,256,1,fic);
    fread(&(DFen->order),sizeof(ENTIER),1,fic);
    fread(&(DFen->sorting),sizeof(ENTIER),1,fic);

    IOver=1;
    IOerr=0;

    if (chdir(DFen->path)!=0)
        {
        strcpy(nom,DFen->path);
        strcpy(tnom,"");
        do
            {
            n=0;
            for (i=0;i<strlen(DFen->path);i++)
                if (DFen->path[i]=='\\') n=i;

            if (n==0) break;

            strcpy(tnom,DFen->path+n+1);
            DFen->path[n]=0;
            if (chdir(DFen->path)==0) break;
            }
        while(1);

        if (n==0)
            {
            memcpy(DFen->path,"C:\\",4);
            CommandLine("#cd .");
            }
            else
            {
            CommandLine("#cd .");
            for (i=0;i<DFen->nbrfic;i++)
                if (!strncmp(tnom,DFen->F[i]->name,strlen(tnom)))  {
                    DFen->pcur=i;
                    DFen->scur=i;
                    n=0;
                    break;
                    }
            if (n==0)
                {
                EnterArchive();
                if (strlen(nom)!=strlen(DFen->VolName))
                    CommandLine("#cd %s",nom+strlen(DFen->VolName)+1);
                }
            }
        }
        else
        {
        CommandLine("#cd .");
        }


    IOver=0;

    fread(&nbr,4,1,fic);

    DFen->nbrsel=0;

    for (i=0;i<nbr;i++)
        {
        fread(&m,4,1,fic);
        fread(nom,m,1,fic);
        nom[m]=0;

        for (n=0;n<DFen->nbrfic;n++)
            {
            if (!stricmp(nom,DFen->F[n]->name))
                {
                DFen->F[n]->select=1;
                DFen->nbrsel++;
                DFen->taillesel+=DFen->F[n]->size;
                }
            }
        }

    fread(&m,4,1,fic);
    fread(nom,m,1,fic);
    nom[m]=0;

    fread(&(DFen->scur),sizeof(ENTIER),1,fic);

    for (n=0;n<DFen->nbrfic;n++)
        {
        if (!stricmp(nom,DFen->F[n]->name))
            DFen->pcur=n;
        }
    }

fclose(fic);

return 0;
}



/****************
 - Gestion 4DOS -
 ****************/

// Put Cfg->_4dos on if 4dos founded
void _4DOSverif(void)
{
union REGS R;

R.w.ax=0xD44D;
R.h.bh=0;

int386(0x2F,&R,&R);

if (R.w.ax==0x44DD)  {
    Cfg->_4dos=1;
//  PrintAt(0,0,"Found 4DOS V%d.%d",R.h.bl,R.h.bh);
    }
    else
    {
    Cfg->_4dos=0;
    }
}

void _4DOSLhistdir(void)
{
unsigned short seg;
unsigned short *adr;

char a;

register unsigned char n;

union REGS R;

R.w.ax=0xD44D;
R.h.bh=0;

int386(0x2F,&R,&R);

if (R.w.ax==0x44DD)  {
    Cfg->_4dos=1;

    seg=R.w.cx;
    adr=(unsigned short*)(seg*16+0x290);

    seg=adr[1];
    adr=(unsigned short*)(seg*16+0x4C60);

    for (n=0;n<255;n++)  {
        a=((char*)adr)[n];
        Cfg->HistDir[n]=a;
        }
    }
    else
    {
    Cfg->_4dos=0;
    }
}

void _4DOSShistdir(void)
{
unsigned short seg;
unsigned short *adr;

char a;

register unsigned char n;

union REGS R;

R.w.ax=0xD44D;
R.h.bh=0;

int386(0x2F,&R,&R);

if (R.w.ax==0x44DD)  {
    Cfg->_4dos=1;

    seg=R.w.cx;
    adr=(unsigned short*)(seg*16+0x290);

    seg=adr[1];
    adr=(unsigned short*)(seg*16+0x4C60);

    for (n=0;n<255;n++)  {
        a=Cfg->HistDir[n];
        ((char*)adr)[n]=a;
        }
    }
    else
    {
    Cfg->_4dos=0;
    }
}


/* ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ *\
   ³                                                                       ³
   ³                                                                       ³
   ³   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿   ³
   ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´  MAIN  ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´   ³
   ³   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ   ³
   ³                                                                       ³
   ³                                                                       ³
\* ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ */

void main(int argc,char **argv)
{
char *path;
int n;

char *LC;

/**************************
 - Initialise les buffers -
 **************************/

Fenetre[0]=GetMem(sizeof(struct fenetre));
memset(Fenetre[0],0,sizeof(struct fenetre));
Fenetre[0]->F=GetMem(TOTFIC*sizeof(void *));        // allocation des pointeurs

Fenetre[1]=GetMem(sizeof(struct fenetre));
memset(Fenetre[1],0,sizeof(struct fenetre));
Fenetre[1]->F=GetMem(TOTFIC*sizeof(void *));        // allocation des pointeurs

Cfg=GetMem(sizeof(struct config));
memset(Cfg,0,sizeof(struct config));

Fics=GetMem(sizeof(struct fichier));
memset(Fics,0,sizeof(struct fichier));

Screen_Buffer=GetMem(8000);
for (n=0;n<8000;n++)
    Screen_Buffer[n]=Screen_Adr[n];

path=GetMem(256);

/*****************************************
 - Lecture et verifiaction des arguments -
 *****************************************/

strcpy(ShellAdr+128,*argv);
strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
    }

LC=*(argv+argc-1);

if (!strncmp(LC,"6969",4)) {
    printf("%s\n",RBTitle);
    }
    else
    {
    printf("This program cannot be run in DOS mode\n");
    printf("This program cannot be run in WINDOWS mode\n");
    printf("This program cannot be run in OS/2 mode\n");
    printf("This program cannot be run in LINUX mode\n\n");
    printf("This program required KK.EXE\n\n");
    exit(1);
    }



/***********************
 - Gestion des erreurs -
 ***********************/

_harderr(Error_handler);

signal(SIGABRT,Signal_Handler);
signal(SIGFPE,Signal_Handler);
signal(SIGILL,Signal_Handler);
signal(SIGINT,Signal_Handler);
signal(SIGSEGV,Signal_Handler);
signal(SIGTERM,Signal_Handler);






/*******************************
 - Initialisation des fichiers -
 *******************************/

Fics->FicIdfFile=GetMem(256);
strcpy(Fics->FicIdfFile,path);
strcat(Fics->FicIdfFile,"\\idfext.rb");

Fics->CfgFile=GetMem(256);
strcpy(Fics->CfgFile,path);
strcat(Fics->CfgFile,"\\kkrb.cfg");

Fics->view=GetMem(256);
strcpy(Fics->view,path);
strcat(Fics->view,"\\view");

Fics->edit=GetMem(256);
strcpy(Fics->edit,path);
strcat(Fics->edit,"\\edit");

Fics->font=GetMem(256);
strcpy(Fics->font,path);
strcat(Fics->font,"\\font.cfg");

Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\kkc.hlp");

Fics->temp=GetMem(256);
strcpy(Fics->temp,path);
strcat(Fics->temp,"\\temp.tmp");

Fics->trash=GetMem(256);
strcpy(Fics->trash,path);
strcat(Fics->trash,"\\trash");      // repertoire trash



/*************
 -  Default  -
 *************/

Cfg->TailleY=50;

ChangeType(4);
Cfg->SaveSpeed=1800;

Cfg->_4dos=0;
_4DOSverif();

if (Cfg->_4dos==1)  {
    _4DOSLhistdir();
    }
    else
    memset(Cfg->HistDir,0,256);




/******************
 - Initialisation -
 ******************/



Fenetre[0]->x=0;
Fenetre[0]->y=1;
Fenetre[0]->yl=(Cfg->TailleY)-4;
Fenetre[0]->xl=39;
Fenetre[0]->actif=1;

Fenetre[0]->pcur=0;
Fenetre[0]->scur=0;

Fenetre[0]->FenTyp=0;

Fenetre[0]->Fen2=Fenetre[1];

Fenetre[0]->order=1;



Fenetre[1]->x=40;
Fenetre[1]->y=1;
Fenetre[1]->yl=(Cfg->TailleY)-4;
Fenetre[1]->xl=39;
Fenetre[1]->actif=0;

Fenetre[1]->pcur=0;
Fenetre[1]->scur=0;

Fenetre[1]->FenTyp=0;

Fenetre[1]->Fen2=Fenetre[0];

Fenetre[1]->order=1;



/**********************************************
 - Chargement du fichier config (s'il existe) -
 **********************************************/

if (LoadCfg()==-1)
    {
    DFen=Fenetre[0];
    CommandLine("c:");
    DFen=Fenetre[1];
    CommandLine("c:");
    Cfg->KeyAfterShell=0;
    ChangePalette(1);
    }


VerifHistDir();                 // Verifie l'history pour les repertoires

ChangeTaille(Cfg->TailleY);     // Change de taille et affiche tout

Gestion();


/*********
 -  FIN  -
 *********/

TXTMode(Cfg->TailleY);          // Retablit le mode texte normal
ShellAdr[0]=0;
Fin();
}



