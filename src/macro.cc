#include <stdio.h>
#include <string.h>

#include "kk.h"

#define MACROVERSION 2

unsigned long _lbuf[256];
unsigned short _wbuf[256];
char _cbuf[256];
char *_sbuf[256];
unsigned int _lngbuf[256];

int _pos;

int MWinTraite(char *defbuf)
{
int res;

if (Info->defform==0)
    {
    res=(signed char)(_cbuf[0]);
    }
    else
    res=MacroTmt(defbuf);

NumHelp(0);
return res;
}


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void MacAlloc(int n,int nbr)
{
if (Info->macro)
    return;

if (_sbuf[n]!=NULL)
    LibMem(_sbuf[n]);

_sbuf[n]=(char*)GetMem(nbr);
_lngbuf[n]=nbr;
}

void MacFree(int n)
{
if (Info->macro)
    return;

if (_sbuf[n]!=NULL)
    {
    LibMem(_sbuf[n]);
    _sbuf[n]=NULL;
    _lngbuf[n]=0;
    }
}

void MacReadBuf(char *from,char *to,int taille)
{
memcpy(to,from+_pos,taille);
_pos+=taille;
}

int MacReadInt(char *from)
{
int n;
n=*(int*)(from+_pos);
_pos+=4;
return n;
}

int MacReadSChar(char *from)
{
_pos++;
return (signed char)from[_pos-1];
}

int MacReadUChar(char *from)
{
_pos++;
return (unsigned char)from[_pos-1];
}



