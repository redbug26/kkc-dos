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

int DOSlitfic(void);            //--- system 0 -------------------------

int RARlitfic(void);            //--- system 1 -------------------------

int ARJlitfic(void);            //--- system 2 -------------------------

int ZIPlitfic(void);            //--- system 3 -------------------------

int LHAlitfic(void);            //--- system 4 -------------------------

int KKDlitfic(void);            //--- system 5 -------------------------

int DFPlitfic(void);            //--- system 6 -------------------------

int Hostlitfic(void);           //--- system 7 -------------------------

int RAWlitfic(void);            //--- system 8 -------------------------

int DKFlitfic(void);            //--- system 9 -------------------------

void MakeKKD(FENETRE *Fen,char *ficname);


