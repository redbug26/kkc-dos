// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=32;
static char KKTmpX1=32;
static int KKTmpY1=2;

//--- User Field ---

static char KKField0[33];

struct TmtWin F = {-1,-1,36,8,"Selection of files"};

struct Tmt T[] = {
    { 2, 3, 1,KKField0,&KKTmp0},
    { 1, 1, 9,&KKTmpX1,&KKTmpY1},
    { 3, 2, 0,"File mask:",NULL},
    { 2, 5, 2,NULL,NULL},
    {20, 5, 3,NULL,NULL}
   };

strcpy(KKField0,"");

res=WinTraite(T,5,&F,0);

strcpy(?,KKField0);
