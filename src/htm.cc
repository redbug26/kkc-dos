/0*****************************************************************************/
/1 HTM2TXT: Version 0.4                                                   */
/2 Conversion de fichier HTML (format "internet") vers fichiers textes    */
/3**************************************************************************/
/4 AmÇlioration Ö faire                                                   ***/
/5 - Reconnaitre EM, /EM, /FONT, H5, H6                                   ****/
/6 - Changer listes avec stack                                            *****/
/7 - Lorsqu'il y ALT dans une 'IMG', l'afficher                           ******/
/8 - 'H? ... ' ne marche pas                                              */
/9 - 'H?' ne doit pas rÇinitialiser la position Ö ZÇro                    */
/* - Beaucoup de commandes non fonctionnelle                              */
/* - Aide en ligne                                                        */
/* - AmÇlioration des menus                                               */
/* - Fichier d'aide ?                                                     */
/* - Lors de l'acceptation du fichier, donner une liste de fichiers       */
/* - Permettre d'unir deux pages -> une seule                             */
/* - Refaire le NLQ !!!                                                   */
/**************************************************************************/
/* Bugs:                                                                  */
/* -----                                                                  */
/* - Probläme avec la pile (solution actuelle : modification de _stklen ?)*/
/**************************************************************************/
/* History:                                                               */
/* --------                                                               */
/* v0.1:  010195 Plusieurs versions non datÇes. Plante souvent.           */
/* v0.2:  220695 Premiäre version pour laquelle j'affiche le numÇro       */
/*               Supporte (c'est le mot ;}) la plupart des commandes HTML */
/*               Cmds reconnues parfaitement: 'H1', 'H2', 'H3', 'H4',     */
/*                              ... 'PRE', 'P', 'HR', 'BR', 'STRONG', 'B' */
/*               CrÇe des pages spÇciales FILOFAX ou des pages Çcrans     */
/* v0.3:  270695 AmÇlioration du menu: accepte le nom des fichiers        */
/*               Appuie sur F10 pour lancer le programme                  */
/*               Fenàtre de sortie plus belle (Projet de page couleur..?) */
/*               Option PRNCR. Vais je la gardÇe ?                        */
/* v0.4:  270895 Meilleur traitement des titres                           */
/* v0.5:  061095 Totale rÇecriture, supporte mieux le format Filofax      */
/*               Plus lente, un petit truc Ö changer ?                    */
/*               La page de prÇsentation et de commande ne veux rien dire */
/* v0.8:  ?????? Bcp d'amÇlioration en vues                               */
/**************************************************************************/

#include <alloc.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <ctype.h>

#include "c:\src\library\lib.h"

void go(void);

void LigneOK(void);     	    	      	/* Envoie le buffer et imprime */
void Pourcar(char Car);     	    	    /* Caractere */
void Pourprn(char Quoi,char Combien);       /* Attribut imprimante */
void Cadre_bas(void);
void Cadre_haut(void);
void Cadre_gadr(void);

void Print(void);                                                                           /* Envoie le buffer en entier */
void PRN_cr(void);		                    /* DÇfinit l'interligne et imprime le buffer */
void PRN_nlq(char oui);                     /* Impression NLQ */
void PRN_font(char o);                      /* Change les fonts */
void PRN_Gras(char Oui);                    /* BOLD */


FILE *fic;
FILE *infic,*outfic;
int t_buffer;
char buffer[255];
char bufpas[255];
int  tab[255];
int Tot_ligne=0;

char txt=0;

char condense=1;    	    	/* If condense ==> plus petit possible */
char justif=1;		    	    /* Emploie justification */
char nlq=0;                     /* If nlq       ==> imprime avec NLQ */
int pas=4; 						/* Taille d'un caractäre  */
int limite=95*4; 				/* Taille maximum 95 FILOFAX 95 colonnes avec 4 par car */
char bof=1;             		/* Autorise le BOF */
unsigned char inter=90; 		/* Nombre d'interligne (45 ou 90 avec condense) */

int pre=0;            			/* Texte non prÇformatÇ ! */

char retour=0;  				/* LR/HR => PRN CR */

/******************************************/
/* Taille des caracteres   #27+#91+#n     */
/*               12              0        */
/*               10              1        */
/*                8              2        */
/*                7              3        */
/*                6              4        */
/*                5              5        */
/*                4              6        */
/******************************************/

/*****************************/
/* Traitement Imprimante     */
/*****************************/

