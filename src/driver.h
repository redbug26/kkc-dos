/*--------------------------------------------------------------------*\
|- prototype de gestion de lecture des fichiers dans les directories  -|
\*--------------------------------------------------------------------*/

struct find_rb
    {
    int time;
    int date;
    char attrib;
    int size;
    char name[256];
    };

#define RB_ARCHIVE  0x20
#define RB_SUBDIR   0x10
#define RB_VOLID    0x08
#define RB_SYSTEM   0x04
#define RB_HIDDEN   0x02
#define RB_RDONLY   0x01

int DOSlitfic(void);            //--- system 0 -------------------------

int RARlitfic(void);            //--- system 1 -------------------------

int ARJlitfic(void);            //--- system 2 -------------------------

int ZIPlitfic(void);            //--- system 3 -------------------------

int LHAlitfic(void);            //--- system 4 -------------------------

int KKDlitfic(void);            //--- system 5 -------------------------

void MakeKKD(FENETRE *Fen,char *ficname);


