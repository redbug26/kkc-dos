/*--------------------------------------------------------------------*\
|- Programme principal                                                -|
// Test avec:
//  find /d0/httpd-media/temp -type f -exec /home/miguel/kkc/idf {} \;
\*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>                               // For handling signal


#include "kk.h"

#include "idf.h"
#include "win.h"

#include "language.h"

#if defined(__WC32__) | defined(DJGPP)
#include <bios.h>
#include <io.h>     //--- Pour unlink ----------------------------------
#endif

int main(int argc,char **argv)
{
KKVIEW V;

int OldX,OldY;                         // To save the size of the screen
int n;

char *path;

if (argc != 2) {
    return 0;
}


/*--------------------------------------------------------------------*\
|-  Initialisation de l'ecran                                         -|
\*--------------------------------------------------------------------*/

Redinit();

InitScreen(0);                     // Initialise toutes les donn‚es HARD

OldX=GetScreenSizeX(); // A faire apres le SetMode ou le InitScreen
OldY=GetScreenSizeY();

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;                  // Initialisation de la taille ecran

//printf("FIN2(%d,%d)\n\n", Cfg->TailleX, Cfg->TailleY); exit(1);

/*--------------------------------------------------------------------*\
|-  Save the current path                                             -|
\*--------------------------------------------------------------------*/

path=(char*)GetMem(256);

strcpy(path,*argv);
for (n=strlen(path);n>0;n--) {
    if (path[n]=='\\') {
        path[n]=0;
        break;
        }
    }

/*--------------------------------------------------------------------*\
|- Path & File initialisation                                         -|
\*--------------------------------------------------------------------*/

SetDefaultPath(path);


/*
Fics->help=GetMem(256);
strcpy(Fics->help,path);
strcat(Fics->help,"\\test1.hlp");
*/


/*--------------------------------------------------------------------*\
|- Configuration loading                                              -|
\*--------------------------------------------------------------------*/

DefaultCfg(Cfg);

#ifndef LINUX
TXTMode();
LoadPal(Cfg->palette);
#endif

#ifndef NOFONT
InitFont();
#endif


V.Mask=(struct PourMask**)GetMem(sizeof(struct PourMask*)*16);
for (n=0;n<16;n++)
    V.Mask[n]=(struct PourMask*)GetMem(sizeof(struct PourMask));


V.Traduc[0]=10;
V.Traduc[1]=0;

strcpy(V.Mask[0]->title,"C Style");
strcpy(V.Mask[0]->chaine,
							 "asm break case cdecl char const continue "
						 "default do double else enum extern far float "
							 "for goto huge if int interrupt long near "
						   "pascal register short signed sizeof static "
							"struct switch typedef union unsigned void "
													"volatile while @");
V.Mask[0]->Ignore_Case=0;
V.Mask[0]->Other_Col=1;

strcpy(V.Mask[1]->title,"Pascal Style");
strcpy(V.Mask[1]->chaine,
						   "absolute and array begin case const div do "
							"downto else end external file for forward "
							"function goto if implementation in inline "
						  "interface interrupt label mod nil not of or "
							   "packed procedure program record repeat "
						   "set shl shr string then to type unit until "
										   "uses var while with xor @");
V.Mask[1]->Ignore_Case=1;
V.Mask[1]->Other_Col=1;

strcpy(V.Mask[2]->title,"Assembler Style");
strcpy(V.Mask[2]->chaine,
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

V.Mask[1]->Ignore_Case=1;
V.Mask[1]->Other_Col=1;

strcpy(V.Mask[15]->title,"Ketchup^Pulpe Style");
strcpy(V.Mask[15]->chaine,
				  " blackward ketchup killers redbug access darköangel "
					  "off topy kennet typeöone pulpe tyby djamm vatin "
				   "marjorie katana ecstasy cray magicöfred cobra z @");
V.Mask[15]->Ignore_Case=1;
V.Mask[15]->Other_Col=1;

V.warp=1;
V.cnvtable=0;									// Table Ketchup
V.autotrad=0;
V.ajustview=1;
V.saveviewpos=1;
V.wmask=15;									// RedBug preference

V.lnfeed=4;												// CR/LF
V.maskaccel=0;
V.userfeed=0xE3;

V.AnsiSpeed=133;

V.viewhist=(char*)GetMem(256);
strcpy(V.viewhist,"/home/miguel/.kkview.rb");
//ficidf.cc:Path2Abs(KKCfg->V.viewhist,"kkview.rb")

View(&V, argv[1], 0);

DesinitScreen();

Cfg->TailleX=OldX;
Cfg->TailleY=OldY;

#ifndef LINUX
TXTMode();
#endif

puts("That's all folk...");
}
