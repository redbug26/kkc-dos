// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=64;
static char KKTmpX4=64;
static int KKTmpY4=2;

//--- User Field ---

static char KKField0[65];

struct TmtWin F = {-1,-1,68,8,"Edit/Create File"};

struct Tmt T[] = {
    { 2, 3, 1,KKField0,&KKTmp0},
    { 2, 5, 5," Extern.Edit",NULL},
    {19, 5, 5," Intern.Edit",NULL},
    {52, 5, 3,NULL,NULL},
    { 1, 1, 9,&KKTmpX4,&KKTmpY4},
    { 3, 2, 0,"Do you want to save this file ?",NULL}
   };

strcpy(KKField0,"");

res=WinTraite(T,6,&F,0);

strcpy(?,KKField0);
