#define ULONG unsigned int
#define BYTE unsigned char
#define WORD unsigned short

struct key      {
        char buf[25];
        char len;
        short pos;
        char *format;
        char *ext;
        char *pro;
        short numero;
        char proc;              // procedure pour cl‚
        char other;     // autres informations
        char type;              // type de fichier
        };

#define nbrkey 110

struct info
   {
   int handle;
   ULONG posfic;
   char filename[80];
   char fullname[80];		// le plus grand fullname est 40 (farandole)
   char message[10][80];	// 10 Messages de 80 caracters
   char format[80];
   char info[80];               // contient le nom de l'OS si (os!=0)
   char Tinfo[80];
   char composer[80];
   ULONG taille;
   char ext[4];
   int type;
   char os;	// 1: DOS, 2:WIN, 3:WIN32
   int numero;
   int Btype;	// 1: module, 2: image, 3: executable,
				// 4:other,   5:sample/instruments, 6:anim.

   char *buffer;	// buffer pour E/S
   ULONG posbuf;	// position du buffer dans le fichier par rapport … posfic
   WORD sizebuf;		// taille du buffer

   char path[256];              //
};

void Traitefic(char *nomfic,struct info*);	 // traite un fichier

void ClearSpace(char *name);    // efface les espaces inutiles