/*--------------------------------------------------------------------*\
|- Traite une frame                                                   -|
\*--------------------------------------------------------------------*/
int MacroTmt(char *defbuf)
{
int res;
char *buftmt;

char kktver;

struct TmtWin KKWin;
struct Tmt KKTmt[100];
signed char KKNum[100];
signed char KKLnk[100];

int KKnbr;
int KKprem;
int i,n;

buftmt=defbuf;

KKWin.name=(char*)GetMem(80);

if (!memcmp(buftmt,"KKTm0",5))
    {
    _pos=5;
    kktver=1;
    }
else
    {
    _pos=0;
    kktver=0;
    }

KKWin.x1=MacReadSChar(buftmt);
KKWin.y1=MacReadSChar(buftmt);
KKWin.x2=MacReadSChar(buftmt);
KKWin.y2=MacReadSChar(buftmt);

n=MacReadUChar(buftmt);
MacReadBuf(buftmt,KKWin.name,n);
KKWin.name[n]=0;

KKprem=MacReadUChar(buftmt);
KKnbr=MacReadUChar(buftmt);

for(i=0;i<KKnbr;i++)
    KKTmt[i].entier=(int*)GetMem(sizeof(int));

for(i=0;i<KKnbr;i++)
    {
    KKNum[i]=0;
    KKTmt[i].type=(uchar)MacReadUChar(buftmt);

    KKTmt[i].x=MacReadUChar(buftmt);
    KKTmt[i].y=MacReadUChar(buftmt);

    switch(KKTmt[i].type)
        {
        case 0: //--- Titre --------------------------------------------
            KKTmt[i].str=(char*)GetMem(80);
            n=MacReadUChar(buftmt);
            MacReadBuf(buftmt,KKTmt[i].str,n);
            KKTmt[i].str[n]=0;
            break;

        case 1: //--- String -------------------------------------------
            (*(KKTmt[i].entier))=MacReadUChar(buftmt);

            n=MacReadUChar(buftmt);
            if (_sbuf[n]==NULL)
                {
                PrintAt(0,0,"(String %d)",n);
                WinError("Error in Form");
                }

            KKTmt[i].str=_sbuf[n];

            break;

        case 2:  //--- OK ----------------------------------------------
        case 3:  //--- CANCEL ------------------------------------------
        case 4:  //--- Invalid type ------------------------------------
            break;

        case 5:  //--- Bouton personnalis‚ -----------------------------
            KKTmt[i].str=(char*)GetMem(80);
            n=MacReadUChar(buftmt);
            MacReadBuf(buftmt,KKTmt[i].str,n);
            KKTmt[i].str[n]=0;

            KKNum[i]=(uchar)MacReadUChar(buftmt);
            break;

        case 6:
            break;

        case 7:  //--- Integer -----------------------------------------
            KKTmt[i].str=(char*)GetMem(80);
            n=MacReadUChar(buftmt);
            MacReadBuf(buftmt,KKTmt[i].str,n);
            KKTmt[i].str[n]=0;

            KKNum[i]=(uchar)MacReadUChar(buftmt);
            *(KKTmt[i].entier)=_wbuf[KKNum[i]];
            break;

        case 8:  //--- Switch ------------------------------------------
            KKTmt[i].str=(char*)GetMem(80);
            n=MacReadUChar(buftmt);
            MacReadBuf(buftmt,KKTmt[i].str,n);
            KKTmt[i].str[n]=0;

            KKNum[i]=(uchar)MacReadUChar(buftmt);
            *(KKTmt[i].entier)=_cbuf[KKNum[i]];
            break;

        case 9:  //--- Cadre -------------------------------------------
            KKTmt[i].str=(char*)GetMem(1);
            KKTmt[i].str[0]=(uchar)MacReadUChar(buftmt);

            (*(KKTmt[i].entier))=MacReadUChar(buftmt);
            break;

        case 10: //--- Switch multiple --------------------------------
            KKTmt[i].str=(char*)GetMem(80);

            n=MacReadUChar(buftmt);
            MacReadBuf(buftmt,KKTmt[i].str,n);
            KKTmt[i].str[n]=0;

            n=MacReadUChar(buftmt);

            KKNum[i]=(uchar)MacReadUChar(buftmt);

            KKLnk[i]=(uchar)MacReadUChar(buftmt);

            if (n!=i)
                {
                LibMem(KKTmt[i].entier);
                KKTmt[i].entier=KKTmt[n].entier;
                }

            if (KKLnk[i]==_cbuf[KKNum[i]])
                (*(KKTmt[i].entier))=i;

            break;
        }
    }

n=WinTraite(KKTmt,KKnbr,&KKWin,KKprem);

for(i=0;i<KKnbr;i++)
    {
    switch(KKTmt[i].type)
        {
        case 7:
            _wbuf[KKNum[i]]=(ushort)(*(KKTmt[i].entier));
            break;
        case 8:
            _cbuf[KKNum[i]]=(uchar)(*(KKTmt[i].entier));
            break;
        case 10:
            if (*(KKTmt[i].entier)!=i)
                KKNum[i]=-1;
                else
                _cbuf[KKNum[i]]=KKLnk[i];

            break;
        default:
            break;
        }
    }

for(i=0;i<KKnbr;i++)
    if (KKNum[i]!=-1)
        LibMem(KKTmt[i].entier);

if (n==-1)
    res=-1;
    else
    switch (KKTmt[n].type)
        {
        case 3:
            res=-1;
            break;
        case 2:
            res=0;
            break;
        case 5:
            res=KKNum[n];
            break;
        }

// Libere les KKTmt allouer


if (kktver==1)
    {
    char bufcmd[256],temp[256];

    bufcmd[0]=0;

    for(i=0;i<KKnbr;i++)
        {
        char len;

        len=(char)MacReadUChar(buftmt);
        if (len!=0)
            {
            MacReadBuf(buftmt,temp,len);
            temp[len]=0;
            switch(KKTmt[i].type)
                {
                case 1:  //--- string
                    strcat(bufcmd,temp);
                    strcat(bufcmd,KKTmt[i].str);
                    break;
                case 8:  //--- switch
                    if (_cbuf[KKNum[i]]==1)
                        strcat(bufcmd,temp);
                    break;
                case 10:
                    if (KKNum[i]!=-1)
                        strcat(bufcmd,temp);
                    break;
                }
            }
        }

    MacAlloc(10,strlen(bufcmd)+1);
    strcpy(_sbuf[10],bufcmd);
    }



return res;
}

