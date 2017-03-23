/*--------------------------------------------------------------------*\
|-  Header of hard-function               -|
\*--------------------------------------------------------------------*/
// #define CLOCKK_PER_SEC CLOCKS_PER_SEC

void Redinit(void);

#ifndef KKHARD_H

#define KKHARD_H

#ifndef DEBUG
#define NDEBUG
#endif

#include <assert.h>

#define BYTE           unsigned char
#define WORD           unsigned short
#define ULONG          unsigned long


#define clock_k        signed int
#define CLOCKK_PER_SEC 18.2
clock_k GetClock(void);

#define MaxX           90
#define MaxY           50
#define MaxZ           MaxX * MaxY * 2

#define SELCHAR        0x0F

#define TRUE           1
#define FALSE          0

#define bool           char
#define uchar          unsigned char
#define ushort         unsigned short

#ifndef LINUX
#define uint           unsigned int
#endif

#define ulong          unsigned long

#define HI(qsd) ((BYTE)(qsd / 256))
#define LO(qsd) ((BYTE)(qsd % 256))

#ifdef LINUX
#define DEFSLASH       '/'
#else
#define DEFSLASH       '\\'
#endif

#define RBPALDEF       {43, 37, 30, 31, 22, 17,  0, 0, 0, 58, 58, 50, 44, 63, 63, 63, 63, 21, 43, 37, 30,  0, 0, 0, 63, 63, 0, 63, 63, 63, 43, 37, 30, 63, 20, 20, 20, 40, 20,  0, 40, 40, 35, 30, 27,  0, 0, 0}

#define WinError(_ErrMsg_) WinMesg("Error", _ErrMsg_, 0)

#define MAX(_rx, _ry)      (((_rx) > (_ry)) ? (_rx) : (_ry))
#define MIN(_rx, _ry)      (((_rx) > (_ry)) ? (_ry) : (_rx))

/*--------------------------------------------------------------------*\
|- Classe                   -|
\*--------------------------------------------------------------------*/

class KKWin
{
public:
KKWin();
KKWin(char *title);
KKWin(char *buf, int x1, int y1, int x2, int y2, int type, int col);

~KKWin();

void Test(char *chaine);

int Wait(int x, int y);

protected:

char *title;
int left, top, right, bottom, color, type;

void Defaults(void);

};

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/
void Beep(int);

#define BEEP_WARNING0 0
#define BEEP_WARNING1 1
#define BEEP_WARNING2 2
#define BEEP_ERROR    3


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

struct config {
    long SaveSpeed;  // Nbr of ticks before calling the screen saver-04-
    long TailleY;                  // Width of the screen-01-
    long TailleX;                  // Length of the screen-02-
    char palette[48];              // The PALETTE-14-
    char debug;                    // DEBUG mode ?-23-
    char speedkey;                 // Turbo key ?-25-
    char font;                     // You will use font ?-05-
    char display;                  // Display type-06-
    char comport;                  // Serial port number  (eg: 2)-28-
    long comspeed;                 // Speed     (eg:19200)-33-
    char combit;                   // Number of bit       (eg:8)-41-
    char comparity;                // Parity        (eg:'N')-42-
    char comstop;                  // Stop bit        (eg:1)-43-
    char col[64];                  //  Couleur --------------------15-

    char windesign;   // Design of win, 1=NC, 2=WATC., 3=KKC, 4=Font-03-

    char reinit;                   //
    char UseFont;                  // Result after calling font fonction
    char mousemode;                // 0: normal, 1: emulate key-44-
    char crc;                      // CRC: 0x69

};

struct fichier {
    char *path;                    // path
    char *LastDir;                 // lastdir
};

extern struct config *Cfg;
extern struct fichier *Fics;

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

