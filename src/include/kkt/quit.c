// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static char KKTmpX4=29;
static int KKTmpY4=1;

//--- User Field ---

static int KKField0;

struct TmtWin F = {-1,-1,33,9,"Quit KKC"};

struct Tmt T[] = {
    { 2, 4, 8,"Don't ask next time",&KKField0},
    { 2, 6, 5,"     Yes",NULL},
    {17, 6, 5,"      No",NULL},
    { 2, 2, 0,"Do you really want quit KKC ?",NULL},
    { 1, 1, 9,&KKTmpX4,&KKTmpY4}
   };

KKField0=0;

res=WinTraite(T,5,&F,0);

?=KKField0;
