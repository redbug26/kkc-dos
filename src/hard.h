/*--------------------------------------------------------------------*\
|-  Header of hard-function                                           -|
\*--------------------------------------------------------------------*/

#define BYTE unsigned char
#define WORD unsigned short
#define ULONG unsigned long

#define HI(qsd) (qsd/256)
#define LO(qsd) (qsd%256)

#define DEFSLASH '\\'

#define RBPALDEF   {43,37,30, 31,22,17,  0, 0, 0, 58,58,50, \
                    44,63,63, 63,63,21, 43,37,30,  0, 0, 0, \
                    63,63, 0, 63,63,63, 43,37,30, 63,20,20, \
                    20,40,20,  0,40,40,  0, 0, 0,  0, 0, 0}

#define WinError(_ErrMsg_) WinMesg("Error",_ErrMsg_,0)

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

struct barmenu
        {
        char titre[20];
        char help[80];
        short fct;
        };


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

/* Tmt:
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

*/


struct config
     {
     long SaveSpeed;  // Number of ticks before calling the screen saver

     short TailleY;                               // Width of the screen
     short TailleX;                              // Length of the screen

     char palette[48];                                    // The PALETTE

     char debug;                                         // DEBUG mode ?
     char speedkey;                                       // Turbo key ?
     char font;                                   // You will use font ?

     short UseFont;                // Result after calling font fonction

     char display;                                       // Display type

     char comport;                        // Serial port number  (ex: 2)
     int comspeed;                        // Speed            (eg:19200)
     char combit;                         // Number of bit        (eg:8)
     char comparity;                      // Parity             (eg:'N')
     char comstop;                        // Stop bit             (eg:1)

     char Tfont;                 // Character used for the vertical line

     char crc;                                              // CRC: 0x69
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


extern struct config *Cfg;
extern struct fichier *Fics;

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/
extern void (*AffChr)(short x,short y,short c);
extern void (*AffCol)(short x,short y,short c);
extern int (*Wait)(int x,int y,char c);
extern int (*KbHit)(void);
extern void (*GotoXY)(char x,char y);
extern void (*WhereXY)(char *x,char *y);
extern void(*Window)(int left,int top,int right,int bottom,short color);

/*--------------------------------------------------------------------*\
|- Display function                                                   -|
\*--------------------------------------------------------------------*/
void TXTMode(void);
void LoadPal(void);
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
void MoveText(int x1,int y1,int x2,int y2,int x3,int y4);

/*--------------------------------------------------------------------*\
|- Absolute function                                                  -|
\*--------------------------------------------------------------------*/
extern char _RB_screen[];

// char GetChr(short x,short y);
#define GetChr(_rx,_ry) *(_RB_screen+((_ry)*256+(_rx)))

// char GetCol(short x,short y);
#define GetCol(_rx,_ry) *(_RB_screen+((_ry)*256+(_rx))+256*128)

void ColLin(int left,int top,int length,short color);
void ChrLin(int left,int top,int length,short color);
void ChrCol(int left,int top,int length,short color);
void ColCol(int left,int top,int length,short color);
void ColWin(int right,int top,int left,int bottom,short color);
void ChrWin(int right,int top,int left,int bottom,short color);

void PrintAt(int x,int y,char *str,...);
char InputAt(int x,int y,char *str, int length);

/*--------------------------------------------------------------------*\
|- Relative function                                                  -|
\*--------------------------------------------------------------------*/
char GetRChr(int x,int y);
char GetRCol(int x,int y);

void ColRLin(int left,int top,int length,short color);
void ChrRLin(int left,int top,int length,short color);
void ChrRCol(int left,int top,int length,short color);
void ColRCol(int left,int top,int length,short color);
void ColRWin(int right,int top,int left,int bottom,short color);
void ChrRWin(int right,int top,int left,int bottom,short color);

void PrintTo(int x,int y,char *string,...);
char InputTo(int x,int y,char *string, int length);

void AffRChr(int x,int y,char c);
void AffRCol(int x,int y,char c);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/
void Delay(long ms);

void SaveScreen(void);
void LoadScreen(void);

void *GetMem(int);           // Memory allocation with set space to zero
void *GetMemSZ(int);      // Memory allocation without set space to zero
void LibMem(void *);

void WinCadre(int x1,int y1,int x2,int y2,int type);
void WinLine(int x1,int y1,int xl,int type);

void SetPal(int x,char r,char g,char b);

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
int BarMenu(struct barmenu *bar,int nbr,int *poscur,int *xp,int *yp);
int PannelMenu(struct barmenu *bar,int nbr,int *c,int *xp,int *yp);



//--- Retourne 0 si tout va bene ---------------------------------------
int VerifyDisk(char c);  // 1='A'
int __far Error_handler(unsigned deverr,unsigned errcode,
                                                  unsigned far *devhdr);

/*--------------------------------------------------------------------*\
|-        Serial port function                                        -|
\*--------------------------------------------------------------------*/

void interrupt modem_isr(void);
short com_carrier(void);
short com_ch_ready(void);
unsigned char com_read_ch(void);
void com_send_ch(unsigned char ch);
short com_open(short port,long speed,short bit,BYTE parity,BYTE stop);
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


