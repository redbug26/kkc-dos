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
RB_IDF Info;

strcpy(Info.path,argv[1]);

Traitefic(&Info);

printf("%s ", Info.fullname);
if (Info.composer[0] != 0) {
     printf("by %s ", Info.composer);
     }
printf("(%s)\n", Info.format);
}
