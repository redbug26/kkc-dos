/*--------------------------------------------------------------------*\
|-  Header of hard-function                                           -|
\*--------------------------------------------------------------------*/

#define BYTE unsigned char
#define WORD unsigned short
#define ULONG unsigned long

#define HI(qsd) ((BYTE)(qsd/256))
#define LO(qsd) ((BYTE)(qsd%256))

#define DEFSLASH '\\'

#define RBPALDEF   {43,37,30, 31,22,17,  0, 0, 0, 58,58,50, \
                    44,63,63, 63,63,21, 43,37,30,  0, 0, 0, \
                    63,63, 0, 63,63,63, 43,37,30, 63,20,20, \
                    20,40,20,  0,40,40, 35,30,27,  0, 0, 0}
/*
#define RBPALDEF   {   0,0,0,   0,0,42,   0,42,0,  0,42,42, \
                      42,0,0,  42,0,42,  42,21,0, 42,42,42, \
                    21,21,21, 21,21,63, 21,63,21, 21,63,63, \
                    63,21,21, 63,21,63, 63,63,21, 63,63,63 }
*/


#define WinError(_ErrMsg_) WinMesg("Error",_ErrMsg_,0)

#define MAX(_rx,_ry) ((_rx>_ry) ? _rx : _ry)

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/





struct config
     {
     long SaveSpeed;  // Number of ticks before calling the screen saver

     long TailleY;                                // Width of the screen
     long TailleX;                               // Length of the screen

     char palette[48];                                    // The PALETTE

     char debug;                                         // DEBUG mode ?
     char speedkey;                                       // Turbo key ?
     char font;                                   // You will use font ?

     char UseFont;                 // Result after calling font fonction

     char display;                                       // Display type

     char comport;                        // Serial port number  (ex: 2)
     long comspeed;                       // Speed            (eg:19200)
     char combit;                         // Number of bit        (eg:8)
     char comparity;                      // Parity             (eg:'N')
     char comstop;                        // Stop bit             (eg:1)

     char Tfont;                 // Character used for the vertical line
     char reinit;

     char col[64];

     char crc;                                              // CRC: 0x69
     };


struct fichier
     {
     char *path;                                                 // path
     char *help;                                              // kkc.hlp
     char *LastDir;                                           // lastdir
     };


extern struct config *Cfg;
extern struct fichier *Fics;

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

  extern void (*AffChr)(long x,long y,long c);
  extern void (*AffCol)(long x,long y,long c);
  extern long (*Wait)(long x,long y,long c);
  extern int  (*KbHit)(void);
  extern void (*GotoXY)(long x,long y);
  extern void (*WhereXY)(long *x,long *y);
  extern void (*Window)(long left,long top,long right,long bottom,long color);
  extern void (*Clr)(void);

#ifdef __cplusplus
};
#endif

/*--------------------------------------------------------------------*\
|- Display function                                                   -|
\*--------------------------------------------------------------------*/
void TXTMode(void);
void LoadPal(char *);
void SavePal(char *);
void InitFont(void);

/*--------------------------------------------------------------------*\
|- Function:     int InitScreen(int a)                                -|
|-                                                                    -|
|- Description:  Initialisation of the hard library                   -|
|-                                                                    -|
|- Input:        0: Hard display                                      -|
|-               1: Ansi display                                      -|
|-               2: Serial port                                       -|
|-                                                                    -|
|- Return:       1: It's Okay...                                      -|
|-                                                                    -|
|- Notes:        You must call this function before display anything  -|
|-               else.                                                -|
\*--------------------------------------------------------------------*/

int InitScreen(int a);

/*--------------------------------------------------------------------*\
|- Function:     void DesinitScreen(void)                             -|
|-                                                                    -|
|- Description:  Close all function of the hard library               -|
|-                                                                    -|
|- Notes:        You must call this function at end of your program   -|
\*--------------------------------------------------------------------*/

void DesinitScreen(void);

/*--------------------------------------------------------------------*\
|- Cursor function                                                    -|
\*--------------------------------------------------------------------*/
void GetCur(char *x,char *y);
void PutCur(char x,char y);

void ScreenSaver(void);

void ScrollUp(void);

void Pause(int n);
void MoveText(long x1,long y1,long x2,long y2,long x3,long y4);

/*--------------------------------------------------------------------*\
|- Absolute function                                                  -|
\*--------------------------------------------------------------------*/
extern char _RB_screen[];

// char GetChr(long x,long y);
#define GetChr(_rx,_ry) *(_RB_screen+((_ry)*256+(_rx)))

// char GetCol(long x,long y);
#define GetCol(_rx,_ry) *(_RB_screen+((_ry)*256+(_rx))+256*128)

void ColLin(long left,long top,long length,long color);
void ChrLin(long left,long top,long length,long color);
void ChrCol(long left,long top,long length,long color);
void ColCol(long left,long top,long length,long color);
void ColWin(long right,long top,long left,long bottom,long color);
void ChrWin(long right,long top,long left,long bottom,long color);

void PrintAt(long x,long y,char *str,...);
char InputAt(long x,long y,char *str, long length);

/*--------------------------------------------------------------------*\
|- Relative function                                                  -|
\*--------------------------------------------------------------------*/
long GetRChr(long x,long y);
long GetRCol(long x,long y);

void ColRLin(long left,long top,long length,long color);
void ChrRLin(long left,long top,long length,long color);
void ChrRCol(long left,long top,long length,long color);
void ColRCol(long left,long top,long length,long color);
void ColRWin(long right,long top,long left,long bottom,long color);
void ChrRWin(long right,long top,long left,long bottom,long color);
void WinRCadre(long x1,long y1,long x2,long y2,long type);

