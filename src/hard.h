/*--------------------------------------------------------------------*\
|-  Header of hard-function                                           -|
\*--------------------------------------------------------------------*/

#define BYTE unsigned char
#define WORD unsigned short
#define ULONG unsigned long


#define RBPALDEF   {43,37,30, 31,22,17,  0, 0, 0, 58,58,50, \
                    44,63,63, 63,63,21, 43,37,30,  0, 0, 0, \
                    63,63, 0, 63,63,63, 43,37,30, 63,20,20, \
                    20,40,20,  0,40,40,  0, 0, 0,  0, 0, 0}



extern void (*AffChr)(short x,short y,short c);
extern void (*AffCol)(short x,short y,short c);
extern int (*Wait)(int x,int y,char c);
extern int (*KbHit)(void);
extern void (*GotoXY)(char x,char y);
extern void (*WhereXY)(char *x,char *y);
extern void(*Window)(int left,int top,int right,int bottom,short color);

void TXTMode(void);
void LoadPal(void);
void InitFont(void);

void GetCur(char *x,char *y);
void PutCur(char x,char y);

int ScreenSaver(void);

void ScrollUp(void);

void Pause(int n);
void MoveText(int x1,int y1,int x2,int y2,int x3,int y4);

/*--------------------------------------------------------------------*\
|- Fonction absolue                                                   -|
\*--------------------------------------------------------------------*/
char GetChr(short x,short y);
char GetCol(short x,short y);

void ColLin(int left,int top,int length,short color);
void ChrLin(int left,int top,int length,short color);
void ChrCol(int left,int top,int length,short color);
void ColCol(int left,int top,int length,short color);
void ColWin(int right,int top,int left,int bottom,short color);
void ChrWin(int right,int top,int left,int bottom,short color);

void PrintAt(int x,int y,char *string,...);
char InputAt(char colonne,char ligne,char *chaine, int longueur);

/*--------------------------------------------------------------------*\
|- Fonction relative                                                  -|
\*--------------------------------------------------------------------*/
char GetRChr(short x,short y);
char GetRCol(short x,short y);

void ColRLin(int left,int top,int length,short color);
void ChrRLin(int left,int top,int length,short color);
void ChrRCol(int left,int top,int length,short color);
void ColRCol(int left,int top,int length,short color);
void ColRWin(int right,int top,int left,int bottom,short color);
void ChrRWin(int right,int top,int left,int bottom,short color);

void PrintTo(int x,int y,char *string,...);
char InputTo(char colonne,char ligne,char *chaine, int longueur);

void AffRChr(char x,char y,char c);
void AffRCol(char x,char y,char c);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void Delay(long ms);

void SaveEcran(void);
void ChargeEcran(void);

void Font8x(int height);
void *GetMem(int);      // Malloc avec mise … z‚ro
void *GetMemSZ(int);    // Malloc sans mise … z‚ro

void WinCadre(int x1,int y1,int x2,int y2,int type);
void WinLine(int x1,int y1,int xl,int type);

void SetPal(char x,char r,char g,char b);

int WinTraite(struct Tmt *T,int nbr,struct TmtWin *F);
int WinError(char*);
int WinMesg(char *,char *);

int Gradue(int x,int y,int length,int from,int to,int total);

void DefaultCfg(void);

void SetDefaultPath(char *path);   // Initialise les fichiers selon path

/*--------------------------------------------------------------------*\
|-        Gestion de la barre de menu                                 -|
\*--------------------------------------------------------------------*/

int BarMenu(struct barmenu *bar,int nbr,int *poscur,int *xp,int *yp);
int PannelMenu(struct barmenu *bar,int nbr,int *c,int *xp,int *yp);
struct barmenu
        {
        char titre[20];
        char help[80];
        short fct;
        };


struct TmtWin {
     int x1,y1,x2,y2;
     char *name;
     };

struct Tmt {
     int x,y;                                                // position
     char type;
     char *str;
     int *entier;
     };

/* Tmt:
  Type: 0 --> Title
        1 --> String
        2 -->   OK
        3 --> CANCEL
        4 --> Cadre de 4 de hauteur
        5 --> Bouton personnalis‚ (13 bytes length)
        6 --> Cadre de 3 de hauteur
        7 --> Entier (de 9 caracteres)
        8 --> Switch
        9 --> Cadre personnalise
        10 --> Switch Multiple

*/


