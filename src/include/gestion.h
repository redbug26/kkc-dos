/*--------------------------------------------------------------------*\
|-  Header of gestion-function					      -|
\*--------------------------------------------------------------------*/
#define RB_ARCHIVE  0x20
#define RB_SUBDIR   0x10
#define RB_VOLID    0x08
#define RB_SYSTEM   0x04
#define RB_HIDDEN   0x02
#define RB_RDONLY   0x01



void MakeDir(char *Ficname);
int PathExist(char *path);

void RemoveHistDir(int a0,int a1);
void VerifHistDir(void);
char *GetLastHistDir(void);

char *GetPrevHistCom(char *chaine);
char *GetNextHistCom(char *chaine);
void PutInHistCom(char *chaine);

void ExecCom(void);

void AffCmdLine(void);
int CommandLine(char *string,...);
int KeyLine(char *string,...);
int Run(char *chaine);
char GetDrive(void);
void SetDrive(char i);

void SortFic(FENETRE *Fen);

void History2Line(char *src,char *dest);
void Line2History(char *src);
void Line2PrcLine(char *s,char *chaine);

/*--------------------------------------------------------------------*\
|- Function:	 int IsDir(struct file *F)			      -|
|-								      -|
|- Description:  Test if a file is a directory			      -|
|-								      -|
|- Input:	 F: The file					      -|
|-								      -|
|- Return:	 0: It isn't a directory                              -|
|-		 1: It's a directory                                  -|
\*--------------------------------------------------------------------*/
int IsDir(struct file *f);

/*--------------------------------------------------------------------*\
|- Function:	 int FicSelect(FENETRE *Fen,int n,char q)	      -|
|-								      -|
|- Description:  Selection, Deselect or inverse the select. of a file -|
|-								      -|
|- Input:	 Fen: Working pannel				      -|
|-		 n: number of the file				      -|
|-		 q: 0 -> deselect				      -|
|-		    1 -> select 				      -|
|-		    2 -> inverse selection			      -|
|-								      -|
|- Return:	 0: It's Okay...                                      -|
|-		 1: Error, you couldn't do that                       -|
\*--------------------------------------------------------------------*/
int FicSelect(FENETRE *Fen,int n,char q);

/*--------------------------------------------------------------------*\
|- Ligne de commande						      -|
\*--------------------------------------------------------------------*/
extern char CLstr[256];



/**/

struct file *GetFile(FENETRE *Fen,int n);
char *GetFilename(FENETRE *Fen,int n);
void FreeFile(FENETRE *Fen,int n);
void CreateFile(FENETRE *Fen,char *name,unsigned short time,unsigned short date,char attrib,int size);

FENETRE *AllocWin(void);
void FreeWin(FENETRE *SFen);

void CreateNewFen(FENETRE *Fen,int n);
void CloseNewFen(FENETRE *Fen);

