// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=32;
static char KKTmpX4=32;
static int KKTmpY4=1;

//--- User Field ---

static char KKField0[33];

struct TmtWin F = {-1,-1,47,7,"Select/Unselect"};

struct Tmt T[] = {
    {13, 2, 1,KKField0,&KKTmp0},
    { 2, 4, 5,"    Select",NULL},
    {17, 4, 5,"   Unselect",NULL},
    {32, 4, 3,NULL,NULL},
    {12, 1, 9,&KKTmpX4,&KKTmpY4},
    { 2, 2, 0,"Selection:",NULL}
   };

strcpy(KKField0,"");

res=WinTraite(T,6,&F,0);

strcpy(?,KKField0);
