// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=64;
static int KKTmp1=32;
static char KKTmpX5=64;
static int KKTmpY5=4;

//--- User Field ---

static char KKField0[65];
static char KKField1[33];

struct TmtWin F = {-1,-1,68,10,"Advanced Copy"};

struct Tmt T[] = {
    { 2, 3, 1,KKField0,&KKTmp0},
    {34, 5, 1,KKField1,&KKTmp1},
    { 2, 7, 5,"    Link",NULL},
    {20, 7, 3,NULL,NULL},
    { 2, 5, 0,"Information:",NULL},
    { 1, 1, 9,&KKTmpX5,&KKTmpY5},
    { 2, 2, 0,"Destination file:",NULL}
   };

strcpy(KKField0,"");
strcpy(KKField1,"");

res=WinTraite(T,7,&F,0);

strcpy(?,KKField0);
strcpy(?,KKField1);