void PrintTo(long x,long y,char *string,...);
long InputTo(long x,long y,char *string, long length);

void AffRChr(long x,long y,long c);
void AffRCol(long x,long y,long c);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/
void Delay(long ms);

void SaveScreen(void);
void LoadScreen(void);

void *GetMem(int);           // Memory allocation with set space to zero
void *GetMemSZ(int);      // Memory allocation without set space to zero
void LibMem(void *);

void WinCadre(int x1,int y1,int x2,int y2,int type);
void Cadre(int x1,int y1,int x2,int y2,int type,int col1,int col2);
void WinLine(int x1,int y1,int xl,int type);

void SetPal(int x,char r,char g,char b);
void GetPal(int x,char *r,char *g,char *b);

int FreeMem(void);

/*--------------------------------------------------------------------*\
|- User Window                                                        -|
\*--------------------------------------------------------------------*/

struct TmtWin
     {
     int x1,y1,x2,y2;
     char *name;
     };

struct Tmt {
     int x,y;
     char type;
     char *str;
     int *entier;
     };

/*--------------------------------------------------------------------*\
  Type:  0 --> Title
         1 --> String
         2 --> [    OK     ]
         3 --> [  CANCEL   ]
         4 --> Cadre with length = *str, width = 2
         5 --> Bouton personnalis‚ (13 bytes length)
         6 --> Cadre with length = *str, width = 1
         7 --> Integer (length = 9)
         8 --> Switch
         9 --> Cadre with length = *str, width = *entier
        10 --> Switch Multiple
\*--------------------------------------------------------------------*/

int Puce(int x,int y,int lng,char p);
int WinTraite(struct Tmt *T,int nbr,struct TmtWin *F,int first);

/*--------------------------------------------------------------------*\
|- Display a user window with title 'title' and message 'msg'.        -|
|- A message could contains several lines (just insert '\n').         -|
|- The 4 high bits of 'info' contains the start position for button.  -|
|- The 4 low bits contains the info about the button                  -|
|-   0: button "OK" & "CANCEL"                                        -|
|-   1: button "YES" & "NO"                                           -|
|- return value:  the number of the button                            -|
\*--------------------------------------------------------------------*/

int WinMesg(char *title,char *msg,char info);

int Gradue(int x,int y,int length,int from,int to,int total);

void DefaultCfg(void);

void SetDefaultPath(char *path);                      // Set up the file

/*--------------------------------------------------------------------*\
|-        Menu & Pannel function                                      -|
\*--------------------------------------------------------------------*/
struct barmenu
    {
    char *Titre;
    char *Help;
    int fct;
    };

typedef struct __mainmenu
    {
    int x,y;
    int attr;
    int nbrmax;
    int cur;
    } MENU;

/* attr:
   7 6 5 4 3 2 1 0
   ³ ³ ³ ³ ³ ³ ³ ÀÄÄ   1 Use nbrmax items at a time on screen.
   ³ ³ ³ ³ ³ ³ ÀÄÄÄÄ   2 Don't use shortcut.
   ³ ³ ³ ³ ³ ÀÄÄÄÄÄÄ   4 Don't save screen.
   ³ ³ ³ ³ ÀÄÄÄÄÄÄÄÄ   8 Don't accept LEFT & RIGHT key.
   ÀÄÁÄÁÄÁÄÄÄÄÄÄÄÄÄÄ     0
*/

int BarMenu(struct barmenu *bar,int nbr,int *poscur,int *xp,int *yp);
int PannelMenu(struct barmenu *bar,int nbr,MENU *menu);

void ClearEvents(void);
void NewEvents(int (*fct)(struct barmenu *),char *titre,int key);


void Bar(char *);                               // Affichage de la barre

//--- Retourne 0 si tout va bene ---------------------------------------
int VerifyDisk(long c);                                   //--- 1='A'
int __far Error_handler(unsigned deverr,unsigned errcode,
                                                  unsigned far *devhdr);

/*--------------------------------------------------------------------*\
|-        Serial port function                                        -|
\*--------------------------------------------------------------------*/

void interrupt modem_isr(void);
long com_carrier(void);
char com_ch_ready(void);
long com_read_ch(void);
void com_send_ch(long ch);
char com_open(long port,long speed,long bit,BYTE parity,BYTE stop);
void com_close(void);

/*--------------------------------------------------------------------*\
|-  Mouse handler                                                     -|
\*--------------------------------------------------------------------*/
void InitMouse(void);
void GetPosMouse(int *xm,int *ym,int *button);
int MousePosX(void);
int MousePosY(void);
int MouseButton(void);
void ReleaseButton(void);

int MouseRPosX(void);
int MouseRPosY(void);
void GetRPosMouse(int *x,int *y,int *button);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void Debug(char *string,...);

/*--------------------------------------------------------------------*\
|-  Header for help-functions                                         -|
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|- Function:     void Help(void)                                      -|
|-                                                                    -|
|- Description:  Call the helpfile                                    -|
|-                                                                    -|
|- Notes:        Verify that you have put the name of the help file   -|
|-               in Fics->help                                        -|
\*--------------------------------------------------------------------*/

void Help(void);

/*--------------------------------------------------------------------*\
|- Function:     void HelpTopic(char *)                               -|
|-                                                                    -|
|- Description:  Call the helpfile for a special topic                -|
|-                                                                    -|
|- Input:        The topic name                                       -|
|-                                                                    -|
|- Notes:      - Verify that you have put the name of the help file   -|
|-               in Fics->help                                        -|
|-             - You could creat a topic with a '#' in column 1       -|
\*--------------------------------------------------------------------*/
void HelpTopic(char *);


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


