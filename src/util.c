#include <ctype.h>
#include <io.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <conio.h>
#include <dos.h>
#include <bios.h>   // Gestion clavier
#include <fcntl.h>
#include <time.h>

#include "win.h"
#include "kk.h"

#include "util.h"


static char buffer[256];

int WinAttrib(void)
{
static int sa,sh,sr,ss;

static char Day[3],Month[3],Year[5];
static char Hour[3],Minute[3],Second[3];

static int deux=2,quatre=4;

static char x2=26,x3=12;
static int y2=7,y3=3;


struct Tmt T[34] = {
      { 30, 4,10, "",&sa},
      { 38, 4,10, "",&sa},
      { 46, 4,10, "",&sa},
      { 30, 6,10, "",&sh},
      { 38, 6,10, "",&sh},
      { 46, 6,10, "",&sh},
      { 30, 8,10, "",&sr},
      { 38, 8,10, "",&sr},
      { 46, 8,10, "",&sr},
      { 30,10,10, "",&ss},
      { 38,10,10, "",&ss},
      { 46,10,10, "",&ss},

      { 35,13,11, Day,&deux},
      { 38,13,11, Month,&deux},
      { 41,13,11, Year,&quatre},
      { 35,15,11, Hour,&deux},
      { 38,15,11, Minute,&deux},
      { 41,15,11, Second,&deux},

      { 6,2,0,"Change Attribute        Set    Unset   Change",NULL},
      { 45,1,0,"Not",NULL},

      { 6, 4,0,"Archive",NULL},
      { 6, 6,0,"Hidden ",NULL},
      { 6, 8,0,"Read-only",NULL},
      { 6,10,0,"System",NULL},
      { 6,13,0,"Date",NULL},
      { 6,15,0,"Time",NULL},

      {37,13,0,"/",NULL},
      {40,13,0,"/",NULL},
      {37,15,0,":",NULL},
      {40,15,0,":",NULL},

      { 26,3,9,&x2,&y2},
      { 33,12,9,&x3,&y3},

      {10,17,2,NULL,NULL},                // 1:Ok
      {35,17,3,NULL,NULL},
      };

struct TmtWin F =   {   8,4,69,23, "Change attribute/time"};

int n,m;
unsigned short datep,timep;
unsigned o;


sa=sr=sh=ss=-1;

strcpy(Day,"-");
strcpy(Month,"-");
strcpy(Year,"-");
strcpy(Hour,"-");
strcpy(Minute,"-");
strcpy(Second,"-");


for (n=0;n<DFen->nbrfic;n++)
    {
    if ( (DFen->F[n]->select==1) | ((DFen->nbrsel==0) & (n==DFen->pcur)) )
        {
        o=(DFen->F[n]->attrib);
        o=(o&0x20)!=0x20;

        if (sa==-1) sa=0+o;
        if (sa!=o) sa=2;

        o=(DFen->F[n]->attrib);
        o=(o&0x2)!=0x2;

        if (sh==-1) sh=3+o;
        if (sh!=3+o) sh=5;

        o=(DFen->F[n]->attrib);
        o=(o&0x1)!=0x1;

        if (sr==-1) sr=6+o;
        if (sr!=6+o) sr=8;

        o=(DFen->F[n]->attrib);
        o=(o&0x4)!=0x4;

        if (ss==-1) ss=9+o;
        if (ss!=9+o) ss=11;

        sprintf(buffer,"%2d",(DFen->F[n]->date)&31);
        if (!strcmp(Day,"-"))        strcpy(Day,buffer);
        if (strcmp(Day,buffer)!=0)   strcpy(Day,"");

        sprintf(buffer,"%2d",((DFen->F[n]->date)>>5)&15);
        if (!strcmp(Month,"-"))        strcpy(Month,buffer);
        if (strcmp(Month,buffer)!=0)   strcpy(Month,"");

        sprintf(buffer,"%4d",((DFen->F[n]->date)>>9)+1980);
        if (!strcmp(Year,"-"))        strcpy(Year,buffer);
        if (strcmp(Year,buffer)!=0)   strcpy(Year,"");

        sprintf(buffer,"%02d",((DFen->F[n]->time)>>11)&31);
        if (!strcmp(Hour,"-"))        strcpy(Hour,buffer);
        if (strcmp(Hour,buffer)!=0)   strcpy(Hour,"");

        sprintf(buffer,"%02d",((DFen->F[n]->time)>>5)&63);
        if (!strcmp(Minute,"-"))        strcpy(Minute,buffer);
        if (strcmp(Minute,buffer)!=0)   strcpy(Minute,"");

        sprintf(buffer,"%02d",((DFen->F[n]->time)&31)*2);
        if (!strcmp(Second,"-"))        strcpy(Second,buffer);
        if (strcmp(Second,buffer)!=0)   strcpy(Second,"");
        }
    }


n=WinTraite(T,34,&F);

if (n!=27)  // pas escape
    {
    if (T[n].type!=3)    // pas cancel
        for (n=0;n<DFen->nbrfic;n++)
            {
            if ( (DFen->F[n]->select==1) | ((DFen->nbrsel==0) & (n==DFen->pcur)) )
                {
                int hand2;

                strcpy(buffer,DFen->path);
                Path2Abs(buffer,DFen->F[n]->name);

                o=0;
                if (sa!=2)  o=0x20*(1-sa);
                if (sa!=5)  o+=+0x2*(4-sh);
                if (sr!=8)  o+=0x1*(7-sr);
                if (ss!=11) o+=0x4*(10-ss);

                _dos_setfileattr(buffer,o);
                _dos_getfileattr(buffer,&o);
                DFen->F[n]->attrib=o;

                _dos_open(buffer,O_RDONLY,&hand2);
                _dos_getftime(hand2,&datep,&timep);

                if (*Day!=0)
                    {
                    sscanf(Day,"%d",&m);
                    datep=(datep&65504)+m;
                    }
                if (*Month!=0)
                    {
                    sscanf(Month,"%d",&m);
                    datep=(datep&65055)+(m<<5);
                    }
                if (*Year!=0)
                    {
                    sscanf(Year,"%d",&m);
                    datep=(datep&511)+((m-1980)<<9);
                    }
                if (*Hour!=0)
                    {
                    sscanf(Hour,"%d",&m);
                    timep=(timep&2047)+(m<<11);
                    }
                if (*Minute!=0)
                    {
                    sscanf(Minute,"%d",&m);
                    timep=(timep&63519)+(m<<5);
                    }
                if (*Second!=0)
                    {
                    sscanf(Second,"%d",&m);
                    timep=(timep&65504)+(m/2);
                    }

                _dos_setftime(hand2,datep,timep);
                _dos_close(hand2);

                _dos_open(buffer,O_RDONLY,&hand2);
                _dos_getftime(hand2,&datep,&timep);
                _dos_close(hand2);

                DFen->F[n]->date=datep;
                DFen->F[n]->time=timep;
                }

            }
    }

return 1;       // Erreur
}




