/*--------------------------------------------------------------------*\
|- KK fonction                                                        -|
\*--------------------------------------------------------------------*/
#include <mem.h>

#include "kk.h"

void DefaultKKCfg(void)
{
short Nm[]={72,68,42,40,3,63};

KKCfg->KeyAfterShell=0;

memcpy(KKCfg->Qmenu,"ChgDrive"
                    "  Swap  "
                    "Go Trash"
                    "QuickDir"
                    " Select "
                    "  Info  ",48);

memcpy(KKCfg->Nmenu,&Nm,8*sizeof(short));


strcpy(Mask[0]->title,"C Style");
strcpy(Mask[0]->chaine,      "asm break case cdecl char const continue "
                         "default do double else enum extern far float "
                             "for goto huge if int interrupt long near "
                           "pascal register short signed sizeof static "
                            "struct switch typedef union unsigned void "
                                                    "volatile while @");
Mask[0]->Ignore_Case=0;
Mask[0]->Other_Col=1;

strcpy(Mask[1]->title,"Pascal Style");
strcpy(Mask[1]->chaine,    "absolute and array begin case const div do "
                            "downto else end external file for forward "
                            "function goto if implementation in inline "
                          "interface interrupt label mod nil not of or "
                               "packed procedure program record repeat "
                           "set shl shr string then to type unit until "
                                           "uses var while with xor @");
Mask[1]->Ignore_Case=1;
Mask[1]->Other_Col=1;

strcpy(Mask[2]->title,"Assembler Style");
strcpy(Mask[2]->chaine,"aaa aad aam aas adc add and arpl bound bsf bsr "
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

Mask[1]->Ignore_Case=1;
Mask[1]->Other_Col=1;

strcpy(Mask[15]->title,"Ketchup^Pulpe Style");
strcpy(Mask[15]->chaine,      "ketchup killers redbug access darköangel "
                   "off topy kennet typeöone pulpe "
                   "marjorie katana ecstasy cray magicöfred cobra z @");
Mask[15]->Ignore_Case=1;
Mask[15]->Other_Col=1;

strcpy(KKCfg->extens,"RAR ARJ ZIP LHA DIZ EXE COM BAT BTM");



KKCfg->fentype=4;

KKCfg->mtrash=100000;

KKCfg->currentdir=1;
KKCfg->overflow1=0;
KKCfg->overflow2=0;

KKCfg->autoreload=1;
KKCfg->verifhist=1;

KKCfg->noprompt=0;

KKCfg->crc=0x69;

KKCfg->key=0;

KKCfg->dispcolor=1;

KKCfg->insdown=1;
KKCfg->seldir=1;

KKCfg->hidfil=1;

KKCfg->enterkkd=1;

KKCfg->palafter=0;

KKCfg->cnvhist=1;
KKCfg->esttime=1;

KKCfg->editeur[0]=0;
KKCfg->vieweur[0]=0;

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

KKCfg->warp=1;
KKCfg->cnvtable=0;                                      // Table Ketchup
KKCfg->autotrad=0;
KKCfg->ajustview=1;
KKCfg->saveviewpos=1;
KKCfg->wmask=15;                                      // RedBug preference

KKCfg->AnsiSpeed=133;
}