struct ligne  {
	char car;
	char prn[10];
	};

struct ligne Bufis[240];            /* Buffer Ö imprimer */
struct ligne Bucou;					/* Dernier buffer imprimÇ */
int Bupos;                          /* Position dans le buffer */

int		Pr_large;                   /* Largeur du cadre d'impression en mm */
int		Pr_haut;                    /* Hauteur du cadre d'impression en mm */
double	Hocount;                    /* DÇja fait en  hauteur du cadre     */
int		Lacount;                    /* Reste du en largeur   */
char Pr_inter;
int Marge_haut;
int Marge_bas;
int Marge_droite;
int Marge_gauche;
char Type_impr;						/* Grandeur de dÇpart pour les font */
#define PRINTER 0.1176				/* Grandeur de l'interligne en MM avec ESC 3 n */

/*****************************/

void main(int argv,char **argc)
{
char Filefrom[255];                     /* Fichier Ö convertir */
char Fileto[255];                       /* Fichier convertit */
long int taille,n;
char bog,bogold=0;
struct traitement tract[20];
int liste=0,count=0;


int     vari;                           /* Variable */
int compteur;

FILE *othfic;

char tab;               				/* Nombre de TAB demandÇ */

clrscr();
textmode(C80);

strcpy(Filefrom,"**");
strcpy(Fileto,".PRN");


Pr_large=96;
Pr_haut=172;
Marge_haut=10;
Marge_bas=10;
Marge_droite=3;
Marge_gauche=11;
Type_impr=0;

Intro("HTM2TXT v0.5 (c) 1995 by Red Bug.",1);
PrintAt(1,1,"Conversion HTML vers fichier texte format imprimante IBM graphics printer");
Ligne(1,2,78,2);

outfic=NULL;

if (argv!=1)
	{
	int n;
	for (n=1;n<=argv-1;n++)
		{
		switch((argc[n])[1])
			{
			case 'N':       strcpy(Filefrom,argc[n]+2);     break;
			case 'T':       strcpy(Fileto,argc[n]+2);       break;
			case 'C':       txt=1; break;
			case 'L':       /* option large */
				{
				Pr_large=160;
				Pr_haut=230;
				Marge_haut=10;
				Marge_bas=10;
				Marge_droite=3;
				Marge_gauche=11;
				Type_impr=2;
				} break;
			default: break;
			}
		} /* Fin du FOR */
	} /* FIN DU ARGV */




if (txt)
	{
	Pourprn(2,4); 	/* Taille d'un caractäre  */
	limite=80*4; 	/* Taille maximum 95 FILOFAX 95 colonnes avec 4 par car */
	bof=0;          /* Autorise le BOF */
	}

InitPercent();

PrintAt(2, 6,"Justification:       %s",justif ? "oui" : "non");
PrintAt(2, 8,"Taille du caractäre: %-3d points",pas);
PrintAt(2, 9,"Nombre de colonnes:  %-3d points",limite);
PrintAt(2,10,"Nombre de lignes:    %-3d",inter);

PrintAt(2,12,"Impression NLQ :     %s",nlq ? "oui" : "non");
PrintAt(2,13,"CondensÇ ?     :     %s",condense ? "oui" : "non");

PrintAt(2,15,"HR/LR -> PRN_CR:     %s",retour ? "oui" : "non");

PrintAt(2,17,"File to convert    : %s ",Filefrom);
PrintAt(2,18,"File to convert to : %s ",Fileto);

tract[1].type=1;
tract[1].pointeur=&justif;
tract[1].posx=23;
tract[1].posy=6;
tract[1].from=&(tract[5]);
tract[1].next=&(tract[2]);

tract[2].type=1;
tract[2].pointeur=&nlq;
tract[2].posx=23;
tract[2].posy=12;
tract[2].from=&(tract[1]);
tract[2].next=&(tract[7]);

tract[7].type=1;
tract[7].pointeur=&condense;
tract[7].posx=23;
tract[7].posy=13;
tract[7].from=&(tract[2]);
tract[7].next=&(tract[3]);


tract[3].type=1;
tract[3].pointeur=&retour;
tract[3].posx=23;
tract[3].posy=15;
tract[3].from=&(tract[7]);
tract[3].next=&(tract[4]);

tract[4].type=2;
tract[4].pointeur=(void*)Filefrom;
tract[4].length=12;
tract[4].posx=23;
tract[4].posy=17;
tract[4].from=&(tract[3]);
tract[4].next=&(tract[5]);

tract[5].type=2;
tract[5].pointeur=(void*)Fileto;
tract[5].length=12;
tract[5].posx=23;
tract[5].posy=18;
tract[5].from=&(tract[4]);
tract[5].next=&(tract[6]);

tract[6].type=0;
tract[6].pointeur=NULL;
tract[6].posx=1;
tract[6].posy=1;
tract[6].from=&(tract[4]);
tract[6].next=&(tract[1]);


if (Traite(&(tract[1]))) The_End(0,"Stop by user");

if (!strcmp(Filefrom,"**"))
	The_End(1,"Pas de fichier d'entrÇe");
	else
	{
	infic=fopen(Filefrom,"rb");
	if (infic==NULL)
		The_End(2,"Le fichier %s n'existe pas !!!",Filefrom);
	}

if (Fileto[0]=='.')
	{
	char tyn;
	char bup[220];
	strcpy(bup,Filefrom);
	for (tyn=0;bup[tyn]!='.';tyn++);
	bup[tyn]=0;
	strcat(bup,Fileto);
	strcpy(Fileto,bup);
	}

outfic=fopen(Fileto,"wb");
othfic=fopen("other.prn","wt");

taille=filelength(fileno(infic));

if (txt) goto Label1;


/***************************************************************************/
/******************** DEBUT INITIALISATION *********************************/
fic=outfic;

if (condense)
	fprintf(outfic,"%c%c%c",27,83,48); /* EXPOSANTS */
	else
	fprintf(outfic,"%c%c",27,84); /* EXPOSANTS NON !*/

Bucou.prn[1]=nlq;		/* QualitÇ par dÇfault */
Bucou.prn[2]=4;			/* Font par dÇfault */
Bucou.prn[3]=0;			/* Epaisseur par dÇfault */

for (vari=0;vari<240;vari++)
	{
	Bufis[vari].car=0;
	memcpy(Bufis[vari].prn,Bucou.prn,10);
	}

Pr_inter=condense ? 14 : 28;		/* Interligne */
Cadre_haut();



Lacount=(int)(((Pr_large-(Marge_gauche+Marge_droite))*1000.)/211);

Bupos=0;		/* Commence Bufis Ö zÇro */


/**************** FIN INITIALISATION ***************************************/
/***************************************************************************/



if (bof==1)
	fprintf(outfic,"%c%c%c",27,67,inter); /* nombre d'interligne par page */
	else
	fprintf(outfic,"%c%c",27,79); /* annule le bof */
Pourprn(2,4);

Label1: /* Label qui evite l'impression des caractäres d'imprimante */


tab=0;


for(n=0;n<taille;n++)
	{
	unsigned char car;

	car=(unsigned char)fgetc(infic);
	bog=0;

	switch (car)
		{
		case '<':
			{
			char go;
			int pos,m,nr;
			char buf[2000],oldbuf[2000];
			long offset;

			offset=ftell(infic);
			pos=0;
			go=0;

			do
				{
				car=fgetc(infic);
				buf[pos]=car;
				pos++;
				}
			while ((car!='>') & (pos<150) );

			if (pos>100)
				{
				go=0;
				goto go_cont;
				}

			buf[pos-1]=0;
			nr=0;

			while (*(buf+nr)!=0)
				{
				if (*(buf+nr)==10)              *(buf+nr)=32;
				if (*(buf+nr)<32)
					for(m=nr;m<strlen(buf)-1;m++)
					*(buf+m)=*(buf+m+1);
				else
				nr++;
				}
			strcpy(oldbuf,buf);

			strupr(buf);

			if (!strnicmp(buf,"A HREF",6))
				{
				fprintf(othfic,"%s\n",oldbuf);
				go=1;
				}
			if (!strcmp(buf,"/A")) go=1;

			if (!strnicmp(buf,"A NAME",6))
				{
				fprintf(othfic,"%s\n",oldbuf);
				go=1;
				}

			if (!strnicmp(buf,"IMG",3))
				{
				fprintf(othfic,"%s\n",oldbuf);
				go=1;
				}

			if (!strcmp(buf,"H1") )
				{
				LigneOK();
				Pourprn(2,8);
				Pourprn(1,1); /* Impression NLQ=1 DRAFT=0 */
				go=1;
				}
			if (!strcmp(buf,"/H1"))
				{
				LigneOK();
				Pourprn(2,4);
				Pourprn(1,nlq);
				go=1;
				}
			if (!strcmp(buf,"H2"))
				{
				LigneOK();
				go=1;
				Pourprn(2,6);
				Pourprn(1,1); /* Impression NLQ=1 DRAFT=0 */
				}
			if (!strcmp(buf,"/H2"))
				{
				go=1;
				LigneOK();
				Pourprn(2,4);
				Pourprn(1,nlq);
				}
			if (!strcmp(buf,"H3"))
				{
				LigneOK();
				go=1;
				Pourprn(2,5);
				Pourprn(1,1); /* Impression NLQ=1 DRAFT=0 */
				}
			if (!strcmp(buf,"/H3"))
				{
				LigneOK();
				Pourprn(2,4);
				Pourprn(1,nlq);
				go=1;
				}

			if (!strcmp(buf,"H4"))
				{
				LigneOK();
				Pourprn(2,6);
				Pourprn(3,1);   /* Gras ON */
				Pourprn(1,1); /* Impression NLQ=1 DRAFT=0 */
				go=1;
				}
			if (!strcmp(buf,"/H4"))
				{
				LigneOK();
				Pourprn(2,4);
				Pourprn(3,0);   /* Gras OFF */
				Pourprn(1,nlq);
				go=1;
				}

			if (!strcmp(buf,"/PRE"))        { pre=0; go=1; }

			if (!strcmp(buf,"PRE")) { pre=1; go=1; }

			if (!strcmp(buf,"P"))   { LigneOK(); LigneOK(); go=1;  }  /* Fin de paragraphe */

			if (!strcmp(buf,"LI"))
				{
				go=1;
				LigneOK();
				if (liste==1)
					{
					count++;
					Pourcar('-');
					Pourcar(32);
					}
				if (liste==2)
					{
					char string[20];
					count++;
					sprintf(string,"%d",count);
					strcat(string,")          ");
					string[5]=0;
					for (vari=0;vari<strlen(string);vari++)
						Pourcar(string[vari]);
					}
				}

			if (!strcmp(buf,"UL"))  { go=1; liste=1; count=0; }
			if (!strcmp(buf,"/UL")) { go=1; liste=0; }
			if (!strcmp(buf,"OL"))  { go=1; liste=2; count=0; }
			if (!strcmp(buf,"/OL")) { go=1; liste=0; }

			if (!strcmp(buf,"HR"))
				{
				double g;
				go=1;
				LigneOK();
				g=Lacount;
				do Pourcar ('-'); while(g!=Lacount);
				LigneOK();
				}  /* Ligne horizontale */

			if (!strcmp(buf,"BR"))
				{  go=1;        LigneOK(); }

			if (!strcmp(buf,"STRONG") )      {  go=1; Pourprn(3,1);  /* Gras ON */ }
			if (!strcmp(buf,"/STRONG") )     {  go=1; Pourprn(3,0);  /* Gras OFF */ }
			if (!strcmp(buf,"EM") )          {  go=1; Pourprn(4,1);  /* Italique ON */ }
			if (!strcmp(buf,"/EM") )         {  go=1; Pourprn(4,0);  /* Italique OFF */ }
			if (!strcmp(buf,"B") )           {  go=1; Pourprn(3,1);  /* Gras ON */ }
			if (!strcmp(buf,"/B") )          {  go=1; Pourprn(3,0);  /* Gras OFF */ }

			if (!strnicmp(buf,"FONT SIZE",9))
				{
				int fs;
				sscanf(buf,"FONT SIZE=%d",&fs);
				go=1;
				}


			if (!strcmp(buf,"I")) go=1;             /* Italique + */
			if (!strcmp(buf,"/I")) go=1;    		/* Italique - */


			if (!strcmp(buf,"BODY")) go=1;
			if (!strcmp(buf,"/BODY")) go=1;
			if (!strcmp(buf,"CITE")) go=1;
			if (!strcmp(buf,"/CITE")) go=1;
			if (!strcmp(buf,"CENTER")) go=1;
			if (!strcmp(buf,"/CENTER")) go=1;


			if (!strcmp(buf,"TITLE")) go=1;
			if (!strcmp(buf,"/TITLE")) go=1;
			if (!strcmp(buf,"HEADER")) go=1;
			if (!strcmp(buf,"/HEADER")) go=1;
			if (!strcmp(buf,"ADDRESS")) { pre=1; go=1; }
			if (!strcmp(buf,"/ADDRESS")) { pre=0; go=1; }
			if (!strcmp(buf,"HEAD")) go=1;
			if (!strcmp(buf,"/HEAD")) go=1;

			if (!strcmp(buf,"BLOCKQUOTE")) go=1;
			if (!strcmp(buf,"/BLOCKQUOTE")) go=1;

			if (!strnicmp(buf,"BODY",4)) go=1;
			if (!strnicmp(buf,"HTML",4)) go=1;
			if (!strnicmp(buf,"FONT",4)) go=1;

			if (!strcmp(buf,"DL")) go=1;
			if (!strcmp(buf,"/DL")) go=1;
			if (!strcmp(buf,"DT")) go=1;            /* TABULATEUR  */
			if (!strcmp(buf,"/DT")) go=1;           /* TABULATEUR  */
			if (!strcmp(buf,"DD")) go=1;            /* TABULATEUR  */
			if (!strcmp(buf,"/DD")) { LigneOK(); go=1;}      /* TABULATEUR */

			if (!strcmp(buf,"U")) go=1;
			if (!strcmp(buf,"/U")) go=1;

			go_cont:

			if (go==0)
				{
				fseek(infic,offset,SEEK_SET);
				bog='<';
				}
				else
				{
				n+=pos;
				}

			} break;
		case '&':
			{
			int pos;
			char buf[2000];
			long offset;

			pos=0;

			offset=ftell(infic);
			do
				{
				car=fgetc(infic);
				buf[pos]=car;
				pos++;
				}
			while ( (car!=';') & (car!=' ') & (pos<100) );
			if ( (car==' ') | (pos>=100) )
				{
				fseek(infic,offset,SEEK_SET);
				bog='&';
				}
				else
				{
				buf[pos-1]=0;
				n+=pos;
				if (!strcmp(buf,"amp")) bog='&';
				if (!strcmp(buf,"eacute")) bog='Ç';
				if (!strcmp(buf,"egrave")) bog='ä';
				if (!strcmp(buf,"agrave")) bog='Ö';
				if (!strcmp(buf,"Eacute")) bog='ê';
				if (!strcmp(buf,"ecirc")) bog='à';
				if (!strcmp(buf,"lt")) bog='<';
				if (!strcmp(buf,"gt")) bog='>';
				}
			}
			break;

		case 0x0D: break;       /* 0D */
		case 0x0A:
			{
			if ( (Bupos!=0) & (pre==0) )
				if (isalpha(bogold)) bog=32; else bog=0;
			if ((pre==1) | (retour==1))     LigneOK();
			} break;

		case 0xA3: bog='ú'; break;   /**/
		case 0xE9: bog='Ç'; break;   /**/
		case 0xE2: bog='É'; break;   /***/
		case 0xE7: bog='á'; break;   /***/
		case 0xFB: bog='ñ'; break;   /***/
		case 0xEF: bog='ã'; break;   /*****/
		case 0xE8: bog='ä'; break;   /**********  Conv. accent de Ouindauss */
		case 0xEA: bog='à'; break;   /*****/
		case 0xE0: bog='Ö'; break;   /***/
		case 0xEE: bog='å'; break;   /***/
		case 0xF9: bog='ó'; break;   /***/
		case 0xB0: bog='¯'; break;   /**/
		case 0xF4: bog='ì'; break;   /**/
		case 9  :  { bog=0; tab++; } break;

		default: bog=car;  break;
		} /* FIN DU SWITCH */

	if (tab>=1)
		{
		if (isalpha(bog))
			{
			for (compteur=0;compteur<3*tab;compteur++)
				Pourcar (32);
			tab=0;
			}
		if (car!=9) tab=0;
		}

	if ( (bog!=0) & ( (bogold!=32) | (bog!=32) | (pre==1) ) )
		{
		bogold=bog;
		Pourcar (bog);
		}

	ToPercent(n,taille);
	}

ToPercent(n,n);

fprintf(outfic,"%c",12);

fclose(infic);
fclose(outfic);
fclose(othfic);

PrintAt(2,20,"Report:");
PrintAt(2,21,"Nombre de lignes:%4d",Tot_ligne);
if (bof) PrintAt(30,21,"==> %4d page(s).",(int)ceil((double)(Tot_ligne)/(inter)));

getch();

The_End(0,"That's all");
}

