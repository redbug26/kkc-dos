/*--------------------------------------------------------------------*\
|- Gestion des ressources DOS - prototype                             -|
\*--------------------------------------------------------------------*/

long GetDiskFree(char c);

int _4DOSverif(void);
char *_4DOSLhistdir(void);
void _4DOSShistdir(char*);

char KeyTurbo(char val);

int TypeDisk(int drive,char *info);
short windows(short *HVersion, short *NVersion );

void DriveInfo(char drive,char *volume);
void DrivePath(char drive,char *path);
int DriveReady(char drive);
int DriveExist(char drive);
void DriveSet(char *path);

void GetVolume(char disk,char *volume);

void InfoLongFile(FENETRE *Fen,char *chaine);
void Short2LongFile(char *from,char *to);
void UpdateLongName(char *from,char *to);
char Verif95(void);

void EjectCD(FENETRE *Fen);
void EjectDrive(char lect);
void SetWindowsTitle(void);

char Win95Rename(char *from,char *to);

void ClipBoardInfo(void);

char IsFileExist(char *name);
