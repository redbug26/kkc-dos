/*--------------------------------------------------------------------*\
|- liaison application fichier                                        -|
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
|-                                                                    -|
|- Code de retour:                                                    -|
|- 0: perfect                                                         -|
|- 1: error                                                           -|
|- 2: no player for this file                                         -|
|- 3: Arret ESCape                                                    -|
|-                                                                    -|
|- kefaire: 0 lancer application + fichier                            -|
|-          1 lancer application toute seule                          -|
|-          2 mettre l'application dans le buffer *name               -|
|-                                                                    -|
\*--------------------------------------------------------------------*/
int FicIdf(char *dest,char *name,int numero,int quefaire);



/*--------------------------------------------------------------------*\
|- Code de retour:                                                    -|
|- 0: perfect                                                         -|
|- 1: error                                                           -|
|- 2: no player for this file                                         -|
\*--------------------------------------------------------------------*/
int PlayerIdf(char *name,int numero);


/*--------------------------------------------------------------------*\
|- Gestion des players dont le numero idf est curr                    -|
\*--------------------------------------------------------------------*/
void FicIdfMan(int curr,char *chaine);
