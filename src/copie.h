/*--------------------------------------------------------------------*\
|- prototype fonction de copie                                        -|
\*--------------------------------------------------------------------*/
void Copie(FENETRE *F1,FENETRE *FTrash,char *path);
void Move(FENETRE *F1,FENETRE *FTrash,char *path);

int CountRepSize(FENETRE *F1,FENETRE *FTrash,int *nbr,int *size);
                    //--- Renvoit -1 si le nombre de fichier est nul ---

void WinRename(FENETRE *F1);
void WinRename2(FENETRE *F1);
