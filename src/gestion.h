/*--------------------------------------------------------------------*\
|-  Header of gestion-function                                        -|
\*--------------------------------------------------------------------*/


void RemoveHistDir(int a0,int a1);
void VerifHistDir(void);
char *GetLastHistDir(void);

char *GetPrevHistCom(char *chaine);
char *GetNextHistCom(char *chaine);

void ExecCom(void);

void AffCmdLine(void);
int CommandLine(char *string,...);
int Run(char *chaine);
char GetDrive(void);
void SetDrive(char i);

void SortFic(FENETRE *Fen);

void History2Line(char *src,char *dest);
void Line2History(char *src);
void Line2PrcLine(char *s,char *chaine);

/*--------------------------------------------------------------------*\
|- Function:     int IsDir(struct file *F)                            -|
|-                                                                    -|
|- Description:  Test if a file is a directory                        -|
|-                                                                    -|
|- Input:        F: The file                                          -|
|-                                                                    -|
|- Return:       0: It isn't a directory                              -|
|-               1: It's a directory                                  -|
\*--------------------------------------------------------------------*/
int IsDir(struct file *f);

/*--------------------------------------------------------------------*\
|- Function:     int FicSelect(FENETRE *Fen,int n,char q)             -|
|-                                                                    -|
|- Description:  Selection, Deselect or inverse the select. of a file -|
|-                                                                    -|
|- Input:        Fen: Working pannel                                  -|
|-               n: number of the file                                -|
|-               q: 0 -> deselect                                     -|
|-                  1 -> select                                       -|
|-                  2 -> inverse selection                            -|
|-                                                                    -|
|- Return:       0: It's Okay...                                      -|
|-               1: Error, you couldn't do that                       -|
\*--------------------------------------------------------------------*/
int FicSelect(FENETRE *Fen,int n,char q);

/*--------------------------------------------------------------------*\
|- Ligne de commande                                                  -|
\*--------------------------------------------------------------------*/
extern char CLstr[256];

