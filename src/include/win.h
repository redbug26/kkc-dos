/*--------------------------------------------------------------------*\
|- prototype pour la gestion des fenetres (?)                         -|
\*--------------------------------------------------------------------*/

void ScreenSetup(void);
void LongnameSetup(void);

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
void FenInfo(FENETRE *Fen);

int InfoIDF(FENETRE *Fen);
int NameIDF(char *buf);
void AfficheTout(void);
void ErrWin95(void);
void ASCIItable(void);
void YouMad(char *s);

void MenuBar(signed char c);  // Bar de menu (tout en dessous) F1.. F2..

void UseCfg(void);

void Setup(void);

extern int *TailleX;





