

void Font(void)
{
FILE *fic;
char *pol;

union REGS R;

fic=fopen("font.cfg","rb");
if (fic==NULL) return;

pol=GetMem(2048);


R.h.al=4;
R.h.ah=0x11;
R.h.bl=0;
int386(0x10,&R,&R);

R.h.ah=0x11;
R.h.al=0;
R.h.bh=16;
R.h.bl=0;
R.x.cx=2;           // nombre de
R.x.dx=253;         // a partir de
R.x.bp=pol

int386(0x10,&R,&R);
}

