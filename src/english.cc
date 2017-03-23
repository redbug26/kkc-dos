#include "language.h"

// Format of the infofct:
// - Number of the fonction.
// - Available (or not) for the key mapping
// - Short description (for Fkeys)
// - Long description
// - Minimal version of Ketchup Killers Commander needed
// - Minimal version of Ketchup Killers Macro needed (0 if not handled)

INFOFCT InfoFct[NBRFUNCT]= {
 {  1,1," Help ","Help",91,1},
 {  2,1," Invt ","Invert selection",91,1},
 {  3,1," Slct ","Select group..",91,1},
 {  4,1,"Unslct","Unselect group..",91,1},
 {  5,1,"Search","Search files..",91,1},
 {  6,1," MKKD ","Create a KKD file",91,0},
 {  7,1," View ","View file",91,0},
 {  8,1,"InView","Quick view file",91,0},
 {  9,1," Edit ","Edit file",91,0},
 { 10,1," Copy ","Copy files",91,0},
 { 11,1," Move ","Move files",91,0},
 { 12,1," MDir ","Create directory..",91,0},
 { 13,1,"Delete","Delete files",91,0},
 { 14,1,"On-Off","Left window on/off",91,0},
 { 15,1,"On-Off","Right window on/off",91,0},
 { 16,1," SelF ","Select the current file",91,0},
 { 17,1," CPal ","Palette definition..",91,0},
 { 18,1," About","About",91,0},
 { 19,1," SelT ","Select temporary file",91,0},
 { 20,1," ---- ","Nothing",91,0},
 { 21,1," FDiz ","Window DIZ",91,0},
 { 22,1," Name ","Sort by name",91,0},
 { 23,1," .ext ","Sort by extension",91,0},
 { 24,1," Date ","Sort by date",91,0},
 { 25,1," Size ","Sort by size",91,0},
 { 26,1,"Unsort","Unsort",91,0},
 { 27,1,"Reload","Reload Directory",91,0},
 { 28,1,"ASCIIt","ASCII Table",91,0},
 { 29,1,"Win_CD","Win CD",91,0},
 { 30,1,"P_File","Put file on command line",91,0},
 { 31,1," Setup","Configuration..",91,0},
 { 32,1," Font ","Switch fonts",91,0},
 { 33,1," Spec ","Switch special sort",91,0},
 { 34,1,"ETrash","Erase files in trash",91,0},
 { 35,1,"QuInfo","Display 'quick information' on file",91,0},
 { 36,1," Down ","Scroll down",91,0},
 { 37,1,"  Up  ","Scroll up",91,0},
 { 38,1,"DiInfo","Display disk information",91,0},
 { 39,1,"Attrib","Change attribut",91,0},
 { 40,1," Hist ","Directories history",91,0},
 { 41,1,"Parent","Go in the parent directory",91,0},
 { 42,1," User ","Go in the user directory",91,0},
 { 43,0," ---- ","?? Go in the mainwindow",91,0},
 { 44,1,"Rename","Rename Window",91,0},
 { 45,1,"InEdit","Internal editor",91,0},
 { 46,1,"EnterD","Enter in directory",91,0},
 { 47,1," Line ","Switch lines",91,0},
 { 48,1," Type ","Switch screen",91,0},
 { 49,1,"DrLeft","Change drive of the left window",91,0},
 { 50,1,"DrRght","Change drive of the right window",91,0},
 { 51,1,"RunDir","Go in the running directory",91,0},
 { 52,1," Pgup ","10 lines up",91,0},
 { 53,1," Pgdn ","10 lines down",91,0},
 { 54,1," Home ","First line",91,0},
 { 55,1,"  End ","Last line",91,0},
 { 56,1," Left ","Go in the left window",91,0},
 { 57,1," Rght ","Go in the right window",91,0},
 { 58,0," ---- ","?? ",91,0},
 { 59,1,"Edit..","Edit a file..",91,0},
 { 60,1," Disp ","Switch Display Mode",91,0},
 { 61,1,"Server","Server mode",91,0},
 { 62,1,"KSetup","Main Setup..",91,0},
 { 63,1,"DiInfo","Info Window",91,0},
 { 64,1,"Player","Run linked player",91,0},
 { 65,1," Login","Login menu",91,0},
 { 66,1,"SSetup","Screen setup..",91,0},
 { 67,1,"RefScr","Refresh screen",91,0},
 { 68,1,"SwapWn","Swap right & left window",91,0},
 { 69,1," Hist ","History of command",91,0},
 { 70,1,"PrevLn","Get previous command line",91,0},
 { 71,1,"FileID","Display file_id.diz in the other window",91,0},
 { 72,1,"ChDriv","Change drive in the current window..",91,0},
 { 73,1,"RedRun","Run command & redirect output",91,0},
 { 74,1,"TmpFil","Display temporary file",91,0},
 { 75,0," ---- ","?? Tree",91,0},
 { 76,1,"Ssaver","Screen saver",91,0},
 { 77,0," ---- ","?? Error: Protected support",91,0},
 { 78,1,"SoLSel","Save or load a selection file..",91,1},
 { 79,1," Menu ","Call user menu",91,0},
 { 80,0," ---- ","?? Help on error",91,0},
 { 81,1," SRow ","Switch columns",91,0},
 { 82,1," Color","Color definition..",91,0},
 { 83,1,"ChPath","Go in a directory of the path",91,0},
 { 84,1," Enter","ENTER",91,0},
 { 85,1,"SafeMd","Safe mode display",91,0},
 { 86,1,"EjectD","Eject disk",91,0},
 { 87,1,"Config","Pannel Menu",91,0},
 { 88,1," Quit ","Quit KKC",91,0},
 { 89,1,"ActivW","Toggle active window",91,0},
 { 90,1," Ansi ","View/save background",91,0},
 { 91,0," ---- ","?? Console",91,0},
 { 92,1,"SUnsel","Select or unselect list of files",91,1},
 { 93,1,"NextLn","Get next command line",91,0},
 { 94,1,"QkSrch","Quick search",91,0},
 { 95,1,"BarIdf","Toggle status bar & idf bar",91,0},
 { 96,0," ---- ","?? MessageBox(sbuf0,sbuf1,cbuf0)",91,1},
 { 97,1,"ScrAtt","Copy screen attributes to DOS box",91,0},
 { 98,1,"FSetup","File setup pannel",91,0},
 { 99,1,"HistVw","History viewer",91,0},
 {100,1,"CmpDir","Compare directories..",92,1},
 {101,1,"LSetup","Longname setup pannel",100,0},
 {102,1,"TogCmd","Toggle command line",100,0},
 {103,0," ---- ","?? Accede a un fichier 'pour link macro'",100,0},
 {104,1,"SMouse","Display shortcut mouse menu",100,0},
 {105,1,"CloseW","Close the two windows",100,0},
 {106,1,"SrchPl","Search new players",100,0},
 {107,1,"RunPla","Run a player",100,0},
 {108,1,"ExCopy","Extended copy",100,0},
 {109,1," Bkgr ","View background",100,0},
 {110,1,"View..","View a file (internal or external)",100,0},
 {111,1,"SizeDr","Size of directory",110,0},
 {112,1,"NCompo","Add a new compo",130,0},
 {113,1,"CompoC","Compo Configuration",130,0}

 };


