void PutInHistDir(void);
void ChangeTaille(int i);

void SpeedTest(void);                               // Independant de KK



void FenNor(struct fenetre *Fen);
void FenDIZ(struct fenetre *Fen);
void FenDisk(struct fenetre *Fen);
void LitInfo(struct fenetre *Fen);


int InfoIDF(struct fenetre *Fen);
int NameIDF(char *buf);
void AfficheTout(void);
void ErrWin95(void);
void ASCIItable(void);
void YouMad(char *s);

// void SetTaille(void);

void MenuBar(char c);           // Bar de menu (tout en dessous) F1.. F2..

void UseCfg(void);

void Setup(void);

extern int *TailleX;

/*----------------------------*
 - Fonction de la secret part -
 *----------------------------*/

void WinInfo(struct fenetre **Fenetre);
short windows(short *HVersion, short *NVersion );
void FenInfo(struct fenetre *Fen);
void PacNoe(void);



