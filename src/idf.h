#define ULONG unsigned long
#define BYTE unsigned char
#define WORD unsigned short

#define nbrkey 140
        // number of format + 6

/*------------------------------------*
 - Internal description of all format -
 *------------------------------------*/
struct key
        {
        char buf[25];
        char len;
        short pos;
        char *format;
        char *ext;
        char *pro;
        short numero;
        char proc;
        char other;
        char type;
        };


/*--------------------------------*
 * Information that you muse use  *
 *--------------------------------*/

typedef struct __idfinfo
   {
   int numero;           // ID KEY of the format

   char path[256];       // The path of the file

   char filename[80];    // Name of the file
   char fullname[80];    // Title of the file
   char message[10][80]; // Various message
   char format[80];      // Name of the format
   char info[80];        // Name of O.S. (if os!=0)
   char Tinfo[80];
   char composer[80];    // Name of the composer (for the module)
   ULONG taille;         // Size of the file (if !=0)
   char ext[4];          // The extension of the format
   char os;              // 1: DOS, 2:WIN, 3:WIN32
   int Btype;            // 1: module, 2: sample, 3: archive,
                         // 4: bitmap, 5: anim  , 6: other.
/* Internal variable */
   char *buffer;         // buffer for E/S
   ULONG posbuf;         // position in buffer (from posfic)
   WORD sizebuf;         // size of buffer
   FILE *fic;
   ULONG posfic;


} RB_IDF;



void Traitefic(RB_IDF*);       // traite un fichier

// Call this function after giving the full pathname of the file



