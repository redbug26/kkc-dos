#define ENTIER short

// Header of hard-function
void TXTMode(char lig);         // Nombre de ligne

void LoadPal(void);
void NoFlash(void);


char GetChr(short x,short y);
char GetCol(short x,short y);
void AffChr(short x,short y,short c);
void AffCol(short x,short y,short c);
void Clr(void);
void ColLin(int left,int top,int length,short color);
void ChrLin(int left,int top,int length,short color);
void ChrCol(int left,int top,int length,short color);
void ColWin(int right,int top,int left,int bottom,short color);
void ChrWin(int right,int top,int left,int bottom,short color);
void GotoXY(char,char);
void Pause(int n);
void MoveText(int x1,int y1,int x2,int y2,int x3,int y4);
void PrintAt(int x,int y,char *string,...);
char InputAt(char colonne,char ligne,char *chaine, int longueur);
int Wait(int x,int y,char c);

void SaveEcran(void);
void ChargeEcran(void);

void Font(void);
void *GetMem(int);

void WinCadre(int x1,int y1,int x2,int y2,int type);
void WinLine(int x1,int y1,int xl,int type);

void SetPal(char x,char r,char g,char b);

int crc32file(char *name,unsigned long *crc);   // Compute CRC-32 of file


struct config {
     int _4dos;           // equal 1 if 4DOS found

     char HistDir[256];   // History of disk
     ENTIER fentype;        // Type de fenˆtre, 1=NC, 2=WATCOM, 3=KKC, 4=Font
     ENTIER TailleY;        // Nombre de caratctere verticalement

     ENTIER colnor;
     ENTIER bkcol;
     ENTIER inscol;

     ENTIER FenTyp[2];      // Type des fenˆtres SHELL
     ENTIER KeyAfterShell;  // Vaut 1 si wait key after dosshell
     ENTIER UseFont;        // Type de Font (0:normal, 1:8x8)
     char Tfont[33];          // Caracteres employ‚s pour les fenˆtres

     char palette[48];      // The PALETTE
     };

struct fichier {
    char *FicIdfFile;      // idfext.rb
    char *CfgFile;         // kkrb.cfg
    char *view;            // view
    char *edit;            // edit
    char *font;            // font.cfg
    char *help;            // kkc.hlp
    char *temp;            // temp.tmp
    char *trash;           // repertoire trash
    };

extern struct config *Cfg;
extern struct fichier *Fics;

#define HI(qsd) (qsd/256)
#define LO(qsd) (qsd%256)



