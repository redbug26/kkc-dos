/*--------------------------------------------------------------------*\
|- KK fonction                                                        -|
\*--------------------------------------------------------------------*/
#include <mem.h>
#include <stdlib.h>       //--- Pour getenv ----------------------------
#include <stdio.h>

#include "kk.h"

static char buffer[256];


void DefaultKKCfg(void)
{
short Nm[]={72,68,42,40,3,63};

KKCfg->sizewin=0;

KKCfg->addselect=0;

KKCfg->KeyAfterShell=0;

KKCfg->savekey=0;           //--- Vaut 0 par defaut --------------------

memcpy(KKCfg->Qmenu,"ChgDrive"
                    "  Swap  "
                    "Go Trash"
                    "QuickDir"
                    " Select "
                    "  Info  ",48);

memcpy(KKCfg->Nmenu,&Nm,8*sizeof(short));

KKCfg->scrrest=1;

KKCfg->confexit=1;

KKCfg->dispath=0;
KKCfg->pathdown=0;

strcpy(KKCfg->V.Mask[0]->title,"C Style");
strcpy(KKCfg->V.Mask[0]->chaine,
                             "asm break case cdecl char const continue "
                         "default do double else enum extern far float "
                             "for goto huge if int interrupt long near "
                           "pascal register short signed sizeof static "
                            "struct switch typedef union unsigned void "
                                                    "volatile while @");
KKCfg->V.Mask[0]->Ignore_Case=0;
KKCfg->V.Mask[0]->Other_Col=1;

strcpy(KKCfg->V.Mask[1]->title,"Pascal Style");
strcpy(KKCfg->V.Mask[1]->chaine,
                           "absolute and array begin case const div do "
                            "downto else end external file for forward "
                            "function goto if implementation in inline "
                          "interface interrupt label mod nil not of or "
                               "packed procedure program record repeat "
                           "set shl shr string then to type unit until "
                                           "uses var while with xor @");
KKCfg->V.Mask[1]->Ignore_Case=1;
KKCfg->V.Mask[1]->Other_Col=1;

strcpy(KKCfg->V.Mask[2]->title,"Assembler Style");
strcpy(KKCfg->V.Mask[2]->chaine,
                       "aaa aad aam aas adc add and arpl bound bsf bsr "
      "bswap bt btc btr bts call cbw cdq clc cld cli clts cmc cmp cmps "
      "cmpxchg cwd cwde daa das dec div enter esc hlt idiv imul in inc "
     "ins int into invd invlpg iret iretd jcxz jecxz jmp ja jae jb jbe "
      "jc jcxz je jg jge jl jle jna jnae jnb jnbe jnc jne jng jnge jnl "
  "jnle jno jnp jns jnz jo jp jpe jpo js jz lahf lar lds lea leave les "
        "lfs lgdt lidt lgs lldt lmsw lock lods loop loope loopz loopnz "
  "loopne lsl lss ltr mov movs movsx movsz mul neg nop not or out outs "
  "pop popa popad push pusha pushad pushf pushfd rcl rcr rep repe repz "
   "repne repnz ret retf rol ror sahf sal shl sar sbb scas setae setnb "
       "setb setnae setbe setna sete setz setne setnz setl setng setge "
       "setnl setle setng setg setnle sets setns setc setnc seto setno "
     "setp setpe setnp setpo sgdt sidt shl shr shld shrd sldt smsw stc "
      "std sti stos str sub test verr verw wait fwait wbinvd xchg xlat "
                               "db dw dd endp ends assume xlatb xor @");

KKCfg->V.Mask[1]->Ignore_Case=1;
KKCfg->V.Mask[1]->Other_Col=1;

strcpy(KKCfg->V.Mask[15]->title,"Ketchup^Pulpe Style");
strcpy(KKCfg->V.Mask[15]->chaine,
                             "ketchup killers redbug access darköangel "
                      "off topy kennet typeöone pulpe tyby djamm vatin "
                   "marjorie katana ecstasy cray magicöfred cobra z @");
KKCfg->V.Mask[15]->Ignore_Case=1;
KKCfg->V.Mask[15]->Other_Col=1;

strcpy(KKCfg->extens,"RAR ARJ ZIP LHA DIZ EXE COM BAT BTM");


KKCfg->fentype=4;

KKCfg->mtrash=100000;

KKCfg->currentdir=1;
KKCfg->alcddir=0;
KKCfg->overflow1=0;
KKCfg->overflow2=0;

KKCfg->autoreload=1;
KKCfg->verifhist=0;

KKCfg->noprompt=0;

KKCfg->crc=0x69;

KKCfg->key=0;

KKCfg->dispcolor=1;

KKCfg->insdown=1;
KKCfg->seldir=1;

KKCfg->hidfil=1;

KKCfg->enterkkd=1;

KKCfg->palafter=1;

KKCfg->cnvhist=1;
KKCfg->esttime=1;

KKCfg->editeur[0]=0;
KKCfg->vieweur[0]=0;
KKCfg->ssaver[0]=0;

KKCfg->Esc2Close=0;

strcpy(KKCfg->ExtTxt,
                 "ASM BAS C CPP DIZ DOC H HLP HTM INI LOG NFO PAS TXT");
KKCfg->Enable_Txt=1;
strcpy(KKCfg->ExtBmp,
             "BMP GIF ICO JPG LBM PCX PIC PKM PNG RAW TGA TIF WMF WPG");
KKCfg->Enable_Bmp=1;
strcpy(KKCfg->ExtSnd,"IT IFF MID MOD MTM S3M VOC WAV XM RTM MXM");
KKCfg->Enable_Snd=1;
strcpy(KKCfg->ExtArc,"ARJ LHA RAR ZIP KKD DFP");
KKCfg->Enable_Arc=1;
strcpy(KKCfg->ExtExe,"BAT BTM COM EXE PRG");
KKCfg->Enable_Exe=1;
strcpy(KKCfg->ExtUsr,"XYZ");
KKCfg->Enable_Usr=1;

strcpy(KKCfg->HistDir,"C:\\");

strcpy(KKCfg->HistCom,"!.!");

KKCfg->V.warp=1;
KKCfg->V.cnvtable=0;                                    // Table Ketchup
KKCfg->V.autotrad=0;
KKCfg->V.ajustview=1;
KKCfg->V.saveviewpos=1;
KKCfg->V.wmask=15;                                  // RedBug preference

KKCfg->V.lnfeed=4;                                              // CR/LF
KKCfg->V.userfeed=0xE3;

KKCfg->V.AnsiSpeed=133;

NbrFunct=0;
}

