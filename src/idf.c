/*--------------------------------------------------------------------*\
|-                      Identification of file                        -|
\*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <dos.h>
#include <direct.h>
#include <bios.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include "idf.h"

struct key K[nbrkey]=   {
{  {"C64S tap"},
        8,
        0,
        "C64S tape file",
        "T64",
        "",
        76,0,1,6},
{  {"UC2"},
        3,
        0,
        "UC2 Archive",
        "UC2",
        "Ad Infinitum Programs",
        75,0,0,3},
{  {0xD7,0xCD,0xC6,0x9A},
        4,
        0,
        "Windows Vectorial Image",
        "WMF",
        "Microsoft Corp.",
        74,0,0,4},
{  {"mhwanh"},
        6,
        0,
        "HSI raw bitmap",
        "RAW",
        "Handmade Software, Inc",   //--- HSI (Creator of alchemy) -----
        73,0,1,4},
{  {0,0},
        0,
        0,
        "Module File", //--- 31 instruments: NoiseTracker --------------
        "MOD",
        "",
        1,1,1,1},
{  {"DDMF"},
        4,
        0,
        "Digital Music Module",
        "DMF",
        "Delusion",
        6,0,1,1},
{  {"IMPM"},
        4,
        0,
        "Impulse Tracker Module",
        "IT",
        "Impulse",
        10,0,1,1},
{  {"MThd"},
        4,
        0,
        "Midi file",
        "MID",
        "I.M.A.",       //--- International MIDI Association -----------
        14,0,1,1},
{  {"MTM"},
        3,
        0,
        "Multitracker Module",
        "MTM",
        "Renaissance",
        15,0,1,1},
{  {0,0},
        4,
        0,
        "669 Module",
        "669",
        "Renaissance",
        4,1,1,1},
{  {"Extended Module: "},
        17,
        0,
        "Fast Tracker Module",
        "XM",
        "Triton",
        2,0,1,1},
{  {"SCRM"},
        4,
        44,
        "Scream Tracker 3 Module",
        "S3M",
        "Future Crew",
        3,0,1,1},
{  {"MAS_UTrack"},
        10,
        0,
        "Ultratracker module",
        "UT",
        "Mysterious MAS",
        19,0,1,1},
{  {0x52,0x61,0x72,0x21,0x1A,0x07,0x00},
        7,
        0,
        "RAR Archive",
        "RAR",
        "Eugene Roshal",
        34,0,1,3},
{  {"GIF8"},
        4,
        0,
        "Compuserve GIF",
        "GIF",
        "Compuserve",
        37,0,1,4},
{  {"BM"},
        2,
        0,
        "Windows Bitmap",
        "BMP",
        "Microsoft Corp.",
        36,0,1,4},
{  {"OKTASONG"},
        8,
        0,
        "Oktalyser module",
        "OKT",
        "Armin Sander",
        16,0,1,1},
{  {".snd"},
        4,
        0,
        "Sun/NeXT audio file",
        "AU",
        "Sun/NeXT",
        21,0,1,2},
{  {0,0},
        0,
        0,
        "Amiga sampled voice",
        "IFF",
        "Electronic Arts",
        23,1,1,2},
{  {0,0},
        0,
        0,
        "Interleaved Bitmap",
        "LBM",
        "Electronic Arts",
        39,1,1,4},
{  {0,0},
        0,
        0,
        "ARJ Archive",
        "ARJ",
        "Robert K Jung",
        30,1,0,3},
{  {"-lh"},
        3,
        2,
        "LHA Archive",
        "LHA",
        "H. Yoshizaki",
        32,0,0,3},
{  {"MMD"},
        3,
        0,
        "OctaMED module",
        "MED",
        "Teijo Kinnunen",
        13,0,1,1},
{  {"Creative Voice File"},
        19,
        0,
        "Creative Voice File",
        "VOC",
        "Creative Labs",
        27,0,1,2},
{  {"PSM "},
        4,
        0,
        "Epic PSM/MOD",
        "PSM",
        "Epic Megagames",
        17,0,1,1},
{  {"MZ"},
        2,
        0,
        "Executable file",
        "EXE",
        "M.Z. ;)",
        57,0,1,6},
{  {"Extended Instrument: "},
        21,
        0,
        "Instrument Fast Tracker",
        "XI",
        "Triton",
        29,0,1,2},
{  {"GF1PATCH"},
        8,
        0,
        "Patch Gravis Ultrasound",
        "PAT",
        "Gravis Ultrasound",
        24,0,1,2},
{  {"Turbo C Project File"},
        20,
        0,
        "Turbo C Project File",
        "PRJ",
        "Borland",
        62,0,0,6},
{  {"Turbo C Context File"},
        20,
        0,
        "Turbo C Context File",
        "DSK",
        "Borland",
        55,0,0,6},
{  {"TPU"},
        3,
        0,
        "Turbo Pascal Unit",
        "TPU",
        "Borland",
        64,0,1,6},
{  {"CTMF"},
        4,
        0,
        "Creative Music File",
        "CMF",
        "Creative Labs",
        5,0,1,1},
{  {0x3F,0x5F,0x03,0x00},
        4,
        0,
        "Windows Help File",
        "HLP",
        "Microsoft Corp.",
        59,0,1,6},
{  {"!Scream!"},
        8,
        0x14,
        "Scream Tracker Module",
        "STM",
        "Future Crew",
        18,0,1,1},
{  {"FARþ"},
        4,
        0,
        "Farandole Module",
        "FAR",
        "Digital Infinity",     //--- Daniel Potter / DI ---------------
        8,0,1,1},
{  {"FSMþ"},
        4,
        0,
        "Far. Sample Data Signed",
        "FSM",
        "Digital Infinity",     //--- Daniel Potter / DI ---------------
        22,0,1,2},
{  {"USMþ"},
        4,
        0,
        "Far. Sample Data Unsigned",
        "USM",
        "Digital Infinity",     //--- Daniel Potter / DI ---------------
        26,0,1,2},
{  {"FPTþ"},
        4,
        0,
        "Farandole Pattern",
        "FPT",
        "Digital Infinity",     //--- Daniel Potter / DI ---------------
        9,0,1,1},
{  {"F2RFAR"},
        6,
        0,
        "Far. Linear Module V2.0",
        "F2R",
        "Digital Infinity",     //--- Daniel Potter / DI ---------------
        7,0,1,1},
{  {"DMDL"},
        4,
        0,
        "Digitrakker Module",
        "MDL",
        "n-Factor",
        12,0,1,1},
{  {"DSPL"},
        4,
        0,
        "Digitrakker Sample",
        "SPL",
        "n-Factor",
        25,0,1,2},
{  {0xD0,0xCf,0x11,0xE0,0xA1,0xB1,0x1A,0xE1},
        8,
        0,
        "Microsoft File Format",
        "MS",
        "Microsoft Corp.",
        61,0,0,6},
{  {"PMCC"},
        4,
        0,
        "Windows Group File",
        "GRP",
        "Microsoft Corp.",
        58,0,1,6},
{  {0xFF,0x57,0x50,0x43},
        4,
        0,
        "WordPerfect Graphic",
        "WPG",
        "WordPerfect",
        46,0,0,4},
{  {0x49,0x49,0x2A,0x00},
        4,
        0,
        "Intel TIFF Picture",
        "TIF",
        "Intel",
        45,0,0,4},
{  {0xFF,0xD8,0xFF,0xE0,0x00,0x10,0x4A,0x46,0x49,0x46,0x00},
        11,
        0,
        "JPEG Picture",
        "JPG",
        "Joint Photo.Exp.Group", //--- Joint Photographic Experts Group-
        38,0,0,4},
{  {0x89,0x50,0x4E,0x47},
        4,
        0,
        "PNG Network Graphics",
        "PNG",
        "Thomas Boutell",
        41,0,0,4},
{  {"JMUSIC"},
        6,
        0,
        "Jmplayer module",
        "JMS",
        "Ultra Force",
        11,0,0,1},
{   "",
        7,
        0,
        "MPEG Movie",
        "MPG",
        "ISO",
        52,1,0,5},
{  {"mdat"},
        4,
        4,
        "QuickTime Movie",
        "MOV",
        "Apple",
        51,0,0,5},
{  {"THNL"},
        4,
        0,
        "GWS Quick View",
        "THN",
        "Alchemy MindWorks",
        44,0,0,4},
{  {"MV - CPCEMU"},
        11,
        0,
        "Cpcemu Disk File",
        "DSK",
        "Marco Vieth",
        56,0,0,6},
{  {"ACONLIST"},
        8,
        8,
        "Windows Animated Cursor",
        "ANI",
        "Microsoft Corp.",
        47,0,0,5},
{  {"WAVE"},
        4,
        8,
        "Windows Wave",
        "WAV",
        "Microsoft Corp.",
        28,0,0,2},
{  {"AVI LIST"},
        8,
        8,
        "AVI Animation",
        "AVI",
        "Microsoft Corp.",
        48,0,0,5},
{  {0x0A,0x05,0x01},
        3,
        0,
        "ZSoft PCX 3.0",
        "PCX",
        "ZSoft",
        40,0,1,4},
{  {"RIX3"},
        4,
        0,
        "ColoRix Picture",
        "SCF",
        "",
        42,0,0,4},
{  {"AIFF"},
        4,
        8,
        "AIFF Audio File",
        "AIF",
        "Electronic Arts",
        20,0,0,2},
{  {0xFF,0xFF,0x53,0x45,0x4D},
        5,
        0,
        "QEDIT Macro",
        "MAC",
        "SemWare Corp.",
        60,0,0,6},
{  {"/IMPHOBIA Ressource File/"},
        25,
        0,
        "IMPHOBIA Ressource File",
        "DAT",
        "Darkness",
        54,0,0,6},
{  {"/IMPHOBIA Config/"},
        17,
        0,
        "IMPHOBIA Config File",
        "CFG",
        "Darkness",
        53,0,0,6},
{  {0xB4,0x4C,0xCD,0x21,0x9D,0x89,0x64,0x6C,0x7A},
        9,
        0,
        "DIET Packed Data File",
        "DLZ",
        "",
        31,0,1,3},
{  {0x11,0xAF},
        2,
        4,
        "FLI Animation",
        "FLI",
        "Autodesk",
        49,0,1,5},
{  {0x12,0xAF},
        2,
        4,
        "FLC Animation",
        "FLC",
        "Autodesk",
        50,0,1,5},
{  {"ziRCONia"},
        8,
        0,
        "Music Module Compressor",
        "MMC",
        "Zirconia",
        33,0,0,3},
{  {"SWAGOLX.EXE"},
        11,
        0,
        "SWAG Source Code",
        "SWG",
        "",
        63,0,1,6},
{  {0,0,3,0xF3,0,0,0,0},
        8,
        0,
        "Executable file Amiga",
        "EXE",
        "Commodore",
        66,0,0,6},

{  {0xE3,0x10,0,1,0,0,0,0},
        8,
        0,
        "Information File Amiga",
        "INF",
        "",
        67,0,0,6},
{  {"AMShdr"},
        6,
        0,
        "Advanced module system",
        "AMS",
        "Extreme",  //--- Ou alors ( Velvet Development) Velvet Studio -
        68,0,1,1},
{  {"AIL3DIG"},
        7,
        0,
        "AIL 3.0 Driver",
        "DIG",
        "",
        69,0,0,6},
{  {"DMS!"},
        4,
        0,
        "DISK-Masher Archive",
        "DMS",
        "SDS Software",
        70,0,0,3},
{  {"SZDD"},
        4,
        0,
        "EXPAND compressed file",
        "___",
        "Microsoft Corp.",
        71,0,0,3},
{  {0x1F,0x8B,0x08,0x08},
        3,
        0,
        "GZIP compressed file",
        "GZ",
        "GNU",
        72,0,0,3},
{  {"MICROSOFT PIFEX"},
   15,
   0x171,
   "Pif File",
   "PIF",
   "Microsoft Corp.",
   77,0,0,6},
{  {0xBE,0,0,0,0xAB,0,0,0},
   8,
   1,
   "WRI Text",
   "WRI",
   "Microsoft Corp.",
   78,0,0,6},
{  {0x4C,0,0,0,1,0x14,2,0},
   8,
   0,
   "Link Windows 95",
   "LNK",
   "Microsoft Corp.",
   79,0,0,6},
{  {0x00,0x00,0x01,0x00,0x01,0x00,0x20,0x20,
        0x10,0x00,0x00,0x00,0x00,0x00,0xE8,0x02,
        0x00,0x00,0x16,0x00,0x00,0x00,0x28,0x00},
        24,
        0,
        "Windows Icon",
        "ICO","Microsoft Corp.",80,0,0,4},
{  {0x00,0x00,0x02,0x00,0x00,0x00,0x08,0x00    },
        8,
        1044,
        "Mac Executor Volume",
        "HFV",
        "ARDI",
        81,0,0,6},
{  {0x00,0x6A,0x00,0x00,0x00,0x0C,0x00,0x00,// 8
        0x00,0x20,0x00,0x40,0x00,0x01,0x00,0x01,// 8
        0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x43},
        24,
        9,
        "OS2 Icon",
        "ICO",
        "IBM Corp.",82,0,0,4},
{  {0x43,0x36,0x34,0x46,0x69,0x6C,0x65},
        7,
        0,
        "C64 file",
        "P00",
        "Wolfgang Lorenz",83,0,1,6},

{  {0x4D,0x56,0x20,0x2D,0x20,0x53,0x4E,0x41},
        8,
        0,
        "Snapshot CPCEMU",
        "SNA",
        "Marco Vieth",84,0,0,6},
{  {0x01,0x00,0x06,0x00,0x0B,0x00,0x01,0x00,// 8
        0x01,0x00,0x06,0x00,0x10,0x00,0x00,0x00},
        16,
        7211,
        "Snapshot Z80",
        "SNA",
        "",85,0,0,6},
{  {0x1B,0x5B},
        2,
        0,
        "Ansi File",
        "ANS",
        "",86,0,0,6},
{  {"%PDF"},
        4,
        0,
        "Acrobat Text",
        "PDF",
        "Adobe",87,0,0,6},
{  {0x50,0x57,0x41,0x44},
        4,
        0,
        "WAD file",
        "WAD",
        "ID Software",88,0,0,6},
{  {0x00,0x00,0x00,0x00,0x13,0x00,0x81,0x5A,// 8
        0x00,0x00,0x81,0x5A,0x00,0x00,0xFF,0x7F,// 8
        0x00,0x00,0x7F,0xA5,0x00,0x00,0x81,0x5A},// 8
        24,
        138,
        "FS4 Mode",
        "MOD",
        "Microsoft Corp.",89,0,0,6},
{  {0x80},
        1,
        0,
        "OBJect File",      //--- Les object sont fort mauvais ---------
        "OBJ",
        "",90,0,0,6},
{  {0x48,0x50,0x48,0x50,0x34,0x38,0x2D    },
        0x7,
        0,
        "Binary File HP48",
        "HP",
        "Hewlett Packard",92,0,0,6},
{  {0xFF,0xFF,0xFF,0xFF    }, // ú
        4,
        0x0,
        "Device Driver",
        "SYS",
        "",93,0,0,6},
{  {77,88,77,0},  // MXM
        4,
        0,
        "MXM Module",
        "MXM",
        "Cubic Team",94,0,0,1},             //--- Pascal/Cubic Team ----
{  {0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x01,// úúúúúúúú
        0x00,0x00,0x80,0x02,0x00,0x00,0xE0,0xC5}, // úú€úúúúú
        16,
        0x85,
        "Replay of Need for Speed",
        "RPL",
        "E. Arts",95,0,0,6},
{  {0x50,0x4B,0x08,0x08,0x42,0x47,0x49,0x20// PKúúBGI
                }, // PKúúBGI
        0x8,
        0,
        "CHR Font",
        "CHR",
        "Borland",96,0,0,6},
{  {"SCDH" },
        4,
        0x8,
        "Simcity 2000 Save",
        "SC2",
        "Maxis",97,0,0,6},
{  {0x70,0x6B,0x08,0x08,0x42,0x47,0x49,0x20// PKúúBGI
        }, // PKúúBGI
        0x8,
        0,
        "BGI Driver",
        "BGI",
        "Borland",98,0,0,6},
{  {0x25,0x25,0x48,0x50,0x3A,0x20,0x54,0x28,// %%HP: T(
        0x33,0x29,0x41,0x28,0x44,0x29,0x46,0x28,// 3)A(D)F(
        0x2E,0x29,0x3B    }, // .);
        0x13,
        0,
        "ASCII File HP48",
        "HP",
        "Hewlett Packard",99,0,0,6},
{  {0x50,0x53,0x31,0x36,0xFE},
        5,
        0,
        "PS16 Module",                //--- Protracker Studio 16 -------
        "P16",
        "Renaissance",100,0,0,1},   //--- Joshua C. Jensen -------------
                                    //--- aka CyberStrike/Renaissance --

{  {"KKRB"},
        4,
        0,
        "IAR Description",
        "KKR",
        "RedBug", 101, 0, 0,  6},
{  {"KKD"},
        3,
        0,
        "Description of disk",
        "KKR",
        "RedBug", 102, 0, 0, 6},
{  {"SMK"},
        3,
        0,
        "Smacker animation",
        "SMK",
        "", 103, 0, 0, 5},
{  {0x50,0x61,0x63,0x6B,0x65,0x64,0x20,0x46,0x69,0x6C,0x65,0x20    },
    12,
    0,
    "NetWare Packed File",
    "___",
    "NetWare",105,0,0,3},
{  {0x41,0x4D,0x46    },
    3,
    0,
    "DSMi Module",
    "AMF",
    "Otto Chrons",106,0,1,1},   //--- Otto Chrons  (Virtual Visions ?) -
{  {0xCE,0xED,0x66,0x66,
    0xCC,0x0D,0x00,0x0B,0x03,0x73,0x00,0x83,
    0x00,0x0C,0x00,0x0D,0x00,0x08,0x11,0x1F},
    20,
    0x104,
    "GameBoy Cartridge",
    "GB",
    "Nintendo",107,0,1,6},
{  {0x00,0x00,0x00,0x00,0x00,0x50,0x54,0x4D,// úúúúúPTM
    0x46,0x00    }, // Fú
    10,
    0x27,
    "PTM Module",
    "PTM",
    "Nostalgia",108,0,1,1},
{  {"SAdT"},
    4,
    0,
    "Surprise Adlib Tracker Module",
    "SAT",
    "Surprise!",109,0,0,1},
{  {"PKM"},
    3,
    0,
    "PKM Bitmap",
    "PKM",
    "Karl Maritaud",110,0,1,4},
{  {0x49,0x4D,0x50,0x53}, // IMPS
    4,
    0,
    "Impulse Tracker Sample",
    "ITS",
    "Impulse",111,0,1,2},
{  {0x53,0x43,0x52,0x53,0x80}, // SCRS€
    5,
    0x4c,
    "Scream Tracker Sample",
    "DP3",
    "Future Crew",112,0,1,2},
{  {"DDSF"},
    4,
    0,
    "X-Tracker Sample",
    "DSF",
    "?",113,0,1,2},
{  {"MUWFD"},
    5,
    0x22,
    "Ultratracker Sample",
    "UWF",
    "Mysteriois MAS",114,0,1,2},
{  {"AMS"},
    3,
    0,
    "Audio Manager Sample",
    "AMS",
    "?",115,0,1,2},
{  {"FC14"},
    4,
    0,
    "Futur Composer Music",
    "FC4",
    "?",116,0,0,1},
{  {"PACG"},
    4,
    0,
    "SBStudio Module",
    "PAC",
    "Henning Hellstr”m",117,0,0,1},
{  {"ustar"},
    5,
    0x101,
    "TAR Archive",
    "TAR",
    "?",118,0,0,3},
{  {"JCH"},
    3,
    0,
    "EdLib compressed module",   //--- Edlib Compresse -----------------
    "D00",
    "Jens Christian Huus",119,0,0,1},  // Jean Christian Huus / Vibrants
{ {0x00,0x06,0xFE,0xFD    }, // úúúú
    4,
    0x0,
    "EdLib module",   //--- Edlib Non Compresse ------------------------
    "EDL",
    "Jens Christian Huus",132,0,0,1},  // Jean Christian Huus / Vibrants
{  {"AST 0001"},
    8,
    1,
    "All Sound Tracker Module",
    "AST",
    "Cagliostro",120,0,1,1},   //--- Patrice Bouchand a.k.a Cagliostro -
{  {'D','S','M',0x10},
    4,
    0,
    "DigiSound Module",
    "DSM",          //--- Used in a music disk from Necros (in Ace) ----
    "Pelusa",121,0,1,1},        // Carlos Hasan aka Pelusa/Psychik Monks
                                 //--- (chasan@dcc.uchile.cl) ----------
{  {0x50,0x53,0x49,0x44,0x00    },
    5,
    0,
    "PlaySid Module",
    "DAT",
    "?",122,0,1,1},
{  {0x00,0xE8,0x02,0x00,0x00,0x16,0x00,0x00,// úúúúúúúú
    0x00,0x28,0x00,0x00,0x00,0x20,0x00,0x00},// ú(úúú úú
    16,
    0xd,
    "Win95 Cursor",
    "CUR",
    "Microsoft Corp.",123,0,0,4},
{  {0xFF,0xFF,0x00,0x01,0x64,0x00,0x00,0x00,// úúúúdúúú
    0x03,0x00    }, // úú
    10,
    0x0,
    "Alpha Microsystems BMP", // No information(conversion from Alchemy)
    "BMP",
    "Alpha Microsystems",124,0,0,4},
{  {0x00,0x11,0x02,0xFF,0x0C,0x00,0xFF,0xFE,// úúúúúúúú
    0x00,0x00,0x00,0x48,0x00,0x00,0x00,0x48,// úúúHúúúH
    0x00,0x00,0x00,0x00,0x00,0x00,0x01    }, // úúúúúúú
    23,
    0x20a,
    "Macintosh PICT",
    "PIC",
    "?",125,0,0,4},
{  {"LZANIM"},
    6,
    0,
    "LZA animation",
    "LZA",
    "goto64",126,0,1,5},        // Brian Strack Jensen aka goto64/Purple
{  {"RAD by REALiTY!!"},
    16,
    0,
    "Reality ADlib Tracker Module",
    "RAD",
    "Reality",130,0,0,1},      //--- Shayde/Reality (?) ----------------
{  {0xFC,0x00,0x01,0x00,0x0C,0x00,0x81,0x01,// úúúúúúúú
    0x82,0x01,0x06,0x00,0x01,0x02,0x03,0x04,// úúúúúúúú
    0x05,0x08,0x10    }, // úúú
    19,
    0x0,
    "QuickBasic Listing",
    "BAS",
    "Microsoft Corp.",131,0,0,6},
{  {"DISPTNL"},
    7,
    0,
    "Thumbnail Picture",
    "TNL",
    "Jih-Shin Ho",133,0,1,4},
{  {"hsi"},
     3,
     0,
     "HSI Jpeg bitmap",
     "HSI",
     "Handmade Software, Inc",   //--- HSI (Creator of alchemy) --------
     134,0,1,4},
{  {0xFF,0xFD,0x70},
     3,
     0,
     "MPEG 1 Audio Layer 2",
     "MP2",
     "ISO",
     135,0,0,2},
{  {0xFF,0xFB,0x90},
     3,
     0,
     "MPEG 1 Audio Layer 3",
     "MP3",
     "ISO",
     136,0,0,2},
{  {0x63,0x6F,0x64,0x65,0x00,0x01,0x00    }, // codeúúú
    7,
    0x4e,
    "Executable file Us Pilot",
    "PRC",
    "US Robotics",137,0,1,6},



// Dernier employe: 137

/*--------------------------------------------------------------------*\
|-              structures … traiter en dernier ressort               -|
\*--------------------------------------------------------------------*/
{  {0x34,0x12},
    2,
    0,
    "PCPaint/Pictor file",
    "PIC",
    "John Bridges",
    129,0,0,4},
{  {0x00,0x95},
    2,
    4,
    "Animator Pro PIC File",
    "PIC",
    "Autodesk",
    127,0,0,4},
{  {0x19,0x91},
    2,
    0,
    "Original Animator PIC File",
    "PIC",
    "Autodesk",
    128,0,0,4},
{  {0,0},
    0,
    0,
    "Targa Picture",
    "TGA",
    "Truevision, Inc.",
    43,1,1,4},
{  {0x00,0x01,0x00,0x00},
    4,
    0,
    "True Type Font",
    "TTF",
    "",
    65,0,0,6},
{  {"PK"},
    2,
    0,
    "ZIP Archive",
    "ZIP",
    "PKWARE Inc.",
    35,0,0,3},
{  {0,0},
    0,
    0,
    "HTML File",
    "HTM",
    "",
    104,1,0,6},
{  {0,0},
    2,
    0,
    "Text File",
    "TXT",
    "",
    91,1,0,6},    //--- Laisser celui-ci dernier -----------------------

