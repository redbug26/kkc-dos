/*--------------------------------------------------------------------*\
|-                      Identification of file                        -|
\*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#include "idf.h"

static char *FORMPIC=" Picture is    %5d * %4d / %2dBps";
static char *FORMSMP=" Sampling rate: %16d Hz";
static char *FORMBIT=" Smp.Rate: %5dHz Bitrate: %3dkbps";

struct key K[nbrkey]=   {
{  NULL,
        0,
        0,
        "C64S tape file",
        "T64",
        "",
        76,1*2+1,6},
{  "UC2",
        3,
        0,
        "UC2 Archive",
        "UC2",
        "Ad Infinitum Programs",
        75,0*2+0,3},
{  "\xD7\xCD\xC6\x9A",
        4,
        0,
        "Windows Vectorial Image",
        "WMF",
        "Microsoft Corp.",
        74,0*2+0,4},
{  "mhwanh",
        6,
        0,
        "HSI raw bitmap",
        "RAW",
        "Handmade Software, Inc",   //--- HSI (Creator of alchemy) -----
        73,0*2+1,4},
{  NULL,
        0,
        0,
        "Module File", //--- 31 instruments: NoiseTracker --------------
        "MOD",
        "",
        1,1*2+1,1},
{  "DDMF",
        4,
        0,
        "Digital Music Module",
        "DMF",
        "Delusion",
        6,0*2+1,1},
{  "IMPM",
        4,
        0,
        "Impulse Tracker Module",
        "IT",
        "Impulse",
        10,0*2+1,1},
{  "MThd",
        4,
        0,
        "Midi file",
        "MID",
        "I.M.A.",       //--- International MIDI Association -----------
        14,0*2+1,1},
{  "MTM",
        3,
        0,
        "Multitracker Module",
        "MTM",
        "Renaissance",
        15,0*2+1,1},
{  NULL,
        0,
        0,
        "669 Module",
        "669",
        "Renaissance",
        4,1*2+1,1},
{  "Extended Module: ",
        17,
        0,
        "Fast Tracker Module",
        "XM",
        "Triton",
        2,0*2+1,1},
{  "SCRM",
        4,
        44,
        "Scream Tracker 3 Module",
        "S3M",
        "Future Crew",
        3,0*2+1,1},
{  "MAS_UTrack",
        10,
        0,
        "Ultratracker module",
        "UT",
        "Mysterious MAS",
        19,0*2+1,1},
{  "\x52\x61\x72\x21\x1A\x07\x00",
        7,
        0,
        "RAR Archive",
        "RAR",
        "Eugene Roshal",
        34,0*2+1,3},
{  "GIF8",
        4,
        0,
        "Compuserve GIF",
        "GIF",
        "Compuserve",
        37,0*2+1,4},

{  "OKTASONG",
        8,
        0,
        "Oktalyser module",
        "OKT",
        "Armin Sander",
        16,0*2+1,1},
{  ".snd",
        4,
        0,
        "Sun/NeXT audio file",
        "AU",
        "Sun/NeXT",
        21,0*2+1,2},
{  NULL,
        0,
        0,
        "Amiga sampled voice",
        "IFF",
        "Electronic Arts",
        23,1*2+1,2},
{  NULL,
        0,
        0,
        "Interleaved Bitmap",
        "LBM",
        "Electronic Arts",
        39,1*2+1,4},
{  NULL,
        0,
        0,
        "ARJ Archive",
        "ARJ",
        "Robert K Jung",
        30,1*2+0,3},
{  "-lh",
        3,
        2,
        "LHA Archive",
        "LHA",
        "H. Yoshizaki",
        32,0*2+0,3},
{  "MMD",
        3,
        0,
        "OctaMED module",
        "MED",
        "Teijo Kinnunen",
        13,0*2+1,1},
{  "Creative Voice File",
        19,
        0,
        "Creative Voice File",
        "VOC",
        "Creative Labs",
        27,0*2+1,2},
{  "PSM ",
        4,
        0,
        "Epic PSM/MOD",
        "PSM",
        "Epic Megagames",
        17,0*2+1,1},
{  "MZ",
        2,
        0,
        "Executable file",
        "EXE",
        "M.Z. ;)",
        57,0*2+1,6},
{  "Extended Instrument: ",
        21,
        0,
        "Instrument Fast Tracker",
        "XI",
        "Triton",
        29,0*2+1,2},
{  "GF1PATCH",
        8,
        0,
        "Patch Gravis Ultrasound",
        "PAT",
        "Gravis Ultrasound",
        24,0*2+1,2},
{  "Turbo C Project File",
        20,
        0,
        "Turbo C Project File",
        "PRJ",
        "Borland",
        62,0*2+0,6},
{  "Turbo C Context File",
        20,
        0,
        "Turbo C Context File",
        "DSK",
        "Borland",
        55,0*2+0,6},
{  "TPU",
        3,
        0,
        "Turbo Pascal Unit",
        "TPU",
        "Borland",
        64,0*2+1,6},
{  "CTMF",
        4,
        0,
        "Creative Music File",
        "CMF",
        "Creative Labs",
        5,0*2+1,1},
{  "\x3F\x5F\x03\x00",
        4,
        0,
        "Windows Help File",
        "HLP",
        "Microsoft Corp.",
        59,0*2+1,6},
{  "!Scream!",
        8,
        0x14,
        "Scream Tracker Module",
        "STM",
        "Future Crew",
        18,0*2+1,1},
{  "FARþ",
        4,
        0,
        "Farandole Module",
        "FAR",
        "Digital Infinity",     //--- Daniel Potter / DI ---------------
        8,0*2+1,1},
{  "FSMþ",
        4,
        0,
        "Far. Sample Data Signed",
        "FSM",
        "Digital Infinity",     //--- Daniel Potter / DI ---------------
        22,0*2+1,2},
{  "USMþ",
        4,
        0,
        "Far. Sample Data Unsigned",
        "USM",
        "Digital Infinity",     //--- Daniel Potter / DI ---------------
        26,0*2+1,2},
{  "FPTþ",
        4,
        0,
        "Farandole Pattern",
        "FPT",
        "Digital Infinity",     //--- Daniel Potter / DI ---------------
        9,0*2+1,1},
{  "F2RFAR",
        6,
        0,
        "Far. Linear Module V2.0",
        "F2R",
        "Digital Infinity",     //--- Daniel Potter / DI ---------------
        7,0*2+1,1},
{  "DMDL",
        4,
        0,
        "Digitrakker Module",
        "MDL",
        "n-Factor",
        12,0*2+1,1},
{  "DSPL",
        4,
        0,
        "Digitrakker Sample",
        "SPL",
        "n-Factor",
        25,0*2+1,2},

{  "PMCC",
        4,
        0,
        "Windows Group File",
        "GRP",
        "Microsoft Corp.",
        58,0,6},
{  "\xFF\x57\x50\x43",
        4,
        0,
        "WordPerfect Graphic",
        "WPG",
        "WordPerfect",
        46,0*2+0,4},
{  "\x49\x49\x2A\x00",
        4,
        0,
        "Intel TIFF Picture",
        "TIF",
        "Intel",
        45,0*2+0,4},
{  "\xFF\xD8\xFF\xE0\x00\x10\x4A\x46\x49\x46\x00",
        11,
        0,
        "JPEG Picture",
        "JPG",
        "Joint Photo.Exp.Group", //--- Joint Photographic Experts Group-
        38,0*2+0,4},
{  "\x89\x50\x4E\x47",
        4,
        0,
        "PNG Network Graphics",
        "PNG",
        "Thomas Boutell",
        41,0*2+0,4},
{  "JMUSIC",
        6,
        0,
        "Jmplayer module",
        "JMS",
        "Ultra Force",
        11,0*2+0,1},
{   NULL,
        0,
        0,
        "MPEG Movie",
        "MPG",
        "ISO",
        52,1*2+0,5},
{  "mdat",
        4,
        4,
        "QuickTime Movie",
        "MOV",
        "Apple",
        51,0*2+0,5},
{  "THNL",
        4,
        0,
        "GWS Quick View",
        "THN",
        "Alchemy MindWorks",
        44,0*2+0,4},
{  "MV - CPCEMU",
        11,
        0,
        "Cpcemu Disk File",
        "DSK",
        "Marco Vieth",
        56,0*2+0,6},
{  "ACONLIST",
        8,
        8,
        "Windows Animated Cursor",
        "ANI",
        "Microsoft Corp.",
        47,0*2+0,5},
{  "WAVE",
        4,
        8,
        "Windows Wave",
        "WAV",
        "Microsoft Corp.",
        28,0*2+0,2},
{  "AVI LIST",
        8,
        8,
        "AVI Animation",
        "AVI",
        "Microsoft Corp.",
        48,0*2+0,5},
{  "\x0A\x05\x01",
        3,
        0,
        "ZSoft PCX 3.0",
        "PCX",
        "ZSoft",
        40,0*2+1,4},
{  "RIX3",
        4,
        0,
        "ColoRix Picture",
        "SC?",
        "",
        42,0*2+0,4},
{  "AIFF",
        4,
        8,
        "Apple Audio File",
        "AIF",
        "Electronic Arts",
        20,0*2+0,2},
{  "\xFF\xFF\x53\x45\x4D",
        5,
        0,
        "QEDIT Macro",
        "MAC",
        "SemWare Corp.",
        60,0*2+0,6},
{  "/IMPHOBIA Ressource Fil",
        23,
        0,
        "IMPHOBIA Ressource File",
        "DAT",
        "Darkness",
        54,0*2+0,6},
{  "/IMPHOBIA Config/",
        17,
        0,
        "IMPHOBIA Config File",
        "CFG",
        "Darkness",
        53,0*2+0,6},
{  "\xB4\x4C\xCD\x21\x9D\x89\x64\x6C\x7A",
        9,
        0,
        "DIET Packed Data File",
        "DLZ",
        "",
        31,0*2+1,3},
{  "\x11\xAF",
        2,
        4,
        "FLI Animation",
        "FLI",
        "Autodesk",
        49,0*2+1,5},
{  "\x12\xAF",
        2,
        4,
        "FLC Animation",
        "FLC",
        "Autodesk",
        50,0*2+1,5},
{  "ziRCONia",
        8,
        0,
        "Music Module Compressor",
        "MMC",
        "Zirconia",
        33,0*2+0,3},
{  "SWAGOLX.EXE",
        11,
        0,
        "SWAG Source Code",
        "SWG",
        "",
        63,0*2+1,6},
{  "\0\0\3\xF3\0\0\0\0",
        8,
        0,
        "Executable file Amiga",
        "EXE",
        "Commodore",
        66,0*2+0,6},

{  "\xE3\x10\0\x01\0\0\0\0",
        8,
        0,
        "Information File Amiga",
        "INF",
        "",
        67,0*2+0,6},
{  "AMShdr",
        6,
        0,
        "Advanced module system",
        "AMS",
        "Extreme",  //--- Ou alors ( Velvet Development) Velvet Studio -
        68,0*2+1,1},
{  "AIL3DIG",
        7,
        0,
        "AIL 3.0 Driver",
        "DIG",
        "",
        69,0*2+0,6},

{  "AIL3MDI",
        7,
        0,
        "AIL 3.0 Midi Driver",
        "MDI",
        "",
        142,0*2+0,6},      //000000D6h   14       13
{  "DMS!",
        4,
        0,
        "DISK-Masher Archive",
        "DMS",
        "SDS Software",
        70,0*2+0,3},
{  "SZDD",
        4,
        0,
        "EXPAND compressed file",
        "___",
        "Microsoft Corp.",
        71,0*2+0,3},
{  "\x1F\x8B\x08\x08",
        4,
        0,
        "GZIP compressed file",
        "GZ",
        "GNU",
        72,0*2+0,3},
{  "MICROSOFT PIFEX",
   15,
   0x171,
   "Pif File",
   "PIF",
   "Microsoft Corp.",
   77,0*2+0,6},
{  "\xBE\0\0\0\xAB\0\0\0",
   8,
   1,
   "WRI Text",
   "WRI",
   "Microsoft Corp.",
   78,0*2+0,6},
{  "\x4C\0\0\0\x1\x14\x2\0",
   8,
   0,
   "Link Windows 95",
   "LNK",
   "Microsoft Corp.",
   79,0*2+0,6},
{  "\x00\x00\x01\x00\x01",
        6,
        0,
        "Windows Icon",
        "ICO","Microsoft Corp.",80,0*2+1,4},
{  "\x00\x00\x02\x00\x00\x00\x08\x00",
        8,
        1044,
        "Mac Executor Volume",
        "HFV",
        "ARDI",
        81,0*2+0,6},
{  "\x00\x6A\x00\x00\x00\x0C\x00\x00"
   "\x00\x20\x00\x40\x00\x01\x00\x01"
   "\x00\x00\x00\x00\xFF\xFF\xFF\x43",
        24,
        9,
        "OS/2 Icon",
        "ICO",
        "IBM Corp.",82,0*2+0,4},
{  "C64File",
        7,
        0,
        "C64 file",
        "P00",
        "Wolfgang Lorenz",83,0*2+1,6},

{  "\x4D\x56\x20\x2D\x20\x53\x4E\x41",
        8,
        0,
        "Snapshot CPCEMU",
        "SNA",
        "Marco Vieth",84,0*2+0,6},
{  "\x01\x00\x06\x00\x0B\x00\x01\x00"
   "\x01\x00\x06\x00\x10\x00\x00\x00",
        16,
        7211,
        "Snapshot Z80",
        "SNA",
        "",85,0*2+0,6},
{  "\x1B\x5B",
        2,
        0,
        "Ansi File",
        "ANS",
        "",86,8+0*2,6},
{  "%PDF",
        4,
        0,
        "Acrobat Text",
        "PDF",
        "Adobe",87,0*2+0,6},
{  "\x50\x57\x41\x44",
        4,
        0,
        "WAD file",
        "WAD",
        "ID Software",88,0*2+0,6},
{  "\x00\x00\x00\x00\x13\x00\x81\x5A"
   "\x00\x00\x81\x5A\x00\x00\xFF\x7F"
   "\x00\x00\x7F\xA5\x00\x00\x81\x5A",
        24,
        138,
        "FS4 Mode",
        "MOD",
        "Microsoft Corp.",89,0*2+0,6},
{  "\x4A\x20\x20\x20\x20\x20\x20\x20\x00\x00\x00\xFF\x00\x00\x00\xFF",
    16,
    0x22c8,
    "Megadrive ROM",
    "SMD",
    "Sega",145,0*2+1,6},

{  "\x48\x50\x48\x50\x34\x38\x2D",
        0x7,
        0,
        "Binary File HP48",
        "HP",
        "Hewlett Packard",92,0*2+0,6},

{  "MXM\0",  // MXM
        4,
        0,
        "MXM Module",
        "MXM",
        "Cubic Team",94,0*2+0,1},             //--- Pascal/Cubic Team ----
{  "\x00\x00\x00\x00\x00\x00\xE0\x01"
   "\x00\x00\x80\x02\x00\x00\xE0\xC5", // úú€úúúúú
        16,
        0x85,
        "Replay of Need for Speed",
        "RPL",
        "E. Arts",95,0*2+0,6},
{  "\x50\x4B\x08\x08\x42\x47\x49\x20", // PKúúBGI
        0x8,
        0,
        "CHR Font",
        "CHR",
        "Borland",96,0*2+0,6},
{  "SCDH" ,
        4,
        0x8,
        "Simcity 2000 Save",
        "SC2",
        "Maxis",97,0*2+0,6},
{  "\x70\x6B\x08\x08\x42\x47\x49\x20", // PKúúBGI
        0x8,
        0,
        "BGI Driver",
        "BGI",
        "Borland",98,0*2+0,6},
{  "\x25\x25\x48\x50\x3A\x20\x54\x28"
   "\x33\x29\x41\x28\x44\x29\x46\x28"
   "\x2E\x29\x3B",
        0x13,
        0,
        "ASCII File HP48",
        "HP",
        "Hewlett Packard",99,0*2+0,6},
{  "\x50\x53\x31\x36\xFE",
        5,
        0,
        "PS16 Module",                //--- Protracker Studio 16 -------
        "P16",
        "Renaissance",100,0*2+0,1},   //--- Joshua C. Jensen -------------
                                    //--- aka CyberStrike/Renaissance --

{  "KKRB",
        4,
        0,
        "IAR Description",
        "KKR",
        "RedBug", 101, 0*2+0,  6},
{  "KKD",
        3,
        0,
        "Description of disk",
        "KKR",
        "RedBug", 102, 0*2+0, 6},
{  "SMK",
        3,
        0,
        "Smacker Graphics File",
        "SMK",
        "Jeff Roberts", 103, 0*2+0, 5}, //--- RAD Software -------------
{  "\x50\x61\x63\x6B\x65\x64\x20\x46\x69\x6C\x65\x20",
    12,
    0,
    "NetWare Packed File",
    "___",
    "NetWare",105,0*2+0,3},
{  "\x41\x4D\x46",
    3,
    0,
    "DSMi Module",
    "AMF",
    "Otto Chrons",106,0*2+1,1},   //--- Otto Chrons  (Virtual Visions ?) -
{  "\xCE\xED\x66\x66"
   "\xCC\x0D\x00\x0B\x03\x73\x00\x83"
   "\x00\x0C\x00\x0D\x00\x08\x11\x1F",
    20,
    0x104,
    "GameBoy Cartridge",
    "GB",
    "Nintendo",107,0*2+1,6},
{  "\x00\x00\x00\x00\x00\x50\x54\x4D"    // úúúúúPTM
   "\x46\x00", // Fú
    10,
    0x27,
    "PTM Module",
    "PTM",
    "Nostalgia",108,0*2+1,1},
{  "SAdT",
    4,
    0,
    "Surprise Adlib Tracker Module",
    "SAT",
    "Surprise!",109,0*2+0,1},
{  "PKM",
    3,
    0,
    "PKM Bitmap",
    "PKM",
    "Sunset Design",110,0*2+1,4},
{  "IMPS",
    4,
    0,
    "Impulse Tracker Sample",
    "ITS",
    "Impulse",111,0*2+1,2},
{  "\x53\x43\x52\x53\x80", // SCRS€
    5,
    0x4c,
    "Scream Tracker Sample",
    "DP3",
    "Future Crew",112,0*2+1,2},
{  "DDSF",
    4,
    0,
    "X-Tracker Sample",
    "DSF",
    "?",113,0*2+1,2},
{  "MUWFD",
    5,
    0x22,
    "Ultratracker Sample",
    "UWF",
    "Mysteriois MAS",114,0*2+1,2},
{  "AMS",
    3,
    0,
    "Audio Manager Sample",
    "AMS",
    "?",115,0*2+1,2},
{  "FC14",
    4,
    0,
    "Futur Composer Music",
    "FC4",
    "?",116,0*2+0,1},
{  "PACG",
    4,
    0,
    "SBStudio Module",
    "PAC",
    "Henning Hellstrom",117,0*2+0,1},   // hellstr”m
{  "ustar",
    5,
    0x101,
    "TAR Archive",
    "TAR",
    "?",118,0*2+0,3},
{  "JCH",
    3,
    0,
    "EdLib compressed module",   //--- Edlib Compresse -----------------
    "D00",
    "Jens Christian Huus",119,0*2+0,1},  // Jean Christian Huus / Vibrants
{  "\x00\x06\xFE\xFD", // úúúú
    4,
    0x0,
    "EdLib module",   //--- Edlib Non Compresse ------------------------
    "EDL",
    "Jens Christian Huus",132,0*2+0,1},  // Jean Christian Huus / Vibrants
{  "AST 000",
    7,
    1,
    "All Sound Tracker Module",
    "AST",
    "Cagliostro",120,0*2+1,1},   //--- Patrice Bouchand a.k.a Cagliostro -
{  "DSM\x10",
    4,
    0,
    "DigiSound Module",
    "DSM",          //--- Used in a music disk from Necros (in Ace) ----
    "Pelusa",121,0*2+1,1},        // Carlos Hasan aka Pelusa/Psychik Monks
                                 //--- (chasan@dcc.uchile.cl) ----------
{  "\x50\x53\x49\x44\x00",
    5,
    0,
    "PlaySid Module",
    "DAT",
    "?",122,0*2+1,1},
{  "\x00\xE8\x02\x00\x00\x16\x00\x00"
   "\x00\x28\x00\x00\x00\x20\x00\x00",
    16,
    0xd,
    "Win95 Cursor",
    "CUR",
    "Microsoft Corp.",123,0*2+0,4},
{  "\xFF\xFF\x00\x01\x64\x00\x00\x00"
   "\x03\x00", // úú
    10,
    0x0,
    "Alpha Microsystems BMP", // No information(conversion from Alchemy)
    "BMP",
    "Alpha Microsystems",124,0*2+0,4},
{  "\x00\x11\x02\xFF\x0C\x00\xFF\xFE"
   "\x00\x00\x00\x48\x00\x00\x00\x48"
   "\x00\x00\x00\x00\x00\x00\x01",
    23,
    0x20a,
    "Macintosh PICT",
    "PCT",
    "?",125,0*2+0,4},
{  "LZANIM",
    6,
    0,
    "LZA animation",
    "LZA",
    "goto64",126,0*2+1,5},        // Brian Strack Jensen aka goto64/Purple
{  "RAD by REALiTY!!",
    16,
    0,
    "Reality ADlib Tracker Module",
    "RAD",
    "Reality",130,0*2+0,1},      //--- Shayde/Reality (?) ----------------
{  "\xFC\x00\x01\x00\x0C\x00\x81\x01"
   "\x82\x01\x06\x00\x01\x02\x03\x04"
   "\x05\x08\x10",
    19,
    0x0,
    "QuickBasic Listing",
    "BAS",
    "Microsoft Corp.",131,0*2+0,6},
{  "DISPTNL",
    7,
    0,
    "Thumbnail Picture",
    "TNL",
    "Jih-Shin Ho",133,0*2+1,4},
{  "hsi",
     3,
     0,
     "HSI Jpeg bitmap",
     "HSI",
     "Handmade Software, Inc",   //--- HSI (Creator of alchemy) --------
     134,0*2+1,4},

{  "\x63\x6F\x64\x65\x00\x01\x00",
    7,
    0x4e,
    "Executable file Us Pilot",
    "PRC",
    "US Robotics",137,0*2+1,6},
{  "RTMM",
    4,
    0,
    "RTM Module",
    "RTM",
    "Arnaud Hasenfratz",138,0*2+0,1},       // arnaud.hasenfratz@utbm.fr
{  "DFPv",
    4,
    0,
    "DFP Archive",
    "DFP",
    "Pascal Bestebroer",
    139,0*2+0,3},
{  "RBKK_VENUS",
    10,
    0,
    "ScreenSaver",
    "RBI",
    "Ketchup Multimedia",143,0*2+0,6},
{  "\xCC\xDD\x59\x13\x61",
    5,
    0,
    "Watcom help file",
    "IHP",
    "Watcom",144,0*2+1,6},

{  "\x53\x45\x47\x41\x20", // SEGA
    5,
    0x100,
    "Megadrive ROM",
    "BIN",
    "Sega",146,0*2+1,6},
{  "\x54\x4D\x52\x20\x53\x45\x47\x41",    // TMR SEGA
    8,
    0x7ff0,
    "GameGear/MasterSystem ROM",
    "GG",
    "Sega",147,0*2+0,6},
{  "BM",
    2,
    0,
    "Windows Bitmap",
    "BMP",
    "Microsoft Corp.",
    36,0*2+1,4},
{  "BM",
    2,
    0,
    "OS/2 Bitmap",
    "BMP",
    "IBM",
    148,0*2+1,4},
{  "XIMG",
    4,
    16,
    "GEM Bitmap",
    "IMG",
    "",
    149,0*2+0,4},
{  "PNT\0",
    4,
    0,
    "Truepaint Bitmap",
    "TPI",
    "",
    150,0*2+0,4},
{  "TMS\0",
    4,
    0,
    "Enhanced Simplex Bitmap",
    "ESM",
    "TMS",
    151,0*2+0,4},
{  "B&W256",
    6,
    0,
    "Imagelab Bitmap",
    "B&W",
    "",
    152,0*2+0,4},
{  "pM8",
    3,
    0,
    "Stad Bitmap",
    "PAC",
    "",
    153,0*2+0,4},
{  "\x4E\x45\x53\x1A", // NESú
    4,
    0x0,
    "NES ROM",
    "NES",
    "Nintendo",154,0*2+0,6},
/*
{  "\x08\x83@\x08\x85`\x08\x87\x80\x08", 
    10,
    0x2c2,
*/
{  "\x01\x00\x02", 
    3,
    0xe,
    "Atari Disk DUMP",
    "ST",
    "ATARI",155,0*2+0,6},
{  "\x01\x20\x05\x50\x05\xC0\x01", // ú úPúúú
    7,
    0x165,
    "MoonBlaster music",
    "MBM",
    "",156,0*2+1,1},  // Format MSX --------------------------------------
{  "USLM",
    4,
    0,
    "Useless Module",
    "USM",
    "Useless",157,0*2+1,1},  // By FreddyV -------------------------------
{  "\x7F\x45\x4C\x46\x01\x01\x01\x00",// ELFúúúú
    8,
    0,
    "Elf executable",    //--- Executable and Linking Format -----------
    "OUT",
    "USL",158,0*2+0,6},    //--- UNIX System Laboratories --------------
{  "[MDF]",
    5,                 // Gautier Porter aka Fire Dragon / Knights -----
    0,                          //--- Impulse pour la version [MDF3] ---
    "MusicDisk Factory File",
    "MDF",
    "Fire Dragon",159,4,1},
{  "NG\x00\x01\x00",
    5,
    0x0,
    "Norton Guide",
    "NG",
    "Peter Norton",160,1,6},
{   "PNCI\x00",
    5,
    0x0,
    "Norton Directory List",
    "NCD",
    "Peter Norton",161,0,6},
{   "PP20",
    4,
    0x0,
    "PowerPacker Archive",
    "PP",
    "?",162,0,3},
{  "\x1A\x00\x00\x03\x00",
    5,
    0,
    "Lotus Notes Database",
    "NSF",
    "Lotus",163,0*2+1,6},
{  "HSP",      // en 3: ?, en 4: "\x9B\x00\x02\x00"
    3,
    0x0,
    "OS/2 Help File",
    "HLP",
    "IBM",
    164,0*2+1,6},
{  "KKTmt",
    5,
    0,
    "Ketchup Killers Dialog Box",
    "KKT",
    "RedBug",
    165,0,6},
{  "\xFF""FONT   ",
    5,
    0,
    "Code Page Info(Standard Font)",
    "CPI",
    "",
    166,0,6},
{  "\x7F""DRFONT ",
    5,
    0,
    "Code Page Info(Enhanced Font)",
    "CPI",    //--- Utilis‚ par DR-DOS et Novell DOS -------------------
    "",
    167,0,6},
{  "DiamondWare Digitized",
    21,
    0,
    "DiamondWare Sound File",
    "DWD",
    "",
    168,0,2},
{  "AIFC",
    4,
    8,
   "Apple-C Audio File",
   "AFC",
   "Electronic Arts",
   169,0*2+0,2},
{  "\xF0\x7E\x00\x01",
    4,
    0,
   "MIDI sample dump",
   "SDS",
   "",
   170,0*2+0,2},
{  "\x00\x00\x03\xE8\x00",
    5,
    0,
   "Matlab sound files",
   "MAT",
   "",
   171,0*2+0,2},
{  "FIF",
    3,
    0,
    "Fractal Imager",
    "FIF",
    "",172,0*2+1,4},
{  "MCRB",
    4,
    0,
    "Ketchup Killers C. Macro",
    "KKP",
    "RedBug",
    173,0*2+1,6},
{  "\xCA\xFE\xBA\xBE\x00\x03\x00\x2D",
    8,
    0x0,
    "Applet Java",
    "CLASS",
    "Sun Microsystem",181,0*2+0,6},