extern void (*AffChr)(long x, long y, long c);
extern void (*AffCol)(long x, long y, long c);
extern long (*Wait)(long x, long y);
extern int (*KbHit)(void);
extern void (*GotoXY)(long x, long y);
extern void (*WhereXY)(long *x, long *y);
extern void (*Window)(long left, long top, long right, long bottom, long color);
extern void (*Clr)(void);
extern int (*SetMode)(void);
extern void (*Cadre)(int x1, int y1, int x2, int y2, int type, int col1, int col2);

void Buffer_Clr(void);

char CnvASCII(char table, char car);

/*--------------------------------------------------------------------*\
|- Gestion interne des touches                -|
\*--------------------------------------------------------------------*/
void PutKey(int key);
int GetKey(void);

/*--------------------------------------------------------------------*\
|- Display function                 -|
\*--------------------------------------------------------------------*/
void TXTMode(void);
void LoadPal(void);
void SavePal(char *);
void InitFont(void);
void InitFontFile(char *);



/*--------------------------------------------------------------------*\
|- Display handler                  -|
\*--------------------------------------------------------------------*/

extern int (*disp_system)(int, char *);

int givesyst(int x);



/*--------------------------------------------------------------------*\
|- Function:   int InitScreen(int a)              -|
|-                      -|
|- Description:  Initialisation of the hard library         -|
|-                      -|
|- Input:  0: Hard display              -|
|-     1: Ansi display              -|
|-     2: Serial port                       -|
|-                      -|
|- Return:   1: It's Okay...                                      -|
|-                      -|
|- Notes:  You must call this function before display anything  -|
|-     else.                  -|
\*--------------------------------------------------------------------*/

int InitScreen(int a);

/*--------------------------------------------------------------------*\
|- Function:   void DesinitScreen(void)           -|
|-                      -|
|- Description:  Close all function of the hard library           -|
|-                      -|
|- Notes:  You must call this function at end of your program   -|
\*--------------------------------------------------------------------*/

#define DesinitScreen() disp_system(3, NULL)

/*--------------------------------------------------------------------*\
|- Cursor function                  -|
\*--------------------------------------------------------------------*/
void GetCur(char *x, char *y);
void PutCur(char x, char y);

void ScreenSaver(void);

void Pause(int n);
void MoveText(long x1, long y1, long x2, long y2, long x3, long y3);

/*--------------------------------------------------------------------*\
|- Absolute function                  -|
\*--------------------------------------------------------------------*/

extern char _RB_screen[];

#define GetChr(_rx, _ry) *(_RB_screen + ((_ry) * 256 + (_rx)))
#define GetCol(_rx, _ry) *(_RB_screen + ((_ry) * 256 + (_rx)) + 256 * 128)

void ColLin(long left, long top, long length, long color);
void ChrLin(long left, long top, long length, long color);
void ChrCol(long left, long top, long length, long color);
void ColCol(long left, long top, long length, long color);
void ColWin(long right, long top, long left, long bottom, long color);
void ChrWin(long right, long top, long left, long bottom, long color);

void PrintAt(long x, long y, char *str, ...);
char InputAt(long x, long y, char *str, long length);

/*--------------------------------------------------------------------*\
|- Relative function                  -|
\*--------------------------------------------------------------------*/
long GetRChr(long x, long y);
long GetRCol(long x, long y);

void ColRLin(long left, long top, long length, long color);
void ChrRLin(long left, long top, long length, long color);
void ChrRCol(long left, long top, long length, long color);
void ColRCol(long left, long top, long length, long color);
void ColRWin(long right, long top, long left, long bottom, long color);
void ChrRWin(long right, long top, long left, long bottom, long color);
void WinRCadre(long x1, long y1, long x2, long y2, long type);

void PrintTo(long x, long y, char *string, ...);
long InputTo(long x, long y, char *string, long length);

void AffRChr(long x, long y, long c);
void AffRCol(long x, long y, long c);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/
void Delay(long ms);

void SaveScreen(void);
void LoadScreen(void);

