#include <dos.h>
#include <direct.h>
#include <io.h>
#include <stdlib.h>
#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <bios.h>

#include <time.h>

#include "kk.h"
#include "idf.h"

#define GAUCHE 0x4B
#define DROITE 0x4D

extern struct key K[nbrkey];
extern int IOerr;

char PathOfKK[256];
char ActualPath[256];

// Pour Statistique;
int St_App;
int St_Dir;

struct player {
    char *Filename;
    char *Meneur;
    char *Titre;
    unsigned long Checksum;
    short ext;      // Numero d'extension
    short pres;     // 0 si pas trouv‚ sinon numero du directory
    char type;
    } *app[5000];

char dir[50][128]; // 50 directory diff‚rents de 128 caracteres

short nbr;    // nombre d'application lu dans les fichiers KKR
short nbrdir; //

char OldY;

struct fenetre *Fenetre[3];

void SSearch(char *nom);
void ApplSearch(void);
void ClearAllSpace(char *name);

int posy;

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


int sort_function(const void *a,const void *b)
{
struct key *a1,*b1;

a1=(struct key*)a;
b1=(struct key*)b;

// return (a1->numero)-(b1->numero);

if (a1->type!=b1->type) return (a1->type)-(b1->type);

return strcmp(a1->ext,b1->ext);		// ou format ?
}




void IdfListe(void)
{
int car,y;

int n;
int info;

int prem;

SaveEcran();
PutCur(32,0);

info=0;

qsort((void*)K,nbrkey,sizeof(struct key),sort_function);

y=3;

PrintAt(2,1,"%-77s","List of the format");

prem=0;

do
{
// ChrWin(1,3,78,48,32);
// ColWin(1,3,78,48,10*16+1);

y=3;

for (n=prem;n<nbrkey;n++)
    {
    if (K[n].ext[0]=='*')
        {
        ChrLin(1,y,78,196);
        PrintAt(4,y,"%s",K[n].format);

        if (y&1==1)
            ColLin(1,y,78,10*16+3);
            else
            ColLin(1,y,78,15*16+3);
        }
        else
        {
        if (y&1==1)
            {
            ColLin(1,y,4,  10*16+3);
            ColLin(5,y,1,  10*16+3);
            ColLin(6,y,32, 10*16+4);
            ColLin(38,y,6, 10*16+3);
            ColLin(44,y,29,10*16+5);
            ColLin(73,y,1, 10*16+3);
            ColLin(74,y,5, 10*16+3);
            }
            else
            {
            ColLin(1,y,4,  15*16+3);
            ColLin(5,y,1,  15*16+3);
            ColLin(6,y,32, 15*16+4);
            ColLin(38,y,6, 15*16+3);
            ColLin(44,y,29,15*16+5);
            ColLin(73,y,1, 15*16+3);
            ColLin(74,y,5, 15*16+3);
            }

        PrintAt(1,y," %3s %-32s from %29s %4s ",K[n].ext,K[n].format,K[n].pro,K[n].other==1 ? "Info" : "----");

        if (K[n].other==1) info++;
        }

    y++;

    if ( (y==(Cfg->TailleY-2)) | (n==nbrkey-1) ) break;
    }

car=Wait(0,0,0);

switch(HI(car))
    {
    case 72:        // UP
        prem--;
        break;
    case 80:        // DOWN
        prem++;
        break;
    case 0x49:      // PGUP
        prem-=10;
        break;
    case 0x51:      // PGDN
        prem+=10;
        break;
    case 0x47:  // HOME
        prem=0;
        break;
    case 0X4F: // END
        prem=nbrkey-(Cfg->TailleY-5);
        break;
    }

if (prem<0) prem=0;
if (prem>nbrkey-(Cfg->TailleY-5)) prem=nbrkey-(Cfg->TailleY-5);


// if ( (y==48) | (n==nbrkey-1) )

}
while (car!=27);

ChargeEcran();
}