/*--------------------------------------------------------------------*\
|-                    structures … ne pas toucher                     -|
\*--------------------------------------------------------------------*/

{  {0,0},
        2,
        0,
        "Module",
        "*",
        "?",
        -1,0,0,1},
{  {0,0},
        2,
        0,
        "Sample",
        "*",
        "?",
        -1,0,0,2},
{  {0,0},
        2,
        0,
        "Archive",
        "*",
        "?",
        -1,0,0,3},
{  {0,0},
        2,
        0,
        "Bitmap",
        "*",
        "?",
        -1,0,0,4},
{  {0,0},
        2,
        0,
        "Animation",
        "*",
        "?",
        -1,0,0,5},
{  {0,0},
        2,
        0,
        "Others",
        "*",
        "?",
        -1,0,0,6}
};


char *GetFile(char *p,char *Ficname);
void SplitName(char *filename,char *name,char *ext);

void Size2Chr(int Size,char *Taille);
                              // transforme la taille indiqu‚e en chaine

short Infotxt(RB_IDF *Info);   //--- Test pour voir si c'est du texte --
short Infomtm(RB_IDF *Info);
short Infoexe1(RB_IDF *Info);
short Infopsm(RB_IDF *Info);
short Info669(RB_IDF *Info);
short Infomod(RB_IDF *Info);   // Nø1
short Infodmf(RB_IDF *Info);
short Infoxm(RB_IDF *Info);    // Nø2
short Infoxi(RB_IDF *Info);
short Infos3m(RB_IDF *Info);   // Nø3
short Infout(RB_IDF *Info);
short Inforar(RB_IDF *Info);
short Infogif(RB_IDF *Info);
short Infopcx(RB_IDF *Info);
short Infobmp(RB_IDF *Info);
short Infookt(RB_IDF *Info);
short Infoau(RB_IDF *Info);
short Infoiff(RB_IDF *Info);
short Infolbm(RB_IDF *Info);
short Infomid(RB_IDF *Info);
short Infoarj(RB_IDF *Info);
short Infoit(RB_IDF *Info);
short Infomed(RB_IDF *Info);
short Infovoc(RB_IDF *Info);
short Infotpu(RB_IDF *Info);
short Infocmf(RB_IDF *Info);
short Infohlp(RB_IDF *Info);
short Infostm(RB_IDF *Info);
short Infofar(RB_IDF *Info);
short Infofsm(RB_IDF *Info);
short Infousm(RB_IDF *Info);
short Infofpt(RB_IDF *Info);
short Infof2r(RB_IDF *Info);
short Infomdl(RB_IDF *Info);
short Infospl(RB_IDF *Info);
short Infottf(RB_IDF *Info);
short Infogrp(RB_IDF *Info);
short Infotga(RB_IDF *Info);
short Infompg(RB_IDF *Info);
short Infodlz(RB_IDF *Info);
short Infofli(RB_IDF *Info);
short Infolza(RB_IDF *Info);
short Infoflc(RB_IDF *Info);
short Infoswg(RB_IDF *Info);
short Infoams(RB_IDF *Info);
short Infot64(RB_IDF *Info);
short Infop00(RB_IDF *Info);
short Infoamf(RB_IDF *Info);
short Infopkm(RB_IDF *Info);
short Infogb(RB_IDF *Info);
short Infoptm(RB_IDF *Info);
short Infohtm(RB_IDF *Info);
short Inforaw(RB_IDF *Info);
short Infoits(RB_IDF *Info);
short Infodp3(RB_IDF *Info);
short Infopat(RB_IDF *Info);
short Infodsf(RB_IDF *Info);
short Infouwf(RB_IDF *Info);
short Infoams2(RB_IDF *Info);
short Infoast(RB_IDF *Info);
short Infodsm(RB_IDF *Info);
short Infodat(RB_IDF *Info);
short Infotnl(RB_IDF *Info);
short Infohsi(RB_IDF *Info);
short Infoprc(RB_IDF *Info);


