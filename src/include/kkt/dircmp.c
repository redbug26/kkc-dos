// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static char KKTmpX7=26;
static int KKTmpY7=2;
static char KKTmpX8=25;
static int KKTmpY8=3;

//--- User Field ---

static int KKField2;
static int KKField3;
static int KKField4;
static int KKField5;

struct TmtWin F = {-1,-1,57,9,"Compare Directories"};

struct Tmt T[] = {
    { 2, 6, 2,NULL,NULL},
    {41, 6, 3,NULL,NULL},
    { 3, 2, 8,"Compare Size",&KKField2},
    { 3, 3, 8,"Compare Date & Time",&KKField3},
    { 3, 4, 8,"Compare Attribut",&KKField4},
    {30, 2,10,"Select files",&KKField5},
    {30, 3,10,"Unselect files",&KKField5},
    {28, 1, 9,&KKTmpX7,&KKTmpY7},
    { 1, 1, 9,&KKTmpX8,&KKTmpY8}
   };

KKField2=0;
KKField3=0;
KKField4=0;
KKField5=5;

res=WinTraite(T,9,&F,0);

?=KKField2;
?=KKField3;
?=KKField4;
?=(KKField5-5);
