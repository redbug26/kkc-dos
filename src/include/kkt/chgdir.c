// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=64;
static char KKTmpX5=64;
static int KKTmpY5=2;

//--- User Field ---

static char KKField0[65];

struct TmtWin F = {-1,-1,68,8,"Change Directory"};

struct Tmt T[] = {
    { 2, 3, 1,KKField0,&KKTmp0},
    { 2, 5, 2,NULL,NULL},
    {19, 5, 5,"  Normal CD",NULL},
    {35, 5, 5," Create Tree",NULL},
    {52, 5, 3,NULL,NULL},
    { 1, 1, 9,&KKTmpX5,&KKTmpY5},
    { 3, 2, 0,"Change to which directory:",NULL}
   };

strcpy(KKField0,"");

res=WinTraite(T,7,&F,0);

strcpy(?,KKField0);