void ClearSpace(char *name);    //--- efface les espaces inutiles ------

ULONG ReadLng(RB_IDF *Info,ULONG position,char type);
WORD ReadInt(RB_IDF *Info,ULONG position,char type);
void ReadStr(RB_IDF *Info,ULONG position,char *str,short taille);

ULONG InvLong(ULONG entier);           // Inverse un ULONG HILO <-> LOHI
WORD InvWord(WORD entier);              // Inverse un WORD HILO <-> LOHI


char buffer[32768];

char tampon[1024];                   // Tampon pour faire n'importe quoi

/*--------------------------------------------------------------------*\
|-                        FIN DES DECLARATIONS                        -|
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|- positon par rapport … posfic                                       -|
|- type =1 LoHi --> PC                                                -|
|-      =2 HiLo --> Amiga                                             -|
\*--------------------------------------------------------------------*/
ULONG ReadLng(RB_IDF *Info,ULONG position,char type)
{
ULONG entier;
int pos;
char *a;
char *b;
ULONG result;

if (position>(Info->posbuf+Info->sizebuf+4)) {
        pos=ftell(Info->fic);
        fread(&entier,4,1,Info->fic);
        fseek(Info->fic,pos,SEEK_SET);
                }
        else
        entier=*(ULONG*)(Info->buffer+(WORD)position);

if (type==1) return entier;
if (type==2) {
        result=entier;
        a=(char*)&entier;
        b=(char*)&result;

        b[3]=a[0];
        b[2]=a[1];
        b[1]=a[2];
        b[0]=a[3];

        return result;
        }
return -1;
}

/*--------------------------------------------------------------------*\
|- positon par rapport … posfic                                       -|
|- type =1 LoHi --> PC                                                -|
|-      =2 HiLo --> Amiga                                             -|
\*--------------------------------------------------------------------*/
WORD ReadInt(RB_IDF *Info,ULONG position,char type)
{
WORD entier;
char *a;
char *b;
WORD result;
int pos;


if (position>(Info->posbuf+Info->sizebuf+2))
    {
    pos=ftell(Info->fic);
    fread(&entier,2,1,Info->fic);
    fseek(Info->fic,pos,SEEK_SET);
    }
else
    {
    entier=*(WORD*)(Info->buffer+(WORD)position);
    }

if (type==1)
    return entier;
if (type==2)
    {
    result=entier;
    a=(char*)&entier;
    b=(char*)&result;

    b[1]=a[0];
    b[0]=a[1];

    return result;
    }

return -1;
}

/*--------------------------------------------------------------------*\
|-  positon par rapport … posfic                                      -|
\*--------------------------------------------------------------------*/
void ReadStr(RB_IDF *Info,ULONG position,char *str,short taille)
{
int pos;

if (position>(Info->posbuf+Info->sizebuf+taille)) {
        pos=ftell(Info->fic);
        fread(str,taille,1,Info->fic);
        fseek(Info->fic,pos,SEEK_SET);
         }
        else
        memcpy(str,Info->buffer+(WORD)position,taille);


str[taille]=0;
ClearSpace(str);
}


/*--------------------------------------------------------------------*\
|-  Inverse un ULONG (hihilolo -> lolohihi)                           -|
\*--------------------------------------------------------------------*/
ULONG InvLong(ULONG entier)
{
char *a;
char *b;
ULONG result;

result=entier;
a=(char*)&entier;
b=(char*)&result;

b[3]=a[0];
b[2]=a[1];
b[1]=a[2];
b[0]=a[3];

return result;

}

/*--------------------------------------------------------------------*\
|-  Inverse un WORD (hihilolo -> lolohihi)                            -|
\*--------------------------------------------------------------------*/
WORD InvWord(WORD entier)
{
char *a;
char *b;
WORD result;

result=entier;
a=(char*)&entier;
b=(char*)&result;

b[1]=a[0];
b[0]=a[1];

return result;
}



void SplitName(char *filename,char *name,char *ext)     // name:8, ext:3
{
short n;

if (name!=NULL) memcpy(name,filename,8);
strcpy(ext,"");

for (n=strlen(filename);n>=0;n--)
    {
    if ( (filename[n]=='.') & (strlen(filename+n+1)<=3) & (ext!=NULL) )
        strcpy(ext,filename+n+1);

    if ( (filename[n]=='\\') & (name!=NULL) )
        {
        memcpy(name,filename+n+1,8);
        break;
        }
    }

if (name!=NULL)
    {
    name[8]=0;
    for (n=0;n<strlen(name);n++)
        if (name[n]=='.')
            name[n]=0;
    }
}


