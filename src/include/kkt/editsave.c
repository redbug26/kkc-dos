// Created by Dialog Box Constructor/RedBug

int res;

//--- Internal Variable ---

static char KKTmpX3=48;
static int KKTmpY3=1;

//--- User Field ---


struct TmtWin F = {-1,-1,53,7,"Save confirmation"};

struct Tmt T[] = {
    { 2, 4, 5,"     YES",NULL},
    {17, 4, 5,"      NO",NULL},
    {38, 4, 3,NULL,NULL},
    { 2, 1, 9,&KKTmpX3,&KKTmpY3},
    { 4, 2, 0,"Do you want to save this file ?",NULL}
   };


res=WinTraite(T,5,&F,0);

