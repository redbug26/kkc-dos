/*--------------------------------------------------------------------*\
|- KKSETUP: Main configuration program                                -|
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
#include <bios.h>

#include <time.h>

#include "kk.h"
#include "idf.h"
#include "rbdos.h"

#define MAXDIR 250

char *RBTitle2="Ketchup Killers Setup V"VERSION" / RedBug";

#define GAUCHE 0x4B
#define DROITE 0x4D

extern struct key K[nbrkey];
extern int IOerr;

char PathOfKK[256];
char ActualPath[256];

char LoadDefCfg;

struct kkconfig *KKCfg;
struct PourMask **Mask;
struct kkfichier *KKFics;

/*--------------------------------------------------------------------*\
|- Fake MenuCreat pour ficidf                                         -|
\*--------------------------------------------------------------------*/
void MenuCreat(char *titbuf,char *buf,char *path)
{

}


// Variable globale

int todo;


/*--------------------------------------------------------------------*\
|- prototype                                                          -|
\*--------------------------------------------------------------------*/



int crc32file(char *name,unsigned long *crc);  // Compute CRC-32 of file
int FileComp(char *a,char *b);   // Comparaison entre 2 noms de fichiers

void SearchPart(char *);            // Search in file .ini all the parts

void InitMode(void);
void ClearSpace(char *name);    //--- efface les espaces inutiles ------

void Interroge(char *path,struct player *app,char *Verif,char *GVerif);

void Col2Str(char *from,char *to);
void Str2Col(char *from,char *to);

void ChangePalette(void);
void ColorChange(void);
void IColor(void);       // InitColor
int DColor(int col);

void AffColScreen(int a);

/*--------------------------------------------------------------------*\
|- Pour Statistique;                                                  -|
\*--------------------------------------------------------------------*/
int St_App;
int St_Dir;

char GVerif=0;  // Verification globale: 0, on interroge
                //                       1, toujours oui
                //                       2, toujours non

struct player {
    char *Filename;
    char *Meneur;
    char *Titre;
    unsigned long Checksum;
    short ext;                                     // Numero d'extension
    short pres;             // 0 si pas trouv‚ sinon numero du directory
    char type;
    char os;
    char info;
    } *app[5000];

char dir[MAXDIR][128];      // 50 directory diff‚rents de 128 caracteres

short nbr;              // nombre d'application lu dans les fichiers KKR
short nbrdir;

long OldCol;                                // Ancienne couleur du texte
long OldY,OldX,PosX,PosY;

char *Screen_Adr=(char*)0xB8000;
char *Screen_Buffer;


FENETRE *Fenetre[NBWIN];

void SSearch(char *nom);
void ApplSearch(void);
void ClearAllSpace(char *name);

void GestionFct(int i);
int GestionBar(void);

char ShowYourPlayer(void);



/*--------------------------------------------------------------------*
 -                          Gestion Message                           -
 *--------------------------------------------------------------------*/

int MesgY;

/*--------------------------------------------------------------------*
 -                      Initialise les messages                       -
 *--------------------------------------------------------------------*/
void InitMessage(void)
{
MesgY=3;
}

/*--------------------------------------------------------------------*
 -                        Affiche un message                          -
 *--------------------------------------------------------------------*/
void DispMessage(char *string,...)
{
static char sortie[256];
va_list arglist;
char *suite;
int a;

suite=sortie;

va_start(arglist,string);
vsprintf(sortie,string,arglist);
va_end(arglist);

a=2;
while (*suite!=0)
	{
/*
    AffChr(a,MesgY,'Û');
    while ((inp(0x3DA) & 8)==8);
    while ((inp(0x3DA) & 8)!=8);
    while ((inp(0x3DA) & 1)==1);
    while ((inp(0x3DA) & 1)!=1);
*/
    AffChr(a,MesgY,*suite);
	a++;
	suite++;
    GotoXY(a,MesgY);
    PutCur(7,7);
	}

MesgY++;
if (MesgY>(Cfg->TailleY-3))
    {
    MoveText(1,3,Cfg->TailleX-2,Cfg->TailleY-3,1,2);
    MesgY--;
    ChrLin(1,(Cfg->TailleY-3),Cfg->TailleX-2,32);
    }
}

/*--------------------------------------------------------------------*
 -                         Gestion profile                            -
 *--------------------------------------------------------------------*/

#define MAX_LINE_LENGTH    80

int get_private_profile_int(char *, char *, int,    char *);
int get_private_profile_string(char *,char *,char *,char *,int, char *);
int write_private_profile_string(char *, char *, char *, char *);
int write_private_profile_int(char *, char *, int, char *);

/*--------------------------------------------------------------------*
 - Function:     read_line()                                          -
 - Arguments:    <FILE *> fp - a pointer to the file to be read from  -
 -               <char *> bp - a pointer to the copy buffer           -
 - Returns:      TRUE if successful FALSE otherwise                   -
 *--------------------------------------------------------------------*/
int read_line(FILE *fp, char *bp)
{
int cc;
int i = 0;
/* Read one line from the source file */

while( (cc = getc(fp)) != '\n' )
    {
    if (cc==-1) return 0;       /* return FALSE on unexpected EOF */
    bp[i++] = cc;
    }
bp[i] = '\0';
return(1);
}
/*--------------------------------------------------------------------*
 - Function:     get_private_profile_int()                            -
 - Arg: <char *> section - the name of the section to search for      -
 -      <char *> entry - the name of the entry to find the value of   -
 -      <int> def - the default value in the event of a failed read   -
 -      <char *> file_name - the name of the .ini file to read from   -
 - Returns:      the value located at entry                           -
 *--------------------------------------------------------------------*/
int get_private_profile_int(char *section,char *entry, int def,
                                                        char *file_name)
{
FILE *fp = fopen(file_name,"r");
char buff[MAX_LINE_LENGTH];
char *ep;
char t_section[MAX_LINE_LENGTH];
char value[6];

int len = strlen(entry);
int i;
if (!fp) return(0);

sprintf(t_section,"[%s]",section);            // Format the section name
 // Move through file 1 line at a time until a section is matched or EOF

do
    {
    if( !read_line(fp,buff) )
        {
        fclose(fp);
        return(def);
        }
    } while( strcmp(buff,t_section) );
                  // Now that the section has been found, find the entry
                       // Stop searching upon leaving the section's area
do
    {
    if( !read_line(fp,buff) || buff[0] == '[' )
        {
        fclose(fp);
        return(def);
        }
    }  while( strncmp(buff,entry,len) );

ep = strrchr(buff,'=');                      // Parse out the equal sign
ep++;
if( !strlen(ep) )                                         // No setting?
        return(def);
                                 // Copy only numbers fail on characters
for(i = 0; isdigit(ep[i]); i++ )
    value[i] = ep[i];

value[i] = '\0';
fclose(fp);                             // Clean up and return the value
return(atoi(value));
}
/*--------------------------------------------------------------------*
 - Function:     get_private_profile_string()                         -
 - Arg:   <char *> section - the name of the section to search for    -
 -        <char *> entry - the name of the entry to find the value of -
 -        <char *> def - default string in the event of a failed read -
 -        <char *> buffer - a pointer to the buffer to copy into      -
 -        <int> buffer_len - the max number of characters to copy     -
 -        <char *> file_name - the name of the .ini file to read from -
 - Returns:  the number of characters copied into the supplied buffer -
 *--------------------------------------------------------------------*/

int get_private_profile_string(char *section, char *entry, char *def, 
    char *buffer, int buffer_len, char *file_name)
{
FILE *fp = fopen(file_name,"r");
char buff[MAX_LINE_LENGTH];
char *ep;
char t_section[MAX_LINE_LENGTH];
int len = strlen(entry);

if( !fp ) return(0);
sprintf(t_section,"[%s]",section);            // Format the section name
//  Move through file 1 line at a time until a section is matched or EOF

do
    {
    if( !read_line(fp,buff) )
        {
        fclose(fp);
        strncpy(buffer,def,buffer_len);
        return(strlen(buffer));
        }
    }
    while( strcmp(buff,t_section) );

                  // Now that the section has been found, find the entry
                       // Stop searching upon leaving the section's area
do
    {
    if( !read_line(fp,buff) || buff[0] == '[' )
        {
        fclose(fp);
        strncpy(buffer,def,buffer_len);
        return(strlen(buffer));
        }
    }  while( strncmp(buff,entry,len) );

ep = strrchr(buff,'=');                      // Parse out the equal sign
ep++;
                                // Copy up to buffer_len chars to buffer
memcpy(buffer,ep,buffer_len);

buffer[buffer_len] = '\0';
fclose(fp);                     // Clean up and return the amount copied
return(strlen(buffer));
}


/*--------------------------------------------------------------------*
 - Function:    write_private_profile_string()                        -
 - Arg: <char *> section - the name of the section to search for      -
 -      <char *> entry - the name of the entry to find the value of   -
 -      <char *> buffer - pointer to the buffer that holds the string -
 -      <char *> file_name - the name of the .ini file to read from   -
 - Returns:     TRUE if successful, otherwise FALSE                   -
 *--------------------------------------------------------------------*/
int write_private_profile_string(char *section, char *entry,
                                 char *buffer, char *file_name)
{
FILE *rfp, *wfp;
char tmp_name[15];
char buff[MAX_LINE_LENGTH];
char t_section[MAX_LINE_LENGTH];
int len = strlen(entry);
tmpnam(tmp_name);                // Get a temporary file name to copy to
sprintf(t_section,"[%s]",section);            // Format the section name
if( !(rfp = fopen(file_name,"r")) )    // If the .ini file doesn't exist
    {
    if( !(wfp = fopen(file_name,"w")) )                 // then make one
        {
        return(0);
        }
    fprintf(wfp,"%s\n",t_section);
    fprintf(wfp,"%s=%s\n",entry,buffer);
    fclose(wfp);
    return(1);
    }

if( !(wfp = fopen(tmp_name,"w")) )
    {
    fclose(rfp);
    return(0);
    }
          // Move through the file one line at a time until a section is
                // matched or until EOF. Copy to temp file as it is read
do
    {
    if( !read_line(rfp,buff) )
        {               // Failed to find section, so add one to the end
        fprintf(wfp,"\n%s\n",t_section);
        fprintf(wfp,"%s=%s\n",entry,buffer);
                                                  // Clean up and rename
        fclose(rfp);
        fclose(wfp);
        unlink(file_name);
        rename(tmp_name,file_name);
        return(1);
        }
    fprintf(wfp,"%s\n",buff);
    } while( strcmp(buff,t_section) );

  // Now that the section has been found, find the entry. Stop searching
         // upon leaving the section's area. Copy the file as it is read
                              // and create an entry if one is not found

while( 1 )
    {
    if( !read_line(rfp,buff) )
        {                            // EOF without an entry so make one
        fprintf(wfp,"%s=%s\n",entry,buffer);      // Clean up and rename
        fclose(rfp);
        fclose(wfp);
        
        return(1);
        }

    if( !strncmp(buff,entry,len) || buff[0] == '\0' )
        break;
    fprintf(wfp,"%s\n",buff);
    }

if( buff[0] == '\0' )
    {
    fprintf(wfp,"%s=%s\n",entry,buffer);
    do
        {
        fprintf(wfp,"%s\n",buff);
        }
    while( read_line(rfp,buff) );
    }
    else
    {
    fprintf(wfp,"%s=%s\n",entry,buffer);
    while( read_line(rfp,buff) )
        {
        fprintf(wfp,"%s\n",buff);
        }
    }

//--- Clean up and rename ----------------------------------------------
fclose(wfp);
fclose(rfp);
unlink(file_name);
rename(tmp_name,file_name);
return(1);
}


int write_private_profile_int(char *section, char *entry, int entier,
                           char *file_name)
{
char buffer[32];

sprintf(buffer,"%d",entier);

return write_private_profile_string(section,entry,buffer,file_name);
}