void SaveCfg(void)
{
int m,n,t;
FILE *fic;
short taille;
struct fenetre *Fen;

Cfg->FenTyp[0]=0;
Cfg->FenTyp[1]=0;

fic=fopen(Fics->CfgFile,"wb");
fwrite((void*)Cfg,sizeof(struct config),1,fic);

for(n=0;n<16;n++)
    {
    fwrite(&(Mask[n]->Ignore_Case),1,1,fic);
    fwrite(&(Mask[n]->Other_Col),1,1,fic);
    taille=strlen(Mask[n]->chaine);
    fwrite(&taille,2,1,fic);
    fwrite(Mask[n]->chaine,taille,1,fic);
    taille=strlen(Mask[n]->title);
    fwrite(&taille,2,1,fic);
    fwrite(Mask[n]->title,taille,1,fic);
    }

for(t=0;t<3;t++)
{
Fen=Fenetre[t];

fwrite(Fen->path,256,1,fic);
fwrite(&(Fen->order),sizeof(ENTIER),1,fic);
fwrite(&(Fen->sorting),sizeof(ENTIER),1,fic);

fwrite(&(Fen->nbrsel),4,1,fic);

for (n=0;n<=Fen->nbrfic;n++)
    {
    m=strlen(Fen->F[n]->name);
    fwrite(&m,4,1,fic);
    fwrite(Fen->F[n]->name,1,m,fic);
    }

fwrite(&(Fen->scur),sizeof(ENTIER),1,fic);
}


fclose(fic);
}


// Retourne -1 en cas d'erreur
//           0 si tout va bien
int LoadCfg(void)
{
int i,t,n,m;
FILE *fic;
short taille;
char nom[256];
struct fenetre *DFen;


fic=fopen(Fics->CfgFile,"rb");
if (fic==NULL) return -1;

fread((void*)Cfg,sizeof(struct config),1,fic);


for(n=0;n<16;n++)
    {
    fread(&(Mask[n]->Ignore_Case),1,1,fic);
    fread(&(Mask[n]->Other_Col),1,1,fic);
    fread(&taille,2,1,fic);
    fread(Mask[n]->chaine,taille,1,fic);
    Mask[n]->chaine[taille]=0;
    fread(&taille,2,1,fic);
    fread(Mask[n]->title,taille,1,fic);
    Mask[n]->title[taille]=0;
    }

for (t=0;t<3;t++)
    {
    DFen=Fenetre[t];

    DFen->FenTyp=Cfg->FenTyp[t];
    
    fread(DFen->path,256,1,fic);
    fread(&(DFen->order),sizeof(ENTIER),1,fic);
    fread(&(DFen->sorting),sizeof(ENTIER),1,fic);

    fread(&nbr,4,1,fic);

    DFen->nbrsel=0;

    for (i=0;i<=nbr;i++)
        {
        DFen->F[i]=GetMem(sizeof(struct file));

        fread(&m,4,1,fic);
        fread(nom,m,1,fic);
        nom[m]=0;

        DFen->F[i]->name=GetMem(m+1);
        memcpy(DFen->F[i]->name,nom,m+1);
        }

    DFen->nbrsel=nbr;

    fread(&(DFen->scur),sizeof(ENTIER),1,fic);
    }


fclose(fic);

return 0;
}




