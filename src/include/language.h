#ifndef RB_LANG

#define RB_LANG

#define NBRFUNCT 113

typedef struct __toto
    {
    int nbr;
    char active;
    char bar[7];
    char *info;
    int kkcver;
    int kkpver;
    } INFOFCT;


extern INFOFCT InfoFct[NBRFUNCT];

extern char _BarHeader[][10];

/*--------------------------------------------------------------------*\
|- Label in pannel                                                    -|
\*--------------------------------------------------------------------*/

extern char _PannelHeader[][20];

extern char _OneFilHeader[];
extern char _MulFilHeader[];
extern char _OneSelHeader[];
extern char _MulSelHeader[];

extern char _LabelSubDir[];
extern char _LabelVolume[];
extern char _LabelReload[];
extern char _LabelFunctn[];
extern char _LabelUpdire[];

extern char _LabelCdrom[];
extern char _LabelDrive[];
extern char _LabelUnsel[];
extern char _LabelUnkno[];

extern char _LabelSel[];

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/



#endif