void Traitefic(RB_IDF *Info)
{
short n;
FILE *fic;
short err;

char path[256];

short trv=-1;     //--- vaut -1 tant que l'on a rien trouv‚ ------------

Info->numero=-1;

strcpy(path,Info->path);

fic=fopen(path,"rb");
if (fic==NULL) return;

memset(buffer,0,32768U);

fread(buffer,32768U,1,fic);

n=0;    //--- recherche dans tous les formats --------------------------

memset(Info,0,sizeof(RB_IDF));
strcpy(Info->path,path);
Info->fic=fic;
Info->posfic=0L;

GetFile(Info->path,Info->filename);

Info->buffer=buffer;    //--- buffer pour E/S --------------------------
Info->posbuf=Info->posfic;
Info->sizebuf=32768U;

for (n=0;n<nbrkey-6;n++)  //--- Il faut ignorer les 6 derniers clefs ---
    {
    if (K[n].proc==0)
        if (!memcmp(buffer+K[n].pos,K[n].buf,K[n].len)) trv=n;

    if ( (K[n].proc==1) | ((K[n].other==1) & (trv!=-1)) )
        {
        switch(K[n].numero)
            {
            case  2: err=Infoxm (Info); break;
            case 83: err=Infop00(Info); break;
            case 19: err=Infout (Info); break;
            case  3: err=Infos3m(Info); break;
            case  6: err=Infodmf(Info); break;
            case  4: err=Info669(Info); break;
            case 15: err=Infomtm(Info); break;
            case 34: err=Inforar(Info); break;
            case 37: err=Infogif(Info); break;
            case 40: err=Infopcx(Info); break;
            case 36: err=Infobmp(Info); break;
            case 73: err=Inforaw(Info); break;
            case 16: err=Infookt(Info); break;
            case 21: err=Infoau (Info); break;
            case 23: err=Infoiff(Info); break;
            case 39: err=Infolbm(Info); break;
            case 10: err=Infoit (Info); break;
            case 14: err=Infomid(Info); break;
            case 30: err=Infoarj(Info); break;
            case 13: err=Infomed(Info); break;
            case 27: err=Infovoc(Info); break;
            case  1: err=Infomod(Info); break;
            case 17: err=Infopsm(Info); break;
            case 57: err=Infoexe1(Info);break;
            case 29: err=Infoxi (Info); break;
            case 64: err=Infotpu(Info); break;
            case  5: err=Infocmf(Info); break;
            case 59: err=Infohlp(Info); break;
            case 18: err=Infostm(Info); break;
            case  8: err=Infofar(Info); break;
            case 22: err=Infofsm(Info); break;
            case 26: err=Infousm(Info); break;
            case  9: err=Infofpt(Info); break;
            case  7: err=Infof2r(Info); break;
            case 12: err=Infomdl(Info); break;
            case 25: err=Infospl(Info); break;
            case 58: err=Infogrp(Info); break;
            case 43: err=Infotga(Info); break;
            case 52: err=Infompg(Info); break;
            case 31: err=Infodlz(Info); break;
            case 49: err=Infofli(Info); break;
            case 50: err=Infoflc(Info); break;
            case 63: err=Infoswg(Info); break;
            case 68: err=Infoams(Info); break;
            case 76: err=Infot64(Info); break;
            case 91: err=Infotxt(Info); break;
            case 104:err=Infohtm(Info); break;
            case 106:err=Infoamf(Info); break;
            case 107:err=Infogb(Info); break;
            case 108:err=Infoptm(Info); break;
            case 110:err=Infopkm(Info); break;
            case 111:err=Infoits(Info); break;
            case 112:err=Infodp3(Info); break;
            case 113:err=Infodsf(Info); break;
            case 114:err=Infouwf(Info); break;
            case 115:err=Infoams2(Info); break;
            case 120:err=Infoast(Info); break;
            case 121:err=Infodsm(Info); break;
            case 122:err=Infodat(Info); break;
            case 24:err=Infopat(Info); break;
            case 126:err=Infolza(Info); break;
            case 133:err=Infotnl(Info); break;
            case 134:err=Infohsi(Info); break;
            case 137:err=Infoprc(Info); break;
            default:     //--- Ca serait une erreur de ma part alors ---
                sprintf(Info->format,"Pingouin %d",K[n].numero);
                trv=1;
                break;
            }
        if (err==0) trv=n;
        }
    if (trv!=-1) break;
    }

if (trv==-1)
    {
    n=-2; //--- format non reconnu -------------------------------------

    strcpy(Info->format,"Unknown Format");
    SplitName(Info->filename,NULL,Info->ext);

    Info->Btype=0;
    }
    else
    {
    if (*Info->format==0)
        strcpy(Info->format,K[trv].format);
    if (*Info->ext==0)
        strcpy(Info->ext,K[trv].ext);

    n=K[trv].numero;
    Info->Btype=K[trv].type;
    }

if (*Info->fullname==0)
    strcpy(Info->fullname,Info->filename);

ClearSpace(Info->fullname);

Info->posfic+=Info->taille;
Info->numero=n;

fclose(fic);
}


void ClearSpace(char *name)
{
char c,buf[1024];
short i,j;

i=0;    //--- navigation dans name -------------------------------------
j=0;    //--- position dans buf ----------------------------------------

while ( (name[i]==32) & (name[i]!=0) ) i++;

if (name[i]!=0)
    while ((c=name[i])!=0)
        {
        buf[j]=name[i];
        j++;
        i++;
        if (name[i]==32)
            {
            c=name[i];
            while ( (name[i]==32) & (name[i]!=0) )
                {
                if (name[i]=='=') c='=';
                i++;
                }
            buf[j]=c;
            j++;
            }
        }
buf[j]=0;

if (buf[j-1]==32)
    buf[j-1]=0;

strcpy(name,buf);
}



short Infomod(RB_IDF *Info)
{
short chnl,instr;
char *buf;

buf=Info->buffer;

chnl=0;

if (!memcmp(buf+1080,"TDZ1",4)) { strcpy(Info->format,"TakeTracker");
                                                     chnl=1; instr=31; }
if (!memcmp(buf+1080,"TDZ2",4)) { strcpy(Info->format,"TakeTracker");
                                                     chnl=2; instr=31; }
if (!memcmp(buf+1080,"TDZ3",4)) { strcpy(Info->format,"TakeTracker");
                                                     chnl=3; instr=31; }
if (!memcmp(buf+1080,"M.K.",4)) { strcpy(Info->format,"Protracker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf+1080,"M&K&",4)) { strcpy(Info->format,"Noisetracker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf+1080,"M!K!",4)) { strcpy(Info->format,"Protracker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf+1080,"4CHN",4)) { strcpy(Info->format,"Protracker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf+1080,"6CHN",4)) { strcpy(Info->format,"PC-FTracker");
                                                     chnl=6; instr=31; }
if (!memcmp(buf+1080,"8CHN",4)) { strcpy(Info->format,"PC-FTracker");
                                                     chnl=8; instr=31; }
if (!memcmp(buf+1080,"CD81",4)) { strcpy(Info->format,"Octalyser");
                                                     chnl=8; instr=31; }
       //--- octalyser on Atari Ste/falcon nombre de channel ? ---------
if (!memcmp(buf+1080,"OCTA",4)) { strcpy(Info->format,"Oktotracker");
                                                     chnl=8; instr=31; }
if (!memcmp(buf+1080,"FLT4",4)) { strcpy(Info->format,"StarTrekker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf+1080,"RASP",4)) { strcpy(Info->format,"StarTrekker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf+1080,"FLT8",4)) { strcpy(Info->format,"StarTrekker");
                                                     chnl=8; instr=31; }
if (!memcmp(buf+1080,"EXO4",4)) { strcpy(Info->format,"StarTrekker");
                                                     chnl=8; instr=31; }
                                                         // EXO OU EX0 ?
if (!memcmp(buf+1080,"EXO8",4)) { strcpy(Info->format,"StarTrekker");
                                                     chnl=8; instr=31; }
if (!memcmp(buf+1080,"FA04",4))
      { strcpy(Info->format,"Digital Tracker F030"); chnl=4; instr=31; }
if (!memcmp(buf+1080,"FA08",4))
      { strcpy(Info->format,"Digital Tracker F030"); chnl=8; instr=31; }

if (chnl==0)  return 1;

strcat(Info->format," module");

ReadStr(Info,0,Info->fullname,20);

sprintf(Info->info, "%3d /%3d /%3d",instr,buf[950],chnl);
strcpy(Info->Tinfo,"Inst/Patt/Chnl");

return 0;
}

short Infodmf(RB_IDF *Info)
{
ReadStr(Info,13,Info->fullname,30);
ReadStr(Info,43,Info->composer,20);
// ReadStr(Info,5,Info->format,8); // Nom du tracker

// pour le nombre de sample, il faut rechercher le chunk 'CMPI'.
//      L'octet suivant est le nombre de sample (MAXSAMPLE)

/* strcpy(Info[3].help,"INFO");
sprintf(Info[3].txt, "%3d /%3d /%3d",instr,buf[950],chnl);
strcpy(Info[3].title,"Inst/Patt/Chnl"); */

return 0;
}

short Infoit(RB_IDF *Info)
{
char *buf;

buf=Info->buffer;
ReadStr(Info,4,Info->fullname,26);

sprintf(Info->format,
                  "Impulse Tracker %d.%02d module",buf[0x29],buf[0x28]);

sprintf(Info->info, "%3d /%3d /%3d",
           *(short*)(buf+0x22),*(short*)(buf+0x26),*(short*)(buf+0x20));
strcpy(Info->Tinfo,"Inst/Patt/Orde");

return 0;
}

short Infomid(RB_IDF *Info)
{
char chunk[5];
int lng,pos;
short info;
short mess;



sprintf(Info->Tinfo, "Tracks");
sprintf(Info->info,"%3d",ReadInt(Info,10,2));

fseek(Info->fic,Info->posfic,SEEK_SET);


mess=-1;

pos=0;
lng=0;

while(1)
        {
        fseek(Info->fic,pos+lng,SEEK_SET);
        if (fread(chunk,4,1,Info->fic)!=4) break;
        if (!((!strncmp(chunk,"MThd",4)) | (!strncmp(chunk,"MTrk",4))))
                                                                  break;

        fread(&lng,4,1,Info->fic);
        pos=ftell(Info->fic);
        lng=InvLong(lng);

        fread(&info,2,1,Info->fic);
        info=InvWord(info);
        if      (info==0xFF)
                {
                char chaine[40];
                char strlng;

                fread(&strlng,1,1,Info->fic);    //--- lit le type -----
                if (strlng!=3) continue;

                fread(&strlng,1,1,Info->fic);
                if (strlng>34) continue;
                fread(chaine,strlng,1,Info->fic);
                chaine[strlng]=0;

                if (mess==-1)
                        sprintf(Info->fullname,"%s",chaine);

//              sprintf(Info->message[mess]," %s",chaine);
                mess++;
//              if (mess==10) mess=9;
                }
        }

Info->taille=(pos+lng)-Info->posfic;

return 0;
}


