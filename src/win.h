void PutInHistDir(void);
void ChangeTaille(int i);

void Fenet(int x1,int y1,int x2,int y2,int type);

void FenNor(struct fenetre *Fen);
void FenDIZ(struct fenetre *Fen);

void FenInfo(struct fenetre *Fen);
void LitInfo(struct fenetre *Fen);

void WinInfo(void);

int InfoIDF(struct fenetre *Fen);
int NameIDF(char *buf);
void AfficheTout(void);
void MenuBar(char c);
void ErrWin95(void);
void ASCIItable(void);
void YouMad(char *s);

// void SetTaille(void);




void Setup(void);

char *Long2Str(long n);

extern int *TailleX;


/*********************************************
 -       Gestion de la barre de menu         -
 *********************************************/

int BarMenu(struct barmenu *bar,int nbr,int *poscur,int *xp,int *yp);
int PannelMenu(struct barmenu *bar,int nbr,int *c,int *xp,int *yp);
struct barmenu
        {
        char titre[20];
        char help[80];
        short fct;
        };