void RunFirstTime(void)
{
RunMacro(Fics->path,"firstrun.kkp");
}



void RunAutoMacro(void)
{
RunMacro(KKFics->trash,"autorun.kkp");
}


void RunMacro(char *path,char *filename)
{
char file[256];

char *prog;
FILE *fic;
long lng,pos;
char fct;

unsigned short wvar;

int n;

strcpy(file,path);
Path2Abs(file,filename);

fic=fopen(file,"rb");
if (fic==NULL) return;

fseek(fic,36,SEEK_SET);
fread(&lng,1,4,fic);
prog=(char*)GetMem(lng);
fread(&pos,1,4,fic);
fread(prog,1,lng,fic);
fclose(fic);

Info->macro=1;

Info->defform=0;

for(n=0;n<256;n++)
    {
    _lbuf[n]=0;
    _cbuf[n]=0;
    _wbuf[n]=0;
    _sbuf[n]=NULL;
    _lngbuf[n]=0;
    }

do
{
fct=prog[pos];

switch(fct)
    {
    case 0x01:
        wvar=*(unsigned short*)(prog+pos+1);
        GestionFct(wvar);
        pos+=3;
        break;

    case 0x02:
        wvar=*(unsigned short*)(prog+pos+2);
        if (_sbuf[prog[pos+1]]!=NULL)
            {
            if (_lngbuf[prog[pos+1]]<wvar+1)
                {
                LibMem(_sbuf[prog[pos+1]]);
                _sbuf[prog[pos+1]]=(char*)GetMem(wvar+1);
                _lngbuf[prog[pos+1]]=wvar+1;
                }
            }
            else
            {
            _sbuf[prog[pos+1]]=(char*)GetMem(wvar+1);
            _lngbuf[prog[pos+1]]=wvar+1;
            }

        _sbuf[prog[pos+1]][wvar]=0;
        memcpy(_sbuf[prog[pos+1]],prog+pos+4,wvar);

        pos+=wvar+4;
        break;

    case 0x04:
        _cbuf[prog[pos+1]]=prog[pos+2];
        pos+=3;
        break;

    case 0x05:
        _lbuf[prog[pos+1]]=*(unsigned long*)(prog+pos+2);
        pos+=6;
        break;

    case 0x07:
        fic=NULL;
        switch(prog[pos+1])
            {
            case 1:
                strcpy(file,KKFics->trash);
                Path2Abs(file,"font8x16.cfg");
                fic=fopen(file,"wb");
                break;
            case 2:
                strcpy(file,KKFics->trash);
                Path2Abs(file,"font8x8.cfg");
                fic=fopen(file,"wb");
                break;
            }
        if (fic!=NULL)
            {
            fwrite(_sbuf[prog[pos+2]],_lngbuf[prog[pos+2]],1,fic);
            fclose(fic);
            pos+=3;
            }
            else
            {
            WinError("Erreur in macro");
            fct=6;
            }
        break;

    case 0x08:
        _wbuf[prog[pos+1]]=*(unsigned short*)(prog+pos+2);
        pos+=4;
        break;

    case 0x09:
        switch(prog[pos+2])
            {
            case  1: Cfg->TailleX=_lbuf[prog[pos+1]];             break;
            case  2: Cfg->TailleY=_lbuf[prog[pos+1]];             break;
            case  3: Cfg->windesign=_cbuf[prog[pos+1]];           break;
            case  4: Cfg->SaveSpeed=_lbuf[prog[pos+1]];           break;
            case  5: Cfg->font=_cbuf[prog[pos+1]];                break;
            case  6: Cfg->display=_cbuf[prog[pos+1]];             break;
            case  7: KKCfg->Esc2Close=_cbuf[prog[pos+1]];         break;
            case  8: KKCfg->pathdown=_cbuf[prog[pos+1]];          break;
            case  9: KKCfg->dispath=_cbuf[prog[pos+1]];           break;
            case 10: KKCfg->sizewin=_cbuf[prog[pos+1]];           break;
            case 11: KKCfg->isidf=_cbuf[prog[pos+1]];             break;
            case 12: KKCfg->isbar=_cbuf[prog[pos+1]];             break;
            case 13: KKCfg->dispcolor=_cbuf[prog[pos+1]];         break;
            case 14: memcpy(Cfg->palette,_sbuf[prog[pos+1]],48);  break;
            case 15: memcpy(Cfg->col,_sbuf[prog[pos+1]],64);      break;
            case 16: KKCfg->userfont=_cbuf[prog[pos+1]];          break;
            case 17: KKCfg->pntrep=_cbuf[prog[pos+1]];            break;
            case 18: KKCfg->hidfil=_cbuf[prog[pos+1]];            break;
            case 19: KKCfg->logfile=_cbuf[prog[pos+1]];           break;
            case 20: KKCfg->autoreload=_cbuf[prog[pos+1]];        break;
            case 21: KKCfg->verifhist=_cbuf[prog[pos+1]];         break;
            case 22: KKCfg->palafter=_cbuf[prog[pos+1]];          break;
            case 23: Cfg->debug=_cbuf[prog[pos+1]];               break;
            case 24: KKCfg->currentdir=_cbuf[prog[pos+1]];        break;
            case 25: Cfg->speedkey=_cbuf[prog[pos+1]];            break;
            case 26: KKCfg->insdown=_cbuf[prog[pos+1]];           break;
            case 27: KKCfg->seldir=_cbuf[prog[pos+1]];            break;
            case 28: Cfg->comport=_cbuf[prog[pos+1]];             break;
            case 29: KKCfg->enterkkd=_cbuf[prog[pos+1]];          break;
            case 30: strcpy(KKCfg->editeur,_sbuf[prog[pos+1]]);   break;
            case 31: strcpy(KKCfg->vieweur,_sbuf[prog[pos+1]]);   break;
            case 32: strcpy(KKCfg->ssaver,_sbuf[prog[pos+1]]);    break;
            case 33: Cfg->comspeed=_lbuf[prog[pos+1]];            break;
            case 34: KKCfg->cnvhist=_cbuf[prog[pos+1]];           break;
            case 35: KKCfg->esttime=_cbuf[prog[pos+1]];           break;
            case 36: KKCfg->confexit=_cbuf[prog[pos+1]];          break;
            case 37: KKCfg->savekey=_cbuf[prog[pos+1]];           break;
            case 38: KKCfg->KeyAfterShell=_cbuf[prog[pos+1]];     break;
            case 39: KKCfg->addselect=_cbuf[prog[pos+1]];         break;
            case 40: KKCfg->mtrash=_lbuf[prog[pos+1]];            break;
            case 41: Cfg->combit=_cbuf[prog[pos+1]];              break;
            case 42: Cfg->comparity=_cbuf[prog[pos+1]];           break;
            case 43: Cfg->comstop=_cbuf[prog[pos+1]];             break;
            case 44: Cfg->mousemode=_cbuf[prog[pos+1]];           break;
            case 45: KKCfg->lift=_cbuf[prog[pos+1]];              break;
            case 46: break;  // MACROVERSION
            case 47: Info->defform=_cbuf[prog[pos+1]];            break;
            case 48: KKCfg->internshell=_cbuf[prog[pos+1]];       break;
            case 49: memcpy(KKCfg->ExtTxt,_sbuf[prog[pos+1]],64); break;
            case 50: KKCfg->Enable_Txt=_cbuf[prog[pos+1]];        break;
            case 51: memcpy(KKCfg->ExtBmp,_sbuf[prog[pos+1]],64); break;
            case 52: KKCfg->Enable_Bmp=_cbuf[prog[pos+1]];        break;
            case 53: memcpy(KKCfg->ExtSnd,_sbuf[prog[pos+1]],64); break;
            case 54: KKCfg->Enable_Snd=_cbuf[prog[pos+1]];        break;
            case 55: memcpy(KKCfg->ExtArc,_sbuf[prog[pos+1]],64); break;
            case 56: KKCfg->Enable_Arc=_cbuf[prog[pos+1]];        break;
            case 57: memcpy(KKCfg->ExtExe,_sbuf[prog[pos+1]],64); break;
            case 58: KKCfg->Enable_Exe=_cbuf[prog[pos+1]];        break;
            case 59: memcpy(KKCfg->ExtUsr,_sbuf[prog[pos+1]],64); break;
            case 60: KKCfg->Enable_Usr=_cbuf[prog[pos+1]];        break;
            }
        pos+=3;
        break;

    case 0x2D:  //--- Affiche une forme --------------------------------
        _cbuf[0]=(uchar)MacroTmt(prog+pos+1);
        pos+=_pos+1;
        break;

    case 0x2E:  //--- Alloue de la m‚moire -----------------------------
        wvar=*(unsigned short*)(prog+pos+2);
        if (_sbuf[prog[pos+1]]!=NULL)
            LibMem(_sbuf[prog[pos+1]]);

        _sbuf[prog[pos+1]]=(char*)GetMem(wvar);
        _lngbuf[prog[pos+1]]=wvar;

        pos+=4;
        break;

    case 0x31:
        switch(prog[pos+2])
            {
            case  1: _lbuf[prog[pos+1]]=Cfg->TailleX;             break;
            case  2: _lbuf[prog[pos+1]]=Cfg->TailleY;             break;
            case  3: _cbuf[prog[pos+1]]=Cfg->windesign;           break;
            case  4: _lbuf[prog[pos+1]]=Cfg->SaveSpeed;           break;
            case  5: _cbuf[prog[pos+1]]=Cfg->font;                break;
            case  6: _cbuf[prog[pos+1]]=Cfg->display;             break;
            case  7: _cbuf[prog[pos+1]]=KKCfg->Esc2Close;         break;
            case  8: _cbuf[prog[pos+1]]=KKCfg->pathdown;          break;
            case  9: _cbuf[prog[pos+1]]=KKCfg->dispath;           break;
            case 10: _cbuf[prog[pos+1]]=KKCfg->sizewin;           break;
            case 11: _cbuf[prog[pos+1]]=KKCfg->isidf;             break;
            case 12: _cbuf[prog[pos+1]]=KKCfg->isbar;             break;
            case 13: _cbuf[prog[pos+1]]=KKCfg->dispcolor;         break;
            case 14: memcpy(_sbuf[prog[pos+1]],Cfg->palette,48);  break;
            case 15: memcpy(_sbuf[prog[pos+1]],Cfg->col,64);      break;
            case 16: _cbuf[prog[pos+1]]=KKCfg->userfont;          break;
            case 17: _cbuf[prog[pos+1]]=KKCfg->pntrep;            break;
            case 18: _cbuf[prog[pos+1]]=KKCfg->hidfil;            break;
            case 19: _cbuf[prog[pos+1]]=KKCfg->logfile;           break;
            case 20: _cbuf[prog[pos+1]]=KKCfg->autoreload;        break;
            case 21: _cbuf[prog[pos+1]]=KKCfg->verifhist;         break;
            case 22: _cbuf[prog[pos+1]]=KKCfg->palafter;          break;
            case 23: _cbuf[prog[pos+1]]=Cfg->debug;               break;
            case 24: _cbuf[prog[pos+1]]=KKCfg->currentdir;        break;
            case 25: _cbuf[prog[pos+1]]=Cfg->speedkey;            break;
            case 26: _cbuf[prog[pos+1]]=KKCfg->insdown;           break;
            case 27: _cbuf[prog[pos+1]]=KKCfg->seldir;            break;
            case 28: _cbuf[prog[pos+1]]=Cfg->comport;             break;
            case 29: _cbuf[prog[pos+1]]=KKCfg->enterkkd;          break;
            case 30: strcpy(_sbuf[prog[pos+1]],KKCfg->editeur);   break;
            case 31: strcpy(_sbuf[prog[pos+1]],KKCfg->vieweur);   break;
            case 32: strcpy(_sbuf[prog[pos+1]],KKCfg->ssaver);    break;
            case 33: _lbuf[prog[pos+1]]=Cfg->comspeed;            break;
            case 34: _cbuf[prog[pos+1]]=KKCfg->cnvhist;           break;
            case 35: _cbuf[prog[pos+1]]=KKCfg->esttime;           break;
            case 36: _cbuf[prog[pos+1]]=KKCfg->confexit;          break;
            case 37: _cbuf[prog[pos+1]]=KKCfg->savekey;           break;
            case 38: _cbuf[prog[pos+1]]=KKCfg->KeyAfterShell;     break;
            case 39: _cbuf[prog[pos+1]]=KKCfg->addselect;         break;
            case 40: _lbuf[prog[pos+1]]=KKCfg->mtrash;            break;
            case 41: _cbuf[prog[pos+1]]=Cfg->combit;              break;
            case 42: _cbuf[prog[pos+1]]=Cfg->comparity;           break;
            case 43: _cbuf[prog[pos+1]]=Cfg->comstop;             break;
            case 44: _cbuf[prog[pos+1]]=Cfg->mousemode;           break;
            case 45: _cbuf[prog[pos+1]]=KKCfg->lift;              break;
            case 46: _cbuf[prog[pos+1]]=MACROVERSION;             break;
            case 47: _cbuf[prog[pos+1]]=Info->defform;            break;
            case 48: _cbuf[prog[pos+1]]=KKCfg->internshell;       break;
            case 49: memcpy(_sbuf[prog[pos+1]],KKCfg->ExtTxt,64); break;
            case 50: _cbuf[prog[pos+1]]=KKCfg->Enable_Txt;        break;
            case 51: memcpy(_sbuf[prog[pos+1]],KKCfg->ExtBmp,64); break;
            case 52: _cbuf[prog[pos+1]]=KKCfg->Enable_Bmp;        break;
            case 53: memcpy(_sbuf[prog[pos+1]],KKCfg->ExtSnd,64); break;
            case 54: _cbuf[prog[pos+1]]=KKCfg->Enable_Snd;        break;
            case 55: memcpy(_sbuf[prog[pos+1]],KKCfg->ExtArc,64); break;
            case 56: _cbuf[prog[pos+1]]=KKCfg->Enable_Arc;        break;
            case 57: memcpy(_sbuf[prog[pos+1]],KKCfg->ExtExe,64); break;
            case 58: _cbuf[prog[pos+1]]=KKCfg->Enable_Exe;        break;
            case 59: memcpy(_sbuf[prog[pos+1]],KKCfg->ExtUsr,64); break;
            case 60: _cbuf[prog[pos+1]]=KKCfg->Enable_Usr;        break;
            }
        pos+=3;
        break;

    case 0x32:
        if (MACROVERSION<prog[pos+1])
            {
            WinError("Requiert a more recent version of KKC");
            fct=6;
            }
        pos+=2;
        break;
    }
}
while(fct!=6);

for(n=0;n<256;n++)
    if (_sbuf[n]!=NULL)
        {
        LibMem(_sbuf[n]);
        _sbuf[n]=NULL;
        _lngbuf[n]=0;
        }

Info->macro=0;

Info->defform=1;
}