void FileSetup(void)
{
char buffer[256];

static char Edit[64],View[64],SSaver[64];
static int DirLength=63;


struct Tmt T[10] = {
      { 8,3,1,View,&DirLength},
      { 8,5,1,Edit,&DirLength},
      { 8,7,1,SSaver,&DirLength},
      { 1,3,0, "Viewer:",NULL},
      { 1,5,0, "Editor:",NULL},
      { 1,7,0, "SSaver:",NULL},
      
      { 3,9,5," Auto Editor ",NULL},
      {18,9,5," Auto SSaver ",NULL},

      {3,12,2,NULL,NULL},                                       // le OK
      {18,12,3,NULL,NULL}                                   // le CANCEL
      };

struct TmtWin F = {-1,5,74,20,"File Setup"};

int n;
char fin;

strcpy(Edit,KKCfg->editeur);
strcpy(View,KKCfg->vieweur);
strcpy(SSaver,KKCfg->ssaver);

do
{
fin=1;

n=WinTraite(T,10,&F,0);

if (n==-1) return;                                             // ESCape
if (T[n].type==3) return;                                      // Cancel

if (n==6)
    {
    static char dest[256];
    static int i;

    i=FicIdf(dest,buffer,91,2);

    switch(i)
        {
        case 0:
            strcpy(Edit,buffer);
            break;
        case 1:
            WinError("Run Main Setup before");
            break;  //--- error ----------------------------------------
        case 2:
            WinError("No editor found");
            break;  //--- no player ------------------------------------
        case 3:
            break; //--- Escape ----------------------------------------
        }
    fin=0;
    }
if (n==7)
    {
    static char dest[256];
    static int i;

    i=FicIdf(dest,buffer,143,2);

    switch(i)
        {
        case 0:
            strcpy(SSaver,buffer);
            break;
        case 1:
            WinError("Run Main Setup before");
            break;  //--- error ----------------------------------------
        case 2:
            WinError("No screen saver found");
            break;  //--- no player ------------------------------------
        case 3:
            break; //--- Escape ----------------------------------------
        }
    fin=0;
    }
}
while(!fin);

strcpy(KKCfg->editeur,Edit);
strcpy(KKCfg->vieweur,View);
strcpy(KKCfg->ssaver,SSaver);
}

