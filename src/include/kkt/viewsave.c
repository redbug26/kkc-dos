// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=70;
static char KKTmpX3=70;
static int KKTmpY3=2;

//--- User Field ---

static char KKField0[71];

struct TmtWin F = {-1,-1,74,8,"Save File"};

struct Tmt T[] = {
    { 2, 3, 1,KKField0,&KKTmp0},
    {12, 5, 5,"     SAVE",NULL},
    {49, 5, 3,NULL,NULL},
    { 1, 1, 9,&KKTmpX3,&KKTmpY3},
    { 3, 2, 0,"Filename to save",NULL}
   };

strcpy(KKField0,"");

res=WinTraite(T,5,&F,0);

strcpy(?,KKField0);
