// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=15;
static int KKTmp1=15;
static int KKTmp2=8;
static int KKTmp4=16;
static int KKTmp5=16;
static int KKTmp6=32;
static char KKTmpX16=47;
static int KKTmpY16=7;

//--- User Field ---

static char KKField0[16];
static char KKField1[16];
static char KKField2[9];
static char KKField4[17];
static char KKField5[17];
static char KKField6[33];

struct TmtWin F = {-1,-1,51,13,"Compo Information"};

struct Tmt T[] = {
    {16, 2, 1,KKField0,&KKTmp0},
    {16, 3, 1,KKField1,&KKTmp1},
    {16, 5, 1,KKField2,&KKTmp2},
    {35, 5, 5,"  From List",NULL},
    {16, 6, 1,KKField4,&KKTmp4},
    {16, 7, 1,KKField5,&KKTmp5},
    {16, 8, 1,KKField6,&KKTmp6},
    { 2,10, 5,"   No Disk",NULL},
    {20,10, 2,NULL,NULL},
    {35,10, 3,NULL,NULL},
    { 2, 2, 0,"Handle",NULL},
    { 2, 3, 0,"Groupe",NULL},
    { 2, 5, 0,"Compo",NULL},
    { 2, 6, 0,"Number",NULL},
    { 2, 7, 0,"Description",NULL},
    { 2, 8, 0,"Title",NULL},
    { 1, 1, 9,&KKTmpX16,&KKTmpY16}
   };

strcpy(KKField0,"");
strcpy(KKField1,"");
strcpy(KKField2,"");
strcpy(KKField4,"");
strcpy(KKField5,"");
strcpy(KKField6,"");

res=WinTraite(T,17,&F,0);

strcpy(?,KKField0);
strcpy(?,KKField1);
strcpy(?,KKField2);
strcpy(?,KKField4);
strcpy(?,KKField5);
strcpy(?,KKField6);