char _BarHeader[][10]=
    {"File","Panel","Disk","Selection","Tools","Options","Help"};


/*--------------------------------------------------------------------*\
|- Label in pannel                                                    -|
\*--------------------------------------------------------------------*/

char _PannelHeader[][20]=
    {"    Name    ",               // 12
     "   Size   ",                 // 10
     "  Date  ",                   // 8
     "Time ",                      // 5
     "     Fullname     "};        // 18

char _OneFilHeader[]="%-10s bytes in one file         ";
char _MulFilHeader[]="%-10s bytes in %3d files        ";
char _OneSelHeader[]="%-10s b. in one selected file   ";
char _MulSelHeader[]="%-10s b. in %3d selected files  ";

char _LabelSubDir[]="SUB--DIR";
char _LabelVolume[]="##Vol-ID##";
char _LabelReload[]=" RELOAD ";
char _LabelFunctn[]="Function";
char _LabelUpdire[]=" UP-DIR ";

char _LabelCdrom[]="CD-ROM Drive";
char _LabelDrive[]="Drive       ";
char _LabelUnsel[]="(Un)select  ";
char _LabelUnkno[]="Unknow      ";

char _LabelSel[]="<SEL>";




// 500  cd team menu
// 501  execute player (ficidf)
// 502  dos4gw error
// 503  create new user menu
// 504  archive