int sort_function(const void *a,const void *b)
{
struct key *a1,*b1;

a1=(struct key*)a;
b1=(struct key*)b;

// return (a1->numero)-(b1->numero);

if (a1->type!=b1->type) return (a1->type)-(b1->type);

return strcmp(a1->ext,b1->ext);                           // ou format ?
}


/*--------------------------------------------------------------------*\
|- Liste des fichiers reconnus par IDF                                -|
\*--------------------------------------------------------------------*/
void IdfListe(void)
{
int car,y;

int n;
int info;

int prem;

SaveScreen();
PutCur(32,0);

info=0;

qsort((void*)K,nbrkey,sizeof(struct key),sort_function);

y=3;

PrintAt(2,0,"%-77s","List of the format");

prem=0;

do
{
y=3;

for (n=prem;n<nbrkey;n++)
    {
    if (K[n].ext[0]=='*')
        {
        ChrLin(1,y,Cfg->TailleX-2,196);
        PrintAt(4,y,"%s",K[n].format);

        if (y&1==1)
            ColLin(1,y,Cfg->TailleX-2,Cfg->col[17]);
            else
            ColLin(1,y,Cfg->TailleX-2,Cfg->col[16]);
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
            if (Cfg->TailleX!=80)
                ColLin(79,y,Cfg->TailleX-80,10*16+3),
                ChrLin(79,y,Cfg->TailleX-80,32);
            }
            else
            {
            ColLin(1,y,4,  1*16+3);
            ColLin(5,y,1,  1*16+3);
            ColLin(6,y,32, 1*16+4);
            ColLin(38,y,6, 1*16+3);
            ColLin(44,y,29,1*16+5);
            ColLin(73,y,1, 1*16+3);
            ColLin(74,y,5, 1*16+3);
            if (Cfg->TailleX!=80)
                ColLin(79,y,Cfg->TailleX-80,1*16+3),
                ChrLin(79,y,Cfg->TailleX-80,32);
            }

        PrintAt(1,y," %3s %-32s from %29s %4s ",K[n].ext,
            K[n].format,K[n].pro,((K[n].other)&1)==1 ? "Info" : "----");

        if (((K[n].other)&1)==1) info++;
        }

    y++;

    if ( (y==(Cfg->TailleY-3)) | (n==nbrkey-1) ) break;
    }

car=Wait(0,0,0);

switch(HI(car))
    {
    case 72:                                                       // UP
        prem--;
        break;
    case 80:                                                     // DOWN
        prem++;
        break;
    case 0x49:                                                   // PGUP
        prem-=10;
        break;
    case 0x51:                                                   // PGDN
        prem+=10;
        break;
    case 0x47:                                                   // HOME
        prem=0;
        break;
    case 0X4F:                                                    // END
        prem=nbrkey-(Cfg->TailleY-6);
        break;
    }

if (prem<0)
    prem=0;
if (prem>nbrkey-(Cfg->TailleY-6))
    prem=nbrkey-(Cfg->TailleY-6);

}
while (car!=27);

LoadScreen();
}

void SaveCfg(void)
{
int m,n,t;
FILE *fic;
short taille;
FENETRE *Fen;

KKCfg->FenTyp[0]=0;
KKCfg->FenTyp[1]=0;

fic=fopen(KKFics->CfgFile,"wb");
fwrite((void*)Cfg,sizeof(struct config),1,fic);

fwrite((void*)KKCfg,sizeof(struct kkconfig),1,fic);

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

for(t=0;t<NBWIN;t++)
{
Fen=Fenetre[t];

fwrite(Fen->path,256,1,fic);
fwrite(&(Fen->order),sizeof(short),1,fic);
fwrite(&(Fen->sorting),sizeof(short),1,fic);

fwrite(&(Fen->nbrfic),4,1,fic);

for (n=0;n<=Fen->nbrfic;n++)
    {
    m=strlen(Fen->F[n]->name);
    fwrite(&m,4,1,fic);
    fwrite(Fen->F[n]->name,1,m,fic);
    }

fwrite(&(Fen->scur),sizeof(short),1,fic);
}


fclose(fic);
}



/*--------------------------------------------------------------------*\
|- Chargement de KKRB.CFG                                             -|
|-  Retourne -1 en cas d'erreur                                       -|
|-            0 si tout va bien                                       -|
\*--------------------------------------------------------------------*/
int LoadCfg(void)
{
int i,t,n,m;
FILE *fic;
short taille;
char nom[256];
FENETRE *DFen;


fic=fopen(KKFics->CfgFile,"rb");
if (fic==NULL) return -1;

fread((void*)Cfg,sizeof(struct config),1,fic);

fread((void*)KKCfg,sizeof(struct kkconfig),1,fic);

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

for (t=0;t<NBWIN;t++)
    {
    DFen=Fenetre[t];

    DFen->FenTyp=KKCfg->FenTyp[t];
    
    fread(DFen->path,256,1,fic);
    fread(&(DFen->order),sizeof(short),1,fic);
    fread(&(DFen->sorting),sizeof(short),1,fic);

    fread(&(DFen->nbrfic),4,1,fic);

    for (i=0;i<=DFen->nbrfic;i++)
        {
        DFen->F[i]=GetMem(sizeof(struct file));

        fread(&m,4,1,fic);
        fread(nom,m,1,fic);
        nom[m]=0;

        DFen->F[i]->name=GetMem(m+1);
        memcpy(DFen->F[i]->name,nom,m+1);
        }

    fread(&(DFen->scur),sizeof(short),1,fic);
    }


fclose(fic);

return 0;
}


/*--------------------------------------------------------------------*\
|-                 Change drive of current window -                   -|
\*--------------------------------------------------------------------*/
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
    drive[i]=DriveExist(i);
    if (drive[i]==1)
        nbr++;
    }

l=76/nbr;
if (l>3) l=3;
if (nbr<5) l=6;
if (nbr<2) l=9;
x=(Cfg->TailleX-(l*nbr))/2;

SaveScreen();
PutCur(32,0);

Cadre(x-2,6,x+l*nbr+1,11,0,Cfg->col[46],Cfg->col[47]);
Window(x-1,7,x+l*nbr,10,Cfg->col[28]);

Cadre(x-1,8,x+l*nbr,10,1,Cfg->col[46],Cfg->col[47]);

PrintAt(x,7,"Select the drive");

m=x+l/2;
for (n=0;n<26;n++)
    {
    if (drive[n]==1)
        {
        if (DriveReady(n)==1)
            etat[n]=1;

        drive[n]=m;
        AffChr(m,9,n+'A');
        if (lstdrv[n]==0)
            AffCol(drive[n],9,14*16+7);
            else
            AffCol(drive[n],9,7*16+4);
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

do
    {
    do
        {
        if ( (LO(car)==32) & (drive[i]!=0) )
            {
            if (lstdrv[i]==0)
                {
                lstdrv[i]=1;
                if (DriveReady(i)==1)
                    etat[i]=1;
                    else
                    etat[i]=0;
                }
                else
                lstdrv[i]=0;
            car=9;
            }

        if (i>=0)
            {
            if (drive[i]!=0)
                {
                if (lstdrv[i]==0)
                    AffCol(drive[i],9,14*16+7);
                else
                    AffCol(drive[i],9,7*16+4);
                }
            }

        if (HI(car)==GAUCHE) i--;
        if (HI(car)==DROITE) i++;
        if (HI(car)==0xF) i--;
        if (LO(car)==9) i++;

        if (i==26) i=0;
        if (i<0) i=25;
        }
    while (drive[i]==0);

    xx=2;
    yy=13;

    for(n=0;n<26;n++)
        if (lstdrv[n]!=0)
            {
            PrintAt(xx,yy,"Search in %c: (%9s)",
                    n+'A',etat[n]==1 ? "Ready" : "Not Ready");
            yy++;
            if (yy>=Cfg->TailleY-5) yy=13,xx+=39;
            }

    while(xx!=80)
        {
        PrintAt(xx,yy,"                        ");
        yy++;
        if (yy>=Cfg->TailleY-5) yy=13,xx+=39;
        }

    if (lstdrv[i]==0)
        AffCol(drive[i],9,14*16+3);
        else
        AffCol(drive[i],9,7*16+3);

    car=Wait(0,0,0);

    if (lstdrv[i]==0)
        AffCol(drive[i],9,14*16+7);
        else
        AffCol(drive[i],9,7*16+7);

} while ( (LO(car)!=27) & (LO(car)!=13));

LoadScreen();

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



k=Cfg->TailleY-6;

DispMessage("Select the directory where you will that KKSETUP"
                                           " copy the batch files for");
DispMessage("the execution of KK and KKDESC. (ESC for cancel)");
DispMessage("");

erreur=1;

TPath=getenv("PATH");
strcpy(path,TPath);
k=strlen(path);

for(i=0;i<k;i++)
    if (path[i]==';')
        path[i]=0;

dir=GetMem(MAXDIR*sizeof(char *));

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
    int x=2,y,pos,car,max,prem;

    pos=0;

    y=MesgY;

    SaveScreen();
    PutCur(32,0);

    max=i;
    if (max>Cfg->TailleY-(y+1)-1) max=Cfg->TailleY-(y+1)-1;

    Cadre(x-2,y-1,x+Mlen+1,y+max,0,Cfg->col[55],Cfg->col[56]);
    Window(x-1,y,x+Mlen,y+max-1,Cfg->col[16]);

    prem=0;

    do {
    while((pos-prem)>=max) prem++;
    while((pos-prem)<0) prem--;

    for (j=0;j<max;j++)
        PrintAt(x,y+j,"%-*s",Mlen,dir[j+prem]);

    ColLin(x-1,y+(pos-prem),Mlen+2,7*16+5);

    car=Wait(0,0,0);

    ColLin(x-1,y+(pos-prem),Mlen+2,0*16+1);

    switch(HI(car))
        {
        case 72:                                                   // UP
            pos--;
            if (pos==-1) pos=0;  // i-1;
            break;
        case 80:                                                 // DOWN
            pos++;
            if (pos==i) pos=i-1; // 0;
            break;
        case 0x47:                                               // HOME
            pos=0;
            break;
        case 0x4F:                                                // END
            pos=i-1;
            break;
        case 0x49:                                               // PGUP
            pos-=5;
            if (pos<0) pos=0;
            break;
        case 0x51:                                               // PGDN
            pos+=5;
            if (pos>=i) pos=i-1;
            break;
        }
    }
    while ( (car!=13) & (car!=27) & (HI(car)!=0x8D) );

    LoadScreen();



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

            DispMessage("%s is done",path);
            DispMessage("");

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

                DispMessage("%s is done",path);
                DispMessage("");
                }
            }
        
        }
    }
free(dir);

if (erreur==1)
    strcpy(PathOfKK,"");

}


/*--------------------------------------------------------------------*\
|- Procedure principale                                               -|
\*--------------------------------------------------------------------*/
void main(short argc,char **argv)
{

char buffer[256],chaine[256];
short n;
int i;
FILE *fic;

char *path;

IOerr=1;

/*--------------------------------------------------------------------*\
|-  Initialisation de l'ecran                                         -|
\*--------------------------------------------------------------------*/

todo=0;

if (!stricmp((argv[1])+1,"ABOUT")) todo=2;
if (!stricmp((argv[1])+1,"COLOR")) todo=12;
if (!stricmp((argv[1])+1,"PALETTE")) todo=13;

Cfg=GetMem(sizeof(struct config));

Screen_Buffer=(char*)GetMem(8000);                      // maximum 80x50

OldX=(*(char*)(0x44A));
OldY=(*(char*)(0x484))+1;

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;                  // Initialisation de la taille ecran

InitScreen(0);                     // Initialise toutes les donn‚es HARD

WhereXY(&PosX,&PosY);

for (n=0;n<8000;n++)
    Screen_Buffer[n]=Screen_Adr[n];

/*--------------------------------------------------------------------*\
|-  Gestion des erreurs                                               -|
\*--------------------------------------------------------------------*/

KKCfg=GetMem(sizeof(struct kkconfig));

_harderr(Error_handler);

path=GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
    }