struct config
     {
    // Selon user
    //-----------
     long SaveSpeed; // Temps a attendre avant d'activer le screen saver
     long AnsiSpeed;

     short fentype;    // Type de fenˆtre, 1=NC, 2=WATCOM, 3=KKC, 4=Font
     short TailleY;                 // Nombre de caractere verticalement
     short TailleX;               // Nombre de caractere horizontalement

     char palette[48];                                    // The PALETTE

     unsigned char wmask;             // C'est quel masque kon emploie ?

     char pntrep;              // vaut 1 si on affiche le repertoire "."
     char hidfil;            // vaut 1 si on affiche les fichiers caches

     char logfile;                    // vaut 1 si on utilise un logfile
     char debug;                       // vaut 1 si on est en mode DEBUG

     char autoreload;   // Reload auto. quand les 2 fen. sont identiques
     char verifhist;  // Verify history at any loading of KK (CTRL-PGDN)
     char palafter;    // Load the palette only when configuration is ok
     char noprompt;    // Si x&1 vaut 1 alors on ne prompte pdt la copie
     char currentdir;                   // Va dans le repertoire courant

     char font;                                 // utilisation des fonts
     char dispcolor;    // Highlight les fichiers suivant les extensions
     char speedkey;           // vaut 1 si on veut accelerer les touches

     char insdown;    // vaut 1 si on descent quand on appuie sur insert
     char seldir;     // vaut 1 si on selectionne les repertoires avec +

     long strash;                         // taille actuelle de la trash

     char display;                                   // type d'affichage

     char comport;                       // Numero du port serie (ex: 2)
     int comspeed;                    // Vitesse              (ex:19200)
     char combit;                         // bit                  (ex:8)
     char comparity;                    // parity               (ex:'N')
     char comstop;                        // Bit de stop          (ex=1)

     char enterkkd;           // entre dans les kkd pendant la recherche
     char warp;                    // 0: pas de warp, 1: word, 2: entier

     char cnvhist;          // 1: si on converit a chaque fois l'history
     char esttime;                 // estime le temps pendant la copie ?

     char editeur[64];               // ligne de commande pour l'editeur
     char vieweur[64];               // ligne de commande pour le viewer

     char ExtTxt[64],Enable_Txt;
     char ExtBmp[64],Enable_Bmp;
     char ExtSnd[64],Enable_Snd;
     char ExtArc[64],Enable_Arc;
     char ExtExe[64],Enable_Exe;
     char ExtUsr[64],Enable_Usr;

     char Qmenu[48];
     short Nmenu[8];

     char ajustview;

     char Esc2Close;   // vaut 1 si on doit fermer les fenˆtres avec ESC

    // Pas touche
    //-----------
     long mtrash;                          // taille maximum de la trash
     long FenAct;                         // Quelle fenˆtre est active ?
     char _4dos;                                // equal 1 if 4DOS found
     char _Win95;                                    // Support nom long

     char HistDir[256];                                // History of dir
     char overflow1;

     char HistCom[512];                            // History of command
     char overflow2;                       // Vaut tjs 0 (pour overflow)
     short posinhist;                  // Position in history of command

     char extens[39];              // extension qui viennent tout devant

     short FenTyp[3];                         // Type des fenˆtres SHELL
     short KeyAfterShell;           // Vaut 1 si wait key after dosshell
     short UseFont;                    // Type de Font (0:normal, 1:8x8)
     char Tfont;          // Caracteres employ‚s pour la ligne verticale

     short key;                              // code touche a reutiliser
     char FileName[256]; // Nom du dernier fichier select. for F3 on arc
     char crc;                           // Vaut tjs 0x69 (genre de crc)
     };

struct RB_info
     {
     int temps;          // Temps entre le main() et l'appel utilisateur
     };

struct fichier
     {
     char *FicIdfFile;                                      // idfext.rb
     char *CfgFile;                                          // kkrb.cfg
     char *path;                                                 // path
     char *help;                                              // kkc.hlp
     char *temp;                                             // temp.tmp
     char *trash;                                    // repertoire trash
     char *log;                                               // logfile
     char *LastDir;                                           // lastdir
     };

struct PourMask
     {
     char Ignore_Case;                         // 1 si on ignore la case
     char Other_Col;                  // 1 si on colorie les autres noms
     char chaine[1024];  // chaine de comparaison EX: "asm break case @"
     char title[40];                                 // nom de ce masque
     };

int InitScreen(int a);                      // Renvoit 1 si tout va bien
void DesinitScreen(void);

extern struct config *Cfg;
extern struct RB_info *Info;
extern struct fichier *Fics;
extern struct PourMask **Mask;

#define HI(qsd) (qsd/256)
#define LO(qsd) (qsd%256)

//--- Retourne 0 si tout va bene ---------------------------------------
int VerifyDisk(char c);  // 1='A'
int __far Error_handler(unsigned deverr,unsigned errcode,
                                                  unsigned far *devhdr);


/*--------------------------------------------------------------------*\
|-                        Gestion du port s‚rie                       -|
\*--------------------------------------------------------------------*/

void interrupt modem_isr(void);
short com_carrier(void);
short com_ch_ready(void);
unsigned char com_read_ch(void);
void com_send_ch(unsigned char ch);
short com_open(short port,long speed,short bit,BYTE parity,BYTE stop);
void com_close(void);

/*--------------------------------------------------------------------*\
|-                             Gestion souris                         -|
\*--------------------------------------------------------------------*/

void InitMouse(void);
void GetPosMouse(int *xm,int *ym,int *button);
int MousePosX(void);
int MousePosY(void);
int MouseButton(void);

int MouseRPosX(void);
int MouseRPosY(void);
void GetRPosMouse(int *x,int *y,int *button);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void Debug(char *string,...);

/*--------------------------------------------------------------------*\
|-  Header for help-functions                                         -|
\*--------------------------------------------------------------------*/
void Help(void);
void HelpTopic(char *);


#define DEFSLASH '\\'
