/*--------------------------------------------------------------------*\
|-  Header of gestion-function                                        -|
\*--------------------------------------------------------------------*/


void RemoveHistDir(int a0,int a1);
void VerifHistDir(void);
char *GetLastHistDir(void);

char *GetLastHistCom(void);

void ExecCom(void);

void ChangeLine(void);
int CommandLine(char *string,...);
int Run(char *chaine);
char GetDrive(void);
void SetDrive(char i);
int FicSelect(int n,char q);
void SortFic(FENETRE *Fen);

void History2Line(char *src,char *dest);
void Line2History(char *src);

int IsDir(struct file *f);


