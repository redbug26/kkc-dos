// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=48;
static int KKTmp1=48;
static int KKTmp3=48;
static char KKTmpX7=48;
static int KKTmpY7=2;
static char KKTmpX9=48;
static int KKTmpY9=2;
static char KKTmpX11=48;
static int KKTmpY11=2;

//--- User Field ---

static char KKField0[49];
static char KKField1[49];
static char KKField3[49];

struct TmtWin F = {-1,-1,66,16,"File Setup"};

struct Tmt T[] = {
    { 2, 3, 1,KKField0,&KKTmp0},
    { 2, 7, 1,KKField1,&KKTmp1},
    {51, 7, 5," Auto Editor",NULL},
    { 2,11, 1,KKField3,&KKTmp3},
    {51,11, 5," Auto SSaver",NULL},
    { 2,13, 2,NULL,NULL},
    {20,13, 3,NULL,NULL},
    { 1, 1, 9,&KKTmpX7,&KKTmpY7},
    { 3, 2, 0,"Viewer:",NULL},
    { 1, 5, 9,&KKTmpX9,&KKTmpY9},
    { 3, 6, 0,"Editor:",NULL},
    { 1, 9, 9,&KKTmpX11,&KKTmpY11},
    { 2,10, 0," Screen Saver:",NULL}
   };

strcpy(KKField0,"");
strcpy(KKField1,"");
strcpy(KKField3,"");

res=WinTraite(T,13,&F,0);

strcpy(?,KKField0);
strcpy(?,KKField1);
strcpy(?,KKField3);
