// Header of gestion-function

#define ULONG unsigned int
#define BYTE unsigned char
#define WORD unsigned short


int Maskcmp(char *src,char *mask);
char *FileinPath(char *p,char *Ficname);
int find1st(char *src,char *dest,char *mask);

void RemoveHistDir(int a0,int a1);
void VerifHistDir(void);
char *GetLastHistDir(void);


void Path2Abs(char *p,char *Ficname);
int WildCmp(char *a,char *b);

void ChangeLine(void);
int CommandLine(char *string,...);

int Run(char *chaine);
char GetDrive(void);
void SetDrive(char i);

int FicSelect(int n,char q);
char *getext(char *nom,char *ext);