/*--------------------------------------------------------------------*\
|-  Initialisation des variables                                      -|
\*--------------------------------------------------------------------*/

Fenetre[0]=GetMem(sizeof(FENETRE));
Fenetre[0]->F=GetMem(TOTFIC*sizeof(void *));

Fenetre[1]=GetMem(sizeof(FENETRE));
Fenetre[1]->F=GetMem(TOTFIC*sizeof(void *));

Fenetre[2]=GetMem(sizeof(FENETRE));
Fenetre[2]->F=GetMem(TOTFIC*sizeof(void *));

Fenetre[3]=GetMem(sizeof(FENETRE));
Fenetre[3]->F=GetMem(TOTFIC*sizeof(void *));

Fics=GetMem(sizeof(struct fichier));
KKFics=(struct kkfichier*)GetMem(sizeof(struct kkfichier));

Mask=GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    Mask[n]=GetMem(sizeof(struct PourMask));


/*--------------------------------------------------------------------*\
|-                      Initialisation des fichiers                   -|
\*--------------------------------------------------------------------*/

SetDefaultPath(path);
SetDefaultKKPath(path);

Fics->help=GetMem(256);
strcpy(Fics->help,path);
Path2Abs(Fics->help,"kksetup.hlp");

/*--------------------------------------------------------------------*\
|- Chargement de la configuration                                     -|
\*--------------------------------------------------------------------*/

LoadDefCfg=0;

if (LoadCfg()==-1)
    {
    FENETRE *DFen;
    int t;

    DefaultCfg();
    DefaultKKCfg();

    LoadDefCfg=1;
    
    for (t=0;t<4;t++)
        {
        DFen=Fenetre[t];

        DFen->FenTyp=KKCfg->FenTyp[t];

        strcpy(DFen->path,Fics->path);
        DFen->order=1;
        DFen->sorting=1;

        DFen->nbrsel=0;

        DFen->F[0]=GetMem(sizeof(struct file));

        DFen->F[0]->name=GetMem(256);
        strcpy(DFen->F[0]->name,"kk.exe");


        DFen->scur=0;
        }
    LoadConfigFile("main");
    }

Cfg->reinit=0;
InitMode();

if (todo!=0)
    {
    int x;

    x=(Cfg->TailleX-14)/2;

    Window(x,9,x+13,9,Cfg->col[28]);
    Cadre(x-1,8,x+14,10,0,Cfg->col[46],Cfg->col[47]);

    PrintAt(x+1,9,"Hi everybody");
    }

/*--------------------------------------------------------------------*\
|- Autres configurations                                              -|
\*--------------------------------------------------------------------*/

if (LoadDefCfg)
    ConfigSetup();

/*--------------------------------------------------------------------*\
|-  Gestion Message                                                   -|
\*--------------------------------------------------------------------*/

if (todo==0)
    {
    strcpy(chaine,KKFics->trash);
    if (mkdir(chaine)==0)
        {
        DispMessage("Creation of the trash directory '%s': OK",chaine);
        DispMessage("");
        }
        else
        {
        char path[256];
        getcwd(path,256);

        if (chdir(chaine)==0)
            {
            DispMessage("Trash directory exist: %s",chaine);
            DispMessage("");
            }
            else
            {
            DispMessage("You must uncompress archive on your disk");
            DispMessage("");
            }
        chdir(path);
        }


/*--------------------------------------------------------------------*\
|-  Insertion de KK dans la path si pas d‚ja pr‚sent !                -|
\*--------------------------------------------------------------------*/

    strcpy(ActualPath,path);

    _searchenv("KK.BAT","PATH",buffer);
    if (strlen(buffer)!=0)
        {
        FILE *fic;
        static char toto[256];
        static char tata[256];

        fic=fopen(buffer,"rt");
        fgets(toto,256,fic);

        sprintf(tata,"@%s",ActualPath);
        Path2Abs(tata,"kk.exe\n");

        if (stricmp(tata,toto)!=0)
            PutInPath();

        Path2Abs(buffer,"..");
        strcpy(PathOfKK,buffer);
        }
        else
        PutInPath();


    fic=fopen(KKFics->FicIdfFile,"rb");
    if (fic==NULL)
        {
        DispMessage("It's the first time that you run KKSETUP");
        DispMessage("  -> Go to menu 'Player'");
        DispMessage("  -> Select 'Search Player'");
        DispMessage("");
        }
        else
        fclose(fic);
    }


if (todo!=0)
    {
    GestionFct(todo);
    }
    else
    {
    do
        {
        i=GestionBar();
        if ((i==7) | (i==0)) break;
        GestionFct(i);
        }
    while(1);

    if (strlen(PathOfKK)!=0)
        {
        DispMessage("KK.BAT & KKDESC.BAT are now in PATH (%s)",
                                                              PathOfKK);
        DispMessage("  -> You could run KK from everywhere");
        }
        else
        {
        DispMessage("WARNING: You couldn't run KK from everywhere "
                                                    "(Reload KKSETUP)");
        }

    DispMessage("");
    DispMessage("Press a key to continue");
    Wait(0,0,0);
    }

SaveCfg();

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

TXTMode();

GotoXY(0,PosY);

for (n=0;n<8000;n++)
    Screen_Adr[n]=Screen_Buffer[n];

if (todo==0)
    cputs(RBTitle2);
}


void ApplSearch(void)
{
char lstdrv[26];
short n,t;
char ch[256];
FILE *fic;

/*--------------------------------------------------------------------*\
|- Initialise les variables globales                                  -|
\*--------------------------------------------------------------------*/

GVerif=0;
St_App=0;
St_Dir=0;

SaveScreen();
PutCur(32,0);

ChrWin(1,3,Cfg->TailleX-2,Cfg->TailleY-3,32);
ColWin(1,3,Cfg->TailleX-2,Cfg->TailleY-3,10*16+1);

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
        if (DriveReady(n)==1)
            KKR_Search(ch);
    }

nbrdir=0;

for (n=0;n<26;n++)
    {
    sprintf(ch,"%c:\\*.*",n+'A');
    if (lstdrv[n]==1)
        if (DriveReady(n)==1)
            SSearch(ch);
    }

if (nbr>0)
    {
    fic=fopen(KKFics->FicIdfFile,"wb");
    if (fic!=NULL)
        {
        fwrite("RedBLEXU",1,8,fic);

        fputc(_getdrive()-1,fic);

        t=0;

        for(n=0;n<nbr;n++)
            if (app[n]->pres!=0) t++;

        fwrite(&t,1,2,fic);

        for(n=0;n<nbr;n++)
            if (app[n]->pres!=0)
                {
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

                fwrite(&(app[n]->ext),2,1,fic);      // Numero de format
                fwrite(&(app[n]->pres),2,1,fic);     // Numero directory

                fwrite(&(app[n]->type),1,1,fic);          // Numero type

                fwrite(&(app[n]->os),1,1,fic);// Numero operating system

                if (app[n]->Checksum==0)
                    app[n]->info=0;
                    else
                    app[n]->info=1;

                fwrite(&(app[n]->info),1,1,fic);  // Information fichier
                }

        fwrite(&nbrdir,1,2,fic);

        for(n=0;n<nbrdir;n++)
            fwrite(dir[n],1,128,fic);

        fclose(fic);
        }

    PrintAt((Cfg->TailleX-22)/2,(Cfg->TailleY-3),"Press a key to continue");
    ColLin(1,(Cfg->TailleY-3),(Cfg->TailleX-2),10*16+3);

    Wait(0,0,0);

    LoadScreen();

    DispMessage("Statistics");
    DispMessage("I have found %3d applications",St_App);
    DispMessage("          in %3d directories",St_Dir);
    DispMessage("");
    }
    else
    {
    LoadScreen();
    }
}

int posy=3;