//---------------------------------
// Change drive of current window -
//---------------------------------
void ListDrive(char *lstdrv)
{
char drive[26],etat[26];
short m,n,x,l,nbr;
int xx,yy;
signed char i;

int car;


nbr=0;

for (i=0;i<26;i++)
    {
    etat[i]=0;
    drive[i]=GetDriveReady(i);
    if (drive[i]==0)
        nbr++;
    }

l=76/nbr;
if (l>3) l=3;
if (nbr<5) l=6;
if (nbr<2) l=9;
x=(80-(l*nbr))/2;

SaveEcran();
PutCur(32,0);

WinCadre(x-2,6,x+l*nbr+1,11,0);
ColWin(x-1,7,x+l*nbr,10,0*16+1);
ChrWin(x-1,7,x+l*nbr,10,32);

WinCadre(x-1,8,x+l*nbr,10,1);

PrintAt(x,7,"Select the drive");

m=x+l/2;
for (n=0;n<26;n++)
    {
    if (drive[n]==0)
        {
        if (VerifyDisk(n+1)==0)
            etat[n]=1;

        drive[n]=m;
        AffChr(m,9,n+'A');
        if (lstdrv[n]==0)
            AffCol(drive[n],9,0*16+1);
            else
            AffCol(drive[n],9,2*16+1);
        m+=l;
        }
        else
        {
        drive[n]=0;
        lstdrv[n]=0;
        }
    }


i=-1;
car=DROITE*256;

do	{
    do {


        if ( (LO(car)==32) & (drive[i]!=0) )
            {
            if (lstdrv[i]==0)
                {
                lstdrv[i]=1;
                if (VerifyDisk(i+1)==0)
                    etat[i]=1;
                    else
                    etat[i]=0;
                }
                else
                lstdrv[i]=0;
            car=9;
            }

        if (lstdrv[i]==0)
            AffCol(drive[i],9,0*16+1);
            else
            AffCol(drive[i],9,2*16+1);

        if (HI(car)==GAUCHE) i--;
        if (HI(car)==DROITE) i++;
        if (HI(car)==0xF) i--;
        if (LO(car)==9) i++;

        if (i==26) i=0;
        if (i<0) i=25;
        } while (drive[i]==0);



    xx=2;
    yy=13;

    for(n=0;n<26;n++)
        if (lstdrv[n]!=0)
            {
            PrintAt(xx,yy,"Search in %c: (%9s)",n+'A',etat[n]==1 ? "Ready" : "Not Ready");
            yy++;
            if (yy>=Cfg->TailleY-4) yy=13,xx+=39;
            }

    while(xx!=80)
        {
        PrintAt(xx,yy,"                        ");
        yy++;
        if (yy>=Cfg->TailleY-4) yy=13,xx+=39;
        }

    if (lstdrv[i]==0)
        AffCol(drive[i],9,1*16+5);
        else
        AffCol(drive[i],9,2*16+5);

    car=Wait(0,0,0);

    if (lstdrv[i]==0)
        AffCol(drive[i],9,0*16+1);
        else
        AffCol(drive[i],9,2*16+1);

} while ( (LO(car)!=27) & (LO(car)!=13));

ChargeEcran();

if (car==27)
    for (n=0;n<26;n++)
        lstdrv[n]=0;
}

void PutInPath(void)
{
char *TPath;
char path[2048];
int Mlen;
char erreur;
FILE *fic;

int i,j,k;
static char **dir;

SaveEcran();



k=Cfg->TailleY-5;

WinCadre(9,k-1,71,k+3,3);
ChrWin(10,k,70,k+2,32);
ColWin(10,k,70,k+2,10*16+1);
PrintAt(10,k,"Select the directory where you will that KKSETUP copy the");
PrintAt(10,k+1,"batch files for the execution of KK and KKDESC.");
PrintAt(10,k+2,"ESC for cancel.");

erreur=1;

TPath=getenv("PATH");
strcpy(path,TPath);
k=strlen(path);

for(i=0;i<k;i++)
    if (path[i]==';')
        path[i]=0;

dir=GetMem(50*sizeof(char *));

j=0;
Mlen=0;
for (i=0;i<50;i++)
    {
    dir[i]=path+j;
    dir[i]=strupr(dir[i]);
    if (strlen(dir[i])>Mlen) Mlen=strlen(dir[i]);
    if (strlen(dir[i])==0) break;
    while ( (j!=k) & (path[j]!=0) ) j++;
    j++;
    }

if (i!=0)
    {
    int x=2,y=5;
    int pos;
    int car;

    pos=0;

    SaveEcran();
    PutCur(32,0);

    WinCadre(x-2,y-1,x+Mlen+1,y+i,0);
    ColWin(x-1,y,x+Mlen,y+i-1,10*16+1);
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
            if (pos==-1) pos=i-1;
            break;
        case 80:        // DOWN
            pos++;
            if (pos==i) pos=0;
            break;
        case 0x47:      // HOME
            pos=0;
            break;
        case 0x4F:      // END
            pos=i-1;
            break;
        case 0x49:      // PGUP
            pos-=5;
            if (pos<0) pos=0;
            break;
        case 0x51:      // PGDN
            pos+=5;
            if (pos>=i) pos=i-1;
            break;
        }
    }
    while ( (car!=13) & (car!=27) & (HI(car)!=0x8D) );

    ChargeEcran();

    if (car==13)
        {
        strcpy(path,dir[pos]);
        Path2Abs(path,"kk.bat");
        fic=fopen(path,"wt");
        if (fic!=NULL)
            {
            fprintf(fic,"@%s\\kk.exe\n",ActualPath);
            fprintf(fic,"@REM This file was making by KKSETUP\n");
            fclose(fic);

            strcpy(path,dir[pos]);
            Path2Abs(path,"kkdesc.bat");
            fic=fopen(path,"wt");
            if (fic!=NULL)
                {
                fprintf(fic,"@%s\\kkdesc.exe %%1 \n",ActualPath);
                fprintf(fic,"@REM This file was making by KKSETUP\n");
                fclose(fic);
                erreur=0;

                strcpy(PathOfKK,dir[pos]);
                }
            }
        }
    }
