// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp0=68;
static char KKTmpX1=68;
static int KKTmpY1=2;
static char KKTmpX3=26;
static int KKTmpY3=7;
static int KKTmp10=26;
static char KKTmpX11=40;
static int KKTmpY11=2;
static int KKTmp13=40;
static char KKTmpX14=40;
static int KKTmpY14=2;

//--- User Field ---

static char KKField0[69];
static int KKField4;
static char KKField10[27];
static char KKField13[41];
static int KKField15;

struct TmtWin F = {-1,-1,72,17,"Search File(s)"};

struct Tmt T[] = {
    { 2, 3, 1,KKField0,&KKTmp0},
    { 1, 1, 9,&KKTmpX1,&KKTmpY1},
    { 3, 2, 0,"Filename",NULL},
    { 1, 5, 9,&KKTmpX3,&KKTmpY3},
    { 2, 6,10,"Current drive",&KKField4},
    { 2, 7,10,"Current dir & subdir",&KKField4},
    { 2, 8,10,"Selected dir & subdir",&KKField4},
    { 2, 9,10,"Only current directory",&KKField4},
    { 2,10,10,"All drive",&KKField4},
    { 2,11,10,"Selected drive",&KKField4},
    { 2,12, 1,KKField10,&KKTmp10},
    {29, 5, 9,&KKTmpX11,&KKTmpY11},
    {31, 6, 0,"Search String",NULL},
    {30, 7, 1,KKField13,&KKTmp13},
    {29,10, 9,&KKTmpX14,&KKTmpY14},
    {30,11, 8,"Search in KKD",&KKField15},
    {31,12, 0,"Ketchup Killers",NULL},
    {54,12, 0,"String Function",NULL},
    {11,14, 2,NULL,NULL},
    {47,14, 3,NULL,NULL}
   };

strcpy(KKField0,"");
KKField4=4;
strcpy(KKField10,"");
strcpy(KKField13,"");
KKField15=0;

res=WinTraite(T,20,&F,0);

strcpy(?,KKField0);
?=(KKField4-4);
strcpy(?,KKField10);
strcpy(?,KKField13);
?=KKField15;
