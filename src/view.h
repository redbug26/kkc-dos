/*--------------------------------------------------------------------*\
|-  Prototype du viewer                                               -|
\*--------------------------------------------------------------------*/


typedef struct _viewer
    {
    char warp;                    // 0: pas de warp, 1: word, 2: entier
    char cnvtable;                // Table de conversion pour le viewer
    char autotrad;                  // Traduit automatiquement un table
    char ajustview;                  // Fit the width of file in viewer
    unsigned char wmask;             // C'est quel masque kon emploie ?

    char lnfeed;                // 0: CR/LF  1:CR  2:LF  3:user 4:CR|LF
    char userfeed;                                       // Si lnfeed=3

    char saveviewpos;                // Save position of file in viewer
    char *viewhist;               // Fichier o— copie les save position

    long AnsiSpeed;

    struct PourMask **Mask;
    } KKVIEW;


void View(KKVIEW *V,char *file,int type);

//   0: Auto Type
//   1: Ansi viewer
//   2: Html viewer
//   3: Hexa viewer
//   4: Text viewer
//   5: Raw  viewer


struct PourMask
     {
     char Ignore_Case;                         // 1 si on ignore la case
     char Other_Col;                  // 1 si on colorie les autres noms
     char chaine[1024];  // chaine de comparaison EX: "asm break case @"
     char title[40];                                 // nom de ce masque
     };

// extern struct PourMask **Mask;
