
struct find_rb {
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

#include "dos.h"

void InstallRAR(void);
int RARlitfic(void);

void InstallARJ(void);
int ARJlitfic(void);

void InstallZIP(void);
int ZIPlitfic(void);

void InstallLHA(void);
int LHAlitfic(void);

void InstallKKD(void);
int KKDlitfic(void);
void MakeKKD(struct fenetre *Fen,char *ficname);