void * GetMem(int);           // Memory allocation with set space to zero
void * GetMemSZ(int);      // Memory allocation without set space to zero
void LibMem(void *);

void WinCadre(int x1, int y1, int x2, int y2, int type);
void WinLine(int x1, int y1, int xl, int type);

void SetPal(int x, char r, char g, char b);
void GetPal(int x, char *r, char *g, char *b);

int GetScreenSizeX(void);
int GetScreenSizeY(void);

void Buf2Scr(char *buffer);
void Scr2Buf(char *buffer);


/*--------------------------------------------------------------------*\
|- Function:   int FreeMem(void)              -|
|-                      -|
|- Description:  Return the number of bytes available for the system  -|
\*--------------------------------------------------------------------*/
int FreeMem(void);

/*--------------------------------------------------------------------*\
|- User Window                    -|
\*--------------------------------------------------------------------*/

struct TmtWin {
    int x1, y1, x2, y2;
    char *name;
};

struct Tmt {
    int x, y;
    char type;
    char *str;
    int *entier;
};

/*--------------------------------------------------------------------*\
*  Type:  0 --> Title
*        1 --> String
*        2 --> [    OK     ]
*        3 --> [  CANCEL   ]
*        4 --> Cadre with length = *str, width = 2
*        5 --> Bouton personnalis‚ (13 bytes length)
*        6 --> Cadre with length = *str, width = 1
*        7 --> Integer (length = 9)
*        8 --> Switch
*        9 --> Cadre with length = *str, width = *entier
*       10 --> Switch Multiple
\*--------------------------------------------------------------------*/

int Puce(int x, int y, int lng, char p);

/*--------------------------------------------------------------------*\
|- Function:   int WinTraite(struct Tmt,int,struct TmtWin,int)      -|
|-                      -|
|- Description:  Call a dialog box              -|
|-                      -|
|- Input:  The description of the main box          -|
|-     The number of section              -|
|-     The description of all the sections          -|
|-     The first section              -|
|-                      -|
|- Output:   The number of the select widget(ok,cancel or user)   -|
|-     -1 if ESCape               -|
\*--------------------------------------------------------------------*/

int WinTraite(struct Tmt *T, int nbr, struct TmtWin *F, int first);

/*--------------------------------------------------------------------*\
|- Display a user window with title 'title' and message 'msg'.        -|
|- A message could contains several lines (just insert '\n').         -|
|- The 4 high bits of 'info' contains the start position for button.  -|
|- The 4 low bits contains the info about the button          -|
|-   0: button "OK" & "CANCEL"                                        -|
|-   1: button "YES" & "NO"                                           -|
|-   2: button "OK"                                                   -|
|- return value:  the number of the button            -|
\*--------------------------------------------------------------------*/

char WinMesg(const char *title, const char *msg, char info);

int Gradue(int x, int y, int length, int from, int to, int total);

void DefaultCfg(struct config *Cfg2);
void ReadCfg(struct config *Cfg2);

void SetDefaultPath(char *path);                      // Set up the file

char IsSlash(char c);

/*--------------------------------------------------------------------*\
|-    Menu & Pannel function              -|
\*--------------------------------------------------------------------*/
typedef struct barmenu {
    char *Titre;
    int Help;
    int fct;
} BARMENU;

typedef struct __mainmenu {
    int x, y;
    int attr;
    int nbrmax;
    int cur;
} MENU;

/* attr:
 * 7 6 5 4 3 2 1 0
 * ³ ³ ³ ³ ³ ³ ³ ÀÄÄ   1 Use nbrmax items at a time on screen.
 * ³ ³ ³ ³ ³ ³ ÀÄÄÄÄ   2 Don't use shortcut.
 * ³ ³ ³ ³ ³ ÀÄÄÄÄÄÄ   4 Don't save screen.
 * ³ ³ ³ ³ ÀÄÄÄÄÄÄÄÄ   8 Don't accept LEFT & RIGHT key.
 * ÀÄÁÄÁÄÁÄÄÄÄÄÄÄÄÄÄ   0
 */