free(dir);

if (erreur==1)
    strcpy(PathOfKK,"");

ChargeEcran();
}

void main(short argc,char **argv)
{
char buffer[256];
char chaine[256];
short n;
int car;

char *path;

IOerr=1;

/*****************************
 - Initialisation de l'ecran -
 *****************************/

InitScreen();                   // Initialise toutes les donn‚es HARD

/***********************
 - Gestion des erreurs -
 ***********************/

_harderr(Error_handler);

OldY=(*(char*)(0x484))+1;

path=GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
    }




/********************************
 - Initialisation des variables -
 ********************************/

Fenetre[0]=GetMem(sizeof(struct fenetre));
Fenetre[0]->F=GetMem(TOTFIC*sizeof(void *));        // allocation des pointeurs

Fenetre[1]=GetMem(sizeof(struct fenetre));
Fenetre[1]->F=GetMem(TOTFIC*sizeof(void *));        // allocation des pointeurs

Fenetre[2]=GetMem(sizeof(struct fenetre));
Fenetre[2]->F=GetMem(TOTFIC*sizeof(void *));        // allocation des pointeurs

Cfg=GetMem(sizeof(struct config));
Fics=GetMem(sizeof(struct fichier));

Mask=GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    Mask[n]=GetMem(sizeof(struct PourMask));

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

Fics->path=GetMem(256);
strcpy(Fics->path,path);

Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\kksetup.hlp");

if (LoadCfg()==-1)
    {
    struct fenetre *DFen;
    int t;

    DefaultCfg();

    for (t=0;t<3;t++)
        {
        DFen=Fenetre[t];

        DFen->FenTyp=Cfg->FenTyp[t];

        strcpy(DFen->path,Fics->path);
        DFen->order=1;
        DFen->sorting=1;

        DFen->nbrsel=0;

        DFen->F[0]=GetMem(sizeof(struct file));

        DFen->F[0]->name=GetMem(256);
        strcpy(DFen->F[0]->name,"kk.exe");


        DFen->scur=0;
        }
    ConfigFile();
    }

TXTMode(Cfg->TailleY);
NoFlash();

switch (Cfg->TailleY)
    {
    case 50:
        Font8x8();
        break;
    case 25:
    case 30:
        Font8x16();
        break;
    }


SetPal(0, 43, 37, 30);
SetPal(1, 31, 22, 17);
SetPal(2, 0, 0, 0);
SetPal(3, 58, 58, 50);
SetPal(4, 44, 63, 63);
SetPal(5, 63, 63, 21);
SetPal(6,43,37,30);
SetPal(7,  0,  0,  0);
SetPal(10, 43, 37, 30);
SetPal(15, 47, 41, 34);

WinCadre(0,0,79,(Cfg->TailleY-1),1);
ChrWin(1,1,78,(Cfg->TailleY-2),32);
ColWin(1,1,78,(Cfg->TailleY-2),10*16+1);
ColLin(1,1,78,10*16+5);
WinLine(1,2,78,0);

PrintAt(21,1,"Setup of Ketchup Killers Commander");


posy=3;



strcpy(chaine,Fics->path);
strcat(chaine,"\\trash");
mkdir(chaine);


PrintAt(10, 5,"F1: Help");

PrintAt(10, 9,"F2: Search Application");

PrintAt(10,13,"F3: Load KKSETUP.INI");

PrintAt(10,17,"F4: List all the format");

PutCur(32,0);


/********************************
 - Insertion de KK dans la path -
 - Si pas d‚ja pr‚sent !        -
 ********************************/
strcpy(ActualPath,path);

_searchenv("KK.BAT","PATH",buffer);
if (strlen(buffer)!=0)
    {
    FILE *fic;
    static char toto[256];
    static char tata[256];

    fic=fopen(buffer,"rt");
    fgets(toto,256,fic);

    sprintf(tata,"@%s\\kk.exe\n",ActualPath);

    if (stricmp(tata,toto)!=0)
        PutInPath();

    Path2Abs(buffer,"..");
    strcpy(PathOfKK,buffer);
    }
    else
    PutInPath();


