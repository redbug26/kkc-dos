// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=72;
static char KKTmpX4=72;
static int KKTmpY4=2;

//--- User Field ---

static char KKField0[73];

struct TmtWin F = {-1,-1,76,8,"Text Capture"};

struct Tmt T[] = {
    { 2, 3, 1,KKField0,&KKTmp0},
    { 2, 5, 5,"  Overwrite",NULL},
    {17, 5, 5,"    Append",NULL},
    {60, 5, 3,NULL,NULL},
    { 1, 1, 9,&KKTmpX4,&KKTmpY4},
    { 2, 2, 0,"Destination file",NULL}
   };

strcpy(KKField0,"");

res=WinTraite(T,6,&F,0);

strcpy(?,KKField0);