/*
void LigneOK(void)
{
int n;
int oui;
char buf2[255];
char buf3[255];

if (vil>=limite)
	{
	int max,max2,blanc,pos;

	for (n=0;n<t_buffer;n++)
		{
		buf2[n]=buffer[n];
		buf3[n]=0;
		}

	max=t_buffer-1;
	while ( ( (buffer[max]!=32) & (buffer[max]!=39) & (buffer[max]!='-') )
	| (tab[max]==12345) ) max--;

	if (buffer[max]==32)
		{
		max2=max;
		blanc=limite-tab[max];
		}
		else
		{
		max2=max+1;
		blanc=limite-tab[max]-4;
		}

	pos=0;

	oui=0;
	for(n=0;n<max;n++)      if (buffer[n]==32) oui++;

	if (oui==0)
		{
		blanc=0;
		max=0;
		while (oui<limite)
			{
			oui+=bufpas[max];
			max++;
			}
		max2=max;
		max--;
		tab[max]=oui-bufpas[max];
		}

	while (blanc>0)
		{
		blanc-=4;

		do
			{
			pos++;
			if (pos>=max) pos=0;
			}
		while (buffer[pos]!=32);

		if (tab[pos]!=12345) buf3[pos]++; else blanc+=4;
		}

	for (n=0;n<max2;n++)
			{
			int x;
			if ( (buf3[n]!=0) & (bufpas[n]==4) & (justif) )
				for(x=0;x<buf3[n];x++) fprintf(outfic," ");
			fprintf(outfic,"%c",buffer[n]);
			}

	t_buffer=t_buffer-max-1;
	for (n=0;n<t_buffer;n++)
		buffer[n]=buf2[n+max+1];

	if (t_buffer<0) t_buffer=0;

	vil=limite-tab[max]-4;
	}
	else
	{
    for (n=0;n<t_buffer;n++)
		fprintf(outfic,"%c",buffer[n]);

	t_buffer=0;
	vil=0;
	}

fprintf(outfic,"%c%c",0x0D,0x0A);
Tot_ligne++;
}
*/