/*****************************
 - Gestion du menu principal -
 *****************************/

do
{

car=Wait(0,0,0);

switch(HI(car))
    {
    case 0x3B:  // F1
        SaveEcran();
        PutCur(32,0);
        ChrWin(0,0,79,49,32);
        Help();
        ChargeEcran();
        break;
    case 0x3C:  // F2
        ApplSearch();
        break;
    case 0x3D:  // F3
        ConfigFile();
        break;
    case 0x3E:  // F4
        IdfListe();
        break;
    case 0x3F:
        PutInPath();
        break;
    }

} while ( (HI(car)!=0x44) & (LO(car)!=27) );

if (strlen(PathOfKK)!=0)
    {
    PrintAt(10,(Cfg->TailleY-6),"KK.BAT & KKDESC.BAT are now in PATH (%s)",PathOfKK);
    PrintAt(20,(Cfg->TailleY-5),"-> You could run KK from everywhere");
    PrintAt(10,(Cfg->TailleY-4),"%s is done",chaine);
    }
    else
    {
    PrintAt(10,(Cfg->TailleY-5),"WARNING: You couldn't run KK from everywhere (Reload KKSETUP)");
    PrintAt(10,(Cfg->TailleY-4),"%s is done",chaine);
    }


PrintAt(29,(Cfg->TailleY-2),"Press a key to continue");
ColLin(1,(Cfg->TailleY-2),78,0*16+2);

Wait(0,0,0);
TXTMode(OldY);

SaveCfg();
}


void ApplSearch(void)
{
char lstdrv[26];
short n,t;
char ch[256];
FILE *fic;

SaveEcran();
PutCur(32,0);

ChrWin(1,3,78,(Cfg->TailleY-2),32);
ColWin(1,3,78,(Cfg->TailleY-2),10*16+1);

nbr=0;

for (n=2;n<26;n++)
    lstdrv[n]=1;
lstdrv[0]=0;
lstdrv[1]=0;

ListDrive(lstdrv);

for (n=0;n<26;n++)
    {
    sprintf(ch,"%c:\\",n+'A');
    if (lstdrv[n]==1)
        if (VerifyDisk(n+1)==0)
            KKR_Search(ch);
    }

nbrdir=0;

for (n=0;n<26;n++)
    {
    sprintf(ch,"%c:\\*.*",n+'A');
    if (lstdrv[n]==1)
        if (VerifyDisk(n+1)==0)
            SSearch(ch);
    }

fic=fopen("idfext.rb","wb");
if (fic!=NULL) {
	fwrite("RedBLEXU",1,8,fic);

    fputc(_getdrive()-1,fic);

	t=0;

	for(n=0;n<nbr;n++)
        if (app[n]->pres!=0) t++;

	fwrite(&t,1,2,fic);

	for(n=0;n<nbr;n++)
        if (app[n]->pres!=0)  {
            char sn;
            char *a;

            a=app[n]->Filename;
            sn=strlen(a);
            fwrite(&sn,1,1,fic);
            fwrite(a,sn,1,fic);

            a=app[n]->Titre;
            sn=strlen(a);
            fwrite(&sn,1,1,fic);
            fwrite(a,sn,1,fic);

            a=app[n]->Meneur;
            sn=strlen(a);
            fwrite(&sn,1,1,fic);
            fwrite(a,sn,1,fic);

            fwrite(&(app[n]->ext),2,1,fic);    // Numero de format
            fwrite(&(app[n]->pres),2,1,fic);   // Numero directory

            fwrite(&(app[n]->type),1,1,fic);   // Numero directory
            }

	fwrite(&nbrdir,1,2,fic);

	for(n=0;n<nbrdir;n++)
		fwrite(dir[n],1,128,fic);

	fclose(fic);
	}

PrintAt(29,(Cfg->TailleY-2),"Press a key to continue");
ColLin(1,(Cfg->TailleY-2),78,0*16+2);

Wait(0,0,0);
ColWin(1,1,78,(Cfg->TailleY-2),0*16+1);
ChrWin(1,1,78,(Cfg->TailleY-2),32);  // '±'

PrintAt(10,10,"Stat.");
PrintAt(10,12,"I have found %3d applications",St_App);
PrintAt(10,13,"            in %3d directories",St_Dir);

Wait(0,0,0);

ChargeEcran();
}

