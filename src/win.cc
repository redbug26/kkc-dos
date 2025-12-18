/*--------------------------------------------------------------------*\
|-             Procedure pour gestion des fenêtres                    -|
\*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>

/*
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
*/

#include "idf.h"
#include "kk.h"
#include "lnsetup.h"

static char winbuffer[256];  //--- Buffer interne ------------------

extern FENETRE* Fenetre[NBWIN];  // uniquement pour trouver la 3éme trash

void AffUpperPath(FENETRE* Fen, int y);

void InfoSelect(FENETRE* Fen);
void ExtSetup(void);
void SerialSetup(void);
void MasqueSetup(void);
void SplitMasque(char* chaine, char* buf1, char* buf2);
void JointMasque(char* chaine, char* buf1, char* buf2);

void InfoFile(struct file* F, char* buf);

void InfoLongFile(FENETRE* Fen, char* chaine);

void CtrlMenu(void);
void ShiftMenu(void);
void AltMenu(void);

static char _intbuffer[256];

/*--------------------------------------------------------------------*\
|- Fonction cachée IDF                                                -|
\*--------------------------------------------------------------------*/

void ClearSpace(char* name);  //--- efface les espaces inutiles ------

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/
// RB_IDF
// RB_info *

int NameIDF(char* name) {
    RB_IDF Info;

    strcpy(Info.path, name);

    if (Info.path[0] == 0)
        WinError("IDF 5");

    Traitefic(&Info);

    return Info.numero;
}

/*--------------------------------------------------------------------*\
|- Donne le nom long du fichier courant                               -|
\*--------------------------------------------------------------------*/
void InfoLongFile(FENETRE* Fen, char* chaine) {
    char old[256], nouv[256];

    strcpy(old, Fen->path);

    if (Fen->F[Fen->pcur]->name[0] != '.')
        Path2Abs(old, Fen->F[Fen->pcur]->name);

    if (DFen->system != 0) {
        strcpy(chaine, Fen->F[Fen->pcur]->name);
    } else {
        Short2LongFile(old, nouv);
        FileinPath(nouv, chaine);
    }

    if ((Fen->F[Fen->pcur]->longname == NULL) &
        (Fen->F[Fen->pcur]->name[0] != '.') &
        (Fen->F[Fen->pcur]->name[0] != '*')) {
        Fen->F[Fen->pcur]->longname = (char*)GetMem(strlen(chaine) + 1);
        memcpy(Fen->F[Fen->pcur]->longname, chaine, strlen(chaine) + 1);
    }
}

/*--------------------------------------------------------------------*\
|- Information with IDF                                               -|
\*--------------------------------------------------------------------*/

int InfoIDF(FENETRE* Fen) {
    static char buffer[256];
    struct file* F;
    RB_IDF Info;

    F = Fen->F[Fen->pcur];

    if (KKCfg->longname)
        InfoLongFile(Fen, buffer);
    else
        strcpy(buffer, F->name);

    if (F->name[0] == '*') {
        PrintAt(0, 0, "%-40s%-*s", "Internal function", (Cfg->TailleX) - 40, "");
        return -1;
    }

    if ((F->attrib & RB_SUBDIR) == RB_SUBDIR) {
        buffer[(Cfg->TailleX) - 40] = 0;
        PrintAt(0, 0, "%-40s%-*s", "Directory", (Cfg->TailleX) - 40, buffer);
        return -1;
    }

    if (Fen->system != 0) {
        buffer[(Cfg->TailleX) - 40] = 0;
        PrintAt(0, 0, "%-40s%-*s", "Internal File", (Cfg->TailleX) - 40, buffer);
        return -1;
    }

    strcpy(Info.path, DFen->path);
    Path2Abs(Info.path, F->name);

    strcpy(Info.format, "Invalid Name");
    strcpy(Info.fullname, "");

    if (Info.path[0] == 0)
        WinError("IDF 6");

    Traitefic(&Info);

    if (!strcmp(Info.fullname, F->name))
        strcpy(Info.fullname, buffer);

    Info.fullname[(Cfg->TailleX) - 40] = 0;
    Info.format[40] = 0;

    PrintAt(0, 0, "%-40s%-*s", Info.format, (Cfg->TailleX) - 40, Info.fullname);

    return Info.numero;
}

/*--------------------------------------------------------------------*\
|- Affiche la ligne de commande en haut de la fenetre                 -|
\*--------------------------------------------------------------------*/
void AffUpperPath(FENETRE* Fen, int y) {
    int x;
    char buffer[40];

    if (Fen->FenTyp != 0) return;

    if (strlen(Fen->path) > 36) {
        memcpy(buffer, Fen->path, 3);
        memcpy(buffer + 3, "...", 3);
        memcpy(buffer + 6, Fen->path + strlen(Fen->path) - 30, 30);
        buffer[36] = 0;
    } else {
        strcpy(buffer, Fen->path);
    }

    x = Fen->x + 1 + (36 - strlen(buffer)) / 2;

    if (y == Fen->y) {
        if (KKCfg->FenAct == Fen->nfen)
            ColLin(x, y, strlen(buffer) + 2, Cfg->col[1]);
        else
            ColLin(x, y, strlen(buffer) + 2, Cfg->col[0]);
        PrintAt(x, y, " %s ", buffer);
    } else {
        ChrLin(Fen->x + 2, y, x - Fen->x - 2, 32);
        PrintAt(x, y, " %s ", buffer);
        ChrLin(x + strlen(buffer) + 2, y, Fen->x + 36 - x - strlen(buffer), 32);
    }
}

/*--------------------------------------------------------------------*\
|-            For the FILE_ID.DIZ                                     -|
\*--------------------------------------------------------------------*/