int BarMenu(BARMENU *bar, int nbr, int *poscur, int *xp, int yp);

/*--------------------------------------------------------------------*\
|-  1: [RIGHT]   -1: [LEFT]               -|
|-  0: [ESC]    2: [ENTER]                -|
\*--------------------------------------------------------------------*/
int PannelMenu(BARMENU *bar, int nbr, MENU *menu);

void NewEvents(register int ( *__func)(struct barmenu *), char *titre, int key);

void ClearEvents(void);

void Bar(char *);                            // Display the bar function
int GetMouseFctBar(int status);    // Return Fx key for the bar function

/*--------------------------------------------------------------------*\
|- Gestion des fontes                 -|
\*--------------------------------------------------------------------*/
void Font2Buf(char *);
void Buf2Font(char *);

/*--------------------------------------------------------------------*\
|- Gestion de la palette                -|
\*--------------------------------------------------------------------*/
void Pal2Buf(char *);
void Buf2Pal(char *);

/*--------------------------------------------------------------------*\
|-    Serial port function                -|
\*--------------------------------------------------------------------*/
#ifdef __WC32__
void interrupt modem_isr(void);
long com_carrier(void);
char com_ch_ready(void);
long com_read_ch(void);
void com_send_ch(long ch);
char com_open(long port, long speed, long bit, BYTE parity, BYTE stop);
void com_close(void);
#endif

/*--------------------------------------------------------------------*\
|-  Mouse handler                 -|
\*--------------------------------------------------------------------*/
void InitMouse(void);
void GetPosMouse(int *xm, int *ym, int *button);
int MousePosX(void);
int MousePosY(void);
int MouseButton(void);
void ReleaseButton(void);

int MouseRPosX(void);
int MouseRPosY(void);
void GetRPosMouse(int *x, int *y, int *button);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void Debug(char *string, ...);

/*--------------------------------------------------------------------*\
|-  Header for help-functions               -|
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|- Function:   void Help(void)              -|
|-                      -|
|- Description:  Call the helpfile              -|
|-                      -|
|- Notes:  Verify that you have put the name of the help file   -|
|-     in Fics->help                -|
\*--------------------------------------------------------------------*/

void Help(void);

/*--------------------------------------------------------------------*\
|- Function:   void HelpTopic(int x)              -|
|-                      -|
|- Description:  Call the helpfile for a special topic          -|
|-                      -|
|- Input:  The number of the link "lnd%d",x                     -|
|-                      -|
|- Notes:      - Verify that you have put the name of the help file   -|
|-     in Fics->help                -|
\*--------------------------------------------------------------------*/
void HelpTopic(int);

void LoadHelpHandler(register void (*fct)(void));
void LoadHelpTopicHandler(register void (*fct)(char *));

void NumHelp(int n);



/*--------------------------------------------------------------------*\
|- Personnal Function                 -|
\*--------------------------------------------------------------------*/
void LoadErrorHandler(void);   // --- Load the internal error handler ---

// PCDISPLAY    -> appelle les fonctions DJGPP d'affichage
// NOMOUSE    -> pas du support souris
// NODIRECTVIDEO  -> acces direct … l'‚cran
// NOINT10
// NOCOM
// NOFONT
// NOINT
// NO4DOS
// NOWIN95
// NOWINDOWS
// NOSAVE
// NODRIVE
// NOANSI


#ifdef DJGPP
#define PCDISPLAY     1
#endif

#ifdef DJGPP

#define NOMOUSE       1
#define NODIRECTVIDEO 1
#define NOINT10       1
#define NOCOM         1
#define NOFONT        1
#define NOINT         1
#define NO4DOS        1
#define NOWIN95       1
#define NOWINDOWS     1
#define NOSAVE        1