char KKR_Read(FILE *Fic)
{
char Key[4];
char KKType;
char Comment[255],SComment;
char Titre[255],STitre;
char Meneur[255],SMeneur;
char Filename[255],SFilename;
int Checksum;
short format;

char Code;
char fin;

fin=0;

fread(Key,4,1,Fic);
if (!strncmp(Key,"KKRB",4))
    do {
    fread(&Code,1,1,Fic);
    switch(Code)  {
        case 0:             // Commentaire (sans importance)
            fread(&SComment,1,1,Fic);
            fread(Comment,SComment,1,Fic);
            break;
        case 1:             // Code Titre
            fread(&STitre,1,1,Fic);
            Titre[STitre]=0;
            fread(Titre,STitre,1,Fic);

            PrintAt(3,posy,"Loading information about %s",Titre);
            posy++;

            if (posy>(Cfg->TailleY-2))
                {
                MoveText(1,4,78,(Cfg->TailleY-2),1,3);
                posy--;
                ChrLin(1,(Cfg->TailleY-2),78,32);
                }
            break;
        case 2:             // Code Programmeur
            fread(&SMeneur,1,1,Fic);
            fread(Meneur,SMeneur,1,Fic);
            Meneur[SMeneur]=0;
            break;
        case 3:             // Code Nom du programme
            fread(&SFilename,1,1,Fic);
            Filename[SFilename]=0;
            fread(Filename,SFilename,1,Fic);
            break;
        case 4:             // Checksum
            fread(&Checksum,4,1,Fic);
            break;
        case 5:             // Format
            fread(&format,2,1,Fic);
            app[nbr]=malloc(sizeof(struct player));

            app[nbr]->Filename=malloc(SFilename+1);
            strcpy(app[nbr]->Filename,Filename);

            app[nbr]->Meneur=malloc(SMeneur+1);
            strcpy(app[nbr]->Meneur,Meneur);

            app[nbr]->Titre=malloc(STitre+1);
            strcpy(app[nbr]->Titre,Titre);

            app[nbr]->Checksum=Checksum;
            app[nbr]->ext=format;
            app[nbr]->pres=0;

            app[nbr]->type=KKType;

            nbr++;
            break;
        case 6:             // Fin de fichier
            fin=1;
            break;
        case 7:             // Reset
            Checksum=0;

            strcpy(Titre,"?");
            strcpy(Meneur,"?");
            strcpy(Filename,".");

            SFilename=strlen(Filename);
            STitre=strlen(Titre);
            SMeneur=strlen(Meneur);

            KKType=0;
            break;
        case 8:             // Checksum
            fread(&KKType,1,1,Fic);
            break;
        }
    }
    while(fin==0);
    else return 0;
return 1;
}




void KKR_Search(char *nom2)
{
struct find_t fic;
char moi[256],nom[256];
char ok;
FILE *Fic;

char **TabRec;  // Tableau qui remplace les appels recursifs
int NbrRec;     // Nombre d'element dans le tableau

TabRec=malloc(500*sizeof(char*));
TabRec[0]=malloc(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
strcpy(nom,TabRec[NbrRec-1]);

PrintAt(1,1,"%-78s",nom);

strcpy(moi,nom);
strcat(moi,"*.KKR");

if (_dos_findfirst(moi,63-_A_SUBDIR,&fic)==0)
do
    {
    ok=0;
    if ((fic.attrib&_A_SUBDIR)!=_A_SUBDIR)  {
        strcpy(moi,nom);
        strcat(moi,fic.name);
        Fic=fopen(moi,"rb");
        if (Fic==NULL)  {
            PrintAt(0,0,"KKR_Read (1)");
            exit(1);
            }
        ok=KKR_Read(Fic);
        fclose(Fic);
        }
    }
while (_dos_findnext(&fic)==0);

free(TabRec[NbrRec-1]);
NbrRec--;

strcpy(moi,nom);
strcat(moi,"*.*");

if (_dos_findfirst(moi,_A_SUBDIR,&fic)==0)
do
    {
    if  ( (fic.name[0]!='.') & (((fic.attrib) & _A_SUBDIR) == _A_SUBDIR) )
            {
            strcpy(moi,nom);
            strcat(moi,fic.name);
            strcat(moi,"\\");

            TabRec[NbrRec]=malloc(strlen(moi)+1);
            memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
            NbrRec++;
            }
    }
while (_dos_findnext(&fic)==0);
}
while(NbrRec>0);


free(TabRec);

}


