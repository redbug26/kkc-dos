/*--------------------------------------------------------------------*\
|- prototype fonction de copie                                        -|
\*--------------------------------------------------------------------*/
void ExtendedCopy(void);

void Copie(FENETRE *F1,char *path);
void Move(FENETRE *F1,char *path);

int CountRepSize(FENETRE *F1,int *nbr,int *size);
                    //--- Renvoit -1 si le nombre de fichier est nul ---

int RepSize(char *path);                    // renvoit -1 si on a pas lu

void WinRename(FENETRE *F1);
void WinRename2(FENETRE *F1);