void Pourprn(char Quoi,char Combien)
{
Bucou.prn[Quoi]=Combien;
}

void Pourcar(char car)
{
int Vrlng,Vrdeb,n;

Lacount-=(Bucou.prn[2]);

Bufis[Bupos].car=car;
memcpy(Bufis[Bupos].prn,Bucou.prn,10);


Bupos++;

if (Lacount<2)
	{
	Vrlng=Bupos;    /* Veritable fin */
	for (Vrdeb=Bupos;Vrdeb>=0;Vrdeb--)
		if((Bufis[Vrdeb].car)==32) break;
	if (Vrdeb==-1)	Vrdeb=Bupos;

	Bupos=Vrdeb;
	LigneOK();
	for (n=Vrdeb+1;n<Vrlng;n++)
		{
		(Bufis[Bupos].car)=(Bufis[n].car);
		memcpy(Bucou.prn,Bufis[Bupos].prn,10);
		Lacount-=(Bucou.prn[2]);
		Bupos++;
		}
	}
}

void LigneOK(void)
{
if (Lacount!=(int)(((Pr_large-(Marge_gauche+Marge_droite))*1000.)/211))
	{
	Lacount=(int)(((Pr_large-(Marge_gauche+Marge_droite))*1000.)/211);
	Print();
	PRN_cr();
	} /* Uniquement si on a pas dÇjÖ appuyÇ sur ENTER */
}


