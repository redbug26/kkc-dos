/*-----------------------------------------------------------------------*\
|-                                                                       -|
|-  Toutes les fonctions independantes (logiquement) … l'O.S. ainsi qu'  -|
|-                                                   aux fichiers hard.c -|
|-                                                                       -|
|- By RedBug/Ketchup Killers                                             -|
|-                                                                       -|
\*-----------------------------------------------------------------------*/

char *getext(const char *nom);
char CnvASCII(char table,char car);
int Maskcmp(char *src,char *mask);
int find1st(char *src,char *dest,char *mask);
int WildCmp(char *a,char *b);
char *FileinPath(char *p,char *Ficname);
void Path2Abs(char *p,const char *Ficname);
char FoundExt(char *ext,char *src);
char *Long2Str(long n,char *chaine);         // Convertit un long en chaine
char *Int2Char(int n,char *s,char length); // Convertit un entier en chaine
int LongGradue(int x,int y,int length,int from,int to,int total);
char *StrUpr(char *chaine);
