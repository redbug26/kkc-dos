/*--------------------------------------------------------------------*\
|- prototype pour la gestion des fenetres (?)                         -|
\*--------------------------------------------------------------------*/

void CalcSizeWin(FENETRE *Fen);

void ScreenSetup(void);

void PutInHistDir(void);
void ChangeTaille(int i);

void SpeedTest(void);                               // Independant de KK

void FenNor(FENETRE *Fen);
void FenDIZ(FENETRE *Fen);
void FenDisk(FENETRE *Fen);
void FenTree(FENETRE *Fen);
void FenFileID(FENETRE *Fen);
void ViewFileID(FENETRE *Fen);
int SearchInfo(FENETRE *Fen);
void DispInfo(FENETRE *Fen);
void WinInfo(FENETRE **Fenetre);
void FenInfo(FENETRE *Fen);


int InfoIDF(FENETRE *Fen);
int NameIDF(char *buf);
void AfficheTout(void);
void ErrWin95(void);
void ASCIItable(void);
void YouMad(char *s);

void MenuBar(char c);         // Bar de menu (tout en dessous) F1.. F2..

void UseCfg(void);

void Setup(void);

extern int *TailleX;



