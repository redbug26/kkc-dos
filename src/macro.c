#include <stdio.h>
#include <string.h>

#include "kk.H"

static unsigned long lbuf[256];
static unsigned short wbuf[256];
static char cbuf[256];
static char *buf[256];
static unsigned short lngbuf[256];


void RunMacro(char *filename)
{
char file[256];

char *prog;
FILE *fic;
long lng,pos;
char fct;

unsigned short wvar;

int n;

strcpy(file,DFen->path);
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

for(n=0;n<256;n++)
    {
    lbuf[n]=0;
    cbuf[n]=0;
    wbuf[n]=0;
    buf[n]=NULL;
    lngbuf[n]=0;
    }

do
{
fct=prog[pos];
switch(fct)
    {
    case 1:
        wvar=*(unsigned short*)(prog+pos+1);
        Info->sbuf0=buf[0];
        Info->sbuf1=buf[1];
        Info->cbuf0=cbuf[0];
        Info->cbuf1=cbuf[1];
        GestionFct(wvar);
        cbuf[0]=Info->cbuf0;
        cbuf[1]=Info->cbuf1;
        pos+=3;
        break;

    case 2:
        wvar=*(unsigned short*)(prog+pos+2);
        if (buf[prog[pos+1]]!=NULL)
            LibMem(buf[prog[pos+1]]);
        buf[prog[pos+1]]=(char*)GetMem(wvar+1);
        buf[prog[pos+1]][wvar]=0;

        memcpy(buf[prog[pos+1]],prog+pos+4,wvar);
        lngbuf[prog[pos+1]]=wvar;
        pos+=wvar+4;
        break;

    case 4:
        cbuf[prog[pos+1]]=prog[pos+2];
        pos+=3;
        break;

    case 5:
        lbuf[prog[pos+1]]=*(unsigned long*)(prog+pos+2);
        pos+=6;
        break;

    case 7:
        fic=NULL;
        switch(prog[pos+1])
            {
            case 1:
                strcpy(file,KKFics->trash);
                Path2Abs(file,"font8x16.cfg");
                fic=fopen(file,"wb");
                break;
            }
        if (fic!=NULL)
            {
            fwrite(buf[prog[pos+2]],lngbuf[prog[pos+2]],1,fic);
            fclose(fic);
            pos+=3;
            }
            else
            {
            WinError("Erreur in macro");
            fct=6;
            }
        break;
    case 9:
        switch(prog[pos+2])
            {
            case  1: Cfg->TailleX=lbuf[prog[pos+1]];              break;
            case  2: Cfg->TailleY=lbuf[prog[pos+1]];              break;
            case  3: KKCfg->fentype=cbuf[prog[pos+1]];            break;
            case  4: Cfg->SaveSpeed=lbuf[prog[pos+1]];            break;
            case  5: Cfg->font=cbuf[prog[pos+1]];                 break;
            case  6: Cfg->display=cbuf[prog[pos+1]];              break;
            case  7: KKCfg->Esc2Close=cbuf[prog[pos+1]];          break;
            case  8: KKCfg->pathdown=cbuf[prog[pos+1]];           break;
            case  9: KKCfg->dispath=cbuf[prog[pos+1]];            break;
            case 10: KKCfg->sizewin=cbuf[prog[pos+1]];            break;
            case 11: KKCfg->isidf=cbuf[prog[pos+1]];              break;
            case 12: KKCfg->isbar=cbuf[prog[pos+1]];              break;
            case 13: KKCfg->dispcolor=cbuf[prog[pos+1]];          break;
            case 14: memcpy(Cfg->palette,buf[prog[pos+1]],48);    break;
            case 15: memcpy(Cfg->col,buf[prog[pos+1]],64);        break;
            case 16: KKCfg->userfont=cbuf[prog[pos+1]];           break;
            case 17: KKCfg->pntrep=cbuf[prog[pos+1]];             break;
            case 18: KKCfg->hidfil=cbuf[prog[pos+1]];             break;
            case 19: KKCfg->logfile=cbuf[prog[pos+1]];            break;
            case 20: KKCfg->autoreload=cbuf[prog[pos+1]];         break;
            case 21: KKCfg->verifhist=cbuf[prog[pos+1]];          break;
            case 22: KKCfg->palafter=cbuf[prog[pos+1]];           break;
            case 23: Cfg->debug=cbuf[prog[pos+1]];                break;
            case 24: KKCfg->currentdir=cbuf[prog[pos+1]];         break;
            case 25: Cfg->speedkey=cbuf[prog[pos+1]];             break;
            case 26: KKCfg->insdown=cbuf[prog[pos+1]];            break;
            case 27: KKCfg->seldir=cbuf[prog[pos+1]];             break;
            case 28: Cfg->comport=cbuf[prog[pos+1]];              break;
            case 29: KKCfg->enterkkd=cbuf[prog[pos+1]];           break;
            case 30: strcpy(KKCfg->editeur,buf[prog[pos+1]]);     break;
            case 31: strcpy(KKCfg->vieweur,buf[prog[pos+1]]);     break;
            case 32: strcpy(KKCfg->ssaver,buf[prog[pos+1]]);      break;
            case 33: Cfg->comspeed=lbuf[prog[pos+1]];             break;
            case 34: KKCfg->cnvhist=cbuf[prog[pos+1]];            break;
            case 35: KKCfg->esttime=cbuf[prog[pos+1]];            break;
            case 36: KKCfg->confexit=cbuf[prog[pos+1]];           break;
            case 37: KKCfg->savekey=cbuf[prog[pos+1]];            break;
            case 38: KKCfg->KeyAfterShell=cbuf[prog[pos+1]];      break;
            case 39: KKCfg->addselect=cbuf[prog[pos+1]];          break;
            case 40: KKCfg->mtrash=lbuf[prog[pos+1]];             break;
            case 41: Cfg->combit=cbuf[prog[pos+1]];               break;
            case 42: Cfg->comparity=cbuf[prog[pos+1]];            break;
            case 43: Cfg->comstop=cbuf[prog[pos+1]];              break;
            }
        pos+=3;
        break;
    }
}
while(fct!=6);

for(n=0;n<256;n++)
    if (buf[n]!=NULL)
        LibMem(buf[n]);

Info->macro=0;
}
