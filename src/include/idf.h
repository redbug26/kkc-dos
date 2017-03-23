#define uchar  unsigned char
#define ushort unsigned short
#define uint   unsigned int
#define ulong  unsigned long


#define nbrkey 201
        // number of format + 6

/*--------------------------------------------------------------------*\
|-  Internal description of all format                                -|
\*--------------------------------------------------------------------*/
struct key
        {
        char *buf;
        char len;
        short pos;
        char *format;
        char *ext;
        char *pro;
        short numero;
        char other;
        char type;
        };

/*
   other: bit 7 6 5 4 3 2 1 0
              ³ ³ ³ ³ ³ ³ ³ ³
              ³ ³ ³ ³ ³ ³ ³ ÀÄÄÄ 1: IDF have more information on file
              ³ ³ ³ ³ ³ ³ ÀÄÄÄÄÄ 2: The key is not defined in structure
              ³ ³ ³ ³ ³ ÀÄÄÄÄÄÄÄ 4: This is a text file
              ³ ³ ³ ³ ÀÄÄÄÄÄÄÄÄÄ 8: Sauce information possible
              ÀÄÁÄÁÄÁÄÄÄÄÄÄÄÄÄÄÄ  : Always to 0

   type: 1: module
         2: sample
         3: archive
         4: bitmap
         5: animation
         6: others
*/


/*--------------------------------------------------------------------*\
|-  Information that you muse use                                     -|
\*--------------------------------------------------------------------*/

typedef struct __idfinfo
   {
   char path[256];                        //--- The path of the file ---
   char *inbuf;   //--- Buffer sur lequel on travaille si path[0]==0 ---
   int buflen;    //--- Longueur de ce buffer --------------------------

   int numero;                                   // ID KEY of the format
   char filename[80];                                // Name of the file
   char fullname[80];                               // Title of the file
   char message[10][80];                              // Various message
   char format[80];                                // Name of the format
   char info[80];                             // Name of O.S. (if os!=0)
   char Tinfo[80];
   char composer[80];           // Name of the composer (for the module)
   ulong taille;                            // Size of the file (if !=0)
   char ext[32];                          // The extension of the format
   char os;                                    // 1: DOS, 2:WIN, 3:WIN32
   int Btype;                       // 1: module, 2: sample, 3: archive,
                                      // 4: bitmap, 5: anim  , 6: other.
/* Internal variable */
   char *buffer;                                       // buffer for E/S
   ulong posbuf;                     // position in buffer (from posfic)
   ushort sizebuf;                                     // size of buffer
   FILE *fic;
   ulong posfic;
   ulong sizemax;                            // taille reelle du fichier
   ulong begin;
} RB_IDF;



#ifdef __cplusplus
extern "C"
{
#endif

void Traitefic(RB_IDF*);       //--- traite un fichier -----------------
void QuickIdf(RB_IDF*,int n);       //--- traite un fichier ------------

extern struct key K[nbrkey];


#ifdef __cplusplus
};
#endif

/*--------------------------------------------------------------------*\
|-  Call this function after giving the full pathname of the file     -|
\*--------------------------------------------------------------------*/



