void PutInHistDir(void);
void ChangeTaille(int i);

void SpeedTest(void);                               // Independant de KK



void FenNor(struct fenetre *Fen);
void FenDIZ(struct fenetre *Fen);
void LitInfo(struct fenetre *Fen);


int InfoIDF(struct fenetre *Fen);
int NameIDF(char *buf);
void AfficheTout(void);
void MenuBar(char c);
void ErrWin95(void);
void ASCIItable(void);
void YouMad(char *s);

// void SetTaille(void);



void UseCfg(void);

void Setup(void);

extern int *TailleX;

/*----------------------------*
 - Fonction de la secret part -
 *----------------------------*/

void WinInfo(struct fenetre **Fenetre);
short windows(short *HVersion, short *NVersion );
void FenInfo(struct fenetre *Fen);


/*-------------------------------------------*
 -       Gestion de la barre de menu         -
 *-------------------------------------------*/

int BarMenu(struct barmenu *bar,int nbr,int *poscur,int *xp,int *yp);
int PannelMenu(struct barmenu *bar,int nbr,int *c,int *xp,int *yp);
struct barmenu
        {
        char titre[20];
        char help[80];
        short fct;
        };