// int Allform[1024];


void SSearch(char *nom2)
{
struct find_t fic;
char moi[256],nom[256];
short n;
short o;
char ok;
char bill;
signed long wok;

unsigned long KKcrc;
unsigned long K1crc;    // crc calcul‚ une fois pour toutes

unsigned long C;

char **TabRec;  // Tableau qui remplace les appels recursifs
int NbrRec;     // Nombre d'element dans le tableau

char *StrVerif,Verif;

TabRec=malloc(500*sizeof(char*));
TabRec[0]=malloc(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
o=nbrdir+1;

PrintAt(1,1,"%-78s",nom);
St_Dir++;

strcpy(nom,TabRec[NbrRec-1]);

PrintAt(1,1,"%-78s",nom);

if (_dos_findfirst(nom,63-_A_SUBDIR,&fic)==0)
do
    {
    ok=0;
    wok=-1;

    if ((fic.attrib&_A_SUBDIR)!=_A_SUBDIR)
        {
        C=0;
        KKcrc=0;    // CRC du fichier courant
        K1crc=0;

        for(n=0;n<nbr;n++)
            if ( (!stricmp(fic.name,app[n]->Filename)) & (app[n]->Checksum!=0) )
                {
                if (KKcrc==0)
                    {
                    if (K1crc==0)
                        {
                        strcpy(moi,nom);
                        moi[strlen(moi)-3]=0;
                        strcat(moi,fic.name);
                        crc32file(moi,&KKcrc);
                        K1crc=KKcrc;
                        }
                        else
                        KKcrc=K1crc;

                    if (KKcrc!=app[n]->Checksum) KKcrc=0;
                    }
                }

        Verif=0;
        StrVerif=app[0]->Titre;     // pour pas mettre NULL, ca veut rien dire

        for(n=0;n<nbr;n++)
            {
            bill=0;
            if (!stricmp(fic.name,app[n]->Filename))
                {
                if ( (KKcrc==0) & (app[n]->Checksum==0) )
                    {
                    int x,t;

                    if ( (Verif==0) | (strcmp(StrVerif,app[n]->Titre)!=0) )
                        {
                        strcpy(moi,nom);
                        moi[strlen(moi)-3]=0;
                        strcat(moi,fic.name);

                        x=20;
                        if (strlen(moi)>x) x=strlen(moi);
                        if ((6+strlen(app[n]->Titre))>x) x=(strlen(app[n]->Titre)+6);
                        if (strlen(app[n]->Meneur)>x) x=strlen(app[n]->Meneur);

                        if (x>78) x=78;
                        t=(80-x)/2;

                        SaveEcran();
                        PutCur(32,0);

                        WinCadre(t-1,9,t+x,15,0);
                        ColWin(t,10,t+x-1,14,10*16+2);
                        ChrWin(t,10,t+x-1,14,32);
                        PrintAt(t,10,"Do you think that");
                        PrintAt(t,11,"%s",moi);
                        PrintAt(t,12,"is %s of",app[n]->Titre);
                        PrintAt(t,13,"%s",app[n]->Meneur);
                        PrintAt(t,14,"(Y/N)");

                        do
                            {
                            t=Wait(0,0,0);
                            }
                        while ( (t!='y') & (t!='Y') & (t!='n') & (t!='N'));

                        ChargeEcran();
                        if ( (t=='y') | (t=='Y') )
                            Verif=1;
                            else
                            Verif=2;

                        StrVerif=app[n]->Titre;
                        }

                    if (Verif==1)
                        {
                        app[n]->pres=o;   // l'appl. n est presente dans le dir. o
                        bill=1;
                        }
                    }

                if ( (KKcrc==app[n]->Checksum) & (KKcrc!=0) )
                    {
                    app[n]->pres=o;
                    bill=1;
                    }

                if (bill==1)
                    {
                    strcpy(moi,nom);
                    moi[strlen(moi)-3]=0;
                    ok=1;
                    wok=n;
                    }
                }
            }
        }

    if (ok==1)
        {
        St_App++;

        nbrdir=o;
        strcpy(dir[o-1],moi);

        PrintAt(3,posy,"Found %s in %s",app[wok]->Titre,dir[o-1]);
        posy++;

        if (posy>(Cfg->TailleY-2))
                {
                MoveText(1,4,78,(Cfg->TailleY-2),1,3);
                posy--;
                ChrLin(1,(Cfg->TailleY-2),78,32);
                }
        }
   }
while (_dos_findnext(&fic)==0);

free(TabRec[NbrRec-1]);
NbrRec--;

if (_dos_findfirst(nom,_A_SUBDIR,&fic)==0)
do
	{
    if  ( (fic.name[0]!='.') & (((fic.attrib) & _A_SUBDIR) == _A_SUBDIR) )
			{
			strcpy(moi,nom);
			moi[strlen(moi)-3]=0;
            strcat(moi,fic.name);
			strcat(moi,"\\*.*");

            TabRec[NbrRec]=malloc(strlen(moi)+1);
            memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
            NbrRec++;
			}
	}
while (_dos_findnext(&fic)==0);

}
while(NbrRec>0);


free(TabRec);
}