short Infomtm(RB_IDF *Info)
{

char *buf;

buf=Info->buffer;

ReadStr(Info,4,Info->fullname,20);

sprintf(Info->info, "%3d /%3d",buf[27]+1,buf[33]);
strcpy(Info->Tinfo,"SngL/Chnl"); // sngl = songlength

// le nombre de channel est contraire … la doc
// le nombre de sample aussi !!!

return 0;
}

short Info669(RB_IDF *Info)
{
char *buf;

buf=Info->buffer;
if ( (memcmp(buf,"if",2)!=0) & (memcmp(buf,"JN",2)!=0) )
    return 1;

ReadStr(Info, 2,Info->message[0],36); //--- 108 octets en r‚alit‚ ------
ReadStr(Info,38,Info->message[1],36); //--- 108 octets en r‚alit‚ ------
ReadStr(Info,74,Info->message[2],36); //--- 108 octets en r‚alit‚ ------

if (!memcmp(buf,"if",2)) // 0x6669
    strcpy(Info->format,"669 Module");
    else
    strcpy(Info->format,"Extended 669 Module");

sprintf(Info->info, "%3d /%3d /%3d",buf[0x6E],buf[0x6F],8);
strcpy(Info->Tinfo,"Inst/Patt/Chnl");

return 0;
}

short Infoxm(RB_IDF *Info)
{
char bname[25];

char *buf;

buf=Info->buffer;

ReadStr(Info,17,Info->fullname,20);

ReadStr(Info,38,bname,20);
sprintf(Info->format,"Module %s",bname);

sprintf(Info->info, "%3d /%3d /%3d",*(short*)(buf+72),
                                   *(short*)(buf+64),*(short*)(buf+68));
strcpy(Info->Tinfo,"Inst/Patt/Chnl");

return 0;
}



short Infos3m(RB_IDF *Info)
{
short n,chnl;

char *buf;

buf=Info->buffer;

memcpy(Info->fullname,buf,28);
Info->fullname[28]=0;

chnl=0;
for (n=0;n<32;n++)      if (buf[0x40+n]!=-1) chnl++;

sprintf(Info->info, "%3d /%3d /%3d",
                          *(short*)(buf+0X22),*(short*)(buf+0X24),chnl);
strcpy(Info->Tinfo,"Inst/Orde/Chnl");

return 0;
}

short Infout(RB_IDF *Info)
{
short chnl,instr,patt;
char version[5];
short res,patt_seq;

short sizestruct; //--- taille de la structure sample ------------------

char *buf;

buf=Info->buffer;


if (memcmp(buf,"MAS_UTrack",10))
        return 1;
        else
        switch(buf[14])
            {
            case '1': strcpy(version,"1.0"); sizestruct=64; break;
            case '2': strcpy(version,"1.4"); sizestruct=64; break;
            case '3': strcpy(version,"1.5"); sizestruct=64; break;
            case '4': strcpy(version,"1.6"); sizestruct=66; break;
            default:  strcpy(version,">1.6");sizestruct=66; break;
            }

res=buf[47]*32;
instr=buf[48+res];
patt_seq=49+(instr*sizestruct)+res;// la doc dit 48+ mais elle se trompe
patt=buf[patt_seq+257];
chnl=buf[patt_seq+256];



memcpy(Info->fullname,buf+15,32);
Info->fullname[32]=0;

strcpy(Info->format,"Ultratracker v");
strcat(Info->format,version);
strcat(Info->format," module");


sprintf(Info->info, "%3d /%3d /%3d",instr,patt,chnl);
strcpy(Info->Tinfo,"Inst/Patt/Chnl");

return 0;
}


/*--------------------------------------------------------------------*\
|- FICHIER .RAR                                                       -|
\*--------------------------------------------------------------------*/
struct dos_temps {
 unsigned t_tsec : 5;
 unsigned t_min : 6;
 unsigned t_heure : 5;
 unsigned t_day : 5;
 unsigned t_mois : 4;
 unsigned t_year : 7;
};


struct FileEntete
{
 WORD TeteCRC;
 BYTE TeteType;
 WORD Flags;
 WORD TeteSize;
 ULONG PackSize;
 ULONG UnpSize;
 BYTE HostOS;
 ULONG FileCRC;
 struct dos_temps Filetemps;
 BYTE UnpVer;
 BYTE Method;
 WORD NomSize;
 ULONG FileAttr;
};

short Inforar(RB_IDF *Info)
{
char Nomarch[256];
char buffer[256];
struct FileEntete Lt;
short SolidType;

char *buf;

buf=Info->buffer;


// Test la solidit‚ ;) de l'archive

memcpy(&Lt,buf+7,13);
SolidType=(Lt.Flags & 8);               // &MHD_SOLID

// Okay.

strcpy(buffer,"");
if (SolidType) strcat(buffer,"Solid ");

if (Lt.Flags & 1)       //--- MHD_MULT_VOL -----------------------------
                sprintf(Nomarch,"%colume ",(SolidType) ? 'v':'V');
                else
                sprintf(Nomarch,"%crchive ",(SolidType) ? 'a':'A');

strcat(buffer,Nomarch);

sprintf(Info->message[0],"  Archive type: %20s",buffer);

return 0;
}

short Infogif(RB_IDF *Info)
{
char car;
short Lp,Hp;
char map,fond;
char PG,BP;

char *buf;

buf=Info->buffer;



if (strncmp(buf,"GIF8",4)) return 1;
if (buf[5]!='a') return 1;

sprintf(Info->format,"Compuserve GIF8%ca",buf[4]);

fseek(Info->fic,Info->posfic+6,SEEK_SET);

fread(&Lp,2,1,Info->fic);
fread(&Hp,2,1,Info->fic);

fread(&map,1,1,Info->fic);
PG=(map&128)>>7;
BP=(map&7)+1;

if (Lp>9999) Lp=9999;
if (Hp>9999) Hp=9999;
if (BP>99) BP=99;


sprintf(Info->message[0],"  Picture is    %4d * %4d / %2dBps",Lp,Hp,BP);



fread(&fond,1,1,Info->fic);
fread(&car,1,1,Info->fic);

if (PG==1)
        fseek(Info->fic,(1<<BP)*3,SEEK_CUR);  // palette

// while((car=fgetc(fic))!=';')

do {
fread(&car,1,1,Info->fic);

// PrshortPos(10,0,"'%c':",car);
switch(car)
    {
    case '!':   //--- Information --------------------------------------
        {
        short n;
        BYTE lng;
        short mess;

        fread(&car,1,1,Info->fic);  //--- code information -------------

        mess=1;

        do
            {
            fread(&lng,1,1,Info->fic);

            if ( (car==-1) & (lng==11) ) //--- application names -------
                {
                char appl[11];

                fread(appl,11,1,Info->fic);   //--- comment ------------
                appl[8]=0;

                ClearSpace(appl);

                for(n=0;n<strlen(appl);n++)
                    if (appl[n]<32)
                        break;

                if (appl[n]==0)
                    {
                    sprintf(Info->message[mess],
                                       "  Application name: %15s",appl);
                    mess++;
                    }
                }
            else
            fseek(Info->fic,lng,SEEK_CUR);       // passe information
            }
        while(lng!=0);
        car=0;
        break;
        }
    case',':  //--- DATA -----------------------------------------------
        {
        short DX,DY,TX,TY;
        char PL,BP;
        unsigned char Code_size,t;

        fread(&DX,2,1,Info->fic);
        fread(&DY,2,1,Info->fic);
        fread(&TX,2,1,Info->fic);
        fread(&TY,2,1,Info->fic);

        fread(&map,1,1,Info->fic);

        PL=(map&128)>>7;
        BP=(map&7)+1;

        if (PL==1)      //--- tester -----------------------------------
            fseek(Info->fic,(1<<BP)*3,SEEK_CUR);              // palette

        fread(&Code_size,1,1,Info->fic);

        do
            {
            fread(&t,1,1,Info->fic);
            fseek(Info->fic,t,SEEK_CUR);                      // palette
            }
        while (t!=0);

        }break;
    }  //--- fin du case -----------------------------------------------
}      //--- fin du while (buf) ----------------------------------------
while(car!=';');

Info->taille=(ftell(Info->fic)-Info->posfic);

return 0;
}

short Infopcx(RB_IDF *Info)
{
short Lp,Hp;
char BP;

Lp=ReadInt(Info,8,1)+1;
Hp=ReadInt(Info,10,1)+1;

BP=Info->buffer[3];

sprintf(Info->message[0]," Picture is    %4d * %4d / %2dBps",Lp,Hp,BP);

return 0;
}

short Infotnl(RB_IDF *Info)
{
long Lp,Hp;

Lp=ReadLng(Info,8,1);
Hp=ReadLng(Info,12,1);

sprintf(Info->message[0]," Picture is             %4d * %4d",Lp,Hp);

Lp=ReadLng(Info,16,1);
Hp=ReadLng(Info,20,1);

sprintf(Info->message[1]," Original picture is    %4d * %4d",Lp,Hp);

return 0;
}

short Infobmp(RB_IDF *Info)
{
char *buf;

buf=Info->buffer;


memcpy(&(Info->taille),buf+2,4);   // Ordre des bytes pour le int: LO-HI

return 0;
}

short Infopkm(RB_IDF *Info)
{
short Lp,Hp;

Lp=ReadInt(Info,6,1);
Hp=ReadInt(Info,8,1);

sprintf(Info->message[0]," Picture is    %4d * %4d / 8Bps",Lp,Hp);

return 0;
}

short Inforaw(RB_IDF *Info)
{
WORD Lp,Hp,Bp;
char ok=0;

Lp=ReadInt(Info,8,2);
Hp=ReadInt(Info,10,2);
Bp=ReadInt(Info,12,2);

if ((Bp==0) | (Bp==-24))    Bp=24;
    else
    if (Bp==256)  Bp=8;
        else
        ok=1;

if (ok==0)
    sprintf(Info->message[0],
                         " Picture is    %4d * %4d / %2d Bps",Lp,Hp,Bp);
    else
    sprintf(Info->message[0],
                          " Picture is %4d * %4d  (%3d col.)",Lp,Hp,Bp);

return 0;
}

short Infohsi(RB_IDF *Info)
{
WORD Lp,Hp;

Lp=ReadInt(Info,6,2);  //--- Motorola mode -----------------------------
Hp=ReadInt(Info,8,2);

sprintf(Info->message[0]," Picture is    %4d * %4d / 24 Bps",Lp,Hp);

return 0;
}

short Infoprc(RB_IDF *Info)
{
ReadStr(Info,0,Info->fullname,32);

return 0;
}

