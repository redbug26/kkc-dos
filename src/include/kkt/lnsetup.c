// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static char KKTmpX11=50;
static int KKTmpY11=3;
static char KKTmpX12=22;
static int KKTmpY12=2;

//--- User Field ---

static int KKField0;
static int KKField1;
static int KKField2;
static int KKField3;
static int KKField4;
static int KKField5;

struct TmtWin F = {-1,-1,54,12,"Longname Setup"};

struct Tmt T[] = {
    {28, 2, 8,"Read / Write",&KKField0},
    {28, 3, 8,"Read",&KKField1},
    {40, 3, 8,"Write",&KKField2},
    {28, 4, 8,"Read",&KKField3},
    { 3, 7, 8,"Longname support",&KKField4},
    { 3, 8, 8,"Extended display",&KKField5},
    {25, 7, 2,NULL,NULL},
    {39, 7, 3,NULL,NULL},
    { 3, 2, 0,"Windows95 interrupt",NULL},
    { 3, 3, 0,"TRANS.TBL",NULL},
    { 3, 4, 0,"Direct Access",NULL},
    { 1, 1, 9,&KKTmpX11,&KKTmpY11},
    { 1, 6, 9,&KKTmpX12,&KKTmpY12}
   };

KKField0=0;
KKField1=0;
KKField2=0;
KKField3=0;
KKField4=0;
KKField5=0;

res=WinTraite(T,13,&F,0);

?=KKField0;
?=KKField1;
?=KKField2;
?=KKField3;
?=KKField4;
?=KKField5;
