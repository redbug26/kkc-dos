#define ENTIER short

// Header of hard-function
void TXTMode(char lig);         // Nombre de ligne

void LoadPal(void);
void NoFlash(void);

void GetCur(char *x,char *y);
void PutCur(char x,char y);

int ScreenSaver(void);

void ScrollUp(void);
char GetChr(short x,short y);
char GetCol(short x,short y);
void AffChr(short x,short y,short c);
void AffCol(short x,short y,short c);
void Clr(void);
void ColLin(int left,int top,int length,short color);
void ChrLin(int left,int top,int length,short color);
void ChrCol(int left,int top,int length,short color);
void ColCol(int left,int top,int length,short color);
void ColWin(int right,int top,int left,int bottom,short color);
void ChrWin(int right,int top,int left,int bottom,short color);
void GotoXY(char,char);
void WhereXY(char*,char*);
void Pause(int n);
void MoveText(int x1,int y1,int x2,int y2,int x3,int y4);
void PrintAt(int x,int y,char *string,...);
char InputAt(char colonne,char ligne,char *chaine, int longueur);
int Wait(int x,int y,char c);

void SaveEcran(void);
void ChargeEcran(void);

void Font8x8(void);
void Font8x16(void);
void *GetMem(int);

void WinCadre(int x1,int y1,int x2,int y2,int type);
void WinLine(int x1,int y1,int xl,int type);

void SetPal(char x,char r,char g,char b);

int crc32file(char *name,unsigned long *crc);   // Compute CRC-32 of file

int WinTraite(struct Tmt *T,int nbr,struct TmtWin *F);
int WinError(char*);
int WinMesg(char *,char *);

int Gradue(int x,int y,int length,int from,int to,int total);

void DefaultCfg(void);

struct TmtWin {
     int x1,y1,x2,y2;
     char *name;
     };

struct Tmt {
     int x,y;       // position
     char type;
     char *str;
     int *entier;
     };

/* Tmt:
  Type: 0 --> Titre
        1 --> String
        2 --> OK OK OK
        3 --> CANCEL
        4 --> Cadre de 4 de hauteur
        5 --> Bouton personnalis‚
        6 --> Cadre de 3 de hauteur
        7 --> Entier (de 9 caracteres)
        8 --> Switch
        9 --> Cadre personnalise
        10 --> Switch Multiple

*/


struct config {
    // Selon user
    //-----------
     long SaveSpeed;        // Temps a attendre avant d'activer le screen saver
     long AnsiSpeed;

     ENTIER fentype;        // Type de fenˆtre, 1=NC, 2=WATCOM, 3=KKC, 4=Font
     ENTIER TailleY;        // Nombre de caratctere verticalement

     char palette[48];      // The PALETTE

     unsigned char wmask;   // C'est quel masque kon emploie ?

     char pntrep;           // vaut 1 si on affiche le repertoire "."
     char hidfil;           // vaut 1 si on affiche les fichiers caches

     char logfile;          // vaut 1 si on utilise un logfile
     char debug;            // vaut 1 si on est en mode DEBUG

     char autoreload;       // Reload automatiquement lorsque les deux fenˆtres sont les mˆmes
     char verifhist;        // Verify history at any loading of KK (CTRL-PGDN)
     char palafter;         // Load the palette only when configuration is ok
     char noprompt;         // Si x&1 vaut 1 alors on ne prompte pdt la copie

     char font;             // utilisation des fonts

     

     long strash;           // taille actuelle de la trash
    // Pas touche
    //-----------
     long mtrash;           // taille maximum de la trash
     long FenAct;           // Quelle fenˆtre est active ?
     int _4dos;             // equal 1 if 4DOS found

     char HistDir[256];     // History of disk
     char overflow;         // Vaut tjs 0 (pour overflow)

     char extens[39];       // extension qui viennent tout devant

     ENTIER colnor;
     ENTIER bkcol;
     ENTIER inscol;

     ENTIER FenTyp[2];      // Type des fenˆtres SHELL
     ENTIER KeyAfterShell;  // Vaut 1 si wait key after dosshell
     ENTIER UseFont;        // Type de Font (0:normal, 1:8x8)
     char Tfont[33];        // Caracteres employ‚s pour les fenˆtres

     short key;             // code touche a reutiliser
     char FileName[256];    // Nom du dernier fichier selectionne for F3 on arc.
     char crc;              // Vaut tjs 0x69 (genre de crc)
     };

struct fichier {
    char *FicIdfFile;      // idfext.rb
    char *CfgFile;         // kkrb.cfg
    char *view;            // view
    char *edit;            // edit
    char *path;            // path
    char *help;            // kkc.hlp
    char *temp;            // temp.tmp
    char *trash;           // repertoire trash
    char *log;             // logfile
    char *LastDir;         // lastdir
    };

struct PourMask
    {
    char Ignore_Case;   // 1 si on ignore la case
    char Other_Col;     // 1 si on colorie les autres noms
    char chaine[1024];       // chaine de comparaison EX: "asm break case @"
    char title[40];        // nom de ce masque
    };



extern struct config *Cfg;
extern struct fichier *Fics;
extern struct PourMask **Mask;

#define HI(qsd) (qsd/256)
#define LO(qsd) (qsd%256)

// Retourne 0 si tout va bene
int VerifyDisk(char c);  // 1='A'
int __far Error_handler(unsigned deverr,unsigned errcode,unsigned far *devhdr);