short Infookt(RB_IDF *Info)
{
short chnl,inst,patt;
ULONG pos;
char chunk[5];
char cont;

fseek(Info->fic,Info->posfic+8,SEEK_SET);

patt=0;
inst=0;
chnl=8;

do
{
if (fread(chunk,4,1,Info->fic)!=4) break;

cont=0;

if (!memcmp(chunk,"SBOD",4)) cont=1;
if (!memcmp(chunk,"PBOD",4)) cont=2;
if (!memcmp(chunk,"PATT",4)) cont=3;
if (!memcmp(chunk,"SPEE",4)) cont=4;
if (!memcmp(chunk,"PLEN",4)) cont=5;
if (!memcmp(chunk,"SAMP",4)) cont=6;
if (!memcmp(chunk,"SLEN",4)) cont=7;
if (!memcmp(chunk,"CMOD",4)) cont=8;

if (cont==0)
    {
    fseek(Info->fic,-4,SEEK_CUR);
    break;
    }

fread(&pos,4,1,Info->fic);
pos=InvLong(pos);

if (cont==1) inst++;

if (cont==5)
    {
    fread(&patt,2,1,Info->fic);
    patt=InvWord(patt);
    fseek(Info->fic,-2,SEEK_CUR);
    }

fseek(Info->fic,pos,SEEK_CUR);
}
while(1);


sprintf(Info->info, "%3d /%3d /%3d",inst,patt,chnl);
strcpy(Info->Tinfo,"Inst/Patt/Chnl");

Info->taille=(ftell(Info->fic)-Info->posfic);


return 0;
}

short Infoau(RB_IDF *Info)
{
ULONG dataLocation,dataSize,dataFormat,samplingRate;
char *text;

char *buf;

buf=Info->buffer;



if (memcmp(buf,".snd",4))   return 1;

memcpy((char*)(&dataLocation),buf+4,4);
dataLocation=InvLong(dataLocation);

memcpy((char*)(&dataSize),buf+8,4);
dataSize=InvLong(dataSize);

memcpy((char*)(&dataFormat),buf+12,4);
dataFormat=InvLong(dataFormat);

memcpy((char*)(&samplingRate),buf+16,4);
samplingRate=InvLong(samplingRate);

text=buf+24;


sprintf(Info->message[0], "  Sampling rate: %15d Hz",samplingRate);
if (dataFormat==1)
        sprintf(Info->message[1],"  Format:      8-bit mu-law samples");

if (strlen(text)<30) strcpy(Info->message[9],text);

Info->taille=dataSize+dataLocation;

return 0;
}

short Infoiff(RB_IDF *Info)
{
ULONG pos;
WORD samplingRate;
char chunk[5];
char fullname[255];

char *buf;

buf=Info->buffer;


if (memcmp(buf,"FORM",4))   return 1;

if ( (memcmp(buf+8,"8SVX",4)!=0) & (memcmp(buf+8,"16SV",4)!=0) )
        return 1;

if (memcmp(buf+8,"16SV",4))
                      strcpy(Info->format,"Amiga 16-bit sampled voice");
if (memcmp(buf+8,"8SVX",4))
                       strcpy(Info->format,"Amiga 8-bit sampled voice");


memcpy((char*)(&pos),buf+4,4);
Info->taille=InvLong(pos)+8;

fseek(Info->fic,Info->posfic+12,SEEK_SET);

do
{
if (fread(chunk,4,1,Info->fic)!=4) break;

if (!memcmp(chunk,"NAME",4))
    {
    fread(&pos,4,1,Info->fic);
    pos=InvLong(pos);
    if (pos<256L)
        {
        fread(fullname,(short)pos,1,Info->fic);
        ClearSpace(fullname);
        if (strlen(fullname)<40) strcpy(Info->fullname,fullname);
        }
    continue;
    }
if (!memcmp(chunk,"VHDR",4))
    {
    fseek(Info->fic,16,SEEK_CUR);
    fread(&samplingRate,2,1,Info->fic);
    samplingRate=InvWord(samplingRate);
    sprintf(Info->message[0], "  Sampling rate: %15d Hz",samplingRate);
    break;
    }

break;
} while(1);


return 0;
}

short Infolbm(RB_IDF *Info)
{
ULONG pos;
char chunk[5];
char fullname[255];
short Lp,Hp;
BYTE BP;

char *buf;

buf=Info->buffer;

if (memcmp(buf,"FORM",4))   return 1;

if ( (memcmp(buf+ 8,"ILBM",4))
   & (memcmp(buf+ 8,"HPBM",4))
   & (memcmp(buf+12,"BMHD",4)) )   return 1;

Info->taille=ReadLng(Info,4,2)+8;

fseek(Info->fic,Info->posfic+12,SEEK_SET);

do
{
if (fread(chunk,4,1,Info->fic)!=4) break;

if (!memcmp(chunk,"NAME",4))
    {
    fread(&pos,4,1,Info->fic);
    pos=InvLong(pos);
    if (pos<256L)
        {
        fread(fullname,(short)pos,1,Info->fic);
        ClearSpace(fullname);
        if (strlen(fullname)<40) strcpy(Info->fullname,fullname);
        }
    continue;
    }

if (!memcmp(chunk,"BMHD",4))
    {
    fseek(Info->fic,4,SEEK_CUR);
    fread(&Lp,2,1,Info->fic);
    Lp=InvWord(Lp);
    fread(&Hp,2,1,Info->fic);
    Hp=InvWord(Hp);
    fseek(Info->fic,4,SEEK_CUR);
    fread(&BP,1,1,Info->fic);

    if (Lp>9999) Lp=9999;
    if (Hp>9999) Hp=9999;
    if (BP>99) BP=99;

    sprintf(Info->message[0],
                         "  Picture is    %4d * %4d / %2dBps",Lp,Hp,BP);
    break;
    }

break;
} while(1);


return 0;
}



short Infoarj(RB_IDF *Info)
{
unsigned char *buf;

buf=Info->buffer;

if ( (buf[0]!=0x60) | (buf[1]!=234) | (buf[3]!=0x00) | (buf[4]!=0x1E) )
        return 1;

return 0;
}


/*
struct MMD0 {
        ULONG   id;
        ULONG   modlen;  //--- length of entire module -----------------
        struct MMD0song *song;
        ULONG   reserved0;
        struct MMD0Block **blockarr;
        ULONG   reserved1;
        struct InstrHdr **smplarr;
        ULONG   reserved2;
        struct MMD0exp *expdata;
        ULONG   reserved3;
        UWORD   pstate;  //--- some data for the player routine --------
        UWORD   pblock;
        UWORD   pline;
        UWORD   pseqnum;
        WORD    actplayline;
        UBYTE   counter;
        UBYTE   extra_songs; //--- number of songs - 1 -----------------
}; // length = 52 bytes


struct MMD0song {
        struct MMD0sample sample[63];
        UWORD   numblocks;
        UWORD   songlen;
        UBYTE   playseq[256];
        UWORD   deftempo;
        BYTE    playtransp;
        UBYTE   flags;
        UBYTE   flags2;
        UBYTE   tempo2;
        UBYTE   trkvol[16];
        UBYTE   mastervol;
        UBYTE   numsamples;
}; */ // length = 788 bytes


short Infomed(RB_IDF *Info)
{
char *buf;

buf=Info->buffer;


sprintf(Info->format,"OctaMED ver. %c.0 module",buf[3]+1);

Info->taille=ReadLng(Info,4,2);

return 0;
}

short Infovoc(RB_IDF *Info)
{
short type;
int size,pos;
BYTE Csize[40];
short mess;




fseek(Info->fic,Info->posfic+0x1A,SEEK_SET);

mess=0;

while(1)
        {
        if (fread(&type,1,1,Info->fic)!=1) break;
        if (type==0) break;

        fread(Csize,3,1,Info->fic);
        size=((int)Csize[0])+((int)Csize[1])*256+((int)Csize[2])*65536;
        pos=ftell(Info->fic);

        if ( (type==1) & (mess<9) )
                {
                fread(Csize,2,1,Info->fic);
                sprintf(Info->message[mess],
                   "  Sampling rate: %15ld Hz",1000000L/(256-Csize[0]));
                if (Csize[1]==0) sprintf(Info->message[mess+1],
                                 "  Format:            8-bits samples");
                if (Csize[1]==1) sprintf(Info->message[mess+1],
                                 "  Format:            4-bits samples");
                if (Csize[1]==2) sprintf(Info->message[mess+1],
                                 "  Format:          2.6-bits samples");
                if (Csize[1]==3) sprintf(Info->message[mess+1],
                                 "  Format:            2-bits samples");
                mess+=2;
                }

        if (type==5)
                {
                if ( (size<=30) & ((Info->fullname[0])==0) ) //msg ASCII
                        {
                        fread(Csize,(unsigned short)size,1,Info->fic);
                        Csize[(unsigned short)size]=0;
                        strcpy(Info->fullname,(char*)Csize);
                        }
                        else
                if ( (size<=34) & (mess<10) )                //msg ASCII
                        {
                        fread(Csize,(unsigned short)size,1,Info->fic);
                        Csize[(unsigned short)size]=0;
                        sprintf(Info->message[mess],"  %s",Csize);
                        mess++;
                        }

                }

        fseek(Info->fic,pos+size,SEEK_SET);

        }

Info->taille=ftell(Info->fic)-Info->posfic;

return 0;
}

short Infopsm(RB_IDF *Info)
{



Info->taille=ReadLng(Info,4,1)+12;

return 0;
}