#endif


#ifdef LINUX

#define NOMOUSE       1
#define NODIRECTVIDEO 1
#define NOINT10       1
#define NOCOM         1
#define NOFONT        1
#define NOINT         1
#define NO4DOS        1
#define NOWIN95       1
#define NOWINDOWS     1
#define NOSAVE        1

#define CURSES        1

#endif


#ifdef LCCWIN

#define NOMOUSE       1
#define NODIRECTVIDEO 1
#define NOINT10       1
#define NOCOM         1
#define NOFONT        1
#define NOINT         1
#define NO4DOS        1
#define NOWIN95       1
#define NOWINDOWS     1
#define NOSAVE        1

#define CURSES        1

#endif

// #define USEPTC
//
#ifndef LINUX
#define USEVESA
#endif


// #define NODRIVE 1


// remettre ROR

#ifdef CURSES
#include <curses.h>

#define KEY_C_UP      0x991232
#define KEY_A_F11     0x991233
#define KEY_A_F12     0x991234
#define KEY_C_RIGHT   0x991235
#define KEY_C_LEFT    0x991236
#define KEY_C_HOME    0x991237

#else

#define KEY_F(n) (0x3A + n) * 256
#define KEY_PPAGE     0x4900
#define KEY_NPAGE     0x5100
#define KEY_UP        0x4800
#define KEY_DOWN      0x5000
#define KEY_RIGHT     0x4D00
#define KEY_LEFT      0x4B00
#define KEY_END       0x4F00
#define KEY_HOME      0x4700
#define KEY_DC        0x5300

#define KEY_C_HOME    0x7700

#define KEY_F11       0x8500
#define KEY_F12       0x8600

#define KEY_A_F11     0x8B00
#define KEY_A_F12     0x8C00

#define KEY_BACKSPACE 0x0008

#define KEY_C_PPAGE   0x8400
#define KEY_C_NPAGE   0x7600

#define KEY_C_RIGHT   0x7400
#define KEY_C_LEFT    0x7300

#define KEY_C_UP      0x8D00
#define KEY_C_DOWN    0x9100

#define KEY_A_NPAGE   0xA100

#define KEY_INS       0x5200

#define KEY_A_ENTER   0x1C00

#define KEY_ENTER     13

#endif // ifdef CURSES



#ifdef LINUX

// for comptability only

int strnicmp(char *, char *, int);
int stricmp(const char *a, const char *b);
char * strlwr(char *a);
char * strupr(char *a);
unsigned _bios_keybrd(unsigned cmd);
int filelength(int fic);

#define toupper(n) ((((n) >= 'a') & ((n) <= 'z')) ? (n) - 'a' + 'A' : (n))

#endif

#endif // ifndef KKHARD_H


#define NORTONPAL "\x00\x00\x00\x00\x00\x2A\x00\x2A\x00\x00\x2A\x2A" \
    "\x2A\x00\x00\x2A\x00\x2A\x2A\x15\x00\x2A\x2A\x2A" \
    "\x15\x15\x15\x15\x15\x3F\x15\x3F\x15\x15\x3F\x3F" \
    "\x3F\x15\x15\x3F\x15\x3F\x3F\x3F\x15\x3F\x3F\x3F"

#define NORTONCOL "\x1B\x30\x1E\x3E\x1E\x03\x30\x30\x0F\x30\x3F\x3E" \
    "\x0F\x0E\x30\x19\x1B\x13\x30\x3F\x3E\x0F\x15\x12" \
    "\x30\x3F\x0F\x3E\x4F\x4E\x70\x00\x14\x13\xB4\x60" \
    "\x70\x1B\x1B\x1B\x1E\x30\x30\x0F\x3F\x3F\x4F\x4F" \
    "\x3F\x0F\x70\x90\x30\x30\x0E\x1B\x1B\xA0\xB0\xC0" \
    "\x0E\x0F\x04\x07"