void SetDefaultKKPath(char *path)
{
char *Trash;

Trash=getenv("KKUSER");
if (Trash==NULL) Trash="trash";

KKFics->trash=(char*)GetMem(256);
strcpy(KKFics->trash,path);
Path2Abs(KKFics->trash,Trash);                       // repertoire trash

KKFics->FicIdfFile=(char*)GetMem(256);
strcpy(KKFics->FicIdfFile,KKFics->trash);
Path2Abs(KKFics->FicIdfFile,"idfext.rb");

KKFics->CfgFile=(char*)GetMem(256);
strcpy(KKFics->CfgFile,KKFics->trash);
Path2Abs(KKFics->CfgFile,"kkrb.cfg");

KKFics->temp=(char*)GetMem(256);
strcpy(KKFics->temp,KKFics->trash);
Path2Abs(KKFics->temp,"kktemp.tmp");

KKFics->log=(char*)GetMem(256);
strcpy(KKFics->log,KKFics->trash);
Path2Abs(KKFics->log,"logfile");                        // logfile trash

KKFics->menu=(char*)GetMem(256);
strcpy(KKFics->menu,KKFics->trash);
Path2Abs(KKFics->menu,"kkc.mnu");

KKFics->ficscreen=(char*)GetMem(256);
strcpy(KKFics->ficscreen,KKFics->trash);
Path2Abs(KKFics->ficscreen,"kkc.scr");

KKCfg->V.viewhist=(char*)GetMem(256);
strcpy(KKCfg->V.viewhist,KKFics->trash);
Path2Abs(KKCfg->V.viewhist,"kkview.rb");
}


/*--------------------------------------------------------------------*\
|- Remplis le vide si on est en mode 90 colonnes                      -|
\*--------------------------------------------------------------------*/
void RemplisVide(void)
{
if (Cfg->TailleX==80) return;

Cadre(80,0,Cfg->TailleX-1,2,3,Cfg->col[55],Cfg->col[56]);
PrintAt(81,1,"%*s",Cfg->TailleX-82,"KetchupK");

Cadre(80,3,Cfg->TailleX-1,Cfg->TailleY-2,2,Cfg->col[55],Cfg->col[56]);
Window(81,4,Cfg->TailleX-2,Cfg->TailleY-3,Cfg->col[16]);

AffChr(Cfg->TailleX-2,Cfg->TailleY-3,3);
}

void Console(void)
{
FILE *fic;
int n;

SaveScreen();

strcpy(buffer,KKFics->trash);
Path2Abs(buffer,"console.log");

fic=fopen(buffer,"wt");

for(n=1;n<14;n++)
    {
    if (n>1)
        Window(1,1,Cfg->TailleX-2,n-1,Cfg->col[10]);
    Cadre(0,0,Cfg->TailleX-1,n,3,Cfg->col[9],Cfg->col[41]);
    Pause(1);
    }

Wait(0,0);
LoadScreen();
}