void Makediz(RB_IDF* Info, char* Buf) {
    char ligne[256];
    char affmsg = 0;
    int m;

    strcat(Buf, "????????????????????????????????????\r\n");

    if (*Info->fullname != 0) {
        ClearSpace(Info->fullname);
        Info->fullname[30] = 0;

        if (strlen(Info->fullname) <= 26)
            sprintf(ligne, " Title: %27s \r\n", Info->fullname);  // 34
        else
            sprintf(ligne, " T.:%31s \r\n", Info->fullname);  // 34
        strcat(Buf, ligne);
    }

    if (*Info->format != 0) {
        if (strlen(Info->format) <= 26)
            sprintf(ligne, " Type : %27s \r\n", Info->format);  // 34
        else
            sprintf(ligne, " T.:%31s \r\n", Info->format);  // 34
        strcat(Buf, ligne);
    }

    if (*Info->info != 0) {
        int n;
        sprintf(ligne, " %s :", Info->Tinfo);
        n = strlen(ligne);
        sprintf(ligne + n, "%*s ", 35 - n, Info->info);  // 34
        ligne[36] = 0;
        strcat(ligne, "\r\n");
        strcat(Buf, ligne);
    }

    if (*Info->composer != 0) {
        sprintf(ligne, " Composer: %24s \r\n", Info->composer);  // 34
        strcat(Buf, ligne);
    }

    for (m = 0; m < 9; m++)
        if (*Info->message[m] != 0)
            affmsg = 1;

    if (affmsg) {
        sprintf(ligne, "????????????????????????????????????\r\n");
        strcat(Buf, ligne);
        for (m = 0; m < 9; m++) {
            if (*Info->message[m] != 0) {
                sprintf(ligne, "%-36s\r\n", Info->message[m]);
                strcat(Buf, ligne);
            }
        }
    }

    if (Info->taille != 0) {
        sprintf(ligne, " True Size: %23ld \r\n", Info->taille);
        strcat(Buf, ligne);
    }

    return;
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

#include "language.h"

void MenuBar(signed char c) {
    /*
    static char bar[][61]=
     {" Help  User  View  Edit  Copy  Move  MDir Delete Menu  Quit ",  //NOR
      "PlayerAttrib View  Edit  Host Rename ----  ----   Row  ---- ",//SHIFT
      "On-OffOn-Off Name  .Ext  Date  Size Unsort Spec  ----  ---- ", //CTRL
      " Drv1  Drv2  FDiz FileID Bkgr  Hist Search Type  Line  Disp "}; //ALT
    char Tbar[60];
    int x;
    */

    static signed char d = -1;

    if ((Cfg->TailleX != 80) & ((DFen->FenTyp != 2) | (DFen->Fen2->FenTyp != 2))) {
        if ((DFen->init != 1) & (DFen->Fen2->init != 1)) {
            time_t clock;
            static char buffer[10];

            clock = time(NULL);
            strftime(buffer, 9, "%H:%M:%S", localtime(&clock));

            PrintAt(41, DFen->yl - 1, "%-8s", buffer);

            //        PrintAt(0,0,"Memory: %20d octets",FreeMem());
        }
    }

    if (d == c) return;
    d = c;

    if (DFen->FenTyp == 0)  // Fenetre Normale
        switch (c) {
            case 0:
                InfoSelect(DFen);
                InfoSelect(DFen->Fen2);
                break;
            case 1:
                ShiftMenu();
                break;
            case 2:
                CtrlMenu();
                break;
            case 3:
                AltMenu();
                //        d=-1;  //--- Pour mettre à jour (pour la date)
                break;
            default:
                c = 4;
                break;
        }

    if ((c != 4) & (KKCfg->isbar)) {
        int TY;
        int i, j, n;

        int deb;

        switch (c) {
            case 0:
                deb = 0x3B;
                break;
            case 1:
                deb = 0x3B + 0x19;
                break;
            case 2:
                deb = 0x3B + 0x23;
                break;
            case 3:
                deb = 0x3B + 0x2D;
                break;
        }

        /*    memcpy(Tbar,bar[c],60);

            if (c==2)
                {
                x=(DFen->order)&15;
                if (x==0)
                    Tbar[41]=SELCHAR;
                    else
                    Tbar[11+6*x]=SELCHAR;

                if ((DFen->order&16)==16)
                    Tbar[47]=SELCHAR;
                }
        */

        TY = Cfg->TailleY - 1;

        n = 0;
        for (i = 0; i < 10; i++) {
            ushort k;

            PrintAt(n, TY, "F%d", (i + 1) % 10);
            for (j = 0; j < 2; j++, n++)
                AffCol(n, TY, Cfg->col[5]);

            ColLin(n, TY, 6, Cfg->col[6]);

            k = GetKeyFct((ushort)((i + deb) * 256));

            if (k == 0)
                PrintAt(n, TY, " ---- ");
            else
                PrintAt(n, TY, "%s", InfoFct[k - 1].bar);

            switch (k) {
                case 22:
                    if (((DFen->order) & 15) == 1) AffChr(n + 5, TY, SELCHAR);
                    break;
                case 23:
                    if (((DFen->order) & 15) == 2) AffChr(n + 5, TY, SELCHAR);
                    break;
                case 24:
                    if (((DFen->order) & 15) == 3) AffChr(n + 5, TY, SELCHAR);
                    break;
                case 25:
                    if (((DFen->order) & 15) == 4) AffChr(n + 5, TY, SELCHAR);
                    break;
                case 26:
                    if (((DFen->order) & 15) == 0) AffChr(n + 5, TY, SELCHAR);
                    break;
                case 33:
                    if (((DFen->order) & 16) == 16) AffChr(n + 5, TY, SELCHAR);
                    break;
            }

            n += 6;

            if (Cfg->TailleX == 90) {
                AffCol(n, TY, Cfg->col[6]);
                AffChr(n, TY, 32);
                n++;
            }
        }
    }
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-  ASCII Table                                                       -|
\*--------------------------------------------------------------------*/

void ASCIItable(void) {
    int n, x, y, m;

    m = ((Cfg->TailleX) - 80) / 2;

    KKWin Win("ASCII Table", m, 5, m + 79, 22, 0, Cfg->col[16]);

    for (n = 0; n < 256; n++) {
        x = (n / 16) * 5 + m;
        y = (n % 16) + 6;

        if (n < 16)
            x++;

        PrintAt(x, y, "%X %c", n, (n > 32) ? n : 32);
    }

    while (Win.Wait(0, 0) != 27);
}

/*

void ASCIItable(void)
{
        int n,x,y;

        SaveScreen();

        WinCadre(0,5,79,22,0);
        ColWin(1,6,78,21,10*16+5);

        for (n=0;n<256;n++)
                    {
                                x=(n/16)*5;
                                    y=(n%16)+6;

                                        PrintAt(x,y,"%2X   ",n);
                                            AffChr(x+3,y,n);
                                                }


        while (Wait(0,0) != 27);

        LoadScreen();
}
*/

/*--------------------------------------------------------------------*\
|-  Erreur (l'utilisateur est fou !)                                  -|
\*--------------------------------------------------------------------*/

void YouMad(char* s) {
    int x, l;
    static char Buffer[70];
    static char CadreLength = 70;

    struct Tmt T[5] =
        {{15, 5, 2, NULL, NULL},
         {45, 5, 3, NULL, NULL},
         {2, 2, 0, "You are MAD!", NULL},
         {1, 1, 4, &CadreLength, NULL},
         {2, 3, 0, Buffer, NULL}};

    struct TmtWin F = {-1, 10, 74, 17, "Error!"};

    l = strlen(s);

    x = ((Cfg->TailleX) - l) / 2;  // 1-> 39, 2->39
    if (x > 25) x = 25;

    l = (Cfg->TailleX) - 2 * x;

    CadreLength = (char)l;

    F.x1 = x - 2;
    F.x2 = x + l + 1;

    l = l + 3;

    T[0].x = (l / 4) - 5;
    T[1].x = (3 * l / 4) - 6;

    strcpy(Buffer, s);

    WinTraite(T, 5, &F, 0);
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-                              Setup                                 -|
\*--------------------------------------------------------------------*/

void Setup(void) {
    static int l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11, l12, l13, l14, l15, l16, l17;
    static int l18, l19, l20, l21;

    static char x1 = 32, x2 = 32, x3 = 32;
    static int y1 = 9, y2 = 3, y3 = 16;

    struct Tmt T[] = {
        {40, 17, 2, NULL, NULL},  // le OK
        {55, 17, 3, NULL, NULL},  // le CANCEL

        {5, 2, 8, "Convert History", &l7},
        {5, 3, 8, "Confirmation before exiting", &l3},
        {5, 4, 8, "Point SubDir", &l4},
        {5, 5, 8, "LogFile", &l5},
        {5, 6, 8, "ESC to close windows", &l6},
        {5, 7, 8, "Display Hidden File", &l8},
        {5, 8, 8, "Auto Reload Directory", &l9},
        {5, 9, 8, "Verify History Directory", &l10},
        {5, 10, 8, "Quick Palette", &l11},
        {5, 11, 8, "Highlight groups", &l12},
        {5, 12, 8, "Insert move down", &l13},
        {5, 13, 8, "Select directories", &l14},
        {5, 14, 8, "Estimated copying time", &l15},
        {5, 15, 8, "Display lift", &l16},
        {5, 16, 8, "Save position in viewer", &l18},
        {5, 17, 8, "Load startup directory", &l17},

        {39, 2, 7, "Size Trash   ", &l1},
        {39, 3, 8, "Key After Shell", &l2},
        {39, 4, 8, "Add selection function", &l19},

        {3, 1, 9, &x3, &y3},
        {37, 6, 9, &x1, &y1},
        {37, 1, 9, &x2, &y2},

        {40, 8, 5, " Serial Port ", &l1},    // la gestion du port serie
        {55, 8, 5, " Mask Setup  ", &l2},    // la gestion des masques
        {40, 10, 5, " File Setup  ", &l3},   // la gestion des masques
        {55, 10, 5, " Ext. Setup  ", &l4},   // la gestion des extensions
        {40, 12, 5, " Screen Set. ", &l20},  // la gestion ecran
        {55, 12, 5, " Longname St ", &l21}   // la gestion des noms longs
    };

    struct TmtWin F = {-1, 3, 74, 22, "Setup"};

    int n;

    l1 = KKCfg->mtrash;
    l2 = KKCfg->KeyAfterShell;
    l3 = KKCfg->confexit;
    l4 = KKCfg->pntrep;
    l5 = KKCfg->logfile;
    l6 = KKCfg->Esc2Close;
    l7 = KKCfg->cnvhist;
    l8 = KKCfg->hidfil;
    l9 = KKCfg->autoreload;
    l10 = KKCfg->verifhist;
    l11 = KKCfg->palafter;
    l12 = KKCfg->dispcolor;
    l13 = KKCfg->insdown;
    l14 = KKCfg->seldir;
    l15 = KKCfg->esttime;
    l16 = KKCfg->lift;
    l17 = KKCfg->currentdir;
    l18 = KKCfg->V.saveviewpos;
    l19 = KKCfg->addselect;

    do {
        n = WinTraite(T, 30, &F, 0);

        if (n == -1) return;         // ESCape
        if (T[n].type == 3) return;  // Cancel

        if (T[n].type == 5) {
            if (T[n].entier == &l1) SerialSetup();
            if (T[n].entier == &l2) MasqueSetup();
            if (T[n].entier == &l3) FileSetup();
            if (T[n].entier == &l4) ExtSetup();
            if (T[n].entier == &l20) GestionFct(66);
            if (T[n].entier == &l21) GestionFct(101);
        }
    } while (T[n].type == 5);

    KKCfg->mtrash = l1;
    KKCfg->KeyAfterShell = (char)l2;
    KKCfg->confexit = (char)l3;
    KKCfg->pntrep = (char)l4;
    KKCfg->logfile = (char)l5;
    KKCfg->Esc2Close = (char)l6;
    KKCfg->cnvhist = (char)l7;
    KKCfg->hidfil = (char)l8;
    KKCfg->autoreload = (char)l9;
    KKCfg->verifhist = (char)l10;
    KKCfg->palafter = (char)l11;
    KKCfg->dispcolor = (char)l12;
    KKCfg->insdown = (char)l13;
    KKCfg->seldir = (char)l14;
    KKCfg->esttime = (char)l15;
    KKCfg->lift = (char)l16;
    KKCfg->currentdir = (char)l17;
    KKCfg->V.saveviewpos = (char)l18;
    KKCfg->addselect = (char)l19;

    if (!KKCfg->nosave)
        SaveCfg();

    DesinitScreen();
    while (!InitScreen(Cfg->display));
    {
        Cfg->display++;
        if (Cfg->display > 16) Cfg->display = 0;
    }

    DFen = DFen->Fen2;
    CommandLine("#cd .");
    DFen = DFen->Fen2;
    CommandLine("#cd .");

    if (!KKCfg->nosave)
        LoadCfg(Cfg);

    Cfg->reinit = 0;
    UseCfg();
}

void LongnameSetup(void) {
    int n;

    if (!(Info->macro)) {
        _cbuf[10] = KKCfg->win95ln;
        _cbuf[11] = (((KKCfg->transtbl) & 1) == 1);
        _cbuf[12] = (((KKCfg->transtbl) & 2) == 2);
        _cbuf[13] = KKCfg->daccessln;
        _cbuf[14] = KKCfg->longname;
        _cbuf[15] = KKCfg->fullnamesup;
    }

    n = MWinTraite(lnsetup_kkt);

    if (n == -1)
        return;  //--- ESCape ---

    KKCfg->win95ln = _cbuf[10];
    KKCfg->transtbl = (char)((_cbuf[11]) + (_cbuf[12]) * 2);
    KKCfg->daccessln = _cbuf[13];
    KKCfg->fullnamesup = _cbuf[15];

    if ((_cbuf[10] == 0) & (_cbuf[11] == 0) & (_cbuf[13] == 0))
        KKCfg->longname = 0;
    else
        KKCfg->longname = _cbuf[14];
}

void ExtSetup(void) {
    static int l1, l2, l3, l4, l5, l6;
    static char Txt[64], Bmp[64], Snd[64], Arc[64], Exe[64], Usr[64];
    static int DirLength = 63;

    struct Tmt T[16] = {
        {1, 1, 8, "Text:         ", &l1},
        {9, 2, 1, Txt, &DirLength},
        {1, 3, 8, "Bitmap:       ", &l2},
        {9, 4, 1, Bmp, &DirLength},
        {1, 5, 8, "Sound:        ", &l3},
        {9, 6, 1, Snd, &DirLength},
        {1, 7, 8, "Archive:      ", &l4},
        {9, 8, 1, Arc, &DirLength},
        {1, 9, 8, "Executable:   ", &l5},
        {9, 10, 1, Exe, &DirLength},
        {1, 11, 8, "User Defined: ", &l6},
        {9, 12, 1, Usr, &DirLength},

        {13, 14, 2, NULL, NULL},  // le OK
        {50, 14, 3, NULL, NULL}   // le CANCEL
    };

    struct TmtWin F = {-1, 5, 74, 21, "Extension Setup"};

    int n;

    l1 = KKCfg->Enable_Txt;
    l2 = KKCfg->Enable_Bmp;
    l3 = KKCfg->Enable_Snd;
    l4 = KKCfg->Enable_Arc;
    l5 = KKCfg->Enable_Exe;
    l6 = KKCfg->Enable_Usr;

    strcpy(Txt, KKCfg->ExtTxt);
    strcpy(Bmp, KKCfg->ExtBmp);
    strcpy(Snd, KKCfg->ExtSnd);
    strcpy(Arc, KKCfg->ExtArc);
    strcpy(Exe, KKCfg->ExtExe);
    strcpy(Usr, KKCfg->ExtUsr);

    n = WinTraite(T, 14, &F, 0);

    if (n == -1) return;         // ESCape
    if (T[n].type == 3) return;  // Cancel

    strcpy(KKCfg->ExtTxt, Txt);
    strcpy(KKCfg->ExtBmp, Bmp);
    strcpy(KKCfg->ExtSnd, Snd);
    strcpy(KKCfg->ExtArc, Arc);
    strcpy(KKCfg->ExtExe, Exe);
    strcpy(KKCfg->ExtUsr, Usr);

    KKCfg->Enable_Txt = (char)l1;
    KKCfg->Enable_Bmp = (char)l2;
    KKCfg->Enable_Snd = (char)l3;
    KKCfg->Enable_Arc = (char)l4;
    KKCfg->Enable_Exe = (char)l5;
    KKCfg->Enable_Usr = (char)l6;
}

void ScreenSetup(void) {
    static int sw, sy;

    static int l1, l2, l3, l4, l5, l6, l7, l8;

    static char x1 = 32, x2 = 32, x3 = 32, x4 = 32;
    static int y1 = 3, y2 = 5, y3 = 4, y4 = 3;

    struct Tmt T[] = {
        {5, 2, 10, "25 lines", &sw},
        {5, 3, 10, "30 lines", &sw},
        {5, 4, 10, "50 lines", &sw},
        {5, 7, 8, "Display lower path", &l4},
        {5, 8, 8, "Display upper path", &l8},
        {5, 9, 8, "Display IDF bar", &l5},
        {5, 10, 8, "Display function bar", &l6},
        {5, 11, 7, "Size of pannel", &l3},
        {39, 8, 8, "Use Font", &l1},
        {39, 9, 8, "Use User Font", &l7},
        {39, 10, 7, "Screen Saver ", &l2},

        {39, 2, 10, "Norton like ", &sy},
        {39, 3, 10, "7-bit mode  ", &sy},
        {39, 4, 10, "Thin mode   ", &sy},
        {39, 5, 10, "Ketchup Mode", &sy},

        {3, 1, 9, &x1, &y1},
        {3, 6, 9, &x2, &y2},
        {37, 1, 9, &x3, &y3},
        {37, 7, 9, &x4, &y4},

        {11, 1, 0, " Number of lines ", NULL},
        {46, 1, 0, " Window Design ", NULL},

        {13, 13, 2, NULL, NULL},  // le OK
        {50, 13, 3, NULL, NULL}   // le CANCEL
    };

    struct TmtWin F = {-1, 5, 74, 20, "Window & Color"};

    int n;

    switch (Cfg->TailleY) {
        case 25:
            sw = 0;
            break;
        case 50:
            sw = 2;
            break;
        case 30:
        default:
            sw = 1;
            break;
    }
    sy = Cfg->windesign + 10;
    l1 = Cfg->font;
    l2 = Cfg->SaveSpeed;
    l3 = KKCfg->sizewin;
    l4 = KKCfg->pathdown;
    l5 = KKCfg->isidf;
    l6 = KKCfg->isbar;
    l7 = KKCfg->userfont;
    l8 = KKCfg->dispath;

    n = WinTraite(T, 23, &F, 0);

    if (n == -1) return;         // ESCape
    if (T[n].type == 3) return;  // Cancel

    switch (sw) {
        case 0:
            Cfg->TailleY = 25;
            break;
        case 1:
            Cfg->TailleY = 30;
            break;
        case 2:
            Cfg->TailleY = 50;
            break;
    }

    Cfg->windesign = (char)(sy - 10);
    Cfg->font = (char)l1;
    Cfg->SaveSpeed = l2;
    if ((l3 >= 6) | (l3 == 0))
        KKCfg->sizewin = (char)l3;

    KKCfg->pathdown = (char)l4;
    KKCfg->isidf = (char)l5;
    KKCfg->isbar = (char)l6;
    KKCfg->userfont = (char)l7;
    KKCfg->dispath = (char)l8;

    GestionFct(67);  // Rafraichit l'ecran
}

void SerialSetup(void) {
    static char Dir[3];
    static int DirLength = 2;

    static int l1, l2, l3, l5;

    struct Tmt T[8] = {
        {5, 3, 7, "Port COM ", &l1},
        {5, 4, 7, "Speed    ", &l2},
        {5, 5, 7, "Data_bit ", &l3},
        {5, 6, 0, "Parity   ", NULL},
        {16, 6, 1, Dir, &DirLength},
        {5, 7, 7, "Stop_bit ", &l5},

        {3, 10, 2, NULL, NULL},  // le OK
        {18, 10, 3, NULL, NULL}  // le CANCEL
    };

    struct TmtWin F = {-1, 5, 34, 18, "Serial Setup"};

    int n;

    l1 = Cfg->comport;
    l2 = Cfg->comspeed;
    l3 = Cfg->combit;
    sprintf(Dir, "%c", Cfg->comparity);
    l5 = Cfg->comstop;

    n = WinTraite(T, 8, &F, 0);

    if (n == -1) return;         // ESCape
    if (T[n].type == 3) return;  // Cancel

    Cfg->comport = (char)l1;
    Cfg->comspeed = l2;
    Cfg->combit = (char)l3;
    sscanf(Dir, "%c", &(Cfg->comparity));
    Cfg->comstop = (char)l5;
}

void MasqueSetup(void) {
    static char buffer[8][80];

    static int DirLength = 78;

    static int l1, l2, l3, l4;

    struct Tmt T[18] = {
        {2, 14, 2, NULL, NULL},   // le OK
        {16, 14, 3, NULL, NULL},  // le CANCEL

        {1, 1, 0, "User config 1", NULL},
        {40, 1, 8, "Ignore Case", &l1},
        {1, 2, 1, buffer[0], &DirLength},
        {1, 3, 1, buffer[1], &DirLength},

        {1, 4, 0, "User config 2", NULL},
        {40, 4, 8, "Ignore Case", &l2},
        {1, 5, 1, buffer[2], &DirLength},
        {1, 6, 1, buffer[3], &DirLength},

        {1, 7, 0, "User config 3", NULL},
        {40, 7, 8, "Ignore Case", &l3},
        {1, 8, 1, buffer[4], &DirLength},
        {1, 9, 1, buffer[5], &DirLength},

        {1, 10, 0, "User config 4", NULL},
        {40, 10, 8, "Ignore Case", &l4},
        {1, 11, 1, buffer[6], &DirLength},
        {1, 12, 1, buffer[7], &DirLength}};

    struct TmtWin F = {-1, 5, 80, 22, "Mask Setup"};

    int n;

    l1 = KKCfg->V.Mask[11]->Ignore_Case;
    l2 = KKCfg->V.Mask[12]->Ignore_Case;
    l3 = KKCfg->V.Mask[13]->Ignore_Case;
    l4 = KKCfg->V.Mask[14]->Ignore_Case;

    SplitMasque(KKCfg->V.Mask[11]->chaine, buffer[0], buffer[1]);
    SplitMasque(KKCfg->V.Mask[12]->chaine, buffer[2], buffer[3]);
    SplitMasque(KKCfg->V.Mask[13]->chaine, buffer[4], buffer[5]);
    SplitMasque(KKCfg->V.Mask[14]->chaine, buffer[6], buffer[7]);

    n = WinTraite(T, 18, &F, 0);

    if (n == -1) return;         // ESCape
    if (T[n].type == 3) return;  // Cancel

    KKCfg->V.Mask[11]->Ignore_Case = (char)l1;
    KKCfg->V.Mask[12]->Ignore_Case = (char)l2;
    KKCfg->V.Mask[13]->Ignore_Case = (char)l3;
    KKCfg->V.Mask[14]->Ignore_Case = (char)l4;

    JointMasque(KKCfg->V.Mask[11]->chaine, buffer[0], buffer[1]);
    if (strlen(KKCfg->V.Mask[11]->chaine) > 2)
        strcpy(KKCfg->V.Mask[11]->title, "User config 1");
    else
        strcpy(KKCfg->V.Mask[11]->title, "");
    JointMasque(KKCfg->V.Mask[12]->chaine, buffer[2], buffer[3]);
    if (strlen(KKCfg->V.Mask[12]->chaine) > 2)
        strcpy(KKCfg->V.Mask[12]->title, "User config 2");
    else
        strcpy(KKCfg->V.Mask[12]->title, "");
    JointMasque(KKCfg->V.Mask[13]->chaine, buffer[4], buffer[5]);
    if (strlen(KKCfg->V.Mask[13]->chaine) > 2)
        strcpy(KKCfg->V.Mask[13]->title, "User config 3");
    else
        strcpy(KKCfg->V.Mask[13]->title, "");
    JointMasque(KKCfg->V.Mask[14]->chaine, buffer[6], buffer[7]);
    if (strlen(KKCfg->V.Mask[14]->chaine) > 2)
        strcpy(KKCfg->V.Mask[14]->title, "User config 4");
    else
        strcpy(KKCfg->V.Mask[14]->title, "");
}

void SplitMasque(char* chaine, char* buf1, char* buf2) {
    int n, m;

    m = strlen(chaine);
    if (m < 3) {
        strcpy(chaine, " @");
        strcpy(buf1, "");
        strcpy(buf2, "");
        return;
    }

    if (m <= 80) {
        memcpy(buf1, chaine, m - 2);
        buf1[m - 2] = 0;
        strcpy(buf2, "");
        return;
    }
    n = 78;
    while ((n != 0) & (chaine[n] != 32))
        n--;

    memcpy(buf1, chaine, n);
    buf1[n] = 0;
    memcpy(buf2, chaine + n + 1, m - n - 3);
    buf2[m - n - 3] = 0;
}

void JointMasque(char* chaine, char* buf1, char* buf2) {
    int n, m;

    strcpy(chaine, buf1);
    strcat(chaine, " ");
    strcat(chaine, buf2);
    strcat(chaine, " @");

    n = 0;

    while (chaine[n] != 0) {
        if ((chaine[n] == 32) & ((chaine[n + 1] == 32) | (n == 0) |
                                 (chaine[n + 1] == 0))) {
            for (m = n + 1; m < strlen(chaine) + 1; m++)
                chaine[m - 1] = chaine[m];
        } else
            n++;
    }
}

/*--------------------------------------------------------------------*\
|-    Utilise les donnees dans la structure configuration avec les    -|
|-    verifications necessaires                                       -|
\*--------------------------------------------------------------------*/

void UseCfg(void) {
    GestionFct(67);  // Rafraichit l'ecran

#ifndef NOINT
    if (Cfg->speedkey == 1)
        Cfg->speedkey = KeyTurbo(1);
#endif
}

/*--------------------------------------------------------------------*\
|-  Information on FILE_ID.DIZ                                        -|
\*--------------------------------------------------------------------*/
void ViewFileID(FENETRE* Fen) {
    char path[256], *name;

    strcpy(path, Fen->Fen2->path);
    if (Fen->Fen2->F[Fen->Fen2->pcur]->name[0] != '.')
        Path2Abs(path, Fen->Fen2->F[Fen->Fen2->pcur]->name);  // Ajout gedeon
    Path2Abs(path, "FILE_ID.DIZ");

    name = AccessAbsFile(path);  // Ajout GEDEON -----------------------------

    if (name != NULL)
        View(&(KKCfg->V), name, 4);

    DFen->init = 1;
}

/*--------------------------------------------------------------------*\
|-                     Test la vitesse d'affichage                    -|
\*--------------------------------------------------------------------*/

void SpeedTest(void) {
    static char vit1[64], vit2[64];

    int x, y;
    clock_k Cl;
    long Cl1;
    char c;
    int n, m;
    double fvit1;

    struct Tmt T[6] = {
        {6, 17, 2, NULL, NULL},   // le OK
        {21, 17, 3, NULL, NULL},  // le CANCEL

        {5, 3, 0, "No-Random Char", NULL},
        {5, 4, 0, "Random Char", NULL},
        {25, 3, 0, vit1, NULL},
        {25, 4, 0, vit2, NULL}};

    struct TmtWin F = {-1, 3, 74, 22, "Speed Test"};

    SaveScreen();

    c = (char)((rand() * 80) / RAND_MAX + 32);
    m = 0;
    n = 0;

    Cl = GetClock();

    for (n = 0; n < 500000; n++) {
        x = (rand() * (Cfg->TailleX)) / RAND_MAX;
        y = (rand() * Cfg->TailleY) / RAND_MAX;

        if (m > 10000) m = 0, c = (char)((rand() * 80) / RAND_MAX + 32);
        m++;

        AffChr(x, y, c);
    }
    Cl1 = GetClock() - Cl;

    LoadScreen();

    fvit1 = (500000. / Cl1) * CLOCKK_PER_SEC;

    sprintf(vit1, "%f characters/seconds", fvit1);
    sprintf(vit2, "? characters/seconds");

    n = WinTraite(T, 6, &F, 0);
}

/*--------------------------------------------------------------------*\
|- Information about files                                            -|
\*--------------------------------------------------------------------*/
int SearchInfo(FENETRE* Fen) {
    int i;
    struct file* F;
    RB_IDF Info;
    FENETRE* Fen2;
    static char Buffer[256];
    int nouv = 0;  // Renvoit le nombre de nouvelles informations

    Fen2 = Fen->Fen2;

    for (i = 0; i < Fen2->nbrfic; i++) {
        F = Fen2->F[i];

        if (F->info == NULL) {
            nouv++;

            F->info = (char*)GetMem(82);

            if ((F->attrib & RB_SUBDIR) == RB_SUBDIR)
                sprintf(F->info, "%cDirectory", 0);
            else if (Fen2->system != 0)
                sprintf(F->info, "%cInternal File", 0);
            else {
                IOerr = 1;  //--- Ignore les erreurs suivantes -------------
                strcpy(Info.path, Fen2->path);
                Path2Abs(Info.path, F->name);

                if (Info.path[0] == 0)
                    WinError("IDF 7");

                Traitefic(&Info);
                sprintf(Buffer, "%s - %s - %s", Info.format, Info.fullname,
                        Info.message[0]);
                ClearSpace(Buffer);
                Buffer[80] = 0;
                sprintf(F->info + 1, "%-80s", Buffer);

                F->info[0] = (char)(Info.Btype);
                IOerr = 0;  //--- Intercepte les erreurs -------------------
            }
        }
        if (KbHit()) break;
    }

    return nouv;
}

/*--------------------------------------------------------------------*\
|-  Fenetre TREE                                                      -|
\*--------------------------------------------------------------------*/
/*
void FenTree(FENETRE *Fen)
{
int x,y,n,i,j,t,pos;
FENETRE *OldFen;
char path[256];

static char oldpath[256];
static char prem[256];
static char tab;
static char op[50];

x=Fen->x+1;
y=Fen->y+1;

if ( (!stricmp(oldpath,Fen->Fen2->path)) & (Fen->init==0) )
    {
    PrintAt(Fen->x+38,Fen->y+Fen->yl-1,"?");
    }
    else
    {
    WinCadre(Fen->x,Fen->y,Fen->x+Fen->xl,Fen->y+Fen->yl,1);
    Window(Fen->x+1,Fen->y+1,Fen->x+Fen->xl-1,Fen->y+Fen->yl-1,164);

    strcpy(oldpath,Fen->Fen2->path);

    PrintAt(Fen->x+38,Fen->y+Fen->yl-1,"");
    for(i=0;i<50;i++) op[i]=0;

    strcpy(prem,Fen->Fen2->path);
    prem[3]=0;
    tab=1;
    pos=0;
    op[12]=1;

    Fen->init=0;
    }

strcpy(path,prem);

OldFen=DFen;
DFen=Fenetre[2];

t=tab;
j=0;
CommandLine("#cd %s",path);

for(i=0;i<Fen->yl-Fen->y-1;i++)
    {
    while ( (j<DFen->nbrfic) & ((DFen->F[j]->attrib&16)!=16) ) j++;

    if (j>=DFen->nbrfic) break;

    if (op[i]==1)
        {
        Path2Abs(path,DFen->F[j]->name);
        CommandLine("#cd %s",path);
        j=0;
        t++;
        }
        else
        {
        for (n=0;n<t;n++)
            PrintAt(Fen->x+1+n*2,Fen->y+2+i,"??");

        PrintAt(Fen->x+1+t*2,Fen->y+2+i,"%s",DFen->F[j]->name);
        j++;
        }
    }

for(;i<Fen->yl-Fen->y-1;i++)
    PrintAt(Fen->x+1,Fen->y+2+i,"%-38s","|-|");

DFen=OldFen;
}
*/

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
|-  Change la taille de l'ecran                                       -|
\*--------------------------------------------------------------------*/

void ChangeTaille(int i) {
    if (i == 0)
        switch (Cfg->TailleY) {
            case 25:
                Cfg->TailleY = 30;
                break;
            case 30:
                Cfg->TailleY = 50;
                break;
            default:
                Cfg->TailleY = 25;
                break;
        }
    else
        Cfg->TailleY = i;

    SetMode();
    LoadPal();

#ifndef NOFONT
    if (KKCfg->userfont)
        InitFontFile(KKFics->trash);
    else
        InitFont();
#endif

    CalcSizeWin(Fenetre[0]);
    CalcSizeWin(Fenetre[1]);

    AfficheTout();
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

static char lengthd[] = {0, 12, 10, 8, 5, 18};

/*--------------------------------------------------------------------*\
|-  Efface la fenêtre gestion de fichier                              -|
\*--------------------------------------------------------------------*/

void ClearNor(FENETRE* Fen) {
    char col;
    int sb;

    int i, j, m;
    int x, y;
    int xl, yl;

    char a;

    char disp[16];

    if (KKCfg->fullnamesup)
        sprintf(disp, "\x5\x2\x3");
    else
        sprintf(disp, "\x1\x2\x3\x4");

    Fen->oldscur = 0;
    Fen->oldpcur = -1;

    sb = 1 + (KKCfg->pathdown);

    switch (Cfg->windesign)  //---Couleur uniquement pour fentype=1,2 ou 3 ---
    {
        case 1:
        case 2:
        case 3:
            ColWin(Fen->x + 1, Fen->y + 1, Fen->xl - 1, Fen->yl - 1, Cfg->col[0]);

            for (x = Fen->x; x < Fen->xl; x++)
                AffCol(x, Fen->y, Cfg->col[37]);
            for (y = Fen->y; y <= Fen->yl; y++)
                AffCol(Fen->x, y, Cfg->col[37]);

            for (x = Fen->x + 1; x <= Fen->xl; x++)
                AffCol(x, Fen->yl, Cfg->col[38]);
            for (y = Fen->y; y < Fen->yl; y++)
                AffCol(Fen->xl, y, Cfg->col[38]);
            break;
    }

    x = Fen->x;
    y = Fen->y;

    xl = Fen->xl;
    yl = Fen->yl;

    switch (Cfg->windesign) {
        case 1:
            a = 179;

            PrintAt(x, y, "????????????????????????????????????????");
            PrintAt(x, y + 1, "?                                      ?");
            PrintAt(x, yl - sb - 1, "????????????????????????????????????????");
            PrintAt(x, yl, "????????????????????????????????????????");

            col = Cfg->col[4];

            j = x + 1;

            m = 0;
            while (disp[m] != 0) {
                PrintAt(j, Fen->y2, "%s", _PannelHeader[disp[m] - 1]);
                ColLin(j, Fen->y2, lengthd[disp[m]], col);
                j += lengthd[disp[m]];
                m++;

                if (disp[m] != 0) {
                    AffChr(j, Fen->y2 - 1, 209);
                    AffCol(j, Fen->y2, Cfg->col[0]);
                    AffChr(j, Fen->y2, a);
                    AffChr(j, yl - sb - 1, 193);
                    j++;
                }
            }

            for (i = y + 2; i < yl - sb - 1; i++)
                PrintAt(x, i, "?                                      ?");
            for (i = yl - sb; i < yl; i++)
                PrintAt(x, i, "?                                      ?");

            break;
        case 2:
            for (i = y + 1; i < yl - sb - 1; i++)
                PrintAt(x, i, "?                                      ?");
            PrintAt(x, yl - sb - 1, "????????????????????????????????????????");
            for (i = yl - sb; i < yl; i++)
                PrintAt(x, i, "?                                      ?");
            PrintAt(x, yl, "????????????????????????????????????????");
            col = (char)((Cfg->col[4] & 15) + (Cfg->col[37] & 15) * 16);

            a = 179;

            PrintAt(x, y, "????????????????????????????????????????");

            j = x + 1;

            m = 0;
            while (disp[m] != 0) {
                PrintAt(j, Fen->y2, "%s", _PannelHeader[disp[m] - 1]);
                ColLin(j, Fen->y2, lengthd[disp[m]], col);
                j += lengthd[disp[m]];
                m++;

                if (disp[m] != 0) {
                    AffChr(j, Fen->y2, 32);
                    AffCol(j, Fen->y2, col);
                    j++;
                }
            }

            break;
        case 3:
            PrintAt(x, y, "????????????????????????????????????????");
            PrintAt(x, yl - sb - 1, "????????????????????????????????????????");
            PrintAt(x, yl, "????????????????????????????????????????");

            col = (char)((Cfg->col[37] & 240) + (Cfg->col[4]) & 15);

            j = x + 1;

            m = 0;
            while (disp[m] != 0) {
                char temp[32];
                strcpy(temp, _PannelHeader[disp[m] - 1]);
                for (i = 0; i < strlen(temp); i++) {
                    if (temp[i] == 32)
                        temp[i] = 196;
                    else
                        AffCol(j + i, Fen->y2, col);
                }

                PrintAt(j, Fen->y2, "%s", temp);
                j += lengthd[disp[m]];
                m++;

                if (disp[m] != 0) {
                    AffChr(j, Fen->y2, 194);
                    AffChr(j, yl - sb - 1, 193);
                    j++;
                }
            }

            for (i = y + 1; i < yl - sb - 1; i++)
                PrintAt(x, i, "?                                      ?");
            for (i = yl - sb; i < yl; i++)
                PrintAt(x, i, "?                                      ?");

            break;
        case 4:
            if (Cfg->UseFont)
                a = 168;  // Barre Verticale | with 8x?
            else
                a = 179;

            Window(x + 1, yl - sb, xl - 1, yl - 1, Cfg->col[0]);

            col = Cfg->col[4];

            j = x + 1;

            m = 0;
            while (disp[m] != 0) {
                PrintAt(j, Fen->y2, "%s", _PannelHeader[disp[m] - 1]);
                ColLin(j, Fen->y2, lengthd[disp[m]], col);
                j += lengthd[disp[m]];
                m++;

                if (disp[m] != 0) {
                    AffCol(j, Fen->y2, Cfg->col[0]);
                    AffChr(j, Fen->y2, a);
                    j++;
                }
            }

            ColLin(x + 1, yl - sb - 1, 38, Cfg->col[0]);

            WinLine(x + 1, yl - sb - 1, 38, 1);
            Cadre(x, y, xl, yl, 1, Cfg->col[37], Cfg->col[38]);
            break;
    }
    // Wait(0,0);
}

/*--------------------------------------------------------------------*\
|-  Affichage:                                                        -|
|----------------------------------------------------------------------|
|-  1 -> short name 8+3                                               -|
|-  2 -> taille                                                       -|
|-  3 -> date                                                         -|
|-  4 -> heure                                                        -|
|-  5 -> longname                                                     -|
\*--------------------------------------------------------------------*/
void FenNor(FENETRE* Fen) {
    int j, n, m;  // Compteur
    int date, time;
    char ch1[13], ch2[11], ch3[9], ch4[6];
    char ch5[19];
    char temp[16], temp2[20];
    char nom[13], ext[4];
    char buf[256];

    char disp[16];

    char dispall;

    char col;

    short i;

    char a;  // Separator Character

    int x1, y1;

    // printf("Traite Commandline after dos %s FIN)\n\n","fennor"); exit(1);

    if (KKCfg->fullnamesup)
        sprintf(disp, "\x5\x2\x3");
    else
        sprintf(disp, "\x1\x2\x3\x4");

    if (Fen->init == 1)
        ClearNor(Fen);

    switch (Cfg->windesign) {
        case 1:
        case 3:
            a = 179;
            break;
        case 2:
        case 4:
            if (Cfg->UseFont)
                a = 168;
            else
                a = 179;
            break;
    }

    if (Fen->scur > Fen->pcur)
        Fen->scur = Fen->pcur;

    while (Fen->pcur < 0) {
        Fen->scur++;
        Fen->pcur++;
    }

    while (Fen->pcur >= Fen->nbrfic) {
        Fen->pcur--;
        Fen->scur--;
    }

    if (Fen->nbrfic - Fen->pcur + Fen->scur < Fen->yl2) {
        Fen->scur = Fen->yl2 + Fen->pcur - Fen->nbrfic;
        if (Fen->scur >= Fen->pcur)
            Fen->scur = Fen->pcur;
    }

    if (Fen->scur >= Fen->yl2)
        Fen->scur = Fen->yl2 - 1;

    if (Fen->scur < 0)
        Fen->scur = 0;

    x1 = Fen->x + 1;
    y1 = Fen->y2 + 1;

    n = (Fen->pcur) - (Fen->scur);  // premier

    InfoSelect(Fen);

    if (KKCfg->pathdown)
        AffUpperPath(Fen, Fen->yl - 2);

    dispall = ((Fen->oldpcur - Fen->oldscur) != (Fen->pcur - Fen->scur));

    for (i = 0; (i < Fen->yl2) & (n < Fen->nbrfic); i++, n++, y1++) {
        if ((n == Fen->oldpcur) | (n == Fen->pcur) | (dispall)) {
            m = 0;

            while (disp[m] != 0) {
                switch (disp[m]) {
                    case 1:
                    case 5:
                        strcpy(nom, Fen->F[n]->name);
                        if ((Fen->F[n]->attrib & RB_SUBDIR) != RB_SUBDIR)
                            strlwr(nom);

                        ext[0] = 0;
                        j = 0;

                        if (nom[0] != '.')
                            while (nom[j] != 0) {
                                if (nom[j] == '.') {
                                    memcpy(ext, nom + j + 1, 4);
                                    ext[3] = 0;
                                    nom[j] = 0;
                                    break;
                                }
                                j++;
                            }
                        nom[8] = 0;  // Dans le cas où on aurait un nom long

                        sprintf(ch1, "%-8s %-3s", nom, ext);

                        if (nom[0] == '*') {
                            int d;
                            sscanf(nom + 1, "%d", &d);

                            switch (d) {
                                case 86:  //--- Eject CD ------------------------------
                                    strcpy(ch1, _LabelCdrom);
                                    break;
                                case 27:  //--- Reload Disk ---------------------------
                                    strcpy(ch1, _LabelDrive);
                                    break;
                                case 92:
                                    strcpy(ch1, _LabelUnsel);
                                    break;
                                default:
                                    strcpy(ch1, _LabelUnkno);
                                    break;
                            }
                        }

                        if ((Fen->F[n]->attrib & RB_HIDDEN) == RB_HIDDEN)
                            ch1[8] = 176;  // 176,177,178

                        if ((Fen->system == 5) &
                            (Fen->F[n]->desc == 1) &
                            ((Fen->F[n]->attrib & RB_SUBDIR) != RB_SUBDIR))
                            ch1[8] = 0x18;

                        if (Fen->F[n]->longname != NULL) {
                            strcpy(buf, Fen->F[n]->longname);
                            buf[18] = 0;  //--- 12
                            sprintf(ch5, "%-18s", buf);
                        } else
                            sprintf(ch5, "%-18s", ch1);

                        break;
                    case 2:
                        sprintf(ch2, "%10s", Long2Str(Fen->F[n]->size, temp2));
                        if (((Fen->F[n]->attrib & RB_SUBDIR) == RB_SUBDIR) &
                            (Fen->F[n]->size == 0))
                            strcpy(ch2, _LabelSubDir);

                        if ((Fen->F[n]->attrib & RB_VOLID) == RB_VOLID)
                            strcpy(ch2, _LabelVolume);

                        if (Fen->F[n]->name[0] == '*') {
                            int d;
                            sscanf(nom + 1, "%d", &d);

                            switch (d) {
                                case 86:  //--- Eject CD --------------------
                                case 27:  //--- Reload Disk -----------------
                                    strcpy(ch2, _LabelReload);
                                    break;
                                case 92:
                                default:
                                    strcpy(ch2, _LabelFunctn);
                                    break;
                            }
                        }

                        if (Fen->F[n]->name[0] == '.') {
                            if (Fen->F[n]->name[1] != '.')
                                memcpy(ch2, _LabelReload, 10);
                            else
                                memcpy(ch2, _LabelUpdire, 10);
                        }
                        break;

                    case 3:
                        date = Fen->F[n]->date;
                        Int2Char((date >> 9) + 80, temp, 2);
                        sprintf(ch3, "%02d/%02d/%2s", (date & 31), (date >> 5) & 15, temp);
                        break;

                    case 4:
                        time = Fen->F[n]->time;
                        sprintf(ch4, "%02d:%02d", (time >> 11) & 31, (time >> 5) & 63);

                        if (Fen->F[n]->select != 0)
                            memcpy(ch4, "<SEL>", 5);
                        break;
                    case 6:
                        date = Fen->F[n]->date;
                        Int2Char((date >> 9) + 80, temp, 2);
                        sprintf(ch3, "%02d/%02d/%2s", (date & 31), (date >> 5) & 15, temp);

                        time = Fen->F[n]->time;
                        sprintf(ch4, "%02d:%02d", (time >> 11) & 31, (time >> 5) & 63);

                        if (Fen->F[n]->select != 0)
                            memcpy(ch4, _LabelSel, 5);
                        break;
                }
                m++;
            }

            /*--------------------------------------------------------------------*\
            |------------------- Line Activity ------------------------------------|
            \*--------------------------------------------------------------------*/
            if ((KKCfg->FenAct == Fen->nfen) & (n == Fen->pcur)) {
                if (Fen->F[n]->select == 0)
                    col = Cfg->col[1];
                else
                    col = Cfg->col[3];
            } else {
                if (Fen->F[n]->select == 0) {
                    col = Cfg->col[0];

                    if (KKCfg->dispcolor == 1) {
                        if ((FoundExt(ext, KKCfg->ExtExe)) &
                            (KKCfg->Enable_Exe))
                            col = Cfg->col[15];  // Executable
                        else if ((FoundExt(ext, KKCfg->ExtArc)) &
                                 (KKCfg->Enable_Arc))
                            col = Cfg->col[22];  // Archive
                        else if ((FoundExt(ext, KKCfg->ExtSnd)) &
                                 (KKCfg->Enable_Snd))
                            col = Cfg->col[23];  // Son
                        else if ((FoundExt(ext, KKCfg->ExtBmp)) &
                                 (KKCfg->Enable_Bmp))
                            col = Cfg->col[32];  // Image
                        else if ((FoundExt(ext, KKCfg->ExtTxt)) &
                                 (KKCfg->Enable_Txt))
                            col = Cfg->col[33];  // Texte
                        else if ((FoundExt(ext, KKCfg->ExtUsr)) &
                                 (KKCfg->Enable_Usr))
                            col = Cfg->col[34];  // User defined
                    }
                } else
                    col = Cfg->col[2];
            }

            // ---------------------------------------------------------------------

            j = x1;

            m = 0;
            while (disp[m] != 0) {
                switch (disp[m]) {
                    case 1:
                        PrintAt(j, y1, "%s", ch1);
                        break;
                    case 2:
                        PrintAt(j, y1, "%s", ch2);
                        break;
                    case 3:
                        PrintAt(j, y1, "%s", ch3);
                        break;
                    case 4:
                        PrintAt(j, y1, "%s", ch4);
                        break;
                    case 5:
                        PrintAt(j, y1, "%s", ch5);
                        break;
                }
                ColLin(j, y1, lengthd[disp[m]], col);
                j += lengthd[disp[m]];

                m++;

                if (disp[m] != 0) {
                    AffCol(j, y1, Cfg->col[0]);
                    AffChr(j, y1, a);
                    j++;
                }
            }
        }
    }

    /*--------------------------------------------------------------------*\
    |------------------- Remplis le reste de la fenêtre -------------------|
    \*--------------------------------------------------------------------*/

    if (dispall)
        for (; (i < Fen->yl2); i++, y1++) {
            j = x1;

            m = 0;
            while (disp[m] != 0) {
                PrintAt(j, y1, "%*s", lengthd[disp[m]], "");
                ColLin(j, y1, lengthd[disp[m]], Cfg->col[0]);
                j += lengthd[disp[m]];
                m++;

                if (disp[m] != 0) {
                    AffCol(j, y1, Cfg->col[0]);
                    AffChr(j, y1, a);
                    j++;
                }
            }
        }

    if (KKCfg->lift) {
        char r0, r1;
        int n, y;

        switch ((Cfg->windesign) + (Cfg->UseFont) * 10) {
            case 14:
                r0 = 145;
                r1 = 0xAE;
                break;
            default:
                r0 = 176;
                r1 = 177;
                break;
        }

        y = Fen->yl - Fen->y2;

        if (y != 0) {
            y = y * Fen->pcur;
            y = y / Fen->nbrfic;
            y += Fen->y2;

            for (n = Fen->y2; n < y; n++)
                AffChr(Fen->xl, n, r0);
            AffChr(Fen->xl, y, r1);
            for (n = y + 1; n < Fen->yl; n++)
                AffChr(Fen->xl, n, r0);
        }
    }

    if ((dispall) & (KKCfg->dispath) & (Fen->y2 != Fen->y)) {
        AffUpperPath(Fen, Fen->y);
    }

    Fen->oldscur = Fen->scur;
    Fen->oldpcur = Fen->pcur;

    Fen->init = 0;
}

void InfoFile(struct file* F, char* buf) {
    char m = 0;
    char ligne[256];

    if (F->truesize != 0)
        m = 1;

    if (m)
        strcat(buf, "????????????????????????????????????\r\n");

    if (F->truesize != 0) {
        sprintf(ligne, " Size on support: %17ld \r\n", F->truesize);
        strcat(buf, ligne);
    }
}

void FenDIZ(FENETRE* Fen) {
    char idf;

    char* Buf;
    RB_IDF Info;
    FENETRE* Fen2;
    struct file* F;
    int x, y, i;
    int hsize;

    idf = 1;

    Fen2 = Fen->Fen2;
    F = Fen2->F[Fen2->pcur];

    if (((F->attrib & RB_SUBDIR) == RB_SUBDIR) & (F->name[0] == '.'))
        idf = 0;

    if (F->name[0] == '*')
        idf = 0;

    if (Fen2->system != 0)
        idf = 0;

    Cadre(Fen->x, Fen->y, Fen->xl, Fen->yl, 1, Cfg->col[37], Cfg->col[38]);

    Buf = (char*)GetMem(4000);
    strcpy(Buf, "                                    \r\n");
    strcat(Buf, "      Ketchup Killers  (C)1998      \r\n");

    if (idf) {
        strcpy(Info.path, Fen2->path);
        Path2Abs(Info.path, F->name);

        if ((F->attrib & RB_SUBDIR) == RB_SUBDIR) {
            FENETRE* OldDFen;
            int nbr, size;
            char sel;
            char temp[32];

            memset(&Info, 0, sizeof(RB_IDF));

            if (KKCfg->longname) {
                InfoLongFile(Fen2, winbuffer);
                strncpy(Info.fullname, winbuffer, 80);
                Info.fullname[79] = 0;
            } else
                strcpy(Info.fullname, F->name);

            OldDFen = DFen;

            sel = F->select;
            F->select = 1;

            CountRepSize(Fen2, &nbr, &size);

            F->select = sel;

            DFen = OldDFen;

            strcpy(Info.format, "Directory");

            Long2Str(size, temp);

            sprintf(Info.message[0], " %-12s bytes in %d files", temp, nbr);
        } else {
            if (Info.path[0] == 0)
                WinError("IDF 8");

            Traitefic(&Info);
        }
        Makediz(&Info, Buf);
    }

    InfoFile(F, Buf);

    strcat(Buf, "                                    \r\n");

    hsize = 0;
    i = 0;
    while (Buf[i] != 0) {
        if (Buf[i] == 13)
            hsize++;
        i++;
    }
    // Centre le diz

    hsize = (Fen->yl - Fen->y - hsize - 3) / 2;

    if (hsize > 1)
        Window(Fen->x + 1, Fen->y + 1, Fen->xl - 1, Fen->y + hsize, Cfg->col[16]);
    else
        hsize = 0;

    x = Fen->x + 2;
    y = Fen->y + hsize + 2;

    i = 0;
    while (Buf[i] != 0) {
        switch (Buf[i]) {
            case 10:
                x = Fen->x + 2;
                break;
            case 13:
                y++;
                break;
            default:
                AffChr(x, y, Buf[i]);
                x++;
                break;
        }
        if (y == Fen->yl - 1) break;
        i++;
    }

    Cadre(Fen->x + 1, Fen->y + hsize + 1, Fen->xl - 1, y, 2, Cfg->col[37], Cfg->col[38]);
    ColWin(Fen->x + 2, Fen->y + hsize + 2, Fen->xl - 2, y - 1, Cfg->col[39]);  // Int. info

    Window(Fen->x + 1, Fen->y + 1, Fen->xl - 1, Fen->y + hsize, Cfg->col[16]);
    Window(Fen->x + 1, y + 1, Fen->xl - 1, Fen->yl - 1, Cfg->col[16]);

    // ChrWin(Fen->x+2,Fen->y+6,Fen->xl-2,y-1,'1');
    // ChrWin(Fen->x+1,y+1,Fen->xl-1,Fen->yl-1,'2');

    LibMem(Buf);

    Fen->init = 0;
}

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void ClearInfo(FENETRE* Fen) {
    Cadre(Fen->x, Fen->y, Fen->xl, Fen->yl, 1, Cfg->col[37], Cfg->col[38]);

    Window(Fen->x + 1, Fen->y + 1, Fen->xl - 1, Fen->yl - 1, Cfg->col[39]);

    ChrLin(Fen->x + 1, Fen->yl - 2, Fen->xl - Fen->x - 1, 196);
    PrintAt(Fen->x + 1, Fen->yl - 1, "        Press CTRL-Y to close");
}

void DispInfo(FENETRE* Fen) {
    int n;  // Compteur
    FENETRE* Fen2;
    static char buffer[40];

    short i;

    int x, y;

    Fen2 = Fen->Fen2;

    if (Fen2->InfoPos > 40) Fen2->InfoPos = 40;

    x = Fen->x + 1;
    y = Fen2->y2 + 1;

    n = (Fen2->pcur) - (Fen2->scur);  // premier

    if (n < 0) n = 0;

    for (i = 0; (i < Fen2->yl2) & (n < Fen2->nbrfic); i++, n++, y++) {
        // ------------------ Line Activity ------------------------------------
        if (n == (Fen2->pcur))
            ColLin(x, y, 38, Cfg->col[1]);
        else {
            if (Fen2->F[n]->info != NULL)
                switch (Fen2->F[n]->info[0]) {
                    case 1:
                        ColLin(x, y, 38, Cfg->col[23]);
                        break;
                    case 2:
                        ColLin(x, y, 38, Cfg->col[23]);
                        break;
                    case 3:
                        ColLin(x, y, 38, Cfg->col[22]);
                        break;
                    case 4:
                        ColLin(x, y, 38, Cfg->col[32]);
                        break;
                    case 5:
                        ColLin(x, y, 38, Cfg->col[32]);
                        break;
                    case 6:
                        ColLin(x, y, 38, Cfg->col[0]);
                        break;
                    default:
                        ColLin(x, y, 38, Cfg->col[0]);
                        break;
                }
            else
                ColLin(x, y, 38, Cfg->col[4]);
        }

        if (Fen2->F[n]->info != NULL) {
            memcpy(buffer, Fen2->F[n]->info + 1 + Fen2->InfoPos, 38);
            buffer[38] = 0;
            PrintAt(x, y, "%-38s", buffer);
        } else
            PrintAt(x, y, "%-38s", "?");
    }

    for (; (i < Fen2->yl2); i++, y++) {
        PrintAt(x, y, "%-38s", "");
        ColLin(x, y, 38, Cfg->col[4]);
    }
}

/*--------------------------------------------------------------------*\
|-  Fenêtre avec les infos sur fichiers d'apres header                -|
\*--------------------------------------------------------------------*/

void FenInfo(FENETRE* Fen) {
    if (Fen->init == 1) {
        ClearInfo(Fen);
        Fen->init = 0;
        Fen->Fen2->InfoPos = 0;
    }

    DispInfo(Fen);

    if (SearchInfo(Fen) != 0)
        DispInfo(Fen);
}

/*--------------------------------------------------------------------*\
|-           Display about select                                     -|
\*--------------------------------------------------------------------*/

void InfoSelect(FENETRE* Fen) {
    char temp[20];

    if (Fen->FenTyp != 0) return;

    if (Fen->nbrsel == 0) {
        if (Fen->nbrfic == 1)
            PrintAt(Fen->x + 2, Fen->yl - 1, _OneFilHeader,
                    Long2Str(Fen->taillefic, temp));
        else
            PrintAt(Fen->x + 2, Fen->yl - 1, _MulFilHeader,
                    Long2Str(Fen->taillefic, temp), Fen->nbrfic);
    } else {
        if (Fen->nbrsel == 1)
            PrintAt(Fen->x + 2, Fen->yl - 1, _OneSelHeader,
                    Long2Str(Fen->taillesel, temp));
        else
            PrintAt(Fen->x + 2, Fen->yl - 1, _MulSelHeader,
                    Long2Str(Fen->taillesel, temp), Fen->nbrsel);
    }
}

/*--------------------------------------------------------------------*\
|-  The KEY's menu                                                    -|
\*--------------------------------------------------------------------*/

void CtrlMenu(void) {
    char temp[20];

    if (DFen->yl == 0) return;
    if (DFen->nbrfic == 1)
        PrintAt(DFen->x + 2, DFen->yl - 1, _OneFilHeader,
                Long2Str(DFen->taillefic, temp));
    else
        PrintAt(DFen->x + 2, DFen->yl - 1, _MulFilHeader,
                Long2Str(DFen->taillefic, temp), DFen->nbrfic);
}

void ShiftMenu(void) {
    if (DFen->yl == 0) return;

    if ((KKCfg->pathdown) | (KKCfg->dispath))
        return;

    AffUpperPath(DFen, DFen->yl - 1);
    AffUpperPath(DFen->Fen2, DFen->yl - 1);
}

void AltMenu(void) {
    static char buffer[40];

    if (DFen->yl == 0) return;

    if ((KKCfg->longname) & (DFen->system == 0))
        InfoLongFile(DFen, _intbuffer);
    else
        strcpy(_intbuffer, DFen->F[DFen->pcur]->name);

    if (strlen(_intbuffer) > 36) {
        memcpy(buffer, _intbuffer, 3);
        memcpy(buffer + 3, "...", 3);
        memcpy(buffer + 6, _intbuffer + strlen(_intbuffer) - 30, 30);
        buffer[36] = 0;
    } else {
        strcpy(buffer, _intbuffer);
    }

    PrintAt(DFen->x + 2, DFen->yl - 1, "%-36s", buffer);
}

/*--------------------------------------------------------------------*\
|-  Information on disk                                               -|
\*--------------------------------------------------------------------*/

void FenDisk(FENETRE* Fen) {
    int posy;
    char *Buf, temp[32], disk[4], volume[32];
    char InfoVol[32];
    char timebuf[40];
    time_t clock;

    FENETRE* Fen2;
    struct file* F;
    int x, y;

    static char chaine[80];
    short WindowsActif, HVer, NVer;

    static long oldfree;
    static char oldpath[256];

    long tfree, ttotal;

    char drive;

    drive = (char)(toupper(DFen->path[0]) - 'A');

    Fen2 = Fen->Fen2;
    F = Fen2->F[Fen2->pcur];

    if (Fen->init == 1) {
        oldfree = 0;
        oldpath[0] = 0;
    }

    ttotal = GetDiskTotal((char)(toupper(Fen2->path[0]) - 'A')) / 1024;

    tfree = GetDiskFree((char)(toupper(Fen2->path[0]) - 'A')) / 1024;

    if ((tfree != oldfree) | (strcmp(oldpath, Fen2->path) != 0))
        Fen->init = 1;

    /*
    if (Fen->init==0)
        {
        PrintAt(Fen->xl-1,Fen->yl-1,"?");
        return;
        }
    */

    if (Fen->init == 1) {
        Cadre(Fen->x, Fen->y, Fen->xl, Fen->yl, 1, Cfg->col[37], Cfg->col[38]);
        Window(Fen->x + 1, Fen->y + 1, Fen->xl - 1, Fen->yl - 1, Cfg->col[39]);
    }

    x = Fen->x + 1;
    y = Fen->y + 1;

    Buf = Fen2->path;
    if (strlen(Buf) > 37) Buf += (strlen(Buf) - 37);

#ifndef NOWINDOWS

    WindowsActif = windows(&HVer, &NVer);

    switch (WindowsActif) {
        case 0:
            sprintf(chaine, "Windows not present");
            break;
        case 0x81:
            sprintf(chaine, "Windows in Real Mode");
            break;
        case 0x82:
            sprintf(chaine, "Windows in Standard Mode");
            break;
        case 0x01:
            sprintf(chaine, "Windows/386 V 2.x");
            break;
        case 0x83:
            sprintf(chaine, "Windows V %d.%d Extended Mode", HVer, NVer);
            break;
    }
#else
    strcpy(chaine, "No Windows Support");
#endif

    posy = y;

    memcpy(disk, Fen2->path, 3);
    disk[3] = 0;
    drive = (char)(toupper(disk[0]) - 'A');

    DriveInfo(drive, volume);

    TypeDisk(drive, InfoVol);

    PrintAt(x, posy, " %s", RBTitle);
    ColLin(x, posy, 38, Cfg->col[40]);
    posy++;
    ChrLin(x, posy, 38, 196);
    posy++;

    PrintAt(x, posy, " Current directory");
    ColLin(x, posy, 38, Cfg->col[40]);
    posy++;
    PrintAt(x, posy, " %s", Buf);
    posy++;
    ChrLin(x, posy, 38, 196);
    posy++;

    PrintAt(x, posy, " Current disk");
    ColLin(x, posy, 38, Cfg->col[40]);
    posy++;

    PrintAt(x, posy, " %s [%s] %s", disk, volume, InfoVol);
    posy++;

    PrintAt(x, posy, " Free space: %8s kilobytes", Long2Str(tfree, temp));
    posy++;
    PrintAt(x, posy, " Capacity:   %8s kilobytes", Long2Str(ttotal, temp));
    posy++;

    ChrLin(x, posy, 38, 196);
    posy++;
    PrintAt(x + 1, posy, "System Information");
    ColLin(x + 1, posy, 37, Cfg->col[40]);
    posy++;

    PrintAt(x, posy, " ? %s", chaine);
    posy++;  // Information about windows
    PrintAt(x, posy, " ? Initialisation: %d clocks ", Info->temps);
    posy++;
    PrintAt(x, posy, " ? Long filenames support: %s ",
            KKCfg->longname ? "Yes" : "No");
    posy++;

    ChrLin(x, posy, 38, 196);
    posy++;

    clock = time(NULL);
    strftime(timebuf, 40, "%b %d,%Y %H:%M:%S", localtime(&clock));

    PrintAt(x, posy, " ? Date & Time: %21s", timebuf);
    posy++;

    ChrLin(x, posy, 38, 196);
    posy++;

    /*
    int t;
    PrintAt(x,posy,"handle: %d",t=open("file_id.diz",O_RDONLY)); posy++;

    int n;
    for(n=0;n<64;n++)
        {
        struct stat filestat;
        if (fstat(n,&filestat)==0)
            {
            PrintAt(x,posy,"%2d '%s'%d'%d'",n,filestat.st_name,filestat.st_dev,filestat.st_size);
            posy++;
            }
        }
    close(t);
    */

    PrintAt(Fen->xl - 1, Fen->yl - 1, "");

    oldfree = tfree;
    strcpy(oldpath, Fen2->path);

    Fen->init = 0;
}

/*--------------------------------------------------------------------*\
|-  Information on FILE_ID.DIZ                                        -|
\*--------------------------------------------------------------------*/
void FenFileID(FENETRE* Fen) {
    int x, y, n, l;
    char c;
    char path[256], *name;
    FILE* fic;
    char* ext;
    static char oldpath[256], oldname[256];
    FENETRE* Fen2;

    if (Fen->init) {
        oldpath[0] = 0;
        oldname[0] = 0;
    }

    Fen2 = Fen->Fen2;

    x = Fen->x + 1;
    y = Fen->y + 1;

    ext = getext(Fen2->F[Fen2->pcur]->name);

    strcpy(path, Fen2->path);
    if (((Fen2->F[Fen2->pcur]->name[0] != '.') & (IsDir(Fen2->F[Fen2->pcur]))) |
        (!stricmp(ext, "ARJ")) |
        (!stricmp(ext, "ZIP")) |
        (!stricmp(ext, "DFP")) |
        (!stricmp(ext, "RAR")) |
        (!stricmp(ext, "LHA")))
        Path2Abs(path, Fen2->F[Fen2->pcur]->name);  // Ajout gedeon
    /*    else
        oldpath[0]=0;*/
    // PQ ?
    Path2Abs(path, "FILE_ID.DIZ");

    if (strcmp(path, oldpath) != 0) {
        Cadre(Fen->x, Fen->y, Fen->xl, Fen->yl, 1, Cfg->col[37], Cfg->col[38]);
        Window(Fen->x + 1, Fen->y + 1, Fen->xl - 1, Fen->yl - 1, Cfg->col[39]);

        strcpy(oldpath, path);

        name = AccessAbsFile(path);  // Ajout GEDEON ------------------------

        if (name == NULL) {
            PrintAt(x + 1, y + 1, "No FILE_ID.DIZ.");
            strcpy(oldname, "");
        } else
            strcpy(oldname, name);

        PrintAt(Fen->xl - 1, Fen->yl - 1, "");
    } else
        PrintAt(Fen->xl - 1, Fen->yl - 1, "?");

    // Affichage du file_id.diz dans la fenetre ----------------------------

    if (oldname[0] != 0) {
        fic = fopen(oldname, "rb");
        if (fic == NULL) {
            PrintAt(x + 1, y + 1, "FILE_ID.DIZ doesn't exist !");
            return;
        }

        l = flength(fileno(fic));

        if (l > 32768) l = 32768;

        for (n = 0; n < l; n++) {
            c = (char)(fgetc(fic));

            switch (c) {
                case 10:
                    x = Fen->x + 1;
                    y++;
                    break;
                case 7:
                case 13:
                    break;
                default:
                    if (x < Fen->xl) {
                        AffChr(x, y, c);
                        x++;
                    }
                    break;
            }
            if (y >= Fen->yl) break;
        }
        fclose(fic);
    }

    Fen->init = 0;
}
