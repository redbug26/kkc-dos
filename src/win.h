/*--------------------------------------------------------------------*\
|- prototype pour la gestion des fenetres (?)                         -|
\*--------------------------------------------------------------------*/

void ScreenSetup(void);

void PutInHistDir(void);
void ChangeTaille(int i);

void SpeedTest(void);                               // Independant de KK

void FenNor(FENETRE *Fen);
void FenDIZ(FENETRE *Fen);
void FenDisk(FENETRE *Fen);
void FenTree(FENETRE *Fen);
void FenFileID(FENETRE *Fen);
int SearchInfo(FENETRE *Fen);
void DispInfo(FENETRE *Fen);


int InfoIDF(FENETRE *Fen);
int NameIDF(char *buf);
void AfficheTout(void);
void ErrWin95(void);
void ASCIItable(void);
void YouMad(char *s);

void MenuBar(char c);         // Bar de menu (tout en dessous) F1.. F2..
void Bar(char *);                               // Affichage de la barre

void UseCfg(void);

void Setup(void);

extern int *TailleX;

void RemplisVide(void);  // Remplissage du vide pour les plus de 80 col.

/*--------------------------------------------------------------------*\
|-  Fonction de la secret part                                        -|
\*--------------------------------------------------------------------*/

void WinInfo(FENETRE **Fenetre);
short windows(short *HVersion, short *NVersion );
void FenInfo(FENETRE *Fen);
void PacNoe(void);