// Dernier employe: 181 //marjo


/*--------------------------------------------------------------------*\
|- Gestion produit microsoft                                          -|
\*--------------------------------------------------------------------*/

{  NULL,
   0,
   0,
   "Microsoft Publisher Document",
   "PUB",
   "Microsoft Corp.",
   175,1*2+0,6},
{  NULL,
   0,
   0,
   "Microsoft Excel Document",
   "XLS",
   "Microsoft Corp.",
   176,1*2+0,6},
{  NULL,
   0,
   0,
   "Microsoft Word Document",
   "DOC",
   "Microsoft Corp.",
   177,1*2+0,6},
{  NULL,
   0,
   0,
   "Microsoft WinWorks Document",
   "WPS",
   "Microsoft Corp.",
   178,1*2+0,6},
{  NULL,
   0,
   0,
   "Microsoft WinWorks Database",
   "WDB",
   "Microsoft Corp.",
   179,1*2+0,6},
{  NULL,
   0,
   0,
   "Microsoft Powerpoint",
   "PPT",
   "Microsoft Corp.",
   180,1*2+0,6},

{  "\xD0\xCf\x11\xE0\xA1\xB1\x1A\xE1",
   8,
   0,
   "Microsoft File Format",
   "MS",
   "Microsoft Corp.",
   61,0*2+0,6},