void PRN_Gras(char Oui)
{
if (Oui==0)
	fprintf(fic,"%c%c",27,70); /* Caractäre gras OFF */
	else
	fprintf(fic,"%c%c",27,69); /* Caractäre gras ON */
}

void PRN_font(char fs)
{
switch (fs) {
	case 4:
		fprintf(fic,"%c%c%c",27,91,6); /* 30 car/in */
		break;
	case 5:
		fprintf(fic,"%c%c%c",27,91,5); /* 24 car/in */
		break;
	case 6:
		fprintf(fic,"%c%c%c",27,91,4); /* 20 car/in */
		break;
	case 7:
		fprintf(fic,"%c%c%c",27,91,3); /* 17,1 car/in */
		break;
	case 8:
		fprintf(fic,"%c%c%c",27,91,2); /* 15 car/in */
		break;
	case 10:
		fprintf(fic,"%c%c%c",27,91,1); /* 12 car/in */
		break;
	case 12:
		fprintf(fic,"%c%c%c",27,91,0); /* 10 car/in */
		break;
	default:
		break;
	}
}

void PRN_cr(void)      /* DÇfinit l'interligne */
{

Cadre_gadr();

if (Hocount>=(Pr_haut-Marge_bas))
	{
	while(Hocount<(Pr_haut-(Pr_inter*PRINTER)))  Cadre_gadr();
	Cadre_bas();
	fprintf(outfic,"%c",12);		/* Fin de page */
	Cadre_haut();
	}

}