char KKR_Read(FILE *Fic)
{
char Key[4];
char KKType,KKos;
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
    switch(Code)
        {
        case 0:                         // Commentaire (sans importance)
            fread(&SComment,1,1,Fic);
            fread(Comment,SComment,1,Fic);
            break;
        case 1:                                            // Code Titre
            fread(&STitre,1,1,Fic);
            Titre[STitre]=0;
            fread(Titre,STitre,1,Fic);

            PrintAt(3,posy,"Loading information about %s",Titre);
            posy++;

            if (posy>(Cfg->TailleY-3))
                {
                MoveText(1,4,78,(Cfg->TailleY-3),1,3);
                posy--;
                ChrLin(1,(Cfg->TailleY-3),78,32);
                }
            break;
        case 2:                                      // Code Programmeur
            fread(&SMeneur,1,1,Fic);
            fread(Meneur,SMeneur,1,Fic);
            Meneur[SMeneur]=0;
            break;
        case 3:                                 // Code Nom du programme
            fread(&SFilename,1,1,Fic);
            Filename[SFilename]=0;
            fread(Filename,SFilename,1,Fic);
            break;
        case 4:                                              // Checksum
            fread(&Checksum,4,1,Fic);
            break;
        case 5:                                                // Format
            fread(&format,2,1,Fic);
            app[nbr]=GetMem(sizeof(struct player));

            app[nbr]->Filename=GetMem(SFilename+1);
            strcpy(app[nbr]->Filename,Filename);

            app[nbr]->Meneur=GetMem(SMeneur+1);
            strcpy(app[nbr]->Meneur,Meneur);

            app[nbr]->Titre=GetMem(STitre+1);
            strcpy(app[nbr]->Titre,Titre);

            app[nbr]->Checksum=Checksum;
            app[nbr]->ext=format;
            app[nbr]->pres=0;

            app[nbr]->type=KKType;

            app[nbr]->os=KKos;

            nbr++;
            break;
        case 6:                                        // Fin de fichier
            fin=1;
            break;
        case 7:                                                 // Reset
            Checksum=0;

            strcpy(Titre,"?");
            strcpy(Meneur,"?");
            strcpy(Filename,".");

            SFilename=strlen(Filename);
            STitre=strlen(Titre);
            SMeneur=strlen(Meneur);

            KKType=0;
            KKos=0;
            break;
        case 8:                                     // type du programme
            fread(&KKType,1,1,Fic);
            break;
        case 9:                                      // Operating system
            fread(&KKos,1,1,Fic);
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

char **TabRec;              // Tableau qui remplace les appels recursifs
int NbrRec;                          // Nombre d'element dans le tableau

TabRec=GetMem(500*sizeof(char*));
TabRec[0]=GetMem(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
strcpy(nom,TabRec[NbrRec-1]);

PrintAt(1,2,"%-78s",nom);

strcpy(moi,nom);
Path2Abs(moi,"*.KKR");

if (_dos_findfirst(moi,63-_A_SUBDIR,&fic)==0)
do
    {
    ok=0;
    if ((fic.attrib&_A_SUBDIR)!=_A_SUBDIR)
        {
        strcpy(moi,nom);
        Path2Abs(moi,fic.name);
        Fic=fopen(moi,"rb");
        if (Fic==NULL)
            {
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
Path2Abs(moi,"*.*");

if (_dos_findfirst(moi,_A_SUBDIR,&fic)==0)
do
    {
    if  ( (fic.name[0]!='.') & (((fic.attrib)&_A_SUBDIR) == _A_SUBDIR) )
            {
            strcpy(moi,nom);
            Path2Abs(moi,fic.name);

            TabRec[NbrRec]=GetMem(strlen(moi)+1);
            memcpy(TabRec[NbrRec],moi,strlen(moi)+1);
            NbrRec++;
            }
    }
while (_dos_findnext(&fic)==0);
}
while(NbrRec>0);


free(TabRec);
}






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
unsigned long K1crc;                 // crc calcul‚ une fois pour toutes

unsigned long C;

char **TabRec;              // Tableau qui remplace les appels recursifs
int NbrRec;                          // Nombre d'element dans le tableau

char *StrVerif,Verif;



TabRec=GetMem(500*sizeof(char*));
TabRec[0]=GetMem(strlen(nom2)+1);
memcpy(TabRec[0],nom2,strlen(nom2)+1);
NbrRec=1;

do
{
o=nbrdir+1;

PrintAt(1,2,"%-78s",nom);
St_Dir++;

strcpy(nom,TabRec[NbrRec-1]);

PrintAt(1,2,"%-78s",nom);

if (_dos_findfirst(nom,63-_A_SUBDIR,&fic)==0)
do
    {
    ok=0;
    wok=-1;

    if ((fic.attrib&_A_SUBDIR)!=_A_SUBDIR)
        {
        C=0;
        KKcrc=0;                               // CRC du fichier courant
        K1crc=0;

        for(n=0;n<nbr;n++)
            if ( (!FileComp(fic.name,app[n]->Filename)) &
                                                 (app[n]->Checksum!=0) )
                {
                if (KKcrc==0)
                    {
                    if (K1crc==0)
                        {
                        int n;

                        strcpy(moi,nom);
                        moi[strlen(moi)-3]=0;
                        Path2Abs(moi,fic.name);
                        n=0;
                        while(moi[n]!=0)
                            if (moi[n]==32) moi[n]=0; else n++;

                        crc32file(moi,&KKcrc);
                        K1crc=KKcrc;
                        }
                        else
                        KKcrc=K1crc;

                    if (KKcrc!=app[n]->Checksum) KKcrc=0;
                    }
                }

        Verif=0;
        StrVerif=app[0]->Titre;//pour pas mettre NULL, ca veut rien dire

        for(n=0;n<nbr;n++)
            {
            bill=0;
            if (!FileComp(fic.name,app[n]->Filename))
                {
                if ( (KKcrc==0) & (app[n]->Checksum==0) )
                    {
                    if ((Verif==0)| (strcmp(StrVerif,app[n]->Titre)!=0))
                        {
                        strcpy(moi,nom);
                        moi[strlen(moi)-3]=0;
                        Path2Abs(moi,fic.name);

                        if (GVerif==0)
                            Interroge(moi,app[n],&Verif,&GVerif);
                            else
                            Verif=GVerif;

                        StrVerif=app[n]->Titre;
                        }

                    if (Verif==1)
                        {
                        app[n]->pres=o;
                                // l'appl. n est presente dans le dir. o
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

                //--- Editeur par default ------------------------------
                    if (app[n]->ext==91)
                        if (KKCfg->editeur[0]==0)
                            if (strlen(app[n]->Filename)<63)
                                {
                                strcpy(KKCfg->editeur,moi);
                                Path2Abs(KKCfg->editeur,app[n]->Filename);
                                }
                    }
                }
            }
        }

    if (ok==1)
        {
        St_App++;

        nbrdir=o;
        strcpy(dir[o-1],moi);

        if (app[wok]->Checksum==0)
            PrintAt(3,posy,"? Found %s in %s",app[wok]->Titre,dir[o-1]);
            else
            PrintAt(3,posy,"Found %s in %s",app[wok]->Titre,dir[o-1]);
        posy++;

        if (posy>(Cfg->TailleY-3))
                {
                MoveText(1,4,78,(Cfg->TailleY-3),1,3);
                posy--;
                ChrLin(1,(Cfg->TailleY-3),78,32);
                }
        }
   }
while (_dos_findnext(&fic)==0);

free(TabRec[NbrRec-1]);
NbrRec--;

if (_dos_findfirst(nom,_A_SUBDIR,&fic)==0)
do
	{
    if ( (fic.name[0]!='.') & (((fic.attrib) & _A_SUBDIR)==_A_SUBDIR) )
			{
			strcpy(moi,nom);
			moi[strlen(moi)-3]=0;
            Path2Abs(moi,fic.name);
            Path2Abs(moi,"*.*");

            TabRec[NbrRec]=GetMem(strlen(moi)+1);
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

i=0;                                             // navigation dans name
j=0;                                                // position dans buf

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



void LoadConfigFile(char *part)
{
char buf[200],buf1[81],buf2[81];
char buffer[32];
char section[32];
char filename[128];
int n,m;

strcpy(section,part);
strcpy(filename,Fics->path);
Path2Abs(filename,"kksetup.ini");

KKCfg->wmask=get_private_profile_int(section,"mask",
                                                 KKCfg->wmask,filename);
Cfg->TailleY=get_private_profile_int(section,"vsize",
                                                 Cfg->TailleY,filename);
Cfg->TailleX=get_private_profile_int(section,"hsize",
                                                 Cfg->TailleX,filename);
KKCfg->fentype=get_private_profile_int(section,"wintype",
                                               KKCfg->fentype,filename);
KKCfg->AnsiSpeed=get_private_profile_int(section,"ansispeed",
                                             KKCfg->AnsiSpeed,filename);
Cfg->SaveSpeed=get_private_profile_int(section,"ssaverspeed",
                                               Cfg->SaveSpeed,filename);
KKCfg->pntrep=get_private_profile_int(section,"directpoint",
                                                KKCfg->pntrep,filename);
KKCfg->hidfil=get_private_profile_int(section,"hiddenfile",
                                                KKCfg->hidfil,filename);
KKCfg->logfile=get_private_profile_int(section,"logfile",
                                               KKCfg->logfile,filename);
Cfg->font=get_private_profile_int(section,"font",Cfg->font,filename);
Cfg->debug=get_private_profile_int(section,"debug",Cfg->debug,filename);
KKCfg->confexit=get_private_profile_int(section,"confexit",
                                              KKCfg->confexit,filename);
KKCfg->mtrash=get_private_profile_int(section,"sizetrash",
                                                KKCfg->mtrash,filename);
Cfg->display=get_private_profile_int(section,"display",
                                                 Cfg->display,filename);
Cfg->comport=get_private_profile_int(section,"serial_port",
                                                 Cfg->comport,filename);
Cfg->comspeed=get_private_profile_int(section,"serial_speed",
                                                Cfg->comspeed,filename);
Cfg->combit=get_private_profile_int(section,"serial_databit",Cfg->combit
                                                             ,filename);
get_private_profile_string(section,"serial_parity","N",buffer,16,
                                                              filename);
KKCfg->autoreload=get_private_profile_int(section,"autoreload",
                                            KKCfg->autoreload,filename);
KKCfg->dispath=get_private_profile_int(section,"dispupperpath",
                                               KKCfg->dispath,filename);
KKCfg->verifhist=get_private_profile_int(section,"verifhist",
                                             KKCfg->verifhist,filename);
KKCfg->palafter=get_private_profile_int(section,"palafter",
                                              KKCfg->palafter,filename);
KKCfg->dispcolor=get_private_profile_int(section,"dispcolor",
                                             KKCfg->dispcolor,filename);
KKCfg->insdown=get_private_profile_int(section,"insdown",
                                               KKCfg->insdown,filename);
KKCfg->seldir=get_private_profile_int(section,"seldir",
                                                KKCfg->seldir,filename);
KKCfg->esttime=get_private_profile_int(section,"esttime",
                                               KKCfg->esttime,filename);
KKCfg->ajustview=get_private_profile_int(section,"ajustview",
                                             KKCfg->ajustview,filename);
KKCfg->currentdir=get_private_profile_int(section,"loadstartdir",
                                            KKCfg->currentdir,filename);
KKCfg->saveviewpos=get_private_profile_int(section,"saveviewpos",
                                           KKCfg->saveviewpos,filename);

KKCfg->warp=get_private_profile_int(section,"warp",
                                                  KKCfg->warp,filename);
KKCfg->autotrad=get_private_profile_int(section,"autotrad",
                                              KKCfg->autotrad,filename);
KKCfg->cnvtable=get_private_profile_int(section,"cnvtable",
                                              KKCfg->cnvtable,filename);


Pal2Str(Cfg->palette,buf);
get_private_profile_string(section,"palette",buf,buf,48,filename);
Str2Pal(buf,Cfg->palette);

Col2Str(Cfg->col,buf);
memcpy(buf1,buf,64);
buf1[64]=0;
memcpy(buf2,buf+64,64);
buf2[64]=0;
get_private_profile_string(section,"col1",buf1,buf,64,filename);
get_private_profile_string(section,"col2",buf2,buf+64,64,filename);
Str2Col(buf,Cfg->col);


switch(toupper(buffer[0]))
    {
    default:
    case 'N': Cfg->comparity='N'; break;
    case 'O': Cfg->comparity='O'; break;
    case 'E': Cfg->comparity='E'; break;
    case 'M': Cfg->comparity='M'; break;
    case 'S': Cfg->comparity='S'; break;
    }

Cfg->comstop=get_private_profile_int(section,"serial_stopbit",
                                                 Cfg->comstop,filename);

get_private_profile_string(section,"editor",KKCfg->editeur,
                                            KKCfg->editeur,63,filename);
get_private_profile_string(section,"viewer",KKCfg->vieweur,
                                            KKCfg->vieweur,63,filename);

for (n=11;n<15;n++)
    {
    strcpy(Mask[n]->chaine,"");

    for (m=0;m<10;m++)
        {
        sprintf(buffer,"usermask%d-%d",n-10,m);
        get_private_profile_string(section,buffer," ",buf,80,filename);
        JoinMask(Mask[n]->chaine,buf);
        }

    strcat(Mask[n]->chaine," @");

    if (strlen(Mask[n]->chaine)>2)
        sprintf(Mask[n]->title,"User config %d",n-10);
        else
        sprintf(Mask[n]->title,"");

    sprintf(buffer,"ignorecase%d",n-10);
    Mask[n]->Ignore_Case=get_private_profile_int(section,buffer,0,
                                                              filename);
    }


DispMessage("Loading of KKSETUP.INI: OK");
DispMessage("");
}

void SplitMask(char *chaine,char *buf1,char *buf2,char *buf3)
{
int n,m;
char *str;

str=chaine;

m=strlen(str)-2;
if (m==0)
    {
    strcpy(chaine," @");
    strcpy(buf1,"");
    strcpy(buf2,"");
    strcpy(buf3,"");
    return;
    }

if (m<=54)
    {
    memcpy(buf1,str,m);
    buf1[m]=0;
    strcpy(buf2,"");
    strcpy(buf3,"");
    return;
    }
n=54;
while ( (n!=0) & (str[n]!=32) )
    n--;
memcpy(buf1,str,n);
buf1[n]=0;

str+=n+1;
m=strlen(str)-2;

if (m<=54)
    {
    memcpy(buf2,str,m);
    buf2[m]=0;
    strcpy(buf3,"");
    return;
    }
n=54;
while ( (n!=0) & (str[n]!=32) )
    n--;

memcpy(buf2,str,n);
buf2[n]=0;

str+=n+1;
m=strlen(str)-2;

if (m<=54)
    {
    memcpy(buf3,str,m);
    buf3[m]=0;
    return;
    }
n=54;
while ( (n!=0) & (str[n]!=32) )
    n--;

memcpy(buf3,str,n);
buf3[n]=0;
}

void JoinMask(char *chaine,char *buf1)
{
int n,m;

strcat(chaine," ");
strcat(chaine,buf1);

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

void SaveConfigFile(void)
{
int n;
char buf1[80],buf2[80],buf3[80];
char buf[129];
char buffer[32];
char section[32];
char filename[128];

strcpy(section,"current");
strcpy(filename,Fics->path);
Path2Abs(filename,"kksetup.ini");

write_private_profile_int(section,"mask",KKCfg->wmask,filename);
write_private_profile_int(section,"hsize",Cfg->TailleX,filename);
write_private_profile_int(section,"vsize",Cfg->TailleY,filename);
write_private_profile_int(section,"wintype",KKCfg->fentype,filename);
write_private_profile_int(section,"ansispeed",KKCfg->AnsiSpeed,
                                                              filename);
write_private_profile_int(section,"ssaverspeed",Cfg->SaveSpeed,
                                                              filename);
write_private_profile_int(section,"directpoint",KKCfg->pntrep,filename);
write_private_profile_int(section,"hiddenfile",KKCfg->hidfil,filename);
write_private_profile_int(section,"logfile",KKCfg->logfile,filename);
write_private_profile_int(section,"font",Cfg->font,filename);
write_private_profile_int(section,"debug",Cfg->debug,filename);
write_private_profile_int(section,"confexit",KKCfg->confexit,filename);
write_private_profile_int(section,"sizetrash",KKCfg->mtrash,filename);
write_private_profile_int(section,"display",Cfg->display,filename);

write_private_profile_int(section,"serial_port",Cfg->comport,filename);
write_private_profile_int(section,"serial_speed",Cfg->comspeed,
                                                              filename);
write_private_profile_int(section,"serial_databit",Cfg->combit,
                                                              filename);
write_private_profile_int(section,"dispupperpath",
                                               KKCfg->dispath,filename);
sprintf(buffer,"%c",Cfg->comparity);
write_private_profile_string(section,"serial_parity",buffer,filename);
write_private_profile_int(section,"serial_stopbit",Cfg->comstop,
                                                              filename);

write_private_profile_int(section,"autoreload",KKCfg->autoreload,
                                                              filename);
write_private_profile_int(section,"verifhist",KKCfg->verifhist,
                                                              filename);
write_private_profile_int(section,"palafter",KKCfg->palafter,filename);
write_private_profile_int(section,"dispcolor",KKCfg->dispcolor,
                                                              filename);
write_private_profile_int(section,"insdown",KKCfg->insdown,filename);
write_private_profile_int(section,"seldir",KKCfg->seldir,filename);
write_private_profile_int(section,"esttime",KKCfg->esttime,filename);
write_private_profile_int(section,"ajustview",KKCfg->ajustview,
                                                              filename);
write_private_profile_int(section,"loadstartdir",KKCfg->currentdir,
                                                              filename);
write_private_profile_int(section,"saveviewpos",
                                             KKCfg->saveviewpos,filename);
write_private_profile_int(section,"warp",KKCfg->warp,filename);
write_private_profile_int(section,"autotrad",KKCfg->autotrad,filename);
write_private_profile_int(section,"cnvtable",KKCfg->cnvtable,filename);

write_private_profile_string(section,"editor",KKCfg->editeur,filename);
write_private_profile_string(section,"viewer",KKCfg->vieweur,filename);


Pal2Str(Cfg->palette,buf1);
write_private_profile_string(section,"palette",buf1,filename);

Col2Str(Cfg->col,buf);
memcpy(buf1,buf,64);
buf1[64]=0;
memcpy(buf2,buf+64,64);
buf2[64]=0;

write_private_profile_string(section,"col1",buf1,filename);
write_private_profile_string(section,"col2",buf2,filename);


for (n=11;n<15;n++)
    {
    SplitMask(Mask[n]->chaine,buf1,buf2,buf3);

    sprintf(buffer,"usermask%d-0",n-10);
    write_private_profile_string(section,buffer,buf1,filename);

    sprintf(buffer,"usermask%d-1",n-10);
    write_private_profile_string(section,buffer,buf2,filename);

    sprintf(buffer,"usermask%d-2",n-10);
    write_private_profile_string(section,buffer,buf3,filename);

    sprintf(buffer,"ignorecase%d",n-10);
    write_private_profile_int(section,buffer,Mask[n]->Ignore_Case,
                                                              filename);
    }

DispMessage("Saving [current] section in KKSETUP.INI: OK");
DispMessage("");
}

struct {
    char *Filename;
    char *Titre;
    char *Meneur;
    short ext;
    short NoDir;
    char type;  // 0: Rien de particulier
                // 1: Decompacteur
                // 2: Compacteur
    char os;
    char info;
    char dir[128];
    } IDF2_app;

/*--------------------------------------------------------------------*\
|-  Montre tous les players que vous possedez                         -|
\*--------------------------------------------------------------------*/
char ShowYourPlayer(void)
{
char key[9];
FILE *fic;
short prem,paff,j,k,nbr;
int car,y;
char di;    //--- Ne sert … rien ---------------------------------------
char info;

int ndir;
char name[256],dir[129];

fic=fopen(KKFics->FicIdfFile,"rb");


if (fic==NULL)
    {
	PUTSERR("IDFEXT.RB missing");
    return 0;
	}

fread(key,1,8,fic);
if (memcmp(key,"RedBLEXU",8))
    {
	PUTSERR("File IDFEXT.RB is bad");
    return 0;
	}
di=fgetc(fic);

if (fread(&nbr,1,2,fic)==0) return 0;

if (nbr==0) return 0;


SaveScreen();
PutCur(32,0);

ChrWin(1,3,78,(Cfg->TailleY-3),32);
ColWin(1,3,78,(Cfg->TailleY-3),10*16+1);


prem=0;
paff=0;

do
    {
    fseek(fic,11,SEEK_SET);
    y=3;

    for (j=0;j<paff;j++)
        {
        char n;

        fread(&n,1,1,fic);
        fseek(fic,n,SEEK_CUR);

        fread(&n,1,1,fic);
        fseek(fic,n,SEEK_CUR);

        fread(&n,1,1,fic);
        fseek(fic,n,SEEK_CUR);

        fseek(fic,2,SEEK_CUR);                       // Numero de format
        fseek(fic,2,SEEK_CUR);                       // Numero directory
        fseek(fic,1,SEEK_CUR);                            // Numero type
        fseek(fic,1,SEEK_CUR);                              // Numero OS
        fread(&info,1,1,fic);                                    // info
        }

    for (j=paff;j<nbr;j++)
        {
        char n;
        char *a;

        fread(&n,1,1,fic);
        IDF2_app.Filename=GetMem(n+1);
        a=IDF2_app.Filename;
        a[n]=0;
        fread(a,n,1,fic);

        fread(&n,1,1,fic);
        IDF2_app.Titre=GetMem(n+1);
        a=IDF2_app.Titre;
        a[n]=0;
        fread(a,n,1,fic);

        fread(&n,1,1,fic);
        IDF2_app.Meneur=GetMem(n+1);
        a=IDF2_app.Meneur;
        a[n]=0;
        fread(a,n,1,fic);

        fread(&(IDF2_app.ext),2,1,fic);              // Numero de format

        fread(&(IDF2_app.NoDir),2,1,fic);            // Numero directory

        fread(&(IDF2_app.type),1,1,fic);                  // Numero type

        fread(&(IDF2_app.os),1,1,fic);        // Numero operating system

        fread(&(IDF2_app.info),1,1,fic);    // information sur le player

        for(k=0;k<nbrkey;k++)
            if (K[k].numero == IDF2_app.ext) break;

        if (prem==j)
            {
            ColLin(1,y,1,  12*16+3);
            ColLin(2,y,3,  12*16+4);
            ColLin(5,y,1,  12*16+3);
            ColLin(6,y,35, 12*16+4);
            ColLin(41,y,6, 12*16+3);
            ColLin(47,y,30,12*16+5);
            ColLin(77,y,1, 12*16+3);
            if (Cfg->TailleX!=80)
                ColLin(78,y,Cfg->TailleX-79,12*16+3),
                ChrLin(78,y,Cfg->TailleX-79,32);

            ndir=IDF2_app.NoDir;
            strcpy(name,IDF2_app.Filename);
            info=IDF2_app.info;
            }
        else
        if (y&1==1)
            {
            ColLin(1,y,1,  10*16+3);
            ColLin(2,y,3,  10*16+4);
            ColLin(5,y,1,  10*16+3);
            ColLin(6,y,35, 10*16+4);
            ColLin(41,y,6, 10*16+3);
            ColLin(47,y,30,10*16+5);
            ColLin(77,y,1, 10*16+3);
            if (Cfg->TailleX!=80)
                ColLin(78,y,Cfg->TailleX-79,10*16+3),
                ChrLin(78,y,Cfg->TailleX-79,32);
            }
        else
            {
            ColLin(1,y,1,  1*16+3);
            ColLin(2,y,3,  1*16+4);
            ColLin(5,y,1,  1*16+3);
            ColLin(6,y,35, 1*16+4);
            ColLin(41,y,6, 1*16+3);
            ColLin(47,y,30,1*16+5);
            ColLin(77,y,1, 1*16+3);
            if (Cfg->TailleX!=80)
                ColLin(78,y,Cfg->TailleX-79,1*16+3),
                ChrLin(78,y,Cfg->TailleX-79,32);
            }

        PrintAt(1,y," %3s %-35s from %30s ",K[k].ext,IDF2_app.Titre,
                                                       IDF2_app.Meneur);
        y++;

        if ( (y==(Cfg->TailleY-3)) | (j==nbr-1) ) break;
        }

    while(j<nbr-1)
        {
        char n;

        fread(&n,1,1,fic);
        fseek(fic,n,SEEK_CUR);

        fread(&n,1,1,fic);
        fseek(fic,n,SEEK_CUR);

        fread(&n,1,1,fic);
        fseek(fic,n,SEEK_CUR);

        fseek(fic,2,SEEK_CUR);                       // Numero de format
        fseek(fic,2,SEEK_CUR);                       // Numero directory

        fseek(fic,1,SEEK_CUR);
        fseek(fic,1,SEEK_CUR);
        fseek(fic,1,SEEK_CUR);

        j++;
        }

    car=Wait(0,0,0);
    switch(HI(car))
        {
        case 72:                                                   // UP
            prem--;            break;
        case 80:                                                 // DOWN
            prem++;            break;
        case 0x49:                                               // PGUP
            prem-=10;          break;
        case 0x51:                                               // PGDN
            prem+=10;          break;
        case 0x47:                                               // HOME
            prem=0;            break;
        case 0X4F:                                                // END
            prem=nbr-1;        break;
        }

    if (car==13)
        {
        fseek(fic,2,SEEK_CUR);
        fseek(fic,(ndir-1)*128,SEEK_CUR);
        fread(dir,1,128,fic);
        dir[128]=0;

        if (info==1)
            strcat(dir,"\nPlayer without doubt");
            else
            strcat(dir,"\nPlayer with a doubt");

        WinMesg(name,dir,0);
        }

    if (prem<0) prem=0;
    if (prem>=nbr) prem=nbr-1;

    while (paff<prem-(Cfg->TailleY-7)) paff++;
    while (paff>prem) paff--;
    }
while(car!=27);


LoadScreen();

return 1;
}



/*--------------------------------------------------------------------*\
|-  Gestion de la barre de menu du haut                               -|
\*--------------------------------------------------------------------*/

int GestionBar(void)
{
MENU menu;
int retour,nbmenu;
int u,v,s,x;

struct barmenu bar[20];

short fin;

static int cpos[20],poscur;

SaveScreen();
PutCur(32,0);

retour=0;

do
{
bar[0].Titre="Player";
bar[1].Titre="Information";
bar[2].Titre="Disk";
bar[3].Titre="Help";

bar[0].Help=NULL;
bar[1].Help=NULL;
bar[2].Help=NULL;
bar[3].Help=NULL;

if (retour==0)                             // Navigation sur bar de menu
    v=1;
    else
    v=0;

u=BarMenu(bar,4,&poscur,&x,&v);
                               // Renvoit t: position du machin surligne
            // Renvoit v: 0 si rien, autre si position dans sous fenetre
if (u==0)
    {
    fin=0;
    break;
    }

switch (poscur)
 {
 case 0:
    bar[0].Titre="Search player";   bar[0].fct=4; bar[0].Help=NULL;
    nbmenu=1;
    break;
 case 1:
    bar[0].Titre="Show all format "; bar[0].fct=3; bar[0].Help=NULL;
    bar[1].Titre="Show your player"; bar[1].fct=8; bar[1].Help=NULL;
    nbmenu=2;
    break;
 case 2:
    bar[0].Titre="Config. Default "; bar[0].fct=10; bar[0].Help=NULL;
    bar[1].Titre="File setting    "; bar[1].fct=11; bar[1].Help=NULL;
    bar[2].Titre=NULL;               bar[2].fct=0;  bar[2].Help=NULL;
    bar[3].Titre="Color Definition"; bar[3].fct=12; bar[3].Help=NULL;
    bar[4].Titre="Palette         "; bar[4].fct=13; bar[4].Help=NULL;
    bar[5].Titre=NULL;               bar[5].fct=0;  bar[5].Help=NULL;
    bar[6].Titre="Load KKSETUP.INI"; bar[6].fct=5;  bar[6].Help=NULL;
    bar[7].Titre="Write Profile   "; bar[7].fct=9;  bar[7].Help=NULL;
    nbmenu=8;
    break;
 case 3:
    bar[0].Titre="Help "; bar[0].fct=1; bar[0].Help=NULL;
    bar[1].Titre="About"; bar[1].fct=2; bar[1].Help=NULL;
    bar[2].Titre=NULL;    bar[2].fct=0; bar[2].Help=NULL;
    bar[3].Titre="Exit "; bar[3].fct=7; bar[3].Help=NULL;
    nbmenu=4;
    break;
    }

s=2;
menu.x=x;
menu.y=s;
menu.cur=cpos[poscur];
menu.attr=0;

retour=PannelMenu(bar,nbmenu,&menu);      // (x,y)=(t,s)

cpos[poscur]=menu.cur;

if (retour==2)
    {
    fin=bar[cpos[poscur]].fct;
    break;
    }
    else
    {
    poscur+=retour;
    fin=0;
    }

}
while(1);

LoadScreen();

return fin;
}

/*--------------------------------------------------------------------*\
|- 0: Quit                                                            -|
|- 1: Help                                                            -|
|- 2: About                                                           -|
|- 3: list all the format                                             -|
|- 4: search application                                              -|
|- 5: load kksetup.ini                                                -|
|- 6: Putinpath                                                       -|
|- 7: Exit                                                            -|
|- 8: Show all player                                                 -|
|- 9: Sauve le fichier des configurations                             -|
|-10: Appelle le menu setup configuration                             -|
|-11: Configuration de l'editeur                                      -|
|-12: Setup couleur                                                   -|
|-13: Setup Palette                                                   -|
\*--------------------------------------------------------------------*/

void GestionFct(int i)
{
char buffer[256];

switch(i)
    {
    case 0:
        break;  // Quit
    case 1:
        SaveScreen();
        PutCur(32,0);
        ChrWin(0,0,Cfg->TailleX-1,Cfg->TailleY-1,32);
        Help();
        LoadScreen();
        break;
    case 2:
        WinMesg("About",RBTitle,0);
        break;
    case 3:
        IdfListe();
        break;
    case 4:
        ApplSearch();
        break;
    case 5:
        SearchPart(buffer);
        if (buffer[0]!='*')
            {
            LoadConfigFile(buffer);
            InitMode();
            }
        break;
    case 6:
        PutInPath();
        break;
    case 7:
        break;
    case 8:
        if (ShowYourPlayer()==0)
            {
            DispMessage("KKSETUP must search application");
            DispMessage("  -> Go to menu 'Player'");
            DispMessage("  -> Select 'Search Player'");
            DispMessage("");
            }
        break;
    case 9:
        SaveConfigFile();
        break;
    case 10:
        ConfigSetup();
        break;
    case 11:
        FileSetup();
        break;
    case 12:
        ColorChange();
        break;
    case 13:
        ChangePalette();
        break;
    }
}

/*--------------------------------------------------------------------*\
|---- Crc - 32 BIT ANSI X3.66 CRC checksum files ----------------------|
\*--------------------------------------------------------------------*/

static unsigned long int crc_32_tab[] = {   // CRC polynomial 0xedb88320
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


/*--------------------------------------------------------------------*\
|-  Return -1 if error, 0 in other case                               -|
\*--------------------------------------------------------------------*/

int crc32file(char *name,unsigned long *crc)
{
FILE *fin;
unsigned long oldcrc32;
unsigned long crc32;
unsigned long oldcrc;
int c;
long charcnt;

oldcrc32 = 0xFFFFFFFF;
charcnt = 0;

if ((fin=fopen(name, "rb"))==NULL)
    {
    perror(name);
    return -1;
    }

while ((c=getc(fin))!=EOF)
    {
    ++charcnt;
    oldcrc32 = (crc_32_tab[(oldcrc32^c) & 0xff]^(oldcrc32 >> 8));
    }

if (ferror(fin))
    {
    perror(name);
    charcnt = -1;
    }
fclose(fin);

crc32 = oldcrc32;
oldcrc = oldcrc32 = ~oldcrc32;

*crc=oldcrc;

return 0;
}

/*--------------------------------------------------------------------*\
|- Charge un configuration par default                                -|
|- ------------------------------------------------------------------ -|
|- 0: Ketchup Improvement                                             -|
|- 1: Classic Configuration                                           -|
\*--------------------------------------------------------------------*/

void DefaultConfig(int a)
{
switch(a)
    {
    case 0:                                  //--- Configuration normale
        KKCfg->enterkkd=1;                      //--- Entre dans les KKD
        KKCfg->ajustview=1;      //--- Ajuste l'affichage pour le viewer
        KKCfg->cnvhist=1;                  //--- Conversion de l'history
        KKCfg->dispcolor=1;                  //--- Souligne les couleurs
        KKCfg->hidfil=1;               //--- Affiche les fichiers caches
        break;
    case 1:
        KKCfg->enterkkd=0;                //--- N'entre pas dans les KKD
        KKCfg->ajustview=0;//--- N'ajuste pas l'affichage pour le viewer
        KKCfg->cnvhist=0;           //--- Pas de conversion de l'history
        KKCfg->dispcolor=0;           //--- Ne souligne pas les couleurs
        KKCfg->hidfil=0;         //--- N'affiche pas les fichiers caches
        break;
    }
}

/*--------------------------------------------------------------------*\
|- Confiration par default                                            -|
\*--------------------------------------------------------------------*/
void ConfigSetup(void)
{
static int sy;

static char x1=32;
static int y1=2;

struct Tmt T[] = {
      {5, 2,10,"Ketchup Improvement ",&sy},
      {5, 3,10,"Classic Configuration ",&sy},

      {3,1,9,&x1,&y1},

      {3,5,2,NULL,NULL},                                      // le OK
      {24,5,3,NULL,NULL}                                   // le CANCEL
      };

struct TmtWin F = {-1,5,40,12,"Configuration"};

int n;

sy=0;

n=WinTraite(T,5,&F,0);

if (n==27) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

DefaultConfig(sy);
}

/*--------------------------------------------------------------------*\
|- Comparaison entre 2 noms de fichiers                               -|
\*--------------------------------------------------------------------*/
int FileComp(char *a,char *b)
{
int n;
char an,bn;

n=0;

while(1)
    {
    an=toupper(a[n]);
    bn=toupper(b[n]);

    if ( ((an==0) | (an==32)) & ((bn==0) | (bn==32)) )
        return 0;

    if (an!=bn)
        return 1;

    n++;
    }
}


/*--------------------------------------------------------------------*\
|- Search Part in file .ini                                           -|
\*--------------------------------------------------------------------*/
void SearchPart(char *part)
{
char filename[128];
FILE *fic;
char buffer[256];

int x,y,n;

MENU menu;
struct barmenu bar[20];
char bars[20][25];
int nbmenu=0,retour;

strcpy(filename,Fics->path);
Path2Abs(filename,"kksetup.ini");

fic=fopen(filename,"rt");
if (fic==NULL) return;

while(fgets(buffer,256,fic)!=NULL)
    {
    buffer[strlen(buffer)-1]=0;          //--- Efface le ENTER ---------
    ClearSpace(buffer);
    buffer[22]=0;

    if (buffer[0]=='[')
        {
        buffer[strlen(buffer)-1]=0;      //--- Efface le ENTER ---------
        strcpy(bars[nbmenu],buffer+1);
        bar[nbmenu].Titre=bars[nbmenu];
        bar[nbmenu].fct=nbmenu+1;
        bar[nbmenu].Help=NULL;
        nbmenu++;
        if (nbmenu==20) break;
        }
    }
fclose(fic);

if (nbmenu>1)
    {
    DispMessage("Select the configuration");

    x=45;
    y=3;
    n=0;

    do
        {
        menu.x=x;
        menu.y=y;
        menu.cur=n;
        menu.attr=0;

        retour=PannelMenu(bar,nbmenu,&menu);

        n=menu.cur;
        }
    while ((retour==1) | (retour==-1));

    if (retour==2)
        strcpy(part,bar[n].Titre);
        else
        strcpy(part,"*");
    }

if (nbmenu==1)
    strcpy(part,bar[n].Titre);


}

void Str2Pal(char *from,char *to)
{
int n;
char cf,ct;

for(n=0;n<48;n++)
    {
    ct=0;
    cf=from[n];

    if ((cf>='A') & (cf<='Z')) ct=cf-'A';
    if ((cf>='a') & (cf<='z')) ct=cf-'a'+26;
    if ((cf>='0') & (cf<='9')) ct=cf-'0'+52;
    if (cf=='+') ct=62;
    if (cf=='-') ct=63;

    to[n]=ct;
    }
}

void Pal2Str(char *from,char *to)
{
int n;
char cf,ct;

for(n=0;n<48;n++)
    {
    ct=0;
    cf=from[n];

    if (cf<26) ct=cf+'A';
    if ((cf>=26) & (cf<52)) ct=cf+'a'-26;
    if ((cf>=52) & (cf<62)) ct=cf+'0'-52;
    if (cf==62) ct='+';
    if (cf==63) ct='-';

    to[n]=ct;
    }
to[48]=0;
}

void Interroge(char *path,struct player *app,char *Verif,char *GVerif)
{
static char CadreLength=70;
static char Msg1[256],Msg2[256],Msg3[256],Msg4[256];
static int CadreWidth=4;

struct Tmt T[] = {
      { 5,7,5,"     Yes     ",NULL},
      {22,7,5,"     No      ",NULL},
      {39,7,5,"  Always Yes ",NULL},
      {56,7,5,"  Always No  ",NULL},
      { 5,2,0,Msg1,NULL},
      { 5,3,0,Msg2,NULL},
      { 5,4,0,Msg3,NULL},
      { 5,5,0,Msg4,NULL},
      { 1,1,9,&CadreLength,&CadreWidth}
      };

struct TmtWin F = {-1,3,74,12, "Security prompt" };

strcpy(Msg1,"Do you think that");
strcpy(Msg2,path);
sprintf(Msg3,"is %s of",app->Titre);
sprintf(Msg4,"%s",app->Meneur);

switch(WinTraite(T,9,&F,0))
    {
    case 0:
        (*Verif)=1;     // Yes
        break;
    case 27:
    case 1:
        (*Verif)=2;     // Cancel
    case 2:
        (*Verif)=1;
        (*GVerif)=1;    // Replace ALL
        break;
    case 3:
        (*Verif)=2;
        (*GVerif)=2;    // Cancel ALL
    }
}

void Col2Str(char *from,char *to)
{
int n;

for(n=0;n<64;n++)
    sprintf(to+n*2,"%02X",from[n]);
}

void Str2Col(char *from,char *to)
{
int n,m;

for(n=0;n<64;n++)
    {
    sscanf(from+n*2,"%02X",&m);
    to[n]=m;
    }
}

void InitMode(void)
{
int x,y;

if (todo==0)
    {
    TXTMode();
    InitFont();

    LoadPal(Cfg->palette);

    Cadre(0,1,Cfg->TailleX-1,(Cfg->TailleY-2),1,Cfg->col[55],Cfg->col[56]);
    Window(1,2,Cfg->TailleX-2,(Cfg->TailleY-3),Cfg->col[16]);

    ColLin(0,0,Cfg->TailleX,Cfg->col[17]);
    ChrLin(0,0,Cfg->TailleX,32);

    PrintAt((Cfg->TailleX-38)/2,0,"Setup of Ketchup Killers Commander");
    }
    else
    {
    TXTMode();
    InitFont();

    LoadPal(Cfg->palette);

    for(x=0;x<OldX;x++)
        for(y=0;y<OldY;y++)
            {
            AffCol(x,y,Screen_Buffer[(x+y*OldX)*2+1]);
            AffChr(x,y,Screen_Buffer[(x+y*OldX)*2]);
            }
    }

InitMessage();
}

void ColorChange(void)
{
MENU menu;
int retour,n,x,y;
static struct barmenu bar[19];

bar[0].Titre="Pannel"; bar[0].Help=NULL;
bar[0].fct=1;

bar[1].Titre="KeyBar"; bar[1].Help=NULL;
bar[1].fct=2;

bar[2].Titre="Window 1"; bar[2].Help=NULL;
bar[2].fct=3;

bar[3].Titre="Window 2"; bar[3].Help=NULL;
bar[3].fct=4;

bar[4].Titre="Help"; bar[4].Help=NULL;
bar[4].fct=5;

bar[5].Titre="Input Box"; bar[5].Help=NULL;
bar[5].fct=6;

bar[6].Titre="PullDown Bar"; bar[6].Help=NULL;
bar[6].fct=7;

bar[7].Titre="PullDown Menu"; bar[7].Help=NULL;
bar[7].fct=8;

bar[8].Titre="HTML Viewer"; bar[8].Help=NULL;
bar[8].fct=9;

SaveScreen();

IColor();


x=20;
y=4;
n=0;

do
    {
    menu.cur=n;
    menu.x=x;
    menu.y=y;
    menu.attr=0;

    retour=PannelMenu(bar,9,&menu);

    n=menu.cur;
    if (retour==2)
        CColor(bar[n].fct-1);
    }
while ( (retour==1) | (retour==-1) | (retour==2) );

LoadScreen();
}

void IColor(void)       // InitColor
{
int x,y;
int Hexa[]={'0','1','2','3','4','5','6','7','8','9',
                                               'A','B','C','D','E','F'};

Cadre(0,0,18,18,0,Cfg->col[55],Cfg->col[56]);
AffChr(1,1,3);
for(x=0;x<16;x++)
    {
    AffCol(x+2,1,Cfg->col[16]);
    AffCol(1,x+2,Cfg->col[16]);
    AffChr(x+2,1,Hexa[x]);
    AffChr(1,x+2,Hexa[x]);
    for(y=0;y<16;y++)
        {
        AffCol(x+2,y+2,y*16+x);
        AffChr(x+2,y+2,'+');
        }
    }
}


int DColor(int col)
{
int x,y,car;

x=col&15;
y=col/16;

AffCol(x+2,1,Cfg->col[18]);
AffCol(1,y+2,Cfg->col[18]);
AffChr(x+2,y+2,'*');
car=Wait(0,0,0);
AffCol(x+2,1,Cfg->col[16]);
AffCol(1,y+2,Cfg->col[16]);
AffChr(x+2,y+2,'+');

switch(HI(car))
    {
    case 72:    y--;    break;
    case 80:    y++;    break;
    case 0x4B:  x--;    break;
    case 0x4D:  x++;    break;
    }
x=x&15;
y=y&15;

if (car==27)
    return -1;
    else
    return y*16+x;
}

void CColor(int m)
{
int retour,n,x,y;
static struct barmenu bar[16];
MENU menu;
int r;

SaveScreen();

x=20;
y=4;

switch(m)
    {
    case 0:      // Pannel
        bar[0].Titre="Border1"; bar[0].Help=NULL;
        bar[0].fct=38;

        bar[1].Titre="Border2"; bar[1].Help=NULL;
        bar[1].fct=39;

        bar[2].Titre="Normal"; bar[2].Help=NULL;
        bar[2].fct=1;

        bar[3].Titre="Bright"; bar[3].Help=NULL;
        bar[3].fct=3;

        bar[4].Titre="Reverse"; bar[4].Help=NULL;
        bar[4].fct=2;

        bar[5].Titre="Underline"; bar[5].Help=NULL;
        bar[5].fct=5;

        bar[6].Titre="Bright Reverse"; bar[6].Help=NULL;
        bar[6].fct=4;

        bar[7].Titre="Underline"; bar[7].Help=NULL;
        bar[7].fct=40;

        bar[8].Titre="Bright Reverse"; bar[8].Help=NULL;
        bar[8].fct=41;

        bar[9].Titre=""; bar[9].Help=NULL;
        bar[9].fct=0;

        bar[10].Titre="EXEcutable"; bar[10].Help=NULL;
        bar[10].fct=16;
        bar[11].Titre="ARChive"; bar[11].Help=NULL;
        bar[11].fct=23;
        bar[12].Titre="SouNDfile"; bar[12].Help=NULL;
        bar[12].fct=24;
        bar[13].Titre="BitMaP"; bar[13].Help=NULL;
        bar[13].fct=33;
        bar[14].Titre="TeXT"; bar[14].Help=NULL;
        bar[14].fct=34;
        bar[15].Titre="USeR defined"; bar[15].Help=NULL;
        bar[15].fct=35;

        n=0;

        Window(59,2,77,18,0);
        AffColScreen(m);

        do
            {
            menu.x=x;
            menu.y=y;
            menu.cur=n;
            menu.attr=0;

            retour=PannelMenu(bar,16,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;



    case 1:     // KeyBar
        bar[0].Titre="Normal"; bar[0].Help=NULL;
        bar[0].fct=7;

        bar[1].Titre="Bright"; bar[1].Help=NULL;
        bar[1].fct=6;

        n=0;

        Window(60,3,77,5,0);
        AffColScreen(m);

        do
            {
            menu.x=x;
            menu.y=y;
            menu.cur=n;
            menu.attr=0;

            retour=PannelMenu(bar,2,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 2:     // Window 1
        bar[0].Titre="Border1"; bar[0].Help=NULL;
        bar[0].fct=56;

        bar[1].Titre="Border2"; bar[1].Help=NULL;
        bar[1].fct=57;

        bar[2].Titre="Normal"; bar[2].Help=NULL;
        bar[2].fct=17;

        bar[3].Titre="Bright"; bar[3].Help=NULL;
        bar[3].fct=18;

        bar[4].Titre="Reverse"; bar[4].Help=NULL;
        bar[4].fct=19;

        n=0;

        Window(59,2,77,8,0);
        AffColScreen(m);

        do
            {
            menu.x=x;
            menu.y=y;
            menu.cur=n;
            menu.attr=0;

            retour=PannelMenu(bar,5,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 3:    // Window 2
        bar[0].Titre="Border1"; bar[0].Help=NULL;
        bar[0].fct=47;

        bar[1].Titre="Border2"; bar[1].Help=NULL;
        bar[1].fct=48;

        bar[2].Titre="Normal"; bar[2].Help=NULL;
        bar[2].fct=29;

        bar[3].Titre="Bright"; bar[3].Help=NULL;
        bar[3].fct=30;

        bar[4].Titre="Reverse"; bar[4].Help=NULL;
        bar[4].fct=31;

        n=0;

        Window(59,2,77,8,0);
        AffColScreen(m);


        do
            {
            menu.x=x;
            menu.y=y;
            menu.cur=n;
            menu.attr=0;

            retour=PannelMenu(bar,5,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );

        Cfg->col[50]=Cfg->col[47];
        Cfg->col[51]=Cfg->col[30];
        break;

    case 4:     // Help
        bar[0].Titre="Border1"; bar[0].Help=NULL;
        bar[0].fct=53;

        bar[1].Titre="Border2"; bar[1].Help=NULL;
        bar[1].fct=54;

        bar[2].Titre="Normal"; bar[2].Help=NULL;
        bar[2].fct=25;

        bar[3].Titre="Bright"; bar[3].Help=NULL;
        bar[3].fct=26;

        bar[4].Titre="Reverse"; bar[4].Help=NULL;
        bar[4].fct=27;

        bar[5].Titre="Underline"; bar[5].Help=NULL;
        bar[5].fct=28;

        bar[6].Titre="Bright Reverse"; bar[6].Help=NULL;
        bar[6].fct=55;

        n=0;

        Window(59,2,77,10,0);
        AffColScreen(m);

        do
            {
            menu.x=x;
            menu.y=y;
            menu.cur=n;
            menu.attr=0;

            retour=PannelMenu(bar,7,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 5:     // Input Box
        bar[0].Titre="Border1"; bar[0].Help=NULL;
        bar[0].fct=45;

        bar[1].Titre="Border2"; bar[1].Help=NULL;
        bar[1].fct=46;

        bar[2].Titre="Normal"; bar[2].Help=NULL;
        bar[2].fct=20;

        bar[3].Titre="Bright"; bar[3].Help=NULL;
        bar[3].fct=21;

        bar[4].Titre="Reverse"; bar[4].Help=NULL;
        bar[4].fct=22;

        bar[5].Titre="Button Off"; bar[5].Help=NULL;
        bar[5].fct=49;

        bar[6].Titre="Button On"; bar[6].Help=NULL;
        bar[6].fct=50;

        n=0;

        Window(59,2,77,10,0);
        AffColScreen(m);

        do
            {
            menu.x=x;
            menu.y=y;
            menu.cur=n;
            menu.attr=0;

            retour=PannelMenu(bar,7,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 6:     // PullDown Bar
        bar[0].Titre="Normal"; bar[0].Help=NULL;
        bar[0].fct=8;

        bar[1].Titre="Bright"; bar[1].Help=NULL;
        bar[1].fct=43;

        bar[2].Titre="Reverse"; bar[2].Help=NULL;
        bar[2].fct=9;

        bar[3].Titre="Bright Reverse"; bar[3].Help=NULL;
        bar[3].fct=44;

        n=0;

        Window(60,3,75,5,0);
        AffColScreen(m);

        do
            {
            menu.x=x;
            menu.y=y;
            menu.cur=n;
            menu.attr=0;

            retour=PannelMenu(bar,4,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 7:     // PullDown Menu
        bar[0].Titre="Border1"; bar[0].Help=NULL;
        bar[0].fct=10;

        bar[1].Titre="Border2"; bar[1].Help=NULL;
        bar[1].fct=42;

        bar[2].Titre="Normal"; bar[2].Help=NULL;
        bar[2].fct=11;

        bar[3].Titre="Bright"; bar[3].Help=NULL;
        bar[3].fct=12;

        bar[4].Titre="Reverse"; bar[4].Help=NULL;
        bar[4].fct=13;

        bar[5].Titre="Bright Reverse"; bar[5].Help=NULL;
        bar[5].fct=14;

        n=0;

        Window(59,2,77,7,0);
        AffColScreen(m);

        do
            {
            menu.x=x;
            menu.y=y;
            menu.cur=n;
            menu.attr=0;

            retour=PannelMenu(bar,6,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;

    case 8:     // Viewer HTML
        bar[0].Titre="Title"; bar[0].Help=NULL;
        bar[0].fct=36;

        bar[1].Titre="H1"; bar[1].Help=NULL;
        bar[1].fct=37;

        bar[2].Titre="H2"; bar[2].Help=NULL;
        bar[2].fct=51;

        bar[3].Titre="H3"; bar[3].Help=NULL;
        bar[3].fct=52;

        bar[4].Titre="H4"; bar[4].Help=NULL;
        bar[4].fct=58;

        bar[5].Titre="H5"; bar[5].Help=NULL;
        bar[5].fct=59;

        bar[6].Titre="H6"; bar[6].Help=NULL;
        bar[6].fct=60;

        bar[7].Titre="Bold"; bar[7].Help=NULL;
        bar[7].fct=61;

        bar[8].Titre="Italic"; bar[8].Help=NULL;
        bar[8].fct=62;

        bar[9].Titre="Underline"; bar[9].Help=NULL;
        bar[9].fct=63;

        n=0;

        Window(59,2,77,15,0);
        AffColScreen(m);

        do
            {
            menu.x=x;
            menu.y=y;
            menu.cur=n;
            menu.attr=0;

            retour=PannelMenu(bar,10,&menu);

            n=menu.cur;

            if (retour==2)
                {
                do
                    {
                    AffColScreen(m);

                    r=DColor(Cfg->col[(bar[n].fct)-1]);
                    if (r!=-1)
                        Cfg->col[(bar[n].fct)-1]=r;
                    }
                while(r!=-1);
                }
            }
        while ( (retour==1) | (retour==-1) | (retour==2) );
        break;
    }
LoadScreen();
}

void AffColScreen(int a)
{
switch(a)
    {
    case 0:
        Cadre(60,3,76,17,0,Cfg->col[37],Cfg->col[38]);
        ColLin(61,4,15,Cfg->col[0]);
        ColLin(61,5,15,Cfg->col[2]);
        ColLin(61,6,15,Cfg->col[1]);
        ColLin(61,7,15,Cfg->col[4]);
        ColLin(61,8,15,Cfg->col[3]);
        ColLin(61,9,15,Cfg->col[39]);
        ColLin(61,10,15,Cfg->col[40]);

        ColLin(61,11,15,Cfg->col[15]);
        ColLin(61,12,15,Cfg->col[22]);
        ColLin(61,13,15,Cfg->col[23]);
        ColLin(61,14,15,Cfg->col[32]);
        ColLin(61,15,15,Cfg->col[33]);
        ColLin(61,16,15,Cfg->col[34]);

        PrintAt(61,4,"normal  exe");
        PrintAt(61,5,"select  com");
        PrintAt(61,6,"current ini");
        PrintAt(61,7,"c:\path");
        PrintAt(61,8,"cursel  rb");
        PrintAt(61,9, "normal");
        PrintAt(61,10,"bright");

        PrintAt(61,11,"exe com bat");
        PrintAt(61,12,"rar arj zip");
        PrintAt(61,13,"mod s3m wav");
        PrintAt(61,14,"bmp lbm png");
        PrintAt(61,15,"txt diz doc");
        PrintAt(61,16,"xyz");
        break;
    case 1:
        ColLin(61,4,2,Cfg->col[5]);
        ColLin(63,4,6,Cfg->col[6]);
        ColLin(69,4,2,Cfg->col[5]);
        ColLin(71,4,6,Cfg->col[6]);
        PrintAt(61,4,"F1 Help F2 Menu");
        break;

    case 2:
        Cadre(60,3,76,7,0,Cfg->col[55],Cfg->col[56]);
        ColLin(61,4,15,Cfg->col[16]);
        ColLin(61,5,15,Cfg->col[17]);
        ColLin(61,6,15,Cfg->col[18]);

        PrintAt(61,4,"Normal");
        PrintAt(61,5,"Bright");
        PrintAt(61,6,"Reverse");
        break;

    case 3:
        Cadre(60,3,76,7,0,Cfg->col[46],Cfg->col[47]);
        ColLin(61,4,15,Cfg->col[28]);
        ColLin(61,5,15,Cfg->col[29]);
        ColLin(61,6,15,Cfg->col[30]);

        PrintAt(61,4,"Normal");
        PrintAt(61,5,"Bright");
        PrintAt(61,6,"Reverse");
        break;

    case 4:
        Cadre(60,3,76,9,0,Cfg->col[52],Cfg->col[53]);
        ColLin(61,4,15,Cfg->col[24]);
        ColLin(61,5,15,Cfg->col[25]);
        ColLin(61,6,15,Cfg->col[26]);
        ColLin(61,7,15,Cfg->col[27]);
        ColLin(61,8,15,Cfg->col[54]);
                    

        PrintAt(61,4,"Normal");
        PrintAt(61,5,"Bright");
        PrintAt(61,6,"Reverse");
        PrintAt(61,7,"Underline");
        PrintAt(61,8,"Bright Reverse");
        break;

    case 5:
        Cadre(60,3,76,9,0,Cfg->col[44],Cfg->col[45]);
        Window(61,4,75,8,Cfg->col[19]);
        ColLin(61,5,15,Cfg->col[20]);
        ColLin(61,6,15,Cfg->col[21]);

        PrintAt(61,4,"Normal");
        PrintAt(61,5,"Bright");
        PrintAt(61,6,"Reverse");

        Puce(62,7,4,2);
        Puce(69,7,5,0);
        PrintAt(63,7,"On");
        PrintAt(70,7,"Off");
        break;

    case 6:
        ColLin(61,4,1,Cfg->col[42]);
        ColLin(62,4,4,Cfg->col[7]);
        ColLin(66,4,1,Cfg->col[43]);
        ColLin(67,4,7,Cfg->col[8]);
        PrintAt(61,4,"Menu Selected");
        break;

    case 7:
        Cadre(60,3,76,6,0,Cfg->col[9],Cfg->col[41]);
        ColLin(61,4,1,Cfg->col[11]);
        ColLin(62,4,14,Cfg->col[10]);
        ColLin(61,5,1,Cfg->col[13]);
        ColLin(62,5,14,Cfg->col[12]);

        PrintAt(61,4,"Menu");
        PrintAt(61,5,"Selected");
        break;

    case 8:
        Cadre(60,3,76,14,0,Cfg->col[37],Cfg->col[38]);
        ColLin(61,4,15,Cfg->col[35]);
        ColLin(61,5,15,Cfg->col[36]);
        ColLin(61,6,15,Cfg->col[50]);
        ColLin(61,7,15,Cfg->col[51]);
        ColLin(61,8,15,Cfg->col[57]);
        ColLin(61,9,15,Cfg->col[58]);
        ColLin(61,10,15,Cfg->col[59]);
        ColLin(61,11,15,Cfg->col[60]);
        ColLin(61,12,15,Cfg->col[61]);
        ColLin(61,13,15,Cfg->col[62]);

        PrintAt(61,4,"Title");
        PrintAt(61,5,"H1");
        PrintAt(61,6,"H2");
        PrintAt(61,7,"H3");
        PrintAt(61,8,"H4");
        PrintAt(61,9,"H5");
        PrintAt(61,10,"H6");
        PrintAt(61,11,"Bold");
        PrintAt(61,12,"Italic");
        PrintAt(61,13,"Underline");
        break;
    }
}


void ChangePalette(void)
{
int x,y,i;
char rec,reloadpal;

int n,m,ntot;
int nt,mt;
int car;

int ex,ey;
int x1,y1;

char *titre="Palette configuration";

#define NBRS 4

char defcol[NBRS][48]={ RBPALDEF ,
                     { 0, 0, 0, 42,42,42,  0, 0, 0, 63,63,63,
                      63,63,63, 63,63,32, 42,63,63,  0, 0,43,
                      63,63, 0, 63,63,63,  0, 0,43, 57,63, 0,
                      30,60,30,  0,40,63,  0, 0, 0,  0, 0, 0},
                     {25,36,29, 36,18,15,  0, 0, 0, 49,39,45,
                      44,63,63, 42,37,63, 45,39,35,  0, 0, 0,
                       0,63,63, 63,63,63, 25,36,29, 63, 0, 0,
                       0,63, 0,  0, 0,63,  0, 0, 0,  0, 0, 0},
                     {42,37,63, 14,22,17,  0, 0, 0, 58,58,50,
                      18, 1,36, 63,63,21, 58,42,49, 16,16,32,
                      63,63, 0, 63,63,63, 43,37,63, 63,20,20,
                      20,40,20,  0,40,40,  0, 0, 0,  0, 0, 0}
                       };

char *Style[NBRS]={"Default Style","Norton Style","Cyan Style",
                                                             "Venus "};
int posx[NBRS],posy[NBRS];

SaveScreen();
PutCur(32,0);

Cadre(0,0,Cfg->TailleX-1,(Cfg->TailleY)-2,2,Cfg->col[55],Cfg->col[56]);
Window(1,1,Cfg->TailleX-2,(Cfg->TailleY)-3,Cfg->col[16]);

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
    Cadre(posx[n]-2,posy[n]-1,posx[n]+15,posy[n]+1,2
                                            ,Cfg->col[55],Cfg->col[56]);
    ColLin(posx[n],posy[n],strlen(Style[n]),Cfg->col[16]);
    PrintAt(posx[n],posy[n],Style[n]);
    }

if (Cfg->TailleY==50)
    {
    Cadre(2,2,77,4,2,Cfg->col[55],Cfg->col[56]);
    Cadre(2,5,77,44,2,Cfg->col[55],Cfg->col[56]);
    PrintAt(30,3,titre);

    Cadre(2,45,77,47,2,Cfg->col[55],Cfg->col[56]);

    Cadre(52,16,73,40,2,Cfg->col[55],Cfg->col[56]);
    PrintAt(5,46,"%s /RedBug",RBTitle);

    Cadre(53,17,72,21,2,Cfg->col[55],Cfg->col[56]);
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

        Cadre(x-1,y-1,x+9,y+3,1,Cfg->col[55],Cfg->col[56]);
        Window(x,y,x+8,y+2,Cfg->col[16]);

        Cadre(x+10,y-1,x+14,y+3,1,Cfg->col[55],Cfg->col[56]);
        ColWin(x+11,y,x+13,y+2,nt*16+nt);
        ChrWin(x+11,y,x+13,y+2,220);

        for(mt=0;mt<3;mt++)
            {
            Gradue(x,y+mt,8,0,Cfg->palette[nt*3+mt],64);
            ColWin(x,y+mt,x+8,y+mt,Cfg->col[16]);
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

    ColWin(x,y+m,x+8,y+m,Cfg->col[17]);

    car=Wait(0,0,0);

    ColWin(x,y+m,x+8,y+m,Cfg->col[16]);

    switch(HI(car))
        {
        case 0x47:                                               // HOME
            ChrWin(x,y+m,x+8,y+m,32);
            Cfg->palette[n*3+m]=0;
            reloadpal=1;
            break;
        case 0X4F:                                                // END
            ChrWin(x,y+m,x+8,y+m,32);
            Cfg->palette[n*3+m]=63;
            reloadpal=1;
            break;
        case 80:                                                  // bas
            m++;
            break;
        case 72:                                                 // haut
            m--;
            break;
        case 0x4B:                                             // gauche
            if (Cfg->palette[n*3+m]!=0)
                Cfg->palette[n*3+m]--;
            reloadpal=1;
            break;
        case 0x4D:                                             // droite
            if (Cfg->palette[n*3+m]!=63)
                Cfg->palette[n*3+m]++;
            reloadpal=1;
            break;
        case 0xF:                                           // SHIFT-TAB
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

    if (reloadpal)
        {
        LoadPal(Cfg->palette);
        reloadpal=0;
        }
    }
    else
    {
    ColLin(posx[n-16],posy[n-16],strlen(Style[n-16]),Cfg->col[17]);

    car=Wait(0,0,0);

    ColLin(posx[n-16],posy[n-16],strlen(Style[n-16]),Cfg->col[16]);

    switch(HI(car))
        {
        case 80:                                                  // bas
            n++;
            break;
        case 72:                                                 // haut
            n--;
            break;
        case 0xF:                                           // SHIFT-TAB
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
            LoadPal(Cfg->palette);
            rec=1;
            break;
        }

    if (n<0)        n=ntot;
    if (n>ntot)     n=0;
    }
}
while(car!=27);


LoadScreen();
}