char TestCar(char c)
{
if (c==32) return 1;
if (c=='=') return 1;
return 0;
}

void ClearAllSpace(char *name)
{
char c,buf[128];
short i,j;

i=0;    // navigation dans name
j=0;    // position dans buf

while ( (TestCar(name[i])) & (name[i]!=0) ) i++;

if (name[i]!=0)
    while ((c=name[i])!=0)
        {
        buf[j]=name[i];
        j++;
        i++;
        if (TestCar(name[i]))
            {
            c=name[i];
            while ( (TestCar(name[i])) & (name[i]!=0) )
                {
                if (name[i]=='=') c='=';
                i++;
                }
            buf[j]=c;
            j++;
            }
        }
buf[j]=0;

strcpy(name,buf);
}

int Traite(char *from,char *to)
{
int i;

i=0;

while( (from[i]!=0) & (from[i]!='=') )
    {
    to[i]=from[i];
    i++;
    }
to[i]=0;

if (from[i]!=0)
    {
    sscanf(from+i+1,"%d",&i);
    return i;
    }
return 0;
}



void ConfigFile(void)
{
FILE *fic;
char chaine[128];

char var[128];
int valeur;

char erreur;

fic=fopen("kksetup.ini","rt");
if (fic==NULL) return;

SaveEcran();
PutCur(32,0);


while (fgets(chaine,128,fic)!=NULL)
{
erreur=1;
ClearAllSpace(chaine);

if (chaine[0]==';') erreur=0;

if (chaine[0]=='\n') erreur=0;

if (strlen(chaine)>2) chaine[strlen(chaine)-1]=0;

valeur=Traite(chaine,var);

if (!stricmp(var,"mask"))
    {
    Cfg->wmask=valeur;
    erreur=0;
    }

if (!stricmp(var,"vsize"))
    {
    Cfg->TailleY=valeur;
    erreur=0;
    }

if (!stricmp(var,"wintype"))
    {
    Cfg->fentype=valeur;
    erreur=0;
    }

if (!stricmp(var,"ansispeed"))
    {
    Cfg->AnsiSpeed=valeur;
    erreur=0;
    }

if (!stricmp(var,"ssaverspeed"))
    {
    Cfg->SaveSpeed=valeur;
    erreur=0;
    }

if (!stricmp(var,"directpoint"))
    {
    Cfg->pntrep=valeur;
    erreur=0;
    }

if (!stricmp(var,"hiddenfile"))
    {
    Cfg->hidfil=valeur;
    erreur=0;
    }

if (!stricmp(var,"logfile"))
    {
    Cfg->logfile=valeur;
    erreur=0;
    }

if (!stricmp(var,"font"))
    {
    Cfg->font=valeur;
    erreur=0;
    }

if (!stricmp(var,"debug"))
    {
    Cfg->debug=valeur;
    erreur=0;
    }

if (!stricmp(var,"sizetrash"))
    {
    Cfg->mtrash=valeur;
    erreur=0;
    }

if (erreur==1)
    {
    WinError(chaine);
    }
}

ChargeEcran();
}