void Cadre_haut(void)	/* Trace le cadre haut */
{
int n;

Tot_ligne++;
Hocount=0;
PRN_nlq(1);
PRN_font(8);
fprintf(outfic,"%c",218);
n=(int)((Pr_large*1000.)/211);
PRN_font(5);
while (((n+7)&7)!=7) { fprintf(outfic,"%c",196); n-=5; }
PRN_font(8);
while (n>8) { fprintf(outfic,"%c",196); n-=8; }
PRN_font(8);
fprintf(outfic,"%c",191);

/* SÇlÇctionne la qualitÇ de l'impression */
PRN_nlq(nlq); /* Impression NLQ=1 DRAFT=0 */
PRN_font(Bucou.prn[2]);

fprintf(fic,"%c%c%c%c%c",27,51,Pr_inter,13,10);
Hocount+=(Pr_inter*PRINTER);


while (Hocount<=(double)Marge_haut)	PRN_cr();
	/* DÇfinit l'interligne et s'alighe Ö la marge haut */
}

void Cadre_gadr(void)
{
int n;

fprintf(fic,"%c",13);
PRN_nlq(1);
PRN_font(8);
fprintf(fic,"%c",179);
n=(int)((Pr_large*1000.)/211);
PRN_font(5);
while (((n+7)&7)!=7) { fprintf(fic,"%c",32); n-=5; }
PRN_font(8);
while (n>8) { fprintf(fic,"%c",32); n-=8; }
PRN_font(8);
fprintf(fic,"%c",179);

/* SÇlÇctionne la qualitÇ de l'impression */
PRN_nlq(nlq); /* Impression NLQ=1 DRAFT=0 */
PRN_font(Bucou.prn[2]);

fprintf(fic,"%c%c%c%c%c",27,51,Pr_inter,13,10);
Hocount+=(Pr_inter*PRINTER);
}

