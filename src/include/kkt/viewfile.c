// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=64;
static char KKTmpX5=64;
static int KKTmpY5=2;

//--- User Field ---

static char KKField0[65];

struct TmtWin F = {-1,-1,68,8,"View File"};

struct Tmt T[] = {
    { 2, 3, 1,KKField0,&KKTmp0},
    { 2, 5, 5," Intern.View",NULL},
    {17, 5, 5," Text Viewer",NULL},
    {32, 5, 5," Extern.View",NULL},
    {52, 5, 3,NULL,NULL},
    { 1, 1, 9,&KKTmpX5,&KKTmpY5},
    { 3, 2, 0,"Do you want to view this file ?",NULL}
   };

strcpy(KKField0,"");

res=WinTraite(T,7,&F,0);

strcpy(?,KKField0);
