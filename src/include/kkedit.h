/*--------------------------------------------------------------------*\
|- Prototype de l'editeur                                             -|
\*--------------------------------------------------------------------*/

#ifndef KKEDIT_H

#define KKEDIT_H


typedef struct _editor
    {
    char zoom;
    } KKEDIT;

int TxtEdit(KKEDIT *E,char *fichier);

#endif