short Infoexe1(RB_IDF *Info)
{
unsigned short modulo,file,header,overlay,pe,ip,cs;
char htc[]={0xBA,0,0,0x2E,0x89,0x16,0,0,0xB4,0x30,0xCD,0x21}; //---  C++
char hqc[]={0xB4,0x30,0xcd,0x21,0X3C,0x02,0x73,0x05,0x33,0xC0}; //--- QC
char htp[]={0x9A,0x00,0x00};   // header TP
char hai[]={0x0E,0x07,0xB9,0x14,0x00,0xBE,0x00,0x01}; //------------ AIN
char pkl[]={0xB8,0,0,0xBA,0,0,0x05,0,0,0x3B,0x06,0x02,0x00}; //-- PKLITE
char exe[]={0x8B,0xE8,0x8C,0xC0,0x05,0x10,0,0x0E,0x1F,0xA3,0x04,0};
                                                           //--- Exepack
char lze[]={0x06,0x0E,0x1F,0x8B,0x0E,0x0C,0x00,0X8B,0xF1,0x4E}; // LZEXE

char dsh[]={0x06,0xE8,0x00,0x00,0x5E}; //------------------------- LZEXE
char rdb[]="RBID";  // header Redbug File information

char buf2[33];

char *buf;

buf=Info->buffer;

Info->os=1;
sprintf(Info->Tinfo,"Operating Sys.");
sprintf(Info->info,"DOS");



memcpy(&modulo ,buf+0x02,2);
memcpy(&file   ,buf+0x04,2);
memcpy(&header ,buf+0x08,2);
memcpy(&overlay,buf+0x1A,2);
memcpy(&ip         ,buf+0x14,2);
memcpy(&cs         ,buf+0x16,2);

if ( (cs==0xFFF0) & (ip==0x100) )
        pe=(header*16);
        else
        pe=ip+(header*16)+(cs*16);

fseek(Info->fic,Info->posfic+(int)pe,SEEK_SET);
fread(buf2,32,1,Info->fic);

sprintf(Info->message[0]," Header is %18ld bytes",((int)header)*16);


if (!memcmp(buf2,hai,7))
        sprintf(Info->message[1]," Compressed with         BROKEN AIN");
if (!memcmp(buf2,exe,12))
        sprintf(Info->message[1]," Compressed with      EXEPACK V3.63");
if (!memcmp(buf2,exe+2,10))
        sprintf(Info->message[1]," Compressed with      EXEPACK V4.05");
if (!memcmp(buf2,lze,9))
        sprintf(Info->message[1]," Compressed with       BROKEN LZEXE");
if ( (!memcmp(buf2+9,pkl+9,4)) & (buf2[0]==pkl[0])
                               & (buf2[3]==pkl[3]) & (buf2[6]==pkl[6]) )
        sprintf(Info->message[1]," Compressed with      BROKEN PKLITE");

if (!memcmp(buf+0x1E,"PKLITE",6))
        sprintf(Info->message[1],
                     " Compressed with       PKLITE V1.%02d",buf[0x1C]);
if (!memcmp(buf+0x1C,"LZ91",4))
        sprintf(Info->message[1]," Compressed with              LZEXE");
if (!memcmp(buf+0x1C,"WWP ",4))
        sprintf(Info->message[1]," Compressed with             WWPACK");
if (!memcmp(buf+0x20,"RNC",3))
        sprintf(Info->message[1]," Compressed with           PRO-PACK");
if (!memcmp(buf+0x20,"AIN2",4))
        sprintf(Info->message[1]," Compressed with          AIN V2.23");




if (!memcmp(buf+0x55,"PMODE",5))
        {
        sprintf(Info->message[2]," Using   ");
        memcpy((Info->message[2])+9,buf+0x55,27);
        (Info->message[2])[37]=0;
        }
if (!memcmp(buf+0x25C,"DOS/4G",6))
        sprintf(Info->message[3]," Using                       DOS/4G");


if (!memcmp(buf+0x1C,"L.S.",4))
        sprintf(Info->message[3]," Crypt  by  LIGHT SHOW/Eclipse 1.13");
if (!memcmp(buf+0x1C,"@KLS",4))
        sprintf(Info->message[3]," Crypt. by  LIGHT SHOW/Eclipse 1.15");
if (!memcmp(buf2,dsh,5))
        sprintf(Info->message[3]," Crypted by                 DSHIELD");

if (!memcmp(buf+0x371,"WATCOM C",8))
        sprintf(Info->message[3]," Compiled with             WATCOM C");
if (!memcmp(buf+0x279,"WATCOM C",8))
        sprintf(Info->message[3]," Compiled with             WATCOM C");
if (!memcmp(buf+0x273,"WATCOM C",8))
        sprintf(Info->message[3]," Compiled with             WATCOM C");
if (!memcmp(buf2,htp,3))
        sprintf(Info->message[3]," Compiled with         TURBO PASCAL");
if ( (buf2[0]==htc[0]) & (!memcmp(buf2+3,htc+3,3))
                                           & (!memcmp(buf2+8,htc+8,4)) )
        sprintf(Info->message[3]," Compiled with          TURBO C/C++");
if (!memcmp(buf2,hqc,9))
        sprintf(Info->message[3]," Compiled with              QUICK C");


if (!memcmp(buf+(*(WORD*)(buf+0x3C)),"NE",2))
    {
    unsigned char os,lv,hv,n;
    ULONG pos;

    os=*(buf+(*(WORD*)(buf+0x3C)+0x36));
    sprintf(Info->info,"Unknown OS");
    if ((os&1)==1)
        sprintf(Info->info,"OS2");
    if ((os&2)==2)
        {
        sprintf(Info->info,"WIN ");
        lv=*(buf+(*(WORD*)(buf+0x3C)+0x3E));
        hv=*(buf+(*(WORD*)(buf+0x3C)+0x3F));
        switch(hv)
            {
            case  4:        sprintf(buf2,"95"); break;
            default:        sprintf(buf2,"%d.%02d",hv,lv); break;
            }
        strcat(Info->info,buf2);
        }
    if ((os&4)==4)
        sprintf(Info->info,"WIN386");


    pos=ReadLng(Info,*(WORD*)(buf+0x3C)+0x2C,1)+1;

    ReadStr(Info,pos,buf2,79);
    buf2[79]=0;
    strcpy(Info->fullname,buf2);
    for (n=0;n<strlen(buf2);n++)
        if (buf2[n]==':')
            {
            ReadStr(Info,pos+n+1,Info->fullname,79);
            Info->fullname[79]=0;
            break;
            }

    if ((ReadInt(Info,*(WORD*)(buf+0x3C)+0x0C,1)&0x8000)==0x8000)
        {
        strcpy(Info->ext,"DLL");
        strcpy(Info->format,"Library File");
        }
    }



if (!memcmp(buf+(*(WORD*)(buf+0x3C)),"LE",2))
    {
    unsigned char os;
    ULONG pos;

    os=*(buf+(*(WORD*)(buf+0x3C)+0x0A));
    sprintf(Info->info,"Unknow OS");
    if (os==1) sprintf(Info->info,"OS/2");
    if (os==2) sprintf(Info->info,"WINDOWS");
    if (os==3) sprintf(Info->info,"European DOS 4.0");
    if (os==4) sprintf(Info->info,"WIN386");

    pos=ReadLng(Info,*(WORD*)(buf+0x3C)+0x58,1);
    pos+=*(WORD*)(buf+0x3C);
    ReadStr(Info,pos+1,buf2,buf[pos]);
    strcpy(Info->fullname,buf2);
    }

if (!memcmp(buf+(*(WORD*)(buf+0x3C)),"PE",2))
    {
    sprintf(Info->info,"WIN32");
    }

if (!memcmp(buf+(*(WORD*)(buf+0x3C)),"LX",2))
    {
    sprintf(Info->info,"OS/2");
    }

if (!memcmp(buf+(*(WORD*)(buf+0x3C)),"W3",2))
    {
    sprintf(Info->info,"WIN386 file");
    }

if (!memcmp(buf+(*(WORD*)(buf+0x3C)),"DL",2))
    {
    sprintf(Info->info,"HP 100LX/200LX");
    }

if (!memcmp(buf+(*(WORD*)(buf+0x3C)),"MP",2))
    {
    sprintf(Info->info,"old PharLap .EXP");
    }

if (!memcmp(buf+(*(WORD*)(buf+0x3C)),"P2",2))
    {
    sprintf(Info->info,"PharLap 286 .EXP");
    }

if (!memcmp(buf+(*(WORD*)(buf+0x3C)),"P3",2))
    {
    sprintf(Info->info,"PharLap 386 .EXP");
    }

if (!memcmp(buf+0x1C,rdb,4))
    {
    short n=0;
    memcpy(Info->fullname,buf+0x20,32);
    while ( (Info->fullname[n]>=32) & (n<32) ) n++;
    Info->fullname[n]=0;
    ClearSpace(Info->fullname);
    }




Info->taille=(int)modulo+((int)file)*512-512;

return 0;
}

short Infoxi(RB_IDF *Info)
{
char bname[21];

char *buf;

buf=Info->buffer;



memcpy(Info->fullname,buf+21,22);
Info->fullname[22]=0;

memcpy(bname,buf+44,20);
bname[20]=0;
ClearSpace(bname);
sprintf(Info->format,"Instr. %s",bname);

return 0;
}


short Infotpu(RB_IDF *Info)
{
char *buf;

buf=Info->buffer;


if (buf[3]=='6') strcat(Info->format," Ver 5.5");
if (buf[3]=='9') strcat(Info->format," Ver 6.0");
return 0;
}

struct Scmf {
        char Mkey[4];
        char Hbver,Lbver;
        short offins;
        short offmus;
        short tick;
        short ticks;
        short offtit;
        short offcom;
        short offrem;
        char channel[16];
        short nbrinst;
        short bastempo;
        };
short Infocmf(RB_IDF *Info)
{
struct Scmf *Bcmf;
short l,n;

char *buf;

buf=Info->buffer;

Bcmf=(struct Scmf*)buf;

sprintf(Info->format,
                  "Creative Music File v%d.%d",Bcmf->Hbver,Bcmf->Lbver);

l=Bcmf->offins;

if (Bcmf->offrem!=0) l=Bcmf->offrem;
if (Bcmf->offcom!=0)
    {
    if ( (l-Bcmf->offcom)<=23)
        memcpy(Info->composer,buf+Bcmf->offcom,l-Bcmf->offcom);
    l=Bcmf->offcom;
    }

if (Bcmf->offtit!=0)
    {
    if ( (l-Bcmf->offtit)<=30)
        memcpy(Info->fullname,buf+Bcmf->offtit,l-Bcmf->offtit);
    }

l=0;
for (n=0;n<16;n++)
        if ((Bcmf->channel[n])!=0) l++;

strcpy(Info->Tinfo,"Inst / Channel");
sprintf(Info->info,"%3d /%3d",Bcmf->nbrinst,l);

return 0;
}

short Infohlp(RB_IDF *Info)
{
char keysize[]={0x00,0x00,0x01,0x00};

char *buf;

buf=Info->buffer;

memcpy(&(Info->taille),buf+12,4);  // Ordre des bytes pour le int: LO-HI

if (!memcmp(buf+0x23,keysize,4))
        if (ReadInt(Info,0x27,1)<30)
                ReadStr(Info,0x29,Info->fullname,ReadInt(Info,0x27,1));

return 0;
}

short Infostm(RB_IDF *Info)    // 8 channel
{
char *buf;

buf=Info->buffer;
memcpy(Info->fullname,buf,20);
Info->fullname[20]=0;

if (buf[29]==2)
   sprintf(Info->format,"Mod. Scream Tracker V%d.%02d",buf[30],buf[31]);
if (buf[29]==1)
   sprintf(Info->format,"Song Scream Tracker V%d.%02d",buf[30],buf[31]);

strcpy(Info->Tinfo,"Patt/Chnl");
sprintf(Info->info,"%3d / 8",buf[33]);
return 0;
}

short Infofar(RB_IDF *Info)    // 16 channel
{
char *buf;

buf=Info->buffer;
memcpy(Info->fullname,buf+4,40);
Info->fullname[40]=0;

sprintf(Info->format,"Farandole Module V%d.%d",buf[49]/16,buf[49]&15);

return 0;
}

short Infofsm(RB_IDF *Info)
{
ReadStr(Info,0x04,Info->fullname,32);
return 0;
}

short Infousm(RB_IDF *Info)
{
ReadStr(Info,0x04,Info->fullname,32);
return 0;
}


short Infofpt(RB_IDF *Info)
{
ReadStr(Info,0x04,Info->fullname,32);
return 0;
}

short Infof2r(RB_IDF *Info)
{
ReadStr(Info,0x06,Info->fullname,40);
return 0;
}

short Infoits(RB_IDF *Info)
{
ReadStr(Info,0x14,Info->fullname,22);
return 0;
}

short Infodp3(RB_IDF *Info)
{
ReadStr(Info,0x30,Info->fullname,22);
return 0;
}

short Infopat(RB_IDF *Info)
{
ReadStr(Info,0x83,Info->fullname,16);
return 0;
}

