/*-----------------------------------------------------------------------*
 -                                                                       -
 -  Toutes les fonctions independantes (logiquement) … l'O.S. ainsi qu'  -
 -                                                   aux fichiers hard.c -
 -                                                                       -
 - By RedBug/Ketchup Killers                                             -
 -                                                                       -
 *-----------------------------------------------------------------------*/

char *getext(const char *nom);
char CnvASCII(char car);
int Maskcmp(char *src,char *mask);
int find1st(char *src,char *dest,char *mask);
int WildCmp(char *a,char *b);
char *FileinPath(char *p,char *Ficname);
void Path2Abs(char *p,char *Ficname);