/*--------------------------------------------------------------------*\
|-              structures … traiter en dernier ressort               -|
\*--------------------------------------------------------------------*/
{  "\xFF\xFF\xFF\xFF",
        4,
        0x0,
        "Device Driver",
        "SYS",
        "",93,0*2+0,6},
{  NULL,
     0,
     0,
     "MPEG 2 Audio Layer 2",
     "MP2",
     "ISO",
     135,1*2+1,2},
{  NULL,
     0,
     0,
     "MPEG 2 Audio Layer 3",
     "MP3",
     "ISO",
     136,1*2+1,2},
{  "\x80",
    1,
    0,
    "OBJect File",      //--- Les object sont fort mauvais ---------
    "OBJ",
    "",90,0*2+0,6},
{  "\x34\x12",
    2,
    0,
    "PCPaint/Pictor file",
    "PIC",
    "John Bridges",
    129,0*2+0,4},
{  "\x00\x95",
    2,
    4,
    "Animator Pro PIC File",
    "PIC",
    "Autodesk",
    127,0*2+0,4},
{  "\x19\x91",
    2,
    0,
    "Original Animator PIC File",
    "PIC",
    "Autodesk",
    128,0*2+0,4},
{  NULL,
    0,
    0,
    "dBase II file",
    "DBF",
    "Ashton-Tate",
    140,1*2+1,6},
{  NULL,
    0,
    0,
    "dBase III file",
    "DBF",
    "Ashton-Tate",
    141,1*2+1,6},
{  NULL,
    0,
    0,
    "Targa Picture",
    "TGA",
    "Truevision, Inc.",
    43,1*2+1,4},
{  "\x00\x01\x00\x00",
    4,
    0,
    "True Type Font",
    "TTF",
    "",
    65,0*2+0,6},
{  "PK",
    2,
    0,
    "ZIP Archive",
    "ZIP",
    "PKWARE Inc.",
    35,0*2+0,3},

{  NULL,
    0,
    0,
    "C Like Source File",
    "C",
    "K&R",
    174,1*4+1*2+0,6},
{  NULL,
    0,
    0,
    "HTML File",
    "HTML",
    "",
    104,1*4+1*2+0,6},
{  NULL,
    0,
    0,
    "Text File",
    "TXT",
    "",
    91,1*4+1*2+0,6},    //--- Laisser celui-ci dernier -----------------

/*--------------------------------------------------------------------*\
|-                    structures … ne pas toucher                     -|
\*--------------------------------------------------------------------*/

{  NULL,
        0,
        0,
        "Module",
        "*",
        "?",
        -1,0*2+0,1},
{  NULL,
        0,
        0,
        "Sample",
        "*",
        "?",
        -1,0*2+0,2},
{  NULL,
        0,
        0,
        "Archive",
        "*",
        "?",
        -1,0*2+0,3},
{  NULL,
        0,
        0,
        "Bitmap",
        "*",
        "?",
        -1,0*2+0,4},
{  NULL,
        0,
        0,
        "Animation",
        "*",
        "?",
        -1,0*2+0,5},
{  NULL,
        0,
        0,
        "Others",
        "*",
        "?",
        -1,0*2+0,6}
};


char *GetFile(char *p,char *Ficname);
void SplitName(char *filename,char *name,char *ext);

void Size2Chr(int Size,char *Taille);
                              // transforme la taille indiqu‚e en chaine

char IsTxt(RB_IDF *Info); //--Renvoit le pourcentage texte (>50=texte)--
short Infotxt(RB_IDF *Info);   //--- Test pour voir si c'est du texte --
short InfoSauce(RB_IDF *Info);
short Infomtm(RB_IDF *Info);
short Infong(RB_IDF *Info);
short Infoexe1(RB_IDF *Info);
short Infopsm(RB_IDF *Info);
short Info669(RB_IDF *Info);
short Infokkp(RB_IDF *Info);
short Infomod(RB_IDF *Info);   // Nø1
short Infodmf(RB_IDF *Info);
short Infoxm(RB_IDF *Info);    // Nø2
short Infoxi(RB_IDF *Info);
short Infos3m(RB_IDF *Info);   // Nø3
short Infout(RB_IDF *Info);
short Inforar(RB_IDF *Info);
short Infogif(RB_IDF *Info);
short Infofif(RB_IDF *Info);
short Infopcx(RB_IDF *Info);
short Infonsf(RB_IDF *Info);
short Infobmp(RB_IDF *Info);
short Infobmp2(RB_IDF *Info);
short Infomp2(RB_IDF *Info);
short Infomp3(RB_IDF *Info);
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
short Infohlp2(RB_IDF *Info);
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
short Infoico(RB_IDF *Info);
short Infoamf(RB_IDF *Info);
short Infopkm(RB_IDF *Info);
short Infogb(RB_IDF *Info);
short Infoptm(RB_IDF *Info);
short Infombm(RB_IDF *Info);
short Infousm2(RB_IDF *Info);
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
short Infodbf2(RB_IDF *Info);
short Infodbf3(RB_IDF *Info);
short Infoihp(RB_IDF *Info);
short Infosmd(RB_IDF *Info);
short Infobin(RB_IDF *Info);
short Infoc(RB_IDF *Info);

short Infopub(RB_IDF *Info);
short Infoxls(RB_IDF *Info);
short Infodoc(RB_IDF *Info);
short Infowps(RB_IDF *Info);
short Infowdb(RB_IDF *Info);
short Infoppt(RB_IDF *Info);


void ClearSpace(char *name);    //--- efface les espaces inutiles ------

