/*--------------------------------------------------------------------*\
|- Gestion des ressources DOS - source                                -|
\*--------------------------------------------------------------------*/

#include <dos.h>

#include "rbdos.h"


/*--------------------------------------------------------------------*\
|- Renvoit le nombre d'octets libres sur l'unite drive                -|
|-      1 -> 'a:'                                                     -|
\*--------------------------------------------------------------------*/
long GetDiskFree(char drive)
{
union REGS R;
long l;
unsigned short ax,bx,cx;

R.h.ah=0x36;
R.h.dl=drive;

int386(0x21,&R,&R);

ax=(unsigned short)R.w.ax;
bx=(unsigned short)R.w.bx;
cx=(unsigned short)R.w.cx;

l=((unsigned long)ax)*((unsigned long)bx)*((unsigned long)cx);

return l;
}


