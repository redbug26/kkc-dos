// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static int KKTmp1=72;
static char KKTmpX5=72;
static int KKTmpY5=2;

//--- User Field ---

static char KKField1[73];

struct TmtWin F = {-1,-1,76,11,"Text Capture"};

struct Tmt T[] = {
    {60, 2, 5,"     GO",NULL},
    { 2, 6, 1,KKField1,&KKTmp1},
    { 2, 8, 5,"  Overwrite",NULL},
    {17, 8, 5,"    Append",NULL},
    {60, 8, 3,NULL,NULL},
    { 1, 4, 9,&KKTmpX5,&KKTmpY5},
    { 2, 5, 0,"Destination file",NULL},
    { 2, 2, 0,"Copy in Windows Clipboard -->",NULL}
   };

strcpy(KKField1,"");

res=WinTraite(T,8,&F,0);

strcpy(?,KKField1);