short Infodsf(RB_IDF *Info)
{
ReadStr(Info,4,Info->fullname,22);
return 0;
}

short Infouwf(RB_IDF *Info)
{
ReadStr(Info,0,Info->fullname,22);
return 0;
}

short Infoams2(RB_IDF *Info)
{
ReadStr(Info,37,Info->fullname,22);
return 0;
}

short Infoast(RB_IDF *Info)
{
WORD lng;

lng=ReadInt(Info,10,1);
ReadStr(Info,12,tampon,lng);
tampon[lng]=0;

ClearSpace(tampon);

tampon[79]=0;
strcpy(Info->fullname,tampon);

return 0;
}

short Infodsm(RB_IDF *Info)
{
ReadStr(Info,4,tampon,79);
tampon[79]=0;

ClearSpace(tampon);

tampon[79]=0;
strcpy(Info->fullname,tampon);

return 0;
}

short Infodat(RB_IDF *Info)
{
ReadStr(Info,0x16,Info->fullname,32);
ReadStr(Info,0x36,Info->composer,32);
// ReadStr(Info,0x56,Info->fullname,32);    // C'est quoi ici ?

return 0;
}



short Infomdl(RB_IDF *Info)
{
char chunk[2];
ULONG taille;

taille=Info->posfic+5;

while(1)
    {
    fseek(Info->fic,taille,SEEK_SET);

    if (fread(chunk,2,1,Info->fic)!=2) break;
    if (fread(&taille,4,1,Info->fic)!=4) break;
    taille+=ftell(Info->fic);

    if (!memcmp(chunk,"IN",2))
        {
        char nom[33];

        fread(nom,32,1,Info->fic);
        nom[32]=0;
        ClearSpace(nom);
        nom[30]=0;
        memcpy(Info->fullname,nom,31);

        fread(Info->composer,20,1,Info->fic);
        ClearSpace(Info->composer);
        continue;
        }
    if (!memcmp(chunk,"PN",2)) continue;
    if (!memcmp(chunk,"ME",2)) continue;
    if (!memcmp(chunk,"PA",2)) continue;
    if (!memcmp(chunk,"TR",2)) continue;
    if (!memcmp(chunk,"IS",2)) continue;
    if (!memcmp(chunk,"SA",2)) continue;

    fseek(Info->fic,-6,SEEK_CUR);
    break;
    }

Info->taille=ftell(Info->fic)-Info->posfic;

return 0;
}

short Infospl(RB_IDF *Info)
{
char *buf;

ReadStr(Info,0x05,Info->fullname,22);

buf=Info->buffer;
sprintf(Info->message[0], "  Sampling rate: %15d Hz",*(WORD*)(buf+45));
return 0;
}


short Infogrp(RB_IDF *Info)
{
char name[10];
char key2[]={0x00,0x80,0xFF,0xFF,0x0A,0x00};
char key[]="PMCC";

char *buf;

buf=Info->buffer;


Info->taille=(int)(*(WORD*)(buf+6));

fseek(Info->fic,Info->taille+Info->posfic,SEEK_SET);
if (fread(name,6,1,Info->fic)==6)
  if (!memcmp(name,key2,6))
     if (fread(name,10,1,Info->fic)==10)
        if (!memcmp(name,key,4))
           Info->taille+=(16+((*(WORD*)(name+6))+1)*(*(WORD*)(name+8)));

return 0;
}


short Infotga(RB_IDF *Info)
{
char key1[]={0,1,1,0};
char key2[]={0,0,2,0};
char key[]={0,0,0,0};

char *buf;

buf=Info->buffer;

if (memcmp(buf+8,key,4)) return 1;

if (!memcmp(buf,key1,4))
    sprintf(Info->format,"Targa Picture 256 Col.");
    else
    if (!memcmp(buf,key2,4))
        sprintf(Info->format,"Targa Picture True Col.");
        else
        return 1;
return 0;
}

short Infompg(RB_IDF *Info)
{
char key[]={0x00,0x00,0x01,0xBA,0x21,0x00};
char key2[]={0x00,0x00,0x01,0xB3,0x0A,0x00,0x78};

char *buf;

buf=Info->buffer;

if ( (memcmp(buf,key,6)) &
     (memcmp(buf,key2,7)) )  return 1;

return 0;
}


short Infodlz(RB_IDF *Info)
{
char *buf;

buf=Info->buffer;
Info->taille=(ULONG)(*(WORD*)(buf+0xA))+17+(ULONG)(buf[0x9])*65536L;
return 0;
}

short Infolza(RB_IDF *Info)
{
short Lp,Hp,BP,frame;

Lp=ReadInt(Info,9,1);
Hp=ReadInt(Info,11,1);
BP=8;
frame=ReadInt(Info,7,1);

if (Lp>9999) Lp=9999;
if (Hp>9999) Hp=9999;
if (BP>99) BP=99;
if (BP==0) BP=8;

sprintf(Info->message[0]," Picture is     %4d * %4d / %2dBps",Lp,Hp,BP);
sprintf(Info->message[1], "                     %7d frames",frame);

// Info->taille=ReadLng(Info,0,1);
return 0;
}

short Infofli(RB_IDF *Info)
{
short Lp,Hp,BP,frame;

Lp=ReadInt(Info,8,1);
Hp=ReadInt(Info,10,1);
BP=ReadInt(Info,12,1);
frame=ReadInt(Info,6,1);

if (Lp>9999) Lp=9999;
if (Hp>9999) Hp=9999;
if (BP>99) BP=99;
if (BP==0) BP=8;


sprintf(Info->message[0]," Picture is     %4d * %4d / %2dBps",Lp,Hp,BP);
sprintf(Info->message[1], "                      %7d frame",frame);

Info->taille=ReadLng(Info,0,1);
return 0;
}

short Infoflc(RB_IDF *Info)
{
short Lp,Hp,BP,frame;

Lp=ReadInt(Info,8,1);
Hp=ReadInt(Info,10,1);
BP=ReadInt(Info,12,1);
frame=ReadInt(Info,6,1);

if (BP==0) BP=8;
if (Lp>9999) Lp=9999;
if (Hp>9999) Hp=9999;
if (BP>99) BP=99;

sprintf(Info->message[0],"  Picture is    %4d * %4d / %2dBps",Lp,Hp,BP);
sprintf(Info->message[1], "                      %7d frame",frame);

Info->taille=ReadLng(Info,0,1);
return 0;
}

short Infoswg(RB_IDF *Info)
{
ReadStr(Info,57,Info->fullname,48);
return 0;
}

short Infoams(RB_IDF *Info)
{
ReadStr(Info,8,Info->fullname,Info->buffer[7]);
return 0;
}


short Infot64(RB_IDF *Info)
{
ReadStr(Info,0x28,Info->fullname,24);
return 0;
}

short Infop00(RB_IDF *Info)
{
ReadStr(Info,0x08,Info->fullname,18);
return 0;
}

short Infoamf(RB_IDF *Info)
{
ReadStr(Info,0x04,Info->fullname,32);
return 0;
}

short Infogb(RB_IDF *Info)
{
ReadStr(Info,0x134,Info->fullname,16);
return 0;
}

short Infoptm(RB_IDF *Info)
{
ReadStr(Info,0,Info->fullname,28);
return 0;
}

short Infotxt(RB_IDF *Info)
{
unsigned short pos,i;
unsigned char a;

/*--------------------------------------------------------------------*\
|-  Recherche de la valeur texte par statistique                      -|
\*--------------------------------------------------------------------*/
/*
unsigned short tab[256];
short ord[256];
short i,j,n;
char col[]=" etanoris-hdlc";
short val=0;

for (n=0;n<256;n++) {
        tab[n]=0;
        ord[n]=n;
        }

for (pos=0;pos<Info->sizebuf;pos++)
                tab[Info->buffer[pos]]++;

for (i=0;i<256;i++)
        for(j=i;j<256;j++)
                if (tab[ord[i]]<tab[ord[j]]) {
                        n=ord[i];
                        ord[i]=ord[j];
                        ord[j]=n;
                        }

for (i=0;i<15;i++)      {
        for (n=0;n<15;n++)
                if (ord[i]==col[n])             {
                        val+=100+((50*abs(i-n))/15);
                        break;
                        }
        }

val=val/15;
if (val<50) return 1; */


for (pos=0;pos<Info->sizebuf;pos++) {
                a=(unsigned char)(Info->buffer[pos]);
                if ((a<32) &
                        (a!=10) & (a!=13) &          // passage de ligne
                        (a!=12) &                     // passage de page
                        (a!=17) &
                        (a!=16) &
                        (a!=9) &                           // tabulation
                        (a!=0) &                        // erreur buffer
                        (a!=2) &                       // la petite tˆte
                        (a!=3) &                       // le petit coeur
                        (a!=26)) {                     // fin de fichier
                        // cprintf("%d",Info->buffer[pos]);
                        return 1;
                        }
                }


SplitName(Info->filename,NULL,Info->ext);


pos=0;

do
{
a=0;

while ( ((Info->buffer[pos]>123)| (Info->buffer[pos]<65)) & (pos<1000) )
        pos++;

ReadStr(Info,pos,Info->fullname,60);


for (i=0;i<60;i++)
        if (Info->fullname[i]<32) Info->fullname[i]=0;

if (!strnicmp(Info->fullname,"REM",3)) a=3;
if (!strnicmp(Info->fullname,"//",2)) a=2;

if (!strnicmp(Info->fullname,"include",7)) {
        a=1;
        while ( (Info->buffer[pos]!=13) & (pos<1000) ) pos++;
        }

if (strlen(Info->fullname)<3) a=1;


if (a==0)
        pos+=strlen(Info->fullname);
        else
        pos+=a;

}
while ( ( (!strnicmp(Info->fullname,"@ECHO",5)) |
                  (!strnicmp(Info->fullname,"ECHO",4)) |
                  (!strnicmp(Info->fullname,"#INCLUDE",8)) |
                  (a!=0)
                ) & (pos<1000)
          );

Info->fullname[79]=0;

if (pos>999) Info->fullname[0]=0;

return 0;
}

short Infohtm(RB_IDF *Info)
{
unsigned short n,d;
char *buf;
char titre[64];

buf=Info->buffer;

d=0;
for(n=0;n<32768;n++)
    {
    switch(buf[n])  {
        case 10:
        case 13:
            break;
        case '<':
            d=n+1;
            break;
        case '>':
            if (d==0) return 1;
            memcpy(titre,buf+d,32);
            if (n-d<64)
                {
                titre[n-d]=0;
                if (!stricmp(titre,"HTML")) return 0;
                if (!stricmp(titre,"TITLE")) return 0;
                if (!stricmp(titre,"PRE")) return 0;
                d=0;
                }
            break;
        default:
            if (d==0) return 1;
            break;
        }
    }

return 1;
}

char *GetFile(char *p,char *Ficname)
{
int n;
char *s;

if (*p==0) return Ficname;

for (n=0;n<strlen(p);n++)
    if ( (p[n]=='\\') | (p[n]=='/') )
        s=p+n+1;

strcpy(Ficname,s);

return Ficname;
}


