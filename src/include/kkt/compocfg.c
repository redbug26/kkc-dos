// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=32;
static int KKTmp1=8;
static char KKTmpX6=46;
static int KKTmpY6=2;

//--- User Field ---

static char KKField0[33];
static char KKField1[9];

struct TmtWin F = {-1,-1,50,8,"Compo Configuration"};

struct Tmt T[] = {
    {16, 2, 1,KKField0,&KKTmp0},
    {16, 3, 1,KKField1,&KKTmp1},
    { 3, 5, 2,NULL,NULL},
    {33, 5, 3,NULL,NULL},
    { 2, 2, 0,"Compo Name:",NULL},
    { 2, 3, 0,"Directory:",NULL},
    { 1, 1, 9,&KKTmpX6,&KKTmpY6}
   };

strcpy(KKField0,"");
strcpy(KKField1,"");

res=WinTraite(T,7,&F,0);

strcpy(?,KKField0);
strcpy(?,KKField1);
