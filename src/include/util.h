/*--------------------------------------------------------------------*\
|- prototype pour les utilitaires                                     -|
\*--------------------------------------------------------------------*/
int WinAttrib(void);
void ServerMode(void);

void Menu(void);
char MenuInsert(char *section,char *titre,char *buffer);
void MenuCreat(char *titbuf,char *buf,char *path);

void ExecLCD(FENETRE *Fen,char *dir);
int MakeNCD(void);