void Cadre_bas(void)
{
int n;

fprintf(fic,"%c",13);
PRN_nlq(1);
PRN_font(8);
fprintf(fic,"%c",192);
n=(int)((Pr_large*1000.)/211);
PRN_font(5);
while (((n+7)&7)!=7) { fprintf(fic,"%c",196); n-=5; }
PRN_font(8);
while (n>8) { fprintf(fic,"%c",196); n-=8; }
PRN_font(8);
fprintf(fic,"%c",217);

/* SÇlÇctionne la qualitÇ de l'impression */
PRN_nlq(nlq); /* Impression NLQ=1 DRAFT=0 */
PRN_font(Bucou.prn[2]);


}


void PRN_nlq(char oui)
{
fprintf(fic,"%c%c%c",27,120,oui); /* Impression NLQ=1 DRAFT=0 */
}

/************************************/
/* Envoie une ligne vers le buffer  */
/* Sans provoquer l'impression et   */
/* Le changement de ligne (mais     */
/* bien la cadre !)                 */
/************************************/

void Print(void)
{
int n;

/* Marge de gauche */
PRN_font(4);
for(n=0;n<(Marge_gauche*1000)/211;n+=4)
	fputc(32,fic);
PRN_font(Bucou.prn[2]);

for (n=0;n<Bupos;n++)
	{
	if ((Bufis[n].prn[1])!=(Bucou.prn[1]))	PRN_nlq(Bufis[n].prn[1]);
	if ((Bufis[n].prn[2])!=(Bucou.prn[2]))	PRN_font(Bufis[n].prn[2]);
	if ((Bufis[n].prn[3])!=(Bucou.prn[3]))	PRN_Gras(Bufis[n].prn[3]);
	memcpy(Bucou.prn,Bufis[n].prn,10);
	fputc(Bufis[n].car,fic);
	}

for (n=0;n<Bupos;n++)
	{
	Bufis[n].car=0;
	memcpy(Bufis[n].prn,Bucou.prn,10);
	}
Bupos=0;
}
