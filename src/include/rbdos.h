/*--------------------------------------------------------------------*\
|- Gestion des ressources DOS - prototype                             -|
\*--------------------------------------------------------------------*/

int _4DOSverif(void);
char *_4DOSLhistdir(void);
void _4DOSShistdir(char*);

char KeyTurbo(char val);


/*--------------------------------------------------------------------*\
|- gestion des disques                                                -|
\*--------------------------------------------------------------------*/

//--- drive 0 -> A:


//--- teste si le disque est pret --------------------------------------
int DriveReady(char drive);

//--- type de disque ---------------------------------------------------
// return 0: hard disk
//        1: cdrom
//        2: link
int TypeDisk(int drive,char *info);

//--- nombre d'octets libres -------------------------------------------
long GetDiskFree(char c);

//--- nombre de bytes total --------------------------------------------
long GetDiskTotal(char drive);

//--- information (label, ...) -----------------------------------------
void DriveInfo(char drive,char *volume);

//--- Renvoie la path pour le drive courrant ---------------------------
void DrivePath(char drive,char *path);

//--- Test l'existence -------------------------------------------------
int DriveExist(char drive);

//--- Accede … la path -------------------------------------------------
void DriveSet(char *path);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

short windows(short *HVersion, short *NVersion );

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

void Short2LongFile(char *from,char *to);
char Verif95(void);

void EjectCD(FENETRE *Fen);
void EjectDrive(char lect);
void SetWindowsTitle(void);


/*--------------------------------------------------------------------*\
|- Rename from -> to                                                  -|
|-      0 -> ok                                                       -|
|-  autre -> erreur                                                   -|
\*--------------------------------------------------------------------*/

bool InternalRename(char *from,char *to);

void ClipBoardInfo(void);

char IsFileExist(char *name);

void TitleBox(char *string,...);

char ExistClipboard(void);
void PutInClipboard(char *buffer,int lng);

/*--------------------------------------------------------------------*\
\*--------------------------------------------------------------------*/

int iskeypressed(int);

#define SHIFT 1
#define CTRL 2
#define ALT 3