ulong  ReadLng(RB_IDF *Info,ulong position,char type);
ushort ReadInt(RB_IDF *Info,ulong position,char type);
char   ReadChar(RB_IDF *Info,ulong position);
void   ReadStr(RB_IDF *Info,ulong position,char *str,short taille);

char sstricmp(char *dest,char *src);
char sstrnicmp(char *src,char *dest,int i);

ulong  InvLong(ulong entier);          // Inverse un ulong HILO <-> LOHI
ushort InvWord(ushort entier);        // Inverse un ushort HILO <-> LOHI



char tampon[1024];                   // Tampon pour faire n'importe quoi

/*--------------------------------------------------------------------*\
|-                        FIN DES DECLARATIONS                        -|
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|- positon par rapport … posfic                                       -|
|- type =1 LoHi --> PC                                                -|
|-      =2 HiLo --> Amiga                                             -|
\*--------------------------------------------------------------------*/
ulong ReadLng(RB_IDF *Info,ulong position,char type)
{
ulong entier;
int pos;
char *a;
char *b;
ulong result;

if (position>=(Info->sizemax-Info->posfic))
    return 0;

if ((position>(Info->posbuf+Info->sizebuf+4)) & (Info->fic!=NULL))
    {
    pos=ftell(Info->fic);
    fseek(Info->fic,position,SEEK_SET);
    fread(&entier,4,1,Info->fic);
    fseek(Info->fic,pos,SEEK_SET);
    }
else
    entier=*(ulong*)(Info->buffer+(ushort)position);

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
ushort ReadInt(RB_IDF *Info,ulong position,char type)
{
ushort entier;
char *a;
char *b;
ushort result;
int pos;

if (position>=(Info->sizemax-Info->posfic))
    return 0;


if ((position>(Info->posbuf+Info->sizebuf+2)) & (Info->fic!=NULL))
    {
    pos=ftell(Info->fic);
    fseek(Info->fic,position,SEEK_SET);
    fread(&entier,2,1,Info->fic);
    fseek(Info->fic,pos,SEEK_SET);
    }
else
    {
    entier=*(ushort*)(Info->buffer+(ushort)position);
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
void ReadStr(RB_IDF *Info,ulong position,char *str,short taille)
{
int pos;

if ((position+taille)>(Info->sizemax-Info->posfic))
    return;

if ((position>(Info->posbuf+Info->sizebuf+taille)) & (Info->fic!=NULL))
    {
    pos=ftell(Info->fic);
    fseek(Info->fic,position,SEEK_SET);
    fread(str,taille,1,Info->fic);
    fseek(Info->fic,pos,SEEK_SET);
    }
else
    memcpy(str,Info->buffer+(ushort)position,taille);


str[taille]=0;
ClearSpace(str);
}

/*--------------------------------------------------------------------*\
|-  positon par rapport … posfic                                      -|
\*--------------------------------------------------------------------*/
char ReadChar(RB_IDF *Info,ulong position)
{
int pos;
char a;

if ((position>(Info->posbuf+Info->sizebuf+1)) & (Info->fic!=NULL))
    {
    pos=ftell(Info->fic);
    fseek(Info->fic,position,SEEK_SET);
    fread(&a,1,1,Info->fic);
    fseek(Info->fic,pos,SEEK_SET);
    }
else
    a=Info->buffer[position];

return a;
}


/*--------------------------------------------------------------------*\
|-  Inverse un ulong (hihilolo -> lolohihi)                           -|
\*--------------------------------------------------------------------*/
ulong InvLong(ulong entier)
{
char *a;
char *b;
ulong result;

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
|-  Inverse un ushort (hihilolo -> lolohihi)                            -|
\*--------------------------------------------------------------------*/
ushort InvWord(ushort entier)
{
char *a;
char *b;
ushort result;

result=entier;
a=(char*)&entier;
b=(char*)&result;

b[1]=a[0];
b[0]=a[1];

return result;
}



void SplitName(char *filename,char *name,char *ext)     // name:8, ext:3
{
int n;

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


char sstricmp(char *dest,char *src)
{
int n;

for (n=0;n<strlen(src);n++)
    if (toupper(dest[n])!=toupper(src[n]))
        return 1;

return 0;
}

char sstrnicmp(char *src,char *dest,int i)
{
int n;

for (n=0;n<i;n++)
    if (toupper(dest[n])!=toupper(src[n]))
        return 1;

return 0;


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



void Traitefic(RB_IDF *Info)
{
int n;
int err;

int trv=-1;       //--- vaut -1 tant que l'on a rien trouv‚ ------------

Info->numero=-1;

n=0;    //--- recherche dans tous les formats --------------------------

memset(((char*)Info)+264,0,sizeof(RB_IDF)-264);

if (Info->path[0]!=0)
    {
    GetFile(Info->path,Info->filename);

    Info->fic=fopen(Info->path,"rb");
    if (Info->fic==NULL)
        {
        strcpy(Info->format,"Invalid Filename");
        strcpy(Info->fullname,"Unknow");
        return;
        }

    Info->posfic=0L;

    Info->buffer=(char*)malloc(32768);
    memset(Info->buffer,0,32768U);

    Info->sizebuf=fread(Info->buffer,1,32768,Info->fic);
    if (Info->sizebuf==0)
        {
        strcpy(Info->format,"Null File");
        strcpy(Info->fullname,"Unknow");

        return;
        }

    fseek(Info->fic,0,SEEK_END);
    Info->sizemax=ftell(Info->fic);        // --- taille du fichier ----
    fseek(Info->fic,0,SEEK_SET);

    Info->posbuf=Info->posfic;
    }
    else
    {
    Info->fic=NULL;
    Info->posfic=0;
    Info->posbuf=0;

    Info->buffer=Info->inbuf;
    Info->sizemax=Info->buflen;

    Info->sizebuf=Info->buflen;

    }


for (n=0;n<nbrkey-6;n++)  //--- Il faut ignorer les 6 derniers clefs ---
    {
    if ( (((K[n].other)&2)==0) & ((K[n].buf)!=NULL) )
        if (!memcmp(Info->buffer+K[n].pos,K[n].buf,K[n].len)) trv=n;

    if ( (((K[n].other)&2)==2) | ((((K[n].other)&1)==1) & (trv!=-1)) )
        {
        switch(K[n].numero)
            {
            case  2: err=Infoxm (Info); break;
            case 83: err=Infop00(Info); break;
            case 80: err=Infoico(Info); break;
            case 19: err=Infout (Info); break;
            case  3: err=Infos3m(Info); break;
            case  6: err=Infodmf(Info); break;
            case  4: err=Info669(Info); break;
            case 15: err=Infomtm(Info); break;
            case 34: err=Inforar(Info); break;
            case 37: err=Infogif(Info); break;
            case 172: err=Infofif(Info); break;
            case 40: err=Infopcx(Info); break;
            case 36: err=Infobmp(Info); break;
            case 148: err=Infobmp2(Info); break;
            case 135: err=Infomp2(Info); break;
            case 136: err=Infomp3(Info); break;
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
            case 43: err=Infotga(Info); break;
            case 52: err=Infompg(Info); break;
            case 31: err=Infodlz(Info); break;
            case 49: err=Infofli(Info); break;
            case 50: err=Infoflc(Info); break;
            case 63: err=Infoswg(Info); break;
            case 68: err=Infoams(Info); break;
            case 76: err=Infot64(Info); break;
            case 91: err=Infotxt(Info); break;
            case 174:err=Infoc(Info); break;
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
            case 140:err=Infodbf2(Info); break;
            case 141:err=Infodbf3(Info); break;
            case 144:err=Infoihp(Info); break;
            case 145:err=Infosmd(Info); break;
            case 146:err=Infobin(Info); break;
            case 156:err=Infombm(Info); break;
            case 157:err=Infousm2(Info); break;
            case 160:err=Infong(Info); break;
            case 163:err=Infonsf(Info); break;
            case 164:err=Infohlp2(Info);break;
            case 173:err=Infokkp(Info); break;
            case 175:err=Infopub(Info); break;
            case 176:err=Infoxls(Info); break;
            case 177:err=Infodoc(Info); break;
            case 178:err=Infowps(Info); break;
            case 179:err=Infowdb(Info); break;
            case 180:err=Infoppt(Info); break;
            default:     //--- Ca serait une erreur de ma part alors ---
                sprintf(Info->format,"Pingouin %d",K[n].numero);
                trv=1;
                break;
            }
        if (err==0)
            trv=n;
            else
            trv=-1;
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
    if (((K[trv].other)&8)==8)
        InfoSauce(Info);

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

if (Info->fic!=NULL)
    {
    fclose(Info->fic);
    free(Info->buffer);
    }
}


void ClearSpace(char *name)
{
char c,buf[1024];
short i,j;

i=0;    //--- navigation dans name -------------------------------------
j=0;    //--- position dans buf ----------------------------------------

while (name[i]==32) i++;

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

j--;

while (buf[j]==32) buf[j]=0,j--;

strcpy(name,buf);
}



short Infomod(RB_IDF *Info)
{
short chnl,instr;
char buf[5];

chnl=0;

ReadStr(Info,1080,buf,4);

if (!memcmp(buf,"TDZ1",4)) { strcpy(Info->format,"TakeTracker");
                                                     chnl=1; instr=31; }
if (!memcmp(buf,"TDZ2",4)) { strcpy(Info->format,"TakeTracker");
                                                     chnl=2; instr=31; }
if (!memcmp(buf,"TDZ3",4)) { strcpy(Info->format,"TakeTracker");
                                                     chnl=3; instr=31; }
if (!memcmp(buf,"M.K.",4)) { strcpy(Info->format,"Protracker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf,"M&K&",4)) { strcpy(Info->format,"Noisetracker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf,"M!K!",4)) { strcpy(Info->format,"Protracker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf,"4CHN",4)) { strcpy(Info->format,"Protracker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf,"6CHN",4)) { strcpy(Info->format,"PC-FTracker");
                                                     chnl=6; instr=31; }
if (!memcmp(buf,"8CHN",4)) { strcpy(Info->format,"PC-FTracker");
                                                     chnl=8; instr=31; }
if (!memcmp(buf,"16CH",4)) { strcpy(Info->format,"PC-FTracker");
                                                    chnl=16; instr=31; }
if (!memcmp(buf,"CD81",4)) { strcpy(Info->format,"Octalyser");
                                                     chnl=8; instr=31; }
       //--- octalyser on Atari Ste/falcon nombre de channel ? ---------
if (!memcmp(buf,"OCTA",4)) { strcpy(Info->format,"Oktotracker");
                                                     chnl=8; instr=31; }
if (!memcmp(buf,"FLT4",4)) { strcpy(Info->format,"StarTrekker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf,"RASP",4)) { strcpy(Info->format,"StarTrekker");
                                                     chnl=4; instr=31; }
if (!memcmp(buf,"FLT8",4)) { strcpy(Info->format,"StarTrekker");
                                                     chnl=8; instr=31; }
if (!memcmp(buf,"EXO4",4)) { strcpy(Info->format,"StarTrekker");
                                                     chnl=8; instr=31; }
                                                         // EXO OU EX0 ?
if (!memcmp(buf,"EXO8",4)) { strcpy(Info->format,"StarTrekker");
                                                     chnl=8; instr=31; }
if (!memcmp(buf,"FA04",4))
      { strcpy(Info->format,"Digital Tracker F030"); chnl=4; instr=31; }
if (!memcmp(buf,"FA08",4))
      { strcpy(Info->format,"Digital Tracker F030"); chnl=8; instr=31; }

if (chnl==0)  return 1;

strcat(Info->format," module");

ReadStr(Info,0,Info->fullname,20);

sprintf(Info->info, "%3d /%3d /%3d",instr,ReadChar(Info,950),chnl);
strcpy(Info->Tinfo,"Inst/Lngt/Chnl");

return 0;
}

short Infoihp(RB_IDF *Info)
{
ReadStr(Info,0x3A,Info->fullname,32);

return 0;
}

short Infosmd(RB_IDF *Info)
{
int n;

// Titre complet

for(n=0;n<16;n++)
    {
    Info->fullname[n*2]=ReadChar(Info,0x2290+n);
    Info->fullname[n*2+1]=ReadChar(Info,0x290+n);
    }
Info->fullname[32]=0;
ClearSpace(Info->fullname);

// Copyright

for(n=0;n<16;n++)
    {
    Info->message[0][n*2]=ReadChar(Info,0x2280+n);
    Info->message[0][n*2+1]=ReadChar(Info,0x280+n);
    }
Info->message[0][32]=0;
ClearSpace(Info->message[0]);


return 0;
}

short Infobin(RB_IDF *Info)
{

// Titre complet
ReadStr(Info,0x150,Info->fullname,32);

// Copyright
ReadStr(Info,0x100,Info->message[0],32);

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
sprintf(Info->message[0], "%3d /%3d /%3d",instr,ReadChar(950),chnl);
strcpy(Info->message[0],"Inst/Patt/Chnl"); */

return 0;
}

short Infoit(RB_IDF *Info)
{
ReadStr(Info,4,Info->fullname,26);

sprintf(Info->format,"Impulse Tracker %d.%02d module",
                               ReadChar(Info,0x29),ReadChar(Info,0x28));

sprintf(Info->info, "%3d /%3d /%3d",ReadInt(Info,0x22,1),
                             ReadInt(Info,0x26,1),ReadInt(Info,0x20,1));
strcpy(Info->Tinfo,"Inst/Patt/Orde");

return 0;
}

short Infomid(RB_IDF *Info)
{
char chunk[5];
int lng,pos;
short info;
short mess;
int tpos;


sprintf(Info->Tinfo, "Tracks");
sprintf(Info->info,"%3d",ReadInt(Info,10,2));

tpos=Info->posfic;


mess=-1;

pos=0;
lng=0;

while(1)
    {
    tpos=pos+lng;

    if (tpos>Info->sizemax) break;
    ReadStr(Info,tpos,chunk,4);
    tpos+=4;

    if (!((!strncmp(chunk,"MThd",4)) | (!strncmp(chunk,"MTrk",4))))
                                                              break;

    if (tpos>Info->sizemax) break;
    lng=ReadLng(Info,tpos,2);
    tpos+=4;
    pos=tpos;

    if (tpos>Info->sizemax) break;
    info=ReadInt(Info,tpos,2);
    tpos+=2;

    if (info==0xFF)
        {
        char chaine[40];
        char strlng;

        if (tpos>Info->sizemax) break;
        strlng=ReadChar(Info,tpos);
        tpos++;
        if ((strlng<1) | (strlng>7))
             continue;  //--- C'est pas une information texte ----------

        if (tpos>Info->sizemax) break;
        strlng=ReadChar(Info,tpos);
        tpos++;

        if (tpos>Info->sizemax) break;

        if (strlng>34)
            ReadStr(Info,tpos,chaine,34);
            else
            ReadStr(Info,tpos,chaine,strlng);

        tpos+=strlng;

        if (mess==-1)
            sprintf(Info->fullname,"%s",chaine);

        mess++;
        }
    }

Info->taille=(pos+lng)-Info->posfic;

return 0;
}


short Infomtm(RB_IDF *Info)
{
ReadStr(Info,4,Info->fullname,20);

sprintf(Info->info, "%3d /%3d",ReadChar(Info,27)+1,ReadChar(Info,33));
strcpy(Info->Tinfo,"SngL/Chnl"); // sngl = songlength

// le nombre de channel est contraire … la doc
// le nombre de sample aussi !!!

return 0;
}

short Info669(RB_IDF *Info)
{
char buf[3];

ReadStr(Info,0,buf,2);

if ( (memcmp(buf,"if",2)!=0) & (memcmp(buf,"JN",2)!=0) )
    return 1;

ReadStr(Info, 2,Info->message[0],36); //--- 108 octets en r‚alit‚ ------
ReadStr(Info,38,Info->message[1],36); //--- 108 octets en r‚alit‚ ------
ReadStr(Info,74,Info->message[2],36); //--- 108 octets en r‚alit‚ ------

if (!memcmp(buf,"if",2)) // 0x6669
    strcpy(Info->format,"669 Module");
    else
    strcpy(Info->format,"Extended 669 Module");

sprintf(Info->info, "%3d /%3d /%3d",ReadChar(Info,0x6E),
                                                 ReadChar(Info,0x6F),8);
strcpy(Info->Tinfo,"Inst/Patt/Chnl");

return 0;
}

short Infoxm(RB_IDF *Info)
{
char bname[25];

ReadStr(Info,17,Info->fullname,20);

ReadStr(Info,38,bname,20);
sprintf(Info->format,"Module %s",bname);

sprintf(Info->info, "%3d /%3d /%3d",
              ReadInt(Info,72,1),ReadInt(Info,64,1),ReadInt(Info,68,1));
strcpy(Info->Tinfo,"Inst/Patt/Chnl");

return 0;
}



short Infos3m(RB_IDF *Info)
{
short n,chnl;

ReadStr(Info,0,Info->fullname,28);

chnl=0;
for (n=0;n<32;n++)
    if ((ReadChar(Info,0x40+n)&128)!=128) chnl++;

sprintf(Info->info, "%3d /%3d /%3d",
                        ReadInt(Info,0x22,1),ReadInt(Info,0x24,1),chnl);
strcpy(Info->Tinfo,"Inst/Orde/Chnl");

return 0;
}

short Infout(RB_IDF *Info)
{
int chnl,instr,patt;
char version[5];
int res,patt_seq;

short sizestruct; //--- taille de la structure sample ------------------

char key[10];

ReadStr(Info,0,key,10);

if (memcmp(key,"MAS_UTrack",10))
        return 1;
        else
        switch(ReadChar(Info,14))
            {
            case '1': strcpy(version,"1.0"); sizestruct=64; break;
            case '2': strcpy(version,"1.4"); sizestruct=64; break;
            case '3': strcpy(version,"1.5"); sizestruct=64; break;
            case '4': strcpy(version,"1.6"); sizestruct=66; break;
            default:  strcpy(version,">1.6");sizestruct=66; break;
            }

res=ReadChar(Info,47)*32;
instr=ReadChar(Info,48+res);
patt_seq=49+(instr*sizestruct)+res;// la doc dit 48+ mais elle se trompe
patt=ReadChar(Info,patt_seq+257);
chnl=ReadChar(Info,patt_seq+256);


ReadStr(Info,15,Info->fullname,32);

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

short Inforar(RB_IDF *Info)
{
char Nomarch[256];
char buffer[256];
int SolidType;
int flag;

flag=ReadInt(Info,7+3,1);

SolidType=(flag&8);

//--- Test la solidit‚ ;) de l'archive ---------------------------------

strcpy(buffer,"");
if (SolidType) strcat(buffer,"Solid ");

if (flag & 1)       //--- MHD_MULT_VOL ---------------------------------
    sprintf(Nomarch,"%colume ",(SolidType) ? 'v':'V');
    else
    sprintf(Nomarch,"%crchive ",(SolidType) ? 'a':'A');

strcat(buffer,Nomarch);

sprintf(Info->message[0],"  Archive type: %20s",buffer);

return 0;
}

short Infogif(RB_IDF *Info)
{
signed char car;
short Lp,Hp;
char map,fond;
int PG,BP;
char fin=0;

int pos;

char buf[7];

ReadStr(Info,0,buf,6);

if (strncmp(buf,"GIF8",4)) return 1;
if (buf[5]!='a') return 1;

sprintf(Info->format,"Compuserve GIF8%ca",buf[4]);

pos=Info->posfic+6;

Lp=ReadInt(Info,pos,1);
pos+=2;

Hp=ReadInt(Info,pos,1);
pos+=2;

map=ReadChar(Info,pos);
pos++;

PG=(map&128)>>7;
BP=(map&7)+1;

if (Lp>9999) Lp=9999;
if (Hp>9999) Hp=9999;
if (BP>99) BP=99;

sprintf(Info->message[0],FORMPIC,Lp,Hp,BP);


fond=ReadChar(Info,pos);
pos++;

car=ReadChar(Info,pos);
pos++;

if (PG==1)
    pos+=(1<<BP)*3; //--- palette --------------------------------------

do {
car=ReadChar(Info,pos);
pos++;

switch(car)
    {
    case '!':   //--- Information --------------------------------------
        {
        short n;
        uchar lng;
        short mess;

        car=ReadChar(Info,pos);
        pos++;  //--- code information ---------------------------------

        mess=1;

        do
            {
            lng=ReadChar(Info,pos);
            pos++;

            if ((mess<10) & (car==-1) & (lng<40))  //--- Texte ---------
                {
                char appl[64];

                ReadStr(Info,pos,appl,lng);
                pos+=lng;   //--- comment ------------------------------

                for(n=0;n<strlen(appl);n++)
                    if (appl[n]<32)
                        break;

                if ((appl[n]==0) & (lng<=34) & (strlen(appl)>1) )
                    {
                    if ((lng==11) & (mess==1))
                        sprintf(Info->message[mess],
                                    " Application name: %16s",appl);
                        else
                        sprintf(Info->message[mess], " %-34s",appl);
                    mess++;
                    }
                }
            else
                pos+=lng;      //--- passe information -----------------
            }
        while(lng!=0);
        car=0;
        break;
        }
    case',':  //--- DATA -----------------------------------------------
        {
        short DX,DY,TX,TY;
        int PL,BP;
        unsigned char Code_size,t;

        DX=ReadInt(Info,pos,1);
        pos+=2;
        DY=ReadInt(Info,pos,1);
        pos+=2;
        TX=ReadInt(Info,pos,1);
        pos+=2;
        TY=ReadInt(Info,pos,1);
        pos+=2;

        map=ReadChar(Info,pos);
        pos++;

        PL=(map&128)>>7;
        BP=(map&7)+1;

        if (PL==1)      //--- tester -----------------------------------
            pos+=(1<<BP)*3;       //--- palette ------------------------

        Code_size=ReadChar(Info,pos);
        pos++;

        do
            {
            t=ReadChar(Info,pos);
            pos++;
            pos+=t;
            if (pos>Info->sizemax) fin=1;  //--- palette ---------------
            }
        while ((t!=0) & (!fin));

        }break;
    }  //--- fin du case -----------------------------------------------
}      //--- fin du while (buf) ----------------------------------------
while ((car!=';') & (!fin));

if (fin)
    {
    int n;
    strcpy(Info->message[0],"Corrupted file");
    for(n=1;n<10;n++)
        *(Info->message[n])=0;
    }
    else
    {
    Info->taille=(pos-Info->posfic);
    }

return 0;
}


short Infopcx(RB_IDF *Info)
{
int Lp,Hp;
char BP;

Lp=ReadInt(Info,8,1)+1;
Hp=ReadInt(Info,10,1)+1;

BP=ReadChar(Info,3);

sprintf(Info->message[0],FORMPIC,Lp,Hp,BP);

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

short Infobmp(RB_IDF *Info)      //--- Windows BMP ---------------------
{
long Lp,Hp;
int bps;

if (ReadInt(Info,14,1)!=40) return 1;

Info->taille=ReadLng(Info,2,1);    // Ordre des bytes pour le int: LO-HI

Lp=ReadLng(Info,18,1);
Hp=ReadLng(Info,22,1);
bps=ReadInt(Info,28,1);
                     
sprintf(Info->message[0],FORMPIC,Lp,Hp,bps);

return 0;
}

short Infofif(RB_IDF *Info)
{
long Lp,Hp;

Info->taille=ReadLng(Info,0x10,1); // Ordre des bytes pour le int: LO-HI

Lp=ReadLng(Info,6,1);
Hp=ReadLng(Info,10,1);

if ((Lp<=4096) & (Hp<=4096))
    sprintf(Info->message[0],FORMPIC,Lp,Hp,24);

return 0;
}

short Infobmp2(RB_IDF *Info)     // OS2 BMP
{
long Lp,Hp;
int bps;

if (ReadInt(Info,14,1)!=12) return 1;

Info->taille=ReadLng(Info,2,1);    // Ordre des bytes pour le int: LO-HI

Lp=ReadInt(Info,18,1);
Hp=ReadInt(Info,20,1);
bps=ReadInt(Info,24,1);
                     
sprintf(Info->message[0],FORMPIC,Lp,Hp,bps);

return 0;
}

short Infomp2(RB_IDF *Info)     // Mpeg2 layer 2
{
int info;
int deb,freq,tfreq[]={44100,48000,32000,0};

info=ReadInt(Info,0,2);

if ((info&0xFFF0)!=0xFFF0) return 1;
if ((info&0x0008)!=0x0008) return 1;      // mpeg 2
if ((info&0x0004)!=0x0004) return 1;      // layer 2

info=ReadInt(Info,2,2);

deb=((info&0xF000)/4096)*16;
freq=tfreq[(info&0x0C00)>>10];
sprintf(Info->message[0],FORMBIT,freq,deb);

return 0;
}

short Infomp3(RB_IDF *Info)     // Mpeg2 layer 3
{
int info;
int deb,freq,tfreq[]={44100,48000,32000,0};

info=ReadInt(Info,0,2);

if ((info&0xFFF0)!=0xFFF0) return 1;
if ((info&0x0008)!=0x0008) return 1;      // mpeg 2
if ((info&0x0006)!=0x0002) return 1;      // layer 3

info=ReadInt(Info,2,2);

deb=(((info&0xF000)/4096)-1)*16;
freq=tfreq[(info&0x0C00)>>10];
sprintf(Info->message[0],FORMBIT,freq,deb);

return 0;
}

short Infopkm(RB_IDF *Info)
{
short Lp,Hp;

Lp=ReadInt(Info,6,1);
Hp=ReadInt(Info,8,1);

sprintf(Info->message[0],FORMPIC,Lp,Hp,8);

return 0;
}

short Infoico(RB_IDF *Info)
{
short Lp,Hp,bps;

Lp=ReadChar(Info,6);
Hp=ReadChar(Info,7);

switch(ReadChar(Info,8))
    {
    case 2: bps=1; break;
    case 8: bps=3; break;
    case 16: bps=4; break;
    default: bps=0; break;
    }

if ( (Lp!=16) & (Lp!=32) & (Lp!=64) ) return 1;
if ( (Hp!=16) & (Hp!=32) & (Hp!=64) ) return 1;
if (bps==0) return 1;
                     
sprintf(Info->message[0],FORMPIC,Lp,Hp,bps);

return 0;
}

short Inforaw(RB_IDF *Info)
{
short int Lp,Hp,Bp;
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
    sprintf(Info->message[0],FORMPIC,Lp,Hp,Bp);
    else
    sprintf(Info->message[0],
                          " Picture is %4d * %4d  (%3d col.)",Lp,Hp,Bp);

return 0;
}

short Infohsi(RB_IDF *Info)
{
ushort Lp,Hp;

Lp=ReadInt(Info,6,2);  //--- Motorola mode -----------------------------
Hp=ReadInt(Info,8,2);

sprintf(Info->message[0],FORMPIC,Lp,Hp,24);

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
ulong pos;
char chunk[5];
char cont;
int tpos;

tpos=Info->posfic+8;

patt=0;
inst=0;
chnl=8;

do
{
if (tpos>Info->sizemax) break;
ReadStr(Info,tpos,chunk,4);
tpos+=4;


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
    tpos-=4;
    break;
    }

if (tpos>Info->sizemax) break;
pos=ReadLng(Info,tpos,2);
tpos+=4;

if (cont==1) inst++;

if (cont==5)
    patt=ReadInt(Info,tpos,2);

tpos+=pos;
}
while(1);

sprintf(Info->info, "%3d /%3d /%3d",inst,patt,chnl);
strcpy(Info->Tinfo,"Inst/Patt/Chnl");

Info->taille=(tpos-Info->posfic);

return 0;
}

short Infoau(RB_IDF *Info)
{
ulong dataLocation,dataSize,dataFormat,samplingRate;

dataLocation=ReadLng(Info,4,2);
dataSize=ReadLng(Info,8,2);
dataFormat=ReadLng(Info,12,2);
samplingRate=ReadLng(Info,16,2);

sprintf(Info->message[0],FORMSMP,samplingRate);
if (dataFormat==1)
    sprintf(Info->message[1]," Format:       8-bit mu-law samples");

ReadStr(Info,24,Info->message[9],30);

Info->taille=dataSize+dataLocation;

return 0;
}

short Infoiff(RB_IDF *Info)
{
ulong pos;
ushort samplingRate;
char chunk[5];
char fullname[255];
char buf[5];
int tpos;

ReadStr(Info,0,buf,4);
if (memcmp(buf,"FORM",4))
    return 1;

ReadStr(Info,8,buf,4);
if ( (memcmp(buf,"8SVX",4)!=0) & (memcmp(buf,"16SV",4)!=0) )
    return 1;

if (memcmp(buf,"16SV",4))
    strcpy(Info->format,"Amiga 16-bit sampled voice");

if (memcmp(buf,"8SVX",4))
    strcpy(Info->format,"Amiga 8-bit sampled voice");


Info->taille=ReadLng(Info,4,2)+8;


tpos=Info->posfic+12;

do
{
if (tpos>Info->sizemax) break;
ReadStr(Info,tpos,chunk,4);
tpos+=4;

if (!memcmp(chunk,"NAME",4))
    {
    if (tpos>Info->sizemax) break;
    pos=ReadLng(Info,tpos,2);
    tpos+=4;
    if (pos<256L)
        {
        ReadStr(Info,tpos,fullname,pos);
        tpos+=pos;

        if (strlen(fullname)<40)
            strcpy(Info->fullname,fullname);
        }
    continue;
    }
if (!memcmp(chunk,"VHDR",4))
    {
    tpos+=16;
    if (tpos>Info->sizemax) break;
    samplingRate=ReadInt(Info,tpos,2);
    tpos+=2;
    sprintf(Info->message[0],FORMSMP,samplingRate);
    break;
    }

break;
} while(1);


return 0;
}

short Infolbm(RB_IDF *Info)
{
ulong pos;
char chunk[5];
char fullname[255];
short Lp,Hp;
uchar BP;
char buf[5],buf2[5];
int tpos;

ReadStr(Info,0,buf,4);
if (memcmp(buf,"FORM",4))   return 1;

ReadStr(Info,8,buf,4);
ReadStr(Info,12,buf2,4);
if ( (memcmp(buf,"ILBM",4))
   & (memcmp(buf,"HPBM",4))
   & (memcmp(buf2,"BMHD",4)) )   return 1;

Info->taille=ReadLng(Info,4,2)+8;

tpos=Info->posfic+12;

do
{
if (tpos>Info->sizemax) break;
ReadStr(Info,tpos,chunk,4);
tpos+=4;

if (!memcmp(chunk,"NAME",4))
    {
    if (tpos>Info->sizemax) break;
    pos=ReadLng(Info,tpos,2);
    tpos+=4;
    if (pos<256L)
        {
        ReadStr(Info,tpos,fullname,pos);
        tpos+=pos;
        if (strlen(fullname)<40) strcpy(Info->fullname,fullname);
        }
    continue;
    }

if (!memcmp(chunk,"BMHD",4))
    {
    tpos+=4;
    Lp=ReadInt(Info,tpos,2);
    tpos+=2;
    Hp=ReadInt(Info,tpos,2);
    tpos+=2;
    tpos+=4;
    BP=ReadChar(Info,tpos);
    tpos++;

    if (Lp>9999) Lp=9999;
    if (Hp>9999) Hp=9999;
    if (BP>99) BP=99;

    sprintf(Info->message[0],FORMPIC,Lp,Hp,BP);
    break;
    }

break;
} while(1);


return 0;
}



short Infoarj(RB_IDF *Info)
{
if ( (ReadChar(Info,0)!=0x60) | (ReadChar(Info,1)!=234)
                                            | (ReadChar(Info,4)!=0x1E) )
        return 1;

return 0;
}


/*
struct MMD0 {
        ulong   id;
        ulong   modlen;  //--- length of entire module -----------------
        struct MMD0song *song;
        ulong   reserved0;
        struct MMD0Block **blockarr;
        ulong   reserved1;
        struct InstrHdr **smplarr;
        ulong   reserved2;
        struct MMD0exp *expdata;
        ulong   reserved3;
        Uushort   pstate;  //--- some data for the player routine --------
        Uushort   pblock;
        Uushort   pline;
        Uushort   pseqnum;
        ushort    actplayline;
        Uuchar   counter;
        Uuchar   extra_songs; //--- number of songs - 1 -----------------
}; // length = 52 bytes


struct MMD0song {
        struct MMD0sample sample[63];
        Uushort   numblocks;
        Uushort   songlen;
        Uuchar   playseq[256];
        Uushort   deftempo;
        uchar    playtransp;
        Uuchar   flags;
        Uuchar   flags2;
        Uuchar   tempo2;
        Uuchar   trkvol[16];
        Uuchar   mastervol;
        Uuchar   numsamples;
}; */ // length = 788 bytes


short Infomed(RB_IDF *Info)
{
sprintf(Info->format,"OctaMED ver. %c.0 module",ReadChar(Info,3)+1);

Info->taille=ReadLng(Info,4,2);

return 0;
}

short Infovoc(RB_IDF *Info)
{
short type;
int size,pos;
uchar Csize[40];
short mess;
int tpos;


tpos=Info->posfic+0x1A;

mess=0;

while(1)
        {
        if (tpos>Info->sizemax) break;
        type=ReadChar(Info,tpos);
        tpos++;
        if (type==0) break;

        ReadStr(Info,tpos,Csize,3);
        tpos+=3;
        size=((int)Csize[0])+((int)Csize[1])*256+((int)Csize[2])*65536;
        pos=tpos;

        if ( (type==1) & (mess<9) )
                {
                ReadStr(Info,tpos,Csize,2);
                tpos+=2;
                sprintf(Info->message[mess],FORMSMP
                                              ,1000000L/(256-Csize[0]));
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
                        ReadStr(Info,tpos,Csize,size);
                        tpos+=size;
                        strcpy(Info->fullname,(char*)Csize);
                        }
                        else
                if ( (size<=34) & (mess<10) )                //msg ASCII
                        {
                        ReadStr(Info,tpos,Csize,size);
                        tpos+=size;
                        sprintf(Info->message[mess],"  %s",Csize);
                        mess++;
                        }

                }

        tpos=pos+size;
        }

Info->taille=pos-Info->posfic;

return 0;
}

short Infopsm(RB_IDF *Info)
{
Info->taille=ReadLng(Info,4,1)+12;

return 0;
}

short Infoexe1(RB_IDF *Info)
{
unsigned short modulo,file,header,overlay,ip,cs;
long pe;
char htc[]={0xBA,0,0,0x2E,0x89,0x16,0,0,0xB4,0x30,0xCD,0x21}; //---  C++
char hqc[]={0xB4,0x30,0xcd,0x21,0X3C,0x02,0x73,0x05,0x33,0xC0}; //--- QC
char hqb[]={0xA1,0x02,0x00,0x2E,0xA3,0x36};            //--- Quick basic
char htp[]={0x9A,0x00,0x00};   // header TP
char hai[]={0x0E,0x07,0xB9,0x14,0x00,0xBE,0x00,0x01}; //------------ AIN
char pkl[]={0xB8,0,0,0xBA,0,0,0x05,0,0,0x3B,0x06,0x02,0x00}; //-- PKLITE
char exe[]={0x8B,0xE8,0x8C,0xC0,0x05,0x10,0,0x0E,0x1F,0xA3,0x04,0};
                                                           //--- Exepack
char lze[]={0x06,0x0E,0x1F,0x8B,0x0E,0x0C,0x00,0X8B,0xF1,0x4E}; // LZEXE

char dsh[]={0x06,0xE8,0x00,0x00,0x5E}; //------------------------- LZEXE
char gws[]={0xE9,0x9A,0x14,0xCE,0xA7,0x01,0x00,0x05,0x00}; //------- GWS
char rdb[]="RBID";  // header Redbug File information

char buf2[33];

char buf3[3];

char *buf;


//

buf=Info->buffer;

Info->os=1;
sprintf(Info->Tinfo,"Operating Sys.");
sprintf(Info->info,"DOS");

modulo=ReadInt(Info,0x02,1);
file=ReadInt(Info,0x04,1);
header=ReadInt(Info,0x08,1);
overlay=ReadInt(Info,0x1A,1);
ip=ReadInt(Info,0x14,1);
cs=ReadInt(Info,0x16,1);

if ( (cs==0xFFF0) & (ip==0x100) )
        pe=(header*16);
        else
        pe=ip+(header*16)+(cs*16);

ReadStr(Info,Info->posfic+(int)pe,buf2,32);



sprintf(Info->message[3]," Header is %18ld bytes",((int)header)*16);

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

//

if (!memcmp(buf+0x55,"PMODE",5))
        {
        sprintf(Info->message[2]," Using   ");
        memcpy((Info->message[2])+9,buf+0x55,27);
        (Info->message[2])[37]=0;
        }
if (!memcmp(buf+0x25C,"DOS/4G",6))
        sprintf(Info->message[0]," Using                       DOS/4G");

if (!memcmp(buf+0x55,"PMODE/W",7))
        sprintf(Info->message[0]," Using                      PMODE/W");


if (!memcmp(buf+0x1C,"L.S.",4))
        sprintf(Info->message[0]," Crypt  by  LIGHT SHOW/Eclipse 1.13");
if (!memcmp(buf+0x1C,"@KLS",4))
        sprintf(Info->message[0]," Crypt. by  LIGHT SHOW/Eclipse 1.15");
if (!memcmp(buf2,dsh,5))
        sprintf(Info->message[0]," Crypted by                 DSHIELD");

if (!memcmp(buf2+3,"WATCOM C/C++",12))
        sprintf(Info->message[0]," Compiled with         WATCOM C/C++");
if (!memcmp(buf+0x371,"WATCOM C",8))
        sprintf(Info->message[0]," Compiled with             WATCOM C");
if (!memcmp(buf+0x279,"WATCOM C",8))
        sprintf(Info->message[0]," Compiled with             WATCOM C");
if (!memcmp(buf+0x273,"WATCOM C",8))
        sprintf(Info->message[0]," Compiled with             WATCOM C");
if (!memcmp(buf2,htp,3))
        sprintf(Info->message[0]," Compiled with         TURBO PASCAL");
if ( (buf2[0]==htc[0]) & (!memcmp(buf2+3,htc+3,3))
                                           & (!memcmp(buf2+8,htc+8,4)) )
        sprintf(Info->message[0]," Compiled with          TURBO C/C++");
if (!memcmp(buf2,hqc,9))
        sprintf(Info->message[0]," Compiled with              QUICK C");
if (!memcmp(buf2+3,hqb,6))
        sprintf(Info->message[0]," Compiled with          QUICK BASIC");

if (!memcmp(buf2,gws,9))
    {
    char temp[32];
    ReadStr(Info,Info->posfic+(int)pe+29,temp,32);
    sprintf(Info->message[0]," Created by%24s",temp);
    }

/*
sprintf(Info->message[0],"%02X %02X %02X %02X %02X %02X %02X %02X %02X",
    buf2[0],buf2[1],buf2[2],buf2[3],buf2[4],buf2[5],buf2[6],buf2[7],buf2[8]);
*/

memset(buf3,0,3);

ReadStr(Info,Info->posfic+(*(ushort*)(buf+0x3C)),buf3,2);

if (!memcmp(buf3,"NE",2))
    {
    unsigned char os,lv,hv,n;
    ulong pos;

    os=*(buf+(*(ushort*)(buf+0x3C)+0x36));
    sprintf(Info->info,"Unknown OS");
    if ((os&1)==1)
        sprintf(Info->info,"OS2");
    if ((os&2)==2)
        {
        sprintf(Info->info,"WIN ");
        lv=*(buf+(*(ushort*)(buf+0x3C)+0x3E));
        hv=*(buf+(*(ushort*)(buf+0x3C)+0x3F));
        switch(hv)
            {
            case  4:        sprintf(buf2,"95"); break;
            default:        sprintf(buf2,"%d.%02d",hv,lv); break;
            }
        strcat(Info->info,buf2);
        }
    if ((os&4)==4)
        sprintf(Info->info,"WIN386");


    pos=ReadLng(Info,*(ushort*)(buf+0x3C)+0x2C,1)+1;

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

    if ((ReadInt(Info,*(ushort*)(buf+0x3C)+0x0C,1)&0x8000)==0x8000)
        {
        strcpy(Info->ext,"DLL");
        strcpy(Info->format,"Library File");
        }
    }


if (!memcmp(buf3,"LE",2))
    {
    unsigned char os;
    ulong pos;

    os=*(buf+(*(ushort*)(buf+0x3C)+0x0A));
    sprintf(Info->info,"Unknow OS");
    if (os==1) sprintf(Info->info,"OS/2");
    if (os==2) sprintf(Info->info,"WINDOWS");
    if (os==3) sprintf(Info->info,"European DOS 4.0");
    if (os==4) sprintf(Info->info,"WIN386");

    pos=ReadLng(Info,*(ushort*)(buf+0x3C)+0x58,1);
    pos+=*(ushort*)(buf+0x3C);
    ReadStr(Info,pos+1,buf2,buf[pos]);
    strcpy(Info->fullname,buf2);
    }

if (!memcmp(buf3,"PE",2))
    {
    sprintf(Info->info,"WIN32");
    }

if (!memcmp(buf3,"LX",2))
    {
    sprintf(Info->info,"OS/2");
    }

if (!memcmp(buf3,"W3",2))
    {
    sprintf(Info->info,"WIN386 file");
    }

if (!memcmp(buf3,"DL",2))
    {
    sprintf(Info->info,"HP 100LX/200LX");
    }

if (!memcmp(buf3,"MP",2))
    {
    sprintf(Info->info,"old PharLap .EXP");
    }

if (!memcmp(buf3,"P2",2))
    {
    sprintf(Info->info,"PharLap 286 .EXP");
    }

if (!memcmp(buf3,"P3",2))
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

ReadStr(Info,21,Info->fullname,22);

ReadStr(Info,44,bname,20);
sprintf(Info->format,"Instr. %s",bname);

return 0;
}


short Infotpu(RB_IDF *Info)
{
char b;

b=ReadChar(Info,3);

if (b=='6')
    strcpy(Info->format,"Turbo Pascal Unit Ver 5.5");

if (b=='9')
    strcpy(Info->format,"Turbo Pascal Unit Ver 6.0");
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
Info->taille=ReadLng(Info,12,1);

if (ReadLng(Info,0x23,2)==256)
    if (ReadInt(Info,0x27,1)<30)
        ReadStr(Info,0x29,Info->fullname,ReadInt(Info,0x27,1));

return 0;
}

short Infohlp2(RB_IDF *Info)
{
ReadStr(Info,0x6B,Info->fullname,48);

return 0;
}

short Infostm(RB_IDF *Info)    // 8 channel
{
ReadStr(Info,0,Info->fullname,20);

if (ReadChar(Info,29)==2)
   sprintf(Info->format,"Mod. Scream Tracker V%d.%02d",ReadChar(Info,30)
                                                    ,ReadChar(Info,31));
if (ReadChar(Info,29)==1)
   sprintf(Info->format,"Song Scream Tracker V%d.%02d",ReadChar(Info,30)
                                                    ,ReadChar(Info,31));

strcpy(Info->Tinfo,"Patt/Chnl");
sprintf(Info->info,"%3d / 8",ReadChar(Info,33));
return 0;
}

short Infofar(RB_IDF *Info)    // 16 channel
{
int a;

ReadStr(Info,4,Info->fullname,40);

a=ReadChar(Info,49);

sprintf(Info->format,"Farandole Module V%d.%d",a/16,a&15);

return 0;
}

short Infofsm(RB_IDF *Info)
{
ReadStr(Info,0x04,Info->fullname,32);
return 0;
}

short Infousm2(RB_IDF *Info)
{
int instr,patt,chnl;

instr=ReadInt(Info,46,1);
patt=ReadInt(Info,44,1);
chnl=ReadInt(Info,42,1);

sprintf(Info->info, "%3d /%3d /%3d",instr,patt,chnl);
strcpy(Info->Tinfo,"Inst/Patt/Chnl");

ReadStr(Info,6,Info->fullname,32);

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
ushort lng;

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
ulong taille;
int tpos;

taille=Info->posfic+5;

while(1)
    {
    tpos=taille;


    if (tpos>Info->sizemax) break;
    ReadStr(Info,tpos,chunk,2);
    tpos+=2;

    if (tpos>Info->sizemax) break;
    taille=ReadLng(Info,tpos,1);
    tpos+=4;

    taille+=tpos;

    if (!memcmp(chunk,"IN",2))
        {
        char nom[33];

        ReadStr(Info,tpos,nom,32);
        tpos+=32;

        nom[30]=0;
        memcpy(Info->fullname,nom,31);

        ReadStr(Info,tpos,Info->composer,20);
        tpos+=20;
        continue;
        }
    if (!memcmp(chunk,"PN",2)) continue;
    if (!memcmp(chunk,"ME",2)) continue;
    if (!memcmp(chunk,"PA",2)) continue;
    if (!memcmp(chunk,"TR",2)) continue;
    if (!memcmp(chunk,"IS",2)) continue;
    if (!memcmp(chunk,"SA",2)) continue;

    if (!memcmp(chunk,"II",2)) continue;
    if (!memcmp(chunk,"VE",2)) continue;
    if (!memcmp(chunk,"PE",2)) continue;
    if (!memcmp(chunk,"FE",2)) continue;

    tpos-=6;
    break;
    }

Info->taille=tpos-Info->posfic;

return 0;
}

short Infospl(RB_IDF *Info)
{
ReadStr(Info,0x05,Info->fullname,22);

sprintf(Info->message[0],FORMSMP,ReadInt(Info,45,1));
return 0;
}

/*
short Infogrp(RB_IDF *Info)
{
char name[10];
char key2[]={0x00,0x80,0xFF,0xFF,0x0A,0x00};
char key[]="PMCC";
int tpos;

Info->taille=ReadInt(Info,6,1);

tpos=Info->taille+Info->posfic;

if (fread(name,6,1,Info->fic)==6)
    {
    if (!memcmp(name,key2,6))
        if (fread(name,10,1,Info->fic)==10)
            {
            if (!memcmp(name,key,4))
                Info->taille+=(16+((*(ushort*)(name+6))+1)*(*(ushort*)(name+8)));
            }


return 0;
}
*/


short Infotga(RB_IDF *Info)
{
long Lp,Hp;
char bps;
char key1[]={0,1,1,0};
char key2[]={0,0,2,0};
char key[]={0,0,0,0};

char buf[13];

ReadStr(Info,0,buf,12);

if (memcmp(buf+8,key,4)) return 1;

if (!memcmp(buf,key1,4))
    sprintf(Info->format,"Targa Picture 256 Col.");
    else
    if (!memcmp(buf,key2,4))
        sprintf(Info->format,"Targa Picture True Col.");
        else
        return 1;

Lp=ReadLng(Info,12,1);
Hp=ReadLng(Info,14,1);
bps=ReadChar(Info,16);
                     
sprintf(Info->message[0],FORMPIC,Lp,Hp,bps);

return 0;
}

short Infodbf2(RB_IDF *Info)
{
char day,month,year;

if (ReadChar(Info,0)!=2) return 1;

day=ReadChar(Info,4);
month=ReadChar(Info,3);
year=ReadChar(Info,5);

if ((month<1) | (month>12)) return 1;
if (day>31) return 1;

sprintf(Info->message[0]," Last update:              %2d/%2d/%2d",
                                                        day,month,year);
sprintf(Info->message[1]," %-5d data records",ReadInt(Info,1,1));

return 0;
}

short Infodbf3(RB_IDF *Info)
{
char day,month,year;

if ( (ReadChar(Info,0)!=3) & (ReadChar(Info,0)!=(char)0x83) )  return 1;

day=ReadChar(Info,3);
month=ReadChar(Info,2);
year=ReadChar(Info,1);

if ((month<1) | (month>12)) return 1;
if (day>31) return 1;

sprintf(Info->message[0]," Last update:              %2d/%2d/%2d",
                                                        day,month,year);
sprintf(Info->message[1]," %-5d data records",ReadLng(Info,4,1));

return 0;
}

short Infompg(RB_IDF *Info)
{
char key[]={0x00,0x00,0x01,0xBA,0x21,0x00};
char key2[]={0x00,0x00,0x01,0xB3,0x0A,0x00,0x78};

char buf[8];

ReadStr(Info,0,buf,7);

if ( (memcmp(buf,key,6)) &
     (memcmp(buf,key2,7)) )  return 1;

return 0;
}


short Infodlz(RB_IDF *Info)
{
char *buf;

buf=Info->buffer;
Info->taille=(ulong)(ReadInt(Info,0xA,1))+17+
                                     (ulong)(ReadChar(Info,0x9))*65536L;
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

sprintf(Info->message[0],FORMPIC,Lp,Hp,BP);
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


sprintf(Info->message[0],FORMPIC,Lp,Hp,BP);
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

sprintf(Info->message[0],FORMPIC,Lp,Hp,BP);
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
ReadStr(Info,8,Info->fullname,ReadChar(Info,7));
return 0;
}


short Infot64(RB_IDF *Info)
{

ReadStr(Info,0,tampon,14);
if ( (memcmp(tampon,"C64S tape file",14)!=0) &
     (memcmp(tampon,"C64 tape image",14)!=0) ) return 1;

ReadStr(Info,0x28,Info->fullname,24);
return 0;
}

/*--------------------------------------------------------------------*\
|- 00  08  C64File (null terminated)                                  -|
|- 08  17  File Name (null terminated)                                -|
|- 25  01  Record Size. P00 = 0                                       -|
\*--------------------------------------------------------------------*/
short Infop00(RB_IDF *Info)
{
ReadStr(Info,0x08,Info->fullname,17);
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

short Infombm(RB_IDF *Info)
{
ReadStr(Info,0xCF,Info->fullname,40);
return 0;
}

short Infousm(RB_IDF *Info)
{
ReadStr(Info,4,Info->fullname,32);

return 0;
}

short Infong(RB_IDF *Info)
{
ReadStr(Info,8,Info->fullname,40);

return 0;
}

short Infokkp(RB_IDF *Info)
{
ReadStr(Info,4,Info->fullname,32);

return 0;
}

short Infonsf(RB_IDF *Info)
{
int n;

ReadStr(Info,0xC8,Info->fullname,80);

for(n=0;n<80;n++)
    if (Info->fullname[n]==0x0A)
        Info->fullname[n]=0;

Info->taille=ReadLng(Info,0x58,1);

return 0;
}


/*--------------------------------------------------------------------*\
|- gestion des fichiers textes                                        -|
\*--------------------------------------------------------------------*/
short Infoc(RB_IDF *Info)
{
char pasok=1;

if (IsTxt(Info)<50) return 1;

*Info->ext=0;

ReadStr(Info,0,tampon,80);
if (!strncmp(tampon,"/*",2)) pasok=0;
if (!strncmp(tampon,"//",2)) pasok=0;
if (!strncmp(tampon,"#define ",8)) pasok=0;
if (!strncmp(tampon,"#include",8)) pasok=0;
if (!strncmp(tampon,"#ifdef",6)) pasok=0;
if (!strncmp(tampon,"#ifndef",7)) pasok=0;
if (!strncmp(tampon,"void ",5)) pasok=0;
if (!strncmp(tampon,"int ",4)) pasok=0;

return pasok;
}


short Infotxt(RB_IDF *Info)
{
unsigned short pos;
unsigned char a;
int i,val;

val=IsTxt(Info);
if (val<50) return 1;

sprintf(Info->message[0]," English probability:         %3d %%",val);

SplitName(Info->filename,NULL,Info->ext);


//--- Recherche du nom -------------------------------------------------

pos=0;

do
{
a=0;

while ( ((Info->buffer[pos]>123)| (Info->buffer[pos]<65)) & (pos<1000) )
        pos++;

ReadStr(Info,pos,Info->fullname,60);


for (i=0;i<60;i++)
        if (Info->fullname[i]<32) Info->fullname[i]=0;

if (!sstrnicmp(Info->fullname,"REM",3)) a=3;
if (!sstrnicmp(Info->fullname,"//",2)) a=2;

if (!sstrnicmp(Info->fullname,"include",7)) {
        a=1;
        while ( (Info->buffer[pos]!=13) & (pos<1000) ) pos++;
        }

if (strlen(Info->fullname)<3) a=1;


if (a==0)
        pos+=strlen(Info->fullname);
        else
        pos+=a;

}
while ( ( (!sstrnicmp(Info->fullname,"@ECHO",5)) |
                  (!sstrnicmp(Info->fullname,"ECHO",4)) |
                  (!sstrnicmp(Info->fullname,"#INCLUDE",8)) |
                  (a!=0)
                ) & (pos<1000)
          );

Info->fullname[79]=0;

if (pos>999) Info->fullname[0]=0;

return 0;
}


char IsTxt(RB_IDF *Info)
{
unsigned short pos;
unsigned char a;

/*--------------------------------------------------------------------*\
|-  Recherche de la valeur texte par statistique                      -|
\*--------------------------------------------------------------------*/

unsigned short tab[256];
short ord[256];
short i,j,n;
char col[]=" etanoris-hdlc";
int val=0;
unsigned char c;
unsigned short nm;

nm=0;

for (n=0;n<256;n++)
    {
    tab[n]=0;
    ord[n]=n;
    }

for (pos=0;pos<Info->sizebuf;pos++)
    {
    c=ReadChar(Info,pos);
    if ((c>='A') & (c<='Z')) c=c+'a'-'A';
    tab[c]++;
    }

for(i=32;i<126;i++)
    nm+=tab[i];
nm+=tab[10]+tab[13];

for (i=0;i<256;i++)
    for(j=i;j<256;j++)
        if (tab[ord[i]]<tab[ord[j]])
            {
            n=ord[i];
            ord[i]=ord[j];
            ord[j]=n;
            }

for (i=0;i<15;i++)
    {
    for (n=0;n<15;n++)
        if (ord[i]==col[n])
            {
            val+=100+((50*abs(i-n))/15);
            break;
            }
    }

val=((val/15)*nm)/Info->sizebuf;
if (val<50)
    {
    for (pos=0;pos<Info->sizebuf;pos++)
        {
        a=(unsigned char)(Info->buffer[pos]);
        if ((a<32) &
            (a!=10) & (a!=13) &          //--- passage de ligne --------
            (a!=12) &                    //--- passage de page ---------
            (a!=17) &
            (a!=16) &
            (a!=9) &                     //--- tabulation --------------
            (a!=0) &                     //--- erreur buffer -----------
            (a!=2) &                     //--- la petite tˆte ----------
            (a!=3) &                     //--- le petit coeur ----------
            (a!=26))
            {                            //--- fin de fichier ----------
            // cprintf("%d",Info->buffer[pos]);
            return 0;
            }
        }
    val=50;
    }

return val;
}

short Infohtm(RB_IDF *Info)
{
unsigned long n,d;
char *buf;
char titre[64];

buf=Info->buffer;

Info->fullname[0]=0;

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
                if (!sstricmp(titre,"HTML")) return 0;
                if (!sstricmp(titre,"TITLE")) return 0;
                if (!sstricmp(titre,"PRE")) return 0;
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


/*--------------------------------------------------------------------*\
|- produit microsoft                                                  -|
\*--------------------------------------------------------------------*/

short Infoms(RB_IDF *Info)
{
char key[]="\xD0\xCf\x11\xE0\xA1\xB1\x1A\xE1";
char key2[8];

ReadStr(Info,0,key2,8);
return (memcmp(key,key2,8)!=0);
}


short Infopub(RB_IDF *Info)
{
long pos;
if (Infoms(Info)) return 1;
pos=(ReadLng(Info,0x30,1)+1)*0x200;
return (ReadInt(Info,pos+0x50,1)!=0x1201);
}

short Infoxls(RB_IDF *Info)
{
long pos;
if (Infoms(Info)) return 1;
pos=(ReadLng(Info,0x30,1)+1)*0x200;
return (ReadInt(Info,pos+0x50,1)!=0x0810);
}

short Infodoc(RB_IDF *Info)
{
long pos;
if (Infoms(Info)) return 1;
pos=(ReadLng(Info,0x30,1)+1)*0x200;
return (ReadInt(Info,pos+0x50,1)!=0x0900);
}

short Infowps(RB_IDF *Info)
{
long pos;
if (Infoms(Info)) return 1;
pos=(ReadLng(Info,0x30,1)+1)*0x200;
return (ReadInt(Info,pos+0x50,1)!=0xDBC2);
}

short Infowdb(RB_IDF *Info)
{
long pos;
if (Infoms(Info)) return 1;
pos=(ReadLng(Info,0x30,1)+1)*0x200;
return (ReadInt(Info,pos+0x50,1)!=0xDBC3);
}

short Infoppt(RB_IDF *Info)
{
long pos;
if (Infoms(Info)) return 1;
pos=(ReadLng(Info,0x30,1)+1)*0x200;
return (ReadInt(Info,pos+0x50,1)!=0x4851);
}


/*--------------------------------------------------------------------*\
|- extended-information sauce                                         -|
\*--------------------------------------------------------------------*/
short InfoSauce(RB_IDF *Info)
{
char buffer[6];
int pos;

//--- Teste si on a sauce ----------------------------------------------

pos=Info->sizemax-128;

ReadStr(Info,pos,buffer,5);
if (!strcmp(buffer,"SAUCE"))
    {
    ReadStr(Info,pos+7,Info->fullname,35);
    ReadStr(Info,pos+42,Info->composer,20);
    strcpy(Info->Tinfo,"Group");
    ReadStr(Info,pos+62,Info->info,20);

//  if (ReadChar(Info,pos+104)!=0) --> Commentaire avant, on s'en fout !
    return 0;
    }

return 1;
}


