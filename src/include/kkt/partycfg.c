// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=32;
static int KKTmp1=32;
static int KKTmp2=32;

//--- User Field ---

static char KKField0[33];
static char KKField1[33];
static char KKField2[33];

struct TmtWin F = {-1,-1,54,12,"Compo Information"};

struct Tmt T[] = {
    {16, 2, 1,KKField0,&KKTmp0},
    {16, 3, 1,KKField1,&KKTmp1},
    {16, 4, 1,KKField2,&KKTmp2},
    { 4, 9, 5,"    Compo",NULL},
    {25, 9, 2,NULL,NULL},
    {39, 9, 3,NULL,NULL},
    { 2, 2, 0,"Release Path",NULL},
    { 2, 3, 0,"Compo Path",NULL},
    { 2, 4, 0,"Source",NULL}
   };

strcpy(KKField0,"");
strcpy(KKField1,"");
strcpy(KKField2,"");

res=WinTraite(T,9,&F,0);

strcpy(?,KKField0);
strcpy(?,KKField1);
strcpy(?,KKField2);
