void Fenet(int x1,int y1,int x2,int y2,int type);

int Gradue(int x,int y,int length,int from,int to,int total);

void FenNor(struct fenetre *Fen);
void FenDIZ(struct fenetre *Fen);
int InfoIDF(struct fenetre *Fen);
void AfficheTout(void);
void MenuBar(char c);
void ErrWin95(void);
void ASCIItable(void);
void YouMad(char *s);
int WinError(char*);

void SetTaille(void);

int WinTraite(struct Tmt *T,int nbr,struct TmtWin *F);

char *Long2Str(long n);

extern int *TailleX;



struct TmtWin {
     int x1,y1,x2,y2;
     char *name;
     };

struct Tmt {
     int x,y;       // position
     char type;
     char *str;
     int *entier;
     };

/* Tmt:
  Type: 0 --> Titre
        1 --> String
        2 --> OK OK OK
        3 --> CANCEL
        4 --> Cadre 4 de hauteur
        5 --> Bouton personnalis‚
*/



